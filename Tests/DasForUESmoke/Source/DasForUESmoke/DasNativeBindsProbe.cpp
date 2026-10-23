// Native binds from a game module (DasNativeBinds.h): the module "smoke_native" binds functions of every supported
// type; RunDasNativeBindsProbe checks them through Content/Das/Scripts/native_binds_probe.das and checks the
// registration and unload errors with modules it registers itself. Used by the automation test
// DasForUE.NativeBinds and by the packaged game flag -DasNativeBinds.
#include "DasNativeBinds.h"
#include "DasRuntime.h"
#include "DasScriptComponent.h"
#include "HAL/FileManager.h"
#include "Misc/AutomationTest.h"
#include "Misc/FileHelper.h"
#include "Misc/Guid.h"
#include "Modules/ModuleManager.h"
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
}

DAS_NATIVE_BINDS(SmokeNative, "smoke_native")
{
    using namespace DasNativeBindsProbe;
    Binds.Function<&EchoBool>(
        TEXT("smoke_echo_bool"))
            .Function<&EchoInt>(
                TEXT("smoke_echo_int"))
                    .Function<&EchoFloat>(
                        TEXT("smoke_echo_float"))
                            .Function<&EchoDouble>(
                                TEXT("smoke_echo_double"))
                                    .Function<&EchoByte>(
                                        TEXT("smoke_echo_byte"))
                                            .Function<&EchoString>(
                                                TEXT("smoke_echo_string"))
                                                    .Function<&EchoName>(
                                                        TEXT("smoke_echo_name"))
                                                            .Function<&EchoText>(
                                                                TEXT("smoke_echo_text"))
                                                                    .Function<&EchoVector>(
                                                                        TEXT("smoke_echo_vector"))
                                                                            .Function<&EchoVector2D>(
                                                                                TEXT("smoke_echo_vector2d"))
                                                                                    .Function<&EchoRotator>(TEXT("smoke_echo_rotator"))
                                                                                                                .Function<&EchoQuat>(TEXT("smoke_echo_quat"))
                                                                                                                                         .Function<&EchoTransform>(TEXT("smoke_echo_transform"))
                                                                                                                                                                       .Function<&EchoLinearColor>(TEXT("smoke_echo_linear_color"))
                                                                                                                                                                                                       .Function<&EchoColor>(TEXT("smoke_echo_color"))
                                                                                                                                                                                                                                 .Function<&NameString>(TEXT("smoke_name_string"))
                                                                                                                                                                                                                                                            .Function<
                                                                                                                                                                                                                                                                &TextString>(TEXT("smoke_text_string"))
                                                                                                                                                                                                                                                                                 .Function<
                                                                                                                                                                                                                                                                                     &QuatIs>(
                                                                                                                                                                                                                                                                                     TEXT("smoke_quat_is"))
                                                                                                                                                                                                                                                                                         .Function<
                                                                                                                                                                                                                                                                                             &MakeTransform>(
                                                                                                                                                                                                                                                                                             TEXT("smoke_make_transform"))
                                                                                                                                                                                                                                                                                                 .Function<&TransformIs>(TEXT("smoke_transform_is"))
                                                                                                                                                                                                                                                                                                                             .Function<&Describe>(TEXT("smoke_describe"))
                                                                                                                                                                                                                                                                                                                                                      .Function<
                                                                                                                                                                                                                                                                                                                                                          &CountCall>(
                                                                                                                                                                                                                                                                                                                                                          TEXT("smoke_count_call"))
                                                                                                                                                                                                                                                                                                                                                              .Function<&VoidCallCount>(TEXT("smoke_void_calls"))
                                                                                                                                                                                                                                                                                                                                                                                            .Function<
                                                                                                                                                                                                                                                                                                                                                                                                &Square>(TEXT("smoke_square"), EDasBind::
                                                                                                                                                                                                                                                                                                                                                                                                                                  Pure)
                                                                                                                                                                                                                                                                                                                                                                                                             .Function<
                                                                                                                                                                                                                                                                                                                                                                                                                 &Add>(TEXT("smoke_add"));
}

namespace DasNativeBindsProbe
{
void BuildTemporary(FDasNativeModule& Binds)
{
    Binds.Function<&Add>(TEXT("temporary_add"));
}
void BuildDuplicate(FDasNativeModule& Binds)
{
    Binds.Function<&Add>(TEXT("twice")).Function<&Square>(TEXT("twice"));
}
// A script file that exists for one check.
struct FTemporaryScript
{
    FString Path;
    FTemporaryScript(FDasRuntimeModule& R, const FString& Source)
        : Path(R.ScriptPath(TEXT("native_binds_check_") + FGuid::NewGuid().ToString(EGuidFormats::Digits) +
                                 TEXT(".das")))
    {
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
        Report += FString::Printf(TEXT("%s %s%s%s\n"), bOK ? TEXT("PASS") : TEXT("FAIL"), Name, Detail.IsEmpty() ? TEXT("") : TEXT(": "), *Detail);
    };
    // Every type and call shape: the script returns 0 or the number of the first failed check.
    VoidCalls = 0;
    const auto Values = R.RunFile(R.ScriptPath(TEXT("native_binds_probe.das")), 0);
    const FString ValuesDetail = !Values.bSucceeded ? Values.Error
                                                    : Values.Value != 0
                                     ? FString::Printf(TEXT("check %d failed"), Values.Value)
                                     : FString();
    Check(Values.bSucceeded && Values.Value == 0, TEXT("native_binds_values_round_trip"), ValuesDetail);
    Check(VoidCalls == 2, TEXT("native_binds_void_function_runs"), FString::FromInt(VoidCalls));
    Check(DasNativeModuleStatus(TEXT("smoke_native")).IsEmpty(), TEXT("native_binds_module_available"),
                                                                      DasNativeModuleStatus(TEXT("smoke_native")));

