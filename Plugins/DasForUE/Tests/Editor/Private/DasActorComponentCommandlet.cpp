#include "DasActorComponentCommandlet.h"
#include "DasDSLService.h"
#include "DasAuthoring.h"
#include "UObject/MetaData.h"
#include "DasActorComponentProbe.h"
#include "Engine/Blueprint.h"
#include "Engine/BlueprintGeneratedClass.h"
#include "Engine/SimpleConstructionScript.h"
#include "Engine/SCS_Node.h"
#include "Kismet2/KismetEditorUtilities.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "UObject/SavePackage.h"
#include "Misc/PackageName.h"
#include "Misc/Parse.h"

// Keeps the callee's message after the failed expectation.
static bool Fail(FString& Error, const FString& What)
{
    Error = Error.IsEmpty() ? What : What + TEXT(": ") + Error;
    return false;
}

bool CheckComponentFieldMetadata(UBlueprint* BP, bool Recompile, FString& Error)
{
    auto Valid = [&]()
    {
        auto* P = BP->GeneratedClass ? BP->GeneratedClass->FindPropertyByName(TEXT("Behavior")) : nullptr;
        return P && P->HasAllPropertyFlags(CPF_BlueprintVisible | CPF_BlueprintReadOnly | CPF_Edit | CPF_EditConst) &&
               P->GetMetaData(TEXT("Category")) == TEXT("Components") &&
                                                        P->GetMetaData(TEXT("ToolTip")) == TEXT("Подсчёт очков");
    };
    if (!Valid())
    {
        Error = TEXT("Component field metadata/flags missing");
        return false;
    }
    if (Recompile)
    {
        FKismetEditorUtilities::CompileBlueprint(BP);
        if (BP->Status == BS_Error || !Valid())
        {
            Error = TEXT("Component metadata lost on Blueprint compilation");
            return false;
        }
        auto& R = FModuleManager::LoadModuleChecked<FDasRuntimeModule>(TEXT("DasRuntime"));
        FDasAuthoringModule Model;
        for (const auto Pair : {TPair<const TCHAR*, const TCHAR*>(
                 TEXT("dsl_component_field_bad_flags.das"), TEXT("Conflicting component VisibleAnywhere/EditAnywhere")),
                 TPair<const TCHAR*, const TCHAR*>(
                     TEXT("dsl_component_field_bad_access.das"),
                          TEXT("Conflicting component BlueprintReadOnly/BlueprintReadWrite")),
                     TPair<const TCHAR*, const TCHAR*>(
                         TEXT("dsl_component_field_bad_marker.das"), TEXT("Repeated ue_component field marker")),
                         TPair<const TCHAR*, const TCHAR*>(
                             TEXT("dsl_component_field_bad_type.das"),
                                  TEXT("Component field requires imported authored reference type")),
                             TPair<const TCHAR*, const TCHAR*>(
                                 TEXT("dsl_component_field_bad_initializer.das"),
                                      TEXT("Component fields are initialized by Unreal"))})
            if (R.ReadAuthoringScript(R.ScriptPath(Pair.Key), Model, Error) || !Error.Contains(Pair.Value))
                return Fail(Error, FString::Printf(TEXT("%s was not refused with [%s]"), Pair.Key, Pair.Value));
        UE_LOG(LogTemp, Display, TEXT("PASS DSL_component_field_invalid_flags_marker_type_initializer_refused"));
        for (const auto Pair : {TPair<const TCHAR*, const TCHAR*>(
                 TEXT("dsl_component_access_bad_replace.das"),
                      TEXT("Cannot replace BlueprintReadOnly component reference")),
                 TPair<const TCHAR*, const TCHAR*>(TEXT("dsl_component_access_bad_field.das"),
                                                        TEXT("Unknown authored object field"))})
            if (R.ReadAuthoringScript(R.ScriptPath(Pair.Key), Model, Error) || !Error.Contains(Pair.Value))
                return Fail(Error, FString::Printf(TEXT("%s was not refused with [%s]"), Pair.Key, Pair.Value));
        UE_LOG(LogTemp, Display, TEXT("PASS DSL_self_component_replacement_and_unknown_field_refused"));
    }
    UE_LOG(LogTemp, Display, TEXT("PASS DSL_component_field_metadata_flags_and_recompile"));
    Error.Reset();
    return true;
}

