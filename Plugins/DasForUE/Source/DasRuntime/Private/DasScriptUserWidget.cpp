#include "DasScriptUserWidget.h"
#include "DasRuntime.h"
#include "Input/Events.h"
#include "Input/Reply.h"
#include "Engine/World.h"
#include "Misc/Paths.h"
#include "Modules/ModuleManager.h"

bool UDasScriptUserWidget::IsSupportedInputEvent(FName Event)
{
 static const TSet<FName> Names={TEXT("OnKeyChar"),TEXT("OnPreviewKeyDown"),TEXT("OnKeyDown"),TEXT("OnKeyUp"),TEXT("OnAnalogValueChanged"),TEXT("OnMouseButtonDown"),TEXT("OnPreviewMouseButtonDown"),TEXT("OnMouseButtonUp"),TEXT("OnMouseMove"),TEXT("OnMouseWheel"),TEXT("OnMouseButtonDoubleClick"),TEXT("OnTouchGesture"),TEXT("OnTouchStarted"),TEXT("OnTouchMoved"),TEXT("OnTouchEnded"),TEXT("OnTouchForceChanged"),TEXT("OnTouchFirstMove")};
 return Names.Contains(Event);
}
bool UDasScriptUserWidget::BindInputHandler(FName Event,FName Handler)
{
 if(!IsInGameThread() || !IsSupportedInputEvent(Event))return false;
 if(Handler.IsNone())InputHandlers.Remove(Event);else InputHandlers.Add(Event,Handler);
 return true;
}
bool UDasScriptUserWidget::DispatchInput(FName EventName,const UScriptStruct* Type,const void* Source,FReply& Reply)
{
 auto* Runtime=FModuleManager::GetModulePtr<FDasRuntimeModule>(TEXT("DasRuntime"));
 if(IsInGameThread() && Runtime && ActiveScript!=ScriptFile){Runtime->ReleaseObject(this);ActiveScript=ScriptFile;}
 const auto* Handler=InputHandlers.Find(EventName);
 if(!Handler || Handler->IsNone() || ScriptFile.IsEmpty())return false;
 auto* World=GetWorld();
 if(!IsInGameThread() || !Runtime || IsTemplate() || !World || !World->IsGameWorld() || World->bIsTearingDown || ScriptFile!=FPaths::GetCleanFilename(ScriptFile) || FPaths::GetExtension(ScriptFile)!=TEXT("das")){
  LastInputError=TEXT("Widget input requires a live game-world widget and a script filename");return false;
 }
 return Runtime->DispatchWidgetInput(Runtime->ScriptPath(ScriptFile),Handler->ToString(),Type,Source,this,Reply,LastInputError);
}
void UDasScriptUserWidget::NativeDestruct()
{
 if(auto* Runtime=FModuleManager::GetModulePtr<FDasRuntimeModule>(TEXT("DasRuntime")))Runtime->ReleaseObject(this);
 ActiveScript.Empty();
 Super::NativeDestruct();
}
#define DAS_WIDGET_INPUT(Name,Type) \
 FReply UDasScriptUserWidget::Native##Name(const FGeometry& Geometry,const Type& Event){ \
  FReply Reply=FReply::Unhandled();if(DispatchInput(TEXT(#Name),Type::StaticStruct(),&Event,Reply))return Reply; \
  return Super::Native##Name(Geometry,Event); \
 }
DAS_WIDGET_INPUT(OnKeyChar,FCharacterEvent)
DAS_WIDGET_INPUT(OnPreviewKeyDown,FKeyEvent)
DAS_WIDGET_INPUT(OnKeyDown,FKeyEvent)
DAS_WIDGET_INPUT(OnKeyUp,FKeyEvent)
DAS_WIDGET_INPUT(OnAnalogValueChanged,FAnalogInputEvent)
DAS_WIDGET_INPUT(OnMouseButtonDown,FPointerEvent)
DAS_WIDGET_INPUT(OnPreviewMouseButtonDown,FPointerEvent)
DAS_WIDGET_INPUT(OnMouseButtonUp,FPointerEvent)
DAS_WIDGET_INPUT(OnMouseMove,FPointerEvent)
DAS_WIDGET_INPUT(OnMouseWheel,FPointerEvent)
DAS_WIDGET_INPUT(OnMouseButtonDoubleClick,FPointerEvent)
DAS_WIDGET_INPUT(OnTouchGesture,FPointerEvent)
DAS_WIDGET_INPUT(OnTouchStarted,FPointerEvent)
DAS_WIDGET_INPUT(OnTouchMoved,FPointerEvent)
DAS_WIDGET_INPUT(OnTouchEnded,FPointerEvent)
DAS_WIDGET_INPUT(OnTouchForceChanged,FPointerEvent)
DAS_WIDGET_INPUT(OnTouchFirstMove,FPointerEvent)
#undef DAS_WIDGET_INPUT
