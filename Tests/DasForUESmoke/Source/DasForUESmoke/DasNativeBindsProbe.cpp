// Native binds from a game module (DasNativeBinds.h). The module "smoke_native" binds functions of every supported
// type and the classes Probe / ProbeChild with methods; "smoke_native_ext" adds a method to Probe and
// "smoke_native_late" declares ProbeGrandchild, so inherited methods come from modules built before and after.
// RunDasNativeBindsProbe checks them through Content/Das/Scripts/native_binds_probe.das and
// native_binds_objects.das, and checks registration, null and unload errors. Used by the automation test
// DasForUE.NativeBinds and by the packaged game flag -DasNativeBinds.
#include "DasNativeBinds.h"
#include "DasRuntime.h"
#include "DasScriptComponent.h"
#include "HAL/FileManager.h"
#include "Misc/AutomationTest.h"
#include "Misc/FileHelper.h"
#include "Misc/Guid.h"
#include "Modules/ModuleManager.h"
#include "NativeBindsProbeObjects.h"
#include "UObject/Package.h"
#include "UObject/StrongObjectPtr.h"

namespace DasNativeBindsProbe
{
int32 VoidCalls = 0;

bool EchoBool(bool V)
{
    return V;
}
int32 EchoInt(int32 V)
{
    return V;
}
int64 EchoInt64(int64 V)
{
    return V;
}
float EchoFloat(float V)
{
    return V;
}
double EchoDouble(double V)
{
    return V;
}
uint8 EchoByte(uint8 V)
{
    return V;
}
FString EchoString(const FString& V)
{
    return V;
}
FName EchoName(FName V)
{
    return V;
}
FText EchoText(const FText& V)
{
    return V;
}
FVector EchoVector(const FVector& V)
{
    return V;
}
FVector2D EchoVector2D(const FVector2D& V)
{
    return V;
}
FRotator EchoRotator(const FRotator& V)
{
    return V;
}
FQuat EchoQuat(const FQuat& V)
{
    return V;
}
FTransform EchoTransform(const FTransform& V)
{
    return V;
}
FLinearColor EchoLinearColor(const FLinearColor& V)
{
    return V;
}
FColor EchoColor(FColor V)
{
    return V;
}

// Checks the script cannot do itself: text and name contents, quaternion and transform components.
FString NameString(FName V)
{
    return V.ToString();
}
FString TextString(const FText& V)
{
    return V.ToString();
}
bool QuatIs(const FQuat& Q, double X, double Y, double Z, double W)
{
    return Q.Equals(FQuat(X, Y, Z, W), 1e-9);
}
FTransform MakeTransform(const FVector& Location, double Scale)
{
    return FTransform(FQuat::Identity, Location, FVector(Scale));
}
bool TransformIs(const FTransform& T, const FVector& Location, double Scale)
{
    return T.GetLocation().Equals(Location) && T.GetScale3D().Equals(FVector(Scale)) && T.GetRotation().IsIdentity();
}

FString Describe(int32 A, float B, const FString& C, const FVector& D, FName E)
{
    return FString::Printf(TEXT("%d|%.2f|%s|%.1f,%.1f,%.1f|%s"), A, B, *C, D.X, D.Y, D.Z, *E.ToString());
}
void CountCall()
{
    ++VoidCalls;
}
int32 VoidCallCount()
{
    return VoidCalls;
}
int32 Square(int32 V)
{
    return V * V;
}
int32 Add(int32 A, int32 B)
{
    return A + B;
}

// Objects: created in the transient package and not kept alive; a script run does not collect garbage.
template <class T> T* Make(int32 Value)
{
    T* Probe = NewObject<T>(GetTransientPackage());
    Probe->Value = Value;
    return Probe;
}
UNativeBindsProbe* MakeProbe(int32 Value)
{
    return Make<UNativeBindsProbe>(Value);
}
UNativeBindsProbeChild* MakeChild(int32 Value)
{
    return Make<UNativeBindsProbeChild>(Value);
}
UNativeBindsProbeGrandchild* MakeGrandchild(int32 Value)
{
    return Make<UNativeBindsProbeGrandchild>(Value);
}
void KillProbe(UNativeBindsProbe* Probe)
{
    if (Probe)
        Probe->MarkAsGarbage();
}
bool ArePartners(const UNativeBindsProbe* A, const UNativeBindsProbe* B)
{
    return A && B && A->GetPartner() == B;
}
int32 PartnerValue(UNativeBindsProbe* Probe)
{
    return Probe->Value; // bound with NonNullArgs
}
bool IsScriptComponent(UDasScriptComponent* Component)
{
    return Component != nullptr; // a class without a script type: passed as UObjectHandle
}
int32 ValuePlus(UNativeBindsProbe& Probe, int32 Delta)
{
    return Probe.Value + Delta;
}
// A capture-less lambda converted to a function pointer can be bound as a method too.
constexpr auto DoubleValue = +[](const UNativeBindsProbe& Probe)
{
    return Probe.Value * 2;
};
int32 One(UDasScriptComponent&)
{
    return 1;
}
}

