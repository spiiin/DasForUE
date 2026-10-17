// The nominal source wrapper carries a checked, immutable native soft value.
bool DSLSoft(const das::TypeDeclPtr& T){return T && T->baseType==das::Type::tStructure && T->structType && DSLAnnotation(T->structType->annotations,"ue_soft_ref");}
const das::AnnotationDeclaration* DSLSoftAnnotation(const das::TypeDeclPtr& T){if(!DSLSoft(T))DSLFail(TEXT("Expected UESoftObject<T> or UESoftClass<T>"));return DSLAnnotation(T->structType->annotations,"ue_soft_ref");}
FString DSLSoftPath(const das::TypeDeclPtr& T){const auto* A=DSLSoftAnnotation(T);auto* Base=FindObject<UClass>(nullptr,*DSLString(A->arguments,"path"));FString Error;auto* Carrier=DasSoftCarrier(Base,DSLBool(*A->arguments.find("class_ref",das::Type::tBool)),Error);if(!Carrier)DSLFail(Error);return Carrier->GetPathName();}
das::ExpressionPtr DSLSoftPack(const das::LineInfo& At,const das::TypeDeclPtr& T,das::ExpressionPtr V){return DSLCall(At,"__ue_soft_check",{new das::ExprField(At,V,"value"),new das::ExprConstString(At,DSLUtf8(DSLSoftPath(T)))});}
das::ExpressionPtr DSLSoftUnpack(const das::LineInfo& At,const das::TypeDeclPtr& T,das::ExpressionPtr V){auto* Make=new das::ExprMakeStruct(At);Make->makeType=DSLObjectCopyType(T);Make->makeType->ref=false;Make->makeType->constant=false;Make->useInitializer=true;Make->alwaysUseInitializer=true;auto* Fields=new das::MakeStruct();Fields->push_back(new das::MakeFieldDecl(At,"value",DSLCall(At,"__ue_soft_check",{V,new das::ExprConstString(At,DSLUtf8(DSLSoftPath(T)))}),false,false));Make->structs.push_back(Fields);return Make;}
int DSLSoftUpcastDistance(const das::TypeDeclPtr& From,const das::TypeDeclPtr& To){
 if(!DSLSoft(From) || !DSLSoft(To))return -1;const auto* A=DSLSoftAnnotation(From);const auto* B=DSLSoftAnnotation(To);
 if(DSLBool(*A->arguments.find("class_ref",das::Type::tBool))!=DSLBool(*B->arguments.find("class_ref",das::Type::tBool)))return -1;
 auto* Child=FindObject<UClass>(nullptr,*DSLString(A->arguments,"path"));auto* Parent=FindObject<UClass>(nullptr,*DSLString(B->arguments,"path"));
 if(!Child || !Parent || !Child->IsChildOf(Parent))return -1;int Distance=0;for(auto* C=Child;C!=Parent;C=C->GetSuperClass())++Distance;return Distance;
}
das::ExpressionPtr DSLSoftUpcast(const das::LineInfo& At,const das::TypeDeclPtr& From,const das::TypeDeclPtr& To,das::ExpressionPtr Value){
 if(DSLSoftUpcastDistance(From,To)<0)DSLFail(TEXT("Soft references can only convert to a parent of the same object/class kind"));
 return DSLSoftUnpack(At,To,DSLCall(At,"__ue_soft_upcast",{DSLSoftPack(At,From,Value),new das::ExprConstString(At,DSLUtf8(DSLSoftPath(From))),new das::ExprConstString(At,DSLUtf8(DSLSoftPath(To)))}));
}
struct FUESoftUpcastMacro:das::CallMacro {
 FUESoftUpcastMacro():CallMacro("ue_soft_upcast"){}
 virtual bool canVisitArguments(das::ExprCallMacro*,int I)override{return I!=0;}
 virtual das::ExpressionPtr visit(das::Program* P,das::Module* M,das::ExprCallMacro* E)override{try{
  if(E->arguments.size()!=2 || !E->arguments[0]->rtti_isTypeDecl())DSLFail(TEXT("Expected ue_soft_upcast(type<ParentSoftType>, value)"));
  auto Target=DSLObjectNormalize(static_cast<das::ExprTypeDecl*>(E->arguments[0])->typeexpr,*P->thisModuleGroup,M);
  return DSLSoftUpcast(E->at,E->arguments[1]->type,Target,E->arguments[1]);
 }catch(const std::exception& X){P->error("Soft upcast failed",X.what(),"",E->at,das::CompilationError::invalid_type);return nullptr;}}
};
struct FUESoftAnnotation:das::StructureAnnotation {
 FUESoftAnnotation():StructureAnnotation("ue_soft_ref"){}
 virtual bool look(const das::StructurePtr&,das::ModuleGroup&,das::AnnotationArgumentList&,das::string&)override{return true;}
 virtual bool touch(const das::StructurePtr& S,das::ModuleGroup&,das::AnnotationArgumentList& A,das::string& Error)override{try{
  if(S->isClass || S->parent || S->fields.size()!=1 || S->fields[0].name!="value" || !S->fields[0].type || S->fields[0].type->ref || S->fields[0].init || S->fields[0].type->baseType!=das::Type::tHandle || !S->fields[0].type->annotation || S->fields[0].type->annotation->name!="UEStructValue" || A.size()!=3 || !A.find("path",das::Type::tString) || !A.find("class_ref",das::Type::tBool) || !A.find("target",das::Type::tString))DSLFail(TEXT("Invalid typed soft wrapper"));
  auto* T=new das::TypeDecl(das::Type::tStructure,S->at);T->structType=S;DSLSoftPath(T);return true;
 }catch(const std::exception& X){Error=X.what();return false;}}
};
struct FUESoftTypeMacro:das::TypeMacro {
 bool ClassReference;
 explicit FUESoftTypeMacro(bool C):TypeMacro(C?"UESoftClass":"UESoftObject"),ClassReference(C){}
 virtual das::TypeDeclPtr visit(das::Program* P,das::Module* M,const das::TypeDeclPtr& T,const das::TypeDeclPtr& Pass)override{try{
  if(T->typeMacroExpr.size()!=2 || !T->typeMacroExpr[1]->rtti_isTypeDecl())DSLFail(TEXT("Soft type requires one generated native object type"));
  auto Base=DSLObjectNormalize(static_cast<das::ExprTypeDecl*>(T->typeMacroExpr[1])->typeexpr,*P->thisModuleGroup,M);if(!DSLTypedObject(Base))DSLFail(TEXT("Soft type requires a native object wrapper"));const auto Path=DSLTypedObjectPath(Base);FString Error;if(!DasSoftCarrier(FindObject<UClass>(nullptr,*Path),ClassReference,Error))DSLFail(Error);
  das::TypeDeclPtr Target=Base;if(ClassReference){FUEClassTypeMacro Strong;Target=Strong.visit(P,M,T,nullptr);if(!Target)return nullptr;}
  const auto Name=DSLUtf8((ClassReference?TEXT("__ue_soft_class_"):TEXT("__ue_soft_object_"))+FMD5::HashAnsiString(*Path));auto Found=P->thisModuleGroup->findStructure(Name,M);if(auto Local=M->findStructure(Name)){bool Has=false;for(auto& S:Found)Has |= S==Local;if(!Has)Found.push_back(Local);}if(Found.size()>1)DSLFail(TEXT("Ambiguous soft type; import a shared alias module"));das::StructurePtr S=Found.empty()?nullptr:Found[0];
  if(!S){S=new das::Structure();S->name=Name;S->module=M;S->at=T->at;das::Structure::FieldDeclaration F;F.flags=0;F.name="value";F.at=T->at;auto Handles=P->thisModuleGroup->findAnnotation("UEStructValue",M);if(Handles.size()!=1)DSLFail(TEXT("Missing soft value transport"));F.type=new das::TypeDecl(das::Type::tHandle,T->at);F.type->annotation=static_cast<das::TypeAnnotation*>(Handles[0]);S->fields.push_back(F);
   auto Marks=P->thisModuleGroup->findAnnotation("ue_soft_ref",M);if(Marks.size()!=1)DSLFail(TEXT("Missing soft type annotation"));auto* A=new das::AnnotationDeclaration();A->flags=0;A->annotation=Marks[0];A->at=T->at;A->arguments.push_back(das::AnnotationArgument("path",DSLUtf8(Path)));A->arguments.push_back(das::AnnotationArgument("class_ref",ClassReference));A->arguments.push_back(das::AnnotationArgument("target",Target->structType->module->name+"::"+Target->structType->name));S->annotations.push_back(A);if(!M->addStructure(S,true))DSLFail(TEXT("Soft type name collision"));
  }
  auto* Result=new das::TypeDecl(das::Type::tStructure,T->at);Result->structType=S;if(!DSLSoft(Result) || DSLSoftPath(Result)!=(DasSoftCarrier(FindObject<UClass>(nullptr,*Path),ClassReference,Error)->GetPathName()))DSLFail(TEXT("Soft type collision"));das::TypeDecl::applyAutoContracts(Result,T);if(Pass && (Pass->baseType!=das::Type::tStructure || Pass->structType!=S))return nullptr;return Result;
 }catch(const std::exception& X){P->error("Soft type macro failed",X.what(),"",T->at,das::CompilationError::invalid_type);return nullptr;}}
};
struct FUESoftCallMacro:das::CallMacro {
 int Operation;
 FUESoftCallMacro(const char* N,int Op):CallMacro(N),Operation(Op){}
 virtual bool canVisitArguments(das::ExprCallMacro*,int I)override{return Operation!=0 || I!=0;}
 virtual das::ExpressionPtr visit(das::Program* P,das::Module* M,das::ExprCallMacro* E)override{try{
  if(Operation==0){if(E->arguments.size()!=2 || !E->arguments[0]->rtti_isTypeDecl())DSLFail(TEXT("Expected ue_soft(type<SoftType>, path)"));auto T=DSLObjectNormalize(static_cast<das::ExprTypeDecl*>(E->arguments[0])->typeexpr,*P->thisModuleGroup,M);return DSLSoftUnpack(E->at,T,DSLCall(E->at,"__ue_soft_make",{new das::ExprConstString(E->at,DSLUtf8(DSLSoftPath(T))),E->arguments[1]}));}
  if(E->arguments.size()!=1 || !DSLSoft(E->arguments[0]->type))DSLFail(TEXT("Expected one typed soft reference"));const auto T=E->arguments[0]->type;
  if(Operation==1)return DSLSoftPack(E->at,T,E->arguments[0]);
  const auto* A=DSLSoftAnnotation(T);const auto Target=DSLUtf8(DSLString(A->arguments,"target"));das::StructurePtr Found=nullptr;for(auto* Module:P->thisModuleGroup->getModules())Module->structures.foreach([&](const das::StructurePtr& S){if(S->module->name+"::"+S->name==Target)Found=S;});if(!Found)M->structures.foreach([&](const das::StructurePtr& S){if(S->module->name+"::"+S->name==Target)Found=S;});if(!Found)DSLFail(TEXT("Missing canonical soft result type"));auto* Out=new das::TypeDecl(das::Type::tStructure,E->at);Out->structType=Found;const bool Class=DSLBool(*A->arguments.find("class_ref",das::Type::tBool));
  das::ExpressionPtr Value=DSLCall(E->at,Class?"__ue_soft_get_class":"__ue_soft_get_object",{DSLSoftPack(E->at,T,E->arguments[0]),new das::ExprConstBool(E->at,Operation==3)});
  return Class?DSLClassUnpack(E->at,Out,Value):DSLCall(E->at,Found->module->name+"::"+Found->name+"_from",{Value});
 }catch(const std::exception& X){P->error("Soft reference expression failed",X.what(),"",E->at,das::CompilationError::invalid_type);return nullptr;}}
};
