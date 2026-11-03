#include "DasInt64Commandlet.h"
#include "DasDSLService.h"
#include "DasTypeService.h"
#include "DasRuntime.h"
#include "DasAuthoring.h"
#include "DasPropertyValues.h"
#include "Engine/Blueprint.h"
#include "UObject/StrongObjectPtr.h"
#include "UObject/UnrealType.h"
#include "UObject/StructOnScope.h"
#include "Serialization/JsonWriter.h"
#include "Dom/JsonValue.h"

UDasInt64Commandlet::UDasInt64Commandlet() { IsEditor = true; LogToConsole = true; }
int32 UDasInt64Commandlet::Main(const FString&)
{
    int Checks = 0, Failures = 0;
    auto Check = [&](bool OK, const TCHAR* Name, const FString& Error = FString()) {
        ++Checks; Failures += !OK;
        UE_LOG(LogTemp, Display, TEXT("DAS_INT64_%s %s %s"), OK ? TEXT("PASS") : TEXT("FAIL"), Name, *Error);
    };
    int64 Parsed = 17;
    Check(ParseDasInt64Literal(TEXT("9223372036854775807"), Parsed) && Parsed == MAX_int64, TEXT("maximum"));
    Check(ParseDasInt64Literal(TEXT("-9223372036854775808"), Parsed) && Parsed == MIN_int64, TEXT("minimum"));
    Check(!ParseDasInt64Literal(TEXT("9223372036854775808"), Parsed) &&
          !ParseDasInt64Literal(TEXT("-9223372036854775809"), Parsed) &&
          !ParseDasInt64Literal(TEXT("12.5"), Parsed) && !ParseDasInt64Literal(TEXT("1e3"), Parsed), TEXT("reject_overflow_and_noninteger"));
    auto& R = FModuleManager::LoadModuleChecked<FDasRuntimeModule>(TEXT("DasRuntime"));
    FString Error;
    TArray<UObject*> Types;
    Check(FDasTypeService::Apply(TEXT("ue_int64_types.das"), TEXT("/Game/DasInt64/Types"), Types, Error), TEXT("reflected_struct_exact_default"), Error);
    UBlueprint* BP = nullptr;
    Check(FDasDSLService::Apply(TEXT("dsl_int64_counter.das"), TEXT("/Game/DasInt64/BP_Counter"), TEXT("int64test.Counter"), BP, Error), TEXT("apply"), Error);
    if (!BP) return 1;
    TStrongObjectPtr<UBlueprint> KeepBP(BP);
    Check(FDasDSLService::Reimport(BP, Error), TEXT("reimport_exact_defaults"), Error);
    Check(R.PrepareEditorPrograms({TEXT("dsl_int64_counter.das")}, Error), TEXT("prepare"), Error);
    TStrongObjectPtr<UObject> Owner(NewObject<UObject>(GetTransientPackage(), BP->GeneratedClass));
    auto Read = [&](const TCHAR* Name) {
        auto* P = FindFProperty<FInt64Property>(Owner->GetClass(), Name);
        return P ? P->GetPropertyValue_InContainer(Owner.Get()) : int64(0);
    };
    Check(Read(TEXT("Zero")) == 0 && Read(TEXT("Total")) == 9007199254740993LL && Read(TEXT("Computed")) == 9007199254740995LL && Read(TEXT("Smallest")) == MIN_int64 && Read(TEXT("Largest")) == MAX_int64,
          TEXT("CDO_and_instance_preserve_beyond_double_precision"));
    auto Call = [&](const TCHAR* Name, int64 Value) {
        UFunction* F = Owner->FindFunction(Name);
        if (!F) return int64(0);
        FStructOnScope Frame(F);
        auto* In = FindFProperty<FInt64Property>(F, TEXT("input"));
        auto* Out = FindFProperty<FInt64Property>(F, TEXT("ReturnValue"));
        if (!In || !Out) return int64(0);
        In->SetPropertyValue_InContainer(Frame.GetStructMemory(), Value);
        Owner->ProcessEvent(F, Frame.GetStructMemory());
        return Out->GetPropertyValue_InContainer(Frame.GetStructMemory());
    };
    Check(Call(TEXT("Echo"), MAX_int64) == MAX_int64 && Call(TEXT("Echo"), MIN_int64) == MIN_int64,
          TEXT("Blueprint_call_frame_endpoints"));
    Check(Call(TEXT("NativeEcho"), 9007199254740993LL) == 9007199254740993LL,
          TEXT("generic_ue_arg_and_native_result"));
    auto* Accumulate = Owner->FindFunction(TEXT("Accumulate"));
    if (Accumulate) {
        FStructOnScope Frame(Accumulate);
        auto* Value = FindFProperty<FInt64Property>(Accumulate, TEXT("value"));
        auto* Double = FindFProperty<FInt64Property>(Accumulate, TEXT("doubled"));
        auto* Return = FindFProperty<FInt64Property>(Accumulate, TEXT("ReturnValue"));
        if (Value && Double && Return) {
            Value->SetPropertyValue_InContainer(Frame.GetStructMemory(), 2);
            Owner->ProcessEvent(Accumulate, Frame.GetStructMemory());
            Check(Read(TEXT("Total")) == 9007199254740995LL && Value->GetPropertyValue_InContainer(Frame.GetStructMemory()) == 9007199254740995LL &&
                  Double->GetPropertyValue_InContainer(Frame.GetStructMemory()) == 18014398509481990LL && Return->GetPropertyValue_InContainer(Frame.GetStructMemory()) == 9007199254740995LL,
                  TEXT("self_mutation_ref_and_out"));
        } else Check(false,TEXT("reflected_ref_out_properties"));
    } else Check(false,TEXT("reflected_ref_out_function"));
    auto Persistent = [&]() {
        auto* F = Owner->FindFunction(TEXT("PersistentStep"));
        if (!F) return int64(0);
        int64 Value = 0;
        Owner->ProcessEvent(F, &Value);
        return Value;
    };
    Check(Persistent() == 9007199254740994LL && Persistent() == 9007199254740995LL, TEXT("persistent_global_int64_exact"));
    bool Containers = false;
    if (auto* F = Owner->FindFunction(TEXT("Containers"))) Owner->ProcessEvent(F, &Containers);
    Check(Containers, TEXT("array_map_and_set_roundtrip"));
    FDasAuthoringModule Model, Roundtrip;
    FString Wire, RoundtripWire;
    Check(R.ReadAuthoringScript(R.ScriptPath(TEXT("dsl_int64_counter.das")), Model, Error) &&
          Model.ToJson(Wire, Error) && FDasAuthoringModule::FromJson(Wire, Roundtrip, Error) && Roundtrip.ToJson(RoundtripWire, Error) && Wire == RoundtripWire,
          TEXT("authoring_json_exact_roundtrip"), Error);
    auto* P = FindFProperty<FInt64Property>(Owner->GetClass(), TEXT("Largest"));
    int64 Stored = 42;
    auto Default = ExportDasPropertyDefault(P, &Stored);
    Check(Default && Default->Type == EJson::String && Default->AsString() == TEXT("42"), TEXT("default_export_decimal"));
    Check(ParseDasPropertyDefault(P, MakeShared<FJsonValueString>(TEXT("9223372036854775807")), &Stored, Error) && Stored == MAX_int64,
          TEXT("typed_default_exact_endpoint"), Error);
    Check(!ParseDasPropertyDefault(P, MakeShared<FJsonValueString>(TEXT("9223372036854775808")), &Stored, Error) && Stored == MAX_int64,
          TEXT("typed_default_overflow_is_atomic"));
    R.ReleaseObject(Owner.Get());
    UE_LOG(LogTemp, Display, TEXT("DAS_INT64_RESULT=%s checks=%d failures=%d"), Failures ? TEXT("FAIL") : TEXT("PASS"), Checks, Failures);
    return Failures ? 1 : 0;
}
