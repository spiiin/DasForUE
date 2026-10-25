#pragma once
#include "CoreMinimal.h"
#include "UObject/UnrealType.h"
#include "Curves/CurveFloat.h"
#include "UObject/GarbageCollection.h"
inline bool ComputedGenericAssetContainers(UObject* O)
{
    auto* C = O->GetClass();
    auto* S = FindFProperty<FSetProperty>(C, TEXT("AssetSet"));
    auto* I = FindFProperty<FMapProperty>(C, TEXT("AssetsById"));
    auto* T = FindFProperty<FMapProperty>(C, TEXT("AssetsByText"));
    auto* W = FindFProperty<FMapProperty>(C, TEXT("AssetWeights"));
    if (!S || !I || !T || !W)
        return false;
    auto* SP = CastField<FObjectPropertyBase>(S->ElementProp);
    auto* IP = CastField<FObjectPropertyBase>(I->ValueProp);
    auto* TP = CastField<FObjectPropertyBase>(T->ValueProp);
    auto* WP = CastField<FObjectPropertyBase>(W->KeyProp);
    if (!SP || !IP || !TP || !WP)
        return false;
    FScriptSetHelper Set(S, S->ContainerPtrToValuePtr<void>(O));
    bool Null = false, Asset = false;
    for (int32 N = 0; N < Set.GetMaxIndex(); ++N)
        if (Set.IsValidIndex(N))
        {
            auto* V = SP->GetObjectPropertyValue(Set.GetElementPtr(N));
            Null |= V == nullptr;
            Asset |= V && V->GetName() == TEXT("ArrayAsset");
        }
    if (Set.Num() != 2 || !Null || !Asset)
        return false;
    FScriptMapHelper ById(I, I->ContainerPtrToValuePtr<void>(O));
    int32 Key = 7, N = ById.FindMapIndexWithKey(&Key);
    auto* V = N != INDEX_NONE ? IP->GetObjectPropertyValue(ById.GetValuePtr(N)) : nullptr;
    if (!V || V->GetName() != TEXT("MapAsset"))
        return false;
    FScriptMapHelper ByText(T, T->ContainerPtrToValuePtr<void>(O));
    FString Text = TEXT("main");
    N = ByText.FindMapIndexWithKey(&Text);
    V = N != INDEX_NONE ? TP->GetObjectPropertyValue(ByText.GetValuePtr(N)) : nullptr;
    if (!V || V->GetName() != TEXT("ArrayAsset"))
        return false;
    FScriptMapHelper Weights(W, W->ContainerPtrToValuePtr<void>(O));
    bool GoodAsset = false, GoodNull = false;
    for (int32 J = 0; J < Weights.GetMaxIndex(); ++J)
        if (Weights.IsValidIndex(J))
        {
            V = WP->GetObjectPropertyValue(Weights.GetKeyPtr(J));
            double D = *reinterpret_cast<double*>(Weights.GetValuePtr(J));
            GoodAsset |= V && V->GetName() == TEXT("MapAsset") && D == 2.5;
            GoodNull |= !V && D == 1.0;
        }
    return Weights.Num() == 2 && GoodAsset && GoodNull;
}
inline bool ComputedAssetContainers(UObject* O)
{
    if (!ComputedGenericAssetContainers(O))
        return false;
    auto* A = FindFProperty<FArrayProperty>(O->GetClass(), TEXT("Assets"));
    auto* M = FindFProperty<FMapProperty>(O->GetClass(), TEXT("AssetMap"));
    if (!A || !M)
        return false;
    auto* AP = CastField<FObjectPropertyBase>(A->Inner);
    auto* MP = CastField<FObjectPropertyBase>(M->ValueProp);
    if (!AP || !MP)
        return false;
    FScriptArrayHelper Array(A, A->ContainerPtrToValuePtr<void>(O));
    if (Array.Num() != 2 || AP->GetObjectPropertyValue(Array.GetRawPtr(1)))
        return false;
    auto* Asset = Cast<UCurveFloat>(AP->GetObjectPropertyValue(Array.GetRawPtr(0)));
    if (!Asset || Asset->GetPathName() != TEXT("/Game/DasAssetDefaults/ArrayAsset.ArrayAsset") ||
                                               Asset->GetFloatValue(0) != 17)
        return false;
    FScriptMapHelper Map(M, M->ContainerPtrToValuePtr<void>(O));
    FName Main(TEXT("MAIN")), Null(TEXT("NoneValue"));
    const int32 I = Map.FindMapIndexWithKey(&Main), J = Map.FindMapIndexWithKey(&Null);
    if (Map.Num() != 2 || I == INDEX_NONE || J == INDEX_NONE || MP->GetObjectPropertyValue(Map.GetValuePtr(J)))
        return false;
    Asset = Cast<UCurveFloat>(MP->GetObjectPropertyValue(Map.GetValuePtr(I)));
    return Asset && Asset->GetPathName() == TEXT("/Game/DasAssetDefaults/MapAsset.MapAsset") &&
                                                 Asset->GetFloatValue(0) == 23;
}
inline void ComputedManualAssets(UObject* O)
{
    auto* A = FindFProperty<FArrayProperty>(O->GetClass(), TEXT("Assets"));
    FScriptArrayHelper(A, A->ContainerPtrToValuePtr<void>(O)).EmptyValues();
    auto* M = FindFProperty<FMapProperty>(O->GetClass(), TEXT("AssetMap"));
    FScriptMapHelper(M, M->ContainerPtrToValuePtr<void>(O)).EmptyValues();
}
inline bool ComputedManualAssetsMatch(UObject* O)
{
    auto* A = FindFProperty<FArrayProperty>(O->GetClass(), TEXT("Assets"));
    auto* M = FindFProperty<FMapProperty>(O->GetClass(), TEXT("AssetMap"));
    return A && M && FScriptArrayHelper(A, A->ContainerPtrToValuePtr<void>(O)).Num() == 0 &&
           FScriptMapHelper(M, M->ContainerPtrToValuePtr<void>(O)).Num() == 0;
}

