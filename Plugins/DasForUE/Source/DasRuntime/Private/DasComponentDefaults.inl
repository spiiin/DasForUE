
struct FDSLComponentDefault {FString Component,Property;FDasDeclaredLiteral Value;};
double DSLDefaultNumber(das::ExpressionPtr E){
 double V=0;
 if(E->__rtti==das::string("ExprConstInt"))V=static_cast<das::ExprConstInt*>(E)->getValue();
 else if(E->__rtti==das::string("ExprConstFloat"))V=static_cast<das::ExprConstFloat*>(E)->getValue();
 else if(E->__rtti==das::string("ExprConstDouble"))V=static_cast<das::ExprConstDouble*>(E)->getValue();
 else DSLFail(TEXT("Component defaults require numeric literals"));
 if(!FMath::IsFinite(V))DSLFail(TEXT("Component defaults require finite numbers"));return V;
}
TArray<FDSLComponentDefault> DSLCollectComponentDefaults(const das::StructurePtr& S){
 TArray<FDSLComponentDefault> Result;das::vector<das::Structure::FieldDeclaration> Fields;
 for(const auto& F:S->fields){
  auto* Call=F.init && F.init->rtti_isCall()?static_cast<das::ExprCall*>(F.init):nullptr;
  if(!Call || (Call->name!="ue_defaults" && Call->name!="ue_dsl::ue_defaults")){Fields.push_back(F);continue;}
  if(!F.annotation.empty() || Call->arguments.size()!=1 || !Call->arguments[0]->rtti_isMakeBlock())DSLFail(TEXT("ue_defaults requires a plain trailing block"));
  auto* Closure=static_cast<das::ExprMakeBlock*>(Call->arguments[0]);
  auto* Block=Closure->block && Closure->block->rtti_isBlock()?static_cast<das::ExprBlock*>(Closure->block):nullptr;
  if(!Block || !Block->arguments.empty() || !Block->finalList.empty() || Block->list.empty())DSLFail(TEXT("ue_defaults requires nonempty assignments"));
  for(auto* E:Block->list){
   if(!E->rtti_isCopy())DSLFail(TEXT("ue_defaults accepts only component property assignments"));
   auto* Assignment=static_cast<das::ExprCopy*>(E);
   auto* Property=Assignment->left->rtti_isField()?static_cast<das::ExprField*>(Assignment->left):nullptr;
   auto* Component=Property && Property->value->rtti_isField()?static_cast<das::ExprField*>(Property->value):nullptr;
   if(!Component || !Component->value->rtti_isVar() || static_cast<das::ExprVar*>(Component->value)->name!="self")DSLFail(TEXT("Expected self.Component.Property in ue_defaults"));
   FDSLComponentDefault D;D.Component=DSLText(Component->name);D.Property=DSLText(Property->name);auto* V=Assignment->right;
   if(V->__rtti==das::string("ExprConstBool")){D.Value.Kind=TEXT("bool");D.Value.Boolean=static_cast<das::ExprConstBool*>(V)->getValue();}
   else if(V->__rtti==das::string("ExprConstString")){D.Value.Kind=TEXT("string");D.Value.String=DSLText(static_cast<das::ExprConstString*>(V)->text);}
   else if(V->rtti_isCall()){
    auto* Math=static_cast<das::ExprCall*>(V);if((Math->name!="ue_vector" && Math->name!="ue_rotator") || Math->arguments.size()!=3)DSLFail(TEXT("Only ue_vector/ue_rotator literal constructors are supported in defaults"));
    const double X=DSLDefaultNumber(Math->arguments[0]),Y=DSLDefaultNumber(Math->arguments[1]),Z=DSLDefaultNumber(Math->arguments[2]);
    D.Value.Kind=Math->name=="ue_vector"?TEXT("vector"):TEXT("rotator");D.Value.String=FString::Printf(TEXT("X=%.17g Y=%.17g Z=%.17g"),X,Y,Z);
   }else{D.Value.Kind=TEXT("number");D.Value.Number=DSLDefaultNumber(V);}
   Result.Add(MoveTemp(D));
  }
 }
 S->fields=std::move(Fields);return Result;
}
void DSLApplyComponentDefaults(FDasDeclaredClass& C,const TArray<FDSLComponentDefault>& Defaults){
 for(const auto& D:Defaults){auto* Component=C.Components.FindByPredicate([&](const auto& V){return V.Name==D.Component;});
  if(!Component)DSLFail(TEXT("Unknown defaults component: ")+D.Component);

  if(Component->Defaults.Contains(D.Property))DSLFail(TEXT("Duplicate component default: ")+D.Component+TEXT(".")+D.Property);
  Component->Defaults.Add(D.Property,D.Value);
 }
}
