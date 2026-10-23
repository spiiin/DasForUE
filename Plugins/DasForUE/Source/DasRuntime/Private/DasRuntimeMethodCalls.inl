// Part of DasRuntime.cpp: Reflected method call frames: arguments, results and math value packing.
// Included in place; declaration order matters.
void ValidateConstraintCallInputs(FMarshalledCall&, das::Context*);
void BindPaintCall(FMarshalledCall&, das::Context*);
bool StagePaintCall(FMarshalledCall&, das::Context*);
void BindEventReplyCall(FMarshalledCall&, das::Context*);
bool DispatchEventReplyCall(FMarshalledCall&, das::Context*);
bool DispatchInputNavigationCall(FMarshalledCall&, das::Context*);
bool DispatchImageSoftBrush(FMarshalledCall&, das::Context*);
void CaptureWidgetAnimationOutput(FMarshalledCall&, das::Context*);
void ValidateWidgetAnimationCall(FMarshalledCall&, das::Context*);
void ValidateGeometryCall(FMarshalledCall&, das::Context*, bool);
void ValidateInputEventCallInputs(FMarshalledCall&, das::Context*);
void CaptureConstraintCallOutput(FMarshalledCall&, das::Context*);
#include "DasMarshalling.inl"
// Legacy scalar adapters use the same checked frame and dispatch scope as
// generated interface methods, including recursion and error propagation.
int32 InterfaceCallInt(const FDasObjectHandle& H, const char* Path, const char* Name, int32 Input, das::Context* C)
{
    auto* Object = ResolveObject(H, C);
    CheckedInterfaceFunction(Object, Path, Name, C);
    const auto Shape = CheckCall(Object, Name, ECallKind::Int, ECallKind::Int, C);
    auto Call = BeginInterfaceCall(H, Path, Name, C);
    ON_SCOPE_EXIT
    {
        ActiveContext->Calls.Remove(Call.Token);
    };
    SetCallInt(Call, TCHAR_TO_UTF8(*Shape.Input->GetName()), Input, C);
    InvokeCall(Call, C);
    return GetCallScalar<FIntProperty>(Call, TCHAR_TO_UTF8(*Shape.Output->GetName()), C);
}
char* InterfaceCallString(const FDasObjectHandle& H, const char* Path, const char* Name, das::Context* C)
{
    auto* Object = ResolveObject(H, C);
    auto* Function = CheckedInterfaceFunction(Object, Path, Name, C);
    if (Function->NumParms != 1 || !CastField<FStrProperty>(Function->GetReturnProperty()))
        C->throw_error("interface string getter signature mismatch");
    auto Call = BeginInterfaceCall(H, Path, Name, C);
    ON_SCOPE_EXIT
    {
        ActiveContext->Calls.Remove(Call.Token);
    };
    InvokeCall(Call, C);
    return GetCallString(Call, TCHAR_TO_UTF8(*Function->GetReturnProperty()->GetName()), C);
}

#include "DasTextValues.inl"
#include "DasAuthoredTextByte.inl"
#include "DasTextByteContainers.inl"

