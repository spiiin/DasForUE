#pragma once
#include "CoreMinimal.h"
#include "DasScriptValues.h"
#include "Misc/DateTime.h"
#include "Misc/Timespan.h"
#include "Templates/SubclassOf.h"
#include "UObject/Object.h"
#include <tuple>
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
//         Binds.Class<AMyPickup>(TEXT("AMyPickup"))
//             .Method<&AMyPickup::GetAmount>(TEXT("get_amount"))
//             .Method<+[](AMyPickup& Self, int32 Bonus) { Self.Grant(Bonus); }>(TEXT("grant"));
//     }
//
// The script sees `damage_for(base:int; multiplier:float):int`, a type `AMyPickup` and the methods as functions
// whose first parameter is the receiver: `pickup.get_amount()`. Calls run on the game thread under the same
// guards as the plugin's own externs. Only engine types appear here: daScript itself stays inside DasRuntime.
//
// Objects. `T*` parameters and results (T derived from UObject) are object references. A class bound with
// Class<T>(Name) has its own script type; other classes are passed as UObjectHandle and checked when called.
// Name the type as the C++ class (AMyPickup, like ue_engine's AActor): short names such as Pickup can clash with
// DSL base classes (ue_dsl) and with generated wrapper aliases in a script that requires both.
// For every class type the module also binds: `as_<Name>(x)` (from UObjectHandle, an ancestor or a descendant
// type; null when the object is not a <Name>), `x.handle()` (UObjectHandle), `x.valid()`, `==` and `!=`.
// `var x : <Name>` is a null reference. Methods of ancestor classes are callable on descendant types.
// References are weak: a script does not keep objects alive, a destroyed or collected object reads as null.
// A null or expired receiver is a script error; a null or expired argument reaches C++ as nullptr (see
// EDasBind::NonNullArgs).
// Classes. A `UClass*` or `TSubclassOf<T>` parameter is a UEClassHandle in the script; a null handle reaches C++ as
// nullptr, a class that is not a T is a script error. Results cannot be classes.
// Arrays. A `TArray<T*>` result (T derived from UObject) is `array<T>` in the script (the class's script type, or
// UObjectHandle); parameters cannot be arrays. Functions with an array result stay in the interpreter under AOT.
//
// Function names may also be operators (+ - * / % == != < <= > >=; "-" with one parameter is the unary minus;
// += -= *= /= %= take their first parameter by reference).
// A bound function reports a script error with DasNativeError (bad arguments, failed parsing, ...).
//
// References. A `T&` parameter (not const) of a value type - a scalar, FString, FDateTime, FTimespan or a math
// value - is a `var x : T&` parameter in the script: the caller passes a variable and sees the function's changes
// (setters such as SetLocation, output parameters such as ToAxisAndAngle). The value is copied in and written
// back after the call, only when the call succeeds: a call that reports DasNativeError leaves it unchanged. Objects,
// FName and FText cannot be references; Pure functions and Reflects methods take no references.

// Types of parameters and results. A const reference counts as its value type.
enum class EDasNativeType : uint8
{
    Void,
    Bool,
    Int32,
    Int64,
    Float,
    Double,
    Byte,
    String,
    Name,
    Text,
    DateTime, // script type UEDateTime (ticks)
    Timespan, // script type UETimespan (ticks)
    Object,
    Class,       // UClass* / TSubclassOf<T> parameter: script type UEClassHandle
    ObjectArray, // TArray<T*> result: script type array<T>
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
                  "Unsupported native bind type: use bool, int32, int64, float, double, uint8, FString, FName, "
                  "FText, FDateTime, FTimespan, FVector, FVector2D, FRotator, FQuat, FTransform, FLinearColor, "
                  "FColor, a pointer to a UObject class, UClass* / TSubclassOf<T> or a TArray<T*> result");
};
#define DAS_NATIVE_TYPE(CppType, Kind)                                                                                 \
    template <> struct TDasNativeType<CppType>                                                                         \
    {                                                                                                                  \
        static constexpr EDasNativeType Value = EDasNativeType::Kind;                                                  \
    };
