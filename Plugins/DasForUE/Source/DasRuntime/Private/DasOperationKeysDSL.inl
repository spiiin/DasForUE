// Named callbacks are stable operation identities within one owner/session and API family.
das::string DSLOperationName(const das::string& Name)
{
    const auto I = Name.rfind("::");
    return I == das::string::npos ? Name : Name.substr(I + 2);
}
FString DSLOperationFamily(const das::string& Name)
{
    const auto N = DSLOperationName(Name);
    if (N.find("ue_timer_") == 0 && N != "ue_timer_cancel_all")
        return TEXT("timer");
    if (N == "ue_latent_delay" || N == "ue_latent_cancel" || N == "ue_latent_is_pending")
        return TEXT("latent");
    if (N == "ue_effect_play" || N == "ue_effect_at" || N == "ue_sound_play" || N == "ue_niagara_spawn" ||
        N == "ue_effect_stop" || N == "ue_effect_active")
        return TEXT("effect");
    if (N == "ue_montage_play" || N == "ue_montage_stop" || N == "ue_montage_cancel" || N == "ue_montage_playing")
        return TEXT("montage");
    return {};
}
bool DSLOperationStart(const das::string& Name)
{
    const auto N = DSLOperationName(Name);
    return N == "ue_timer_start" || N == "ue_timer_next_tick" || N == "ue_latent_delay" || N == "ue_effect_play" ||
           N == "ue_effect_at" || N == "ue_sound_play" || N == "ue_niagara_spawn" || N == "ue_montage_play";
}
void DSLNormalizeOperationKeys(const das::StructurePtr& St, const das::AnnotationArgumentList& Args)
{
    const FString Module = DSLString(Args, "module_id", FPaths::GetBaseFilename(DSLText(St->at.fileInfo->name)));
    const FString ClassId = DSLString(Args, "id", Module + TEXT(".")+ DSLText(St->name));
    struct FNormalize : das::Visitor
    {
        das::StructurePtr St;
        FString ClassId;
        das::string Self;
        TMap<FString, TSet<FString>> Starts;
        struct FUse
        {
            FString Family, Key;
            das::LineInfo At;
        };
        TArray<FUse> Uses;
        FNormalize(das::StructurePtr S, const FString& Id) : St(S), ClassId(Id) {}
        das::ExprField* Method(das::Expression* E)
        {
            if (!E || !E->rtti_isField())
                return nullptr;
            auto* F = static_cast<das::ExprField*>(E);
            const auto* Member = St->findField(F->name);
            if (Member && !Member->classMethod)
                return nullptr; // A self property remains an ordinary value/key.
            return F->value && F->value->rtti_isVar() && static_cast<das::ExprVar*>(F->value)->name == Self ? F
                                                                                                            : nullptr;
        }
        FString Key(const FString& Family, das::ExprField* F)
        {
            const auto* Member = St->findField(F->name);
            if (!Member || !Member->classMethod)
                DSLFail(TEXT("Unknown operation callback self.")+ DSLText(F->name));
            const auto Bytes = DSLUtf8(Family + TEXT("::")+ ClassId + TEXT("::")+ DSLText(F->name));
            return TEXT("__ue_op_")+ FMD5::HashBytes(reinterpret_cast<const uint8*>(Bytes.data()), Bytes.size());
        }
        virtual void preVisit(das::ExprCall* E) override
        {
            const auto Family = DSLOperationFamily(E->name);
            if (Family.IsEmpty() || E->arguments.empty())
                return;
            const auto N = DSLOperationName(E->name);
            const bool Start = DSLOperationStart(N);
            if (Start)
            {
                const int32 Callback = DSLCallbackArgument(N, E->arguments.size() + 1) - 1;
                if (Callback >= 0 && size_t(Callback) < E->arguments.size())
                    if (auto* F = Method(E->arguments[Callback]))
                        E->arguments.insert(E->arguments.begin(),
                                            new das::ExprConstString(E->at, DSLUtf8(Key(Family, F))));
            }
            if (auto* F = Method(E->arguments[0]))
                E->arguments[0] = new das::ExprConstString(F->at, DSLUtf8(Key(Family, F)));
            if (E->arguments[0]->__rtti == das::string("ExprConstString"))
            {
                const auto K = DSLText(static_cast<das::ExprConstString*>(E->arguments[0])->text);
                if (Start)
                    Starts.FindOrAdd(Family).Add(K);
                else
                    Uses.Add({Family, K, E->at});
            }
        }
    } Normalize(St, ClassId);
    St->module->functions.foreach (
        [&](const das::FunctionPtr& F)
        {
            if (F->classParent == St && F->body && !F->arguments.empty())
            {
                Normalize.Self = F->arguments[0]->name;
                F->body = F->body->visit(Normalize);
            }
        });
    if (!CollectTypesOnly)
        for (const auto& Use : Normalize.Uses)
            if (!Normalize.Starts.FindRef(Use.Family).Contains(Use.Key))
                UE_LOG(
                    LogDasRuntime, Warning,
                    TEXT("DAS_DSL_UNKNOWN_OPERATION_KEY %s:%u: no statically declared %s start for key %s in class %s"),
                         Use.At.fileInfo ? *DSLText(Use.At.fileInfo->name)
                                         : TEXT("?"), Use.At.line, *Use.Family, *Use.Key, *ClassId);
}
