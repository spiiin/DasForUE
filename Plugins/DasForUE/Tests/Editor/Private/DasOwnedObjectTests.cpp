#include "DasRuntime.h"
#include "DasDSLService.h"
#include "DasFoundationManifest.h"
#include "DasAuthoring.h"
#include "DasObject.h"
#include "DasActor.h"
#include "Engine/Blueprint.h"
#include "Engine/World.h"
#include "Engine/Engine.h"
#include "GameFramework/WorldSettings.h"
#include "Kismet2/KismetEditorUtilities.h"
#include "Misc/AutomationTest.h"
#include "Misc/ScopeExit.h"
#include "Misc/Paths.h"
#include "Misc/FileHelper.h"
#include "UObject/StrongObjectPtr.h"
#include "UObject/StructOnScope.h"
#include "UObject/UnrealType.h"
#include "UObject/GarbageCollection.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FDasOwnedObjectTest, "DasForUE.Runtime.OwnedObjects",
                                 EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FDasOwnedObjectTest::RunTest(const FString&)
{
    auto& Runtime = FModuleManager::LoadModuleChecked<FDasRuntimeModule>(TEXT("DasRuntime"));
    const FString Root = TEXT("/Game/Tests/OwnedObjects/") + FGuid::NewGuid().ToString(EGuidFormats::Digits);
    FString Error;
    auto Apply = [&](const TCHAR* File, const TCHAR* Name, const TCHAR* Id) -> UBlueprint*
    {
        UBlueprint* BP = nullptr;
        if (!FDasDSLService::Apply(File, Root / Name, Id, BP, Error))
            AddError(Error);
        return BP;
    };
    TStrongObjectPtr<UBlueprint> Action(
        Apply(TEXT("owned_object_action.das"), TEXT("Action"), TEXT("owned_object_action.OwnedAction")));
    if (!Action)
        return false;
    TStrongObjectPtr<UBlueprint> Holder(
        Apply(TEXT("owned_object_owner.das"), TEXT("Owner"), TEXT("owned_object_owner.OwnedActionOwner")));
    if (!Holder)
        return false;
    auto* TemplateProperty = FindFProperty<FObjectProperty>(Holder->GeneratedClass, TEXT("ActionTemplate"));
    if (!TestNotNull(TEXT("Inline template property"), TemplateProperty))
        return false;
    TestTrue(TEXT("Native instanced flags"), TemplateProperty->HasAllPropertyFlags(
                                                 CPF_InstancedReference | CPF_ExportObject | CPF_PersistentInstance));
    FDasAuthoringModule Wire;
    if (!Runtime.ReadAuthoringScript(Runtime.ScriptPath(TEXT("owned_object_owner.das")), Wire, Error))
    {
        AddError(Error);
        return false;
    }
    FString Json;
    TestTrue(TEXT("Serialize instanced descriptor"), Wire.ToJson(Json, Error));
    FDasAuthoringModule RoundTrip;
    TestTrue(TEXT("Read instanced descriptor"), FDasAuthoringModule::FromJson(Json, RoundTrip, Error));
    TestTrue(TEXT("Instanced survives JSON"),
                  RoundTrip.Classes.Num() &&
                      RoundTrip.Classes[0].Properties.ContainsByPredicate(
                          [](const auto& P) { return P.Name == TEXT("ActionTemplate") && P.bInstanced; }));
    auto Invalid = RoundTrip;
    for (auto& Property : Invalid.Classes[0].Properties)
        if (Property.bInstanced)
            Property.Type.Kind = TEXT("int");
    TestFalse(TEXT("Instanced on scalar is rejected"), Invalid.Validate(Error));
    Error.Reset();
    auto* Cost = FindFProperty<FIntProperty>(Action->GeneratedClass, TEXT("Cost"));
    auto* Initializations = FindFProperty<FIntProperty>(Action->GeneratedClass, TEXT("Initializations"));
    auto* OwnerProperty = FindFProperty<FObjectProperty>(Action->GeneratedClass, TEXT("Owner"));
    auto* CDO = Holder->GeneratedClass->GetDefaultObject();
    auto* Template = NewObject<UDasObject>(CDO, Action->GeneratedClass,
                                           TEXT("ConfiguredAction"), RF_ArchetypeObject | RF_Transactional);
    Cost->SetPropertyValue_InContainer(Template, 37);
    TemplateProperty->SetObjectPropertyValue_InContainer(CDO, Template);
    // Recompile must retain the configured subobject and its property flags.
    FKismetEditorUtilities::CompileBlueprint(Holder.Get());
    TestTrue(TEXT("Blueprint recompile succeeds"), Holder->Status != BS_Error);
    TemplateProperty = FindFProperty<FObjectProperty>(Holder->GeneratedClass, TEXT("ActionTemplate"));
    Template = Cast<UDasObject>(
        TemplateProperty->GetObjectPropertyValue_InContainer(Holder->GeneratedClass->GetDefaultObject()));
    if (!TestNotNull(TEXT("Template retained on recompile"), Template))
        return false;
    TestEqual(TEXT("Edited template value retained"), Cost->GetPropertyValue_InContainer(Template), 37);
    if (!FDasDSLService::Reimport(Holder.Get(), Error))
    {
        AddError(Error);
        return false;
    }
    TemplateProperty = FindFProperty<FObjectProperty>(Holder->GeneratedClass, TEXT("ActionTemplate"));
    Template = Cast<UDasObject>(
        TemplateProperty->GetObjectPropertyValue_InContainer(Holder->GeneratedClass->GetDefaultObject()));
    if (!TestNotNull(TEXT("Template retained on automatic reimport"), Template))
        return false;
    TestEqual(TEXT("Configured cost retained on reimport"), Cost->GetPropertyValue_InContainer(Template), 37);
    TStrongObjectPtr<UWorld> World(UWorld::CreateWorld(EWorldType::Game, false));
    GEngine->CreateNewWorldContext(EWorldType::Game).SetCurrentWorld(World.Get());
    ON_SCOPE_EXIT
    {
        if (World->HasBegunPlay())
            World->EndPlay(EEndPlayReason::Quit);
        GEngine->DestroyWorldContext(World.Get());
        World->DestroyWorld(false);
    };
    if (!Runtime.PreparePrograms(World.Get(), {TEXT("owned_object_action.das"), TEXT("owned_object_owner.das")}, Error))
    {
        AddError(Error);
        return false;
    }
    World->InitializeActorsForPlay(FURL());
    World->GetWorldSettings()->NotifyBeginPlay();
    auto* First = World->SpawnActor<ADasActor>(Holder->GeneratedClass);
    auto* Second = World->SpawnActor<ADasActor>(Holder->GeneratedClass);
    if (!First || !Second)
        return false;
    auto* FirstTemplate = TemplateProperty->GetObjectPropertyValue_InContainer(First);
    auto* SecondTemplate = TemplateProperty->GetObjectPropertyValue_InContainer(Second);
    if (!TestNotNull(TEXT("Owner receives inline template"), FirstTemplate) || !SecondTemplate)
        return false;
    TestTrue(TEXT("Separate template per owner"), FirstTemplate != SecondTemplate && FirstTemplate != Template);
    TestEqual(TEXT("Instanced template outer"), FirstTemplate->GetOuter(), static_cast<UObject*>(First));
    auto Invoke = [&](ADasActor* Owner, const TCHAR* Name, UClass* Kind) -> bool
    {
        auto* F = Owner->FindFunction(Name);
        if (!TestNotNull(TEXT("Generated function"), F))
            return false;
        FStructOnScope Frame(F);
        if (auto* Input = FindFProperty<FClassProperty>(F, TEXT("kind")))
            Input->SetObjectPropertyValue_InContainer(Frame.GetStructMemory(), Kind);
        Owner->ProcessEvent(F, Frame.GetStructMemory());
        if (!TestTrue(TEXT("No DAS runtime error: ") + Owner->LastDasError, Owner->LastDasError.IsEmpty()))
            return false;
        return !F->GetReturnProperty() || CastFieldChecked<FBoolProperty>(F->GetReturnProperty())
                                              ->GetPropertyValue_InContainer(Frame.GetStructMemory());
    };
    if (!TestTrue(TEXT("Create from template and explicit Initialize"),
                       Invoke(First, TEXT("Create"), Action->GeneratedClass)))
        return false;
    if (!TestTrue(TEXT("Create from class defaults"), Invoke(Second, TEXT("CreateDefault"), Action->GeneratedClass)))
        return false;
    auto* Actions = FindFProperty<FArrayProperty>(Holder->GeneratedClass, TEXT("Actions"));
    auto ActionAt = [&](ADasActor* O) -> UObject*
    {
        FScriptArrayHelper Values(Actions, Actions->ContainerPtrToValuePtr<void>(O));
        return Values.Num()
                   ? CastFieldChecked<FObjectProperty>(Actions->Inner)->GetObjectPropertyValue(Values.GetRawPtr(0))
                   : nullptr;
    };
    auto* Created = ActionAt(First);
    auto* DefaultCreated = ActionAt(Second);
    if (!Created || !DefaultCreated)
        return false;
    TestEqual(TEXT("Copied configured cost"), Cost->GetPropertyValue_InContainer(Created), 37);
    TestEqual(TEXT("Default cost"), Cost->GetPropertyValue_InContainer(DefaultCreated), 10);
    TestEqual(TEXT("Initialize called once"), Initializations->GetPropertyValue_InContainer(Created), 1);
    TestEqual(TEXT("Explicit owner assigned"), OwnerProperty->GetObjectPropertyValue_InContainer(Created),
                   static_cast<UObject*>(First));
    TestEqual(TEXT("Factory Outer"), Created->GetOuter(), static_cast<UObject*>(First));
    TestEqual(TEXT("Source template not initialized"), Initializations->GetPropertyValue_InContainer(FirstTemplate), 0);
    TWeakObjectPtr<UObject> Weak(Created);
    CollectGarbage(RF_NoFlags);
    TestTrue(TEXT("Reflected array retains object through GC"), Weak.IsValid());
    const int32 Contexts = Runtime.PersistentContextCount(World.Get());
    TestTrue(TEXT("Release strong references"), Invoke(First, TEXT("Release"), nullptr));
    CollectGarbage(RF_NoFlags);
    TestFalse(TEXT("Outer alone does not retain created object"), Weak.IsValid());
    TestTrue(TEXT("Collected object context released"), Runtime.PersistentContextCount(World.Get()) < Contexts);
    const FString Repo = FPaths::ConvertRelativePathToFull(FPaths::ProjectDir() / TEXT("../.."));
    FFileHelper::SaveStringToFile(DasFoundationExternManifest(), *(Repo / TEXT("logs/owned-objects-externs.txt")),
                                                                   FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM);
    return true;
}