DAS_NATIVE_TYPE(void, Void)
DAS_NATIVE_TYPE(bool, Bool)
DAS_NATIVE_TYPE(int32, Int32)
DAS_NATIVE_TYPE(int64, Int64)
DAS_NATIVE_TYPE(float, Float)
DAS_NATIVE_TYPE(double, Double)
DAS_NATIVE_TYPE(uint8, Byte)
DAS_NATIVE_TYPE(FString, String)
DAS_NATIVE_TYPE(FName, Name)
DAS_NATIVE_TYPE(FText, Text)
DAS_NATIVE_TYPE(FDateTime, DateTime)
DAS_NATIVE_TYPE(FTimespan, Timespan)
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
    FDasNativeClassFn Class = nullptr; // Object: its class; Class: the base; ObjectArray: the element class
    bool bRef = false;                 // a T& parameter: the script passes a variable the function may change
};
// A T& parameter that is not const.
template <class T>
constexpr bool TDasNativeIsRef = std::is_lvalue_reference_v<T> && !std::is_const_v<std::remove_reference_t<T>>;
template <class T> struct TDasNativeArrayOf
{
    using Element = void;
};
template <class T> struct TDasNativeArrayOf<TArray<T*>>
{
    using Element = std::remove_cv_t<T>;
};
template <class T> struct TDasNativeSubclassOf
{
    using Base = void;
};
template <class T> struct TDasNativeSubclassOf<TSubclassOf<T>>
{
    using Base = T;
};
template <class T> FDasNativeParam DasNativeParamOf()
{
    using D = std::decay_t<T>;
    static_assert(!TDasNativeIsRef<T> || (!std::is_same_v<D, UClass*> && !TIsTArray<D>::Value &&
                                          std::is_void_v<typename TDasNativeSubclassOf<D>::Base>),
                  "Classes and arrays cannot be references");
    if constexpr (std::is_same_v<D, UClass*>)
        return {EDasNativeType::Class, &UObject::StaticClass};
    else if constexpr (!std::is_void_v<typename TDasNativeSubclassOf<D>::Base>)
        return {EDasNativeType::Class, &TDasNativeSubclassOf<D>::Base::StaticClass};
    else if constexpr (TIsTArray<D>::Value)
    {
        using E = typename TDasNativeArrayOf<D>::Element;
        static_assert(std::is_base_of_v<UObject, E>, "Native bind arrays must be TArray<T*> of a UObject class");
        return {EDasNativeType::ObjectArray, &E::StaticClass};
    }
    else if constexpr (std::is_pointer_v<D>)
    {
        using C = std::remove_cv_t<std::remove_pointer_t<D>>;
        static_assert(std::is_base_of_v<UObject, C>, "Native bind pointers must point to UObject classes");
        static_assert(!TDasNativeIsRef<T>, "Object parameters cannot be references");
        return {EDasNativeType::Object, &C::StaticClass};
    }
    else
    {
        static_assert(!TDasNativeIsRef<T> || !(std::is_same_v<D, FName> || std::is_same_v<D, FText>),
                      "FName and FText parameters cannot be references");
        return {TDasNativeType<D>::Value, nullptr, TDasNativeIsRef<T>};
    }
}

namespace das
{
class Context;
}

// AOT (DasUEAot.h): a function whose parameters and result all have an AOT ABI gets an entry that generated code
// calls through the guarded bridge; the others keep the interpreter (FName, FText and operators for now).
DASRUNTIME_API char* DasNativeAotString(const FString& Value, das::Context* Context);
DASRUNTIME_API FString* DasNativeAotEnter();           // the error slot of an AOT call; returns the outer one
DASRUNTIME_API void DasNativeAotLeave(FString* Outer); // restores it; dasue::aot::Finish reports the error
DASRUNTIME_API bool DasNativeAotFailed();              // the current AOT call has reported an error
// Objects cross the bridge as their handles and are checked as the interpreter checks them: the class, a null or
// expired receiver, and a null argument of a NonNullArgs function fail the call (Number: the argument's number in
// the error). The result's handle is made for its class; an object of another class reads as null.
enum class EDasNativeAotObject : uint8
{
    Argument,
    NonNullArgument,
    Receiver
};
DASRUNTIME_API UObject* DasNativeAotObject(const FDasObjectHandle& Handle, UClass* Class, das::Context* Context,
                                           int32 Number, EDasNativeAotObject Kind);
