// The plugin's native module ue_math (DasNativeMath.cpp): RunDasNativeMathProbe checks it through
// Content/Das/Scripts/native_math_probe.das. Used by the automation test DasForUE.NativeMath and by the packaged
// game flag -DasNativeBinds.
#include "DasRuntime.h"
#include "Misc/AutomationTest.h"
#include "Modules/ModuleManager.h"

// Returns false with the failed check in Report ("PASS <name>" or "FAIL <name>").
bool RunDasNativeMathProbe(FString& Report)
{
    auto& R = FModuleManager::LoadModuleChecked<FDasRuntimeModule>(TEXT("DasRuntime"));
    const auto Values = R.RunFile(R.ScriptPath(TEXT("native_math_probe.das")), 0);
    const bool bPassed = Values.bSucceeded && Values.Value == 0;
    FString Detail = Values.bSucceeded ? FString() : Values.Error.Left(300);
    if (Values.bSucceeded && Values.Value != 0)
        Detail = FString::Printf(TEXT("check %d failed"), Values.Value);
    Report += FString::Printf(TEXT("%s native_math_values"), bPassed ? TEXT("PASS") : TEXT("FAIL"));
    if (!Detail.IsEmpty())
        Report += TEXT(": ") + Detail;
    Report += TEXT("\n");
    return bPassed;
}

#if WITH_DEV_AUTOMATION_TESTS
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FDasNativeMathTest, "DasForUE.NativeMath",
                                 EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FDasNativeMathTest::RunTest(const FString&)
{
    FString Report;
    const bool bPassed = RunDasNativeMathProbe(Report);
    UE_LOG(LogTemp, Display, TEXT("DAS_NATIVE_MATH %s"), *Report.TrimEnd());
    if (!bPassed)
        AddError(Report.TrimEnd());
    return bPassed;
}
#endif
