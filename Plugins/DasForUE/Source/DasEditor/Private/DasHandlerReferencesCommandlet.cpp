#include "DasHandlerReferencesCommandlet.h"
#include "DasAuthoring.h"
#include "DasDSLService.h"
#include "DasRuntime.h"
#include "DasProbeActor.h"
#include "../../DasRuntime/Private/DasScriptDelegateProxy.h"
#include "Engine/Blueprint.h"
#include "Engine/World.h"
#include "Engine/Engine.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "Misc/ScopeExit.h"
#include "HAL/FileManager.h"
#include "UObject/StrongObjectPtr.h"
#include "UObject/Script.h"
#include "UObject/StructOnScope.h"
#include "UObject/GarbageCollection.h"
#include "UObject/UObjectIterator.h"
#include "TimerManager.h"
#include "Containers/Ticker.h"
UDasHandlerReferencesCommandlet::UDasHandlerReferencesCommandlet()
{
    IsClient = false;
    IsServer = false;
    IsEditor = true;
    LogToConsole = true;
}
int32 UDasHandlerReferencesCommandlet::Main(const FString&)
{
    int32 Checks = 0, Failures = 0;
    auto Check = [&](bool OK, const TCHAR* N, const FString& E = FString())
    {
        ++Checks;
        if (!OK)
            ++Failures;
        UE_LOG(LogTemp, Display, TEXT("DAS_HANDLER_REF_%s %s %s"), OK ? TEXT("PASS"): TEXT("FAIL"), N, *E);
    };
    auto& R = FModuleManager::LoadModuleChecked<FDasRuntimeModule>(TEXT("DasRuntime"));
    const FString Repo = FPaths::ConvertRelativePathToFull(FPaths::ProjectDir() / TEXT("../.."));
    FString Authored, Free, Error;
    if (!FFileHelper::LoadFileToString(Authored, *(Repo / TEXT("Tests/HandlerReferences/authored.das"))) ||
                                       !FFileHelper::LoadFileToString(
                                           Free, *(Repo / TEXT("Tests/HandlerReferences/free.das"))))
        return 2;
    const FString Id = FGuid::NewGuid().ToString(EGuidFormats::Digits),
                  File = TEXT("handler_refs_")+ Id + TEXT(".das"), Path = R.ScriptPath(File);
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
        Write(S);
        FDasAuthoringModule M;
        return R.ReadAuthoringScript(Path, M, Error);
    };
    auto FreeCompile = [&](const FString& S)
    {
        Write(S);
        return R.ValidateIntMethod(Path, Error);
    };
    auto Bad = [&](const FString& S, bool Plain, const TCHAR* N, const TCHAR* Hint)
    {
        const bool OK = Plain ? FreeCompile(S) : Compile(S);
        Check(!OK && Error.Contains(Hint) && !Error.Contains(TEXT("syntax error")), N, Error);
    };
    Check(Compile(Authored), TEXT("all_self_delegate_reference_forms_compile"), Error);
    FDasAuthoringModule Model;
    Write(Authored);
    const bool Read = R.ReadAuthoringScript(Path, Model, Error);
    auto Hidden = [&](const TCHAR* N)
    {
        return Read &&
               Model.Classes[0].Methods.ContainsByPredicate(
                   [&](const auto& M)
                   { return M.Name == N && M.Metadata.FindRef(TEXT("BlueprintInternalUseOnly")) == TEXT("true"); });
    };
    Check(Hidden(TEXT("Query")) && Hidden(TEXT("Hit")) && Hidden(TEXT("Mutate")),
                                                                 TEXT("void_result_and_refout_handlers_hidden"), Error);
    Bad(Authored.Replace(TEXT("(),self.Query)"), TEXT("(),self.Missing)")), false,
                         TEXT("unknown_factory_method_rejected"), TEXT("Missing"));
    Bad(Authored.Replace(TEXT("rebound.BindUFunction(ue_self_object(),self.Query)"),
                              TEXT("rebound.BindUFunction(ue_self_object(),self.Missing)")),
                         false, TEXT("unknown_bind_method_rejected"), TEXT("Missing"));
    Bad(Authored.Replace(TEXT("signal.AddUFunction(ue_self_object(),self.Hit)"),
                              TEXT("signal.AddUFunction(ue_self_object(),self.Missing)")),
                         false, TEXT("unknown_add_method_rejected"), TEXT("Missing"));
    Bad(Authored.Replace(TEXT("signal.RemoveUFunction(ue_self_object(),self.Hit)"),
                              TEXT("signal.RemoveUFunction(ue_self_object(),self.Missing)")),
                         false, TEXT("unknown_remove_method_rejected"), TEXT("Missing"));
    Bad(Authored.Replace(TEXT("RefQuery {signature:function<(value:int):int>}"),
                              TEXT("RefQuery {signature:function<(value:int):float>}")),
                         false, TEXT("self_wrong_result_type_rejected"), TEXT("signature mismatch"));
    Bad(Authored.Replace(TEXT("RefEvent {signature:function<(value:int):void>}"),
                              TEXT("RefEvent {signature:function<(value:float):void>}")),
                         false, TEXT("self_wrong_parameter_type_rejected"), TEXT("signature mismatch"));
    Bad(Authored.Replace(TEXT("def Mutate(var value:int&;@ue_out var accepted:bool&)"),
                              TEXT("def Mutate(var value:int&;var accepted:bool&)")),
                         false, TEXT("self_refout_direction_rejected"), TEXT("signature mismatch"));
    Check(FreeCompile(Free), TEXT("all_free_delegate_and_managed_reference_forms_compile"), Error);
    Bad(Free.Replace(TEXT("@@FreeQuery"), TEXT("@@Missing")), true,
                     TEXT("unknown_free_function_rejected"), TEXT("Missing"));
    Bad(Free.Replace(TEXT("@@FreeQuery"), TEXT("@@Wrong")), true,
                     TEXT("free_wrong_parameter_type_rejected"), TEXT("signature mismatch"));
    Bad(Free.Replace(TEXT("RefQuery {signature:function<(value:int):int>}"),
                          TEXT("RefQuery {signature:function<(value:int):float>}")),
                     true, TEXT("free_wrong_result_type_rejected"), TEXT("signature mismatch"));
    Bad(Free.Replace(TEXT("def Mutate(var value:int&;@ue_out var accepted:bool&)"),
                          TEXT("def Mutate(var value:int&;var accepted:bool&)")),
                     true, TEXT("free_refout_direction_rejected"), TEXT("signature mismatch"));
    Bad(Free.Replace(TEXT("def Pulse():void {Total+=1}"), TEXT("def Pulse():int {return 1}")), true,
                     TEXT("managed_free_result_rejected"), TEXT("return void"));
    Bad(Free.Replace(TEXT("def Pulse():void"), TEXT("def Pulse(value:int):void")), true,
                     TEXT("managed_free_arity_rejected"), TEXT("argument count"));
    Write(Authored);
    UBlueprint* BP = nullptr;
    Check(FDasDSLService::Apply(File, TEXT("/Game/DasHandlerReferences/")+ Id +
                                           TEXT("/BP_Refs"), TEXT("handlerrefs.ReferenceActor"), BP, Error),
                                           TEXT("Apply_checked_self_references"), Error);
    if (!BP || !BP->GeneratedClass)
        return 1;
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
    FEditorScriptExecutionGuard AllowScript;
    auto* Actor = World->SpawnActor<ADasProbeActor>(BP->GeneratedClass);
    TStrongObjectPtr<ADasProbeActor> Keep(Actor);
    auto Started = R.RunLifecycle(Path, EDasCallback::Tick, Actor, .01f);
    auto Exercise =
        R.RunNamedFramePersistent(Path, TEXT("ue_ReferenceActor_Exercise"), {}, EDasMethodSignature::Int, Actor);
    Check(Started.bSucceeded && Exercise.bSucceeded && Exercise.Value == 28,
          TEXT("self_factory_bind_add_remove_and_refout_runtime"), Started.Error + Exercise.Error);
    FDasScalarValue Other;
    Other.Kind = int32(EDasMethodSignature::Object);
    Other.ObjectClass = TEXT("/Script/Engine.Actor");
    Other.ObjectValue = World->SpawnActor<AActor>();
    const auto Wrong = R.RunNamedFramePersistent(Path, TEXT("ue_ReferenceActor_WrongTarget"), {Other},
                                                            EDasMethodSignature::Void, Actor);
    Check(!Wrong.bSucceeded && Wrong.Error.Contains(TEXT("current self")),
                                                    TEXT("self_method_cannot_bind_another_target"), Wrong.Error);
    R.ReleaseObject(Actor);
    Write(Free);
    auto* Owner = World->SpawnActor<ADasProbeActor>();
    TStrongObjectPtr<ADasProbeActor> KeepOwner(Owner);
    const auto Main = R.RunPersistent(Path, 0, Owner);
    Check(Main.bSucceeded && Main.Value == 42, TEXT("free_factory_bind_add_remove_refout_object_array_runtime"),
                                                    Main.Error);
    if (!Main.bSucceeded)
        return 1;
    auto ReadTotal = [&]()
    {
        return R.RunNamedFramePersistent(Path, TEXT("read"), {}, EDasMethodSignature::Int, Owner);
    };
    Check(ReadTotal().Value == 2, TEXT("free_multicast_add_is_unique_and_remove_matches_function"));
    Owner->OnDasSignal.Broadcast(3);
    Check(ReadTotal().Value == 5 && Owner->OnDasSignal.GetAllObjects().Num() == 1,
          TEXT("free_native_event_uses_vm_proxy"));
    const auto Timer = R.RunNamedVoidPersistent(Path, TEXT("timer"), Owner);
    for (int32 I = 0; I < 2; ++I)
    {
        ++GFrameCounter;
        FWorldDelegates::OnWorldTickStart.Broadcast(World.Get(), LEVELTICK_All, .02f);
        World->GetTimerManager().Tick(.02f);
        FTSTicker::GetCoreTicker().Tick(.02f);
    }
    Check(Timer.bSucceeded && ReadTotal().Value == 6, TEXT("unannotated_free_timer_function_exported_and_delivered"),
                                                           Timer.Error);
    UDasScriptDelegateProxy* Proxy = nullptr;
    for (TObjectIterator<UDasScriptDelegateProxy> It; It; ++It)
        if (It->Owner == Owner && It->Schema.ReturnKind == EDasMethodSignature::Int &&
            It->Schema.Parameters.Num() == 1 && It->Schema.Parameters[0].Kind == EDasMethodSignature::Int)
        {
            Proxy = *It;
            break;
        }
    TStrongObjectPtr<UDasScriptDelegateProxy> KeepProxy(Proxy);
    Check(Proxy && Proxy->IsActive(), TEXT("free_delegate_proxy_retained_by_VM"));
    if (!Proxy)
        return 1;
    FScriptDelegate D;
    D.BindUFunction(Proxy, *Proxy->Schema.EntryPoint);
    auto* Signature = Proxy->FindFunction(D.GetFunctionName());
    FStructOnScope Params(Signature);
    FindFProperty<FIntProperty>(Signature, TEXT("value"))->SetPropertyValue_InContainer(Params.GetStructMemory(), 4);
    D.ProcessDelegate<UObject>(Params.GetStructMemory());
    Check(CastFieldChecked<FIntProperty>(Signature->GetReturnProperty())
                  ->GetPropertyValue_InContainer(Params.GetStructMemory()) == 14,
          TEXT("native_delegate_invokes_free_result_synchronously"));
    CollectGarbage(RF_NoFlags);
    Owner->OnDasSignal.Broadcast(1);
    Check(Proxy->IsActive() && ReadTotal().Value == 7, TEXT("free_proxy_and_snapshot_contract_survive_GC"));
    const auto Stop = R.RunNamedVoidPersistent(Path, TEXT("stop"), Owner);
    Owner->OnDasSignal.Broadcast(100);
    Check(Stop.bSucceeded && Owner->OnDasSignal.GetAllObjects().IsEmpty() && ReadTotal().Value == 7,
          TEXT("free_native_event_remove_cleans_owned_registration"), Stop.Error);
    Write(Free.Replace(TEXT("return value+10"), TEXT("return missing+10")));
    const auto Rejected = ReadTotal();
    Check(Rejected.bSucceeded && !Rejected.ReloadError.IsEmpty() && Proxy->IsActive(),
          TEXT("rejected_reload_keeps_free_proxy_lease"), Rejected.ReloadError);
    Write(Free.Replace(TEXT("// RELOAD_MARKER"), TEXT("// accepted replacement")));
    FDasScalarValue NativeInput;
    NativeInput.Kind = int32(EDasMethodSignature::Int);
    NativeInput.IntValue = 4;
    const auto StaleDispatch = R.RunScriptDelegateFrame(Proxy->Lease.Pin(), Path, Proxy->Entry, {NativeInput},
                                                        Proxy->Schema.ReturnKind, Owner, Proxy->Schema.ReturnClass);
    const auto Reload = ReadTotal();
    Check(!StaleDispatch.bSucceeded &&
          StaleDispatch.Error.Contains(TEXT("generation")) && Reload.bSucceeded && Reload.ReloadError.IsEmpty() &&
                                       Reload.Revision > Main.Revision && !Proxy->IsActive(),
                                       TEXT("accepted_reload_revokes_old_proxy_even_if_native_pinned"),
                                            Reload.Error + Reload.ReloadError);
    const auto NewMain = R.RunPersistent(Path, 0, Owner);
    Check(NewMain.bSucceeded && NewMain.Value == 42 && Owner->OnDasSignal.GetAllObjects().Num() == 1,
          TEXT("free_rebind_after_reload_has_one_registration"), NewMain.Error);
    const auto End = R.RunLifecycle(Path, EDasCallback::EndPlay, Owner);
    Check(End.bSucceeded && Owner->OnDasSignal.GetAllObjects().IsEmpty(),
          TEXT("free_EndPlay_without_BeginPlay_revokes_native_bindings"), End.Error);
    R.ReleaseObject(Owner);
    Check(Owner->OnDasSignal.GetAllObjects().IsEmpty(), TEXT("release_detaches_free_native_event_and_revokes_lease"));
    const auto Restart = R.RunPersistent(Path, 0, Owner);
    Check(Restart.bSucceeded && Owner->OnDasSignal.GetAllObjects().Num() == 1, TEXT("free_restart_before_destroy"),
                                                                                    Restart.Error);
    Owner->Destroy();
    FTSTicker::GetCoreTicker().Tick(.02f);
    R.PersistentContextCount();
    Check(!Proxy->IsActive() && R.SubscriptionCount(Owner) == 0,
          TEXT("destroy_removes_owner_session_and_proxy_capabilities"));
    UE_LOG(LogTemp, Display,
           TEXT("DAS_HANDLER_REF_RESULT=%s checks=%d"), Failures ? TEXT("FAIL"): TEXT("PASS"), Checks);
    return Failures ? 1 : 0;
}
