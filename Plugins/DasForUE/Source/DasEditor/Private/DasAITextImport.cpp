#include "AssetRegistry/AssetRegistryModule.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardData.h"
#include "EnvironmentQuery/EnvQuery.h"
#include "Editor.h"
#include "HAL/IConsoleManager.h"
#include "Misc/FileHelper.h"
#include "Misc/PackageName.h"
#include "UObject/Package.h"

// Editor-only, explicit creation of native AI assets from UE's own text export.
// No text or JSON is parsed by the packaged game. Existing assets are never overwritten.
static FAutoConsoleCommand DasAIImportText(TEXT("Das.AI.ImportText"),
    TEXT("Das.AI.ImportText class-path /Game/NewPackage source.t3d (new dirty asset; no save/overwrite)"),
         FConsoleCommandWithArgsDelegate::CreateLambda(
             [](const TArray<FString>& Args)
             {
                 FString Text;
                 if (Args.Num() != 3 || GEditor->PlayWorld ||
                     !Args[1].StartsWith(TEXT("/Game/")) || !FPackageName::IsValidLongPackageName(Args[1]) ||
                                         FPackageName::DoesPackageExist(Args[1]) || FindPackage(nullptr, *Args[1]) ||
                                         !FFileHelper::LoadFileToString(Text, *Args[2]))
                 {
                     UE_LOG(LogTemp, Warning,
                            TEXT("DAS_AI_IMPORT rejected: expected a new /Game package, readable export and no PIE"));
                     return;
                 }
                 UClass* Class = LoadObject<UClass>(nullptr, *Args[0]);
                 if (Class != UBehaviorTree::StaticClass() && Class != UBlackboardData::StaticClass() &&
                     Class != UEnvQuery::StaticClass())
                 {
                     UE_LOG(
                         LogTemp, Warning,
                         TEXT("DAS_AI_IMPORT rejected: supported roots are BehaviorTree, BlackboardData and EnvQuery"));
                     return;
                 }
                 FString Header, Body;
                 if (!Text.Split(TEXT("\n"), &Header, &Body) ||
                                 !Header.StartsWith(TEXT("Begin Object Class=")+ Args[0] + TEXT(" ")))
                 {
                     UE_LOG(LogTemp, Warning, TEXT("DAS_AI_IMPORT rejected: mismatched export header"));
                     return;
                 }
                 auto* Package = CreatePackage(*Args[1]);
                 auto* Asset = NewObject<UObject>(Package, Class, *FPackageName::GetLongPackageAssetName(Args[1]),
                                                  RF_Public | RF_Standalone);
                 TMap<FSoftObjectPath, UObject*> Remapper;
                 EditorUtilities::FMultiStepsImportObjectParams Params;
                 Params.DestData = reinterpret_cast<uint8*>(Asset);
                 Params.SourceText = Body;
                 Params.ObjectRemapper = &Remapper;
                 Params.ObjectStruct = Class;
                 Params.SubobjectRoot = Asset;
                 Params.SubobjectOuter = Asset;
                 Params.Warn = GWarn;
                 // A graph must not rebuild/prune its owner while sibling nodes are still being imported.
                 Params.bShouldCallEditChange = false;
                 if (!EditorUtilities::ImportCreateObjectsStep(Params) ||
                     !EditorUtilities::ImportObjectsPropertiesStep(Params))
                 {
                     Asset->ClearFlags(RF_Public | RF_Standalone);
                     Asset->MarkAsGarbage();
                     UE_LOG(LogTemp, Warning, TEXT("DAS_AI_IMPORT rejected: invalid object properties"));
                     return;
                 }
                 Asset->PostEditChange();
                 FAssetRegistryModule::AssetCreated(Asset);
                 Package->MarkPackageDirty();
                 UE_LOG(LogTemp, Display, TEXT("DAS_AI_IMPORT PASS %s (dirty; save in editor)"), *Asset->GetPathName());
             }));
