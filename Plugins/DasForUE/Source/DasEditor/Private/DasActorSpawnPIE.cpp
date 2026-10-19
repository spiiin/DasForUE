#include "DasDSLService.h"
#include "DasProbeActor.h"
#include "DasScriptComponent.h"
#include "DasRuntime.h"
#include "DasClassMethodsProbe.h"
#include "DasInitializerProbe.h"
#include "Editor.h"
#include "Engine/Blueprint.h"
#include "Engine/BlueprintGeneratedClass.h"
#include "EngineUtils.h"
#include "Kismet2/KismetEditorUtilities.h"
#include "UObject/StrongObjectPtr.h"
#include "UObject/UnrealType.h"
#if WITH_DEV_AUTOMATION_TESTS
#include "Misc/AutomationTest.h"
#include "Tests/AutomationEditorCommon.h"
namespace
{
int32 SpawnValue(UObject* O, const TCHAR* N)
{
    auto* P = O ? FindFProperty<FIntProperty>(O->GetClass(), N) : nullptr;
    return P ? P->GetPropertyValue_InContainer(O) : MIN_int32;
}
struct FSpawnState
{
    TStrongObjectPtr<UBlueprint> Driver, Target, Child;
    TStrongObjectPtr<ADasProbeActor> Spawned;
    TStrongObjectPtr<UDasScriptComponent> Component;
    TStrongObjectPtr<UWorld> ForeignWorld;
    TStrongObjectPtr<AActor> ForeignActor;
};
class FSpawnCommand : public IAutomationLatentCommand
{
    TSharedRef<FSpawnState> S;
    FAutomationTestBase* Test;
    int Phase = 0;
    double Started = FPlatformTime::Seconds();

public:
    FSpawnCommand(TSharedRef<FSpawnState> V, FAutomationTestBase* T) : S(V), Test(T) {}
    bool Update() override
    {
        auto* W = GEditor->PlayWorld.Get();
        auto& R = FModuleManager::LoadModuleChecked<FDasRuntimeModule>(TEXT("DasRuntime"));
        if (Phase == 2)
        {
            if (W && FPlatformTime::Seconds() - Started < 30)
                return false;
            if (S->ForeignWorld.IsValid())
            {
                S->ForeignWorld->DestroyWorld(false);
                S->ForeignActor.Reset();
                S->ForeignWorld.Reset();
            }
            Test->TestTrue(TEXT("EndPIE releases spawned sessions"), !W && R.PersistentContextCount() == 0);
            return true;
        }
        ADasProbeActor* Driver = nullptr;
        if (W)
            for (TActorIterator<ADasProbeActor> I(W); I; ++I)
                if (I->GetFName() == TEXT("SpawnDriver"))
                    Driver = *I;
        if (!Driver || !W->HasBegunPlay())
        {
            if (FPlatformTime::Seconds() - Started < 45)
                return false;
            Test->AddError(TEXT("PIE startup timeout"));
            GEditor->RequestEndPlayMap();
            Phase = 2;
            Started = FPlatformTime::Seconds();
            return false;
        }
        auto Call = [&](const TCHAR* Name)
        {
            return R.RunNamedBoolPersistent(R.ScriptPath(TEXT("dsl_spawn_driver.das")),
                                                         FString(TEXT("ue_SpawnDriver_")) + Name, false, Driver);
        };
        if (Phase == 0)
        {
            if (auto* Foreign = FindFProperty<FObjectProperty>(Driver->GetClass(), TEXT("ForeignActor")))
                Foreign->SetObjectPropertyValue_InContainer(Driver, S->ForeignActor.Get());
            auto Empty = Call(TEXT("QueryEmpty"));
            Test->TestTrue(TEXT("missing first/all and authored identity filtering"),
                                Empty.bSucceeded && Empty.BoolValue);
            if (!Empty.bSucceeded)
                Test->AddError(Empty.Error);
            bool QueriesReleased = true;
            for (int32 Iteration = 0; Iteration < 70; ++Iteration)
            {
                auto Interrupted = Call(TEXT("QueryInterrupted"));
                QueriesReleased &= !Interrupted.bSucceeded && Interrupted.Error.Contains(TEXT("query unwind fixture"));
            }
            Test->TestTrue(TEXT("query snapshots reclaimed on script errors"), QueriesReleased);
            const int32 Before = W->GetActorCount();
            for (const auto* N :
                 {TEXT("WrongClass"), TEXT("WrongCollision"), TEXT("ForeignDestroy"), TEXT("QueryForeign")})
            {
                auto Rejected = Call(N);
                Test->TestTrue(FString(TEXT("reject ")) + N, !Rejected.bSucceeded && !Rejected.Error.IsEmpty());
            }
            Test->TestTrue(TEXT("rejection leaves world unchanged"),
                                W->GetActorCount() == Before && !Driver->IsActorBeingDestroyed());
            auto Native = Call(TEXT("Native"));
            Test->TestTrue(TEXT("native actor spawn destroy and stale handle"), Native.bSucceeded && Native.BoolValue);
            auto Created = Call(TEXT("Spawn"));
            Test->TestTrue(TEXT("authored spawn and nested BeginPlay"), Created.bSucceeded && Created.BoolValue);
            if (!Created.bSucceeded)
                Test->AddError(Created.Error);
            auto* Field = FindFProperty<FObjectProperty>(Driver->GetClass(), TEXT("Spawned"));
            auto* Actor = Field ? Cast<ADasProbeActor>(Field->GetObjectPropertyValue_InContainer(Driver)) : nullptr;
            if (!Actor)
            {
                Test->AddError(TEXT("spawned actor missing"));
                GEditor->RequestEndPlayMap();
                Phase = 2;
                Started = FPlatformTime::Seconds();
                return false;
            }
            S->Spawned.Reset(Actor);
            Test->TestTrue(TEXT("Blueprint subclass and owner preserved"),
                                Actor->GetClass() == S->Child->GeneratedClass && Actor->GetOwner() == Driver);
            Test->TestTrue(TEXT("spawn transform applied"), Actor->GetActorLocation().Equals(FVector(125, 250, 375)) &&
                                                                Actor->GetActorRotation().Equals(FRotator(0, 30, 0)));
            auto* Life = FindFProperty<FObjectProperty>(Actor->GetClass(), TEXT("Life"));
            S->Component.Reset(Life ? Cast<UDasScriptComponent>(Life->GetObjectPropertyValue_InContainer(Actor))
                                    : nullptr);
            Test->TestTrue(TEXT("SCS component BeginPlay during actor spawn"),
                                SpawnValue(S->Component.Get(), TEXT("Begins")) == 1);
            FString InitReport;
            Test->TestTrue(TEXT("Initialize hierarchy and once guards"),
                                RunDasInitializerProbe(Actor, S->Component.Get(), InitReport));
            UE_LOG(LogTemp, Display, TEXT("%s"), *InitReport);
            FString ClassReport;
            Test->TestTrue(TEXT("Actor class methods"), RunDasClassMethodsProbe(Actor, ClassReport));
            Test->TestTrue(TEXT("Component class methods"), RunDasClassMethodsProbe(S->Component.Get(), ClassReport));
            UE_LOG(LogTemp, Display, TEXT("%s"), *ClassReport);
            auto Classes = Call(TEXT("ClassMethods"));
            Test->TestTrue(TEXT("cross-context class methods"), Classes.bSucceeded && Classes.BoolValue);
            if (!Classes.bSucceeded)
                Test->AddError(Classes.Error);
            auto BadClass = Call(TEXT("WrongClassMethod"));
            Test->TestTrue(TEXT("cross-context class base rejected"), !BadClass.bSucceeded);
            auto ForgedClass = Call(TEXT("ForgedClassMethod"));
            Test->TestTrue(TEXT("forged typed class base rejected"), !ForgedClass.bSucceeded);
            auto Extra = Call(TEXT("SpawnExtra"));
            Test->TestTrue(TEXT("spawn second query match"), Extra.bSucceeded && Extra.BoolValue);
            auto Queries = Call(TEXT("QueryMatches"));
            Test->TestTrue(TEXT("typed Actor/component first/all and foreign-world exclusion"),
                                Queries.bSucceeded && Queries.BoolValue);
            if (!Queries.bSucceeded)
                Test->AddError(Queries.Error);
            Phase = 1;
            Started = FPlatformTime::Seconds();
            return false;
        }
        auto* Actor = S->Spawned.Get();
        auto* Component = S->Component.Get();
        if ((SpawnValue(Actor, TEXT("Ticks")) < 2 || SpawnValue(Component, TEXT("Ticks")) < 2) &&
                        FPlatformTime::Seconds() - Started < 15)
            return false;
        Test->TestTrue(TEXT("actor and component tick"),
                            SpawnValue(Actor, TEXT("Ticks")) >= 2 && SpawnValue(Component, TEXT("Ticks")) >= 2);
        Test->TestTrue(TEXT("BeginPlay once"),
                            SpawnValue(Actor, TEXT("Begins")) == 1 && SpawnValue(Component, TEXT("Begins")) == 1);
        auto Removed = Call(TEXT("Remove"));
        Test->TestTrue(TEXT("owned actor destroy invalidates handle"), Removed.bSucceeded && Removed.BoolValue);
        if (!Removed.bSucceeded)
            Test->AddError(Removed.Error);
        Test->TestTrue(TEXT("actor and component EndPlay once"),
                            SpawnValue(Actor, TEXT("Ends")) == 1 && SpawnValue(Component, TEXT("Ends")) == 1);
        Test->TestTrue(TEXT("no lifecycle errors"),
                            Actor->LastDasError.IsEmpty() && Component && Component->LastDasError.IsEmpty());
        S->Component.Reset();
        S->Spawned.Reset();
        GEditor->RequestEndPlayMap();
        Phase = 2;
        Started = FPlatformTime::Seconds();
        return false;
    }
};
}
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FDasActorSpawnPIE, "DasForUE.Actors.SpawnPIE",
                                 EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FDasActorSpawnPIE::RunTest(const FString&)
{
    auto S = MakeShared<FSpawnState>();
    FString Error;
    UBlueprint* Target = nullptr;
    UBlueprint* Driver = nullptr;
    const FString Root = TEXT("/Game/DasSpawnPIE/")+ FGuid::NewGuid().ToString(EGuidFormats::Digits);
    if (!FDasDSLService::Apply(TEXT("dsl_spawn_target.das"),
                                    Root + TEXT("/BP_Target"), TEXT("spawntarget.SpawnTarget"), Target, Error) ||
                                                !FDasDSLService::Apply(TEXT("dsl_spawn_driver.das"),
                                                                            Root + TEXT("/BP_Driver"),
                                                                                        TEXT("spawndriver.SpawnDriver"),
                                                                                             Driver, Error))
    {
        AddError(Error);
        return false;
    }
    S->Target.Reset(Target);
    S->Driver.Reset(Driver);
    auto* Child = FKismetEditorUtilities::CreateBlueprint(
        Target->GeneratedClass, CreatePackage(*(Root + TEXT("/BP_Child"))),
                                              TEXT("BP_Child"), BPTYPE_Normal, UBlueprint::StaticClass(),
                                                   UBlueprintGeneratedClass::StaticClass());
    if (!Child)
    {
        AddError(TEXT("child Blueprint creation failed"));
        return false;
    }
    FKismetEditorUtilities::CompileBlueprint(Child);
    S->Child.Reset(Child);
    auto* W = FAutomationEditorCommonUtils::CreateNewMap();
    FActorSpawnParameters P;
    P.Name = TEXT("SpawnDriver");
    auto* Actor = W->SpawnActor<ADasProbeActor>(Driver->GeneratedClass, FTransform::Identity, P);
    auto* Field = FindFProperty<FClassProperty>(Driver->GeneratedClass, TEXT("SpawnClass"));
    if (!Actor || !Field)
    {
        AddError(TEXT("driver setup failed"));
        return false;
    }
    Field->SetObjectPropertyValue_InContainer(Actor, Child->GeneratedClass);
    S->ForeignWorld.Reset(UWorld::CreateWorld(EWorldType::Game, false));
    S->ForeignActor.Reset(S->ForeignWorld->SpawnActor<AActor>(Child->GeneratedClass));
    if (!TestTrue(TEXT("foreign world contains same authored type"), S->ForeignActor.IsValid()))
        return false;
    ADD_LATENT_AUTOMATION_COMMAND(FStartPIECommand(false));
    FAutomationTestFramework::Get().EnqueueLatentCommand(MakeShared<FSpawnCommand>(S, this));
    return true;
}
#endif
