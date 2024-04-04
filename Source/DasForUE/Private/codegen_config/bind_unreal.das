require cbind/cbind_boost
require fio
require daslib/defer
require daslib/strings_boost

let UNREAL_PATH = "../../../../../../../UnrealEngine" //TODO: get from environment?
let BIND_ROOT = "../codegen/"

class UnrealGen : CppGenBind
    override generate_lsp = true
    override func_to_stdout = false
    override generate_constant_decl = false
    unreal_skip_func : table<string;bool>
    unreal_skip_struct : table<string;bool>
    unreal_arg_part_names : array<string>

    canMoveIfCopyConstructable : table<string;bool>

    declaredFunctionsHashes : table<uint64;bool>

    def UnrealGen()
        bind_root = BIND_ROOT
        bind_module = "dasUnreal"
        bind_das_module = "unreal"
        let pfp = "./"
        let pfn = "definitions.h"
        let engineIncludes = [[string
            "{UNREAL_PATH}/Engine/Source/Runtime/Core/Public";
            "{UNREAL_PATH}/Engine/Source/Runtime/Core/Public/Containers/";
            "{UNREAL_PATH}/Engine/Source/Runtime/TraceLog/Public/";

            "{UNREAL_PATH}/Engine/Source/Runtime/CoreUObject/Public/";
        ]]

        var args <- [{string
            "-xc++-header";
            "-std=c++1z";
        }]
        for engineInclude in engineIncludes
            args |> push <| "-I{engineInclude}"

        func_per_chunk = 100
        init_args(pfn,pfp,args)
        setDefaultFiles()
        init_skip()
        openAllFiles()

    def init_skip()
        unreal_skip_func <- {{
            //UnrealString.h
            "FString::Format" => true;
            "FString::CreateIterator" => true;
            "FString::CreateConstIterator" => true;
            "FString::GetCharArray" => true;
            "FString::begin" => true;
            "FString::end" => true;
            "FString::rbegin" => true;
            "FString::rend" => true;
            "FString::SerializeAsANSICharArray" => true; //FArchive
            "FString::CountBytes" => true; //FArchive

             //UObject/UObjectBase.h
             "FStat_STAT_UObjectsStatGroupTester::GetStatName" => true; //related to inner stat
             "FStat_STAT_UObjectsStatGroupTester::GetDescription" => true;
             "FStat_STAT_UObjectsStatGroupTester::GetStatType" => true;
             "FStat_STAT_UObjectsStatGroupTester::IsClearEveryFrame" => true;
             "FStat_STAT_UObjectsStatGroupTester::IsCycleStat" => true;
             "FStat_STAT_UObjectsStatGroupTester::GetFlags" => true;
             "FStat_STAT_UObjectsStatGroupTester::GetMemoryRegion" => true;
             "RegisterCompiledInInfo" => true; //FClassRegisterCompiledInInfo
             "UObjectBaseInit" => true; //some inner stuff?
             "UObjectBaseShutdown" => true;
        }}

        //TODO:
        // - find substring is slow
        // - apply macro expansion? (`clang -E` + unexpand includes back)
        unreal_arg_part_names <- [{string
            "TArray";           //TODO: temp, due to name das/unreal name conflict (namespace das -> dasext?)

            "const WIDECHAR *";  //mostly (for UnrealString, at least), it related to overloads char/wchar/unsigned short/UTF8CHAR
            "const UCS2CHAR *";  //check what can be done with them
            "const UTF8CHAR *";  //

        }]
        
        unreal_skip_struct <- {{
            //UnrealString.h
            "FStringOutputDevice" => true;
            "FStringOutputDeviceCountLines" => true;

            //UObject/UObjectBase.h
            "UObject" => true;
            "UClass" => true;
            "FRegisterCompiledInInfo" => true;
            "FClassReloadVersionInfo" => true;
            "FClassRegisterCompiledInInfo" => true;
            "FStructReloadVersionInfo" => true;
            "FStructRegisterCompiledInInfo" => true;
            "FEnumReloadVersionInfo" => true;
            "FEnumRegisterCompiledInInfo" => true;
            "FPackageReloadVersionInfo" => true;
            "FStat_STAT_UObjectsStatGroupTester" => true; //unrelated
        }}

        canMoveIfCopyConstructable <- {{
            "FString" => true;
        }}

    //TODO: check if this may be moved to cbind_boost
    def isMoveType(name, cppname:string)
        return canMoveIfCopyConstructable[name]

    def override isArgByValue(name:string)
        return true

    def override skip_file(fname:string) : bool
        //print("fname = {fname}\n")
        if (
            fname |> find("UnrealString.h") != -1 ||
            fname |> find("UObjectBase.h") != -1
        )
            return false
        return true

    def functionArgumentRules : TypeRules
        return TypeRules char_ptr_is_string | TypeRules function_proto_is_void_ptr | TypeRules top_level_pointers_are_implicit

    //TODO: check if possible to override skip_type instead of create new function
    def skip_arguments_and_result_types(var c : CXCursor) : bool
        let narg = clang_Cursor_getNumArguments(c)
        for ai in urange(narg)
            var carg = clang_Cursor_getArgument(c, ai)
            let arg_name = string(clang_getCursorSpelling(carg))
            var arg_type = clang_getCursorType(carg)
            let ncname = string(clang_getTypeSpelling(arg_type))

            //skip rvalues args
            if arg_type.kind == CXTypeKind RValueReference
                return true
            
            //print("ArgType: {ncname}\n")
            for skipName in unreal_arg_part_names
                if ncname |> find(skipName) != -1
                    return true

        var fun_type = clang_getCursorType(c)
        var res_type = clang_getResultType(fun_type)
        //skip rvalue results
        if res_type.kind == CXTypeKind RValueReference
            return true

        let ncresult = string(clang_getTypeSpelling(res_type))
        for skipName in unreal_arg_part_names
            if ncresult |> find(skipName) != -1
                return true

        //skip methods on rvalue objects
        let refQualifier = clang_Type_getCXXRefQualifier(fun_type)
        if refQualifier != CXRefQualifierKind None
            return true
        
        return false

    def override skip_anyFunction(var c : CXCursor; isMethod:bool) : bool
        if AnyGenBind`skip_anyFunction(self, c, isMethod)
            return true
        let function_name = namespace_name(string(clang_getCursorSpelling(c)))
        if function_name |> find("operator") != -1 //TODO: check this
            return true
        if skip_arguments_and_result_types(c)
            return true
        if unreal_skip_func |> key_exists(function_name)
            return true
        return false

    def override skip_struct(name: string)
        return unreal_skip_struct |> key_exists(name)

    def override writeFuncPrefix
        fwrite(func_file, "#include \"{bind_module}.h\"\n")
        fwrite(func_file, "#include \"need_{bind_module}.h\"\n")
        fwrite(func_file, "namespace das \{\n")
        fwrite(func_file, "#include \"{bind_module}.func.aot.decl.inc\"\n")
        fwrite(func_file, "void Module_{bind_module}::initFunctions_{func_chunk_index}() \{\n")

    def override generateModuleNeedH
        module_need_h_file |> fwrite("#pragma once\n")
        generateModuleNeedHPrefix()
        module_need_h_file |> fwrite("#include \"CoreMinimal.h\"\n")

        module_need_h_file |> fwrite("#include \"{bind_module}.enum.decl.inc\"\n")
        module_need_h_file |> fwrite("#include \"{bind_module}.enum.decl.cast.inc\"\n")
        module_need_h_file |> fwrite("#include \"{bind_module}.struct.decl.inc\"\n")
        module_need_h_file |> fwrite("namespace das \{\n")
        module_need_h_file |> fwrite("\t#include \"{bind_module}.struct.cast.inc\"\n")
        module_need_h_file |> fwrite("\}\n\n")

    def override generateModuleHPrefix
        fwrite(module_h_file, "#include \"das_config.h\"\n")
        fwrite(module_h_file, "THIRD_PARTY_INCLUDES_START\n")
        fwrite(module_h_file, "#include \"daScript/misc/platform.h\"\n")
        fwrite(module_h_file, "#include \"daScript/ast/ast.h\"\n")
        fwrite(module_h_file, "#include \"daScript/ast/ast_interop.h\"\n")
        fwrite(module_h_file, "#include \"daScript/ast/ast_handle.h\"\n")
        fwrite(module_h_file, "#include \"daScript/ast/ast_typefactory_bind.h\"\n")
        fwrite(module_h_file, "#include \"daScript/simulate/bind_enum.h\"\n")
        fwrite(module_h_file, "#include \"daScript/simulate/fs_file_info.h\"\n")
        fwrite(module_h_file, "THIRD_PARTY_INCLUDES_END\n")
        fwrite(module_h_file, "#include \"using_das.inc\"\n")
        fwrite(module_h_file, "#include \"cb_dasUnreal.h\"\n")

    def override generateModuleH
        module_h_file |> fwrite("#pragma once\n")
        generateModuleHPrefix()
        module_h_file |> fwrite("namespace das \{\n")
        module_h_file |> fwrite("class Module_{bind_module} : public Module \{\n")
        module_h_file |> fwrite("public:\n")
        module_h_file |> fwrite("\tModule_{bind_module}();\n")
        module_h_file |> fwrite("protected:\n")
        module_h_file |> fwrite("\tvirtual bool initDependencies() override;\n")
        module_h_file |> fwrite("\tvoid initAdditional();\n")
        module_h_file |> fwrite("\tvoid initMain ();\n")
        if length(aot_alias)!=0
            module_h_file |> fwrite("\tvoid initAotAlias ();\n")
        module_h_file |> fwrite("\tvirtual ModuleAotType aotRequire ( TextWriter & tw ) const override;\n")
        module_h_file |> fwrite("\t#include \"{bind_module}.func.decl.inc\"\n")
        module_h_file |> fwrite("public:\n")
        module_h_file |> fwrite("\tModuleLibrary lib;\n")
        module_h_file |> fwrite("\tbool initialized = false;\n")
        generateModuleHSuffix()
        module_h_file |> fwrite("};\n")
        module_h_file |> fwrite("}\n\n")

    def override generateModuleCpp
        module_cpp_file |> fwrite("#include \"{bind_module}.h\"\n")
        module_cpp_file |> fwrite("#include \"need_{bind_module}.h\"\n")
        module_cpp_file |> fwrite("#include \"{bind_module}.struct.impl.inc\"\n")
        module_cpp_file |> fwrite("namespace das \{\n")
        module_cpp_file |> fwrite("#include \"{bind_module}.enum.class.inc\"\n")
        module_cpp_file |> fwrite("#include \"{bind_module}.struct.class.inc\"\n")
        module_cpp_file |> fwrite("#include \"{bind_module}.func.aot.inc\"\n")
        module_cpp_file |> fwrite("Module_{bind_module}::Module_{bind_module}() : Module(\"{bind_das_module}\") \{\n")  // TODO: das module name
        module_cpp_file |> fwrite("\}\n")
        module_cpp_file |> fwrite("bool Module_{bind_module}::initDependencies() \{\n")
        module_cpp_file |> fwrite("\tif ( initialized ) return true;\n")
        for rm in require_modules
            module_cpp_file |> fwrite("\tauto mod_{rm} = Module::require(\"{rm}\");\n")
            module_cpp_file |> fwrite("\tif ( !mod_{rm} ) return false;\n")
            module_cpp_file |> fwrite("\tif ( !mod_{rm}->initDependencies() ) return false;\n")
        module_cpp_file |> fwrite("\tinitialized = true;\n")
        module_cpp_file |> fwrite("\tlib.addModule(this);\n")
        module_cpp_file |> fwrite("\tlib.addBuiltInModule();\n")
        for rm in require_modules
            module_cpp_file |> fwrite("\tlib.addModule(mod_{rm});\n")
        if length(aot_alias)!=0
            module_cpp_file |> fwrite("\tinitAotAlias();\n")
        if generate_constant_decl
            module_cpp_file |> fwrite("\t#include \"{bind_module}.const.inc\"\n")
        module_cpp_file |> fwrite("\t#include \"{bind_module}.enum.add.inc\"\n")
        module_cpp_file |> fwrite("\t#include \"{bind_module}.dummy.add.inc\"\n")
        module_cpp_file |> fwrite("\t#include \"{bind_module}.struct.add.inc\"\n")
        module_cpp_file |> fwrite("\t#include \"{bind_module}.struct.postadd.inc\"\n")
        module_cpp_file |> fwrite("\t#include \"{bind_module}.alias.add.inc\"\n")
        module_cpp_file |> fwrite("\tinitAdditional();\n")
        module_cpp_file |> fwrite("\t#include \"{bind_module}.func.reg.inc\"\n")
        module_cpp_file |> fwrite("\treturn true;\n")
        module_cpp_file |> fwrite("\}\n")
        module_cpp_file |> fwrite("}\n")
        module_cpp_file |> fwrite("REGISTER_MODULE_IN_NAMESPACE(Module_{bind_module},das);\n\n")

    //TODO: check if really need overload, or base class expanded needed
    def override parse_Struct ( var c : CXCursor; struct_name : string )
        return if struct_name |> starts_with <| "(" // do nothing for anonymous unions/structs
        let struct_namespace_name = namespace_name(struct_name)
        nsst2st[struct_namespace_name] = struct_name
        st2nsst[struct_name] = struct_namespace_name
        let isAotAlias = aot_alias |> key_exists(struct_namespace_name)
        let isAlreadyDefined = already_defined |> key_exists(struct_namespace_name)
        let isForward = clang_isForwardDeclaration(c)
        var isForwardDummy = false
        if isForward
            var stdecl = clang_getTypeDeclaration(clang_getCursorType(c))
            isForwardDummy = clang_isForwardDeclaration(stdecl)
        if !struct_ann |> key_exists(struct_namespace_name)
            if !isAotAlias && !isAlreadyDefined
                if !isForward || isForwardDummy
                    fprint(struct_decl_file, "MAKE_EXTERNAL_TYPE_FACTORY({struct_name},{struct_namespace_name});\n")
                    fprint(struct_impl_file, "IMPLEMENT_EXTERNAL_TYPE_FACTORY({struct_name},{struct_namespace_name});\n")
        if isForward
            if isForwardDummy
                to_log(LOG_INFO, "Forward declaration {struct_namespace_name} at {clang_getCursorLocationDescription(c)}\n")
                struct_ann[struct_namespace_name] ||= false
            return
        struct_ann[struct_namespace_name] = true
        if isAotAlias || isAlreadyDefined
            return
        let dashing_name = namespace_name(struct_name,"_")
        let st = build_string() <| $ ( var writer:StringBuilderWriter )
            swrite |> push(unsafe(addr(writer)))
            writer |> write("// from {clang_getCursorLocationDescription(c)}\n")
            writer |> write("struct {dashing_name}_GeneratedAnnotation : ManagedStructureAnnotation<{struct_namespace_name}> \{\n")
            if isMoveType(struct_name,struct_namespace_name)
                //TODO:
                // - split tp copy/move/clone?
                // - check if it safe to move this logic in some daScript base Annotation type, remove copy-paste of compile-time checks
                // - is_copy_constructible type-trait for additional compiler check if it is ok
                writer |> write("\tvirtual bool canCopy() const override \{ return std::is_copy_constructible<{struct_name}>::value; \}\n")
                writer |> write("\tvirtual bool canClone() const override \{ return std::is_copy_constructible<{struct_name}>::value; \}\n")
                writer |> write("\tvirtual bool canMove() const override \{ return std::is_copy_constructible<{struct_name}>::value; \}\n")
            if isLocalType(struct_name,struct_namespace_name)
                writer |> write("\tvirtual bool isLocal() const override \{ return true; \}\n")
                writer |> write("\tvirtual bool canBePlacedInContainer() const override \{ return true; \}\n")
            writer |> write("\t{dashing_name}_GeneratedAnnotation(ModuleLibrary & ml) : ManagedStructureAnnotation (\"{struct_name}\", ml, \"{struct_namespace_name}\") \{\n")
            writer |> write("\t\}\n")
            writer |> write("\tvoid init () \{\n")
            let isAbstract = clang_CXXRecord_isAbstract(c)!=0u
            abstract_class |> push(isAbstract)
            AnyGenBind`parse_Struct(self, c, struct_name)
            abstract_class |> pop()
            writer |> write("\t\}\n\};\n")
            if isArgByValue(struct_namespace_name)
                fprint(struct_cast_file, "template <> struct cast_arg<{struct_namespace_name}> \{\n")
                fprint(struct_cast_file, "\tstatic __forceinline const {struct_namespace_name} & to ( Context & ctx, SimNode * node ) \{\n")
                fprint(struct_cast_file, "\t\tvec4f res = node->eval(ctx);\n")
                fprint(struct_cast_file, "\t\treturn * cast<{struct_namespace_name} *>::to(res);\n")
                fprint(struct_cast_file, "\t\}\n")
                fprint(struct_cast_file, "\};\n")
            swrite |> pop()
        fprint(struct_class_file, "{st}\n")
        fprint(struct_add_file,"auto ann_{struct_name} = make_smart<{dashing_name}_GeneratedAnnotation>(lib);\n")
        fprint(struct_add_file,"addAnnotation(ann_{struct_name});\n")
        fprint(struct_postadd_file,"initRecAnnotation(ann_{struct_name},lib);\n")

    //TODO: check if really need overload, or base class expanded needed
    def override parse_anyFunctionDecl ( var c : CXCursor; isMethod : bool )
        step_Function()
        let isStatic = clang_Cursor_getStorageClass(c)==CX_StorageClass Static
        let function_name = string(clang_getCursorSpelling(c))
        var das_function_name = function_name
        var isOperator = false
        if function_name |> starts_with("operator")
            let Ch = character_at(function_name,8)
            if !(is_alpha(Ch) || is_number(Ch) || Ch=='_')
                das_function_name = function_name |> slice(8)
                isOperator = true
        let isVirtual = clang_CXXMethod_isVirtual(c)!=0u
        var fun_type = clang_getCanonicalType(clang_getCursorType(c))
        var res_type = clang_getResultType(fun_type)
        var nc_res_type = clang_getResultType(clang_getCursorType(c))
        let function_cpp_name = namespace_name(function_name)
        das_function_name = rename_function(das_function_name,function_cpp_name)
        var bind_enchantation = ""
        var cpp_enchantation = ""
        var extra_enchantation = ", SimNode_ExtFuncCall "
        if clang_das_isRef(res_type)
            extra_enchantation = ", SimNode_ExtFuncCallRef "
        elif clang_das_isCopyOnReturn(res_type) && !isAotAliasType(nc_res_type)
            extra_enchantation = ", SimNode_ExtFuncCallAndCopyOrMove "
        if isMethod
            let using_mname = methodName()
            if isVirtual || isStatic
                let self_type = namespace_struct_name()
                let function_cpp_type = functionWrapperSpelling(c,self_type)
                let wrapper_cpp_name = gen_functionWrapper(c,self_type)
                bind_enchantation = " {function_cpp_type} , {wrapper_cpp_name} "
                cpp_enchantation = wrapper_cpp_name
            else
                let function_cpp_type = functionPtrSpelling(c,true)
                let using_mname_expr = "das::das_call_member< {function_cpp_type},&{function_cpp_name} >"
                bind_enchantation = "DAS_CALL_METHOD({using_mname})"
                cpp_enchantation = "das_call_member< {function_cpp_type} , &{function_cpp_name} >::invoke"
                fprint(func_file,"\tusing {using_mname} = {using_mname_expr};\n");
        else
            let function_cpp_type = functionPtrSpelling(c,false)
            bind_enchantation = " {function_cpp_type} , {function_cpp_name} "
            cpp_enchantation = function_cpp_name

        let funcNameHash = hash("{bind_enchantation}{extra_enchantation}")
        var skipAlreadyDeclared = declaredFunctionsHashes |> key_exists(funcNameHash)
        if skipAlreadyDeclared
            to_log(LOG_INFO, "Function declaration {bind_enchantation} {extra_enchantation} already generated, skipped (possible declared in header twice)\n")
            return

        declaredFunctionsHashes[funcNameHash] = true

        fprint(func_file, "// from {clang_getCursorLocationDescription(c)}\n")
        fprint(func_file, "\tmakeExtern<{bind_enchantation}{extra_enchantation}{get_module_temp_fn()}>(lib,\"{das_function_name}\",\"{cpp_enchantation}\")")
        let narg = clang_Cursor_getNumArguments(c)
        if narg!=0 || isMethod
            fprint(func_file,"\n\t\t->args(\{")
            if isMethod
                fprint(func_file,"\"self\"")
            for ai in urange(narg)
                if ai != 0u || isMethod
                    fprint(func_file,",")
                var carg = clang_Cursor_getArgument(c, ai)
                let arg_name = string(clang_getCursorSpelling(carg))
                fprint(func_file,"\"{arg_name}\"")
            fprint(func_file,"\})")
            for ai in urange(narg)
                let aii = int(isMethod ? ai+1u : ai)
                var carg = clang_Cursor_getArgument(c, ai)
                let cp = getCursorArgType(clang_getCursorType(carg))     // non-canonical
                if cp != ""
                    fprint(func_file,"\n\t\t->arg_type({aii},makeType<{cp}>(lib))")
                let cv = getCursorInit(carg,cp)
                if cv != ""
                    fprint(func_file,"\n\t\t->arg_init({aii},{cv})")
        let rp = getCursorArgType(nc_res_type)
        if rp != ""
            fprint(func_file,"\n\t\t->res_type(makeType<{rp}>(lib))")
		fprint(func_file, "\n\t\t->addToModule(*this, SideEffects::worstDefault)")
        fprint(func_file,";\n")

[export]
def main
    // generate
    var cgb = new UnrealGen()
    defer <|
        unsafe
            delete cgb
    cgb->generate()
    cgb->genCMakeDecl("DAS_UNREAL_BIND")
