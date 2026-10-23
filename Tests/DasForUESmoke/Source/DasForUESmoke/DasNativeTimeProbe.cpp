// The plugin's native module ue_time (DasNativeTime.cpp): RunDasNativeTimeProbe checks it through
// Content/Das/Scripts/native_time_probe.das and checks the script errors of bad input. Used by the automation
// test DasForUE.NativeTime and by the packaged game flag -DasNativeBinds.
#include "DasRuntime.h"
#include "HAL/FileManager.h"
#include "Misc/AutomationTest.h"
#include "Misc/FileHelper.h"
#include "Misc/Guid.h"
#include "Modules/ModuleManager.h"

// Returns false with the failed checks in Report. Each check prints "PASS <name>" or "FAIL <name>".
bool RunDasNativeTimeProbe(FString& Report)
{
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
    const auto Values = R.RunFile(R.ScriptPath(TEXT("native_time_probe.das")), 0);
    FString Detail = Values.bSucceeded ? FString() : Values.Error;
    if (Values.bSucceeded && Values.Value != 0)
        Detail = FString::Printf(TEXT("check %d failed"), Values.Value);
    Check(Values.bSucceeded && Values.Value == 0, TEXT("native_time_values"), Detail);

    // Bad input is a script error that names the function, not an engine assertion.
    auto Refused = [&](const TCHAR* Name, const FString& Body, const TCHAR* Expected)
    {
        const FString Head = TEXT("options gen2\nrequire ue_time\n[export]\ndef main(input:int):int {\n");
        const FString File = TEXT("native_time_check_") + FGuid::NewGuid().ToString(EGuidFormats::Digits);
        const FString Path = R.ScriptPath(File + TEXT(".das"));
        const FString Source = Head + Body + TEXT("\n}\n");
        FFileHelper::SaveStringToFile(Source, *Path, FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM);
        const auto Result = R.RunFile(Path, 0);
        IFileManager::Get().Delete(*Path);
        Check(!Result.bSucceeded && Result.Error.Contains(Expected), Name, Result.Error.Left(300));
    };
    const TCHAR* BadDate = TEXT("return make_date_time(2023, 2, 29).get_day()");
    const TCHAR* BadText = TEXT("return parse_iso8601(\"tomorrow\").get_day()");
    const TCHAR* BadMonth = TEXT("return days_in_month(2024, 13)");
    const TCHAR* ZeroDivision = TEXT("return (make_timespan(1, 0, 0) / 0.0lf).get_hours()");
    const TCHAR* LateDate = TEXT("return (max_date_time() + make_timespan(0, 0, 1)).get_day()");
    const TCHAR* LongSpan = TEXT("return (-min_timespan()).get_days()");
    Refused(TEXT("native_time_invalid_date_refused"), BadDate, TEXT("make_date_time: invalid date 2023-2-29"));
    Refused(TEXT("native_time_bad_text_refused"), BadText, TEXT("parse_iso8601: cannot parse 'tomorrow'"));
    Refused(TEXT("native_time_bad_month_refused"), BadMonth, TEXT("days_in_month: month 13 is not 1..12"));
    Refused(TEXT("native_time_division_by_zero_refused"), ZeroDivision, TEXT("division by zero"));
    Refused(TEXT("native_time_date_overflow_refused"), LateDate, TEXT("date is out of range"));
    Refused(TEXT("native_time_span_overflow_refused"), LongSpan, TEXT("time span is out of range"));
    return bPassed;
}

#if WITH_DEV_AUTOMATION_TESTS
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FDasNativeTimeTest, "DasForUE.NativeTime",
                                 EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FDasNativeTimeTest::RunTest(const FString&)
{
    FString Report;
    const bool bPassed = RunDasNativeTimeProbe(Report);
    TArray<FString> Lines;
    Report.ParseIntoArrayLines(Lines);
    for (const FString& Line : Lines)
    {
        UE_LOG(LogTemp, Display, TEXT("DAS_NATIVE_TIME %s"), *Line);
        if (Line.StartsWith(TEXT("FAIL")))
            AddError(Line);
    }
    return bPassed;
}
#endif