bool CheckOwnerAccessor(const FString& Base, UBlueprint* Owner, FString& Error)
{
    auto& R = FModuleManager::LoadModuleChecked<FDasRuntimeModule>(TEXT("DasRuntime"));
    FDasAuthoringModule Model;
    for (const auto Pair : {TPair<const TCHAR*, const TCHAR*>(
             TEXT("dsl_owner_bad_type.das"), TEXT("ue_owner requires an authored reference type")),
             TPair<const TCHAR*, const TCHAR*>(TEXT("dsl_owner_bad_args.das"),
                                                    TEXT("Expected ue_owner(type<Reference>)"))})
        if (R.ReadAuthoringScript(R.ScriptPath(Pair.Key), Model, Error) || !Error.Contains(Pair.Value))
            return Fail(Error, FString::Printf(TEXT("%s was not refused with [%s]"), Pair.Key, Pair.Value));
    UE_LOG(LogTemp, Display, TEXT("PASS DSL_ue_owner_invalid_parent_type_and_arguments_refused"));
    UBlueprint* BP = nullptr;
    if (!FDasDSLService::Apply(TEXT("dsl_owner_probe.das"), Base + TEXT("/BP_OwnerAccessorProbe"),
                                                                        TEXT("ownerprobe.OwnerAccessorProbe"), BP,
                                                                             Error))
        return Fail(Error, TEXT("Owner accessor probe import failed"));
    TStrongObjectPtr<UWorld> W(UWorld::CreateWorld(EWorldType::Game, false));
    auto* Actor = W->SpawnActor<ADasActor>(Owner->GeneratedClass);
    auto* Component = NewObject<UDasScriptComponent>(Actor, BP->GeneratedClass);
    const FString File = R.ScriptPath(TEXT("dsl_owner_probe.das"));
    const auto Read =
        R.RunNamedFramePersistent(File, TEXT("ue_OwnerAccessorProbe_Read"), {}, EDasMethodSignature::Int, Component);
    const auto Wrong =
        R.RunNamedFramePersistent(File, TEXT("ue_OwnerAccessorProbe_Wrong"), {}, EDasMethodSignature::Int, Component);
    if (!Read.bSucceeded || Read.Value != 0 || Wrong.bSucceeded || !Wrong.Error.Contains(TEXT("identity mismatch")))
    {
        Error = Read.Error + TEXT("; ")+ Wrong.Error;
        return false;
    }
    UE_LOG(LogTemp, Display, TEXT("PASS DSL_ue_owner_live_actor_and_wrong_identity_checked"));
    R.ReleaseObject(Component);
    W->DestroyWorld(false);
    Error.Reset();
    return true;
}

