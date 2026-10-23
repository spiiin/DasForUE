#pragma once
#include "CoreMinimal.h"
#include <type_traits>
#include <utility>

// Native binds: C++ functions of a game (or plugin) module called directly from daScript, without reflection
// or a call frame. A module registered with DAS_NATIVE_BINDS is available to scripts as `require <name>`:
//
//     static int32 DamageFor(int32 Base, float Multiplier);
//     DAS_NATIVE_BINDS(MyGame, "my_game")
//     {
//         Binds.Function<&DamageFor>(TEXT("damage_for"), EDasBind::Pure);
//     }
//
// The script sees `damage_for(base:int; multiplier:float):int`. Calls run on the game thread under the same
// guards as the plugin's own externs. Only engine types appear here: daScript itself stays inside DasRuntime.

// Types of parameters and results. A const reference counts as its value type.
enum class EDasNativeType : uint8
{
    Void,
    Bool,
    Int32,
    Float,
    Double,
    Byte,
    String,
    Name,
    Text,
    Vector,
    Vector2D,
    Rotator,
    Quat,
    Transform,
    LinearColor,
    Color
};
enum class EDasBind : uint8
{
    None = 0,
    // No side effects and no engine state: the compiler may evaluate a call with constant arguments while it
    // compiles the script, outside any world. Never mark functions that read or change game state.
    Pure = 1
};
ENUM_CLASS_FLAGS(EDasBind)

template <class T> struct TDasNativeType
{
    static_assert(sizeof(T) == 0,
                  "Unsupported native bind type: use bool, int32, float, double, uint8, FString, FName, "
                  "FText, FVector, FVector2D, FRotator, FQuat, FTransform, FLinearColor or FColor");
};
#define DAS_NATIVE_TYPE(CppType, Kind)                                                                                 \
    template <> struct TDasNativeType<CppType>                                                                         \
    {                                                                                                                  \
        static constexpr EDasNativeType Value = EDasNativeType::Kind;                                                  \
    };
DAS_NATIVE_TYPE(void, Void)
DAS_NATIVE_TYPE(bool, Bool)
DAS_NATIVE_TYPE(int32, Int32)
DAS_NATIVE_TYPE(float, Float)
DAS_NATIVE_TYPE(double, Double)
DAS_NATIVE_TYPE(uint8, Byte)
DAS_NATIVE_TYPE(FString, String)
DAS_NATIVE_TYPE(FName, Name)
DAS_NATIVE_TYPE(FText, Text)
DAS_NATIVE_TYPE(FVector, Vector)
DAS_NATIVE_TYPE(FVector2D, Vector2D)
DAS_NATIVE_TYPE(FRotator, Rotator)
DAS_NATIVE_TYPE(FQuat, Quat)
DAS_NATIVE_TYPE(FTransform, Transform)
DAS_NATIVE_TYPE(FLinearColor, LinearColor)
DAS_NATIVE_TYPE(FColor, Color)
#undef DAS_NATIVE_TYPE

// Calls the bound function: Args[i] points to a value of the i-th parameter type, Result to a constructed value of
// the result type (ignored for void).
using FDasNativeThunk = void (*)(void* const* Args, void* Result);

class DASRUNTIME_API FDasNativeModule
{
public:
    static constexpr int32 MaxArguments = 16;

    template <auto Fn> FDasNativeModule& Function(const TCHAR* ScriptName, EDasBind Flags = EDasBind::None)
    {
        using FSignature = TSignature<decltype(Fn)>;
        static_assert(FSignature::Count <= MaxArguments, "Native binds take at most 16 parameters");
        Add(ScriptName, &FSignature::template Thunk<Fn>, FSignature::Result, FSignature::Arguments(), Flags);
        return *this;
    }

private:
    template <class F> struct TSignature;
    template <class R, class... A> struct TSignature<R (*)(A...)>
    {
        static constexpr int32 Count = sizeof...(A);
        static constexpr EDasNativeType Result = TDasNativeType<std::decay_t<R>>::Value;
        static TArray<EDasNativeType> Arguments() { return {TDasNativeType<std::decay_t<A>>::Value...}; }
        template <auto Fn> static void Thunk(void* const* Args, void* Out)
        {
            Call<Fn>(Args, Out, std::index_sequence_for<A...>());
        }
        template <auto Fn, size_t... I> static void Call(void* const* Args, void* Out, std::index_sequence<I...>)
        {
            if constexpr (std::is_void_v<R>)
                Fn(*static_cast<std::decay_t<A>*>(Args[I])...);
            else
                *static_cast<std::decay_t<R>*>(Out) = Fn(*static_cast<std::decay_t<A>*>(Args[I])...);
        }
    };
    void Add(const TCHAR* ScriptName, FDasNativeThunk Thunk, EDasNativeType Result, TArray<EDasNativeType> Args,
             EDasBind Flags);
    friend struct FDasNativeModuleBuilder;
    void* Module = nullptr; // the module being described (DasRuntime internal)
};

// Registers a module; Build runs on the game thread before the next script compilation. Status of a module: empty
// when it is available to scripts, otherwise why it is not (registration error, unloaded, not built yet).
DASRUNTIME_API void RegisterDasNativeModule(const TCHAR* ModuleName, void (*Build)(FDasNativeModule&));
DASRUNTIME_API void UnregisterDasNativeModule(const TCHAR* ModuleName);
DASRUNTIME_API FString DasNativeModuleStatus(const TCHAR* ModuleName);

struct FDasNativeBindsRegistrar
{
    FString Name;
    FDasNativeBindsRegistrar(const TCHAR* ModuleName, void (*Build)(FDasNativeModule&)) : Name(ModuleName)
    {
        RegisterDasNativeModule(ModuleName, Build);
    }
    ~FDasNativeBindsRegistrar() { UnregisterDasNativeModule(*Name); }
};
// Defines a native module in a .cpp file; the body receives `FDasNativeModule& Binds`.
#define DAS_NATIVE_BINDS(Id, ModuleName)                                                                               \
    static void DasNativeBinds_##Id(FDasNativeModule& Binds);                                                          \
    static const FDasNativeBindsRegistrar DasNativeBindsRegistrar_##Id(TEXT(ModuleName), &DasNativeBinds_##Id);        \
    static void DasNativeBinds_##Id(FDasNativeModule& Binds)
