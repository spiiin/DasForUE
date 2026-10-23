struct FDasNativeDiagnosticSite
{
    das::Context* Context = nullptr;
    const das::LineInfo* At = nullptr;
};
thread_local FDasNativeDiagnosticSite DasNativeDiagnosticSite;
template <class Base> struct TDasDiagnosticNode : Base
{
    using Base::Base;
    DAS_EVAL_ABI virtual vec4f eval(das::Context& C) override
    {
        const auto Previous = DasNativeDiagnosticSite;
        DasNativeDiagnosticSite = {&C, &this->debugInfo};
        const auto Result = Base::eval(C);
        DasNativeDiagnosticSite = Previous;
        return Result;
    }
#define EVAL_NODE(TYPE, CTYPE)                                                                                         \
    virtual CTYPE eval##TYPE(das::Context& C) override                                                                 \
    {                                                                                                                  \
        const auto Previous = DasNativeDiagnosticSite;                                                                 \
        DasNativeDiagnosticSite = {&C, &this->debugInfo};                                                              \
        const auto Result = Base::eval##TYPE(C);                                                                       \
        DasNativeDiagnosticSite = Previous;                                                                            \
        return Result;                                                                                                 \
    }
    DAS_EVAL_NODE
#undef EVAL_NODE
};
template <class F> using TDasDiagnosticExtern = TDasDiagnosticNode<das::SimNode_ExtFuncCall<F>>;
template <class F> using TDasDiagnosticCMRES = TDasDiagnosticNode<das::SimNode_ExtFuncCallAndCopyOrMove<F>>;
