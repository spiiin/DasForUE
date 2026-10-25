#include "DasEnumNamesCommandlet.h"
#include "DasRuntime.h"
#include "DasAuthoring.h"
#include "DasFoundationManifest.h"
#include "DasActor.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "Engine/CollisionProfile.h"
#include "Components/BoxComponent.h"
#include "Misc/Paths.h"
#include "Misc/FileHelper.h"
#include "Misc/ScopeExit.h"
#include "HAL/FileManager.h"
#include "UObject/StrongObjectPtr.h"
UDasEnumNamesCommandlet::UDasEnumNamesCommandlet()
{
    IsClient = false;
    IsServer = false;
    IsEditor = true;
    LogToConsole = true;
}
int32 UDasEnumNamesCommandlet::Main(const FString& Params)
{
    int Checks = 0, Failures = 0;
    auto Check = [&](bool OK, const TCHAR* Name, const FString& Error = FString())
    {
        ++Checks;
        if (!OK)
            ++Failures;
        UE_LOG(LogTemp, Display, TEXT("DAS_ENUM_NAMES_%s %s %s"), OK ? TEXT("PASS"): TEXT("FAIL"), Name, *Error);
    };
    auto& R = FModuleManager::LoadModuleChecked<FDasRuntimeModule>(TEXT("DasRuntime"));
    const FString Repo = FPaths::ConvertRelativePathToFull(FPaths::ProjectDir() / TEXT("../.."));
    FString Source, Error;
    if (!FFileHelper::LoadFileToString(Source, *(Repo / TEXT("Tests/EnumNames/probe.das"))))
        return 2;
    const FString File = TEXT("enum_names_")+ FGuid::NewGuid().ToString(EGuidFormats::Digits) +
                              TEXT(".das"), Path = R.ScriptPath(File);
    ON_SCOPE_EXIT
    {
        IFileManager::Get().Delete(*Path, false, true);
    };
    auto Write = [&](const FString& S)
    {
        FFileHelper::SaveStringToFile(S, *Path, FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM);
    };
    auto Compile = [&](const FString& S)
    {
        Write(
            S +
            TEXT("\n[ue_class(module_id=\"enumnames\")]\nclass EnumNamesCompile {\n @ue_property=(BlueprintReadWrite)\n Value:int=0\n [ue_function(BlueprintCallable)]\n def ReadValue():int {return self.Value}\n}\n"));
        FDasAuthoringModule M;
        return R.ReadAuthoringScript(Path, M, Error);
    };
    Check(Compile(Source), TEXT("typed_profiles_and_trigger_overloads_compile"), Error);
    FString Expected;
    FFileHelper::LoadFileToString(Expected, *(Repo / TEXT("Bindings/FoundationExterns.txt")));
    Expected.ReplaceInline(TEXT("\r\n"), TEXT("\n"));
    const FString Actual = DasFoundationExternManifest();
    FFileHelper::SaveStringToFile(Actual, *(Repo / TEXT("logs/enum-names-externs.txt")));
    Check(Actual == Expected, TEXT("foundation_extern_manifest_matches"));
    Check(!Compile(Source.Replace(TEXT("CollisionProfile.OverlapOnlyPawn"), TEXT("CollisionProfile.UnknownProfile"))),
                   TEXT("unknown_profile_symbol_rejected"), Error);
    Check(!Compile(Source.Replace(TEXT("ue_collision_profile_name(profile)"), TEXT("ue_collision_profile_name(1)"))),
                   TEXT("untyped_integer_profile_rejected"), Error);
    Check(!Compile(Source.Replace(TEXT("ue_collision_profile_name(profile)"),
                                       TEXT("ue_collision_profile_name(\"OverlapOnlyPawn\")"))),
                   TEXT("string_profile_rejected"), Error);
    Check(!Compile(Source.Replace(TEXT("TriggerEvent.Triggered"), TEXT("TriggerEvent.Triggred"))),
                   TEXT("unknown_trigger_enum_rejected"), Error);
    const auto Symbols = DasCollisionProfileConstants();
    Check(Symbols.Num() == UCollisionProfile::Get()->GetNumOfProfiles(),
          TEXT("all_loaded_project_profiles_have_symbols"));
    FString Space, Dash, Keyword;
    for (const auto& Pair : Symbols)
    {
        if (Pair.Value == TEXT("Das Profile Test"))
            Space = Pair.Key;
        if (Pair.Value == TEXT("Das-Profile-Test"))
            Dash = Pair.Key;
        if (Pair.Value == TEXT("private"))
            Keyword = Pair.Key;
    }
    Check(!Space.IsEmpty() && !Dash.IsEmpty() && Space != Dash,
          TEXT("sanitized_profile_label_collisions_are_distinct"));
    const auto IVS = UWorld::InitializationValues()
                         .CreatePhysicsScene(false)
                         .CreateNavigation(false)
                         .CreateAISystem(false)
                         .AllowAudioPlayback(false);
    TStrongObjectPtr<UWorld> World(
        UWorld::CreateWorld(EWorldType::Game, false, NAME_None, nullptr, true, ERHIFeatureLevel::Num, &IVS));
    GEngine->CreateNewWorldContext(EWorldType::Game).SetCurrentWorld(World.Get());
    ON_SCOPE_EXIT
    {
        GEngine->DestroyWorldContext(World.Get());
        World->DestroyWorld(false);
    };
    World->InitializeActorsForPlay(FURL());
    auto* Actor = World->SpawnActor<ADasActor>();
    auto* Box = NewObject<UBoxComponent>(Actor);
    Actor->AddInstanceComponent(Box);
    Box->RegisterComponent();
    Write(Source);
    const auto Run = R.RunPersistent(Path, 0, Box);
    Check(Run.bSucceeded && Run.Value == 42 &&
          Box->GetCollisionProfileName() == TEXT("OverlapOnlyPawn"), TEXT("profile_name_native_roundtrip"), Run.Error);
    const auto Custom = R.RunNamedFramePersistent(Path, TEXT("custom"), {}, EDasMethodSignature::Name, Actor);
    Check(Custom.bSucceeded && Custom.NameValue == TEXT("DasProfileTest"),
                                                        TEXT("project_specific_profile_runtime_name"), Custom.Error);
    const auto Forged = R.RunNamedFramePersistent(Path, TEXT("forged"), {}, EDasMethodSignature::Name, Actor);
    Check(!Forged.bSucceeded && Forged.Error.Contains(TEXT("unknown")), TEXT("forged_profile_id_rejected"),
                                                                             Forged.Error);
    R.ReleaseObject(Actor);
    R.ReleaseObject(Box);
    FString Body = TEXT("options gen2\nrequire ue_collision_profiles\n[export]\ndef profile():UENameValue{return ue_collision_profile_name(CollisionProfile.__LABEL__)}\n");
    Write(Body.Replace(TEXT("__LABEL__"), *Space));
    auto Escaped = R.RunNamedFramePersistent(Path, TEXT("profile"), {}, EDasMethodSignature::Name, Actor);
    Check(Escaped.bSucceeded && Escaped.NameValue == TEXT("Das Profile Test"), TEXT("escaped_space_profile_roundtrips"),
                                                                                    Escaped.Error);
    R.ReleaseObject(Actor);
    Write(Body.Replace(TEXT("__LABEL__"), *Dash));
    Escaped = R.RunNamedFramePersistent(Path, TEXT("profile"), {}, EDasMethodSignature::Name, Actor);
    Check(Escaped.bSucceeded && Escaped.NameValue == TEXT("Das-Profile-Test"), TEXT("escaped_dash_profile_roundtrips"),
                                                                                    Escaped.Error);
    R.ReleaseObject(Actor);
    Write(Body.Replace(TEXT("__LABEL__"), *Keyword));
    Escaped = R.RunNamedFramePersistent(Path, TEXT("profile"), {}, EDasMethodSignature::Name, Actor);
    Check(!Keyword.IsEmpty() && Escaped.bSucceeded &&
          Escaped.NameValue == TEXT("private"), TEXT("keyword_profile_roundtrips"), Escaped.Error);
    R.ReleaseObject(Actor);
    R.ReleaseObject(Box);
    FDasAuthoringModule M;
    Check(R.ReadAuthoringScript(R.ScriptPath(TEXT("dsl_playable_pickup.das")), M, Error),
                                TEXT("migrated_pickup_compiles"), Error);
    UE_LOG(LogTemp, Display, TEXT("DAS_ENUM_NAMES_RESULT=%s checks=%d"), Failures ? TEXT("FAIL"): TEXT("PASS"), Checks);
    return Failures ? 1 : 0;
}
