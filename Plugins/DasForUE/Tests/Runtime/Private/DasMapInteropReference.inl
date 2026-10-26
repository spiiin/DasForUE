// Test-only reference for measuring the previous typed int-key/int-value path.
int32 MapProbeTypedGet(const char* Name, int32 Key, das::Context* C)
{
    UObject* Self = nullptr;
    auto* P = SelfContainer<FMapProperty>(Self, Name, false, C);
    SelfContainerElement<FDasKindInt>(P->KeyProp, C);
    auto* V = SelfContainerElement<FDasKindInt>(P->ValueProp, C);
    FScriptMapHelper Map(P, P->ContainerPtrToValuePtr<void>(Self));
    const uint8* Found = Map.FindValueFromHash(&Key);
    return Found ? V->GetPropertyValue(Found) : 0;
}
void MapProbeTypedSet(const char* Name, int32 Key, int32 Value, das::Context* C)
{
    UObject* Self = nullptr;
    auto* P = SelfContainer<FMapProperty>(Self, Name, true, C);
    SelfContainerElement<FDasKindInt>(P->KeyProp, C);
    SelfContainerElement<FDasKindInt>(P->ValueProp, C);
    FScriptMapHelper Map(P, P->ContainerPtrToValuePtr<void>(Self));
    if (!Map.FindValueFromHash(&Key))
        SelfContainerGrow(Map.Num(), false, C);
    Map.AddPair(&Key, &Value);
}

int32 MapProbeBindingCount()
{
    int32 MapBindings = 0;
    bool Legacy = false;
    das::Module::require("ue_foundation")
        ->functions.foreach (
            [&](const das::FunctionPtr& Fn)
            {
                const FString N = UTF8_TO_TCHAR(Fn->name.c_str());
                if (N.StartsWith(
                        TEXT("__ue_self_map_get_")) ||
                        N.StartsWith(TEXT("__ue_self_map_set_")) ||
                                     N.StartsWith(TEXT("ue_struct_map_get_")) ||
                                                  N.StartsWith(TEXT("__ue_map_get_token_")) ||
                                                               N.StartsWith(TEXT("__ue_map_with_token_")) ||
                                                                            N == TEXT("__ue_map_struct_token") ||
                                                                                      N ==
                                                                                          TEXT("__ue_map_object_token"))
                    ++MapBindings;
                Legacy |=
                    N == TEXT("__ue_self_map_get_int_int") || N == TEXT("ue_struct_map_get_string_int") ||
                                                                        N == TEXT("__ue_struct_map_get_object_float");
            });

    return Legacy ? -MapBindings : MapBindings;
}
