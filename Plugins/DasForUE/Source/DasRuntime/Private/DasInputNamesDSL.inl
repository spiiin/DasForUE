thread_local TSet<FString> DSLInputNameWarnings;
void DSLResetInputNameLint()
{
    DSLInputNameWarnings.Reset();
}
struct FUEInputNameLint : das::PassMacro
{
    FUEInputNameLint() : PassMacro("ue_string_input_names") {}
    virtual bool apply(das::Program*, das::Module* M) override
    {
        FString File = FPaths::ConvertRelativePathToFull(UTF8_TO_TCHAR(M->fileName.c_str()));
        FString Library = FPaths::ConvertRelativePathToFull(UTF8_TO_TCHAR(das::getDasRoot().c_str())) / TEXT("daslib");
        FPaths::NormalizeFilename(File);
        FPaths::NormalizeFilename(Library);
        // UE annotations/type macros consult loaded reflected definitions and record
        // host-only compiler metadata. Reparse those modules even when bytes match.
        // Pure stdlib records retain daScript's own dependency/policy/builtin checks.
        auto* Foundation = das::Module::require("ue_foundation");
        if (!FPaths::IsUnderDirectory(File, Library) || (Foundation && M->isVisibleDirectly(Foundation)))
            M->registersAtCompile = true;
        struct FCheck : das::Visitor
        {
            virtual void preVisit(das::ExprCall* E) override
            {
                const auto Name = DSLOperationName(E->name);
                if ((Name != "ue_enhanced_bind" && Name != "ue_enhanced_unbind") || E->arguments.size() < 2 ||
                    !E->at.fileInfo || E->arguments[1]->__rtti != das::string("ExprConstString"))
                    return;
                const FString File = DSLText(E->at.fileInfo->name);
                if (FPaths::GetBaseFilename(File).StartsWith(TEXT("ue_")))
                    return;
                const FString Where = File + FString::Printf(TEXT(":%u:%u"), E->at.line, E->at.column);
                if (DSLInputNameWarnings.Contains(Where))
                    return;
                DSLInputNameWarnings.Add(Where);
                const FString Trigger = DSLText(static_cast<das::ExprConstString*>(E->arguments[1])->text);
                UE_LOG(LogDasRuntime, Warning,
                       TEXT("DAS_DSL_STRING_TRIGGER %s: prefer TriggerEvent.%s over a string trigger"), *Where,
                            *Trigger);
            }
        } Check;
        M->functions.foreach (
            [&](das::Function* F)
            {
                if (F->body && !F->generated)
                    F->body->visit(Check);
            });
        return false;
    }
};
