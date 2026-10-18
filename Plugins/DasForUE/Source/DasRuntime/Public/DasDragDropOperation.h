#pragma once
#include "CoreMinimal.h"
#include "Blueprint/DragDropOperation.h"
#include "DasDragDropOperation.generated.h"
class UDasScriptUserWidget;

// Native drag data only. No VM pointers, DAS closures, or borrowed input paths.
UCLASS(NotBlueprintable)
class DASRUNTIME_API UDasDragDropOperation final : public UDragDropOperation {
 GENERATED_BODY()
public:
 void Initialize(UDasScriptUserWidget* Source,UObject* Data,UWidget* Visual,const FString& Label,int32 InPivot,const FVector2D& InOffset,uint32 InUserIndex,uint32 InPointerIndex);
 virtual UWorld* GetWorld()const override;
 bool IsLive()const;
 bool MatchesPointer(const FPointerEvent& Event)const{return Event.GetUserIndex()==UserIndex && Event.GetPointerIndex()==PointerIndex;}
 void Revoke();
 virtual void Drop_Implementation(const FPointerEvent& Event) override;
 virtual void DragCancelled_Implementation(const FPointerEvent& Event) override;
 virtual void Dragged_Implementation(const FPointerEvent& Event) override;
private:
 TWeakObjectPtr<UDasScriptUserWidget> SourceWidget;
 bool Live=false;uint32 UserIndex=0,PointerIndex=0;
};

struct FDasDragLifetime {
 TArray<TWeakObjectPtr<UDasDragDropOperation>> Operations;
 bool Track(UDasDragDropOperation* Op);
 void Revoke();
 ~FDasDragLifetime(){Revoke();}
};
