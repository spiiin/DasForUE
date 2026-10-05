// Included inside the runtime namespace; frames never expose raw UE memory to scripts.
    struct FCallField
    {
        FProperty* Property;
        void* Memory;
        FString Path;
    };

    UEnum* CallEnumType(FProperty* Property)
    {
        if (auto* Enum = CastField<FEnumProperty>(Property)) return Enum->GetEnum();
        if (auto* Byte = CastField<FByteProperty>(Property)) return Byte->Enum;
        return nullptr;
    }
    FNumericProperty* CallEnumStorage(FProperty* Property)
    {
        if (auto* Enum = CastField<FEnumProperty>(Property)) return Enum->GetUnderlyingProperty();
        return CastField<FByteProperty>(Property);
    }

    EMarshallingKind FieldKind(FProperty* Property, das::Context* Context)
    {
        if (Property->ArrayDim != 1) Context->throw_error("fixed arrays are not supported by call marshalling");
        if (CastField<FMapProperty>(Property)) return EMarshallingKind::Map;
        if (CastField<FSetProperty>(Property)) return EMarshallingKind::Set;
        if (CastField<FArrayProperty>(Property)) return EMarshallingKind::Array;
        if (CallEnumType(Property)) return EMarshallingKind::Enum;
        if (CastField<FNameProperty>(Property)) return EMarshallingKind::Name;
        if (CastField<FIntProperty>(Property)) return EMarshallingKind::Int;
        if (CastField<FFloatProperty>(Property)) return EMarshallingKind::Float;
        if (CastField<FDoubleProperty>(Property)) return EMarshallingKind::Double;
        if (CastField<FBoolProperty>(Property)) return EMarshallingKind::Bool;
        if (CastField<FStrProperty>(Property)) return EMarshallingKind::String;
        if (CastField<FObjectProperty>(Property) && !CastField<FClassProperty>(Property)) return EMarshallingKind::Object;
        if (CastField<FStructProperty>(Property)) return EMarshallingKind::Struct;
        Context->throw_error("unsupported UFunction or struct field type");
    }

    bool IsHashScalar(FProperty* Property, das::Context* Context)
    {
        const auto Kind = FieldKind(Property, Context);
        return Kind == EMarshallingKind::Int || Kind == EMarshallingKind::Float || Kind == EMarshallingKind::Double || Kind == EMarshallingKind::Bool || Kind == EMarshallingKind::String || Kind == EMarshallingKind::Name || Kind == EMarshallingKind::Enum;
    }

    // Recursively validate the entire signature, including outputs, before allocating a frame.
    void ValidateCallField(FProperty* Property, const FString& Path, bool bInput, bool bOutput, TArray<FMarshalledParameter>& Signature, das::Context* Context, int32 Depth = 0)
    {
        if (auto* Map = CastField<FMapProperty>(Property))
        {
            if (Depth != 0 || Path.Contains(TEXT(".*")) || Property->ArrayDim != 1) Context->throw_error("nested hash containers require an adapter");
            if (!IsHashScalar(Map->KeyProp, Context) || !IsHashScalar(Map->ValueProp, Context)) Context->throw_error("unsupported map key or value type");
            if (!Map->KeyProp->HasAllPropertyFlags(CPF_HasGetValueTypeHash)) Context->throw_error("map key requires UE value hashing");
            Signature.Add({Path.ToLower(), EMarshallingKind::Map, bInput, bOutput, Map->KeyProp->GetClass()->GetName() + TEXT(";") + Map->ValueProp->GetClass()->GetName()});
            ValidateCallField(Map->KeyProp, Path + TEXT(".*.key"), bInput, bOutput, Signature, Context);
            ValidateCallField(Map->ValueProp, Path + TEXT(".*.value"), bInput, bOutput, Signature, Context);
            return;
        }
        if (auto* Set = CastField<FSetProperty>(Property))
        {
            if (Depth != 0 || Path.Contains(TEXT(".*")) || Property->ArrayDim != 1) Context->throw_error("nested hash containers require an adapter");
            if (!IsHashScalar(Set->ElementProp, Context)) Context->throw_error("unsupported set element type");
            if (!Set->ElementProp->HasAllPropertyFlags(CPF_HasGetValueTypeHash)) Context->throw_error("set element requires UE value hashing");
            Signature.Add({Path.ToLower(), EMarshallingKind::Set, bInput, bOutput, Set->ElementProp->GetClass()->GetName()});
            ValidateCallField(Set->ElementProp, Path + TEXT(".*"), bInput, bOutput, Signature, Context);
            return;
        }
        if (auto* Array = CastField<FArrayProperty>(Property))
        {
            if (Property->ArrayDim != 1) Context->throw_error("fixed arrays are not supported by call marshalling");
            if (Depth != 0) Context->throw_error("array fields inside structs are not supported yet");
            if (FieldKind(Array->Inner, Context) == EMarshallingKind::Array)
                Context->throw_error("nested arrays require a dedicated adapter");
            Signature.Add({Path.ToLower(), EMarshallingKind::Array, bInput, bOutput, Array->Inner->GetClass()->GetName()});
            // Wildcard leaves describe every element without storing UE property addresses.
            // Direct object elements are supported; object fields inside structs still require another adapter.
            ValidateCallField(Array->Inner, Path + TEXT(".*"), bInput, bOutput, Signature, Context);
            return;
        }
        if (Depth > 0 && FieldKind(Property, Context) == EMarshallingKind::Object) Context->throw_error("object fields inside structs require a GC-aware struct adapter");
        if (Depth > 16) Context->throw_error("call struct nesting limit exceeded");
        if (FieldKind(Property, Context) == EMarshallingKind::Struct)
        {
            UScriptStruct* Type = CastFieldChecked<FStructProperty>(Property)->Struct;
            // Native opaque state cannot be described by reflection alone.
            if (Type->GetCppStructOps() && Type->GetCppStructOps()->HasAddStructReferencedObjects())
                Context->throw_error("structs with native object references are unsupported");
            for (TFieldIterator<FProperty> It(Type); It; ++It) ValidateCallField(*It, Path + TEXT(".") + It->GetName(), bInput, bOutput, Signature, Context, Depth + 1);
        }
        else Signature.Add({Path.ToLower(), FieldKind(Property, Context), bInput, bOutput, CallEnumType(Property) ? CallEnumType(Property)->GetPathName() : (CastField<FObjectProperty>(Property) ? CastFieldChecked<FObjectProperty>(Property)->PropertyClass->GetPathName() : FString())});
    }

    TArray<FMarshalledParameter> DescribeCall(UFunction* Function, das::Context* Context)
    {
        TArray<FMarshalledParameter> Signature;
        for (TFieldIterator<FProperty> It(Function); It; ++It)
        {
            if (!It->HasAnyPropertyFlags(CPF_Parm)) continue;
            const bool bReturn = It->HasAnyPropertyFlags(CPF_ReturnParm);
            const bool bMutableOut = !It->HasAnyPropertyFlags(CPF_ConstParm) && It->HasAnyPropertyFlags(CPF_OutParm | CPF_ReferenceParm);
            const bool bInput = !bReturn && (!bMutableOut || It->HasAnyPropertyFlags(CPF_ReferenceParm));
            ValidateCallField(*It, It->GetName(), bInput, bReturn || bMutableOut, Signature, Context);
        }
        return Signature;
    }
    const TCHAR* CallKindName(EMarshallingKind Kind)
    {
        switch (Kind) {
        case EMarshallingKind::Int: return TEXT("int");
        case EMarshallingKind::Float: return TEXT("float");
        case EMarshallingKind::Double: return TEXT("double");
        case EMarshallingKind::Bool: return TEXT("bool");
        case EMarshallingKind::String: return TEXT("string");
        case EMarshallingKind::Object: return TEXT("object");
        case EMarshallingKind::Name: return TEXT("name");
        case EMarshallingKind::Enum: return TEXT("enum");
        case EMarshallingKind::Array: return TEXT("array");
        case EMarshallingKind::Map: return TEXT("map");
        case EMarshallingKind::Set: return TEXT("set");
        default: return TEXT("struct"); }
    }
    void AppendCallShape(FProperty* Property, FString& Key)
    {
        Key += Property->GetName() + TEXT(":") + Property->GetClass()->GetName() + TEXT(":") +
            FString::Printf(TEXT("%llu"), uint64(Property->GetPropertyFlags() & (CPF_Parm | CPF_ReturnParm | CPF_OutParm | CPF_ReferenceParm | CPF_ConstParm)));
        if (UEnum* Enum = CallEnumType(Property)) Key += TEXT(":") + Enum->GetPathName();
        if (auto* Object = CastField<FObjectProperty>(Property)) Key += TEXT(":") + Object->PropertyClass->GetPathName();
        if (auto* Array = CastField<FArrayProperty>(Property))
        {
            Key += TEXT("["); AppendCallShape(Array->Inner, Key); Key += TEXT("]");
        }
        if (auto* Map = CastField<FMapProperty>(Property))
        { Key += TEXT("["); AppendCallShape(Map->KeyProp, Key); AppendCallShape(Map->ValueProp, Key); Key += TEXT("]"); }
        if (auto* Set = CastField<FSetProperty>(Property))
        { Key += TEXT("["); AppendCallShape(Set->ElementProp, Key); Key += TEXT("]"); }
        if (auto* Struct = CastField<FStructProperty>(Property))
        {
            Key += TEXT(":") + Struct->Struct->GetPathName() + TEXT("{");
            for (TFieldIterator<FProperty> It(Struct->Struct); It; ++It) AppendCallShape(*It, Key);
            Key += TEXT("}");
        }
        Key += TEXT(";");
    }
    FString CallSignatureKey(UFunction* Function)
    {
        FString Key = TEXT("v1;") + Function->GetName() + TEXT(";");
        for (TFieldIterator<FProperty> It(Function); It; ++It)
            if (It->HasAnyPropertyFlags(CPF_Parm)) AppendCallShape(*It, Key);
        return Key;
    }

    FDasCallHandle BeginCall(const FDasObjectHandle& Target, const char* Name, das::Context* Context)
    {
        UObject* Object = ResolveObject(Target, Context);
        if (ActiveContext->bCollectingSubscriptions) Context->throw_error("UFunction calls are forbidden in bind_events");
        if (!Name || !*Name || FCStringAnsi::Strlen(Name) >= NAME_SIZE) Context->throw_error("invalid function name");
        UFunction* Function = Object->FindFunction(FName(UTF8_TO_TCHAR(Name)));
        if (!Function || !Function->HasAnyFunctionFlags(FUNC_BlueprintCallable) ||
            Function->HasAnyFunctionFlags(FUNC_Static | FUNC_Net | FUNC_Delegate | FUNC_MulticastDelegate))
            Context->throw_error("expected a local BlueprintCallable instance UFunction");
        auto Signature = DescribeCall(Function, Context);
        if (ActiveContext->Calls.Num() >= 256) Context->throw_error("call frame limit reached; release unused calls");
        auto Call = MakeShared<FMarshalledCall>();
        Call->Signature = MoveTemp(Signature);
        Call->SignatureKey = CallSignatureKey(Function);
        Call->Target = Object;
        Call->Function.Reset(Function);
        Call->Frame = MakeUnique<FStructOnScope>(Function);
        const uint64 Token = NextCallHandle++;
        ActiveContext->Calls.Add(Token, MoveTemp(Call));
        return {Token};
    }

    FMarshalledCall& ResolveCall(const FDasCallHandle& Handle, das::Context* Context)
    {
        RequireObjectContext(Context);
        auto* Found = ActiveContext->Calls.Find(Handle.Token);
        if (!Found) Context->throw_error("call handle is released or belongs to another context");
        return **Found;
    }

    void ExpectCallSignature(const FDasCallHandle& Handle, const char* Expected, das::Context* Context)
    {
        auto& Call = ResolveCall(Handle, Context);
        if (Call.bExecuted || !Expected || Call.SignatureKey != UTF8_TO_TCHAR(Expected))
            Context->throw_error("generated binding signature changed; regenerate bindings");
    }

    FCallField ResolveCallField(FMarshalledCall& Call, const char* Path, bool bOutput, das::Context* Context)
    {
        if (Call.bFailed) Context->throw_error("call failed; results are unavailable");
        if (Call.bExecuted != bOutput) Context->throw_error("arguments require a pending call; results require an executed call");
        if (!Path || !*Path) Context->throw_error("parameter path is empty");
        TArray<FString> Parts;
        FString(UTF8_TO_TCHAR(Path)).ParseIntoArray(Parts, TEXT("."), false);
        UStruct* Type = Call.Function.Get();
        void* Memory = Call.Frame->GetStructMemory();
        FString Canonical, DescriptionPath;
        for (int32 Index = 0; Index < Parts.Num(); ++Index)
        {
            if (Parts[Index].IsEmpty() || Parts[Index].Len() >= NAME_SIZE) Context->throw_error("invalid parameter path");
            FProperty* Property = FindFProperty<FProperty>(Type, *Parts[Index]);
            if (!Property) Context->throw_error("unknown parameter or wrong parameter direction");
            const FString Name = Property->GetName().ToLower();
            Canonical += (Canonical.IsEmpty() ? TEXT("") : TEXT(".")) + Name;
            DescriptionPath += (DescriptionPath.IsEmpty() ? TEXT("") : TEXT(".")) + Name;
            Memory = Property->ContainerPtrToValuePtr<void>(Memory);
            if (auto* Array = CastField<FArrayProperty>(Property); Array && Index + 1 < Parts.Num())
            {
                const auto* Root = Call.Signature.FindByPredicate([&](const auto& Item) { return Item.Path == DescriptionPath; });
                if (!Root || !(bOutput ? Root->bOutput : Root->bInput)) Context->throw_error("wrong array parameter direction");
                if (!bOutput && !Call.Assigned.Contains(Canonical)) Context->throw_error("initialize array before assigning elements");
                const FString& Number = Parts[++Index];
                if (Number.IsEmpty() || Number.Len() > 10) Context->throw_error("invalid array index");
                for (TCHAR Ch : Number) if (Ch < TEXT('0') || Ch > TEXT('9')) Context->throw_error("invalid array index");
                int64 Parsed = -1;
                if (!LexTryParseString(Parsed, *Number) || Parsed < 0 || Parsed > MAX_int32) Context->throw_error("invalid array index");
                FScriptArrayHelper Helper(Array, Memory);
                const int32 Element = static_cast<int32>(Parsed);
                if (Element >= Helper.Num()) Context->throw_error("array index out of bounds");
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
                const auto* Direction = Call.Signature.FindByPredicate([&](const auto& Item) { return Item.Path == Root; });
                if (!Direction || !(bOutput ? Direction->bOutput : Direction->bInput)) Context->throw_error("wrong hash container direction");
                const FString& Number = Parts[++Index];
                int64 Parsed = -1;
                if (Number.IsEmpty() || Number.Len() > 10) Context->throw_error("invalid hash index");
                for (TCHAR Ch : Number) if (Ch < TEXT('0') || Ch > TEXT('9')) Context->throw_error("invalid hash index");
                if (!LexTryParseString(Parsed, *Number) || Parsed < 0 || Parsed > MAX_int32) Context->throw_error("invalid hash index");
                bool bKey = true;
                FString Suffix;
                if (Map)
                {
                    if (++Index >= Parts.Num()) Context->throw_error("map slot requires key or value");
                    Suffix = Parts[Index].ToLower();
                    if (Suffix != TEXT("key") && Suffix != TEXT("value")) Context->throw_error("map slot requires key or value");
                    bKey = Suffix == TEXT("key");
                    Suffix = TEXT(".") + Suffix;
                }
                if (Index != Parts.Num() - 1) Context->throw_error("nested hash values are unsupported");
                const int32 Slot = static_cast<int32>(Parsed);
                Property = Map ? (bKey ? Map->KeyProp : Map->ValueProp) : Set->ElementProp;
                if (!bOutput)
                {
                    auto* Pending = Call.PendingHashes.Find(Root);
                    if (!Pending) Context->throw_error("initialize hash container before assigning slots");
                    if (Slot >= (*Pending)->Keys.Num()) Context->throw_error("hash index out of bounds");
                    Memory = (bKey ? (*Pending)->Keys[Slot] : (*Pending)->Values[Slot])->Memory;
                }
                else
                {
                    const auto* Indices = Call.HashIndices.Find(Root);
                    if (!Indices || Slot >= Indices->Num()) Context->throw_error("hash index out of bounds");
                    const int32 Internal = (*Indices)[Slot];
                    if (Map) { FScriptMapHelper Helper(Map, Memory); Memory = bKey ? Helper.GetKeyPtr(Internal) : Helper.GetValuePtr(Internal); }
                    else { FScriptSetHelper Helper(Set, Memory); Memory = Helper.GetElementPtr(Internal); }
                }
                Canonical += TEXT(".") + FString::FromInt(Slot) + Suffix;
                DescriptionPath += TEXT(".*") + Suffix;
            }
            if (Index == Parts.Num() - 1)
            {
                const auto* Description = Call.Signature.FindByPredicate([&](const auto& Item) { return Item.Path == DescriptionPath; });
                if (!Description || !(bOutput ? Description->bOutput : Description->bInput)) Context->throw_error("unknown parameter or wrong parameter direction");
                return {Property, Memory, Canonical};
            }
            auto* Struct = CastField<FStructProperty>(Property);
            if (!Struct) Context->throw_error("parameter path traverses a non-struct value");
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
        if (!Property) Context->throw_error("expected an array argument");
        if (Count < 0 || Count > MaxCallArrayElements) Context->throw_error("array size limit exceeded");
        FScriptArrayHelper Helper(Property, Field.Memory);
        Helper.EmptyValues();
        Helper.AddValues(Count);
        const FString Prefix = Field.Path + TEXT(".");
        for (auto It = Call.Assigned.CreateIterator(); It; ++It) if (It->StartsWith(Prefix)) It.RemoveCurrent();
        for (auto It = Call.ObjectPins.CreateIterator(); It; ++It) if (It.Key().StartsWith(Prefix)) It.RemoveCurrent();
        Call.Assigned.Add(Field.Path);
    }
    int32 GetCallArraySize(const FDasCallHandle& Handle, const char* Path, das::Context* Context)
    {
        auto& Call = ResolveCall(Handle, Context);
        auto Field = ResolveCallField(Call, Path, true, Context);
        auto* Property = CastField<FArrayProperty>(Field.Property);
        if (!Property) Context->throw_error("expected an array result");
        FScriptArrayHelper Helper(Property, Field.Memory);
        if (Helper.Num() > MaxCallArrayElements) Context->throw_error("array size limit exceeded");
        return Helper.Num();
    }

    void ValidateHashNumber(FProperty* Property, void* Memory, bool bNormalizeZero, das::Context* Context)
    {
        if (auto* Float = CastField<FFloatProperty>(Property))
        {
            const float Value = Float->GetPropertyValue(Memory);
            if (!FMath::IsFinite(Value)) Context->throw_error("hash key or set element must be finite");
            if (bNormalizeZero && Value == 0) Float->SetPropertyValue(Memory, 0.f);
        }
        if (auto* Double = CastField<FDoubleProperty>(Property))
        {
            const double Value = Double->GetPropertyValue(Memory);
            if (!FMath::IsFinite(Value)) Context->throw_error("hash key or set element must be finite");
            if (bNormalizeZero && Value == 0) Double->SetPropertyValue(Memory, 0.0);
        }
    }
    void PrepareCallHash(const FDasCallHandle& Handle, const char* Path, int32 Count, das::Context* Context)
    {
        auto& Call = ResolveCall(Handle, Context);
        auto Field = ResolveCallField(Call, Path, false, Context);
        auto* Map = CastField<FMapProperty>(Field.Property);
        auto* Set = CastField<FSetProperty>(Field.Property);
        if (!Map && !Set) Context->throw_error("expected a map or set argument");
        if (Count < 0 || Count > MaxCallArrayElements) Context->throw_error("hash container size limit exceeded");
        auto Pending = MakeShared<FPendingHash>();
        Pending->Keys.Reserve(Count); Pending->Values.Reserve(Map ? Count : 0);
        for (int32 Index = 0; Index < Count; ++Index)
        {
            Pending->Keys.Add(MakeUnique<FHashValue>(Map ? Map->KeyProp : Set->ElementProp));
            if (Map) Pending->Values.Add(MakeUnique<FHashValue>(Map->ValueProp));
        }
        Call.PendingHashes.Add(Field.Path, MoveTemp(Pending));
        const FString Prefix = Field.Path + TEXT(".");
        for (auto It = Call.Assigned.CreateIterator(); It; ++It) if (It->StartsWith(Prefix)) It.RemoveCurrent();
        Call.Assigned.Add(Field.Path);
    }
    int32 GetCallHashSize(const FDasCallHandle& Handle, const char* Path, das::Context* Context)
    {
        auto& Call = ResolveCall(Handle, Context);
        auto Field = ResolveCallField(Call, Path, true, Context);
        auto* Map = CastField<FMapProperty>(Field.Property);
        auto* Set = CastField<FSetProperty>(Field.Property);
        if (!Map && !Set) Context->throw_error("expected a map or set result");
        auto& Indices = Call.HashIndices.FindOrAdd(Field.Path); Indices.Reset();
        if (Map)
        {
            FScriptMapHelper Helper(Map, Field.Memory);
            if (Helper.Num() > MaxCallArrayElements) Context->throw_error("hash container size limit exceeded");
            for (int32 Index = 0; Index < Helper.GetMaxIndex(); ++Index) if (Helper.IsValidIndex(Index)) { ValidateHashNumber(Map->KeyProp, Helper.GetKeyPtr(Index), false, Context); Indices.Add(Index); }
        }
        else
        {
            FScriptSetHelper Helper(Set, Field.Memory);
            if (Helper.Num() > MaxCallArrayElements) Context->throw_error("hash container size limit exceeded");
            for (int32 Index = 0; Index < Helper.GetMaxIndex(); ++Index) if (Helper.IsValidIndex(Index)) { ValidateHashNumber(Set->ElementProp, Helper.GetElementPtr(Index), false, Context); Indices.Add(Index); }
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
                if (!Call.Assigned.Contains(Slot + (Map ? TEXT(".key") : TEXT(""))) || (Map && !Call.Assigned.Contains(Slot + TEXT(".value")))) Context->throw_error("missing map key/value or set element");
                const auto& Key = *Pending.Keys[Index];
                ValidateHashNumber(Key.Property, Key.Memory, true, Context);
                const uint32 Hash = Key.Property->GetValueTypeHash(Key.Memory);
                for (auto It = Seen.CreateConstKeyIterator(Hash); It; ++It)
                    if (Key.Property->Identical(Key.Memory, It.Value())) Context->throw_error("duplicate map key or set element");
                Seen.Add(Hash, Key.Memory);
            }
            if (Map)
            {
                FScriptMapHelper Helper(Map, Field.Memory); Helper.EmptyValues(Pending.Keys.Num());
                for (int32 Index = 0; Index < Pending.Keys.Num(); ++Index) Helper.AddPair(Pending.Keys[Index]->Memory, Pending.Values[Index]->Memory);
            }
            else
            {
                FScriptSetHelper Helper(Set, Field.Memory); Helper.EmptyElements(Pending.Keys.Num());
                for (const auto& Key : Pending.Keys) Helper.AddElement(Key->Memory);
            }
        }
    }

    template<class P, class V> void SetCallScalar(const FDasCallHandle& Handle, const char* Path, V Value, das::Context* Context)
    {
        auto& Call = ResolveCall(Handle, Context);
        auto Field = ResolveCallField(Call, Path, false, Context);
        auto* Property = CastField<P>(Field.Property);
        if (!Property) Context->throw_error("argument scalar type mismatch");
        Property->SetPropertyValue(Field.Memory, Value);
        Call.Assigned.Add(Field.Path);
    }
    void SetCallInt(const FDasCallHandle& H, const char* P, int32 V, das::Context* C) { SetCallScalar<FIntProperty>(H, P, V, C); }
    void SetCallFloat(const FDasCallHandle& H, const char* P, float V, das::Context* C) { SetCallScalar<FFloatProperty>(H, P, V, C); }
    void SetCallDouble(const FDasCallHandle& H, const char* P, double V, das::Context* C) { SetCallScalar<FDoubleProperty>(H, P, V, C); }
    void SetCallBool(const FDasCallHandle& H, const char* P, bool V, das::Context* C) { SetCallScalar<FBoolProperty>(H, P, V, C); }
    void SetCallString(const FDasCallHandle& H, const char* P, const char* V, das::Context* C)
    { SetCallScalar<FStrProperty>(H, P, FString(V ? UTF8_TO_TCHAR(V) : TEXT("")), C); }

    void SetCallName(const FDasCallHandle& Handle, const char* Path, const char* Value, das::Context* Context)
    {
        const FString Text(Value ? UTF8_TO_TCHAR(Value) : TEXT(""));
        if (Text.Len() >= NAME_SIZE) Context->throw_error("FName value is too long");
        // Validate the field before adding a name to UE's global name table.
        auto& Call = ResolveCall(Handle, Context);
        auto Field = ResolveCallField(Call, Path, false, Context);
        auto* Property = CastField<FNameProperty>(Field.Property);
        if (!Property) Context->throw_error("argument FName type mismatch");
        Property->SetPropertyValue(Field.Memory, FName(*Text));
        Call.Assigned.Add(Field.Path);
    }
    UEnum* RequireCallEnum(FProperty* Property, const char* TypePath, das::Context* Context)
    {
        UEnum* Enum = CallEnumType(Property);
        if (!Enum) Context->throw_error("expected an enum parameter");
        if (!TypePath || Enum->GetPathName() != UTF8_TO_TCHAR(TypePath)) Context->throw_error("enum type path mismatch");
        return Enum;
    }
    bool IsCallEnumMember(UEnum* Enum, int32 Index)
    {
        // Stable in cooked builds: never depend on editor-only Hidden metadata.
        return Index != INDEX_NONE && !Enum->GetNameStringByIndex(Index).EndsWith(TEXT("_MAX"));
    }
    void SetCallEnum(const FDasCallHandle& Handle, const char* Path, const char* TypePath, const char* Member, das::Context* Context)
    {
        auto& Call = ResolveCall(Handle, Context);
        auto Field = ResolveCallField(Call, Path, false, Context);
        UEnum* Enum = RequireCallEnum(Field.Property, TypePath, Context);
        const FString Symbol(Member ? UTF8_TO_TCHAR(Member) : TEXT(""));
        if (Symbol.Len() >= NAME_SIZE) Context->throw_error("enum member is too long");
        int32 Index = INDEX_NONE;
        // Compare registered symbols directly: never intern unchecked input, use display names or redirects.
        for (int32 Candidate = 0; Candidate < Enum->NumEnums(); ++Candidate)
            if (Symbol.Equals(Enum->GetNameStringByIndex(Candidate), ESearchCase::IgnoreCase) ||
                Symbol.Equals(Enum->GetNameByIndex(Candidate).ToString(), ESearchCase::IgnoreCase))
            { Index = Candidate; break; }
        if (!IsCallEnumMember(Enum, Index)) Context->throw_error("unknown or reserved enum member");
        CallEnumStorage(Field.Property)->SetIntPropertyValue(Field.Memory, Enum->GetValueByIndex(Index));
        Call.Assigned.Add(Field.Path);
    }
    char* GetCallEnum(const FDasCallHandle& Handle, const char* Path, const char* TypePath, das::Context* Context)
    {
        auto& Call = ResolveCall(Handle, Context);
        auto Field = ResolveCallField(Call, Path, true, Context);
        UEnum* Enum = RequireCallEnum(Field.Property, TypePath, Context);
        const int64 Value = CallEnumStorage(Field.Property)->GetSignedIntPropertyValue(Field.Memory);
        const int32 Index = Enum->GetIndexByValue(Value);
        if (!IsCallEnumMember(Enum, Index)) Context->throw_error("UFunction returned an unknown or reserved enum value");
        const FString Name = Enum->GetNameStringByIndex(Index);
        const FTCHARToUTF8 Utf8(*Name);
        return Context->allocateString(Utf8.Get(), Utf8.Length(), nullptr);
    }

    void SetCallObject(const FDasCallHandle& Handle, const char* Path, const FDasObjectHandle& Value, das::Context* Context)
    {
        auto& Call = ResolveCall(Handle, Context);
        auto Field = ResolveCallField(Call, Path, false, Context);
        auto* Property = CastField<FObjectProperty>(Field.Property);
        if (!Property || CastField<FClassProperty>(Property)) Context->throw_error("argument object type mismatch");
        UObject* Object = Value.Token ? ResolveObject(Value, Context) : nullptr;
        if (Object && !Object->IsA(Property->PropertyClass)) Context->throw_error("object argument has an incompatible class");
        Call.ObjectPins.Add(Field.Path, TStrongObjectPtr<UObject>(Object));
        Property->SetObjectPropertyValue(Field.Memory, Object);
        Call.Assigned.Add(Field.Path);
    }
    FDasObjectHandle GetCallObject(const FDasCallHandle& Handle, const char* Path, das::Context* Context)
    {
        auto& Call = ResolveCall(Handle, Context);
        auto Field = ResolveCallField(Call, Path, true, Context);
        auto* Property = CastField<FObjectProperty>(Field.Property);
        if (!Property || CastField<FClassProperty>(Property)) Context->throw_error("result object type mismatch");
        return MakeObjectHandle(Property->GetObjectPropertyValue(Field.Memory), Property->PropertyClass, Context);
    }

    void InvokeCall(const FDasCallHandle& Handle, das::Context* Context)
    {
        auto& Call = ResolveCall(Handle, Context);
        if (ActiveContext->bCollectingSubscriptions) Context->throw_error("UFunction calls are forbidden in bind_events");
        if (Call.bExecuted) Context->throw_error("a call frame may only execute once");
        UObject* Object = Call.Target.Get();
        if (!IsValid(Object) || (Cast<AActor>(Object) && CastChecked<AActor>(Object)->IsActorBeingDestroyed()))
            Context->throw_error("call target expired");
        if (Object->FindFunction(Call.Function->GetFName()) != Call.Function.Get()) Context->throw_error("UFunction changed; create a new call frame");
        for (const auto& Parameter : Call.Signature)
            if (Parameter.bInput && !Parameter.Path.Contains(TEXT(".*")) && !Call.Assigned.Contains(Parameter.Path))
                Context->throw_error("missing input argument or struct field");
        for (const auto& Parameter : Call.Signature)
            if (Parameter.bInput && Parameter.Kind == EMarshallingKind::Array)
            {
                auto Field = ResolveCallField(Call, TCHAR_TO_UTF8(*Parameter.Path), false, Context);
                FScriptArrayHelper Helper(CastFieldChecked<FArrayProperty>(Field.Property), Field.Memory);
                const FString Prefix = Parameter.Path + TEXT(".*");
                for (const auto& Leaf : Call.Signature)
                    if (Leaf.Path.StartsWith(Prefix))
                        for (int32 Index = 0; Index < Helper.Num(); ++Index)
                            if (!Call.Assigned.Contains(Leaf.Path.Replace(TEXT(".*"), *(TEXT(".") + FString::FromInt(Index)))))
                                Context->throw_error("missing array element or struct field");
            }
        CommitPendingHashes(Call, Context);
        for (const auto& Pin : Call.ObjectPins)
            if (UObject* Argument = Pin.Value.Get(); Argument && (!IsValid(Argument) || (Cast<AActor>(Argument) && CastChecked<AActor>(Argument)->IsActorBeingDestroyed())))
                Context->throw_error("object argument expired before invocation");
        TStrongObjectPtr<UObject> KeepAlive(Object);
        Call.bExecuted = true;
        bool bComplete = false;
        ON_SCOPE_EXIT { if (!bComplete) Call.bFailed = true; };
        Object->ProcessEvent(Call.Function.Get(), Call.Frame->GetStructMemory());
        for (const auto& Parameter : Call.Signature)
            if (Parameter.bOutput && Parameter.Kind == EMarshallingKind::Array)
                GetCallArraySize(Handle, TCHAR_TO_UTF8(*Parameter.Path), Context);
        for (const auto& Parameter : Call.Signature)
            if (Parameter.bOutput && (Parameter.Kind == EMarshallingKind::Map || Parameter.Kind == EMarshallingKind::Set))
                GetCallHashSize(Handle, TCHAR_TO_UTF8(*Parameter.Path), Context);
        // Hold every new object output before control returns to the VM (where scripts may collect GC).
        // Release old inout-array pins only after replacement pins are established.
        TMap<FString, TStrongObjectPtr<UObject>> OutputPins;
        for (const auto& Parameter : Call.Signature)
            if (Parameter.Kind == EMarshallingKind::Object && Parameter.bOutput)
            {
                auto Pin = [&](const FString& Path)
                {
                    auto Field = ResolveCallField(Call, TCHAR_TO_UTF8(*Path), true, Context);
                    UObject* Output = CastFieldChecked<FObjectProperty>(Field.Property)->GetObjectPropertyValue(Field.Memory);
                    OutputPins.Add(Path, TStrongObjectPtr<UObject>(IsValid(Output) ? Output : nullptr));
                };
                const int32 Wildcard = Parameter.Path.Find(TEXT(".*"));
                if (Wildcard == INDEX_NONE) Pin(Parameter.Path);
                else
                {
                    const FString Root = Parameter.Path.Left(Wildcard);
                    const int32 Count = GetCallArraySize(Handle, TCHAR_TO_UTF8(*Root), Context);
                    for (int32 Index = 0; Index < Count; ++Index)
                        Pin(Parameter.Path.Replace(TEXT(".*"), *(TEXT(".") + FString::FromInt(Index))));
                }
            }
        for (const auto& Parameter : Call.Signature)
            if (Parameter.Kind == EMarshallingKind::Array && Parameter.bOutput)
            {
                const FString Prefix = Parameter.Path + TEXT(".");
                for (auto It = Call.ObjectPins.CreateIterator(); It; ++It) if (It.Key().StartsWith(Prefix)) It.RemoveCurrent();
            }
        for (auto& Pin : OutputPins) Call.ObjectPins.Add(Pin.Key, MoveTemp(Pin.Value));
        bComplete = true;
    }
    template<class P> auto GetCallScalar(const FDasCallHandle& Handle, const char* Path, das::Context* Context)
    {
        auto& Call = ResolveCall(Handle, Context);
        auto Field = ResolveCallField(Call, Path, true, Context);
        auto* Property = CastField<P>(Field.Property);
        if (!Property) Context->throw_error("result scalar type mismatch");
        return Property->GetPropertyValue(Field.Memory);
    }
    int32 GetCallInt(const FDasCallHandle& H, const char* P, das::Context* C) { return GetCallScalar<FIntProperty>(H, P, C); }
    float GetCallFloat(const FDasCallHandle& H, const char* P, das::Context* C) { return GetCallScalar<FFloatProperty>(H, P, C); }
    double GetCallDouble(const FDasCallHandle& H, const char* P, das::Context* C) { return GetCallScalar<FDoubleProperty>(H, P, C); }
    bool GetCallBool(const FDasCallHandle& H, const char* P, das::Context* C) { return GetCallScalar<FBoolProperty>(H, P, C); }
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
            : ManagedStructureAnnotation("UCallHandle", Library, "FDasCallHandle") {}
        virtual bool isLocal() const override { return true; }
        virtual bool canCopy() const override { return true; }
        virtual bool canMove() const override { return true; }
        virtual bool hasNonTrivialCtor() const override { return false; }
        virtual bool isPod() const override { return true; }
        virtual bool canBePlacedInContainer() const override { return true; }
    };
