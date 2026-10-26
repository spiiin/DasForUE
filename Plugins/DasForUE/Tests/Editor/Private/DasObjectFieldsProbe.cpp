#include "DasDSLService.h"
#include "DasRuntime.h"
#include "DasActor.h"
#include "Engine/Blueprint.h"
#include "Engine/World.h"
#include "Engine/Level.h"
#include "Curves/CurveFloat.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "Modules/ModuleManager.h"
#include "UObject/MetaData.h"
#include "UObject/SavePackage.h"
#include "UObject/StrongObjectPtr.h"
#include "Misc/Parse.h"
#include "Misc/PackageName.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "HAL/FileManager.h"
void DasFailRenameForTesting(int32 Stage);
namespace
{
ADasActor* ObjectFieldActor(UWorld* W, FName N)
{
    if (W && W->PersistentLevel)
        for (auto& E : W->PersistentLevel->Actors)
            if (auto* A = Cast<ADasActor>(E.Get()))
                if (A->GetFName() == N)
                    return A;
    return nullptr;
}
UObject* ObjectFieldValue(UObject* O, FName Name)
{
    auto* P = O ? FindFProperty<FObjectProperty>(O->GetClass(), Name) : nullptr;
    return P ? P->GetObjectPropertyValue_InContainer(O) : nullptr;
}
void AssignObjectField(UObject* O, FName Name, UObject* V)
{
    FindFProperty<FObjectProperty>(O->GetClass(), Name)->SetObjectPropertyValue_InContainer(O, V);
}
bool SaveObjectFieldAsset(UObject* O)
{
    const FString File = FPackageName::LongPackageNameToFilename(
        O->GetOutermost()->GetName(),
        O->IsA<UWorld>() ? FPackageName::GetMapPackageExtension() : FPackageName::GetAssetPackageExtension());
    IFileManager::Get().MakeDirectory(*FPaths::GetPath(File), true);
    FSavePackageArgs A;
    A.TopLevelFlags = RF_Public | RF_Standalone;
    A.SaveFlags = SAVE_NoError;
    return UPackage::SavePackage(O->GetOutermost(), O, *File, A);
}
}
int32 RunDasObjectFieldsProbe(const FString& Params)
{
    if (FApp::GetProjectName() != FString(TEXT("DasForUESmoke")))
        return 1;
    FString Id, Phase, Error;
    if (!FParse::Value(*Params, TEXT("ProbeId="), Id) || Id.Len() != 32 ||
                       !FParse::Value(*Params, TEXT("Phase="), Phase))
        return 1;
    for (TCHAR C : Id)
        if (!FChar::IsHexDigit(C))
            return 1;
    const FString Base = TEXT("/Game/DasFrameAssetProbe/ObjectFieldsAsset_")+ Id +
                              TEXT("/"), Script = TEXT("dsl_object_fields_asset_")+ Id + TEXT(".das");
    auto& R = FModuleManager::LoadModuleChecked<FDasRuntimeModule>(TEXT("DasRuntime"));
    bool Passed = true;
    auto Check = [&](bool V, const TCHAR* N)
    {
        Passed &= V;
        UE_LOG(LogTemp, Display, TEXT("DAS_OBJECT_FIELDS_ASSET %s %s"), V ? TEXT("PASS"): TEXT("FAIL"), N);
    };
    FString Original;
    FFileHelper::LoadFileToString(Original, *R.ScriptPath(TEXT("dsl_object_fields.das")));
    Original = Original.Replace(TEXT("\r\n"), TEXT("\n"));
    const FString Renamed =
        Original.Replace(TEXT("Target : UObjectHandle"),
                              TEXT("@ue_id=\"objectfields.ObjectFields.Target\"\n    PeerActor : UObjectHandle"))
                                  .Replace(TEXT("self.Target"), TEXT("self.PeerActor"));
    const FString Block = TEXT("    @ue_property=(EditAnywhere,BlueprintReadWrite)\n    @ue_id=\"objectfields.ObjectFields.Target\"\n    PeerActor : UObjectHandle = ue_object_value(\"/Script/Engine.Actor\")\n");
    const FString Removed = Renamed.Replace(
        *Block,
        TEXT(""))
            .Replace(TEXT("self.PeerActor = value"),
                TEXT(""))
                    .Replace(TEXT("return self.PeerActor"),
                        TEXT("return ue_null_object()"))
                            .Replace(TEXT("ue_object_valid(self.PeerActor)"),
                                          TEXT("false")).Replace(TEXT("self.PeerActor = ue_null_object(); "), TEXT(""));
    UBlueprint* BP = nullptr;
    UWorld* World = nullptr;
    if (Phase == TEXT("Create"))
    {
        Check(FFileHelper::SaveStringToFile(Original, *R.ScriptPath(Script),
                                            FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM),
              TEXT("create_object_field_source"));
        Check(FDasDSLService::Apply(Script, Base + TEXT("BP_Fields"), TEXT("objectfields.ObjectFields"), BP, Error),
                                                TEXT("create_object_field_blueprint"));
        if (!BP)
            return 1;
        World = UWorld::CreateWorld(EWorldType::Editor, false, TEXT("Map"), CreatePackage(*(Base + TEXT("Map"))));
        World->SetFlags(RF_Public | RF_Standalone);
        World->GetOutermost()->SetPackageFlags(PKG_ContainsMap);
        FAssetRegistryModule::AssetCreated(World);
        FActorSpawnParameters Spawn;
        Spawn.Name = TEXT("ObjectA");
        auto* A = World->SpawnActor<ADasActor>(BP->GeneratedClass, FTransform::Identity, Spawn);
        Spawn.Name = TEXT("ObjectB");
        auto* B = World->SpawnActor<ADasActor>(BP->GeneratedClass, FTransform::Identity, Spawn);
        AssignObjectField(A, TEXT("Target"), B);
        AssignObjectField(B, TEXT("Target"), A);
        AssignObjectField(A, TEXT("Component"), B->GetRootComponent());
        auto* Owned = NewObject<UCurveFloat>(CreatePackage(*(Base + TEXT("OwnedCurve"))),
                                                           TEXT("OwnedCurve"), RF_Public | RF_Standalone);
        FAssetRegistryModule::AssetCreated(Owned);
        AssignObjectField(A, TEXT("Held"), Owned);
        AssignObjectField(A, TEXT("Observed"), B);
        Check(SaveObjectFieldAsset(Owned) && SaveObjectFieldAsset(BP) && SaveObjectFieldAsset(World),
              TEXT("save_typed_object_cycle_component_and_owned_object"));
    }
    else
    {
        BP = LoadObject<UBlueprint>(nullptr, *(Base + TEXT("BP_Fields.BP_Fields")));
        if (!BP)
            return 1;
        const bool Rename = Phase == TEXT("Rename"), Retire = Phase == TEXT("Retire"),
                                                                            Restore = Phase == TEXT("Restore");
        if (Rename || Retire || Restore)
        {
            Check(!FindObject<UWorld>(nullptr, *(Base + TEXT("Map.Map"))), TEXT("object_map_initially_unloaded"));
            Check(FFileHelper::SaveStringToFile(Retire ? Removed : Renamed, *R.ScriptPath(Script),
                                                FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM),
                  TEXT("write_object_migration"));
            if (Rename)
            {
                DasFailRenameForTesting(2);
                const bool Failed = !FDasDSLService::ReimportRenamedFields(BP, Error, true);
                DasFailRenameForTesting(0);
                Check(Failed && Error.StartsWith(TEXT("Rename migration rolled back:")),
                                                 TEXT("saved_object_field_rename_rollback"));
                Check(FDasDSLService::ReimportRenamedFields(BP, Error, true), TEXT("saved_object_field_rename"));
            }
            else
                Check(FDasDSLService::ReimportArchivedFields(BP, Restore, true, Error),
                      TEXT("saved_object_field_retire_or_restore"));
            if (!Error.IsEmpty())
            {
                UE_LOG(LogTemp, Error, TEXT("%s"), *Error);
                return 1;
            }
        }
        World = LoadObject<UWorld>(nullptr, *(Base + TEXT("Map.Map")));
        auto* A = ObjectFieldActor(World, TEXT("ObjectA"));
        auto* B = ObjectFieldActor(World, TEXT("ObjectB"));
        if (!A || !B)
            return 1;
        const bool Retired = Retire || Phase == TEXT("ReopenRetired");
        const FName Name = Phase == TEXT("Reopen")? FName(TEXT("Target")) : FName(TEXT("PeerActor"));
        if (Retired)
        {
            const FString Key = TEXT("objectfields.ObjectFields::objectfields.ObjectFields.Target");
            const auto* VA = A->RetiredDasValues.Find(Key);
            const auto* VB = B->RetiredDasValues.Find(Key);
            Check(!A->GetClass()->FindPropertyByName(TEXT("PeerActor")) && VA && VB && VA->ObjectValue.Get() == B &&
                                                     VB->ObjectValue.Get() == A,
                                                     TEXT("saved_retired_object_cycle_reopens"));
        }
        else
            Check(ObjectFieldValue(A, Name) == B && ObjectFieldValue(B, Name) == A,
                  TEXT("saved_typed_object_cycle_reopens"));
        auto* Held = ObjectFieldValue(A, TEXT("Held"));
        Check(ObjectFieldValue(A, TEXT("Component")) == B->GetRootComponent() && Held && Held->IsA<UCurveFloat>() &&
                               Held->GetFName() ==
                                   TEXT("OwnedCurve") &&
                                        ObjectFieldValue(
                                            A, TEXT("Observed")) == B,
                                            TEXT("saved_component_owned_object_and_readonly_reference_reopen"));
        TStrongObjectPtr<UWorld> KeepWorld(World);
        TStrongObjectPtr<UBlueprint> KeepBlueprint(BP);
        CollectGarbage(RF_NoFlags);
        Check(ObjectFieldValue(A, TEXT("Held")) == Held, TEXT("reopened_strong_reference_survives_GC"));
        if (Rename || Retire || Restore)
            Check(SaveObjectFieldAsset(BP) && SaveObjectFieldAsset(World), TEXT("save_object_migration"));
        if (Phase.StartsWith(TEXT("Reopen")))
            Check(FDasDSLService::Reimport(BP, Error), TEXT("object_field_model_revalidates_after_reopen"));
    }
    if (World && World->IsInitialized())
        World->DestroyWorld(false);
    UE_LOG(LogTemp, Display, TEXT("DAS_OBJECT_FIELDS_ASSET_RESULT=%s"), Passed ? TEXT("PASS"): TEXT("FAIL"));
    return Passed ? 0 : 1;
}
