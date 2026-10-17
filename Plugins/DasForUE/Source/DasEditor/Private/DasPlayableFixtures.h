#pragma once
#include "InputAction.h"
#include "InputMappingContext.h"
#include "InputModifiers.h"
#include "InputCoreTypes.h"
#include "UObject/UnrealType.h"
#include "UObject/SavePackage.h"
#include "Misc/PackageName.h"
#include "HAL/FileManager.h"
#include "Engine/Blueprint.h"
inline bool PrepareDasPlayableInputAssets(const FString& Root,UBlueprint* BP,FString& Error,bool SaveAssets){
 auto Save=[&](UObject* O){if(!SaveAssets)return true;const FString File=FPackageName::LongPackageNameToFilename(O->GetOutermost()->GetName(),FPackageName::GetAssetPackageExtension());IFileManager::Get().MakeDirectory(*FPaths::GetPath(File),true);FSavePackageArgs Args;Args.TopLevelFlags=RF_Public|RF_Standalone;return UPackage::SavePackage(O->GetOutermost(),O,*File,Args);};
 auto Action=[&](const TCHAR* Name,EInputActionValueType Type){auto* A=NewObject<UInputAction>(CreatePackage(*(Root+TEXT("/")+Name)),Name,RF_Public|RF_Standalone);A->ValueType=Type;return A;};
 auto* Move=Action(TEXT("IA_Move"),EInputActionValueType::Axis2D);auto* Look=Action(TEXT("IA_Look"),EInputActionValueType::Axis2D);auto* Jump=Action(TEXT("IA_Jump"),EInputActionValueType::Boolean);
 auto* Context=NewObject<UInputMappingContext>(CreatePackage(*(Root+TEXT("/IMC_Character"))),TEXT("IMC_Character"),RF_Public|RF_Standalone);
 auto Map=[&](UInputAction* A,FKey Key,bool Y,bool Negative){auto& M=Context->MapKey(A,Key);if(Negative)M.Modifiers.Add(NewObject<UInputModifierNegate>(Context));if(Y){auto* Swizzle=NewObject<UInputModifierSwizzleAxis>(Context);Swizzle->Order=EInputAxisSwizzle::YXZ;M.Modifiers.Add(Swizzle);}};
 Map(Move,EKeys::W,true,false);Map(Move,EKeys::S,true,true);Map(Move,EKeys::D,false,false);Map(Move,EKeys::A,false,true);Map(Look,EKeys::MouseX,false,false);Map(Look,EKeys::MouseY,true,true);Map(Jump,EKeys::SpaceBar,false,false);
 auto Assign=[&](const TCHAR* N,UObject* O){auto* P=FindFProperty<FObjectPropertyBase>(BP->GeneratedClass,N);if(!P)return false;P->SetObjectPropertyValue_InContainer(BP->GeneratedClass->GetDefaultObject(),O);return true;};
 if(!Assign(TEXT("Controls"),Context) || !Assign(TEXT("MoveAction"),Move) || !Assign(TEXT("LookAction"),Look) || !Assign(TEXT("JumpAction"),Jump) || !Save(Move) || !Save(Look) || !Save(Jump) || !Save(Context)){Error=TEXT("Cannot create/save playable input assets");return false;}
 return true;
}
