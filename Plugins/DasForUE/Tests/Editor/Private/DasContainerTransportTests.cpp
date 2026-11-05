#include "DasRuntime.h"
#include "DasFoundationManifest.h"
#include "Misc/Paths.h"
#include "DasDSLService.h"
#include "DasAuthoring.h"
#include "DasContainerTypes.h"
#include "DasPropertyTypes.h"
#include "DasDefaultRoots.h"
#include "DasActor.h"
#include "Dom/JsonValue.h"
#include "Engine/Blueprint.h"
#include "Engine/BlueprintGeneratedClass.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "GameFramework/WorldSettings.h"
#include "Kismet2/KismetEditorUtilities.h"
#include "Misc/AutomationTest.h"
#include "Misc/FileHelper.h"
#include "Misc/ScopeExit.h"
#include "HAL/FileManager.h"
#include "UObject/GarbageCollection.h"
#include "UObject/Package.h"
#include "UObject/StrongObjectPtr.h"
#include "UObject/StructOnScope.h"
#include "UObject/UnrealType.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FDasContainerTransportTest, "DasForUE.Runtime.ContainerTransport",
                                 EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FDasContainerTransportTest::RunTest(const FString&)
{
    auto& Runtime = FModuleManager::LoadModuleChecked<FDasRuntimeModule>(TEXT("DasRuntime"));
    const FString Id = FGuid::NewGuid().ToString(EGuidFormats::Digits);
    const FString Module = TEXT("ct_") + Id;
    const FString BaseFile = Module + TEXT("_base.das"), RefsFile = Module + TEXT("_refs.das"),
                                                                                  File = Module + TEXT("_holder.das");
    const FString Root = TEXT("/Game/Tests/ContainerTransport/") + Id;
    FString Error;
    TStrongObjectPtr<UWorld> World;
    ON_SCOPE_EXIT
    {
        if (World)
        {
            if (World->HasBegunPlay())
                World->EndPlay(EEndPlayReason::Quit);
            GEngine->DestroyWorldContext(World.Get());
            World->DestroyWorld(false);
        }
        for (const auto& Name : {BaseFile, RefsFile, File})
            IFileManager::Get().Delete(*Runtime.ScriptPath(Name), false, true);
    };
    auto Write = [&](const FString& Name, const FString& Text)
    {
        return FFileHelper::SaveStringToFile(Text, *Runtime.ScriptPath(Name),
                                             FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM);
    };
    if (!Write(BaseFile, TEXT("options gen2\nrequire ue_dsl\n[ue_class(module_id=\"") + Module +
                              TEXT("\",Blueprintable)]\nclass Action : ScriptObject { [ue_function(BlueprintPure)] def Value():int { return 1 } }\n")))
        return false;
    UBlueprint* RawBase = nullptr;
    if (!FDasDSLService::Apply(BaseFile, Root + TEXT("/Action"), Module + TEXT(".Action"), RawBase, Error))
    {
        AddError(Error);
        return false;
    }
    TStrongObjectPtr<UBlueprint> Base(RawBase);
    TStrongObjectPtr<UBlueprint> Child(FKismetEditorUtilities::CreateBlueprint(
        Base->GeneratedClass, CreatePackage(*(Root + TEXT("/Child"))), TEXT("Child"), BPTYPE_Normal,
                                                                            UBlueprint::StaticClass(),
                                                                            UBlueprintGeneratedClass::StaticClass()));
    if (!TestNotNull(TEXT("Child Blueprint"), Child.Get()))
        return false;
    auto Leaf = [](const TCHAR* Kind, const FString& Path = FString())
    {
        FDasDeclaredType T;
        T.Kind = Kind;
        T.TypePath = Path;
        return T;
    };
    auto Array = [](const FDasDeclaredType& Value)
    {
        FDasDeclaredType T;
        T.Kind = TEXT("array");
        T.Element = MakeShared<FDasDeclaredType>(Value);
        return T;
    };
    auto Map = [](const FDasDeclaredType& Key, const FDasDeclaredType& Value)
    {
        FDasDeclaredType T;
        T.Kind = TEXT("map");
        T.Key = MakeShared<FDasDeclaredType>(Key);
        T.Value = MakeShared<FDasDeclaredType>(Value);
        return T;
    };
    const auto Classes = Leaf(TEXT("class"), Base->GeneratedClass->GetPathName());
    const auto Objects = Leaf(TEXT("object"), Base->GeneratedClass->GetPathName());
    // Every accepted combination must have an actual transport, with the exact
    // base class and storage shape, not just an optimistic capability flag.
    TArray<FDasDeclaredType> Shapes = {Array(Classes), Array(Objects)};
    for (const TCHAR* Key : {TEXT("int"), TEXT("string"), TEXT("name"), TEXT("byte")})
        for (const auto& Value : {Classes, Objects, Leaf(TEXT("double")), Leaf(TEXT("text"))})
            Shapes.Add(Map(Leaf(Key), Value));
    for (const auto& Shape : Shapes)
    {
        if (!TestTrue(TEXT("Accepted shape"), ValidateDasPropertyType(Shape, EDasPropertyUsage::Container, Error)))
        {
            AddError(Error);
            return false;
        }
        auto* Carrier = DasContainerCarrier(Shape);
        if (!TestNotNull(TEXT("Transport for accepted shape"), Carrier))
            return false;
        auto* Property = DasContainerProperty(Carrier->GetPathName());
        FDasDeclaredType Described;
        if (!TestTrue(TEXT("Describe transport"), DescribeDasProperty(Property, Described, Error)) ||
                      !TestEqual(TEXT("Physical shape preserved"), Described.StorageIdentity(),
                                      Shape.StorageIdentity()))
            return false;
        TestEqual(TEXT("Reuse identical transport"), DasContainerCarrier(Shape), Carrier);
        TestEqual(TEXT("Property-to-carrier roundtrip"), DasContainerCarrierForProperty(Property), Carrier);
    }
    for (const auto& Bad : {Array(Array(Classes)), Map(Classes, Leaf(TEXT("int"))), Map(Leaf(TEXT("float")), Classes)})
    {
        TestFalse(TEXT("Unsupported shape rejected by validation"),
                       ValidateDasPropertyType(Bad, EDasPropertyUsage::Container, Error));
        TestNull(TEXT("Unsupported shape has no transport"), DasContainerCarrier(Bad));
    }
    auto Weak = Objects;
    Weak.WeakReference = true;
    TestFalse(TEXT("Weak array remains rejected"),
                   ValidateDasPropertyType(Array(Weak), EDasPropertyUsage::Container, Error));
    TestNull(TEXT("Malformed recipe rejected"), DasContainerProperty(TEXT("/Script/DasRuntime.DasContainerV1_FF")));

    auto* ArrayCarrier = DasContainerCarrier(Array(Classes));
    FInstancedStruct Snapshot;
    const FString ValidJson = TEXT("[{\"class\":\"") + Child->GeneratedClass->GetPathName() + TEXT("\"},{\"class\":\"\"}]");
    if (!TestTrue(TEXT("Subclass and null accepted"),
                       ParseDasStructDefault(ArrayCarrier->GetPathName(), ValidJson, Snapshot, Error)))
    {
        AddError(Error);
        return false;
    }
    FInstancedStruct Invalid;
    TestFalse(
        TEXT("Unrelated class refused"),
             ParseDasStructDefault(ArrayCarrier->GetPathName(), TEXT("[{\"class\":\"/Script/Engine.Actor\"}]"), Invalid, Error));
    FDasDefaultRoots Pins;
    Pins.Values.Add(Snapshot);
    TWeakObjectPtr<UClass> ChildClass(Child->GeneratedClass);
    CollectGarbage(RF_NoFlags, true);
    TestTrue(TEXT("Class snapshot retained across GC"), ChildClass.IsValid());

    // Use normal DSL properties and UFunction dispatch, including actual class
    // values inside arrays/maps. Aliases resolve in an imported module.
    const FString RefsModule = Module + TEXT("_refs");
    if (!Write(
            RefsFile,
            TEXT("options gen2\nmodule ") + RefsModule +
                TEXT(" public\nrequire ue_dsl public\n") +
                    TEXT("[ue_object_ref(script=\"") + BaseFile +
                        TEXT("\",type=\"Action\")]\n") +
                            TEXT("struct ActionRef { handle:UObjectHandle }\ntypedef ActionClass = $UEClass<ActionRef>\n")))
        return false;
    const FString Source = TEXT("options gen2\nrequire ") + RefsModule +
        TEXT("\n") +
            TEXT("[ue_class(module_id=\"") + Module +
                TEXT("\",Blueprintable)]\nclass Holder : Actor {\n") +
                    TEXT(" @ue_property=(EditAnywhere,BlueprintReadWrite)\n Kinds:array<ActionClass>\n") +
                        TEXT(" @ue_property=(EditAnywhere,BlueprintReadWrite)\n ByName:table<string;ActionClass>\n") +
                            TEXT(" @ue_property=(EditAnywhere,BlueprintReadWrite)\n Selected:ActionClass\n") +
                                TEXT(" [ue_function(BlueprintCallable)]\n def RoundTrip(kind:ActionClass):int {\n") +
                                    TEXT("  self.Selected=kind\n clear(self.Kinds); push(self.Kinds,self.Selected)\n self.ByName[\"Primary\"]=kind\n") +
                                        TEXT("  var kinds := self.Kinds\n var byName := self.ByName\n") +
                                             TEXT("  return int(length(kinds)+length(byName))\n }\n") +
        TEXT(" @ue_property=(BlueprintReadWrite)\n Numbers:array<int>\n") +
        TEXT(" [ue_function(BlueprintCallable)]\n def PackArray():int {\n") +
        TEXT("  var values:array<int>; resize(values,1000)\n") +
        TEXT("  for (index in range(1000)) { values[index]=index }\n") +
        TEXT("  self.Numbers:=values\n return self.Numbers[999]\n }\n") +
        TEXT(" [ue_function(BlueprintCallable)]\n def Echo(kind:ActionClass):ActionClass { return kind }\n}\n");
    if (!Write(File, Source))
        return false;
    UBlueprint* RawHolder = nullptr;
    if (!FDasDSLService::Apply(File, Root + TEXT("/Holder"), Module + TEXT(".Holder"), RawHolder, Error))
    {
        AddError(Error);
        return false;
    }
    TStrongObjectPtr<UBlueprint> Holder(RawHolder);
    auto* Kinds = FindFProperty<FArrayProperty>(Holder->GeneratedClass, TEXT("Kinds"));
    if (!Kinds ||
        !TestEqual(TEXT("DSL class constraint is authored base"),
                        CastFieldChecked<FClassProperty>(Kinds->Inner)->MetaClass.Get(), Base->GeneratedClass.Get()))
        return false;
    World.Reset(UWorld::CreateWorld(EWorldType::Game, false));
    GEngine->CreateNewWorldContext(EWorldType::Game).SetCurrentWorld(World.Get());
    if (!Runtime.PreparePrograms(World.Get(), {File}, Error))
    {
        AddError(Error);
        return false;
    }
    World->InitializeActorsForPlay(FURL());
    World->GetWorldSettings()->NotifyBeginPlay();
    if (!TestTrue(TEXT("Runtime world began play"), World->HasBegunPlay()))
        return false;
    auto* Owner = World->SpawnActor<ADasActor>(Holder->GeneratedClass);
    if (!TestNotNull(TEXT("Runtime owner"), Owner))
        return false;
    ON_SCOPE_EXIT
    {
        Runtime.ReleaseObject(Owner);
    };
    auto* Function = Owner->FindFunction(TEXT("RoundTrip"));
    if (!TestNotNull(TEXT("RoundTrip method"), Function))
        return false;
    FStructOnScope Frame(Function);
    auto* Input = FindFProperty<FClassProperty>(Function, TEXT("kind"));
    Input->SetObjectPropertyValue_InContainer(Frame.GetStructMemory(), Child->GeneratedClass);
    Owner->ProcessEvent(Function, Frame.GetStructMemory());
    TestTrue(TEXT("RoundTrip runtime error is empty: ") + Owner->LastDasError, Owner->LastDasError.IsEmpty());
    TestEqual(TEXT("DAS array/map read-write roundtrip"),
                   *Function->GetReturnProperty()->ContainerPtrToValuePtr<int32>(Frame.GetStructMemory()), 2);
    FScriptArrayHelper Values(Kinds, Kinds->ContainerPtrToValuePtr<void>(Owner));
    if (TestEqual(TEXT("One class stored"), Values.Num(), 1))
        TestEqual(TEXT("Stored class is the child"),
                       CastFieldChecked<FClassProperty>(Kinds->Inner)->GetObjectPropertyValue(Values.GetRawPtr(0)),
                       static_cast<UObject*>(Child->GeneratedClass.Get()));
    auto* Echo = Owner->FindFunction(TEXT("Echo"));
    if (!TestNotNull(TEXT("Class-returning method"), Echo))
        return false;
    FStructOnScope EchoFrame(Echo);
    auto* EchoInput = FindFProperty<FClassProperty>(Echo, TEXT("kind"));
    EchoInput->SetObjectPropertyValue_InContainer(EchoFrame.GetStructMemory(), Child->GeneratedClass);
    Owner->ProcessEvent(Echo, EchoFrame.GetStructMemory());
    TestTrue(TEXT("Echo runtime error is empty: ") + Owner->LastDasError, Owner->LastDasError.IsEmpty());
    TestEqual(TEXT("Authored class returned through frame"),
        CastFieldChecked<FClassProperty>(Echo->GetReturnProperty())->GetObjectPropertyValue_InContainer(EchoFrame.GetStructMemory()),
        static_cast<UObject*>(Child->GeneratedClass.Get()));
    auto* PackArray = Owner->FindFunction(TEXT("PackArray"));
    if (!TestNotNull(TEXT("Bulk array packing function"), PackArray)) return false;
    const uint64 ReclaimedBefore = Runtime.StructSnapshotsReclaimed(Owner);
    FStructOnScope PackFrame(PackArray);
    Owner->ProcessEvent(PackArray, PackFrame.GetStructMemory());
    TestTrue(TEXT("Bulk packing runtime error: ") + Owner->LastDasError, Owner->LastDasError.IsEmpty());
    TestEqual(TEXT("All 1000 packed values survive transport"),
        CastFieldChecked<FIntProperty>(PackArray->GetReturnProperty())->GetPropertyValue_InContainer(PackFrame.GetStructMemory()), 999);
    const uint64 Reclaimed = Runtime.StructSnapshotsReclaimed(Owner) - ReclaimedBefore;
    TestTrue(TEXT("Packing a scalar array publishes a bounded number of snapshots"), Reclaimed <= 8);

    const FString Repo = FPaths::ConvertRelativePathToFull(FPaths::ProjectDir() / TEXT("../.."));
    FString Expected;
    FFileHelper::LoadFileToString(Expected, *(Repo / TEXT("Bindings/FoundationExterns.txt")));
    TArray<FString> Lines;
    Expected.ParseIntoArrayLines(Lines);
    const FString Actual = DasFoundationExternManifest();
    TestFalse(TEXT("Existing extern manifest is nonempty"), Lines.IsEmpty());
    for (const auto& Line : Lines)
        if (!Actual.Contains(Line + TEXT("\n")))
            AddError(TEXT("Existing extern contract changed: ") + Line);
    TestTrue(TEXT("Export updated extern manifest"),
                  FFileHelper::SaveStringToFile(Actual, *(Repo / TEXT("logs/container-transport-externs.txt")),
                                                          FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM));
    return true;
}
