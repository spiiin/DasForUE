// Included inside the runtime namespace; frames never expose raw UE memory to scripts.
struct FCallField
{
    FProperty* Property;
    void* Memory;
    FString Path;
};

// VM adapters choose the transport slot; native memory access belongs to
// the shared property-value service. Direction, assignment and GC ownership
// remain properties of this invocation-local frame.


FDasScalarValue ReadCallValue(const FCallField& Field, das::Context* C)
{
    return ReadNativePropertyValue(Field.Property, Field.Memory, C);
}
void WriteCallValue(FMarshalledCall& Call, const FCallField& Field, const FDasScalarValue& Value, das::Context* C)
{
    WriteNativePropertyValue(Field.Property, Field.Memory, Value, C);
    Call.Assigned.Add(Field.Path);
}
void WriteCallEnum(FMarshalledCall& Call, const FCallField& Field, UEnum* Enum, int64 Number, das::Context* C)
{
    WriteNativeEnumNumber(Field.Property, Field.Memory, Enum, Number, C);
    Call.Assigned.Add(Field.Path);
}
int64 ReadCallEnum(const FCallField& Field, das::Context* C)
{
    return ReadNativeEnumNumber(Field.Property, Field.Memory, C);
}

UEnum* CallEnumType(FProperty* Property)
{
    return DasEnumPropertyType(Property);
}
FNumericProperty* CallEnumStorage(FProperty* Property)
{
    if (auto* Enum = CastField<FEnumProperty>(Property))
        return Enum->GetUnderlyingProperty();
    return CastField<FByteProperty>(Property);
}

EMarshallingKind FieldKind(FProperty* Property, das::Context* Context)
{
    switch (DasPropertyKind(Property))
    {
    case EDasTypeKind::Map:
        return EMarshallingKind::Map;
    case EDasTypeKind::Set:
        return EMarshallingKind::Set;
    case EDasTypeKind::Array:
        return EMarshallingKind::Array;
    case EDasTypeKind::Delegate:
        return EMarshallingKind::Delegate;
    case EDasTypeKind::Enum:
        return EMarshallingKind::Enum;
    case EDasTypeKind::Byte:
        return EMarshallingKind::Byte;
    case EDasTypeKind::Text:
        return EMarshallingKind::Text;
    case EDasTypeKind::Name:
        return EMarshallingKind::Name;
    case EDasTypeKind::Int:
        return EMarshallingKind::Int;
    case EDasTypeKind::Float:
        return EMarshallingKind::Float;
    case EDasTypeKind::Double:
        return EMarshallingKind::Double;
    case EDasTypeKind::Bool:
        return EMarshallingKind::Bool;
    case EDasTypeKind::String:
        return EMarshallingKind::String;
    case EDasTypeKind::Class:
        return EMarshallingKind::Class;
    case EDasTypeKind::Object:
        return EMarshallingKind::Object;
    case EDasTypeKind::Struct:
        return EMarshallingKind::Struct;
    default:
        break;
    }
    Context->throw_error("unsupported UFunction or struct field type");
}

bool IsHashScalar(FProperty* Property, das::Context* Context)
{
    const auto Kind = FieldKind(Property, Context);
    return Kind == EMarshallingKind::Byte || Kind == EMarshallingKind::Int || Kind == EMarshallingKind::Float ||
           Kind == EMarshallingKind::Double || Kind == EMarshallingKind::Bool || Kind == EMarshallingKind::String ||
           Kind == EMarshallingKind::Name || Kind == EMarshallingKind::Enum;
}

