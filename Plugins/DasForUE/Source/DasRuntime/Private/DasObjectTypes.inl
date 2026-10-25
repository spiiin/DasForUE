FDasDeclaredClass DSLReferenceDeclaration(const das::TypeDeclPtr&);
bool DSLAuthoredView(const das::TypeDeclPtr&);
FString DSLAuthoredReferencePath(const das::TypeDeclPtr&);
das::ExpressionPtr DSLAuthoredUnpack(const das::LineInfo&, const das::TypeDeclPtr&, das::ExpressionPtr);
das::ExpressionPtr DSLAuthoredPack(const das::LineInfo&, const das::TypeDeclPtr&, das::ExpressionPtr);
// UEObject reuses generated nominal wrappers; it never assumes a UObject layout.
bool DSLTypedObject(const das::TypeDeclPtr& T)
{
    return T && T->baseType == das::Type::tStructure && T->structType &&
           DSLAnnotation(T->structType->annotations, "ue_native_object");
}
FString DSLTypedObjectPath(const das::TypeDeclPtr&);
bool DSLTypedInterface(const das::TypeDeclPtr& T)
{
    return T && T->baseType == das::Type::tStructure && T->structType &&
           (DSLAnnotation(T->structType->annotations, "ue_native_interface") ||
            DSLAnnotation(T->structType->annotations, "ue_interface_ref"));
}
bool DSLTypedReference(const das::TypeDeclPtr& T)
{
    return DSLTypedObject(T) || DSLTypedInterface(T) || DSLAuthoredView(T);
}
FString DSLTypedReferencePath(const das::TypeDeclPtr& T)
{
    if (DSLAuthoredView(T))
        return DSLAuthoredReferencePath(T);
    if (DSLTypedInterface(T))
    {
        const auto* A = DSLAnnotation(T->structType->annotations, "ue_native_interface");
        if (!A)
            A = DSLAnnotation(T->structType->annotations, "ue_interface_ref");
        const FString Path = DSLString(A->arguments, "path");
        auto* C = FindObject<UClass>(nullptr, *Path);
        if (!DasInterfaceIdentityMatches(C))
            DSLFail(TEXT("Typed interface requires loaded interface"));
        if (A->annotation->name == "ue_interface_ref")
        {
            auto* Authored = Cast<UDasAuthoredInterfaceClass>(C);
            const FString ExpectedId = DSLString(A->arguments, "id");
            if (!Authored || !DasInterfaceIdentityMatches(C, &ExpectedId))
                DSLFail(TEXT("Typed interface stable ID mismatch"));
        }
        return Path;
    }
    return DSLTypedObjectPath(T);
}
FString DSLTypedObjectPath(const das::TypeDeclPtr& T)
{
    const auto* A = DSLAnnotation(T->structType->annotations, "ue_native_object");
    const FString Path = DSLString(A->arguments, "path");
    auto* C = FindObject<UClass>(nullptr, *Path);
    if (!Path.StartsWith(TEXT("/Script/")) || !C || C->HasAnyClassFlags(CLASS_Interface))
        DSLFail(TEXT("Typed object requires a loaded native UObject class: ")+ Path);
    return Path;
}
das::TypeDeclPtr DSLObjectCopyType(const das::TypeDeclPtr& T)
{
    das::TypeDeclPtr C = nullptr;
    das::TypeDecl::clone(C, T);
    return C;
}
das::TypeDeclPtr DSLObjectNormalize(const das::TypeDeclPtr& T, das::ModuleGroup& Group, das::Module* Module,
                                    int32 Depth = 0)
{
    if (!T)
        return T;
    if (Depth > 16)
        DSLFail(TEXT("Typed object alias depth exceeds 16"));
    if (T->baseType == das::Type::tFixedArray)
    {
        auto Element = DSLObjectNormalize(T->firstType, Group, Module, Depth + 1);
        if (DSLTypedObject(Element))
            DSLFail(TEXT("UEObject does not support fixed-array wrapper types"));
        return T;
    }
    if (T->baseType == das::Type::alias)
    {
        auto Aliases = Group.findAlias(T->alias, Module);
        if (Aliases.size() == 1)
        {
            auto C = DSLObjectNormalize(Aliases[0], Group, Module, Depth + 1);
            C = DSLObjectCopyType(C);
            das::TypeDecl::applyAutoContracts(C, T);
            return C;
        }
        auto Structures = Group.findStructure(T->alias, Module);
        if (Structures.size() == 1)
        {
            auto* C = new das::TypeDecl(das::Type::tStructure, T->at);
            C->structType = Structures[0];
            das::TypeDecl::applyAutoContracts(C, T);
            return C;
        }
        return T;
    }
    if (T->baseType == das::Type::typeMacro &&
        (T->typeMacroName() == "UEObject" || T->typeMacroName() == "ue_foundation::UEObject"))
    {
        if (T->typeMacroExpr.size() != 2 || !T->typeMacroExpr[1]->rtti_isTypeDecl())
            DSLFail(TEXT("UEObject requires one native wrapper type"));
        auto Input = static_cast<das::ExprTypeDecl*>(T->typeMacroExpr[1])->typeexpr;
        auto C = DSLObjectNormalize(Input, Group, Module, Depth + 1);
        if (!DSLTypedObject(C))
            DSLFail(TEXT("UEObject requires a generated native object wrapper"));
        DSLTypedObjectPath(C);
        C = DSLObjectCopyType(C);
        das::TypeDecl::applyAutoContracts(C, T);
        return C;
    }
    return T;
}
// Canonical reference construction does not depend on a per-type helper function.
das::ExpressionPtr DSLNativeReferenceUnpack(const das::LineInfo& At, const das::TypeDeclPtr& T,
                                            das::ExpressionPtr Handle)
{
    auto* Value = new das::ExprMakeStruct(At);
    Value->makeType = DSLObjectCopyType(T);
    Value->makeType->ref = false;
    Value->makeType->constant = false;
    Value->useInitializer = true;
    Value->alwaysUseInitializer = true;
    auto* Check = new das::ExprCall(At, DSLTypedInterface(T) ? "ue_cast_interface" : "ue_cast_nullable_object");
    Check->arguments.push_back(Handle);
    Check->arguments.push_back(new das::ExprConstString(At, DSLUtf8(DSLTypedReferencePath(T))));
    auto* Fields = new das::MakeStruct();
    Fields->push_back(new das::MakeFieldDecl(At, "handle", Check, false, false));
    Value->structs.push_back(Fields);
    return Value;
}
struct FUEObjectTypeAnnotation : das::StructureAnnotation
{
    FUEObjectTypeAnnotation() : StructureAnnotation("ue_native_object") {}
    virtual bool touch(const das::StructurePtr& S, das::ModuleGroup&, das::AnnotationArgumentList& Args,
                       das::string& Error) override
    {
        try
        {
            if (S->isClass || S->parent || S->fields.size() != 1 || S->fields[0].name != "handle" ||
                !S->fields[0].type || S->fields[0].type->ref || S->fields[0].type->baseType != das::Type::tHandle ||
                !S->fields[0].type->annotation || S->fields[0].type->annotation->name != "UObjectHandle" ||
                S->fields[0].init || Args.size() != 1 || Args[0].name != "path" || Args[0].type != das::Type::tString)
                DSLFail(TEXT("Native object wrapper requires one UObjectHandle field and path"));
            const FString Path = DSLString(Args, "path");
            auto* C = FindObject<UClass>(nullptr, *Path);
            if (!Path.StartsWith(TEXT("/Script/")) || !C || C->HasAnyClassFlags(CLASS_Interface))
                DSLFail(TEXT("Native object wrapper requires a loaded native UObject class"));
            return true;
        }
        catch (const std::exception& X)
        {
            Error = X.what();
            return false;
        }
    }
    virtual bool look(const das::StructurePtr& St, das::ModuleGroup&, das::AnnotationArgumentList&,
                      das::string& Error) override
    {
        try
        {
            DSLDeclarationKind(St->annotations);
            DSLAnnotation(St->annotations, "ue_native_object");
            return true;
        }
        catch (const std::exception& X)
        {
            Error = X.what();
            return false;
        }
    }
};
struct FUEObjectTypeMacro : das::TypeMacro
{
    FUEObjectTypeMacro() : TypeMacro("UEObject") {}
    virtual das::TypeDeclPtr visit(das::Program* Program, das::Module* Module, const das::TypeDeclPtr& Type,
                                   const das::TypeDeclPtr& Pass) override
    {
        try
        {
            auto C = DSLObjectNormalize(Type, *Program->thisModuleGroup, Module);
            if (!DSLTypedObject(C))
                DSLFail(TEXT("UEObject requires a concrete native wrapper"));
            if (Pass && (Pass->baseType != das::Type::tStructure || Pass->structType != C->structType))
                return nullptr;
            return C;
        }
        catch (const std::exception& X)
        {
            Program->error("UEObject type macro failed", X.what(), "", Type->at, das::CompilationError::invalid_type);
            return nullptr;
        }
    }
};

