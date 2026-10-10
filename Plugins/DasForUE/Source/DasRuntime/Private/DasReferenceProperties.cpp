#include "DasSchema.h"
#include "UObject/UnrealType.h"
#include "UObject/ScriptInterface.h"
UClass* DasReferencePropertyClass(const FProperty* P){if(auto* I=CastField<FInterfaceProperty>(P))return I->InterfaceClass.Get();if(auto* O=CastField<FObjectProperty>(P);O && !CastField<FClassProperty>(P))return O->PropertyClass.Get();return nullptr;}
bool DasReferenceMatches(const UObject* O,const UClass* C){return C && (!O || (C->HasAnyClassFlags(CLASS_Interface)?O->GetClass()->ImplementsInterface(C):O->IsA(C)));}
UObject* DasReferenceGet(const FProperty* P,const void* M){if(!P || !M)return nullptr;if(auto* I=CastField<FInterfaceProperty>(P))return I->GetPropertyValue(M).GetObject();if(auto* O=CastField<FObjectProperty>(P);O && !CastField<FClassProperty>(P))return O->GetObjectPropertyValue(M);return nullptr;}
bool DasReferenceSet(const FProperty* P,void* M,UObject* O){auto* C=DasReferencePropertyClass(P);if(!M || !DasReferenceMatches(O,C))return false;if(auto* I=CastField<FInterfaceProperty>(P)){FScriptInterface Value;Value.SetObject(O);Value.SetInterface(O?O->GetInterfaceAddress(C):nullptr);I->SetPropertyValue(M,Value);}else CastFieldChecked<FObjectProperty>(P)->SetObjectPropertyValue(M,O);return true;}
