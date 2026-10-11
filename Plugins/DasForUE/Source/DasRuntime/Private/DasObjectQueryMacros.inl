uint64 DSLNextQueryHelper=1;
struct FUEObjectQueryFactory:das::CallMacro {
 bool Component,All;
 FUEObjectQueryFactory(bool C,bool A):CallMacro(C?(A?"ue_component_find_all":"ue_component_find"):(A?"ue_actor_find_all":"ue_actor_find")),Component(C),All(A){}
 virtual bool canVisitArguments(das::ExprCallMacro*,int Index)override{return Index!=0;}
 virtual das::ExpressionPtr visit(das::Program* P,das::Module* M,das::ExprCallMacro* E)override{
  try{
   if(E->arguments.size()!=(Component?2:1) || !E->arguments[0]->rtti_isTypeDecl())DSLFail(TEXT("Expected query(type<Reference>[, actor])"));
   auto T=DSLObjectNormalize(static_cast<das::ExprTypeDecl*>(E->arguments[0])->typeexpr,*P->thisModuleGroup,M);
   if(!DSLTypedReference(T) || DSLTypedInterface(T))DSLFail(TEXT("Query requires native/authored object reference"));
   const FString Base=DSLTypedReferencePath(T);auto* Class=FindObject<UClass>(nullptr,*Base);
   if(!Class || !Class->IsChildOf(Component?UActorComponent::StaticClass():AActor::StaticClass()))DSLFail(TEXT("Query reference does not match Actor/component domain"));
   const FString Id=DSLAuthoredView(T)?DSLReferenceDeclaration(T).Id:FString();
   auto* Query=DSLCall(E->at,All?"__ue_query_begin":"__ue_find_by_type",{Component?static_cast<das::ExpressionPtr>(new das::ExprField(E->at,E->arguments[1],"handle")):static_cast<das::ExpressionPtr>(DSLCall(E->at,"ue_object_value",{new das::ExprConstString(E->at,"/Script/Engine.Actor")})),new das::ExprConstString(E->at,DSLUtf8(Base)),new das::ExprConstString(E->at,DSLUtf8(Id)),new das::ExprConstBool(E->at,Component)});
   auto Unpack=[&](das::ExpressionPtr H){return DSLAuthoredView(T)?DSLAuthoredUnpack(E->at,T,H):DSLDirectUnpack(E->at,T,H,Base);};
   if(!All)return Unpack(Query);
   auto* Fn=new das::Function();Fn->at=E->at;Fn->name="__ue_query_array_"+das::to_string(DSLNextQueryHelper++);Fn->generated=true;Fn->privateFunction=false;
   auto* Arg=new das::Variable();Arg->at=E->at;Arg->name="snapshot";Arg->type=new das::TypeDecl(das::Type::tUInt64,E->at);Arg->type->constant=true;Fn->arguments.push_back(Arg);
   auto* Array=new das::TypeDecl(das::Type::tArray,E->at);Array->firstType=DSLObjectCopyType(T);Array->firstType->ref=false;Array->firstType->constant=false;Fn->result=Array;
   auto* Body=new das::ExprBlock();Body->at=E->at;auto* V=new das::Variable();V->at=E->at;V->name="result";V->type=DSLObjectCopyType(Fn->result);
   auto* Empty=new das::ExprMakeStruct(E->at);Empty->makeType=DSLObjectCopyType(Fn->result);Empty->useInitializer=true;Empty->alwaysUseInitializer=true;V->init=Empty;
   auto* Let=new das::ExprLet();Let->at=E->at;Let->variables.push_back(V);Body->list.push_back(Let);
   auto* Loop=new das::ExprFor(E->at);Loop->iterators.push_back("index");Loop->iteratorsAka.push_back("");Loop->iteratorsAt.push_back(E->at);Loop->iteratorsTags.push_back(nullptr);Loop->iteratorsTupleExpansion.push_back(0);
   Loop->sources.push_back(DSLCall(E->at,"range",{new das::ExprConstInt(E->at,0),DSLCall(E->at,"__ue_query_count",{new das::ExprVar(E->at,"snapshot")})}));
   auto* Each=new das::ExprBlock();Each->at=E->at;Each->list.push_back(DSLCall(E->at,"push_clone",{new das::ExprVar(E->at,"result"),Unpack(DSLCall(E->at,"__ue_query_get",{new das::ExprVar(E->at,"snapshot"),new das::ExprVar(E->at,"index")}))}));Loop->body=Each;Body->list.push_back(Loop);
   Body->list.push_back(DSLCall(E->at,"__ue_query_release",{new das::ExprVar(E->at,"snapshot")}));auto* Return=new das::ExprReturn(E->at,new das::ExprVar(E->at,"result"));Return->moveSemantics=true;Body->list.push_back(Return);Fn->body=Body;
   if(!M->addFunction(Fn,true))DSLFail(TEXT("Query helper collision"));return DSLCall(E->at,Fn->name,{Query});
  }catch(const std::exception& X){P->error("Typed object query failed",X.what(),"",E->at,das::CompilationError::invalid_type);return nullptr;}
 }
};
