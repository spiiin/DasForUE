#pragma once
#include "CoreMinimal.h"
#include "DasSchema.h"
#include "UObject/UnrealType.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "Camera/CameraComponent.h"
inline TArray<UClass*> DasNativeGameplayClasses(){return {ACharacter::StaticClass(),UCharacterMovementComponent::StaticClass(),APlayerController::StaticClass(),UCameraComponent::StaticClass()};}
inline FString DasNativeGameplayKind(FProperty* P,FString& Path){
 Path.Reset();if(!P || P->ArrayDim!=1)return {};
 if(auto* E=DasEnumPropertyType(P)){TSet<int64> Values;for(int32 I=0;I<E->NumEnums();++I){const auto N=E->GetNameStringByIndex(I);if(N.EndsWith(TEXT("_MAX")))continue;const int64 V=E->GetValueByIndex(I);if(V<0 || V>254 || Values.Contains(V))return {};Values.Add(V);}if(Values.IsEmpty())return {};Path=E->GetPathName();return TEXT("enum");}
 if(CastField<FIntProperty>(P))return TEXT("int");if(CastField<FFloatProperty>(P))return TEXT("float");if(CastField<FDoubleProperty>(P))return TEXT("double");if(CastField<FBoolProperty>(P))return TEXT("bool");if(CastField<FStrProperty>(P))return TEXT("string");if(CastField<FNameProperty>(P))return TEXT("name");
 if(auto* K=CastField<FClassProperty>(P)){Path=K->MetaClass->GetPathName();return TEXT("class");}
 if(auto* O=CastField<FObjectProperty>(P)){if(O->PropertyClass->HasAnyClassFlags(CLASS_Interface))return {};Path=O->PropertyClass->GetPathName();return TEXT("object");}
 if(auto* S=CastField<FStructProperty>(P)){Path=S->Struct->GetPathName();if(Path==TEXT("/Script/CoreUObject.Vector") || Path==TEXT("/Script/CoreUObject.Vector2D") || Path==TEXT("/Script/CoreUObject.Rotator") || Path==TEXT("/Script/CoreUObject.Transform"))return TEXT("struct");}
 Path.Reset();return {};
}
