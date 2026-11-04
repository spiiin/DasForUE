#include "WidgetBlueprint.h"
#include "Blueprint/WidgetTree.h"
#include "Components/Widget.h"
#include "Editor.h"
#include "HAL/IConsoleManager.h"
#include "Kismet2/BlueprintEditorUtils.h"

// WidgetTree.RootWidget is intentionally not writable through the Python bridge.
// The tree's widgets/slots are ordinary editor objects; this supplies its missing root operation.
static FAutoConsoleCommand DasUISetRoot(TEXT("Das.UI.SetRoot"),
    TEXT("Das.UI.SetRoot /Game/UI/WBP_Name RootWidgetName (editor only; dirty, no save)"),
        FConsoleCommandWithArgsDelegate::CreateLambda(
            [](const TArray<FString>& Args)
            {
                auto* BP = Args.Num() == 2 && Args[0].StartsWith(TEXT("/Game/"))
                                                                 ? LoadObject<UWidgetBlueprint>(nullptr, *Args[0])
                                                                 : nullptr;
                auto* Tree = BP ? BP->WidgetTree.Get() : nullptr;
                auto* Root = Tree ? FindObject<UWidget>(Tree, *Args[1]) : nullptr;
                if (GEditor->PlayWorld || !Tree || !Root || Root->GetOuter() != Tree || Root->GetParent() ||
                    (Tree->RootWidget && Tree->RootWidget != Root && Tree->RootWidget->GetParent() != Root))
                {
                    UE_LOG(
                        LogTemp, Warning,
                        TEXT("DAS_UI_SET_ROOT rejected: expected an unparented widget in an empty, matching or directly wrapped editor WidgetTree"));
                    return;
                }
                BP->Modify();
                Tree->Modify();
                Tree->RootWidget = Root;
                FBlueprintEditorUtils::MarkBlueprintAsStructurallyModified(BP);
                BP->MarkPackageDirty();
                UE_LOG(LogTemp, Display, TEXT("DAS_UI_SET_ROOT PASS %s %s"), *BP->GetPathName(), *Root->GetName());
            }));
