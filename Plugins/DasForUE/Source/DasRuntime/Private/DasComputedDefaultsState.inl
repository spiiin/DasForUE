// Per-compilation values; never retained by a runtime object or stored in an asset.
struct FDSLComputedDefault {FString Key;das::FunctionPtr Function;};
struct FDSLComputedDefaults {
 das::gc_root SavedExpressions;
 bool Resolved=false;
 TArray<FDSLComputedDefault> Pending;
 TSet<das::ExpressionPtr> PendingReferences,PendingNativeExpressions;
 TMap<FString,das::ExpressionPtr> Values,SourceFields;
};
FDSLComputedDefaults* DSLComputedDefaults=nullptr;
FString DSLNativeDefaultsParentHint;
void DSLEvaluateComputedDefaults(const das::ProgramPtr&,das::TextWriter&);

// daScript gen2 lowers array/table literals to these constructors before infer.
das::ExprMakeArray* DSLDefaultItems(das::ExpressionPtr E,const das::TypeDeclPtr& Type){
 if(E && E->rtti_isMakeStruct()){auto* M=static_cast<das::ExprMakeStruct*>(E);if(M->structs.empty() && !M->block && M->makeType && M->makeType->baseType==Type->baseType)return nullptr;}
 auto* Call=E && E->rtti_isCall()?static_cast<das::ExprCall*>(E):nullptr;const das::string Name=Type->baseType==das::Type::tArray?"to_array_move":"to_table_move";
 if(!Call || Call->name!=Name || Call->arguments.size()!=1 || Call->arguments[0]->__rtti!=das::string("ExprMakeArray"))DSLFail(TEXT("Container defaults require array/table literals, not container-returning helpers"));
 auto* Items=static_cast<das::ExprMakeArray*>(Call->arguments[0]);if(Items->values.size()>4096)DSLFail(TEXT("Container default exceeds element limit"));return Items;
}
