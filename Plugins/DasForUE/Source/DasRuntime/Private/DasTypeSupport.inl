// Registry diagnostics: no independent per-kind support decisions live here.
TSharedRef<FJsonObject> SupportDescriptor(const FDasDeclaredType& Type)
{
    auto J = MakeShared<FJsonObject>();
    J->SetStringField(TEXT("kind"), Type.Kind);
    J->SetStringField(TEXT("type_path"), Type.TypePath);
    J->SetBoolField(TEXT("weak"), Type.WeakReference);
    J->SetNumberField(TEXT("enum_storage"), int32(Type.EnumStorage));
    J->SetStringField(TEXT("authored_id"), Type.AuthoredId);
    J->SetStringField(TEXT("delegate_wire"), Type.DelegateWire);
    for (const auto& Pair : {TPair<const TCHAR*, TSharedPtr<const FDasDeclaredType>>(
             TEXT("element"), Type.Element), {TEXT("key"), Type.Key}, {TEXT("value"), Type.Value}})
        if (Pair.Value)
            J->SetObjectField(Pair.Key, SupportDescriptor(*Pair.Value));
    return J;
}
TSharedRef<FJsonObject> SupportCell(const FDasDeclaredType& Type, EDasPropertyUsage Usage)
{
    FString Error;
    const bool Supported = ValidateDasPropertyType(Type, Usage, Error);
    auto J = MakeShared<FJsonObject>();
    J->SetStringField(TEXT("state"), Supported ? TEXT("supported") : TEXT("unsupported"));
    J->SetStringField(TEXT("reason"), Error);
    return J;
}
FString BuildTypeAdapterSupportManifest()
{
    check(IsInGameThread());
    auto Root = MakeShared<FJsonObject>();
    Root->SetNumberField(TEXT("schema_version"), 3);
    Root->SetStringField(TEXT("scope"),
                              TEXT("registered adapter property ABI, signature, callback and default type gates"));
    Root->SetStringField(TEXT("evidence"),
                              TEXT("ValidateDasPropertyType; QueryDasTypeUse; adapter and script facet queries"));
    Root->SetStringField(TEXT("aot_scope"), TEXT("per binding; not inferred from a type adapter"));
    TArray<TSharedPtr<FJsonValue>> Unmodeled;
    for (const TCHAR* S :
         {TEXT("complete DSL syntax, expression evaluation, literal validity and declaration graph resolution"),
               TEXT("source receiver/signature compatibility, payload budgets and delivery liveness"),
                    TEXT("generated native API and unregistered engine transports")})
        Unmodeled.Add(MakeShared<FJsonValueString>(S));
    Root->SetArrayField(TEXT("not_inferred"), Unmodeled);
    TArray<TSharedPtr<FJsonValue>> Uses;
    for (const auto& Use : DasTypeUses())
    {
        auto J = MakeShared<FJsonObject>();
        J->SetStringField(TEXT("name"), Use.Name);
        J->SetStringField(TEXT("boundary"), Use.Boundary);
        Uses.Add(MakeShared<FJsonValueObject>(J));
    }
    Root->SetArrayField(TEXT("contexts"), Uses);
    TArray<TPair<FString, TSharedPtr<FJsonValue>>> Sorted;
    for (const auto& Entry : FDasTypeAdapters::Get().Entries())
    {
        const auto& A = Entry.Adapter.Get();
        auto J = MakeShared<FJsonObject>();
        const FString Path = A.GetStruct() ? A.GetStruct()->GetPathName() : FString();
        const FString Id = DasTypeKindName(A.GetKind()) +
                           (Path.IsEmpty() ? A.IsStructFamily() ? TEXT(":family") : TEXT("") : TEXT(":") + Path);
        J->SetStringField(TEXT("id"), Id);
        J->SetStringField(TEXT("kind"), DasTypeKindName(A.GetKind()));
        J->SetStringField(TEXT("struct_path"), Path);
        J->SetBoolField(TEXT("family"), A.IsStructFamily());
        J->SetBoolField(TEXT("leaf"), A.IsLeafValue());
        J->SetBoolField(TEXT("reference_value"), A.IsReferenceValue());
        if (const auto* P = A.GetNativePolicy())
        {
            auto N = MakeShared<FJsonObject>();
            N->SetNumberField(TEXT("roles"), int32(P->Roles));
            N->SetStringField(TEXT("transport"), P->Transport ? P->Transport : TEXT(""));
            N->SetStringField(TEXT("handle"), P->Handle ? UTF8_TO_TCHAR(P->Handle) : TEXT(""));
            N->SetStringField(TEXT("lifetime"), P->Lifetime ? P->Lifetime : TEXT(""));
            N->SetStringField(TEXT("storage_error"), P->StorageError ? P->StorageError : TEXT(""));
            N->SetStringField(TEXT("call_constraint"), P->CallError ? UTF8_TO_TCHAR(P->CallError) : TEXT(""));
            N->SetBoolField(TEXT("call_gate"), P->AllowCall != nullptr);
            N->SetBoolField(TEXT("capture_snapshot"), P->Capture != nullptr);
            N->SetBoolField(TEXT("validate_snapshot"), P->ValidateSnapshot != nullptr);
            J->SetObjectField(TEXT("native_policy"), N);
        }
        else
            J->SetField(TEXT("native_policy"), MakeShared<FJsonValueNull>());

        J->SetBoolField(TEXT("container_value_policy"), A.SupportsContainerValue());
        J->SetStringField(TEXT("default_policy"),
                               A.GetDefaultPolicy() == EDasAdapterDefault::ImplicitEmpty ? TEXT("implicit_empty")
                                                                                         : TEXT("explicit"));
        J->SetStringField(TEXT("default_kind"), A.GetDefaultKind() ? A.GetDefaultKind() : TEXT(""));
        TArray<TSharedPtr<FJsonValue>> Slots;
        for (const auto& Slot : A.GetContainerSlots())
        {
            auto S = MakeShared<FJsonObject>();
            S->SetStringField(TEXT("name"), Slot.Descriptor == &FDasDeclaredType::Element
                                   ? TEXT("element")
                                   : Slot.Descriptor == &FDasDeclaredType::Key
                                   ? TEXT("key")
                                   : TEXT("value"));
            S->SetBoolField(TEXT("hash_key"), Slot.bHashKey);
            Slots.Add(MakeShared<FJsonValueObject>(S));
        }
        J->SetArrayField(TEXT("container_slots"), Slots);
        auto Script = MakeShared<FJsonObject>();
        Script->SetBoolField(TEXT("registered"), Entry.Script != nullptr);
        const auto* F = Entry.Script;
        Script->SetStringField(TEXT("module"), F && F->GetViewModule() ? F->GetViewModule() : TEXT(""));
        Script->SetStringField(TEXT("handle"), F && F->GetHandleName() ? UTF8_TO_TCHAR(F->GetHandleName()) : TEXT(""));
        for (const auto& Use : {TPair<const TCHAR*, EDasScriptViewUse>(
                 TEXT("native_property"), EDasScriptViewUse::NativeProperty),
                 {TEXT("self_property"), EDasScriptViewUse::SelfProperty},
                  {TEXT("authored_reference"), EDasScriptViewUse::AuthoredReference}})
            Script->SetBoolField(Use.Key, F && EnumHasAnyFlags(F->GetViewUses(), Use.Value));
        for (const auto& Bridge :
             {TPair<const TCHAR*, EDasCallBridge>(TEXT("frame_bridge"), EDasCallBridge::FrameSlot),
                                                  {TEXT("method_bridge"), EDasCallBridge::MethodBridge}})
            Script->SetStringField(Bridge.Key, F && F->GetCallBridge(Bridge.Value)
                                                   ? UTF8_TO_TCHAR(F->GetCallBridge(Bridge.Value))
                                                   : TEXT(""));
        J->SetObjectField(TEXT("script"), Script);
        TArray<FDasDeclaredType> Types;
        A.GetSupportSamples(Types);
        Types.Sort([](const auto& L, const auto& R) { return L.StorageIdentity() < R.StorageIdentity(); });
        TArray<TSharedPtr<FJsonValue>> Samples;
        for (const auto& Type : Types)
        {
            auto S = MakeShared<FJsonObject>();
            S->SetStringField(TEXT("id"), Type.StorageIdentity());
            S->SetObjectField(TEXT("type"), SupportDescriptor(Type));
            auto Caps = MakeShared<FJsonObject>();
            Caps->SetObjectField(TEXT("value_property"), SupportCell(Type, EDasPropertyUsage::Value));
            Caps->SetObjectField(TEXT("snapshot_member"), SupportCell(Type, EDasPropertyUsage::Container));
            Caps->SetObjectField(TEXT("delegate_parameter"), SupportCell(Type, EDasPropertyUsage::DelegateParameter));
            for (const TCHAR* Context :
                 {TEXT("array_element"), TEXT("map_value"), TEXT("map_key"), TEXT("set_element")})
            {
                FDasDeclaredType C;
                const FString Name(Context);
                C.Kind = Name == TEXT("array_element") ? TEXT("array")
                                                       : Name == TEXT("set_element") ? TEXT("set") : TEXT("map");
                for (const auto& Slot : FDasTypeAdapters::Get().FindKind(C.GetKind())->GetContainerSlots())
                {
                    const bool Target =
                        C.GetKind() != EDasTypeKind::Map || (Slot.bHashKey == (Name == TEXT("map_key")));
                    C.*Slot.Descriptor = MakeShared<FDasDeclaredType>(Target ? Type : FDasDeclaredType());
                }
                Caps->SetObjectField(Context, SupportCell(C, EDasPropertyUsage::Value));
            }
            S->SetObjectField(TEXT("capabilities"), Caps);
            auto Contracts = MakeShared<FJsonObject>();
            for (const auto& Use : DasTypeUses())
            {
                const auto Result = QueryDasTypeUse(Type, Use.Use);
                auto Cell = MakeShared<FJsonObject>();
                Cell->SetStringField(TEXT("state"), Result.State == EDasTypeSupport::Supported
                                          ? TEXT("supported")
                                          : Result.State == EDasTypeSupport::RequiresContext
                                          ? TEXT("requires_context")
                                          : TEXT("unsupported"));
                Cell->SetStringField(TEXT("reason"), Result.Reason);
                Contracts->SetObjectField(Use.Name, Cell);
            }
            S->SetObjectField(TEXT("uses"), Contracts);
            Samples.Add(MakeShared<FJsonValueObject>(S));
        }
        J->SetArrayField(TEXT("samples"), Samples);
        Sorted.Add({Id, MakeShared<FJsonValueObject>(J)});
    }
    Sorted.Sort([](const auto& L, const auto& R) { return L.Key < R.Key; });
    TArray<TSharedPtr<FJsonValue>> Rows;
    for (const auto& Row : Sorted)
        Rows.Add(Row.Value);
    Root->SetArrayField(TEXT("adapters"), Rows);
    FString Result;
    auto Writer = TJsonWriterFactory<>::Create(&Result);
    FJsonSerializer::Serialize(Root, Writer);
    Result.ReplaceInline(TEXT("\r\n"), TEXT("\n"));
    return Result + TEXT("\n");
}