// Recursively validate the entire signature, including outputs, before allocating a frame.
void ValidateCallField(FProperty* Property, const FString& Path, bool bInput, bool bOutput,
                       TArray<FMarshalledParameter>& Signature, das::Context* Context, int32 Depth = 0)
{
    if (Depth > 16)
        Context->throw_error("call struct nesting limit exceeded");
    if (DasHasDirectWeakContainer(Property))
        Context->throw_error("Direct weak-reference containers need a storage-preserving carrier");
    if (auto* Map = CastField<FMapProperty>(Property))
    {
        if (Property->ArrayDim != 1)
            Context->throw_error("fixed hash arrays require an adapter");
        const auto ValueKind = FieldKind(Map->ValueProp, Context);
        if ((!IsHashScalar(Map->KeyProp, Context) && !IsDasObjectHashProperty(Map->KeyProp)) ||
            (!IsHashScalar(Map->ValueProp, Context) && ValueKind != EMarshallingKind::Text &&
             ValueKind != EMarshallingKind::Struct && ValueKind != EMarshallingKind::Object))
            Context->throw_error("unsupported map key or value type");
        if (!Map->KeyProp->HasAllPropertyFlags(CPF_HasGetValueTypeHash))
            Context->throw_error("map key requires UE value hashing");
        Signature.Add({Path.ToLower(), EMarshallingKind::Map, bInput, bOutput,
                       Map->KeyProp->GetClass()->GetName() + TEXT(";") + Map->ValueProp->GetClass()->GetName()});
        ValidateCallField(Map->KeyProp, Path + TEXT(".*.key"), bInput, bOutput, Signature, Context);
        ValidateCallField(Map->ValueProp, Path + TEXT(".*.value"), bInput, bOutput, Signature, Context);
        return;
    }
    if (auto* Set = CastField<FSetProperty>(Property))
    {
        if (Property->ArrayDim != 1)
            Context->throw_error("fixed hash arrays require an adapter");
        if (!IsHashScalar(Set->ElementProp, Context) && !IsDasObjectHashProperty(Set->ElementProp))
            Context->throw_error("unsupported set element type");
        if (!Set->ElementProp->HasAllPropertyFlags(CPF_HasGetValueTypeHash))
            Context->throw_error("set element requires UE value hashing");
        Signature.Add(
            {Path.ToLower(), EMarshallingKind::Set, bInput, bOutput, Set->ElementProp->GetClass()->GetName()});
        ValidateCallField(Set->ElementProp, Path + TEXT(".*"), bInput, bOutput, Signature, Context);
        return;
    }
    if (auto* Array = CastField<FArrayProperty>(Property))
    {
        if (Property->ArrayDim != 1)
            Context->throw_error("fixed arrays are not supported by call marshalling");
        if (FieldKind(Array->Inner, Context) == EMarshallingKind::Array)
            Context->throw_error("nested arrays require a dedicated adapter");
        Signature.Add({Path.ToLower(), EMarshallingKind::Array, bInput, bOutput, Array->Inner->GetClass()->GetName()});
        // Wildcard leaves describe every element without storing UE property addresses.
        // Reflected object leaves are pinned independently, including nested structs.
        ValidateCallField(Array->Inner, Path + TEXT(".*"), bInput, bOutput, Signature, Context);
        return;
    }
    if (Depth > 16)
        Context->throw_error("call struct nesting limit exceeded");
    if (DasHasDirectWeakContainer(Property))
        Context->throw_error("Direct weak-reference containers need a storage-preserving carrier");
    if (auto* Soft = DasSoftCarrierForProperty(Property))
        Signature.Add({Path.ToLower(), EMarshallingKind::Struct, bInput, bOutput, Soft->GetPathName()});
    else if (FieldKind(Property, Context) == EMarshallingKind::Struct)
    {
        UScriptStruct* Type = CastFieldChecked<FStructProperty>(Property)->Struct;
        if (DasIsWidgetAnimationType(Type))
        {
            auto* Function = Cast<UFunction>(Property->GetOwnerStruct());
            if (Depth != 0 || Property->ArrayDim != 1 || !Property->HasAnyPropertyFlags(CPF_ReturnParm) || bInput ||
                !bOutput || !DasWidgetAnimationFunctionAllowed(Function))
                Context->throw_error("WidgetAnimation requires exact scalar producer result");
            Signature.Add({Path.ToLower(), EMarshallingKind::Struct, bInput, bOutput, Type->GetPathName()});
            return;
        }
        if (DasIsPaintContextType(Type))
        {
            auto* Function = Cast<UFunction>(Property->GetOwnerStruct());
            if (Depth != 0 || Property->ArrayDim != 1 || !Property->HasAnyPropertyFlags(CPF_Parm) ||
                Property->HasAnyPropertyFlags(CPF_ReturnParm) || !DasPaintFunctionAllowed(Function))
                Context->throw_error("PaintContext requires exact scalar synchronous OnPaint adapter");
            Signature.Add({Path.ToLower(), EMarshallingKind::Struct, bInput, bOutput, Type->GetPathName()});
            return;
        }
        if (DasIsEventReplyType(Type))
        {
            auto* Function = Cast<UFunction>(Property->GetOwnerStruct());
            if (Depth != 0 || Property->ArrayDim != 1 || !Property->HasAnyPropertyFlags(CPF_Parm) ||
                !DasEventReplyFunctionAllowed(Function))
                Context->throw_error("EventReply requires exact scalar synchronous callback adapter");
            Signature.Add({Path.ToLower(), EMarshallingKind::Struct, bInput, bOutput, Type->GetPathName()});
            return;
        }
        if (DasIsGeometryType(Type))
        {
            auto* Function = Cast<UFunction>(Property->GetOwnerStruct());
            if (Depth != 0 || Property->ArrayDim != 1 || !Property->HasAnyPropertyFlags(CPF_Parm) ||
                !DasGeometryFunctionAllowed(Function))
                Context->throw_error("Geometry requires exact scalar snapshot function allowlist");
            Signature.Add({Path.ToLower(), EMarshallingKind::Struct, bInput, bOutput, Type->GetPathName()});
            return;
        }
        if (DasIsInputEventType(Type))
        {
            auto* Function = Cast<UFunction>(Property->GetOwnerStruct());
            if (Depth != 0 || Property->ArrayDim != 1 || !Property->HasAnyPropertyFlags(CPF_Parm) ||
                !DasInputEventFunctionAllowed(Function))
                Context->throw_error(
                    "input events require exact scalar snapshot function allowlist; navigation/reply need live adapters");
            Signature.Add({Path.ToLower(), EMarshallingKind::Struct, bInput, bOutput, Type->GetPathName()});
            return;
        }
        if (Type == FInputActionValue::StaticStruct())
        {
            if (Property->ArrayDim != 1)
                Context->throw_error("fixed InputActionValue arrays need an adapter");
            Signature.Add({Path.ToLower(), EMarshallingKind::Struct, bInput, bOutput, Type->GetPathName()});
            return;
        }
        // Native collision state is an opaque leaf; never flatten ActorInstanceHandle.
        if (Type == FHitResult::StaticStruct())
        {
            if (Property->ArrayDim != 1)
                Context->throw_error("fixed hit result arrays need an adapter");
            Signature.Add({Path.ToLower(), EMarshallingKind::Struct, bInput, bOutput, Type->GetPathName()});
            return;
        }
        if (Type == FConstraintInstanceAccessor::StaticStruct())
        {
            auto* Function = Cast<UFunction>(Property->GetOwnerStruct());
            const bool Getter =
                Function && Function->GetOuterUClass() == UPhysicsConstraintComponent::StaticClass() &&
                Function->GetFName() == TEXT("GetConstraint") && Property->HasAnyPropertyFlags(CPF_ReturnParm);
            const bool Consumer = Function &&
                                  Function->GetOuterUClass() == UConstraintInstanceBlueprintLibrary::StaticClass() &&
                                  !Property->HasAnyPropertyFlags(CPF_ReturnParm) && bInput;
            if (Property->ArrayDim != 1 || !Property->HasAnyPropertyFlags(CPF_Parm) || (!Getter && !Consumer))
                Context->throw_error("constraint accessors require scalar component-owned getter/library contract");
            Signature.Add({Path.ToLower(), EMarshallingKind::Struct, bInput, bOutput, Type->GetPathName()});
            return;
        }
        if (Type->GetPathName() == TEXT("/Script/Engine.BodyInstanceAsyncPhysicsTickHandle"))
            Context->throw_error("async physics handles require a scoped frozen-game-thread physics callback adapter");
        if (Type->GetPathName() == TEXT("/Script/Engine.ActorInstanceHandle"))
            Context->throw_error("Actor instance handles require the collision result adapter");
        // Native opaque state cannot be described by reflection alone.
        if (Type->GetCppStructOps() && Type->GetCppStructOps()->HasAddStructReferencedObjects())
            Context->throw_error("structs with native object references are unsupported");
        for (TFieldIterator<FProperty> It(Type); It; ++It)
            ValidateCallField(*It, Path + TEXT(".") + It->GetName(), bInput, bOutput, Signature, Context, Depth + 1);
    }
    else
        Signature.Add({Path.ToLower(), FieldKind(Property, Context), bInput, bOutput,
                       CastField<FDelegateProperty>(Property)
                           ? CastFieldChecked<FDelegateProperty>(Property)->SignatureFunction->GetPathName()
                       : CastField<FClassProperty>(Property)
                           ? CastFieldChecked<FClassProperty>(Property)->MetaClass->GetPathName()
                       : CallEnumType(Property)
                           ? CallEnumType(Property)->GetPathName()
                           : (DasReferencePropertyClass(Property) ? DasReferencePropertyClass(Property)->GetPathName()
                                                                  : FString())});
}

TArray<FMarshalledParameter> DescribeCall(UFunction* Function, das::Context* Context)
{
    TArray<FMarshalledParameter> Signature;
    for (TFieldIterator<FProperty> It(Function); It; ++It)
    {
        if (!It->HasAnyPropertyFlags(CPF_Parm))
            continue;
        const bool bReturn = It->HasAnyPropertyFlags(CPF_ReturnParm);
        const bool bMutableOut =
            !It->HasAnyPropertyFlags(CPF_ConstParm) && It->HasAnyPropertyFlags(CPF_OutParm | CPF_ReferenceParm);
        const bool bInput = !bReturn && (!bMutableOut || It->HasAnyPropertyFlags(CPF_ReferenceParm));
        ValidateCallField(*It, It->GetName(), bInput, bReturn || bMutableOut, Signature, Context);
    }
    return Signature;
}
const TCHAR* CallKindName(EMarshallingKind Kind)
{
    switch (Kind)
    {
    case EMarshallingKind::Byte:
        return TEXT("byte");
    case EMarshallingKind::Text:
        return TEXT("text");
    case EMarshallingKind::Int:
        return TEXT("int");
    case EMarshallingKind::Float:
        return TEXT("float");
    case EMarshallingKind::Double:
        return TEXT("double");
    case EMarshallingKind::Bool:
        return TEXT("bool");
    case EMarshallingKind::String:
        return TEXT("string");
    case EMarshallingKind::Object:
        return TEXT("object");
    case EMarshallingKind::Class:
        return TEXT("class");
    case EMarshallingKind::Name:
        return TEXT("name");
    case EMarshallingKind::Enum:
        return TEXT("enum");
    case EMarshallingKind::Delegate:
        return TEXT("delegate");
    case EMarshallingKind::Array:
        return TEXT("array");
    case EMarshallingKind::Map:
        return TEXT("map");
    case EMarshallingKind::Set:
        return TEXT("set");
    default:
        return TEXT("struct");
    }
}
void AppendCallShape(FProperty* Property, FString& Key)
{
    Key += Property->GetName() +
           TEXT(":") + Property->GetClass()->GetName() +
                TEXT(":") + FString::Printf(TEXT("%llu"), uint64(Property->GetPropertyFlags() &
                                                                 (CPF_Parm | CPF_ReturnParm | CPF_OutParm |
                                                                  CPF_ReferenceParm | CPF_ConstParm)));
    if (auto* D = CastField<FDelegateProperty>(Property))
    {
        Key += TEXT(":")+ (D->SignatureFunction ? D->SignatureFunction->GetPathName() : TEXT("missing"));
        if (D->SignatureFunction)
            for (TFieldIterator<FProperty> It(D->SignatureFunction); It; ++It)
            {
                if (CastField<FDelegateProperty>(*It))
                    Key += TEXT(":nested_delegate");
                else
                    AppendCallShape(*It, Key);
            }
    }
    if (UEnum* Enum = CallEnumType(Property))
        Key += TEXT(":") + Enum->GetPathName();
    // Include the constraint even though soft properties share the struct ABI.
    if (auto* Soft = CastField<FSoftObjectProperty>(Property))
    {
        auto* Class = CastField<FSoftClassProperty>(Soft);
        Key += TEXT(":soft:")+ (Class ? Class->MetaClass->GetPathName() : Soft->PropertyClass->GetPathName());
    }
    if (auto* Class = CastField<FClassProperty>(Property))
        Key += TEXT(":meta:")+ Class->MetaClass->GetPathName();
    if (auto* ReferenceClass = DasReferencePropertyClass(Property))
        Key += TEXT(":") + ReferenceClass->GetPathName();
    if (auto* Array = CastField<FArrayProperty>(Property))
    {
        Key += TEXT("[");
        AppendCallShape(Array->Inner, Key);
        Key += TEXT("]");
    }
    if (auto* Map = CastField<FMapProperty>(Property))
    {
        Key += TEXT("[");
        AppendCallShape(Map->KeyProp, Key);
        AppendCallShape(Map->ValueProp, Key);
        Key += TEXT("]");
    }
    if (auto* Set = CastField<FSetProperty>(Property))
    {
        Key += TEXT("[");
        AppendCallShape(Set->ElementProp, Key);
        Key += TEXT("]");
    }
    if (auto* Struct = CastField<FStructProperty>(Property))
    {
        Key += TEXT(":") + Struct->Struct->GetPathName() + TEXT("{");
        for (TFieldIterator<FProperty> It(Struct->Struct); It; ++It)
            AppendCallShape(*It, Key);
        Key += TEXT("}");
    }
    Key += TEXT(";");
}
FString CallSignatureKey(UFunction* Function)
{
    FString Key = TEXT("v1;") + Function->GetName() + TEXT(";");
    for (TFieldIterator<FProperty> It(Function); It; ++It)
        if (It->HasAnyPropertyFlags(CPF_Parm))
            AppendCallShape(*It, Key);
    return Key;
}