    // Wrong calls are compile errors that name the function.
    auto Refused = [&](const TCHAR* Name, const FString& Source, const TCHAR* Expected)
    {
        FTemporaryScript Script(R, Source);
        const auto Result = R.RunFile(Script.Path, 0);
        Check(!Result.bSucceeded && Result.Error.Contains(Expected), Name, Result.Error.Left(300));
    };
    const FString Head = TEXT("options gen2\nrequire smoke_native\n[export]\ndef main(input:int):int {\n");
    Refused(TEXT("native_binds_wrong_argument_type_refused"), Head + TEXT("    return smoke_echo_int(\"x\")\n}\n"),
                                                                          TEXT("smoke_echo_int"));
    Refused(TEXT("native_binds_unknown_function_refused"), Head + TEXT("    return smoke_missing(1)\n}\n"),
                                                                       TEXT("smoke_missing"));
    Refused(TEXT("native_binds_unknown_module_refused"),
                 TEXT("options gen2\nrequire smoke_native_missing\n[export]\ndef main(input:int):int { return 0 }\n"),
                      TEXT("smoke_native_missing"));

    // Registration errors leave the module out; the status says why.
    const FString Run = FGuid::NewGuid().ToString(EGuidFormats::Digits).Left(8).ToLower();
    RegisterDasNativeModule(TEXT("ue_foundation"), &BuildTemporary);
    const FString Duplicate = TEXT("smoke_native_duplicate_") + Run;
    RegisterDasNativeModule(*Duplicate, &BuildDuplicate);
    {
        FTemporaryScript Trigger(R, TEXT("options gen2\n[export]\ndef main(input:int):int { return 0 }\n"));
        R.RunFile(Trigger.Path, 0); // compiling builds newly registered modules
    }
    Check(DasNativeModuleStatus(TEXT("ue_foundation"))
                                    .Contains(TEXT("taken by another daScript module")),
                                              TEXT("native_binds_taken_module_name_refused"),
                                                   DasNativeModuleStatus(TEXT("ue_foundation")));
    Check(DasNativeModuleStatus(*Duplicate)
              .Contains(TEXT("bound twice")), TEXT("native_binds_duplicate_refused"),
                                                   DasNativeModuleStatus(*Duplicate));
    UnregisterDasNativeModule(TEXT("ue_foundation"));
    UnregisterDasNativeModule(*Duplicate);

    // An unloaded module: compiled calls report an error instead of calling unloaded code.
    const FString Temporary = TEXT("smoke_native_temporary_") + Run;
    RegisterDasNativeModule(*Temporary, &BuildTemporary);
    FTemporaryScript Uses(R, FString::Printf(TEXT("options gen2\nrequire %s\n[export]\ndef call(value:int):int "
                                                  "{ return temporary_add(value, 1) }\n"),
                                                  *Temporary));
    TStrongObjectPtr<UDasScriptComponent> Owner(NewObject<UDasScriptComponent>());
    const auto Before = R.RunNamedPersistent(Uses.Path, TEXT("call"), 41, Owner.Get());
    Check(Before.bSucceeded && Before.Value == 42, TEXT("native_binds_runtime_registered_module_calls"), Before.Error);
    UnregisterDasNativeModule(*Temporary);
    const auto After = R.RunNamedPersistent(Uses.Path, TEXT("call"), 41, Owner.Get());
    Check(!After.bSucceeded && After.Error.Contains(TEXT("was unloaded")), TEXT("native_binds_unloaded_module_refused"),
                                                                                After.Error.Left(300));
    R.ReleaseObject(Owner.Get());
    return bPassed;
}

#if WITH_DEV_AUTOMATION_TESTS
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FDasNativeBindsTest, "DasForUE.NativeBinds",
                                 EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FDasNativeBindsTest::RunTest(const FString&)
{
    // The refused registrations log their reason once.
    AddExpectedError(TEXT("taken by another daScript module"), EAutomationExpectedErrorFlags::Contains, 1);
    AddExpectedError(TEXT("is bound twice"), EAutomationExpectedErrorFlags::Contains, 1);
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
