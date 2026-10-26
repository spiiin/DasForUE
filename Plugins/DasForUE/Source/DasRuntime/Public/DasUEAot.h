#pragma once
#include "CoreMinimal.h"
#include "daScript/simulate/simulate.h"
#include <type_traits>
#include <utility>

// Generated code only sees this ABI, never private UE function addresses or UObject layout.
namespace dasue::aot
{
DASRUNTIME_API void* Resolve(uint64_t Key, das::Context* Context);
template <class T> struct Type
{
    static constexpr const char* Name = nullptr;
};
#define DAS_UE_AOT_TYPE(T, N)                                                                                          \
    template <> struct Type<T>                                                                                         \
    {                                                                                                                  \
        static constexpr const char* Name = N;                                                                         \
    };
DAS_UE_AOT_TYPE(void, "void")
DAS_UE_AOT_TYPE(bool, "bool")
DAS_UE_AOT_TYPE(int8_t, "int8_t")
DAS_UE_AOT_TYPE(uint8_t, "uint8_t")
DAS_UE_AOT_TYPE(int16_t, "int16_t")
DAS_UE_AOT_TYPE(uint16_t, "uint16_t")
DAS_UE_AOT_TYPE(int32_t, "int32_t")
DAS_UE_AOT_TYPE(uint32_t, "uint32_t")
DAS_UE_AOT_TYPE(int64_t, "int64_t")
DAS_UE_AOT_TYPE(uint64_t, "uint64_t")
DAS_UE_AOT_TYPE(float, "float")
DAS_UE_AOT_TYPE(double, "double")
DAS_UE_AOT_TYPE(const char*, "const char *")
DAS_UE_AOT_TYPE(char*, "char *")
DAS_UE_AOT_TYPE(das::Context*, "das::Context *")
#undef DAS_UE_AOT_TYPE

template <class F> struct Signature
{
    static constexpr bool Supported = false;
};
template <class R, class... A> struct Signature<R (*)(A...)>
{
    static constexpr bool Supported = Type<R>::Name && !std::is_same_v<R, das::Context*> &&
                                      (bool(Type<A>::Name) && ...) &&
                                      ((0 + ... + int(std::is_same_v<A, das::Context*>)) == 1);
    static FString CppTypes()
    {
        FString S = UTF8_TO_TCHAR(Type<R>::Name);
        ((S += FString(TEXT(",")) + UTF8_TO_TCHAR(Type<A>::Name)), ...);
        return S;
    }
};
// Same identity in the emitter and consumer; changed ABI gets a different key.
inline uint64_t Key(const char* Name, const char* Signature)
{
    uint64_t H = 14695981039346656037ULL;
    for (auto* P : {Name, Signature})
    {
        for (; *P; ++P)
            H = (H ^ uint8_t(*P)) * 1099511628211ULL;
        H = (H ^ 0xff) * 1099511628211ULL;
    }
    return H;
}
template <uint64_t Id, class R, class... A> R Call(A... Args)
{
    static_assert(Signature<R (*)(A...)>::Supported, "Unsupported UE AOT ABI");
    das::Context* Context = nullptr;
    (
        [&]
        {
            if constexpr (std::is_same_v<A, das::Context*>)
                Context = Args;
        }(),
        ...);
    // Resolve checks thread, role and active context before any native function is entered.
    auto Fn = reinterpret_cast<R (*)(A...)>(Resolve(Id, Context));
    return Fn(std::forward<A>(Args)...);
}
// Interop calls carry VM-owned argument values and TypeInfo only for the duration of the call.
// The registry stores a checked thunk, never an unguarded builtin address.
template <uint64_t Id> vec4f Interop(das::Context& Context, das::SimNode_CallBase* Node, vec4f* Args)
{
    using FInterop = vec4f (*)(das::Context&, das::SimNode_CallBase*, vec4f*);
    auto Fn = reinterpret_cast<FInterop>(Resolve(Id, &Context));
    return Fn(Context, Node, Args);
}

}