FObjectProperty* StaticWorldParameter(UFunction* Function, const FString& Name, das::Context* Context)
{
    if (Name.IsEmpty())
        return nullptr;
    auto* P = FindFProperty<FObjectProperty>(Function, *Name);
    if (!P || CastField<FClassProperty>(P) || P->ArrayDim != 1 || !P->HasAnyPropertyFlags(CPF_Parm) ||
        P->HasAnyPropertyFlags(CPF_ReturnParm) ||
        (P->HasAnyPropertyFlags(CPF_OutParm | CPF_ReferenceParm) && !P->HasAnyPropertyFlags(CPF_ConstParm)))
        Context->throw_error("WorldContext must name an input object parameter");
    return P;
}
#include "DasCollisionProfileAdapter.inl"

void SetCallConstructionPolicy(FMarshalledCall& Call, das::Context* Context, bool Dispatch = false)
{
#if WITH_EDITOR
    Call.bConstructionPolicyKnown = true;
    Call.bConstructionUnsafe |= Call.Function->HasMetaData(TEXT("UnsafeDuringActorConstruction"));
#endif
    if (Dispatch && ConstructingObject && !Call.bConstructionPolicyKnown && !Call.bAuthored)
        Context->throw_error("ConstructionScript requires a generated call context policy");
    Call.bConstructionUnsafe |= IsCollisionProfilePropertyAdapter(Call.Function.Get());
    CheckConstructionCall(Call.bConstructionUnsafe, Context);
}
FDasCallHandle BeginStaticCall(const char* Path, const char* Name, const char* WorldParameter, das::Context* Context)
{
    RequireObjectContext(Context);
    if (ActiveContext->bCollectingSubscriptions)
        Context->throw_error("UFunction calls are forbidden in bind_events");
    const FString ClassPath = Path ? UTF8_TO_TCHAR(Path) : TEXT("");
    auto* Class = FindObject<UClass>(nullptr, *ClassPath);
    if (!ClassPath.StartsWith(TEXT("/Script/")) || !Class || !Class->HasAnyClassFlags(CLASS_Native))
        Context->throw_error("expected loaded native static owner class");
    if (!Name || !*Name || FCStringAnsi::Strlen(Name) >= NAME_SIZE)
        Context->throw_error("invalid static function name");
    auto* Function = Class->FindFunctionByName(FName(UTF8_TO_TCHAR(Name)));
    if (!Function || !Function->HasAllFunctionFlags(FUNC_Static | FUNC_Native | FUNC_BlueprintCallable) ||
        Function->HasAnyFunctionFlags(FUNC_Net | FUNC_Delegate | FUNC_MulticastDelegate))
        Context->throw_error("expected native static BlueprintCallable function");
    const bool ProfileAdapter = IsCollisionProfilePropertyAdapter(Function);
    if (ProfileAdapter)
        ValidateCollisionProfileAdapterSignature(Function, Context);
    const FString WorldName = WorldParameter ? UTF8_TO_TCHAR(WorldParameter) : TEXT("");
#if WITH_EDITOR
    if (Function->HasMetaData(TEXT("Latent")) || (Function->HasMetaData(TEXT("CustomThunk")) && !ProfileAdapter))
        Context->throw_error("static function needs a special call adapter");
    if (Function->GetMetaData(TEXT("WorldContext")).ToLower() != WorldName.ToLower())
        Context->throw_error("WorldContext contract changed; regenerate bindings");
#endif
    auto* WorldProperty = StaticWorldParameter(Function, WorldName, Context);
    UObject* WorldObject = nullptr;
    UWorld* World = nullptr;
    if (WorldProperty)
    {
        WorldObject = ResolveWorldContextObject(Context);
        World = GEngine ? GEngine->GetWorldFromContextObject(WorldObject, EGetWorldErrorMode::ReturnNull) : nullptr;
        if (!World || !World->IsGameWorld() || !WorldObject->IsA(WorldProperty->PropertyClass))
            Context->throw_error("WorldContext requires script owner's live game world and compatible object");
    }
    auto Signature = DescribeCall(Function, Context);
    if (ActiveContext->Calls.Num() >= 256)
        Context->throw_error("call frame limit reached");
    auto Call = MakeShared<FMarshalledCall>();
    Call->Signature = MoveTemp(Signature);
    Call->SignatureKey = CallSignatureKey(Function);
    Call->Target = Class->GetDefaultObject();
    Call->Function.Reset(Function);
    Call->Frame = MakeUnique<FStructOnScope>(Function);
    if (WorldProperty)
    {
        FDasScalarValue Native;
        Native.Kind = int32(EDasMethodSignature::Object);
        Native.ObjectClass = WorldProperty->PropertyClass->GetPathName();
        Native.ObjectValue = WorldObject;
        WriteNativePropertyValue(WorldProperty,
                                 WorldProperty->ContainerPtrToValuePtr<void>(Call->Frame->GetStructMemory()), Native,
                                 Context);
        Call->AutomaticWorldContext = WorldProperty->GetName().ToLower();
        Call->StaticWorld = World;
        Call->Assigned.Add(Call->AutomaticWorldContext);
        Call->ObjectPins.Add(Call->AutomaticWorldContext, TStrongObjectPtr<UObject>(WorldObject));
    }
    SetCallConstructionPolicy(*Call, Context);
    BindEventReplyCall(*Call, Context);
    BindPaintCall(*Call, Context);
    const uint64 Token = NextCallHandle++;
    ActiveContext->Calls.Add(Token, MoveTemp(Call));
    return {Token};
}

