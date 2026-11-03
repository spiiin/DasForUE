#include "DasDSLService.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "Misc/PackageName.h"
#include "UObject/MetaData.h"

void DasFailStructuralForTesting(int32 Stage);
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
    TArray<FString> Scripts, FixtureFiles;
    ON_SCOPE_EXIT
    {
        for (const auto& Script : Scripts)
            IFileManager::Get().Delete(*Runtime.ScriptPath(Script), false, true);
        for (const auto& File : FixtureFiles)
            IFileManager::Get().Delete(*File, false, true);
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
        if (!Runtime.PrepareEditorPrograms({Script}, Error))
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
    TestTrue(TEXT("Prepare programs for isolated game world"), Runtime.PreparePrograms(World.Get(), Scripts, Error));
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
    // Plain ScriptObject structural updates must preserve the cooked contract
    // transactionally, including a failed apply after Blueprint compilation.
    UBlueprint* Mutable = nullptr;
    const FString MutableSource =
        Prefix +
        TEXT("[ue_class(module_id=\"genericparents\",parent=\"/Script/DasRuntime.DasObject\")]\nclass Mutable {\n") +
             Body + TEXT("}\n");
    if (!Apply(TEXT("Mutable"), MutableSource, Mutable))
        return false;
    const FString MutableScript = Scripts.Last();
    auto* Contract = DasGenericClassData(Mutable);
    if (!TestNotNull(TEXT("ScriptObject cooked contract"), Contract))
        return false;
    TStrongObjectPtr<UObject> LiveMutable(NewObject<UObject>(First, Mutable->GeneratedClass));
    FindFProperty<FIntProperty>(LiveMutable->GetClass(),
                                TEXT("Score"))->SetPropertyValue_InContainer(LiveMutable.Get(), 41);
    const auto ReplacementHandle = FCoreUObjectDelegates::OnObjectsReplaced.AddLambda(
        [&](const TMap<UObject*, UObject*>& Replacements)
        {
            if (auto* Replacement = Replacements.Find(LiveMutable.Get()))
                LiveMutable.Reset(*Replacement);
        });
    ON_SCOPE_EXIT
    {
        FCoreUObjectDelegates::OnObjectsReplaced.Remove(ReplacementHandle);
    };
    const auto PriorMethods = Contract->MethodScripts;
    const FString PriorModel = Mutable->GetOutermost()->GetMetaData().GetValue(Mutable, TEXT("DasDSLModel"));
    FString NewSource = MutableSource;
    NewSource.ReplaceInline(
        TEXT("class Mutable {\n"),
             TEXT("class Mutable {\n @ue_property=(EditAnywhere,BlueprintReadWrite)\n Bonus:int=11\n")
                 TEXT(" [ue_function(BlueprintCallable)]\n def Extra(input:int):int { return self.Bonus+input }\n"));
    TestTrue(TEXT("Write ScriptObject structural update"),
                  FFileHelper::SaveStringToFile(NewSource, *Runtime.ScriptPath(MutableScript),
                                                FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM));
    DasFailStructuralForTesting(2);
    const bool FailedApply = FDasDSLService::Reimport(Mutable, Error, EDasDefaultMigrationPolicy::Refuse);
    DasFailStructuralForTesting(0);
    TestFalse(TEXT("Injected ScriptObject migration rolls back"), FailedApply);
    TestTrue(TEXT("Rollback restores cooked method map"),
                  Contract->MethodScripts.OrderIndependentCompareEqual(PriorMethods));
    TestEqual(TEXT("Rollback restores declaration"),
                   FString(Mutable->GetOutermost()->GetMetaData().GetValue(Mutable, TEXT("DasDSLModel"))), PriorModel);
    TestTrue(TEXT("Rollback contract owns restored generated class"), Contract->OwnerClass == Mutable->GeneratedClass);
    TestEqual(TEXT("Rollback retains live object value"),
                   FindFProperty<FIntProperty>(
                       LiveMutable->GetClass(), TEXT("Score"))->GetPropertyValue_InContainer(LiveMutable.Get()), 41);
    TestNull(TEXT("Rollback removes new field"), Mutable->GeneratedClass->FindPropertyByName(TEXT("Bonus")));
    TestNull(TEXT("Rollback removes new method"), Mutable->GeneratedClass->FindFunctionByName(TEXT("Extra")));
    Error.Reset();
    if (!TestTrue(TEXT("ScriptObject structural reimport succeeds"),
                       FDasDSLService::Reimport(Mutable, Error, EDasDefaultMigrationPolicy::Refuse)))
    {
        AddError(Error);
        return false;
    }
    TestTrue(TEXT("Updated contract owns generated class"), Contract->OwnerClass == Mutable->GeneratedClass);
    TestEqual(TEXT("Updated cooked method map includes new entry"), Contract->MethodScripts.Num(),
                   PriorMethods.Num() + 1);
    TestTrue(TEXT("Prepare migrated ScriptObject"), Runtime.PreparePrograms(World.Get(), {MutableScript}, Error));
    TStrongObjectPtr<UObject> MutableObject(NewObject<UObject>(First, Mutable->GeneratedClass));
    TestEqual(TEXT("New method executes via migrated cooked contract"), Call(MutableObject.Get(), TEXT("Extra"), 3),
                                                                             14);
    TestEqual(TEXT("Existing method still executes"), Call(MutableObject.Get(), TEXT("Add"), 2), 9);
    TestEqual(TEXT("Structural update retains live object state"), Call(LiveMutable.Get(), TEXT("Add"), 1), 42);
    TestEqual(TEXT("New default reaches live object"), Call(LiveMutable.Get(), TEXT("Extra"), 1), 12);
    // A borrowed component reference acquires UE's implicit InstancedReference
    // flag because ActorComponent is DefaultToInstanced; it is still a valid
    // ordinary reflected DAS object field without an explicit Instanced marker.
    UBlueprint* ComponentReference = nullptr;
    if (!Apply(TEXT("ComponentReference"),
            Prefix +
                TEXT("require ue_object_types\n") +
                    TEXT("[ue_native_object(path=\"/Script/Engine.ActorComponent\")]\nstruct BorrowedComponentRef { handle:UObjectHandle }\n") +
                        TEXT("[ue_class(module_id=\"genericparents\",parent=\"ScriptObject\")]\nclass ComponentReference {\n") +
                            TEXT(" @ue_property=(BlueprintReadWrite)\n ComponentState:BorrowedComponentRef\n") +
                                TEXT(" [ue_function(BlueprintCallable)]\n def ReadComponent(input:int):int { if(ue_is_valid(self.ComponentState)) { return input }; return 0 }\n}\n"),
                                    ComponentReference))
        return false;
    auto* ComponentField =
        FindFProperty<FObjectPropertyBase>(ComponentReference->GeneratedClass, TEXT("ComponentState"));
    if (!TestNotNull(TEXT("Borrowed ActorComponent reference property"), ComponentField))
        return false;
    TestTrue(TEXT("UE adds implicit instanced flag for component class"),
                  ComponentField->HasAnyPropertyFlags(CPF_InstancedReference));
    TestTrue(TEXT("Prepare borrowed component reference program"),
                  Runtime.PreparePrograms(World.Get(), {Scripts.Last()}, Error));
    TStrongObjectPtr<UObject> ReferenceObject(NewObject<UObject>(First, ComponentReference->GeneratedClass));
    auto* BorrowedComponent = NewObject<USceneComponent>(First);
    ComponentField->SetObjectPropertyValue_InContainer(ReferenceObject.Get(), BorrowedComponent);
    TestEqual(TEXT("Implicitly instanced component reference passes owner validation"),
                   Call(ReferenceObject.Get(), TEXT("ReadComponent"), 42), 42);

    UBlueprint* DescriptorChild = nullptr;
    if (!Apply(TEXT("DescriptorChild"),
            Prefix +
                TEXT("[ue_class(module_id=\"genericparents\",parent=\"") + Mutable->GeneratedClass->GetPathName() +
                    TEXT("\")]\nclass DescriptorChild {\n [ue_function(BlueprintCallable)]\n def Ping(input:int):int { return input }\n}\n"),
                        DescriptorChild))
        return false;
    auto* ChildContract = DasGenericClassData(DescriptorChild);
    if (!TestNotNull(TEXT("Child cooked contract"), ChildContract))
        return false;
    ChildContract->OwnerClass = Mutable->GeneratedClass;
    TestTrue(TEXT("Reimport repairs a stale direct contract before identity preflight"),
                  FDasDSLService::Reimport(DescriptorChild, Error, EDasDefaultMigrationPolicy::Refuse));
    TestTrue(TEXT("Child contract is canonical after reimport"),
                  ChildContract->OwnerClass == DescriptorChild->GeneratedClass);
    // A child with no own methods has no bytecode graph to retain its contract.
    UBlueprint* ContractOnlyChild = nullptr;
    if (!Apply(TEXT("ContractOnlyChild"),
            Prefix + TEXT("require ") + FPaths::GetBaseFilename(MutableScript) +
                TEXT("\n[ue_class(module_id=\"genericparents\")]\nclass ContractOnlyChild : Mutable {}\n"),
                    ContractOnlyChild))
        return false;
    TStrongObjectPtr<UBlueprint> KeepContractChild(ContractOnlyChild);
    TWeakObjectPtr<UDasScriptClassData> EmptyContract = DasGenericClassData(ContractOnlyChild);
    TestNotNull(TEXT("Contract-only child has its own contract"), EmptyContract.Get());
    CollectGarbage(RF_NoFlags);
    if (!TestTrue(TEXT("CDO retains contract without method graphs across GC"), EmptyContract.IsValid()))
        return false;
    TestTrue(TEXT("Contract-only child lookup uses its own contract"),
                  UDasScriptClassData::Find(ContractOnlyChild->GeneratedClass) == EmptyContract.Get());
    TestEqual(TEXT("Parent identity is not overwritten by empty child"),
                   DasGenericClassData(Mutable)->AuthoredTypeId, FString(TEXT("genericparents.Mutable")));
    Error.Reset();
    if (!TestTrue(TEXT("Contract-only child reimports after GC"),
                       FDasDSLService::Reimport(ContractOnlyChild, Error, EDasDefaultMigrationPolicy::Refuse)))
    {
        AddError(Error);
        return false;
    }
    // A generic-object consumer has a public cooked-contract export beside its
    // Blueprint. Adding an Actor member must preserve that referring package.
    UBlueprint *MigrationActor = nullptr, *Consumer = nullptr;
    const FString ActorSource =
        Prefix + TEXT("[ue_class(module_id=\"genericparents\",parent=\"Actor\")]\nclass MigrationActor {\n") + Body +
                      TEXT("}\n");
    if (!Apply(TEXT("MigrationActor"), ActorSource, MigrationActor))
        return false;
    const FString ActorScript = Scripts.Last();
    const FString ConsumerSource =
        Prefix +
        TEXT("require ue_native_gameplay\n") +
            TEXT("[ue_class(module_id=\"genericparents\",parent=\"/Script/DasRuntime.DasObject\")]\nclass Consumer {\n") +
                TEXT(" @ue_property=(EditAnywhere,BlueprintReadWrite)\n Target:ActorClass\n defaults=ue_defaults() { self.Target=ue_class_default(\"") +
                    MigrationActor->GeneratedClass->GetPathName() +
                    TEXT("\") }\n [ue_function(BlueprintCallable)]\n def Ping(input:int):int { return input }\n}\n");
    if (!Apply(TEXT("Consumer"), ConsumerSource, Consumer))
        return false;
    auto* ConsumerContract = DasGenericClassData(Consumer);
    FAssetRegistryModule::AssetCreated(ConsumerContract);
    for (auto* Blueprint : {MigrationActor, Consumer})
    {
        const FString File = FPackageName::LongPackageNameToFilename(Blueprint->GetOutermost()->GetName(),
                                                                     FPackageName::GetAssetPackageExtension());
        IFileManager::Get().MakeDirectory(*FPaths::GetPath(File), true);
        FSavePackageArgs Save;
        Save.TopLevelFlags = RF_Public | RF_Standalone;
        if (!TestTrue(TEXT("Save structural referencer fixture"),
                           UPackage::SavePackage(Blueprint->GetOutermost(), Blueprint, *File, Save)))
            return false;
        FixtureFiles.Add(File);
    }
    auto& Registry = FModuleManager::LoadModuleChecked<FAssetRegistryModule>(TEXT("AssetRegistry")).Get();
    Registry.ScanModifiedAssetFiles(FixtureFiles);
    TArray<FAssetData> ConsumerExports;
    Registry.GetAssetsByPackageName(Consumer->GetOutermost()->GetFName(), ConsumerExports);
    TestEqual(TEXT("Consumer package has Blueprint and cooked-contract exports"), ConsumerExports.Num(), 2);
    FString ExtendedActor = ActorSource;
    ExtendedActor.ReplaceInline(
        TEXT("class MigrationActor {\n"),
             TEXT("class MigrationActor {\n @ue_property=(EditAnywhere,BlueprintReadWrite)\n Extra:int=13\n"));
    FFileHelper::SaveStringToFile(ExtendedActor, *Runtime.ScriptPath(ActorScript),
                                  FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM);
    if (!TestTrue(TEXT("Additive Actor migration includes independent generic referencer"),
                       FDasDSLService::Reimport(MigrationActor, Error, EDasDefaultMigrationPolicy::Refuse)))
    {
        AddError(Error);
        return false;
    }
    TestTrue(TEXT("Consumer class value follows Actor reinstancing"),
                  FindFProperty<FClassProperty>(
                      Consumer->GeneratedClass,
                      TEXT("Target"))->GetPropertyValue_InContainer(Consumer->GeneratedClass->GetDefaultObject()) ==
                      MigrationActor->GeneratedClass);
    TestTrue(TEXT("Consumer cooked contract remains canonical"),
                  ConsumerContract->OwnerClass == Consumer->GeneratedClass);
    // A new callback and comment-derived tooltips are additive even with independent consumers.
    FString CallbackActor = ExtendedActor;
    CallbackActor.ReplaceInline(TEXT("require ue_dsl\n"), TEXT("require ue_dsl\nrequire ue_native_gameplay\n"));
    CallbackActor.ReplaceInline(TEXT("class MigrationActor {\n"),
        TEXT("class MigrationActor {\n [ue_event(source=self, source_type=ActorRef, name=OnDestroyed)]\n def HandleDestroyed(actor:ActorRef):void {}\n"));
    CallbackActor.ReplaceInline(TEXT(" Score:int=7"), TEXT(" // Updated score tooltip\n Score:int=7"));
    FFileHelper::SaveStringToFile(CallbackActor, *Runtime.ScriptPath(ActorScript),
                                  FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM);
    Error.Reset();
    if (!TestTrue(TEXT("Additive callback and tooltip migration includes independent consumer"),
                       FDasDSLService::Reimport(MigrationActor, Error, EDasDefaultMigrationPolicy::Refuse)))
    {
        AddError(Error);
        return false;
    }
    const FString CallbackModel = MigrationActor->GetOutermost()->GetMetaData().GetValue(MigrationActor, TEXT("DasDSLModel"));
    UClass* CallbackClass = MigrationActor->GeneratedClass.Get();
    FFileHelper::SaveStringToFile(ExtendedActor, *Runtime.ScriptPath(ActorScript),
                                  FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM);
    Error.Reset();
    TestFalse(TEXT("Removing an existing callback still refuses independent consumers"),
                   FDasDSLService::Reimport(MigrationActor, Error, EDasDefaultMigrationPolicy::Refuse));
    TestTrue(TEXT("Rejected callback removal retains generated class"), MigrationActor->GeneratedClass.Get() == CallbackClass);
    TestEqual(TEXT("Rejected callback removal retains model"),
        FString(MigrationActor->GetOutermost()->GetMetaData().GetValue(MigrationActor, TEXT("DasDSLModel"))), CallbackModel);
    TestTrue(TEXT("Consumer contract survives callback migration"), ConsumerContract->OwnerClass == Consumer->GeneratedClass);
    FFileHelper::SaveStringToFile(CallbackActor, *Runtime.ScriptPath(ActorScript),
                                  FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM);
    Runtime.ReleaseObject(LiveMutable.Get());
    Runtime.ReleaseObject(MutableObject.Get());
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
    TestTrue(TEXT("Prepare updated generic body"), Runtime.PreparePrograms(World.Get(), {Scripts[0]}, Error));
    TestTrue(TEXT("Reimport retains exact native parent"), Mode->ParentClass == AGameModeBase::StaticClass());
    TestEqual(TEXT("Updated body preserves object values"), Call(First, TEXT("Add"), 1), 125);
    Runtime.ReleaseObject(First);
    // Verify a supported native host remains available alongside the generic path.
    TestTrue(
        TEXT("Existing Actor and PlayerController hosts retained"),
             DasNativeScriptHost(
                 TEXT("Actor"))->GetPathName() ==
                 TEXT("/Script/DasRuntime.DasActor") &&
                      DasNativeScriptHost(TEXT("/Script/Engine.PlayerController"))->IsChildOf<APlayerController>());
#if DAS_WITH_TEST_PROBES
    TestFalse(TEXT("Existing probe stays on its native host path"),
                   DasGenericScriptParent(TEXT("/Script/DasRuntime.DasProbeActor")));
#endif
    AddInfo(TEXT("DAS_GENERIC_PARENTS=PASS"));
    return true;
}
