#include "DasNativeBindsRegistry.h"

namespace
{
bool IsScriptIdentifier(const FString& Name)
{
    if (Name.IsEmpty() || FChar::IsDigit(Name[0]) || Name.Len() >= NAME_SIZE)
        return false;
    for (TCHAR Ch : Name)
        if (!(FChar::IsAlnum(Ch) && Ch < 128) && Ch != TEXT('_'))
            return false;
    return true;
}
}

TArray<FDasNativeModuleDesc*>& DasNativeModuleDescs()
{
    static TArray<FDasNativeModuleDesc*> Descs;
    return Descs;
}
FDasNativeModuleDesc* FindDasNativeModuleDesc(const FString& Name)
{
    for (FDasNativeModuleDesc* Desc : DasNativeModuleDescs())
        if (Desc->Name == Name && Desc->bLoaded)
            return Desc;
    return nullptr;
}

void RegisterDasNativeModule(const TCHAR* ModuleName, void (*Build)(FDasNativeModule&))
{
    // Static initializers of code modules may run before the engine: only record here, build on first compile.
    auto* Desc = new FDasNativeModuleDesc();
    Desc->Name = ModuleName ? ModuleName : TEXT("");
    Desc->Build = Build;
    if (!IsScriptIdentifier(Desc->Name))
        Desc->Error = FString::Printf(TEXT("Native module name '%s' is not a script identifier"), *Desc->Name);
    else if (FindDasNativeModuleDesc(Desc->Name))
        Desc->Error = FString::Printf(TEXT("Native module '%s' is already registered"), *Desc->Name);
    DasNativeModuleDescs().Add(Desc);
}
void UnregisterDasNativeModule(const TCHAR* ModuleName)
{
    for (FDasNativeModuleDesc* Desc : DasNativeModuleDescs())
        if (Desc->Name == ModuleName && Desc->bLoaded)
        {
            Desc->bLoaded = false;
            Desc->Error = FString::Printf(TEXT("Native module '%s' was unloaded"), ModuleName);
        }
}
FString DasNativeModuleStatus(const TCHAR* ModuleName)
{
    FString Last = FString::Printf(TEXT("Native module '%s' is not registered"), ModuleName);
    for (FDasNativeModuleDesc* Desc : DasNativeModuleDescs())
        if (Desc->Name == ModuleName)
        {
            if (Desc->bLoaded && Desc->bBuilt && Desc->Error.IsEmpty())
                return FString();
            Last = !Desc->Error.IsEmpty() ? Desc->Error
                                          : FString::Printf(TEXT("Native module '%s' is not built yet"), ModuleName);
        }
    return Last;
}

void FDasNativeModule::Add(const TCHAR* ScriptName, FDasNativeThunk Thunk, EDasNativeType Result,
                           TArray<EDasNativeType> Args, EDasBind Flags)
{
    auto& Desc = *static_cast<FDasNativeModuleDesc*>(Module);
    if (!Desc.Error.IsEmpty())
        return;
    FDasNativeFunctionDesc F;
    F.Name = ScriptName ? ScriptName : TEXT("");
    F.Thunk = Thunk;
    F.Result = Result;
    F.Arguments = MoveTemp(Args);
    F.Flags = Flags;
    if (!IsScriptIdentifier(F.Name))
        Desc.Error = FString::Printf(TEXT("Native module '%s': function name '%s' is not a script identifier"),
                                          *Desc.Name, *F.Name);
    else if (Desc.Functions.ContainsByPredicate([&](const FDasNativeFunctionDesc& G) { return G.Name == F.Name; }))
        // Overloads by parameter types are a later step; one name per module keeps errors readable.
        Desc.Error = FString::Printf(TEXT("Native module '%s': function '%s' is bound twice"), *Desc.Name, *F.Name);
    for (EDasNativeType Type : F.Arguments)
        if (Type == EDasNativeType::Void)
            Desc.Error =
                FString::Printf(TEXT("Native module '%s': function '%s' has a void parameter"), *Desc.Name, *F.Name);
    if (Desc.Error.IsEmpty())
        Desc.Functions.Add(MoveTemp(F));
}

void FDasNativeModuleBuilder::Describe(FDasNativeModuleDesc& Desc)
{
    Desc.bAttempted = true;
    if (!Desc.Error.IsEmpty() || !Desc.Build)
        return;
    FDasNativeModule Module;
    Module.Module = &Desc;
    Desc.Build(Module);
    if (Desc.Error.IsEmpty() && Desc.Functions.IsEmpty())
        Desc.Error = FString::Printf(TEXT("Native module '%s' binds no functions"), *Desc.Name);
}
