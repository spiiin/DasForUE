#pragma once
#include "DasAuthoring.h"
#include "StructUtils/InstancedStruct.h"
#include "UObject/UnrealType.h"
#include "UObject/StrProperty.h"

// Shared parser for both initial import and staged default merging.
static bool DasComponentDefaultValue(FProperty* P,const FDasDeclaredLiteral& V,void* Data,FString& Error){
  auto Fail=[&](){Error=TEXT("Unsupported component default/type or non-editable template property: ")+(P?P->GetName():TEXT("<missing>"));return false;};
  if(!P || P->ArrayDim!=1 || !P->HasAnyPropertyFlags(CPF_Edit) || P->HasAnyPropertyFlags(CPF_EditConst|CPF_DisableEditOnTemplate))return Fail();
  if(auto* Bool=CastField<FBoolProperty>(P)){if(V.Kind!=TEXT("bool"))return Fail();if(Data)Bool->SetPropertyValue(Data,V.Boolean);}
  else if(auto* Int=CastField<FIntProperty>(P)){if(V.Kind!=TEXT("number") || !FMath::IsFinite(V.Number) || V.Number<MIN_int32 || V.Number>MAX_int32 || V.Number!=FMath::TruncToDouble(V.Number))return Fail();if(Data)Int->SetPropertyValue(Data,int32(V.Number));}
  else if(auto* Float=CastField<FFloatProperty>(P)){if(V.Kind!=TEXT("number") || !FMath::IsFinite(V.Number) || !FMath::IsFinite(float(V.Number)) || (V.Number!=0 && float(V.Number)==0))return Fail();if(Data)Float->SetPropertyValue(Data,float(V.Number));}
  else if(auto* Double=CastField<FDoubleProperty>(P)){if(V.Kind!=TEXT("number") || !FMath::IsFinite(V.Number))return Fail();if(Data)Double->SetPropertyValue(Data,V.Number);}
  else if(auto* String=CastField<FStrProperty>(P)){if(V.Kind!=TEXT("string"))return Fail();if(Data)String->SetPropertyValue(Data,V.String);}
  else if(auto* Name=CastField<FNameProperty>(P)){if(V.Kind!=TEXT("string") || V.String.Len()>=NAME_SIZE)return Fail();if(Data)Name->SetPropertyValue(Data,FName(*V.String));}
  else if(auto* ClassProperty=CastField<FClassProperty>(P)){
   UObject* Value=nullptr;if(V.Kind!=TEXT("class") || !ResolveDasReferenceDefault(true,ClassProperty->MetaClass->GetPathName(),V.String,Value,Error))return Fail();if(Data)ClassProperty->SetObjectPropertyValue(Data,Value);
  }else if(auto* Reference=CastField<FObjectProperty>(P)){
   UObject* Value=nullptr;if(V.Kind!=TEXT("asset") || !ResolveDasReferenceDefault(false,Reference->PropertyClass->GetPathName(),V.String,Value,Error))return Fail();if(Data)Reference->SetObjectPropertyValue(Data,Value);
  }else if(auto* Struct=CastField<FStructProperty>(P)){
   const FString Path=Struct->Struct->GetPathName();const FString Expected=Path==TEXT("/Script/CoreUObject.Vector")?TEXT("vector"):Path==TEXT("/Script/CoreUObject.Rotator")?TEXT("rotator"):Path==TEXT("/Script/CoreUObject.Vector2D")?TEXT("vector2d"):Path==TEXT("/Script/CoreUObject.Transform")?TEXT("transform"):FString();
   if(V.Kind!=TEXT("struct") && (Expected.IsEmpty() || V.Kind!=Expected))return Fail();
   FInstancedStruct Value;FString Text=V.String;FVector Legacy;
   if((V.Kind==TEXT("vector") || V.Kind==TEXT("rotator")) && Legacy.InitFromString(Text)){
    FRotator Rotation(Legacy.X,Legacy.Y,Legacy.Z);Text.Reset();Struct->Struct->ExportText(Text,V.Kind==TEXT("vector")?static_cast<const void*>(&Legacy):static_cast<const void*>(&Rotation),nullptr,nullptr,PPF_None,nullptr);
   }
   if(!ParseDasStructDefault(Path,Text,Value,Error))return Fail();if(Data)Struct->Struct->CopyScriptStruct(Data,Value.GetMemory());
  }else return Fail();
 return true;
}
static bool DasComponentDefaults(UClass* Class,const FDasDeclaredComponent& C,UObject* Template,FString& Error){
 for(const auto& Pair:C.Defaults)if(!DasComponentDefaultValue(Class->FindPropertyByName(*Pair.Key),Pair.Value,nullptr,Error))return false;
 if(Template)for(const auto& Pair:C.Defaults){
  auto* P=Class->FindPropertyByName(*Pair.Key);Template->PreEditChange(P);
  if(!DasComponentDefaultValue(P,Pair.Value,P->ContainerPtrToValuePtr<void>(Template),Error))return false;
  FPropertyChangedEvent Changed(P,EPropertyChangeType::ValueSet);Template->PostEditChangeProperty(Changed);
 }
 return true;
}
