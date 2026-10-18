#include "DasScriptUserWidget.h"
#include "DasRuntime.h"
#include "Input/Events.h"
#include "Input/Reply.h"
#include "Engine/World.h"
#include "Misc/Paths.h"
#include "Modules/ModuleManager.h"

bool UDasScriptUserWidget::IsSupportedInputEvent(FName Event)
{
 static const TSet<FName> Names={TEXT("OnFocusReceived"),TEXT("OnFocusLost"),TEXT("OnKeyChar"),TEXT("OnPreviewKeyDown"),TEXT("OnKeyDown"),TEXT("OnKeyUp"),TEXT("OnAnalogValueChanged"),TEXT("OnMouseButtonDown"),TEXT("OnPreviewMouseButtonDown"),TEXT("OnMouseButtonUp"),TEXT("OnMouseMove"),TEXT("OnMouseWheel"),TEXT("OnMouseButtonDoubleClick"),TEXT("OnTouchGesture"),TEXT("OnTouchStarted"),TEXT("OnTouchMoved"),TEXT("OnTouchEnded"),TEXT("OnTouchForceChanged"),TEXT("OnTouchFirstMove")};
 return Names.Contains(Event);
}
bool UDasScriptUserWidget::BindInputHandler(FName Event,FName Handler)
{
 if(!IsInGameThread() || !IsSupportedInputEvent(Event))return false;
 if(Handler.IsNone())InputHandlers.Remove(Event);else InputHandlers.Add(Event,Handler);
 return true;
}
bool UDasScriptUserWidget::DispatchInput(FName EventName,const FGeometry& Geometry,const UScriptStruct* Type,const void* Source,FReply& Reply)
{
 auto* Runtime=FModuleManager::GetModulePtr<FDasRuntimeModule>(TEXT("DasRuntime"));
 if(IsInGameThread() && Runtime && ActiveScript!=ScriptFile){Runtime->ReleaseObject(this);ActiveScript=ScriptFile;}
 const auto* Handler=InputHandlers.Find(EventName);
 if(!Handler || Handler->IsNone() || ScriptFile.IsEmpty())return false;
 auto* World=GetWorld();
 if(!IsInGameThread() || !Runtime || IsTemplate() || !World || !World->IsGameWorld() || World->bIsTearingDown || ScriptFile!=FPaths::GetCleanFilename(ScriptFile) || FPaths::GetExtension(ScriptFile)!=TEXT("das")){
  LastInputError=TEXT("Widget input requires a live game-world widget and a script filename");return false;
 }
 return Runtime->DispatchWidgetInput(Runtime->ScriptPath(ScriptFile),Handler->ToString(),Geometry,Type,Source,this,Reply,LastInputError);
}
int32 UDasScriptUserWidget::NativePaint(const FPaintArgs& Args,const FGeometry& Geometry,const FSlateRect& Culling,FSlateWindowElementList& Elements,int32 Layer,const FWidgetStyle& Style,bool ParentEnabled)const{
 const int32 Base=Super::NativePaint(Args,Geometry,Culling,Elements,Layer,Style,ParentEnabled);
 auto* Self=const_cast<UDasScriptUserWidget*>(this);if(PaintHandler.IsNone() || ScriptFile.IsEmpty())return Base;
 auto* Runtime=FModuleManager::GetModulePtr<FDasRuntimeModule>(TEXT("DasRuntime"));
 if(!IsInGameThread() || !Runtime || ScriptFile!=FPaths::GetCleanFilename(ScriptFile) || FPaths::GetExtension(ScriptFile)!=TEXT("das")){Self->LastPaintError=TEXT("OnPaint requires a game-thread runtime and script filename");return Base;}
 if(Self->ActiveScript!=ScriptFile){Runtime->ReleaseObject(Self);Self->ActiveScript=ScriptFile;}
 FPaintContext Context(Geometry,Culling,Elements,Base,Style,ParentEnabled);
 return Runtime->DispatchWidgetPaint(Runtime->ScriptPath(ScriptFile),PaintHandler.ToString(),Self,Context,Self->LastPaintError)?FMath::Max(Base,Context.MaxLayer):Base;
}
void UDasScriptUserWidget::NativeDestruct()
{
 if(auto* Runtime=FModuleManager::GetModulePtr<FDasRuntimeModule>(TEXT("DasRuntime")))Runtime->ReleaseObject(this);
 ActiveScript.Empty();
 Super::NativeDestruct();
}
#define DAS_WIDGET_INPUT(Name,Type) \
 FReply UDasScriptUserWidget::Native##Name(const FGeometry& Geometry,const Type& Event){ \
  FReply Reply=FReply::Unhandled();if(DispatchInput(TEXT(#Name),Geometry,Type::StaticStruct(),&Event,Reply))return Reply; \
  return Super::Native##Name(Geometry,Event); \
 }
DAS_WIDGET_INPUT(OnFocusReceived,FFocusEvent)
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

void UDasScriptUserWidget::NativeOnFocusLost(const FFocusEvent& Event){
 auto* Runtime=FModuleManager::GetModulePtr<FDasRuntimeModule>(TEXT("DasRuntime"));
 if(IsInGameThread() && Runtime && ActiveScript!=ScriptFile){Runtime->ReleaseObject(this);ActiveScript=ScriptFile;}
 const auto* Handler=InputHandlers.Find(TEXT("OnFocusLost"));
 if(!Handler || Handler->IsNone() || ScriptFile.IsEmpty()){Super::NativeOnFocusLost(Event);return;}
 if(!IsInGameThread() || !Runtime || ScriptFile!=FPaths::GetCleanFilename(ScriptFile) || FPaths::GetExtension(ScriptFile)!=TEXT("das")){LastInputError=TEXT("Focus notification requires a game-thread runtime and script filename");Super::NativeOnFocusLost(Event);return;}
 if(!Runtime->DispatchWidgetFocusLost(Runtime->ScriptPath(ScriptFile),Handler->ToString(),Event,this,LastInputError))Super::NativeOnFocusLost(Event);
}


bool UDasScriptUserWidget::IsSupportedDragEvent(FName Event){
 static const TSet<FName> Names={TEXT("OnDragDetected"),TEXT("OnDragEnter"),TEXT("OnDragLeave"),TEXT("OnDragOver"),TEXT("OnDrop"),TEXT("OnDragCancelled")};return Names.Contains(Event);
}
bool UDasScriptUserWidget::BindDragHandler(FName Event,FName Handler){
 if(!IsInGameThread() || !IsSupportedDragEvent(Event))return false;if(Handler.IsNone())DragHandlers.Remove(Event);else DragHandlers.Add(Event,Handler);return true;
}
bool UDasScriptUserWidget::DispatchDrag(FName EventName,const FGeometry* Geometry,const FPointerEvent& Event,UDragDropOperation* Operation,UDragDropOperation*& OutOperation,bool& Handled){
 auto* Runtime=FModuleManager::GetModulePtr<FDasRuntimeModule>(TEXT("DasRuntime"));
 if(IsInGameThread() && Runtime && ActiveScript!=ScriptFile){Runtime->ReleaseObject(this);ActiveScript=ScriptFile;}
 const auto* Handler=DragHandlers.Find(EventName);if(!Handler || Handler->IsNone() || ScriptFile.IsEmpty())return false;
 if(!IsInGameThread() || !Runtime || ScriptFile!=FPaths::GetCleanFilename(ScriptFile) || FPaths::GetExtension(ScriptFile)!=TEXT("das")){LastDragError=TEXT("drag requires a game-thread runtime and script filename");return false;}
 return Runtime->DispatchWidgetDrag(Runtime->ScriptPath(ScriptFile),Handler->ToString(),EventName,Geometry,Event,Operation,this,OutOperation,Handled,LastDragError);
}
void UDasScriptUserWidget::NativeOnDragDetected(const FGeometry& G,const FPointerEvent& E,UDragDropOperation*& O){
 UDragDropOperation* Out=nullptr;bool Handled=false;if(DispatchDrag(TEXT("OnDragDetected"),&G,E,nullptr,Out,Handled))O=Out;else Super::NativeOnDragDetected(G,E,O);
}
void UDasScriptUserWidget::NativeOnDragEnter(const FGeometry& G,const FDragDropEvent& E,UDragDropOperation* O){UDragDropOperation* Out=nullptr;bool H=false;if(!DispatchDrag(TEXT("OnDragEnter"),&G,E,O,Out,H))Super::NativeOnDragEnter(G,E,O);}
void UDasScriptUserWidget::NativeOnDragLeave(const FDragDropEvent& E,UDragDropOperation* O){UDragDropOperation* Out=nullptr;bool H=false;if(!DispatchDrag(TEXT("OnDragLeave"),nullptr,E,O,Out,H))Super::NativeOnDragLeave(E,O);}
bool UDasScriptUserWidget::NativeOnDragOver(const FGeometry& G,const FDragDropEvent& E,UDragDropOperation* O){UDragDropOperation* Out=nullptr;bool H=false;return DispatchDrag(TEXT("OnDragOver"),&G,E,O,Out,H)?H:Super::NativeOnDragOver(G,E,O);}
bool UDasScriptUserWidget::NativeOnDrop(const FGeometry& G,const FDragDropEvent& E,UDragDropOperation* O){UDragDropOperation* Out=nullptr;bool H=false;return DispatchDrag(TEXT("OnDrop"),&G,E,O,Out,H)?H:Super::NativeOnDrop(G,E,O);}
void UDasScriptUserWidget::NativeOnDragCancelled(const FDragDropEvent& E,UDragDropOperation* O){UDragDropOperation* Out=nullptr;bool H=false;if(!DispatchDrag(TEXT("OnDragCancelled"),nullptr,E,O,Out,H))Super::NativeOnDragCancelled(E,O);}
