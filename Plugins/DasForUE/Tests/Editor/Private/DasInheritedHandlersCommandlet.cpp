#include "DasInheritedHandlersCommandlet.h"
#include "DasAuthoring.h"
#include "DasDSLService.h"
#include "DasRuntime.h"
#include "DasActor.h"
#include "Engine/Blueprint.h"
#include "Engine/World.h"
#include "Engine/Engine.h"
#include "GameFramework/PlayerController.h"
#include "EnhancedInputComponent.h"
#include "EnhancedPlayerInput.h"
#include "InputAction.h"
#include "TimerManager.h"
#include "Containers/Ticker.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "Misc/ScopeExit.h"
#include "HAL/FileManager.h"
#include "UObject/StrongObjectPtr.h"
#include "UObject/Script.h"
UDasInheritedHandlersCommandlet::UDasInheritedHandlersCommandlet()
{
    IsClient = false;
    IsServer = false;
    IsEditor = true;
    LogToConsole = true;
}
int32 UDasInheritedHandlersCommandlet::Main(const FString&)
{
    int32 Checks = 0, Failures = 0;
    auto Check = [&](bool OK, const TCHAR* Name, const FString& Error = FString())
    {
        ++Checks;
        if (!OK)
            ++Failures;
        UE_LOG(LogTemp, Display, TEXT("DAS_INHERITED_HANDLER_%s %s %s"), OK ? TEXT("PASS"): TEXT("FAIL"), Name, *Error);
    };
    auto& R = FModuleManager::LoadModuleChecked<FDasRuntimeModule>(TEXT("DasRuntime"));
    const FString Repo = FPaths::ConvertRelativePathToFull(FPaths::ProjectDir() / TEXT("../.."));
    FString Base, Middle, Leaf, Error;
    if (!FFileHelper::LoadFileToString(Base, *(Repo / TEXT("Tests/InheritedHandlers/base.das"))) ||
                                       !FFileHelper::LoadFileToString(
                                           Middle, *(Repo / TEXT("Tests/InheritedHandlers/middle.das"))) ||
                                           !FFileHelper::LoadFileToString(
                                               Leaf, *(Repo / TEXT("Tests/InheritedHandlers/leaf.das"))))
        return 2;
    const FString Id = FGuid::NewGuid().ToString(EGuidFormats::Digits);
    const FString B = TEXT("inherited_base_")+ Id, M = TEXT("inherited_middle_")+ Id, L = TEXT("inherited_leaf_")+ Id;
    const FString BPth = R.ScriptPath(B + TEXT(".das")), MPth = R.ScriptPath(M + TEXT(".das")),
                                                                             LPth = R.ScriptPath(L + TEXT(".das"));
    Middle = Middle.Replace(TEXT("@BASE@"), *B);
    Leaf = Leaf.Replace(TEXT("@MIDDLE@"), *M);
    auto Write = [](const FString& P, const FString& S)
    {
        return FFileHelper::SaveStringToFile(S, *P, FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM);
    };
    ON_SCOPE_EXIT
    {
        IFileManager::Get().Delete(*BPth, false, true);
        IFileManager::Get().Delete(*MPth, false, true);
        IFileManager::Get().Delete(*LPth, false, true);
    };
    Write(BPth, Base);
    Write(MPth, Middle);
    Write(LPth, Leaf);
    FDasAuthoringModule Model;
    Check(R.ReadAuthoringScript(LPth, Model, Error), TEXT("three_level_imported_hierarchy_compiles"), Error);
    if (Model.Classes.IsEmpty())
        return 1;
    auto Effective = [&](const TCHAR* Name, FDasDeclaredClass& Out)
    {
        return Model.ResolveClass(FString(TEXT("handlerinherit.")) + Name, Out, Error);
    };
    FDasDeclaredClass BC, MC, LC;
    Check(Effective(TEXT("CallbackBase"), BC) && Effective(TEXT("CallbackMiddle"), MC) &&
                                                           Effective(TEXT("CallbackLeaf"), LC),
                                                                     TEXT("effective_hierarchy_resolves"), Error);
    auto Hidden = [](const FDasDeclaredClass& C, const TCHAR* N)
    {
        const auto* V = C.Methods.FindByPredicate([&](const auto& X) { return X.Name == N; });
        return V && V->bBlueprintEvent && V->Metadata.FindRef(TEXT("BlueprintInternalUseOnly")) == TEXT("true");
    };
    Check(Hidden(BC, TEXT("Pulse")) && Hidden(LC, TEXT("Pulse")) &&
                                              Hidden(LC, TEXT("Query")) &&
                                                     Hidden(LC, TEXT("Input")),
                                                            TEXT("hidden_handlers_virtual_and_hidden"));
    Check(!BC.Methods.ContainsByPredicate([](const auto& X) { return X.Name == TEXT("Bump"); }) &&
          Hidden(MC, TEXT("Bump")), TEXT("child_adapter_does_not_mutate_imported_parent"));
    Check(LC.Methods.ContainsByPredicate(
              [&](const auto& X) { return X.Name == TEXT("Query")&& X.EntryPoint == TEXT("ue_CallbackLeaf_Query"); }),
          TEXT("nearest_override_selected_in_IR"));
    Check(LC.Methods.ContainsByPredicate(
              [](const auto& V)
              {
                  return V.Name == TEXT("PublicCallback") && V.bBlueprintEvent &&
                                        !V.Metadata.Contains(TEXT("BlueprintInternalUseOnly"));
              }),
          TEXT("visible_base_callback_policy_inherited_without_annotation"));
    auto LocalPart = [](const FString& Text, const FString& Import)
    {
        TArray<FString> Lines;
        Text.ParseIntoArrayLines(Lines, false);
        FString Out;
        for (const auto& Line : Lines)
        {
            const auto Trimmed = Line.TrimStartAndEnd();
            if (!Trimmed.StartsWith(TEXT("options ")) &&
                                    Trimmed != TEXT("require ")+ Import +
                                                    TEXT(" public") && !Trimmed.StartsWith(TEXT("def Tick(")) &&
                                                                                           !Trimmed.StartsWith(TEXT("def override Tick(")))
                Out += Line + TEXT("\n");
        }
        return Out;
    };
    // Lifecycle inheritance still requires separate files; this probe isolates method inheritance.
    const FString SameFile =
        TEXT("options gen2\n") + LocalPart(Base, FString()) + LocalPart(Middle, B) + LocalPart(Leaf, M);
    Write(LPth, SameFile);
    FDasAuthoringModule SameModel;
    Check(R.ReadAuthoringScript(LPth, SameModel, Error), TEXT("same_file_handler_hierarchy_compiles_without_lifecycle"),
                                                              Error);
    Write(LPth, Leaf);
    Write(LPth, Leaf.Replace(TEXT("def override Pulse():void"),
                                  TEXT("[ue_function(BlueprintCallable)]\n def override Pulse():void")));
    FDasAuthoringModule VisibleModel;
    FDasDeclaredClass VisibleLeaf;
    const bool VisibleOK = R.ReadAuthoringScript(LPth, VisibleModel, Error) &&
                           VisibleModel.ResolveClass(TEXT("handlerinherit.CallbackLeaf"), VisibleLeaf, Error);
    Check(VisibleOK && VisibleLeaf.Methods.ContainsByPredicate(
                           [](const auto& V)
                           {
                               return V.Name == TEXT("Pulse") && V.bBlueprintEvent &&
                                                     !V.Metadata.Contains(TEXT("BlueprintInternalUseOnly"));
                           }),
          TEXT("explicit_visibility_preserves_inherited_virtual_contract"), Error);
    Write(LPth, Leaf);
    auto Bad = [&](const FString& P, const FString& S, const TCHAR* Name)
    {
        Write(P, S);
        FDasAuthoringModule Invalid;
        const bool OK = R.ReadAuthoringScript(LPth, Invalid, Error);
        Check(!OK && !Error.IsEmpty() && !Error.Contains(TEXT("syntax error")), Name, Error);
    };
    Bad(MPth, Middle.Replace(TEXT("self.Bump)"), TEXT("self.Missing)")), TEXT("unknown_inherited_handler_rejected"));
    Write(MPth, Middle);
    Bad(LPth, Leaf.Replace(TEXT("override Query(value:int):int"), TEXT("override Query(value:float):int")),
                           TEXT("override_parameter_type_change_rejected"));
    Write(LPth, Leaf);
    Bad(LPth, Leaf.Replace(TEXT("override Pulse():void"), TEXT("override Pulse(value:int):void")),
                           TEXT("override_arity_change_rejected"));
    Write(LPth, Leaf);
    Bad(LPth, Leaf.Replace(TEXT("override Mutate(var value:int&;@ue_out var accepted:bool&)"),
                                TEXT("override Mutate(var value:int&;var accepted:bool&)")),
                           TEXT("override_out_direction_change_rejected"));
    Write(LPth, Leaf);
    UBlueprint* BaseBP = nullptr;
    UBlueprint* MidBP = nullptr;
    UBlueprint* LeafBP = nullptr;
    const FString Asset = TEXT("/Game/DasInheritedHandlers/")+ Id;
    Check(FDasDSLService::Apply(B + TEXT(".das"), Asset / TEXT("BP_Base"),
                                                               TEXT("handlerinherit.CallbackBase"), BaseBP, Error),
                                                      TEXT("Apply_base"), Error);
    Check(FDasDSLService::Apply(M + TEXT(".das"), Asset / TEXT("BP_Middle"),
                                                               TEXT("handlerinherit.CallbackMiddle"), MidBP, Error),
                                                      TEXT("Apply_middle"), Error);
    Check(FDasDSLService::Apply(L + TEXT(".das"), Asset / TEXT("BP_Leaf"),
                                                               TEXT("handlerinherit.CallbackLeaf"), LeafBP, Error),
                                                      TEXT("Apply_leaf"), Error);
    if (!LeafBP || !LeafBP->GeneratedClass)
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
    FEditorScriptExecutionGuard Allow;
    auto* A = World->SpawnActor<ADasActor>(LeafBP->GeneratedClass);
    TStrongObjectPtr<ADasActor> Keep(A);
    auto Value = [&](const TCHAR* N)
    {
        return FindFProperty<FIntProperty>(A->GetClass(), N)->GetPropertyValue_InContainer(A);
    };
    auto Call = [&](const TCHAR* N)
    {
        A->ProcessEvent(A->FindFunctionChecked(N), nullptr);
    };
    auto Query = [&](const TCHAR* N)
    {
        int32 Result = 0;
        A->ProcessEvent(A->FindFunctionChecked(N), &Result);
        return Result;
    };
    auto Step = [&]()
    {
        ++GFrameCounter;
        FWorldDelegates::OnWorldTickStart.Broadcast(World.Get(), LEVELTICK_All, .02f);
        World->GetTimerManager().Tick(.02f);
        FTSTicker::GetCoreTicker().Tick(.02f);
    };
    auto* IC = NewObject<UEnhancedInputComponent>(A);
    A->InputComponent = IC;
    IC->RegisterComponent();
    auto* PC = World->SpawnActor<APlayerController>();
    TStrongObjectPtr<UEnhancedPlayerInput> Input(NewObject<UEnhancedPlayerInput>(PC));
    PC->PlayerInput = Input.Get();
    TStrongObjectPtr<UInputAction> Action(NewObject<UInputAction>());
    Action->ValueType = EInputActionValueType::Axis1D;
    FindFProperty<FObjectPropertyBase>(A->GetClass(),
                                       TEXT("Action"))->SetObjectPropertyValue_InContainer(A, Action.Get());
    auto Inject = [&]()
    {
        TArray<UInputComponent*> Stack{IC};
        Input->InjectInputForAction(Action.Get(), FInputActionValue(2.f));
        Input->ProcessInputStack(Stack, .016f, false);
        FTSTicker::GetCoreTicker().Tick(.02f);
    };
    Check(Query(TEXT("BaseQuery")) == 111, TEXT("base_delegate_reference_dispatches_leaf_override"), A->LastDasError);
    Check(Query(TEXT("BaseMutation")) == 108, TEXT("inherited_delegate_refout_and_Super_preserve_caller_storage"),
                                                   A->LastDasError);
    Check(Query(TEXT("ChildQuery")) == 112, TEXT("inherited_BindUFunction_dispatches_leaf_override"), A->LastDasError);
    Check(Query(TEXT("LeafQuery")) == 113, TEXT("nearest_self_reference_dispatches_override_and_Super"),
                                                A->LastDasError);
    Call(TEXT("StartBase"));
    Step();
    Step();
    Check(Value(TEXT("Pulses")) == 11, TEXT("base_timer_registration_dispatches_leaf_override"), A->LastDasError);
    Check(Value(TEXT("PublicHits")) == 55, TEXT("visible_base_callback_registration_dispatches_leaf_override"),
                                                A->LastDasError);
    Inject();
    Check(Value(TEXT("InputTotal")) == 22, TEXT("base_input_registration_dispatches_leaf_override"), A->LastDasError);
    Call(TEXT("StartChild"));
    Step();
    Step();
    Check(Value(TEXT("Pulses")) == 22 && Value(TEXT("PublicHits")) == 132,
                                               TEXT("inherited_timer_reference_dispatches_leaf_override"),
                                                    A->LastDasError);
    Check(Value(TEXT("Bumps")) == 29, TEXT("script_only_ancestor_adapter_and_Super_execute"), A->LastDasError);
    Check(Value(TEXT("PayloadTotal")) == 82, TEXT("inherited_scalar_and_multi_capture_dispatch"), A->LastDasError);
    Call(TEXT("ChildEvents"));
    Check(Value(TEXT("HitTotal")) == 22, TEXT("inherited_Add_Remove_use_same_override"), A->LastDasError);
    Call(TEXT("BindBase"));
    auto* Event = FindFProperty<FMulticastDelegateProperty>(A->GetClass(), TEXT("Changed"));
    int32 Signal = 3;
    Event->GetMulticastDelegate(Event->ContainerPtrToValuePtr<void>(A))->ProcessDelegate<UObject>(&Signal);
    Check(Value(TEXT("HitTotal")) == 55, TEXT("base_event_binding_dispatches_leaf_override"), A->LastDasError);
    Call(TEXT("UnbindBase"));
    Event->GetMulticastDelegate(Event->ContainerPtrToValuePtr<void>(A))->ProcessDelegate<UObject>(&Signal);
    Check(Value(TEXT("HitTotal")) == 55, TEXT("base_event_remove_detaches_leaf_override"), A->LastDasError);
    Call(TEXT("Clicked"));
    Check(Value(TEXT("Bumps")) == 129, TEXT("inherited_widget_handler_reflected_dispatch"), A->LastDasError);
    auto First = R.RunLifecycle(LPth, EDasCallback::Tick, A, .01f);
    Write(LPth, Leaf.Replace(TEXT("self.Pulses+=10"), TEXT("self.Pulses+=missing")));
    auto Rejected = R.RunLifecycle(LPth, EDasCallback::Tick, A, .01f);
    Check(Rejected.bSucceeded && !Rejected.ReloadError.IsEmpty(), TEXT("rejected_override_reload_preserves_VM"),
                                                                       Rejected.Error + Rejected.ReloadError);
    Write(LPth, Leaf.Replace(TEXT("self.Pulses+=10"), TEXT("self.Pulses+=100")));
    auto Reload = R.RunLifecycle(LPth, EDasCallback::Tick, A, .01f);
    Check(First.bSucceeded && Reload.bSucceeded && Reload.ReloadError.IsEmpty() && Reload.Revision > First.Revision,
          TEXT("body_only_override_reload"), Reload.Error + Reload.ReloadError);
    int32 Before = Value(TEXT("Pulses"));
    Call(TEXT("StartBase"));
    Step();
    Step();
    Check(Value(TEXT("Pulses")) == Before + 101, TEXT("base_registration_observes_reloaded_leaf_override"),
                                                      A->LastDasError);
    R.ReleaseObject(A);
    Check(R.ScriptTimerCount(A) == 0 && R.SubscriptionCount(A) == 0 && IC->GetActionEventBindings().Num() == 0,
          TEXT("release_cleans_all_inheritance_sessions"));
    A->Destroy();
    Step();
    Check(R.SubscriptionCount(A) == 0, TEXT("destroy_keeps_handlers_detached"));
    UE_LOG(LogTemp, Display,
           TEXT("DAS_INHERITED_HANDLER_RESULT=%s checks=%d"), Failures ? TEXT("FAIL"): TEXT("PASS"), Checks);
    return Failures ? 1 : 0;
}
