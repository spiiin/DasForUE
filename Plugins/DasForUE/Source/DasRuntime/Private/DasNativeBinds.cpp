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
// Function names: identifiers or operators.
bool IsScriptFunctionName(const FString& Name)
{
    static const TArray<FString> Operators = []
    {
        TArray<FString> List;
        FString(TEXT("+ - * / % == != < <= > >=")).ParseIntoArray(List, TEXT(" "));
        return List;
    }();
    if (Operators.Contains(Name))
        return true;
    return IsScriptIdentifier(Name);
}
}

FString*& DasNativeErrorSlot()
{
    static FString* Slot = nullptr;
    return Slot;
}
FString& DasNativeAotPendingError()
{
    static FString Error;
    return Error;
}
FString* DasNativeAotEnter()
{
    FString* Outer = DasNativeErrorSlot();
    DasNativeErrorSlot() = &DasNativeAotPendingError();
    return Outer;
}
void DasNativeAotLeave(FString* Outer)
{
    DasNativeErrorSlot() = Outer;
}
void DasNativeError(const FString& Message)
{
    FString* Slot = DasNativeErrorSlot();
    if (!Slot)
    {
        UE_LOG(LogTemp, Warning, TEXT("DasNativeError outside a native bind call: %s"), *Message);
        return;
    }
    if (Slot->IsEmpty())
        *Slot = Message.IsEmpty() ? FString(TEXT("failed")) : Message;
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

static bool SameParam(const FDasNativeParam& A, const FDasNativeParam& B)
{
    return A.Type == B.Type && A.Class == B.Class;
}

void FDasNativeModule::Add(const TCHAR* ScriptName, FDasNativeThunk Thunk, FDasNativeParam Result,
                           TArray<FDasNativeParam> Args, EDasBind Flags, FDasNativeClassFn Receiver, void* AotEntry)
{
    auto& Desc = *static_cast<FDasNativeModuleDesc*>(Module);
    if (!Desc.Error.IsEmpty())
        return;
    FDasNativeFunctionDesc F;
    F.Name = ScriptName ? ScriptName : TEXT("");
    F.Thunk = Thunk;
    F.AotEntry = AotEntry;
    F.Result = Result;
    F.Arguments = MoveTemp(Args);
    F.Flags = Flags;
    F.Receiver = Receiver;
    const bool bObjects =
        Result.Type == EDasNativeType::Object ||
        F.Arguments.ContainsByPredicate([](const FDasNativeParam& P) { return P.Type == EDasNativeType::Object; });
    auto Fail = [&](const FString& Why)
    {
        Desc.Error = FString::Printf(TEXT("Native module '%s': function '%s' %s"), *Desc.Name, *F.Name, *Why);
    };
    if (!IsScriptFunctionName(F.Name))
        Fail(TEXT("has a name that is not a script identifier"));
    else if (Desc.Functions.ContainsByPredicate(
                 [&](const FDasNativeFunctionDesc& G)
                 {
                     if (G.Name != F.Name || G.Arguments.Num() != F.Arguments.Num())
                         return false;
                     for (int32 I = 0; I < G.Arguments.Num(); ++I)
                         if (!SameParam(G.Arguments[I], F.Arguments[I]))
                             return false;
                     return true;
                 }))
        // Overloads differ by parameter types; the same name and parameters twice would be ambiguous.
        Fail(TEXT("is bound twice"));
    else if (F.Arguments.ContainsByPredicate([](const FDasNativeParam& P) { return P.Type == EDasNativeType::Void; }))
        Fail(TEXT("has a void parameter"));
    else if (EnumHasAnyFlags(Flags, EDasBind::Pure) && bObjects)
        Fail(TEXT("is Pure but takes or returns objects"));
    if (Desc.Error.IsEmpty())
        Desc.Functions.Add(MoveTemp(F));
}
void FDasNativeModule::AddClass(const TCHAR* ScriptName, FDasNativeClassFn Class)
{
    auto& Desc = *static_cast<FDasNativeModuleDesc*>(Module);
    if (!Desc.Error.IsEmpty())
        return;
    const FString Name = ScriptName ? ScriptName : TEXT("");
    if (!IsScriptIdentifier(Name))
        Desc.Error =
            FString::Printf(TEXT("Native module '%s': class name '%s' is not a script identifier"), *Desc.Name, *Name);
    else if (Desc.Classes.ContainsByPredicate([&](const FDasNativeClassDecl& C)
                                              { return C.ScriptName == Name || C.Class == Class; }))
        Desc.Error = FString::Printf(TEXT("Native module '%s': class '%s' is declared twice"), *Desc.Name, *Name);
    else
        Desc.Classes.Add({Name, Class});
}

void FDasNativeModuleBuilder::Describe(FDasNativeModuleDesc& Desc)
{
    Desc.bAttempted = true;
    if (!Desc.Error.IsEmpty() || !Desc.Build)
        return;
    FDasNativeModule Module;
    Module.Module = &Desc;
    Desc.Build(Module);
    if (Desc.Error.IsEmpty() && Desc.Functions.IsEmpty() && Desc.Classes.IsEmpty())
        Desc.Error = FString::Printf(TEXT("Native module '%s' binds nothing"), *Desc.Name);
}
