#pragma once
#include "CoreMinimal.h"
#include "DasScriptValues.h"
#include "daScript/simulate/simulate.h"
#include <type_traits>
#include <utility>

// Generated code only sees this ABI, never private UE function addresses or UObject layout.
namespace dasue::aot
{
DASRUNTIME_API void* Resolve(uint64_t Key, das::Context* Context);
// After the call: a native bind that reported DasNativeError fails the script call here.
DASRUNTIME_API void Finish(uint64_t Key, das::Context* Context);
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
// Object references as their handles: the extern resolves and checks them itself, the same code on both paths.
DAS_UE_AOT_TYPE(const FDasObjectHandle&, "const FDasObjectHandle &")
DAS_UE_AOT_TYPE(FDasObjectHandle, "FDasObjectHandle")
#undef DAS_UE_AOT_TYPE
// Engine values (by const reference, results by value) are only in the ABI of native binds (DasNativeBinds.h);
// AddUEExtern keeps the scalar and object ABI above.
template <class T> struct ValueType
{
    static constexpr const char* Name = Type<T>::Name;
};
#define DAS_UE_AOT_VALUE(T, N)                                                                                         \
    template <> struct ValueType<T>                                                                                    \
    {                                                                                                                  \
        static constexpr const char* Name = N;                                                                         \
    };
DAS_UE_AOT_VALUE(const FVector&, "const FVector &")
DAS_UE_AOT_VALUE(FVector, "FVector")
DAS_UE_AOT_VALUE(const FVector2D&, "const FVector2D &")
DAS_UE_AOT_VALUE(FVector2D, "FVector2D")
DAS_UE_AOT_VALUE(const FRotator&, "const FRotator &")
DAS_UE_AOT_VALUE(FRotator, "FRotator")
DAS_UE_AOT_VALUE(const FQuat&, "const FQuat &")
DAS_UE_AOT_VALUE(FQuat, "FQuat")
DAS_UE_AOT_VALUE(const FTransform&, "const FTransform &")
DAS_UE_AOT_VALUE(FTransform, "FTransform")
DAS_UE_AOT_VALUE(const FLinearColor&, "const FLinearColor &")
DAS_UE_AOT_VALUE(FLinearColor, "FLinearColor")
DAS_UE_AOT_VALUE(const FColor&, "const FColor &")
DAS_UE_AOT_VALUE(FColor, "FColor")
DAS_UE_AOT_VALUE(FDasDateTimeValue, "FDasDateTimeValue")
DAS_UE_AOT_VALUE(FDasTimespanValue, "FDasTimespanValue")
DAS_UE_AOT_VALUE(FDasClassHandle, "FDasClassHandle")
// Reference parameters of native binds (DasNativeBinds.h: copied in, written back when the call succeeds).
DAS_UE_AOT_VALUE(bool&, "bool &")
DAS_UE_AOT_VALUE(int32_t&, "int32_t &")
DAS_UE_AOT_VALUE(int64_t&, "int64_t &")
DAS_UE_AOT_VALUE(float&, "float &")
DAS_UE_AOT_VALUE(double&, "double &")
DAS_UE_AOT_VALUE(uint8_t&, "uint8_t &")
DAS_UE_AOT_VALUE(char*&, "char * &")
DAS_UE_AOT_VALUE(FDasDateTimeValue&, "FDasDateTimeValue &")
DAS_UE_AOT_VALUE(FDasTimespanValue&, "FDasTimespanValue &")
DAS_UE_AOT_VALUE(FVector&, "FVector &")
DAS_UE_AOT_VALUE(FVector2D&, "FVector2D &")
DAS_UE_AOT_VALUE(FRotator&, "FRotator &")
DAS_UE_AOT_VALUE(FQuat&, "FQuat &")
DAS_UE_AOT_VALUE(FTransform&, "FTransform &")
DAS_UE_AOT_VALUE(FLinearColor&, "FLinearColor &")
DAS_UE_AOT_VALUE(FColor&, "FColor &")
#undef DAS_UE_AOT_VALUE

template <class F> struct Signature
{
    static constexpr bool Supported = false;
};
template <class R, class... A> struct Signature<R (*)(A...)>
{
    static constexpr bool Supported =
        Type<R>::Name && !std::is_same_v<R, das::Context*> && (bool(Type<A>::Name) && ...) && ((0 + ... + int(std::is_same_v<A, das::Context*>)) == 1);
    static FString CppTypes()
    {
        FString S = UTF8_TO_TCHAR(Type<R>::Name);
        ((S += FString(TEXT(",")) + UTF8_TO_TCHAR(Type<A>::Name)), ...);
        return S;
    }
};
template <class F> struct CallSignature
{
    static constexpr bool Supported = false;
};
template <class R, class... A> struct CallSignature<R (*)(A...)>
{
    static constexpr bool Supported = ValueType<R>::Name && !std::is_same_v<R, das::Context*> &&
                                      (bool(ValueType<A>::Name) && ...) &&
                                      ((0 + ... + int(std::is_same_v<A, das::Context*>)) == 1);
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
    static_assert(CallSignature<R (*)(A...)>::Supported, "Unsupported UE AOT ABI");
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
    if constexpr (std::is_void_v<R>)
    {
        Fn(std::forward<A>(Args)...);
        Finish(Id, Context);
    }
    else
    {
        R Result = Fn(std::forward<A>(Args)...);
        Finish(Id, Context);
        return Result;
    }
}
}
