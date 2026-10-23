#pragma once
#include "CoreMinimal.h"
#include "UObject/Object.h"
#include <type_traits>
#include <utility>

// Native binds: C++ functions and methods of a game (or plugin) module called directly from daScript, without
// reflection or a call frame. A module registered with DAS_NATIVE_BINDS is available to scripts as
// `require <name>`:
//
//     static int32 DamageFor(int32 Base, float Multiplier);
//     DAS_NATIVE_BINDS(MyGame, "my_game")
//     {
//         Binds.Function<&DamageFor>(TEXT("damage_for"), EDasBind::Pure);
//         Binds.Class<AMyPickup>(TEXT("Pickup"))
//             .Method<&AMyPickup::GetAmount>(TEXT("get_amount"))
//             .Method<+[](AMyPickup& Self, int32 Bonus) { Self.Grant(Bonus); }>(TEXT("grant"));
//     }
//
// The script sees `damage_for(base:int; multiplier:float):int`, a type `Pickup` and the methods as functions
// whose first parameter is the receiver: `pickup.get_amount()`. Calls run on the game thread under the same
// guards as the plugin's own externs. Only engine types appear here: daScript itself stays inside DasRuntime.
//
// Objects. `T*` parameters and results (T derived from UObject) are object references. A class bound with
// Class<T>(Name) has its own script type; other classes are passed as UObjectHandle and checked when called.
// For every class type the module also binds: `as_<Name>(x)` (from UObjectHandle, an ancestor or a descendant
// type; null when the object is not a <Name>), `x.handle()` (UObjectHandle), `x.valid()`, `==` and `!=`.
// `var x : <Name>` is a null reference. Methods of ancestor classes are callable on descendant types.
// References are weak: a script does not keep objects alive, a destroyed or collected object reads as null.
// A null or expired receiver is a script error; a null or expired argument reaches C++ as nullptr (see
// EDasBind::NonNullArgs).

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
    Object,
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
    // compiles the script, outside any world. Never mark functions that read or change game state; not allowed
    // with object parameters or results.
    Pure = 1,
    // A null or expired object argument is a script error instead of nullptr.
    NonNullArgs = 2
};
ENUM_CLASS_FLAGS(EDasBind)

