#pragma once
#include "CoreMinimal.h"
#include "DasSchema.h"
#include "Misc/DateTime.h"
#include "Misc/Timespan.h"
#include "Templates/ValueOrError.h"
#include "UObject/Object.h"
#include <type_traits>

// Typed calls of script functions from C++. A TDasFunction names an exported function of a script and its
// signature; Call runs it with an owner object as self, in the owner's persistent session of that script (the
// session RunNamedPersistent uses: globals, timers and subscriptions stay between calls):
//
//     TDasFunction<int32(int32, float)> EnemyCount{TEXT("waves.das"), TEXT("enemy_count")};
//     TValueOrError<int32, FString> Count = EnemyCount.Call(this, Wave, Difficulty);
//
//     [export]
//     def enemy_count(wave:int; difficulty:float):int { ... }
//
// The script function must be [export] and have exactly these types (C++ -> script):
//     bool -> bool, int32 -> int, int64 -> int64, float -> float, double -> double, uint8 -> uint8,
//     FString -> string, FName -> UENameValue, FText -> UETextValue, FVector -> UEVector,
//     FVector2D -> UEVector2D, FRotator -> UERotator, FQuat -> UEQuat, FTransform -> UETransform,
//     FLinearColor -> UELinearColor, FColor -> UEColor, FDateTime -> UEDateTime, FTimespan -> UETimespan,
//     T* (T a UObject class) -> UObjectHandle, void result -> void.
// The signature is checked when the session first runs the function; a mismatch, a missing function, a script
// error or a dead owner is an error value, never a crash. A reload that changes the signature of a function the
// session has been called with is rejected (logged) and the previous version keeps running. A returned object of
// another class than T reads as nullptr. Game thread only.
//
// Speed: a TDasFunction remembers the session and the function of its last owner. Until that script changes or
// another owner is used, a call goes straight to the VM (das.ScriptFunctionFastPath 0 turns this off).

// One argument or the result of a call: its kind and the address of the C++ value (an object: of its UObject*).
struct FDasScriptArgument
{
    EDasMethodSignature Kind = EDasMethodSignature::Void;
    void* Value = nullptr;
    UClass* Class = nullptr; // objects: the class the value must have
};
// What a TDasFunction remembers between calls (DasRuntime internal).
struct FDasScriptCallSite;

// The call behind TDasFunction. Result.Value receives the result; Error is set on failure.
DASRUNTIME_API bool DasCallScriptFunction(TSharedPtr<FDasScriptCallSite>& Site, UObject* Owner, const FString& Script,
                                          const FString& Entry, TArrayView<const FDasScriptArgument> Arguments,
                                          const FDasScriptArgument& Result, FString& Error);
// How many TDasFunction calls took the fast path since the start; for tests.
DASRUNTIME_API uint64 DasScriptFunctionFastCalls();
// The script name of a value kind that a type adapter owns (math values), for signatures in error texts.
DASRUNTIME_API FString DasScriptTypeName(EDasMethodSignature Kind);

template <class T, class Enable = void> struct TDasScriptType
{
    static_assert(sizeof(T) == 0, "Unsupported TDasFunction type: use bool, int32, int64, float, double, uint8, "
                                  "FString, FName, FText, FVector, FVector2D, FRotator, FQuat, FTransform, "
                                  "FLinearColor, FColor, FDateTime, FTimespan, a pointer to a UObject class or void");
};
// Kind and script name of a type; Storage holds a result.
#define DAS_SCRIPT_TYPE(CppType, KindName, ScriptName)                                                                 \
    template <> struct TDasScriptType<CppType>                                                                         \
    {                                                                                                                  \
        using Storage = CppType;                                                                                       \
        static constexpr EDasMethodSignature Kind = EDasMethodSignature::KindName;                                     \
        static FString Name()                                                                                          \
        {                                                                                                              \
            return ScriptName;                                                                                         \
        }                                                                                                              \
        static UClass* Class()                                                                                         \
        {                                                                                                              \
            return nullptr;                                                                                            \
        }                                                                                                              \
        static void* Address(const CppType& Value)                                                                     \
        {                                                                                                              \
            return const_cast<CppType*>(&Value);                                                                       \
        }                                                                                                              \
        static CppType Read(const CppType& Value)                                                                      \
        {                                                                                                              \
            return Value;                                                                                              \
        }                                                                                                              \
    };
