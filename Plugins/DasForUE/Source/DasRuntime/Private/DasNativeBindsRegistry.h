#pragma once
#include "DasNativeBinds.h"

// Registered native modules (DasNativeBinds.h). Descriptions are never freed: compiled scripts keep pointers to
// them, and an unregistered module only turns its functions into script errors.
struct FDasNativeFunctionDesc
{
    FString Name;
    FDasNativeThunk Thunk = nullptr;
    EDasNativeType Result = EDasNativeType::Void;
    TArray<EDasNativeType> Arguments;
    EDasBind Flags = EDasBind::None;
};
struct FDasNativeModuleDesc
{
    FString Name;
    void (*Build)(FDasNativeModule&) = nullptr;
    TArray<FDasNativeFunctionDesc> Functions;
    FString Error;           // why the module is not available
    bool bLoaded = true;     // false once its code module unregistered it
    bool bBuilt = false;     // the daScript module exists
    bool bAttempted = false; // Build ran (successfully or not)
};
// Game thread only.
TArray<FDasNativeModuleDesc*>& DasNativeModuleDescs();
FDasNativeModuleDesc* FindDasNativeModuleDesc(const FString& Name);
struct FDasNativeModuleBuilder
{
    // Runs the module's Build and validates names; fills Desc.Error on failure.
    static void Describe(FDasNativeModuleDesc& Desc);
};