DAS_NATIVE_BINDS(SmokeNative, "smoke_native")
{
    using namespace DasNativeBindsProbe;
    Binds.Function<&EchoBool>(TEXT("smoke_echo_bool"));
    Binds.Function<&EchoInt>(TEXT("smoke_echo_int"));
    Binds.Function<&EchoInt64>(TEXT("smoke_echo_int64"));
    Binds.Function<&EchoFloat>(TEXT("smoke_echo_float"));
    Binds.Function<&EchoDouble>(TEXT("smoke_echo_double"));
    Binds.Function<&EchoByte>(TEXT("smoke_echo_byte"));
    Binds.Function<&EchoString>(TEXT("smoke_echo_string"));
    Binds.Function<&EchoName>(TEXT("smoke_echo_name"));
    Binds.Function<&EchoText>(TEXT("smoke_echo_text"));
    Binds.Function<&EchoVector>(TEXT("smoke_echo_vector"));
    Binds.Function<&EchoVector2D>(TEXT("smoke_echo_vector2d"));
    Binds.Function<&EchoRotator>(TEXT("smoke_echo_rotator"));
    Binds.Function<&EchoQuat>(TEXT("smoke_echo_quat"));
    Binds.Function<&EchoTransform>(TEXT("smoke_echo_transform"));
    Binds.Function<&EchoLinearColor>(TEXT("smoke_echo_linear_color"));
    Binds.Function<&EchoColor>(TEXT("smoke_echo_color"));
    Binds.Function<&NameString>(TEXT("smoke_name_string"));
    Binds.Function<&TextString>(TEXT("smoke_text_string"));
    Binds.Function<&QuatIs>(TEXT("smoke_quat_is"));
    Binds.Function<&MakeTransform>(TEXT("smoke_make_transform"));
    Binds.Function<&TransformIs>(TEXT("smoke_transform_is"));
    Binds.Function<&Describe>(TEXT("smoke_describe"));
    Binds.Function<&CountCall>(TEXT("smoke_count_call"));
    Binds.Function<&VoidCallCount>(TEXT("smoke_void_calls"));
    Binds.Function<&Square>(TEXT("smoke_square"), EDasBind::Pure);
    Binds.Function<&Add>(TEXT("smoke_add"));

    using FScaled = int32 (UNativeBindsProbe::*)(int32) const;
    auto Probe = Binds.Class<UNativeBindsProbe>(TEXT("Probe"));
    Probe.Method<&UNativeBindsProbe::GetValue>(TEXT("get_value"));
    Probe.Method<&UNativeBindsProbe::SetValue>(TEXT("set_value"));
    Probe.Method<static_cast<FScaled>(&UNativeBindsProbe::Scaled)>(TEXT("scaled"));
    Probe.Method<&UNativeBindsProbe::GetPartner>(TEXT("get_partner"));
    Probe.Method<&UNativeBindsProbe::SetPartner>(TEXT("set_partner"));
    Probe.Method<&ValuePlus>(TEXT("value_plus"));
    auto Child = Binds.Class<UNativeBindsProbeChild>(TEXT("ProbeChild"));
    Child.Method<&UNativeBindsProbeChild::ChildOnly>(TEXT("child_only"));
    Binds.Function<&MakeProbe>(TEXT("smoke_make_probe"));
    Binds.Function<&MakeChild>(TEXT("smoke_make_child"));
    Binds.Function<&KillProbe>(TEXT("smoke_kill_probe"));
    Binds.Function<&ArePartners>(TEXT("smoke_are_partners"));
    Binds.Function<&PartnerValue>(TEXT("smoke_partner_value"), EDasBind::NonNullArgs);
    Binds.Function<&IsScriptComponent>(TEXT("smoke_is_script_component"));
}