FDasCallHandle BeginCallSelected(const FDasObjectHandle& Target, const char* Name, das::Context* Context,
                                 UFunction* Selected = nullptr)
{
    UObject* Object = ResolveObject(Target, Context);
    if (ActiveContext->bCollectingSubscriptions)
        Context->throw_error("UFunction calls are forbidden in bind_events");
    if (!Name || !*Name || FCStringAnsi::Strlen(Name) >= NAME_SIZE)
        Context->throw_error("invalid function name");
    UFunction* Function = Selected ? Selected : Object->FindFunction(FName(UTF8_TO_TCHAR(Name)));
    if (!Function || !Function->HasAnyFunctionFlags(FUNC_BlueprintCallable) ||
        Function->HasAnyFunctionFlags(FUNC_Static | FUNC_Net | FUNC_Delegate | FUNC_MulticastDelegate))
        Context->throw_error("expected a local BlueprintCallable instance UFunction");
    auto Signature = DescribeCall(Function, Context);
    if (ActiveContext->Calls.Num() >= 256)
        Context->throw_error("call frame limit reached; release unused calls");
    auto Call = MakeShared<FMarshalledCall>();
    Call->Signature = MoveTemp(Signature);
    Call->SignatureKey = CallSignatureKey(Function);
    Call->Target = Object;
    Call->Function.Reset(Function);
    Call->Frame = MakeUnique<FStructOnScope>(Function);
    SetCallConstructionPolicy(*Call, Context);
    const uint64 Token = NextCallHandle++;
    ActiveContext->Calls.Add(Token, MoveTemp(Call));
    return {Token};
}

FDasCallHandle BeginCall(const FDasObjectHandle& T, const char* N, das::Context* C)
{
    return BeginCallSelected(T, N, C);
}

FMarshalledCall& ResolveCall(const FDasCallHandle& Handle, das::Context* Context)
{
    RequireObjectContext(Context);
    auto* Found = ActiveContext->Calls.Find(Handle.Token);
    if (!Found)
        Context->throw_error("call handle is released or belongs to another context");
    return **Found;
}

FDasCallHandle BeginInterfaceCall(const FDasObjectHandle& Target, const char* Path, const char* Name, das::Context* C)
{
    auto* Object = ResolveObject(Target, C);
    auto* Interface = CheckedInterface(Path, C);
    CheckedInterfaceFunction(Object, Path, Name, C);
    auto Handle = BeginCall(Target, Name, C);
    auto& Call = ResolveCall(Handle, C);
    Call.bAuthored = true;
    Call.RequiredInterface = Interface;
    Call.InterfaceSignatureKey = CallSignatureKey(Interface->FindFunctionByName(Call.Function->GetFName()));
    return Handle;
}
void ExpectInterfaceSignature(const FDasCallHandle& Handle, const char* Expected, das::Context* C)
{
    auto& Call = ResolveCall(Handle, C);
    if (Call.bExecuted || !Call.RequiredInterface.IsValid() || !Expected ||
        Call.InterfaceSignatureKey != UTF8_TO_TCHAR(Expected))
        C->throw_error("generated interface signature changed; regenerate bindings");
}
void RequireCallContext(const FDasCallHandle& Handle, bool ConstructionUnsafe, das::Context* Context)
{
    auto& Call = ResolveCall(Handle, Context);
    if (Call.bExecuted)
        Context->throw_error("call context policy requires a pending frame");
    Call.bConstructionPolicyKnown = true;
    Call.bConstructionUnsafe |= ConstructionUnsafe;
    CheckConstructionCall(Call.bConstructionUnsafe, Context);
}
void ExpectCallSignature(const FDasCallHandle& Handle, const char* Expected, das::Context* Context)
{
    auto& Call = ResolveCall(Handle, Context);
    if (Call.bExecuted || !Expected || Call.SignatureKey != UTF8_TO_TCHAR(Expected))
        Context->throw_error("generated binding signature changed; regenerate bindings");
}

FCallField ResolveCallField(FMarshalledCall& Call, const char* Path, bool bOutput, das::Context* Context,
                            bool bAllowStruct = false)
{
    if (Call.bFailed)
        Context->throw_error("call failed; results are unavailable");
    if (Call.bExecuted != bOutput)
        Context->throw_error("arguments require a pending call; results require an executed call");
    if (!Path || !*Path)
        Context->throw_error("parameter path is empty");
    if (!bOutput && !Call.AutomaticWorldContext.IsEmpty() &&
        Call.AutomaticWorldContext.Equals(UTF8_TO_TCHAR(Path), ESearchCase::IgnoreCase))
        Context->throw_error("automatic WorldContext cannot be overridden");
    TArray<FString> Parts;
    FString(UTF8_TO_TCHAR(Path)).ParseIntoArray(Parts, TEXT("."), false);
    UStruct* Type = Call.Function.Get();
    void* Memory = Call.Frame->GetStructMemory();
    FString Canonical, DescriptionPath;
    for (int32 Index = 0; Index < Parts.Num(); ++Index)
    {
        if (Parts[Index].IsEmpty() || Parts[Index].Len() >= NAME_SIZE)
            Context->throw_error("invalid parameter path");
        FProperty* Property = FindFProperty<FProperty>(Type, *Parts[Index]);
        if (!Property)
            Context->throw_error("unknown parameter or wrong parameter direction");
        const FString Name = Property->GetName().ToLower();
        Canonical += (Canonical.IsEmpty() ? TEXT("") : TEXT(".")) + Name;
        DescriptionPath += (DescriptionPath.IsEmpty() ? TEXT("") : TEXT(".")) + Name;
        Memory = Property->ContainerPtrToValuePtr<void>(Memory);
        if (auto* Array = CastField<FArrayProperty>(Property); Array && Index + 1 < Parts.Num())
        {
            const auto* Root =
                Call.Signature.FindByPredicate([&](const auto& Item) { return Item.Path == DescriptionPath; });
            if (!Root || !(bOutput ? Root->bOutput : Root->bInput))
                Context->throw_error("wrong array parameter direction");
            if (!bOutput && !Call.Assigned.Contains(Canonical))
                Context->throw_error("initialize array before assigning elements");
            const FString& Number = Parts[++Index];
            if (Number.IsEmpty() || Number.Len() > 10)
                Context->throw_error("invalid array index");
            for (TCHAR Ch : Number)
                if (Ch < TEXT('0') || Ch > TEXT('9'))
                    Context->throw_error("invalid array index");
            int64 Parsed = -1;
            if (!LexTryParseString(Parsed, *Number) || Parsed < 0 || Parsed > MAX_int32)
                Context->throw_error("invalid array index");
            FScriptArrayHelper Helper(Array, Memory);
            const int32 Element = static_cast<int32>(Parsed);
            if (Element >= Helper.Num())
                Context->throw_error("array index out of bounds");
            Memory = Helper.GetRawPtr(Element);
            Property = Array->Inner;
            Canonical += TEXT(".") + FString::FromInt(Element);
            DescriptionPath += TEXT(".*");
        }
        if ((CastField<FMapProperty>(Property) || CastField<FSetProperty>(Property)) && Index + 1 < Parts.Num())
        {
            auto* Map = CastField<FMapProperty>(Property);
            auto* Set = CastField<FSetProperty>(Property);
            const FString Root = Canonical;
            const auto* Direction =
                Call.Signature.FindByPredicate([&](const auto& Item) { return Item.Path == DescriptionPath; });
            if (!Direction || !(bOutput ? Direction->bOutput : Direction->bInput))
                Context->throw_error("wrong hash container direction");
            const FString& Number = Parts[++Index];
            int64 Parsed = -1;
            if (Number.IsEmpty() || Number.Len() > 10)
                Context->throw_error("invalid hash index");
            for (TCHAR Ch : Number)
                if (Ch < TEXT('0') || Ch > TEXT('9'))
                    Context->throw_error("invalid hash index");
            if (!LexTryParseString(Parsed, *Number) || Parsed < 0 || Parsed > MAX_int32)
                Context->throw_error("invalid hash index");
            bool bKey = true;
            FString Suffix;
            if (Map)
            {
                if (++Index >= Parts.Num())
                    Context->throw_error("map slot requires key or value");
                Suffix = Parts[Index].ToLower();
                if (Suffix != TEXT("key") && Suffix != TEXT("value"))
                    Context->throw_error("map slot requires key or value");
                bKey = Suffix == TEXT("key");
                Suffix = TEXT(".") + Suffix;
            }
            if (Index != Parts.Num() - 1)
                Context->throw_error("nested hash values are unsupported");
            const int32 Slot = static_cast<int32>(Parsed);
            Property = Map ? (bKey ? Map->KeyProp : Map->ValueProp) : Set->ElementProp;
            if (!bOutput)
            {
                auto* Pending = Call.PendingHashes.Find(Root);
                if (!Pending)
                    Context->throw_error("initialize hash container before assigning slots");
                if (Slot >= (*Pending)->Keys.Num())
                    Context->throw_error("hash index out of bounds");
                Memory = (bKey ? (*Pending)->Keys[Slot] : (*Pending)->Values[Slot])->Memory;
            }
            else
            {
                const auto* Indices = Call.HashIndices.Find(Root);
                if (!Indices || Slot >= Indices->Num())
                    Context->throw_error("hash index out of bounds");
                const int32 Internal = (*Indices)[Slot];
                if (Map)
                {
                    FScriptMapHelper Helper(Map, Memory);
                    Memory = bKey ? Helper.GetKeyPtr(Internal) : Helper.GetValuePtr(Internal);
                }
                else
                {
                    FScriptSetHelper Helper(Set, Memory);
                    Memory = Helper.GetElementPtr(Internal);
                }
            }
            Canonical += TEXT(".") + FString::FromInt(Slot) + Suffix;
            DescriptionPath += TEXT(".*") + Suffix;
        }
        if (Index == Parts.Num() - 1)
        {
            // An aggregate still derives its access direction from the validated leaf contract.
            const auto* Description = Call.Signature.FindByPredicate(
                [&](const auto& Item)
                {
                    return Item.Path == DescriptionPath || (bAllowStruct && CastField<FStructProperty>(Property) &&
                                                            Item.Path.StartsWith(DescriptionPath + TEXT(".")));
                });
            if (!Description || !(bOutput ? Description->bOutput : Description->bInput))
                Context->throw_error("unknown parameter or wrong parameter direction");
            return {Property, Memory, Canonical};
        }
        auto* Struct = CastField<FStructProperty>(Property);
        if (!Struct)
            Context->throw_error("parameter path traverses a non-struct value");
        Type = Struct->Struct;
    }
    Context->throw_error("invalid parameter path");
}

