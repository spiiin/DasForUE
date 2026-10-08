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
    bool DSLIsMetadata(const das::string& N) {return N=="Category" || N=="DisplayName" || N=="ToolTip";}
    void DSLMetadata(const das::AnnotationArgument& A,TMap<FString,FString>& Out)
    {if(A.type!=das::Type::tString) DSLFail(TEXT("Metadata must be string: ")+DSLText(A.name));Out.Add(DSLText(A.name),DSLText(A.sValue));}
    const das::AnnotationDeclaration* DSLAnnotation(const das::AnnotationList& L,const char* N)
    {const das::AnnotationDeclaration* Found=nullptr;for(auto* A:L)if(A->annotation->name==N){if(Found)DSLFail(TEXT("Repeated singleton annotation: ")+DSLText(N));Found=A;}return Found;}
    void DSLAnnotationArguments(const das::AnnotationArgumentList& Args,const TSet<FString>& Repeatable={}){
        TSet<FString> Seen;for(const auto& A:Args){const FString Name=DSLText(A.name);if(Seen.Contains(Name) && !Repeatable.Contains(Name))DSLFail(TEXT("Repeated annotation option: ")+Name);Seen.Add(Name);}
    }
    void DSLDeclarationKind(const das::AnnotationList& Args){TArray<FString> Kinds;for(const auto* A:Args){const auto& Name=A->annotation->name;if(Name=="ue_class" || Name=="ue_interface" || Name=="ue_struct" || Name=="ue_native_object")Kinds.Add(DSLText(Name));}if(Kinds.Num()>1)DSLFail(TEXT("Repeated/conflicting declaration annotations: ")+FString::Join(Kinds,TEXT(",")));}
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
    #include "DasObjectTypes.inl"
    FString DSLTypeName(const das::TypeDeclPtr& T){if(!T)return {};if(DSLTypedObject(T))return TEXT("object");if(T->baseType==das::Type::tEnumeration && T->enumType && DSLAnnotation(T->enumType->annotations,"ue_enum"))return TEXT("enum");if(T->baseType==das::Type::tStructure && T->structType && DSLAnnotation(T->structType->annotations,"ue_struct"))return TEXT("struct");switch(T->baseType){case das::Type::tArray:return TEXT("array");case das::Type::tTable:return T->secondType && T->secondType->baseType==das::Type::tVoid?TEXT("set"):TEXT("map");case das::Type::tInt:return TEXT("int");case das::Type::tFloat:return TEXT("float");case das::Type::tBool:return TEXT("bool");case das::Type::tDouble:return TEXT("double");case das::Type::tString:return TEXT("string");case das::Type::tVoid:return TEXT("void");case das::Type::tHandle:if(T->annotation && T->annotation->name=="UEEnumValue")return TEXT("enum");if(T->annotation && T->annotation->name=="UENameValue")return TEXT("name");if(T->annotation && T->annotation->name=="UObjectHandle")return TEXT("object");if(T->annotation && (T->annotation->name=="UEVector" || T->annotation->name=="UERotator" || T->annotation->name=="UETransform" || T->annotation->name=="UEStructValue"))return TEXT("struct");default:break;}return {};}
    FString DSLMathPath(const das::TypeDeclPtr& T){if(!T || !T->annotation)return {};return T->annotation->name=="UEVector"?TEXT("/Script/CoreUObject.Vector"):T->annotation->name=="UERotator"?TEXT("/Script/CoreUObject.Rotator"):TEXT("/Script/CoreUObject.Transform");}
    bool DSLPropertyMatches(FProperty* P,const FString& K){return K==TEXT("int")?CastField<FIntProperty>(P)!=nullptr:K==TEXT("float")?CastField<FFloatProperty>(P)!=nullptr:K==TEXT("bool")?CastField<FBoolProperty>(P)!=nullptr:K==TEXT("double")?CastField<FDoubleProperty>(P)!=nullptr:K==TEXT("string")?CastField<FStrProperty>(P)!=nullptr:K==TEXT("struct")?CastField<FStructProperty>(P)!=nullptr:K==TEXT("name")?CastField<FNameProperty>(P)!=nullptr:K==TEXT("enum")?(CastField<FByteProperty>(P) && CastField<FByteProperty>(P)->Enum):CastField<FObjectProperty>(P)!=nullptr && !CastField<FClassProperty>(P);}
    bool DSLSet(const das::TypeDeclPtr& T){return T && T->baseType==das::Type::tTable && T->secondType && T->secondType->baseType==das::Type::tVoid;}
    bool DSLContainer(const das::TypeDeclPtr& T){return T && (T->baseType==das::Type::tArray || T->baseType==das::Type::tTable);}
    FDasDeclaredType DSLContainerType(const das::TypeDeclPtr& T){
        FDasDeclaredType D;D.Kind=DSLTypeName(T);
        auto First=MakeShared<FDasDeclaredType>();First->Kind=DSLTypeName(T->firstType);
        if(D.Kind==TEXT("map")){D.Key=First;auto Value=MakeShared<FDasDeclaredType>();Value->Kind=DSLTypeName(T->secondType);D.Value=Value;}else D.Element=First;
        if(!DasContainerCarrier(D))DSLFail(TEXT("Standalone containers require primitive arrays, int/string sets or int/string-key scalar/name maps"));
        return D;
    }
    #include "DasDirectTypes.inl"
    struct FDSLBodyVisitor:das::Visitor
    {
        const FDasDeclaredClass& Decl;das::string Self;const das::StructurePtr Source;FString Folder;TSet<const das::Expression*> LeftFields;FString Error;
        FDSLBodyVisitor(const FDasDeclaredClass& C,const das::string& S,const das::StructurePtr& St,const FString& F):Decl(C),Self(S),Source(St),Folder(F){}
        das::ExprField* SelfField(das::Expression* E)
        {
            if(!E || !E->rtti_isField()) return nullptr;
            auto* F=static_cast<das::ExprField*>(E);
            return F->value && F->value->rtti_isVar() && static_cast<das::ExprVar*>(F->value)->name==Self?F:nullptr;
        }
        const FDasDeclaredProperty* Property(das::ExprField* F)
        {
            const FString N=DSLText(F->name);const auto* P=Decl.Properties.FindByPredicate([&](const auto& X){return X.Name==N;});
            if(!P) Error=TEXT("Unknown reflected self property: ")+N;return P;
        }
        das::ExpressionPtr Read(das::ExprField* F)
        {
            const auto* P=Property(F);if(!P) return F;
            auto* C=new das::ExprCall(F->at,P->Type.Kind==TEXT("int")?"ue_get_self_int":P->Type.Kind==TEXT("float")?"ue_get_self_float":P->Type.Kind==TEXT("bool")?"ue_get_self_bool":P->Type.Kind==TEXT("double")?"ue_get_self_double":P->Type.Kind==TEXT("string")?"ue_get_self_string":P->Type.Kind==TEXT("struct")?"ue_get_self_struct":P->Type.Kind==TEXT("object")?"ue_get_self_object":P->Type.Kind==TEXT("enum")?"ue_get_self_enum":"ue_get_self_name");
            C->arguments.push_back(new das::ExprConstString(F->at,DSLUtf8(P->Name)));const auto* Field=Source->findField(DSLUtf8(P->Name));return Field && (DSLDirectEnum(Field->type) || DSLDirectStruct(Field->type) || DSLTypedObject(Field->type))?DSLDirectUnpack(F->at,Field->type,C,P->Type.TypePath):C;
        }
        das::ExpressionPtr Write(das::ExprOp2* E,das::ExprField* F,bool Compound)
        {
            const auto* P=Property(F);if(!P) return E;
            if(P->bReadOnly){Error=TEXT("Cannot write BlueprintReadOnly self property: ")+P->Name;return E;}
            auto* C=new das::ExprCall(E->at,P->Type.Kind==TEXT("int")?"ue_set_self_int":P->Type.Kind==TEXT("float")?"ue_set_self_float":P->Type.Kind==TEXT("bool")?"ue_set_self_bool":P->Type.Kind==TEXT("double")?"ue_set_self_double":P->Type.Kind==TEXT("string")?"ue_set_self_string":P->Type.Kind==TEXT("struct")?"ue_set_self_struct":P->Type.Kind==TEXT("object")?"ue_set_self_object":P->Type.Kind==TEXT("enum")?"ue_set_self_enum":"ue_set_self_name");
            C->arguments.push_back(new das::ExprConstString(E->at,DSLUtf8(P->Name)));
            auto* Value=Compound?static_cast<das::ExpressionPtr>(new das::ExprOp2(E->at,E->op.substr(0,E->op.size()-1),Read(F),E->right)):E->right;const auto* Field=Source->findField(DSLUtf8(P->Name));C->arguments.push_back(Field && (DSLDirectEnum(Field->type) || DSLDirectStruct(Field->type) || DSLTypedObject(Field->type))?DSLDirectPack(E->at,Field->type,Value,P->Type.TypePath):Value);return C;
        }
        das::ExprField* StructRoot(das::Expression* E){for(int32 Depth=0;E && E->rtti_isField() && Depth<18;++Depth){auto* F=static_cast<das::ExprField*>(E);if(auto* Root=SelfField(F->value)){const auto* Member=Source->findField(Root->name);return Member && DSLDirectStruct(Member->type)?Root:nullptr;}E=F->value;}return nullptr;}
        void MarkLeft(das::Expression* E){bool Indexed=false;auto* Check=E;for(int32 D=0;Check && D<32;++D){if(auto* Root=SelfField(Check)){const auto* Member=Source->findField(Root->name);if(Indexed && Member && DSLDirectStruct(Member->type)){Error=TEXT("Indexed self array writes require local clone and whole-property assignment");return;}break;}if(Check->rtti_isAt()){Indexed=true;Check=static_cast<das::ExprAt*>(Check)->subexpr;}else if(Check->rtti_isField())Check=static_cast<das::ExprField*>(Check)->value;else break;}if(SelfField(E))LeftFields.Add(E);if(auto* Root=StructRoot(E)){for(auto* Current=E;Current && Current->rtti_isField();){LeftFields.Add(Current);if(Current==Root)break;Current=static_cast<das::ExprField*>(Current)->value;}}}
        das::ExpressionPtr WriteLeaf(das::ExprOp2* E,bool Compound){auto* Leaf=static_cast<das::ExprField*>(E->left);auto* Root=StructRoot(Leaf);const auto* P=Property(Root);if(!P)return E;if(P->bReadOnly){Error=TEXT("Cannot write BlueprintReadOnly self struct: ")+P->Name;return E;}das::vector<das::string> Names;for(auto* Current=Leaf;Current!=Root;Current=static_cast<das::ExprField*>(Current->value))Names.push_back(Current->name);auto Type=Source->findField(Root->name)->type;das::string Path;
            for(auto It=Names.rbegin();It!=Names.rend();++It){if(!DSLDirectStruct(Type)){Error=TEXT("Nested write crosses non-authored struct");return E;}const auto* Field=Type->structType->findField(*It);if(!Field){Error=TEXT("Unknown self struct field");return E;}for(const auto& A:Field->annotation)if(A.name=="ue_property" && A.type==das::Type::tString && A.sValue=="BlueprintReadOnly"){Error=TEXT("Cannot write BlueprintReadOnly struct member");return E;}if(!Path.empty())Path+=".";Path+=*It;Type=Field->type;}
            const FString K=DSLTypeName(Type);if(DSLContainer(Type) || K==TEXT("struct") || K==TEXT("enum") || K.IsEmpty()){Error=TEXT("Nested member assignment supports scalar leaves; copy and assign the whole self property for nested values");return E;}const char* Suffix=DSLScalarSuffix(K);auto Token=[&](){return DSLCall(E->at,"ue_get_self_struct",{new das::ExprConstString(E->at,Root->name)});};auto* Value=Compound?static_cast<das::ExpressionPtr>(new das::ExprOp2(E->at,E->op.substr(0,E->op.size()-1),DSLCall(E->at,das::string("ue_struct_get_")+Suffix,{Token(),new das::ExprConstString(E->at,Path)}),E->right)):E->right;return DSLCall(E->at,das::string("ue_dsl_commit_struct_member_")+Suffix,{new das::ExprConstString(E->at,Root->name),new das::ExprConstString(E->at,Path),Value});}
        virtual void preVisit(das::ExprCall* E) override {if((E->name=="insert" || E->name=="erase" || E->name=="clear") && !E->arguments.empty() && (SelfField(E->arguments[0]) || StructRoot(E->arguments[0])))Error=TEXT("Container mutation over self requires a local clone and whole-property assignment");if((E->name=="keys" || E->name=="values") && !E->arguments.empty() && (SelfField(E->arguments[0]) || StructRoot(E->arguments[0])))Error=TEXT("Container iterators over self require a local clone");}
        virtual void preVisit(das::ExprCopy* E) override {MarkLeft(E->left);}
        virtual void preVisit(das::ExprMove* E) override {MarkLeft(E->left);}
        virtual void preVisit(das::ExprOp2* E) override {if(E->op=="+=" || E->op=="-=" || E->op=="*=" || E->op=="/=")MarkLeft(E->left);}
        virtual das::ExpressionPtr visit(das::ExprField* E) override {return SelfField(E) && !LeftFields.Contains(E)?Read(E):E;}
        virtual das::ExpressionPtr visit(das::ExprCopy* E) override {if(StructRoot(E->left))return WriteLeaf(E,false);auto* F=SelfField(E->left);return F?Write(E,F,false):E;}
        virtual das::ExpressionPtr visit(das::ExprMove* E) override {if(StructRoot(E->left))return WriteLeaf(E,false);auto* F=SelfField(E->left);return F?Write(E,F,false):E;}
        virtual das::ExpressionPtr visit(das::ExprOp2* E) override {if(LeftFields.Contains(E->left) && StructRoot(E->left))return WriteLeaf(E,true);auto* F=SelfField(E->left);return F && LeftFields.Contains(F)?Write(E,F,true):E;}
        virtual das::ExpressionPtr visit(das::ExprWith* E) override {return E->with && E->with->rtti_isVar() && static_cast<das::ExprVar*>(E->with)->name==Self?E->body:E;}
    };
    struct FUEFunctionAnnotation:das::FunctionAnnotation
    {
        FUEFunctionAnnotation():FunctionAnnotation("ue_function"){}
        virtual bool apply(const das::FunctionPtr& F,das::ModuleGroup&,das::AnnotationArgumentList&,das::string& E) override {if(!F->classParent){E="ue_function must be a member of ue_class or ue_interface";return false;}return true;}
        virtual bool finalize(const das::FunctionPtr& F,das::ModuleGroup&,das::AnnotationArgumentList&,const das::AnnotationArgumentList&,das::string& Error) override {try{DSLAnnotation(F->annotations,"ue_function");if(!F->classParent || (!DSLAnnotation(F->classParent->annotations,"ue_class") && !DSLAnnotation(F->classParent->annotations,"ue_interface")))DSLFail(TEXT("ue_function requires ue_class or ue_interface"));return true;}catch(const std::exception& X){Error=X.what();return false;}}
        virtual bool apply(das::ExprBlock*,das::ModuleGroup&,das::AnnotationArgumentList&,das::string& E) override {E="ue_function cannot annotate block";return false;}
        virtual bool finalize(das::ExprBlock*,das::ModuleGroup&,das::AnnotationArgumentList&,const das::AnnotationArgumentList&,das::string&) override {return false;}
    };
    struct FUEImplementsAnnotation:das::StructureAnnotation {
        FUEImplementsAnnotation():StructureAnnotation("ue_implements"){}
        virtual bool touch(const das::StructurePtr& St,das::ModuleGroup&,das::AnnotationArgumentList& Args,das::string& Error)override{try{if(!St->isClass)DSLFail(TEXT("ue_implements requires ue_class"));if(Args.size()!=1 || Args[0].name!="interface" || Args[0].type!=das::Type::tString || DSLString(Args,"interface").IsEmpty())DSLFail(TEXT("Expected ue_implements(interface=NameOrPath)"));return true;}catch(const std::exception& X){Error=X.what();return false;}}
        virtual bool look(const das::StructurePtr& St,das::ModuleGroup&,das::AnnotationArgumentList&,das::string& Error)override{if(!DSLAnnotation(St->annotations,"ue_class")){Error="ue_implements requires ue_class";return false;}return true;}
    };
    struct FUEClassAnnotation:das::StructureAnnotation
    {
        FUEClassAnnotation():StructureAnnotation("ue_class"){}
        virtual bool look(const das::StructurePtr& St,das::ModuleGroup& Group,das::AnnotationArgumentList& Args,das::string& Error) override {try{
            DSLDeclarationKind(St->annotations);DSLAnnotation(St->annotations,"ue_class");
            if(CollectTypesOnly)return true;
            if(const auto* Done=Args.find("_ue_implements_resolved",das::Type::tBool)){if(Done->at.fileInfo)DSLFail(TEXT("Reserved implements argument"));return true;}
            auto* Wire=Args.find(DSLModelKey,das::Type::tString);FDasAuthoringModule Model;FString Issue;if(!Wire || !FDasAuthoringModule::FromJson(DSLText(Wire->sValue),Model,Issue))DSLFail(Issue);auto& C=Model.Classes[0];const bool LegacyInterfaces=C.Interfaces.Num()!=0;
            for(const auto* A:St->annotations)if(A->annotation->name=="ue_implements"){const FString Path=DSLResolveType(EDSLTypeKind::Interface,DSLString(A->arguments,"interface"),FDSLTypeScope{Model.Id,DSLString(Args,"type_folder"),DSLString(Args,"type_registry"),&Group,St->module});if(C.Interfaces.Contains(Path))DSLFail(TEXT("Duplicate interface implementation: ")+Path);C.Interfaces.Add(Path);}
            // Legacy scalar adapters rename their single input before class annotations
            // are attached. Interface contracts must retain the source parameter name.
            if(!LegacyInterfaces && !C.Interfaces.IsEmpty())St->module->functions.foreach([&](const das::FunctionPtr& F){if(F->classParent!=St || !DSLAnnotation(F->annotations,"ue_function") || F->arguments.size()!=2)return;auto* M=C.Methods.FindByPredicate([&](const auto& Candidate){return Candidate.Name==DSLText(F->name);});if(!M || M->Parameters.Num()!=1)return;auto& P=M->Parameters[0];const FString K=DSLTypeName(F->arguments[1]->type);if(P.Direction==TEXT("in") && P.Name==TEXT("Input") && K==M->ReturnType.Kind && (K==TEXT("int") || K==TEXT("float") || K==TEXT("bool"))){P.Name=DSLText(F->arguments[1]->name);P.Id=M->Id+TEXT(".")+P.Name;}});
            FString Json;if(!Model.ToJson(Json,Issue))DSLFail(Issue);FDasClassSchema Schema;if(!Model.ToActorSchema(C.Id,Schema,Issue))DSLFail(Issue);for(auto& Arg:Args)if(Arg.name==DSLModelKey)Arg.sValue=DSLUtf8(Json);Args.push_back(das::AnnotationArgument("_ue_implements_resolved",true));return true;
        }catch(const std::exception& X){Error=X.what();return false;}}
        virtual bool touch(const das::StructurePtr& St,das::ModuleGroup& Group,das::AnnotationArgumentList& Args,das::string& Error) override
        {
            try
            {
                DSLAnnotationArguments(Args,{TEXT("implements")});
                if(CollectTypesOnly)return true;
                if(const auto* Wire=Args.find(DSLModelKey,das::Type::tString)){if(Wire->at.fileInfo) DSLFail(TEXT("Reserved internal annotation argument"));return true;}
                FDasAuthoringModule Model;const FString Script=FPaths::GetCleanFilename(DSLText(St->at.fileInfo->name));
                Model.Id=DSLString(Args,"module_id",FPaths::GetBaseFilename(Script));
                const FString TypeFolder=DSLString(Args,"type_folder"),TypeRegistry=DSLString(Args,"type_registry");if(!TypeFolder.IsEmpty() && !TypeRegistry.IsEmpty())DSLFail(TEXT("Choose type_registry or type_folder"));das::vector<das::FunctionPtr> HelpersGenerated;TSet<FString> Helpers;
                FDasDeclaredClass C;C.Name=DSLText(St->name);C.Id=DSLString(Args,"id",Model.Id+TEXT(".")+C.Name);const FDSLTypeScope TypeScope{Model.Id,TypeFolder,TypeRegistry,&Group,St->module};C.NativeParent=DSLResolveType(EDSLTypeKind::Class,DSLString(Args,"parent",TEXT("/Script/Engine.Actor")),TypeScope);
                for(const auto& A:Args)
                {
                    if(A.name=="module_id" || A.name=="id" || A.name=="parent" || A.name=="type_folder" || A.name=="type_registry") continue;if(A.name=="implements"){TArray<FString> Items;DSLString(Args,"implements").ParseIntoArray(Items,TEXT(","),false);for(auto I:Items){I=I.TrimStartAndEnd();I=DSLResolveType(EDSLTypeKind::Interface,I,TypeScope);if(C.Interfaces.Contains(I))DSLFail(TEXT("Empty/duplicate interface path"));C.Interfaces.Add(I);}if(C.Interfaces.IsEmpty())DSLFail(TEXT("Expected interface paths"));continue;}
                    if(A.name=="Blueprintable"){if(!DSLBool(A)) DSLFail(TEXT("Adapter requires Blueprintable"));continue;}
                    if(DSLIsMetadata(A.name)) DSLMetadata(A,C.Metadata);else DSLFail(TEXT("Unknown ue_class annotation: ")+DSLText(A.name));
                }
                if(St->parent) DSLFail(TEXT("Use ue_class parent identity; native daScript inheritance is not supported yet"));
                for(auto& Field:St->fields)if(!Field.classMethod && !Field.generated && Field.name!="__rtti")Field.type=DSLObjectNormalize(Field.type,Group,St->module);
                St->module->functions.foreach([&](const das::FunctionPtr& Method){if(Method->classParent==St && DSLAnnotation(Method->annotations,"ue_function")){Method->result=DSLObjectNormalize(Method->result,Group,St->module);for(size_t I=1;I<Method->arguments.size();++I)Method->arguments[I]->type=DSLObjectNormalize(Method->arguments[I]->type,Group,St->module);}});
                for(const auto& F:St->fields)
                {
                    // Pinned makeClassRtti adds __rtti without setting generated.
                    if(F.classMethod || F.generated || F.name=="__rtti") continue;
                    FDasDeclaredProperty P;P.Name=DSLText(F.name);P.Id=C.Id+TEXT(".")+P.Name;bool Marked=false,ReadWrite=false,ReadOnly=false;
                    for(const auto& A:F.annotation)
                    {
                        if(A.name=="ue_property")
                        {
                            Marked=true;
                            if(A.type==das::Type::tBool){if(!A.bValue) DSLFail(TEXT("ue_property must be enabled"));}
                            else if(A.type==das::Type::tString)
                            {
                                if(A.sValue=="EditAnywhere") P.bEditable=true;
                                else if(A.sValue=="BlueprintReadWrite") {P.bReadOnly=false;ReadWrite=true;}
                                else if(A.sValue=="BlueprintReadOnly") {P.bReadOnly=true;ReadOnly=true;}
                                else DSLFail(TEXT("Unsupported property specifier: ")+DSLText(A.sValue));
                            }
                            else DSLFail(TEXT("ue_property requires flags or true"));
                        }
                        else if(A.name=="ue_id"){if(A.type!=das::Type::tString) DSLFail(TEXT("ue_id must be string"));P.Id=DSLText(A.sValue);}
                        else if(DSLIsMetadata(A.name)) DSLMetadata(A,P.Metadata);else DSLFail(TEXT("Unknown field metadata: ")+DSLText(A.name));
                    }
                    if(ReadWrite && ReadOnly) DSLFail(TEXT("Conflicting BlueprintReadOnly/BlueprintReadWrite specifiers"));
                    if(!Marked) DSLFail(TEXT("Script-private fields unsupported; add @ue_property: ")+P.Name);
                    const FString FieldType=DSLTypeName(F.type);
                    if(!F.type || F.type->ref || FieldType.IsEmpty() || FieldType==TEXT("void")) DSLFail(TEXT("Adapter supports int/float/bool/string/double/name properties: ")+P.Name);
                    P.Type.Kind=FieldType;
                    if(DSLDirectEnum(F.type)){P.Type.TypePath=DSLDirectPath(F.type,TypeFolder,TypeRegistry);P.Default.Kind=TEXT("string");if(F.init && F.init->__rtti==das::string("ExprConstEnumeration"))P.Default.String=DSLText(static_cast<das::ExprConstEnumeration*>(F.init)->text);else if(F.init && F.init->rtti_isField()){auto* Init=static_cast<das::ExprField*>(F.init);if(!Init->value || !Init->value->rtti_isVar() || static_cast<das::ExprVar*>(Init->value)->name!=F.type->enumType->name)DSLFail(TEXT("Direct enum initializer must name its declared enum"));P.Default.String=DSLText(Init->name);}else DSLFail(TEXT("Direct enum property requires named enum literal initializer: ")+(F.init?DSLText(F.init->__rtti):TEXT("none")));if(P.Default.String.Contains(TEXT(".")))P.Default.String=P.Default.String.RightChop(P.Default.String.Find(TEXT("."))+1);if(P.Default.String.Contains(TEXT("::")))P.Default.String=P.Default.String.RightChop(P.Default.String.Find(TEXT("::"))+2);FString Issue;auto* E=ResolveDasEnum(P.Type.TypePath,Issue);if(!E || !ValidateDasEnumNumber(P.Type.TypePath,E->GetValueByNameString(P.Default.String),Issue))DSLFail(Issue);C.Properties.Add(P);continue;}
                    if(DSLDirectStruct(F.type)){P.Type.TypePath=DSLDirectPath(F.type,TypeFolder,TypeRegistry);DSLEnsureStructHelpers(F.type,P.Type.TypePath,Group,St->module,HelpersGenerated,Helpers);P.Default.Kind=TEXT("string");P.Default.String=TEXT("");if(F.init){bool Empty=false;if(F.init->rtti_isCall()){auto* I=static_cast<das::ExprCall*>(F.init);Empty=I->name==F.type->structType->name && I->arguments.empty();}else if(F.init->rtti_isMakeStruct()){auto* I=static_cast<das::ExprMakeStruct*>(F.init);Empty=I->structs.empty() && !I->block;}if(!Empty)DSLFail(TEXT("Direct struct defaults require empty struct constructor; field overrides are not supported yet"));}C.Properties.Add(P);continue;}
                    if(FieldType==TEXT("enum")){if(!F.init || !F.init->rtti_isCall())DSLFail(TEXT("Enum field requires ue_enum_value(path, member)"));auto* Call=static_cast<das::ExprCall*>(F.init);if(Call->name!="ue_enum_value" || Call->arguments.size()!=2 || Call->arguments[0]->__rtti!=das::string("ExprConstString") || Call->arguments[1]->__rtti!=das::string("ExprConstString"))DSLFail(TEXT("Enum initializer requires literal path/member"));P.Type.TypePath=DSLResolveType(EDSLTypeKind::Enum,DSLText(static_cast<das::ExprConstString*>(Call->arguments[0])->text),TypeScope);static_cast<das::ExprConstString*>(Call->arguments[0])->text=DSLUtf8(P.Type.TypePath);P.Default.Kind=TEXT("string");P.Default.String=DSLText(static_cast<das::ExprConstString*>(Call->arguments[1])->text);FString Issue;auto* E=ResolveDasEnum(P.Type.TypePath,Issue);if(!E || !ValidateDasEnumNumber(P.Type.TypePath,E->GetValueByNameString(P.Default.String),Issue))DSLFail(Issue);C.Properties.Add(P);continue;}
                    if(DSLTypedObject(F.type)){P.Type.TypePath=DSLTypedObjectPath(F.type);if(F.init)DSLFail(TEXT("Typed object property defaults must be omitted (null)"));P.Default.Kind=TEXT("null");C.Properties.Add(MoveTemp(P));continue;}
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
                das::vector<das::FunctionPtr> Functions,Generated;
                St->module->functions.foreach([&](const das::FunctionPtr& F){if(F->classParent==St && DSLAnnotation(F->annotations,"ue_function")) Functions.push_back(F);});
                for(const auto& F:Functions)
                {
                    const auto* A=DSLAnnotation(F->annotations,"ue_function");DSLAnnotationArguments(A->arguments,{TEXT("out"),TEXT("ref")});FDasDeclaredMethod M;
                    M.Name=DSLText(F->name.substr(F->name.find_last_of(das::string(1,char(96))+das::string(1,char(39)))+1));M.Id=C.Id+TEXT(".")+M.Name;M.ScriptFile=Script;M.EntryPoint=TEXT("ue_")+C.Name+TEXT("_")+M.Name;M.bCallable=false;TMap<FString,FString> ObjectClasses;TMap<FString,FString> Directions;
                    for(const auto& V:A->arguments)
                    {
                        if(V.name=="id"){M.Id=DSLString(A->arguments,"id");continue;}
                        if(V.name=="out" || V.name=="ref"){DSLAddDirections(V,Directions);continue;}
                        if(V.name=="BlueprintCallable") M.bCallable=DSLBool(V);
                        else if(V.name=="return_class" || V.name=="return_struct" || V.name=="return_enum" || (V.name.find("arg_")==0 && V.name.size()>10 && (V.name.substr(V.name.size()-6)=="_class" || V.name.substr(V.name.size()-7)=="_struct" || V.name.substr(V.name.size()-5)=="_enum"))){ObjectClasses.Add(DSLText(V.name),DSLString(A->arguments,V.name.c_str()));}
                        else if(DSLIsMetadata(V.name)) DSLMetadata(V,M.Metadata);else DSLFail(TEXT("Unsupported ue_function specifier: ")+DSLText(V.name));
                    }
                    const FString ResultType=DSLTypeName(F->result);
                    if(!M.bCallable || F->isStaticClassMethod || !F->result || F->result->ref || ResultType.IsEmpty() || F->arguments.empty() || F->arguments.size()>17) DSLFail(TEXT("Adapter requires BlueprintCallable instance scalar/void method with up to 16 by-value inputs: ")+M.Name);
                    M.ReturnType.Kind=ResultType;if(DSLContainer(F->result)){M.ReturnType=DSLContainerType(F->result);DSLEnsureStructHelpers(F->result,DSLDirectPath(F->result,TypeFolder,TypeRegistry),Group,St->module,HelpersGenerated,Helpers);}if(ResultType==TEXT("enum")){M.ReturnType.TypePath=DSLConstraint(ObjectClasses,TEXT("return_enum"),EDSLTypeKind::Enum,DSLDirectEnum(F->result)?DSLDirectPath(F->result,TypeFolder,TypeRegistry):FString(),TypeScope);if(M.ReturnType.TypePath.IsEmpty())DSLFail(TEXT("Enum result requires return_enum"));}if(ResultType==TEXT("struct")){if(DSLDirectStruct(F->result)){M.ReturnType.TypePath=DSLConstraint(ObjectClasses,TEXT("return_struct"),EDSLTypeKind::Struct,DSLDirectPath(F->result,TypeFolder,TypeRegistry),TypeScope);DSLEnsureStructHelpers(F->result,M.ReturnType.TypePath,Group,St->module,HelpersGenerated,Helpers);}else if(F->result->annotation->name=="UEStructValue"){M.ReturnType.TypePath=DSLConstraint(ObjectClasses,TEXT("return_struct"),EDSLTypeKind::Struct,FString(),TypeScope);if(M.ReturnType.TypePath.IsEmpty())DSLFail(TEXT("UEStructValue result requires return_struct"));}else M.ReturnType.TypePath=DSLConstraint(ObjectClasses,TEXT("return_struct"),EDSLTypeKind::Struct,DSLMathPath(F->result),TypeScope);}if(ResultType==TEXT("object")){M.ReturnType.TypePath=DSLConstraint(ObjectClasses,TEXT("return_class"),EDSLTypeKind::Class,DSLTypedObject(F->result)?DSLTypedObjectPath(F->result):FString(),TypeScope);if(M.ReturnType.TypePath.IsEmpty())M.ReturnType.TypePath=TEXT("/Script/CoreUObject.Object");}
                    const bool LegacySingle=C.Interfaces.IsEmpty() && Directions.IsEmpty() && F->arguments.size()==2 && !F->arguments[1]->type->ref && (ResultType==TEXT("int") || ResultType==TEXT("float") || ResultType==TEXT("bool")) && DSLTypeName(F->arguments[1]->type)==ResultType;
                    for(size_t I=1;I<F->arguments.size();++I){const auto& V=F->arguments[I];const FString K=DSLTypeName(V->type);const FString Direction=DSLParameterDirection(V,Directions.FindRef(DSLText(V->name)));if(K.IsEmpty() || K==TEXT("void") || V->init) DSLFail(TEXT("Unsupported parameter type/ref/default: ")+DSLText(V->name));FDasDeclaredParameter P;P.Direction=Direction;Directions.Remove(DSLText(V->name));if(P.Direction!=TEXT("in") && K!=TEXT("int") && K!=TEXT("float") && K!=TEXT("double") && K!=TEXT("bool") && K!=TEXT("string") && K!=TEXT("name") && !DSLContainer(V->type))DSLFail(TEXT("Out/ref currently requires primitives or standalone containers"));P.Name=LegacySingle?TEXT("Input"):DSLText(V->name);P.Id=M.Id+TEXT(".")+(LegacySingle?TEXT("input"):P.Name);P.Type.Kind=K;if(DSLContainer(V->type)){P.Type=DSLContainerType(V->type);DSLEnsureStructHelpers(V->type,DSLDirectPath(V->type,TypeFolder,TypeRegistry),Group,St->module,HelpersGenerated,Helpers);}if(K==TEXT("enum")){const FString Key=TEXT("arg_")+DSLText(V->name)+TEXT("_enum");P.Type.TypePath=DSLConstraint(ObjectClasses,Key,EDSLTypeKind::Enum,DSLDirectEnum(V->type)?DSLDirectPath(V->type,TypeFolder,TypeRegistry):FString(),TypeScope);if(P.Type.TypePath.IsEmpty())DSLFail(TEXT("Enum argument requires arg_name_enum"));}if(K==TEXT("struct")){if(DSLDirectStruct(V->type)){const FString Key=TEXT("arg_")+DSLText(V->name)+TEXT("_struct");P.Type.TypePath=DSLConstraint(ObjectClasses,Key,EDSLTypeKind::Struct,DSLDirectPath(V->type,TypeFolder,TypeRegistry),TypeScope);DSLEnsureStructHelpers(V->type,P.Type.TypePath,Group,St->module,HelpersGenerated,Helpers);}else if(V->type->annotation->name=="UEStructValue"){const FString Key=TEXT("arg_")+DSLText(V->name)+TEXT("_struct");P.Type.TypePath=DSLConstraint(ObjectClasses,Key,EDSLTypeKind::Struct,FString(),TypeScope);if(P.Type.TypePath.IsEmpty())DSLFail(TEXT("UEStructValue parameter requires arg_name_struct"));}else{const FString Key=TEXT("arg_")+DSLText(V->name)+TEXT("_struct");P.Type.TypePath=DSLConstraint(ObjectClasses,Key,EDSLTypeKind::Struct,DSLMathPath(V->type),TypeScope);}}if(K==TEXT("object")){const FString Key=TEXT("arg_")+DSLText(V->name)+TEXT("_class");P.Type.TypePath=DSLConstraint(ObjectClasses,Key,EDSLTypeKind::Class,DSLTypedObject(V->type)?DSLTypedObjectPath(V->type):FString(),TypeScope);if(P.Type.TypePath.IsEmpty())P.Type.TypePath=TEXT("/Script/CoreUObject.Object");}M.Parameters.Add(P);}
                    if(!ObjectClasses.IsEmpty())DSLFail(TEXT("Unused object class annotation"));if(!Directions.IsEmpty())DSLFail(TEXT("Unknown out/ref parameter name"));
                    auto Clone=F->clone();Clone->name=DSLUtf8(M.EntryPoint);Clone->classParent=nullptr;Clone->annotations.clear();Clone->exports=true;Clone->generated=true;Clone->privateFunction=false;Clone->isStaticClassMethod=false;Clone->isConstClassMethod=false;
                    FDSLBodyVisitor Rewrite(C,Clone->arguments[0]->name,St,TypeFolder);Clone->body=Clone->body->visit(Rewrite);Clone->arguments.erase(Clone->arguments.begin());
                    if(!Rewrite.Error.IsEmpty()) DSLFail(Rewrite.Error);
                    bool HasOutputs=false;for(const auto& P:M.Parameters)HasOutputs |= P.Direction!=TEXT("in");bool Direct=HasOutputs || DSLContainer(F->result) || DSLDirectEnum(F->result) || DSLDirectStruct(F->result) || DSLTypedObject(F->result);for(size_t I=1;I<F->arguments.size();++I)Direct |= DSLContainer(F->arguments[I]->type) || DSLDirectEnum(F->arguments[I]->type) || DSLDirectStruct(F->arguments[I]->type) || DSLTypedObject(F->arguments[I]->type);
                    if(Direct){
                        auto Wrapper=Clone->clone();Clone->name=DSLUtf8(TEXT("__impl_")+M.EntryPoint);Clone->exports=false;auto* Call=new das::ExprCall(F->at,Clone->name);auto* Block=new das::ExprBlock();Block->at=F->at;
                        auto Local=[&](const das::string& Name,const das::TypeDeclPtr& Type,das::ExpressionPtr Init){auto* V=new das::Variable();V->at=F->at;V->name=Name;V->type=DSLCopyType(Type);V->type->ref=false;V->type->constant=false;V->init=Init;V->init_via_move=DSLContainer(Type);auto* Let=new das::ExprLet();Let->at=F->at;Let->variables.push_back(V);Block->list.push_back(Let);};
                        for(size_t I=0;I<Wrapper->arguments.size();++I){auto& Arg=Wrapper->arguments[I];auto Original=F->arguments[I+1]->type;Arg->type=DSLCopyType(Arg->type);Arg->type->ref=false;Arg->type->constant=true;das::ExpressionPtr Value=new das::ExprVar(F->at,Arg->name);
                            if(DSLContainer(Original) || DSLDirectEnum(Original) || DSLDirectStruct(Original) || DSLTypedObject(Original)){Arg->type=DSLHandleType(Group,St->module,DSLTypedObject(Original)?"UObjectHandle":DSLDirectEnum(Original)?"UEEnumValue":"UEStructValue");Value=DSLDirectUnpack(F->at,Original,Value,DSLContainer(Original)?DSLDirectPath(Original,TypeFolder,TypeRegistry):M.Parameters[int32(I)].Type.TypePath);}
                            if(M.Parameters[int32(I)].Direction!=TEXT("in")){const das::string Name="__ue_ref_"+das::to_string(I);Local(Name,Original,Value);Value=new das::ExprVar(F->at,Name);}Call->arguments.push_back(Value);
                        }
                        das::ExpressionPtr Value=Call;
                        if(HasOutputs){if(ResultType==TEXT("void")){Block->list.push_back(Call);Value=nullptr;}else{Local("__ue_return",F->result,Call);Value=new das::ExprVar(F->at,"__ue_return");}
                            for(int32 I=0;I<M.Parameters.Num();++I)if(M.Parameters[I].Direction!=TEXT("in")){auto Original=F->arguments[size_t(I)+1]->type;das::ExpressionPtr Updated=new das::ExprVar(F->at,"__ue_ref_"+das::to_string(I));const das::string Suffix=DSLContainer(Original)?"struct":DSLScalarSuffix(M.Parameters[I].Type.Kind);if(DSLContainer(Original))Updated=DSLDirectPack(F->at,Original,Updated,DSLDirectPath(Original,TypeFolder,TypeRegistry));Block->list.push_back(DSLCall(F->at,"ue_frame_out_"+Suffix,{new das::ExprConstInt(F->at,I),Updated}));}
                        }
                        if(DSLContainer(F->result) || DSLDirectEnum(F->result) || DSLDirectStruct(F->result) || DSLTypedObject(F->result)){Wrapper->result=DSLHandleType(Group,St->module,DSLTypedObject(F->result)?"UObjectHandle":DSLDirectEnum(F->result)?"UEEnumValue":"UEStructValue");Value=DSLDirectPack(F->at,F->result,Value,DSLContainer(F->result)?DSLDirectPath(F->result,TypeFolder,TypeRegistry):M.ReturnType.TypePath);}
                        if(Value)Block->list.push_back(M.ReturnType.Kind==TEXT("void")?Value:static_cast<das::ExpressionPtr>(new das::ExprReturn(F->at,Value)));Wrapper->body=Block;Generated.push_back(Wrapper);
                    }
                    Generated.push_back(Clone);C.Methods.Add(MoveTemp(M));
                }
                Model.Classes.Add(C);FString Json,Issue;
                if(!Model.ToJson(Json,Issue)) DSLFail(Issue);
                FDasClassSchema Supported;if(!Model.ToActorSchema(C.Id,Supported,Issue)) DSLFail(Issue);
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
        virtual bool touch(const das::StructurePtr& St,das::ModuleGroup&,das::AnnotationArgumentList& Args,das::string& Error)override{try{
            if(const auto* Wire=Args.find(DSLInterfaceKey,das::Type::tString)){if(Wire->at.fileInfo)DSLFail(TEXT("Reserved interface annotation argument"));return true;}
            DSLAnnotationArguments(Args,{TEXT("out")});if(!St->isClass || St->parent)DSLFail(TEXT("ue_interface requires a class without inheritance"));
            FDasAuthoringModule Model;Model.Id=DSLString(Args,"module_id",FPaths::GetBaseFilename(DSLText(St->at.fileInfo->name)));FDasDeclaredClass C;C.Id=DSLString(Args,"id",Model.Id+TEXT(".")+DSLText(St->name));C.Name=DSLText(St->name);C.NativeParent=TEXT("/Script/CoreUObject.Interface");TSet<FString> LegacyOuts;
            for(const auto& A:Args){if(A.name=="module_id" || A.name=="id")continue;if(A.name=="BlueprintType"){if(!DSLBool(A))DSLFail(TEXT("BlueprintType must be true"));continue;}if(A.name=="out"){TArray<FString> Names;DSLString(Args,"out").ParseIntoArray(Names,TEXT(","),false);for(auto N:Names){N=N.TrimStartAndEnd();if(N.IsEmpty() || LegacyOuts.Contains(N))DSLFail(TEXT("Duplicate/empty interface out name"));LegacyOuts.Add(N);}continue;}if(DSLIsMetadata(A.name))DSLMetadata(A,C.Metadata);else DSLFail(TEXT("Unknown ue_interface annotation"));}
            auto Type=[&](const das::TypeDeclPtr& T){FDasDeclaredType D;D.Kind=DSLTypeName(T);if(DSLContainer(T))return DSLContainerType(T);if(D.Kind!=TEXT("int") && D.Kind!=TEXT("float") && D.Kind!=TEXT("double") && D.Kind!=TEXT("bool") && D.Kind!=TEXT("string") && D.Kind!=TEXT("name") && D.Kind!=TEXT("void"))DSLFail(TEXT("Authored interface requires primitive/current container types"));return D;};
            St->module->functions.foreach([&](const das::FunctionPtr& F){if(F->classParent!=St || F->generated)return;const auto Name=F->name.substr(F->name.find_last_of("`")+1);if(F->isStaticClassMethod || Name==St->name || Name=="finalize" || !St->findField(Name))DSLFail(TEXT("Interface definitions cannot declare static methods, constructors or finalizers"));});
            for(const auto& Field:St->fields){
                if(Field.generated || Field.name=="__rtti")continue;if(!Field.classMethod)DSLFail(TEXT("Interface definitions cannot declare fields"));
                FDasDeclaredMethod M;M.Name=DSLText(Field.name);M.Id=C.Id+TEXT(".")+M.Name;M.ScriptFile=FPaths::GetCleanFilename(DSLText(St->at.fileInfo->name));M.EntryPoint=M.Name;
                TMap<FString,FString> Directions;das::FunctionPtr Function=nullptr;das::TypeDeclPtr Return;das::vector<das::TypeDeclPtr> Types;das::vector<das::string> Names;
                if(Field._abstract){
                    if(Field.init || !Field.type || Field.type->baseType!=das::Type::tFunction)DSLFail(TEXT("Invalid legacy abstract interface signature"));Return=Field.type->firstType;
                    for(size_t I=1;I<Field.type->argTypes.size();++I){Types.push_back(Field.type->argTypes[I]);Names.push_back(Field.type->argNames[I]);}
                }else{
                    St->module->functions.foreach([&](const das::FunctionPtr& Candidate){if(Candidate->classParent==St && Candidate->name==St->name+"`"+Field.name)Function=Candidate;});
                    if(!Function || Function->isStaticClassMethod || Function->arguments.empty())DSLFail(TEXT("Interface requires ordinary instance method stubs"));
                    int32 AnnotationCount=0;for(const auto* A:Function->annotations)if(A->annotation->name=="ue_function")++AnnotationCount;
                    if(AnnotationCount!=1)DSLFail(TEXT("Interface stub requires exactly one ue_function annotation"));const auto* Annotation=DSLAnnotation(Function->annotations,"ue_function");DSLAnnotationArguments(Annotation->arguments,{TEXT("out"),TEXT("ref")});M.bCallable=false;
                    for(const auto& A:Annotation->arguments){
                        if(A.name=="id"){M.Id=DSLString(Annotation->arguments,"id");continue;}
                        if(A.name=="BlueprintCallable"){M.bCallable=DSLBool(A);continue;}
                        if(A.name=="out" || A.name=="ref"){DSLAddDirections(A,Directions);continue;}
                        if(DSLIsMetadata(A.name))DSLMetadata(A,M.Metadata);else DSLFail(TEXT("Unsupported interface ue_function specifier: ")+DSLText(A.name));
                    }
                    if(!M.bCallable)DSLFail(TEXT("Interface stub requires BlueprintCallable"));
                    das::ExpressionPtr Body=Function->body;
                    if(Body && Body->rtti_isBlock()){auto* Outer=static_cast<das::ExprBlock*>(Body);if(Outer->generated && Outer->list.size()==1 && Outer->finalList.empty() && Outer->list[0]->rtti_isWith()){auto* With=static_cast<das::ExprWith*>(Outer->list[0]);if(With->generated)Body=With->body;}}
                    if(!Body || !Body->rtti_isBlock())DSLFail(TEXT("Interface method body must be an empty declaration stub"));auto* Stub=static_cast<das::ExprBlock*>(Body);if(!Stub->list.empty() || !Stub->finalList.empty() || !Stub->arguments.empty())DSLFail(TEXT("Interface method body must be an empty declaration stub"));
                    Return=Function->result;for(size_t I=1;I<Function->arguments.size();++I){if(Function->arguments[I]->init)DSLFail(TEXT("Interface parameter defaults are unsupported"));Types.push_back(Function->arguments[I]->type);Names.push_back(Function->arguments[I]->name);}
                }
                M.ReturnType=Type(Return);if(!Return || Return->ref)DSLFail(TEXT("Interface return references unsupported"));
                for(size_t I=0;I<Types.size();++I){const auto& T=Types[I];FDasDeclaredParameter P;P.Name=DSLText(Names[I]);P.Id=M.Id+TEXT(".")+P.Name;P.Type=Type(T);if(!T || P.Type.Kind==TEXT("void") || (T->ref && T->constant))DSLFail(TEXT("Interface parameters require value or var mutable reference types"));const FString Key=M.Name+TEXT(".")+P.Name;const bool LegacyOut=LegacyOuts.Contains(Key);const FString Direction=Directions.FindRef(P.Name);
                    if(LegacyOut && !Direction.IsEmpty())DSLFail(TEXT("Do not mix class-level and method-level directions for one parameter"));if(Field._abstract){P.Direction=LegacyOut?TEXT("out"):T->ref?TEXT("inout"):TEXT("in");if(LegacyOut && !T->ref)DSLFail(TEXT("Interface out requires mutable reference"));}else{P.Direction=DSLParameterDirection(Function->arguments[I+1],LegacyOut?TEXT("out"):Direction);}
                    Directions.Remove(P.Name);LegacyOuts.Remove(Key);M.Parameters.Add(P);
                }
                if(!Directions.IsEmpty())DSLFail(TEXT("Unknown interface out/ref parameter name"));if(M.Parameters.Num()>16)DSLFail(TEXT("Interface parameter count exceeds 16"));bool Output=M.ReturnType.Kind!=TEXT("void");for(const auto& P:M.Parameters)Output |= P.Direction!=TEXT("in");C.Methods.Add(M);
                if(Function){
                    // Empty source bodies are declaration markers. Compiled direct calls trap.
                    auto* Trap=new das::ExprBlock();Trap->at=Function->at;Trap->generated=true;Trap->list.push_back(DSLCall(Function->at,"panic",{new das::ExprConstString(Function->at,"ue_interface declaration stub cannot be called directly")}));
                    if(M.ReturnType.Kind!=TEXT("void")){auto* Default=new das::ExprMakeStruct(Function->at);Default->makeType=DSLCopyType(Return);Default->alwaysUseInitializer=true;auto* Result=new das::ExprReturn(Function->at,Default);Result->moveSemantics=DSLContainer(Return);Trap->list.push_back(Result);}Function->body=Trap;
                }
            }
            if(C.Methods.IsEmpty() || C.Methods.Num()>64 || !LegacyOuts.IsEmpty())DSLFail(TEXT("Expected 1..64 interface methods and known legacy out names"));Model.Classes.Add(C);FString Wire,Issue;if(!Model.ToJson(Wire,Issue))DSLFail(Issue);Args.push_back(das::AnnotationArgument(DSLInterfaceKey,DSLUtf8(Wire)));return true;
        }catch(const std::exception& X){Error=X.what();return false;}}
    };
    const char* DSLTypesKey="_ue_types_json";
    void DSLTypeArgs(const das::AnnotationArgumentList& Args,TMap<FString,FString>& Meta){DSLAnnotationArguments(Args);for(const auto& A:Args){if(A.name=="module_id" || A.name=="id")continue;if(A.name=="BlueprintType"){if(!DSLBool(A))DSLFail(TEXT("BlueprintType must be enabled"));continue;}if(DSLIsMetadata(A.name))DSLMetadata(A,Meta);else DSLFail(TEXT("Unknown reflected type annotation"));}}
    struct FUEEnumAnnotation:das::EnumerationAnnotation {
        FUEEnumAnnotation():EnumerationAnnotation("ue_enum"){}
        virtual bool touch(const das::EnumerationPtr& E,das::ModuleGroup&,das::AnnotationArgumentList& Args,das::string& Error)override{try{
            if(const auto* Wire=Args.find(DSLTypesKey,das::Type::tString)){if(Wire->at.fileInfo)DSLFail(TEXT("Reserved type annotation argument"));return true;}
            FDasReflectedTypes M;M.Id=DSLString(Args,"module_id",FPaths::GetBaseFilename(DSLText(E->at.fileInfo->name)));FDasEnumDeclaration D;D.Name=DSLText(E->name);D.Id=DSLString(Args,"id",M.Id+TEXT(".")+D.Name);das::AnnotationArgumentList TypeArgs;for(const auto& A:Args)if(A.name.find("member_id_")!=0)TypeArgs.push_back(A);DSLTypeArgs(TypeArgs,D.Metadata);for(const auto& A:Args)if(A.name.find("member_id_")==0){bool Found=false;for(const auto& V:E->list)Found |= A.name=="member_id_"+V.name;if(!Found || A.type!=das::Type::tString)DSLFail(TEXT("Unknown member or invalid member ID annotation"));}
            for(const auto& V:E->list){if(!V.value || V.value->__rtti!=das::string("ExprConstInt"))DSLFail(TEXT("ue_enum requires explicit int literal values"));D.Entries.Add({DSLText(V.name),static_cast<das::ExprConstInt*>(V.value)->getValue(),DSLString(Args,("member_id_"+V.name).c_str(),D.Id+TEXT(".")+DSLText(V.name))});}M.Enums.Add(D);FString Wire,Issue;if(!M.ToJson(Wire,Issue))DSLFail(Issue);Args.push_back(das::AnnotationArgument(DSLTypesKey,DSLUtf8(Wire)));return true;
        }catch(const std::exception& X){Error=X.what();return false;}}
    };
    struct FUEStructAnnotation:das::StructureAnnotation {
        FUEStructAnnotation():StructureAnnotation("ue_struct"){}
        virtual bool look(const das::StructurePtr& St,das::ModuleGroup&,das::AnnotationArgumentList&,das::string& Error)override{try{DSLDeclarationKind(St->annotations);DSLAnnotation(St->annotations,"ue_struct");return true;}catch(const std::exception& X){Error=X.what();return false;}}
        virtual bool touch(const das::StructurePtr& S,das::ModuleGroup&,das::AnnotationArgumentList& Args,das::string& Error)override{try{
            if(const auto* Wire=Args.find(DSLTypesKey,das::Type::tString)){if(Wire->at.fileInfo)DSLFail(TEXT("Reserved type annotation argument"));return true;}
            if(S->isClass || S->parent)DSLFail(TEXT("ue_struct requires plain struct without inheritance"));FDasReflectedTypes M;M.Id=DSLString(Args,"module_id",FPaths::GetBaseFilename(DSLText(S->at.fileInfo->name)));FDasDeclaredClass D;D.Name=DSLText(S->name);D.Id=DSLString(Args,"id",M.Id+TEXT(".")+D.Name);DSLTypeArgs(Args,D.Metadata);
            for(const auto& F:S->fields){if(F.generated)continue;if(F.classMethod)DSLFail(TEXT("Reflected struct methods require separate support"));FDasDeclaredProperty P;P.Name=DSLText(F.name);P.Id=D.Id+TEXT(".")+P.Name;P.Type.Kind=DSLTypeName(F.type);if(DSLSet(F.type)){const FString K=DSLTypeName(F.type->firstType);if(F.init || (K!=TEXT("int") && K!=TEXT("string")))DSLFail(TEXT("Authored sets require int/string elements and empty defaults"));P.Type.Kind=TEXT("set");auto E=MakeShared<FDasDeclaredType>();E->Kind=K;P.Type.Element=E;}else if(F.type && F.type->baseType==das::Type::tTable){const FString Key=DSLTypeName(F.type->firstType),Value=DSLTypeName(F.type->secondType);if(F.init || (Key!=TEXT("int") && Key!=TEXT("string")) || (Value!=TEXT("int") && Value!=TEXT("string") && Value!=TEXT("float") && Value!=TEXT("double") && Value!=TEXT("bool") && Value!=TEXT("name")))DSLFail(TEXT("Authored maps require int/string keys, scalar/name values and empty defaults"));P.Type.Kind=TEXT("map");auto K=MakeShared<FDasDeclaredType>();K->Kind=Key;auto V=MakeShared<FDasDeclaredType>();V->Kind=Value;P.Type.Key=K;P.Type.Value=V;}if(F.type && F.type->baseType==das::Type::tArray){const FString K=DSLTypeName(F.type->firstType);if((!DSLDirectStruct(F.type->firstType) && K!=TEXT("int") && K!=TEXT("float") && K!=TEXT("double") && K!=TEXT("bool") && K!=TEXT("string") && K!=TEXT("name")) || F.init)DSLFail(TEXT("Authored arrays require supported scalar/ue_struct elements and empty defaults"));auto E=MakeShared<FDasDeclaredType>();E->Kind=K;if(DSLDirectStruct(F.type->firstType)){const auto* A=DSLAnnotation(F.type->firstType->structType->annotations,"ue_struct");const FString Module=DSLString(A->arguments,"module_id",FPaths::GetBaseFilename(DSLText(F.type->firstType->structType->at.fileInfo->name)));const FString Id=DSLString(A->arguments,"id",Module+TEXT(".")+DSLText(F.type->firstType->structType->name));E->TypePath=TEXT("/DasTypes/")+FMD5::HashAnsiString(*Id)+TEXT(".Type");}P.Type.Kind=TEXT("array");P.Type.Element=E;}if(DSLDirectStruct(F.type)){const auto* Child=DSLAnnotation(F.type->structType->annotations,"ue_struct");const FString Module=DSLString(Child->arguments,"module_id",FPaths::GetBaseFilename(DSLText(F.type->structType->at.fileInfo->name)));const FString Id=DSLString(Child->arguments,"id",Module+TEXT(".")+DSLText(F.type->structType->name));P.Type.TypePath=TEXT("/DasTypes/")+FMD5::HashAnsiString(*Id)+TEXT(".Type");if(F.init){bool Empty=false;if(F.init->rtti_isCall()){auto* Init=static_cast<das::ExprCall*>(F.init);Empty=Init->name==F.type->structType->name && Init->arguments.empty();}else if(F.init->rtti_isMakeStruct()){auto* Init=static_cast<das::ExprMakeStruct*>(F.init);Empty=Init->structs.empty() && !Init->block;}if(!Empty)DSLFail(TEXT("Nested ue_struct fields require an empty child constructor"));}}bool Marked=false;for(const auto& A:F.annotation){if(A.name=="ue_property"){Marked=true;if(A.type==das::Type::tString && A.sValue=="BlueprintReadOnly")P.bReadOnly=true;else if(A.type==das::Type::tString && (A.sValue=="EditAnywhere" || A.sValue=="BlueprintReadWrite")){}else if(A.type!=das::Type::tBool || !A.bValue)DSLFail(TEXT("Unsupported struct property annotation"));}else if(A.name=="ue_id")P.Id=DSLText(A.sValue);else if(DSLIsMetadata(A.name))DSLMetadata(A,P.Metadata);else DSLFail(TEXT("Unknown struct field annotation"));}if(!Marked || !F.type || F.type->ref)DSLFail(TEXT("Struct fields require ue_property and by-value scalar type"));
                if(F.init && P.Type.Kind!=TEXT("struct")){if(F.init->__rtti==das::string("ExprConstInt")){P.Default.Kind=TEXT("number");P.Default.Number=static_cast<das::ExprConstInt*>(F.init)->getValue();}else if(F.init->__rtti==das::string("ExprConstFloat")){P.Default.Kind=TEXT("number");P.Default.Number=static_cast<das::ExprConstFloat*>(F.init)->getValue();}else if(F.init->__rtti==das::string("ExprConstDouble")){P.Default.Kind=TEXT("number");P.Default.Number=static_cast<das::ExprConstDouble*>(F.init)->getValue();}else if(F.init->__rtti==das::string("ExprConstBool")){P.Default.Kind=TEXT("bool");P.Default.Boolean=static_cast<das::ExprConstBool*>(F.init)->getValue();}else if(F.init->__rtti==das::string("ExprConstString")){P.Default.Kind=TEXT("string");P.Default.String=DSLText(static_cast<das::ExprConstString*>(F.init)->text);}else DSLFail(TEXT("Struct defaults require scalar literals"));}D.Properties.Add(P);
            }M.Structs.Add(D);FString Wire,Issue;if(!M.ToJson(Wire,Issue,true))DSLFail(Issue);Args.push_back(das::AnnotationArgument(DSLTypesKey,DSLUtf8(Wire)));return true;
        }catch(const std::exception& X){Error=X.what();return false;}}
    };

    bool DSLValidateOwner(const das::ProgramPtr& Program,const FString& Entry,UObject* Self,FString& Wire,FString& Error)
    {
        bool OK=true;
        Program->getThisModule()->structures.foreach([&](const das::StructurePtr& St){
            const auto* A=DSLAnnotation(St->annotations,"ue_class");if(!A || !OK) return;
            const auto* J=A->arguments.find(DSLModelKey,das::Type::tString);if(!J) return;
            FDasAuthoringModule Model;if(!FDasAuthoringModule::FromJson(DSLText(J->sValue),Model,Error)){OK=false;return;}
            for(const auto& C:Model.Classes) if(C.Methods.ContainsByPredicate([&](const auto& M){return M.EntryPoint==Entry;}))
            {
                Wire=DSLText(J->sValue);
                if(!IsValid(Self)){Error=TEXT("DSL entry requires live self");OK=false;return;}
                const bool ActorAdapter=Self->IsA<ADasProbeActor>();
                for(const auto& P:C.Properties)
                {
                    FProperty* Actual=Self->GetClass()->FindPropertyByName(*P.Name);
                    if(!DSLPropertyMatches(Actual,P.Type.Kind) || P.bReadOnly!=Actual->HasAnyPropertyFlags(CPF_BlueprintReadOnly))
                    {Error=TEXT("DSL property differs from owner; apply/reimport Blueprint: ")+P.Name;OK=false;return;}
                    if(P.Type.Kind==TEXT("enum")){auto* E=CastField<FByteProperty>(Actual);auto* Type=ResolveDasEnum(P.Type.TypePath,Error);if(!Type || E->Enum!=Type || (ActorAdapter && E->GetPropertyValue_InContainer(Actual->GetOwnerClass()->GetDefaultObject())!=Type->GetValueByNameString(P.Default.String))){Error=TEXT("DSL enum identity/default differs from owner");OK=false;return;}continue;}
                    if(P.Type.Kind==TEXT("object")){auto* Object=CastField<FObjectProperty>(Actual);if(Object->PropertyClass->GetPathName()!=P.Type.TypePath || (ActorAdapter && Object->GetObjectPropertyValue_InContainer(Actual->GetOwnerClass()->GetDefaultObject()))){Error=TEXT("DSL object type/default differs from owner; apply/reimport Blueprint: ")+P.Name;OK=false;return;}continue;}
                    if(P.Type.Kind==TEXT("struct")){
                        auto* Struct=CastField<FStructProperty>(Actual);FInstancedStruct Default;
                        if(Struct->Struct->GetPathName()!=P.Type.TypePath || !ParseDasStructDefault(P.Type.TypePath,P.Default.String,Default,Error) || (ActorAdapter && !Struct->Struct->CompareScriptStruct(Struct->ContainerPtrToValuePtr<void>(Actual->GetOwnerClass()->GetDefaultObject()),Default.GetMemory(),0))){Error=TEXT("DSL struct type/default differs from owner; apply/reimport Blueprint: ")+P.Name;OK=false;return;}continue;
                    }
                    if(ActorAdapter)
                    {
                        UObject* CDO=Actual->GetOwnerClass()->GetDefaultObject();
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
                    auto Matches=[](FProperty* P,const FDasDeclaredType& T){if(auto* Carrier=DasContainerCarrier(T))return P && Carrier->FindPropertyByName(TEXT("Values"))->SameType(P);return DSLPropertyMatches(P,T.Kind);};
                    bool Fits=Method && Method->NumParms==M.Parameters.Num()+(M.ReturnType.Kind==TEXT("void")?0:1);
                    bool Directed=false;for(const auto& P:M.Parameters)Directed |= P.Direction!=TEXT("in");if(Fits && !Directed){int32 Index=0;for(TFieldIterator<FProperty> It(Method);It;++It) if(It->HasAnyPropertyFlags(CPF_Parm) && !It->HasAnyPropertyFlags(CPF_ReturnParm)){Fits &= M.Parameters.IsValidIndex(Index) && It->GetFName()==FName(*M.Parameters[Index].Name);++Index;}Fits &= Index==M.Parameters.Num();}
                    if(Fits) for(const auto& P:M.Parameters){auto* Actual=Method->FindPropertyByName(*P.Name);Fits &= Matches(Actual,P.Type) && (P.Direction==TEXT("out")?Actual->HasAnyPropertyFlags(CPF_OutParm) && !Actual->HasAnyPropertyFlags(CPF_ReferenceParm):P.Direction==TEXT("inout")?Actual->HasAllPropertyFlags(CPF_OutParm|CPF_ReferenceParm):(Actual->HasAnyPropertyFlags(CPF_ConstParm) || P.Type.Kind==TEXT("array") || !Actual->HasAnyPropertyFlags(CPF_OutParm|CPF_ReferenceParm)));if(Fits && P.Type.Kind==TEXT("enum"))Fits=CastField<FByteProperty>(Actual)->Enum->GetPathName()==P.Type.TypePath;if(Fits && P.Type.Kind==TEXT("struct"))Fits=CastField<FStructProperty>(Actual)->Struct->GetPathName()==P.Type.TypePath;if(Fits && P.Type.Kind==TEXT("object"))Fits=CastField<FObjectProperty>(Actual)->PropertyClass->GetPathName()==P.Type.TypePath;}
                    if(Fits) Fits=M.ReturnType.Kind==TEXT("void")?!Method->GetReturnProperty():Matches(Method->GetReturnProperty(),M.ReturnType);
                    if(Fits && M.ReturnType.Kind==TEXT("enum"))Fits=CastField<FByteProperty>(Method->GetReturnProperty())->Enum->GetPathName()==M.ReturnType.TypePath;if(Fits && M.ReturnType.Kind==TEXT("struct"))Fits=CastField<FStructProperty>(Method->GetReturnProperty())->Struct->GetPathName()==M.ReturnType.TypePath;
                    if(Fits && M.ReturnType.Kind==TEXT("object"))Fits=CastField<FObjectProperty>(Method->GetReturnProperty())->PropertyClass->GetPathName()==M.ReturnType.TypePath;
                    if(!Fits)
                    {Error=TEXT("DSL method differs from owner; apply/reimport Blueprint: ")+M.Name;OK=false;return;}
                }
            }
        });return OK;
    }
