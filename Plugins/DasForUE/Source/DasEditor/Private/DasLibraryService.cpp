#include "DasLibraryService.h"
#include "DasAuthoring.h"
#include "DasBlueprintService.h"
#include "DasBlueprintLibrary.h"
#include "DasTypeRegistry.h"
#include "Engine/Blueprint.h"
#include "Engine/BlueprintGeneratedClass.h"
#include "Kismet2/KismetEditorUtilities.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "UObject/MetaData.h"
#include "Misc/PackageName.h"
bool ApplyDasLibrary(const FDasAuthoringModule& Model, const FString& Package, const FString& Id, UBlueprint*& Out,
                     FString& Error)
{
    Out = nullptr;
    FString Json;
    if (!Model.ToJson(Json, Error))
        return false;
    const auto* C = Model.Classes.FindByPredicate([&](const auto& Item) { return Item.Id == Id; });
    if (!C || Model.Classes.Num() != 1 || C->NativeParent != TEXT("/Script/Engine.BlueprintFunctionLibrary"))
    {
        Error = TEXT("One static library per source file is required");
        return false;
    }
    const FString Path = Package + TEXT(".")+ FPackageName::GetLongPackageAssetName(Package);
    auto* Object = LoadObject<UObject>(nullptr, *Path, nullptr, LOAD_NoWarn);
    auto* BP = Cast<UBlueprint>(Object);
    if ((Object && (!BP || BP->BlueprintType != BPTYPE_FunctionLibrary ||
                    BP->ParentClass != UDasBlueprintLibrary::StaticClass())) ||
        (!Object && FPackageName::DoesPackageExist(Package)))
    {
        Error = TEXT("Static library package is occupied or unreadable");
        return false;
    }
    for (const auto& M : C->Methods)
        if (UDasBlueprintLibrary::StaticClass()->FindFunctionByName(*M.Name))
        {
            Error = TEXT("Static method conflicts with native library infrastructure");
            return false;
        }
    if (BP)
    {
        const FString Wire = BP->GetOutermost()->GetMetaData().GetValue(BP, TEXT("DasDSLModel"));
        FDasAuthoringModule Prior;
        if (!FDasAuthoringModule::FromJson(Wire, Prior, Error) || Prior.Classes.Num() != 1 ||
            Prior.Classes[0].Id != Id || FDasAuthoringModule::Compare(Prior.Classes[0], *C).bStructural)
        {
            Error = TEXT("Static library structural changes require a later policy");
            return false;
        }
    }
    UDasTypeRegistry* Registry = nullptr;
    if (!C->LibraryTypeRegistry.IsEmpty())
    {
        Registry = LoadObject<UDasTypeRegistry>(nullptr, *C->LibraryTypeRegistry);
        if (!Registry)
        {
            Error = TEXT("Library type registry is missing");
            return false;
        }
    }
    TArray<TObjectPtr<UObject>> ValueTypes;
    bool TypesValid = true;
    TFunction<void(const FDasDeclaredType&)> Depend = [&](const auto& T)
    {
        if (T.TypePath.StartsWith(TEXT("/Game/")))
        {
            auto* Asset = LoadObject<UObject>(nullptr, *T.TypePath);
            if (!Asset)
                TypesValid = false;
            else
                ValueTypes.AddUnique(Asset);
        }
        if (T.Element)
            Depend(*T.Element);
        if (T.Key)
            Depend(*T.Key);
        if (T.Value)
            Depend(*T.Value);
    };
    for (const auto& M : C->Methods)
    {
        Depend(M.ReturnType);
        for (const auto& P : M.Parameters)
            Depend(P.Type);
    }
    if (!TypesValid)
    {
        Error = TEXT("Library value type dependency is missing");
        return false;
    }
    const bool Created = !BP;
    if (!BP)
    {
        BP = FKismetEditorUtilities::CreateBlueprint(UDasBlueprintLibrary::StaticClass(), CreatePackage(*Package),
                                                     *FPackageName::GetLongPackageAssetName(Package),
                                                     BPTYPE_FunctionLibrary, UBlueprint::StaticClass(),
                                                     UBlueprintGeneratedClass::StaticClass());
        if (!BP)
        {
            Error = TEXT("Cannot create function library Blueprint");
            return false;
        }
    }
    if (!FDasBlueprintService::ApplyLibraryGraphs(BP, Model, Id, Error))
        return false;
    auto* Defaults = CastChecked<UDasBlueprintLibrary>(BP->GeneratedClass->GetDefaultObject());
    Defaults->TypeRegistry = Registry;
    Defaults->ValueTypes = MoveTemp(ValueTypes);
    for (const auto& Pair : C->Metadata)
    {
        BP->GetOutermost()->GetMetaData().SetValue(BP, *Pair.Key, *Pair.Value);
        BP->GeneratedClass->SetMetaData(*Pair.Key, *Pair.Value);
    }
    auto& Meta = BP->GetOutermost()->GetMetaData();
    Meta.SetValue(BP, TEXT("DasDSLSource"), *C->SourceScript);
    Meta.SetValue(BP, TEXT("DasDSLClassId"), *Id);
    Meta.SetValue(BP, TEXT("DasDSLModel"), *Json);
    BP->MarkPackageDirty();
    if (Created)
        FAssetRegistryModule::AssetCreated(BP);
    Out = BP;
    return true;
}
