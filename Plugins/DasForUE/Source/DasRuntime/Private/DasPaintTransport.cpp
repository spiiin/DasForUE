#include "DasPaintTransport.h"
#include "Blueprint/UserWidget.h"
bool DasIsPaintContextType(const UScriptStruct* Type){return Type==FPaintContext::StaticStruct();}
bool DasPaintFunctionAllowed(const UFunction* F){
 if(!F || !F->HasAllFunctionFlags(FUNC_Native|FUNC_Static) || F->GetOuterUClass()->GetPathName()!=TEXT("/Script/UMG.WidgetBlueprintLibrary"))return false;
 static const TCHAR* Allowed[]={TEXT("DrawBox"),TEXT("DrawLine"),TEXT("DrawLines"),TEXT("DrawSpline"),TEXT("DrawTextFormatted")};
 for(const TCHAR* Name:Allowed)if(F->GetName()==Name)return true;return false;
}
