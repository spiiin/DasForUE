// Value kinds with their property class and daScript self/reference types, for the Python generators.
// Foundation tests compare it with Bindings/Kinds.json.
FString DasKindManifest()
{
    TMap<FString, FString> Results;
    if (das::Module* Module = das::Module::require("ue_foundation"))
        Module->functions.foreach (
            [&](const das::FunctionPtr& Fn)
            {
                if (!Fn->result)
                    return;
                FString Type = UTF8_TO_TCHAR(Fn->result->describe().c_str());
                Type.RemoveFromEnd(TEXT(" const"));
                Type.RemoveFromStart(TEXT("ue_foundation::"));
                Results.Add(UTF8_TO_TCHAR(Fn->name.c_str()), Type);
            });
    TArray<FString> Rows;
    auto Row = [&](const TCHAR* Name, const FString& Property, bool bStructLeaf)
    {
        Rows.Add(FString::Printf(
            TEXT("    {\"name\": \"%s\", \"property\": \"%s\", \"self\": \"%s\", \"reference\": \"%s\", ")
                TEXT("\"struct_leaf\": %s}"), Name, *Property,
                     *Results.FindRef(FString(TEXT("ue_get_self_")) + Name),
                                      *Results.FindRef(FString(TEXT("__ue_authored_get_")) + Name),
                                                       bStructLeaf ? TEXT("true") : TEXT("false")));
    };
#define DAS_KIND_MANIFEST_SCALAR(Suffix, SelfKind, RefKind) Row(TEXT(#Suffix), DasKindPropertyName<SelfKind>(), true);
#define DAS_KIND_MANIFEST_REFERENCE(Suffix, SelfKind, RefKind)                                                         \
    Row(TEXT(#Suffix), DasKindPropertyName<SelfKind>(), false);
    DAS_SCALAR_ACCESSOR_KINDS(DAS_KIND_MANIFEST_SCALAR)
    DAS_REFERENCE_ACCESSOR_KINDS(DAS_KIND_MANIFEST_REFERENCE)
#undef DAS_KIND_MANIFEST_SCALAR
#undef DAS_KIND_MANIFEST_REFERENCE
    return TEXT("{\n  \"kinds\": [\n") + FString::Join(Rows, TEXT(",\n")) + TEXT("\n  ]\n}\n");
}

// Informational: cost of the guard every UE extern call runs (foundation benchmark breakdown).
void DasSetSelfSitesEnabled(bool bEnabled)
{
    GDasSelfSitesEnabled = bEnabled;
}
double DasExternGuardBenchmarkNs(int32 Iterations)
{
    const double Start = FPlatformTime::Seconds();
    for (int32 I = 0; I < Iterations; ++I)
        GuardUEExtern();
    return (FPlatformTime::Seconds() - Start) * 1e9 / FMath::Max(1, Iterations);
}
