#include "DasGASAssetService.h"
#include "DasGASAuthoring.h"
#include "DasRuntime.h"
#include "Engine/Blueprint.h"
#include "Engine/BlueprintGeneratedClass.h"
#include "Kismet2/KismetEditorUtilities.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "EdGraphSchema_K2.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "UObject/Package.h"
#include "UObject/SavePackage.h"
#include "Misc/PackageName.h"
#include "Misc/Paths.h"
#include "Misc/SecureHash.h"
#include "Misc/ScopeExit.h"
#include "HAL/FileManager.h"
#include "HAL/IConsoleManager.h"

namespace
{
FString SetDigest(const FDasGASSetDeclaration& D)
{
    FString Value = D.Id;
    for (const auto& A : D.Attributes)
        Value += FString::Printf(TEXT("|%s|%.9g|%d|%.9g|%d|%.9g|%s"), *A.Name.ToString(), double(A.Initial), A.HasMin,
                                      double(A.Minimum), A.HasMax, double(A.Maximum), *A.MaximumAttribute.ToString());
    return FMD5::HashAnsiString(*Value);
}
FString EffectDigest(const FDasGASEffectDeclaration& D, const FString& SetHash)
{
    FString Value = D.Id + TEXT("|")+ SetHash;
    for (const auto& M : D.Modifiers)
        Value += FString::Printf(TEXT("|%s|%s|%.9g|%s"), *M.Attribute.ToString(), *M.Operation, double(M.Magnitude),
                                      *M.SetByCaller.ToString());
    return FMD5::HashAnsiString(*Value);
}
UBlueprint* NewGASBlueprint(const FString& Path, UClass* Parent)
{
    return FKismetEditorUtilities::CreateBlueprint(Parent, CreatePackage(*Path),
                                                   FName(*FPackageName::GetLongPackageAssetName(Path)), BPTYPE_Normal,
                                                   UBlueprint::StaticClass(), UBlueprintGeneratedClass::StaticClass());
}
}
bool FDasGASAssetService::Apply(const FString& Script, const FString& Folder, TMap<FString, UBlueprint*>& Out,
                                FString& Error, bool SaveAssets)
{
    Out.Empty();
    Error.Reset();
    auto Fail = [&](const FString& Message)
    {
        Error = Message;
        return false;
    };
    if (!IsInGameThread() || FPaths::GetCleanFilename(Script) != Script ||
        FPaths::GetExtension(Script) != TEXT("das") || !Folder.StartsWith(TEXT("/Game/")) ||
                                                                          !FPackageName::IsValidLongPackageName(Folder))
        return Fail(TEXT("Expected local Script.das and /Game/folder"));
    FDasGASDeclarations Model;
    if (!FModuleManager::LoadModuleChecked<FDasRuntimeModule>(
            TEXT("DasRuntime"))
                .ReadGASScript(FModuleManager::GetModuleChecked<FDasRuntimeModule>(
                    TEXT("DasRuntime")).ScriptPath(Script), Model, Error))
        return false;
    struct FWork
    {
        FString Name, Id, Path, Hash;
        UClass* Parent = nullptr;
        UBlueprint* BP = nullptr;
    };
    TArray<FWork> Work;
    TMap<FString, FString> Hashes;
    for (const auto& Set : Model.Sets)
    {
        const auto Hash = SetDigest(Set);
        Hashes.Add(Set.Id, Hash);
        Work.Add({Set.Name, Set.Id,
                  Folder + TEXT("/AS_")+ Set.Name, Hash, UDasAuthoredAttributeSet::StaticClass(), nullptr});
    }
    for (const auto& Effect : Model.Effects)
    {
        for (const auto& M : Effect.Modifiers)
            if (!M.SetByCaller.IsNone() && !FGameplayTag::RequestGameplayTag(M.SetByCaller, false).IsValid())
                return Fail(TEXT("SetByCaller tag is not registered: ")+ M.SetByCaller.ToString());
        Work.Add({Effect.Name, Effect.Id,
                  Folder + TEXT("/GE_")+ Effect.Name, EffectDigest(Effect, Hashes.FindChecked(Effect.Attributes)),
                                UDasAuthoredGameplayEffect::StaticClass(), nullptr});
    }
    // Check the entire batch before creating any UObject or changing an existing package.
    for (auto& W : Work)
    {
        const FString ObjectPath = W.Path + TEXT(".")+ FPackageName::GetLongPackageAssetName(W.Path);
        auto* Existing = LoadObject<UObject>(nullptr, *ObjectPath, nullptr, LOAD_NoWarn);
        if (!Existing && FPackageName::DoesPackageExist(W.Path))
            return Fail(TEXT("Existing GAS package cannot be loaded"));
        if (!Existing)
            continue;
        W.BP = Cast<UBlueprint>(Existing);
        if (!W.BP || W.BP->ParentClass != W.Parent || !W.BP->GeneratedClass || W.BP->Status == BS_Error)
            return Fail(TEXT("GAS import target is not a compatible compiled Blueprint"));
        const auto* CDO = W.BP->GeneratedClass->GetDefaultObject();
        FString Id, Hash;
        if (const auto* Set = Cast<UDasAuthoredAttributeSet>(CDO))
        {
            Id = Set->DefinitionId;
            Hash = Set->DefinitionHash;
        }
        if (const auto* Effect = Cast<UDasAuthoredGameplayEffect>(CDO))
        {
            Id = Effect->DefinitionId;
            Hash = Effect->DefinitionHash;
        }
        if (Id != W.Id || Hash != W.Hash)
            return Fail(TEXT("GAS schema changed; live structural migration is deferred. Import into a new folder."));
    }
    TArray<UBlueprint*> Created;
    bool Success = false;
    ON_SCOPE_EXIT
    {
        if (!Success)
            for (auto* BP : Created)
            {
                FAssetRegistryModule::AssetDeleted(BP);
                BP->ClearFlags(RF_Public | RF_Standalone);
                BP->GetOutermost()->SetDirtyFlag(false);
            }
    };
    TMap<FString, UClass*> Sets;
    for (const auto& Set : Model.Sets)
    {
        auto& W = *Work.FindByPredicate([&](const auto& A) { return A.Id == Set.Id; });
        if (!W.BP)
        {
            W.BP = NewGASBlueprint(W.Path, W.Parent);
            if (!W.BP)
                return Fail(TEXT("Cannot create AttributeSet Blueprint"));
            Created.Add(W.BP);
            for (const auto& A : Set.Attributes)
            {
                FEdGraphPinType Type;
                Type.PinCategory = UEdGraphSchema_K2::PC_Struct;
                Type.PinSubCategoryObject = FGameplayAttributeData::StaticStruct();
                if (!FBlueprintEditorUtils::AddMemberVariable(W.BP, A.Name, Type))
                    return Fail(TEXT("Cannot add GameplayAttributeData variable"));
                W.BP->NewVariables.Last().PropertyFlags |= CPF_BlueprintReadOnly;
            }
            FKismetEditorUtilities::CompileBlueprint(W.BP);
            if (W.BP->Status == BS_Error || !W.BP->GeneratedClass)
                return Fail(TEXT("AttributeSet Blueprint compilation failed"));
            auto* CDO = CastChecked<UDasAuthoredAttributeSet>(W.BP->GeneratedClass->GetDefaultObject());
            CDO->DefinitionId = Set.Id;
            CDO->DefinitionName = FName(*Set.Name);
            CDO->DefinitionHash = W.Hash;
            CDO->Rules = Set.Attributes;
            CDO->InitializeDeclaredValues();
            // CDO data was assigned after compilation. Refresh the Blueprint fast
            // construction list so new instances copy native declaration fields.
            CastChecked<UBlueprintGeneratedClass>(W.BP->GeneratedClass.Get())->UpdateCustomPropertyListForPostConstruction();
            FAssetRegistryModule::AssetCreated(W.BP);
            W.BP->MarkPackageDirty();
        }
        Sets.Add(Set.Id, W.BP->GeneratedClass);
        Out.Add(Set.Name, W.BP);
    }
    for (const auto& Effect : Model.Effects)
    {
        auto& W = *Work.FindByPredicate([&](const auto& A) { return A.Id == Effect.Id; });
        if (!W.BP)
        {
            W.BP = NewGASBlueprint(W.Path, W.Parent);
            if (!W.BP)
                return Fail(TEXT("Cannot create GameplayEffect Blueprint"));
            Created.Add(W.BP);
            FKismetEditorUtilities::CompileBlueprint(W.BP);
            if (W.BP->Status == BS_Error || !W.BP->GeneratedClass)
                return Fail(TEXT("GameplayEffect Blueprint compilation failed"));
            auto* CDO = CastChecked<UDasAuthoredGameplayEffect>(W.BP->GeneratedClass->GetDefaultObject());
            CDO->DefinitionId = Effect.Id;
            CDO->DefinitionHash = W.Hash;
            CDO->DurationPolicy = EGameplayEffectDurationType::Instant;
            CDO->Modifiers.Empty();
            CDO->RequiredMagnitudes.Empty();
            auto* SetCDO =
                CastChecked<UDasAuthoredAttributeSet>(Sets.FindChecked(Effect.Attributes)->GetDefaultObject());
            for (const auto& M : Effect.Modifiers)
            {
                FGameplayModifierInfo Mod;
                Mod.Attribute = SetCDO->Attribute(M.Attribute);
                if (!Mod.Attribute.IsValid())
                    return Fail(TEXT("Generated attribute storage does not match declaration"));
                Mod.ModifierOp = M.Operation == TEXT("Multiply")
                                                     ? EGameplayModOp::MultiplyCompound
                                                     : M.Operation == TEXT("Override")? EGameplayModOp::Override
                                                                                      : EGameplayModOp::Additive;
                if (M.SetByCaller.IsNone())
                    Mod.ModifierMagnitude = FGameplayEffectModifierMagnitude(FScalableFloat(M.Magnitude));
                else
                {
                    FSetByCallerFloat Magnitude;
                    Magnitude.DataTag = FGameplayTag::RequestGameplayTag(M.SetByCaller, false);
                    Mod.ModifierMagnitude = FGameplayEffectModifierMagnitude(Magnitude);
                    CDO->RequiredMagnitudes.AddUnique(M.SetByCaller);
                }
                CDO->Modifiers.Add(Mod);
            }
            // CDO data was assigned after compilation. Refresh the Blueprint fast
            // construction list so new instances copy native declaration fields.
            CastChecked<UBlueprintGeneratedClass>(W.BP->GeneratedClass.Get())->UpdateCustomPropertyListForPostConstruction();
            FAssetRegistryModule::AssetCreated(W.BP);
            W.BP->MarkPackageDirty();
        }
        Out.Add(Effect.Name, W.BP);
    }
    if (SaveAssets)
        for (auto* BP : Created)
        {
            const auto File = FPackageName::LongPackageNameToFilename(BP->GetOutermost()->GetName(),
                                                                      FPackageName::GetAssetPackageExtension());
            IFileManager::Get().MakeDirectory(*FPaths::GetPath(File), true);
            FSavePackageArgs Args;
            Args.TopLevelFlags = RF_Public | RF_Standalone;
            if (!UPackage::SavePackage(BP->GetOutermost(), BP, *File, Args))
                return Fail(TEXT("Cannot save GAS package: ")+ File);
        }
    Success = true;
    return true;
}
static FAutoConsoleCommand DasGASImportCommand(
    TEXT("Das.GASImport"),
         TEXT("Das.GASImport Script.das /Game/Folder : import authored AttributeSets and instant GameplayEffects"),
              FConsoleCommandWithArgsDelegate::CreateLambda(
                  [](const TArray<FString>& Args)
                  {
                      TMap<FString, UBlueprint*> Assets;
                      FString Error;
                      if (Args.Num() != 2 || !FDasGASAssetService::Apply(Args[0], Args[1], Assets, Error))
                      {
                          UE_LOG(LogTemp, Error, TEXT("DAS_GAS_IMPORT=FAIL %s"), *Error);
                          return;
                      }
                      for (const auto& Pair : Assets)
                          UE_LOG(LogTemp, Display, TEXT("DAS_GAS_ASSET %s %s"), *Pair.Key, *Pair.Value->GetPathName());
                      UE_LOG(LogTemp, Display, TEXT("DAS_GAS_IMPORT=PASS assets=%d"), Assets.Num());
                  }));