DASRUNTIME_API FDasObjectHandle DasNativeAotHandle(const UObject* Object, UClass* Class, das::Context* Context);
struct FDasNativeAbiNone
{
};
// Arg: a parameter, RefArg: a T& parameter (copied in and written back with FromCpp), Result: the result.
template <class T> struct TDasNativeAbi
{
    static constexpr bool Supported = false;
    using Arg = FDasNativeAbiNone;
    using RefArg = FDasNativeAbiNone;
    using Result = FDasNativeAbiNone;
};
template <class T> struct TDasNativeAbiScalar
{
    static constexpr bool Supported = true;
    using Arg = T;
    using RefArg = T&;
    using Result = T;
    static T ToCpp(T Value) { return Value; }
    static T FromCpp(T Value, das::Context*) { return Value; }
};
template <> struct TDasNativeAbi<bool> : TDasNativeAbiScalar<bool>
{
};
template <> struct TDasNativeAbi<int32> : TDasNativeAbiScalar<int32>
{
};
template <> struct TDasNativeAbi<int64> : TDasNativeAbiScalar<int64>
{
};
template <> struct TDasNativeAbi<float> : TDasNativeAbiScalar<float>
{
};
template <> struct TDasNativeAbi<double> : TDasNativeAbiScalar<double>
{
};
template <> struct TDasNativeAbi<uint8> : TDasNativeAbiScalar<uint8>
{
};
template <class T> struct TDasNativeAbiValue
{
    static constexpr bool Supported = true;
    using Arg = const T&;
    using RefArg = T&;
    using Result = T;
    static const T& ToCpp(const T& Value) { return Value; }
    static T FromCpp(const T& Value, das::Context*) { return Value; }
};
template <> struct TDasNativeAbi<FVector> : TDasNativeAbiValue<FVector>
{
};
template <> struct TDasNativeAbi<FVector2D> : TDasNativeAbiValue<FVector2D>
{
};
template <> struct TDasNativeAbi<FRotator> : TDasNativeAbiValue<FRotator>
{
};
template <> struct TDasNativeAbi<FQuat> : TDasNativeAbiValue<FQuat>
{
};
template <> struct TDasNativeAbi<FTransform> : TDasNativeAbiValue<FTransform>
{
};
template <> struct TDasNativeAbi<FLinearColor> : TDasNativeAbiValue<FLinearColor>
{
};
template <> struct TDasNativeAbi<FColor> : TDasNativeAbiValue<FColor>
{
};
template <> struct TDasNativeAbi<FString>
{
    static constexpr bool Supported = true;
    using Arg = const char*;
    using RefArg = char*&;
    using Result = char*;
    static FString ToCpp(const char* Value) { return Value ? FString(UTF8_TO_TCHAR(Value)) : FString(); }
    static char* FromCpp(const FString& Value, das::Context* Context) { return DasNativeAotString(Value, Context); }
};
template <> struct TDasNativeAbi<FDateTime>
{
    static constexpr bool Supported = true;
    using Arg = FDasDateTimeValue;
    using RefArg = FDasDateTimeValue&;
    using Result = FDasDateTimeValue;
    static FDateTime ToCpp(FDasDateTimeValue Value) { return FDateTime(Value.Ticks); }
    static FDasDateTimeValue FromCpp(const FDateTime& Value, das::Context*) { return {Value.GetTicks()}; }
};
template <> struct TDasNativeAbi<FTimespan>
{
    static constexpr bool Supported = true;
    using Arg = FDasTimespanValue;
    using RefArg = FDasTimespanValue&;
    using Result = FDasTimespanValue;
    static FTimespan ToCpp(FDasTimespanValue Value) { return FTimespan(Value.Ticks); }
    static FDasTimespanValue FromCpp(const FTimespan& Value, das::Context*) { return {Value.GetTicks()}; }
};
// A class argument: the handle; DasNativeAotClass checks it (null passes as nullptr).
DASRUNTIME_API UClass* DasNativeAotClass(FDasClassHandle Handle, UClass* Base, das::Context* Context, int32 Number);
struct FDasNativeAbiClass
{
    static constexpr bool Supported = true;
    using Arg = FDasClassHandle;
    using RefArg = FDasNativeAbiNone;
    using Result = FDasNativeAbiNone;
};
template <> struct TDasNativeAbi<UClass*> : FDasNativeAbiClass
{
};
template <class T> struct TDasNativeAbi<TSubclassOf<T>> : FDasNativeAbiClass
{
};
template <class T> struct TDasNativeAbi<T*>
{
    static constexpr bool Supported = std::is_base_of_v<UObject, std::remove_cv_t<T>>;
    using Arg = const FDasObjectHandle&;
    using RefArg = FDasNativeAbiNone;
    using Result = FDasObjectHandle;
    static FDasObjectHandle FromCpp(const UObject* Value, das::Context* Context)
    {
        return DasNativeAotHandle(Value, std::remove_cv_t<T>::StaticClass(), Context);
    }
};
template <class R> struct TDasNativeAotResult
{
    using Type = typename TDasNativeAbi<std::decay_t<R>>::Result;
};
template <> struct TDasNativeAotResult<void>
{
    using Type = void;
};

