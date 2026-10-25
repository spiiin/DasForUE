// All enum source identity, conversion and default policy lives in this facet.
bool DSLNativeEnum(const das::TypeDeclPtr& T)
{
    return T && T->baseType == das::Type::tEnumeration && T->enumType &&
           DSLAnnotation(T->enumType->annotations, "ue_native_enum");
}
bool DSLDirectEnum(const das::TypeDeclPtr& T)
{
    return T && T->baseType == das::Type::tEnumeration && T->enumType &&
           (DSLAnnotation(T->enumType->annotations, "ue_enum") ||
            DSLAnnotation(T->enumType->annotations, "ue_native_enum"));
}
FString DSLStructEnumDefault(const das::Structure::FieldDeclaration& Field)
{
    FString Name;
    if (!Field.init)
        DSLFail(TEXT("Enum struct members require an explicit named default"));
    if (Field.init->__rtti == das::string("ExprConstEnumeration"))
    {
        auto* E = static_cast<das::ExprConstEnumeration*>(Field.init);
        if (E->enumType && E->enumType != Field.type->enumType)
            DSLFail(TEXT("Enum struct default has a different enum type"));
        Name = DSLText(E->text);
    }
    else if (Field.init->rtti_isField())
    {
        auto* E = static_cast<das::ExprField*>(Field.init);
        const auto Qualified = Field.type->enumType->module->name + "::" + Field.type->enumType->name;
        if (!E->value->rtti_isVar() || (static_cast<das::ExprVar*>(E->value)->name != Field.type->enumType->name &&
                                        static_cast<das::ExprVar*>(E->value)->name != Qualified))
            DSLFail(TEXT("Enum struct default must name its declared enum"));
        Name = DSLText(E->name);
    }
    else
        DSLFail(TEXT("Enum struct defaults require named enum literals"));
    if (Name.Contains(TEXT("::")))
        Name = Name.RightChop(Name.Find(TEXT("::"), ESearchCase::CaseSensitive, ESearchDir::FromEnd) + 2);
    if (!std::any_of(Field.type->enumType->list.begin(), Field.type->enumType->list.end(),
                     [&](const auto& V) { return DSLText(V.name) == Name; }))
        DSLFail(TEXT("Unknown enum struct default member"));
    return Name;
}

