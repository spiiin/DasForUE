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
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "daScript")
    FString ScriptFile;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "daScript")
    TMap<FName, FName> InputHandlers;
    UPROPERTY(Transient, BlueprintReadOnly, Category = "daScript")
    FString LastInputError;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "daScript")
    FName PaintHandler;
    UPROPERTY(Transient, BlueprintReadOnly, Category = "daScript")
    FString LastPaintError;

    UFUNCTION(BlueprintCallable, Category = "daScript")
    bool BindInputHandler(FName Event, FName Handler);
    static bool IsSupportedInputEvent(FName Event);
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "daScript")
    TMap<FName, FName> DragHandlers;
    UPROPERTY(Transient, BlueprintReadOnly, Category = "daScript")
    FString LastDragError;
    UFUNCTION(BlueprintCallable, Category = "daScript")
    bool BindDragHandler(FName Event, FName Handler);
    static bool IsSupportedDragEvent(FName Event);

protected:
    virtual void NativeDestruct() override;
    virtual FReply NativeOnFocusReceived(const FGeometry& Geometry, const FFocusEvent& Event) override;
    virtual void NativeOnFocusLost(const FFocusEvent& Event) override;
    virtual void NativeOnDragDetected(const FGeometry& Geometry, const FPointerEvent& Event,
                                      UDragDropOperation*& Operation) override;
    virtual void NativeOnDragEnter(const FGeometry& Geometry, const FDragDropEvent& Event,
                                   UDragDropOperation* Operation) override;
    virtual void NativeOnDragLeave(const FDragDropEvent& Event, UDragDropOperation* Operation) override;
    virtual bool NativeOnDragOver(const FGeometry& Geometry, const FDragDropEvent& Event,
                                  UDragDropOperation* Operation) override;
    virtual bool NativeOnDrop(const FGeometry& Geometry, const FDragDropEvent& Event,
                              UDragDropOperation* Operation) override;
    virtual void NativeOnDragCancelled(const FDragDropEvent& Event, UDragDropOperation* Operation) override;
    virtual int32 NativePaint(const FPaintArgs& Args, const FGeometry& Geometry, const FSlateRect& Culling,
                              FSlateWindowElementList& Elements, int32 Layer, const FWidgetStyle& Style,
                              bool ParentEnabled) const override;
    virtual FReply NativeOnKeyChar(const FGeometry& Geometry, const FCharacterEvent& Event) override;
    virtual FReply NativeOnPreviewKeyDown(const FGeometry& Geometry, const FKeyEvent& Event) override;
    virtual FReply NativeOnKeyDown(const FGeometry& Geometry, const FKeyEvent& Event) override;
    virtual FReply NativeOnKeyUp(const FGeometry& Geometry, const FKeyEvent& Event) override;
    virtual FReply NativeOnAnalogValueChanged(const FGeometry& Geometry, const FAnalogInputEvent& Event) override;
    virtual FReply NativeOnMouseButtonDown(const FGeometry& Geometry, const FPointerEvent& Event) override;
    virtual FReply NativeOnPreviewMouseButtonDown(const FGeometry& Geometry, const FPointerEvent& Event) override;
    virtual FReply NativeOnMouseButtonUp(const FGeometry& Geometry, const FPointerEvent& Event) override;
    virtual FReply NativeOnMouseMove(const FGeometry& Geometry, const FPointerEvent& Event) override;
    virtual FReply NativeOnMouseWheel(const FGeometry& Geometry, const FPointerEvent& Event) override;
    virtual FReply NativeOnMouseButtonDoubleClick(const FGeometry& Geometry, const FPointerEvent& Event) override;
    virtual FReply NativeOnTouchGesture(const FGeometry& Geometry, const FPointerEvent& Event) override;
    virtual FReply NativeOnTouchStarted(const FGeometry& Geometry, const FPointerEvent& Event) override;
    virtual FReply NativeOnTouchMoved(const FGeometry& Geometry, const FPointerEvent& Event) override;
    virtual FReply NativeOnTouchEnded(const FGeometry& Geometry, const FPointerEvent& Event) override;
    virtual FReply NativeOnTouchForceChanged(const FGeometry& Geometry, const FPointerEvent& Event) override;
    virtual FReply NativeOnTouchFirstMove(const FGeometry& Geometry, const FPointerEvent& Event) override;

private:
    FString ActiveScript;
    bool DispatchDrag(FName EventName, const FGeometry* Geometry, const FPointerEvent& Event,
                      UDragDropOperation* Operation, UDragDropOperation*& OutOperation, bool& Handled);
    bool DispatchInput(FName EventName, const FGeometry& Geometry, const UScriptStruct* Type, const void* Source,
                       FReply& Reply);
};
