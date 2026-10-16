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