// The body of an AOT entry. Converts every argument first (objects are checked); when one fails, the C++
// function does not run and the bridge reports the error. bMethod: the first argument is the receiver. A reference
// is a copy, written back when the call succeeds (as in the interpreter).
template <class R, class... A> struct TDasNativeAot
{
    static constexpr bool Supported = (std::is_void_v<R> || TDasNativeAbi<std::decay_t<R>>::Supported) &&
                                      (TDasNativeAbi<std::decay_t<A>>::Supported && ...);
    using Result = typename TDasNativeAotResult<R>::Type;
    template <class T>
    using TArg = std::conditional_t<TDasNativeIsRef<T>, typename TDasNativeAbi<std::decay_t<T>>::RefArg,
                                    typename TDasNativeAbi<std::decay_t<T>>::Arg>;
    template <class T>
    static decltype(auto) Convert(TArg<T> Value, das::Context* Context, int32 Number, EDasNativeAotObject Kind)
    {
        using D = std::decay_t<T>;
        if constexpr (TDasNativeIsRef<T>)
            return D(TDasNativeAbi<D>::ToCpp(Value));
        else if constexpr (std::is_same_v<D, UClass*>)
            return DasNativeAotClass(Value, UObject::StaticClass(), Context, Number);
        else if constexpr (!std::is_void_v<typename TDasNativeSubclassOf<D>::Base>)
            return D(DasNativeAotClass(Value, TDasNativeSubclassOf<D>::Base::StaticClass(), Context, Number));
        else if constexpr (std::is_pointer_v<D>)
            return static_cast<D>(
                DasNativeAotObject(Value, std::remove_cv_t<std::remove_pointer_t<D>>::StaticClass(), Context, Number,
                                   Kind));
        else
            return TDasNativeAbi<D>::ToCpp(Value);
    }
    static EDasNativeAotObject KindOf(bool bMethod, bool bNonNull, size_t Index)
    {
        return bMethod && Index == 0 ? EDasNativeAotObject::Receiver
               : bNonNull            ? EDasNativeAotObject::NonNullArgument
                                     : EDasNativeAotObject::Argument;
    }
    template <class F> static Result Run(bool bMethod, bool bNonNull, das::Context* Context, F Call, TArg<A>... Args)
    {
        return RunAt(bMethod, bNonNull, Context, Call, std::index_sequence_for<A...>(), Args...);
    }
    template <class F, size_t... I>
    static Result RunAt(bool bMethod, bool bNonNull, das::Context* Context, F Call, std::index_sequence<I...>,
                        TArg<A>... Args)
    {
        FString* Outer = DasNativeAotEnter();
        std::tuple<decltype(Convert<A>(Args, Context, 0, EDasNativeAotObject::Argument))...> Values{
            Convert<A>(Args, Context, bMethod ? int32(I) : int32(I) + 1, KindOf(bMethod, bNonNull, I))...};
        if (DasNativeAotFailed())
        {
            DasNativeAotLeave(Outer);
            if constexpr (!std::is_void_v<R>)
                return Result{};
        }
        else if constexpr (std::is_void_v<R>)
        {
            Call(std::get<I>(Values)...);
            if (!DasNativeAotFailed())
                (WriteBack<A>(Args, std::get<I>(Values), Context), ...);
            DasNativeAotLeave(Outer);
        }
        else
        {
            auto Value = Call(std::get<I>(Values)...);
            if (!DasNativeAotFailed())
                (WriteBack<A>(Args, std::get<I>(Values), Context), ...);
            DasNativeAotLeave(Outer);
            return TDasNativeAbi<std::decay_t<R>>::FromCpp(Value, Context);
        }
    }
    template <class T, class V> static void WriteBack(TArg<T> Arg, const V& Value, das::Context* Context)
    {
        if constexpr (TDasNativeIsRef<T>)
            Arg = TDasNativeAbi<std::decay_t<T>>::FromCpp(Value, Context);
        else
        {
            (void)Arg;
            (void)Value;
            (void)Context;
        }
    }
};

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
        Add(ScriptName, &S::template Thunk<Fn>, S::Result(), S::Arguments(), Flags, nullptr,
            S::template AotEntry<Fn>(Flags));
        return *this;
    }
    // Class<T>(Name) declares the script type Name for T (one module owns a class); Class<T>() adds methods to
    // the type another module declared.
    template <class T> TDasNativeClass<T> Class(const TCHAR* ScriptName = nullptr)
    {
        static_assert(std::is_base_of_v<UObject, T>, "Class<T> needs a UObject class");
        if (ScriptName)
            AddClass(ScriptName, &T::StaticClass, reinterpret_cast<void*>(&AotCast<T>));
        return TDasNativeClass<T>(*this);
    }

