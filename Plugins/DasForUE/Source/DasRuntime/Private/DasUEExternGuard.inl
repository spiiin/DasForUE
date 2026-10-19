// Every UE FFI binding uses this factory. The guarded pointer is also the JIT
// builtin address; scalar and CMRES nodes keep their original ABI.
void GuardUEExtern();
int32 UEExternRegistrationCount = 0;
template <class F> struct TCheckedUEExtern;
template <class R, class... A> struct TCheckedUEExtern<R (*)(A...)>
{
    template <R (*Fn)(A...)> static R Invoke(A... Args)
    {
        GuardUEExtern();
        return Fn(std::forward<A>(Args)...);
    }
};
template <class F, F Fn, template <class> class Node = das::SimNode_ExtFuncCall, class QQ = das::defaultTempFn>
auto AddUEExtern(das::Module& Module, const das::ModuleLibrary& Library, const char* Name, das::SideEffects Effects,
                 const char* CppName = nullptr, QQ&& Temp = QQ())
{
    auto* Bound = das::addExtern<F, &TCheckedUEExtern<F>::template Invoke<Fn>, Node>(
        Module, Library, Name, Effects | das::SideEffects::accessExternal, CppName, std::forward<QQ>(Temp));
    // The foundation module already has no AOT implementation. Make the guard
    // requirement explicit so emitted raw cppName calls cannot bypass it.
    Bound->noAot = true;
    ++UEExternRegistrationCount;
    return Bound;
}

void GuardPhysicsDataExtern();
template <class F> struct TCheckedPhysicsDataExtern;
template <class R, class... A> struct TCheckedPhysicsDataExtern<R (*)(A...)>
{
    template <R (*Fn)(A...)> static R Invoke(A... Args)
    {
        GuardPhysicsDataExtern();
        return Fn(std::forward<A>(Args)...);
    }
};
template <class F, F Fn>
auto AddPhysicsDataExtern(das::Module& Module, const das::ModuleLibrary& Library, const char* Name)
{
    auto* Bound = das::addExtern<F, &TCheckedPhysicsDataExtern<F>::template Invoke<Fn>>(
        Module, Library, Name, das::SideEffects::accessExternal | das::SideEffects::modifyExternal);
    Bound->noAot = true;
    return Bound;
}