bool CheckAuthoredReferences(const FString& Base, UBlueprint* Owner, FString& Error)
{
    auto& R = FModuleManager::LoadModuleChecked<FDasRuntimeModule>(TEXT("DasRuntime"));
    UBlueprint* TargetBP = nullptr;
    UBlueprint* DriverBP = nullptr;
    if (!FDasDSLService::Apply(TEXT("dsl_ref_scalar_target.das"),
                                    Base + TEXT("/BP_ScalarTarget"), TEXT("refscalar.ScalarTarget"), TargetBP, Error) ||
                                                !FDasDSLService::Apply(TEXT("dsl_ref_scalar_driver.das"),
                                                                            Base + TEXT("/BP_RefHarness"),
                                                                                        TEXT("refscalar.RefHarness"),
                                                                                             DriverBP, Error))
        return Fail(Error, TEXT("Scalar reference fixtures import failed"));
    UClass* OriginalClass = TargetBP->GeneratedClass;
    CastChecked<ADasActor>(OriginalClass->GetDefaultObject())->AuthoredTypeId.Reset();
    UBlueprint* Reapplied = nullptr;
    if (!FDasDSLService::Apply(
            TEXT("dsl_ref_scalar_target.das"),
                 Base + TEXT("/BP_ScalarTarget"), TEXT("refscalar.ScalarTarget"), Reapplied, Error) ||
                             Reapplied != TargetBP || TargetBP->GeneratedClass != OriginalClass ||
                             CastChecked<ADasActor>(OriginalClass->GetDefaultObject())->AuthoredTypeId !=
                                 TEXT("refscalar.ScalarTarget"))
        return Fail(Error, TEXT("Reapplied import did not restore cooked identity in place"));
    UE_LOG(LogTemp, Display, TEXT("PASS DSL_existing_import_receives_cooked_identity_without_recompile"));
    TStrongObjectPtr<UWorld> World(UWorld::CreateWorld(EWorldType::Game, false));
    auto* Target = World->SpawnActor<ADasActor>(TargetBP->GeneratedClass);
    auto* Driver = World->SpawnActor<ADasActor>(DriverBP->GeneratedClass);
    FDasScalarValue Input;
    Input.Kind = 8;
    Input.ObjectClass = TEXT("/Script/CoreUObject.Object");
    Input.ObjectValue = Target;
    const FString File = R.ScriptPath(TEXT("dsl_ref_scalar_driver.das"));
    auto Call = [&](const TCHAR* Name, const TArray<FDasScalarValue>& Inputs, EDasMethodSignature Return)
    {
        return R.RunNamedFramePersistent(File, Name, Inputs, Return, Driver);
    };
    const auto Changed = Call(TEXT("ue_RefHarness_Exercise"), {Input}, EDasMethodSignature::Int);
    auto* Number = FindFProperty<FIntProperty>(Target->GetClass(), TEXT("Number"));
    if (!Changed.bSucceeded || Changed.Value != 115 || !Number || Number->GetPropertyValue_InContainer(Target) != 15)
    {
        Error = TEXT("Typed scalar object access failed: ")+ Changed.Error;
        return false;
    }
    UE_LOG(LogTemp, Display, TEXT("PASS DSL_typed_other_object_six_scalar_fields_alias_and_compound_write"));
    FDasScalarValue PeerValue = Input;
    PeerValue.ObjectValue = Driver;
    const auto Values = Call(TEXT("ue_RefHarness_Values"), {Input, PeerValue}, EDasMethodSignature::Int);
    if (!Values.bSucceeded || Values.Value != 131)
    {
        Error = TEXT("Typed enum/struct/object field access failed: ") + Values.Error;
        return false;
    }
    UE_LOG(LogTemp, Display, TEXT("PASS DSL_typed_other_object_enum_struct_and_object_fields"));
    const auto BadStruct = Call(TEXT("ue_RefHarness_WrongStruct"), {Input}, EDasMethodSignature::Void);
    const auto BadEnum = Call(TEXT("ue_RefHarness_WrongEnum"), {Input}, EDasMethodSignature::Void);
    PeerValue.ObjectValue = NewObject<UDasScriptComponent>();
    const auto BadObject = Call(TEXT("ue_RefHarness_WrongObject"), {Input, PeerValue}, EDasMethodSignature::Void);
    if (BadStruct.bSucceeded || BadEnum.bSucceeded || BadObject.bSucceeded ||
        !BadStruct.Error.Contains(
            TEXT("Property snapshot type mismatch")) ||
            !BadEnum.Error.Contains(TEXT("Property enum identity/member mismatch")) ||
                                    !BadObject.Error.Contains(TEXT("object field class mismatch")))
        return Fail(Error, FString::Printf(TEXT("Wrong-type writes not refused: struct=[%s] enum=[%s] object=[%s]"),
                                                *BadStruct.Error, *BadEnum.Error, *BadObject.Error));
    auto* PayloadField = FindFProperty<FStructProperty>(Target->GetClass(), TEXT("Payload"));
    auto* CountField =
        PayloadField ? CastField<FIntProperty>(FindDasStructField(PayloadField->Struct, TEXT("Count"))) : nullptr;
    auto* ModeField = Target->GetClass()->FindPropertyByName(TEXT("Mode"));
    auto* TargetField = FindFProperty<FObjectProperty>(Target->GetClass(), TEXT("Target"));
    if (!PayloadField || !CountField || !ModeField || !TargetField ||
        CountField->GetPropertyValue_InContainer(PayloadField->ContainerPtrToValuePtr<void>(Target)) != 31 ||
        DasEnumPropertyNumber(ModeField, ModeField->ContainerPtrToValuePtr<void>(Target)) != 7 ||
        TargetField->GetObjectPropertyValue_InContainer(Target) != Driver)
        return Fail(Error, TEXT("Refused assignments changed the target fields"));
    UE_LOG(LogTemp, Display, TEXT("PASS DSL_typed_other_object_incompatible_value_assignments_refused"));
    Input.ObjectValue = World->SpawnActor<ADasActor>(Owner->GeneratedClass);
    const auto Wrong = Call(TEXT("ue_RefHarness_Exercise"), {Input}, EDasMethodSignature::Int);
    const auto Forged = Call(TEXT("ue_RefHarness_Forged"), {Input}, EDasMethodSignature::Int);
    if (Wrong.bSucceeded || Forged.bSucceeded ||
        !Wrong.Error.Contains(TEXT("identity mismatch")) || !Forged.Error.Contains(TEXT("identity mismatch")))
        return Fail(Error, FString::Printf(TEXT("Wrong class/forged handle not refused: wrong=[%s] forged=[%s]"),
                                                *Wrong.Error, *Forged.Error));
    UE_LOG(LogTemp, Display, TEXT("PASS DSL_typed_other_object_wrong_class_and_forged_handle_refused"));
    Input.ObjectValue = nullptr;
    const auto Null = Call(TEXT("ue_RefHarness_Exercise"), {Input}, EDasMethodSignature::Int);
    if (Null.bSucceeded || Null.Error.IsEmpty())
        return Fail(Error, TEXT("Null reference access was not refused"));
    Input.ObjectValue = Target;
    const auto Remember = Call(TEXT("ue_RefHarness_Remember"), {Input}, EDasMethodSignature::Void);
    if (!Remember.bSucceeded)
    {
        Error = TEXT("Remembering a typed reference failed: ") + Remember.Error;
        return false;
    }
    Target->Destroy();
    Input.ObjectValue = nullptr;
    const auto Expired = Call(TEXT("ue_RefHarness_Recall"), {}, EDasMethodSignature::Int);
    if (Expired.bSucceeded || Expired.Error.IsEmpty())
        return Fail(Error, TEXT("Destroyed reference access was not refused"));
    UE_LOG(LogTemp, Display, TEXT("PASS DSL_typed_other_object_null_and_destroyed_reference_refused"));
    FDasAuthoringModule Model;
    if (R.ReadAuthoringScript(R.ScriptPath(TEXT("dsl_ref_readonly_bad.das")), Model, Error) || Error.IsEmpty())
        return Fail(Error, TEXT("dsl_ref_readonly_bad.das was not refused"));
    if (R.ReadAuthoringScript(R.ScriptPath(TEXT("dsl_ref_unknown_bad.das")), Model, Error) || Error.IsEmpty())
        return Fail(Error, TEXT("dsl_ref_unknown_bad.das was not refused"));
    UE_LOG(LogTemp, Display, TEXT("PASS DSL_typed_other_object_readonly_and_unknown_field_refused"));
    UBlueprint* PeerBP = nullptr;
    if (!FDasDSLService::Apply(TEXT("dsl_ref_same_class.das"), Base + TEXT("/BP_Peer"), TEXT("refpeer.Peer"), PeerBP,
                                                                                             Error))
        return Fail(Error, TEXT("Same-class peer import failed"));
    auto* First = World->SpawnActor<ADasActor>(PeerBP->GeneratedClass);
    auto* Second = World->SpawnActor<ADasActor>(PeerBP->GeneratedClass);
    FDasScalarValue PeerInput;
    PeerInput.Kind = 8;
    PeerInput.ObjectClass = TEXT("/Script/CoreUObject.Object");
    PeerInput.ObjectValue = Second;
    const auto PeerResult =
        R.RunNamedFramePersistent(R.ScriptPath(TEXT("dsl_ref_same_class.das")), TEXT("ue_Peer_AddToPeer"), {PeerInput},
                                                                                     EDasMethodSignature::Int, First);
    auto* Score = FindFProperty<FIntProperty>(First->GetClass(), TEXT("Score"));
    if (!PeerResult.bSucceeded || PeerResult.Value != 20 || !Score ||
        Score->GetPropertyValue_InContainer(First) != 10 || Score->GetPropertyValue_InContainer(Second) != 20)
    {
        Error = TEXT("Same-class peer access failed: ") + PeerResult.Error;
        return false;
    }
    UE_LOG(LogTemp, Display, TEXT("PASS DSL_typed_other_instance_of_same_authored_class"));
    World->DestroyWorld(false);
    Error.Reset();
    return true;
}