// Cleanup is idempotent: a rejected BeginCall may not have produced a handle yet.
void ReleaseAuthoredCall(const FDasCallHandle& H, das::Context* C)
{
    if (C == ActiveContext && ActiveContext)
        ActiveContext->Calls.Remove(H.Token);
}
FDasCallHandle BeginAuthoredCallSelected(const FDasObjectHandle& Target, const char* Id, const char* Name,
                                         const char* Wire, das::Context* Context, UFunction* Selected = nullptr)
{
    auto* Object = ResolveAuthoredObject(Target, Id, Context);
    FDasAuthoringModule Model;
    FString Error;
    if (!Wire || !FDasAuthoringModule::FromJson(UTF8_TO_TCHAR(Wire), Model, Error) || Model.Classes.Num() != 1 ||
        Model.Classes[0].Id != UTF8_TO_TCHAR(Id))
        Context->throw_error("invalid authored method contract");
    const auto* Method = Model.Classes[0].Methods.FindByPredicate([&](const auto& Item)
                                                                  { return Name && Item.Name == UTF8_TO_TCHAR(Name); });
    if (!Method || !Method->bCallable || Method->bStatic)
        Context->throw_error("unknown callable authored method");
    auto Handle = BeginCallSelected(Target, Name, Context, Selected);
    auto& Call = ResolveCall(Handle, Context);

    auto Matches = [&](FProperty* Property, const FDasDeclaredType& Type)
    {
        if (!Property || FString(CallKindName(FieldKind(Property, Context))) != Type.Kind)
            return false;
        if (Type.Kind == TEXT("array") || Type.Kind == TEXT("map") || Type.Kind == TEXT("set"))
        {
            auto* Carrier = DasContainerCarrier(Type);
            auto* Values = Carrier ? Carrier->FindPropertyByName(TEXT("Values")) : nullptr;
            return Values && Values->SameType(Property);
        }
        if (auto* D = CastField<FDelegateProperty>(Property))
        {
            TStrongObjectPtr<UFunction> S(MakeDasSingleDelegateSignature(Type.DelegateWire, Error));
            return S && D->SignatureFunction && S->IsSignatureCompatibleWith(D->SignatureFunction);
        }
        if (auto* Enum = CallEnumType(Property))
            return Enum->GetPathName() == Type.TypePath;
        if (auto* Struct = CastField<FStructProperty>(Property))
            return Struct->Struct->GetPathName() == Type.TypePath;
        if (auto* Class = CastField<FClassProperty>(Property))
            return Class->MetaClass->GetPathName() == Type.TypePath;
        if (auto* Class = DasReferencePropertyClass(Property))
            return Class->GetPathName() == Type.TypePath;
        return true;
    };
    if (Method->ReturnType.Kind == TEXT("void")? Call.Function->GetReturnProperty() != nullptr
                                               : !Matches(Call.Function->GetReturnProperty(), Method->ReturnType))
        Context->throw_error("authored method return contract changed");
    int32 Index = 0;
    for (TFieldIterator<FProperty> It(Call.Function.Get()); It; ++It)
    {
        if (!It->HasAnyPropertyFlags(CPF_Parm) || It->HasAnyPropertyFlags(CPF_ReturnParm))
            continue;
        ++Index;
        const auto* DeclaredParameter =
            Method->Parameters.FindByPredicate([&](const auto& P) { return P.Name == It->GetName(); });
        if (!DeclaredParameter)
            Context->throw_error("authored method parameter name changed");
        const auto& Param = *DeclaredParameter;
        const bool Out = It->HasAnyPropertyFlags(CPF_OutParm) && !It->HasAnyPropertyFlags(CPF_ConstParm);
        const FString Direction =
            Out ? (It->HasAnyPropertyFlags(CPF_ReferenceParm) ? TEXT("inout"): TEXT("out")) : TEXT("in");
        const bool ContainerInput =
            Param.Direction ==
            TEXT("in") &&
                 Direction == TEXT("inout") &&
                                   (Param.Type.Kind ==
                                    TEXT("array") || Param.Type.Kind == TEXT("map") || Param.Type.Kind == TEXT("set"));
        if (It->GetName() != Param.Name || (Direction != Param.Direction && !ContainerInput) ||
            !Matches(*It, Param.Type))
            Context->throw_error(TCHAR_TO_UTF8(*FString::Printf(
                TEXT("authored method parameter contract changed: %s.%s direction=%s/%s kind=%s/%s matches=%d"),
                     *Method->Name, *It->GetName(), *Direction, *Param.Direction, CallKindName(FieldKind(*It, Context)),
                     *Param.Type.Kind, Matches(*It, Param.Type))));
    }
    if (Index != Method->Parameters.Num())
        Context->throw_error("authored method parameter count changed");
    Call.bAuthored = true;
    return Handle;
}
void MethodArgenum(const FDasCallHandle& H, const char* Path, FDasEnumValue Value, das::Context* C)
{
    const auto& V = ResolveEnumValue(Value, C);
    auto& Call = ResolveCall(H, C);
    const auto Field = ResolveCallField(Call, Path, false, C);
    RequireCallEnum(Field.Property, TCHAR_TO_UTF8(*V.Type->GetPathName()), C);
    WriteCallEnum(Call, Field, V.Type.Get(), int64(V.Value), C);
}
FDasEnumValue MethodResultenum(const FDasCallHandle& H, const char* Path, das::Context* C)
{
    auto& Call = ResolveCall(H, C);
    const auto Field = ResolveCallField(Call, Path, true, C);
    auto* Enum = CallEnumType(Field.Property);
    if (!Enum)
        C->throw_error("expected enum result");
    return MakeEnumValue(Enum->GetPathName(), int32(ReadCallEnum(Field, C)), C);
}
// Whole struct snapshots avoid exposing UE addresses or retaining stale property pointers.
FCallField MethodStructField(FMarshalledCall& Call, const char* Path, bool Output, das::Context* C)
{
    if (Call.bFailed || Call.bExecuted != Output || !Path)
        C->throw_error("invalid method snapshot access direction");
    auto* Prop = FindFProperty<FProperty>(Call.Function.Get(), FName(UTF8_TO_TCHAR(Path)));
    if (!Prop || !Prop->HasAnyPropertyFlags(CPF_Parm) ||
        (!CastField<FStructProperty>(Prop) && !DasContainerCarrierForProperty(Prop)))
        C->throw_error("expected struct/container method parameter");
    const bool MutableOut = Prop->HasAnyPropertyFlags(CPF_OutParm) && !Prop->HasAnyPropertyFlags(CPF_ConstParm);
    const bool Return = Prop->HasAnyPropertyFlags(CPF_ReturnParm);
    if (Output ? !(Return || MutableOut) : (Return || (MutableOut && !Prop->HasAnyPropertyFlags(CPF_ReferenceParm))))
        C->throw_error("wrong snapshot parameter direction");
    return {Prop, Prop->ContainerPtrToValuePtr<void>(Call.Frame->GetStructMemory()), Prop->GetName().ToLower()};
}
FDasCallHandle BeginAuthoredCall(const FDasObjectHandle& T, const char* Id, const char* N, const char* W,
                                 das::Context* C)
{
    return BeginAuthoredCallSelected(T, Id, N, W, C);
}
thread_local bool ExplicitParentTick = false;
thread_local UObject* ParentLifecycleTarget = nullptr;
FDasCallHandle BeginAuthoredSuperCall(const FDasObjectHandle& Target, const char* CurrentId, const char* ParentId,
                                      const char* Name, const char* Wire, das::Context* C)
{
    auto* Object = ResolveAuthoredObject(Target, CurrentId, C);
    auto Identity = [](UClass* Class)
    {
        const auto* D = Class ? Class->GetDefaultObject() : nullptr;
        return DasHostIdentity(D);
    };
    UClass* Owner = nullptr;
    for (auto* Class = Object->GetClass(); Class; Class = Class->GetSuperClass())
        if (Identity(Class) == UTF8_TO_TCHAR(CurrentId) && Identity(Class->GetSuperClass()) != UTF8_TO_TCHAR(CurrentId))
        {
            Owner = Class;
            break;
        }
    auto* Parent = Owner ? Owner->GetSuperClass() : nullptr;
    if (!Parent || Identity(Parent) != UTF8_TO_TCHAR(ParentId))
        C->throw_error("Super parent identity changed");
    auto* Function = Parent->FindFunctionByName(FName(UTF8_TO_TCHAR(Name)));
    if (!Function)
        C->throw_error("Super method is missing");
    auto H = BeginAuthoredCallSelected(Target, ParentId, Name, Wire, C, Function);
    ResolveCall(H, C).FunctionScopeClass = Parent;
    return H;
}
void InvokeParentLifecycle(const char* Id, int32 Kind, float Delta, int32 Reason, das::Context* C)
{
    RequireObjectContext(C);
    if (Kind < 0 || Kind > 4 || !FMath::IsFinite(Delta) || Delta < 0)
        C->throw_error("invalid parent lifecycle call");
    if (Kind == 3 && !ActiveContext->bInitializingObject)
        C->throw_error("Super.Initialize is only allowed during Initialize");
    if (Kind == 4 && !ConstructingObject)
        C->throw_error("Super.ConstructionScript is only allowed during ConstructionScript");
    if (ConstructingObject && Kind != 4)
        C->throw_error("gameplay lifecycle is forbidden during ConstructionScript");
    auto* Object = ResolveSelf(C);
    auto Identity = [](UClass* Class)
    {
        const auto* D = Class ? Class->GetDefaultObject() : nullptr;
        return DasHostIdentity(D);
    };
    UClass* Owner = nullptr;
    for (auto* Class = Object->GetClass(); Class; Class = Class->GetSuperClass())
        if (Identity(Class) == UTF8_TO_TCHAR(Id) && Identity(Class->GetSuperClass()) != UTF8_TO_TCHAR(Id))
        {
            Owner = Class;
            break;
        }
    if (!Owner || !Owner->GetSuperClass())
        C->throw_error("authored lifecycle parent is missing");
    const auto* Default = Owner->GetSuperClass()->GetDefaultObject();
    const FString File = DasHostLifecycle(Default);
    if (File.IsEmpty())
        return;
    TGuardValue<bool> TickScope(ExplicitParentTick, Kind == 1);
    TGuardValue<UObject*> ParentScope(ParentLifecycleTarget, Object);
    auto& R = FModuleManager::LoadModuleChecked<FDasRuntimeModule>(TEXT("DasRuntime"));
    const auto Result = R.RunLifecycle(R.ScriptPath(File),
                                       Kind == 0   ? EDasCallback::BeginPlay
                                       : Kind == 1 ? EDasCallback::Tick
                                       : Kind == 2 ? EDasCallback::EndPlay
                                       : Kind == 3 ? EDasCallback::Initialize
                                                   : EDasCallback::Construction,
                                       Object, Delta, Reason);
    if (!Result.bSucceeded)
        C->throw_error(TCHAR_TO_UTF8(*Result.Error));
}
void PinMethodSnapshot(FProperty* Property, void* Memory, FMarshalledCall& Call, const FString& Path, das::Context* C,
                       int32& Budget, int32 Depth = 0)
{
    if (Depth > 16 || --Budget < 0)
        C->throw_error("method snapshot traversal limit exceeded");
    if (CastField<FWeakObjectProperty>(Property))
        return;
    if (DasReferencePropertyClass(Property))
    {
        auto* Object = DasReferenceGet(Property, Memory);
        if (Object &&
            (!IsValid(Object) || (Cast<AActor>(Object) && CastChecked<AActor>(Object)->IsActorBeingDestroyed())))
            C->throw_error("method snapshot object is expired");
        Call.ObjectPins.Add(Path, TStrongObjectPtr<UObject>(Object));
    }
    else if (auto* Struct = CastField<FStructProperty>(Property))
    {
        for (TFieldIterator<FProperty> It(Struct->Struct); It; ++It)
            PinMethodSnapshot(*It, It->ContainerPtrToValuePtr<void>(Memory), Call,
                              Path + TEXT(".")+ It->GetName().ToLower(), C, Budget, Depth + 1);
    }
    else if (auto* Array = CastField<FArrayProperty>(Property))
    {
        FScriptArrayHelper Values(Array, Memory);
        if (Values.Num() > MaxCallArrayElements)
            C->throw_error("method array size limit exceeded");
        for (int32 Index = 0; Index < Values.Num(); ++Index)
            PinMethodSnapshot(Array->Inner, Values.GetRawPtr(Index), Call,
                              Path + TEXT(".")+ FString::FromInt(Index), C, Budget, Depth + 1);
    }
    else if (auto* Map = CastField<FMapProperty>(Property))
    {
        FScriptMapHelper Values(Map, Memory);
        if (Values.Num() > MaxCallArrayElements)
            C->throw_error("method map size limit exceeded");
        for (int32 Index = 0; Index < Values.GetMaxIndex(); ++Index)
            if (Values.IsValidIndex(Index))
            {
                PinMethodSnapshot(Map->KeyProp, Values.GetKeyPtr(Index), Call,
                                  Path + TEXT(".key")+ FString::FromInt(Index), C, Budget, Depth + 1);
                PinMethodSnapshot(Map->ValueProp, Values.GetValuePtr(Index), Call,
                                  Path + TEXT(".value")+ FString::FromInt(Index), C, Budget, Depth + 1);
            }
    }
    else if (auto* Set = CastField<FSetProperty>(Property))
    {
        FScriptSetHelper Values(Set, Memory);
        if (Values.Num() > MaxCallArrayElements)
            C->throw_error("method set size limit exceeded");
        for (int32 Index = 0; Index < Values.GetMaxIndex(); ++Index)
            if (Values.IsValidIndex(Index))
                PinMethodSnapshot(Set->ElementProp, Values.GetElementPtr(Index), Call,
                                  Path + TEXT(".")+ FString::FromInt(Index), C, Budget, Depth + 1);
    }
}
void MethodArgstruct(const FDasCallHandle& H, const char* Path, const FDasStructValue& Value, das::Context* C)
{
    auto& Call = ResolveCall(H, C);
    const auto Field = MethodStructField(Call, Path, false, C);
    FString Error;
    if (!WriteDasPropertySnapshot(Field.Property, Field.Memory, ResolveStructValue(Value, C), Error))
        C->throw_error(TCHAR_TO_UTF8(*Error));
    Call.Assigned.Add(Field.Path);
    int32 Budget = 1048576;
    PinMethodSnapshot(Field.Property, Field.Memory, Call, Field.Path, C, Budget);
    Call.WholeInputs.Add(Field.Path);
}
FDasStructValue MethodResultstruct(const FDasCallHandle& H, const char* Path, das::Context* C, das::LineInfoArg* At)
{
    auto& Call = ResolveCall(H, C);
    const auto Field = MethodStructField(Call, Path, true, C);
    return StoreStructValue(ReadCallValue(Field, C).StructValue, C, At);
}


