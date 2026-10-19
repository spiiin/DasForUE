// Canonical opaque source views, using the existing owned UEStructValue table.
bool DSLGameplay(const das::TypeDeclPtr& T){return T && T->baseType==das::Type::tStructure && T->structType && DSLAnnotation(T->structType->annotations,"ue_gameplay_value");}
bool DSLGameplayContainer(const das::TypeDeclPtr& T){return T->structType->name=="GameplayTagContainer";}
FString DSLGameplayPath(const das::TypeDeclPtr& T){return DSLGameplayContainer(T)?TEXT("/Script/GameplayTags.GameplayTagContainer"):TEXT("/Script/GameplayTags.GameplayTag");}
das::ExpressionPtr DSLGameplayPack(const das::LineInfo& At,const das::TypeDeclPtr& T,das::ExpressionPtr V){return DSLCall(At,"__ue_tag_check",{new das::ExprField(At,V,"value"),new das::ExprConstBool(At,DSLGameplayContainer(T))});}
das::ExpressionPtr DSLGameplayUnpack(const das::LineInfo& At,const das::TypeDeclPtr& T,das::ExpressionPtr V){auto* Make=new das::ExprMakeStruct(At);Make->makeType=DSLObjectCopyType(T);Make->makeType->ref=false;Make->makeType->constant=false;Make->useInitializer=true;Make->alwaysUseInitializer=true;auto* Fields=new das::MakeStruct();Fields->push_back(new das::MakeFieldDecl(At,"value",DSLCall(At,"__ue_tag_check",{V,new das::ExprConstBool(At,DSLGameplayContainer(T))}),false,false));Make->structs.push_back(Fields);return Make;}
struct FUEGameplayValueAnnotation:das::StructureAnnotation {
 FUEGameplayValueAnnotation():StructureAnnotation("ue_gameplay_value"){}
 virtual bool look(const das::StructurePtr&,das::ModuleGroup&,das::AnnotationArgumentList&,das::string&)override{return true;}
 virtual bool touch(const das::StructurePtr& S,das::ModuleGroup&,das::AnnotationArgumentList& A,das::string& Error)override{try{
  if(!S->module || S->module->name!="ue_gameplay_tags" || (S->name!="GameplayTag" && S->name!="GameplayTagContainer") || S->isClass || S->parent || !A.empty() || S->fields.size()!=1 || S->fields[0].name!="value" || S->fields[0].init || !S->fields[0].type || S->fields[0].type->ref || S->fields[0].type->baseType!=das::Type::tHandle || !S->fields[0].type->annotation || S->fields[0].type->annotation->name!="UEStructValue")DSLFail(TEXT("GameplayTag views must use the canonical ue_gameplay_tags declarations"));return true;
 }catch(const std::exception& X){Error=X.what();return false;}}
};
