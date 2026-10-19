#include "DasImageSoftBrush.h"
#include "DasSoftReferences.h"
#include "DasExecutionRole.h"
#include "Components/Image.h"
#include "Blueprint/UserWidget.h"
#include "Engine/World.h"
#include "Engine/Texture2D.h"
#include "Materials/MaterialInterface.h"
#include "Engine/AssetManager.h"
#include "Engine/StreamableManager.h"
#include "AssetRegistry/AssetData.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "UObject/UnrealType.h"
namespace
{
TMap<TWeakObjectPtr<UImage>, TWeakPtr<FDasImageSoftBrushRequest>> Latest;
bool Role()
{
    return IsInGameThread() && GetDasExecutionRole() == EDasExecutionRole::Gameplay &&
           !IsDasFrozenPhysicsWorld(nullptr);
}
}
bool DasImageSoftBrushFunctionAllowed(const UFunction* F)
{
    if (!F || F->GetOuterUClass() != UImage::StaticClass() ||
        !F->HasAllFunctionFlags(FUNC_Native | FUNC_BlueprintCallable) || F->HasAnyFunctionFlags(FUNC_Static) ||
        F->GetReturnProperty())
        return false;
    const bool Texture = F->GetFName() == TEXT("SetBrushFromSoftTexture"),
                                               Material = F->GetFName() == TEXT("SetBrushFromSoftMaterial");
    if (!Texture && !Material)
        return false;
    int32 Count = 0;
    for (TFieldIterator<FProperty> I(F); I; ++I)
    {
        auto* P = *I;
        if (!P->HasAnyPropertyFlags(CPF_Parm))
            continue;
        ++Count;
        if (P->ArrayDim != 1 || P->HasAnyPropertyFlags(CPF_ReturnParm | CPF_OutParm | CPF_ReferenceParm))
            return false;
        if (P->GetFName() == (Texture ? TEXT("SoftTexture"): TEXT("SoftMaterial")))
        {
            auto* Soft = CastField<FSoftObjectProperty>(P);
            if (!Soft || Soft->IsA<FSoftClassProperty>() ||
                Soft->PropertyClass != (Texture ? UTexture2D::StaticClass() : UMaterialInterface::StaticClass()))
                return false;
        }
        else if (!Texture || P->GetFName() != TEXT("bMatchSize") || !P->IsA<FBoolProperty>())
            return false;
    }
    return Count == (Texture ? 2 : 1);
}
bool FDasImageSoftBrushRequest::IsLive() const
{
    auto* I = Image.Get();
    auto* O = Owner.Get();
    auto* W = World.Get();
    if (!Role() || !IsValid(I) || !IsValid(O) || !IsValid(W) || W->bIsTearingDown || !W->IsGameWorld() ||
        I->GetWorld() != W || O->GetWorld() != W || I->GetTypedOuter<UUserWidget>() != O)
        return false;
    auto A = ImageSlate.Pin(), B = OwnerSlate.Pin();
    return A.IsValid() && B.IsValid() && A == I->GetCachedWidget() && B == O->GetCachedWidget();
}
FDasImageSoftBrushRequest::~FDasImageSoftBrushRequest()
{
    Cancel();
}
void FDasImageSoftBrushRequest::Cancel()
{
    State = EDasImageSoftBrushState::Cancelled;
    if (Stream)
    {
        Stream->CancelHandle();
        Stream.Reset();
    }
    if (Watch.IsValid())
    {
        FTSTicker::GetCoreTicker().RemoveTicker(Watch);
        Watch.Reset();
    }
    if (auto* O = Owner.Get())
        O->OnNativeDestruct.Remove(Destruct);
    Destruct.Reset();
}
void FDasImageSoftBrushRequest::Start()
{
    if (State == EDasImageSoftBrushState::Loading && Stream)
        Stream->StartStalledHandle();
}
void FDasImageSoftBrushRequest::Complete()
{
    if (State != EDasImageSoftBrushState::Loading)
        return;
    auto* I = Image.Get();
    const auto* Entry = Latest.Find(Image);
    if (!IsLive() || !Entry || Entry->Pin().Get() != this ||
        I->GetBrush().GetResourceObject() != PreviousResource.Get())
    {
        Cancel();
        return;
    }
    UObject* Asset = Path.ResolveObject();
    if (!IsValid(Asset) || (Texture ? !Asset->IsA<UTexture2D>() : !Asset->IsA<UMaterialInterface>()))
    {
        State = EDasImageSoftBrushState::Failed;
        Error = TEXT("Soft brush asset could not be loaded with the expected type");
        Stream.Reset();
        return;
    }
    // Native setters preserve notifications, streaming cancellation and bMatchSize semantics.
    if (Texture)
        I->SetBrushFromTexture(CastChecked<UTexture2D>(Asset), MatchSize);
    else
        I->SetBrushFromMaterial(CastChecked<UMaterialInterface>(Asset));
    State = EDasImageSoftBrushState::Ready;
    Stream.Reset();
    if (Watch.IsValid())
    {
        FTSTicker::GetCoreTicker().RemoveTicker(Watch);
        Watch.Reset();
    }
}
void CancelDasImageSoftBrush(UImage* Image)
{
    if (!IsInGameThread())
        return;
    if (auto* Entry = Latest.Find(Image))
    {
        if (auto P = Entry->Pin())
            P->Cancel();
        Latest.Remove(Image);
    }
}
FDasImageSoftBrushLifetime::~FDasImageSoftBrushLifetime()
{
    Revoke();
}
void FDasImageSoftBrushLifetime::Revoke()
{
    for (auto& P : Requests)
        P.Value->Cancel();
    Requests.Empty();
}
TSharedPtr<FDasImageSoftBrushRequest> FDasImageSoftBrushLifetime::Begin(UImage* I, UWorld* W,
                                                                        const FSoftObjectPath& Path, bool Texture,
                                                                        bool MatchSize, FString& Error,
                                                                        bool StartImmediately)
{
    Error.Empty();
    auto* O = IsValid(I) ? I->GetTypedOuter<UUserWidget>() : nullptr;
    if (!Role() || !IsValid(I) || I->IsTemplate() || !IsValid(O) || O->IsTemplate() || !IsValid(W) ||
        !W->IsGameWorld() || W->bIsTearingDown || I->GetWorld() != W || O->GetWorld() != W ||
        !I->GetCachedWidget().IsValid() || !O->GetCachedWidget().IsValid())
    {
        Error = TEXT("Soft brush requires a constructed Image in a live game-world UserWidget");
        return {};
    }
    if (!ValidateDasSoftPath(Path.ToString(), Error))
        return {};
    auto* Base = Texture ? UTexture2D::StaticClass() : UMaterialInterface::StaticClass();
    UObject* Loaded = Path.ResolveObject();
    if (Loaded && !Loaded->IsA(Base))
    {
        Error = TEXT("Soft brush asset type mismatch");
        return {};
    }
    if (!Path.IsNull() && !Loaded)
    {
        auto& Registry = FModuleManager::LoadModuleChecked<FAssetRegistryModule>(TEXT("AssetRegistry")).Get();
        const auto Data = Registry.GetAssetByObjectPath(Path);
        if (!Data.IsValid())
        {
            Error = TEXT("Soft brush asset is not registered");
            return {};
        }
        auto* Class = FindObject<UClass>(nullptr, *Data.AssetClassPath.ToString());
        if (Class && !Class->IsChildOf(Base))
        {
            Error = TEXT("Soft brush asset type mismatch");
            return {};
        }
    }
    for (auto It = Requests.CreateIterator(); It; ++It)
        if (!It.Value()->IsLive())
            It.RemoveCurrent();
    for (auto It = Latest.CreateIterator(); It; ++It)
        if (!It.Key().IsValid() || !It.Value().IsValid())
            It.RemoveCurrent();
    if (!Requests.Contains(I) && Requests.Num() >= 256)
    {
        Error = TEXT("Soft brush request limit exceeded");
        return {};
    }
    auto P = MakeShared<FDasImageSoftBrushRequest>();
    P->Image = I;
    P->Owner = O;
    P->World = W;
    P->Path = Path;
    P->Texture = Texture;
    P->MatchSize = MatchSize;
    P->ImageSlate = I->GetCachedWidget();
    P->OwnerSlate = O->GetCachedWidget();
    P->PreviousResource = I->GetBrush().GetResourceObject();
    if (!Path.IsNull() && !Loaded)
    {
        TWeakPtr<FDasImageSoftBrushRequest> Weak = P;
        P->Stream = UAssetManager::GetStreamableManager().RequestAsyncLoad(Path,
                                                                           FStreamableDelegate::CreateLambda(
                                                                               [Weak]()
                                                                               {
                                                                                   if (auto Value = Weak.Pin())
                                                                                       Value->Complete();
                                                                               }),
                                                                           FStreamableManager::AsyncLoadHighPriority,
                                                                           false, true, TEXT("DasForUE:ImageBrush"));
        if (!P->Stream)
        {
            Error = TEXT("Soft brush load request could not be created");
            return {};
        }
    }
    CancelDasImageSoftBrush(I);
    Requests.Add(I, P);
    Latest.Add(I, P);
    TWeakPtr<FDasImageSoftBrushRequest> Weak = P;
    P->Destruct = O->OnNativeDestruct.AddLambda(
        [Weak](UUserWidget*)
        {
            if (auto Value = Weak.Pin())
                Value->Cancel();
        });
    if (P->Stream)
        P->Watch = FTSTicker::GetCoreTicker().AddTicker(FTickerDelegate::CreateLambda(
            [Weak](float)
            {
                auto Value = Weak.Pin();
                if (!Value || Value->State != EDasImageSoftBrushState::Loading)
                    return false;
                if (!Value->IsLive())
                {
                    Value->Cancel();
                    return false;
                }
                return true;
            }));
    if (Path.IsNull())
    {
        if (Texture)
            I->SetBrushFromTexture(nullptr, MatchSize);
        else
            I->SetBrushFromMaterial(nullptr);
        P->State = EDasImageSoftBrushState::Ready;
    }
    else if (Loaded)
        P->Complete();
    else if (StartImmediately)
        P->Start();
    return P;
}
