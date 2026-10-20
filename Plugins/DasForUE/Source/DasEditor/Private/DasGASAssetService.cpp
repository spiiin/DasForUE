#include "DasGASAssetService.h"
#include "DasGASAuthoring.h"
#include "DasGASCalculations.h"
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
    if (D.Duration != TEXT("Instant"))
        Value += FString::Printf(TEXT("|v2|%s|%.9g|%.9g|%d|%s|%d|%s|%s|%s|%d"), *D.Duration, double(D.Seconds),
                                      double(D.Period), D.ExecuteOnApplication, *D.Stacking, D.StackLimit,
                                      *D.StackDuration, *D.StackPeriod, *D.StackExpiration, D.DenyOverflow);
    for (const auto& M : D.Modifiers)
        Value += FString::Printf(TEXT("|%s|%s|%.9g|%s"), *M.Attribute.ToString(), *M.Operation, double(M.Magnitude),
                                      *M.SetByCaller.ToString());
    return FMD5::HashAnsiString(*Value);
}
FString AbilityDigest(const FDasGASAbilityDeclaration& D, const TMap<FString, FString>& Hashes)
{
    FString Value =
        D.Id + TEXT("|ability-v1|") + (D.Effect.IsEmpty() ? FString() : Hashes.FindChecked(D.Effect)) +
                    TEXT("|") + (D.Cost.IsEmpty() ? FString() : Hashes.FindChecked(D.Cost)) +
                         FString::Printf(TEXT("|%.9g|%s|%d"), double(D.Cooldown), *D.CooldownTag.ToString(), D.AutoEnd);
    if (!D.ActivateEntry.IsEmpty())
        Value += TEXT("|script-v1|") + D.ScriptFile + TEXT("|") + D.ScriptContract;
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
    for (const auto& D : Model.Calculations)
    {
        FString Value = D.ScriptContract + TEXT("|")+ Hashes.FindChecked(D.Attributes);
        for (const auto& C : D.Captures)
            Value += TEXT("|")+ Hashes.FindChecked(C.Attributes);
        const auto Hash = FMD5::HashAnsiString(*Value);
        Hashes.Add(D.Id, Hash);
        Work.Add({D.Name, D.Id,
                  Folder + TEXT("/GC_")+ D.Name, Hash,
                                D.Kind == TEXT("Magnitude")? UDasGASMagnitudeCalculation::StaticClass()
                                                           : UDasGASExecutionCalculation::StaticClass(),
                                               nullptr});
    }
    for (const auto& Effect : Model.Effects)
    {
        for (const auto& M : Effect.Modifiers)
            if (!M.SetByCaller.IsNone() && !FGameplayTag::RequestGameplayTag(M.SetByCaller, false).IsValid())
                return Fail(TEXT("SetByCaller tag is not registered: ")+ M.SetByCaller.ToString());
        FString Value = EffectDigest(Effect, Hashes.FindChecked(Effect.Attributes));
        bool Calculated = false;
        if (!Effect.Execution.IsEmpty())
        {
            Value += TEXT("|")+ Hashes.FindChecked(Effect.Execution);
            Calculated = true;
        }
        for (const auto& M : Effect.Modifiers)
            if (!M.Calculation.IsEmpty())
            {
                Value += TEXT("|")+ Hashes.FindChecked(M.Calculation);
                Calculated = true;
            }
        const auto Hash = Calculated ? FMD5::HashAnsiString(*Value) : Value;
        Hashes.Add(Effect.Id, Hash);
        Work.Add({Effect.Name, Effect.Id,
                  Folder + TEXT("/GE_") + Effect.Name, Hash, UDasAuthoredGameplayEffect::StaticClass(), nullptr});
    }
    for (const auto& Ability : Model.Abilities)
    {
        if (!Ability.CooldownTag.IsNone() && !FGameplayTag::RequestGameplayTag(Ability.CooldownTag, false).IsValid())
            return Fail(TEXT("Ability cooldown tag is not registered: ") + Ability.CooldownTag.ToString());
        Work.Add({Ability.Name, Ability.Id,
                  Folder + TEXT("/GA_") + Ability.Name, AbilityDigest(Ability, Hashes),
                                UDasAuthoredGameplayAbility::StaticClass(), nullptr});
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
        if (const auto* Ability = Cast<UDasAuthoredGameplayAbility>(CDO))
        {
            Id = Ability->DefinitionId;
            Hash = Ability->DefinitionHash;
        }
        if (const auto* C = Cast<UDasGASMagnitudeCalculation>(CDO))
        {
            Id = C->Definition.Id;
            Hash = C->Definition.Hash;
        }
        if (const auto* C = Cast<UDasGASExecutionCalculation>(CDO))
        {
            Id = C->Definition.Id;
            Hash = C->Definition.Hash;
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
            CastChecked<UBlueprintGeneratedClass>(W.BP->GeneratedClass.Get())
                ->UpdateCustomPropertyListForPostConstruction();
            FAssetRegistryModule::AssetCreated(W.BP);
            W.BP->MarkPackageDirty();
        }
        Sets.Add(Set.Id, W.BP->GeneratedClass);
        Out.Add(Set.Name, W.BP);
    }
    TMap<FString, UClass*> Calculations;
    for (const auto& D : Model.Calculations)
    {
        auto& W = *Work.FindByPredicate([&](const auto& A) { return A.Id == D.Id; });
        if (!W.BP)
        {
            W.BP = NewGASBlueprint(W.Path, W.Parent);
            if (!W.BP)
                return Fail(TEXT("Cannot create GAS calculation Blueprint"));
            Created.Add(W.BP);
            FKismetEditorUtilities::CompileBlueprint(W.BP);
            if (W.BP->Status == BS_Error || !W.BP->GeneratedClass)
                return Fail(TEXT("Calculation Blueprint compilation failed"));
            FDasGASCalculationDefinition Def;
            Def.Id = D.Id;
            Def.Hash = W.Hash;
            Def.Script = D.ScriptFile;
            Def.Contract = D.ScriptContract;
            Def.Entry = D.Entry;
            Def.Operation = D.Operation == TEXT("Multiply")? EGameplayModOp::MultiplyCompound
                                                           : D.Operation == TEXT("Override")? EGameplayModOp::Override
                                                                                            : EGameplayModOp::Additive;
            for (const auto& C : D.Captures)
            {
                FDasGASCapture Capture;
                Capture.Name = C.Name;
                auto* Set = CastChecked<UDasAuthoredAttributeSet>(Sets.FindChecked(C.Attributes)->GetDefaultObject());
                Capture.Definition = FGameplayEffectAttributeCaptureDefinition(
                    Set->Attribute(C.Attribute), C.Source == TEXT("Source")
                                                                  ? EGameplayEffectAttributeCaptureSource::Source
                                                                  : EGameplayEffectAttributeCaptureSource::Target,
                                                                  C.Snapshot);
                Def.Captures.Add(Capture);
            }
            if (D.Kind == TEXT("Execution"))
                Def.Output = CastChecked<UDasAuthoredAttributeSet>(Sets.FindChecked(D.Attributes)->GetDefaultObject())
                                 ->Attribute(FName(*D.Output));
            auto* CDO = W.BP->GeneratedClass->GetDefaultObject();
            if (auto* C = Cast<UDasGASMagnitudeCalculation>(CDO))
            {
                C->Definition = Def;
                C->ConfigureCaptures();
            }
            if (auto* C = Cast<UDasGASExecutionCalculation>(CDO))
            {
                C->Definition = Def;
                C->ConfigureCaptures();
            }
            CastChecked<UBlueprintGeneratedClass>(W.BP->GeneratedClass.Get())
                ->UpdateCustomPropertyListForPostConstruction();
            FAssetRegistryModule::AssetCreated(W.BP);
            W.BP->MarkPackageDirty();
        }
        Calculations.Add(D.Id, W.BP->GeneratedClass.Get());
        Out.Add(D.Name, W.BP);
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
            CDO->DurationPolicy =
                Effect.Duration == TEXT("HasDuration")
                                        ? EGameplayEffectDurationType::HasDuration
                                        : Effect.Duration == TEXT("Infinite") ? EGameplayEffectDurationType::Infinite
                                                                              : EGameplayEffectDurationType::Instant;
            CDO->DurationMagnitude = FGameplayEffectModifierMagnitude(FScalableFloat(Effect.Seconds));
            CDO->Period = FScalableFloat(Effect.Period);
            CDO->bExecutePeriodicEffectOnApplication = Effect.ExecuteOnApplication;
            const auto Stacking =
                Effect.Stacking == TEXT("AggregateByTarget")
                                        ? EGameplayEffectStackingType::AggregateByTarget
                                        : Effect.Stacking == TEXT("AggregateBySource")
                                                                  ? EGameplayEffectStackingType::AggregateBySource
                                                                  : EGameplayEffectStackingType::None;
            // UE 5.8 declares SetStackingType without exporting it from the DLL.
            // Populate its reflected editor property instead of calling an unexported symbol.
            auto* StackingProperty = FindFProperty<FEnumProperty>(UGameplayEffect::StaticClass(), TEXT("StackingType"));
            if (!StackingProperty)
                return Fail(TEXT("UE GameplayEffect stacking property is missing"));
            StackingProperty->GetUnderlyingProperty()->SetIntPropertyValue(
                StackingProperty->ContainerPtrToValuePtr<void>(CDO), int64(Stacking));
            CDO->StackLimitCount = Effect.StackLimit;
            CDO->StackDurationRefreshPolicy =
                Effect.StackDuration ==
                TEXT("NeverRefresh") ? EGameplayEffectStackingDurationPolicy::NeverRefresh
                                     : Effect.StackDuration ==
                                           TEXT("ExtendDuration")
                                                ? EGameplayEffectStackingDurationPolicy::ExtendDuration
                                                : EGameplayEffectStackingDurationPolicy::RefreshOnSuccessfulApplication;
            CDO->StackPeriodResetPolicy =
                Effect.StackPeriod == TEXT("NeverReset")
                                           ? EGameplayEffectStackingPeriodPolicy::NeverReset
                                           : EGameplayEffectStackingPeriodPolicy::ResetOnSuccessfulApplication;
            CDO->StackExpirationPolicy =
                Effect.StackExpiration ==
                TEXT("RemoveSingleStackAndRefreshDuration")
                     ? EGameplayEffectStackingExpirationPolicy::RemoveSingleStackAndRefreshDuration
                     : Effect.StackExpiration == TEXT("RefreshDuration")
                                                      ? EGameplayEffectStackingExpirationPolicy::RefreshDuration
                                                      : EGameplayEffectStackingExpirationPolicy::ClearEntireStack;
            // UE checks overflow even on the first active application. Non-stacking
            // effects have limit zero, so they must not opt into overflow denial.
            CDO->bDenyOverflowApplication = Effect.Stacking != TEXT("None") && Effect.DenyOverflow;
            CDO->bClearStackOnOverflow = false;
            CDO->Modifiers.Empty();
            CDO->RequiredMagnitudes.Empty();
            CDO->Executions.Empty();
            if (!Effect.Execution.IsEmpty())
            {
                FGameplayEffectExecutionDefinition Execution;
                Execution.CalculationClass = Calculations.FindChecked(Effect.Execution);
                CDO->Executions.Add(Execution);
            }
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
                if (!M.Calculation.IsEmpty())
                {
                    FCustomCalculationBasedFloat Magnitude;
                    Magnitude.CalculationClassMagnitude = Calculations.FindChecked(M.Calculation);
                    Mod.ModifierMagnitude = FGameplayEffectModifierMagnitude(Magnitude);
                }
                else if (M.SetByCaller.IsNone())
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
            CastChecked<UBlueprintGeneratedClass>(W.BP->GeneratedClass.Get())
                ->UpdateCustomPropertyListForPostConstruction();
            FAssetRegistryModule::AssetCreated(W.BP);
            W.BP->MarkPackageDirty();
        }
        Out.Add(Effect.Name, W.BP);
    }
    for (const auto& Ability : Model.Abilities)
    {
        auto& W = *Work.FindByPredicate([&](const auto& A) { return A.Id == Ability.Id; });
        if (!W.BP)
        {
            W.BP = NewGASBlueprint(W.Path, W.Parent);
            if (!W.BP)
                return Fail(TEXT("Cannot create GameplayAbility Blueprint"));
            Created.Add(W.BP);
            FKismetEditorUtilities::CompileBlueprint(W.BP);
            if (W.BP->Status == BS_Error || !W.BP->GeneratedClass)
                return Fail(TEXT("GameplayAbility Blueprint compilation failed"));
            auto* CDO = CastChecked<UDasAuthoredGameplayAbility>(W.BP->GeneratedClass->GetDefaultObject());
            auto FindEffect = [&](const FString& Id) -> UClass*
            {
                if (Id.IsEmpty())
                    return nullptr;
                return Work.FindByPredicate([&](const auto& A) { return A.Id == Id; })->BP->GeneratedClass.Get();
            };
            CDO->DefinitionId = Ability.Id;
            CDO->DefinitionName = FName(*Ability.Name);
            CDO->DefinitionHash = W.Hash;
            CDO->Effect = FindEffect(Ability.Effect);
            CDO->Cost = FindEffect(Ability.Cost);
            CDO->CooldownSeconds = Ability.Cooldown;
            CDO->CooldownTags.Reset();
            if (!Ability.CooldownTag.IsNone())
                CDO->CooldownTags.AddTag(FGameplayTag::RequestGameplayTag(Ability.CooldownTag, false));
            CDO->AutoEnd = Ability.AutoEnd;
            CDO->ScriptFile = Ability.ScriptFile;
            CDO->ScriptContract = Ability.ScriptContract;
            CDO->ActivateEntry = Ability.ActivateEntry;
            CDO->EndEntry = Ability.EndEntry;
            CDO->CancelEntry = Ability.CancelEntry;
            CDO->TaskEntry = Ability.TaskEntry;
            CastChecked<UBlueprintGeneratedClass>(W.BP->GeneratedClass.Get())
                ->UpdateCustomPropertyListForPostConstruction();
            FAssetRegistryModule::AssetCreated(W.BP);
            W.BP->MarkPackageDirty();
        }
        Out.Add(Ability.Name, W.BP);
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
static FAutoConsoleCommand DasGASImportCommand(TEXT("Das.GASImport"),
    TEXT("Das.GASImport Script.das /Game/Folder : import authored AttributeSets, GameplayEffects and GameplayAbilities"),
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