class FDasEnumScriptFacet final : public IDasScriptTypeFacet
{
public:
    bool MatchesScriptType(const das::TypeDeclPtr& T) const override { return DSLDirectEnum(T) || MatchesCallType(T); }
    bool MatchesCallType(const das::TypeDeclPtr& T) const override
    {
        return T && T->baseType == das::Type::tHandle && T->annotation && T->annotation->name == "UEEnumValue";
    }
    bool IsNativeType(const das::TypeDeclPtr& T) const override { return DSLNativeEnum(T); }
    FString GetTypePath(const das::TypeDeclPtr& T) const override
    {
        return DSLNativeEnum(T)
                   ? DSLString(DSLAnnotation(T->enumType->annotations, "ue_native_enum")->arguments, "path")
                   : FString();
    }
    FString ResolveTypePath(const das::TypeDeclPtr& T, const FString& Folder, const FString& Registry) const override
    {
        if (DSLNativeEnum(T))
            return GetTypePath(T);
        FString Error;
        if (Registry.IsEmpty() && !Folder.StartsWith(TEXT("/Game/")))
            DSLFail(TEXT("Direct authored types require type_registry or type_folder"));
        const auto* Annotation = DSLAnnotation(T->enumType->annotations, "ue_enum");
        const auto* DeclaredWire = Annotation->arguments.find("_ue_types_json", das::Type::tString);
        FDasReflectedTypes Declared;
        if (!DeclaredWire || !FDasReflectedTypes::FromJson(DSLText(DeclaredWire->sValue), Declared, Error, true))
            DSLFail(TEXT("Missing authored type identity"));
        const FString Name = DSLText(T->enumType->name);
        const FString Path = DSLResolveType(EDSLTypeKind::Enum, Registry.IsEmpty()
                                            ? Folder + TEXT("/") + Name +
                                                            TEXT(".") + Name : Declared.Enums[0].Id,
                                                                 FDSLTypeScope{Declared.Id, Folder, Registry});
        auto* E = ResolveDasEnum(Path, Error);
        if (!E)
            DSLFail(Error);
        if (E->NumEnums() != static_cast<int32>(T->enumType->list.size()) + 1)
            DSLFail(TEXT("Direct enum declaration differs from loaded asset"));
        for (const auto& V : T->enumType->list)
        {
            if (!V.value || V.value->__rtti != das::string("ExprConstInt") ||
                E->GetValueByNameString(DSLText(V.name)) != static_cast<das::ExprConstInt*>(V.value)->getValue())
                DSLFail(TEXT("Direct enum value differs from loaded asset"));
        }
        auto* Authored = Cast<UDasDefinedEnum>(E);
        if (Authored && !Authored->MemberIds.IsEmpty())
        {
            const auto* A = DSLAnnotation(T->enumType->annotations, "ue_enum");
            const auto* Wire = A->arguments.find("_ue_types_json", das::Type::tString);
            FDasReflectedTypes Model;
            if (!Wire || !FDasReflectedTypes::FromJson(DSLText(Wire->sValue), Model, Error, true) ||
                Model.Enums.Num() != 1 || Authored->MemberIds.Num() != Model.Enums[0].Entries.Num())
                DSLFail(TEXT("Direct enum member identity differs from loaded asset"));
            for (const auto& V : Model.Enums[0].Entries)
                if (Authored->MemberIds.FindRef(V.Value) != V.Id)
                    DSLFail(TEXT("Direct enum member identity differs from loaded asset"));
        }
        return Path;
    }
    das::ExpressionPtr Pack(const das::LineInfo&, das::ExpressionPtr Value) const override { return Value; }
    das::ExpressionPtr Unpack(const das::LineInfo&, const das::TypeDeclPtr&, das::ExpressionPtr Value) const override
    {
        return Value;
    }
    das::ExpressionPtr PackForPath(const das::LineInfo& At, const das::TypeDeclPtr& Type, das::ExpressionPtr V,
                                   const FString& Path) const override
    {
        if (DSLNativeEnum(Type))
            return DSLCall(At, Type->enumType->module->name + "::__ue_pack_native_enum_" + Type->enumType->name, {V});
        return DSLCall(At, "ue_enum_from_number",
                       {new das::ExprConstString(At, DSLUtf8(Path)), DSLCall(At, "int", {V})});
    }
    das::ExpressionPtr UnpackForPath(const das::LineInfo& At, const das::TypeDeclPtr& Type, das::ExpressionPtr V,
                                     const FString& Path) const override
    {
        auto T = DSLObjectCopyType(Type);
        T->ref = false;
        T->constant = false;
        auto* C = new das::ExprCast(
            At, DSLCall(At, "__ue_checked_enum_number", {V, new das::ExprConstString(At, DSLUtf8(Path))}), T);
        C->reinterpret = true;
        auto* U = new das::ExprUnsafe(At);
        U->body = C;
        return U;
    }
    EDasScriptViewUse GetViewUses() const override { return EDasScriptViewUse::NativeProperty; }
    das::TypeDeclPtr MakeView(das::ModuleGroup& Group, das::Module* Module, const das::LineInfo&) const override
    {
        return DSLHandleType(Group, Module, "UEEnumValue");
    }
    das::TypeDeclPtr MakeViewForPath(const FString& Path, das::ModuleGroup& Group, das::Module* Module,
                                     const das::LineInfo& At) const override;
    const TCHAR* GetViewModule() const override { return TEXT("ue_native_gameplay"); }
    bool ParseDefault(EDasScriptDefaultUse Use, const das::TypeDeclPtr& Type, das::ExpressionPtr E, das::ModuleGroup&,
                      das::Module*, FDasDeclaredLiteral& Default) const override
    {
        if (!DSLDirectEnum(Type) || Use == EDasScriptDefaultUse::Parameter)
            return false;
        Default.Kind = TEXT("string");
        if (Use == EDasScriptDefaultUse::StructField)
        {
            das::Structure::FieldDeclaration Field;
            Field.type = Type;
            Field.init = E;
            Default.String = DSLStructEnumDefault(Field);
            return true;
        }
        if (E && E->__rtti == das::string("ExprConstEnumeration"))
            Default.String = DSLText(static_cast<das::ExprConstEnumeration*>(E)->text);
        else if (E && E->rtti_isField())
        {
            auto* Init = static_cast<das::ExprField*>(E);
            if (!Init->value || !Init->value->rtti_isVar() ||
                static_cast<das::ExprVar*>(Init->value)->name != Type->enumType->name)
                DSLFail(TEXT("Direct enum initializer must name its declared enum"));
            Default.String = DSLText(Init->name);
        }
        else
            DSLFail(TEXT("Direct enum property requires named enum literal initializer: ")+
                         (E ? DSLText(E->__rtti) : TEXT("none")));
        if (Default.String.Contains(TEXT(".")))
            Default.String = Default.String.RightChop(Default.String.Find(TEXT(".")) + 1);
        if (Default.String.Contains(TEXT("::")))
            Default.String = Default.String.RightChop(Default.String.Find(TEXT("::")) + 2);
        return true;
    }
    TSharedPtr<FJsonValue> DefaultJson(const das::TypeDeclPtr& T, das::ExpressionPtr E, das::ModuleGroup&,
                                       das::Module*) const override
    {
        if (!DSLDirectEnum(T))
            return nullptr;
        das::Structure::FieldDeclaration F;
        F.type = T;
        F.init = E;
        FString Member;
        if (E->rtti_isField())
        {
            auto* V = static_cast<das::ExprField*>(E);
            if (!V->value->rtti_isVar() || static_cast<das::ExprVar*>(V->value)->name != T->enumType->name)
                DSLFail(TEXT("Container enum default requires its declared enum"));
            Member = DSLText(V->name);
        }
        else if (E->__rtti == das::string("ExprConstEnumeration"))
        {
            Member = DSLText(static_cast<das::ExprConstEnumeration*>(E)->text);
            int32 Dot;
            if (Member.FindLastChar('.', Dot))
                Member = Member.Mid(Dot + 1);
            int32 Colon = Member.Find(TEXT("::"), ESearchCase::CaseSensitive, ESearchDir::FromEnd);
            if (Colon != INDEX_NONE)
                Member = Member.Mid(Colon + 2);
        }
        else
            DSLFail(TEXT("Container enum default requires a named enum member"));
        return MakeShared<FJsonValueString>(Member);
    }
};

