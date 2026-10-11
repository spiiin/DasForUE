#pragma once
#include "DasAuthoring.h"
#include "UObject/UnrealType.h"
#include "UObject/StrProperty.h"

// Validate without writing first. Only value types with no object loads or callbacks
// are accepted; defaults are applied to unregistered SCS templates, never instances.
static bool DasComponentDefaults(UClass* Class,const FDasDeclaredComponent& C,UObject* Template,FString& Error){
 for(const auto& Pair:C.Defaults){
  auto* P=Class->FindPropertyByName(*Pair.Key);const auto& V=Pair.Value;
  auto Fail=[&](){Error=TEXT("Unsupported component default/type or non-editable template property: ")+C.Name+TEXT(".")+Pair.Key;return false;};
  if(!P || P->ArrayDim!=1 || !P->HasAnyPropertyFlags(CPF_Edit) || P->HasAnyPropertyFlags(CPF_EditConst|CPF_DisableEditOnTemplate))return Fail();
  void* Data=Template?P->ContainerPtrToValuePtr<void>(Template):nullptr;
  if(auto* Bool=CastField<FBoolProperty>(P)){if(V.Kind!=TEXT("bool"))return Fail();if(Data)Bool->SetPropertyValue(Data,V.Boolean);}
  else if(auto* Int=CastField<FIntProperty>(P)){if(V.Kind!=TEXT("number") || !FMath::IsFinite(V.Number) || V.Number<MIN_int32 || V.Number>MAX_int32 || V.Number!=FMath::TruncToDouble(V.Number))return Fail();if(Data)Int->SetPropertyValue(Data,int32(V.Number));}
  else if(auto* Float=CastField<FFloatProperty>(P)){if(V.Kind!=TEXT("number") || !FMath::IsFinite(V.Number) || !FMath::IsFinite(float(V.Number)) || (V.Number!=0 && float(V.Number)==0))return Fail();if(Data)Float->SetPropertyValue(Data,float(V.Number));}
  else if(auto* Double=CastField<FDoubleProperty>(P)){if(V.Kind!=TEXT("number") || !FMath::IsFinite(V.Number))return Fail();if(Data)Double->SetPropertyValue(Data,V.Number);}
  else if(auto* String=CastField<FStrProperty>(P)){if(V.Kind!=TEXT("string"))return Fail();if(Data)String->SetPropertyValue(Data,V.String);}
  else if(auto* Name=CastField<FNameProperty>(P)){if(V.Kind!=TEXT("string") || V.String.Len()>=NAME_SIZE)return Fail();if(Data)Name->SetPropertyValue(Data,FName(*V.String));}
  else if(auto* Struct=CastField<FStructProperty>(P)){
   FVector Value;if(!Value.InitFromString(V.String) || Value.ContainsNaN())return Fail();
   if(Struct->Struct==TBaseStructure<FVector>::Get() && V.Kind==TEXT("vector")){if(Data)*static_cast<FVector*>(Data)=Value;}
   else if(Struct->Struct==TBaseStructure<FRotator>::Get() && V.Kind==TEXT("rotator")){if(Data)*static_cast<FRotator*>(Data)=FRotator(Value.X,Value.Y,Value.Z);}
   else return Fail();
  }else return Fail();
 }return true;
}
