#include "DasWidgetLayoutsCommandlet.h"
#include "DasDSLService.h"
#include "DasRuntime.h"
#include "DasWidgetLayouts.h"
#include "WidgetBlueprint.h"
#include "Blueprint/WidgetBlueprintGeneratedClass.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/WidgetTree.h"
#include "Components/VerticalBox.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "Kismet2/KismetEditorUtilities.h"
#include "Misc/FileHelper.h"
#include "Misc/ScopeExit.h"
#include "Misc/PackageName.h"
#include "Engine/World.h"
#include "Engine/Engine.h"
#include "Engine/GameInstance.h"
#include "Engine/LocalPlayer.h"
#include "Framework/Application/SlateApplication.h"
#include "Interfaces/ISlateNullRendererModule.h"
#include "Null/NullApplication.h"
#include "UObject/GarbageCollection.h"
#include "HAL/FileManager.h"
#include "UObject/StrongObjectPtr.h"
UDasWidgetLayoutsCommandlet::UDasWidgetLayoutsCommandlet(){IsClient=false;IsServer=false;IsEditor=true;LogToConsole=true;}
void ADasWidgetLayoutController::ConstructHUD(UUserWidget* Widget){HUD=Widget;Slate=Widget->TakeWidget();}
int32 UDasWidgetLayoutsCommandlet::Main(const FString& Params){
 int32 Checks=0,Failures=0;auto Check=[&](bool Pass,const TCHAR* Name,const FString& Detail=FString()){++Checks;if(!Pass)++Failures;UE_LOG(LogTemp,Display,TEXT("DAS_WIDGET_LAYOUT_%s %s %s"),Pass?TEXT("PASS"):TEXT("FAIL"),Name,*Detail);};
 auto& Runtime=FModuleManager::LoadModuleChecked<FDasRuntimeModule>(TEXT("DasRuntime"));const FString Id=FGuid::NewGuid().ToString(EGuidFormats::Digits);TArray<FString> Scratch;ON_SCOPE_EXIT{for(const auto& Path:Scratch)IFileManager::Get().Delete(*Path,false,true);};
 auto* Package=CreatePackage(*(TEXT("/Game/DasWidgetLayouts/")+Id+TEXT("/WBP_HUD")));
 TStrongObjectPtr<UWidgetBlueprint> WBP(Cast<UWidgetBlueprint>(FKismetEditorUtilities::CreateBlueprint(UUserWidget::StaticClass(),Package,TEXT("WBP_HUD"),BPTYPE_Normal,UWidgetBlueprint::StaticClass(),UWidgetBlueprintGeneratedClass::StaticClass())));
 if(!WBP){Check(false,TEXT("create_widget_blueprint"));return 1;}
 auto* Tree=WBP->WidgetTree.Get();auto* Root=Tree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass(),TEXT("Root"));Tree->RootWidget=Root;
 Root->AddChild(Tree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(),TEXT("Score")));Root->AddChild(Tree->ConstructWidget<UButton>(UButton::StaticClass(),TEXT("Damage")));FKismetEditorUtilities::CompileBlueprint(WBP.Get());Check(WBP->Status==BS_UpToDate,TEXT("compiled_WBP_tree_available"));
 auto Script=[&](const FString& Name,const FString& Field,const FString& Type){const FString File=TEXT("widget_layout_")+Id+TEXT("_")+Name+TEXT(".das");const FString Text=FString::Printf(TEXT("options gen2\nrequire ue_gameplay_services\n[ue_widget_layout]\nstruct HUDView { %s:%s; Damage:ButtonRef }\n[ue_class(module_id=\"widgetlayout\",Blueprintable)]\nclass LayoutActor {\n @ue_property=(EditAnywhere,BlueprintReadWrite)\n HUDClass:UserWidgetClass=ue_class_default(\"%s\")\n @ue_property=(BlueprintReadWrite)\n HUD:UserWidgetRef\n [ue_function(BlueprintCallable)]\n def Setup(player:PlayerControllerRef):void { self.HUD=ue_widget_create(self.HUDClass,player); let view=ue_widget_view(type<HUDView>,self.HUD);ue_widget_bind_click(view.Damage,self.Clicked) }\n [ue_function(BlueprintCallable)]\n def Clicked():void {}\n}\n"),*Field,*Type,*WBP->GeneratedClass->GetPathName());const FString Path=Runtime.ScriptPath(File);FFileHelper::SaveStringToFile(Text,*Path);Scratch.Add(Path);return File;};
 FString Error;FDasAuthoringModule Model;const FString Good=Script(TEXT("good"),TEXT("Score"),TEXT("TextBlockRef"));
 Check(Runtime.ReadAuthoringScript(Runtime.ScriptPath(Good),Model,Error),TEXT("typed_view_and_button_method_compile"),Error);
 const auto* C=Model.Classes.IsEmpty()?nullptr:&Model.Classes[0];Check(C && C->WidgetLayouts.Num()==1 && C->WidgetLayouts[0].Fields.Num()==2 && C->WidgetLayouts[0].ClassField==TEXT("HUDClass"),TEXT("layout_and_HUD_class_origin_recorded_in_IR"));
 FString Wire;FDasAuthoringModule Roundtrip;Check(Model.ToJson(Wire,Error) && FDasAuthoringModule::FromJson(Wire,Roundtrip,Error) && C && Roundtrip.Classes[0].WidgetLayouts==C->WidgetLayouts,TEXT("layout_IR_roundtrip"),Error);
 UBlueprint* BP=nullptr;const FString Target=TEXT("/Game/DasWidgetLayouts/")+Id+TEXT("/BP_Layout");Check(C && FDasDSLService::Apply(Good,Target,C->Id,BP,Error),TEXT("Apply_checks_known_WBP_before_import"),Error);
 auto Bad=[&](const TCHAR* Suffix,const TCHAR* Field,const TCHAR* Type,const TCHAR* Message){const auto File=Script(Suffix,Field,Type);FDasAuthoringModule M;FString E;bool Compiled=Runtime.ReadAuthoringScript(Runtime.ScriptPath(File),M,E);Check(Compiled,TEXT("bad_layout_is_well_typed_DAS"),E);UBlueprint* Out=nullptr;const FString Path=Target+TEXT("_")+Suffix;const bool Applied=Compiled && FDasDSLService::Apply(File,Path,M.Classes[0].Id,Out,E);Check(!Applied && !Out && E.Contains(Message) && E.Contains(TEXT("WBP contains")),TEXT("bad_layout_rejected_at_Apply_with_field_type_diagnostic"),E);Check(!FindObject<UBlueprint>(nullptr,*(Path+TEXT(".")+FPackageName::GetLongPackageAssetName(Path))),TEXT("rejected_layout_does_not_create_target_BP"));};
 Bad(TEXT("missing"),TEXT("Scor"),TEXT("TextBlockRef"),TEXT("Scor"));Bad(TEXT("wrongtype"),TEXT("Score"),TEXT("ButtonRef"),TEXT("Score"));
 const auto Invalid=Script(TEXT("invalid_ref"),TEXT("Score"),TEXT("int"));Check(!Runtime.ReadAuthoringScript(Runtime.ScriptPath(Invalid),Roundtrip,Error) && Error.Contains(TEXT("typed widget reference")),TEXT("non_widget_layout_field_rejected_at_compile_time"),Error);
 const auto WrongCallback=Script(TEXT("badcallback"),TEXT("Score"),TEXT("TextBlockRef"));FString Text;FFileHelper::LoadFileToString(Text,*Runtime.ScriptPath(WrongCallback));Text=Text.Replace(TEXT("self.Clicked)"),TEXT("self.DoesNotExist)"));FFileHelper::SaveStringToFile(Text,*Runtime.ScriptPath(WrongCallback));Check(!Runtime.ReadAuthoringScript(Runtime.ScriptPath(WrongCallback),Roundtrip,Error),TEXT("unknown_typed_button_callback_rejected"),Error);
 FDasWidgetLayoutUse L;L.Name=TEXT("HUDView");L.Fields.Add(TEXT("Score"),TEXT("/Script/UMG.TextBlock"));L.Fields.Add(TEXT("Damage"),TEXT("/Script/UMG.Button"));Check(ValidateDasWidgetLayout(Tree,L,Error),TEXT("native_tree_type_check"),Error);L.Fields[TEXT("Score")]=TEXT("/Script/UMG.Button");Check(!ValidateDasWidgetLayout(Tree,L,Error) && Error.Contains(TEXT("TextBlock")) && Error.Contains(TEXT("Button")),TEXT("native_tree_reports_expected_and_actual_type"),Error);
 const bool OwnSlate=!FSlateApplication::IsInitialized();if(OwnSlate){FSlateApplication::Create(MakeShareable(FNullApplication::CreateNullApplication()));FSlateApplication::Get().InitializeRenderer(FModuleManager::LoadModuleChecked<ISlateNullRendererModule>(TEXT("SlateNullRenderer")).CreateSlateNullRenderer());}ON_SCOPE_EXIT{if(OwnSlate)FSlateApplication::Shutdown();};
 TStrongObjectPtr<UWorld> World(UWorld::CreateWorld(EWorldType::Game,false));GEngine->CreateNewWorldContext(EWorldType::Game).SetCurrentWorld(World.Get());ON_SCOPE_EXIT{GEngine->DestroyWorldContext(World.Get());World->DestroyWorld(false);};TStrongObjectPtr<UGameInstance> GameInstance(NewObject<UGameInstance>(GEngine));World->SetGameInstance(GameInstance.Get());
 auto* PC=World->SpawnActor<ADasWidgetLayoutController>();TStrongObjectPtr<ULocalPlayer> Player(NewObject<ULocalPlayer>(GEngine));PC->SetPlayer(Player.Get());
 FString Probe=TEXT("options gen2\noptions gc=true\nrequire ue_gameplay_services\n[ue_widget_layout]\nstruct RuntimeHUD { Score:TextBlockRef;Damage:ButtonRef }\n[ue_widget_layout]\nstruct WrongHUD { Score:ButtonRef }\nvar hud:UserWidgetRef\nvar view:RuntimeHUD\nvar reads=0\ndef once():UserWidgetRef { reads+=1;return hud }\n[export]\ndef main(input:int):int {\n let player=ue_ref(type<PlayerControllerRef>,ue_self_object())\n hud=ue_widget_create(ue_class_loaded(type<UserWidgetClass>,\"_WIDGET_CLASS_\"),player)\n let call=ue_begin_call(ue_self_object(),\"ConstructHUD\");ue_arg_object(call,\"Widget\",hud.handle);ue_invoke_call(call);ue_release_call(call)\n view=ue_widget_view(type<RuntimeHUD>,once())\n ue_widget_set_text(view.Score,ue_text_invariant(\"Typed score\"))\n ue_widget_bind_click(view.Damage,\"Clicked\")\n ue_struct_collect()\n return reads\n}\n[export]\ndef bad(input:int):int {let wrong=ue_widget_view(type<WrongHUD>,hud);return 0}\n[export]\ndef remove(input:int):int {ue_widget_remove(hud);return 1}\n[export]\ndef stale(input:int):int {ue_widget_set_text(view.Score,ue_text_invariant(\"stale\"));return 0}\n");Probe=Probe.Replace(TEXT("_WIDGET_CLASS_"),*WBP->GeneratedClass->GetPathName());const FString ProbeName=TEXT("widget_view_runtime_")+Id+TEXT(".das");const FString ProbePath=Runtime.ScriptPath(ProbeName);FFileHelper::SaveStringToFile(Probe,*ProbePath);Scratch.Add(ProbePath);
 auto R=Runtime.RunNamedPersistent(ProbePath,TEXT("main"),0,PC);Check(R.bSucceeded && R.Value==1,TEXT("runtime_view_evaluates_receiver_once"),R.Error);auto* HUD=PC->HUD.Get();auto* Score=HUD?Cast<UTextBlock>(HUD->GetWidgetFromName(TEXT("Score"))):nullptr;auto* Button=HUD?Cast<UButton>(HUD->GetWidgetFromName(TEXT("Damage"))):nullptr;Check(Score && Score->GetText().ToString()==TEXT("Typed score"),TEXT("typed_text_field_and_GC"));Check(Button && Button->OnClicked.IsBound(),TEXT("typed_button_uses_existing_managed_subscription"));
 auto BadRuntime=Runtime.RunNamedPersistent(ProbePath,TEXT("bad"),0,PC);Check(!BadRuntime.bSucceeded && BadRuntime.Error.Contains(TEXT("Score")) && BadRuntime.Error.Contains(TEXT("TextBlock")),TEXT("runtime_layout_checks_actual_WBP_type"),BadRuntime.Error);
 TWeakObjectPtr<UUserWidget> WeakHUD=HUD;TWeakObjectPtr<UButton> WeakButton=Button;auto Removed=Runtime.RunNamedPersistent(ProbePath,TEXT("remove"),0,PC);Check(Removed.bSucceeded && Button && !Button->OnClicked.IsBound(),TEXT("remove_cleans_typed_button_binding"));PC->Slate.Reset();PC->HUD.Reset();HUD=nullptr;Score=nullptr;Button=nullptr;CollectGarbage(RF_NoFlags);Check(!WeakHUD.IsValid() && !WeakButton.IsValid(),TEXT("view_and_cache_do_not_root_widgets"));auto Stale=Runtime.RunNamedPersistent(ProbePath,TEXT("stale"),0,PC);Check(!Stale.bSucceeded,TEXT("expired_view_field_rejected"),Stale.Error);Runtime.ReleaseObject(PC);
 UE_LOG(LogTemp,Display,TEXT("DAS_WIDGET_LAYOUT_RESULT=%s checks=%d"),Failures?TEXT("FAIL"):TEXT("PASS"),Checks);return Failures?1:0;
}
