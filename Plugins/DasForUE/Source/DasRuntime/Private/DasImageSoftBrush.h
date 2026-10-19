#pragma once
#include "CoreMinimal.h"
#include "UObject/SoftObjectPath.h"
#include "Containers/Ticker.h"
class UImage;
class UUserWidget;
class UWorld;
struct FStreamableHandle;
class SWidget;
enum class EDasImageSoftBrushState : uint8
{
    Loading,
    Ready,
    Failed,
    Cancelled
};

// Native asynchronous state only: no VM/Context/function pointers, no strong widgets.
struct DASRUNTIME_API FDasImageSoftBrushRequest : TSharedFromThis<FDasImageSoftBrushRequest>
{
    ~FDasImageSoftBrushRequest();
    void Cancel();
    void Start();
    void Complete();
    bool IsLive() const;
    EDasImageSoftBrushState State = EDasImageSoftBrushState::Loading;
    FString Error;
    TWeakObjectPtr<UImage> Image;
    TWeakObjectPtr<UUserWidget> Owner;
    TWeakObjectPtr<UWorld> World;
    TWeakPtr<SWidget> ImageSlate, OwnerSlate;
    TWeakObjectPtr<UObject> PreviousResource;
    FSoftObjectPath Path;
    bool Texture = false, MatchSize = false;
    FDelegateHandle Destruct;
    FTSTicker::FDelegateHandle Watch;
    TSharedPtr<FStreamableHandle> Stream;
};
struct DASRUNTIME_API FDasImageSoftBrushLifetime
{
    ~FDasImageSoftBrushLifetime();
    TSharedPtr<FDasImageSoftBrushRequest> Begin(UImage* Image, UWorld* World, const FSoftObjectPath& Path, bool Texture,
                                                bool MatchSize, FString& Error, bool StartImmediately = true);
    void Revoke();
    TMap<TWeakObjectPtr<UImage>, TSharedPtr<FDasImageSoftBrushRequest>> Requests;
};
DASRUNTIME_API bool DasImageSoftBrushFunctionAllowed(const UFunction* Function);
// Invalidate our pending write before other generated Image brush mutations.
DASRUNTIME_API void CancelDasImageSoftBrush(UImage* Image);
