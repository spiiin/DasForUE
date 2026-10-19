#include "DasComputedDefaultsCommandlet.h"
#include "DasContainerDefaultsProbe.h"
#include "DasDefaultRoots.h"
#include "DasDefaultValueProbe.h"
#include "DasContainerTypes.h"
#include "DasSchema.h"
#include "Curves/CurveFloat.h"
#include "DasDSLService.h"
#include "DasRuntime.h"
#include "DasAuthoring.h"
#include "DasProbeActor.h"
#include "DasScriptComponent.h"
#include "Engine/Blueprint.h"
#include "Engine/World.h"
#include "Engine/Level.h"
#include "Components/SceneComponent.h"
#include "Kismet2/KismetEditorUtilities.h"
#include "UObject/UnrealType.h"
#include "UObject/StructOnScope.h"
#include "UObject/StrongObjectPtr.h"
#include "UObject/SavePackage.h"
#include "Misc/FileHelper.h"
#include "Misc/ScopeExit.h"
#include "Misc/PackageName.h"
#include "HAL/FileManager.h"
namespace
{

int32 ComputedInt(UObject* O, const TCHAR* N)
{
    return FindFProperty<FIntProperty>(O->GetClass(), N)->GetPropertyValue_InContainer(O);
}
void ComputedSet(UObject* O, const TCHAR* N, int32 V)
{
    FindFProperty<FIntProperty>(O->GetClass(), N)->SetPropertyValue_InContainer(O, V);
}
bool ComputedSave(UObject* O)
{
    const FString File = FPackageName::LongPackageNameToFilename(
        O->GetOutermost()->GetName(),
        O->IsA<UWorld>() ? FPackageName::GetMapPackageExtension() : FPackageName::GetAssetPackageExtension());
    IFileManager::Get().MakeDirectory(*FPaths::GetPath(File), true);
    FSavePackageArgs Args;
    Args.TopLevelFlags = RF_Public | RF_Standalone;
    return UPackage::SavePackage(O->GetOutermost(), O, *File, Args);
}
int32 ComputedCall(UObject* A)
{
    auto* F = A->FindFunction(TEXT("Read"));
    if (!F)
        return MIN_int32;
    auto* Input = FindFProperty<FIntProperty>(F, TEXT("input"));
    auto* Result = CastField<FIntProperty>(F->GetReturnProperty());
    if (!Input || !Result)
        return MIN_int32;
    FStructOnScope Args(F);
    Input->SetPropertyValue_InContainer(Args.GetStructMemory(), 0);
    A->ProcessEvent(F, Args.GetStructMemory());
    return Result->GetPropertyValue_InContainer(Args.GetStructMemory());
}
}
UDasComputedDefaultsCommandlet::UDasComputedDefaultsCommandlet()
{
    IsEditor = true;
    IsClient = false;
    IsServer = false;
    LogToConsole = true;
}
int32 UDasComputedDefaultsCommandlet::Main(const FString& Params)
{
    FString Id, Phase;
    FParse::Value(*Params, TEXT("ProbeId="), Id);
    FParse::Value(*Params, TEXT("Phase="), Phase);
    FGuid Guid;
    if (!FGuid::ParseExact(Id, EGuidFormats::Digits, Guid))
        return 1;
    FEditorScriptExecutionGuard Guard;
    auto& R = FModuleManager::LoadModuleChecked<FDasRuntimeModule>(TEXT("DasRuntime"));
    FString Error;
    auto Check = [&](bool OK, const TCHAR* N)
    {
        UE_LOG(LogTemp, Display, TEXT("DAS_COMPUTED_DEFAULTS %s %s %s"), OK ? TEXT("PASS"): TEXT("FAIL"), N, *Error);
        return OK;
    };
    const FString Root = TEXT("/Game/DasComputedDefaults/")+ Id, Script = TEXT("dsl_computed_defaults.das"),
                                                                               Path = R.ScriptPath(Script);
    FString Final;
    if (!FFileHelper::LoadFileToString(Final, *Path))
        return 1;
    auto Write = [](const FString& P, const FString& S)
    {
        return FFileHelper::SaveStringToFile(S, *P, FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM);
    };
    ON_SCOPE_EXIT
    {
        Write(Path, Final);
    };
    TStrongObjectPtr<UWorld> WorldKeep;
    UBlueprint* BP = nullptr;
    UWorld* W = nullptr;
    ADasProbeActor* A = nullptr;
    ADasProbeActor* Manual = nullptr;
    ON_SCOPE_EXIT
    {
        if (W)
        {
            if (A)
                R.ReleaseObject(A);
            if (Manual)
                R.ReleaseObject(Manual);
            if (W->IsInitialized())
                W->DestroyWorld(false);
        }
    };
    if (Phase == TEXT("Create"))
    {
        // These test assets have no reference outside the two default containers.
        for (const auto& Entry :
             TArray<TPair<FString, float>>{{TEXT("ArrayAsset"), 17}, {TEXT("MapAsset"), 23}, {TEXT("GCOnly"), 31}})
        {
            const FString Package = TEXT("/Game/DasAssetDefaults/")+ Entry.Key;
            auto* Asset = LoadObject<UCurveFloat>(nullptr, *(Package + TEXT(".")+ Entry.Key), nullptr, LOAD_NoWarn);
            if (!Asset)
                Asset = NewObject<UCurveFloat>(CreatePackage(*Package), *Entry.Key, RF_Public | RF_Standalone);
            Asset->FloatCurve.UpdateOrAddKey(0, Entry.Value);
            if (!ComputedSave(Asset))
                return 1;
        }
        FDasAuthoringModule Model;
        FString Wire;
        if (!Check(R.ReadAuthoringScript(Path, Model, Error) && Model.ToJson(Wire, Error),
                   TEXT("compile_real_das_expressions_and_helpers")))
            return 1;
        FDasDeclaredClass Effective;
        if (!Model.ResolveClass(TEXT("computeddefaults.ComputedDefaults"), Effective, Error))
            return 1;
        if (!Check(Effective.Properties.FindByPredicate([](const auto& P) { return P.Name == TEXT("Score"); })
                           ->Default.Number == 40 &&
                   !Wire.Contains(TEXT("__ue_computed")), TEXT("IR_contains_only_resolved_values")))
            return 1;
        const FString Old = Final.Replace(
            TEXT("value * 4"),
                 TEXT("value * 3")).Replace(TEXT("\"MapAsset.MapAsset\""), TEXT("\"ArrayAsset.ArrayAsset\""));
        if (!Write(Path, Old))
            return 1;
        if (!Check(FDasDSLService::Apply(Script, Root + TEXT("/BP_ComputedDefaults"),
                                                             TEXT("computeddefaults.ComputedDefaults"), BP, Error),
                                                     TEXT("initial_import")))
            return 1;
        FKismetEditorUtilities::CompileBlueprint(BP);
        auto* CDO = BP->GeneratedClass->GetDefaultObject();
        ComputedSet(CDO, TEXT("Manual"), 77);
        ComputedArrayManual(CDO, TEXT("ManualNumbers"));
        if (!Check(FDasDSLService::Reimport(BP, Error), TEXT("record_manual_CDO_source_contract")))
            return 1;
        W = UWorld::CreateWorld(EWorldType::Editor, false, TEXT("Map"), CreatePackage(*(Root + TEXT("/Map"))));
        if (!W)
            return 1;
        WorldKeep.Reset(W);
        W->SetFlags(RF_Public | RF_Standalone);
        W->GetOutermost()->SetPackageFlags(PKG_ContainsMap);
        A = W->SpawnActor<ADasProbeActor>(BP->GeneratedClass);
        Manual = W->SpawnActor<ADasProbeActor>(BP->GeneratedClass);
        if (!A || !Manual)
            return 1;
        A->Tags.Add(TEXT("ComputedDefault"));
        Manual->Tags.Add(TEXT("ComputedManual"));
        ComputedSet(Manual, TEXT("Score"), 111);
        ComputedArrayManual(Manual, TEXT("Numbers"));
        ComputedManualHash(Manual);
        if (!Check(ComputedCall(A) == 62 && A->LastDasError.IsEmpty(),
                   TEXT("runtime_compilation_matches_computed_CDO")))
            return 1;
        const auto* Class = BP->GeneratedClass.Get();
        if (!Write(Path, Final))
            return 1;
        if (!Check(FDasDSLService::Reimport(BP, Error) && BP->GeneratedClass.Get() == Class &&
                   ComputedInt(CDO, TEXT("Score")) == 40 &&
                               ComputedInt(CDO, TEXT("Manual")) == 77 &&
                                           ComputedInt(A, TEXT("Score")) == 40 &&
                                                       ComputedInt(
                                                           Manual, TEXT("Score")) == 111,
                                                           TEXT("computed_change_merges_preserving_manual_values")))
            return 1;
        UE_LOG(LogTemp, Display,
               TEXT("Container merge values CDO=%d actor=%d manual=%d manualCDO=%d rich=%d hash=%d"),
                    ComputedArrayLast(
                        CDO, TEXT("Numbers")),
                        ComputedArrayLast(A, TEXT("Numbers")),
                                          ComputedArrayLast(Manual, TEXT("Numbers")),
                                                            ComputedArrayLast(CDO, TEXT("ManualNumbers")),
                                                                              ComputedContainers(CDO),
                                                                              ComputedManualHashMatches(Manual));
        if (!Check(ComputedArrayLast(CDO, TEXT("Numbers")) == 8 &&
                                     ComputedArrayLast(
                                         A, TEXT("Numbers")) == 8 &&
                                         ComputedArrayLast(
                                             Manual, TEXT("Numbers")) == 777 &&
                                             ComputedArrayLast(
                                                 CDO, TEXT("ManualNumbers")) == 777 && ComputedContainers(CDO) &&
                                                 ComputedManualHashMatches(Manual),
                                                 TEXT("container_defaults_merge_whole_manual_values_and_rich_types")))
            return 1;
        if (!Check(FDasDSLService::Reimport(BP, Error), TEXT("deterministic_unchanged_reimport")))
            return 1;
        const FString BadScript = TEXT("computed_bad_")+ Id + TEXT(".das"), BadPath = R.ScriptPath(BadScript);
        ON_SCOPE_EXIT
        {
            IFileManager::Get().Delete(*BadPath);
        };
        const TArray<TPair<FString, FString>> Cases = {
            {TEXT("global"), TEXT("var state=7\ndef bad():int { return state }\n")},
             {TEXT("effect"), TEXT("def bad():int { print(\"must not print\"); return 1 }\n")},
              {TEXT("loop"), TEXT("def bad():int { var x=0; while (x<4) { x+=1 }; return x }\n")},
               {TEXT("recursive"), TEXT("def bad():int { return bad() }\n")},
                {TEXT("world"), TEXT("def bad():int { return ue_get_self_int(\"Score\") }\n")}};
        for (const auto& C : Cases)
        {
            const FString Bad =
                Final.Replace(TEXT("[ue_class(module_id="), *(C.Value + TEXT("\n[ue_class(module_id=")))
                                                                .Replace(TEXT("self.Score=default_score(self.Seed*2)"),
                                                                              TEXT("self.Score=bad()"));
            if (!Write(BadPath, Bad) ||
                !Check(!R.ReadAuthoringScript(BadPath, Model, Error) && !Error.IsEmpty(), *(TEXT("reject_")+ C.Key)))
                return 1;
        }
        for (const auto& Change : TArray<TPair<FString, FString>>{
                 {TEXT("self.Numbers=[self.Seed,default_score(2)]"), TEXT("self.Numbers=[true]")},
                  {TEXT("TagsDefault:table<string> = {\"red\",\"blue\",string(default_score(2))}"),
                        TEXT("TagsDefault:table<string> = {\"red\",\"red\"}")},
                   {TEXT("ue_name_value(\"Item_3\")=>self.Seed"),
                         TEXT("ue_name_value(\"Item_3\")=>self.Seed,ue_name_value(\"ITEM_3\")=>4")},
                    {TEXT("SpawnCollision.AlwaysSpawn=>default_score(2)"), TEXT("1=>default_score(2)")},
                     {TEXT("self.Numbers=[self.Seed,default_score(2)]"), TEXT("self.Numbers=[1/0]")}})
        {
            const FString Bad = Final.Replace(*Change.Key, *Change.Value);
            if (!Write(BadPath, Bad) || !Check(Bad != Final && !R.ReadAuthoringScript(BadPath, Model, Error),
                                               TEXT("container_invalid_type_duplicate_enum_or_calculation_rejected")))
                return 1;
        }
        for (const auto& Change : TArray<TPair<FString, FString>>{
                 {TEXT("ue_asset_default(default_array_asset())"),
                       TEXT("ue_asset_default(\"/Engine/BasicShapes/Cube.Cube\")")},
                  {TEXT("ue_asset_default(default_array_asset())"),
                        TEXT("ue_asset_default(\"/Game/DasAssetDefaults/Missing.Missing\")")},
                   {TEXT("ue_asset_default(default_array_asset())"),
                         TEXT("ue_class_default(\"/Script/Engine.Actor\")")},
                    {TEXT("ue_asset_default(default_array_asset())"),
                          TEXT("ue_asset_default(\"/Game/DasAssetDefaults/ArrayAsset.ArrayAsset:Subobject\")")},
                     {TEXT("ue_asset_default(default_array_asset())"),
                           TEXT("ue_asset_default(\"/Script/Engine.Default__Actor\")")},
                      {TEXT("ue_name_value(\"NoneValue\")=>ue_asset_default(\"\")"),
                            TEXT("ue_name_value(\"MAIN\")=>ue_asset_default(\"\")")}})
        {
            const FString Bad = Final.Replace(*Change.Key, *Change.Value);
            if (!Write(BadPath, Bad) || !Check(Bad != Final && !R.ReadAuthoringScript(BadPath, Model, Error),
                                               TEXT("reject_invalid_container_asset_class_path_marker_or_duplicate")))
                return 1;
        }
        const FString Cycle = Final.Replace(TEXT("Seed:int=2+3"), TEXT("Seed:int=self.Seed+1"));
        if (!Write(BadPath, Cycle) || !Check(!R.ReadAuthoringScript(BadPath, Model, Error) &&
                                             Error.Contains(TEXT("Cyclic")), TEXT("cyclic_source_fields_reject")))
            return 1;
        const FString WrongType = Final.Replace(TEXT("self.Score=default_score(self.Seed*2)"), TEXT("self.Score=true"));
        if (!Write(BadPath, WrongType) ||
            !Check(!R.ReadAuthoringScript(BadPath, Model, Error), TEXT("typed_default_mismatch_rejects")))
            return 1;
        const FString Division = Final.Replace(TEXT("self.Score=default_score(self.Seed*2)"), TEXT("self.Score=1/0"));
        if (!Write(BadPath, Division) ||
            !Check(!R.ReadAuthoringScript(BadPath, Model, Error), TEXT("evaluation_error_before_asset_mutation")))
            return 1;
        const FString SavedModel = BP->GetOutermost()->GetMetaData().GetValue(BP, TEXT("DasDSLModel"));
        if (!Write(Path, Division) ||
            !Check(!FDasDSLService::Reimport(BP, Error) && BP->GeneratedClass.Get() == Class &&
                   ComputedInt(CDO, TEXT("Score")) == 40 &&
                               SavedModel == BP->GetOutermost()->GetMetaData().GetValue(
                                                 BP, TEXT("DasDSLModel")),
                                                 TEXT("failed_reimport_preserves_working_class_and_metadata")))
            return 1;
        if (!Write(Path, Final))
            return 1;
        // Global initialization is not part of the declaration evaluator.
        const FString Global = Final.Replace(
            TEXT("[ue_class(module_id="), TEXT("var unused:int=ue_get_self_int(\"Score\")\n[ue_class(module_id="));
        if (!Write(BadPath, Global) ||
            !Check(R.ReadAuthoringScript(BadPath, Model, Error), TEXT("global_initializers_are_not_executed")))
            return 1;
        const FString Interface =
            Final +
            TEXT("\n[ue_interface(module_id=\"computeddefaults\",BlueprintType)]\nclass ComputedContract {\n [ue_function(BlueprintCallable)]\n def Query(input:int):int {}\n}\n");
        FDasAuthoringModule Interfaces;
        if (!Write(BadPath, Interface) ||
            !Check(R.ReadAuthoredInterfaces(BadPath, Interfaces, Error) && Interfaces.Classes.Num() == 1,
                   TEXT("declaration_only_collection_erases_computed_component_defaults")))
            return 1;
        UBlueprint* Child = nullptr;
        if (!Check(FDasDSLService::Apply(TEXT("dsl_computed_child.das"),
                                              Root + TEXT("/BP_ComputedChild"),
                                                          TEXT("computeddefaults.ComputedChild"), Child, Error),
                                                  TEXT("computed_inherited_property_defaults")))
            return 1;
        auto* ChildActor = W->SpawnActor<ADasProbeActor>(Child->GeneratedClass);
        if (!Check(ChildActor && ComputedInt(ChildActor, TEXT("Score")) == 85 && ComputedCall(ChildActor) == 172 &&
                                             ChildActor->LastDasError.IsEmpty(),
                                             TEXT("inherited_source_field_dependencies")))
            return 1;
        ChildActor->Tags.Add(TEXT("ComputedChild"));
        R.ReleaseObject(ChildActor);
        UBlueprint* Sibling = nullptr;
        if (!Check(FDasDSLService::Apply(TEXT("dsl_computed_child.das"),
                                              Root + TEXT("/BP_ComputedSibling"),
                                                          TEXT("computeddefaults.ComputedSibling"), Sibling, Error),
                                                  TEXT("computed_sibling_import")))
            return 1;
        auto* SiblingActor = W->SpawnActor<ADasProbeActor>(Sibling->GeneratedClass);
        if (!Check(SiblingActor &&
                   ComputedInt(SiblingActor, TEXT("Score")) == 40 &&
                               ComputedInt(SiblingActor, TEXT("Derived")) == 41 && ComputedCall(SiblingActor) == 81 &&
                                           SiblingActor->LastDasError.IsEmpty(),
                                           TEXT("sibling_does_not_inherit_other_child_source_override")))
            return 1;
        SiblingActor->Tags.Add(TEXT("ComputedSibling"));
        R.ReleaseObject(SiblingActor);
        UBlueprint* ComponentBP = nullptr;
        if (!Check(FDasDSLService::Apply(TEXT("dsl_computed_component.das"),
                Root + TEXT("/BP_ComputedComponent"), TEXT("computedcomponents.ComputedComponent"), ComponentBP, Error),
                    TEXT("computed_component_class")))
            return 1;
        // Blueprint compilation may collect/reinstance objects: retain the world and
        // refresh actor references from its owned actor list before using an Outer.
        for (const auto& Actor : W->PersistentLevel->Actors)
            if (auto* Live = Cast<ADasProbeActor>(Actor))
            {
                if (Live->Tags.Contains(TEXT("ComputedDefault")))
                    A = Live;
                if (Live->Tags.Contains(TEXT("ComputedManual")))
                    Manual = Live;
            }
        auto* Component = NewObject<UDasScriptComponent>(A, ComponentBP->GeneratedClass);
        A->AddInstanceComponent(Component);
        Component->RegisterComponent();
        Component->ComponentTags.Add(TEXT("ComputedComponent"));
        ComputedSet(Component, TEXT("Bonus"), 55);
        if (!Check(ComputedInt(ComponentBP->GeneratedClass->GetDefaultObject(), TEXT("Bonus")) == 42 &&
                               ComputedCall(Component) == 55 && Component->LastDasError.IsEmpty(),
                               TEXT("computed_component_runtime")))
            return 1;
        R.ReleaseObject(Component);
        if (!ComputedSave(Sibling) || !ComputedSave(ComponentBP) || !ComputedSave(Child) || !ComputedSave(BP) ||
            !ComputedSave(W))
            return 1;
    }
    else
    {
        BP = LoadObject<UBlueprint>(nullptr, *(Root + TEXT("/BP_ComputedDefaults.BP_ComputedDefaults")));
        W = LoadObject<UWorld>(nullptr, *(Root + TEXT("/Map.Map")));
        if (!BP || !W)
            return 1;
        WorldKeep.Reset(W);
        W->WorldType = EWorldType::Editor;
        if (!W->IsInitialized())
            W->InitWorld();
        W->UpdateWorldComponents(true, false);
        for (const auto& Actor : W->PersistentLevel->Actors)
            if (auto* P = Cast<ADasProbeActor>(Actor))
            {
                if (P->Tags.Contains(TEXT("ComputedDefault")))
                    A = P;
                if (P->Tags.Contains(TEXT("ComputedManual")))
                    Manual = P;
            }
    }
    if (!A || !Manual)
        return 1;
    if (Phase == TEXT("Recompile"))
    {
        R.ReleaseObject(A);
        R.ReleaseObject(Manual);
        FKismetEditorUtilities::CompileBlueprint(BP);
        A = nullptr;
        Manual = nullptr;
        for (const auto& Actor : W->PersistentLevel->Actors)
            if (auto* P = Cast<ADasProbeActor>(Actor))
            {
                if (P->Tags.Contains(TEXT("ComputedDefault")))
                    A = P;
                if (P->Tags.Contains(TEXT("ComputedManual")))
                    Manual = P;
            }
        auto* Fresh = W->SpawnActor<ADasProbeActor>(BP->GeneratedClass);
        const bool Good = Fresh && ComputedCall(Fresh) == 82 && Fresh->LastDasError.IsEmpty();
        if (Fresh)
        {
            R.ReleaseObject(Fresh);
            Fresh->Destroy();
        }
        if (!Check(Good, TEXT("fresh_spawn_after_Blueprint_compile")))
            return 1;
        if (!A || !Manual)
            return 1;
    }
    // Prove a default snapshot alone retains an asset, then releases it.
    if (Phase == TEXT("Create"))
    {
        FDasAuthoringModule Model;
        FDasDeclaredClass Decl;
        if (!R.ReadAuthoringScript(Path, Model, Error) ||
            !Model.ResolveClass(TEXT("computeddefaults.ComputedDefaults"), Decl, Error))
            return 1;
        const auto* Field = Decl.Properties.FindByPredicate([](const auto& P) { return P.Name == TEXT("Assets"); });
        if (!Field)
            return 1;
        const FString Carrier = DasContainerCarrier(Field->Type)->GetPathName();
        FDasDefaultRoots Roots;
        auto& Snapshot = Roots.Values.AddDefaulted_GetRef();
        if (!ParseDasStructDefault(Carrier, TEXT("[{\"asset\":\"/Game/DasAssetDefaults/GCOnly.GCOnly\"}]"), Snapshot,
                                                 Error))
            return 1;
        auto* Asset = FindObject<UCurveFloat>(nullptr, TEXT("/Game/DasAssetDefaults/GCOnly.GCOnly"));
        if (!Asset)
            return 1;
        Asset->ClearFlags(RF_Standalone);
        TWeakObjectPtr<UObject> Weak(Asset);
        Asset = nullptr;
        CollectGarbage(RF_NoFlags, true);
        if (!Check(Weak.IsValid(), TEXT("default_snapshot_retains_asset_through_GC")))
            return 1;
        Roots.Values.Reset();
        CollectGarbage(RF_NoFlags, true);
        if (!Check(!Weak.IsValid(), TEXT("default_snapshot_releases_asset_without_leaking")))
            return 1;
    }
    if (!Check(CheckDasRecursiveDefaults(Error), TEXT("recursive_struct_array_map_defaults_and_GC")))
        return 1;
    CollectGarbage(RF_NoFlags, true);
    if (!Check(ComputedAssetContainers(BP->GeneratedClass->GetDefaultObject()) && ComputedAssetContainers(A) &&
                   ComputedManualAssetsMatch(Manual),
               TEXT("asset_container_CDO_and_instances_survive_GC")))
        return 1;
    auto* CDO = BP->GeneratedClass->GetDefaultObject();
    const auto& Position =
        *FindFProperty<FStructProperty>(CDO->GetClass(), TEXT("Position"))->ContainerPtrToValuePtr<FVector>(CDO);
    if (!Check(ComputedInt(CDO, TEXT("Seed")) == 5 && ComputedInt(CDO, TEXT("Score")) == 40 &&
                                                                  ComputedInt(CDO, TEXT("Manual")) == 77 &&
                                                                              ComputedInt(CDO, TEXT("Derived")) == 42 &&
                                                                                          Position == FVector(10, 8, 7),
                                                                                          TEXT("saved_CDO_values")))
        return 1;
    if (!Check(FindFProperty<FStrProperty>(
            CDO->GetClass(), TEXT("Label"))->GetPropertyValue_InContainer(CDO) ==
            TEXT("computed defaults") &&
                 FindFProperty<FBoolProperty>(CDO->GetClass(), TEXT("Enabled"))->GetPropertyValue_InContainer(CDO) &&
                                              FindFProperty<FDoubleProperty>(
                                                  CDO->GetClass(), TEXT("Speed"))->GetPropertyValue_InContainer(CDO) ==
                                                  50.0,
                                                  TEXT("computed_string_bool_and_double")))
        return 1;
    if (!Check(ComputedCall(A) == 82 && A->LastDasError.IsEmpty() &&
               ComputedInt(Manual, TEXT("Score")) == 111, TEXT("saved_actor_runtime")))
        return 1;
    if (!Check(ComputedContainers(CDO) && ComputedContainers(A) && ComputedManualHashMatches(Manual) &&
               ComputedArrayLast(CDO, TEXT("Numbers")) == 8 &&
                                 ComputedArrayLast(Manual, TEXT("Numbers")) == 777 &&
                                                   ComputedArrayLast(
                                                       CDO, TEXT("ManualNumbers")) == 777,
                                                       TEXT("saved_container_defaults_and_manual_overrides")))
        return 1;
    auto* Pivot = FindObject<USceneComponent>(A, TEXT("Pivot"));
    if (!Check(Pivot && Pivot->GetRelativeLocation().Z == 42, TEXT("computed_component_template")))
        return 1;
    UE_LOG(LogTemp, Display, TEXT("DAS_COMPUTED_DEFAULTS_RESULT=PASS phase=%s"), *Phase);
    return 0;
}
