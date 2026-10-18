#pragma once
#include "CoreMinimal.h"
#include "Animation/WidgetAnimationHandle.h"
#include "Templates/SharedPointer.h"
class UUserWidget;
class UFunction;
class UScriptStruct;
struct FDasWidgetAnimationLease {
 FWidgetAnimationHandle Handle;
 TWeakObjectPtr<UUserWidget> Widget;
 FDelegateHandle Finished,Destruct;
 bool Revoked=false;
 ~FDasWidgetAnimationLease();
 bool IsLive()const;
};
bool DasIsWidgetAnimationType(const UScriptStruct* Type);
bool DasWidgetAnimationFunctionAllowed(const UFunction* Function);
TSharedPtr<FDasWidgetAnimationLease> BeginDasWidgetAnimationLease(UUserWidget* Widget);
bool CompleteDasWidgetAnimationLease(const TSharedPtr<FDasWidgetAnimationLease>& Lease,const FWidgetAnimationHandle& Handle);
