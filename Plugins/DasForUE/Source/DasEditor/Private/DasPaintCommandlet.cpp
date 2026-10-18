#include "DasPaintCommandlet.h"
#include "DasRuntime.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "Engine/Font.h"
#include "Slate/SlateBrushAsset.h"
#include "Framework/Application/SlateApplication.h"
#include "Interfaces/ISlateNullRendererModule.h"
#include "Null/NullApplication.h"
#include "Rendering/DrawElements.h"
#include "Types/PaintArgs.h"
#include "Input/HittestGrid.h"
#include "Widgets/SWindow.h"
#include "UObject/StrongObjectPtr.h"
#include "UObject/GarbageCollection.h"
#include "Misc/FileHelper.h"
#include "Misc/ScopeExit.h"
void UDasPaintFixture::CollectPaintGarbageForTest(){CollectGarbage(RF_NoFlags);}
bool UDasPaintFixture::NestedPaintForTest(){
 auto& Runtime=FModuleManager::GetModuleChecked<FDasRuntimeModule>(TEXT("DasRuntime"));
 const FGeometry G;const FSlateRect R(0,0,10,10);FSlateWindowElementList E(nullptr);const FWidgetStyle S;FPaintContext P(G,R,E,0,S,true);FString Error;
 return !Runtime.DispatchWidgetPaint(Runtime.ScriptPath(ScriptFile),TEXT("warmup"),this,P,Error) && Error.Contains(TEXT("idle"));
}
UDasPaintCommandlet::UDasPaintCommandlet(){IsClient=false;IsServer=false;IsEditor=true;LogToConsole=true;}
int32 UDasPaintCommandlet::Main(const FString& Params){
 FString Script;if(!FParse::Value(*Params,TEXT("Script="),Script))return 2;
 const bool OwnSlate=!FSlateApplication::IsInitialized();if(OwnSlate){FSlateApplication::Create(MakeShareable(FNullApplication::CreateNullApplication()));FSlateApplication::Get().InitializeRenderer(FModuleManager::LoadModuleChecked<ISlateNullRendererModule>(TEXT("SlateNullRenderer")).CreateSlateNullRenderer());}ON_SCOPE_EXIT{if(OwnSlate)FSlateApplication::Shutdown();};
 auto& Runtime=FModuleManager::LoadModuleChecked<FDasRuntimeModule>(TEXT("DasRuntime"));int32 Checks=0,Failures=0;auto Check=[&](bool Good,const TCHAR* Name){++Checks;if(!Good)++Failures;UE_LOG(LogTemp,Display,TEXT("DAS_PAINT_%s %s"),Good?TEXT("PASS"):TEXT("FAIL"),Name);};
 TStrongObjectPtr<UWorld> World(UWorld::CreateWorld(EWorldType::Game,false));GEngine->CreateNewWorldContext(EWorldType::Game).SetCurrentWorld(World.Get());ON_SCOPE_EXIT{GEngine->DestroyWorldContext(World.Get());World->DestroyWorld(false);};
 TStrongObjectPtr<UDasPaintFixture> Widget(NewObject<UDasPaintFixture>(World.Get()));Widget->Initialize();Widget->ScriptFile=Script;TSharedPtr<SWidget> Slate=Widget->TakeWidget();
 auto Window=SNew(SWindow).ClientSize(FVector2D(200,200));const auto Geometry=FGeometry::MakeRoot(FVector2D(100,100),FSlateLayoutTransform());const FSlateRect Clip(0,0,100,100);FWidgetStyle Style;FHittestGrid Grid;Grid.SetHittestArea(FVector2D::ZeroVector,FVector2D(200,200));
 auto Paint=[&](FName Handler){Widget->PaintHandler=Handler;FSlateWindowElementList Elements(Window);FPaintArgs Args(nullptr,Grid,FVector2D::ZeroVector,0,1.f/60);const int32 Layer=Slate->Paint(Args,Geometry,Clip,Elements,7,Style,true);int32 Count=0;VisitTupleElements([&](const auto& List){Count+=List.Num();},Elements.GetUncachedDrawElements());if(!Widget->LastPaintError.IsEmpty())UE_LOG(LogTemp,Display,TEXT("Paint issue %s: %s"),*Handler.ToString(),*Widget->LastPaintError);UE_LOG(LogTemp,Display,TEXT("PAINT_ELEMENTS handler=%s layer=%d elements=%d"),*Handler.ToString(),Layer,Count);return TPair<int32,int32>(Layer,Count);};
 auto Base=Paint(NAME_None);const int32 ParentLayer=Base.Key;Check(ParentLayer>=7 && Base.Value==0,TEXT("missing_handler_preserves_parent_paint"));auto Warm=Paint(TEXT("warmup"));Check(Warm.Key==ParentLayer && Warm.Value==0 && Widget->LastPaintError.IsEmpty(),TEXT("synchronous_void_paint_handler"));
 TStrongObjectPtr<USlateBrushAsset> Brush(NewObject<USlateBrushAsset>());Brush->Brush.DrawAs=ESlateBrushDrawType::Box;Brush->Brush.ImageSize=FVector2D(10,10);TWeakObjectPtr<USlateBrushAsset> WeakBrush=Brush.Get();
 auto Setup=[&](){TArray<FDasScalarValue> Args;for(UObject* O:{static_cast<UObject*>(Brush.Get()),static_cast<UObject*>(GEngine->GetSmallFont())}){FDasScalarValue A;A.Kind=8;A.ObjectClass=TEXT("/Script/CoreUObject.Object");A.ObjectValue=O;Args.Add(A);}return Runtime.RunNamedFramePersistent(Runtime.ScriptPath(Script),TEXT("setup"),Args,EDasMethodSignature::Int,Widget.Get());};
 Check(Setup().bSucceeded,TEXT("native_brush_and_font_references"));auto Draw=Paint(TEXT("draw_all"));Check(Draw.Key==ParentLayer+5 && Draw.Value==5 && Widget->LastPaintError.IsEmpty(),TEXT("five_generated_draw_functions_emit_real_slate_elements_and_layers"));
 auto Outside=Runtime.RunNamedPersistent(Runtime.ScriptPath(Script),TEXT("outside"),0,Widget.Get());Check(!Outside.bSucceeded && Outside.Error.Contains(TEXT("active synchronous OnPaint")),TEXT("drawing_outside_paint_rejected"));
 for(const auto& Case:TArray<TPair<FName,FString>>{
  {TEXT("stale"),TEXT("expired")},{TEXT("invalid_default"),TEXT("null")},{TEXT("panic_after_draw"),TEXT("deliberate paint failure")},{TEXT("wrong_signature"),TEXT("Expected exported")},
  {TEXT("bad_thickness"),TEXT("thickness")},{TEXT("bad_coordinate"),TEXT("coordinate")},{TEXT("too_many_points"),TEXT("point limit")},{TEXT("too_many_commands"),TEXT("command or layer limit")},
  {TEXT("stale_frame"),TEXT("call handle is released")},{TEXT("result_before_invoke"),TEXT("results require an executed call")},{TEXT("missing_context"),TEXT("missing input argument")}}){auto P=Paint(Case.Key);Check(P.Key==ParentLayer && P.Value==0 && Widget->LastPaintError.Contains(Case.Value),TEXT("failed_paint_discards_all_staged_commands_for_expected_reason"));}
 auto Recover=Paint(TEXT("draw_all"));Check(Recover.Key==ParentLayer+5 && Recover.Value==5 && Widget->LastPaintError.IsEmpty(),TEXT("paint_recovers_after_failures"));
 auto Nested=Paint(TEXT("nested_paint"));Check(Nested.Key==ParentLayer+1 && Nested.Value==1 && Widget->LastPaintError.IsEmpty(),TEXT("reentrant_bridge_rejected_without_disrupting_outer_paint"));
 auto Zero=Paint(TEXT("empty_lines"));Check(Zero.Key==ParentLayer && Zero.Value==0,TEXT("native_degenerate_lines_noop_preserved"));
 auto Destroy=Paint(TEXT("destruct_after_draw"));Check(Destroy.Key==ParentLayer && Destroy.Value==0 && Widget->LastPaintError.Contains(TEXT("expired")),TEXT("NativeDestruct_cancels_current_draw_batch"));
 Slate.Reset();Widget->ReleaseSlateResources(true);Slate=Widget->TakeWidget();auto Reborn=Paint(TEXT("warmup"));Check(Reborn.Value==0 && Widget->LastPaintError.IsEmpty(),TEXT("paint_context_recreated_after_widget_reconstruction"));Check(Setup().bSucceeded,TEXT("reconstructed_widget_setup"));
 FString Original;const FString Path=Runtime.ScriptPath(Script);FFileHelper::LoadFileToString(Original,*Path);ON_SCOPE_EXIT{FFileHelper::SaveStringToFile(Original,*Path);};FFileHelper::SaveStringToFile(Original+TEXT("\n// Paint reload revision\n"),*Path);
 auto Reload=Paint(TEXT("warmup"));Check(Reload.Key==ParentLayer && Reload.Value==0 && Widget->LastPaintError.IsEmpty(),TEXT("reload_preserves_paint_entry_contract"));Check(Setup().bSucceeded,TEXT("reload_setup"));auto ReloadDraw=Paint(TEXT("draw_all"));Check(ReloadDraw.Key==ParentLayer+5 && ReloadDraw.Value==5,TEXT("generated_draw_wrappers_after_reload"));
 Brush.Reset();auto Pin=Paint(TEXT("draw_then_gc"));Check(Pin.Key==ParentLayer+1 && Pin.Value==1 && Widget->LastPaintError.IsEmpty(),TEXT("queued_resource_is_pinned_through_GC_until_commit"));CollectGarbage(RF_NoFlags);Check(!WeakBrush.IsValid(),TEXT("paint_batch_does_not_retain_brush_after_callback"));
 Slate.Reset();Widget->ReleaseSlateResources(true);Check(Runtime.PersistentContextCount(World.Get())==0,TEXT("NativeDestruct_releases_paint_VM"));
 UE_LOG(LogTemp,Display,TEXT("DAS_PAINT_RESULT=%s checks=%d"),Failures?TEXT("FAIL"):TEXT("PASS"),Checks);return Failures?1:0;
}
