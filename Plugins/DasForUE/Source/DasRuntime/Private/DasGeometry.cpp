#include "DasTypeAdapters.h"
#include "DasGeometry.h"
#include "DasExecutionRole.h"
#include "Layout/Geometry.h"
#include "UObject/UnrealType.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "Engine/GameViewportClient.h"
#include "GameFramework/PlayerController.h"
#include "Slate/SGameLayerManager.h"
#include "UnrealClient.h"
#include "Widgets/SWindow.h"
#include <cfloat>
namespace
{
bool Fits(double X)
{
    return FMath::IsFinite(X) && FMath::Abs(X) <= double(FLT_MAX);
}
bool Vec(FVector2D V)
{
    return Fits(V.X) && Fits(V.Y);
}
bool Role(FString& E)
{
    if (!IsInGameThread() || GetDasExecutionRole() != EDasExecutionRole::Gameplay || IsDasFrozenPhysicsWorld(nullptr))
    {
        E = TEXT("Geometry requires gameplay game thread");
        return false;
    }
    return true;
}
bool TransformSafe(const FGeometry& G, FVector2D V, bool Inverse, bool Vector, bool Scalar, FString& E)
{
    float A, B, C, D;
    G.GetAccumulatedRenderTransform().GetMatrix().GetMatrix(A, B, C, D);
    double a = A, b = B, c = C, d = D;
    FVector2D T = G.GetAccumulatedRenderTransform().GetTranslation();
    if (Inverse)
    {
        const double Det = double(A * D - B * C);
        a = D / Det;
        b = -B / Det;
        c = -C / Det;
        d = A / Det;
        T = FVector2D(-(a * T.X + c * T.Y), -(b * T.X + d * T.Y));
    }
    const double X = FMath::Abs(a * V.X) + FMath::Abs(c * V.Y) + (Vector ? 0 : FMath::Abs(T.X));
    const double Y = FMath::Abs(b * V.X) + FMath::Abs(d * V.Y) + (Vector ? 0 : FMath::Abs(T.Y));
    if (!Vec(V) || !Fits(X) || !Fits(Y) || (Scalar && X * X + Y * Y > double(FLT_MAX)))
    {
        E = TEXT("Geometry transform would overflow Slate float math");
        return false;
    }
    return true;
}
}
bool DasIsGeometryType(const UScriptStruct* T)
{
    return DasHasNativeRole(T, EDasNativeTypeRole::Geometry);
}
bool ValidateDasGeometryData(const UScriptStruct* T, const void* Data, FString& E, bool Inverse)
{
    E.Reset();
    if (!Role(E))
        return false;
    if (!DasIsGeometryType(T) || !Data)
    {
        E = TEXT("Geometry snapshot identity/data mismatch");
        return false;
    }
    const auto& G = *static_cast<const FGeometry*>(Data);
    float A, B, C, D;
    G.GetAccumulatedRenderTransform().GetMatrix().GetMatrix(A, B, C, D);
    const auto L = G.GetAccumulatedLayoutTransform();
    if (!Vec(G.GetLocalSize()) || !Vec(G.GetLocalPositionAtCoordinates(FVector2f::ZeroVector)) ||
        !Vec(L.GetTranslation()) || !Fits(L.GetScale()) || !Vec(G.GetAccumulatedRenderTransform().GetTranslation()) ||
        !Fits(A) || !Fits(B) || !Fits(C) || !Fits(D))
    {
        E = TEXT("nonfinite Geometry snapshot");
        return false;
    }
    if (Inverse)
    {
        // Test the same float determinant used by UE, before calling native Inverse (checkSlow on zero).
        const double AD = double(A) * D, BC = double(B) * C;
        if (!Fits(AD) || !Fits(BC) || !Fits(AD - BC))
        {
            E = TEXT("Geometry inverse determinant overflow");
            return false;
        }
        const float Det = A * D - B * C;
        if (Det == 0 || !Fits(1.0 / double(Det)))
        {
            E = TEXT("Geometry transform is singular or inverse overflows");
            return false;
        }
        const double R = 1.0 / double(Det);
        const auto P = G.GetAccumulatedRenderTransform().GetTranslation();
        if (!Fits(D * R) || !Fits(B * R) || !Fits(C * R) || !Fits(A * R) ||
            !Fits(FMath::Abs(P.X * D * R) + FMath::Abs(P.Y * C * R)) ||
            !Fits(FMath::Abs(P.X * B * R) + FMath::Abs(P.Y * A * R)))
        {
            E = TEXT("Geometry inverse overflows");
            return false;
        }
    }
    return true;
}
bool ValidateDasGeometrySnapshot(const FInstancedStruct& V, FString& E)
{
    return ValidateDasGeometryData(V.GetScriptStruct(), V.GetMemory(), E);
}
bool CaptureDasGeometry(const UScriptStruct* T, const void* Source, FInstancedStruct& Out, FString& E)
{
    if (!ValidateDasGeometryData(T, Source, E))
        return false;
    FInstancedStruct Copy;
    Copy.InitializeAs(T, static_cast<const uint8*>(Source));
    Out = MoveTemp(Copy);
    return true;
}
bool DasGeometryFunctionAllowed(const UFunction* F)
{
    if (!F || !F->HasAnyFunctionFlags(FUNC_Native))
        return false;
    const FString Path = F->GetOuterUClass()->GetPathName() + TEXT(".")+ F->GetName();
    static const TCHAR* Allowed[] = {TEXT("/Script/UMG.SlateBlueprintLibrary.AbsoluteToLocal"),
        TEXT("/Script/UMG.SlateBlueprintLibrary.GetAbsoluteSize"),
            TEXT("/Script/UMG.SlateBlueprintLibrary.GetLocalSize"),
                TEXT("/Script/UMG.SlateBlueprintLibrary.GetLocalTopLeft"),
                    TEXT("/Script/UMG.SlateBlueprintLibrary.IsUnderLocation"),
                        TEXT("/Script/UMG.SlateBlueprintLibrary.LocalToAbsolute"),
                            TEXT("/Script/UMG.SlateBlueprintLibrary.LocalToViewport"),
                                TEXT("/Script/UMG.SlateBlueprintLibrary.Scalar_AbsoluteToLocal"),
                                    TEXT("/Script/UMG.SlateBlueprintLibrary.Scalar_LocalToAbsolute"),
                                        TEXT("/Script/UMG.SlateBlueprintLibrary.ScreenToWidgetLocal"),
                                            TEXT("/Script/UMG.SlateBlueprintLibrary.Vector_AbsoluteToLocal"),
                                                TEXT("/Script/UMG.SlateBlueprintLibrary.Vector_LocalToAbsolute"),
                                                    TEXT("/Script/UMG.Widget.GetCachedGeometry"),
                                                        TEXT("/Script/UMG.Widget.GetPaintSpaceGeometry"),
                                                            TEXT("/Script/UMG.Widget.GetTickSpaceGeometry"),
                                                                TEXT("/Script/UMG.WidgetLayoutLibrary.GetPlayerScreenWidgetGeometry"),
                                                                    TEXT("/Script/UMG.WidgetLayoutLibrary.GetViewportWidgetGeometry")};
    for (const TCHAR* P : Allowed)
        if (Path == P)
            return true;
    return false;
}
bool ValidateDasGeometryCall(const UFunction* F, void* Frame, FString& E, bool Outputs)
{
    if (!DasGeometryFunctionAllowed(F))
        return true;
    if (!Role(E))
        return false;
    const FString Name = F->GetName();
    const bool Inverse = Name.Contains(TEXT("AbsoluteToLocal")) || Name == TEXT("ScreenToWidgetLocal");
    const FGeometry* Geometry = nullptr;
    FVector2D Coordinate = FVector2D::ZeroVector;
    bool HasCoordinate = false;
    UObject* WorldObject = nullptr;
    for (TFieldIterator<FProperty> It(F); It; ++It)
    {
        auto* P = *It;
        if (!P->HasAnyPropertyFlags(CPF_Parm))
            continue;
        const bool Output = P->HasAnyPropertyFlags(CPF_ReturnParm) ||
                            (P->HasAnyPropertyFlags(CPF_OutParm) && !P->HasAnyPropertyFlags(CPF_ConstParm));
        if (Output != Outputs)
            continue;
        const void* Memory = P->ContainerPtrToValuePtr<void>(Frame);
        if (auto* S = CastField<FStructProperty>(P))
        {
            if (DasIsGeometryType(S->Struct))
            {
                if (!ValidateDasGeometryData(S->Struct, Memory, E, !Outputs && Inverse))
                    return false;
                Geometry = static_cast<const FGeometry*>(Memory);
            }
            else if (S->Struct == TBaseStructure<FVector2D>::Get())
            {
                Coordinate = *static_cast<const FVector2D*>(Memory);
                HasCoordinate = true;
                if (!Vec(Coordinate))
                {
                    E = TEXT("nonfinite or out-of-range Geometry coordinate");
                    return false;
                }
            }
        }
        else if (auto* N = CastField<FNumericProperty>(P); N && N->IsFloatingPoint())
        {
            const double X = N->GetFloatingPointPropertyValue(Memory);
            if (!Fits(X))
            {
                E = TEXT("nonfinite Geometry scalar");
                return false;
            }
            Coordinate = FVector2D(X, 0);
            HasCoordinate = true;
        }
        else if (auto* O = CastField<FObjectPropertyBase>(P))
            WorldObject = O->GetObjectPropertyValue(Memory);
    }
    if (Outputs)
        return true;
    if (Geometry)
    {
        if (Name == TEXT("GetAbsoluteSize") || Name == TEXT("IsUnderLocation"))
        {
            if (!TransformSafe(*Geometry, Geometry->GetLocalSize(), false, Name == TEXT("GetAbsoluteSize"), false, E))
                return false;
        }
        else if (HasCoordinate &&
                 Name != TEXT("ScreenToWidgetLocal") &&
                              !TransformSafe(*Geometry, Coordinate, Inverse,
                                             Name.StartsWith(TEXT("Vector_")) ||
                                                             Name.StartsWith(TEXT("Scalar_")),
                                                                             Name.StartsWith(TEXT("Scalar_")), E))
            return false;
    }
    const bool Viewport = Name == TEXT("LocalToViewport") || Name == TEXT("ScreenToWidgetLocal");
    if (Name == TEXT("GetPlayerScreenWidgetGeometry"))
    {
        auto* PC = Cast<APlayerController>(WorldObject);
        if (!IsValid(PC) || PC->IsActorBeingDestroyed() || !PC->GetWorld() || !PC->GetWorld()->IsGameWorld())
        {
            E = TEXT("Geometry requires a live player controller in a game world");
            return false;
        }
    }
    if (Viewport)
    {
        auto* W = GEngine ? GEngine->GetWorldFromContextObject(WorldObject, EGetWorldErrorMode::ReturnNull) : nullptr;
        if (!W || !W->IsGameWorld())
        {
            E = TEXT("Geometry viewport conversion requires a game world");
            return false;
        }
        // No viewport/layer: preserve documented engine zero/fallback behavior (e.g. headless server).
        if (auto* V = W->GetGameViewport())
            if (auto Layer = V->GetGameLayerManager(); Layer.IsValid())
            {
                const auto& G = Layer->GetViewportWidgetHostGeometry();
                FVector2D Pixels;
                V->GetViewportSize(Pixels);
                const FVector2D Size = G.GetLocalSize();
                if (!ValidateDasGeometryData(FGeometry::StaticStruct(), &G, E, true))
                    return false;
                if (!Vec(Pixels) || Pixels.X <= 0 || Pixels.Y <= 0 || Size.X <= 0 || Size.Y <= 0)
                {
                    E = TEXT("Geometry viewport has zero or invalid dimensions");
                    return false;
                }
                if (Name == TEXT("LocalToViewport") && Geometry)
                {
                    const FVector2D Absolute = Geometry->LocalToAbsolute(Coordinate);
                    if (!TransformSafe(G, Absolute, true, false, false, E))
                        return false;
                    const FVector2D Local = G.AbsoluteToLocal(Absolute);
                    if (!Vec(Local / Size * Pixels))
                    {
                        E = TEXT("Geometry viewport conversion overflows");
                        return false;
                    }
                }
                else if (Name == TEXT("ScreenToWidgetLocal") && Geometry)
                {
                    const FVector2D Local = Size * (Coordinate / Pixels);
                    if (!TransformSafe(G, Local, false, false, false, E))
                        return false;
                    FVector2D Absolute = G.LocalToAbsolute(Local);
                    auto* Include = FindFProperty<FBoolProperty>(F, TEXT("bIncludeWindowPosition"));
                    if (Include && Include->GetPropertyValue_InContainer(Frame))
                        if (auto Window = V->GetWindow())
                            Absolute -= Window->GetPositionInScreen();
                    if (!Vec(Absolute) || !TransformSafe(*Geometry, Absolute, true, false, false, E))
                        return false;
                }
            }
    }
    return true;
}
