#pragma once
#include "CoreMinimal.h"
#include "DasSchema.h"
#include "DasNativeHashProbeComponent.h"
#include "DasAuthoring.h"
#include "DasContainerTypes.h"
#include "UObject/UnrealType.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "Camera/CameraComponent.h"
#include "Components/TextRenderComponent.h"
inline TArray<UClass*> DasNativeGameplayClasses(){return {ACharacter::StaticClass(),UCharacterMovementComponent::StaticClass(),APlayerController::StaticClass(),UCameraComponent::StaticClass(),UTextRenderComponent::StaticClass(),UDasNativeHashProbeComponent::StaticClass()};}
inline FString DasNativeGameplayKind(FProperty* P,FString& Path){
 Path.Reset();if(!P || P->ArrayDim!=1)return {};
 if(auto* E=DasEnumPropertyType(P)){TSet<int64> Values;for(int32 I=0;I<E->NumEnums();++I){const auto N=E->GetNameStringByIndex(I);if(N.EndsWith(TEXT("_MAX")))continue;const int64 V=E->GetValueByIndex(I);if(V<0 || V>254 || Values.Contains(V))return {};Values.Add(V);}if(Values.IsEmpty())return {};Path=E->GetPathName();return TEXT("enum");}
 if(CastField<FByteProperty>(P))return TEXT("byte");if(CastField<FTextProperty>(P))return TEXT("text");
 if(CastField<FIntProperty>(P))return TEXT("int");if(CastField<FFloatProperty>(P))return TEXT("float");if(CastField<FDoubleProperty>(P))return TEXT("double");if(CastField<FBoolProperty>(P))return TEXT("bool");if(CastField<FStrProperty>(P))return TEXT("string");if(CastField<FNameProperty>(P))return TEXT("name");
 if(auto* K=CastField<FClassProperty>(P)){Path=K->MetaClass->GetPathName();return TEXT("class");}
 if(auto* O=CastField<FObjectProperty>(P)){if(O->PropertyClass->HasAnyClassFlags(CLASS_Interface))return {};Path=O->PropertyClass->GetPathName();return TEXT("object");}
 if(auto* S=CastField<FStructProperty>(P)){Path=S->Struct->GetPathName();if(Path==TEXT("/Script/CoreUObject.Vector") || Path==TEXT("/Script/CoreUObject.Vector2D") || Path==TEXT("/Script/CoreUObject.Rotator") || Path==TEXT("/Script/CoreUObject.Transform"))return TEXT("struct");}
 Path.Reset();return {};
}

// Share the exact container carriers and struct snapshot policy with authored calls.
inline bool DasNativeGameplayType(FProperty* P,FDasDeclaredType& T,int32 Depth=0){
 if(!P || Depth>16)return false;
 T={};T.Kind=DasNativeGameplayKind(P,T.TypePath);

 if(auto* M=CastField<FMapProperty>(P)){
  if(P->ArrayDim!=1)return false;T.Kind=TEXT("map");T.TypePath.Reset();auto Key=MakeShared<FDasDeclaredType>(),Value=MakeShared<FDasDeclaredType>();T.Key=Key;T.Value=Value;
  if(!DasNativeGameplayType(M->KeyProp,*Key,Depth+1) || !DasNativeGameplayType(M->ValueProp,*Value,Depth+1))return false;
  if(Key->Kind!=TEXT("int") && Key->Kind!=TEXT("string") && Key->Kind!=TEXT("name") && Key->Kind!=TEXT("enum") && Key->Kind!=TEXT("object"))return false;
  if(Value->Kind==TEXT("array") || Value->Kind==TEXT("map") || Value->Kind==TEXT("set") || Value->Kind==TEXT("byte") || Value->Kind==TEXT("text") || Value->Kind==TEXT("class"))return false;
  auto* C=DasContainerCarrier(T);auto* Values=C?C->FindPropertyByName(TEXT("Values")):nullptr;return Values && Values->SameType(P);
 }
 if(auto* S=CastField<FSetProperty>(P)){
  if(P->ArrayDim!=1)return false;T.Kind=TEXT("set");T.TypePath.Reset();auto Element=MakeShared<FDasDeclaredType>();T.Element=Element;
  if(!DasNativeGameplayType(S->ElementProp,*Element,Depth+1))return false;
  if(Element->Kind!=TEXT("int") && Element->Kind!=TEXT("string") && Element->Kind!=TEXT("name") && Element->Kind!=TEXT("enum") && Element->Kind!=TEXT("object"))return false;
  auto* C=DasContainerCarrier(T);auto* Values=C?C->FindPropertyByName(TEXT("Values")):nullptr;return Values && Values->SameType(P);
 }
 if(auto* A=CastField<FArrayProperty>(P)){
  if(P->ArrayDim!=1)return false;T.Kind=TEXT("array");T.TypePath.Reset();auto Element=MakeShared<FDasDeclaredType>();T.Element=Element;
  if(!DasNativeGameplayType(A->Inner,*Element,Depth+1) || T.Element->Kind==TEXT("array") || T.Element->Kind==TEXT("map") || T.Element->Kind==TEXT("set") || T.Element->Kind==TEXT("byte") || T.Element->Kind==TEXT("text") || T.Element->Kind==TEXT("class"))return false;
  auto* Carrier=DasContainerCarrier(T);auto* Values=Carrier?Carrier->FindPropertyByName(TEXT("Values")):nullptr;return Values && Values->SameType(P);
 }
 if(auto* S=CastField<FStructProperty>(P)){
  if(!T.Kind.IsEmpty())return true;
  FString Error;if(P->ArrayDim!=1 || !S->Struct->GetPathName().StartsWith(TEXT("/Script/")) || !ValidateDasValueStruct(S->Struct->GetPathName(),Error))return false;
  int32 Count=0;for(TFieldIterator<FProperty> I(S->Struct);I;++I){FDasDeclaredType Child;if(!DasNativeGameplayType(*I,Child,Depth+1) || Child.Kind==TEXT("byte") || Child.Kind==TEXT("text") || Child.Kind==TEXT("class"))return false;++Count;}
  if(!Count)return false;T.Kind=TEXT("struct");T.TypePath=S->Struct->GetPathName();return true;
 }
 return !T.Kind.IsEmpty();
}
