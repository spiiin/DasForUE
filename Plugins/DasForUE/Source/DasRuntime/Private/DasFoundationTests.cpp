#include "DasRuntime.h"
#include "Modules/ModuleManager.h"

bool RunDasFoundationTests(FString& Report)
{
    auto& Runtime = FModuleManager::LoadModuleChecked<FDasRuntimeModule>(TEXT("DasRuntime"));
    int32 Failures = 0;
    auto Expect = [&](bool Condition, const TCHAR* Name, const FString& Detail = FString())
    {
        Report += FString::Printf(TEXT("%s %s %s\n"), Condition ? TEXT("PASS") : TEXT("FAIL"), Name, *Detail);
        if (!Condition) ++Failures;
    };
    auto Run = [&](const TCHAR* Name) { return Runtime.RunFile(Runtime.ScriptPath(Name), 40); };
    const auto Good = Run(TEXT("smoke.das"));
    Expect(Good.bSucceeded && Good.Value == 42, TEXT("native_roundtrip"), Good.Error);
    const auto Again = Run(TEXT("smoke.das"));
    Expect(Again.bSucceeded && Again.Value == 42, TEXT("independent_context"), Again.Error);
    const auto Syntax = Run(TEXT("invalid.das"));
    Expect(!Syntax.bSucceeded && !Syntax.Error.IsEmpty(), TEXT("compile_error_diagnostic"));
    const auto Signature = Run(TEXT("wrong_signature.das"));
    Expect(!Signature.bSucceeded && Signature.Error.Contains(TEXT("main")), TEXT("signature_rejected"));
    const auto Panic = Run(TEXT("panic.das"));
    Expect(!Panic.bSucceeded && Panic.Error.Contains(TEXT("intentional")), TEXT("runtime_exception_caught"));
    const auto Missing = Run(TEXT("not_present.das"));
    Expect(!Missing.bSucceeded && !Missing.Error.IsEmpty(), TEXT("missing_file_diagnostic"));
    const auto Schema1 = Run(TEXT("schema_v1.das"));
    Expect(Schema1.bSucceeded && Schema1.Types.Num() == 1 && Schema1.Types[0].Name == TEXT("ProbeActor") && Schema1.Types[0].Fields == TEXT("speed:float;"), TEXT("macro_schema_v1"), Schema1.Error);
    const auto Schema2 = Run(TEXT("schema_v2.das"));
    Expect(Schema2.bSucceeded && Schema2.Types.Num() == 1 && Schema2.Types[0].Fields == TEXT("speed:float;health:int;"), TEXT("macro_schema_v2_same_process"), Schema2.Error);
    const auto Aot = Runtime.RunFile(Runtime.ScriptPath(TEXT("aot_smoke.das")), 40, true);
    Expect(Aot.bSucceeded && Aot.Value == 42 && Aot.bUsedAot, TEXT("aot_native_execution"), Aot.Error);
    const auto InvalidSchema = Run(TEXT("schema_invalid.das"));
    Expect(!InvalidSchema.bSucceeded && InvalidSchema.Error.Contains(TEXT("ue_probe")), TEXT("unsupported_schema_rejected"), InvalidSchema.Error);
    const auto Unicode = Run(TEXT("unicode.das"));
    Expect(Unicode.bSucceeded && Unicode.Types.Num() == 1 && Unicode.Types[0].Name == TEXT("Тип") && Unicode.Types[0].Fields == TEXT("здоровье:int;"), TEXT("utf8_owned_after_context_destroyed"), Unicode.Error);
    const auto Recovery = Run(TEXT("smoke.das"));
    Expect(Recovery.bSucceeded && Recovery.Value == 42, TEXT("recovery_after_failures"), Recovery.Error);
    Report += FString::Printf(TEXT("DAS_FOUNDATION_RESULT=%s failures=%d\n"), Failures ? TEXT("FAIL") : TEXT("PASS"), Failures);
    return Failures == 0;
}