constexpr int32 MaxCallArrayElements = 65536;
void PrepareCallArray(const FDasCallHandle& Handle, const char* Path, int32 Count, das::Context* Context)
{
    auto& Call = ResolveCall(Handle, Context);
    auto Field = ResolveCallField(Call, Path, false, Context);
    auto* Property = CastField<FArrayProperty>(Field.Property);
    if (!Property)
        Context->throw_error("expected an array argument");
    if (Count < 0 || Count > MaxCallArrayElements)
        Context->throw_error("array size limit exceeded");
    FScriptArrayHelper Helper(Property, Field.Memory);
    Helper.EmptyValues();
    Helper.AddValues(Count);
    const FString Prefix = Field.Path + TEXT(".");
    for (auto It = Call.Assigned.CreateIterator(); It; ++It)
        if (It->StartsWith(Prefix))
            It.RemoveCurrent();
    for (auto It = Call.ObjectPins.CreateIterator(); It; ++It)
        if (It.Key().StartsWith(Prefix))
            It.RemoveCurrent();
    for (auto It = Call.PendingHashes.CreateIterator(); It; ++It)
        if (It.Key().StartsWith(Prefix))
            It.RemoveCurrent();
    Call.Assigned.Add(Field.Path);
}
int32 GetCallArraySize(const FDasCallHandle& Handle, const char* Path, das::Context* Context)
{
    auto& Call = ResolveCall(Handle, Context);
    auto Field = ResolveCallField(Call, Path, true, Context);
    auto* Property = CastField<FArrayProperty>(Field.Property);
    if (!Property)
        Context->throw_error("expected an array result");
    FScriptArrayHelper Helper(Property, Field.Memory);
    if (Helper.Num() > MaxCallArrayElements)
        Context->throw_error("array size limit exceeded");
    return Helper.Num();
}

