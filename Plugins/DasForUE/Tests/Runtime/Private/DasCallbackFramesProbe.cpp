#include "DasCallbackFrames.h"
#include "GameFramework/Actor.h"
#include "DasCallbackPayload.h"
#include "DasCallbackTypes.h"
#include "DasContainerTypes.h"
#include "DasDelegateSignatures.h"
#include "DasPropertyValues.h"
#include "DasTypeRegistry.h"
#include "Components/SceneComponent.h"
#include "UObject/GarbageCollection.h"
#include "UObject/Package.h"
#include "UObject/StrongObjectPtr.h"
#include "UObject/StructOnScope.h"
#include "UObject/UnrealType.h"

bool CheckDasCallbackFrames(FString& Error)
{
    auto Fail = [&](const TCHAR* Why)
    {
        Error = Why;
        return false;
    };
    FInstancedStruct TextA, TextB;
    TextA.InitializeAs(FDasArrayTextCarrier::StaticStruct());
    TextB.InitializeAs(FDasArrayTextCarrier::StaticStruct());
    TextA.GetMutable<FDasArrayTextCarrier>().Values.Add(
        FText::AsLocalizable_Advanced_LocText(TEXT("FrameA"), TEXT("A"), TEXT("Same")));
    TextB.GetMutable<FDasArrayTextCarrier>().Values.Add(
        FText::AsLocalizable_Advanced_LocText(TEXT("FrameB"), TEXT("B"), TEXT("Same")));
    if (IdenticalDasStructSnapshot(TextA, TextB) || !IdenticalDasStructSnapshot(TextA, TextA))
        return Fail(TEXT("Snapshot cache lost FText history"));
    UE_LOG(LogTemp, Display,
           TEXT("DAS_SNAPSHOT_IDENTITY_REGRESSION legacy_equal=%d cache_equal=%d"),
                TextA.GetScriptStruct()->CompareScriptStruct(TextA.GetMemory(), TextB.GetMemory(), 0),
                IdenticalDasStructSnapshot(TextA, TextB));
    FInstancedStruct MapA, MapB;
    MapA.InitializeAs(FDasMapIntTextCarrier::StaticStruct());
    MapB.InitializeAs(FDasMapIntTextCarrier::StaticStruct());
    auto& MA = MapA.GetMutable<FDasMapIntTextCarrier>().Values;
    auto& MB = MapB.GetMutable<FDasMapIntTextCarrier>().Values;
    const auto TA = TextA.Get<FDasArrayTextCarrier>().Values[0], TB = TextB.Get<FDasArrayTextCarrier>().Values[0];
    MA.Add(1, TA);
    MA.Add(2, TB);
    MB.Add(2, TA);
    MB.Add(1, TB);
    if (IdenticalDasStructSnapshot(MapA, MapB))
        return Fail(TEXT("Snapshot cache ignored map key/text associations"));


    auto Leaf = [](const TCHAR* Kind, const FString& Path = FString())
    {
        FDasDeclaredType T;
        T.Kind = Kind;
        T.TypePath = Path;
        return T;
    };
    const auto Int = Leaf(TEXT("int")),
                          Text = Leaf(TEXT("text")),
                                      Object = Leaf(TEXT("object"), UObject::StaticClass()->GetPathName());
    if (DasCallbackFrame({Int}, Error))
        return Fail(TEXT("One-field tuple accepted"));
    TArray<FDasDeclaredType> Limit;
    for (int I = 0; I < 16; ++I)
        Limit.Add(Int);
    auto* Maximum = DasCallbackFrame(Limit, Error);
    if (!Maximum || Maximum != DasCallbackFrame(Limit, Error))
        return Fail(TEXT("16-field tuple/reuse failed"));
    Limit.Add(Int);
    if (DasCallbackFrame(Limit, Error))
        return Fail(TEXT("17-field tuple accepted"));
    const auto Class = Leaf(TEXT("class"), AActor::StaticClass()->GetPathName());
    auto* ClassFrame = DasCallbackFrame({Class, Int}, Error);
    if (!ClassFrame)
        return false;
    FDasDeclaredMethod ClassMethod;
    ClassMethod.Name = TEXT("ClassProbe");
    ClassMethod.ReturnType.Kind = TEXT("void");
    for (const auto& T : {Class, Int})
    {
        FDasDeclaredParameter P;
        P.Name = FString::Printf(TEXT("p%d"), ClassMethod.Parameters.Num());
        P.Type = T;
        P.Direction = TEXT("in");
        ClassMethod.Parameters.Add(P);
    }
    TStrongObjectPtr<UFunction> ClassSignature(
        MakeDasDelegateSignature(ClassMethod, true, GetTransientPackage(), NAME_None, Error));
    if (!ClassSignature || !ValidateDasCallbackFrameSource(ClassSignature.Get(), ClassFrame, Error))
        return false;
    FStructOnScope ClassMemory(ClassSignature.Get());
    auto* ClassProperty = FindFProperty<FClassProperty>(ClassSignature.Get(), TEXT("p0"));
    ClassProperty->SetObjectPropertyValue_InContainer(ClassMemory.GetStructMemory(), AActor::StaticClass());
    FDasScalarValue ClassCapture;
    if (!CaptureDasCallbackFrame(ClassSignature.Get(), ClassMemory.GetStructMemory(), ClassFrame, ClassCapture, Error))
        return false;
    auto* CapturedProperty = FindFProperty<FClassProperty>(ClassFrame, TEXT("arg_0"));
    if (!CapturedProperty || CapturedProperty->GetObjectPropertyValue_InContainer(
                                 ClassCapture.StructValue.GetMemory()) != AActor::StaticClass())
        return Fail(TEXT("Tuple class identity lost"));
    auto* TransientClass = NewObject<UClass>(GetTransientPackage());
    TransientClass->SetSuperStruct(AActor::StaticClass());
    TWeakObjectPtr<UClass> ClassWeak(TransientClass);
    ClassProperty->SetObjectPropertyValue_InContainer(ClassMemory.GetStructMemory(), TransientClass);
    if (!CaptureDasCallbackFrame(ClassSignature.Get(), ClassMemory.GetStructMemory(), ClassFrame, ClassCapture, Error))
        return false;
    TSharedPtr<FDasCallbackPayload> ClassOwned = MakeShared<FDasCallbackPayload>(MoveTemp(ClassCapture));
    ClassMemory.Reset();
    ClassCapture = {};
    CollectGarbage(RF_NoFlags);
    if (!ClassWeak.IsValid() || !ClassOwned->IsAlive())
        return Fail(TEXT("Mixed frame lost captured UClass during GC"));
    TransientClass->ClassFlags |= CLASS_NewerVersionExists;
    if (ClassOwned->IsAlive())
        return Fail(TEXT("Mixed frame delivered obsolete class"));
    ClassOwned.Reset();
    CollectGarbage(RF_NoFlags);
    if (ClassWeak.IsValid())
        return Fail(TEXT("Mixed frame retained class after release"));
    ClassMethod.Parameters[0].Type.TypePath = UObject::StaticClass()->GetPathName();
    TStrongObjectPtr<UFunction> Wider(
        MakeDasDelegateSignature(ClassMethod, true, GetTransientPackage(), NAME_None, Error));
    if (!Wider || ValidateDasCallbackFrameSource(Wider.Get(), ClassFrame, Error))
        return Fail(TEXT("Unsafe class narrowing accepted"));
    auto* Frame = DasCallbackFrame({Int, Text, Object}, Error);
    if (!Frame)
        return false;
    if (Frame == DasCallbackFrame({Text, Int, Object}, Error) || !DasCallbackFrameBuilder(Frame))
        return Fail(TEXT("Tuple field order/builder lost"));
    FDasDeclaredMethod Method;
    Method.Name = TEXT("MultiProbe");
    Method.ReturnType.Kind = TEXT("void");
    for (const auto& T : {Int, Text, Leaf(TEXT("object"), USceneComponent::StaticClass()->GetPathName())})
    {
        FDasDeclaredParameter P;
        P.Name = FString::Printf(TEXT("p%d"), Method.Parameters.Num());
        P.Type = T;
        P.Direction = TEXT("in");
        Method.Parameters.Add(P);
    }
    TStrongObjectPtr<UFunction> Signature(
        MakeDasDelegateSignature(Method, true, GetTransientPackage(), NAME_None, Error));
    if (!Signature || !ValidateDasCallbackFrameSource(Signature.Get(), Frame, Error))
        return false;
    auto* Wrong = DasCallbackFrame({Text, Int, Object}, Error);
    if (ValidateDasCallbackFrameSource(Signature.Get(), Wrong, Error))
        return Fail(TEXT("Wrong tuple order accepted"));
    if (ValidateDasCallbackFrameSource(Signature.Get(), Maximum, Error))
        return Fail(TEXT("Wrong tuple count accepted"));
    TArray<FProperty*> Params;
    for (TFieldIterator<FProperty> P(Signature.Get()); P; ++P)
        if (P->HasAnyPropertyFlags(CPF_Parm))
            Params.Add(*P);
    Params[0]->SetPropertyFlags(CPF_OutParm | CPF_ReferenceParm);
    if (ValidateDasCallbackFrameSource(Signature.Get(), Frame, Error))
        return Fail(TEXT("Mutable tuple source accepted"));
    Params[0]->SetPropertyFlags(CPF_ConstParm);
    if (!ValidateDasCallbackFrameSource(Signature.Get(), Frame, Error))
        return false;
    FStructOnScope Memory(Signature.Get());
    TStrongObjectPtr<USceneComponent> Component(NewObject<USceneComponent>());
    TWeakObjectPtr<UObject> Weak = Component.Get();
    FDasScalarValue Number;
    Number.Kind = 0;
    Number.IntValue = 42;
    FDasScalarValue Label;
    Label.Kind = 18;
    Label.TextValue = FText::AsCultureInvariant(TEXT("owned"));
    FDasScalarValue Reference;
    Reference.Kind = 8;
    Reference.ObjectClass = USceneComponent::StaticClass()->GetPathName();
    Reference.ObjectValue = Component.Get();
    const FDasScalarValue Values[] = {Number, Label, Reference};
    for (int I = 0; I < 3; ++I)
        if (!WriteDasPropertyValue(Params[I], Params[I]->ContainerPtrToValuePtr<void>(Memory.GetStructMemory()),
                                   Values[I], Error))
            return false;
    FDasScalarValue Captured;
    if (!CaptureDasCallbackFrame(Signature.Get(), Memory.GetStructMemory(), Frame, Captured, Error))
        return false;
    const uint8* CapturedMemory = Captured.StructValue.GetMemory();
    TSharedPtr<FDasCallbackPayload> Owned = MakeShared<FDasCallbackPayload>(MoveTemp(Captured));
    if (Captured.StructValue.IsValid() || Owned->GetValue().StructValue.GetMemory() != CapturedMemory)
        return Fail(TEXT("Owned callback capture copied its snapshot buffer"));
    Captured = {};
    Memory.Reset();
    Component.Reset();
    Reference = {};
    CollectGarbage(RF_NoFlags);
    if (!Weak.IsValid() || !Owned->IsAlive())
        return Fail(TEXT("Tuple object lost during GC"));
    auto* Field = Frame->FindPropertyByName(TEXT("arg_0"));
    FDasScalarValue Read;
    const auto& Snapshot = Owned->GetValue();
    if (!ReadDasPropertyValue(Field, Field->ContainerPtrToValuePtr<void>(Snapshot.StructValue.GetMemory()), Read,
                              Error) ||
        Read.IntValue != 42)
        return Fail(TEXT("Tuple snapshot content lost"));
    CastChecked<USceneComponent>(Weak.Get())->DestroyComponent();
    CollectGarbage(RF_NoFlags);
    if (Owned->IsAlive())
        return Fail(TEXT("Destroyed tuple capture stayed alive"));
    Owned.Reset();
    CollectGarbage(RF_NoFlags);
    Error.Reset();
    return true;
}
