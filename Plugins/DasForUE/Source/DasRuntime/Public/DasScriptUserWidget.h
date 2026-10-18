#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DasScriptUserWidget.generated.h"

// Synchronous native Slate entry points. Blueprint children can provide their widget tree.
UCLASS(Blueprintable)
class DASRUNTIME_API UDasScriptUserWidget : public UUserWidget
{
 GENERATED_BODY()
public:
 UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="daScript") FString ScriptFile;
 UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="daScript") TMap<FName,FName> InputHandlers;
 UPROPERTY(Transient, BlueprintReadOnly, Category="daScript") FString LastInputError;

 UFUNCTION(BlueprintCallable, Category="daScript") bool BindInputHandler(FName Event,FName Handler);
 static bool IsSupportedInputEvent(FName Event);
protected:
 virtual void NativeDestruct() override;
 virtual FReply NativeOnKeyChar(const FGeometry& Geometry,const FCharacterEvent& Event) override;
 virtual FReply NativeOnPreviewKeyDown(const FGeometry& Geometry,const FKeyEvent& Event) override;
 virtual FReply NativeOnKeyDown(const FGeometry& Geometry,const FKeyEvent& Event) override;
 virtual FReply NativeOnKeyUp(const FGeometry& Geometry,const FKeyEvent& Event) override;
 virtual FReply NativeOnAnalogValueChanged(const FGeometry& Geometry,const FAnalogInputEvent& Event) override;
 virtual FReply NativeOnMouseButtonDown(const FGeometry& Geometry,const FPointerEvent& Event) override;
 virtual FReply NativeOnPreviewMouseButtonDown(const FGeometry& Geometry,const FPointerEvent& Event) override;
 virtual FReply NativeOnMouseButtonUp(const FGeometry& Geometry,const FPointerEvent& Event) override;
 virtual FReply NativeOnMouseMove(const FGeometry& Geometry,const FPointerEvent& Event) override;
 virtual FReply NativeOnMouseWheel(const FGeometry& Geometry,const FPointerEvent& Event) override;
 virtual FReply NativeOnMouseButtonDoubleClick(const FGeometry& Geometry,const FPointerEvent& Event) override;
 virtual FReply NativeOnTouchGesture(const FGeometry& Geometry,const FPointerEvent& Event) override;
 virtual FReply NativeOnTouchStarted(const FGeometry& Geometry,const FPointerEvent& Event) override;
 virtual FReply NativeOnTouchMoved(const FGeometry& Geometry,const FPointerEvent& Event) override;
 virtual FReply NativeOnTouchEnded(const FGeometry& Geometry,const FPointerEvent& Event) override;
 virtual FReply NativeOnTouchForceChanged(const FGeometry& Geometry,const FPointerEvent& Event) override;
 virtual FReply NativeOnTouchFirstMove(const FGeometry& Geometry,const FPointerEvent& Event) override;
private:
 FString ActiveScript;
 bool DispatchInput(FName EventName,const UScriptStruct* Type,const void* Source,FReply& Reply);
};