// A method added to a type of another module.
DAS_NATIVE_BINDS(SmokeNativeExt, "smoke_native_ext")
{
    Binds.Class<UNativeBindsProbe>().Method<DasNativeBindsProbe::DoubleValue>(TEXT("ext_double"));
}

// A descendant type declared after both: inherits methods of smoke_native and smoke_native_ext.
DAS_NATIVE_BINDS(SmokeNativeLate, "smoke_native_late")
{
    Binds.Class<UNativeBindsProbeGrandchild>(TEXT("ProbeGrandchild"));
    Binds.Function<&DasNativeBindsProbe::MakeGrandchild>(TEXT("smoke_make_grandchild"));
}

namespace DasNativeBindsProbe
{
void BuildTemporary(FDasNativeModule& Binds)
{
    Binds.Function<&Add>(TEXT("temporary_add"));
}
void BuildDuplicate(FDasNativeModule& Binds)
{
    // Other parameters make an overload; the same parameters again are refused.
    Binds.Function<&Add>(TEXT("twice"));
    Binds.Function<&Square>(TEXT("twice"));
    Binds.Function<&Add>(TEXT("twice"));
}
void BuildPureObject(FDasNativeModule& Binds)
{
    Binds.Function<&KillProbe>(TEXT("pure_kill"), EDasBind::Pure);
}
void BuildSecondOwner(FDasNativeModule& Binds)
{
    Binds.Class<UNativeBindsProbe>(TEXT("ProbeAgain"));
}
void BuildUntypedClass(FDasNativeModule& Binds)
{
    Binds.Class<UDasScriptComponent>().Method<&One>(TEXT("one"));
}
// A script file that exists for one check.
struct FTemporaryScript
{
    FString Path;
    FTemporaryScript(FDasRuntimeModule& R, const FString& Source)
    {
        const FString File = TEXT("native_binds_check_") + FGuid::NewGuid().ToString(EGuidFormats::Digits);
        Path = R.ScriptPath(File + TEXT(".das"));
        FFileHelper::SaveStringToFile(Source, *Path, FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM);
    }
    ~FTemporaryScript() { IFileManager::Get().Delete(*Path); }
};
}