void VisitCallContainerPaths(FMarshalledCall& Call, const FString& Path, bool Output, das::Context* Context,
                             TFunctionRef<void(const FString&)> Visit, int32& Budget, int32 Depth = 0)
{
    if (Depth > 16 || --Budget < 0)
        Context->throw_error("call container traversal limit exceeded");
    const int32 Wildcard = Path.Find(TEXT(".*"));
    if (Wildcard == INDEX_NONE)
    {
        Visit(Path);
        return;
    }
    const FString Root = Path.Left(Wildcard);
    auto Field = ResolveCallField(Call, TCHAR_TO_UTF8(*Root), Output, Context);
    if (!Output && !Call.Assigned.Contains(Field.Path))
        Context->throw_error("missing input container initialization");
    int32 Count = 0;
    if (auto* Array = CastField<FArrayProperty>(Field.Property))
    {
        FScriptArrayHelper Values(Array, Field.Memory);
        Count = Values.Num();
    }
    else if (CastField<FMapProperty>(Field.Property) || CastField<FSetProperty>(Field.Property))
    {
        if (Output)
        {
            const auto* Indices = Call.HashIndices.Find(Field.Path);
            if (!Indices)
                Context->throw_error("hash output indices are not initialized");
            Count = Indices->Num();
        }
        else
        {
            const auto* Pending = Call.PendingHashes.Find(Field.Path);
            if (!Pending)
                Context->throw_error("missing input hash initialization");
            Count = (*Pending)->Keys.Num();
        }
    }
    else
        Context->throw_error("container wildcard traverses a non-container");
    if (Count > MaxCallArrayElements)
        Context->throw_error("container size limit exceeded");
    for (int32 Index = 0; Index < Count; ++Index)
    {
        const FString Concrete = Root + TEXT(".")+ FString::FromInt(Index) + Path.Mid(Wildcard + 2);
        VisitCallContainerPaths(Call, Concrete, Output, Context, Visit, Budget, Depth + 1);
    }
}
void ValidateHashNumber(FProperty* Property, void* Memory, bool bNormalizeZero, das::Context* Context)
{
    if (IsDasObjectHashProperty(Property))
    {
        auto* O = DasReferenceGet(Property, Memory);
        if (O && !LiveHashObject(O))
            Context->throw_error("object hash key is destroyed or expired");
    }
    if (auto* Float = CastField<FFloatProperty>(Property))
    {
        const float Value = Float->GetPropertyValue(Memory);
        if (!FMath::IsFinite(Value))
            Context->throw_error("hash key or set element must be finite");
        if (bNormalizeZero && Value == 0)
            Float->SetPropertyValue(Memory, 0.f);
    }
    if (auto* Double = CastField<FDoubleProperty>(Property))
    {
        const double Value = Double->GetPropertyValue(Memory);
        if (!FMath::IsFinite(Value))
            Context->throw_error("hash key or set element must be finite");
        if (bNormalizeZero && Value == 0)
            Double->SetPropertyValue(Memory, 0.0);
    }
}
void PrepareCallHash(const FDasCallHandle& Handle, const char* Path, int32 Count, das::Context* Context)
{
    auto& Call = ResolveCall(Handle, Context);
    auto Field = ResolveCallField(Call, Path, false, Context);
    auto* Map = CastField<FMapProperty>(Field.Property);
    auto* Set = CastField<FSetProperty>(Field.Property);
    if (!Map && !Set)
        Context->throw_error("expected a map or set argument");
    if (Count < 0 || Count > MaxCallArrayElements)
        Context->throw_error("hash container size limit exceeded");
    auto Pending = MakeShared<FPendingHash>();
    Pending->Keys.Reserve(Count);
    Pending->Values.Reserve(Map ? Count : 0);
    for (int32 Index = 0; Index < Count; ++Index)
    {
        Pending->Keys.Add(MakeUnique<FHashValue>(Map ? Map->KeyProp : Set->ElementProp));
        if (Map)
            Pending->Values.Add(MakeUnique<FHashValue>(Map->ValueProp));
    }
    Call.PendingHashes.Add(Field.Path, MoveTemp(Pending));
    const FString Prefix = Field.Path + TEXT(".");
    for (auto It = Call.Assigned.CreateIterator(); It; ++It)
        if (It->StartsWith(Prefix))
            It.RemoveCurrent();
    Call.Assigned.Add(Field.Path);
}
int32 GetCallHashSize(const FDasCallHandle& Handle, const char* Path, das::Context* Context)
{
    auto& Call = ResolveCall(Handle, Context);
    auto Field = ResolveCallField(Call, Path, true, Context);
    auto* Map = CastField<FMapProperty>(Field.Property);
    auto* Set = CastField<FSetProperty>(Field.Property);
    if (!Map && !Set)
        Context->throw_error("expected a map or set result");
    auto& Indices = Call.HashIndices.FindOrAdd(Field.Path);
    Indices.Reset();
    if (Map)
    {
        FScriptMapHelper Helper(Map, Field.Memory);
        if (Helper.Num() > MaxCallArrayElements)
            Context->throw_error("hash container size limit exceeded");
        for (int32 Index = 0; Index < Helper.GetMaxIndex(); ++Index)
            if (Helper.IsValidIndex(Index))
            {
                ValidateHashNumber(Map->KeyProp, Helper.GetKeyPtr(Index), false, Context);
                Indices.Add(Index);
            }
    }
    else
    {
        FScriptSetHelper Helper(Set, Field.Memory);
        if (Helper.Num() > MaxCallArrayElements)
            Context->throw_error("hash container size limit exceeded");
        for (int32 Index = 0; Index < Helper.GetMaxIndex(); ++Index)
            if (Helper.IsValidIndex(Index))
            {
                ValidateHashNumber(Set->ElementProp, Helper.GetElementPtr(Index), false, Context);
                Indices.Add(Index);
            }
    }
    return Indices.Num();
}
void CommitPendingHashes(FMarshalledCall& Call, das::Context* Context)
{
    for (auto& Pair : Call.PendingHashes)
    {
        auto Field = ResolveCallField(Call, TCHAR_TO_UTF8(*Pair.Key), false, Context);
        auto* Map = CastField<FMapProperty>(Field.Property);
        auto* Set = CastField<FSetProperty>(Field.Property);
        auto& Pending = *Pair.Value;
        // Use UE's hash and equality, notably for FName case/number semantics.
        TMultiMap<uint32, const void*> Seen;
        for (int32 Index = 0; Index < Pending.Keys.Num(); ++Index)
        {
            const FString Slot = Pair.Key + TEXT(".") + FString::FromInt(Index);
            if (!Call.Assigned.Contains(Slot + (Map ? TEXT(".key") : TEXT(""))) ||
                                        (Map && !Call.Assigned.Contains(Slot + TEXT(".value"))))
                Context->throw_error("missing map key/value or set element");
            const auto& Key = *Pending.Keys[Index];
            ValidateHashNumber(Key.Property, Key.Memory, true, Context);
            const uint32 Hash = Key.Property->GetValueTypeHash(Key.Memory);
            for (auto It = Seen.CreateConstKeyIterator(Hash); It; ++It)
                if (Key.Property->Identical(Key.Memory, It.Value()))
                    Context->throw_error("duplicate map key or set element");
            Seen.Add(Hash, Key.Memory);
        }
        if (Map)
        {
            FScriptMapHelper Helper(Map, Field.Memory);
            Helper.EmptyValues(Pending.Keys.Num());
            for (int32 Index = 0; Index < Pending.Keys.Num(); ++Index)
                Helper.AddPair(Pending.Keys[Index]->Memory, Pending.Values[Index]->Memory);
        }
        else
        {
            FScriptSetHelper Helper(Set, Field.Memory);
            Helper.EmptyElements(Pending.Keys.Num());
            for (const auto& Key : Pending.Keys)
                Helper.AddElement(Key->Memory);
        }
    }
}

template <class P, class V>
void SetCallScalar(const FDasCallHandle& Handle, const char* Path, V Value, das::Context* Context)
{
    auto& Call = ResolveCall(Handle, Context);
    auto Field = ResolveCallField(Call, Path, false, Context);
    auto* Property = CastField<P>(Field.Property);
    if (!Property)
        Context->throw_error("argument scalar type mismatch");
    FDasScalarValue Native;
    Native.Kind = int32(TPropertyScalarSlot<P>::Kind);
    Native.*TPropertyScalarSlot<P>::Member = Value;
    WriteCallValue(Call, Field, Native, Context);
}
void SetCallInt(const FDasCallHandle& H, const char* P, int32 V, das::Context* C)
{
    SetCallScalar<FIntProperty>(H, P, V, C);
}
void SetCallFloat(const FDasCallHandle& H, const char* P, float V, das::Context* C)
{
    SetCallScalar<FFloatProperty>(H, P, V, C);
}
void SetCallDouble(const FDasCallHandle& H, const char* P, double V, das::Context* C)
{
    SetCallScalar<FDoubleProperty>(H, P, V, C);
}
void SetCallBool(const FDasCallHandle& H, const char* P, bool V, das::Context* C)
{
    SetCallScalar<FBoolProperty>(H, P, V, C);
}
void SetCallString(const FDasCallHandle& H, const char* P, const char* V, das::Context* C)
{
    SetCallScalar<FStrProperty>(H, P, FString(V ? UTF8_TO_TCHAR(V) : TEXT("")), C);
}

void SetCallName(const FDasCallHandle& Handle, const char* Path, const char* Value, das::Context* Context)
{
    const FString Text(Value ? UTF8_TO_TCHAR(Value) : TEXT(""));
    if (Text.Len() >= NAME_SIZE)
        Context->throw_error("FName value is too long");
    // Validate the field before adding a name to UE's global name table.
    auto& Call = ResolveCall(Handle, Context);
    auto Field = ResolveCallField(Call, Path, false, Context);
    auto* Property = CastField<FNameProperty>(Field.Property);
    if (!Property)
        Context->throw_error("argument FName type mismatch");
    FDasScalarValue Native;
    Native.Kind = int32(EDasMethodSignature::Name);
    Native.NameValue = FName(*Text);
    WriteCallValue(Call, Field, Native, Context);
}
UEnum* RequireCallEnum(FProperty* Property, const char* TypePath, das::Context* Context)
{
    UEnum* Enum = CallEnumType(Property);
    if (!Enum)
        Context->throw_error("expected an enum parameter");
    if (!TypePath || Enum->GetPathName() != UTF8_TO_TCHAR(TypePath))
        Context->throw_error("enum type path mismatch");
    return Enum;
}
bool IsCallEnumMember(UEnum* Enum, int32 Index)
{
    // Stable in cooked builds: never depend on editor-only Hidden metadata.
    return Index != INDEX_NONE && !Enum->GetNameStringByIndex(Index).EndsWith(TEXT("_MAX"));
}
void SetCallEnum(const FDasCallHandle& Handle, const char* Path, const char* TypePath, const char* Member,
                 das::Context* Context)
{
    auto& Call = ResolveCall(Handle, Context);
    auto Field = ResolveCallField(Call, Path, false, Context);
    UEnum* Enum = RequireCallEnum(Field.Property, TypePath, Context);
    const FString Symbol(Member ? UTF8_TO_TCHAR(Member) : TEXT(""));
    if (Symbol.Len() >= NAME_SIZE)
        Context->throw_error("enum member is too long");
    int32 Index = INDEX_NONE;
    // Compare registered symbols directly: never intern unchecked input, use display names or redirects.
    for (int32 Candidate = 0; Candidate < Enum->NumEnums(); ++Candidate)
        if (Symbol.Equals(Enum->GetNameStringByIndex(Candidate), ESearchCase::IgnoreCase) ||
            Symbol.Equals(Enum->GetNameByIndex(Candidate).ToString(), ESearchCase::IgnoreCase))
        {
            Index = Candidate;
            break;
        }
    if (!IsCallEnumMember(Enum, Index))
        Context->throw_error("unknown or reserved enum member");
    WriteCallEnum(Call, Field, Enum, Enum->GetValueByIndex(Index), Context);
}
char* GetCallEnum(const FDasCallHandle& Handle, const char* Path, const char* TypePath, das::Context* Context)
{
    auto& Call = ResolveCall(Handle, Context);
    auto Field = ResolveCallField(Call, Path, true, Context);
    UEnum* Enum = RequireCallEnum(Field.Property, TypePath, Context);
    const int64 Value = ReadCallEnum(Field, Context);
    const int32 Index = Enum->GetIndexByValue(Value);
    if (!IsCallEnumMember(Enum, Index))
        Context->throw_error("UFunction returned an unknown or reserved enum value");
    const FString Name = Enum->GetNameStringByIndex(Index);
    const FTCHARToUTF8 Utf8(*Name);
    return Context->allocateString(Utf8.Get(), Utf8.Length(), nullptr);
}

