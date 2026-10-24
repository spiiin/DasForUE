#pragma once
#include "DasNativeBinds.h"

// Registered native modules (DasNativeBinds.h). Descriptions are never freed: compiled scripts keep pointers to
// them, and an unregistered module only turns its functions into script errors.

namespace das
{
struct TypeAnnotation;
}
struct FDasNativeModuleDesc;
// A class with its own script type (Class<T>(Name)): owned by the module that declared it. Never freed: compiled
// scripts and other modules refer to it.
struct FDasNativeClassType
{
    FString Name;
    UClass* Class = nullptr;
    FDasNativeModuleDesc* Owner = nullptr;
    das::TypeAnnotation* Annotation = nullptr; // set when the owner's daScript module is built
    void* AotCast = nullptr;                   // the AOT entry of as_<Name> (FDasNativeModule::AotCast)
};
// An object parameter or result resolved when the daScript module is built: the class an object must have and
// its script type (nullptr: passed as UObjectHandle).
struct FDasNativeResolvedParam
{
    UClass* Class = nullptr;
    const FDasNativeClassType* Script = nullptr;
};

// What a function does: calls the bound C++ code or is one of the helpers generated for a class type.
enum class EDasNativeHelper : uint8
{
    Call,
    Valid,    // x.valid()
    Handle,   // x.handle()
    Cast,     // as_<Name>(x)
    Equal,    // x == y
    NotEqual, // x != y
};
struct FDasNativeFunctionDesc
{
    FString Name;
    FDasNativeThunk Thunk = nullptr;
    void* AotEntry = nullptr; // DasNativeBinds.h AotInvoke, when every type has an AOT ABI
    FDasNativeParam Result;
    TArray<FDasNativeParam> Arguments; // a method's receiver is Arguments[0]
    EDasBind Flags = EDasBind::None;
    FDasNativeClassFn Receiver = nullptr; // methods: the bound class
    EDasNativeHelper Helper = EDasNativeHelper::Call;
    TArray<FDasNativeResolvedParam> Resolved; // per argument, then the result; filled when built
    FString ReflectedName; // Reflects: the UFUNCTION of the receiver class
    FString ReflectedPath; // its path, checked when the module is described
    FString RouteName;     // its route in ue_foundation (gameplay scripts call it), set when the module is built
    bool IsMethod() const { return Receiver != nullptr; }
};
struct FDasNativeClassDecl
{
    FString ScriptName;
    FDasNativeClassFn Class = nullptr;
    void* AotCast = nullptr;
};
struct FDasNativeModuleDesc
{
    FString Name;
    void (*Build)(FDasNativeModule&) = nullptr;
    TArray<FDasNativeFunctionDesc> Functions;
    TArray<FDasNativeClassDecl> Classes;
    // Helpers and inherited methods made when the daScript module is built.
    TArray<TUniquePtr<FDasNativeFunctionDesc>> Generated;
    FString Error;           // why the module is not available
    bool bLoaded = true;     // false once its code module unregistered it
    bool bBuilt = false;     // the daScript module exists
    bool bAttempted = false; // Build ran (successfully or not)
};
// Game thread only.
TArray<FDasNativeModuleDesc*>& DasNativeModuleDescs();
FDasNativeModuleDesc* FindDasNativeModuleDesc(const FString& Name);
// The bind that reflects UFUNCTION Function for a receiver of class Receiver (built modules only), or nullptr.
struct FDasNativeRoute
{
    const FDasNativeModuleDesc* Module = nullptr;
    const FDasNativeFunctionDesc* Function = nullptr;
};
FDasNativeRoute FindDasNativeRoute(const UFunction* Function, const UClass* Receiver);
uint64& DasNativeRoutedCallCount(); // DasNativeRoutedCalls
// The error a running native bind call collects (DasNativeError); nullptr outside such a call.
FString*& DasNativeErrorSlot();
// The error of a call from AOT code, reported by dasue::aot::Finish after the call.
FString& DasNativeAotPendingError();
struct FDasNativeModuleBuilder
{
    // Runs the module's Build and validates names; fills Desc.Error on failure.
    static void Describe(FDasNativeModuleDesc& Desc);
};
