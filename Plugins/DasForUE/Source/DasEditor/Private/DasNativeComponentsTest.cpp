#include "DasDSLService.h"
#include "DasComponentDefaults.h"
#include "DasAuthoring.h"
#include "DasRuntime.h"
#include "DasProbeActor.h"
#include "Engine/Blueprint.h"
#include "Engine/BlueprintGeneratedClass.h"
#include "Engine/World.h"
#include "Components/SceneComponent.h"
#include "UObject/UnrealType.h"
#include "UObject/StrongObjectPtr.h"
#if WITH_DEV_AUTOMATION_TESTS
#include "Misc/AutomationTest.h"
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FDasNativeComponentsTest, "DasForUE.Components.Native",
                                 EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FDasNativeComponentsTest::RunTest(const FString&)
{
    auto& Runtime = FModuleManager::LoadModuleChecked<FDasRuntimeModule>(TEXT("DasRuntime"));
    FDasAuthoringModule Model, Copy;
    FString Error, Wire;
    if (!TestTrue(
            TEXT("collect native and authored components"),
                 Runtime.ReadAuthoringScript(Runtime.ScriptPath(TEXT("dsl_native_components.das")), Model, Error)))
    {
        AddError(Error);
        return false;
    }
    for (const auto* Name : {TEXT("duplicate"), TEXT("unknown"), TEXT("call"), TEXT("expression")})
    {
        FDasAuthoringModule Rejected;
        FString Issue;
        TestFalse(FString(TEXT("defaults reject ")) + Name,
                          Runtime.ReadAuthoringScript(Runtime.ScriptPath(
                              FString(TEXT("dsl_defaults_bad_")) + Name + TEXT(".das")), Rejected, Issue));
        TestFalse(TEXT("defaults diagnostic is present"), Issue.IsEmpty());
    }
    TestTrue(TEXT("wire roundtrip"), Model.ToJson(Wire, Error) && FDasAuthoringModule::FromJson(Wire, Copy, Error) &&
                                         Copy.Classes[0].Components == Model.Classes[0].Components);
    FDasAuthoringModule Bad = Model;
    Bad.Classes[0].Components[0].Attach = TEXT("MissingComponent");
    TestFalse(TEXT("unknown parent rejected"), Bad.Validate(Error));
    Bad = Model;
    for (auto& Component : Bad.Classes[0].Components)
        Component.bRoot = true;
    TestFalse(TEXT("multiple roots rejected"), Bad.Validate(Error));
    Bad = Model;
    for (auto& Component : Bad.Classes[0].Components)
    {
        Component.bRoot = false;
        Component.Attach = Component.Name;
    }
    TestFalse(TEXT("cycles rejected"), Bad.Validate(Error));
    FDasDeclaredComponent InvalidDefault;
    InvalidDefault.Name = TEXT("Pivot");
    FDasDeclaredLiteral Wrong;
    Wrong.Kind = TEXT("number");
    Wrong.Number = 1;
    InvalidDefault.Defaults.Add(TEXT("bHiddenInGame"), Wrong);
    TestFalse(TEXT("numeric value cannot initialize bool"),
                   DasComponentDefaults(USceneComponent::StaticClass(), InvalidDefault, nullptr, Error));
    InvalidDefault.Defaults.Reset();
    Wrong.Kind = TEXT("bool");
    InvalidDefault.Defaults.Add(TEXT("MissingProperty"), Wrong);
    TestFalse(TEXT("unknown template property rejected"),
                   DasComponentDefaults(USceneComponent::StaticClass(), InvalidDefault, nullptr, Error));
    InvalidDefault.Defaults.Reset();
    InvalidDefault.Defaults.Add(TEXT("RelativeLocation"), Wrong);
    TestFalse(TEXT("bool cannot initialize vector"),
                   DasComponentDefaults(USceneComponent::StaticClass(), InvalidDefault, nullptr, Error));
    Error.Reset();
    const FString Package =
        TEXT("/Game/DasNativeComponents/")+ FGuid::NewGuid().ToString(EGuidFormats::Digits) + TEXT("/BP_Owner");
    UBlueprint* BP = nullptr;
    if (!TestTrue(TEXT("create mixed SCS"),
                       FDasDSLService::Apply(TEXT("dsl_native_components.das"), Package,
                                                  TEXT("nativecomponents.NativeOwner"), BP, Error)))
    {
        AddError(Error);
        return false;
    }
    TestNull(TEXT("defaults label is not a runtime field"), BP->GeneratedClass->FindPropertyByName(TEXT("defaults")));
    TStrongObjectPtr<UWorld> World(UWorld::CreateWorld(EWorldType::Game, false));
    auto* Actor = World->SpawnActor<ADasProbeActor>(BP->GeneratedClass);
    if (!TestNotNull(TEXT("spawn"), Actor))
    {
        World->DestroyWorld(false);
        return false;
    }
    auto* Property = FindFProperty<FObjectProperty>(Actor->GetClass(), TEXT("Pivot"));
    auto* Pivot = Property ? Cast<USceneComponent>(Property->GetObjectPropertyValue_InContainer(Actor)) : nullptr;
    TestTrue(TEXT("native template instantiates on owner"), Pivot && Pivot->GetOwner() == Actor);
    TestTrue(TEXT("declared root replaces host fallback"), Pivot && Actor->GetRootComponent() == Pivot);
    auto* ChildProperty = FindFProperty<FObjectProperty>(Actor->GetClass(), TEXT("Child"));
    auto* Child =
        ChildProperty ? Cast<USceneComponent>(ChildProperty->GetObjectPropertyValue_InContainer(Actor)) : nullptr;
    TestTrue(TEXT("implicit attachment to declared root"), Child && Child->GetAttachParent() == Pivot);
    TestTrue(TEXT("template defaults copied at spawn"),
                  Child && Child->GetRelativeLocation().Equals(FVector(0, 0, 50)) &&
                      Child->GetRelativeRotation().Equals(FRotator(0, 30, 0)) && Child->bHiddenInGame);
    auto* GrandProperty = FindFProperty<FObjectProperty>(Actor->GetClass(), TEXT("Grandchild"));
    auto* Grand =
        GrandProperty ? Cast<USceneComponent>(GrandProperty->GetObjectPropertyValue_InContainer(Actor)) : nullptr;
    TestTrue(TEXT("explicit attachment"), Grand && Grand->GetAttachParent() == Child);
    TestTrue(TEXT("mixed authored component present"),
                  FindFProperty<FObjectProperty>(Actor->GetClass(), TEXT("Behavior")) != nullptr);
    auto ComponentValue = [&](const TCHAR* Name)
    {
        auto* Field = FindFProperty<FObjectProperty>(Actor->GetClass(), Name);
        return Field ? Field->GetObjectPropertyValue_InContainer(Actor) : nullptr;
    };
    auto* Behavior = ComponentValue(TEXT("Behavior"));
    auto* OtherBehavior = ComponentValue(TEXT("OtherBehavior"));
    auto* Bonus = Behavior ? FindFProperty<FIntProperty>(Behavior->GetClass(), TEXT("Bonus")) : nullptr;
    TestTrue(TEXT("per-template authored defaults preserve sibling and class CDO"),
                  Bonus && OtherBehavior && Bonus->GetPropertyValue_InContainer(Behavior) == 42 &&
                      Bonus->GetPropertyValue_InContainer(OtherBehavior) == 10 &&
                      Bonus->GetPropertyValue_InContainer(Behavior->GetClass()->GetDefaultObject()) == 10);
    TestTrue(TEXT("assign known instance name for authored lookup"),
                  Behavior && Behavior->Rename(TEXT("LookupBehavior"), nullptr,
                                                    REN_DontCreateRedirectors | REN_NonTransactional));
    auto* DynamicClass = FindFProperty<FStrProperty>(Actor->GetClass(), TEXT("DynamicComponentClass"));
    if (!DynamicClass || !Behavior)
    {
        AddError(TEXT("missing dynamic component fixture"));
        World->DestroyWorld(false);
        return false;
    }
    DynamicClass->SetPropertyValue_InContainer(Actor, Behavior->GetClass()->GetPathName());
    const auto Result = Runtime.RunNamedBoolPersistent(
        Runtime.ScriptPath(TEXT("dsl_native_components.das")), TEXT("ue_NativeOwner_HasPivot"), false, Actor);
    TestTrue(TEXT("native self reference"), Result.bSucceeded && Result.BoolValue);
    UBlueprint* Again = nullptr;
    TestTrue(TEXT("idempotent reimport"),
                  FDasDSLService::Apply(TEXT("dsl_native_components.das"), Package,
                                             TEXT("nativecomponents.NativeOwner"), Again, Error) && Again == BP);
    World->DestroyWorld(false);
    return !HasAnyErrors();
}
#endif
