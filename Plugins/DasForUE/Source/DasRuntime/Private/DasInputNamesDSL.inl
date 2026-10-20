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