// Math values use native copied storage in the same reflected call frame.
template <class T> FDasStructValue MathPack(const T& Value, das::Context* C, das::LineInfoArg* At)
{
    FInstancedStruct Snapshot;
    Snapshot.InitializeAs(TBaseStructure<T>::Get(), reinterpret_cast<const uint8*>(&Value));
    return StoreStructValue(MoveTemp(Snapshot), C, At);
}
template <class T> T MathUnpack(const FDasStructValue& Value, das::Context* C)
{
    const auto& Snapshot = ResolveStructValue(Value, C);
    if (Snapshot.GetScriptStruct() != TBaseStructure<T>::Get() || !MathSnapshotValid(Snapshot))
        C->throw_error("math property snapshot type/value mismatch");
    return Snapshot.Get<T>();
}
FVector2D MakeVector2D(double X, double Y)
{
    return FVector2D(X, Y);
}
FDasStructValue MathPackvector2d(const FVector2D& V, das::Context* C, das::LineInfoArg* At)
{
    return MathPack(V, C, At);
}
FVector2D MathUnpackvector2d(const FDasStructValue& V, das::Context* C)
{
    return MathUnpack<FVector2D>(V, C);
}
FDasStructValue MathPackvector(const FVector& V, das::Context* C, das::LineInfoArg* At)
{
    return MathPack(V, C, At);
}
FDasStructValue MathPackrotator(const FRotator& V, das::Context* C, das::LineInfoArg* At)
{
    return MathPack(V, C, At);
}
FDasStructValue MathPacktransform(const FTransform& V, das::Context* C, das::LineInfoArg* At)
{
    return MathPack(V, C, At);
}
FVector MathUnpackvector(const FDasStructValue& V, das::Context* C)
{
    return MathUnpack<FVector>(V, C);
}
FRotator MathUnpackrotator(const FDasStructValue& V, das::Context* C)
{
    return MathUnpack<FRotator>(V, C);
}
FTransform MathUnpacktransform(const FDasStructValue& V, das::Context* C)
{
    return MathUnpack<FTransform>(V, C);
}
bool MethodMathValid(const FVector2D& Value)
{
    return DasMathValueValid(TBaseStructure<FVector2D>::Get(), &Value);
}
bool MethodMathValid(const FVector& Value)
{
    return DasMathValueValid(TBaseStructure<FVector>::Get(), &Value);
}
bool MethodMathValid(const FRotator& Value)
{
    return DasMathValueValid(TBaseStructure<FRotator>::Get(), &Value);
}
bool MethodMathValid(const FTransform& Value)
{
    return DasMathValueValid(TBaseStructure<FTransform>::Get(), &Value);
}
// Concrete leaf assignments also cover aggregate writes to elements of arrays.
void MarkMathCallLeaves(FMarshalledCall& Call, FProperty* Property, const FString& Path)
{
    if (auto* Struct = CastField<FStructProperty>(Property))
    {
        for (TFieldIterator<FProperty> It(Struct->Struct); It; ++It)
            MarkMathCallLeaves(Call, *It, Path + TEXT(".")+ It->GetName().ToLower());
    }
    else
        Call.Assigned.Add(Path);
}
template <class T> void MethodArgMath(const FDasCallHandle& H, const char* Path, const T& Value, das::Context* C)
{
    auto& Call = ResolveCall(H, C);
    const auto Field = ResolveCallField(Call, Path, false, C, true);
    auto* Prop = CastField<FStructProperty>(Field.Property);
    if (!Prop || Prop->Struct != TBaseStructure<T>::Get() || !MethodMathValid(Value))
        C->throw_error("method math input type/value mismatch");
    FDasScalarValue Native;
    Native.Kind = int32(EDasMethodSignature::Struct);
    Native.StructValue.InitializeAs(TBaseStructure<T>::Get(), reinterpret_cast<const uint8*>(&Value));
    WriteCallValue(Call, Field, Native, C);
    MarkMathCallLeaves(Call, Field.Property, Field.Path);
}
template <class T> T MethodResultMath(const FDasCallHandle& H, const char* Path, das::Context* C)
{
    auto& Call = ResolveCall(H, C);
    const auto Field = ResolveCallField(Call, Path, true, C, true);
    auto* Prop = CastField<FStructProperty>(Field.Property);
    if (!Prop || Prop->Struct != TBaseStructure<T>::Get())
        C->throw_error("method math result type mismatch");
    const auto Native = ReadCallValue(Field, C);
    return Native.StructValue.Get<T>();
}
void MethodArgvector2d(const FDasCallHandle& H, const char* P, const FVector2D& V, das::Context* C)
{
    MethodArgMath(H, P, V, C);
}
void MethodArgvector(const FDasCallHandle& H, const char* P, const FVector& V, das::Context* C)
{
    MethodArgMath(H, P, V, C);
}
void MethodArgrotator(const FDasCallHandle& H, const char* P, const FRotator& V, das::Context* C)
{
    MethodArgMath(H, P, V, C);
}
void MethodArgtransform(const FDasCallHandle& H, const char* P, const FTransform& V, das::Context* C)
{
    MethodArgMath(H, P, V, C);
}
FVector2D MethodResultvector2d(const FDasCallHandle& H, const char* P, das::Context* C)
{
    return MethodResultMath<FVector2D>(H, P, C);
}
FVector MethodResultvector(const FDasCallHandle& H, const char* P, das::Context* C)
{
    return MethodResultMath<FVector>(H, P, C);
}
FRotator MethodResultrotator(const FDasCallHandle& H, const char* P, das::Context* C)
{
    return MethodResultMath<FRotator>(H, P, C);
}
FTransform MethodResulttransform(const FDasCallHandle& H, const char* P, das::Context* C)
{
    return MethodResultMath<FTransform>(H, P, C);
}
#include "DasDelegateValues.inl"


