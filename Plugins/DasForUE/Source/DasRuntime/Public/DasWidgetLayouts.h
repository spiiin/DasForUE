#pragma once
#include "CoreMinimal.h"
#include "UObject/WeakObjectPtr.h"
class UWidget;
class UWidgetTree;
class UUserWidget;
struct FDasResolvedWidgetLayout
{
    TWeakObjectPtr<UUserWidget> Owner;
    TMap<FName, TWeakObjectPtr<UWidget>> Fields;
};
// Syntax-neutral layout contracts shared by the DSL frontend and editor preflight.
struct FDasWidgetLayoutUse
{
    FString Name, WidgetField, ClassField, KnownClassPath;
    TMap<FString, FString> Fields;
    bool operator==(const FDasWidgetLayoutUse& O) const
    {
        return Name == O.Name && WidgetField == O.WidgetField && ClassField == O.ClassField &&
               KnownClassPath == O.KnownClassPath && Fields.OrderIndependentCompareEqual(O.Fields);
    }
};
DASRUNTIME_API bool ValidateDasWidgetLayout(UWidgetTree* Tree, const FDasWidgetLayoutUse& Layout, FString& Error,
                                            FDasResolvedWidgetLayout* Resolved = nullptr);
DASRUNTIME_API bool ParseDasWidgetLayout(const FString& Spec, FDasWidgetLayoutUse& Layout, FString& Error);
DASRUNTIME_API FString DasWidgetLayoutSpec(const FDasWidgetLayoutUse& Layout);
