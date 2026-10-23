// Every UE FFI binding uses this factory. The guarded pointer is also the JIT
// builtin address; scalar and CMRES nodes keep their original ABI.
void GuardUEExtern();
#include "DasDiagnosticNodes.inl"
template <class F> struct TCheckedUEExtern;
struct FUEAotEntry
{
    void* Function;
    FString Identity;
    FString Label;               // the script name, for errors
    const bool* Loaded = nullptr; // native binds: false once the code module is unloaded
};
TMap<uint64, FUEAotEntry> UEAotEntries;

template <class F, F Fn> bool ConfigureUEAot(das::BuiltInFunction* Bound, const char* Name)
{
    if constexpr (dasue::aot::Signature<F>::Supported)
    {
        const FString Types = dasue::aot::Signature<F>::CppTypes();
        const uint64 Key = dasue::aot::Key(Name, TCHAR_TO_UTF8(*Types));
        const FString Identity = FString(UTF8_TO_TCHAR(Name)) + TEXT(":") + Types;
        void* Pointer = reinterpret_cast<void*>(&TCheckedUEExtern<F>::template Invoke<Fn>);
        if (const auto* Existing = UEAotEntries.Find(Key);
            Existing && (Existing->Identity != Identity || Existing->Function != Pointer))
            throw std::runtime_error("UE AOT extern identity collision");
        UEAotEntries.Add(Key, {Pointer, Identity, FString(UTF8_TO_TCHAR(Name))});
        Bound->cppName = TCHAR_TO_UTF8(*FString::Printf(TEXT("dasue::aot::Call<%lluULL,%s>"), Key, *Types));
        return true;
    }
    return false;
}
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
template <template <class> class Node> struct TDasDiagnosticSelect
{
    template <class F> using Type = TDasDiagnosticNode<Node<F>>;
};
template <class F, F Fn, template <class> class Node = das::SimNode_ExtFuncCall, class QQ = das::defaultTempFn>
auto AddUEExtern(das::Module& Module, const das::ModuleLibrary& Library, const char* Name, das::SideEffects Effects,
                 const char* CppName = nullptr, QQ&& Temp = QQ())
{
    auto* Bound =
        das::addExtern<F, &TCheckedUEExtern<F>::template Invoke<Fn>, TDasDiagnosticSelect<Node>::template Type>(
            Module, Library, Name, Effects | das::SideEffects::accessExternal, CppName, std::forward<QQ>(Temp));
    // Only the checked, ABI-described registry bridge is AOT eligible. All other
    // signatures stay closed; raw cppName calls can never bypass the guard.
    Bound->noAot = !ConfigureUEAot<F, Fn>(Bound, Name);
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