bool CheckClassCallbacks(const FString& Base, FString& Error)
{
    auto& Runtime = FModuleManager::LoadModuleChecked<FDasRuntimeModule>(TEXT("DasRuntime"));
    FDasAuthoringModule Model, Roundtrip;
    FString Json;
    if (!Runtime.ReadAuthoringScript(Runtime.ScriptPath(TEXT("dsl_class_lifecycle_only.das")), Model, Error) ||
                                     !Model.Classes[0].Methods.IsEmpty() || Model.Classes[0].Callbacks.Num() != 3 ||
                                     !Model.ToJson(Json, Error) ||
                                     !FDasAuthoringModule::FromJson(Json, Roundtrip, Error) ||
                                     Roundtrip.Classes[0].Callbacks != Model.Classes[0].Callbacks)
        return Fail(Error, TEXT("Callback-only class model or JSON roundtrip failed"));
    UBlueprint* BP = nullptr;
    if (!FDasDSLService::Apply(TEXT("dsl_class_lifecycle_only.das"),
                                    Base + TEXT("/BP_LifecycleOnly"), TEXT("callbackprobe.LifecycleOnly"), BP, Error))
        return Fail(Error, TEXT("Callback-only class import failed"));
    TStrongObjectPtr<UWorld> World(UWorld::CreateWorld(EWorldType::Game, false));
    auto* Actor = World->SpawnActor<ADasActor>(BP->GeneratedClass);
    TStrongObjectPtr<ADasActor> Keep(Actor);
    if (Actor->LifecycleScript != TEXT("dsl_class_lifecycle_only.das"))
        return Fail(Error, TEXT("Callback-only Actor has no lifecycle script"));
    Actor->PostInitializeComponents();
    Actor->DispatchBeginPlay();
    Actor->Tick(0.125f);
    auto* Starts = FindFProperty<FIntProperty>(Actor->GetClass(), TEXT("Starts"));
    auto* Seconds = FindFProperty<FFloatProperty>(Actor->GetClass(), TEXT("Seconds"));
    const bool Began = Starts && Seconds && Starts->GetPropertyValue_InContainer(Actor) == 1 &&
                       FMath::IsNearlyEqual(Seconds->GetPropertyValue_InContainer(Actor), 0.125f) &&
                       Actor->LastDasError.IsEmpty();
    World->SetBegunPlay(true);
    Actor->Destroy();
    auto* Stops = FindFProperty<FIntProperty>(Actor->GetClass(), TEXT("Stops"));
    const bool Ended = Stops && Stops->GetPropertyValue_InContainer(Actor) == 1 &&
                       Runtime.PersistentContextCount(World.Get()) == 0 && Actor->LastDasError.IsEmpty();
    World->DestroyWorld(false);
    if (!Began || !Ended)
    {
        Error = FString::Printf(TEXT("Callback-only Actor lifecycle failed: began=%d ended=%d starts=%d stops=%d seconds=%g contexts=%d error=%s"),
            Began, Ended, Starts ? Starts->GetPropertyValue_InContainer(Actor) : -1,
            Stops ? Stops->GetPropertyValue_InContainer(Actor) : -1,
            Seconds ? Seconds->GetPropertyValue_InContainer(Actor) : -1.f, Runtime.PersistentContextCount(World.Get()),
            *Actor->LastDasError);
        return false;
    }
    UE_LOG(LogTemp, Display, TEXT("PASS DSL_callback_only_class_auto_source_lifecycle_fields_and_wire"));
    for (const auto Pair : {TPair<const TCHAR*, const TCHAR*>(
             TEXT("dsl_class_lifecycle_bad_signature.das"), TEXT("by-value float")),
             TPair<const TCHAR*, const TCHAR*>(
                 TEXT("dsl_class_lifecycle_duplicate.das"), TEXT("Repeated singleton annotation")),
                 TPair<const TCHAR*, const TCHAR*>(TEXT("dsl_class_callback_conflict.das"),
                                                        TEXT("Conflicting UE callback/function annotations"))})
    {
        if (Runtime.ReadAuthoringScript(Runtime.ScriptPath(Pair.Key), Model, Error) || !Error.Contains(Pair.Value))
            return Fail(Error, FString::Printf(TEXT("%s was not refused with [%s]"), Pair.Key, Pair.Value));
    }
    UE_LOG(LogTemp, Display, TEXT("PASS DSL_callback_signature_duplicate_and_conflict_refused"));
    Error.Reset();
    return true;
}

