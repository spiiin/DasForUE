    const char* DSLModelKey="_ue_model_json";
    FString DSLText(const das::string& S) {return UTF8_TO_TCHAR(S.c_str());}
    das::string DSLUtf8(const FString& S) {return TCHAR_TO_UTF8(*S);}
    void DSLFail(const FString& S) {throw std::runtime_error(TCHAR_TO_UTF8(*S));}
    FString DSLString(const das::AnnotationArgumentList& Args,const char* Key,const FString& Default=FString())
    {
        if (const auto* A=Args.find(Key,das::Type::tString)) return DSLText(A->sValue);
        for (const auto& A:Args) if (A.name==Key) DSLFail(TEXT("Expected string annotation: ")+DSLText(A.name));
        return Default;
    }
    bool DSLBool(const das::AnnotationArgument& A)
    {if(A.type!=das::Type::tBool) DSLFail(TEXT("Expected boolean specifier: ")+DSLText(A.name));return A.bValue;}

    FDasDeclaredLiteral DSLTextDefault(das::ExpressionPtr E){
     FDasDeclaredLiteral D;if(!E)return D;if(!E->rtti_isCall())DSLFail(TEXT("FText defaults require ue_text or ue_text_invariant literals"));auto* Call=static_cast<das::ExprCall*>(E);const bool Invariant=Call->name=="ue_text_invariant";if(!Invariant && Call->name!="ue_text")DSLFail(TEXT("FText defaults require ue_text or ue_text_invariant"));if(Call->arguments.size()!=(Invariant?1:3))DSLFail(TEXT("Invalid text default arguments"));TArray<FString> A;for(auto* X:Call->arguments){if(X->__rtti!=das::string("ExprConstString"))DSLFail(TEXT("Text default arguments must be string literals"));A.Add(DSLText(static_cast<das::ExprConstString*>(X)->text));}if((!Invariant && (A[0].IsEmpty() || A[1].IsEmpty() || A[0].Len()>1024 || A[1].Len()>1024)) || A.Last().Len()>65536)DSLFail(TEXT("Text default identity/source limits exceeded"));D.Kind=TEXT("string");D.String=DasTextLiteral(Invariant?FString():A[0],Invariant?FString():A[1],A.Last(),Invariant);return D;
    }
    FDasDeclaredLiteral DSLByteDefault(das::ExpressionPtr E){
     FDasDeclaredLiteral D;if(!E)return D;if(E->rtti_isCall()){auto* Call=static_cast<das::ExprCall*>(E);if(Call->name!="uint8" || Call->arguments.size()!=1)DSLFail(TEXT("uint8 default requires a bounded literal"));E=Call->arguments[0];}double Number=-1;if(E->__rtti==das::string("ExprConstUInt8"))Number=static_cast<das::ExprConstUInt8*>(E)->getValue();else if(E->__rtti==das::string("ExprConstInt"))Number=static_cast<das::ExprConstInt*>(E)->getValue();if(Number<0 || Number>255)DSLFail(TEXT("uint8 default must be in range 0..255"));D.Kind=TEXT("number");D.Number=Number;return D;
    }
    das::ExpressionPtr DSLTextDefaultExpression(const FDasDeclaredLiteral& D,const das::LineInfo& At){FText Text;FString Error;if(!ParseDasTextDefault(D.String,Text,Error))DSLFail(Error);auto* Call=new das::ExprCall(At,Text.IsCultureInvariant()?"ue_text_invariant":"ue_text");if(!Text.IsCultureInvariant()){const auto N=FTextInspector::GetNamespace(Text),K=FTextInspector::GetKey(Text);if(!N.IsSet() || !K.IsSet())DSLFail(TEXT("Text default has no localization identity"));Call->arguments.push_back(new das::ExprConstString(At,DSLUtf8(N.GetValue())));Call->arguments.push_back(new das::ExprConstString(At,DSLUtf8(K.GetValue())));}const auto* Source=FTextInspector::GetSourceString(Text);Call->arguments.push_back(new das::ExprConstString(At,DSLUtf8(Source?*Source:FString())));return Call;}

    FDasDeclaredLiteral DSLParameterDefault(das::ExpressionPtr Init){
      FDasDeclaredLiteral Result;if(!Init)return Result;
      auto E=Init;bool Negative=false;if(E->__rtti==das::string("ExprOp1")){auto* Unary=static_cast<das::ExprOp1*>(E);if(Unary->op!="-")DSLFail(TEXT("Parameter defaults require scalar literals"));Negative=true;E=Unary->subexpr;}
      if(E->__rtti==das::string("ExprConstInt")){Result.Kind=TEXT("number");Result.Number=static_cast<das::ExprConstInt*>(E)->getValue();}
      else if(E->__rtti==das::string("ExprConstFloat")){Result.Kind=TEXT("number");Result.Number=static_cast<das::ExprConstFloat*>(E)->getValue();}
      else if(E->__rtti==das::string("ExprConstDouble")){Result.Kind=TEXT("number");Result.Number=static_cast<das::ExprConstDouble*>(E)->getValue();}
      else if(E->__rtti==das::string("ExprConstBool")){Result.Kind=TEXT("bool");Result.Boolean=static_cast<das::ExprConstBool*>(E)->getValue();}
      else if(E->__rtti==das::string("ExprConstString")){Result.Kind=TEXT("string");Result.String=DSLText(static_cast<das::ExprConstString*>(E)->text);}
      else DSLFail(TEXT("Parameter defaults require scalar literals"));
      if(Negative){if(Result.Kind!=TEXT("number"))DSLFail(TEXT("Only numeric defaults can be negated"));Result.Number=-Result.Number;}

      return Result;
    }
    das::ExpressionPtr DSLParameterDefaultExpression(const FDasDeclaredParameter& P,const das::LineInfo& At){
        if(P.Default.Kind.IsEmpty())return nullptr;
        if(P.Type.Kind==TEXT("text"))return DSLTextDefaultExpression(P.Default,At);
        if(P.Type.Kind==TEXT("byte"))return new das::ExprConstUInt8(At,uint8(P.Default.Number));
        if(P.Type.Kind==TEXT("int"))return new das::ExprConstInt(At,int32(P.Default.Number));
        if(P.Type.Kind==TEXT("float"))return new das::ExprConstFloat(At,float(P.Default.Number));
        if(P.Type.Kind==TEXT("double"))return new das::ExprConstDouble(At,P.Default.Number);
        if(P.Type.Kind==TEXT("bool"))return new das::ExprConstBool(At,P.Default.Boolean);
        if(P.Type.Kind==TEXT("string"))return new das::ExprConstString(At,DSLUtf8(P.Default.String));
        DSLFail(TEXT("Unsupported parameter default type"));return nullptr;
    }
    bool DSLIsMetadata(const das::string& N) {return N=="Category" || N=="DisplayName" || N=="ToolTip";}
    void DSLMetadata(const das::AnnotationArgument& A,TMap<FString,FString>& Out)
    {if(A.type!=das::Type::tString) DSLFail(TEXT("Metadata must be string: ")+DSLText(A.name));Out.Add(DSLText(A.name),DSLText(A.sValue));}
    #include "DasCommentTooltips.inl"
    const das::AnnotationDeclaration* DSLAnnotation(const das::AnnotationList& L,const char* N)
    {const das::AnnotationDeclaration* Found=nullptr;for(auto* A:L)if(A->annotation->name==N){if(Found)DSLFail(TEXT("Repeated singleton annotation: ")+DSLText(N));Found=A;}return Found;}
    void DSLAnnotationArguments(const das::AnnotationArgumentList& Args,const TSet<FString>& Repeatable={}){
        TSet<FString> Seen;for(const auto& A:Args){const FString Name=DSLText(A.name);if(Seen.Contains(Name) && !Repeatable.Contains(Name))DSLFail(TEXT("Repeated annotation option: ")+Name);Seen.Add(Name);}
    }
    void DSLDeclarationKind(const das::AnnotationList& Args){TArray<FString> Kinds;for(const auto* A:Args){const auto& Name=A->annotation->name;if(Name=="ue_library" || Name=="ue_native_base" || Name=="ue_object_ref" || Name=="ue_class" || Name=="ue_interface" || Name=="ue_struct" || Name=="ue_native_struct" || Name=="ue_class_ref" || Name=="ue_native_object" || Name=="ue_native_interface" || Name=="ue_interface_ref" || Name=="ue_delegate" || Name=="ue_multicast")Kinds.Add(DSLText(Name));}if(Kinds.Num()>1)DSLFail(TEXT("Repeated/conflicting declaration annotations: ")+FString::Join(Kinds,TEXT(",")));}
    void DSLAddDirections(const das::AnnotationArgument& A,TMap<FString,FString>& Directions){
        if(A.type!=das::Type::tString)DSLFail(TEXT("Out/ref requires parameter names"));TArray<FString> Names;DSLText(A.sValue).ParseIntoArray(Names,TEXT(","),false);if(Names.IsEmpty())DSLFail(TEXT("Empty out/ref parameter list"));for(auto Name:Names){Name=Name.TrimStartAndEnd();if(Name.IsEmpty() || Directions.Contains(Name))DSLFail(TEXT("Duplicate/overlapping out/ref parameter name"));Directions.Add(Name,A.name=="out"?TEXT("out"):TEXT("inout"));}
    }
    FString DSLParameterDirection(const das::VariablePtr& Parameter,const FString& Legacy){
        FString Marker;TSet<FString> Seen;
        for(const auto& A:Parameter->annotation){const FString Name=DSLText(A.name);if(Name!=TEXT("ue_ref") && Name!=TEXT("ue_out")){if(Name.StartsWith(TEXT("ue_")))DSLFail(TEXT("Unknown UE parameter metadata: ")+Name);continue;}
            if(Seen.Contains(Name))DSLFail(TEXT("Repeated parameter direction metadata: ")+Name);Seen.Add(Name);if(A.type!=das::Type::tBool || !A.bValue)DSLFail(TEXT("Parameter direction marker must be true: ")+Name);const FString Direction=Name==TEXT("ue_out")?TEXT("out"):TEXT("inout");if(!Marker.IsEmpty())DSLFail(TEXT("Conflicting parameter ue_ref/ue_out metadata"));Marker=Direction;
        }
        if(!Marker.IsEmpty() && !Legacy.IsEmpty() && Marker!=Legacy)DSLFail(TEXT("Conflicting parameter and function direction: ")+DSLText(Parameter->name));
        const FString Explicit=Marker.IsEmpty()?Legacy:Marker;const auto& Type=Parameter->type;if(!Type)DSLFail(TEXT("Missing parameter type"));
        if(Type->ref){if(Type->constant)DSLFail(TEXT("Out/ref parameters require var mutable reference: ")+DSLText(Parameter->name));return Explicit.IsEmpty()?TEXT("inout"):Explicit;}
        if(!Explicit.IsEmpty())DSLFail(TEXT("Out/ref direction requires a mutable reference: ")+DSLText(Parameter->name));return TEXT("in");
    }
    #include "DasTypeResolution.inl"
    // Source-only type tokens lower to the existing checked runtime ABI.
    struct FDSLTypeCallVisitor:das::Visitor {
        FDSLTypeScope Scope;bool Resolve;
        explicit FDSLTypeCallVisitor(const FDSLTypeScope& InScope,bool InResolve=true):Scope(InScope),Resolve(InResolve){}
        virtual void preVisit(das::ExprCall* Call)override{
            das::string Name=Call->name;
            const das::string Prefix="ue_foundation::";
            if(Name.find(Prefix)==0)Name=Name.substr(Prefix.size());
            int32 Index=-1;EDSLTypeKind Kind=EDSLTypeKind::Class;
            if(Name=="ue_object_value")Index=0;
            else if(Name=="ue_cast_object" || Name=="ue_cast_nullable_object" || Name=="ue_object_is_a")Index=1;
            else if(Name=="ue_cast_interface" || Name=="ue_begin_interface_call" || Name=="ue_implements_interface" || Name=="ue_interface_call_int" || Name=="ue_interface_call_string"){Index=1;Kind=EDSLTypeKind::Interface;}
            else if(Name=="ue_enum_value" || Name=="ue_enum_from_number"){Index=0;Kind=EDSLTypeKind::Enum;}
            else if(Name=="ue_struct_value" || Name=="ue_struct_default"){Index=0;Kind=EDSLTypeKind::Struct;}
            if(Index<0 || Call->arguments.size()<=size_t(Index))return;
            auto* Argument=Call->arguments[size_t(Index)];bool Token=false;
            if(Argument->rtti_isCall()){
                auto* Marker=static_cast<das::ExprCall*>(Argument);
                if(Marker->name!="ue_type" && Marker->name!="ue_dsl::ue_type")return;
                if(Marker->arguments.size()!=1)DSLFail(TEXT("ue_type requires exactly one type identity"));
                Argument=Marker->arguments[0];Token=true;
            }
            FString Identity;
            if(Argument->__rtti==das::string("ExprConstString"))Identity=DSLText(static_cast<das::ExprConstString*>(Argument)->text);
            else if(Token && Argument->rtti_isVar())Identity=DSLText(static_cast<das::ExprVar*>(Argument)->name);
            else {if(Token)DSLFail(TEXT("ue_type requires a type name or literal identity"));return;}
            // Explicit full paths retain low-level runtime validation compatibility.
            if(!Token && Identity.StartsWith(TEXT("/")))return;
            if(!Resolve){if(Token)Call->arguments[size_t(Index)]=new das::ExprConstString(Argument->at,DSLUtf8(Identity));return;}
            const FString Canonical=DSLResolveType(Kind,Identity,Scope);
            Call->arguments[size_t(Index)]=new das::ExprConstString(Argument->at,DSLUtf8(Canonical));
        }
    };
    #include "DasObjectTypes.inl"
    #include "DasClassTypes.inl"
    #include "DasSoftTypes.inl"
    #include "DasGameplayTagsDSL.inl"
    das::ExpressionPtr DSLDirectPack(const das::LineInfo&,const das::TypeDeclPtr&,das::ExpressionPtr,const FString&);
    das::ExpressionPtr DSLDirectUnpack(const das::LineInfo&,const das::TypeDeclPtr&,das::ExpressionPtr,const FString&);
    #include "DasObjectKeyTypes.inl"
    FString DSLTypeName(const das::TypeDeclPtr& T){if(!T)return {};if(DSLSoft(T) || DSLGameplay(T))return TEXT("struct");if(DSLObjectKey(T))return TEXT("object_key");if(DSLTypedClass(T))return TEXT("class");if(T->baseType==das::Type::tStructure && T->structType && DSLAnnotation(T->structType->annotations,"ue_delegate"))return TEXT("delegate");if(DSLTypedReference(T))return TEXT("object");if(T->baseType==das::Type::tEnumeration && T->enumType && (DSLAnnotation(T->enumType->annotations,"ue_enum") || DSLAnnotation(T->enumType->annotations,"ue_native_enum")))return TEXT("enum");if(T->baseType==das::Type::tStructure && T->structType && (DSLAnnotation(T->structType->annotations,"ue_struct") || DSLAnnotation(T->structType->annotations,"ue_native_struct")))return TEXT("struct");switch(T->baseType){case das::Type::tUInt8:return TEXT("byte");case das::Type::tArray:return TEXT("array");case das::Type::tTable:return T->secondType && T->secondType->baseType==das::Type::tVoid?TEXT("set"):TEXT("map");case das::Type::tInt:return TEXT("int");case das::Type::tFloat:return TEXT("float");case das::Type::tBool:return TEXT("bool");case das::Type::tDouble:return TEXT("double");case das::Type::tString:return TEXT("string");case das::Type::tVoid:return TEXT("void");case das::Type::tHandle:if(T->annotation && T->annotation->name=="UETextValue")return TEXT("text");if(T->annotation && T->annotation->name=="UEClassHandle")return TEXT("class");if(T->annotation && T->annotation->name=="UEEnumValue")return TEXT("enum");if(T->annotation && T->annotation->name=="UENameValue")return TEXT("name");if(T->annotation && T->annotation->name=="UObjectHandle")return TEXT("object");if(T->annotation && (T->annotation->name=="InputActionValue" || T->annotation->name=="UEVector2D" || T->annotation->name=="UEVector" || T->annotation->name=="UERotator" || T->annotation->name=="UETransform" || T->annotation->name=="UEStructValue"))return TEXT("struct");default:break;}return {};}
    FString DSLMathPath(const das::TypeDeclPtr& T){if(!T || !T->annotation)return {};return T->annotation->name=="InputActionValue"?TEXT("/Script/EnhancedInput.InputActionValue"):T->annotation->name=="UEVector2D"?TEXT("/Script/CoreUObject.Vector2D"):T->annotation->name=="UEVector"?TEXT("/Script/CoreUObject.Vector"):T->annotation->name=="UERotator"?TEXT("/Script/CoreUObject.Rotator"):T->annotation->name=="UETransform"?TEXT("/Script/CoreUObject.Transform"):FString();}
    bool DSLPropertyMatches(FProperty* P,const FString& K){const auto Kind=DasTypeKind(K);return Kind!=EDasTypeKind::Invalid && DasPropertyKind(P)==Kind;}
    bool DSLSet(const das::TypeDeclPtr& T){return T && T->baseType==das::Type::tTable && T->secondType && T->secondType->baseType==das::Type::tVoid;}
    bool DSLContainer(const das::TypeDeclPtr& T){return T && (T->baseType==das::Type::tArray || T->baseType==das::Type::tTable);}
    FString DSLDirectPath(const das::TypeDeclPtr&,const FString&,const FString&);
    bool DSLDirectEnum(const das::TypeDeclPtr&);
    FDasDeclaredClass DSLReferenceDeclaration(const das::TypeDeclPtr&);
    FDasDeclaredType DSLContainerType(const das::TypeDeclPtr& T,const FString& Folder=FString(),const FString& Registry=FString()){
        FDasDeclaredType D;D.Kind=DSLTypeName(T);
        auto First=MakeShared<FDasDeclaredType>();First->Kind=DSLTypeName(T->firstType);if(DSLObjectKey(T->firstType)){if(D.Kind!=TEXT("set") && D.Kind!=TEXT("map"))DSLFail(TEXT("Object keys are only supported as map keys or set elements"));const auto* Key=DSLKeyAnnotation(T->firstType);First->Kind=TEXT("object");First->TypePath=Key->ClassPath;First->AuthoredId=Key->AuthoredId;}else if((D.Kind==TEXT("set") || D.Kind==TEXT("map")) && First->Kind==TEXT("object"))DSLFail(TEXT("Use UEObjectKey<Reference> for UObject table keys"));if(D.Kind==TEXT("array") && DSLTypedReference(T->firstType)){if(DSLAuthoredView(T->firstType))First->AuthoredId=DSLReferenceDeclaration(T->firstType).Id;First->TypePath=DSLTypedReferencePath(T->firstType);if(!First->TypePath.StartsWith(TEXT("/Script/")))DSLFail(TEXT("Reference arrays require native object/interface types"));}if((D.Kind==TEXT("array") || D.Kind==TEXT("set") || D.Kind==TEXT("map")) && DSLDirectEnum(T->firstType))First->TypePath=DSLDirectPath(T->firstType,Folder,Registry);if(D.Kind==TEXT("array") && First->Kind==TEXT("struct")){if(DSLGameplay(T->firstType))First->TypePath=DSLGameplayPath(T->firstType);else if(DSLSoft(T->firstType))First->TypePath=DSLSoftPath(T->firstType);else if(!DSLMathPath(T->firstType).IsEmpty())First->TypePath=DSLMathPath(T->firstType);else {if(!T->firstType->structType || (!DSLAnnotation(T->firstType->structType->annotations,"ue_struct") && !DSLAnnotation(T->firstType->structType->annotations,"ue_native_struct")))DSLFail(TEXT("Struct arrays require authored ue_struct or supported math elements"));First->TypePath=DSLDirectPath(T->firstType,Folder,Registry);}}
        if(D.Kind==TEXT("map")){D.Key=First;auto Value=MakeShared<FDasDeclaredType>();Value->Kind=DSLTypeName(T->secondType);if(DSLDirectEnum(T->secondType))Value->TypePath=DSLDirectPath(T->secondType,Folder,Registry);if(DSLTypedReference(T->secondType)){if(DSLAuthoredView(T->secondType))Value->AuthoredId=DSLReferenceDeclaration(T->secondType).Id;Value->TypePath=DSLTypedReferencePath(T->secondType);if(!Value->TypePath.StartsWith(TEXT("/Script/")))DSLFail(TEXT("Reference maps require native object/interface hosts"));}if(Value->Kind==TEXT("struct")){if(DSLGameplay(T->secondType))Value->TypePath=DSLGameplayPath(T->secondType);else if(DSLSoft(T->secondType))Value->TypePath=DSLSoftPath(T->secondType);else if(!DSLMathPath(T->secondType).IsEmpty())Value->TypePath=DSLMathPath(T->secondType);else {if(!T->secondType->structType || (!DSLAnnotation(T->secondType->structType->annotations,"ue_struct") && !DSLAnnotation(T->secondType->structType->annotations,"ue_native_struct")))DSLFail(TEXT("Struct maps require authored ue_struct or supported math values"));Value->TypePath=DSLDirectPath(T->secondType,Folder,Registry);}}D.Value=Value;}else D.Element=First;
        if(!DasContainerCarrier(D))DSLFail(TEXT("Standalone containers require primitive/authored-struct arrays, int/string/name/enum/object-key sets or int/string/name/enum/object-key scalar/name/struct/enum/reference maps"));
        return D;
    }
    bool DSLDelegate(const das::TypeDeclPtr&);
    das::ExpressionPtr DSLDelegateSnapshot(const das::LineInfo&,const das::TypeDeclPtr&,das::ExpressionPtr);
    #include "DasAdapterValueTypes.inl"