inline FScriptArrayHelper ComputedArray(UObject* O, const TCHAR* Name)
{
    auto* P = FindFProperty<FArrayProperty>(O->GetClass(), Name);
    return FScriptArrayHelper(P, P->ContainerPtrToValuePtr<void>(O));
}
inline int32 ComputedArrayLast(UObject* O, const TCHAR* Name)
{
    auto A = ComputedArray(O, Name);
    return A.Num() ? *reinterpret_cast<int32*>(A.GetRawPtr(A.Num() - 1)) : MIN_int32;
}
inline void ComputedArrayManual(UObject* O, const TCHAR* Name)
{
    auto A = ComputedArray(O, Name);
    A.EmptyValues();
    const int32 I = A.AddValue();
    *reinterpret_cast<int32*>(A.GetRawPtr(I)) = 777;
}
inline bool ComputedContainers(UObject* O)
{
    auto* C = O->GetClass();
    auto* M = FindFProperty<FMapProperty>(C, TEXT("ScoresDefault"));
    auto* S = FindFProperty<FSetProperty>(C, TEXT("TagsDefault"));
    auto* E = FindFProperty<FMapProperty>(C, TEXT("EnumDefault"));
    auto* N = FindFProperty<FMapProperty>(C, TEXT("NamesDefault"));
    auto* V = FindFProperty<FArrayProperty>(C, TEXT("VectorsDefault"));
    if (!M || !S || !E || !N || !V || !ComputedAssetContainers(O))
        return false;
    auto Ratios = ComputedArray(O, TEXT("Ratios"));
    if (Ratios.Num() != 1 || *reinterpret_cast<double*>(Ratios.GetRawPtr(0)) != 1.2345678901234567)
        return false;
    FScriptMapHelper Map(M, M->ContainerPtrToValuePtr<void>(O));
    FString Key = TEXT("one");
    int32 I = Map.FindMapIndexWithKey(&Key);
    if (I == INDEX_NONE || *reinterpret_cast<int32*>(Map.GetValuePtr(I)) != 8)
        return false;
    FScriptSetHelper Set(S, S->ContainerPtrToValuePtr<void>(O));
    FString Tag = TEXT("8");
    if (Set.Num() != 3 || Set.FindElementIndex(&Tag) == INDEX_NONE)
        return false;
    FScriptMapHelper Enums(E, E->ContainerPtrToValuePtr<void>(O));
    uint8 EnumKey = 1;
    I = Enums.FindMapIndexWithKey(&EnumKey);
    if (I == INDEX_NONE || *reinterpret_cast<int32*>(Enums.GetValuePtr(I)) != 8)
        return false;
    FScriptMapHelper Names(N, N->ContainerPtrToValuePtr<void>(O));
    FName Name(TEXT("ITEM_3"));
    I = Names.FindMapIndexWithKey(&Name);
    if (I == INDEX_NONE || *reinterpret_cast<int32*>(Names.GetValuePtr(I)) != 5)
        return false;
    FScriptArrayHelper Vectors(V, V->ContainerPtrToValuePtr<void>(O));
    return Vectors.Num() == 1 && *reinterpret_cast<FVector*>(Vectors.GetRawPtr(0)) == FVector(5, 8, 3);
}


inline void ComputedManualHash(UObject* O)
{
    ComputedManualAssets(O);
    auto* M = FindFProperty<FMapProperty>(O->GetClass(), TEXT("ScoresDefault"));
    FScriptMapHelper Map(M, M->ContainerPtrToValuePtr<void>(O));
    Map.EmptyValues();
    FString Key = TEXT("manual");
    int32 Value = 777;
    Map.AddPair(&Key, &Value);
    auto* S = FindFProperty<FSetProperty>(O->GetClass(), TEXT("TagsDefault"));
    FScriptSetHelper Set(S, S->ContainerPtrToValuePtr<void>(O));
    Set.EmptyElements();
    Set.AddElement(&Key);
}
inline bool ComputedManualHashMatches(UObject* O)
{
    auto* M = FindFProperty<FMapProperty>(O->GetClass(), TEXT("ScoresDefault"));
    FScriptMapHelper Map(M, M->ContainerPtrToValuePtr<void>(O));
    FString Key = TEXT("manual");
    int32 I = Map.FindMapIndexWithKey(&Key);
    auto* S = FindFProperty<FSetProperty>(O->GetClass(), TEXT("TagsDefault"));
    FScriptSetHelper Set(S, S->ContainerPtrToValuePtr<void>(O));
    return ComputedManualAssetsMatch(O) && Map.Num() == 1 && I != INDEX_NONE &&
           *reinterpret_cast<int32*>(Map.GetValuePtr(I)) == 777 && Set.Num() == 1 &&
           Set.FindElementIndex(&Key) != INDEX_NONE;
}