bool CheckCompositionImport(const FString& Base, UBlueprint* Owner, FString& Error)
{
    auto& Runtime = FModuleManager::LoadModuleChecked<FDasRuntimeModule>(TEXT("DasRuntime"));
    FDasAuthoringModule Model, Roundtrip;
    FString Json;
    if (!Runtime.ReadAuthoringScript(Runtime.ScriptPath(TEXT("dsl_component_composition_dual.das")), Model, Error) ||
                                     !Model.ToJson(Json, Error) ||
                                     !FDasAuthoringModule::FromJson(Json, Roundtrip, Error) ||
                                     Roundtrip.Classes[0].Components.Num() != 2)
        return Fail(Error, TEXT("Dual composition model or JSON roundtrip failed"));
    UBlueprint* Dual = nullptr;
    if (!FDasDSLService::Apply(TEXT("dsl_component_composition_dual.das"),
                                    Base + TEXT("/BP_DualHost"), TEXT("compositionprobe.DualHost"), Dual, Error))
        return Fail(Error, TEXT("Dual composition import failed"));
    USCS_Node* Primary = nullptr;
    USCS_Node* Secondary = nullptr;
    for (auto* Node : Dual->SimpleConstructionScript->GetAllNodes())
    {
        if (Node->GetVariableName() == TEXT("Primary"))
            Primary = Node;
        if (Node->GetVariableName() == TEXT("Secondary"))
            Secondary = Node;
    }
    if (!Primary || !Secondary || Primary == Secondary || Primary->ComponentClass != Secondary->ComponentClass)
    {
        Error = TEXT("Repeated component annotations did not create two instances of the same generated type");
        return false;
    }
    UE_LOG(LogTemp, Display, TEXT("PASS DSL_repeated_components_shared_class_distinct_templates_and_wire"));
    if (Runtime.ReadAuthoringScript(Runtime.ScriptPath(TEXT("dsl_component_composition_duplicate.das")), Model,
                                                       Error) ||
                                    !Error.Contains(TEXT("Invalid/duplicate component declaration")))
        return Fail(Error, TEXT("Duplicate component names were not refused"));
    UE_LOG(LogTemp, Display, TEXT("PASS DSL_duplicate_component_names_refused"));
    UBlueprint* Invalid = nullptr;
    if (FDasDSLService::Apply(
            TEXT("dsl_component_composition_wrong_type.das"),
                 Base + TEXT("/BP_Invalid"), TEXT("compositionprobe.DualHost"), Invalid, Error) ||
                             !Error.Contains(TEXT("Component type must inherit authored ActorComponent")) ||
                                             FindObject<UObject>(nullptr, *(Base + TEXT("/BP_Invalid.BP_Invalid"))))
        return Fail(Error, TEXT("Non-component type was not refused before Actor creation"));
    UE_LOG(LogTemp, Display, TEXT("PASS DSL_noncomponent_type_refused_before_actor_creation"));
    UBlueprint* Collision = nullptr;
    const FString CollisionPackage = Base + TEXT("/Collision/BP_ScoreBehavior");
    if (FDasDSLService::Apply(TEXT("dsl_actor_component_owner.das"), CollisionPackage,
                                   TEXT("dsl_actor_component_owner.ComponentOwner"), Collision, Error) ||
                                   !Error.Contains(TEXT("conflicts with Actor package")) ||
                                                   FindObject<UObject>(nullptr,
                                                                       *(CollisionPackage + TEXT(".BP_ScoreBehavior"))))
        return Fail(Error, TEXT("Component/Actor asset path collision was not refused"));
    UE_LOG(LogTemp, Display, TEXT("PASS DSL_component_actor_asset_path_collision_refused"));
    const FString ForeignPackage = Base + TEXT("/Foreign/BP_ScoreBehavior");
    auto* Foreign = FKismetEditorUtilities::CreateBlueprint(
        UDasScriptComponent::StaticClass(), CreatePackage(*ForeignPackage),
        TEXT("BP_ScoreBehavior"), BPTYPE_Normal, UBlueprint::StaticClass(), UBlueprintGeneratedClass::StaticClass(),
             TEXT("CompositionOwnershipProbe"));
    if (!Foreign)
        return Fail(Error, TEXT("Could not create the foreign component Blueprint"));
    UClass* ForeignClass = Foreign->GeneratedClass;
    UBlueprint* ForeignOwner = nullptr;
    if (FDasDSLService::Apply(TEXT("dsl_actor_component_owner.das"),
                                   Base + TEXT("/Foreign/BP_Owner"),
                                               TEXT("dsl_actor_component_owner.ComponentOwner"), ForeignOwner, Error) ||
                                               !Error.Contains(TEXT("occupied by a different asset")) ||
                                                               Foreign->GeneratedClass != ForeignClass)
        return Fail(Error, TEXT("Import over an unrelated asset was not refused or changed it"));
    UE_LOG(LogTemp, Display, TEXT("PASS DSL_component_import_preserves_unrelated_asset"));
    auto& Meta = Owner->GetOutermost()->GetMetaData();
    const FString Original = Meta.GetValue(Owner, TEXT("DasDSLModel"));
    if (!FDasAuthoringModule::FromJson(Original, Model, Error))
        return Fail(Error, TEXT("Stored owner model does not parse"));
    // A component whose class changed (here: the stored model claims another class) stays a new Blueprint.
    Model.Classes[0].Components[0].ClassName += TEXT("Old");
    if (!Model.ToJson(Json, Error))
        return Fail(Error, TEXT("Modified owner model does not serialize"));
    Meta.SetValue(Owner, TEXT("DasDSLModel"), *Json);
    UClass* Before = Owner->GeneratedClass;
    UBlueprint* Out = nullptr;
    const bool Applied = FDasDSLService::Apply(TEXT("dsl_actor_component_owner.das"), Owner->GetOutermost()->GetName(),
                                                    TEXT("dsl_actor_component_owner.ComponentOwner"), Out, Error);
    Meta.SetValue(Owner, TEXT("DasDSLModel"), *Original);
    if (Applied || !Error.Contains(TEXT("component class")) || Owner->GeneratedClass != Before)
        return Fail(Error, FString::Printf(TEXT("Component class change not refused (applied=%d, class kept=%d)"),
                                                Applied, Owner->GeneratedClass == Before));
    UE_LOG(LogTemp, Display, TEXT("PASS DSL_component_class_change_refused_without_recompile"));
    Error.Reset();
    return true;
}

