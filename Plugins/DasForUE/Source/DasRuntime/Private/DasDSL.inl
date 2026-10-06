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
    {for(auto* A:L) if(A->annotation->name==N) return A;return nullptr;}
    FString DSLTypeName(const das::TypeDeclPtr& T){if(!T)return {};switch(T->baseType){case das::Type::tInt:return TEXT("int");case das::Type::tFloat:return TEXT("float");case das::Type::tBool:return TEXT("bool");case das::Type::tDouble:return TEXT("double");case das::Type::tString:return TEXT("string");case das::Type::tVoid:return TEXT("void");case das::Type::tHandle:if(T->annotation && T->annotation->name=="UENameValue")return TEXT("name");default:break;}return {};}
    bool DSLPropertyMatches(FProperty* P,const FString& K){return K==TEXT("int")?CastField<FIntProperty>(P)!=nullptr:K==TEXT("float")?CastField<FFloatProperty>(P)!=nullptr:K==TEXT("bool")?CastField<FBoolProperty>(P)!=nullptr:K==TEXT("double")?CastField<FDoubleProperty>(P)!=nullptr:K==TEXT("string")?CastField<FStrProperty>(P)!=nullptr:CastField<FNameProperty>(P)!=nullptr;}
    struct FDSLBodyVisitor:das::Visitor
    {
        const FDasDeclaredClass& Decl;das::string Self;TSet<const das::Expression*> LeftFields;FString Error;
        FDSLBodyVisitor(const FDasDeclaredClass& C,const das::string& S):Decl(C),Self(S){}
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
            auto* C=new das::ExprCall(F->at,P->Type.Kind==TEXT("int")?"ue_get_self_int":P->Type.Kind==TEXT("float")?"ue_get_self_float":P->Type.Kind==TEXT("bool")?"ue_get_self_bool":P->Type.Kind==TEXT("double")?"ue_get_self_double":P->Type.Kind==TEXT("string")?"ue_get_self_string":"ue_get_self_name");
            C->arguments.push_back(new das::ExprConstString(F->at,DSLUtf8(P->Name)));return C;
        }
        das::ExpressionPtr Write(das::ExprOp2* E,das::ExprField* F,bool Compound)
        {
            const auto* P=Property(F);if(!P) return E;
            if(P->bReadOnly){Error=TEXT("Cannot write BlueprintReadOnly self property: ")+P->Name;return E;}
            auto* C=new das::ExprCall(E->at,P->Type.Kind==TEXT("int")?"ue_set_self_int":P->Type.Kind==TEXT("float")?"ue_set_self_float":P->Type.Kind==TEXT("bool")?"ue_set_self_bool":P->Type.Kind==TEXT("double")?"ue_set_self_double":P->Type.Kind==TEXT("string")?"ue_set_self_string":"ue_set_self_name");
            C->arguments.push_back(new das::ExprConstString(E->at,DSLUtf8(P->Name)));
            C->arguments.push_back(Compound?new das::ExprOp2(E->at,E->op.substr(0,E->op.size()-1),Read(F),E->right):E->right);return C;
        }
        virtual void preVisit(das::ExprCopy* E) override {if(SelfField(E->left)) LeftFields.Add(E->left);}
        virtual void preVisit(das::ExprOp2* E) override {if((E->op=="+=" || E->op=="-=" || E->op=="*=" || E->op=="/=") && SelfField(E->left)) LeftFields.Add(E->left);}
        virtual das::ExpressionPtr visit(das::ExprField* E) override {return SelfField(E) && !LeftFields.Contains(E)?Read(E):E;}
        virtual das::ExpressionPtr visit(das::ExprCopy* E) override {auto* F=SelfField(E->left);return F?Write(E,F,false):E;}
        virtual das::ExpressionPtr visit(das::ExprOp2* E) override {auto* F=SelfField(E->left);return F && LeftFields.Contains(F)?Write(E,F,true):E;}
        virtual das::ExpressionPtr visit(das::ExprWith* E) override {return E->with && E->with->rtti_isVar() && static_cast<das::ExprVar*>(E->with)->name==Self?E->body:E;}
    };
    struct FUEFunctionAnnotation:das::FunctionAnnotation
    {
        FUEFunctionAnnotation():FunctionAnnotation("ue_function"){}
        virtual bool apply(const das::FunctionPtr& F,das::ModuleGroup&,das::AnnotationArgumentList&,das::string& E) override {if(!F->classParent){E="ue_function must be a member of ue_class";return false;}return true;}
        virtual bool finalize(const das::FunctionPtr&,das::ModuleGroup&,das::AnnotationArgumentList&,const das::AnnotationArgumentList&,das::string&) override {return true;}
        virtual bool apply(das::ExprBlock*,das::ModuleGroup&,das::AnnotationArgumentList&,das::string& E) override {E="ue_function cannot annotate block";return false;}
        virtual bool finalize(das::ExprBlock*,das::ModuleGroup&,das::AnnotationArgumentList&,const das::AnnotationArgumentList&,das::string&) override {return false;}
    };
    struct FUEClassAnnotation:das::StructureAnnotation
    {
        FUEClassAnnotation():StructureAnnotation("ue_class"){}
        virtual bool look(const das::StructurePtr&,das::ModuleGroup&,das::AnnotationArgumentList&,das::string&) override {return true;}
        virtual bool touch(const das::StructurePtr& St,das::ModuleGroup&,das::AnnotationArgumentList& Args,das::string& Error) override
        {
            try
            {
                if(const auto* Wire=Args.find(DSLModelKey,das::Type::tString)){if(Wire->at.fileInfo) DSLFail(TEXT("Reserved internal annotation argument"));return true;}
                FDasAuthoringModule Model;const FString Script=FPaths::GetCleanFilename(DSLText(St->at.fileInfo->name));
                Model.Id=DSLString(Args,"module_id",FPaths::GetBaseFilename(Script));
                FDasDeclaredClass C;C.Name=DSLText(St->name);C.Id=DSLString(Args,"id",Model.Id+TEXT(".")+C.Name);C.NativeParent=DSLString(Args,"parent",TEXT("/Script/Engine.Actor"));
                for(const auto& A:Args)
                {
                    if(A.name=="module_id" || A.name=="id" || A.name=="parent") continue;
                    if(A.name=="Blueprintable"){if(!DSLBool(A)) DSLFail(TEXT("Adapter requires Blueprintable"));continue;}
                    if(DSLIsMetadata(A.name)) DSLMetadata(A,C.Metadata);else DSLFail(TEXT("Unknown ue_class annotation: ")+DSLText(A.name));
                }
                if(St->parent) DSLFail(TEXT("Use ue_class parent identity; native daScript inheritance is not supported yet"));
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
                    const auto* A=DSLAnnotation(F->annotations,"ue_function");FDasDeclaredMethod M;
                    M.Name=DSLText(F->name.substr(F->name.find_last_of(das::string(1,char(96))+das::string(1,char(39)))+1));M.Id=C.Id+TEXT(".")+M.Name;M.ScriptFile=Script;M.EntryPoint=TEXT("ue_")+C.Name+TEXT("_")+M.Name;M.bCallable=false;
                    for(const auto& V:A->arguments)
                    {
                        if(V.name=="id"){M.Id=DSLString(A->arguments,"id");continue;}
                        if(V.name=="BlueprintCallable") M.bCallable=DSLBool(V);
                        else if(DSLIsMetadata(V.name)) DSLMetadata(V,M.Metadata);else DSLFail(TEXT("Unsupported ue_function specifier: ")+DSLText(V.name));
                    }
                    const FString ResultType=DSLTypeName(F->result);
                    if(!M.bCallable || F->isStaticClassMethod || !F->result || F->result->ref || ResultType.IsEmpty() || F->arguments.empty() || F->arguments.size()>17) DSLFail(TEXT("Adapter requires BlueprintCallable instance scalar/void method with up to 16 by-value inputs: ")+M.Name);
                    M.ReturnType.Kind=ResultType;
                    const bool LegacySingle=F->arguments.size()==2 && (ResultType==TEXT("int") || ResultType==TEXT("float") || ResultType==TEXT("bool")) && DSLTypeName(F->arguments[1]->type)==ResultType;
                    for(size_t I=1;I<F->arguments.size();++I){const auto& V=F->arguments[I];const FString K=DSLTypeName(V->type);if(!V->type || V->type->ref || K.IsEmpty() || K==TEXT("void") || V->init) DSLFail(TEXT("Unsupported parameter type/ref/default: ")+DSLText(V->name));FDasDeclaredParameter P;P.Name=LegacySingle?TEXT("Input"):DSLText(V->name);P.Id=M.Id+TEXT(".")+(LegacySingle?TEXT("input"):P.Name);P.Type.Kind=K;M.Parameters.Add(P);}
                    auto Clone=F->clone();Clone->name=DSLUtf8(M.EntryPoint);Clone->classParent=nullptr;Clone->annotations.clear();Clone->exports=true;Clone->generated=true;Clone->privateFunction=false;Clone->isStaticClassMethod=false;Clone->isConstClassMethod=false;
                    FDSLBodyVisitor Rewrite(C,Clone->arguments[0]->name);Clone->body=Clone->body->visit(Rewrite);Clone->arguments.erase(Clone->arguments.begin());
                    if(!Rewrite.Error.IsEmpty()) DSLFail(Rewrite.Error);
                    Generated.push_back(Clone);C.Methods.Add(MoveTemp(M));
                }
                Model.Classes.Add(C);FString Json,Issue;
                if(!Model.ToJson(Json,Issue)) DSLFail(Issue);
                FDasClassSchema Supported;if(!Model.ToActorSchema(C.Id,Supported,Issue)) DSLFail(Issue);
                for(const auto& F:Generated) if(!St->module->addFunction(F,true)) DSLFail(TEXT("Generated entry point collision"));
                Args.push_back(das::AnnotationArgument(DSLModelKey,DSLUtf8(Json)));return true;
            }
            catch(const std::exception& E){Error=E.what();return false;}
        }
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
                    if(ActorAdapter)
                    {
                        UObject* CDO=Self->GetClass()->GetDefaultObject();
                        const bool DefaultMatches=P.Type.Kind==TEXT("int")
                            ? CastField<FIntProperty>(Actual)->GetPropertyValue_InContainer(CDO)==static_cast<int32>(P.Default.Number)
                            : P.Type.Kind==TEXT("float")?CastField<FFloatProperty>(Actual)->GetPropertyValue_InContainer(CDO)==static_cast<float>(P.Default.Number):P.Type.Kind==TEXT("bool")?CastField<FBoolProperty>(Actual)->GetPropertyValue_InContainer(CDO)==P.Default.Boolean:P.Type.Kind==TEXT("double")?CastField<FDoubleProperty>(Actual)->GetPropertyValue_InContainer(CDO)==P.Default.Number:P.Type.Kind==TEXT("string")?CastField<FStrProperty>(Actual)->GetPropertyValue_InContainer(CDO)==P.Default.String:CastField<FNameProperty>(Actual)->GetPropertyValue_InContainer(CDO)==FName(*P.Default.String);
                        if(!DefaultMatches){Error=TEXT("DSL default differs from owner; apply/reimport Blueprint: ")+P.Name;OK=false;return;}
                    }
                }
                if(ActorAdapter) for(const auto& M:C.Methods)
                {
                    UFunction* Method=Self->FindFunction(*M.Name);
                    auto Matches=[](FProperty* P,const FString& K){return DSLPropertyMatches(P,K);};
                    bool Fits=Method && Method->NumParms==M.Parameters.Num()+(M.ReturnType.Kind==TEXT("void")?0:1);
                    if(Fits){int32 Index=0;for(TFieldIterator<FProperty> It(Method);It;++It) if(It->HasAnyPropertyFlags(CPF_Parm) && !It->HasAnyPropertyFlags(CPF_ReturnParm)){Fits &= M.Parameters.IsValidIndex(Index) && It->GetFName()==FName(*M.Parameters[Index].Name);++Index;}Fits &= Index==M.Parameters.Num();}
                    if(Fits) for(const auto& P:M.Parameters){auto* Actual=Method->FindPropertyByName(*P.Name);Fits &= Matches(Actual,P.Type.Kind) && !Actual->HasAnyPropertyFlags(CPF_OutParm|CPF_ReferenceParm);}
                    if(Fits) Fits=M.ReturnType.Kind==TEXT("void")?!Method->GetReturnProperty():Matches(Method->GetReturnProperty(),M.ReturnType.Kind);
                    if(!Fits)
                    {Error=TEXT("DSL method differs from owner; apply/reimport Blueprint: ")+M.Name;OK=false;return;}
                }
            }
        });return OK;
    }