DAS_SCRIPT_TYPE(bool, Bool, TEXT("bool"))
DAS_SCRIPT_TYPE(int32, Int, TEXT("int"))
DAS_SCRIPT_TYPE(int64, Int64, TEXT("int64"))
DAS_SCRIPT_TYPE(float, Float, TEXT("float"))
DAS_SCRIPT_TYPE(double, Double, TEXT("double"))
DAS_SCRIPT_TYPE(uint8, Byte, TEXT("uint8"))
DAS_SCRIPT_TYPE(FString, String, TEXT("string"))
DAS_SCRIPT_TYPE(FName, Name, DasScriptTypeName(Kind))
DAS_SCRIPT_TYPE(FText, Text, DasScriptTypeName(Kind))
DAS_SCRIPT_TYPE(FVector, Vector, DasScriptTypeName(Kind))
DAS_SCRIPT_TYPE(FRotator, Rotator, DasScriptTypeName(Kind))
DAS_SCRIPT_TYPE(FTransform, Transform, DasScriptTypeName(Kind))
DAS_SCRIPT_TYPE(FVector2D, Vector2D, DasScriptTypeName(Kind))
DAS_SCRIPT_TYPE(FQuat, Quat, DasScriptTypeName(Kind))
DAS_SCRIPT_TYPE(FLinearColor, LinearColor, DasScriptTypeName(Kind))
DAS_SCRIPT_TYPE(FColor, Color, DasScriptTypeName(Kind))
DAS_SCRIPT_TYPE(FDateTime, DateTime, TEXT("UEDateTime"))
DAS_SCRIPT_TYPE(FTimespan, Timespan, TEXT("UETimespan"))
#undef DAS_SCRIPT_TYPE
template <> struct TDasScriptType<void>
{
    static constexpr EDasMethodSignature Kind = EDasMethodSignature::Void;
    static FString Name() { return TEXT("void"); }
    static UClass* Class() { return nullptr; }
};
template <class T> struct TDasScriptType<T*, std::enable_if_t<std::is_base_of_v<UObject, T>>>
{
    using Storage = UObject*;
    static constexpr EDasMethodSignature Kind = EDasMethodSignature::Object;
    static FString Name() { return TEXT("UObjectHandle"); }
    static UClass* Class() { return std::remove_const_t<T>::StaticClass(); }
    // An object argument is passed as the address of its UObject*.
    static void* Address(UObject* const& Value) { return const_cast<UObject**>(&Value); }
    static T* Read(UObject* Value) { return Cast<std::remove_const_t<T>>(Value); }
};

template <class Signature> class TDasFunction;
template <class R, class... A> class TDasFunction<R(A...)>
{
    template <class T> using TValue = std::remove_cv_t<std::remove_reference_t<T>>;
    template <class T> using TType = TDasScriptType<TValue<T>>;
    // An object argument becomes a UObject* first, so that its address can be passed.
    template <class T> using TArgument = std::conditional_t<std::is_pointer_v<TValue<T>>, UObject*, TValue<T>>;

public:
    // Script: a file name under the script folders ("waves.das"; the project's folder first) or a path relative
    // to them. Entry: the exported function.
    TDasFunction(const TCHAR* InScript, const TCHAR* InEntry) : Script(InScript), Entry(InEntry) {}

    TValueOrError<R, FString> Call(UObject* Owner, const TValue<A>&... Args) const
    {
        return Invoke(Owner, TArgument<A>(Args)...);
    }
    // The script signature this function expects, e.g. "enemy_count(int, float) : int".
    FString Signature() const
    {
        const TArray<FString> Names{TType<A>::Name()...};
        return Entry + TEXT("(") + FString::Join(Names, TEXT(", ")) + TEXT(") : ") + TType<R>::Name();
    }
    const FString& GetScript() const { return Script; }
    const FString& GetEntry() const { return Entry; }

private:
    TValueOrError<R, FString> Invoke(UObject* Owner, const TArgument<A>&... Args) const
    {
        const FDasScriptArgument Arguments[sizeof...(A) + 1] = {
            FDasScriptArgument{TType<A>::Kind, TType<A>::Address(Args), TType<A>::Class()}..., FDasScriptArgument()};
        const auto View = MakeArrayView(Arguments, int32(sizeof...(A)));
        FString Error;
        if constexpr (std::is_void_v<R>)
        {
            if (DasCallScriptFunction(Site, Owner, Script, Entry, View, FDasScriptArgument(), Error))
                return MakeValue();
        }
        else
        {
            typename TType<R>::Storage Value{};
            const FDasScriptArgument Result{TType<R>::Kind, &Value, TType<R>::Class()};
            if (DasCallScriptFunction(Site, Owner, Script, Entry, View, Result, Error))
                return MakeValue(TType<R>::Read(Value));
        }
        return MakeError(Script + TEXT(": ") + Error + TEXT(" (C++ expects ") + Signature() + TEXT(")"));
    }
    FString Script, Entry;
    mutable TSharedPtr<FDasScriptCallSite> Site;
};