FProperty* ScalarEventParameter(FMulticastDelegateProperty* P)
{
    if (!P || !P->SignatureFunction)
        return nullptr;
    for (TFieldIterator<FProperty> It(P->SignatureFunction); It; ++It)
        if (It->HasAnyPropertyFlags(CPF_Parm) && !It->HasAnyPropertyFlags(CPF_ReturnParm))
            return *It;
    return nullptr;
}

FDasStructValue MathPackquat(const FQuat& V, das::Context* C, das::LineInfoArg* At)
{
    return MathPack(V, C, At);
}
FQuat MathUnpackquat(const FDasStructValue& V, das::Context* C)
{
    return MathUnpack<FQuat>(V, C);
}
void MethodArgquat(const FDasCallHandle& H, const char* P, const FQuat& V, das::Context* C)
{
    FString Error;
    if (!FDasTypeAdapters::Get().FindStruct(TBaseStructure<FQuat>::Get())->ValidateValue(&V, Error))
        C->throw_error(TCHAR_TO_UTF8(*Error));
    MethodArgstruct(H, P, MathPack(V, C, nullptr), C);
}
FQuat MethodResultquat(const FDasCallHandle& H, const char* P, das::Context* C, das::LineInfoArg* At)
{
    return MathUnpack<FQuat>(MethodResultstruct(H, P, C, At), C);
}

