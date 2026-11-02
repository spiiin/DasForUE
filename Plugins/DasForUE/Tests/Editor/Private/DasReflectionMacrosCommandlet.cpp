#include "DasReflectionMacrosCommandlet.h"
#include "DasAuthoring.h"
#include "DasRuntime.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "Misc/ScopeExit.h"
#include "HAL/FileManager.h"
#include "UObject/StrongObjectPtr.h"
#include "UObject/Script.h"
UDasReflectionMacrosCommandlet::UDasReflectionMacrosCommandlet()
{
    IsClient = false;
    IsServer = false;
    IsEditor = true;
    LogToConsole = true;
}
int32 UDasReflectionMacrosCommandlet::Main(const FString& Params)
{
    int32 Checks = 0, Failures = 0;
    auto Check = [&](bool OK, const TCHAR* Name, const FString& Error = FString())
    {
        ++Checks;
        Failures += !OK;
        UE_LOG(LogTemp, Display, TEXT("DAS_REFLECTION_MACRO_%s %s %s"), OK ? TEXT("PASS") : TEXT("FAIL"), Name, *Error);
    };
    auto& R = FModuleManager::LoadModuleChecked<FDasRuntimeModule>(TEXT("DasRuntime"));
    const FString Repo = FPaths::ConvertRelativePathToFull(FPaths::ProjectDir() / TEXT("../.."));
    const FString Path =
        R.ScriptPath(TEXT("reflection_macro_") + FGuid::NewGuid().ToString(EGuidFormats::Digits) + TEXT(".das"));
    ON_SCOPE_EXIT
    {
        IFileManager::Get().Delete(*Path, false, true);
    };
    FString Source;
    if (!FFileHelper::LoadFileToString(Source, *(Repo / TEXT("Tests/ReflectionMacros/probe.das"))))
        return 2;
    auto Write = [&](const FString& Text)
    {
        return FFileHelper::SaveStringToFile(Text, *Path, FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM);
    };
    Write(Source);
    FDasAuthoringModule Model;
    FString Error;
    Check(R.ValidateIntMethod(Path, Error, TEXT("main")), TEXT("compile_three_macros"), Error);
    auto Reject = [&](const FString& Text, const TCHAR* Expected, const TCHAR* Name)
    {
        Write(Text);
        Error.Reset();
        Check(!R.ValidateIntMethod(Path, Error, TEXT("main")) && Error.Contains(Expected), Name, Error);
    };
    Reject(Source.Replace(TEXT("GameplayStatics.GetTimeSeconds"), TEXT("GameplayStatics.NoSuchFunction")),
                          TEXT("Unknown callable static"), TEXT("unknown_static_rejected"));
    Reject(Source.Replace(TEXT("KismetMathLibrary.Abs"), TEXT("DasFunctionLibraryFixture.UnsupportedInt64")),
                          TEXT("No generated binding"), TEXT("unsupported_transport_rejected"));
    Reject(Source.Replace(TEXT("ue_call(type<DasProbeInterface>, Query,"),
                               TEXT("ue_call(type<DasProbeInterface>, Missing,")),
                          TEXT("Missing"), TEXT("unknown_method_rejected"));
    Reject(Source.Replace(TEXT("ue_struct_get(payload, Count)"), TEXT("ue_struct_get(payload, Missing)")),
                          TEXT("Unknown Unreal struct field"), TEXT("unknown_field_rejected"));
    Reject(Source.Replace(TEXT("ue_struct_get(payload, Count)"), TEXT("ue_struct_get(payload, \"Count\")")),
                          TEXT("Expected ue_struct_get"), TEXT("field_string_rejected"));
    Reject(Source.Replace(TEXT("ue_static(KismetMathLibrary.Abs, -2.5lf)"),
                               TEXT("ue_static(KismetMathLibrary.Abs, \"wrong\")")),
                          TEXT("ue_lib_Abs(string"), TEXT("wrong_argument_type_rejected"));
    Reject(Source.Replace(TEXT("ue_static(KismetMathLibrary.Abs, -2.5lf)"), TEXT("ue_static(KismetMathLibrary.Abs)")),
                          TEXT("ue_lib_Abs()"), TEXT("wrong_argument_count_rejected"));
    Reject(Source.Replace(TEXT("ue_struct_get(payload, Count)"),
                               TEXT("ue_struct_get(ue_struct_value(\"/Script/DasRuntime.DasCallPayload\"), Count)")),
                          TEXT("typed Unreal value struct"), TEXT("untyped_snapshot_rejected"));
    Reject(Source.Replace(TEXT("ue_static(GameplayStatics.GetTimeSeconds)"),
                               TEXT("ue_static(GameplayStatics.GetTimeSeconds)()")),
                          TEXT("syntax error"), TEXT("curried_form_is_not_daScript_grammar"));
    Reject(Source.Replace(TEXT("ue_call(type<DasProbeInterface>, Query, target,5)"),
                               TEXT("ue_call(type<DasProbeInterface>, Query, ue_self_object(),5)")),
                          TEXT("no matching functions"), TEXT("raw_receiver_rejected"));
    Reject(Source.Replace(TEXT("local.Query(5)"), TEXT("local.NoSuchMethod(5)")),
                          TEXT("NoSuchMethod"), TEXT("unbound_interface_unknown_method_rejected"));
    Reject(Source.Replace(TEXT("local.Query(5)"), TEXT("ue_call(type<LocalProbeRef>, Query, local, \"wrong\")")),
                          TEXT("no matching"), TEXT("unbound_interface_wrong_argument_rejected"));
    Reject(Source.Replace(TEXT("ue_arg(call, next_field_name(nameReads), hit)"), TEXT("ue_arg(call, 33, hit)")),
                          TEXT("field name must be a string"), TEXT("generic_arg_field_name_type_rejected"));
    Reject(Source.Replace(TEXT("ue_arg(call, next_field_name(nameReads), hit)"), TEXT("ue_arg(33, \"Value\", hit)")),
                          TEXT("UCallHandle"), TEXT("generic_arg_invalid_frame_type_rejected"));
    Reject(Source.Replace(TEXT("ue_arg(call, next_field_name(nameReads), hit)"),
                               TEXT("ue_arg(call, \"Value\", PlainArgRecord(Value=1))")),
                          TEXT("supported by-value types"), TEXT("generic_arg_plain_struct_rejected"));
    const FString Descriptor = TEXT("\n[ue_reflected_call(owner=\"/Script/DasRuntime.DasFunctionLibraryFixture\",method=\"Add\",signature=\"@SIGNATURE@\")]\ndef duplicate(a:int;b:int):int{return a+b}\n");
    Reject(Source + Descriptor.Replace(TEXT("@SIGNATURE@"), TEXT("stale")),
                                       TEXT("Reflected signature changed"), TEXT("stale_binding_rejected_at_compile"));
    Reject(Source +
           Descriptor.Replace(TEXT("@SIGNATURE@"),
                                   TEXT("v1;Add;A:IntProperty:128;B:IntProperty:128;ReturnValue:IntProperty:1408;")),
                              TEXT("Ambiguous static bindings"), TEXT("ambiguous_bindings_rejected"));
    const FString ClassSource = Source.Replace(TEXT("require ue_native_gameplay"), TEXT("require ue_dsl\nrequire ue_native_gameplay")) +
        TEXT("\n[ue_class(module_id=\"reflectionmacro\",Blueprintable)]\nclass MacroUser {\n[ue_function(BlueprintCallable)]\ndef Read():int { return ue_static(DasFunctionLibraryFixture.Add,20,22) }\n}\n");
    Write(ClassSource);
    Check(R.ReadAuthoringScript(Path, Model, Error) && Model.Classes.Num() == 1, TEXT("macro_inside_authored_method"),
                                                                                      Error);
    Check(R.ValidateIntMethod(R.ScriptPath(TEXT("array_gc_exception_fixture.das")), Error, TEXT("main")),
                              TEXT("unchanged_array_GC_frame_fixture_compiles"), Error);
    for (const TCHAR* Package :
         {TEXT("ue_api_world"), TEXT("ue_api_math"), TEXT("ue_api_collision"), TEXT("ue_api_umg_input")})
    {
        Write(FString::Printf(TEXT("options gen2\nrequire %s\n[export]\ndef main(input:int):int{return input}\n"),
                                   Package));
        Check(R.ValidateIntMethod(Path, Error, TEXT("main")), Package, Error);
    }
    Write(Source);
    auto IV = UWorld::InitializationValues()
                  .CreatePhysicsScene(false)
                  .CreateNavigation(false)
                  .CreateAISystem(false)
                  .AllowAudioPlayback(false);
    TStrongObjectPtr<UWorld> World(
        UWorld::CreateWorld(EWorldType::Game, false, NAME_None, nullptr, true, ERHIFeatureLevel::Num, &IV));
    GEngine->CreateNewWorldContext(EWorldType::Game).SetCurrentWorld(World.Get());
    ON_SCOPE_EXIT
    {
        GEngine->DestroyWorldContext(World.Get());
        World->DestroyWorld(false);
    };
    World->InitializeActorsForPlay(FURL());
    Check(R.PreparePrograms(World.Get(), {FPaths::GetCleanFilename(Path)}, Error),
          TEXT("prepare_isolated_world_program"), Error);
    auto* Actor = World->SpawnActor<ADasReflectionMacroActor>();
    if (!Actor)
        return 2;
    TStrongObjectPtr<ADasReflectionMacroActor> Keep(Actor);
    FEditorScriptExecutionGuard Guard;
    auto Run = [&](int32 Mode, int32 Expected, const TCHAR* Name)
    {
        const auto Result = R.RunNamedPersistent(Path, TEXT("main"), Mode, Actor);
        Check(Result.bSucceeded && Result.Value == Expected, Name, Result.Error);
    };
    Run(0, 42, TEXT("static_scalar_alias_independent"));
    Run(1, 1, TEXT("automatic_WorldContext"));
    Run(2, 42, TEXT("native_interface_dispatch"));
    Run(3, 27, TEXT("interface_ref_out"));
    Run(4, 17, TEXT("typed_struct_read"));
    Run(5, 1, TEXT("nested_struct_read"));
    Run(6, 3, TEXT("interface_container_result"));
    Run(7, 42, TEXT("interface_member_without_generated_wrapper"));
    Run(8, 27, TEXT("interface_ref_out_without_generated_wrapper"));
    Run(9, 3, TEXT("interface_container_without_generated_wrapper"));
    Run(10, 42, TEXT("equivalent_native_reference_view_argument"));
    Run(11, 42, TEXT("typed_class_to_native_class_argument"));
    Run(12, 42, TEXT("generic_arg_scalars_single_evaluation"));
    Run(15, 1, TEXT("generic_arg_runtime_property_type_rejected"));
    Check(Actor->EchoCalls == 0, TEXT("generic_arg_rejection_has_no_native_side_effect"));
    Run(13, 42, TEXT("generic_arg_struct_nested_math_name_byte_object"));
    Check(Actor->EchoCalls == 1, TEXT("generic_arg_struct_invokes_once"));
    Run(14, 3, TEXT("generic_arg_array_carrier"));
    Run(16, 42, TEXT("generic_arg_math_value"));
    Run(17, 42, TEXT("generic_arg_typed_class"));
    R.ReleaseObject(Actor);
    const auto NoOwner = R.RunFile(Path, 1);
    Check(!NoOwner.bSucceeded && NoOwner.Error.Contains(TEXT("self is missing")),
                                                        TEXT("WorldContext_without_owner_rejected"), NoOwner.Error);
    const auto Frame = R.RunFile(R.ScriptPath(TEXT("actor_location.das")), 0, false, Actor);
    Check(Frame.bSucceeded, TEXT("unchanged_low_level_frame_fixture"), Frame.Error);
    R.ReleaseObject(Actor);
    UE_LOG(LogTemp, Display,
           TEXT("DAS_REFLECTION_MACRO_RESULT=%s checks=%d failures=%d"),
                Failures ? TEXT("FAIL") : TEXT("PASS"), Checks, Failures);
    return Failures ? 1 : 0;
}
