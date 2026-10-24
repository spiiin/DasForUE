#include "DasNativeBindsRegistry.h"
#include "UObject/Class.h"
#include "UObject/StrProperty.h"
#include "UObject/TextProperty.h"
#include "UObject/UnrealType.h"

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
bool DasNativeAotFailed()
{
    return !DasNativeAotPendingError().IsEmpty();
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

uint64& DasNativeRoutedCallCount()
{
    static uint64 Count = 0;
    return Count;
}
uint64 DasNativeRoutedCalls()
{
    return DasNativeRoutedCallCount();
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
void FDasNativeModule::AddClass(const TCHAR* ScriptName, FDasNativeClassFn Class, void* AotCast)
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
        Desc.Classes.Add({Name, Class, AotCast});
}

void FDasNativeModule::SetReflects(const TCHAR* FunctionName)
{
    auto& Desc = *static_cast<FDasNativeModuleDesc*>(Module);
    if (!Desc.Error.IsEmpty())
        return;
    const FString Name = FunctionName ? FunctionName : TEXT("");
    if (Desc.Functions.IsEmpty() || !Desc.Functions.Last().IsMethod() || !Desc.Functions.Last().ReflectedName.IsEmpty())
        Desc.Error = FString::Printf(TEXT("Native module '%s': Reflects(%s) must follow a Method that reflects "
                                          "nothing yet"),
                                     *Desc.Name, *Name);
    else if (!IsScriptIdentifier(Name))
        Desc.Error = FString::Printf(TEXT("Native module '%s': function '%s' reflects '%s', which is not a "
                                          "UFUNCTION name"),
                                     *Desc.Name, *Desc.Functions.Last().Name, *Name);
    else
        Desc.Functions.Last().ReflectedName = Name;
}

namespace
{
// The native bind type of a UFUNCTION parameter or result, false when there is none.
bool DasNativeParamOfProperty(const FProperty* P, FDasNativeParam& Out, UClass*& ObjectClass)
{
    ObjectClass = nullptr;
    if (P->ArrayDim != 1)
        return false;
    auto Is = [&](EDasNativeType Type)
    {
        Out.Type = Type;
        return true;
    };
    if (P->IsA<FBoolProperty>())
        return Is(EDasNativeType::Bool);
    if (P->IsA<FIntProperty>())
        return Is(EDasNativeType::Int32);
    if (P->IsA<FInt64Property>())
        return Is(EDasNativeType::Int64);
    if (P->IsA<FFloatProperty>())
        return Is(EDasNativeType::Float);
    if (P->IsA<FDoubleProperty>())
        return Is(EDasNativeType::Double);
    if (const auto* Byte = CastField<FByteProperty>(P))
        return !Byte->Enum && Is(EDasNativeType::Byte);
    if (P->IsA<FStrProperty>())
        return Is(EDasNativeType::String);
    if (P->IsA<FNameProperty>())
        return Is(EDasNativeType::Name);
    if (P->IsA<FTextProperty>())
        return Is(EDasNativeType::Text);
    if (const auto* Object = CastField<FObjectProperty>(P); Object && !P->IsA<FClassProperty>())
    {
        ObjectClass = Object->PropertyClass;
        return Is(EDasNativeType::Object);
    }
    if (const auto* Struct = CastField<FStructProperty>(P))
    {
        const UScriptStruct* S = Struct->Struct;
#define DAS_NATIVE_STRUCT(CppType, Kind)                                                                               \
    if (S == TBaseStructure<CppType>::Get())                                                                           \
        return Is(EDasNativeType::Kind);
        DAS_NATIVE_STRUCT(FVector, Vector)
        DAS_NATIVE_STRUCT(FVector2D, Vector2D)
        DAS_NATIVE_STRUCT(FRotator, Rotator)
        DAS_NATIVE_STRUCT(FQuat, Quat)
        DAS_NATIVE_STRUCT(FTransform, Transform)
        DAS_NATIVE_STRUCT(FLinearColor, LinearColor)
        DAS_NATIVE_STRUCT(FColor, Color)
        DAS_NATIVE_STRUCT(FDateTime, DateTime)
#undef DAS_NATIVE_STRUCT
        // FTimespan has no TBaseStructure.
        if (S->GetStructPathName() == FTopLevelAssetPath(TEXT("/Script/CoreUObject"), TEXT("Timespan")))
            return Is(EDasNativeType::Timespan);
    }
    return false;
}
bool SameAsProperty(const FDasNativeParam& Bind, const FProperty* P)
{
    FDasNativeParam Param;
    UClass* ObjectClass = nullptr;
    if (!DasNativeParamOfProperty(P, Param, ObjectClass) || Param.Type != Bind.Type)
        return false;
    return Bind.Type != EDasNativeType::Object || (Bind.Class && Bind.Class() == ObjectClass);
}
// Why the bind cannot reflect its UFUNCTION, or empty; fills ReflectedPath.
FString CheckReflects(FDasNativeFunctionDesc& F)
{
    UClass* Class = F.Receiver();
    UFunction* U = Class ? Class->FindFunctionByName(FName(*F.ReflectedName)) : nullptr;
    if (!U)
        return FString::Printf(TEXT("is not a UFUNCTION of class %s"), Class ? *Class->GetName() : TEXT("?"));
    // A Blueprint or script override of an event would be skipped by the C++ call.
    if (U->HasAnyFunctionFlags(FUNC_Event | FUNC_BlueprintEvent))
        return TEXT("is an event (BlueprintNativeEvent / BlueprintImplementableEvent)");
    if (!U->HasAllFunctionFlags(FUNC_BlueprintCallable | FUNC_Native))
        return TEXT("is not a BlueprintCallable native function");
    if (U->HasAnyFunctionFlags(FUNC_Static | FUNC_Net | FUNC_Delegate | FUNC_MulticastDelegate | FUNC_EditorOnly))
        return TEXT("is a static, network, delegate or editor-only function");
    if (EnumHasAnyFlags(F.Flags, EDasBind::NonNullArgs))
        return TEXT("is NonNullArgs, the UFUNCTION takes nullptr");
    const FProperty* Return = U->GetReturnProperty();
    if (Return ? !SameAsProperty(F.Result, Return) : F.Result.Type != EDasNativeType::Void)
        return TEXT("has another result type");
    int32 Index = 1; // Arguments[0] is the receiver
    for (TFieldIterator<FProperty> It(U); It && It->HasAnyPropertyFlags(CPF_Parm); ++It)
    {
        if (It->HasAnyPropertyFlags(CPF_ReturnParm))
            continue;
        if (It->HasAnyPropertyFlags(CPF_OutParm) && !It->HasAnyPropertyFlags(CPF_ConstParm))
            return FString::Printf(TEXT("has an output parameter %s"), *It->GetName());
        if (Index >= F.Arguments.Num() || !SameAsProperty(F.Arguments[Index], *It))
            return FString::Printf(TEXT("has another type of parameter %s"), *It->GetName());
        ++Index;
    }
    if (Index != F.Arguments.Num())
        return TEXT("has another number of parameters");
    F.ReflectedPath = U->GetPathName();
    for (const FDasNativeModuleDesc* Other : DasNativeModuleDescs())
        if (Other->bLoaded && Other->Error.IsEmpty())
            for (const FDasNativeFunctionDesc& G : Other->Functions)
                if (&G != &F && G.ReflectedPath == F.ReflectedPath)
                    return FString::Printf(TEXT("is already reflected by '%s' (module '%s')"), *G.Name,
                                           *Other->Name);
    return FString();
}
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
    for (FDasNativeFunctionDesc& F : Desc.Functions)
        if (Desc.Error.IsEmpty() && !F.ReflectedName.IsEmpty())
            if (const FString Why = CheckReflects(F); !Why.IsEmpty())
                Desc.Error = FString::Printf(TEXT("Native module '%s': function '%s' reflects UFUNCTION %s, which "
                                                  "%s"),
                                             *Desc.Name, *F.Name, *F.ReflectedName, *Why);
}

FDasNativeRoute FindDasNativeRoute(const UFunction* Function, const UClass* Receiver)
{
    if (!Function || !Receiver)
        return {};
    const FString Path = Function->GetPathName();
    for (const FDasNativeModuleDesc* Desc : DasNativeModuleDescs())
        if (Desc->bLoaded && Desc->bBuilt && Desc->Error.IsEmpty())
            for (const FDasNativeFunctionDesc& F : Desc->Functions)
                if (F.ReflectedPath == Path && Receiver->IsChildOf(F.Receiver()))
                    return {Desc, &F};
    return {};
}
