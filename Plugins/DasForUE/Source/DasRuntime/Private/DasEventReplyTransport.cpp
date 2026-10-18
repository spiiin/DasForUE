#include "DasEventReplyTransport.h"
#include "Components/SlateWrapperTypes.h"
#include "UObject/Class.h"
bool DasIsEventReplyType(const UScriptStruct* Type){return Type==FEventReply::StaticStruct();}
bool DasEventReplyFunctionAllowed(const UFunction* F){
 if(!F || !F->HasAllFunctionFlags(FUNC_Native|FUNC_Static) || F->GetOuterUClass()->GetPathName()!=TEXT("/Script/UMG.WidgetBlueprintLibrary"))return false;
 static const TCHAR* Allowed[]={TEXT("Handled"),TEXT("Unhandled"),TEXT("CaptureMouse"),TEXT("ReleaseMouseCapture"),TEXT("LockMouse"),TEXT("UnlockMouse"),TEXT("SetUserFocus"),TEXT("ClearUserFocus"),TEXT("SetMousePosition"),TEXT("DetectDrag"),TEXT("EndDragDrop"),TEXT("DetectDragIfPressed")};
 for(const TCHAR* Name:Allowed)if(F->GetName()==Name)return true;return false;
}
