#include "DasDSLService.h"
#include "DasActor.h"
#include "DasScriptComponent.h"
#include "DasRuntime.h"
#include "Editor.h"
#include "Engine/Blueprint.h"
#include "Engine/BlueprintGeneratedClass.h"
#include "EngineUtils.h"
#include "UObject/StrongObjectPtr.h"
#if WITH_DEV_AUTOMATION_TESTS
#include "Misc/AutomationTest.h"
#include "Tests/AutomationEditorCommon.h"
namespace
{
struct FLibraryState
{
    TStrongObjectPtr<UBlueprint> BP;
};
class FLibraryCommand : public IAutomationLatentCommand
{
    TSharedRef<FLibraryState> S;
    FAutomationTestBase* Test;
    bool Ending = false;
    double Start = FPlatformTime::Seconds();

public:
    FLibraryCommand(TSharedRef<FLibraryState> V, FAutomationTestBase* T) : S(V), Test(T) {}
    bool Update() override
    {
        auto* W = GEditor->PlayWorld.Get();
        auto& R = FModuleManager::LoadModuleChecked<FDasRuntimeModule>(TEXT("DasRuntime"));
        if (Ending)
        {
            if (W && FPlatformTime::Seconds() - Start < 30)
                return false;
            Test->TestTrue(TEXT("EndPIE releases library frames"), !W && R.PersistentContextCount() == 0);
            return true;
        }
        ADasActor* Actor = nullptr;
        if (W)
            for (TActorIterator<ADasActor> It(W); It; ++It)
                if (It->GetFName() == TEXT("LibraryProbe"))
                    Actor = *It;
        if (!Actor || !W->HasBegunPlay())
        {
            if (FPlatformTime::Seconds() - Start < 45)
                return false;
            Test->AddError(TEXT("PIE startup timeout"));
            GEditor->RequestEndPlayMap();
            Ending = true;
            Start = FPlatformTime::Seconds();
            return false;
        }
        auto Result = R.RunNamedBoolPersistent(R.ScriptPath(TEXT("dsl_function_libraries.das")),
                                                            TEXT("ue_LibraryProbe_Check"), false, Actor);
        Test->TestTrue(TEXT("generated engine and user C++ libraries; implicit owner WorldContext"),
                            Result.bSucceeded && Result.BoolValue);
        if (!Result.bSucceeded)
            Test->AddError(Result.Error);
        for (const auto* Name : {TEXT("Classes"), TEXT("AbstractClass")})
        {
            auto ClassResult =
                R.RunNamedBoolPersistent(R.ScriptPath(TEXT("dsl_function_libraries.das")),
                                                      FString(TEXT("ue_LibraryProbe_")) + Name, false, Actor);
            Test->TestTrue(FString(TEXT("class marshalling ")) + Name, ClassResult.bSucceeded && ClassResult.BoolValue);
            if (!ClassResult.bSucceeded)
                Test->AddError(ClassResult.Error);
        }
        auto ActualWorld = R.RunNamedPersistent(R.ScriptPath(TEXT("dsl_function_libraries.das")),
                                                             TEXT("ue_LibraryProbe_WorldId"), 0, Actor);
        Test->TestTrue(TEXT("library context is PIE owner's world"),
                            ActualWorld.bSucceeded && ActualWorld.Value == int32(W->GetUniqueID()));
        TStrongObjectPtr<UWorld> OtherWorld(UWorld::CreateWorld(EWorldType::Game, false));
        auto* OtherActor = OtherWorld->SpawnActor<ADasActor>(S->BP->GeneratedClass);
        auto OtherResult = R.RunNamedPersistent(R.ScriptPath(TEXT("dsl_function_libraries.das")),
                                                             TEXT("ue_LibraryProbe_WorldId"), 0, OtherActor);
        Test->TestTrue(TEXT("library context follows owner in a different game world"),
                            OtherResult.bSucceeded && OtherResult.Value == int32(OtherWorld->GetUniqueID()) &&
                                OtherResult.Value != ActualWorld.Value);
        OtherWorld->DestroyWorld(false);
        for (const auto* N :
             {TEXT("OverrideWorld"), TEXT("BadWorldContract"), TEXT("NotLibrary"), TEXT("WrongMetaClass")})
        {
            auto Failed = R.RunNamedBoolPersistent(R.ScriptPath(TEXT("dsl_function_libraries.das")),
                                                                FString(TEXT("ue_LibraryProbe_")) + N, false, Actor);
            Test->TestTrue(FString(TEXT("reject ")) + N, !Failed.bSucceeded && !Failed.Error.IsEmpty());
        }
        GEditor->RequestEndPlayMap();
        Ending = true;
        Start = FPlatformTime::Seconds();
        return false;
    }
};
}
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FDasFunctionLibrariesPIE, "DasForUE.FunctionLibraries.PIE",
                                 EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FDasFunctionLibrariesPIE::RunTest(const FString&)
{
    auto S = MakeShared<FLibraryState>();
    UBlueprint* BP = nullptr;
    FString Error;
    if (!FDasDSLService::Apply(TEXT("dsl_function_libraries.das"),
                                    TEXT("/Game/DasLibraryPIE/")+ FGuid::NewGuid().ToString(EGuidFormats::Digits) +
                                         TEXT("/BP_Probe"), TEXT("libraries.LibraryProbe"), BP, Error))
    {
        AddError(Error);
        return false;
    }
    auto& Runtime = FModuleManager::LoadModuleChecked<FDasRuntimeModule>(TEXT("DasRuntime"));
    TStrongObjectPtr<UObject> Worldless(NewObject<UDasScriptComponent>());
    auto Pure = Runtime.RunNamedBoolPersistent(Runtime.ScriptPath(TEXT("library_worldless_probe.das")),
                                                                  TEXT("pure"), false, Worldless.Get());
    TestTrue(TEXT("pure libraries work without world"), Pure.bSucceeded && Pure.BoolValue);
    if (!Pure.bSucceeded)
        AddError(Pure.Error);
    auto Missing = Runtime.RunNamedBoolPersistent(Runtime.ScriptPath(TEXT("library_worldless_probe.das")),
                                                                     TEXT("world"), false, Worldless.Get());
    TestTrue(TEXT("world-required library rejects worldless owner"),
                  !Missing.bSucceeded && Missing.Error.Contains(TEXT("WorldContext")));
    Runtime.ReleaseObject(Worldless.Get());
    S->BP.Reset(BP);
    auto* W = FAutomationEditorCommonUtils::CreateNewMap();
    FActorSpawnParameters P;
    P.Name = TEXT("LibraryProbe");
    if (!W->SpawnActor<ADasActor>(BP->GeneratedClass, FTransform::Identity, P))
        return false;
    ADD_LATENT_AUTOMATION_COMMAND(FStartPIECommand(false));
    FAutomationTestFramework::Get().EnqueueLatentCommand(MakeShared<FLibraryCommand>(S, this));
    return true;
}
#endif
