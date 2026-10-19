#include "DasTypeService.h"
#include "DasAuthoring.h"
#include "DasDSLService.h"
#include "DasRuntime.h"
#include "DasProbeActor.h"
#include "Engine/Blueprint.h"
#include "Engine/World.h"
#include "Engine/Level.h"
#include "StructUtils/UserDefinedStruct.h"
#include "UObject/StructOnScope.h"
#include "UObject/SavePackage.h"
#include "UObject/StrongObjectPtr.h"
#include "Editor.h"
#include "Misc/Parse.h"
#include "Misc/FileHelper.h"
#include "Misc/PackageName.h"
#include "Misc/Paths.h"
#include "Modules/ModuleManager.h"
#include "HAL/FileManager.h"
void DasFailStructuralForTesting(int32 Stage);
namespace
{
bool SaveAuthoredAsset(UObject* O)
{
    const FString F = FPackageName::LongPackageNameToFilename(
        O->GetOutermost()->GetName(),
        O->IsA<UWorld>() ? FPackageName::GetMapPackageExtension() : FPackageName::GetAssetPackageExtension());
    IFileManager::Get().MakeDirectory(*FPaths::GetPath(F), true);
    FSavePackageArgs A;
    A.TopLevelFlags = RF_Public | RF_Standalone;
    A.SaveFlags = SAVE_NoError;
    return UPackage::SavePackage(O->GetOutermost(), O, *F, A);
}
ADasProbeActor* AuthoredActor(UWorld* W)
{
    if (W && W->PersistentLevel)
        for (auto& V : W->PersistentLevel->Actors)
            if (auto* A = Cast<ADasProbeActor>(V.Get()))
                if (A->GetFName() == TEXT("TypedActor"))
                    return A;
    return nullptr;
}
}
int32 RunDasAuthoredValuesProbe(const FString& Params)
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
    const FString Base = TEXT("/Game/DasFrameAssetProbe/AuthoredValues_")+ Id,
                              Script = TEXT("dsl_authored_values_")+ Id + TEXT(".das");
    auto& R = FModuleManager::LoadModuleChecked<FDasRuntimeModule>(TEXT("DasRuntime"));
    bool Pass = true;
    auto Check = [&](bool V, const TCHAR* N)
    {
        Pass &= V;
        UE_LOG(LogTemp, Display, TEXT("DAS_AUTHORED_VALUES %s %s"), V ? TEXT("PASS"): TEXT("FAIL"), N);
    };
    const bool Direct = FParse::Param(*Params, TEXT("DirectTypes"));
    FString DirectSource;
    if (Direct)
    {
        FFileHelper::LoadFileToString(DirectSource, *R.ScriptPath(TEXT("dsl_direct_types.das")));
        DirectSource = DirectSource.Replace(TEXT("\r\n"), TEXT("\n"));
        DirectSource = DirectSource.Replace(TEXT("/Game/DasAuthoredTypes"), *(Base + TEXT("/Types")));
        FFileHelper::SaveStringToFile(DirectSource, *R.ScriptPath(Script),
                                      FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM);
    }
    TArray<UObject*> Types;
    if (!FDasTypeService::Apply(Direct ? Script
                                       : TEXT("dsl_type_declarations.das"), Base + TEXT("/Types"), Types, Error))
    {
        UE_LOG(LogTemp, Error, TEXT("%s"), *Error);
        return 1;
    }
    const FString EnumPath = Types[0]->GetPathName(), StructPath = Types[1]->GetPathName();
    auto* Type = CastChecked<UScriptStruct>(Types[1]);
    UBlueprint* BP = nullptr;
    UWorld* W = nullptr;
    if (Phase == TEXT("Create"))
    {
        FString Source;
        if (Direct)
            Source = DirectSource;
        else
            FFileHelper::LoadFileToString(Source, *R.ScriptPath(TEXT("dsl_authored_values.das")));
        Source = Source.Replace(TEXT("/Game/DasAuthoredTypes"), *(Base + TEXT("/Types")));
        Check(FFileHelper::SaveStringToFile(Source, *R.ScriptPath(Script),
                                            FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM),
              TEXT("create_bound_source"));
        Check(FDasDSLService::Apply(Script, Base + TEXT("/BP_Actor"), TEXT("authoredvalues.TypedActor"), BP, Error),
                                                TEXT("create_typed_enum_struct_DSL_class"));
        if (!BP)
        {
            UE_LOG(LogTemp, Error, TEXT("%s"), *Error);
            return 1;
        }
        W = UWorld::CreateWorld(EWorldType::Editor, false, TEXT("Map"), CreatePackage(*(Base + TEXT("/Map"))));
        W->SetFlags(RF_Public | RF_Standalone);
        W->GetOutermost()->SetPackageFlags(PKG_ContainsMap);
        FActorSpawnParameters P;
        P.Name = TEXT("TypedActor");
        W->SpawnActor<ADasProbeActor>(BP->GeneratedClass, FTransform::Identity, P);
    }
    else
    {
        BP = LoadObject<UBlueprint>(nullptr, *(Base + TEXT("/BP_Actor.BP_Actor")));
        W = LoadObject<UWorld>(nullptr, *(Base + TEXT("/Map.Map")));
    }
    auto* Actor = AuthoredActor(W);
    if (!Actor || !BP)
        return 1;
    TStrongObjectPtr<UBlueprint> KeepBP(BP);
    TStrongObjectPtr<UWorld> KeepWorld(W);
    auto* Mode = FindFProperty<FByteProperty>(Actor->GetClass(), TEXT("Mode"));
    auto* Config = FindFProperty<FStructProperty>(Actor->GetClass(), TEXT("Config"));
    Check(Mode && Mode->Enum == Types[0] && Config && Config->Struct == Type, TEXT("reflected_property_type_identity"));
    auto* Count = CastFieldChecked<FIntProperty>(FindDasStructField(Type, TEXT("Count")));
    auto* Title = CastFieldChecked<FStrProperty>(FindDasStructField(Type, TEXT("Title")));
    Check(Mode->GetPropertyValue_InContainer(BP->GeneratedClass->GetDefaultObject()) == 7 &&
              Count->GetPropertyValue_InContainer(
                  Config->ContainerPtrToValuePtr<void>(BP->GeneratedClass->GetDefaultObject())) == 15,
          TEXT("typed_declaring_defaults"));
    if (Phase == TEXT("Reopen"))
        Check(Mode->GetPropertyValue_InContainer(Actor) == 42 &&
              Count->GetPropertyValue_InContainer(Config->ContainerPtrToValuePtr<void>(Actor)) == 100 &&
              Title->GetPropertyValue_InContainer(Config->ContainerPtrToValuePtr<void>(Actor)) ==
                  TEXT("Owned 世界"), TEXT("saved_values_verified_before_new_calls"));
    auto* SetMode = Actor->FindFunction(TEXT("SetMode"));
    FStructOnScope InMode(SetMode);
    FindFProperty<FByteProperty>(SetMode, TEXT("value"))->SetPropertyValue_InContainer(InMode.GetStructMemory(), 42);
    {
        FEditorScriptExecutionGuard G;
        Actor->ProcessEvent(SetMode, InMode.GetStructMemory());
    }
    auto* GetMode = Actor->FindFunction(TEXT("GetMode"));
    FStructOnScope OutMode(GetMode);
    {
        FEditorScriptExecutionGuard G;
        Actor->ProcessEvent(GetMode, OutMode.GetStructMemory());
    }
    Check(CastFieldChecked<FByteProperty>(GetMode->GetReturnProperty())
                      ->GetPropertyValue_InContainer(OutMode.GetStructMemory()) == 42 &&
              Mode->GetPropertyValue_InContainer(Actor) == 42,
          TEXT("enum_ProcessEvent_set_get_sparse_value"));
    FInstancedStruct Data;
    Data.InitializeAs(Type);
    Count->SetPropertyValue_InContainer(Data.GetMutableMemory(), 100);
    Title->SetPropertyValue_InContainer(Data.GetMutableMemory(), TEXT("Owned 世界"));
    FDasScalarValue Input;
    Input.Kind = 12;
    Input.StructValue = Data;
    Input.ObjectClass = StructPath;
    Check(R.RunNamedFramePersistent(
        R.ScriptPath(Script), TEXT("ue_TypedActor_SetConfig"), {Input}, EDasMethodSignature::Void, Actor).bSucceeded,
        TEXT("typed_authored_struct_setter"));
    auto* GetConfig = Actor->FindFunction(TEXT("GetConfig"));
    FStructOnScope OutConfig(GetConfig);
    {
        FEditorScriptExecutionGuard G;
        Actor->ProcessEvent(GetConfig, OutConfig.GetStructMemory());
    }
    auto* Returned = CastFieldChecked<FStructProperty>(GetConfig->GetReturnProperty());
    Check(Returned->Struct == Type &&
          Count->GetPropertyValue_InContainer(Returned->ContainerPtrToValuePtr<void>(OutConfig.GetStructMemory())) ==
              100 &&
          Title->GetPropertyValue_InContainer(Returned->ContainerPtrToValuePtr<void>(OutConfig.GetStructMemory())) ==
              TEXT("Owned 世界"), TEXT("struct_ProcessEvent_owned_result"));
    FDasScalarValue EnumArg;
    EnumArg.Kind = 13;
    EnumArg.IntValue = 42;
    EnumArg.ObjectClass = EnumPath;
    const auto Mixed =
        R.RunNamedFramePersistent(R.ScriptPath(Script), TEXT("ue_TypedActor_Evaluate"), {EnumArg, Input},
                                                             EDasMethodSignature::Struct, Actor, StructPath);
    Check(Mixed.bSucceeded && Mixed.StructValue.IsValid() &&
              Count->GetPropertyValue_InContainer(Mixed.StructValue.GetMemory()) == 142,
          TEXT("mixed_enum_struct_frame_friendly_field_access"));
    if (!Mixed.bSucceeded)
        UE_LOG(LogTemp, Error, TEXT("%s"), *Mixed.Error);
    if (Direct)
    {
        FDasScalarValue Amount;
        Amount.Kind = 0;
        Amount.IntValue = 3;
        const auto Bump = R.RunNamedFramePersistent(R.ScriptPath(Script), TEXT("ue_TypedActor_Bump"), {Amount},
                                                                               EDasMethodSignature::Int, Actor);
        Check(Bump.bSucceeded && Bump.Value == 103 &&
                  Count->GetPropertyValue_InContainer(Config->ContainerPtrToValuePtr<void>(Actor)) == 103,
              TEXT("direct_struct_leaf_compound_write"));
        FDasScalarValue Text;
        Text.Kind = 5;
        Text.StringValue = TEXT("Changed 世界");
        Check(R.RunNamedFramePersistent(
            R.ScriptPath(Script),
            TEXT("ue_TypedActor_SetTitle"), {Text}, EDasMethodSignature::Void, Actor).bSucceeded &&
            Title->GetPropertyValue_InContainer(Config->ContainerPtrToValuePtr<void>(Actor)) ==
                TEXT("Changed 世界"), TEXT("direct_struct_leaf_string_write"));
        Check(R.RunNamedFramePersistent(
            R.ScriptPath(Script),
            TEXT("ue_TypedActor_SetConfig"), {Input}, EDasMethodSignature::Void, Actor).bSucceeded,
            TEXT("restore_saved_struct_fixture"));
    }
    if (Direct)
    {
        FString SideSource = DirectSource.Replace(TEXT("self.Config.Count += amount"), TEXT("self.Config.Count += DirectRhs(amount)")) +
            TEXT("\ndef DirectRhs(amount:int):int {\n ue_set_self_struct(\"Config\",ue_struct_with_string(ue_get_self_struct(\"Config\"),\"Title\",\"RHS\"))\n return amount\n}\n");
        Check(FFileHelper::SaveStringToFile(SideSource, *R.ScriptPath(Script),
                                            FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM),
              TEXT("write_direct_RHS_fixture"));
        FDasScalarValue Amount;
        Amount.Kind = 0;
        Amount.IntValue = 3;
        auto Value = R.RunNamedFramePersistent(R.ScriptPath(Script), TEXT("ue_TypedActor_Bump"), {Amount},
                                                                          EDasMethodSignature::Int, Actor);
        Check(Value.bSucceeded && Value.Value == 103 &&
              Title->GetPropertyValue_InContainer(Config->ContainerPtrToValuePtr<void>(Actor)) ==
                  TEXT("RHS"), TEXT("direct_leaf_write_preserves_RHS_changes_to_other_fields"));
        Check(FFileHelper::SaveStringToFile(DirectSource, *R.ScriptPath(Script),
                                            FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM),
              TEXT("restore_direct_RHS_source"));
        Check(R.RunNamedFramePersistent(
            R.ScriptPath(Script),
            TEXT("ue_TypedActor_SetConfig"), {Input}, EDasMethodSignature::Void, Actor).bSucceeded,
            TEXT("restore_direct_RHS_values"));
    }
    if (Direct)
    {
        auto Reject = [&](const FString& Candidate, const TCHAR* Expected, const TCHAR* Name)
        {
            Check(FFileHelper::SaveStringToFile(Candidate, *R.ScriptPath(Script),
                                                FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM),
                  TEXT("write_negative_direct_fixture"));
            FDasAuthoringModule Model;
            FString Issue;
            Check(!R.ReadAuthoringScript(R.ScriptPath(Script), Model, Issue) && Issue.Contains(Expected), Name);
        };
        Reject(DirectSource.Replace(TEXT("Count : int = 15"), TEXT("Count : int = 16")),
                                    TEXT("Direct struct default differs"), TEXT("direct_struct_default_drift_refused"));
        Reject(DirectSource.Replace(TEXT("Active = 42"), TEXT("Active = 43")),
                                    TEXT("Direct enum value differs"), TEXT("direct_enum_asset_drift_refused"));
        Reject(DirectSource.Replace(TEXT("Count : int = 15"), TEXT("Count : float = 15.0")),
                                    TEXT("Direct struct field differs"), TEXT("direct_struct_asset_drift_refused"));
        Reject(DirectSource.Replace(
            TEXT("Config : Settings = Settings()"), TEXT("Config : Settings = Settings(Count=19)")),
            TEXT("Direct struct defaults require empty"), TEXT("direct_nonempty_constructor_refused"));
        Reject(DirectSource.Replace(TEXT("@ue_property=(EditAnywhere, BlueprintReadWrite)\n    Config"),
                                         TEXT("@ue_property=(EditAnywhere, BlueprintReadOnly)\n    Config")),
                                    TEXT("Cannot write BlueprintReadOnly"),
                                         TEXT("direct_readonly_struct_write_refused"));
        Check(FFileHelper::SaveStringToFile(DirectSource, *R.ScriptPath(Script),
                                            FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM),
              TEXT("restore_valid_direct_source"));
    }
    EnumArg.IntValue = 255;
    Check(!R.RunNamedFramePersistent(
        R.ScriptPath(Script), TEXT("ue_TypedActor_SetMode"), {EnumArg}, EDasMethodSignature::Void, Actor).bSucceeded &&
        Mode->GetPropertyValue_InContainer(Actor) == 42,
        TEXT("reserved_enum_value_refused_before_write"));
    CollectGarbage(RF_NoFlags);
    Check(Config->Struct == Type &&
              Count->GetPropertyValue_InContainer(Config->ContainerPtrToValuePtr<void>(Actor)) == 100,
          TEXT("authored_values_survive_GC"));
    if (Phase == TEXT("Create"))
    {
        for (auto* T : Types)
            Check(SaveAuthoredAsset(T), TEXT("save_authored_value_type"));
        Check(SaveAuthoredAsset(BP) && SaveAuthoredAsset(W), TEXT("save_actor_and_map"));
    }
    else if (Phase == TEXT("Reopen"))
        Check(Mode->GetPropertyValue_InContainer(Actor) == 42 && FDasDSLService::Reimport(BP, Error),
              TEXT("reopened_model_idempotent"));
    else
        return 1;
    if (W->IsInitialized())
        W->DestroyWorld(false);
    UE_LOG(LogTemp, Display, TEXT("DAS_AUTHORED_VALUES_RESULT=%s"), Pass ? TEXT("PASS"): TEXT("FAIL"));
    return Pass ? 0 : 1;
}