template <class T> struct TDasNativeType
{
    static_assert(sizeof(T) == 0,
                  "Unsupported native bind type: use bool, int32, float, double, uint8, FString, FName, "
                  "FText, FVector, FVector2D, FRotator, FQuat, FTransform, FLinearColor, FColor or a "
                  "pointer to a UObject class");
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

// StaticClass of an object parameter: registration runs before the engine, so classes are resolved later.
using FDasNativeClassFn = UClass* (*)();
struct FDasNativeParam
{
    EDasNativeType Type = EDasNativeType::Void;
    FDasNativeClassFn Class = nullptr; // Object only
};
template <class T> FDasNativeParam DasNativeParamOf()
{
    using D = std::decay_t<T>;
    if constexpr (std::is_pointer_v<D>)
    {
        using C = std::remove_cv_t<std::remove_pointer_t<D>>;
        static_assert(std::is_base_of_v<UObject, C>, "Native bind pointers must point to UObject classes");
        return {EDasNativeType::Object, &C::StaticClass};
    }
    else
        return {TDasNativeType<D>::Value, nullptr};
}

// Calls the bound function: Args[i] points to a value of the i-th parameter type (UObject* for objects; for a
// method Args[0] is the receiver), Result to a constructed value of the result type (UObject* for objects,
// ignored for void).
using FDasNativeThunk = void (*)(void* const* Args, void* Result);

template <class T> class TDasNativeClass;

class DASRUNTIME_API FDasNativeModule
{
public:
    static constexpr int32 MaxArguments = 16;

    template <auto Fn> FDasNativeModule& Function(const TCHAR* ScriptName, EDasBind Flags = EDasBind::None)
    {
        using S = TSignature<decltype(Fn)>;
        static_assert(S::Count <= MaxArguments, "Native binds take at most 16 parameters");
        Add(ScriptName, &S::template Thunk<Fn>, S::Result(), S::Arguments(), Flags, nullptr);
        return *this;
    }
    // Class<T>(Name) declares the script type Name for T (one module owns a class); Class<T>() adds methods to
    // the type another module declared.
    template <class T> TDasNativeClass<T> Class(const TCHAR* ScriptName = nullptr)
    {
        static_assert(std::is_base_of_v<UObject, T>, "Class<T> needs a UObject class");
        if (ScriptName)
            AddClass(ScriptName, &T::StaticClass);
        return TDasNativeClass<T>(*this);
    }

private:
    template <class A> static decltype(auto) Arg(void* P)
    {
        using D = std::decay_t<A>;
        if constexpr (std::is_pointer_v<D>)
            return static_cast<D>(*static_cast<UObject**>(P));
        else
            return *static_cast<D*>(P);
    }
    template <class R, class F> static void Store(void* Out, F&& Call)
    {
        if constexpr (std::is_void_v<R>)
            Call();
        else if constexpr (std::is_pointer_v<std::decay_t<R>>)
            *static_cast<UObject**>(Out) = const_cast<UObject*>(static_cast<const UObject*>(Call()));
        else
            *static_cast<std::decay_t<R>*>(Out) = Call();
    }
    template <class F> struct TSignature;
    template <class R, class... A> struct TSignature<R (*)(A...)>
    {
        static constexpr int32 Count = sizeof...(A);
        static FDasNativeParam Result() { return DasNativeParamOf<R>(); }
        static TArray<FDasNativeParam> Arguments() { return {DasNativeParamOf<A>()...}; }
        template <auto Fn> static void Thunk(void* const* Args, void* Out)
        {
            Call<Fn>(Args, Out, std::index_sequence_for<A...>());
        }
        template <auto Fn, size_t... I> static void Call(void* const* Args, void* Out, std::index_sequence<I...>)
        {
            Store<R>(Out, [&]() -> decltype(auto) { return Fn(Arg<A>(Args[I])...); });
        }
    };
    // Methods: Args[0] is the receiver (a UObject* of the bound class), the rest are the parameters.
    template <class R, class C, class... A> struct TMemberMethod
    {
        using Receiver = C;
        static constexpr int32 Count = sizeof...(A) + 1;
        static FDasNativeParam Result() { return DasNativeParamOf<R>(); }
        static TArray<FDasNativeParam> Arguments() { return {DasNativeParamOf<A>()...}; }
        template <auto Fn> static void Thunk(void* const* Args, void* Out)
        {
            Call<Fn>(Args, Out, std::index_sequence_for<A...>());
        }
        template <auto Fn, size_t... I> static void Call(void* const* Args, void* Out, std::index_sequence<I...>)
        {
            C* Self = static_cast<C*>(*static_cast<UObject**>(Args[0]));
            Store<R>(Out, [&]() -> decltype(auto) { return (Self->*Fn)(Arg<A>(Args[I + 1])...); });
        }
    };
    template <class F> struct TMethod;
    template <class R, class C, class... A> struct TMethod<R (C::*)(A...)> : TMemberMethod<R, C, A...>
    {
    };
    template <class R, class C, class... A> struct TMethod<R (C::*)(A...) const> : TMemberMethod<R, const C, A...>
    {
    };
    // A function whose first parameter (C* or C&) is the receiver.
    template <class R, class Self, class... A> struct TMethod<R (*)(Self, A...)>
    {
        static_assert(std::is_pointer_v<Self> || std::is_reference_v<Self>,
                      "The receiver parameter of a method function must be C* or C&");
        using Receiver = std::remove_pointer_t<std::remove_reference_t<Self>>;
        static constexpr int32 Count = sizeof...(A) + 1;
        static FDasNativeParam Result() { return DasNativeParamOf<R>(); }
        static TArray<FDasNativeParam> Arguments() { return {DasNativeParamOf<A>()...}; }
        template <auto Fn> static void Thunk(void* const* Args, void* Out)
        {
            Call<Fn>(Args, Out, std::index_sequence_for<A...>());
        }
        template <auto Fn, size_t... I> static void Call(void* const* Args, void* Out, std::index_sequence<I...>)
        {
            auto* Object = static_cast<Receiver*>(*static_cast<UObject**>(Args[0]));
            if constexpr (std::is_pointer_v<Self>)
                Store<R>(Out, [&]() -> decltype(auto) { return Fn(Object, Arg<A>(Args[I + 1])...); });
            else
                Store<R>(Out, [&]() -> decltype(auto) { return Fn(*Object, Arg<A>(Args[I + 1])...); });
        }
    };
    template <class T> friend class TDasNativeClass;

    void Add(const TCHAR* ScriptName, FDasNativeThunk Thunk, FDasNativeParam Result, TArray<FDasNativeParam> Args,
             EDasBind Flags, FDasNativeClassFn Receiver);
    void AddClass(const TCHAR* ScriptName, FDasNativeClassFn Class);
    friend struct FDasNativeModuleBuilder;
    void* Module = nullptr; // the module being described (DasRuntime internal)
};

template <class T> class TDasNativeClass
{
public:
    explicit TDasNativeClass(FDasNativeModule& InModule) : Module(InModule) {}
    // A member function of T or of its base, or a function whose first parameter is T* / T& (or a base). Overloaded
    // member functions are chosen with static_cast.
    template <auto Fn> TDasNativeClass& Method(const TCHAR* ScriptName, EDasBind Flags = EDasBind::None)
    {
        using S = FDasNativeModule::TMethod<decltype(Fn)>;
        using Receiver = std::remove_cv_t<typename S::Receiver>;
        static_assert(std::is_base_of_v<UObject, Receiver>, "The method's class must be a UObject class");
        static_assert(std::is_base_of_v<Receiver, T>, "The method's class must be the bound class or its base");
        static_assert(S::Count <= FDasNativeModule::MaxArguments, "Native binds take at most 16 parameters");
        TArray<FDasNativeParam> Args{FDasNativeParam{EDasNativeType::Object, &T::StaticClass}};
        Args.Append(S::Arguments());
        Module.Add(ScriptName, &S::template Thunk<Fn>, S::Result(), MoveTemp(Args), Flags, &T::StaticClass);
        return *this;
    }

private:
    FDasNativeModule& Module;
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
