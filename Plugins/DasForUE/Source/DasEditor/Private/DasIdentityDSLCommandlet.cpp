#include "DasIdentityDSLCommandlet.h"
#include "DasRuntime.h"
#include "DasAuthoring.h"
#include "DasFoundationManifest.h"
#include "DasDSLService.h"
#include "DasActor.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "Engine/Blueprint.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "Misc/ScopeExit.h"
#include "HAL/FileManager.h"
#include "UObject/StrongObjectPtr.h"
#include "UObject/UnrealType.h"
#include "K2Node_FunctionEntry.h"
#include "EdGraph/EdGraph.h"
#include "EdGraph/EdGraphPin.h"
#include "UObject/GarbageCollection.h"
UDasIdentityDSLCommandlet::UDasIdentityDSLCommandlet()
{
    IsClient = false;
    IsServer = false;
    IsEditor = true;
    LogToConsole = true;
}
int32 UDasIdentityDSLCommandlet::Main(const FString& Params)
{
    int Checks = 0, Failures = 0;
    auto Check = [&](bool OK, const TCHAR* Name, const FString& E = FString())
    {
        ++Checks;
        Failures += !OK;
        UE_LOG(LogTemp, Display, TEXT("DAS_IDENTITY_DSL_%s %s %s"), OK ? TEXT("PASS"): TEXT("FAIL"), Name, *E);
    };
    auto& R = FModuleManager::LoadModuleChecked<FDasRuntimeModule>(TEXT("DasRuntime"));
    const FString Repo = FPaths::ConvertRelativePathToFull(FPaths::ProjectDir() / TEXT("../.."));
    const FString Id = FGuid::NewGuid().ToString(EGuidFormats::Digits);
    const FString File = TEXT("identity_case_")+ Id + TEXT(".das"), Path = R.ScriptPath(File);
    const FString AuthoredFile =
                      TEXT("identity_authored_")+ Id + TEXT(".das"), AuthoredPath = R.ScriptPath(AuthoredFile);
    const FString DefaultsFile =
                      TEXT("identity_defaults_")+ Id + TEXT(".das"), DefaultsPath = R.ScriptPath(DefaultsFile);
    ON_SCOPE_EXIT
    {
        IFileManager::Get().Delete(*Path, false, true);
        IFileManager::Get().Delete(*DefaultsPath, false, true);
        IFileManager::Get().Delete(*AuthoredPath, false, true);
    };
    FString ManifestExpected;
    FFileHelper::LoadFileToString(ManifestExpected, *(Repo / TEXT("Bindings/FoundationExterns.txt")));
    ManifestExpected.ReplaceInline(TEXT("\r\n"), TEXT("\n"));
    const FString Manifest = DasFoundationExternManifest();
    FFileHelper::SaveStringToFile(Manifest, *(Repo / TEXT("logs/identity-dsl-externs.txt")),
                                              FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM);
    Check(ManifestExpected == Manifest, TEXT("foundation_extern_manifest_matches_baseline"));
    FString Source, Defaults, Error;
    if (!FFileHelper::LoadFileToString(Source, *(Repo / TEXT("Tests/IdentityDSL/probe.das"))) ||
                                       !FFileHelper::LoadFileToString(Defaults,
                                                                      *(Repo / TEXT("Tests/IdentityDSL/defaults.das"))))
        return 2;
    auto Write = [&](const FString& S)
    {
        return FFileHelper::SaveStringToFile(S, *Path, FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM);
    };
    Write(Source);
    Check(R.ValidateIntMethod(Path, Error), TEXT("state_loctext_CDO_ref_compile"), Error);
    auto Reject = [&](const FString& S, const TCHAR* Hint, const TCHAR* Name)
    {
        Write(S);
        Error.Reset();
        Check(!R.ValidateIntMethod(Path, Error) && Error.Contains(Hint), Name, Error);
    };
    Reject(Source.Replace(TEXT("Counter:int = 10"), TEXT("Counter:array<int>")),
                          TEXT("ue_state supports"), TEXT("container_state_rejected"));
    Reject(Source.Replace(TEXT("Counter:int = 10"), TEXT("Counter:int = ue_state_get_int(\"other\",2)")),
                          TEXT("literal"), TEXT("computed_state_initializer_rejected"));
    Reject(Source.Replace(TEXT("var @ue_state Counter"), TEXT("let @ue_state Counter")),
                          TEXT("mutable"), TEXT("constant_state_rejected"));
    Reject(Source.Replace(TEXT("var @ue_state Counter"), TEXT("var shared @ue_state Counter")),
                          TEXT("non-shared"), TEXT("shared_state_rejected"));
    Reject(Source.Replace(TEXT("Counter += input"), TEXT("var @ue_state local:int=0;Counter += input")),
                          TEXT("module globals"), TEXT("local_state_rejected"));
    Reject(Source.Replace(TEXT("ue_loctext(Welcome,"), TEXT("ue_loctext(\"Welcome\",")),
                          TEXT("Expected ue_loctext"), TEXT("string_key_rejected"));
    Reject(Source.Replace(TEXT("ue_class_default(type<ActorRef>)"), TEXT("ue_class_default(type<int>)")),
                          TEXT("Unreal object reference"), TEXT("invalid_CDO_type_rejected"));
    Reject(
        Source.Replace(TEXT("ue_ref(type<ActorRef>, ue_self_object())"), TEXT("ue_ref(type<int>, ue_self_object())")),
                       TEXT("reference type"), TEXT("invalid_ref_type_rejected"));
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
    auto* A = World->SpawnActor<ADasActor>();
    auto* B = World->SpawnActor<ADasActor>();
    TStrongObjectPtr<ADasActor> KeepA(A), KeepB(B);
    auto Run = [&](const TCHAR* Entry, int Input, UObject* Owner)
    {
        return R.RunNamedPersistent(Path, Entry, Input, Owner);
    };
    auto Result = Run(TEXT("main"), 2, A);
    Check(Result.bSucceeded && Result.Value == 12, TEXT("state_first_initialization"), Result.Error);
    Result = Run(TEXT("main"), 0, B);
    Check(Result.bSucceeded && Result.Value == 10, TEXT("owners_isolated"), Result.Error);
    Result = Run(TEXT("mutate"), 0, A);
    Check(Result.bSucceeded && Result.Value == 13, TEXT("all_scalar_kinds_mutated"), Result.Error);
    Result = Run(TEXT("inspect"), 0, A);
    Check(Result.bSucceeded && Result.Value == 10, TEXT("localized_text_and_scalar_values"), Result.Error);
    auto Object = R.RunNamedFramePersistent(Path, TEXT("cdo"), {}, EDasMethodSignature::Object, A);
    Check(Object.bSucceeded && Object.ObjectValue.Get() == AActor::StaticClass()->GetDefaultObject(),
          TEXT("typed_CDO_identity"), Object.Error);
    Object = R.RunNamedFramePersistent(Path, TEXT("reference"), {}, EDasMethodSignature::Object, A);
    Check(Object.bSucceeded && Object.ObjectValue.Get() == A, TEXT("generic_reference_identity"), Object.Error);
    Object = R.RunNamedFramePersistent(Path, TEXT("null_reference"), {}, EDasMethodSignature::Object, A);
    Check(Object.bSucceeded && !Object.ObjectValue.IsValid(), TEXT("generic_reference_preserves_null"), Object.Error);
    Object = R.RunNamedFramePersistent(Path, TEXT("minimal_reference"), {}, EDasMethodSignature::Object, A);
    Check(Object.bSucceeded && Object.ObjectValue.Get() == A, TEXT("reference_without_per_type_from_helper"),
                                                                   Object.Error);
    const FString Reload =
        Source.Replace(TEXT("return int(Counter) // REVISION"), TEXT("return int(Counter)+100 // REVISION"));
    const auto Before = Run(TEXT("main"), 0, A);
    Write(Reload);
    Result = Run(TEXT("main"), 0, A);
    Check(Result.bSucceeded && Result.ReloadError.IsEmpty() && Result.Value == 113 && Result.Revision > Before.Revision,
          TEXT("hot_reload_preserves_int"), Result.ReloadError);
    CollectGarbage(RF_NoFlags);
    Result = Run(TEXT("inspect"), 0, A);
    Check(Result.bSucceeded && Result.Value == 13, TEXT("reload_before_dependent_initializer_and_owned_UTF8_string"),
                                                        Result.Error);
    const auto GoodRevision = Result.Revision;
    Write(Reload.Replace(TEXT("Counter += input"), TEXT("verify(ue_is_valid(IllegalCDO));Counter += input")) +
                              TEXT("\nvar IllegalCDO = ue_class_default(type<ActorRef>)\n"));
    Result = Run(TEXT("main"), 0, A);
    Check(Result.bSucceeded && Result.Value == 113 && Result.Revision == GoodRevision &&
          Result.ReloadError.Contains(TEXT("global initialization")),
                                      TEXT("CDO_global_initializer_cannot_change_defaults"), Result.ReloadError);

    const FString KindChange = Reload.Replace(
        TEXT("Counter:int = 10"),
             TEXT("Counter:double = 10.0lf")).Replace(TEXT("Counter += input"), TEXT("Counter += double(input)"));
    Write(KindChange);
    Check(R.ValidateIntMethod(Path, Error), TEXT("type_change_is_syntactically_valid"), Error);
    Result = Run(TEXT("main"), 0, A);
    Check(Result.bSucceeded && Result.Value == 113 && Result.Revision == GoodRevision &&
          Result.ReloadError.Contains(TEXT("ue_state type changed")), TEXT("type_change_keeps_live_VM"),
                                                                           Result.ReloadError);
    Write(Reload.Replace(TEXT("Counter:int = 10"), TEXT("Counter:int = 99")));
    Result = Run(TEXT("main"), 0, A);
    Check(Result.bSucceeded && Result.ReloadError.IsEmpty() && Result.Value == 113,
          TEXT("changed_initializer_keeps_saved_state"), Result.ReloadError);
    Write(Reload);
    Result = Run(TEXT("main"), 0, A);
    Check(Result.bSucceeded && Result.ReloadError.IsEmpty() && Result.Value == 113,
          TEXT("repair_after_rejected_reload"), Result.ReloadError);
    R.ReleaseObject(A);
    Result = Run(TEXT("main"), 0, A);
    Check(Result.bSucceeded && Result.Value == 110, TEXT("release_resets_persistent_state"), Result.Error);
    R.ReleaseObject(A);
    R.ReleaseObject(B);
    FFileHelper::SaveStringToFile(Defaults, *DefaultsPath, FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM);
    FDasAuthoringModule Model;
    const bool Compiled = R.ReadAuthoringScript(DefaultsPath, Model, Error);
    Check(Compiled, TEXT("typed_class_and_loctext_property_defaults_compile"), Error);
    UBlueprint* BP = nullptr;
    const bool Applied =
        Compiled && FDasDSLService::Apply(DefaultsFile, TEXT("/Game/IdentityDSL/")+ Id +
                                                             TEXT("/BP_Defaults"), Model.Classes[0].Id, BP, Error);
    Check(Applied && BP && BP->GeneratedClass, TEXT("Apply_typed_class_and_localized_defaults"), Error);
    if (Applied && BP && BP->GeneratedClass)
    {
        auto* CDO = BP->GeneratedClass->GetDefaultObject();
        auto* ClassProp = FindFProperty<FClassProperty>(BP->GeneratedClass, TEXT("Kind"));
        auto* TextProp = FindFProperty<FTextProperty>(BP->GeneratedClass, TEXT("Caption"));
        Check(ClassProp && ClassProp->GetObjectPropertyValue_InContainer(CDO) == AActor::StaticClass(),
              TEXT("class_field_default_is_UClass_not_CDO"));
        bool TextPinOK = false;
        for (const auto& Graph : BP->FunctionGraphs)
            for (const auto& Node : Graph->Nodes)
                if (auto* Entry = Cast<UK2Node_FunctionEntry>(Node.Get()))
                    if (auto* Pin = Entry->FindPin(TEXT("captionArg")))
                    {
                        const auto NS = FTextInspector::GetNamespace(Pin->DefaultTextValue);
                        const auto Key = FTextInspector::GetKey(Pin->DefaultTextValue);
                        TextPinOK = NS && *NS == TEXT("identity_defaults") && Key && *Key == TEXT("Param");
                    }
        Check(TextPinOK, TEXT("localized_parameter_default_pin"));
        Check(TextProp && FTextInspector::GetNamespace(TextProp->GetPropertyValue_InContainer(CDO)) &&
              *FTextInspector::GetNamespace(TextProp->GetPropertyValue_InContainer(CDO)) ==
                  TEXT("identity_defaults"), TEXT("Apply_keeps_localization_namespace"));
    }
    if (Applied && BP && BP->GeneratedClass)
    {
        const FString RefBase = TEXT("options gen2\nrequire ue_object_types\n[ue_object_ref(script=\"")+ DefaultsFile +
            TEXT("\",type=\"IdentityDefaults\")]\nstruct AuthoredRef {handle:UObjectHandle}\n[export]\ndef cdo_from_class():UObjectHandle {let kind=__CONCRETE__;return ue_class_default(type<AuthoredRef>,kind).handle}\n[export]\ndef authored_null():UObjectHandle{return ue_ref(type<AuthoredRef>,ue_null_object()).handle}\n");
        const FString RefSource =
            RefBase.Replace(TEXT("__CONCRETE__"), *FString::Printf(TEXT("ue_class_loaded(type<AuthoredRef>,\"%s\")"),
                                                                        *BP->GeneratedClass->GetPathName()));
        auto WriteRef = [&](const FString& Value)
        {
            FFileHelper::SaveStringToFile(Value, *AuthoredPath, FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM);
        };
        WriteRef(RefSource);
        auto Authored =
            R.RunNamedFramePersistent(AuthoredPath, TEXT("cdo_from_class"), {}, EDasMethodSignature::Object, A);
        Check(Authored.bSucceeded && Authored.ObjectValue.Get() == BP->GeneratedClass->GetDefaultObject(),
              TEXT("authored_CDO_uses_concrete_class"), Authored.Error);
        auto NullRef =
            R.RunNamedFramePersistent(AuthoredPath, TEXT("authored_null"), {}, EDasMethodSignature::Object, A);
        Check(NullRef.bSucceeded && !NullRef.ObjectValue.IsValid(), TEXT("authored_reference_preserves_null"),
                                                                         NullRef.Error);
        WriteRef(RefBase.Replace(TEXT("__CONCRETE__"), TEXT("ue_class_null()")));
        Authored = R.RunNamedFramePersistent(AuthoredPath, TEXT("cdo_from_class"), {}, EDasMethodSignature::Object, A);
        Check(!Authored.bSucceeded && Authored.Error.Contains(TEXT("null")), TEXT("authored_CDO_null_class_rejected"),
                                                                                  Authored.Error);
        WriteRef(
            RefBase.Replace(TEXT("__CONCRETE__"), TEXT("ue_class_loaded(type<ActorRef>,\"/Script/Engine.Actor\")")));
        Authored = R.RunNamedFramePersistent(AuthoredPath, TEXT("cdo_from_class"), {}, EDasMethodSignature::Object, A);
        Check(!Authored.bSucceeded && Authored.Error.Contains(TEXT("incompatible base")),
                                                              TEXT("authored_CDO_wrong_class_rejected"),
                                                                   Authored.Error);
        const FString Ambiguous = RefSource.Replace(TEXT("type<AuthoredRef>,kind"), TEXT("type<AuthoredRef>"));
        FFileHelper::SaveStringToFile(Ambiguous, *AuthoredPath, FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM);
        Check(!R.ReadAuthoringScript(AuthoredPath, Model, Error) &&
              Error.Contains(TEXT("concrete class handle")), TEXT("ambiguous_authored_CDO_rejected_at_compile"), Error);
        R.ReleaseObject(A);
    }
    for (const TCHAR* Example :
         {TEXT("persistent_counter.das"), TEXT("lifecycle_actor.das"), TEXT("ai_reload_probe.das"),
                                                                            TEXT("effects_reload_probe.das"),
                                                                                 TEXT("montage_reload_probe.das")})
        Check(R.ValidateIntMethod(R.ScriptPath(Example), Error), Example, Error);
    Write(Source + TEXT("\n[export]\ndef path_hint():string {return \"/Script/Engine.Actor\"}\n"));
    Check(R.ValidateIntMethod(Path, Error), TEXT("known_literal_path_warns_without_failure"), Error);
    UE_LOG(LogTemp, Display,
           TEXT("DAS_IDENTITY_DSL_RESULT=%s checks=%d failures=%d"),
                Failures ? TEXT("FAIL"): TEXT("PASS"), Checks, Failures);
    return Failures ? 1 : 0;
}