private:
    // The AOT entry of as_<Name> (any object to T, null when it is not a T): no checks, as in the interpreter.
    template <class T> static FDasObjectHandle AotCast(const FDasObjectHandle& Value, das::Context* Context)
    {
        const UObject* Object =
            DasNativeAotObject(Value, UObject::StaticClass(), Context, 0, EDasNativeAotObject::Argument);
        return DasNativeAotHandle(Object, T::StaticClass(), Context);
    }
    template <class A> static decltype(auto) Arg(void* P)
    {
        using D = std::decay_t<A>;
        if constexpr (!std::is_void_v<typename TDasNativeSubclassOf<D>::Base>)
            return D(*static_cast<UClass**>(P));
        else if constexpr (std::is_pointer_v<D>)
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
        else if constexpr (TIsTArray<std::decay_t<R>>::Value)
        {
            // An array result: the slot is a TArray<UObject*>.
            const auto Values = Call();
            auto& Objects = *static_cast<TArray<UObject*>*>(Out);
            Objects.Reset(Values.Num());
            for (const auto* Value : Values)
                Objects.Add(const_cast<UObject*>(static_cast<const UObject*>(Value)));
        }
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
        // The AOT entry: ABI arguments and the context in, the ABI result out (nullptr when a type has no ABI).
        using Aot = TDasNativeAot<R, A...>;
        template <auto Fn, bool bNonNull>
        static typename Aot::Result AotInvoke(typename Aot::template TArg<A>... Args, das::Context* Context)
        {
            return Aot::Run(false, bNonNull, Context, [](auto&... V) -> decltype(auto) { return Fn(V...); },
                            Args...);
        }
        template <auto Fn> static void* AotEntry(EDasBind Flags)
        {
            if constexpr (Aot::Supported)
                return EnumHasAnyFlags(Flags, EDasBind::NonNullArgs) ? reinterpret_cast<void*>(&AotInvoke<Fn, true>)
                                                                     : reinterpret_cast<void*>(&AotInvoke<Fn, false>);
            else
                return nullptr;
        }
    };
    // The AOT entry of a method of the bound class T: the receiver's handle first.
    template <class R, class T, class... A> struct TMethodAot
    {
        using Aot = TDasNativeAot<R, T*, A...>;
        // Caller::Call(T* Receiver, Arguments...) runs the method.
        template <class Caller, bool bNonNull>
        static typename Aot::Result Invoke(const FDasObjectHandle& Self, typename Aot::template TArg<A>... Args,
                                           das::Context* Context)
        {
            auto Call = [](T* Object, auto&... V) -> decltype(auto) { return Caller::Call(Object, V...); };
            return Aot::Run(true, bNonNull, Context, Call, Self, Args...);
        }
        template <class Caller> static void* Entry(EDasBind Flags)
        {
            if constexpr (Aot::Supported)
                return EnumHasAnyFlags(Flags, EDasBind::NonNullArgs)
                           ? reinterpret_cast<void*>(&Invoke<Caller, true>)
                           : reinterpret_cast<void*>(&Invoke<Caller, false>);
            else
                return nullptr;
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
        template <auto Fn, class T> struct TAotCaller
        {
            template <class... V> static decltype(auto) Call(T* Object, V&... Values)
            {
                return (static_cast<C*>(Object)->*Fn)(Values...);
            }
        };
        template <auto Fn, class T> static void* AotEntry(EDasBind Flags)
        {
            return TMethodAot<R, T, A...>::template Entry<TAotCaller<Fn, T>>(Flags);
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
        template <auto Fn, class T> struct TAotCaller
        {
            template <class... V> static decltype(auto) Call(T* Object, V&... Values)
            {
                if constexpr (std::is_pointer_v<Self>)
                    return Fn(static_cast<Receiver*>(Object), Values...);
                else
                    return Fn(*static_cast<Receiver*>(Object), Values...);
            }
        };
        template <auto Fn, class T> static void* AotEntry(EDasBind Flags)
        {
            return TMethodAot<R, T, A...>::template Entry<TAotCaller<Fn, T>>(Flags);
        }
    };
    template <class T> friend class TDasNativeClass;

    void Add(const TCHAR* ScriptName, FDasNativeThunk Thunk, FDasNativeParam Result, TArray<FDasNativeParam> Args,
             EDasBind Flags, FDasNativeClassFn Receiver, void* AotEntry = nullptr);
    // AotCast: the AOT entry of the class's as_<Name> helper.
    void AddClass(const TCHAR* ScriptName, FDasNativeClassFn Class, void* AotCast = nullptr);
    void SetReflects(const TCHAR* FunctionName);
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
        Module.Add(ScriptName, &S::template Thunk<Fn>, S::Result(), MoveTemp(Args), Flags, &T::StaticClass,
                   S::template AotEntry<Fn, T>(Flags));
        return *this;
    }
    // The method added last does what the UFUNCTION FunctionName of T does. Gameplay scripts that call that
    // UFUNCTION on a native reference (`self.Camera.K2_GetComponentLocation()`) then call this bind instead of
    // going through reflection: the same syntax, the same receiver checks, without the call frame. Checked when
    // the module is built: the UFUNCTION must exist, be BlueprintCallable and native, not an event, static,
    // network or editor-only function, and take and return the same types, every parameter an input; the bind
    // must not use EDasBind::NonNullArgs (reflection passes nullptr). One bind per UFUNCTION. UFUNCTIONs that
    // need authority or are unsafe in construction scripts keep the reflected call.
    TDasNativeClass& Reflects(const TCHAR* FunctionName)
    {
        Module.SetReflects(FunctionName);
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
// Called from a bound function: when it returns, the script call fails with "<function>: <Message>" (the result
// is ignored). Game thread, only during a native bind call; the first message of a call wins.
DASRUNTIME_API void DasNativeError(const FString& Message);
// How many gameplay method calls binds answered instead of reflection (Reflects) since the start; for tests.
DASRUNTIME_API uint64 DasNativeRoutedCalls();

// Read-only description of the registered native modules (manifests, coverage reports, documentation). Builds the
// modules registered since the last compilation; game thread.
struct FDasNativeFunctionInfo
{
    FString Name;
    FString Receiver;          // script type of a method's receiver; empty for a function
    TArray<FString> Arguments; // script types, a method's receiver first
    FString Result;
    FString Origin; // "declared", "inherited" (an ancestor's method on a descendant type) or "helper"
    bool bPure = false;
    bool bNonNullArgs = false;
    FString Reflects; // path of the UFUNCTION it reflects (Reflects), empty when none
    // Gameplay scripts call it instead of the UFUNCTION; false with Reflects when the UFUNCTION's policy keeps
    // the reflected call (RouteNote says which).
    bool bRouted = false;
    FString RouteNote;
};
struct FDasNativeModuleInfo
{
    FString Name;
    FString Error;           // empty when the module is available
    TArray<FString> Classes; // script types the module declares
    TArray<FDasNativeFunctionInfo> Functions;
};
DASRUNTIME_API TArray<FDasNativeModuleInfo> DescribeDasNativeModules();

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