FDasClassHandle NullClass()
{
    return {};
}
UClass* ValidateCallClass(FClassProperty* Property, UClass* Class, das::Context* C)
{
    if (!Property || !Property->MetaClass)
        C->throw_error("expected reflected class parameter");
    if (Class && (!IsValid(Class) || !Class->IsChildOf(Property->MetaClass) ||
                  Class->HasAnyClassFlags(CLASS_Deprecated | CLASS_NewerVersionExists)))
        C->throw_error("class argument/result has incompatible MetaClass or is obsolete");
    return Class;
}
void SetCallClass(const FDasCallHandle& H, const char* Path, FDasClassHandle V, das::Context* C)
{
    auto& Call = ResolveCall(H, C);
    auto Field = ResolveCallField(Call, Path, false, C);
    auto* P = CastField<FClassProperty>(Field.Property);
    auto* Class = V.Token ? Cast<UClass>(ResolveObject(FDasObjectHandle{V.Token}, C)) : nullptr;
    if (V.Token && !Class)
        C->throw_error("expected class handle");
    ValidateCallClass(P, Class, C);
    FDasScalarValue Native;
    Native.Kind = int32(EDasMethodSignature::Class);
    Native.ObjectClass = P->MetaClass->GetPathName();
    Native.ClassValue = Class;
    WriteCallValue(Call, Field, Native, C);
    Call.ObjectPins.Add(Field.Path, TStrongObjectPtr<UObject>(Class));
}
FDasClassHandle GetCallClass(const FDasCallHandle& H, const char* Path, das::Context* C)
{
    auto& Call = ResolveCall(H, C);
    auto Field = ResolveCallField(Call, Path, true, C);
    auto* P = CastField<FClassProperty>(Field.Property);
    if (!P)
        C->throw_error("expected reflected class result");
    auto* Class = ValidateCallClass(P, ReadCallValue(Field, C).ClassValue.Get(), C);
    return {MakeObjectHandle(Class, UClass::StaticClass(), C).Token};
}
void SetCallObject(const FDasCallHandle& Handle, const char* Path, const FDasObjectHandle& Value, das::Context* Context)
{
    auto& Call = ResolveCall(Handle, Context);
    auto Field = ResolveCallField(Call, Path, false, Context);
    auto* Property = DasReferencePropertyClass(Field.Property);
    if (!Property)
        Context->throw_error("argument object type mismatch");
    UObject* Object = Value.Token ? ResolveObject(Value, Context) : nullptr;
    if (Object && !DasReferenceMatches(Object, Property))
        Context->throw_error("object argument has an incompatible class");
    FDasScalarValue Native;
    Native.Kind = int32(EDasMethodSignature::Object);
    Native.ObjectClass = Property->GetPathName();
    Native.ObjectValue = Object;
    WriteCallValue(Call, Field, Native, Context);
    if (!CastField<FWeakObjectProperty>(Field.Property))
        Call.ObjectPins.Add(Field.Path, TStrongObjectPtr<UObject>(Object));
}
FDasObjectHandle GetCallObject(const FDasCallHandle& Handle, const char* Path, das::Context* Context)
{
    auto& Call = ResolveCall(Handle, Context);
    auto Field = ResolveCallField(Call, Path, true, Context);
    auto* Property = DasReferencePropertyClass(Field.Property);
    if (!Property)
        Context->throw_error("result object type mismatch");
    return MakeObjectHandle(ReadCallValue(Field, Context).ObjectValue.Get(), Property, Context);
}

