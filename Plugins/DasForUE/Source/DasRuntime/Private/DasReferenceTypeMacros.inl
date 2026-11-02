// Generate the existing nominal handle wrappers; keep validation and dispatch in one path.
bool DSLReferenceTypeMacro(const das::TypeDeclPtr& Type)
{
    if (!Type || Type->baseType != das::Type::typeMacro)
        return false;
    const auto Name = Type->typeMacroName();
    return Name == "UEObjectRef" || Name == "ue_foundation::UEObjectRef" || Name == "UEInterfaceRef" ||
           Name == "ue_foundation::UEInterfaceRef";
}

das::TypeDeclPtr DSLExpandReferenceType(const das::TypeDeclPtr& Type, das::ModuleGroup& Group, das::Module* Module)
{
    const auto Macro = Type->typeMacroName();
    const bool Object = Macro == "UEObjectRef" || Macro == "ue_foundation::UEObjectRef";
    const size_t Count = Type->typeMacroExpr.size();
    if ((Object && Count != 3) || (!Object && Count != 2 && Count != 3))
        DSLFail(TEXT("Expected UEObjectRef(script, className) or UEInterfaceRef(path, optional authoredId)"));
    das::vector<das::string> Values;
    for (size_t I = 1; I < Count; ++I)
    {
        const auto* Value = Type->typeMacroExpr[I];
        if (!Value || Value->__rtti != das::string("ExprConstString"))
            DSLFail(TEXT("Reference type macro arguments must be string literals"));
        Values.push_back(static_cast<const das::ExprConstString*>(Value)->text);
    }
    const bool AuthoredInterface = !Object && Count == 3;
    const char* Tag = Object ? "ue_object_ref" : AuthoredInterface ? "ue_interface_ref" : "ue_native_interface";
    auto Marks = Group.findAnnotation(Tag, Module);
    auto Handles = Group.findAnnotation("UObjectHandle", Module);
    if (Marks.size() != 1 || Handles.size() != 1)
        DSLFail(TEXT("Missing reference annotation/handle transport"));
    auto* Annotation = new das::AnnotationDeclaration();
    Annotation->flags = 0;
    Annotation->annotation = Marks[0];
    Annotation->at = Type->at;
    Annotation->arguments.push_back(das::AnnotationArgument(Object ? "script" : "path", Values[0]));
    if (Count == 3)
        Annotation->arguments.push_back(das::AnnotationArgument(Object ? "type" : "id", Values[1]));
    das::StructurePtr Candidate = new das::Structure();
    Candidate->at = Type->at;
    Candidate->module = Module;
    das::Structure::FieldDeclaration Handle;
    Handle.flags = 0;
    Handle.name = "handle";
    Handle.at = Type->at;
    Handle.type = new das::TypeDecl(das::Type::tHandle, Type->at);
    Handle.type->annotation = static_cast<das::TypeAnnotation*>(Handles[0]);
    Candidate->fields.push_back(Handle);
    Candidate->annotations.push_back(Annotation);
    // A generated declaration must satisfy exactly the same constraints as handwritten syntax.
    das::string Error;
    auto* Validator = static_cast<das::StructureAnnotation*>(Marks[0]);
    if (!Validator->touch(Candidate, Group, Annotation->arguments, Error))
        DSLFail(DSLText(Error));
    FString Identity = UTF8_TO_TCHAR(Tag);
    for (const auto& Arg : Annotation->arguments)
    {
        const FString Value = DSLText(Arg.sValue);
        Identity += FString::Printf(TEXT("|%d:"), Value.Len()) + Value;
    }
    const auto Name = DSLUtf8(TEXT("__ue_reference_") + FMD5::HashAnsiString(*Identity));
    Candidate->name = Name;
    auto Found = Group.findStructure(Name, Module);
    if (auto Local = Module->findStructure(Name))
        if (std::find(Found.begin(), Found.end(), Local) == Found.end())
            Found.push_back(Local);
    if (Found.size() > 1)
        DSLFail(TEXT("Ambiguous generated reference type; import one shared alias module"));
    das::StructurePtr Structure = Found.empty() ? Candidate : Found[0];
    if (Found.empty() && !Module->addStructure(Structure, true))
        DSLFail(TEXT("Generated reference type name collision"));
    if (!Validator->look(Structure, Group, Annotation->arguments, Error))
        DSLFail(DSLText(Error));
    auto* Result = new das::TypeDecl(das::Type::tStructure, Type->at);
    Result->structType = Structure;
    das::TypeDecl::applyAutoContracts(Result, Type);
    return Result;
}

struct FUEReferenceTypeMacro : das::TypeMacro
{
    FUEReferenceTypeMacro(bool Object) : TypeMacro(Object ? "UEObjectRef" : "UEInterfaceRef") {}
    virtual das::TypeDeclPtr visit(das::Program* Program, das::Module* Module, const das::TypeDeclPtr& Type,
                                   const das::TypeDeclPtr& Pass) override
    {
        try
        {
            auto Result = DSLExpandReferenceType(Type, *Program->thisModuleGroup, Module);
            if (Pass && (Pass->baseType != das::Type::tStructure || Pass->structType != Result->structType))
                return nullptr;
            return Result;
        }
        catch (const std::exception& X)
        {
            Program->error("Reference type macro failed", X.what(), "", Type->at, das::CompilationError::invalid_type);
            return nullptr;
        }
    }
};