#include "DasDirectTypes.inl"
    struct FUESourceValueBinding:das::FunctionAnnotation {
        FUESourceValueBinding():FunctionAnnotation("ue_source_value_binding"){}
        virtual bool apply(das::ExprBlock*,das::ModuleGroup&,das::AnnotationArgumentList&,das::string&)override{return false;}
        virtual bool finalize(das::ExprBlock*,das::ModuleGroup&,das::AnnotationArgumentList&,const das::AnnotationArgumentList&,das::string&)override{return false;}
        virtual bool apply(const das::FunctionPtr&,das::ModuleGroup&,das::AnnotationArgumentList&,das::string&)override{return true;}
        virtual bool finalize(const das::FunctionPtr& Function,das::ModuleGroup& Group,das::AnnotationArgumentList& Args,const das::AnnotationArgumentList&,das::string& Error)override{try{
            DSLAnnotationArguments(Args);if(Args.size()!=4)DSLFail(TEXT("Source value binding requires kind, id, path and registry"));const FString Kind=DSLString(Args,"kind"),Id=DSLString(Args,"id"),Path=DSLString(Args,"path"),Registry=DSLString(Args,"registry");
            das::TypeDeclPtr Source=Function->result;if(!DSLDirectEnum(Source) && !DSLDirectStruct(Source)){if(Function->arguments.size()!=1)DSLFail(TEXT("Source value converter requires one parameter"));Source=Function->arguments[0]->type;}
            if(DSLNativeEnum(Source) || (Kind==TEXT("enum")?!DSLDirectEnum(Source):Kind==TEXT("struct")?!DSLDirectStruct(Source):true) || Registry.IsEmpty())DSLFail(TEXT("Source value binding type/kind mismatch"));
            const auto* A=DSLDirectEnum(Source)?DSLAnnotation(Source->enumType->annotations,"ue_enum"):DSLAnnotation(Source->structType->annotations,"ue_struct");const auto* Wire=A->arguments.find("_ue_types_json",das::Type::tString);FDasReflectedTypes Declared;FString Issue;if(!Wire || !FDasReflectedTypes::FromJson(DSLText(Wire->sValue),Declared,Issue,true))DSLFail(TEXT("Source binding has no declaration identity"));const FString ActualId=Kind==TEXT("enum")?Declared.Enums[0].Id:Declared.Structs[0].Id;
            if(ActualId!=Id || DSLDirectPath(Source,FString(),Registry)!=Path)DSLFail(TEXT("Source value binding identity/layout mismatch"));return true;
        }catch(const std::exception& X){Error=X.what();return false;}}
    };
    FString DSLMathPropertyDefault(const das::Structure::FieldDeclaration& Field);
    FString DSLStructEnumDefault(const das::Structure::FieldDeclaration& Field){FString Name;if(!Field.init)DSLFail(TEXT("Enum struct members require an explicit named default"));if(Field.init->__rtti==das::string("ExprConstEnumeration")){auto* E=static_cast<das::ExprConstEnumeration*>(Field.init);if(E->enumType && E->enumType!=Field.type->enumType)DSLFail(TEXT("Enum struct default has a different enum type"));Name=DSLText(E->text);}else if(Field.init->rtti_isField()){auto* E=static_cast<das::ExprField*>(Field.init);const auto Qualified=Field.type->enumType->module->name+"::"+Field.type->enumType->name;if(!E->value->rtti_isVar() || (static_cast<das::ExprVar*>(E->value)->name!=Field.type->enumType->name && static_cast<das::ExprVar*>(E->value)->name!=Qualified))DSLFail(TEXT("Enum struct default must name its declared enum"));Name=DSLText(E->name);}else DSLFail(TEXT("Enum struct defaults require named enum literals"));if(Name.Contains(TEXT("::")))Name=Name.RightChop(Name.Find(TEXT("::"),ESearchCase::CaseSensitive,ESearchDir::FromEnd)+2);if(!std::any_of(Field.type->enumType->list.begin(),Field.type->enumType->list.end(),[&](const auto& V){return DSLText(V.name)==Name;}))DSLFail(TEXT("Unknown enum struct default member"));return Name;}
    bool DSLDelegate(const das::TypeDeclPtr&);
    das::ExpressionPtr DSLDelegateView(const das::LineInfo&,const das::TypeDeclPtr&,das::ExpressionPtr,const FString&);
    das::ExpressionPtr DSLDelegateFieldRead(const das::LineInfo&,const das::TypeDeclPtr&,das::ExpressionPtr,const FString&);
    das::ExpressionPtr DSLDelegateFieldWrite(const das::LineInfo&,const das::TypeDeclPtr&,das::ExpressionPtr,const FString&,das::ExpressionPtr);
    #include "DasComputedDefaultsState.inl"
    #include "DasAuthoredRefs.inl"
    #include "DasDelegateDSL.inl"
    #include "DasComponentFields.inl"
    #include "DasDeclarationGroups.inl"
    #include "DasComputedDefaults.inl"
    #include "DasComponentDefaults.inl"
    #include "DasContainerDefaultsDSL.inl"
    bool DSLVirtualMethod(const FDasDeclaredClass& C,const FDasDeclaredMethod& M){
        if(M.bBlueprintEvent)return true;
        for(const auto& Path:C.Interfaces)if(auto* Interface=FindObject<UClass>(nullptr,*Path);Interface && Interface->FindFunctionByName(*M.Name))return true;
        return false;
    }
    // Native descriptors are compilation-local. Never serialize them as authored fields.
    FProperty* DSLNativeSelfField(const FDasDeclaredClass& C,const das::string& Name){
        auto* Base=DasNativeDefaultBase(C.NativeParent);
        if(Base){if(auto* P=Base->FindPropertyByName(*DSLText(Name)))return P;}
        // Explicit test-only native host extension, never inferred from bridge storage.
        if(C.NativeParent==TEXT("/Script/Engine.Actor") && IsDasNativeArraySelfProbeEnabled() && DSLText(Name).StartsWith(TEXT("DasNativeArray")))return ADasProbeActor::StaticClass()->FindPropertyByName(*DSLText(Name));return nullptr;
    }
    bool DSLNativeSelfKnown(const FDasDeclaredClass& C,const das::string& N){return DSLNativeSelfField(C,N) || (DasActorNativeParent(C.NativeParent) && DSLText(N).Equals(TEXT("ActorTransform"),ESearchCase::IgnoreCase));}
    FDasDeclaredProperty DSLNativeSelfProperty(const FDasDeclaredClass& C,const das::string& Name){
        const FString N=DSLText(Name);
        if(DasActorNativeParent(C.NativeParent) && N.Equals(TEXT("ActorTransform"),ESearchCase::CaseSensitive)){FDasDeclaredProperty D;D.Name=N;D.Type.Kind=TEXT("struct");D.Type.TypePath=TEXT("/Script/CoreUObject.Transform");return D;}
        auto* P=DSLNativeSelfField(C,Name);
        // Owner is an explicit readonly facade over the public GetOwner(), not
        // permission to access arbitrary non-Blueprint-visible native storage.
        const bool OwnerFacade=DasActorNativeParent(C.NativeParent) && N.Equals(TEXT("Owner"),ESearchCase::CaseSensitive);
        if(!P || !P->GetName().Equals(N,ESearchCase::CaseSensitive) || P->ArrayDim!=1 || P->HasAnyPropertyFlags(CPF_EditorOnly|CPF_Deprecated) || (!OwnerFacade && !P->HasAnyPropertyFlags(CPF_BlueprintVisible)))DSLFail(TEXT("Unknown or inaccessible native self property: ")+N);
        // Accessor-backed fields must go through their reflected getter/setter.
        // UE strips BlueprintGetter/Setter metadata when cooking. Keep the small
        // Actor adapter's accessor policy explicit so editor and game agree.
        const bool ActorAccessor=DasActorNativeParent(C.NativeParent) && (N==TEXT("bAutoDestroyWhenFinished") || N==TEXT("NetCullDistanceSquared") || N==TEXT("NetUpdateFrequency") || N==TEXT("MinNetUpdateFrequency"));
        if(P->HasSetterOrGetter() || ActorAccessor)DSLFail(TEXT("Native self property requires its accessor: ")+N);
        FDasDeclaredProperty D;D.Name=N;D.bReadOnly=OwnerFacade || P->HasAnyPropertyFlags(CPF_BlueprintReadOnly);
        D.Type.Kind=CastField<FIntProperty>(P)?TEXT("int"):CastField<FFloatProperty>(P)?TEXT("float"):CastField<FDoubleProperty>(P)?TEXT("double"):CastField<FBoolProperty>(P)?TEXT("bool"):CastField<FStrProperty>(P)?TEXT("string"):CastField<FNameProperty>(P)?TEXT("name"):CastField<FClassProperty>(P)?TEXT("class"):CastField<FObjectProperty>(P)?TEXT("object"):TEXT("");
        auto NativeValue=[&](FProperty* Value){auto Type=MakeShared<FDasDeclaredType>();if(!Value || Value->ArrayDim!=1)return Type;
            Type->Kind=CastField<FIntProperty>(Value)?TEXT("int"):CastField<FFloatProperty>(Value)?TEXT("float"):CastField<FDoubleProperty>(Value)?TEXT("double"):CastField<FBoolProperty>(Value)?TEXT("bool"):CastField<FStrProperty>(Value)?TEXT("string"):CastField<FNameProperty>(Value)?TEXT("name"):TEXT("");
            if(auto* Enum=DasEnumPropertyType(Value)){Type->Kind=TEXT("enum");Type->TypePath=Enum->GetPathName();}
            if(auto* Struct=CastField<FStructProperty>(Value)){FString Issue;if(!ValidateDasValueStruct(Struct->Struct->GetPathName(),Issue))DSLFail(Issue);Type->Kind=TEXT("struct");Type->TypePath=Struct->Struct->GetPathName();}
            if(!CastField<FClassProperty>(Value) && DasReferencePropertyClass(Value)){Type->Kind=TEXT("object");Type->TypePath=DasReferencePropertyClass(Value)->GetPathName();}
            return Type;};
        if(auto* Array=CastField<FArrayProperty>(P)){D.Type.Kind=TEXT("array");D.Type.Element=NativeValue(Array->Inner);if(D.Type.Element->Kind.IsEmpty())DSLFail(TEXT("Unsupported native array element"));}
        if(auto* Map=CastField<FMapProperty>(P)){D.Type.Kind=TEXT("map");D.Type.Key=NativeValue(Map->KeyProp);D.Type.Value=NativeValue(Map->ValueProp);if((D.Type.Key->Kind!=TEXT("int") && D.Type.Key->Kind!=TEXT("string") && D.Type.Key->Kind!=TEXT("name") && D.Type.Key->Kind!=TEXT("enum") && D.Type.Key->Kind!=TEXT("object")) || D.Type.Value->Kind.IsEmpty())DSLFail(TEXT("Native maps require int/string/name/enum/object keys and supported values"));}
        if(auto* Set=CastField<FSetProperty>(P)){D.Type.Kind=TEXT("set");D.Type.Element=NativeValue(Set->ElementProp);if(D.Type.Element->Kind!=TEXT("int") && D.Type.Element->Kind!=TEXT("string") && D.Type.Element->Kind!=TEXT("name") && D.Type.Element->Kind!=TEXT("enum") && D.Type.Element->Kind!=TEXT("object"))DSLFail(TEXT("Native sets require int/string/name/enum/object elements"));}
        if(D.Type.Kind==TEXT("array") || D.Type.Kind==TEXT("map") || D.Type.Kind==TEXT("set")){auto* Carrier=DasContainerCarrierForProperty(P);auto* Values=Carrier?Carrier->FindPropertyByName(TEXT("Values")):nullptr;if(!Values || !Values->SameType(P))DSLFail(TEXT("Native container carrier differs from property: ")+N);}
        if(auto* Enum=DasEnumPropertyType(P)){FString Issue;if(!ResolveDasEnum(Enum->GetPathName(),Issue))DSLFail(Issue);D.Type.Kind=TEXT("enum");D.Type.TypePath=Enum->GetPathName();}
        if(auto* Struct=CastField<FStructProperty>(P)){FString Issue;if(!ValidateDasValueStruct(Struct->Struct->GetPathName(),Issue))DSLFail(Issue);D.Type.Kind=TEXT("struct");D.Type.TypePath=Struct->Struct->GetPathName();}
        if(D.Type.Kind.IsEmpty())DSLFail(TEXT("Unsupported native self property type: ")+N);
        if(auto* Class=CastField<FClassProperty>(P))D.Type.TypePath=Class->MetaClass->GetPathName();
        else if(auto* Object=CastField<FObjectProperty>(P))D.Type.TypePath=Object->PropertyClass->GetPathName();
        return D;
    }
    FString DSLCallbackKind(const das::TypeDeclPtr& T){
        const FString K=DSLTypeName(T), Math=DSLMathPath(T);
        if(K==TEXT("array")){const FString Wire=TEXT("array_")+DSLTypeName(T->firstType);return FindDasCallbackType(Wire)?Wire:TEXT("struct");}
        if(DSLContainer(T))return TEXT("struct");
        if(!Math.IsEmpty()){
            if(Math==TEXT("/Script/CoreUObject.Vector"))return TEXT("vector");
            if(Math==TEXT("/Script/CoreUObject.Rotator"))return TEXT("rotator");
            if(Math==TEXT("/Script/CoreUObject.Transform"))return TEXT("transform");
            if(Math==TEXT("/Script/CoreUObject.Vector2D"))return TEXT("vector2d");
        }
        return K;
    }
    #include "DasCallbackFramesDSL.inl"
    struct FDSLBodyVisitor:das::Visitor
    {
        const FDasDeclaredClass& Decl;das::string Self;das::TypeDeclPtr SelfReference=nullptr,SuperReference=nullptr;TMap<FString,das::string> LocalTargets,LocalSupers;TMap<FString,das::string> LocalRenames;TMap<FString,das::string> QualifiedLocals;const das::StructurePtr Source;FString Folder,Registry;das::ModuleGroup& Group;das::vector<das::FunctionPtr>& GeneratedHelpers;TSet<FString>& HelperPaths;TSet<const das::Expression*> LeftFields;TMap<FString,FDasDeclaredProperty> NativeProperties;bool NativeOnly=false;FString Error;
        FDSLBodyVisitor(const FDasDeclaredClass& C,const das::string& S,const das::StructurePtr& St,const FString& F,const FString& R,das::ModuleGroup& G,das::vector<das::FunctionPtr>& H,TSet<FString>& Paths):Decl(C),Self(S),Source(St),Folder(F),Registry(R),Group(G),GeneratedHelpers(H),HelperPaths(Paths){}
        das::ExprField* SelfField(das::Expression* E)
        {
            if(!E || !E->rtti_isField()) return nullptr;
            auto* F=static_cast<das::ExprField*>(E);
            if(!F->value || !F->value->rtti_isVar() || static_cast<das::ExprVar*>(F->value)->name!=Self)return nullptr;
            if(NativeOnly && (Source->findField(F->name) || !DSLNativeSelfKnown(Decl,F->name)))return nullptr;
            return F;
        }
        const FDasDeclaredProperty* Property(das::ExprField* F)
        {
            const FString N=DSLText(F->name);const auto* P=Decl.Properties.FindByPredicate([&](const auto& X){return X.Name==N;});
            if(!P && !Source->findField(F->name) && DSLNativeSelfKnown(Decl,F->name)){
                auto Native=DSLNativeSelfProperty(Decl,F->name);
                if(auto* Existing=NativeProperties.Find(N))return Existing;
                return &NativeProperties.Add(N,MoveTemp(Native));
            }
            if(!P) Error=Decl.Components.ContainsByPredicate([&](const auto& C){return C.Name==N;})?TEXT("Cannot replace BlueprintReadOnly component reference"):TEXT("Unknown reflected self property: ")+N;return P;
        }
        das::TypeDeclPtr SelfPropertyType(das::ExprField* F,const FDasDeclaredProperty* P){
            if(const auto* Field=Source->findField(F->name))return Field->type;
            if(P->Type.Kind==TEXT("map") || P->Type.Kind==TEXT("set")){
                auto* Type=new das::TypeDecl(das::Type::tTable,F->at);auto Convert=[&](const auto& Value){FDasDeclaredProperty Child;Child.Type=*Value;auto Result=SelfPropertyType(F,&Child);return Result?Result:DSLPropertyType(Value->Kind,Group,Source->module,F->at);};
                Type->firstType=Convert(P->Type.Kind==TEXT("map")?P->Type.Key:P->Type.Element);if((P->Type.Kind==TEXT("map")?P->Type.Key:P->Type.Element)->Kind==TEXT("object"))Type->firstType=DSLMakeObjectKeyType(Type->firstType,Group,Source->module,F->at);Type->secondType=P->Type.Kind==TEXT("map")?Convert(P->Type.Value):static_cast<das::TypeDeclPtr>(new das::TypeDecl(das::Type::tVoid,F->at));DSLEnsureStructHelpers(Type,PropertyPath(P),Group,Source->module,GeneratedHelpers,HelperPaths);return Type;
            }
            if(P->Type.Kind==TEXT("array")){
                auto* Type=new das::TypeDecl(das::Type::tArray,F->at);FDasDeclaredProperty Element;Element.Type=*P->Type.Element;Type->firstType=SelfPropertyType(F,&Element);if(!Type->firstType)Type->firstType=DSLPropertyType(P->Type.Element->Kind,Group,Source->module,F->at);
                DSLEnsureStructHelpers(Type,PropertyPath(P),Group,Source->module,GeneratedHelpers,HelperPaths);return Type;
            }
            if(P->Type.Kind==TEXT("struct"))return DSLHandleType(Group,Source->module,P->Type.TypePath==TEXT("/Script/CoreUObject.Vector")?"UEVector":P->Type.TypePath==TEXT("/Script/CoreUObject.Vector2D")?"UEVector2D":P->Type.TypePath==TEXT("/Script/CoreUObject.Rotator")?"UERotator":P->Type.TypePath==TEXT("/Script/CoreUObject.Transform")?"UETransform":"UEStructValue");
            if(P->Type.Kind==TEXT("enum")){
                das::EnumerationPtr Match=nullptr;TSet<das::Module*> Seen;
                auto InspectEnum=[&](das::Module* Module){if(!Module || Seen.Contains(Module) || !Source->module->isVisibleDirectly(Module))return;Seen.Add(Module);Module->enumerations.foreach([&](const das::EnumerationPtr& E){if(E->isPrivate && Module!=Source->module)return;const auto* A=DSLAnnotation(E->annotations,"ue_native_enum");if(!A || DSLString(A->arguments,"path")!=P->Type.TypePath)return;if(Match && Match!=E)DSLFail(TEXT("Ambiguous native self enum declaration: ")+P->Type.TypePath);Match=E;});};
                InspectEnum(Source->module);for(auto* Module:Group.getModules())InspectEnum(Module);if(!Match)DSLFail(TEXT("Missing ue_native_enum declaration for ")+P->Type.TypePath);
                auto* Type=new das::TypeDecl(das::Type::tEnumeration,F->at);Type->enumType=Match;return Type;
            }
            if(P->Type.Kind!=TEXT("object") && P->Type.Kind!=TEXT("class"))return nullptr;
            das::StructurePtr Match=nullptr;TSet<das::Module*> Seen;
            auto Inspect=[&](das::Module* Module){if(!Module || Seen.Contains(Module) || !Source->module->isVisibleDirectly(Module))return;Seen.Add(Module);
                Module->structures.foreach([&](const das::StructurePtr& S){
                    if(S->privateStructure && Module!=Source->module)return;
                    const auto* A=DSLAnnotation(S->annotations,P->Type.Kind==TEXT("class")?"ue_class_ref":"ue_native_object");
                    if(!A || DSLString(A->arguments,"path")!=P->Type.TypePath)return;
                    if(Match && Match!=S)DSLFail(TEXT("Ambiguous native self reference wrapper for ")+P->Type.TypePath+TEXT("; import one canonical wrapper"));Match=S;
                });
            };
            Inspect(Source->module);for(auto* Module:Group.getModules())Inspect(Module);
            if(!Match)DSLFail(TEXT("Missing native self reference wrapper for ")+P->Type.TypePath+TEXT("; import its typed wrapper module"));
            auto* Type=new das::TypeDecl(das::Type::tStructure,F->at);Type->structType=Match;return Type;
        }
        das::TypeDeclPtr RootFieldType(das::ExprField* F){
            if(!F)return nullptr;if(const auto* Field=Source->findField(F->name))return Field->type;
            if(!DSLNativeSelfKnown(Decl,F->name))return nullptr;const auto* P=Property(F);return P?SelfPropertyType(F,P):nullptr;
        }
        FString PropertyPath(const FDasDeclaredProperty* P){if(!Source->findField(DSLUtf8(P->Name)) && (P->Type.Kind==TEXT("array") || P->Type.Kind==TEXT("map") || P->Type.Kind==TEXT("set"))){auto* Carrier=DasContainerCarrierForProperty(DSLNativeSelfField(Decl,DSLUtf8(P->Name)));if(!Carrier)DSLFail(TEXT("Native container has no exact carrier"));return Carrier->GetPathName();}auto* Carrier=DasContainerCarrier(P->Type);return Carrier?Carrier->GetPathName():P->Type.TypePath;}
        das::ExpressionPtr Read(das::ExprField* F)
        {
            if(Decl.Components.ContainsByPredicate([&](const auto& C){return C.Name==DSLText(F->name);})){const auto* Field=Source->findField(F->name);return DSLReferenceValue(F->at,Field->type,DSLCall(F->at,"ue_get_self_object",{new das::ExprConstString(F->at,F->name)}));}const auto* P=Property(F);if(!P) return F;
            if(!Source->findField(F->name) && P->Name==TEXT("ActorTransform"))return DSLCall(F->at,"__ue_scene_world_transform",{DSLCall(F->at,"__ue_scene_actor_root",{DSLCall(F->at,"ue_self_object",{})})});
            if(!Source->findField(F->name) && DasActorNativeParent(Decl.NativeParent) && P->Name==TEXT("RootComponent"))return DSLDirectUnpack(F->at,SelfPropertyType(F,P),DSLCall(F->at,"__ue_scene_actor_root",{DSLCall(F->at,"ue_self_object",{})}),P->Type.TypePath);
            if(!Source->findField(F->name) && DasActorNativeParent(Decl.NativeParent) && P->Name==TEXT("Owner"))return DSLDirectUnpack(F->at,SelfPropertyType(F,P),DSLCall(F->at,"__ue_actor_owner",{DSLCall(F->at,"ue_self_object",{})}),P->Type.TypePath);
            if(P->Type.Kind==TEXT("delegate")){const auto* Field=Source->findField(F->name);return DSLDelegateFieldRead(F->at,Field->type,DSLCall(F->at,"ue_self_object",{}),P->Name);}
            if(P->Type.Kind==TEXT("event")){const auto* Field=Source->findField(F->name);return DSLDelegateView(F->at,Field->type,DSLCall(F->at,"ue_self_object",{}),P->Name);}
            auto* C=new das::ExprCall(F->at,P->Type.Kind==TEXT("byte")?"ue_get_self_byte":P->Type.Kind==TEXT("text")?"ue_get_self_text":P->Type.Kind==TEXT("class")?"ue_get_self_class":P->Type.Kind==TEXT("int")?"ue_get_self_int":P->Type.Kind==TEXT("float")?"ue_get_self_float":P->Type.Kind==TEXT("bool")?"ue_get_self_bool":P->Type.Kind==TEXT("double")?"ue_get_self_double":P->Type.Kind==TEXT("string")?"ue_get_self_string":(P->Type.Kind==TEXT("struct") || DasContainerCarrier(P->Type))?"ue_get_self_struct":P->Type.Kind==TEXT("object")?"ue_get_self_object":P->Type.Kind==TEXT("enum")?"ue_get_self_enum":"ue_get_self_name");
            C->arguments.push_back(new das::ExprConstString(F->at,DSLUtf8(P->Name)));const auto Type=SelfPropertyType(F,P);return Type && (DSLTypedClass(Type) || DSLContainer(Type) || DSLDirectEnum(Type) || DSLDirectStruct(Type) || DSLTypedReference(Type) || !DSLMathPath(Type).IsEmpty())?DSLDirectUnpack(F->at,Type,C,PropertyPath(P)):C;
        }
        das::ExpressionPtr Write(das::ExprOp2* E,das::ExprField* F,bool Compound)
        {
            const auto* P=Property(F);if(!P) return E;
            if(P->Type.Kind==TEXT("delegate")){if(P->bReadOnly){Error=TEXT("Cannot write readonly delegate field");return E;}if(Compound){Error=TEXT("Delegate fields only support assignment");return E;}const auto* Field=Source->findField(F->name);return DSLDelegateFieldWrite(E->at,Field->type,DSLCall(E->at,"ue_self_object",{}),P->Name,E->right);}if(P->Type.Kind==TEXT("event")){Error=TEXT("Cannot assign an event; use AddUFunction/RemoveUFunction");return E;}if(P->bReadOnly){Error=TEXT("Cannot write BlueprintReadOnly self property: ")+P->Name;return E;}
            if(!Source->findField(F->name) && P->Name==TEXT("ActorTransform")){if(Compound)DSLFail(TEXT("ActorTransform requires whole-value assignment"));return DSLCall(E->at,"__ue_scene_set_world_transform",{DSLCall(E->at,"__ue_scene_actor_root",{DSLCall(E->at,"ue_self_object",{})}),E->right,new das::ExprConstBool(E->at,false)});}
            auto* C=new das::ExprCall(E->at,P->Type.Kind==TEXT("byte")?"ue_set_self_byte":P->Type.Kind==TEXT("text")?"ue_set_self_text":P->Type.Kind==TEXT("class")?"ue_set_self_class":P->Type.Kind==TEXT("int")?"ue_set_self_int":P->Type.Kind==TEXT("float")?"ue_set_self_float":P->Type.Kind==TEXT("bool")?"ue_set_self_bool":P->Type.Kind==TEXT("double")?"ue_set_self_double":P->Type.Kind==TEXT("string")?"ue_set_self_string":(P->Type.Kind==TEXT("struct") || DasContainerCarrier(P->Type))?"ue_set_self_struct":P->Type.Kind==TEXT("object")?"ue_set_self_object":P->Type.Kind==TEXT("enum")?"ue_set_self_enum":"ue_set_self_name");
            C->arguments.push_back(new das::ExprConstString(E->at,DSLUtf8(P->Name)));
            auto* Value=Compound?static_cast<das::ExpressionPtr>(new das::ExprOp2(E->at,E->op.substr(0,E->op.size()-1),Read(F),E->right)):E->right;const auto* Field=Source->findField(DSLUtf8(P->Name));if(Field && DSLAuthoredView(Field->type)){if(Compound){Error=TEXT("Authored reference fields support assignment only");return E;}const das::string Name=DSLUtf8(TEXT("__ue_set_authored_field_")+FMD5::HashAnsiString(*(P->Type.AuthoredId+TEXT(":")+DSLText(Field->type->structType->module->name)+TEXT("::")+DSLText(Field->type->structType->name))));if(!Source->module->functionsByName.find(das::hash64z(Name.c_str()))){auto* Fn=new das::Function();Fn->at=E->at;Fn->name=Name;Fn->generated=true;Fn->privateFunction=false;Fn->result=new das::TypeDecl(das::Type::tVoid,E->at);for(const auto* ArgName:{"property","value"}){auto* Arg=new das::Variable();Arg->at=E->at;Arg->name=ArgName;Arg->type=Arg->name=="value"?DSLCopyType(Field->type):static_cast<das::TypeDeclPtr>(new das::TypeDecl(das::Type::tString,E->at));Arg->type->ref=false;Arg->type->constant=true;Fn->arguments.push_back(Arg);}auto* Block=new das::ExprBlock();Block->at=E->at;Block->list.push_back(DSLCall(E->at,"ue_set_self_object",{new das::ExprVar(E->at,"property"),DSLAuthoredPack(E->at,Field->type,new das::ExprVar(E->at,"value"))}));Fn->body=Block;if(!Source->module->addFunction(Fn,true))DSLFail(TEXT("Authored field setter helper collision"));}return DSLCall(E->at,Name,{new das::ExprConstString(E->at,DSLUtf8(P->Name)),Value});}const auto Type=SelfPropertyType(F,P);if(!Field && Compound && (P->Type.Kind==TEXT("object") || P->Type.Kind==TEXT("class")))DSLFail(TEXT("Native reference fields support assignment only"));
            // Keep a nominal parameter boundary before DSLDirectPack extracts
            // .handle. Native field assignment must not become an implicit cast.
            if(!Field && DSLTypedObject(Type)){
                const das::string Name=DSLUtf8(TEXT("__ue_native_reference_write_")+FMD5::HashAnsiString(*(P->Type.TypePath+TEXT(":")+DSLText(Type->structType->module->name)+TEXT("::")+DSLText(Type->structType->name))));
                if(!Source->module->functionsByName.find(das::hash64z(Name.c_str()))){auto* Fn=new das::Function();Fn->at=E->at;Fn->name=Name;Fn->generated=true;Fn->privateFunction=false;Fn->result=new das::TypeDecl(das::Type::tVoid,E->at);
                    for(const auto* ArgName:{"property","value"}){auto* Arg=new das::Variable();Arg->at=E->at;Arg->name=ArgName;Arg->type=Arg->name=="value"?DSLCopyType(Type):static_cast<das::TypeDeclPtr>(new das::TypeDecl(das::Type::tString,E->at));Arg->type->ref=false;Arg->type->constant=true;Fn->arguments.push_back(Arg);}
                    auto* Body=new das::ExprBlock();Body->at=E->at;Body->list.push_back(DSLCall(E->at,"ue_set_self_object",{new das::ExprVar(E->at,"property"),DSLDirectPack(E->at,Type,new das::ExprVar(E->at,"value"),PropertyPath(P))}));Fn->body=Body;if(!Source->module->addFunction(Fn,true))DSLFail(TEXT("Native reference setter helper collision"));
                }
                return DSLCall(E->at,Name,{new das::ExprConstString(E->at,DSLUtf8(P->Name)),Value});
            }
            C->arguments.push_back(Type && (DSLTypedClass(Type) || DSLContainer(Type) || DSLDirectEnum(Type) || DSLDirectStruct(Type) || DSLTypedReference(Type) || !DSLMathPath(Type).IsEmpty())?DSLDirectPack(E->at,Type,Value,PropertyPath(P)):Value);return C;
        }
        struct FNativeSceneRead{das::TypeDeclPtr Receiver;das::string Suffix;};TMap<const das::ExprField*,FNativeSceneRead> NativeSceneReads;
        das::string NativeSceneSuffix(das::Expression* E){if(!E || !E->rtti_isField())return {};auto* Field=static_cast<das::ExprField*>(E);auto* Root=SelfField(Field->value);if(!Root || Root->name!="RootComponent")return {};return Field->name=="RelativeLocation"?"translation":Field->name=="RelativeRotation"?"rotation":Field->name=="RelativeScale3D"?"scale":"";}
        virtual void preVisit(das::ExprField* F)override{const auto Suffix=NativeSceneSuffix(F);if(Suffix.empty())return;auto* Root=SelfField(F->value);const auto* P=Property(Root);auto* Native=FindFProperty<FStructProperty>(USceneComponent::StaticClass(),*DSLText(F->name));if(!P || !Native || !Native->HasAllPropertyFlags(CPF_BlueprintVisible|CPF_BlueprintReadOnly))DSLFail(TEXT("Native scene math property policy differs"));NativeSceneReads.Add(F,{SelfPropertyType(Root,P),Suffix});}
        das::ExprField* StructRoot(das::Expression* E){if(auto* F=SelfField(E)){const auto Type=RootFieldType(F);if(Type && DSLContainer(Type))return F;}for(int32 Depth=0;E && E->rtti_isField() && Depth<18;++Depth){auto* F=static_cast<das::ExprField*>(E);if(auto* Root=SelfField(F->value)){const auto Type=RootFieldType(Root);return Type && (DSLDirectStruct(Type) || !DSLMathPath(Type).IsEmpty())?Root:nullptr;}E=F->value;}return nullptr;}

        das::ExprField* IndexedRoot(das::Expression* E){
            for(int Depth=0;E && Depth<64;++Depth){
                if(auto* F=SelfField(E)){const auto Type=RootFieldType(F);return Type && (DSLDirectStruct(Type) || DSLContainer(Type))?F:nullptr;}
                if(E->rtti_isField())E=static_cast<das::ExprField*>(E)->value;
                else if(E->rtti_isAt())E=static_cast<das::ExprAt*>(E)->subexpr;
                else break;
            }return nullptr;
        }
        das::ExprAt* IndexedSelf(das::Expression* E){
            auto* Check=E;das::ExprAt* Index=nullptr;
            for(int Depth=0;Check && Depth<64;++Depth){if(SelfField(Check))break;
                if(Check->rtti_isAt()){if(!Index)Index=static_cast<das::ExprAt*>(Check);Check=static_cast<das::ExprAt*>(Check)->subexpr;}
                else if(Check->rtti_isField())Check=static_cast<das::ExprField*>(Check)->value;else break;
            }return Index && IndexedRoot(E)?Index:nullptr;
        }
        void MarkLeft(das::Expression* E){
            for(auto* Field=E;Field && Field->rtti_isField();Field=static_cast<das::ExprField*>(Field)->value)if(!NativeSceneSuffix(Field).empty())DSLFail(TEXT("Cannot write BlueprintReadOnly native component struct"));
            if(IndexedSelf(E)){for(auto* Current=E;Current;){
                if(Current->rtti_isField()){LeftFields.Add(Current);if(SelfField(Current))break;Current=static_cast<das::ExprField*>(Current)->value;}
                else if(Current->rtti_isAt())Current=static_cast<das::ExprAt*>(Current)->subexpr;else break;
            }return;}
            if(SelfField(E))LeftFields.Add(E);
            if(auto* Root=StructRoot(E))for(auto* Current=E;Current && Current->rtti_isField();){LeftFields.Add(Current);if(Current==Root)break;Current=static_cast<das::ExprField*>(Current)->value;}
        }
        das::ExpressionPtr MutateIndexed(das::Expression* Original,das::Expression* Target,das::ExprOp2* Assignment,das::ExprCall* Operation){
            auto* Root=IndexedRoot(Target);
            try{
                const auto* P=Property(Root);if(!P || P->bReadOnly)DSLFail(TEXT("Cannot write readonly self container"));
                auto RootType=RootFieldType(Root);auto Type=RootType;
                das::vector<das::Expression*> Steps;
                for(auto* Current=Target;Current!=Root;){Steps.push_back(Current);Current=Current->rtti_isField()?static_cast<das::ExprField*>(Current)->value:static_cast<das::ExprAt*>(Current)->subexpr;}
                const das::string Name="__ue_index_write_"+das::to_string(Source->module->functions.unlocked_size())+"_"+das::to_string(Source->module->generics.unlocked_size())+"_"+das::to_string(NextIndexedHelper++);
                das::FunctionPtr Fn=new das::Function();Fn->at=Original->at;Fn->name=Name;Fn->generated=true;Fn->privateFunction=false;
                auto* Call=DSLCall(Original->at,Name,{new das::ExprConstString(Original->at,Root->name)});
                auto Add=[&](const das::string& N,const das::TypeDeclPtr& T,das::ExpressionPtr Input){auto* A=new das::Variable();A->at=Original->at;A->name=N;A->type=DSLCopyType(T);A->type->ref=false;A->type->constant=true;Fn->arguments.push_back(A);if(Input)Call->arguments.push_back(Input);};
                auto IndexType=[&](){auto* T=new das::TypeDecl(das::Type::option,Original->at);T->argTypes.push_back(new das::TypeDecl(das::Type::tInt,Original->at));T->argTypes.push_back(new das::TypeDecl(das::Type::tInt64,Original->at));return T;};
                Add("property",new das::TypeDecl(das::Type::tString,Original->at),nullptr);
                das::ExpressionPtr Left=new das::ExprVar(Original->at,"snapshot");
                for(auto It=Steps.rbegin();It!=Steps.rend();++It){auto* Step=*It;
                    if(Step->rtti_isField()){
                        const auto N=static_cast<das::ExprField*>(Step)->name;
                        if(!DSLMathPath(Type).IsEmpty()){
                            const auto Math=DSLMathPath(Type);
                            if(!((Math==TEXT("/Script/CoreUObject.Vector") && (N=="x" || N=="y" || N=="z")) || (Math==TEXT("/Script/CoreUObject.Rotator") && (N=="pitch" || N=="yaw" || N=="roll"))))DSLFail(TEXT("Unsupported indexed math member"));
                            Type=new das::TypeDecl(das::Type::tDouble,Original->at);
                        }else {
                            if(!DSLDirectStruct(Type))DSLFail(TEXT("Indexed write crosses unsupported field type"));
                            const auto* F=Type->structType->findField(N);if(!F)DSLFail(TEXT("Unknown indexed member"));
                            for(const auto& A:F->annotation)if(A.name=="ue_property" && A.type==das::Type::tString && A.sValue=="BlueprintReadOnly")DSLFail(TEXT("Cannot write readonly indexed member"));
                            Type=F->type;
                        }
                        Left=new das::ExprField(Original->at,Left,N);
                    }else {
                        if(!DSLContainer(Type) || DSLSet(Type))DSLFail(TEXT("Indexed assignment requires array or map"));
                        const bool Array=Type->baseType==das::Type::tArray;const auto N="index_"+das::to_string(Fn->arguments.size());
                        Add(N,Array?static_cast<das::TypeDeclPtr>(IndexType()):Type->firstType,static_cast<das::ExprAt*>(Step)->index);
                        Left=new das::ExprAt(Original->at,Left,new das::ExprVar(Original->at,N));Type=Array?Type->firstType:Type->secondType;
                    }
                }
                bool Result=false;das::ExpressionPtr Mutation;
                if(Operation){
                    if(!DSLContainer(Type))DSLFail(TEXT("Indexed operation requires supported container"));
                    const bool Array=Type->baseType==das::Type::tArray,Set=DSLSet(Type),Push=Operation->name=="push" || Operation->name=="push_clone";const auto Count=Operation->arguments.size();
                    if(Operation->name=="clear"?Count!=1:Operation->name=="pop"?(!Array || Count!=1):Push?(!Array || (Count!=2 && Count!=3)):(Operation->name=="insert" || Operation->name=="insert_clone")?(Array || (Operation->name=="insert_clone" && Set) || Count!=(Set?2:3)):(Array?(Count!=2 && Count!=3):Count!=2))DSLFail(TEXT("Unsupported indexed container operation/arity"));
                    auto* Op=DSLCall(Original->at,Operation->name,{Left});
                    for(size_t I=1;I<Count;++I){const auto N="arg_"+das::to_string(I);Add(N,Array?(Push && I==1?Type->firstType:static_cast<das::TypeDeclPtr>(IndexType())):I==1?Type->firstType:Type->secondType,Operation->arguments[I]);Op->arguments.push_back(new das::ExprVar(Original->at,N));}
                    Mutation=Op;Result=Operation->name=="erase" && !Array;
                }else {
                    Add("value",Type,Assignment->right);auto* Value=new das::ExprVar(Original->at,"value");
                    if(Assignment->rtti_isCopy())Mutation=new das::ExprCopy(Original->at,Left,Value);
                    else if(Assignment->rtti_isClone())Mutation=new das::ExprClone(Original->at,Left,Value);
                    else if(Assignment->__rtti==das::string("ExprMove"))DSLFail(TEXT("Indexed native writes use copy or clone, not consuming move"));
                    else Mutation=new das::ExprOp2(Original->at,Assignment->op,Left,Value);
                }
                Fn->result=new das::TypeDecl(Result?das::Type::tBool:das::Type::tVoid,Original->at);
                auto* Body=new das::ExprBlock();Body->at=Original->at;
                auto Local=[&](const das::string& N,const das::TypeDeclPtr& T,das::ExpressionPtr Init,bool Move){auto* V=new das::Variable();V->at=Original->at;V->name=N;V->type=DSLCopyType(T);V->type->ref=false;V->type->constant=false;V->init=Init;V->init_via_move=Move;auto* L=new das::ExprLet();L->at=Original->at;L->variables.push_back(V);Body->list.push_back(L);};
                Local("snapshot",RootType,DSLDirectUnpack(Original->at,RootType,DSLCall(Original->at,"ue_get_self_struct",{new das::ExprVar(Original->at,"property")}),PropertyPath(P)),true);
                if(Result)Local("result",Fn->result,Mutation,false);else Body->list.push_back(Mutation);
                Body->list.push_back(DSLCall(Original->at,"ue_set_self_struct",{new das::ExprVar(Original->at,"property"),DSLDirectPack(Original->at,RootType,new das::ExprVar(Original->at,"snapshot"),PropertyPath(P))}));
                if(Result)Body->list.push_back(new das::ExprReturn(Original->at,new das::ExprVar(Original->at,"result")));
                Fn->body=Body;if(!(Fn->isGeneric()?Source->module->addGeneric(Fn,true):Source->module->addFunction(Fn,true)))DSLFail(TEXT("Indexed mutation helper collision"));return Call;
            }catch(const std::exception& X){Error=UTF8_TO_TCHAR(X.what());return Original;}
        }
        das::ExpressionPtr WriteIndexed(das::ExprOp2* E,bool){return MutateIndexed(E,E->left,E,nullptr);}
        uint32 NextIndexedHelper=0;
        das::ExpressionPtr WriteLeaf(das::ExprOp2* E,bool Compound){auto* Leaf=static_cast<das::ExprField*>(E->left);auto* Root=StructRoot(Leaf);if(Root && Root->name=="ActorTransform" && !Source->findField(Root->name))DSLFail(TEXT("ActorTransform requires whole-value assignment"));const auto* P=Property(Root);if(!P)return E;if(P->bReadOnly){Error=TEXT("Cannot write BlueprintReadOnly self struct: ")+P->Name;return E;}das::vector<das::string> Names;for(auto* Current=Leaf;Current!=Root;Current=static_cast<das::ExprField*>(Current->value))Names.push_back(Current->name);auto Type=RootFieldType(Root);das::string Path;
            for(auto It=Names.rbegin();It!=Names.rend();++It){if(!DSLMathPath(Type).IsEmpty()){const bool Vector=DSLMathPath(Type)==TEXT("/Script/CoreUObject.Vector");const bool Rotator=DSLMathPath(Type)==TEXT("/Script/CoreUObject.Rotator");if(It+1!=Names.rend() || (Vector?(*It!="x" && *It!="y" && *It!="z"):Rotator?(*It!="pitch" && *It!="yaw" && *It!="roll"):true)){Error=TEXT("Math scalar writes support Vector x/y/z and Rotator pitch/yaw/roll");return E;}if(!Path.empty())Path+=".";Path+=*It;Type=new das::TypeDecl(das::Type::tDouble,E->at);break;}if(!DSLDirectStruct(Type)){Error=TEXT("Nested write crosses non-authored struct");return E;}const auto* Field=Type->structType->findField(*It);if(!Field){Error=TEXT("Unknown self struct field");return E;}for(const auto& A:Field->annotation)if(A.name=="ue_property" && A.type==das::Type::tString && A.sValue=="BlueprintReadOnly"){Error=TEXT("Cannot write BlueprintReadOnly struct member");return E;}if(!Path.empty())Path+=".";Path+=*It;Type=Field->type;}
            const FString K=DSLTypeName(Type);if(DSLDirectEnum(Type) || DSLTypedReference(Type) || DSLDirectStruct(Type) || !DSLMathPath(Type).IsEmpty() || DSLContainer(Type)){if(Compound){Error=TEXT("Typed value struct members support assignment, not compound arithmetic");return E;}const das::string Kind=DSLDirectEnum(Type)?"enum":DSLTypedReference(Type)?"object":"struct";const FString ValuePath=DSLDirectEnum(Type) || DSLDirectStruct(Type) || DSLContainer(Type)?DSLDirectPath(Type,Folder,Registry):DSLTypedReference(Type)?DSLTypedReferencePath(Type):DSLMathPath(Type);if(DSLContainer(Type))DSLEnsureStructHelpers(Type,ValuePath,Group,Source->module,GeneratedHelpers,HelperPaths);const das::string Name=DSLUtf8(TEXT("__ue_commit_typed_leaf_")+FMD5::HashAnsiString(*(DSLText(Kind)+TEXT(":")+ValuePath)));if(!Source->module->functionsByName.find(das::hash64z(Name.c_str()))){auto* F=new das::Function();F->at=E->at;F->name=Name;F->generated=true;F->privateFunction=false;F->result=new das::TypeDecl(das::Type::tVoid,E->at);for(const auto* ArgName:{"property","member","value"}){auto* Arg=new das::Variable();Arg->at=E->at;Arg->name=ArgName;Arg->type=Arg->name=="value"?DSLCopyType(Type):static_cast<das::TypeDeclPtr>(new das::TypeDecl(das::Type::tString,E->at));Arg->type->ref=false;Arg->type->constant=true;F->arguments.push_back(Arg);}auto* Body=new das::ExprBlock();Body->at=E->at;Body->list.push_back(DSLCall(E->at,"ue_dsl_commit_struct_member_"+Kind,{new das::ExprVar(E->at,"property"),new das::ExprVar(E->at,"member"),DSLDirectPack(E->at,Type,new das::ExprVar(E->at,"value"),ValuePath)}));F->body=Body;if(!Source->module->addFunction(F,true))DSLFail(TEXT("Typed leaf commit helper collision"));}return DSLCall(E->at,Name,{new das::ExprConstString(E->at,Root->name),new das::ExprConstString(E->at,Path),E->right});}if(DSLContainer(Type) || K==TEXT("struct") || K==TEXT("enum") || K==TEXT("object") || K.IsEmpty()){Error=TEXT("Nested member assignment supports scalar leaves; copy and assign the whole self property for nested values");return E;}const char* Suffix=DSLScalarSuffix(K);auto Token=[&](){return DSLCall(E->at,"ue_get_self_struct",{new das::ExprConstString(E->at,Root->name)});};auto* Value=Compound?static_cast<das::ExpressionPtr>(new das::ExprOp2(E->at,E->op.substr(0,E->op.size()-1),DSLCall(E->at,das::string("ue_struct_get_")+Suffix,{Token(),new das::ExprConstString(E->at,Path)}),E->right)):E->right;return DSLCall(E->at,das::string("ue_dsl_commit_struct_member_")+Suffix,{new das::ExprConstString(E->at,Root->name),new das::ExprConstString(E->at,Path),Value});}
        bool ContainerOperation(const das::string& N){return N=="insert" || N=="insert_clone" || N=="erase" || N=="clear" || N=="push" || N=="push_clone" || N=="pop";}
        bool OwnMethod(das::ExprField* Field)const {
            const auto* Member=Field?Source->findField(Field->name):nullptr;
            return (Member && Member->classMethod) || (Field && LocalTargets.Contains(DSLText(Field->name)));
        }
        das::ExprField* NativeSelfMethod(das::Expression* E){
            if(!E || !E->rtti_isField())return nullptr;auto* F=static_cast<das::ExprField*>(E);if(!F->value || !F->value->rtti_isVar() || static_cast<das::ExprVar*>(F->value)->name!=Self || Source->findField(F->name) || !DasNativeGameplayMethod(Decl.NativeParent,DSLText(F->name)))return nullptr;return F;
        }
        das::ExpressionPtr NativeSelfCall(das::ExprInvoke* E){
            if(!E->isInvokeMethod || E->arguments.empty())return E;auto* F=NativeSelfMethod(E->arguments[0]);if(!F)return E;
            try{const TCHAR* View=nullptr;for(const auto& V:DasNativeGameplayViews)if(Decl.NativeParent==V.Class){View=V.Source;break;}if(!View)DSLFail(TEXT("Missing generated native self view"));auto* Receiver=DSLCall(E->at,DSLUtf8(View)+"_from",{DSLCall(E->at,"ue_self_object",{})});auto* Call=new das::ExprInvoke();Call->at=E->at;Call->isInvokeMethod=true;Call->arguments.push_back(new das::ExprField(E->at,Receiver,F->name));for(size_t I=1;I<E->arguments.size();++I)Call->arguments.push_back(E->arguments[I]);return Call;}catch(const std::exception& X){Error=UTF8_TO_TCHAR(X.what());return E;}
        }
        virtual void preVisit(das::ExprInvoke* E)override {
            if(E->isInvokeMethod && !E->arguments.empty())if(auto* F=NativeSelfMethod(E->arguments[0]))LeftFields.Add(F);
            if(E->isInvokeMethod && !E->arguments.empty())if(auto* Field=SelfField(E->arguments[0]);OwnMethod(Field))LeftFields.Add(Field);
        }
        das::ExpressionPtr SuperCall(das::ExprInvoke* E){if(!E->isInvokeMethod || E->arguments.empty() || !E->arguments[0]->rtti_isField())return E;auto* Field=static_cast<das::ExprField*>(E->arguments[0]);if(!Field->value->rtti_isVar() || static_cast<das::ExprVar*>(Field->value)->name!="Super")return E;if(const auto* Target=LocalSupers.Find(DSLText(Field->name))){auto* Call=DSLCall(E->at,*Target,{});for(size_t I=2;I<E->arguments.size();++I)Call->arguments.push_back(E->arguments[I]);return Call;}if(!SuperReference){Error=TEXT("Super requires an authored parent");return E;}if(Field->name=="ConstructionScript" || Field->name=="Initialize" || Field->name=="BeginPlay" || Field->name=="Tick" || Field->name=="EndPlay"){const int Kind=Field->name=="BeginPlay"?0:Field->name=="Tick"?1:Field->name=="EndPlay"?2:Field->name=="Initialize"?3:4;const size_t Count=(Kind==0 || Kind==3 || Kind==4)?2:3;if(E->arguments.size()!=Count){Error=TEXT("Super lifecycle argument count mismatch");return E;}return DSLCall(E->at,"__ue_parent_lifecycle",{new das::ExprConstString(E->at,DSLUtf8(Decl.Id)),new das::ExprConstInt(E->at,Kind),Kind==1?E->arguments[2]:static_cast<das::ExpressionPtr>(new das::ExprConstFloat(E->at,0.f)),Kind==2?E->arguments[2]:static_cast<das::ExpressionPtr>(new das::ExprConstInt(E->at,0))});}auto* Call=new das::ExprInvoke();Call->at=E->at;Call->isInvokeMethod=true;auto* Receiver=DSLReferenceValue(E->at,SuperReference,DSLCall(E->at,"ue_self_object",{}));Call->arguments.push_back(new das::ExprField(E->at,Receiver,Field->name));for(size_t I=1;I<E->arguments.size();++I)Call->arguments.push_back(E->arguments[I]);return Call;}
        virtual das::ExpressionPtr visit(das::ExprInvoke* E)override {
            auto Super=SuperCall(E);if(Super!=E)return Super;auto NativeCall=NativeSelfCall(E);if(NativeCall!=E)return NativeCall;
            if(!E->isInvokeMethod || E->arguments.empty())return E;
            auto* Field=SelfField(E->arguments[0]);if(!OwnMethod(Field))return E;
            const FString Name=DSLText(Field->name);
            const auto* Method=Decl.Methods.FindByPredicate([&](const auto& M){return M.Name==Name;});
            if(Method && (DSLVirtualMethod(Decl,*Method) || Method->ScriptFile!=FPaths::GetCleanFilename(DSLText(Source->at.fileInfo->name)))){
                if(!SelfReference){Error=TEXT("Missing self event reference");return E;}
                auto* Call=new das::ExprInvoke();Call->at=E->at;Call->isInvokeMethod=true;
                auto* Receiver=DSLReferenceValue(E->at,SelfReference,DSLCall(E->at,"ue_self_object",{}));
                Call->arguments.push_back(new das::ExprField(E->at,Receiver,Field->name));
                for(size_t I=1;I<E->arguments.size();++I)Call->arguments.push_back(E->arguments[I]);return Call;
            }
            const auto* Local=LocalTargets.Find(Name);const auto Target=Method?DSLUtf8(TEXT("__impl_")+Method->EntryPoint):Local?*Local:DSLUtf8(TEXT("__ue_local_")+Decl.Name+TEXT("_")+Name);
            auto* Call=new das::ExprCall(E->at,Target);
            for(size_t I=2;I<E->arguments.size();++I)Call->arguments.push_back(E->arguments[I]);
            return Call;
        }
        virtual void preVisit(das::ExprCall* E) override {if((ContainerOperation(E->name)) && !E->arguments.empty() && (StructRoot(E->arguments[0]) || IndexedSelf(E->arguments[0])))MarkLeft(E->arguments[0]);if((E->name=="keys" || E->name=="values") && !E->arguments.empty() && (SelfField(E->arguments[0]) || StructRoot(E->arguments[0])))Error=TEXT("Container iterators over self require a local clone");}
        das::ExpressionPtr RewriteTimer(das::ExprCall* E){
            const bool NextTick=E->name=="ue_timer_next_tick" || E->name=="ue_foundation::ue_timer_next_tick";const size_t BaseCount=NextTick?2:4,HandlerIndex=NextTick?1:2,PayloadIndex=NextTick?2:4;const bool HasArgument=E->arguments.size()==BaseCount+1;
            if(E->arguments.size()>=BaseCount+2){try{return DSLMultiTimer(E,Source,Decl,Folder,Registry,Group,GeneratedHelpers,HelperPaths,NextTick);}catch(const std::exception& X){Error=UTF8_TO_TCHAR(X.what());return E;}}
            if((E->arguments.size()!=BaseCount && !HasArgument) || E->arguments[HandlerIndex]->__rtti!=das::string("ExprConstString")){Error=TEXT("Class timer requires a literal callback method name");return E;}
            auto* Name=static_cast<das::ExprConstString*>(E->arguments[HandlerIndex]);const FString MethodName=DSLText(Name->text);bool Found=false;das::TypeDeclPtr CaptureType=nullptr;
            Source->module->functions.foreach([&](const das::FunctionPtr& F){if(F->classParent!=Source)return;const FString N=DSLText(F->name.substr(F->name.find_last_of(das::string(1,char(96))+das::string(1,char(39)))+1));if(N!=MethodName || !DSLAnnotation(F->annotations,"ue_function") || F->isStaticClassMethod || !F->result || F->result->baseType!=das::Type::tVoid || F->arguments.size()!=(HasArgument?2u:1u))return;if(!HasArgument){Found=true;return;}const auto T=F->arguments[1]->type;const auto* Capability=FindDasCallbackType(DSLCallbackKind(T));Found=T && !T->ref && !F->arguments[1]->init && Capability && Capability->bTimer && Capability->Signature!=EDasMethodSignature::Void;if(Found && Capability->bDynamic){const bool Class=DSLTypedClass(T),Object=DSLTypedReference(T),Enum=DSLDirectEnum(T),Struct=DSLDirectStruct(T) || DSLContainer(T);FString Issue;Found=(Class || Object || Enum || Struct) && ValidateDasCallbackTypePath(Class?TEXT("class"):Object?TEXT("object"):Enum?TEXT("enum"):TEXT("struct"),Class?DSLTypedClassPath(T):Object?DSLTypedReferencePath(T):DSLDirectPath(T,Folder,Registry),Issue);if(!Found && !Issue.IsEmpty())Error=Issue;}if(Found)CaptureType=T;});
            if(!Found){Error=TEXT("Timer callback requires an annotated void instance method matching zero/one supported value argument");return E;}Name->text=DSLUtf8(TEXT("ue_")+Decl.Name+TEXT("_")+MethodName);
            if(CaptureType && DSLTypedClass(CaptureType)){const FString Path=DSLTypedClassPath(CaptureType);E->name=NextTick?"__ue_timer_next_tick_class":"__ue_timer_start_class";E->arguments[PayloadIndex]=DSLClassPack(E->at,CaptureType,E->arguments[PayloadIndex]);E->arguments.push_back(new das::ExprConstString(E->at,DSLUtf8(Path)));}else if(CaptureType && DSLTypedReference(CaptureType)){const FString Path=DSLTypedReferencePath(CaptureType);E->name=NextTick?"__ue_timer_next_tick_object":"__ue_timer_start_object";E->arguments[PayloadIndex]=DSLDirectPack(E->at,CaptureType,E->arguments[PayloadIndex],Path);E->arguments.push_back(new das::ExprConstString(E->at,DSLUtf8(Path)));}else if(CaptureType && (DSLContainer(CaptureType) || DSLDirectStruct(CaptureType))){const FString Path=DSLDirectPath(CaptureType,Folder,Registry);DSLEnsureStructHelpers(CaptureType,Path,Group,Source->module,GeneratedHelpers,HelperPaths);E->name=NextTick?"__ue_timer_next_tick_array":"__ue_timer_start_array";E->arguments[PayloadIndex]=DSLDirectPack(E->at,CaptureType,E->arguments[PayloadIndex],Path);}else if(CaptureType && DSLDirectEnum(CaptureType)){const FString Path=DSLDirectPath(CaptureType,Folder,Registry);E->arguments[PayloadIndex]=DSLDirectPack(E->at,CaptureType,E->arguments[PayloadIndex],Path);}return E;
        }
        das::ExpressionPtr RewriteInput(das::ExprCall* E){
            const bool Axis=E->name=="ue_input_bind_axis" || E->name=="ue_foundation::ue_input_bind_axis";const size_t HandlerIndex=Axis?1:2;if(E->arguments.size()!=(Axis?4u:5u) || E->arguments[HandlerIndex]->__rtti!=das::string("ExprConstString")){Error=TEXT("Class input binding requires literal callback name and consume/paused flags");return E;}auto* Name=static_cast<das::ExprConstString*>(E->arguments[HandlerIndex]);const FString Method=DSLText(Name->text);bool Found=false;Source->module->functions.foreach([&](const das::FunctionPtr& F){if(F->classParent!=Source)return;const FString N=DSLText(F->name.substr(F->name.find_last_of(das::string(1,char(96))+das::string(1,char(39)))+1));if(N!=Method || !DSLAnnotation(F->annotations,"ue_function") || F->isStaticClassMethod || !F->result || F->result->baseType!=das::Type::tVoid || F->arguments.size()!=(Axis?2u:1u))return;Found=!Axis || (F->arguments[1]->type && !F->arguments[1]->type->ref && !F->arguments[1]->init && DSLTypeName(F->arguments[1]->type)==TEXT("float"));});if(!Found){Error=TEXT("Input requires annotated instance action():void or axis(value:float):void");return E;}Name->text=DSLUtf8(TEXT("ue_")+Decl.Name+TEXT("_")+Method);return E;
        }
        das::ExpressionPtr RewriteEnhancedInput(das::ExprCall* E){
            if(E->arguments.size()!=3 || E->arguments[2]->__rtti!=das::string("ExprConstString")){Error=TEXT("Enhanced Input class binding requires a literal callback name");return E;}auto* Name=static_cast<das::ExprConstString*>(E->arguments[2]);const FString Method=DSLText(Name->text);bool Found=false;Source->module->functions.foreach([&](const das::FunctionPtr& F){if(F->classParent!=Source)return;const FString N=DSLText(F->name.substr(F->name.find_last_of(das::string(1,char(96))+das::string(1,char(39)))+1));if(N!=Method || !DSLAnnotation(F->annotations,"ue_function") || F->isStaticClassMethod || !F->result || F->result->baseType!=das::Type::tVoid || F->arguments.size()!=2)return;const auto T=F->arguments[1]->type;Found=T && !T->ref && !F->arguments[1]->init && (DSLTypeName(T)==TEXT("bool") || DSLTypeName(T)==TEXT("float") || (DSLMathPath(T)==TEXT("/Script/EnhancedInput.InputActionValue") || DSLMathPath(T)==TEXT("/Script/CoreUObject.Vector") || DSLMathPath(T)==TEXT("/Script/CoreUObject.Vector2D")));});if(!Found){Error=TEXT("Enhanced Input requires annotated void handler(value:InputActionValue/bool/float/UEVector2D/UEVector)");return E;}Name->text=DSLUtf8(TEXT("ue_")+Decl.Name+TEXT("_")+Method);return E;
        }
        das::ExpressionPtr RewriteLatent(das::ExprCall* E){if(E->arguments.size()!=3 || E->arguments[2]->__rtti!=das::string("ExprConstString")){Error=TEXT("Class latent delay requires literal callback name");return E;}auto* Name=static_cast<das::ExprConstString*>(E->arguments[2]);const FString Method=DSLText(Name->text);bool Found=false;Source->module->functions.foreach([&](const das::FunctionPtr& F){if(F->classParent!=Source)return;const FString N=DSLText(F->name.substr(F->name.find_last_of(das::string(1,char(96))+das::string(1,char(39)))+1));if(N==Method && DSLAnnotation(F->annotations,"ue_function") && !F->isStaticClassMethod && F->result && F->result->baseType==das::Type::tVoid && F->arguments.size()==1)Found=true;});if(!Found){Error=TEXT("Latent completion requires annotated instance method():void");return E;}Name->text=DSLUtf8(TEXT("ue_")+Decl.Name+TEXT("_")+Method);return E;}
        das::ExpressionPtr RewriteEffect(das::ExprCall* E){
            const bool Single=E->name=="ue_sound_play" || E->name=="ue_effects::ue_sound_play" || E->name=="ue_niagara_spawn" || E->name=="ue_effects::ue_niagara_spawn";const size_t Index=Single?3:4;if(E->arguments.size()<=Index)return E;
            if(E->arguments[Index]->__rtti!=das::string("ExprConstString")){Error=TEXT("Effect completion requires a literal callback name");return E;}auto* Name=static_cast<das::ExprConstString*>(E->arguments[Index]);if(Name->text.empty())return E;bool Found=false;
            Source->module->functions.foreach([&](const das::FunctionPtr& F){if(F->classParent==Source && DSLAnnotation(F->annotations,"ue_function") && !F->isStaticClassMethod && F->result && F->result->baseType==das::Type::tVoid && F->arguments.size()==1 && F->name.substr(F->name.find_last_of(das::string(1,char(96))+das::string(1,char(39)))+1)==Name->text)Found=true;});
            if(!Found){Error=TEXT("Effect completion requires ue_function void Handler()");return E;}Name->text=DSLUtf8(TEXT("ue_")+Decl.Name+TEXT("_")+DSLText(Name->text));return E;
        }
        das::ExpressionPtr RewriteAIMove(das::ExprCall* E){
            if(E->arguments.size()<4)return E;
            if(E->arguments[3]->__rtti==das::string("ExprConstString") && static_cast<das::ExprConstString*>(E->arguments[3])->text.empty())return E;
            if(E->arguments.size()<4 || E->arguments[3]->__rtti!=das::string("ExprConstString")){Error=TEXT("MoveTo requires a literal callback method name");return E;}
            auto* Name=static_cast<das::ExprConstString*>(E->arguments[3]);bool Found=false;
            Source->module->functions.foreach([&](const das::FunctionPtr& F){if(F->classParent!=Source || !DSLAnnotation(F->annotations,"ue_function") || F->isStaticClassMethod || !F->result || F->result->baseType!=das::Type::tVoid || F->arguments.size()!=2)return;const FString N=DSLText(F->name.substr(F->name.find_last_of(das::string(1,char(96))+das::string(1,char(39)))+1));const auto T=F->arguments[1]->type;if(N==DSLText(Name->text) && T && !T->ref && !F->arguments[1]->init && DSLDirectStruct(T) && DSLDirectPath(T,Folder,Registry)==TEXT("/Script/DasRuntime.DasMoveResult"))Found=true;});
            if(!Found){Error=TEXT("MoveTo callback requires ue_function void(result:MoveResult)");return E;}
            Name->text=DSLUtf8(TEXT("ue_")+Decl.Name+TEXT("_")+DSLText(Name->text));return E;
        }
        das::ExpressionPtr RewriteMontage(das::ExprCall* E){
            if(E->arguments.size()<4 || E->arguments[3]->__rtti!=das::string("ExprConstString")){Error=TEXT("Montage requires a literal callback method name");return E;}
            auto* Name=static_cast<das::ExprConstString*>(E->arguments[3]);bool Found=false;
            Source->module->functions.foreach([&](const das::FunctionPtr& F){if(F->classParent!=Source || !DSLAnnotation(F->annotations,"ue_function") || F->isStaticClassMethod || !F->result || F->result->baseType!=das::Type::tVoid || F->arguments.size()!=2)return;const FString N=DSLText(F->name.substr(F->name.find_last_of(das::string(1,char(96))+das::string(1,char(39)))+1));const auto T=F->arguments[1]->type;if(N==DSLText(Name->text) && T && !T->ref && !F->arguments[1]->init && DSLDirectStruct(T) && DSLDirectPath(T,Folder,Registry)==TEXT("/Script/DasRuntime.DasMontageEvent"))Found=true;});
            if(!Found){Error=TEXT("Montage callback requires ue_function void(event:MontageEvent)");return E;}
            Name->text=DSLUtf8(TEXT("ue_")+Decl.Name+TEXT("_")+DSLText(Name->text));return E;
        }
        das::ExpressionPtr RewriteAssetLoad(das::ExprCall* E){if(E->arguments.size()!=3 || E->arguments[2]->__rtti!=das::string("ExprConstString")){Error=TEXT("Class async load requires a literal callback name");return E;}auto* Name=static_cast<das::ExprConstString*>(E->arguments[2]);const FString Method=DSLText(Name->text);FString Path;Source->module->functions.foreach([&](const das::FunctionPtr& F){if(F->classParent!=Source)return;const FString N=DSLText(F->name.substr(F->name.find_last_of(das::string(1,char(96))+das::string(1,char(39)))+1));if(N!=Method || !DSLAnnotation(F->annotations,"ue_function") || F->isStaticClassMethod || !F->result || F->result->baseType!=das::Type::tVoid || F->arguments.size()!=2)return;const auto T=F->arguments[1]->type;if(T && !T->ref && !F->arguments[1]->init && DSLTypedObject(T))Path=DSLTypedObjectPath(T);});if(Path.IsEmpty()){Error=TEXT("Async asset load requires annotated void callback with typed native object parameter");return E;}Name->text=DSLUtf8(TEXT("ue_")+Decl.Name+TEXT("_")+Method);E->name="__ue_asset_load_async";E->arguments.push_back(new das::ExprConstString(E->at,DSLUtf8(Path)));return E;}
        virtual das::ExpressionPtr visit(das::ExprCall* E) override {if(const auto* Local=QualifiedLocals.Find(DSLText(E->name));Local && !E->arguments.empty() && E->arguments[0]->rtti_isVar() && static_cast<das::ExprVar*>(E->arguments[0])->name==Self){auto* Call=DSLCall(E->at,*Local,{});for(size_t I=1;I<E->arguments.size();++I)Call->arguments.push_back(E->arguments[I]);return Call;}if(const auto* Renamed=LocalRenames.Find(DSLText(E->name)))E->name=*Renamed;if(E->name=="ue_soft_load_async" || E->name=="ue_soft_references::ue_soft_load_async"){if(E->arguments.size()!=3 || E->arguments[2]->__rtti!=das::string("ExprConstString"))DSLFail(TEXT("Soft async callback requires a literal method name"));auto* Name=static_cast<das::ExprConstString*>(E->arguments[2]);bool Found=false;Source->module->functions.foreach([&](const das::FunctionPtr& F){if(F->classParent==Source && F->name.substr(F->name.find_last_of(das::string(1,char(96))+das::string(1,char(39)))+1)==Name->text && DSLAnnotation(F->annotations,"ue_function") && F->result && F->result->baseType==das::Type::tVoid && F->arguments.size()==2 && !F->arguments[1]->type->ref && (DSLTypedClass(F->arguments[1]->type) || DSLTypedObject(F->arguments[1]->type)))Found=true;});if(!Found)DSLFail(TEXT("Soft async handler requires an annotated void method with typed object/class input"));Name->text=DSLUtf8(TEXT("ue_")+Decl.Name+TEXT("_")+DSLText(Name->text));return E;}if(E->name=="ue_effect_play" || E->name=="ue_effects::ue_effect_play" || E->name=="ue_effect_at" || E->name=="ue_effects::ue_effect_at" || E->name=="ue_sound_play" || E->name=="ue_effects::ue_sound_play" || E->name=="ue_niagara_spawn" || E->name=="ue_effects::ue_niagara_spawn")return RewriteEffect(E);if(E->name=="ue_ai_move_to_actor" || E->name=="ue_ai::ue_ai_move_to_actor" || E->name=="ue_ai_move_to_location" || E->name=="ue_ai::ue_ai_move_to_location")return RewriteAIMove(E);if(E->name=="ue_montage_play" || E->name=="ue_animation::ue_montage_play")return RewriteMontage(E);if(E->name=="ue_asset_load_async" || E->name=="ue_foundation::ue_asset_load_async")return RewriteAssetLoad(E);if(E->name=="ue_latent_delay" || E->name=="ue_foundation::ue_latent_delay")return RewriteLatent(E);if(E->name=="ue_enhanced_bind" || E->name=="ue_enhanced_input::ue_enhanced_bind")return RewriteEnhancedInput(E);if(E->name=="ue_input_bind_action" || E->name=="ue_foundation::ue_input_bind_action" || E->name=="ue_input_bind_axis" || E->name=="ue_foundation::ue_input_bind_axis")return RewriteInput(E);if(E->name=="ue_timer_start" || E->name=="ue_foundation::ue_timer_start" || E->name=="ue_timer_next_tick" || E->name=="ue_foundation::ue_timer_next_tick")return RewriteTimer(E);if(!ContainerOperation(E->name) || E->arguments.empty())return E;if(IndexedSelf(E->arguments[0]))return MutateIndexed(E,E->arguments[0],nullptr,E);auto* Root=StructRoot(E->arguments[0]);if(!Root)return E;try{const auto* P=Property(Root);if(!P || P->bReadOnly)DSLFail(TEXT("Cannot mutate readonly self container"));das::vector<das::string> Names;for(auto* Current=E->arguments[0];Current!=Root;Current=static_cast<das::ExprField*>(Current)->value)Names.push_back(static_cast<das::ExprField*>(Current)->name);auto Type=RootFieldType(Root);das::string Path;for(auto It=Names.rbegin();It!=Names.rend();++It){if(!DSLDirectStruct(Type))DSLFail(TEXT("Container mutation path must cross authored structs"));const auto* F=Type->structType->findField(*It);if(!F)DSLFail(TEXT("Unknown self container member"));for(const auto& A:F->annotation)if(A.name=="ue_property" && A.type==das::Type::tString && A.sValue=="BlueprintReadOnly")DSLFail(TEXT("Cannot mutate readonly struct member"));if(!Path.empty())Path+=".";Path+=*It;Type=F->type;}if(!DSLContainer(Type))DSLFail(TEXT("Expected supported self container member"));const bool Array=Type->baseType==das::Type::tArray;const bool Set=DSLSet(Type);const size_t Count=E->arguments.size();const bool Push=E->name=="push" || E->name=="push_clone";if((E->name=="clear"?Count!=1:E->name=="pop"?(!Array || Count!=1):Push?(!Array || (Count!=2 && Count!=3)):(E->name=="insert" || E->name=="insert_clone")?(Array || (E->name=="insert_clone" && Set) || Count!=(Set?2:3)):(Array?(Count!=2 && Count!=3):Count!=2)))DSLFail(TEXT("Unsupported self container operation/arity"));const FString Carrier=Path.empty()?PropertyPath(P):DSLDirectPath(Type,Folder,Registry);DSLEnsureStructHelpers(Type,Carrier,Group,Source->module,GeneratedHelpers,HelperPaths);const das::string Name=DSLUtf8(TEXT("__ue_container_op_")+FMD5::HashAnsiString(*(DSLSourceValueKey(Type,Carrier)+TEXT(":")+DSLText(E->name)+FString::FromInt(int32(Count))+(Path.empty()?TEXT(":root"):TEXT(":member")))));const bool Result=E->name=="erase" && !Array;if(!Source->module->functionsByName.find(das::hash64z(Name.c_str()))){auto* Fn=new das::Function();Fn->at=E->at;Fn->name=Name;Fn->generated=true;Fn->privateFunction=false;Fn->result=new das::TypeDecl(Result?das::Type::tBool:das::Type::tVoid,E->at);auto AddArg=[&](const das::string& N,const das::TypeDeclPtr& T){auto* A=new das::Variable();A->at=E->at;A->name=N;A->type=DSLCopyType(T);A->type->ref=false;A->type->constant=true;Fn->arguments.push_back(A);};AddArg("property",new das::TypeDecl(das::Type::tString,E->at));AddArg("member",new das::TypeDecl(das::Type::tString,E->at));for(size_t I=1;I<Count;++I)AddArg("arg_"+das::to_string(I),Array?(Push && I==1?Type->firstType:static_cast<das::TypeDeclPtr>(new das::TypeDecl(das::Type::tInt,E->at))):I==1?Type->firstType:Type->secondType);auto* Body=new das::ExprBlock();Body->at=E->at;auto Local=[&](const das::string& N,const das::TypeDeclPtr& T,das::ExpressionPtr Init,bool Move){auto* V=new das::Variable();V->at=E->at;V->name=N;V->type=DSLCopyType(T);V->type->ref=false;V->type->constant=false;V->init=Init;V->init_via_move=Move;auto* L=new das::ExprLet();L->at=E->at;L->variables.push_back(V);Body->list.push_back(L);};Local("container",Type,DSLDirectUnpack(E->at,Type,Path.empty()?static_cast<das::ExpressionPtr>(DSLCall(E->at,"ue_get_self_struct",{new das::ExprVar(E->at,"property")})):static_cast<das::ExpressionPtr>(DSLCall(E->at,"ue_struct_nested_get",{DSLCall(E->at,"ue_get_self_struct",{new das::ExprVar(E->at,"property")}),new das::ExprVar(E->at,"member"),new das::ExprConstInt(E->at,-1)})),Carrier),true);auto* Op=DSLCall(E->at,E->name,{new das::ExprVar(E->at,"container")});for(size_t I=1;I<Count;++I)Op->arguments.push_back(new das::ExprVar(E->at,"arg_"+das::to_string(I)));if(Result)Local("result",Fn->result,Op,false);else Body->list.push_back(Op);Body->list.push_back(Path.empty()?DSLCall(E->at,"ue_set_self_struct",{new das::ExprVar(E->at,"property"),DSLDirectPack(E->at,Type,new das::ExprVar(E->at,"container"),Carrier)}):DSLCall(E->at,"ue_dsl_commit_struct_member_struct",{new das::ExprVar(E->at,"property"),new das::ExprVar(E->at,"member"),DSLDirectPack(E->at,Type,new das::ExprVar(E->at,"container"),Carrier)}));if(Result)Body->list.push_back(new das::ExprReturn(E->at,new das::ExprVar(E->at,"result")));Fn->body=Body;if(!Source->module->addFunction(Fn,true))DSLFail(TEXT("Self container operation helper collision"));if(Array && ((Push && Count==3) || (E->name=="erase" && Count>1))){auto Wide=Fn->clone();for(size_t I=Push?3:2;I<Wide->arguments.size();++I){Wide->arguments[I]->type=new das::TypeDecl(das::Type::tInt64,E->at);Wide->arguments[I]->type->constant=true;}if(!Source->module->addFunction(Wide,true))DSLFail(TEXT("Self container wide helper collision"));}}auto* Call=DSLCall(E->at,Name,{new das::ExprConstString(E->at,Root->name),new das::ExprConstString(E->at,Path)});for(size_t I=1;I<Count;++I)Call->arguments.push_back(E->arguments[I]);return Call;}catch(const std::exception& X){Error=UTF8_TO_TCHAR(X.what());return E;}}
        virtual void preVisit(das::ExprCopy* E) override {MarkLeft(E->left);}
        virtual void preVisit(das::ExprMove* E) override {MarkLeft(E->left);}
        virtual void preVisit(das::ExprClone* E) override {MarkLeft(E->left);}
        virtual void preVisit(das::ExprOp2* E) override {if(E->op=="+=" || E->op=="-=" || E->op=="*=" || E->op=="/=")MarkLeft(E->left);}
        virtual das::ExpressionPtr visit(das::ExprField* E) override {if(const auto* Native=NativeSceneReads.Find(E))return DSLCall(E->at,"__ue_scene_relative_"+Native->Suffix,{DSLDirectPack(E->at,Native->Receiver,E->value,DSLTypedReferencePath(Native->Receiver))});return SelfField(E) && !LeftFields.Contains(E)?Read(E):E;}
        virtual das::ExpressionPtr visit(das::ExprCopy* E) override {if(IndexedSelf(E->left))return WriteIndexed(E,false);if(auto* F=SelfField(E->left))return Write(E,F,false);return StructRoot(E->left)?WriteLeaf(E,false):E;}
        virtual das::ExpressionPtr visit(das::ExprMove* E) override {if(IndexedSelf(E->left))return WriteIndexed(E,false);if(auto* F=SelfField(E->left))return Write(E,F,false);return StructRoot(E->left)?WriteLeaf(E,false):E;}
        virtual das::ExpressionPtr visit(das::ExprClone* E) override {if(IndexedSelf(E->left))return WriteIndexed(E,false);if(auto* F=SelfField(E->left))return Write(E,F,false);return StructRoot(E->left)?WriteLeaf(E,false):E;}
        virtual das::ExpressionPtr visit(das::ExprOp2* E) override {if((E->op=="+=" || E->op=="-=" || E->op=="*=" || E->op=="/=") && IndexedSelf(E->left))return WriteIndexed(E,true);if(LeftFields.Contains(E->left) && StructRoot(E->left))return WriteLeaf(E,true);auto* F=SelfField(E->left);return F && LeftFields.Contains(F)?Write(E,F,true):E;}
        virtual das::ExpressionPtr visit(das::ExprWith* E) override {return E->with && E->with->rtti_isVar() && static_cast<das::ExprVar*>(E->with)->name==Self?E->body:E;}
    };
    struct FDSLNativeSourceVisitor:FDSLBodyVisitor {
        using FDSLBodyVisitor::FDSLBodyVisitor;
        virtual das::ExpressionPtr visit(das::ExprCall* E)override{return ContainerOperation(E->name)?FDSLBodyVisitor::visit(E):E;}
        virtual das::ExpressionPtr visit(das::ExprInvoke* E)override{return NativeSelfCall(E);}
        virtual das::ExpressionPtr visit(das::ExprWith* E)override{return E;}
    };
    struct FUEFunctionAnnotation:das::FunctionAnnotation
    {
        FUEFunctionAnnotation():FunctionAnnotation("ue_function"){}
        virtual bool apply(const das::FunctionPtr& F,das::ModuleGroup&,das::AnnotationArgumentList&,das::string& E) override {if(!F->classParent){E="ue_function must be a member of ue_class or ue_interface";return false;}return true;}
        virtual bool finalize(const das::FunctionPtr& F,das::ModuleGroup&,das::AnnotationArgumentList&,const das::AnnotationArgumentList&,das::string& Error) override {try{DSLAnnotation(F->annotations,"ue_function");if(!F->classParent || (!DSLAnnotation(F->classParent->annotations,"ue_class") && !DSLAnnotation(F->classParent->annotations,"ue_interface") && !DSLAnnotation(F->classParent->annotations,"ue_library")))DSLFail(TEXT("ue_function requires ue_class or ue_interface"));return true;}catch(const std::exception& X){Error=X.what();return false;}}
        virtual bool apply(das::ExprBlock*,das::ModuleGroup&,das::AnnotationArgumentList&,das::string& E) override {E="ue_function cannot annotate block";return false;}
        virtual bool finalize(das::ExprBlock*,das::ModuleGroup&,das::AnnotationArgumentList&,const das::AnnotationArgumentList&,das::string&) override {return false;}
    };
    bool DSLStandardLifecycle(const das::FunctionPtr& F){const FString Name=DSLText(F->name.substr(F->name.find_last_of(das::string(1,char(96))+das::string(1,char(39)))+1));return Name==TEXT("ConstructionScript") || Name==TEXT("Initialize") || Name==TEXT("BeginPlay") || Name==TEXT("Tick") || Name==TEXT("EndPlay");}
    struct FUECallbackAnnotation:das::FunctionAnnotation {
        bool Event;
        FUECallbackAnnotation(bool InEvent):FunctionAnnotation(InEvent?"ue_event":"ue_lifecycle"),Event(InEvent){}
        virtual bool apply(const das::FunctionPtr& F,das::ModuleGroup&,das::AnnotationArgumentList& Args,das::string& Error)override{try{if(!F->classParent || F->isStaticClassMethod)DSLFail(TEXT("UE callbacks require an instance method of ue_class"));DSLAnnotationArguments(Args);if(!Event && !Args.empty())DSLFail(TEXT("ue_lifecycle takes no arguments; name the method ConstructionScript, Initialize, BeginPlay, Tick or EndPlay"));if(Event){for(const auto& A:Args)if((A.name!="source" && A.name!="name") || A.type!=das::Type::tString)DSLFail(TEXT("Expected ue_event(source=self|owner, name=DelegateName)"));if(DSLString(Args,"name").IsEmpty())DSLFail(TEXT("ue_event requires delegate name"));}return true;}catch(const std::exception& X){Error=X.what();return false;}}
        virtual bool finalize(const das::FunctionPtr& F,das::ModuleGroup&,das::AnnotationArgumentList&,const das::AnnotationArgumentList&,das::string& Error)override{try{DSLAnnotation(F->annotations,Event?"ue_event":"ue_lifecycle");if(!F->classParent || !DSLAnnotation(F->classParent->annotations,"ue_class"))DSLFail(TEXT("UE callbacks require ue_class"));return true;}catch(const std::exception& X){Error=X.what();return false;}}
        virtual bool apply(das::ExprBlock*,das::ModuleGroup&,das::AnnotationArgumentList&,das::string& Error)override{Error="UE callbacks cannot annotate blocks";return false;}
        virtual bool finalize(das::ExprBlock*,das::ModuleGroup&,das::AnnotationArgumentList&,const das::AnnotationArgumentList&,das::string&)override{return false;}
    };
    struct FUEImplementsAnnotation:das::StructureAnnotation {
        FUEImplementsAnnotation():StructureAnnotation("ue_implements"){}
        virtual bool touch(const das::StructurePtr& St,das::ModuleGroup&,das::AnnotationArgumentList& Args,das::string& Error)override{try{if(!St->isClass)DSLFail(TEXT("ue_implements requires ue_class"));if(Args.size()!=1 || Args[0].name!="interface" || Args[0].type!=das::Type::tString || DSLString(Args,"interface").IsEmpty())DSLFail(TEXT("Expected ue_implements(interface=NameOrPath)"));return true;}catch(const std::exception& X){Error=X.what();return false;}}
        virtual bool look(const das::StructurePtr& St,das::ModuleGroup&,das::AnnotationArgumentList&,das::string& Error)override{if(!DSLAnnotation(St->annotations,"ue_class")){Error="ue_implements requires ue_class";return false;}return true;}
    };
    FDasDeclaredAsyncPhysics DSLAsyncPhysics(const das::AnnotationArgumentList& Args){
        FDasDeclaredAsyncPhysics Spec;TSet<FString> Seen;
        for(const auto& A:Args){const FString Name=DSLText(A.name);if(Seen.Contains(Name) || A.type!=das::Type::tString)DSLFail(TEXT("Duplicate/non-string ue_async_physics argument"));Seen.Add(Name);
            if(Name==TEXT("script")){Spec.ScriptFile=DSLText(A.sValue);continue;}
            if(Name.Len()!=6 || !Name.StartsWith(TEXT("body_")) || Name[5]<TCHAR('0') || Name[5]>TCHAR('7'))DSLFail(TEXT("Expected ue_async_physics(script=File.das, body_0=Component, ... body_7=Component)"));
            Spec.Bodies.Add(Name[5]-TCHAR('0'),DSLText(A.sValue));
        }
        if(Spec.ScriptFile.IsEmpty())DSLFail(TEXT("ue_async_physics requires script"));return Spec;
    }
    struct FUEAsyncPhysicsAnnotation:das::StructureAnnotation {
        FUEAsyncPhysicsAnnotation():StructureAnnotation("ue_async_physics"){}
        virtual bool touch(const das::StructurePtr& St,das::ModuleGroup&,das::AnnotationArgumentList& Args,das::string& Error)override{try{if(!St->isClass)DSLFail(TEXT("ue_async_physics requires ue_class"));DSLAsyncPhysics(Args);return true;}catch(const std::exception& X){Error=X.what();return false;}}
        virtual bool look(const das::StructurePtr& St,das::ModuleGroup&,das::AnnotationArgumentList&,das::string& Error)override{try{DSLAnnotation(St->annotations,"ue_async_physics");if(!DSLAnnotation(St->annotations,"ue_class"))DSLFail(TEXT("ue_async_physics requires ue_class"));return true;}catch(const std::exception& X){Error=X.what();return false;}}
    };
    struct FUEComponentAnnotation:das::StructureAnnotation {
        FUEComponentAnnotation():StructureAnnotation("ue_component"){}
        virtual bool touch(const das::StructurePtr& St,das::ModuleGroup&,das::AnnotationArgumentList& Args,das::string& Error)override{try{if(!St->isClass)DSLFail(TEXT("ue_component requires ue_class"));DSLAnnotationArguments(Args,{});for(const auto& A:Args)if((A.name!="name" && A.name!="script" && A.name!="type" && A.name!="id") || A.type!=das::Type::tString)DSLFail(TEXT("Expected ue_component(name=Name, script=File.das, type=ClassName)"));if(DSLString(Args,"name").IsEmpty() || DSLString(Args,"script").IsEmpty() || DSLString(Args,"type").IsEmpty())DSLFail(TEXT("Component name, script and type are required"));return true;}catch(const std::exception& X){Error=X.what();return false;}}
        virtual bool look(const das::StructurePtr& St,das::ModuleGroup&,das::AnnotationArgumentList&,das::string& Error)override{try{if(!DSLAnnotation(St->annotations,"ue_class"))DSLFail(TEXT("ue_component requires ue_class"));return true;}catch(const std::exception& X){Error=X.what();return false;}}
    };
    // Source methods are type-checked as well as exported clones. Lower timer
    // captures there too, so array payloads use the same snapshot ABI.
    struct FDSLSourceTimerVisitor:das::Visitor {
        FDSLBodyVisitor& Rewrite;
        explicit FDSLSourceTimerVisitor(FDSLBodyVisitor& In):Rewrite(In){}
        virtual das::ExpressionPtr visit(das::ExprCall* E) override {
            if(E->name=="ue_effect_play" || E->name=="ue_effects::ue_effect_play" || E->name=="ue_effect_at" || E->name=="ue_effects::ue_effect_at" || E->name=="ue_sound_play" || E->name=="ue_effects::ue_sound_play" || E->name=="ue_niagara_spawn" || E->name=="ue_effects::ue_niagara_spawn")return Rewrite.RewriteEffect(E);
            if(E->name=="ue_ai_move_to_actor" || E->name=="ue_ai::ue_ai_move_to_actor" || E->name=="ue_ai_move_to_location" || E->name=="ue_ai::ue_ai_move_to_location")return Rewrite.RewriteAIMove(E);
            if(E->name=="ue_montage_play" || E->name=="ue_animation::ue_montage_play")return Rewrite.RewriteMontage(E);
            if(E->name=="ue_timer_start" || E->name=="ue_foundation::ue_timer_start" || E->name=="ue_timer_next_tick" || E->name=="ue_foundation::ue_timer_next_tick")return Rewrite.RewriteTimer(E);
            return E;
        }
    };
    // Native host declarations carry identity only; they are not authored UE classes.
    FString DSLNativeBasePath(const das::StructurePtr& St) {
        const auto* A=St?DSLAnnotation(St->annotations,"ue_native_base"):nullptr;
        return A?DSLString(A->arguments,"path"):FString();
    }
    struct FUENativeBaseAnnotation:das::StructureAnnotation {
        FUENativeBaseAnnotation():StructureAnnotation("ue_native_base"){}
        virtual bool touch(const das::StructurePtr& St,das::ModuleGroup&,das::AnnotationArgumentList& Args,das::string& Error)override{try{
            DSLAnnotationArguments(Args);if(Args.size()!=1 || Args[0].name!="path")DSLFail(TEXT("Expected ue_native_base(path=NativeClassPath)"));
            const FString Path=DSLString(Args,"path");
            if(!DasActorNativeParent(Path) && Path!=TEXT("/Script/Engine.ActorComponent") && Path!=TEXT("/Script/Engine.SaveGame") && Path!=TEXT("/Script/Engine.DataAsset") && Path!=TEXT("/Script/Engine.PrimaryDataAsset"))DSLFail(TEXT("Unsupported native DSL base; expected Actor, Pawn, Character, ActorComponent, SaveGame, DataAsset or PrimaryDataAsset"));
            auto* Native=FindObject<UClass>(nullptr,*Path);if(!Native || !Native->HasAnyClassFlags(CLASS_Native) || Native->HasAnyClassFlags(CLASS_Interface))DSLFail(TEXT("Native DSL base class is not loaded"));
            if(!St->isClass || St->parent)DSLFail(TEXT("Native DSL base requires an empty root class"));
            for(const auto& F:St->fields)if(!F.generated && F.name!="__rtti")DSLFail(TEXT("Native DSL base cannot declare fields or methods"));
            return true;
        }catch(const std::exception& X){Error=X.what();return false;}}
        virtual bool look(const das::StructurePtr& St,das::ModuleGroup&,das::AnnotationArgumentList&,das::string& Error)override{try{DSLDeclarationKind(St->annotations);return true;}catch(const std::exception& X){Error=X.what();return false;}}
    };
    void DSLValidateConstSource(const das::FunctionPtr& Function,const das::StructurePtr& Source) {
        if(!Function->isConstClassMethod || !Function->body || Function->arguments.empty())return;
        struct FCheck:das::Visitor {
            das::string Self;das::StructurePtr Source;
            FCheck(const das::string& Name,const das::StructurePtr& St):Self(Name),Source(St){}
            bool SelfPath(das::Expression* E){
                for(int Depth=0;E && Depth<64;++Depth){
                    if(E->rtti_isVar())return static_cast<das::ExprVar*>(E)->name==Self;
                    if(E->rtti_isField())E=static_cast<das::ExprField*>(E)->value;
                    else if(E->rtti_isAt())E=static_cast<das::ExprAt*>(E)->subexpr;
                    else return false;
                }return false;
            }
            void Write(das::Expression* E){if(SelfPath(E))DSLFail(TEXT("Const method cannot modify self fields"));}
            virtual void preVisit(das::ExprCopy* E)override{Write(E->left);}
            virtual void preVisit(das::ExprMove* E)override{Write(E->left);}
            virtual void preVisit(das::ExprClone* E)override{Write(E->left);}
            virtual void preVisit(das::ExprOp2* E)override{if(E->op=="+=" || E->op=="-=" || E->op=="*=" || E->op=="/=" || E->op=="%=" || E->op=="|=" || E->op=="&=" || E->op=="^=" || E->op=="<<=" || E->op==">>=")Write(E->left);}
            virtual void preVisit(das::ExprOp1* E)override{if(E->op=="++" || E->op=="--" || E->op=="+++")Write(E->subexpr);}
            void Method(const das::string& Name,bool Super){
                bool Found=false,IsConst=false;
                for(auto Owner=Super?Source->parent:Source;Owner && !Found;Owner=Owner->parent)Owner->module->functions.foreach([&](const das::FunctionPtr& F){
                    if(F->classParent==Owner && F->name==Owner->name+"`"+Name){Found=true;IsConst=F->isConstClassMethod;}
                });
                if(Found && !IsConst)DSLFail(TEXT("Const method cannot call a non-const self/Super method: ")+DSLText(Name));
            }
            virtual void preVisit(das::ExprInvoke* E)override{
                if(!E->isInvokeMethod || E->arguments.empty() || !E->arguments[0]->rtti_isField())return;
                auto* F=static_cast<das::ExprField*>(E->arguments[0]);if(!F->value->rtti_isVar())return;
                const auto Name=static_cast<das::ExprVar*>(F->value)->name;
                if(Name==Self || Name=="Super")Method(F->name,Name=="Super");
            }
            virtual void preVisit(das::ExprCall* E)override{
                const auto Name=E->name.substr(E->name.find_last_of(':')+1);
                if((Name=="push" || Name=="push_clone" || Name=="insert" || Name=="insert_clone" || Name=="erase" || Name=="clear" || Name=="pop" || Name=="resize") && !E->arguments.empty())Write(E->arguments[0]);
                if(Name.find("ue_set_self_")==0)DSLFail(TEXT("Const method cannot use self property setters"));
            }
        } Check(Function->arguments[0]->name,Source);
        Function->body->visit(Check);
    }
    struct FUEClassAnnotation:das::StructureAnnotation
    {
        FUEClassAnnotation():StructureAnnotation("ue_class"){}
        virtual bool look(const das::StructurePtr& St,das::ModuleGroup& Group,das::AnnotationArgumentList& Args,das::string& Error) override {try{
            DSLDeclarationKind(St->annotations);DSLAnnotation(St->annotations,"ue_class");
            if(CollectTypesOnly)return true;
            if(const auto* Done=Args.find("_ue_implements_resolved",das::Type::tBool)){if(Done->at.fileInfo)DSLFail(TEXT("Reserved implements argument"));return true;}
            auto* Wire=Args.find(DSLModelKey,das::Type::tString);FDasAuthoringModule Model;FString Issue;if(!Wire || !FDasAuthoringModule::FromJson(DSLText(Wire->sValue),Model,Issue))DSLFail(Issue);auto* Declared=Model.Classes.FindByPredicate([&](const auto& C){return C.Name==DSLText(St->name);});if(!Declared)DSLFail(TEXT("Missing current class declaration"));auto& C=*Declared;const bool LegacyInterfaces=C.Interfaces.Num()!=0;
            for(const auto* A:St->annotations)if(A->annotation->name=="ue_implements"){const FString Path=DSLResolveType(EDSLTypeKind::Interface,DSLString(A->arguments,"interface"),FDSLTypeScope{Model.Id,DSLString(Args,"type_folder"),DSLString(Args,"type_registry"),&Group,St->module});if(C.Interfaces.Contains(Path))DSLFail(TEXT("Duplicate interface implementation: ")+Path);C.Interfaces.Add(Path);}
            for(const auto* A:St->annotations)if(A->annotation->name=="ue_component"){FDasDeclaredComponent Component;Component.Name=DSLString(A->arguments,"name");Component.Id=DSLString(A->arguments,"id",C.Id+TEXT(".")+Component.Name);Component.ScriptFile=DSLString(A->arguments,"script");Component.ClassName=DSLString(A->arguments,"type");C.Components.Add(Component);}
            if(const auto* A=DSLAnnotation(St->annotations,"ue_async_physics"))C.AsyncPhysics=DSLAsyncPhysics(A->arguments);
            C.Components.Sort([](const auto& A,const auto& B){return A.Id<B.Id;});
            // Legacy scalar adapters rename their single input before class annotations
            // are attached. Interface contracts must retain the source parameter name.
            if(!LegacyInterfaces && !C.Interfaces.IsEmpty())St->module->functions.foreach([&](const das::FunctionPtr& F){if(F->classParent!=St || !DSLAnnotation(F->annotations,"ue_function") || F->arguments.size()!=2)return;auto* M=C.Methods.FindByPredicate([&](const auto& Candidate){return Candidate.Name==DSLText(F->name);});if(!M || M->Parameters.Num()!=1)return;auto& P=M->Parameters[0];const FString K=DSLTypeName(F->arguments[1]->type);if(P.Direction==TEXT("in") && P.Name==TEXT("Input") && K==M->ReturnType.Kind && (K==TEXT("int") || K==TEXT("float") || K==TEXT("bool"))){P.Name=DSLText(F->arguments[1]->name);P.Id=M->Id+TEXT(".")+P.Name;}});
            FString Json;if(!Model.ToJson(Json,Issue))DSLFail(Issue);FDasClassSchema Schema;if(!Model.ToActorSchema(C.Id,Schema,Issue))DSLFail(Issue);for(auto& Arg:Args)if(Arg.name==DSLModelKey)Arg.sValue=DSLUtf8(Json);Args.push_back(das::AnnotationArgument("_ue_implements_resolved",true));return true;
        }catch(const std::exception& X){Error=X.what();return false;}}
        virtual bool touch(const das::StructurePtr& St,das::ModuleGroup& Group,das::AnnotationArgumentList& Args,das::string& Error) override
        {
            try
            {
                FString NativeHint=DSLString(Args,"parent",TEXT("/Script/Engine.Actor"));
                auto* NativeRoot=St;while(NativeRoot->parent)NativeRoot=NativeRoot->parent;
                const auto Marker=DSLNativeBasePath(NativeRoot);if(!Marker.IsEmpty())NativeHint=Marker;else if(NativeRoot!=St)if(const auto* ParentAnnotation=DSLAnnotation(NativeRoot->annotations,"ue_class"))NativeHint=DSLString(ParentAnnotation->arguments,"parent",TEXT("/Script/Engine.Actor"));
                if(!NativeHint.StartsWith(TEXT("/")))NativeHint=DSLResolveType(EDSLTypeKind::Class,NativeHint,FDSLTypeScope{DSLString(Args,"module_id",FPaths::GetBaseFilename(DSLText(St->at.fileInfo->name))),DSLString(Args,"type_folder"),DSLString(Args,"type_registry"),&Group,St->module});
                TGuardValue<FString> NativeDefaultsHint(DSLNativeDefaultsParentHint,NativeHint);
                DSLExpandDeclarationGroups(St,true);
                DSLPrepareComputedDefaults(St);
                const auto ComponentDefaults=DSLCollectComponentDefaults(St);
                DSLCommentTooltip(St->at,Args);
                if(!CollectTypesOnly)DSLPrepareClassDefaults(St,ComponentDefaults);
                for(auto& Field:St->fields)if(!Field.classMethod && !Field.generated && Field.name!="__rtti")DSLCommentTooltip(Field.at,Field.annotation);
                DSLAnnotationArguments(Args,{TEXT("implements")});
                if(CollectTypesOnly){
                    // Declaration collection runs before authored assets/registries exist.
                    // Erase syntax markers only; no body runs in this compilation pass.
                    for(auto& Field:St->fields)if(DSLContainer(Field.type))Field.init=nullptr;
                    FDSLTypeCallVisitor SyntaxOnly(FDSLTypeScope{},false);
                    for(const auto& Field:St->fields)if(Field.init)Field.init->visit(SyntaxOnly);
                    St->module->functions.foreach([&](const das::FunctionPtr& Function){if(Function->classParent==St && Function->body && DSLAnnotation(Function->annotations,"ue_function"))Function->body=Function->body->visit(SyntaxOnly);});
                    FDasDeclaredClass NativeHost;NativeHost.NativeParent=NativeHint;das::vector<das::FunctionPtr> NativeHelpers;TSet<FString> NativePaths;
                    das::vector<das::FunctionPtr> NativeSources;St->module->functions.foreach([&](const das::FunctionPtr& F){if(F->classParent==St && !F->generated && F->body && !F->arguments.empty())NativeSources.push_back(F);});
                    for(const auto& F:NativeSources){DSLValidateConstSource(F,St);FDSLNativeSourceVisitor Native(NativeHost,F->arguments[0]->name,St,FString(),FString(),Group,NativeHelpers,NativePaths);Native.NativeOnly=true;F->body=F->body->visit(Native);if(!Native.Error.IsEmpty())DSLFail(Native.Error);}
                    for(const auto& F:NativeHelpers)if(!St->module->addFunction(F,true))DSLFail(TEXT("Native array declaration helper collision"));
                    return true;
                }
                if(const auto* Wire=Args.find(DSLModelKey,das::Type::tString)){if(Wire->at.fileInfo) DSLFail(TEXT("Reserved internal annotation argument"));return true;}
                St->module->functions.foreach([&](const das::FunctionPtr& F){if(F->classParent==St && !F->generated)DSLValidateConstSource(F,St);});
                FDasAuthoringModule Model;const FString Script=FPaths::GetCleanFilename(DSLText(St->at.fileInfo->name));
                Model.Id=DSLString(Args,"module_id",FPaths::GetBaseFilename(Script));
                const FString TypeFolder=DSLString(Args,"type_folder"),TypeRegistry=DSLString(Args,"type_registry");if(!TypeFolder.IsEmpty() && !TypeRegistry.IsEmpty())DSLFail(TEXT("Choose type_registry or type_folder"));das::vector<das::FunctionPtr> HelpersGenerated;TSet<FString> Helpers;
                FDasDeclaredClass C;C.SourceScript=Script;C.Name=DSLText(St->name);C.Id=DSLString(Args,"id",Model.Id+TEXT(".")+C.Name);const FDSLTypeScope TypeScope{Model.Id,TypeFolder,TypeRegistry,&Group,St->module};FDasDeclaredClass BaseEffective;FString BaseId;const FString NativeBase=DSLNativeBasePath(St->parent);const bool Derived=St->parent && NativeBase.IsEmpty();
                if(St->parent && Args.find("parent",das::Type::tString))DSLFail(TEXT("Use either class inheritance or parent annotation"));
                if(Derived){if(Args.find("parent",das::Type::tString))DSLFail(TEXT("Use either class inheritance or parent annotation"));const auto* A=DSLAnnotation(St->parent->annotations,"ue_class");const auto* Wire=A?A->arguments.find(DSLModelKey,das::Type::tString):nullptr;FDasAuthoringModule BaseModel;FString Issue;if(!Wire || !FDasAuthoringModule::FromJson(DSLText(Wire->sValue),BaseModel,Issue))DSLFail(TEXT("Authored inheritance requires a ue_class parent declaration"));const auto* Base=BaseModel.Classes.FindByPredicate([&](const auto& P){return P.Name==DSLText(St->parent->name);});if(!Base || !BaseModel.ResolveClass(Base->Id,BaseEffective,Issue))DSLFail(Issue.IsEmpty()?TEXT("Missing authored parent"):Issue);BaseId=Base->Id;Model.Classes=BaseModel.Classes;C.NativeParent=BaseEffective.NativeParent;C.Properties=BaseEffective.Properties;C.Components=BaseEffective.Components;C.Interfaces=BaseEffective.Interfaces;C.Callbacks=BaseEffective.Callbacks;}
                else C.NativeParent=NativeBase.IsEmpty()?DSLResolveType(EDSLTypeKind::Class,DSLString(Args,"parent",TEXT("/Script/Engine.Actor")),TypeScope):NativeBase;
                if(const auto* A=DSLAnnotation(St->annotations,"ue_async_physics"))C.AsyncPhysics=DSLAsyncPhysics(A->arguments);else if(Derived)C.AsyncPhysics=BaseEffective.AsyncPhysics;
                // Identity/host is available before resolving self-referential property/method types.
                DSLActiveRefDeclarations.Add(Script+TEXT("::")+C.Name,C);
                for(const auto& A:Args)
                {
                    if(A.name=="module_id" || A.name=="id" || A.name=="parent" || A.name=="type_folder" || A.name=="type_registry") continue;if(A.name=="implements"){TArray<FString> Items;DSLString(Args,"implements").ParseIntoArray(Items,TEXT(","),false);for(auto I:Items){I=I.TrimStartAndEnd();I=DSLResolveType(EDSLTypeKind::Interface,I,TypeScope);if(C.Interfaces.Contains(I))DSLFail(TEXT("Empty/duplicate interface path"));C.Interfaces.Add(I);}if(C.Interfaces.IsEmpty())DSLFail(TEXT("Expected interface paths"));continue;}
                    if(A.name=="Blueprintable"){if(!DSLBool(A)) DSLFail(TEXT("Adapter requires Blueprintable"));continue;}
                    if(A.name=="primary_asset_type"){if(A.type!=das::Type::tString)DSLFail(TEXT("primary_asset_type must be a string"));C.Metadata.Add(TEXT("PrimaryAssetType"),DSLText(A.sValue));continue;}
                    if(DSLIsMetadata(A.name)) DSLMetadata(A,C.Metadata);else DSLFail(TEXT("Unknown ue_class annotation: ")+DSLText(A.name));
                }

                for(auto& Field:St->fields)if(!Field.classMethod && !Field.generated && Field.name!="__rtti")Field.type=DSLObjectNormalize(Field.type,Group,St->module);
                St->module->functions.foreach([&](const das::FunctionPtr& Method){if(Method->classParent==St && DSLAnnotation(Method->annotations,"ue_function")){Method->result=DSLObjectNormalize(Method->result,Group,St->module);for(size_t I=1;I<Method->arguments.size();++I)Method->arguments[I]->type=DSLObjectNormalize(Method->arguments[I]->type,Group,St->module);}});
                for(auto& F:St->fields)
                {
                    // Pinned makeClassRtti adds __rtti without setting generated.
                    const bool InheritedDefault=F.inherited && ComponentDefaults.ContainsByPredicate([&](const auto& D){return D.Component.IsEmpty() && D.Property==DSLText(F.name);});
                    if(F.classMethod || F.generated || (F.inherited && !InheritedDefault) || F.name=="__rtti") continue;
                    const auto* InheritedProperty=InheritedDefault?BaseEffective.Properties.FindByPredicate([&](const auto& P){return P.Name==DSLText(F.name);}):nullptr;
                    if(InheritedDefault){if(!InheritedProperty || !InheritedProperty->bEditable)DSLFail(TEXT("Inherited default requires an editable authored property: ")+DSLText(F.name));C.Properties.RemoveAll([&](const auto& P){return P.Id==InheritedProperty->Id;});}
                    if(DSLComponentField(F,C))continue;
                    FDasDeclaredProperty P;P.Name=DSLText(F.name);P.Id=C.Id+TEXT(".")+P.Name;bool Marked=false,ReadWrite=false,ReadOnly=false,EditAnywhere=false;
                    for(const auto& A:F.annotation)
                    {
                        if(A.name=="ue_property")
                        {
                            Marked=true;
                            if(A.type==das::Type::tBool){if(!A.bValue) DSLFail(TEXT("ue_property must be enabled"));}
                            else if(A.type==das::Type::tString)
                            {
                                if(A.sValue=="EditAnywhere") {P.bEditable=true;EditAnywhere=true;}
                                else if(A.sValue=="BlueprintReadWrite") {P.bReadOnly=false;ReadWrite=true;}
                                else if(A.sValue=="BlueprintReadOnly") {P.bReadOnly=true;ReadOnly=true;}
                                else if(A.sValue=="BlueprintAssignable" && DSLDelegate(F.type) && DSLAnnotation(F.type->structType->annotations,"ue_multicast")){}
                                else DSLFail(TEXT("Unsupported property specifier: ")+DSLText(A.sValue));
                            }
                            else DSLFail(TEXT("ue_property requires flags or true"));
                        }
                        else if(A.name=="ue_class_base"){if(A.type!=das::Type::tString || DSLTypeName(F.type)!=TEXT("class"))DSLFail(TEXT("ue_class_base requires a UEClassHandle field and native base path"));P.Type.TypePath=DSLText(A.sValue);}
                        else if(A.name=="ue_id"){if(A.type!=das::Type::tString) DSLFail(TEXT("ue_id must be string"));P.Id=DSLText(A.sValue);}
                        else if(DSLIsMetadata(A.name) || A.name=="AssetBundles") DSLMetadata(A,P.Metadata);else DSLFail(TEXT("Unknown field metadata: ")+DSLText(A.name));
                    }
                    if(ReadWrite && ReadOnly) DSLFail(TEXT("Conflicting BlueprintReadOnly/BlueprintReadWrite specifiers"));
                    if(!Marked) DSLFail(TEXT("Script-private fields unsupported; add @ue_property: ")+P.Name);
                    if(F.init){FDSLTypeCallVisitor TypeCalls(TypeScope);F.init->visit(TypeCalls);}
                    if(DSLDelegate(F.type)){const bool Multi=DSLAnnotation(F.type->structType->annotations,"ue_multicast")!=nullptr;if(EditAnywhere)DSLFail(TEXT("Delegate fields do not support EditAnywhere; bind them at runtime"));if(!Marked || F.init || (Multi && (ReadWrite || ReadOnly)))DSLFail(TEXT("Delegate fields require ue_property and no default; events use BlueprintAssignable"));const auto* A=DSLDelegateAnnotation(F.type);P.DelegateWire=DSLString(A->arguments,"_ue_delegate_wire");P.Type.Kind=Multi?TEXT("event"):TEXT("delegate");P.Type.TypePath=TEXT("/DasDelegates/")+FMD5::HashAnsiString(*P.DelegateWire)+TEXT(".Type");P.bEditable=false;if(Multi)P.bReadOnly=false;C.Properties.Add(P);auto* Init=new das::ExprMakeStruct(F.at);Init->makeType=DSLCopyType(F.type);Init->useInitializer=true;Init->alwaysUseInitializer=true;F.init=Init;continue;}
                    const FString FieldType=DSLTypeName(F.type);
                    if(!F.type || F.type->ref || FieldType.IsEmpty() || FieldType==TEXT("void")) DSLFail(TEXT("Adapter supports int/float/bool/string/double/name properties: ")+P.Name);
                    P.Type.Kind=FieldType;
                    if(DSLSoft(F.type)){P.Type.Kind=TEXT("struct");P.Type.TypePath=DSLSoftPath(F.type);if(F.init){if(F.init->__rtti!=das::string("ExprCallMacro"))DSLFail(TEXT("Soft defaults require ue_soft(type<SoftType>, literal_path)"));auto* Init=static_cast<das::ExprCallMacro*>(F.init);if(Init->name!="ue_soft" || Init->arguments.size()!=2 || Init->arguments[1]->__rtti!=das::string("ExprConstString"))DSLFail(TEXT("Soft default path must be a string literal"));if(!Init->arguments[0]->rtti_isTypeDecl())DSLFail(TEXT("Soft default requires a type argument"));auto DefaultType=DSLObjectNormalize(static_cast<das::ExprTypeDecl*>(Init->arguments[0])->typeexpr,Group,St->module);if(!DSLSoft(DefaultType) || DSLSoftPath(DefaultType)!=P.Type.TypePath)DSLFail(TEXT("Soft default type must match field type"));auto* Text=static_cast<das::ExprConstString*>(Init->arguments[1]);FString Issue;if(!ValidateDasSoftPath(DSLText(Text->text),Issue))DSLFail(Issue);auto Object=MakeShared<FJsonObject>();Object->SetStringField(TEXT("Values"),DSLText(Text->text));P.Default.Kind=TEXT("string");FJsonSerializer::Serialize(Object,TJsonWriterFactory<>::Create(&P.Default.String));}C.Properties.Add(P);continue;}
                    if(FieldType==TEXT("text")){P.Default=DSLTextDefault(F.init);C.Properties.Add(P);continue;}
                    if(FieldType==TEXT("byte")){P.Default=DSLByteDefault(F.init);C.Properties.Add(P);continue;}
                    if(FieldType==TEXT("class")){
                        if(DSLTypedClass(F.type)){const FString Inferred=DSLTypedClassPath(F.type);if(!P.Type.TypePath.IsEmpty() && P.Type.TypePath!=Inferred)DSLFail(TEXT("Conflicting typed class property constraint"));P.Type.TypePath=Inferred;}
                        auto* Base=FindObject<UClass>(nullptr,*P.Type.TypePath);
                        if(!P.Type.TypePath.StartsWith(TEXT("/Script/")) || !Base || Base->HasAnyClassFlags(CLASS_Interface))DSLFail(TEXT("Class property requires ue_class_base with loaded native base"));
                        P.Default=DSLReferencePropertyDefault(F,true);C.Properties.Add(MoveTemp(P));continue;
                    }
                    if(DSLContainer(F.type)){P.Type=DSLContainerType(F.type,TypeFolder,TypeRegistry);const FString Path=DSLDirectPath(F.type,TypeFolder,TypeRegistry);if(F.init){P.Default.Kind=TEXT("string");P.Default.String=DSLContainerDefault(F,Path,Group,St->module);}DSLEnsureStructHelpers(F.type,Path,Group,St->module,HelpersGenerated,Helpers);C.Properties.Add(P);continue;}
                    if(!DSLMathPath(F.type).IsEmpty()){P.Type.TypePath=DSLMathPath(F.type);P.Default.Kind=TEXT("string");P.Default.String=DSLMathPropertyDefault(F);C.Properties.Add(P);continue;}
                    if(DSLDirectEnum(F.type)){P.Type.TypePath=DSLDirectPath(F.type,TypeFolder,TypeRegistry);P.Default.Kind=TEXT("string");if(F.init && F.init->__rtti==das::string("ExprConstEnumeration"))P.Default.String=DSLText(static_cast<das::ExprConstEnumeration*>(F.init)->text);else if(F.init && F.init->rtti_isField()){auto* Init=static_cast<das::ExprField*>(F.init);if(!Init->value || !Init->value->rtti_isVar() || static_cast<das::ExprVar*>(Init->value)->name!=F.type->enumType->name)DSLFail(TEXT("Direct enum initializer must name its declared enum"));P.Default.String=DSLText(Init->name);}else DSLFail(TEXT("Direct enum property requires named enum literal initializer: ")+(F.init?DSLText(F.init->__rtti):TEXT("none")));if(P.Default.String.Contains(TEXT(".")))P.Default.String=P.Default.String.RightChop(P.Default.String.Find(TEXT("."))+1);if(P.Default.String.Contains(TEXT("::")))P.Default.String=P.Default.String.RightChop(P.Default.String.Find(TEXT("::"))+2);FString Issue;auto* E=ResolveDasEnum(P.Type.TypePath,Issue);if(!E || !ValidateDasEnumNumber(P.Type.TypePath,E->GetValueByNameString(P.Default.String),Issue))DSLFail(Issue);C.Properties.Add(P);continue;}
                    if(DSLDirectStruct(F.type)){P.Type.TypePath=DSLDirectPath(F.type,TypeFolder,TypeRegistry);DSLEnsureStructHelpers(F.type,P.Type.TypePath,Group,St->module,HelpersGenerated,Helpers);P.Default.Kind=TEXT("string");P.Default.String=TEXT("");if(F.init && F.init->rtti_isCall()){
                        auto* DefaultCall=static_cast<das::ExprCall*>(F.init);if(DefaultCall->name=="ue_default_text"){
                            if(DefaultCall->arguments.size()!=1 || DefaultCall->arguments[0]->__rtti!=das::string("ExprConstString"))DSLFail(TEXT("ue_default_text requires a literal string"));
                            P.Default.String=DSLText(static_cast<das::ExprConstString*>(DefaultCall->arguments[0])->text);F.init=nullptr;
                            FInstancedStruct Value;FString Issue;if(!ParseDasStructDefault(P.Type.TypePath,P.Default.String,Value,Issue))DSLFail(Issue);
                        }}if(F.init){bool Empty=false;if(F.init->rtti_isCall()){auto* I=static_cast<das::ExprCall*>(F.init);Empty=I->name==F.type->structType->name && I->arguments.empty();}else if(F.init->rtti_isMakeStruct()){auto* I=static_cast<das::ExprMakeStruct*>(F.init);Empty=I->structs.empty() && !I->block;}if(!Empty)DSLFail(TEXT("Direct struct defaults require empty struct constructor; field overrides are not supported yet"));}C.Properties.Add(P);continue;}
                    if(FieldType==TEXT("enum")){if(!F.init || !F.init->rtti_isCall())DSLFail(TEXT("Enum field requires ue_enum_value(path, member)"));auto* Call=static_cast<das::ExprCall*>(F.init);if(Call->name!="ue_enum_value" || Call->arguments.size()!=2 || Call->arguments[0]->__rtti!=das::string("ExprConstString") || Call->arguments[1]->__rtti!=das::string("ExprConstString"))DSLFail(TEXT("Enum initializer requires literal path/member"));P.Type.TypePath=DSLResolveType(EDSLTypeKind::Enum,DSLText(static_cast<das::ExprConstString*>(Call->arguments[0])->text),TypeScope);static_cast<das::ExprConstString*>(Call->arguments[0])->text=DSLUtf8(P.Type.TypePath);P.Default.Kind=TEXT("string");P.Default.String=DSLText(static_cast<das::ExprConstString*>(Call->arguments[1])->text);FString Issue;auto* E=ResolveDasEnum(P.Type.TypePath,Issue);if(!E || !ValidateDasEnumNumber(P.Type.TypePath,E->GetValueByNameString(P.Default.String),Issue))DSLFail(Issue);C.Properties.Add(P);continue;}
                    if(DSLTypedReference(F.type)){P.Type.TypePath=DSLTypedReferencePath(F.type);if(DSLAuthoredView(F.type))P.Type.AuthoredId=DSLReferenceDeclaration(F.type).Id;P.Default=DSLReferencePropertyDefault(F,false);C.Properties.Add(MoveTemp(P));continue;}
                    if(FieldType==TEXT("object")){
                        if(!F.init || !F.init->rtti_isCall())DSLFail(TEXT("Object field requires literal ue_object_value(native_class_path) initializer"));
                        auto* Call=static_cast<das::ExprCall*>(F.init);if(Call->name!="ue_object_value" || Call->arguments.size()!=1 || Call->arguments[0]->__rtti!=das::string("ExprConstString"))DSLFail(TEXT("Object field initializer requires one literal native class path"));
                        P.Type.TypePath=DSLResolveType(EDSLTypeKind::Class,DSLText(static_cast<das::ExprConstString*>(Call->arguments[0])->text),TypeScope);static_cast<das::ExprConstString*>(Call->arguments[0])->text=DSLUtf8(P.Type.TypePath);
                        P.Default.Kind=TEXT("null");C.Properties.Add(MoveTemp(P));continue;
                    }
                    if(FieldType==TEXT("struct")){
                        if(F.type->annotation->name!="UEStructValue" || !F.init || !F.init->rtti_isCall())DSLFail(TEXT("Struct property requires UEStructValue and literal ue_struct_value/default initializer"));
                        auto* Call=static_cast<das::ExprCall*>(F.init);
                        const bool Full=Call->name=="ue_struct_default";
                        if((!Full && Call->name!="ue_struct_value") || Call->arguments.size()!=(Full?2:1))DSLFail(TEXT("Invalid struct property initializer"));
                        for(auto* Arg:Call->arguments)if(Arg->__rtti!=das::string("ExprConstString"))DSLFail(TEXT("Struct initializer arguments must be literal strings"));
                        P.Type.TypePath=DSLResolveType(EDSLTypeKind::Struct,DSLText(static_cast<das::ExprConstString*>(Call->arguments[0])->text),TypeScope);static_cast<das::ExprConstString*>(Call->arguments[0])->text=DSLUtf8(P.Type.TypePath);
                        P.Default.Kind=TEXT("string");P.Default.String=Full?DSLText(static_cast<das::ExprConstString*>(Call->arguments[1])->text):FString();
                        FInstancedStruct Default;FString Issue;if(!ParseDasStructDefault(P.Type.TypePath,P.Default.String,Default,Issue))DSLFail(Issue);
                        C.Properties.Add(MoveTemp(P));continue;
                    }
                    if(F.init)
                    {
                        P.Default.Kind=TEXT("number");
                        if(F.init->__rtti==das::string("ExprConstInt")) P.Default.Number=static_cast<das::ExprConstInt*>(F.init)->getValue();
                        else if(F.init->__rtti==das::string("ExprConstFloat")) P.Default.Number=static_cast<das::ExprConstFloat*>(F.init)->getValue();
                        else if(F.init->__rtti==das::string("ExprConstBool")){P.Default.Kind=TEXT("bool");P.Default.Boolean=static_cast<das::ExprConstBool*>(F.init)->getValue();}
                        else if(F.init->__rtti==das::string("ExprConstDouble")) P.Default.Number=static_cast<das::ExprConstDouble*>(F.init)->getValue();
                        else if(F.init->__rtti==das::string("ExprConstString")){P.Default.Kind=TEXT("string");P.Default.String=DSLText(static_cast<das::ExprConstString*>(F.init)->text);}
                        else if(P.Type.Kind==TEXT("name") && F.init->rtti_isCall()){auto* Call=static_cast<das::ExprCall*>(F.init);if(Call->name!="ue_name_value" || Call->arguments.size()!=1 || Call->arguments[0]->__rtti!=das::string("ExprConstString"))DSLFail(TEXT("Name default requires ue_name_value with literal string"));P.Default.Kind=TEXT("string");P.Default.String=DSLText(static_cast<das::ExprConstString*>(Call->arguments[0])->text);}
                        else DSLFail(TEXT("Only literal property defaults supported: ")+P.Name);
                    }
                    C.Properties.Add(MoveTemp(P));
                }
                if(C.NativeParent!=TEXT("/Script/Engine.Character") && !C.Components.ContainsByPredicate([](const auto& V){return V.bRoot;}))for(auto& V:C.Components){auto* Native=V.NativeClass.IsEmpty()?nullptr:FindObject<UClass>(nullptr,*V.NativeClass);if(Native && Native->IsChildOf(USceneComponent::StaticClass()) && V.Attach.IsEmpty()){V.bRoot=true;break;}}
                for(const auto& D:ComponentDefaults)if(D.Component.IsEmpty())if(const auto* Base=BaseEffective.Properties.FindByPredicate([&](const auto& P){return P.Name==D.Property;})){
                    auto* Parsed=C.Properties.FindByPredicate([&](const auto& P){return P.Name==D.Property;});
                    if(!Parsed || Parsed->Type.Identity()!=Base->Type.Identity())DSLFail(TEXT("Inherited default cannot change the property type: ")+D.Property);
                    FDasPropertyDefaultOverride Override;Override.Id=Base->Id;Override.Name=Base->Name;Override.Default=Parsed->Default;C.PropertyOverrides.Add(Override);*Parsed=*Base;Parsed->Default=Override.Default;
                }
                if(auto* NativeBaseClass=DasNativeDefaultBase(C.NativeParent)){
                    for(const auto& P:C.Properties)if(NativeBaseClass->FindPropertyByName(*P.Name))DSLFail(TEXT("Authored property shadows native self property: ")+P.Name);
                    for(const auto& P:C.Components)if(NativeBaseClass->FindPropertyByName(*P.Name))DSLFail(TEXT("Authored component shadows native self property: ")+P.Name);
                }
                DSLApplyComponentDefaults(C,ComponentDefaults,BaseEffective.Components);
                const FString RefKey=Script+TEXT("::")+C.Name;DSLActiveRefDeclarations.Add(RefKey,C);DSLRememberPropertySources(RefKey,St,C,TypeScope);
                das::vector<das::FunctionPtr> Functions,Generated;
                St->module->functions.foreach([&](const das::FunctionPtr& F){if(F->classParent==St && DSLAnnotation(F->annotations,"ue_function")) Functions.push_back(F);});
                for(const auto& F:Functions)
                {
                    if(DSLAnnotation(F->annotations,"ue_lifecycle") || DSLAnnotation(F->annotations,"ue_event") || DSLStandardLifecycle(F))DSLFail(TEXT("Conflicting UE callback/function annotations"));auto* A=const_cast<das::AnnotationDeclaration*>(DSLAnnotation(F->annotations,"ue_function"));DSLCommentTooltip(F->at,A->arguments,&F->annotations);DSLAnnotationArguments(A->arguments,{TEXT("out"),TEXT("ref")});FDasDeclaredMethod M;
                    M.Name=DSLText(F->name.substr(F->name.find_last_of(das::string(1,char(96))+das::string(1,char(39)))+1));M.Id=C.Id+TEXT(".")+M.Name;M.ScriptFile=Script;M.EntryPoint=TEXT("ue_")+C.Name+TEXT("_")+M.Name;M.bCallable=false;M.bConst=F->isConstClassMethod;TMap<FString,FString> ObjectClasses;TMap<FString,FString> Directions;
                    for(const auto& V:A->arguments)
                    {
                        if(V.name=="id"){M.Id=DSLString(A->arguments,"id");continue;}
                        if(V.name=="out" || V.name=="ref"){DSLAddDirections(V,Directions);continue;}
                        if(V.name=="BlueprintCallable") M.bCallable=DSLBool(V);
                        else if(V.name=="BlueprintPure")M.bPure=DSLBool(V);
                        else if(V.name=="BlueprintEvent")M.bBlueprintEvent=DSLBool(V);
                        else if(V.name=="return_class" || V.name=="return_struct" || V.name=="return_enum" || (V.name.find("arg_")==0 && V.name.size()>10 && (V.name.substr(V.name.size()-6)=="_class" || V.name.substr(V.name.size()-7)=="_struct" || V.name.substr(V.name.size()-5)=="_enum"))){ObjectClasses.Add(DSLText(V.name),DSLString(A->arguments,V.name.c_str()));}
                        else if(DSLIsMetadata(V.name)) DSLMetadata(V,M.Metadata);else DSLFail(TEXT("Unsupported ue_function specifier: ")+DSLText(V.name));
                    }
                    if(M.bPure){const auto* Callable=A->arguments.find("BlueprintCallable",das::Type::tBool);if(Callable && !DSLBool(*Callable))DSLFail(TEXT("BlueprintPure conflicts with BlueprintCallable=false"));M.bCallable=true;}
                    if(M.bBlueprintEvent && !A->arguments.find("BlueprintCallable",das::Type::tBool))M.bCallable=true;
                    const FString ResultType=DSLTypeName(F->result);
                    if(!M.bCallable || F->isStaticClassMethod || !F->result || F->result->ref || ResultType.IsEmpty() || F->arguments.empty() || F->arguments.size()>17) DSLFail(TEXT("Adapter requires BlueprintCallable instance scalar/void method with up to 16 by-value inputs: ")+M.Name);
                    M.ReturnType.Kind=ResultType;if(ResultType==TEXT("delegate"))M.ReturnType=DSLDelegateMethodType(F->result);if(DSLContainer(F->result)){M.ReturnType=DSLContainerType(F->result,TypeFolder,TypeRegistry);DSLEnsureStructHelpers(F->result,DSLDirectPath(F->result,TypeFolder,TypeRegistry),Group,St->module,HelpersGenerated,Helpers);}if(ResultType==TEXT("enum")){M.ReturnType.TypePath=DSLConstraint(ObjectClasses,TEXT("return_enum"),EDSLTypeKind::Enum,DSLDirectEnum(F->result)?DSLDirectPath(F->result,TypeFolder,TypeRegistry):FString(),TypeScope);if(M.ReturnType.TypePath.IsEmpty())DSLFail(TEXT("Enum result requires return_enum"));}if(ResultType==TEXT("struct")){if((DSLDirectStruct(F->result) || (DSLMathPath(F->result)==TEXT("/Script/CoreUObject.Vector2D") || DSLMathPath(F->result)==TEXT("/Script/EnhancedInput.InputActionValue")))){M.ReturnType.TypePath=DSLConstraint(ObjectClasses,TEXT("return_struct"),EDSLTypeKind::Struct,DSLDirectPath(F->result,TypeFolder,TypeRegistry),TypeScope);DSLEnsureStructHelpers(F->result,M.ReturnType.TypePath,Group,St->module,HelpersGenerated,Helpers);}else if(F->result->annotation->name=="UEStructValue"){M.ReturnType.TypePath=DSLConstraint(ObjectClasses,TEXT("return_struct"),EDSLTypeKind::Struct,FString(),TypeScope);if(M.ReturnType.TypePath.IsEmpty())DSLFail(TEXT("UEStructValue result requires return_struct"));}else M.ReturnType.TypePath=DSLConstraint(ObjectClasses,TEXT("return_struct"),EDSLTypeKind::Struct,DSLMathPath(F->result),TypeScope);}if(ResultType==TEXT("class")){M.ReturnType.TypePath=DSLConstraint(ObjectClasses,TEXT("return_class"),EDSLTypeKind::Class,DSLTypedClass(F->result)?DSLTypedClassPath(F->result):FString(),TypeScope);if(M.ReturnType.TypePath.IsEmpty())M.ReturnType.TypePath=TEXT("/Script/CoreUObject.Object");}
                    if(ResultType==TEXT("object")){M.ReturnType.TypePath=DSLConstraint(ObjectClasses,TEXT("return_class"),DSLTypedInterface(F->result)?EDSLTypeKind::Interface:EDSLTypeKind::Class,DSLTypedReference(F->result)?DSLTypedReferencePath(F->result):FString(),TypeScope);if(M.ReturnType.TypePath.IsEmpty())M.ReturnType.TypePath=TEXT("/Script/CoreUObject.Object");}
                    const bool LegacySingle=C.Interfaces.IsEmpty() && Directions.IsEmpty() && F->arguments.size()==2 && !F->arguments[1]->type->ref && (ResultType==TEXT("int") || ResultType==TEXT("float") || ResultType==TEXT("bool")) && DSLTypeName(F->arguments[1]->type)==ResultType;
                    for(size_t I=1;I<F->arguments.size();++I){const auto& V=F->arguments[I];const FString K=DSLTypeName(V->type);const FString Direction=DSLParameterDirection(V,Directions.FindRef(DSLText(V->name)));if(K.IsEmpty() || K==TEXT("void")) DSLFail(TEXT("Unsupported parameter type/ref/default: ")+DSLText(V->name));FDasDeclaredParameter P;P.Default=K==TEXT("text")?DSLTextDefault(V->init):K==TEXT("byte")?DSLByteDefault(V->init):DSLParameterDefault(V->init);P.Direction=Direction;Directions.Remove(DSLText(V->name));P.Name=LegacySingle?TEXT("Input"):DSLText(V->name);P.Id=M.Id+TEXT(".")+(LegacySingle?TEXT("input"):P.Name);P.Type.Kind=K;if(K==TEXT("delegate")){if(Direction!=TEXT("in"))DSLFail(TEXT("Delegate-valued ref/out parameters are a later step"));P.Type=DSLDelegateMethodType(V->type);}if(DSLContainer(V->type)){P.Type=DSLContainerType(V->type,TypeFolder,TypeRegistry);DSLEnsureStructHelpers(V->type,DSLDirectPath(V->type,TypeFolder,TypeRegistry),Group,St->module,HelpersGenerated,Helpers);}if(K==TEXT("enum")){const FString Key=TEXT("arg_")+DSLText(V->name)+TEXT("_enum");P.Type.TypePath=DSLConstraint(ObjectClasses,Key,EDSLTypeKind::Enum,DSLDirectEnum(V->type)?DSLDirectPath(V->type,TypeFolder,TypeRegistry):FString(),TypeScope);if(P.Type.TypePath.IsEmpty())DSLFail(TEXT("Enum argument requires arg_name_enum"));}if(K==TEXT("struct")){if((DSLDirectStruct(V->type) || DSLMathPath(V->type)==TEXT("/Script/CoreUObject.Vector2D"))){const FString Key=TEXT("arg_")+DSLText(V->name)+TEXT("_struct");P.Type.TypePath=DSLConstraint(ObjectClasses,Key,EDSLTypeKind::Struct,DSLDirectPath(V->type,TypeFolder,TypeRegistry),TypeScope);DSLEnsureStructHelpers(V->type,P.Type.TypePath,Group,St->module,HelpersGenerated,Helpers);}else if(V->type->annotation->name=="UEStructValue"){const FString Key=TEXT("arg_")+DSLText(V->name)+TEXT("_struct");P.Type.TypePath=DSLConstraint(ObjectClasses,Key,EDSLTypeKind::Struct,FString(),TypeScope);if(P.Type.TypePath.IsEmpty())DSLFail(TEXT("UEStructValue parameter requires arg_name_struct"));}else{const FString Key=TEXT("arg_")+DSLText(V->name)+TEXT("_struct");P.Type.TypePath=DSLConstraint(ObjectClasses,Key,EDSLTypeKind::Struct,DSLMathPath(V->type),TypeScope);}}if(K==TEXT("class")){const FString Key=TEXT("arg_")+DSLText(V->name)+TEXT("_class");P.Type.TypePath=DSLConstraint(ObjectClasses,Key,EDSLTypeKind::Class,DSLTypedClass(V->type)?DSLTypedClassPath(V->type):FString(),TypeScope);if(P.Type.TypePath.IsEmpty())P.Type.TypePath=TEXT("/Script/CoreUObject.Object");}if(K==TEXT("object")){const FString Key=TEXT("arg_")+DSLText(V->name)+TEXT("_class");P.Type.TypePath=DSLConstraint(ObjectClasses,Key,DSLTypedInterface(V->type)?EDSLTypeKind::Interface:EDSLTypeKind::Class,DSLTypedReference(V->type)?DSLTypedReferencePath(V->type):FString(),TypeScope);if(P.Type.TypePath.IsEmpty())P.Type.TypePath=TEXT("/Script/CoreUObject.Object");}M.Parameters.Add(P);}
                    if(!ObjectClasses.IsEmpty())DSLFail(TEXT("Unused object class annotation"));if(!Directions.IsEmpty())DSLFail(TEXT("Unknown out/ref parameter name"));
                    DSLRememberMethodSource(RefKey,M,F,TypeScope);
                    C.Methods.Add(MoveTemp(M));
                }
                if(Derived){
                    for(const auto& M:BaseEffective.Methods)if(!C.Methods.ContainsByPredicate([&](const auto& Own){return Own.Name==M.Name;})){C.Methods.Add(M);const auto* Owner=Model.Classes.FindByPredicate([&](const auto& Cl){return Cl.Methods.ContainsByPredicate([&](const auto& Method){return Method.Id==M.Id;});});if(Owner){const FString From=M.ScriptFile+TEXT("::")+Owner->Name+TEXT("::")+M.Name;if(const auto* Descriptor=DSLMethodSourceSignatures.Find(From))DSLMethodSourceSignatures.Add(RefKey+TEXT("::")+M.Name,*Descriptor);}}
                }
                // Publish all method contracts before body rewriting opens mutually referring sources.
                DSLActiveRefDeclarations.Add(RefKey,C);
                das::TypeDeclPtr SelfEventReference=nullptr;
                if(C.Methods.ContainsByPredicate([&](const auto& M){return DSLVirtualMethod(C,M) || M.ScriptFile!=Script;})){
                    auto* Ref=new das::Structure();Ref->name=DSLUtf8(TEXT("__ue_self_events_")+C.Name);Ref->module=St->module;Ref->at=St->at;
                    das::Structure::FieldDeclaration Handle;Handle.flags=0;Handle.name="handle";Handle.type=DSLHandleType(Group,St->module,"UObjectHandle");Handle.at=St->at;Ref->fields.push_back(Handle);
                    auto Annotations=Group.findAnnotation("ue_object_ref",St->module);if(Annotations.size()!=1)DSLFail(TEXT("Missing authored reference annotation"));
                    auto* Annotation=new das::AnnotationDeclaration();Annotation->flags=0;Annotation->annotation=Annotations[0];Annotation->at=St->at;
                    Annotation->arguments.push_back(das::AnnotationArgument("script",DSLUtf8(Script)));Annotation->arguments.push_back(das::AnnotationArgument("type",DSLUtf8(C.Name)));Ref->annotations.push_back(Annotation);
                    if(!St->module->addStructure(Ref))DSLFail(TEXT("Self event reference name collision"));
                    SelfEventReference=new das::TypeDecl(das::Type::tStructure,St->at);SelfEventReference->structType=Ref;
                }
                das::TypeDeclPtr SuperReference=nullptr;
                if(Derived){auto* Ref=new das::Structure();Ref->name=DSLUtf8(TEXT("__ue_super_")+C.Name);Ref->module=St->module;Ref->at=St->at;das::Structure::FieldDeclaration Handle;Handle.flags=0;Handle.name="handle";Handle.type=DSLHandleType(Group,St->module,"UObjectHandle");Handle.at=St->at;Ref->fields.push_back(Handle);auto Annotations=Group.findAnnotation("ue_object_ref",St->module);if(Annotations.size()!=1)DSLFail(TEXT("Missing super reference annotation"));auto* A=new das::AnnotationDeclaration();A->flags=0;A->annotation=Annotations[0];A->at=St->at;const auto* Parent=Model.Classes.FindByPredicate([&](const auto& B){return B.Id==BaseId;});if(!Parent || Parent->SourceScript.IsEmpty())DSLFail(TEXT("Super parent source is unavailable"));A->arguments.push_back(das::AnnotationArgument("script",DSLUtf8(Parent->SourceScript)));A->arguments.push_back(das::AnnotationArgument("type",DSLUtf8(Parent->Name)));A->arguments.push_back(das::AnnotationArgument("_ue_super_id",DSLUtf8(C.Id)));Ref->annotations.push_back(A);if(!St->module->addStructure(Ref))DSLFail(TEXT("Super reference collision"));SuperReference=new das::TypeDecl(das::Type::tStructure,St->at);SuperReference->structType=Ref;}
                // Script-only instance methods are lowered once into same-context
                // functions. They are not reflected, exported, or added to the IR.
                struct FLocalVariant{das::StructurePtr Owner;das::FunctionPtr Function;FString Name;das::string Target;};
                TArray<das::StructurePtr> LocalChain;for(auto Owner=St;Owner && DSLNativeBasePath(Owner).IsEmpty();Owner=Owner->parent)LocalChain.Add(Owner);
                TMap<FString,das::string> QualifiedLocals;TArray<FLocalVariant> LocalVariants;TMap<FString,das::string> DynamicLocals,ParentLocals;TMap<FString,das::string> LocalRenames;
                auto VariantName=[&](const FString& Context,das::StructurePtr Owner,const FString& Name){return DSLUtf8(TEXT("__ue_local_impl_")+Context+TEXT("_")+FMD5::HashAnsiString(*(DSLText(Owner->at.fileInfo->name)+TEXT("::")+DSLText(Owner->name)))+TEXT("_")+Name);};
                for(int32 Depth=0;Depth<LocalChain.Num();++Depth){auto Owner=LocalChain[Depth];Owner->module->functions.foreach([&](const das::FunctionPtr& F){
                    if(F->classParent!=Owner || F->generated || DSLAnnotation(F->annotations,"ue_function") || DSLAnnotation(F->annotations,"ue_event") || DSLAnnotation(F->annotations,"ue_lifecycle") || DSLStandardLifecycle(F))return;
                    if(F->isStaticClassMethod || F->arguments.empty() || !F->result || F->result->ref)DSLFail(TEXT("Script-only helpers require instance methods with value/void results"));
                    const FString Name=DSLText(F->name.substr(F->name.find_last_of(das::string(1,char(96))+das::string(1,char(39)))+1));const auto Target=VariantName(C.Name,Owner,Name);
                    if(const auto* Selected=DynamicLocals.Find(Name)){
                        const auto* DerivedVariant=LocalVariants.FindByPredicate([&](const auto& V){return V.Target==*Selected;});auto* D=DerivedVariant->Function;
                        if(D->isConstClassMethod!=F->isConstClassMethod || D->arguments.size()!=F->arguments.size() || D->result->describe()!=F->result->describe())DSLFail(TEXT("Helper override requires unchanged signature: ")+Name);
                        for(size_t I=1;I<F->arguments.size();++I)if(D->arguments[I]->type->describe()!=F->arguments[I]->type->describe())DSLFail(TEXT("Helper override requires unchanged parameter types: ")+Name);
                    }else DynamicLocals.Add(Name,Target);
                    if(Depth>0 && !ParentLocals.Contains(Name))ParentLocals.Add(Name,Target);
                    LocalVariants.Add({Owner,F,Name,Target});
                    for(auto Context:LocalChain)LocalRenames.Add(DSLText(VariantName(DSLText(Context->name),Owner,Name)),Target);
                });}
                for(const auto& V:LocalVariants){QualifiedLocals.Add(DSLText(V.Function->name),DynamicLocals[V.Name]);QualifiedLocals.Add(DSLText(V.Owner->module->name+"::"+V.Function->name),DynamicLocals[V.Name]);}
                for(const auto& Variant:LocalVariants){const auto& F=Variant.Function;auto Clone=F->clone();Clone->name=Variant.Target;Clone->classParent=nullptr;Clone->isClassMethod=false;Clone->annotations.clear();Clone->exports=false;Clone->generated=true;Clone->privateFunction=false;Clone->isConstClassMethod=false;
                    TMap<FString,das::string> Supers;bool PastOwner=false;for(auto Owner:LocalChain){if(Owner==Variant.Owner){PastOwner=true;continue;}if(!PastOwner)continue;for(const auto& V:LocalVariants)if(V.Owner==Owner && !Supers.Contains(V.Name))Supers.Add(V.Name,V.Target);}
                    FDSLTypeCallVisitor Types(TypeScope);Clone->body=Clone->body->visit(Types);
                    FDSLBodyVisitor Rewrite(C,Clone->arguments[0]->name,St,TypeFolder,TypeRegistry,Group,HelpersGenerated,Helpers);Rewrite.SelfReference=SelfEventReference;Rewrite.SuperReference=SuperReference;Rewrite.LocalTargets=DynamicLocals;Rewrite.LocalSupers=Supers;Rewrite.LocalRenames=LocalRenames;Rewrite.QualifiedLocals=QualifiedLocals;Clone->body=Clone->body->visit(Rewrite);
                    if(Variant.Owner==St && Derived){struct FHelperSuperSource:das::Visitor{FDSLBodyVisitor& Owner;explicit FHelperSuperSource(FDSLBodyVisitor& In):Owner(In){}virtual das::ExpressionPtr visit(das::ExprInvoke* E)override{return Owner.SuperCall(E);}} SourceSupers(Rewrite);F->body=F->body->visit(SourceSupers);}
                    if(Variant.Owner==St){struct FSourceLocalCalls:das::Visitor{das::string Self;TMap<FString,das::string> Names;FSourceLocalCalls(const das::string& S):Self(S){}virtual das::ExpressionPtr visit(das::ExprInvoke* E)override{if(!E->isInvokeMethod || E->arguments.empty() || !E->arguments[0]->rtti_isField())return E;auto* Field=static_cast<das::ExprField*>(E->arguments[0]);if(!Field->value->rtti_isVar() || static_cast<das::ExprVar*>(Field->value)->name!=Self)return E;const auto* Name=Names.Find(DSLText(Field->name));if(!Name)return E;auto* Call=DSLCall(E->at,*Name,{Field->value});for(size_t I=2;I<E->arguments.size();++I)Call->arguments.push_back(E->arguments[I]);return Call;}} SourceCalls(F->arguments[0]->name);for(const auto& V:LocalVariants)if(!SourceCalls.Names.Contains(V.Name))SourceCalls.Names.Add(V.Name,V.Owner->module==St->module?V.Function->name:V.Owner->module->name+"::"+V.Function->name);F->body=F->body->visit(SourceCalls);}
                    Clone->arguments.erase(Clone->arguments.begin());if(!Rewrite.Error.IsEmpty())DSLFail(Rewrite.Error);Generated.push_back(Clone);
                }
                for(const auto& F:Functions){
                    const FString MethodName=DSLText(F->name.substr(F->name.find_last_of(das::string(1,char(96))+das::string(1,char(39)))+1));
                    const auto M=*C.Methods.FindByPredicate([&](const auto& Item){return Item.Name==MethodName;});
                    const FString ResultType=DSLTypeName(F->result);
                    FDSLTypeCallVisitor TypeCalls(TypeScope);F->body=F->body->visit(TypeCalls);
                    auto Clone=F->clone();Clone->name=DSLUtf8(M.EntryPoint);Clone->classParent=nullptr;Clone->isClassMethod=false;Clone->annotations.clear();Clone->exports=true;Clone->generated=true;Clone->privateFunction=false;Clone->isStaticClassMethod=false;Clone->isConstClassMethod=false;
                    FDSLBodyVisitor Rewrite(C,Clone->arguments[0]->name,St,TypeFolder,TypeRegistry,Group,HelpersGenerated,Helpers);Rewrite.SelfReference=SelfEventReference;Rewrite.SuperReference=SuperReference;Rewrite.LocalTargets=DynamicLocals;Rewrite.LocalSupers=ParentLocals;Rewrite.LocalRenames=LocalRenames;Rewrite.QualifiedLocals=QualifiedLocals;Clone->body=Clone->body->visit(Rewrite);FDSLSourceTimerVisitor SourceTimers(Rewrite);F->body=F->body->visit(SourceTimers);if(Derived){struct FSuperSource:das::Visitor{FDSLBodyVisitor& Owner;explicit FSuperSource(FDSLBodyVisitor& In):Owner(In){}virtual das::ExpressionPtr visit(das::ExprInvoke* E)override{return Owner.SuperCall(E);}} SuperSource(Rewrite);F->body=F->body->visit(SuperSource);}Clone->arguments.erase(Clone->arguments.begin());
                    if(!Rewrite.Error.IsEmpty()) DSLFail(Rewrite.Error);
                    bool HasOutputs=false;for(const auto& P:M.Parameters)HasOutputs |= P.Direction!=TEXT("in");bool Direct=true;
                    if(Direct){
                        auto Wrapper=Clone->clone();Clone->name=DSLUtf8(TEXT("__impl_")+M.EntryPoint);Clone->exports=false;auto* Call=new das::ExprCall(F->at,Clone->name);auto* Block=new das::ExprBlock();Block->at=F->at;
                        auto Local=[&](const das::string& Name,const das::TypeDeclPtr& Type,das::ExpressionPtr Init){auto* V=new das::Variable();V->at=F->at;V->name=Name;V->type=DSLCopyType(Type);V->type->ref=false;V->type->constant=false;V->init=Init;V->init_via_move=DSLContainer(Type);auto* Let=new das::ExprLet();Let->at=F->at;Let->variables.push_back(V);Block->list.push_back(Let);};
                        for(size_t I=0;I<Wrapper->arguments.size();++I){auto& Arg=Wrapper->arguments[I];Arg->init=nullptr;auto Original=F->arguments[I+1]->type;Arg->type=DSLCopyType(Arg->type);Arg->type->ref=false;Arg->type->constant=true;das::ExpressionPtr Value=new das::ExprVar(F->at,Arg->name);
                            if(DSLDelegate(Original)){Arg->type=new das::TypeDecl(das::Type::tUInt64,F->at);Value=DSLDirectUnpack(F->at,Original,Value,FString());}if(DSLTypedClass(Original) || DSLContainer(Original) || DSLDirectEnum(Original) || (DSLDirectStruct(Original) || (DSLMathPath(Original)==TEXT("/Script/CoreUObject.Vector2D") || DSLMathPath(Original)==TEXT("/Script/EnhancedInput.InputActionValue"))) || DSLTypedReference(Original)){Arg->type=DSLHandleType(Group,St->module,DSLTypedClass(Original)?"UEClassHandle":DSLTypedReference(Original)?"UObjectHandle":DSLDirectEnum(Original)?"UEEnumValue":"UEStructValue");Value=DSLDirectUnpack(F->at,Original,Value,DSLContainer(Original)?DSLDirectPath(Original,TypeFolder,TypeRegistry):M.Parameters[int32(I)].Type.TypePath);}
                            if(M.Parameters[int32(I)].Direction!=TEXT("in")){const das::string Name="__ue_ref_"+das::to_string(I);Local(Name,Original,Value);Value=new das::ExprVar(F->at,Name);}Call->arguments.push_back(Value);
                        }
                        das::ExpressionPtr Value=Call;
                        if(HasOutputs){if(ResultType==TEXT("void")){Block->list.push_back(Call);Value=nullptr;}else{Local("__ue_return",F->result,Call);Value=new das::ExprVar(F->at,"__ue_return");}
                            for(int32 I=0;I<M.Parameters.Num();++I)if(M.Parameters[I].Direction!=TEXT("in")){auto Original=F->arguments[size_t(I)+1]->type;das::ExpressionPtr Updated=new das::ExprVar(F->at,"__ue_ref_"+das::to_string(I));const FString Kind=M.Parameters[I].Type.Kind;const das::string Math=DSLUtf8(DSLMathPath(Original));const das::string Suffix=Kind==TEXT("class")?"class":Kind==TEXT("object")?"object":Kind==TEXT("enum")?"enum":Kind==TEXT("struct")?(Math=="/Script/CoreUObject.Vector"?"vector":Math=="/Script/CoreUObject.Rotator"?"rotator":Math=="/Script/CoreUObject.Transform"?"transform":"struct"):DSLContainer(Original)?"struct":DSLScalarSuffix(Kind);if(DSLTypedClass(Original) || DSLContainer(Original) || (DSLDirectStruct(Original) || (DSLMathPath(Original)==TEXT("/Script/CoreUObject.Vector2D") || DSLMathPath(Original)==TEXT("/Script/EnhancedInput.InputActionValue"))) || DSLDirectEnum(Original) || DSLTypedReference(Original))Updated=DSLDirectPack(F->at,Original,Updated,DSLContainer(Original)?DSLDirectPath(Original,TypeFolder,TypeRegistry):M.Parameters[I].Type.TypePath);Block->list.push_back(DSLCall(F->at,"ue_frame_out_"+Suffix,{new das::ExprConstInt(F->at,I),Updated}));}
                        }
                        if(DSLDelegate(F->result)){Wrapper->result=new das::TypeDecl(das::Type::tUInt64,F->at);Value=DSLDirectPack(F->at,F->result,Value,FString());}if(DSLTypedClass(F->result) || DSLContainer(F->result) || DSLDirectEnum(F->result) || (DSLDirectStruct(F->result) || (DSLMathPath(F->result)==TEXT("/Script/CoreUObject.Vector2D") || DSLMathPath(F->result)==TEXT("/Script/EnhancedInput.InputActionValue"))) || DSLTypedReference(F->result)){Wrapper->result=DSLHandleType(Group,St->module,DSLTypedClass(F->result)?"UEClassHandle":DSLTypedReference(F->result)?"UObjectHandle":DSLDirectEnum(F->result)?"UEEnumValue":"UEStructValue");Value=DSLDirectPack(F->at,F->result,Value,DSLContainer(F->result)?DSLDirectPath(F->result,TypeFolder,TypeRegistry):M.ReturnType.TypePath);}
                        if(Value)Block->list.push_back(M.ReturnType.Kind==TEXT("void")?Value:static_cast<das::ExpressionPtr>(new das::ExprReturn(F->at,Value)));Wrapper->body=Block;Generated.push_back(Wrapper);
                    }
                    Generated.push_back(Clone);
                }
                das::vector<das::FunctionPtr> Callbacks;St->module->functions.foreach([&](const das::FunctionPtr& F){if(F->classParent==St && (DSLAnnotation(F->annotations,"ue_lifecycle") || DSLAnnotation(F->annotations,"ue_event") || DSLStandardLifecycle(F)))Callbacks.push_back(F);});
                TArray<FDasDeclaredCallback> OwnedCallbacks;
                std::sort(Callbacks.begin(),Callbacks.end(),[](const das::FunctionPtr& A,const das::FunctionPtr& B){return A->name<B->name;});das::ExprBlock* BindBlock=nullptr;das::FunctionPtr BindPrototype=nullptr;
                for(const auto& F:Callbacks){const auto* Life=DSLAnnotation(F->annotations,"ue_lifecycle");const auto* Event=DSLAnnotation(F->annotations,"ue_event");if((Life && Event) || DSLAnnotation(F->annotations,"ue_function"))DSLFail(TEXT("Conflicting UE callback/function annotations"));FDasDeclaredCallback Callback;Callback.Name=DSLText(F->name.substr(F->name.find_last_of(das::string(1,char(96))+das::string(1,char(39)))+1));Callback.Kind=Event?TEXT("event"):Callback.Name;if(F->result && F->result->baseType==das::Type::autoinfer)F->result=new das::TypeDecl(das::Type::tVoid,F->at);if(F->isStaticClassMethod || !F->result || F->result->ref || DSLTypeName(F->result)!=TEXT("void") || F->arguments.empty())DSLFail(TEXT("UE callbacks must return void"));const bool MultiEvent=Event && F->arguments.size()>2;FString Parameter=Event?(F->arguments.size()==1?FString():F->arguments.size()==2?DSLTypeName(F->arguments[1]->type):TEXT("invalid")):Callback.Kind==TEXT("Tick")?TEXT("float"):Callback.Kind==TEXT("EndPlay")?TEXT("int"):FString();if(MultiEvent){Parameter=TEXT("struct");Callback.ParameterKind=TEXT("struct");Callback.ParameterTypePath=DSLCallbackTuple(F,TypeFolder,TypeRegistry)->GetPathName();for(size_t I=1;I<F->arguments.size();++I){const auto T=F->arguments[I]->type;const auto D=DSLCallbackField(T,TypeFolder,TypeRegistry);if(D.Kind==TEXT("struct"))DSLEnsureStructHelpers(T,D.TypePath,Group,St->module,HelpersGenerated,Helpers);}}else if(Event){Parameter=F->arguments.size()==1?FString():F->arguments.size()==2?DSLCallbackKind(F->arguments[1]->type):TEXT("invalid");Callback.ParameterKind=Parameter.IsEmpty()?TEXT("void"):Parameter;if(!FindDasCallbackType(Callback.ParameterKind))DSLFail(TEXT("Unsupported event callback payload"));if(Callback.ParameterKind==TEXT("enum") || Callback.ParameterKind==TEXT("struct")){const auto T=F->arguments[1]->type;if(!DSLDirectEnum(T) && !DSLDirectStruct(T) && !DSLContainer(T))DSLFail(TEXT("Event value requires a concrete registered type/carrier"));Callback.ParameterTypePath=DSLDirectPath(T,TypeFolder,TypeRegistry);FString Issue;if(!ValidateDasCallbackTypePath(Callback.ParameterKind,Callback.ParameterTypePath,Issue))DSLFail(Issue);}if(Parameter==TEXT("class")){if(!DSLTypedClass(F->arguments[1]->type))DSLFail(TEXT("Class events require UEClass<T>"));Callback.ParameterTypePath=DSLTypedClassPath(F->arguments[1]->type);}if(Parameter==TEXT("object")){if(!DSLTypedReference(F->arguments[1]->type))DSLFail(TEXT("Object events require a typed object/interface wrapper"));Callback.ParameterTypePath=DSLTypedReferencePath(F->arguments[1]->type);}}if(!MultiEvent && F->arguments.size()!=(Parameter.IsEmpty()?1:2))DSLFail(TEXT("UE callback signature mismatch: ")+Callback.Name);if(!MultiEvent && !Parameter.IsEmpty() && (!F->arguments[1]->type || F->arguments[1]->type->ref || F->arguments[1]->init || DSLTypeName(F->arguments[1]->type)!=((Parameter==TEXT("vector2d") || Parameter==TEXT("vector") || Parameter==TEXT("rotator") || Parameter==TEXT("transform"))?TEXT("struct"):Parameter.StartsWith(TEXT("array_"))?TEXT("array"):(Parameter==TEXT("struct") && DSLContainer(F->arguments[1]->type))?DSLTypeName(F->arguments[1]->type):Parameter)))DSLFail(TEXT("UE callback parameter must be by-value ")+Parameter);if(!Event && Callback.Kind!=TEXT("ConstructionScript") && Callback.Kind!=TEXT("Initialize") && Callback.Kind!=TEXT("BeginPlay") && Callback.Kind!=TEXT("Tick") && Callback.Kind!=TEXT("EndPlay"))DSLFail(TEXT("Unknown lifecycle method; expected ConstructionScript, Initialize, BeginPlay, Tick or EndPlay"));
                    const FString Entry=Event?TEXT("ue_")+C.Name+TEXT("_")+Callback.Name:Callback.Kind==TEXT("ConstructionScript")?TEXT("__ue_construct"):Callback.Kind==TEXT("Initialize")?TEXT("__ue_initialize"):Callback.Kind==TEXT("BeginPlay")?TEXT("begin_play"):Callback.Kind==TEXT("Tick")?TEXT("tick"):TEXT("end_play");FDSLTypeCallVisitor TypeCalls(TypeScope);F->body=F->body->visit(TypeCalls);auto Clone=F->clone();Clone->name=DSLUtf8(Entry);Clone->classParent=nullptr;Clone->isClassMethod=false;Clone->annotations.clear();Clone->exports=true;Clone->generated=true;Clone->privateFunction=false;Clone->isConstClassMethod=false;FDSLBodyVisitor Rewrite(C,Clone->arguments[0]->name,St,TypeFolder,TypeRegistry,Group,HelpersGenerated,Helpers);Rewrite.SelfReference=SelfEventReference;Rewrite.SuperReference=SuperReference;Rewrite.LocalTargets=DynamicLocals;Rewrite.LocalSupers=ParentLocals;Rewrite.LocalRenames=LocalRenames;Rewrite.QualifiedLocals=QualifiedLocals;Clone->body=Clone->body->visit(Rewrite);FDSLSourceTimerVisitor SourceTimers(Rewrite);F->body=F->body->visit(SourceTimers);if(Derived){struct FSuperSource:das::Visitor{FDSLBodyVisitor& Owner;explicit FSuperSource(FDSLBodyVisitor& In):Owner(In){}virtual das::ExpressionPtr visit(das::ExprInvoke* E)override{return Owner.SuperCall(E);}} SuperSource(Rewrite);F->body=F->body->visit(SuperSource);}Clone->arguments.erase(Clone->arguments.begin());if(MultiEvent)DSLCallbackUnpackArguments(Clone,TypeFolder,TypeRegistry,Group,St->module);if(!MultiEvent && Event && (Parameter==TEXT("object") || Parameter==TEXT("class"))){auto Arg=Clone->arguments[0];const auto OriginalName=Arg->name;const auto SourceType=DSLCopyType(Arg->type);Arg->name="__ue_event_raw_object";Arg->type=DSLHandleType(Group,St->module,Parameter==TEXT("class")?"UEClassHandle":"UObjectHandle");auto* Local=new das::Variable();Local->at=F->at;Local->name=OriginalName;Local->type=SourceType;Local->type->ref=false;Local->init=DSLDirectUnpack(F->at,SourceType,new das::ExprVar(F->at,Arg->name),Parameter==TEXT("class")?DSLTypedClassPath(SourceType):DSLTypedReferencePath(SourceType));auto* Let=new das::ExprLet();Let->at=F->at;Let->variables.push_back(Local);static_cast<das::ExprBlock*>(Clone->body)->list.insert(static_cast<das::ExprBlock*>(Clone->body)->list.begin(),Let);}if(!MultiEvent && Event && (Parameter.StartsWith(TEXT("array_")) || Parameter==TEXT("vector2d") || Parameter==TEXT("struct") || Parameter==TEXT("enum"))){auto Arg=Clone->arguments[0];const auto OriginalName=Arg->name;const auto SourceType=DSLCopyType(Arg->type);const FString Carrier=Parameter==TEXT("vector2d")?DSLMathPath(SourceType):DSLDirectPath(SourceType,TypeFolder,TypeRegistry);if(Parameter!=TEXT("enum"))DSLEnsureStructHelpers(SourceType,Carrier,Group,St->module,HelpersGenerated,Helpers);Arg->name="__ue_event_raw_array";Arg->type=DSLHandleType(Group,St->module,Parameter==TEXT("enum")?"UEEnumValue":"UEStructValue");auto* Local=new das::Variable();Local->at=F->at;Local->name=OriginalName;Local->type=SourceType;Local->type->ref=false;Local->type->constant=false;Local->init_via_move=true;Local->init=DSLDirectUnpack(F->at,SourceType,new das::ExprVar(F->at,Arg->name),Carrier);auto* Let=new das::ExprLet();Let->at=F->at;Let->variables.push_back(Local);static_cast<das::ExprBlock*>(Clone->body)->list.insert(static_cast<das::ExprBlock*>(Clone->body)->list.begin(),Let);}if(!Rewrite.Error.IsEmpty())DSLFail(Rewrite.Error);Generated.push_back(Clone);
                    if(Event){if(!BindBlock){BindBlock=new das::ExprBlock();BindBlock->at=St->at;}Callback.Source=DSLString(Event->arguments,"source",TEXT("self"));Callback.EventName=DSLString(Event->arguments,"name");if(Callback.Source!=TEXT("self") && Callback.Source!=TEXT("owner") && !C.Components.ContainsByPredicate([&](const auto& Component){return Component.Name==Callback.Source;}))DSLFail(TEXT("Event source must be self, owner or a declared component"));if(Callback.Source==TEXT("owner") && C.NativeParent!=TEXT("/Script/Engine.ActorComponent"))DSLFail(TEXT("Owner event source requires ActorComponent"));auto* SelfObject=DSLCall(F->at,"ue_self_object",{});das::ExpressionPtr Source=Callback.Source==TEXT("owner")?DSLCall(F->at,"ue_component_owner",{SelfObject}):Callback.Source==TEXT("self")?static_cast<das::ExpressionPtr>(SelfObject):static_cast<das::ExpressionPtr>(DSLCall(F->at,"ue_get_self_object",{new das::ExprConstString(F->at,DSLUtf8(Callback.Source))}));auto* Subscribe=DSLCall(F->at,DSLUtf8(TEXT("ue_subscribe_object_")+Callback.ParameterKind),{Source,new das::ExprConstString(F->at,DSLUtf8(Callback.EventName)),new das::ExprConstString(F->at,DSLUtf8(Entry))});if(!Callback.ParameterTypePath.IsEmpty())Subscribe->arguments.push_back(new das::ExprConstString(F->at,DSLUtf8(Callback.ParameterTypePath)));BindBlock->list.push_back(Subscribe);BindPrototype=Clone;}
                    OwnedCallbacks.Add(Callback);C.Callbacks.RemoveAll([&](const auto& P){return P.Kind==Callback.Kind && Callback.Kind!=TEXT("event");});C.Callbacks.Add(Callback);
                }
                if(Derived){
                    if(Script==BaseEffective.SourceScript && !C.Callbacks.IsEmpty())DSLFail(TEXT("Lifecycle inheritance requires separate source files"));
                    auto ParentHooks=BaseEffective.Callbacks;
                    if(ParentHooks.ContainsByPredicate([](const auto& Hook){return Hook.Kind==TEXT("event");}))for(const auto* Kind:{TEXT("BeginPlay"),TEXT("EndPlay")})if(!ParentHooks.ContainsByPredicate([&](const auto& Hook){return Hook.Kind==Kind;})){FDasDeclaredCallback Hook;Hook.Name=Kind;Hook.Kind=Kind;ParentHooks.Add(Hook);}
                    for(const auto& ParentHook:ParentHooks){if(ParentHook.Kind==TEXT("event"))continue;if(OwnedCallbacks.ContainsByPredicate([&](const auto& Hook){return Hook.Kind==ParentHook.Kind;}))continue;
                        auto* Forward=new das::Function();Forward->at=St->at;Forward->generated=true;Forward->privateFunction=false;Forward->exports=true;Forward->result=new das::TypeDecl(das::Type::tVoid,St->at);const int Kind=ParentHook.Kind==TEXT("BeginPlay")?0:ParentHook.Kind==TEXT("Tick")?1:ParentHook.Kind==TEXT("EndPlay")?2:ParentHook.Kind==TEXT("Initialize")?3:4;Forward->name=Kind==0?"begin_play":Kind==1?"tick":Kind==2?"end_play":Kind==3?"__ue_initialize":"__ue_construct";
                        if(Kind==1 || Kind==2){auto* A=new das::Variable();A->at=St->at;A->name="value";A->type=new das::TypeDecl(Kind==1?das::Type::tFloat:das::Type::tInt,St->at);Forward->arguments.push_back(A);}auto* Body=new das::ExprBlock();Body->at=St->at;Body->list.push_back(DSLCall(St->at,"__ue_parent_lifecycle",{new das::ExprConstString(St->at,DSLUtf8(C.Id)),new das::ExprConstInt(St->at,Kind),Kind==1?static_cast<das::ExpressionPtr>(new das::ExprVar(St->at,"value")):static_cast<das::ExpressionPtr>(new das::ExprConstFloat(St->at,0.f)),Kind==2?static_cast<das::ExpressionPtr>(new das::ExprVar(St->at,"value")):static_cast<das::ExpressionPtr>(new das::ExprConstInt(St->at,0))}));Forward->body=Body;Generated.push_back(Forward);
                    }
                }
                if(Derived && !BindPrototype && BaseEffective.Callbacks.ContainsByPredicate([](const auto& Hook){return Hook.Kind==TEXT("event");})){
                    // Do not resolve an imported parent's binder as this script's binder.
                    auto* EmptyBind=new das::Function();EmptyBind->at=St->at;EmptyBind->name="bind_events";EmptyBind->generated=true;EmptyBind->exports=true;EmptyBind->privateFunction=false;EmptyBind->result=new das::TypeDecl(das::Type::tVoid,St->at);auto* EmptyBody=new das::ExprBlock();EmptyBody->at=St->at;EmptyBind->body=EmptyBody;Generated.push_back(EmptyBind);
                }
                if(BindPrototype){auto Bind=BindPrototype->clone();Bind->name="bind_events";Bind->arguments.clear();Bind->body=BindBlock;Generated.push_back(Bind);}
                // Do this after cloning all runtime bodies. Source helpers retain normal
                // daScript self typing; only native fields lower to the checked runtime ABI.
                das::vector<das::FunctionPtr> NativeSources;St->module->functions.foreach([&](const das::FunctionPtr& F){if(F->classParent==St && !F->generated && F->body && !F->arguments.empty())NativeSources.push_back(F);});
                for(const auto& F:NativeSources){FDSLNativeSourceVisitor Native(C,F->arguments[0]->name,St,TypeFolder,TypeRegistry,Group,HelpersGenerated,Helpers);Native.NativeOnly=true;F->body=F->body->visit(Native);if(!Native.Error.IsEmpty())DSLFail(Native.Error);}
                DSLActiveRefDeclarations.Add(RefKey,C);
                auto Own=C;if(Derived){if(!DSLAnnotation(St->annotations,"ue_async_physics"))Own.AsyncPhysics.Reset();Own.NativeParent.Reset();Own.ParentId=BaseId;Own.Properties.RemoveAll([&](const auto& P){return BaseEffective.Properties.ContainsByPredicate([&](const auto& Base){return Base.Id==P.Id;});});Own.Methods.RemoveAll([&](const auto& M){return BaseEffective.Methods.ContainsByPredicate([&](const auto& Base){return Base.Id==M.Id;});});Own.Components.RemoveAll([&](const auto& P){return BaseEffective.Components.ContainsByPredicate([&](const auto& Base){return Base.Id==P.Id;});});Own.Callbacks=OwnedCallbacks;Own.Interfaces.RemoveAll([&](const auto& P){return BaseEffective.Interfaces.Contains(P);});}
                Model.Classes.Add(Own);FString Json,Issue;
                if(!Model.ToJson(Json,Issue)) DSLFail(Issue);
                FDasClassSchema Supported;if((!C.Methods.IsEmpty() || !C.Callbacks.IsEmpty() || C.AsyncPhysics.IsSet()) && !Model.ToActorSchema(C.Id,Supported,Issue)) DSLFail(Issue);
                for(const auto& F:HelpersGenerated)if(!St->module->addFunction(F,true))DSLFail(TEXT("Direct struct helper collision"));
                for(const auto& F:Generated) if(!St->module->addFunction(F,true)) DSLFail(TEXT("Generated entry point collision"));
                Args.push_back(das::AnnotationArgument(DSLModelKey,DSLUtf8(Json)));return true;
            }
            catch(const std::exception& E){Error=E.what();return false;}
        }
    };


    const char* DSLInterfaceKey="_ue_interface_json";
    struct FUEInterfaceAnnotation:das::StructureAnnotation {
        FUEInterfaceAnnotation():StructureAnnotation("ue_interface"){}
        virtual bool look(const das::StructurePtr& St,das::ModuleGroup&,das::AnnotationArgumentList&,das::string& Error)override{try{DSLDeclarationKind(St->annotations);DSLAnnotation(St->annotations,"ue_interface");return true;}catch(const std::exception& X){Error=X.what();return false;}}
        virtual bool touch(const das::StructurePtr& St,das::ModuleGroup& Group,das::AnnotationArgumentList& Args,das::string& Error)override{try{
            if(const auto* Wire=Args.find(DSLInterfaceKey,das::Type::tString)){if(Wire->at.fileInfo)DSLFail(TEXT("Reserved interface annotation argument"));return true;}
            DSLCommentTooltip(St->at,Args);
            DSLAnnotationArguments(Args,{TEXT("out")});if(!St->isClass || St->parent)DSLFail(TEXT("ue_interface requires a class without inheritance"));
            FDasAuthoringModule Model;Model.Id=DSLString(Args,"module_id",FPaths::GetBaseFilename(DSLText(St->at.fileInfo->name)));FDasDeclaredClass C;C.Id=DSLString(Args,"id",Model.Id+TEXT(".")+DSLText(St->name));C.Name=DSLText(St->name);C.NativeParent=TEXT("/Script/CoreUObject.Interface");TSet<FString> LegacyOuts;
            for(const auto& A:Args){if(A.name=="module_id" || A.name=="id" || A.name=="type_registry" || A.name=="type_folder")continue;if(A.name=="BlueprintType"){if(!DSLBool(A))DSLFail(TEXT("BlueprintType must be true"));continue;}if(A.name=="out"){TArray<FString> Names;DSLString(Args,"out").ParseIntoArray(Names,TEXT(","),false);for(auto N:Names){N=N.TrimStartAndEnd();if(N.IsEmpty() || LegacyOuts.Contains(N))DSLFail(TEXT("Duplicate/empty interface out name"));LegacyOuts.Add(N);}continue;}if(DSLIsMetadata(A.name))DSLMetadata(A,C.Metadata);else DSLFail(TEXT("Unknown ue_interface annotation"));}
            const FString Folder=DSLString(Args,"type_folder"),Registry=DSLString(Args,"type_registry");if(!Folder.IsEmpty() && !Registry.IsEmpty())DSLFail(TEXT("Choose type_registry or type_folder"));const FDSLTypeScope TypeScope{Model.Id,Folder,Registry,&Group,St->module};
            auto Type=[&](const das::TypeDeclPtr& T,TMap<FString,FString>& Constraints,const FString& Prefix){
                FDasDeclaredType D;D.Kind=DSLTypeName(T);if(DSLContainer(T))return DSLContainerType(T,Folder,Registry);
                if(D.Kind==TEXT("object")){D.TypePath=DSLConstraint(Constraints,Prefix+TEXT("class"),DSLTypedInterface(T)?EDSLTypeKind::Interface:EDSLTypeKind::Class,DSLTypedReference(T)?DSLTypedReferencePath(T):FString(),TypeScope);if(D.TypePath.IsEmpty())D.TypePath=TEXT("/Script/CoreUObject.Object");}
                else if(D.Kind==TEXT("class")){D.TypePath=DSLConstraint(Constraints,Prefix+TEXT("class"),EDSLTypeKind::Class,DSLTypedClass(T)?DSLTypedClassPath(T):FString(),TypeScope);if(D.TypePath.IsEmpty())DSLFail(TEXT("Interface class requires UEClass<T> or an explicit base constraint"));}
                else if(D.Kind==TEXT("enum")){D.TypePath=DSLConstraint(Constraints,Prefix+TEXT("enum"),EDSLTypeKind::Enum,DSLDirectEnum(T)?DSLDirectPath(T,Folder,Registry):FString(),TypeScope);if(D.TypePath.IsEmpty())DSLFail(TEXT("Interface enum requires an explicit type constraint"));}
                else if(D.Kind==TEXT("struct")){const FString Inferred=DSLDirectStruct(T)?DSLDirectPath(T,Folder,Registry):T->annotation && T->annotation->name!="UEStructValue"?DSLMathPath(T):FString();D.TypePath=DSLConstraint(Constraints,Prefix+TEXT("struct"),EDSLTypeKind::Struct,Inferred,TypeScope);if(D.TypePath.IsEmpty())DSLFail(TEXT("Interface snapshot requires an explicit struct constraint"));}
                else if(D.Kind!=TEXT("byte") && D.Kind!=TEXT("text") && D.Kind!=TEXT("int") && D.Kind!=TEXT("float") && D.Kind!=TEXT("double") && D.Kind!=TEXT("bool") && D.Kind!=TEXT("string") && D.Kind!=TEXT("name") && D.Kind!=TEXT("void"))DSLFail(TEXT("Unsupported authored interface signature type"));
                return D;
            };
            St->module->functions.foreach([&](const das::FunctionPtr& F){if(F->classParent!=St || F->generated)return;const auto Name=F->name.substr(F->name.find_last_of("`")+1);if(F->isStaticClassMethod || Name==St->name || Name=="finalize" || !St->findField(Name))DSLFail(TEXT("Interface definitions cannot declare static methods, constructors or finalizers"));});
            for(const auto& Field:St->fields){
                if(Field.generated || Field.name=="__rtti")continue;if(!Field.classMethod)DSLFail(TEXT("Interface definitions cannot declare fields"));
                FDasDeclaredMethod M;M.Name=DSLText(Field.name);M.Id=C.Id+TEXT(".")+M.Name;M.ScriptFile=FPaths::GetCleanFilename(DSLText(St->at.fileInfo->name));M.EntryPoint=M.Name;
                TMap<FString,FString> Directions,Constraints;das::FunctionPtr Function=nullptr;das::TypeDeclPtr Return;das::vector<das::TypeDeclPtr> Types;das::vector<das::string> Names;
                if(Field._abstract){
                    if(Field.init || !Field.type || Field.type->baseType!=das::Type::tFunction)DSLFail(TEXT("Invalid legacy abstract interface signature"));Return=Field.type->firstType;M.bConst=!Field.type->argTypes.empty() && Field.type->argTypes[0]->constant;
                    for(size_t I=1;I<Field.type->argTypes.size();++I){Types.push_back(Field.type->argTypes[I]);Names.push_back(Field.type->argNames[I]);}
                }else{
                    St->module->functions.foreach([&](const das::FunctionPtr& Candidate){if(Candidate->classParent==St && Candidate->name==St->name+"`"+Field.name)Function=Candidate;});
                    if(!Function || Function->isStaticClassMethod || Function->arguments.empty())DSLFail(TEXT("Interface requires ordinary instance method stubs"));
                    int32 AnnotationCount=0;for(const auto* A:Function->annotations)if(A->annotation->name=="ue_function")++AnnotationCount;
                    if(AnnotationCount!=1)DSLFail(TEXT("Interface stub requires exactly one ue_function annotation"));auto* Annotation=const_cast<das::AnnotationDeclaration*>(DSLAnnotation(Function->annotations,"ue_function"));DSLCommentTooltip(Function->at,Annotation->arguments,&Function->annotations);DSLAnnotationArguments(Annotation->arguments,{TEXT("out"),TEXT("ref")});M.bCallable=false;
                    for(const auto& A:Annotation->arguments){
                        if(A.name=="id"){M.Id=DSLString(Annotation->arguments,"id");continue;}
                        if(A.name=="BlueprintCallable"){M.bCallable=DSLBool(A);continue;}
                        if(A.name=="BlueprintPure"){M.bPure=DSLBool(A);continue;}
                        if(A.name=="out" || A.name=="ref"){DSLAddDirections(A,Directions);continue;}
                        const FString ConstraintKey=DSLText(A.name);const bool ArgumentConstraint=ConstraintKey.StartsWith(TEXT("arg_")) && (ConstraintKey.EndsWith(TEXT("_class")) || ConstraintKey.EndsWith(TEXT("_enum")) || ConstraintKey.EndsWith(TEXT("_struct")));if(ConstraintKey==TEXT("return_class") || ConstraintKey==TEXT("return_enum") || ConstraintKey==TEXT("return_struct") || ArgumentConstraint){if(A.type!=das::Type::tString)DSLFail(TEXT("Interface type constraint requires a type identity"));Constraints.Add(ConstraintKey,DSLText(A.sValue));continue;}
                        if(DSLIsMetadata(A.name))DSLMetadata(A,M.Metadata);else DSLFail(TEXT("Unsupported interface ue_function specifier: ")+DSLText(A.name));
                    }
                    if(M.bPure){const auto* Callable=Annotation->arguments.find("BlueprintCallable",das::Type::tBool);if(Callable && !DSLBool(*Callable))DSLFail(TEXT("BlueprintPure conflicts with BlueprintCallable=false"));M.bCallable=true;}
                    if(!M.bCallable)DSLFail(TEXT("Interface stub requires BlueprintCallable or BlueprintPure"));
                    das::ExpressionPtr Body=Function->body;
                    if(Body && Body->rtti_isBlock()){auto* Outer=static_cast<das::ExprBlock*>(Body);if(Outer->generated && Outer->list.size()==1 && Outer->finalList.empty() && Outer->list[0]->rtti_isWith()){auto* With=static_cast<das::ExprWith*>(Outer->list[0]);if(With->generated)Body=With->body;}}
                    if(!Body || !Body->rtti_isBlock())DSLFail(TEXT("Interface method body must be an empty declaration stub"));auto* Stub=static_cast<das::ExprBlock*>(Body);if(!Stub->list.empty() || !Stub->finalList.empty() || !Stub->arguments.empty())DSLFail(TEXT("Interface method body must be an empty declaration stub"));
                    M.bConst=Function->isConstClassMethod;Return=Function->result;for(size_t I=1;I<Function->arguments.size();++I){if(Function->arguments[I]->init)DSLFail(TEXT("Interface parameter defaults are unsupported"));Types.push_back(Function->arguments[I]->type);Names.push_back(Function->arguments[I]->name);}
                }
                Return=DSLObjectNormalize(Return,Group,St->module);if(Function)Function->result=Return;M.ReturnType=Type(Return,Constraints,TEXT("return_"));if(!Return || Return->ref)DSLFail(TEXT("Interface return references unsupported"));
                for(size_t I=0;I<Types.size();++I){Types[I]=DSLObjectNormalize(Types[I],Group,St->module);const auto& T=Types[I];if(Function)Function->arguments[I+1]->type=T;FDasDeclaredParameter P;P.Name=DSLText(Names[I]);P.Id=M.Id+TEXT(".")+P.Name;P.Type=Type(T,Constraints,TEXT("arg_")+P.Name+TEXT("_"));if(!T || P.Type.Kind==TEXT("void") || (T->ref && T->constant))DSLFail(TEXT("Interface parameters require value or var mutable reference types"));const FString Key=M.Name+TEXT(".")+P.Name;const bool LegacyOut=LegacyOuts.Contains(Key);const FString Direction=Directions.FindRef(P.Name);
                    if(LegacyOut && !Direction.IsEmpty())DSLFail(TEXT("Do not mix class-level and method-level directions for one parameter"));if(Field._abstract){P.Direction=LegacyOut?TEXT("out"):T->ref?TEXT("inout"):TEXT("in");if(LegacyOut && !T->ref)DSLFail(TEXT("Interface out requires mutable reference"));}else{P.Direction=DSLParameterDirection(Function->arguments[I+1],LegacyOut?TEXT("out"):Direction);}

                    Directions.Remove(P.Name);LegacyOuts.Remove(Key);M.Parameters.Add(P);
                }
                if(!Constraints.IsEmpty())DSLFail(TEXT("Unused interface type constraint"));                if(!Directions.IsEmpty())DSLFail(TEXT("Unknown interface out/ref parameter name"));if(M.Parameters.Num()>16)DSLFail(TEXT("Interface parameter count exceeds 16"));bool Output=M.ReturnType.Kind!=TEXT("void");for(const auto& P:M.Parameters)Output |= P.Direction!=TEXT("in");C.Methods.Add(M);
                if(Function){
                    // Empty source bodies are declaration markers. Compiled direct calls trap.
                    auto* Trap=new das::ExprBlock();Trap->at=Function->at;Trap->generated=true;Trap->list.push_back(DSLCall(Function->at,"panic",{new das::ExprConstString(Function->at,"ue_interface declaration stub cannot be called directly")}));
                    if(M.ReturnType.Kind!=TEXT("void")){auto* Default=new das::ExprMakeStruct(Function->at);Default->makeType=DSLCopyType(Return);Default->alwaysUseInitializer=true;das::ExpressionPtr Placeholder=Default;if(DSLDirectStruct(Return))Placeholder=DSLCall(Function->at,Return->structType->name,{});if(DSLDirectEnum(Return)){if(Return->enumType->list.empty())DSLFail(TEXT("Empty interface enum declaration"));Placeholder=new das::ExprField(Function->at,new das::ExprVar(Function->at,Return->enumType->name),Return->enumType->list.front().name);}auto* Result=new das::ExprReturn(Function->at,Placeholder);Result->moveSemantics=DSLContainer(Return);Trap->list.push_back(Result);}Function->body=Trap;
                }
            }
            if(C.Methods.IsEmpty() || C.Methods.Num()>64 || !LegacyOuts.IsEmpty())DSLFail(TEXT("Expected 1..64 interface methods and known legacy out names"));Model.Classes.Add(C);FString Wire,Issue;if(!Model.ToJson(Wire,Issue))DSLFail(Issue);Args.push_back(das::AnnotationArgument(DSLInterfaceKey,DSLUtf8(Wire)));return true;
        }catch(const std::exception& X){Error=X.what();return false;}}
    };
    const char* DSLTypesKey="_ue_types_json";
    void DSLTypeArgs(const das::AnnotationArgumentList& Args,TMap<FString,FString>& Meta){DSLAnnotationArguments(Args);for(const auto& A:Args){if(A.name=="module_id" || A.name=="id")continue;if(A.name=="BlueprintType"){if(!DSLBool(A))DSLFail(TEXT("BlueprintType must be enabled"));continue;}if(DSLIsMetadata(A.name))DSLMetadata(A,Meta);else DSLFail(TEXT("Unknown reflected type annotation"));}}

    struct FUENativeStructAnnotation:das::StructureAnnotation {
     FUENativeStructAnnotation():StructureAnnotation("ue_native_struct"){}
     virtual bool touch(const das::StructurePtr&,das::ModuleGroup&,das::AnnotationArgumentList& Args,das::string& Error)override{try{DSLAnnotationArguments(Args);if(Args.size()!=1 || Args[0].name!="path" || Args[0].type!=das::Type::tString)DSLFail(TEXT("ue_native_struct requires one native path"));return true;}catch(const std::exception& X){Error=X.what();return false;}}
     virtual bool look(const das::StructurePtr& S,das::ModuleGroup&,das::AnnotationArgumentList&,das::string& Error)override{try{DSLDeclarationKind(S->annotations);auto* T=new das::TypeDecl(das::Type::tStructure,S->at);das::TypeDeclPtr Type=T;T->structType=S;DSLValidateNativeStruct(Type);return true;}catch(const std::exception& X){Error=X.what();return false;}}
    };
    struct FUENativeEnumAnnotation:das::EnumerationAnnotation {
        FUENativeEnumAnnotation():EnumerationAnnotation("ue_native_enum"){}
        virtual bool touch(const das::EnumerationPtr& E,das::ModuleGroup& Group,das::AnnotationArgumentList& Args,das::string& Error)override{try{
            DSLAnnotationArguments(Args);if(Args.size()!=1 || Args[0].name!="path" || Args[0].type!=das::Type::tString || DSLAnnotation(E->annotations,"ue_enum"))DSLFail(TEXT("ue_native_enum requires a single native path and no ue_enum"));
            const FString Path=DSLString(Args,"path");FString Issue;auto* Native=Path.StartsWith(TEXT("/Script/"))?ResolveDasEnum(Path,Issue):nullptr;if(!Native)DSLFail(TEXT("Expected loaded native enum path"));TSet<int64> Values;
            for(const auto& V:E->list){if(!V.value || V.value->__rtti!=das::string("ExprConstInt"))DSLFail(TEXT("Native enum declarations require explicit integer values"));const int64 Number=static_cast<das::ExprConstInt*>(V.value)->getValue();const int64 Actual=Native->GetValueByNameString(DSLText(V.name));const int32 Index=Native->GetIndexByValue(Actual);if(Number<0 || Number>254 || Index==INDEX_NONE || Actual!=Number || !Native->GetNameStringByIndex(Index).Equals(DSLText(V.name),ESearchCase::CaseSensitive) || Values.Contains(Number) || Native->GetNameStringByIndex(Index).EndsWith(TEXT("_MAX")))DSLFail(TEXT("Native enum member name/value differs from Unreal"));Values.Add(Number);}
            for(int32 I=0;I<Native->NumEnums();++I){const int64 Number=Native->GetValueByIndex(I);if(Number>=0 && Number<=254 && !Native->GetNameStringByIndex(I).EndsWith(TEXT("_MAX")) && !Values.Contains(Number))DSLFail(TEXT("Native enum declaration omits a native value"));}
            auto* F=new das::Function();F->at=E->at;F->name="__ue_pack_native_enum_"+E->name;F->generated=true;F->privateFunction=false;F->result=DSLHandleType(Group,E->module,"UEEnumValue");auto* Arg=new das::Variable();Arg->at=E->at;Arg->name="value";Arg->type=new das::TypeDecl(das::Type::tEnumeration,E->at);Arg->type->enumType=E;Arg->type->constant=true;F->arguments.push_back(Arg);auto* Body=new das::ExprBlock();Body->at=E->at;Body->list.push_back(new das::ExprReturn(E->at,DSLCall(E->at,"ue_enum_from_number",{new das::ExprConstString(E->at,DSLUtf8(Path)),DSLCall(E->at,"int",{new das::ExprVar(E->at,"value")})})));F->body=Body;if(!E->module->addFunction(F,true))DSLFail(TEXT("Native enum pack helper collision"));return true;
        }catch(const std::exception& X){Error=X.what();return false;}}
    };
    struct FUEEnumAnnotation:das::EnumerationAnnotation {
        FUEEnumAnnotation():EnumerationAnnotation("ue_enum"){}
        virtual bool touch(const das::EnumerationPtr& E,das::ModuleGroup&,das::AnnotationArgumentList& Args,das::string& Error)override{try{
            if(const auto* Wire=Args.find(DSLTypesKey,das::Type::tString)){if(Wire->at.fileInfo)DSLFail(TEXT("Reserved type annotation argument"));return true;}
            DSLCommentTooltip(E->at,Args);
            FDasReflectedTypes M;M.Id=DSLString(Args,"module_id",FPaths::GetBaseFilename(DSLText(E->at.fileInfo->name)));FDasEnumDeclaration D;D.Name=DSLText(E->name);D.Id=DSLString(Args,"id",M.Id+TEXT(".")+D.Name);das::AnnotationArgumentList TypeArgs;for(const auto& A:Args)if(A.name.find("member_id_")!=0)TypeArgs.push_back(A);DSLTypeArgs(TypeArgs,D.Metadata);for(const auto& A:Args)if(A.name.find("member_id_")==0){bool Found=false;for(const auto& V:E->list)Found |= A.name=="member_id_"+V.name;if(!Found || A.type!=das::Type::tString)DSLFail(TEXT("Unknown member or invalid member ID annotation"));}
            for(const auto& V:E->list){if(!V.value || V.value->__rtti!=das::string("ExprConstInt"))DSLFail(TEXT("ue_enum requires explicit int literal values"));D.Entries.Add({DSLText(V.name),static_cast<das::ExprConstInt*>(V.value)->getValue(),DSLString(Args,("member_id_"+V.name).c_str(),D.Id+TEXT(".")+DSLText(V.name))});}M.Enums.Add(D);FString Wire,Issue;if(!M.ToJson(Wire,Issue))DSLFail(Issue);Args.push_back(das::AnnotationArgument(DSLTypesKey,DSLUtf8(Wire)));return true;
        }catch(const std::exception& X){Error=X.what();return false;}}
    };
    struct FUEStructAnnotation:das::StructureAnnotation {
        FUEStructAnnotation():StructureAnnotation("ue_struct"){}
        virtual bool look(const das::StructurePtr& St,das::ModuleGroup&,das::AnnotationArgumentList&,das::string& Error)override{try{DSLDeclarationKind(St->annotations);DSLAnnotation(St->annotations,"ue_struct");return true;}catch(const std::exception& X){Error=X.what();return false;}}
        virtual bool touch(const das::StructurePtr& S,das::ModuleGroup&,das::AnnotationArgumentList& Args,das::string& Error)override{try{
            DSLCommentTooltip(S->at,Args);
            DSLExpandDeclarationGroups(S,false);
            for(auto& Field:S->fields)if(!Field.generated)DSLCommentTooltip(Field.at,Field.annotation);
            if(const auto* Wire=Args.find(DSLTypesKey,das::Type::tString)){if(Wire->at.fileInfo)DSLFail(TEXT("Reserved type annotation argument"));return true;}
            if(S->isClass || S->parent)DSLFail(TEXT("ue_struct requires plain struct without inheritance"));FDasReflectedTypes M;M.Id=DSLString(Args,"module_id",FPaths::GetBaseFilename(DSLText(S->at.fileInfo->name)));FDasDeclaredClass D;D.Name=DSLText(S->name);D.Id=DSLString(Args,"id",M.Id+TEXT(".")+D.Name);DSLTypeArgs(Args,D.Metadata);
            for(auto& F:S->fields){if(F.generated)continue;if(DSLNativeStruct(F.type) || (F.type && (DSLNativeStruct(F.type->firstType) || DSLNativeStruct(F.type->secondType))))DSLFail(TEXT("Native structs inside authored struct declarations require a separate adapter"));if(DSLNativeEnum(F.type) || (F.type && (DSLNativeEnum(F.type->firstType) || DSLNativeEnum(F.type->secondType))))DSLFail(TEXT("Native enum fields inside authored structs require a separate adapter"));if(F.classMethod)DSLFail(TEXT("Reflected struct methods require separate support"));FDasDeclaredProperty P;P.Name=DSLText(F.name);P.Id=D.Id+TEXT(".")+P.Name;P.Type.Kind=DSLTypeName(F.type);if(DSLGameplay(F.type)){if(F.init)DSLFail(TEXT("GameplayTag struct fields use implicit empty defaults"));P.Type.TypePath=DSLGameplayPath(F.type);}if(DSLSoft(F.type)){if(F.init)DSLFail(TEXT("Authored struct soft fields require implicit null defaults"));P.Type.TypePath=DSLSoftPath(F.type);}if(DSLDirectEnum(F.type)){const auto* A=DSLAnnotation(F.type->enumType->annotations,"ue_enum");const FString Module=DSLString(A->arguments,"module_id",FPaths::GetBaseFilename(DSLText(F.type->enumType->at.fileInfo->name)));const FString Id=DSLString(A->arguments,"id",Module+TEXT(".")+DSLText(F.type->enumType->name));P.Type.TypePath=TEXT("/DasTypes/")+FMD5::HashAnsiString(*Id)+TEXT(".Type");P.Default.Kind=TEXT("string");P.Default.String=DSLStructEnumDefault(F);}if(DSLAuthoredView(F.type))DSLFail(TEXT("Authored reference struct fields require a separate ownership adapter"));if(P.Type.Kind==TEXT("object")){if(!DSLTypedReference(F.type) || F.init)DSLFail(TEXT("Struct object fields require typed native wrappers and implicit null defaults"));P.Type.TypePath=DSLTypedReferencePath(F.type);}if(DSLSet(F.type)){const FString K=DSLTypeName(F.type->firstType);if(F.init || (K!=TEXT("byte") && K!=TEXT("int") && K!=TEXT("string")))DSLFail(TEXT("Authored sets require int/string elements and empty defaults"));P.Type.Kind=TEXT("set");auto E=MakeShared<FDasDeclaredType>();E->Kind=K;P.Type.Element=E;}else if(F.type && F.type->baseType==das::Type::tTable){const FString Key=DSLTypeName(F.type->firstType),Value=DSLTypeName(F.type->secondType);if(F.init || (Key!=TEXT("byte") && Key!=TEXT("int") && Key!=TEXT("string")) || (Value!=TEXT("byte") && Value!=TEXT("text") && Value!=TEXT("int") && Value!=TEXT("string") && Value!=TEXT("float") && Value!=TEXT("double") && Value!=TEXT("bool") && Value!=TEXT("name") && Value!=TEXT("struct") && Value!=TEXT("enum") && Value!=TEXT("object")))DSLFail(TEXT("Authored maps require int/string keys, scalar/name/struct/enum/reference values and empty defaults"));P.Type.Kind=TEXT("map");auto K=MakeShared<FDasDeclaredType>();K->Kind=Key;auto V=MakeShared<FDasDeclaredType>();V->Kind=Value;if(Value==TEXT("enum")){if(!DSLDirectEnum(F.type->secondType))DSLFail(TEXT("Enum maps require authored ue_enum values"));const auto* A=DSLAnnotation(F.type->secondType->enumType->annotations,"ue_enum");const FString Module=DSLString(A->arguments,"module_id",FPaths::GetBaseFilename(DSLText(F.type->secondType->enumType->at.fileInfo->name)));const FString Id=DSLString(A->arguments,"id",Module+TEXT(".")+DSLText(F.type->secondType->enumType->name));V->TypePath=TEXT("/DasTypes/")+FMD5::HashAnsiString(*Id)+TEXT(".Type");}if(Value==TEXT("object")){if(!DSLTypedReference(F.type->secondType) || DSLAuthoredView(F.type->secondType))DSLFail(TEXT("Struct reference map fields require native typed wrappers"));V->TypePath=DSLTypedReferencePath(F.type->secondType);if(!V->TypePath.StartsWith(TEXT("/Script/")))DSLFail(TEXT("Struct reference maps require native object/interface types"));}if(Value==TEXT("struct")){if(DSLSoft(F.type->secondType))V->TypePath=DSLSoftPath(F.type->secondType);else if(!DSLMathPath(F.type->secondType).IsEmpty())V->TypePath=DSLMathPath(F.type->secondType);else {const auto* A=F.type->secondType->structType?DSLAnnotation(F.type->secondType->structType->annotations,"ue_struct"):nullptr;if(!A)DSLFail(TEXT("Map values require authored ue_struct or supported math types"));const FString Module=DSLString(A->arguments,"module_id",FPaths::GetBaseFilename(DSLText(F.type->secondType->structType->at.fileInfo->name)));const FString Id=DSLString(A->arguments,"id",Module+TEXT(".")+DSLText(F.type->secondType->structType->name));V->TypePath=TEXT("/DasTypes/")+FMD5::HashAnsiString(*Id)+TEXT(".Type");}}P.Type.Key=K;P.Type.Value=V;}if(F.type && F.type->baseType==das::Type::tArray){const FString K=DSLTypeName(F.type->firstType);if((!DSLDirectStruct(F.type->firstType) && !DSLDirectEnum(F.type->firstType) && !DSLTypedReference(F.type->firstType) && DSLMathPath(F.type->firstType).IsEmpty() && K!=TEXT("byte") && K!=TEXT("text") && K!=TEXT("int") && K!=TEXT("float") && K!=TEXT("double") && K!=TEXT("bool") && K!=TEXT("string") && K!=TEXT("name")) || F.init)DSLFail(TEXT("Authored arrays require supported scalar/ue_struct elements and empty defaults"));auto E=MakeShared<FDasDeclaredType>();E->Kind=K;if(DSLTypedReference(F.type->firstType)){if(DSLAuthoredView(F.type->firstType))DSLFail(TEXT("Authored object array fields require a separate ownership/identity contract"));E->TypePath=DSLTypedReferencePath(F.type->firstType);if(!E->TypePath.StartsWith(TEXT("/Script/")))DSLFail(TEXT("Reference array fields require native object/interface types"));}else if(DSLDirectEnum(F.type->firstType)){const auto* A=DSLAnnotation(F.type->firstType->enumType->annotations,"ue_enum");const FString Module=DSLString(A->arguments,"module_id",FPaths::GetBaseFilename(DSLText(F.type->firstType->enumType->at.fileInfo->name)));const FString Id=DSLString(A->arguments,"id",Module+TEXT(".")+DSLText(F.type->firstType->enumType->name));E->TypePath=TEXT("/DasTypes/")+FMD5::HashAnsiString(*Id)+TEXT(".Type");}else if(DSLGameplay(F.type->firstType))E->TypePath=DSLGameplayPath(F.type->firstType);else if(DSLSoft(F.type->firstType))E->TypePath=DSLSoftPath(F.type->firstType);else if(!DSLMathPath(F.type->firstType).IsEmpty())E->TypePath=DSLMathPath(F.type->firstType);else if(DSLDirectStruct(F.type->firstType)){const auto* A=DSLAnnotation(F.type->firstType->structType->annotations,"ue_struct");const FString Module=DSLString(A->arguments,"module_id",FPaths::GetBaseFilename(DSLText(F.type->firstType->structType->at.fileInfo->name)));const FString Id=DSLString(A->arguments,"id",Module+TEXT(".")+DSLText(F.type->firstType->structType->name));E->TypePath=TEXT("/DasTypes/")+FMD5::HashAnsiString(*Id)+TEXT(".Type");}P.Type.Kind=TEXT("array");P.Type.Element=E;}if(DSLDirectStruct(F.type) && !DSLSoft(F.type) && !DSLGameplay(F.type)){const auto* Child=DSLAnnotation(F.type->structType->annotations,"ue_struct");const FString Module=DSLString(Child->arguments,"module_id",FPaths::GetBaseFilename(DSLText(F.type->structType->at.fileInfo->name)));const FString Id=DSLString(Child->arguments,"id",Module+TEXT(".")+DSLText(F.type->structType->name));P.Type.TypePath=TEXT("/DasTypes/")+FMD5::HashAnsiString(*Id)+TEXT(".Type");if(F.init){bool Empty=false;if(F.init->rtti_isCall()){auto* Init=static_cast<das::ExprCall*>(F.init);Empty=Init->name==F.type->structType->name && Init->arguments.empty();}else if(F.init->rtti_isMakeStruct()){auto* Init=static_cast<das::ExprMakeStruct*>(F.init);Empty=Init->structs.empty() && !Init->block;}if(!Empty)DSLFail(TEXT("Nested ue_struct fields require an empty child constructor"));}}if(!DSLMathPath(F.type).IsEmpty()){P.Type.TypePath=DSLMathPath(F.type);if(!F.init && P.Type.TypePath==TEXT("/Script/CoreUObject.Transform"))F.init=DSLCall(F.at,"ue_transform_identity",{});P.Default.Kind=TEXT("string");P.Default.String=DSLMathPropertyDefault(F);}bool Marked=false;for(const auto& A:F.annotation){if(A.name=="ue_property"){Marked=true;if(A.type==das::Type::tString && A.sValue=="BlueprintReadOnly")P.bReadOnly=true;else if(A.type==das::Type::tString && (A.sValue=="EditAnywhere" || A.sValue=="BlueprintReadWrite")){}else if(A.type!=das::Type::tBool || !A.bValue)DSLFail(TEXT("Unsupported struct property annotation"));}else if(A.name=="ue_id")P.Id=DSLText(A.sValue);else if(DSLIsMetadata(A.name))DSLMetadata(A,P.Metadata);else DSLFail(TEXT("Unknown struct field annotation"));}if(!Marked || !F.type || F.type->ref)DSLFail(TEXT("Struct fields require ue_property and by-value scalar type"));
                if(P.Type.Kind==TEXT("text"))P.Default=DSLTextDefault(F.init);else if(P.Type.Kind==TEXT("byte"))P.Default=DSLByteDefault(F.init);else if(F.init && P.Type.Kind!=TEXT("struct") && P.Type.Kind!=TEXT("enum")){if(F.init->__rtti==das::string("ExprConstInt")){P.Default.Kind=TEXT("number");P.Default.Number=static_cast<das::ExprConstInt*>(F.init)->getValue();}else if(F.init->__rtti==das::string("ExprConstFloat")){P.Default.Kind=TEXT("number");P.Default.Number=static_cast<das::ExprConstFloat*>(F.init)->getValue();}else if(F.init->__rtti==das::string("ExprConstDouble")){P.Default.Kind=TEXT("number");P.Default.Number=static_cast<das::ExprConstDouble*>(F.init)->getValue();}else if(F.init->__rtti==das::string("ExprConstBool")){P.Default.Kind=TEXT("bool");P.Default.Boolean=static_cast<das::ExprConstBool*>(F.init)->getValue();}else if(F.init->__rtti==das::string("ExprConstString")){P.Default.Kind=TEXT("string");P.Default.String=DSLText(static_cast<das::ExprConstString*>(F.init)->text);}else DSLFail(TEXT("Struct defaults require scalar literals"));}D.Properties.Add(P);
            }M.Structs.Add(D);FString Wire,Issue;if(!M.ToJson(Wire,Issue,true))DSLFail(Issue);Args.push_back(das::AnnotationArgument(DSLTypesKey,DSLUtf8(Wire)));return true;
        }catch(const std::exception& X){Error=X.what();return false;}}
    };

    #include "DasStaticLibraryDSL.inl"

    bool DSLValidateOwner(const das::ProgramPtr& Program,const FString& Entry,UObject* Self,FString& Wire,FString& Error)
    {
        if(LibraryInvocationContract && Self){
            bool Match=false;Program->getThisModule()->structures.foreach([&](const das::StructurePtr& St){if(const auto* A=DSLAnnotation(St->annotations,"ue_library"))if(const auto* J=A->arguments.find(DSLModelKey,das::Type::tString)){FDasAuthoringModule M;FString Issue;if(FDasAuthoringModule::FromJson(DSLText(J->sValue),M,Issue) && DSLText(J->sValue)==*LibraryInvocationContract && M.Classes.ContainsByPredicate([&](const auto& C){return C.Methods.ContainsByPredicate([&](const auto& Method){return Method.EntryPoint==Entry;});})){Wire=DSLText(J->sValue);Match=true;}}});
            if(!Match)Error=TEXT("Static library source contract differs from saved Blueprint; reimport library");return Match;
        }
        bool OK=true;
        Program->getThisModule()->structures.foreach([&](const das::StructurePtr& St){
            const auto* A=DSLAnnotation(St->annotations,"ue_class");if(!A || !OK) return;
            const auto* J=A->arguments.find(DSLModelKey,das::Type::tString);if(!J) return;
            FDasAuthoringModule Model;if(!FDasAuthoringModule::FromJson(DSLText(J->sValue),Model,Error)){OK=false;return;}
            for(const auto& C:Model.Classes) if(C.Methods.ContainsByPredicate([&](const auto& M){return M.EntryPoint==Entry;}) || ((!C.Callbacks.IsEmpty() || C.AsyncPhysics.IsSet()) && (Entry==TEXT("main") || Entry==TEXT("begin_play") || Entry==TEXT("tick") || Entry==TEXT("end_play") || C.Callbacks.ContainsByPredicate([&](const auto& Callback){return Callback.Kind==TEXT("event") && Entry==TEXT("ue_")+C.Name+TEXT("_")+Callback.Name;}))))
            {
                Wire=DSLText(J->sValue);
                if(!IsValid(Self)){Error=TEXT("DSL entry requires live self");OK=false;return;}
                const bool ActorAdapter=Self->IsA<ADasProbeActor>() || Self->IsA<UDasScriptComponent>() || Self->IsA<UDasSaveGame>() || Self->IsA<UDasDataAsset>() || Self->IsA<UDasPrimaryDataAsset>();
                FDasDeclaredClass EffectiveDefaults;if(!Model.ResolveClass(C.Id,EffectiveDefaults,Error)){OK=false;return;}
                if(!DasCheckNativeDefaultOwner(Model,C.Id,Self,Error)){OK=false;return;}
                for(const auto& P:EffectiveDefaults.Properties)
                {
                    FProperty* Actual=Self->GetClass()->FindPropertyByName(*P.Name);
                    auto* DeclaringCDO=Actual && Actual->GetOwnerClass()?Actual->GetOwnerClass()->GetDefaultObject():nullptr;
                    for(const auto* Owner=&C;Owner;){
                        if(Owner->PropertyOverrides.ContainsByPredicate([&](const auto& O){return O.Id==P.Id;})){
                            DeclaringCDO=nullptr;for(auto* Class=Self->GetClass();Class;Class=Class->GetSuperClass()){auto* Candidate=Class->GetDefaultObject();const FString Identity=DasHostIdentity(Candidate);if(Identity==Owner->Id)DeclaringCDO=Candidate;}
                            if(!DeclaringCDO){Error=TEXT("Missing inherited default owner: ")+Owner->Id;OK=false;return;}break;
                        }
                        Owner=Owner->ParentId.IsEmpty()?nullptr:Model.Classes.FindByPredicate([&](const auto& V){return V.Id==Owner->ParentId;});
                    }
                    const auto* Contracts=DasHostContracts(DeclaringCDO);
                    const bool CheckCDODefault=ActorAdapter && (!Contracts || Contracts->FindRef(P.Id)!=DasDefaultSourceContract(P));
                    if(auto* Carrier=DasContainerCarrier(P.Type)){FInstancedStruct Expected;if(!ParseDasStructDefault(Carrier->GetPathName(),P.Default.String,Expected,Error)){OK=false;return;}auto* Values=Carrier->FindPropertyByName(TEXT("Values"));if(!Actual || !Values || !Values->SameType(Actual) || P.bReadOnly!=Actual->HasAnyPropertyFlags(CPF_BlueprintReadOnly) || (CheckCDODefault && !MatchesDasPropertyDefault(Actual,Actual->ContainerPtrToValuePtr<void>(DeclaringCDO),Values->ContainerPtrToValuePtr<void>(Expected.GetMemory())))){Error=TEXT("DSL container identity/default differs from owner: ")+P.Name;OK=false;return;}continue;}
                    if(P.Type.Kind==TEXT("delegate")){auto* D=CastField<FDelegateProperty>(Actual);FDasAuthoringModule Definition;FString Issue;if(!FDasAuthoringModule::FromJson(P.DelegateWire,Definition,Issue)){Error=Issue;OK=false;return;}TStrongObjectPtr<UFunction> Sig(MakeDasDelegateSignature(Definition.Classes[0].Methods[0],false,GetTransientPackage(),NAME_None,Issue));if(!D || !Sig || !Sig->IsSignatureCompatibleWith(D->SignatureFunction) || P.bReadOnly!=D->HasAnyPropertyFlags(CPF_BlueprintReadOnly)){Error=TEXT("Single delegate field differs from owner: ")+P.Name;OK=false;return;}continue;}
                    if(P.Type.Kind==TEXT("event")){auto* Event=CastField<FMulticastDelegateProperty>(Actual);FDasAuthoringModule Definition;FString Issue;if(!FDasAuthoringModule::FromJson(P.DelegateWire,Definition,Issue)){Error=Issue;OK=false;return;}TStrongObjectPtr<UFunction> Signature(MakeDasDelegateSignature(Definition.Classes[0].Methods[0],true,GetTransientPackage(),NAME_None,Issue));if(!Event || !Signature || !Signature->IsSignatureCompatibleWith(Event->SignatureFunction) || !Event->HasAllPropertyFlags(CPF_BlueprintAssignable|CPF_BlueprintCallable)){Error=TEXT("Authored event differs from owner; reimport Blueprint: ")+P.Name;auto Summary=[](UFunction* F){FString S;if(F)for(TFieldIterator<FProperty> I(F);I;++I)if(I->HasAnyPropertyFlags(CPF_Parm)){FDasDeclaredType T;FString E;DescribeDasProperty(*I,T,E);S+=T.StorageIdentity()+FString::Printf(TEXT(" offset=%d size=%d flags=%llx;"),I->GetOffset_ForUFunction(),I->GetSize(),uint64(I->PropertyFlags));}return S;};Error+=TEXT(" expected ")+Summary(Signature.Get())+TEXT(" actual ")+Summary(Event?Event->SignatureFunction.Get():nullptr);OK=false;return;}continue;}
                    if(!DSLPropertyMatches(Actual,P.Type.Kind) || P.bReadOnly!=Actual->HasAnyPropertyFlags(CPF_BlueprintReadOnly))
                    {Error=TEXT("DSL property differs from owner; apply/reimport Blueprint: ")+P.Name;OK=false;return;}
                    if(P.Type.Kind==TEXT("class")){auto* Class=CastField<FClassProperty>(Actual);if(!Class || !Class->MetaClass || Class->MetaClass->GetPathName()!=P.Type.TypePath || (CheckCDODefault && !P.Default.String.IsEmpty() && (Class->GetObjectPropertyValue_InContainer(DeclaringCDO)?Class->GetObjectPropertyValue_InContainer(DeclaringCDO)->GetPathName():FString())!=P.Default.String)){Error=TEXT("DSL class base differs from owner: ")+P.Name;OK=false;return;}continue;}
                    if(P.Type.Kind==TEXT("enum")){auto* Type=ResolveDasEnum(P.Type.TypePath,Error);if(!Type || DasEnumPropertyType(Actual)!=Type || (CheckCDODefault && DasEnumPropertyNumber(Actual,Actual->ContainerPtrToValuePtr<void>(DeclaringCDO))!=Type->GetValueByNameString(P.Default.String))){Error=TEXT("DSL enum identity/default differs from owner");OK=false;return;}continue;}
                    if(P.Type.Kind==TEXT("object")){auto* ReferenceClass=DasReferencePropertyClass(Actual);if(ReferenceClass->GetPathName()!=P.Type.TypePath || (CheckCDODefault && (DasReferenceGet(Actual,Actual->ContainerPtrToValuePtr<void>(DeclaringCDO))?DasReferenceGet(Actual,Actual->ContainerPtrToValuePtr<void>(DeclaringCDO))->GetPathName():FString())!=P.Default.String)){Error=TEXT("DSL object type/default differs from owner; apply/reimport Blueprint: ")+P.Name;OK=false;return;}continue;}
                    if(P.Type.Kind==TEXT("struct")){
                        auto* Struct=CastField<FStructProperty>(Actual);FInstancedStruct Default;
                        if(Struct->Struct->GetPathName()!=P.Type.TypePath || !ParseDasStructDefault(P.Type.TypePath,P.Default.String,Default,Error) || (CheckCDODefault && !MatchesDasStructDefault(Struct->Struct,Struct->ContainerPtrToValuePtr<void>(DeclaringCDO),Default.GetMemory()))){Error=TEXT("DSL struct type/default differs from owner; apply/reimport Blueprint: ")+P.Name;OK=false;return;}continue;
                    }
                    if(P.Type.Kind==TEXT("byte")){if(CheckCDODefault && CastFieldChecked<FByteProperty>(Actual)->GetPropertyValue_InContainer(DeclaringCDO)!=uint8(P.Default.Number)){Error=TEXT("DSL byte default differs from owner");OK=false;return;}continue;}
                    if(P.Type.Kind==TEXT("text")){FText Expected;if(!ParseDasTextDefault(P.Default.String,Expected,Error) || (CheckCDODefault && !DasTextDefaultsEqual(CastFieldChecked<FTextProperty>(Actual)->GetPropertyValue_InContainer(DeclaringCDO),Expected))){Error=TEXT("DSL text default differs from owner");OK=false;return;}continue;}
                    if(CheckCDODefault)
                    {
                        UObject* CDO=DeclaringCDO;
                        const bool DefaultMatches=P.Type.Kind==TEXT("int")
                            ? CastField<FIntProperty>(Actual)->GetPropertyValue_InContainer(CDO)==static_cast<int32>(P.Default.Number)
                            : P.Type.Kind==TEXT("float")?CastField<FFloatProperty>(Actual)->GetPropertyValue_InContainer(CDO)==static_cast<float>(P.Default.Number):P.Type.Kind==TEXT("bool")?CastField<FBoolProperty>(Actual)->GetPropertyValue_InContainer(CDO)==P.Default.Boolean:P.Type.Kind==TEXT("double")?CastField<FDoubleProperty>(Actual)->GetPropertyValue_InContainer(CDO)==P.Default.Number:P.Type.Kind==TEXT("string")?CastField<FStrProperty>(Actual)->GetPropertyValue_InContainer(CDO)==P.Default.String:CastField<FNameProperty>(Actual)->GetPropertyValue_InContainer(CDO)==FName(*P.Default.String);
                        if(!DefaultMatches){Error=TEXT("DSL default differs from owner; apply/reimport Blueprint: ")+P.Name;OK=false;return;}
                    }
                }
                if(ActorAdapter)for(const auto& I:C.Interfaces){auto* Interface=FindObject<UClass>(nullptr,*I);if(!Interface || !Self->GetClass()->ImplementsInterface(Interface)){Error=TEXT("DSL interface differs from owner; apply/reimport Blueprint");OK=false;return;}}
                if(ActorAdapter) for(const auto& M:C.Methods)
                {
                    UFunction* Method=Self->FindFunction(*M.Name);
                    auto Matches=[](FProperty* P,const FDasDeclaredType& T){if(T.Kind==TEXT("delegate")){FString Error;TStrongObjectPtr<UFunction> Sig(MakeDasSingleDelegateSignature(T.DelegateWire,Error));auto* D=CastField<FDelegateProperty>(P);return D && Sig && D->SignatureFunction && Sig->IsSignatureCompatibleWith(D->SignatureFunction);}if(auto* Carrier=DasContainerCarrier(T))return P && Carrier->FindPropertyByName(TEXT("Values"))->SameType(P);return DSLPropertyMatches(P,T.Kind);};
                    bool Fits=Method && Method->HasAnyFunctionFlags(FUNC_BlueprintPure)==M.bPure && Method->HasAnyFunctionFlags(FUNC_Const)==M.bConst && Method->NumParms==M.Parameters.Num()+(M.ReturnType.Kind==TEXT("void")?0:1);
                    bool Directed=false;for(const auto& P:M.Parameters)Directed |= P.Direction!=TEXT("in");if(Fits && !Directed){int32 Index=0;for(TFieldIterator<FProperty> It(Method);It;++It) if(It->HasAnyPropertyFlags(CPF_Parm) && !It->HasAnyPropertyFlags(CPF_ReturnParm)){Fits &= M.Parameters.IsValidIndex(Index) && It->GetFName()==FName(*M.Parameters[Index].Name);++Index;}Fits &= Index==M.Parameters.Num();}
                    if(Fits) for(const auto& P:M.Parameters){auto* Actual=Method->FindPropertyByName(*P.Name);Fits &= Matches(Actual,P.Type) && (P.Direction==TEXT("out")?Actual->HasAnyPropertyFlags(CPF_OutParm) && !Actual->HasAnyPropertyFlags(CPF_ReferenceParm):P.Direction==TEXT("inout")?Actual->HasAllPropertyFlags(CPF_OutParm|CPF_ReferenceParm):(Actual->HasAnyPropertyFlags(CPF_ConstParm) || P.Type.Kind==TEXT("array") || !Actual->HasAnyPropertyFlags(CPF_OutParm|CPF_ReferenceParm)));if(Fits && P.Type.Kind==TEXT("enum"))Fits=DasEnumPropertyType(Actual)->GetPathName()==P.Type.TypePath;if(Fits && P.Type.Kind==TEXT("struct") && !DasContainerCarrier(P.Type)){auto* NativeStruct=CastField<FStructProperty>(Actual);Fits=NativeStruct && NativeStruct->Struct->GetPathName()==P.Type.TypePath;}if(Fits && P.Type.Kind==TEXT("class"))Fits=CastFieldChecked<FClassProperty>(Actual)->MetaClass->GetPathName()==P.Type.TypePath;if(Fits && P.Type.Kind==TEXT("object"))Fits=DasReferencePropertyClass(Actual)->GetPathName()==P.Type.TypePath;}
                    if(Fits) Fits=M.ReturnType.Kind==TEXT("void")?!Method->GetReturnProperty():Matches(Method->GetReturnProperty(),M.ReturnType);
                    if(Fits && M.ReturnType.Kind==TEXT("enum"))Fits=DasEnumPropertyType(Method->GetReturnProperty())->GetPathName()==M.ReturnType.TypePath;if(Fits && M.ReturnType.Kind==TEXT("struct") && !DasContainerCarrier(M.ReturnType)){auto* NativeStruct=CastField<FStructProperty>(Method->GetReturnProperty());Fits=NativeStruct && NativeStruct->Struct->GetPathName()==M.ReturnType.TypePath;}
                    if(Fits && M.ReturnType.Kind==TEXT("class"))Fits=CastFieldChecked<FClassProperty>(Method->GetReturnProperty())->MetaClass->GetPathName()==M.ReturnType.TypePath;if(Fits && M.ReturnType.Kind==TEXT("object"))Fits=DasReferencePropertyClass(Method->GetReturnProperty())->GetPathName()==M.ReturnType.TypePath;
                    if(!Fits)
                    {Error=TEXT("DSL method differs from owner; apply/reimport Blueprint: ")+M.Name;OK=false;return;}
                }
            }
        });return OK;
    }