struct FUENativeInterfaceAnnotation : das::StructureAnnotation
{
    FUENativeInterfaceAnnotation(const char* Name = "ue_native_interface") : StructureAnnotation(Name) {}
    virtual bool touch(const das::StructurePtr& S, das::ModuleGroup&, das::AnnotationArgumentList& Args,
                       das::string& Error) override
    {
        try
        {
            if (S->isClass || S->parent || S->fields.size() != 1 || S->fields[0].name != "handle" ||
                !S->fields[0].type || S->fields[0].type->ref || S->fields[0].type->baseType != das::Type::tHandle ||
                !S->fields[0].type->annotation || S->fields[0].type->annotation->name != "UObjectHandle" ||
                S->fields[0].init || Args.size() != (name == "ue_interface_ref" ? 2u : 1u) || Args[0].name != "path" ||
                Args[0].type != das::Type::tString)
                DSLFail(TEXT("Native interface reference requires one UObjectHandle field and path"));
            const FString Path = DSLString(Args, "path");
            auto* Interface = FindObject<UClass>(nullptr, *Path);
            if (!DasInterfaceIdentityMatches(Interface))
                DSLFail(TEXT("Interface reference requires a loaded interface"));
            if (name == "ue_interface_ref")
            {
                auto* Authored = Cast<UDasAuthoredInterfaceClass>(Interface);
                const FString ExpectedId = DSLString(Args, "id");
                if (!Path.StartsWith(TEXT("/Game/")) || !Authored ||
                                     !DasInterfaceIdentityMatches(Interface, &ExpectedId))
                    DSLFail(TEXT("Authored interface reference identity mismatch"));
            }
            else if (!Path.StartsWith(TEXT("/Script/")))
                DSLFail(TEXT("Native interface reference requires a native path"));
            return true;
        }
        catch (const std::exception& X)
        {
            Error = X.what();
            return false;
        }
    }
    virtual bool look(const das::StructurePtr& S, das::ModuleGroup&, das::AnnotationArgumentList&,
                      das::string& Error) override
    {
        try
        {
            DSLDeclarationKind(S->annotations);
            DSLAnnotation(S->annotations, name.c_str());
            return true;
        }
        catch (const std::exception& X)
        {
            Error = X.what();
            return false;
        }
    }
};