void InvokeCall(const FDasCallHandle& Handle, das::Context* Context)
{
    auto& Call = ResolveCall(Handle, Context);
    if (!Call.CallbackFramePath.IsEmpty())
        Context->throw_error("callback builders cannot invoke a UFunction");
    if (ActiveContext->bCollectingSubscriptions)
        Context->throw_error("UFunction calls are forbidden in bind_events");
    if (Call.bExecuted)
        Context->throw_error("a call frame may only execute once");
    UObject* Object = Call.Target.Get();
    if (!Call.DelegateInvoke &&
        (!IsValid(Object) || (Cast<AActor>(Object) && CastChecked<AActor>(Object)->IsActorBeingDestroyed()) ||
         (Cast<UActorComponent>(Object) && CastChecked<UActorComponent>(Object)->IsBeingDestroyed())))
        Context->throw_error("call target expired");
    if (!Call.InterfaceSignatureKey.IsEmpty())
    {
        auto* Interface = Call.RequiredInterface.Get();
        auto* Declared = Interface ? Interface->FindFunctionByName(Call.Function->GetFName()) : nullptr;
        if (!Interface || !Object->GetClass()->ImplementsInterface(Interface) || !Declared ||
            CallSignatureKey(Declared) != Call.InterfaceSignatureKey ||
            !Call.Function->IsSignatureCompatibleWith(Declared))
            Context->throw_error("interface contract changed; create a new interface frame");
    }
    if (!Call.DelegateInvoke &&
        (Call.FunctionScopeClass.IsValid() ? Call.FunctionScopeClass->FindFunctionByName(Call.Function->GetFName())
                                           : Object->FindFunction(Call.Function->GetFName())) != Call.Function.Get())
        Context->throw_error("UFunction changed; create a new call frame");
    if (!Call.AutomaticWorldContext.IsEmpty())
    {
        auto* Self = ResolveWorldContextObject(Context);
        auto* World = GEngine ? GEngine->GetWorldFromContextObject(Self, EGetWorldErrorMode::ReturnNull) : nullptr;
        auto* P = StaticWorldParameter(Call.Function.Get(), Call.AutomaticWorldContext, Context);
        if (!World || !World->IsGameWorld() || World != Call.StaticWorld.Get() ||
            P->GetObjectPropertyValue_InContainer(Call.Frame->GetStructMemory()) != Self)
            Context->throw_error("WorldContext expired or changed before invocation");
    }
    auto WholeInput = [&](const FString& Path)
    {
        for (const auto& Root : Call.WholeInputs)
            if (Path == Root || Path.StartsWith(Root + TEXT(".")))
                return true;
        return false;
    };
    for (const auto& Parameter : Call.Signature)
        if (Parameter.bInput && !WholeInput(Parameter.Path) &&
            !Parameter.Path.Contains(TEXT(".*")) && !Call.Assigned.Contains(Parameter.Path))
            Context->throw_error("missing input argument or struct field");
    int32 InputTraversalBudget = 1048576;
    for (const auto& Parameter : Call.Signature)
        if (Parameter.bInput && !WholeInput(Parameter.Path) && Parameter.Path.Contains(TEXT(".*")))
            VisitCallContainerPaths(
                Call, Parameter.Path, false, Context,
                [&](const FString& Concrete)
                {
                    if (!Call.Assigned.Contains(Concrete))
                        Context->throw_error(
                            "missing array element or struct field; missing map key/value or set element");
                },
                InputTraversalBudget);
    SetCallConstructionPolicy(Call, Context, true);
    ValidateConstraintCallInputs(Call, Context);
    ValidateInputEventCallInputs(Call, Context);
    ValidateGeometryCall(Call, Context, false);
    ValidateWidgetAnimationCall(Call, Context);
    CommitPendingHashes(Call, Context);
    for (const auto& Pin : Call.ObjectPins)
        if (UObject* Argument = Pin.Value.Get();
            Argument &&
            (!IsValid(Argument) || (Cast<AActor>(Argument) && CastChecked<AActor>(Argument)->IsActorBeingDestroyed())))
            Context->throw_error("object argument expired before invocation");
    TStrongObjectPtr<UObject> KeepAlive(Object);
    Call.bExecuted = true;
    bool bComplete = false;
    ON_SCOPE_EXIT
    {
        if (!bComplete)
            Call.bFailed = true;
    };
    FString DispatchFailure;
    if (Call.bAuthored)
    {
        if (AuthoredDispatchDepth >= 32)
            Context->throw_error("authored method recursion limit exceeded");
        TGuardValue<int32> Depth(AuthoredDispatchDepth, AuthoredDispatchDepth + 1);
        TGuardValue<FString*> Failure(AuthoredDispatchFailure, &DispatchFailure);
        if (Call.DelegateInvoke)
            Call.DelegateInvoke(Call.Frame->GetStructMemory());
        else
            Object->ProcessEvent(Call.Function.Get(), Call.Frame->GetStructMemory());
    }
    else if (!DispatchImageSoftBrush(Call, Context) && !DispatchInputNavigationCall(Call, Context) &&
             !StagePaintCall(Call, Context) && !DispatchEventReplyCall(Call, Context) &&
             !DispatchCollisionProfileProperty(Call, Context))
        Object->ProcessEvent(Call.Function.Get(), Call.Frame->GetStructMemory());
    if (!DispatchFailure.IsEmpty())
        Context->throw_error(TCHAR_TO_UTF8(*DispatchFailure));
    CaptureConstraintCallOutput(Call, Context);
    CaptureWidgetAnimationOutput(Call, Context);
    ValidateGeometryCall(Call, Context, true);
    int32 OutputTraversalBudget = 1048576;
    for (const auto& Parameter : Call.Signature)
        if (Parameter.bOutput && Parameter.Kind == EMarshallingKind::Array)
            VisitCallContainerPaths(
                Call, Parameter.Path, true, Context, [&](const FString& Concrete)
                { GetCallArraySize(Handle, TCHAR_TO_UTF8(*Concrete), Context); }, OutputTraversalBudget);
    for (const auto& Parameter : Call.Signature)
        if (Parameter.bOutput && (Parameter.Kind == EMarshallingKind::Map || Parameter.Kind == EMarshallingKind::Set))
            VisitCallContainerPaths(
                Call, Parameter.Path, true, Context, [&](const FString& Concrete)
                { GetCallHashSize(Handle, TCHAR_TO_UTF8(*Concrete), Context); }, OutputTraversalBudget);
    // Hold every new object output before control returns to the VM (where scripts may collect GC).
    // Release old inout-array pins only after replacement pins are established.
    TMap<FString, TStrongObjectPtr<UObject>> OutputPins;
    for (const auto& Parameter : Call.Signature)
        if ((Parameter.Kind == EMarshallingKind::Object || Parameter.Kind == EMarshallingKind::Class) &&
            Parameter.bOutput)
        {
            auto Pin = [&](const FString& Path)
            {
                auto Field = ResolveCallField(Call, TCHAR_TO_UTF8(*Path), true, Context);
                if (CastField<FWeakObjectProperty>(Field.Property))
                    return;
                const auto Native = ReadCallValue(Field, Context);
                UObject* Output = CastField<FClassProperty>(Field.Property)
                                      ? ValidateCallClass(CastFieldChecked<FClassProperty>(Field.Property),
                                                          Native.ClassValue.Get(), Context)
                                      : Native.ObjectValue.Get();
                OutputPins.Add(Path, TStrongObjectPtr<UObject>(IsValid(Output) ? Output : nullptr));
            };
            VisitCallContainerPaths(Call, Parameter.Path, true, Context, Pin, OutputTraversalBudget);
        }
    for (const auto& Parameter : Call.Signature)
        if (Parameter.Kind == EMarshallingKind::Array && Parameter.bOutput)
        {
            const FString Prefix = Parameter.Path + TEXT(".");
            for (auto It = Call.ObjectPins.CreateIterator(); It; ++It)
                if (It.Key().StartsWith(Prefix))
                    It.RemoveCurrent();
        }
    for (auto& Pin : OutputPins)
        Call.ObjectPins.Add(Pin.Key, MoveTemp(Pin.Value));
    bComplete = true;
}
template <class P> auto GetCallScalar(const FDasCallHandle& Handle, const char* Path, das::Context* Context)
{
    auto& Call = ResolveCall(Handle, Context);
    auto Field = ResolveCallField(Call, Path, true, Context);
    auto* Property = CastField<P>(Field.Property);
    if (!Property)
        Context->throw_error("result scalar type mismatch");
    auto Value = ReadCallValue(Field, Context);
    if (Value.Kind != int32(TPropertyScalarSlot<P>::Kind))
        Context->throw_error("result scalar type mismatch");
    return Value.*TPropertyScalarSlot<P>::Member;
}
int32 GetCallInt(const FDasCallHandle& H, const char* P, das::Context* C)
{
    return GetCallScalar<FIntProperty>(H, P, C);
}
float GetCallFloat(const FDasCallHandle& H, const char* P, das::Context* C)
{
    return GetCallScalar<FFloatProperty>(H, P, C);
}
double GetCallDouble(const FDasCallHandle& H, const char* P, das::Context* C)
{
    return GetCallScalar<FDoubleProperty>(H, P, C);
}
bool GetCallBool(const FDasCallHandle& H, const char* P, das::Context* C)
{
    return GetCallScalar<FBoolProperty>(H, P, C);
}
char* GetCallString(const FDasCallHandle& H, const char* P, das::Context* C)
{
    const FString Value = GetCallScalar<FStrProperty>(H, P, C);
    const FTCHARToUTF8 Utf8(*Value);
    return C->allocateString(Utf8.Get(), Utf8.Length(), nullptr);
}
char* GetCallName(const FDasCallHandle& Handle, const char* Path, das::Context* Context)
{
    const FString Name = GetCallScalar<FNameProperty>(Handle, Path, Context).ToString();
    const FTCHARToUTF8 Utf8(*Name);
    return Context->allocateString(Utf8.Get(), Utf8.Length(), nullptr);
}
void ReleaseCall(const FDasCallHandle& Handle, das::Context* Context)
{
    ResolveCall(Handle, Context);
    ActiveContext->Calls.Remove(Handle.Token);
}
struct FCallHandleAnnotation : das::ManagedStructureAnnotation<FDasCallHandle, false>
{
    explicit FCallHandleAnnotation(das::ModuleLibrary& Library)
        : ManagedStructureAnnotation("UCallHandle", Library, "FDasCallHandle")
    {
    }
    virtual bool isLocal() const override { return true; }
    virtual bool canCopy() const override { return true; }
    virtual bool canMove() const override { return true; }
    virtual bool hasNonTrivialCtor() const override { return false; }
    virtual bool isPod() const override { return true; }
    virtual bool canBePlacedInContainer() const override { return true; }
};

void SetCallByte(const FDasCallHandle& H, const char* Path, int32 Value, das::Context* C)
{
    if (Value < 0 || Value > 255)
        C->throw_error("byte input must be a raw uint8 in range 0..255");
    SetCallScalar<FByteProperty>(H, Path, uint8(Value), C);
}
int32 GetCallByte(const FDasCallHandle& H, const char* Path, das::Context* C)
{
    return GetCallScalar<FByteProperty>(H, Path, C);
}