// Legacy token fields obtain their nominal identity from the explicit initializer and the class scope.
void DSLRawEnumProperty(das::ExpressionPtr InitExpression, const FDSLTypeScope& TypeScope, FDasDeclaredType& Type,
                        FDasDeclaredLiteral& Default)
{
    if (!InitExpression || !InitExpression->rtti_isCall())
        DSLFail(TEXT("Enum field requires ue_enum_value(path, member)"));
    auto* Call = static_cast<das::ExprCall*>(InitExpression);
    if (Call->name != "ue_enum_value" || Call->arguments.size() != 2 ||
        Call->arguments[0]->__rtti != das::string("ExprConstString") ||
        Call->arguments[1]->__rtti != das::string("ExprConstString"))
        DSLFail(TEXT("Enum initializer requires literal path/member"));
    Type.TypePath = DSLResolveType(EDSLTypeKind::Enum,
                                   DSLText(static_cast<das::ExprConstString*>(Call->arguments[0])->text), TypeScope);
    static_cast<das::ExprConstString*>(Call->arguments[0])->text = DSLUtf8(Type.TypePath);
    Default.Kind = TEXT("string");
    Default.String = DSLText(static_cast<das::ExprConstString*>(Call->arguments[1])->text);
    FString Issue;
    auto* E = ResolveDasEnum(Type.TypePath, Issue);
    if (!E || !ValidateDasEnumNumber(Type.TypePath, E->GetValueByNameString(Default.String), Issue))
        DSLFail(Issue);
}
