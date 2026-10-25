// Only resolved, bounded values enter the persisted default wire.
TSharedPtr<FJsonValue> DSLDefaultJsonValue(const das::TypeDeclPtr& T, das::ExpressionPtr E, das::ModuleGroup& Group,
                                           das::Module* Module)
{
    if (!E)
        DSLFail(TEXT("Missing container default value"));
    if (const auto* Adapted = DSLValueAdapter(T))
        if (auto Value = Adapted->Script->DefaultJson(T, E, Group, Module))
            return Value;
    if (DSLObjectKey(T))
    {
        if (!DSLKeyAnnotation(T)->AuthoredId.IsEmpty())
            DSLFail(TEXT("Container asset defaults require native reference types"));
        return DSLReferenceAssetDefault(E);
    }
    if (DSLContainer(T))
    {
        auto* Items = DSLDefaultItems(E, T);
        TArray<TSharedPtr<FJsonValue>> Values;
        if (Items)
            for (const auto& Item : Items->values)
            {
                if (T->baseType == das::Type::tTable && !DSLSet(T))
                {
                    if (Item->__rtti != das::string("ExprMakeTuple"))
                        DSLFail(TEXT("Map defaults require pairs"));
                    auto* Pair = static_cast<das::ExprMakeTuple*>(Item);
                    if (Pair->values.size() != 2)
                        DSLFail(TEXT("Map defaults require pairs"));
                    Values.Add(MakeShared<FJsonValueArray>(TArray<TSharedPtr<FJsonValue>>{
                        DSLDefaultJsonValue(T->firstType, Pair->values[0], Group, Module),
                        DSLDefaultJsonValue(T->secondType, Pair->values[1], Group, Module)}));
                }
                else
                    Values.Add(DSLDefaultJsonValue(T->firstType, Item, Group, Module));
            }
        return MakeShared<FJsonValueArray>(Values);
    }
    if (DSLDirectStruct(T))
    {
        auto* Call = E->rtti_isCall() ? static_cast<das::ExprCall*>(E) : nullptr;
        if (Call && Call->name == "ue_default_text" && Call->arguments.size() == 1 &&
            Call->arguments[0]->__rtti == das::string("ExprConstString"))
        {
            TSharedPtr<FJsonValue> Value;
            FString Error;
            if (!ParseDasDefaultJson(DSLText(static_cast<das::ExprConstString*>(Call->arguments[0])->text), Value,
                                     Error) ||
                Value->Type != EJson::Object)
                DSLFail(TEXT("Struct defaults require JSON fields: ")+ Error);
            return Value;
        }
        if (E->rtti_isMakeStruct())
        {
            auto* S = static_cast<das::ExprMakeStruct*>(E);
            if (S->structs.empty() && !S->block)
                return MakeShared<FJsonValueObject>(MakeShared<FJsonObject>());
        }
        DSLFail(TEXT("Struct defaults require ue_default_text or an empty constructor"));
    }
    if (!DSLMathPath(T).IsEmpty())
    {
        // Constructors are source syntax. Lower their native math fields into the
        // same recursive object representation as any other reflected struct.
        das::Structure::FieldDeclaration F;
        F.type = T;
        F.init = E;
        FInstancedStruct Value;
        FString Error;
        if (!ParseDasStructDefault(DSLMathPath(T), DSLMathPropertyDefault(F), Value, Error))
            DSLFail(Error);
        TFunction<TSharedPtr<FJsonValue>(const UScriptStruct*, const void*)> Struct;
        Struct = [&](const UScriptStruct* Type, const void* Data)
        {
            auto Object = MakeShared<FJsonObject>();
            for (TFieldIterator<FProperty> It(Type); It; ++It)
            {
                auto* P = *It;
                const auto* Field = P->ContainerPtrToValuePtr<void>(Data);
                if (auto* Nested = CastField<FStructProperty>(P))
                    Object->SetField(P->GetName(), Struct(Nested->Struct, Field));
                else if (auto* Number = CastField<FNumericProperty>(P))
                    Object->SetNumberField(P->GetName(), Number->IsInteger()
                                                             ? double(Number->GetSignedIntPropertyValue(Field))
                                                             : Number->GetFloatingPointPropertyValue(Field));
                else
                    DSLFail(TEXT("Unsupported math source field"));
            }
            return MakeShared<FJsonValueObject>(Object);
        };
        return Struct(Value.GetScriptStruct(), Value.GetMemory());
    }
    const auto K = DSLTypeName(T);
    if (K == TEXT("byte"))
    {
        const auto D = DSLByteDefault(E);
        return MakeShared<FJsonValueNumber>(D.Number);
    }
    if (K == TEXT("string") && E->__rtti == das::string("ExprConstString"))
        return MakeShared<FJsonValueString>(DSLText(static_cast<das::ExprConstString*>(E)->text));
    if (K == TEXT("bool") && E->__rtti == das::string("ExprConstBool"))
        return MakeShared<FJsonValueBoolean>(static_cast<das::ExprConstBool*>(E)->getValue());
    if ((K == TEXT("int") && E->__rtti == das::string("ExprConstInt")) ||
         (K == TEXT("float") &&
                    (E->__rtti == das::string("ExprConstFloat") || E->__rtti == das::string("ExprConstInt"))) ||
          (K == TEXT("double") &&
                     (E->__rtti == das::string("ExprConstDouble") || E->__rtti == das::string("ExprConstInt"))))
        return MakeShared<FJsonValueNumber>(DSLDefaultNumber(E));
    DSLFail(TEXT("Container default value type is unsupported or differs from its declaration: ")+ K);
    return nullptr;
}
FString DSLContainerDefault(das::Structure::FieldDeclaration& F, const FString& Path, das::ModuleGroup& Group,
                            das::Module* Module)
{
    if (!F.init)
        return {};
    auto Value = DSLDefaultJsonValue(F.type, F.init, Group, Module);
    const auto& Values = Value->AsArray();
    FString Wire;
    auto Writer = TJsonWriterFactory<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>::Create(&Wire);
    FJsonSerializer::Serialize(Values, Writer);
    FInstancedStruct Parsed;
    FString Error;
    if (DSLComputedDefaults && !DSLComputedDefaults->Resolved && !DSLComputedDefaults->Pending.IsEmpty())
    {
        F.init = nullptr;
        return TEXT("[]");
    }
    if (!ParseDasStructDefault(Path, Wire, Parsed, Error))
        DSLFail(Error);
    F.init = nullptr;
    return Wire;
}
