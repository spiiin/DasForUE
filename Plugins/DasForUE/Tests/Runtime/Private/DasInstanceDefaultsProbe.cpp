#include "DasStaticLibraryProbe.h"
#include "DasProbeActor.h"
#include "DasScriptComponent.h"
#include "DasRuntime.h"
#include "Engine/World.h"
#include "Modules/ModuleManager.h"
#include "Misc/ScopeExit.h"
#include "UObject/StructOnScope.h"
#include "UObject/UnrealType.h"

bool RunDasInstanceDefaultsProbe(ADasStaticLibraryProbe* Carrier, FString& Report)
{
    bool Good = true;
    auto Check = [&](bool Value, const TCHAR* Name)
    {
        Good &= Value;
        Report += FString(Value ? TEXT("PASS "): TEXT("FAIL ")) + Name + TEXT("\n");
    };
    auto* World = Carrier ? Carrier->GetWorld() : nullptr;
    if (!World || !World->IsGameWorld() || !Carrier->DefaultActorClass || !Carrier->DefaultChildClass ||
        !Carrier->DefaultComponentClass)
    {
        Check(false, TEXT("instance_defaults_cooked_class_dependencies"));
        return false;
    }
    auto& Runtime = FModuleManager::LoadModuleChecked<FDasRuntimeModule>(TEXT("DasRuntime"));
    auto* Actor = World->SpawnActor<ADasProbeActor>(Carrier->DefaultActorClass);
    auto* Child = World->SpawnActor<ADasProbeActor>(Carrier->DefaultChildClass);
    UDasScriptComponent* Component =
        Actor ? NewObject<UDasScriptComponent>(Actor, Carrier->DefaultComponentClass) : nullptr;
    ON_SCOPE_EXIT
    {
        if (Component)
        {
            Runtime.ReleaseObject(Component);
            Component->DestroyComponent();
        }
        if (Child)
        {
            Runtime.ReleaseObject(Child);
            Child->Destroy();
        }
        if (Actor)
        {
            Runtime.ReleaseObject(Actor);
            Actor->Destroy();
        }
    };
    if (!Actor || !Child || !Component)
    {
        Check(false, TEXT("instance_defaults_live_objects"));
        return false;
    }
    Actor->AddInstanceComponent(Component);
    Component->RegisterComponent();
    Check(Component->IsRegistered() && Actor->HasActorBegunPlay() && Component->HasBegunPlay() &&
              Child->HasActorBegunPlay(),
          TEXT("instance_defaults_after_BeginPlay"));
    auto Invoke = [&](UObject* Target, const TCHAR* Name, int32 Expected)
    {
        auto* Function = Target->FindFunction(Name);
        if (!Function || !CastField<FIntProperty>(Function->GetReturnProperty()))
            return false;
        FStructOnScope Args(Function);
        Target->ProcessEvent(Function, Args.GetStructMemory());
        return CastFieldChecked<FIntProperty>(Function->GetReturnProperty())
                   ->GetPropertyValue_InContainer(Args.GetStructMemory()) == Expected;
    };
    Check(Invoke(Actor, TEXT("CheckDefaults"), 1), TEXT("instance_defaults_actor_self_scalar_literals"));
    Check(Invoke(Component, TEXT("CheckDefaults"), 1), TEXT("instance_defaults_component_self_scalar_literals"));
    Check(Invoke(Actor, TEXT("EvaluatePure"), 5), TEXT("instance_defaults_actor_Blueprint_omitted_arguments"));
    Check(Invoke(Component, TEXT("EvaluatePure"), 5), TEXT("instance_defaults_component_Blueprint_omitted_arguments"));
    Check(Invoke(Child, TEXT("SuperProbe"), 22) && Invoke(Child, TEXT("EventProbe"), 12),
                                                          TEXT("instance_defaults_virtual_Super_callsite_values"));
    auto Reference = [&](UObject* Target, const TCHAR* Script)
    {
        const auto Result = Runtime.RunNamedFramePersistent(Runtime.ScriptPath(Script),
                                                            TEXT("main"), {}, EDasMethodSignature::Int, Target);
        if (!Result.bSucceeded)
            Report += Result.Error + TEXT("\n");
        return Result.bSucceeded && Result.Value == 1;
    };
    Check(Reference(Actor, TEXT("dsl_defaults_actor_calls.das")),
                    TEXT("instance_defaults_actor_reference_omitted_partial_explicit"));
    Check(Reference(Component, TEXT("dsl_defaults_component_calls.das")),
                    TEXT("instance_defaults_component_reference_omitted_partial_explicit"));
    Check(Reference(Child, TEXT("dsl_defaults_child_calls.das")),
                    TEXT("instance_defaults_inherited_reference_dispatch"));
    Check(Actor->LastDasError.IsEmpty() && Component->LastDasError.IsEmpty() && Child->LastDasError.IsEmpty(),
          TEXT("instance_defaults_no_runtime_errors"));
    return Good;
}