UDasActorComponentCommandlet::UDasActorComponentCommandlet()
{
    IsClient = false;
    IsServer = false;
    IsEditor = true;
    LogToConsole = true;
}
int32 UDasActorComponentCommandlet::Main(const FString& Params)
{
    FString Id, Phase;
    FParse::Value(*Params, TEXT("ProbeId="), Id);
    FParse::Value(*Params, TEXT("Phase="), Phase);
    FGuid Guid;
    if (!FGuid::ParseExact(Id, EGuidFormats::Digits, Guid))
    {
        UE_LOG(LogTemp, Error, TEXT("Invalid -ProbeId=%s"), *Id);
        return 1;
    }
    const FString Base = TEXT("/Game/DasActorComponent/")+ Id;
    UBlueprint* BP = nullptr;
    FString Error;
    if (Phase == TEXT("Create"))
    {
        auto& Runtime = FModuleManager::LoadModuleChecked<FDasRuntimeModule>(TEXT("DasRuntime"));
        if (!Runtime.ValidateIntMethod(Runtime.ScriptPath(TEXT("dsl_method_positive.das")), Error))
        {
            UE_LOG(LogTemp, Error, TEXT("Positive method fixture failed: %s"), *Error);
            return 1;
        }
        for (const auto* File : {TEXT("dsl_method_bad_unknown.das"), TEXT("dsl_method_bad_wrong_arg.das"),
                                                                          TEXT("dsl_method_bad_immutable_ref.das"),
                                                                               TEXT("dsl_method_bad_wrong_count.das")})
        {
            if (Runtime.ValidateIntMethod(Runtime.ScriptPath(File), Error) || !Error.Contains(TEXT("error[")))
            {
                UE_LOG(LogTemp, Error, TEXT("Method negative fixture unexpectedly compiled: %s"), File);
                return 1;
            }
        }
        UE_LOG(LogTemp, Display, TEXT("PASS typed_method_unknown_type_count_and_mutability_diagnostics"));
        if (!FDasDSLService::Apply(TEXT("dsl_actor_component_owner.das"),
                                        Base + TEXT("/BP_Owner"), TEXT("dsl_actor_component_owner.ComponentOwner"), BP,
                                                                       Error))
        {
            UE_LOG(LogTemp, Error, TEXT("%s"), *Error);
            return 1;
        }
        UBlueprint* ComponentBP = LoadObject<UBlueprint>(nullptr, *(Base + TEXT("/BP_ScoreBehavior.BP_ScoreBehavior")));
        if (!ComponentBP)
        {
            UE_LOG(LogTemp, Error, TEXT("Component Blueprint was not created"));
            return 1;
        }
        UClass* BeforeClass = BP->GeneratedClass;
        UBlueprint* Reapplied = nullptr;
        if (!FDasDSLService::Apply(TEXT("dsl_actor_component_owner.das"),
                                        Base + TEXT("/BP_Owner"), TEXT("dsl_actor_component_owner.ComponentOwner"),
                                                                       Reapplied, Error) ||
                                                    Reapplied != BP || BP->GeneratedClass != BeforeClass)
        {
            UE_LOG(LogTemp, Error, TEXT("Composition body-only reimport failed: %s"), *Error);
            return 1;
        }
        UE_LOG(LogTemp, Display, TEXT("PASS DSL_component_composition_body_only_without_recompile"));
        if (!CheckComponentFieldMetadata(BP, true, Error) || !CheckOwnerAccessor(Base, BP, Error) ||
            !CheckAuthoredReferences(Base, BP, Error) || !CheckClassCallbacks(Base, Error) ||
            !CheckCompositionImport(Base, BP, Error))
        {
            UE_LOG(LogTemp, Error, TEXT("Composition check failed: %s"), *Error);
            return 1;
        }
        auto Save = [](UObject* O)
        {
            FSavePackageArgs Args;
            Args.TopLevelFlags = RF_Public | RF_Standalone;
            return UPackage::SavePackage(
                O->GetOutermost(), O,
                *FPackageName::LongPackageNameToFilename(O->GetOutermost()->GetName(),
                                                         O->IsA<UWorld>() ? FPackageName::GetMapPackageExtension()
                                                                          : FPackageName::GetAssetPackageExtension()),
                Args);
        };
        if (!Save(ComponentBP) || !Save(BP))
        {
            UE_LOG(LogTemp, Error, TEXT("Saving the probe Blueprints failed"));
            return 1;
        }
        auto* W = UWorld::CreateWorld(EWorldType::Editor, false, TEXT("Map"), CreatePackage(*(Base + TEXT("/Map"))));
        W->SetFlags(RF_Public | RF_Standalone);
        W->GetOutermost()->SetPackageFlags(PKG_ContainsMap);
        W->SpawnActor<ADasActor>(BP->GeneratedClass);
        if (!Save(W))
        {
            UE_LOG(LogTemp, Error, TEXT("Saving the probe map failed"));
            return 1;
        }
        W->DestroyWorld(false);
    }
    else if (Phase == TEXT("Reopen"))
    {
        BP = LoadObject<UBlueprint>(nullptr, *(Base + TEXT("/BP_Owner.BP_Owner")));
        if (!BP)
        {
            UE_LOG(LogTemp, Error, TEXT("Saved owner Blueprint did not load"));
            return 1;
        }
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Unknown -Phase=%s"), *Phase);
        return 1;
    }
    if (!CheckComponentFieldMetadata(BP, false, Error))
    {
        UE_LOG(LogTemp, Error, TEXT("%s"), *Error);
        return 1;
    }
    TStrongObjectPtr<UBlueprint> Keep(BP);
    FString Report;
    const bool Good = RunDasActorComponentProbe(BP->GeneratedClass, Report);
    UE_LOG(LogTemp, Display, TEXT("%s"), *Report);
    return Good ? 0 : 1;
}
