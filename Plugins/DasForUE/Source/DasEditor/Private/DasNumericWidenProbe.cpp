#include "DasDSLService.h"
#include "DasRuntime.h"
#include "DasActor.h"
#include "Modules/ModuleManager.h"
#include "Engine/Blueprint.h"
#include "Engine/BlueprintGeneratedClass.h"
#include "Engine/World.h"
#include "Engine/Level.h"
#include "Editor.h"
#include "Kismet2/KismetEditorUtilities.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "K2Node_VariableGet.h"
#include "UObject/UnrealType.h"
#include "UObject/SavePackage.h"
#include "UObject/StructOnScope.h"
#include "UObject/MetaData.h"
#include "Misc/Parse.h"
#include "Misc/Paths.h"
#include "Misc/PackageName.h"
#include "Misc/FileHelper.h"
#include "HAL/FileManager.h"
#include <limits>
#if WITH_DEV_AUTOMATION_TESTS
void DasFailWidenAfterCompileForTesting(bool Enabled);
#endif
namespace
{
double WidenValue(UObject* Object, FName Name)
{
    if (!Object)
        return std::numeric_limits<double>::quiet_NaN();
    auto* P = Object->GetClass()->FindPropertyByName(Name);
    if (auto* D = CastField<FDoubleProperty>(P))
        return D->GetPropertyValue_InContainer(Object);
    if (auto* F = CastField<FFloatProperty>(P))
        return F->GetPropertyValue_InContainer(Object);
    if (auto* I = CastField<FIntProperty>(P))
        return I->GetPropertyValue_InContainer(Object);
    return std::numeric_limits<double>::quiet_NaN();
}
void SetWidenValues(UObject* Object, int32 Score, float Rate)
{
    FindFProperty<FIntProperty>(Object->GetClass(), TEXT("Score"))->SetPropertyValue_InContainer(Object, Score);
    FindFProperty<FFloatProperty>(Object->GetClass(), TEXT("Rate"))->SetPropertyValue_InContainer(Object, Rate);
}
bool SaveWidenAsset(UObject* Asset)
{
    if (!Asset)
        return false;
    auto* P = Asset->GetOutermost();
    const FString File = FPackageName::LongPackageNameToFilename(
        P->GetName(),
        Asset->IsA<UWorld>() ? FPackageName::GetMapPackageExtension() : FPackageName::GetAssetPackageExtension());
    IFileManager::Get().MakeDirectory(*FPaths::GetPath(File), true);
    Asset->SetFlags(RF_Public | RF_Standalone);
    FSavePackageArgs Args;
    Args.TopLevelFlags = RF_Public | RF_Standalone;
    Args.SaveFlags = SAVE_NoError;
    return UPackage::SavePackage(P, Asset, *File, Args);
}
UBlueprint* WidenBP(const FString& Base, const TCHAR* Name)
{
    return LoadObject<UBlueprint>(nullptr, *(Base + Name + TEXT(".")+ Name));
}
ADasActor* WidenActor(UWorld* World, FName Name)
{
    if (World && World->PersistentLevel)
        for (auto& Entry : World->PersistentLevel->Actors)
            if (auto* A = Cast<ADasActor>(Entry.Get()))
                if (A->GetFName() == Name)
                    return A;
    return nullptr;
}
}
int32 RunDasNumericWidenProbe(const FString& Params)
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
    const FString Base = TEXT("/Game/DasFrameAssetProbe/Widen_")+ Id +
                              TEXT("/"), Script = TEXT("dsl_widen_")+ Id + TEXT(".das");
    auto& Runtime = FModuleManager::LoadModuleChecked<FDasRuntimeModule>(TEXT("DasRuntime"));
    bool Passed = true;
    auto Check = [&](bool Value, const TCHAR* Name)
    {
        Passed &= Value;
        UE_LOG(LogTemp, Display, TEXT("DAS_WIDEN_TEST %s %s"), Value ? TEXT("PASS"): TEXT("FAIL"), Name);
    };
    UBlueprint* Parent = nullptr;
    UBlueprint* Child = nullptr;
    UWorld* World = nullptr;
    if (Phase == TEXT("Create"))
    {
        FString Source;
        Check(FFileHelper::LoadFileToString(
            Source, *Runtime.ScriptPath(TEXT("dsl_widen_numeric.das"))) &&
            FFileHelper::SaveStringToFile(Source, *Runtime.ScriptPath(Script),
                                          FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM),
            TEXT("create_widen_source"));
        Check(FDasDSLService::Apply(Script, Base + TEXT("BP_Parent"), TEXT("widen.WidenFields"), Parent, Error),
                                                TEXT("create_widen_parent"));
        if (!Parent)
            return 1;
        Child = FKismetEditorUtilities::CreateBlueprint(
            Parent->GeneratedClass, CreatePackage(*(Base + TEXT("BP_Child"))),
                                                  TEXT("BP_Child"), BPTYPE_Normal, UBlueprint::StaticClass(),
                                                       UBlueprintGeneratedClass::StaticClass(), TEXT("DasWidenProbe"));
        FAssetRegistryModule::AssetCreated(Child);
        SetWidenValues(Child->GeneratedClass->GetDefaultObject(), 23, 2.25f);
        World = UWorld::CreateWorld(EWorldType::Editor, false, TEXT("Map"), CreatePackage(*(Base + TEXT("Map"))));
        World->SetFlags(RF_Public | RF_Standalone);
        World->GetOutermost()->SetPackageFlags(PKG_ContainsMap);
        FAssetRegistryModule::AssetCreated(World);
        FActorSpawnParameters Spawn;
        Spawn.Name = TEXT("ParentActor");
        SetWidenValues(World->SpawnActor<ADasActor>(Parent->GeneratedClass, FTransform::Identity, Spawn), MIN_int32,
                       0.1f);
        Spawn.Name = TEXT("ChildActor");
        SetWidenValues(World->SpawnActor<ADasActor>(Child->GeneratedClass, FTransform::Identity, Spawn), MAX_int32,
                       -13.25f);
        Check(SaveWidenAsset(Parent) && SaveWidenAsset(Child) && SaveWidenAsset(World),
              TEXT("save_original_numeric_scope"));
    }
    else if (Phase == TEXT("Migrate") || Phase == TEXT("Reopen"))
    {
        Parent = WidenBP(Base, TEXT("BP_Parent"));
        if (!Parent)
            return 1;
        if (Phase == TEXT("Migrate"))
        {
            Check(!FindObject<UBlueprint>(nullptr, *(Base + TEXT("BP_Child.BP_Child"))) &&
                                          !FindObject<UWorld>(nullptr, *(Base + TEXT("Map.Map"))),
                                                              TEXT("numeric_dependencies_initially_unloaded"));
            FString Source;
            FFileHelper::LoadFileToString(Source, *Runtime.ScriptPath(Script));
            const FString Changed = Source.Replace(
                TEXT("Score : int = 5"),
                     TEXT("Score : double = 5.0lf")).Replace(TEXT("Rate : float = 1.5"), TEXT("Rate : double = 1.5lf"));
            auto Write = [&](const FString& Text)
            {
                return FFileHelper::SaveStringToFile(Text, *Runtime.ScriptPath(Script),
                                                     FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM);
            };
            Check(Write(Changed), TEXT("write_numeric_widening"));
            UClass* Before = Parent->GeneratedClass;
            Check(!FDasDSLService::Reimport(Parent, Error) && Parent->GeneratedClass == Before,
                  TEXT("implicit_type_change_refused"));
            Check(!FDasDSLService::ReimportWidenedNumericFields(Parent, Error) && Parent->GeneratedClass == Before,
                  TEXT("isolated_widening_refuses_saved_dependencies"));
            Check(Write(Changed.Replace(TEXT("Rate : double = 1.5lf"), TEXT("Rate : double = 1.6lf"))) &&
                                        !FDasDSLService::ReimportWidenedNumericFields(Parent, Error, true) &&
                                        Parent->GeneratedClass == Before,
                        TEXT("combined_default_change_refused"));
            Check(Write(Changed.Replace(TEXT("Score : double = 5.0lf"), TEXT("Score : float = 5.0"))) &&
                                        !FDasDSLService::ReimportWidenedNumericFields(Parent, Error, true) &&
                                        Parent->GeneratedClass == Before,
                        TEXT("lossy_int_to_float_refused"));
            Write(Changed);
            auto* Graph = Parent->UbergraphPages[0].Get();
            auto* Getter = NewObject<UK2Node_VariableGet>(Graph);
            Getter->VariableReference.SetSelfMember(TEXT("Score"));
            Graph->AddNode(Getter);
            Getter->CreateNewGuid();
            Getter->AllocateDefaultPins();
            Check(!FDasDSLService::ReimportWidenedNumericFields(Parent, Error, true) &&
                      Parent->GeneratedClass == Before,
                  TEXT("raw_Blueprint_property_reference_refused"));
            Graph->RemoveNode(Getter);
            World = LoadObject<UWorld>(nullptr, *(Base + TEXT("Map.Map")));
            auto* A = WidenActor(World, TEXT("ParentActor"));
            auto* Rate = FindFProperty<FFloatProperty>(A->GetClass(), TEXT("Rate"));
            Rate->SetPropertyValue_InContainer(A, std::numeric_limits<float>::quiet_NaN());
            Check(!FDasDSLService::ReimportWidenedNumericFields(Parent, Error, true) &&
                      Parent->GeneratedClass == Before,
                  TEXT("nonfinite_instance_refused_before_mutation"));
            Rate->SetPropertyValue_InContainer(A, 0.1f);
            const FString OldModel = Parent->GetOutermost()->GetMetaData().GetValue(Parent, TEXT("DasDSLModel"));
#if WITH_DEV_AUTOMATION_TESTS
            DasFailWidenAfterCompileForTesting(true);
            const bool Failed = !FDasDSLService::ReimportWidenedNumericFields(Parent, Error, true);
            DasFailWidenAfterCompileForTesting(false);
            Check(Failed && Error.StartsWith(TEXT("Numeric migration rolled back:")) &&
                                             FindFProperty<FIntProperty>(
                                                 Parent->GeneratedClass, TEXT("Score")) &&
                                                 FindFProperty<FFloatProperty>(
                                                     Parent->GeneratedClass, TEXT("Rate")) &&
                                                     Parent->GetOutermost()->GetMetaData().GetValue(
                                                         Parent, TEXT("DasDSLModel")) == OldModel,
                                                         TEXT("post_compile_failure_rolls_back_types_and_model"));
            A = WidenActor(World, TEXT("ParentActor"));
            auto* B = WidenActor(World, TEXT("ChildActor"));
            Child = WidenBP(Base, TEXT("BP_Child"));
            Check(WidenValue(A, TEXT("Score")) == MIN_int32 &&
                             WidenValue(A, TEXT("Rate")) == static_cast<double>(0.1f) &&
                                        WidenValue(B, TEXT("Score")) == MAX_int32 && Child &&
                                                   WidenValue(Child->GeneratedClass->GetDefaultObject(),
                                                              TEXT("Score")) == 23,
                                                              TEXT("rollback_preserves_instances_and_child_CDO"));
#endif
            Check(FDasDSLService::ReimportWidenedNumericFields(Parent, Error, true), TEXT("exact_numeric_migration"));
            if (!Error.IsEmpty())
                UE_LOG(LogTemp, Error, TEXT("Widen: %s"), *Error);
        }
        Child = WidenBP(Base, TEXT("BP_Child"));
        World = LoadObject<UWorld>(nullptr, *(Base + TEXT("Map.Map")));
        Check(Child && World &&
              FindFProperty<FDoubleProperty>(Parent->GeneratedClass, TEXT("Score")) &&
                                             FindFProperty<FDoubleProperty>(Parent->GeneratedClass, TEXT("Rate")),
                                                                            TEXT("double_fields_persist_on_reopen"));
        Check(WidenValue(Parent->GeneratedClass->GetDefaultObject(), TEXT("Score")) == 5 &&
                         WidenValue(Parent->GeneratedClass->GetDefaultObject(), TEXT("Rate")) == 1.5 && Child &&
                                    WidenValue(Child->GeneratedClass->GetDefaultObject(), TEXT("Score")) == 23 &&
                                               WidenValue(Child->GeneratedClass->GetDefaultObject(), TEXT("Rate")) ==
                                                          2.25,
                                                          TEXT("promoted_root_child_defaults_exact"));
        auto* A = WidenActor(World, TEXT("ParentActor"));
        auto* B = WidenActor(World, TEXT("ChildActor"));
        Check(WidenValue(A, TEXT("Score")) == MIN_int32 &&
                         WidenValue(A, TEXT("Rate")) == static_cast<double>(0.1f) &&
                                    WidenValue(B, TEXT("Score")) == MAX_int32 &&
                                               WidenValue(B, TEXT("Rate")) == -13.25,
                                                          TEXT("promoted_map_extremes_and_fraction_exact"));
        if (B)
        {
            auto* F = B->FindFunction(TEXT("ReadScore"));
            FStructOnScope Frame(F);
            FEditorScriptExecutionGuard Guard;
            B->ProcessEvent(F, Frame.GetStructMemory());
            Check(CastField<FDoubleProperty>(F->GetReturnProperty())
                          ->GetPropertyValue_InContainer(Frame.GetStructMemory()) == MAX_int32,
                  TEXT("inherited_DSL_reads_promoted_value"));
        }
        if (Phase == TEXT("Migrate"))
            Check(SaveWidenAsset(Parent) && SaveWidenAsset(Child) && SaveWidenAsset(World),
                  TEXT("save_promoted_scope"));
        UClass* Before = Parent->GeneratedClass;
        Check(FDasDSLService::ReimportWidenedNumericFields(Parent, Error, true) && Parent->GeneratedClass == Before,
              TEXT("widen_repeat_is_idempotent"));
    }
    else
        return 1;
    if (World && World->IsInitialized())
        World->DestroyWorld(false);
    UE_LOG(LogTemp, Display, TEXT("DAS_NUMERIC_WIDEN_RESULT=%s"), Passed ? TEXT("PASS"): TEXT("FAIL"));
    return Passed ? 0 : 1;
}