FDasStructValue MathPacklinear_color(const FLinearColor& V, das::Context* C, das::LineInfoArg* At)
{
    return MathPack(V, C, At);
}
FLinearColor MathUnpacklinear_color(const FDasStructValue& V, das::Context* C)
{
    return MathUnpack<FLinearColor>(V, C);
}
void MethodArglinear_color(const FDasCallHandle& H, const char* P, const FLinearColor& V, das::Context* C)
{
    FString Error;
    if (!FDasTypeAdapters::Get().FindStruct(TBaseStructure<FLinearColor>::Get())->ValidateValue(&V, Error))
        C->throw_error(TCHAR_TO_UTF8(*Error));
    MethodArgstruct(H, P, MathPack(V, C, nullptr), C);
}
FLinearColor MethodResultlinear_color(const FDasCallHandle& H, const char* P, das::Context* C, das::LineInfoArg* At)
{
    return MathUnpack<FLinearColor>(MethodResultstruct(H, P, C, At), C);
}

FDasStructValue MathPackcolor(const FColor& V, das::Context* C, das::LineInfoArg* At)
{
    return MathPack(V, C, At);
}
FColor MathUnpackcolor(const FDasStructValue& V, das::Context* C)
{
    return MathUnpack<FColor>(V, C);
}
void MethodArgcolor(const FDasCallHandle& H, const char* P, const FColor& V, das::Context* C)
{
    FString Error;
    if (!FDasTypeAdapters::Get().FindStruct(TBaseStructure<FColor>::Get())->ValidateValue(&V, Error))
        C->throw_error(TCHAR_TO_UTF8(*Error));
    MethodArgstruct(H, P, MathPack(V, C, nullptr), C);
}
FColor MethodResultcolor(const FDasCallHandle& H, const char* P, das::Context* C, das::LineInfoArg* At)
{
    return MathUnpack<FColor>(MethodResultstruct(H, P, C, At), C);
}
