#include "DasDSLService.h"
#include "DasScriptHosts.h"
#include "DasGenericParents.h"
#include "DasScriptClassData.h"
#include "DasRuntime.h"
#include "DasAuthoring.h"
#include "DasGenericBlueprints.h"
#include "Misc/AutomationTest.h"
#include "Misc/FileHelper.h"
#include "Misc/ScopeExit.h"
#include "Engine/Blueprint.h"
#include "Engine/BlueprintGeneratedClass.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "GameFramework/GameModeBase.h"
#include "GameFramework/PlayerController.h"
#include "HAL/FileManager.h"
#include "Components/SceneComponent.h"
#include "Kismet2/KismetEditorUtilities.h"
#include "UObject/StrongObjectPtr.h"
#include "UObject/StructOnScope.h"
#include "UObject/Package.h"
#include "UObject/SavePackage.h"
#include "UObject/UnrealType.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FDasGenericParentsTest, "DasForUE.DSL.GenericParents",
                                 EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FDasGenericParentsTest::RunTest(const FString&)
{
    auto& Runtime = FModuleManager::LoadModuleChecked<FDasRuntimeModule>(TEXT("DasRuntime"));
    const FString Token = FGuid::NewGuid().ToString(EGuidFormats::Digits);
    const FString Root = TEXT("/Game/Tests/DasGenericParents/") + Token;
    TArray<FString> Scripts;
    ON_SCOPE_EXIT
    {
        for (const auto& Script : Scripts)
            IFileManager::Get().Delete(*Runtime.ScriptPath(Script), false, true);
    };
    FString Error;
    auto Apply = [&](const FString& Name, const FString& Source, UBlueprint*& BP)
    {
        const FString Script = TEXT("generic_") + Token + TEXT("_") + Name + TEXT(".das");
        Scripts.Add(Script);
        const bool Written = FFileHelper::SaveStringToFile(Source, *Runtime.ScriptPath(Script),
                                                           FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM);
        if (!Written ||
            !FDasDSLService::Apply(Script, Root + TEXT("/BP_") + Name, TEXT("genericparents.") + Name, BP, Error))
        {
            AddError(Error);
            return false;
        }
        return true;
    };
    const FString Prefix = TEXT("options gen2\nrequire ue_dsl\n");
    const FString Body = TEXT(" @ue_property=(EditAnywhere,BlueprintReadWrite)\n Score:int=7\n")
        TEXT(" [ue_function(BlueprintCallable)]\n def Add(input:int):int { self.Score+=input; return self.Score }\n")
            TEXT(" [ue_function(BlueprintPure)]\n def const Read(input:int):int { return self.Score+input }\n");
    UBlueprint *Mode = nullptr, *Component = nullptr, *Existing = nullptr;
    if (!Apply(TEXT("Mode"),
            Prefix +
                TEXT("[ue_class(module_id=\"genericparents\",parent=\"GameModeBase\")]\nclass Mode {\n") + Body +
                    TEXT(" def BeginPlay():void { self.Score+=10 }\n def Tick(delta:float):void { self.Score+=1 }\n def EndPlay(reason:int):void { self.Score+=100 }\n}\n"),
                        Mode))
        return false;
    TestEqual(TEXT("Exact native GameModeBase parent; no host substitution"), Mode->ParentClass.Get(),
                   AGameModeBase::StaticClass());
    if (!Apply(TEXT("Component"),
                    Prefix + TEXT("[ue_native_base(path=\"/Script/Engine.SceneComponent\")]\nclass SceneBase {}\n") +
                                  TEXT("[ue_class(module_id=\"genericparents\")]\nclass Component:SceneBase {\n") +
                                       Body + TEXT("}\n"), Component))
        return false;
    TestEqual(TEXT("Ordinary DAS inheritance resolves SceneComponent"), Component->ParentClass.Get(),
                   USceneComponent::StaticClass());
    // A plain Blueprint parent, created independently of DAS, must keep its exact generated class.
    auto* Plain = FKismetEditorUtilities::CreateBlueprint(
        AGameModeBase::StaticClass(), CreatePackage(*(Root + TEXT("/BP_Plain"))),
                                                    TEXT("BP_Plain"), BPTYPE_Normal, UBlueprint::StaticClass(),
                                                         UBlueprintGeneratedClass::StaticClass());
    if (!TestNotNull(TEXT("Plain Blueprint parent"), Plain))
        return false;
    const FString PlainPath = Plain->GeneratedClass->GetPathName();
    if (!Apply(TEXT("Existing"), Prefix + TEXT("[ue_class(module_id=\"genericparents\",parent=\"") + PlainPath +
                                               TEXT("\")]\nclass Existing {\n") + Body + TEXT("}\n"), Existing))
        return false;
    TestEqual(TEXT("Exact existing Blueprint parent"), Existing->ParentClass.Get(), Plain->GeneratedClass.Get());
    TStrongObjectPtr<UWorld> World(UWorld::CreateWorld(EWorldType::Game, false));
    auto& Context = GEngine->CreateNewWorldContext(EWorldType::Game);
    Context.SetCurrentWorld(World.Get());
    ON_SCOPE_EXIT
    {
        GEngine->DestroyWorldContext(World.Get());
        World->DestroyWorld(false);
    };
    World->InitializeActorsForPlay(FURL());
    auto* First = World->SpawnActor<AGameModeBase>(Mode->GeneratedClass);
    auto* Second = World->SpawnActor<AGameModeBase>(Mode->GeneratedClass);
    if (!TestNotNull(TEXT("Spawn generic owner"), First) || !TestNotNull(TEXT("Spawn independent owner"), Second))
        return false;
    auto Call = [&](UObject* Owner, FName Name, int32 Input)
    {
        auto* F = Owner->FindFunction(Name);
        if (!TestNotNull(TEXT("Real generated UFunction"), F))
            return MIN_int32;
        FStructOnScope Frame(F);
        FindFProperty<FIntProperty>(F, TEXT("input"))->SetPropertyValue_InContainer(Frame.GetStructMemory(), Input);
        Owner->ProcessEvent(F, Frame.GetStructMemory());
        return FindFProperty<FIntProperty>(F,
                                           TEXT("ReturnValue"))->GetPropertyValue_InContainer(Frame.GetStructMemory());
    };
    TestEqual(TEXT("Common bridge executes DAS and mutates owner"), Call(First, TEXT("Add"), 5), 12);
    TestEqual(TEXT("Instances keep independent values"), Call(Second, TEXT("Read"), 0), 7);
    TestEqual(TEXT("Const/pure method via common bridge"), Call(First, TEXT("Read"), 2), 14);
    auto* Data = DasGenericClassData(Mode);
    TestTrue(TEXT("Cookable descriptor owns current class identity"),
                  Data && Data->OwnerClass == Mode->GeneratedClass &&
                      DasHostIdentity(First) == TEXT("genericparents.Mode") && Data->HasAnyFlags(RF_Public));
    // Dispatch reflected UE events through the generated Blueprint event graph.
    auto InvokeEvent = [&](const TCHAR* Name, void* Params)
    {
        First->ProcessEvent(First->FindFunction(Name), Params);
    };
    InvokeEvent(TEXT("ReceiveBeginPlay"), nullptr);
    TestEqual(TEXT("Native BeginPlay event reaches DAS hook"), Call(First, TEXT("Read"), 0), 22);
    struct
    {
        float DeltaSeconds;
    } Tick{0.01f};
    InvokeEvent(TEXT("ReceiveTick"), &Tick);
    TestEqual(TEXT("Native Tick event reaches DAS hook"), Call(First, TEXT("Read"), 0), 23);
    const int32 BeforeEndPlay = Runtime.PersistentContextCount(World.Get());
    uint8 Reason = uint8(EEndPlayReason::Quit);
    InvokeEvent(TEXT("ReceiveEndPlay"), &Reason);
    TestEqual(TEXT("EndPlay releases exactly this owner context"), Runtime.PersistentContextCount(World.Get()),
                   BeforeEndPlay - 1);
    TestEqual(TEXT("Native EndPlay hook runs before context release"), Call(First, TEXT("Read"), 0), 123);
    TStrongObjectPtr<USceneComponent> C(NewObject<USceneComponent>(First, Component->GeneratedClass));
    TestEqual(TEXT("Generic UObject-derived component executes same bridge"), Call(C.Get(), TEXT("Add"), 2), 9);
    auto* Derived = World->SpawnActor<AGameModeBase>(Existing->GeneratedClass);
    TestEqual(TEXT("External Blueprint parent executes DAS child"), Call(Derived, TEXT("Add"), 3), 10);
    Runtime.ReleaseObject(First);
    Runtime.ReleaseObject(Second);
    Runtime.ReleaseObject(C.Get());
    Runtime.ReleaseObject(Derived);
    auto* Invalid = FindObject<UClass>(nullptr, TEXT("/Script/Engine.Engine"));
    TestFalse(TEXT("Non-Blueprintable class rejected without CreateBlueprint assertion"),
                   DasCheckBlueprintParent(Invalid, Error));
    TestFalse(TEXT("Reflected interface rejected as class parent"),
                   DasCheckBlueprintParent(UInterface::StaticClass(), Error));
    FString RevisedSource;
    FFileHelper::LoadFileToString(RevisedSource, *Runtime.ScriptPath(Scripts[0]));
    RevisedSource.ReplaceInline(TEXT("self.Score+=input"), TEXT("self.Score+=input*2"));
    TestTrue(TEXT("Write body update without changing declaration"),
                  FFileHelper::SaveStringToFile(RevisedSource, *Runtime.ScriptPath(Scripts[0]),
                                                FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM));
    TestTrue(TEXT("Body-only reimport retains descriptor"),
                  FDasDSLService::Reimport(Mode, Error, EDasDefaultMigrationPolicy::Automatic));
    TestTrue(TEXT("Reimport retains exact native parent"), Mode->ParentClass == AGameModeBase::StaticClass());
    TestEqual(TEXT("Updated body preserves object values"), Call(First, TEXT("Add"), 1), 125);
    Runtime.ReleaseObject(First);
    // Verify a supported native host remains available alongside the generic path.
    TestTrue(
        TEXT("Existing Actor and PlayerController hosts retained"),
             DasNativeScriptHost(TEXT("Actor"))->GetPathName() ==
                                 TEXT("/Script/DasRuntime.DasActor") &&
                                      DasNativeScriptHost(TEXT("/Script/Engine.PlayerController"))->IsChildOf<APlayerController>());
#if DAS_WITH_TEST_PROBES
    TestFalse(TEXT("Existing probe stays on its native host path"),
        DasGenericScriptParent(TEXT("/Script/DasRuntime.DasProbeActor")));
#endif
    AddInfo(TEXT("DAS_GENERIC_PARENTS=PASS"));
    return true;
}
