#include "DasDSLService.h"
#include "DasProbeActor.h"
#include "DasScriptComponent.h"
#include "DasRuntime.h"
#include "Editor.h"
#include "Engine/Blueprint.h"
#include "Engine/BlueprintGeneratedClass.h"
#include "EngineUtils.h"
#include "UObject/StrongObjectPtr.h"
#include "UObject/UnrealType.h"
#include "UObject/SavePackage.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "Misc/FileHelper.h"
#include "Misc/PackageName.h"
#include "Misc/Paths.h"
#if WITH_DEV_AUTOMATION_TESTS
#include "Misc/AutomationTest.h"
#include "Tests/AutomationEditorCommon.h"
namespace
{
int32 LifeValue(UObject* O, const TCHAR* Name)
{
    auto* P = O ? FindFProperty<FIntProperty>(O->GetClass(), Name) : nullptr;
    return P ? P->GetPropertyValue_InContainer(O) : MIN_int32;
}
struct FLifeState
{
    TStrongObjectPtr<UBlueprint> Owner, Component;
    TStrongObjectPtr<UDasScriptComponent> Instance;
    int32 EndNotifications = 0;
};
class FLifeCommand : public IAutomationLatentCommand
{
    TSharedRef<FLifeState> State;
    FAutomationTestBase* Test;
    int Phase = 0;
    double Started = FPlatformTime::Seconds();

public:
    FLifeCommand(TSharedRef<FLifeState> S, FAutomationTestBase* T) : State(S), Test(T) {}
    bool Update() override
    {
        auto* W = GEditor->PlayWorld.Get();
        auto& R = FModuleManager::LoadModuleChecked<FDasRuntimeModule>(TEXT("DasRuntime"));
        if (Phase == 2)
        {
            if (W && FPlatformTime::Seconds() - Started < 30)
                return false;
            Test->TestTrue(TEXT("PIE ended and sessions released"), !W && R.PersistentContextCount() == 0);
            return true;
        }
        ADasProbeActor* Actor = nullptr;
        if (W)
            for (TActorIterator<ADasProbeActor> I(W); I; ++I)
                if (I->GetFName() == TEXT("DynamicOwner"))
                    Actor = *I;
        if (!Actor || !W->HasBegunPlay())
        {
            if (FPlatformTime::Seconds() - Started < 45)
                return false;
            Test->AddError(TEXT("PIE startup timeout"));
            GEditor->RequestEndPlayMap();
            Phase = 2;
            Started = FPlatformTime::Seconds();
            return false;
        }
        if (Phase == 0)
        {
            const int32 Before = Actor->GetComponents().Num();
            for (const auto* Entry :
                 {TEXT("WrongBase"), TEXT("WrongIdentity"), TEXT("MissingClass"), TEXT("WrongCreate"),
                                                                                       TEXT("WrongWrite")})
            {
                auto Rejected =
                    R.RunNamedBoolPersistent(R.ScriptPath(TEXT("dsl_dynamic_owner.das")),
                                                          FString(TEXT("ue_DynamicOwner_")) + Entry, false, Actor);
                Test->TestTrue(FString(TEXT("class validation rejects ")) + Entry,
                                       !Rejected.bSucceeded && !Rejected.Error.IsEmpty());
            }
            Test->TestTrue(TEXT("class rejection does not create components"), Actor->GetComponents().Num() == Before);
            auto Null = R.RunNamedBoolPersistent(R.ScriptPath(TEXT("dsl_dynamic_owner.das")),
                                                              TEXT("ue_DynamicOwner_NullClass"), false, Actor);
            Test->TestTrue(TEXT("empty class handle is invalid"), Null.bSucceeded && Null.BoolValue);

            auto Roundtrip = R.RunNamedBoolPersistent(
                R.ScriptPath(TEXT("dsl_dynamic_owner.das")), TEXT("ue_DynamicOwner_ClassRoundtrip"), false, Actor);
            Test->TestTrue(TEXT("class property write clear and restore"), Roundtrip.bSucceeded && Roundtrip.BoolValue);
            auto Result = R.RunNamedBoolPersistent(R.ScriptPath(TEXT("dsl_dynamic_owner.das")),
                                                                TEXT("ue_DynamicOwner_Create"), false, Actor);
            Test->TestTrue(TEXT("nested BeginPlay completed before create returned"),
                                Result.bSucceeded && Result.BoolValue);
            if (!Result.bSucceeded)
                Test->AddError(Result.Error);
            for (auto* C : Actor->GetComponents())
                if (C && C->GetFName() == TEXT("DynamicLife"))
                    State->Instance.Reset(Cast<UDasScriptComponent>(C));
            if (!State->Instance.IsValid())
            {
                Test->AddError(TEXT("dynamic component missing"));
                GEditor->RequestEndPlayMap();
                Phase = 2;
                Started = FPlatformTime::Seconds();
                return false;
            }
            State->Instance->OnScriptEnded.AddLambda([S = State](UDasScriptComponent*, EEndPlayReason::Type)
                                                     { ++S->EndNotifications; });
            Phase = 1;
            Started = FPlatformTime::Seconds();
            return false;
        }
        auto* C = State->Instance.Get();
        if (LifeValue(C, TEXT("Ticks")) < 2 && FPlatformTime::Seconds() - Started < 15)
            return false;
        Test->TestTrue(TEXT("dynamic component ticks"), LifeValue(C, TEXT("Ticks")) >= 2);
        Test->TestTrue(TEXT("BeginPlay once"), LifeValue(C, TEXT("Begins")) == 1);
        const int32 Before = LifeValue(C, TEXT("Ticks"));
        auto Result = R.RunNamedBoolPersistent(R.ScriptPath(TEXT("dsl_dynamic_owner.das")),
                                                            TEXT("ue_DynamicOwner_Remove"), false, Actor);
        Test->TestTrue(TEXT("script-owned destruction"), Result.bSucceeded && Result.BoolValue);
        if (!Result.bSucceeded)
            Test->AddError(Result.Error);
        Test->TestTrue(TEXT("EndPlay once during destruction"),
                            LifeValue(C, TEXT("Ends")) == 1 && State->EndNotifications == 1);
        Test->TestTrue(TEXT("destruction disables ticking and unregisters"), !C->IsRegistered() &&
                                                                                 !C->IsComponentTickEnabled() &&
                                                                                 LifeValue(C, TEXT("Ticks")) == Before);
        Test->TestTrue(TEXT("component lifecycle has no error"), C->LastDasError.IsEmpty());
        C->OnScriptEnded.Clear();
        State->Instance.Reset();
        GEditor->RequestEndPlayMap();
        Phase = 2;
        Started = FPlatformTime::Seconds();
        return false;
    }
};
}
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FDasDynamicComponentPIE, "DasForUE.Components.DynamicPIE",
                                 EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FDasDynamicComponentPIE::RunTest(const FString&)
{
    auto S = MakeShared<FLifeState>();
    FString Error;
    UBlueprint* Component = nullptr;
    UBlueprint* Owner = nullptr;
    const FString Root = TEXT("/Game/DasClassPropertyTest/")+ FGuid::NewGuid().ToString(EGuidFormats::Digits);
    if (!FDasDSLService::Apply(TEXT("dsl_dynamic_lifecycle.das"),
                                    Root + TEXT("/BP_Component"), TEXT("dynamiclife.DynamicLife"), Component, Error) ||
                                                !FDasDSLService::Apply(
                                                    TEXT("dsl_dynamic_owner.das"),
                                                         Root + TEXT("/BP_Owner"), TEXT("dynamicowner.DynamicOwner"),
                                                                                        Owner, Error))
    {
        AddError(Error);
        return false;
    }
    S->Component.Reset(Component);
    S->Owner.Reset(Owner);
    auto* W = FAutomationEditorCommonUtils::CreateNewMap();
    FActorSpawnParameters P;
    P.Name = TEXT("DynamicOwner");
    auto* Actor = W->SpawnActor<ADasProbeActor>(Owner->GeneratedClass, FTransform::Identity, P);
    auto* Field = FindFProperty<FClassProperty>(Owner->GeneratedClass, TEXT("ComponentClass"));
    if (!Actor || !Field)
    {
        AddError(TEXT("owner setup failed"));
        return false;
    }
    TestTrue(TEXT("class picker base and editable property"),
                  Field->MetaClass == UDasScriptComponent::StaticClass() &&
                      Field->HasAllPropertyFlags(CPF_Edit | CPF_BlueprintVisible));
    Field->SetObjectPropertyValue_InContainer(Owner->GeneratedClass->GetDefaultObject(), Component->GeneratedClass);
    Field->SetObjectPropertyValue_InContainer(Actor, Component->GeneratedClass);
    UBlueprint* Reimported = nullptr;
    TestTrue(TEXT("reimport preserves selected class default"),
                  FDasDSLService::Apply(
                      TEXT("dsl_dynamic_owner.das"),
                           Root + TEXT("/BP_Owner"), TEXT("dynamicowner.DynamicOwner"), Reimported, Error) &&
                                       Reimported == Owner &&
                                       Field->GetObjectPropertyValue_InContainer(
                                           Owner->GeneratedClass->GetDefaultObject()) == Component->GeneratedClass);
    for (auto* Asset : {Component, Owner})
    {
        FSavePackageArgs Args;
        Args.TopLevelFlags = RF_Public | RF_Standalone;
        Args.SaveFlags = SAVE_NoError;
        const FString File = FPackageName::LongPackageNameToFilename(Asset->GetOutermost()->GetName(),
                                                                     FPackageName::GetAssetPackageExtension());
        if (!UPackage::SavePackage(Asset->GetOutermost(), Asset, *File, Args))
        {
            AddError(TEXT("test class asset save failed"));
            return false;
        }
    }
    auto& Registry = FModuleManager::LoadModuleChecked<FAssetRegistryModule>(TEXT("AssetRegistry")).Get();
    Registry.ScanFilesSynchronous({FPackageName::LongPackageNameToFilename(Owner->GetOutermost()->GetName(),
                                                                           FPackageName::GetAssetPackageExtension())},
                                  true);
    TArray<FName> Dependencies;
    Registry.GetDependencies(Owner->GetOutermost()->GetFName(), Dependencies);
    if (!TestTrue(TEXT("saved owner package records selected component class dependency"),
                       Dependencies.Contains(Component->GetOutermost()->GetFName())))
        return false;
    if (!FFileHelper::SaveStringToFile(Root,
                                       *FPaths::Combine(FPaths::ProjectSavedDir(), TEXT("DasClassPropertyReload.txt"))))
    {
        AddError(TEXT("reload manifest save failed"));
        return false;
    }
    ADD_LATENT_AUTOMATION_COMMAND(FStartPIECommand(false));
    FAutomationTestFramework::Get().EnqueueLatentCommand(MakeShared<FLifeCommand>(S, this));
    return true;
}
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FDasClassPropertyReload, "DasForUE.Components.ClassReload",
                                 EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FDasClassPropertyReload::RunTest(const FString&)
{
    FString Root;
    if (!FFileHelper::LoadFileToString(
            Root, *FPaths::Combine(FPaths::ProjectSavedDir(), TEXT("DasClassPropertyReload.txt"))) ||
            !Root.StartsWith(TEXT("/Game/DasClassPropertyTest/")))
    {
        AddError(TEXT("missing test reload manifest"));
        return false;
    }
    auto S = MakeShared<FLifeState>();
    auto* Owner = LoadObject<UBlueprint>(nullptr, *(Root + TEXT("/BP_Owner.BP_Owner")));
    auto* Component = LoadObject<UBlueprint>(nullptr, *(Root + TEXT("/BP_Component.BP_Component")));
    if (!Owner || !Component || !Owner->GeneratedClass || !Component->GeneratedClass)
    {
        AddError(TEXT("saved test assets failed to load"));
        return false;
    }
    S->Owner.Reset(Owner);
    S->Component.Reset(Component);
    auto* Field = FindFProperty<FClassProperty>(Owner->GeneratedClass, TEXT("ComponentClass"));
    if (!TestTrue(TEXT("class default survives fresh process load"),
                       Field && Field->MetaClass == UDasScriptComponent::StaticClass() &&
                           Field->GetObjectPropertyValue_InContainer(Owner->GeneratedClass->GetDefaultObject()) ==
                               Component->GeneratedClass))
        return false;
    auto* W = FAutomationEditorCommonUtils::CreateNewMap();
    FActorSpawnParameters P;
    P.Name = TEXT("DynamicOwner");
    auto* Actor = W->SpawnActor<ADasProbeActor>(Owner->GeneratedClass, FTransform::Identity, P);
    if (!TestTrue(TEXT("spawn copies saved class default"),
                       Actor && Field->GetObjectPropertyValue_InContainer(Actor) == Component->GeneratedClass))
        return false;
    ADD_LATENT_AUTOMATION_COMMAND(FStartPIECommand(false));
    FAutomationTestFramework::Get().EnqueueLatentCommand(MakeShared<FLifeCommand>(S, this));
    return true;
}

#endif
