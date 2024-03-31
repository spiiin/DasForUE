module unreal_boost_internal shared

require daslib/ast_boost
require daslib/templates
require daslib/templates_boost
require strings

require unreal

[structure_macro(name="unreal_call_params")]
class private UnrealCallParamsMacro : AstStructureAnnotation
    //! If structure marked with macro:
    //!   [unreal_call_params(name="function1")]
    //!   struct FunctionParams
    //!      argument1: T1
    //!      argument2: T2
    //!      ...
    //! Then create correspond function for unreal reflection call:
    //!   function1(var obj: UObject?; argument1:T1; argument2:T2; ...)

    [unused_argument(group)] def override apply(var st : StructurePtr; var group : ModuleGroup; args : AnnotationArgumentList; var errors : das_string) : bool
        //TODO: append_annotation cpp_layout(pod=true)/or some way to control align/padding of struct?

        var inscope fnArguments : array<VariablePtr>;
        let fieldsLen = st.fields |> length
        for i in range(0, fieldsLen)
            fnArguments |> emplace_new <| new [[Variable()
                name:= st.fields[i].name,
                _type <- clone_type(st.fields[i]._type)
            ]]

        var inscope blk : array<ExpressionPtr>;

        //struct ctor parameters: (a = 1, b <- FString("hello"), c = 3.)
        var inscope mkStructFields <- new [[ MakeStruct() ]]
        for x in fnArguments
            unsafe
                var mkF <- new [[MakeFieldDecl()
                    name := x.name,
                    value <- new [[ExprVar() name := x.name]] 
                ]]
                if !x._type.canCopy
                    mkF.flags = MakeFieldDeclFlags moveSemantics
                *mkStructFields |> emplace_new(mkF)

        var stType = compiling_program() |> find_unique_structure("{st.name}")

        //[[FunctionParams() a = 1, b <- FString("hello"), c = 3.]]
        var inscope exprMakeStruct <- new [[ExprMakeStruct()
            makeType <- new [[TypeDecl() baseType = Type tStructure, structType = stType]],
        ]]
        exprMakeStruct.structs |> emplace(mkStructFields)

        //new [[FunctionParams() a = 1, b <- FString("hello"), c = 3.]]
        var inscope exprVarParamsInit <- new [[ExprAscend() subexpr <- exprMakeStruct]]

        //var params <- new [[FunctionParams() a = 1, b <- FString("hello"), c = 3.]]
        var inscope exprVarParams <- new [[ExprLet()]]
        exprVarParams.variables |> emplace_new() <| new [[Variable()
            name := "params",
            _type <- new [[TypeDecl()
                baseType = Type tPointer,
                structType = stType,
                firstType <- new [[TypeDecl() baseType = Type tStructure, structType = stType]]
            ]],
            init <- exprVarParamsInit
        ]]

        if (length(args) != 1) || (args[0].basicType != Type tString) || (args[0].name != "name")
            errors := "[unreal_call_params] macro expect \"name\" argument"
            return false
        var funcName = string(args[0].sValue)

        blk |> emplace(exprVarParams)
        blk |> emplace_new <| qmacro(obj |> call_ufunction_on(FString($v(funcName)), params))

        var inscope fn <- qmacro_function(funcName) <| $ (var obj: UObject?; $a(fnArguments))
            $b(blk)
        
        //print(describe(fn))
        compiling_module() |> add_function(fn)
        return true


[function_macro(name="unreal_call")]
class private UnrealCallMacro : AstFunctionAnnotation
    //! Macro transforms function declaration:
    //!   [unreal_call] function1(var obj: UObject?; argument1:T1; argument2:T2; argument3:T3, ...)
    //! To unreal reflection call:
    //!   obj |> call_ufunction_on("function1") <| [[FunctionParams argument1=argument1, argument2=argument2, argument3=argument3, ...]]

    [unused_argument(group, args, errors)] def override apply(var func : FunctionPtr; var group : ModuleGroup; args : AnnotationArgumentList; var errors : das_string) : bool
        //TODO: check if first argument is UObject (or other options for static/global functions)
        var inscope blk : array<ExpressionPtr>;

        var structParamsName = make_unique_private_name("{func.name}Params", func.at)
        var inscope paramsStruct <- new [[Structure() name := structParamsName]]
        let argLength = length(func.arguments)
        for x in range(1, argLength) //skip first argument, must be UObject pointer
            var argType <- clone_type(func.arguments[x]._type)
            argType.flags &= ~TypeDeclFlags constant //TODO: maybe change flags for func.arguments instead
            paramsStruct |> add_structure_field(string(func.arguments[x].name), argType, [[ExpressionPtr]])
        compiling_module() |> add_structure(paramsStruct)
        var stType = compiling_program() |> find_unique_structure(structParamsName)

        //struct ctor parameters: (a = 1, b <- FString("hello"), c = 3.)
        var inscope mkStructFields <- new [[MakeStruct()]]
        for x in range(1, argLength)
            unsafe
                var mkF <- new [[MakeFieldDecl()
                    name := func.arguments[x].name,
                    value <- new [[ExprVar() name := func.arguments[x].name]] 
                ]]
                if !func.arguments[x]._type.canCopy
                    mkF.flags = MakeFieldDeclFlags moveSemantics
                *mkStructFields |> emplace_new(mkF)

        //[[FunctionParams() a = 1, b <- FString("hello"), c = 3.]]
        var inscope exprMakeStruct <- new [[ExprMakeStruct()
            makeType <- new [[TypeDecl() baseType = Type tStructure, structType = stType]],
        ]]
        exprMakeStruct.structs |> emplace(mkStructFields)

        //new [[FunctionParams() a = 1, b <- FString("hello"), c = 3.]]
        var inscope exprVarParamsInit <- new [[ExprAscend() subexpr <- exprMakeStruct]]

        //var params <- new [[FunctionParams() a = 1, b <- FString("hello"), c = 3.]]
        var inscope exprVarParams <- new [[ExprLet()]]
        exprVarParams.variables |> emplace_new() <| new [[Variable()
            name := "params",
            _type <- new [[TypeDecl()
                baseType = Type tPointer,
                structType = stType,
                firstType <- new [[TypeDecl() baseType = Type tStructure, structType = stType]]
            ]],
            init <- exprVarParamsInit
        ]]

        blk |> emplace(exprVarParams)

        let objArgumentName = string(func.arguments[0].name)
        let funcName = string(func.name)
        blk |> emplace_new <| qmacro($i(objArgumentName) |> call_ufunction_on(FString($v(funcName)), params))

        var inscope body <- qmacro_expr <|
            $b(blk)
        func.body |> move(body)
        return true