// Returns false with the failed checks in Report. Each check prints "PASS <name>" or "FAIL <name>".
bool RunDasNativeBindsProbe(FString& Report)
{
    using namespace DasNativeBindsProbe;
    auto& R = FModuleManager::LoadModuleChecked<FDasRuntimeModule>(TEXT("DasRuntime"));
    bool bPassed = true;
    auto Check = [&](bool bOK, const TCHAR* Name, const FString& Detail = FString())
    {
        bPassed &= bOK;
        Report += FString::Printf(TEXT("%s %s"), bOK ? TEXT("PASS") : TEXT("FAIL"), Name);
        if (!Detail.IsEmpty())
            Report += TEXT(": ") + Detail;
        Report += TEXT("\n");
    };
    // The probe scripts return 0 or the number of the first failed check.
    auto ScriptResult = [](const FDasExecutionResult& Result)
    {
        if (!Result.bSucceeded)
            return Result.Error;
        return Result.Value != 0 ? FString::Printf(TEXT("check %d failed"), Result.Value) : FString();
    };
    TStrongObjectPtr<UDasScriptComponent> Owner(NewObject<UDasScriptComponent>());

    // Every value type and call shape.
    VoidCalls = 0;
    const auto Values = R.RunFile(R.ScriptPath(TEXT("native_binds_probe.das")), 0);
    Check(Values.bSucceeded && Values.Value == 0, TEXT("native_binds_values_round_trip"), ScriptResult(Values));
    Check(VoidCalls == 2, TEXT("native_binds_void_function_runs"), FString::FromInt(VoidCalls));
    const FString Status = DasNativeModuleStatus(TEXT("smoke_native"));
    Check(Status.IsEmpty(), TEXT("native_binds_module_available"), Status);

    // Classes and methods: types, inheritance across modules, casts, null arguments.
    const FString ObjectsPath = R.ScriptPath(TEXT("native_binds_objects.das"));
    const auto Objects = R.RunNamedPersistent(ObjectsPath, TEXT("main"), 0, Owner.Get());
    Check(Objects.bSucceeded && Objects.Value == 0, TEXT("native_binds_objects"), ScriptResult(Objects));

    // Wrong calls: compile errors that name the function, or script errors at the call.
    auto Refused = [&](const TCHAR* Name, const FString& Body, const TCHAR* Expected)
    {
        const FString Head = TEXT("options gen2\nrequire smoke_native\n[export]\ndef main(input:int):int {\n");
        FTemporaryScript Script(R, Head + Body + TEXT("\n}\n"));
        const auto Result = R.RunNamedPersistent(Script.Path, TEXT("main"), 0, Owner.Get());
        Check(!Result.bSucceeded && Result.Error.Contains(Expected), Name, Result.Error.Left(300));
    };
    const TCHAR* WrongType = TEXT("return smoke_partner_value(smoke_make_child(1))");
    const TCHAR* NullReceiver = TEXT("Probe.get_value: receiver is null or expired");
    const TCHAR* Expired = TEXT("let p = smoke_make_probe(1)\nsmoke_kill_probe(p)\nreturn p.get_value()");
    const TCHAR* NullArgument = TEXT("var p : Probe\nreturn smoke_partner_value(p)");
    const TCHAR* WrongClass = TEXT("return smoke_is_script_component(smoke_make_probe(1).handle()) ? 1 : 0");
    const TCHAR* WrongClassError = TEXT("argument 1 must be DasScriptComponent, got NativeBindsProbe");
    const TCHAR* StringForInt = TEXT("return smoke_echo_int(\"x\")");
    Refused(TEXT("native_binds_wrong_argument_type_refused"), StringForInt, TEXT("smoke_echo_int"));
    Refused(TEXT("native_binds_unknown_function_refused"), TEXT("return smoke_missing(1)"), TEXT("smoke_missing"));
    Refused(TEXT("native_binds_wrong_class_type_refused"), WrongType, TEXT("smoke_partner_value"));
    Refused(TEXT("native_binds_null_receiver_refused"), TEXT("var p : Probe\nreturn p.get_value()"), NullReceiver);
    Refused(TEXT("native_binds_expired_receiver_refused"), Expired, NullReceiver);
    Refused(TEXT("native_binds_null_argument_refused"), NullArgument, TEXT("argument 1 is null or expired"));
    Refused(TEXT("native_binds_untyped_wrong_class_refused"), WrongClass, WrongClassError);
    {
        const FString Missing =
            TEXT("options gen2\nrequire smoke_native_missing\n[export]\ndef main(input:int):int { return 0 }\n");
        FTemporaryScript Script(R, Missing);
        const auto Result = R.RunFile(Script.Path, 0);
        const bool bRefused = !Result.bSucceeded && Result.Error.Contains(TEXT("smoke_native_missing"));
        Check(bRefused, TEXT("native_binds_unknown_module_refused"), Result.Error.Left(300));
    }

    // Registration errors leave the module out; the status says why.
    const FString Run = FGuid::NewGuid().ToString(EGuidFormats::Digits).Left(8).ToLower();
    const FString Duplicate = TEXT("smoke_native_duplicate_") + Run;
    const FString PureObject = TEXT("smoke_native_pure_object_") + Run;
    const FString SecondOwner = TEXT("smoke_native_second_owner_") + Run;
    const FString Untyped = TEXT("smoke_native_untyped_") + Run;
    RegisterDasNativeModule(TEXT("ue_foundation"), &BuildTemporary);
    RegisterDasNativeModule(*Duplicate, &BuildDuplicate);
    RegisterDasNativeModule(*PureObject, &BuildPureObject);
    RegisterDasNativeModule(*SecondOwner, &BuildSecondOwner);
    RegisterDasNativeModule(*Untyped, &BuildUntypedClass);
    {
        FTemporaryScript Trigger(R, TEXT("options gen2\n[export]\ndef main(input:int):int { return 0 }\n"));
        R.RunFile(Trigger.Path, 0); // compiling builds newly registered modules
    }
    auto Registration = [&](const TCHAR* Name, const FString& Module, const TCHAR* Expected)
    {
        const FString Why = DasNativeModuleStatus(*Module);
        Check(Why.Contains(Expected), Name, Why);
        UnregisterDasNativeModule(*Module);
    };
    const TCHAR* SecondOwnerError = TEXT("class NativeBindsProbe already has the script type 'Probe'");
    Registration(TEXT("native_binds_taken_module_name_refused"), TEXT("ue_foundation"), TEXT("taken by another"));
    Registration(TEXT("native_binds_duplicate_refused"), Duplicate, TEXT("is bound twice"));
    Registration(TEXT("native_binds_pure_object_refused"), PureObject, TEXT("is Pure but takes or returns objects"));
    Registration(TEXT("native_binds_second_class_owner_refused"), SecondOwner, SecondOwnerError);
    Registration(TEXT("native_binds_class_without_type_refused"), Untyped, TEXT("which has no script type"));

    // An unloaded module: compiled calls report an error instead of calling unloaded code.
    const FString Temporary = TEXT("smoke_native_temporary_") + Run;
    RegisterDasNativeModule(*Temporary, &BuildTemporary);
    const FString UsesHead = TEXT("options gen2\nrequire ") + Temporary + TEXT("\n[export]\n");
    FTemporaryScript Uses(R, UsesHead + TEXT("def call(value:int):int { return temporary_add(value, 1) }\n"));
    const auto Before = R.RunNamedPersistent(Uses.Path, TEXT("call"), 41, Owner.Get());
    const bool bCalled = Before.bSucceeded && Before.Value == 42;
    Check(bCalled, TEXT("native_binds_runtime_registered_module_calls"), Before.Error);
    UnregisterDasNativeModule(*Temporary);
    const auto After = R.RunNamedPersistent(Uses.Path, TEXT("call"), 41, Owner.Get());
    const bool bUnloaded = !After.bSucceeded && After.Error.Contains(TEXT("was unloaded"));
    Check(bUnloaded, TEXT("native_binds_unloaded_module_refused"), After.Error.Left(300));
    R.ReleaseObject(Owner.Get());
    return bPassed;
}

#if WITH_DEV_AUTOMATION_TESTS
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FDasNativeBindsTest, "DasForUE.NativeBinds",
                                 EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FDasNativeBindsTest::RunTest(const FString&)
{
    // The refused registrations log their reason once.
    const auto Contains = EAutomationExpectedErrorFlags::Contains;
    AddExpectedError(TEXT("taken by another daScript module"), Contains, 1);
    AddExpectedError(TEXT("is bound twice"), Contains, 1);
    AddExpectedError(TEXT("is Pure but takes or returns objects"), Contains, 1);
    AddExpectedError(TEXT("already has the script type"), Contains, 1);
    AddExpectedError(TEXT("which has no script type"), Contains, 1);
    FString Report;
    const bool bPassed = RunDasNativeBindsProbe(Report);
    TArray<FString> Lines;
    Report.ParseIntoArrayLines(Lines);
    for (const FString& Line : Lines)
    {
        UE_LOG(LogTemp, Display, TEXT("DAS_NATIVE_BINDS %s"), *Line);
        if (Line.StartsWith(TEXT("FAIL")))
            AddError(Line);
    }
    return bPassed;
}
#endif
