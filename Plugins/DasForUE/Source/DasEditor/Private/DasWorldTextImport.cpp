#include "Editor.h"
#include "AssetCompilingManager.h"
#include "BSPOps.h"
#include "Engine/Brush.h"
#include "Engine/Level.h"
#include "Engine/Texture.h"
#include "Engine/AssetUserData.h"
#include "Components/SceneComponent.h"
#include "Engine/World.h"
#include "HAL/IConsoleManager.h"
#include "Misc/FileHelper.h"
#include "Misc/PackageName.h"
#include "ScopedTransaction.h"
#include "UObject/UObjectHash.h"
#include "UObject/MetaData.h"

// The legacy level factory imports each actor in one pass. Multi-pass creation
// resolves every embedded object (textures, materials, components) before any
// properties or editor callbacks can use them. No clipboard/world replacement.
static FAutoConsoleCommand DasWorldImportText(
    TEXT("Das.World.ImportText"), TEXT("Das.World.ImportText native-scene.t3d (current editor level; dirty, no save)"),
    FConsoleCommandWithArgsDelegate::CreateLambda([](const TArray<FString>& Args)
    {
        UWorld* World = GEditor ? GEditor->GetEditorWorldContext().World() : nullptr;
        FString Text;
        if (Args.Num() != 1 || !World || GEditor->PlayWorld || World->IsGameWorld() ||
            !FFileHelper::LoadFileToString(Text, *Args[0]) || !Text.TrimStart().StartsWith(TEXT("Begin Map")))
        {
            UE_LOG(LogTemp, Warning, TEXT("DAS_WORLD_IMPORT rejected: expected native T3D actors and an idle editor world"));
            return;
        }
        struct FActorText { FString Header; FString Body; AActor* Actor = nullptr; };
        TArray<FActorText> Rows;
        const TCHAR* Cursor = *Text;
        FString Line;
        FActorText* Current = nullptr;
        while (FParse::Line(&Cursor, Line))
        {
            const FString Trimmed = Line.TrimStartAndEnd();
            if (Trimmed.StartsWith(TEXT("Begin Actor ")))
            {
                if (Current)
                    return;
                Current = &Rows.AddDefaulted_GetRef();
                Current->Header = Trimmed;
            }
            else if (Trimmed == TEXT("End Actor"))
                Current = nullptr;
            else if (Current)
                Current->Body += Line + TEXT("\n");
        }
        ULevel* Level = World->GetCurrentLevel();
        const FScopedTransaction Transaction(NSLOCTEXT("DasForUE", "ImportScene", "Import native scene from text"));
        Level->Modify();
        World->GetPackage()->GetMetaData().SetValue(World, TEXT("DasWorldImportComplete"), TEXT("0"));
        TMap<FSoftObjectPath, UObject*> Remapper;
        for (auto& Row : Rows)
        {
            FString ClassPath, Name, ExportPath;
            FParse::Value(*Row.Header, TEXT("Class="), ClassPath);
            FParse::Value(*Row.Header, TEXT("Name="), Name);
            FParse::Value(*Row.Header, TEXT("ExportPath="), ExportPath);
            UClass* Class = ClassPath.StartsWith(TEXT("/Script/")) ? LoadObject<UClass>(nullptr, *ClassPath) : nullptr;
            if (!Class || !Class->IsChildOf(AActor::StaticClass()) || Class->HasAnyClassFlags(CLASS_Abstract))
            {
                UE_LOG(LogTemp, Warning, TEXT("DAS_WORLD_IMPORT rejected: expected concrete native Actor class %s"), *ClassPath);
                return;
            }
            FActorSpawnParameters Params;
            Params.Name = FName(*Name);
            // Destroyed actors can retain a reserved UAID until GC. The export
            // remapper preserves references even when a repeated import needs a
            // fresh internal name; ActorLabel still comes from the source text.
            if (StaticFindObjectFast(UObject::StaticClass(), Level, Params.Name))
                Params.Name = MakeUniqueObjectName(Level, Class, FName(*Class->GetName()));
            Params.OverrideLevel = Level;
            Params.ObjectFlags = RF_Transactional;
            Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
            Row.Actor = World->SpawnActor<AActor>(Class, FTransform::Identity, Params);
            if (!Row.Actor)
                return;
            if (!ExportPath.IsEmpty())
                Remapper.Add(FSoftObjectPath(FPackageName::ExportTextPathToObjectPath(ExportPath)), Row.Actor);
        }
        auto ParamsFor = [&Remapper](FActorText& Row)
        {
            EditorUtilities::FMultiStepsImportObjectParams Params;
            Params.DestData = reinterpret_cast<uint8*>(Row.Actor);
            Params.SourceText = Row.Body;
            Params.ObjectStruct = Row.Actor->GetClass();
            Params.SubobjectRoot = Row.Actor;
            Params.SubobjectOuter = Row.Actor;
            Params.ObjectRemapper = &Remapper;
            Params.Warn = GWarn;
            Params.bShouldCallEditChange = false;
            return Params;
        };
        for (auto& Row : Rows)
        {
            if (Row.Actor->IsA<ABrush>())
                continue; // Native brush text has its own legacy polygon importer.
            auto Params = ParamsFor(Row);
            if (!EditorUtilities::ImportCreateObjectsStep(Params))
            {
                UE_LOG(LogTemp, Warning, TEXT("DAS_WORLD_IMPORT rejected: creating subobjects of %s"), *Row.Actor->GetName());
                return;
            }
        }
        // FProperty's text remapper only handles standalone object values;
        // native arrays/nested structs also contain exported subobject paths.
        // Replace actor prefixes after all subobjects exist, preserving suffixes.
        for (auto& Row : Rows)
            for (const auto& Pair : Remapper)
                if (Pair.Value && Pair.Value->IsA<AActor>())
                    Row.Body.ReplaceInline(*Pair.Key.ToString(), *Pair.Value->GetPathName(), ESearchCase::CaseSensitive);
        for (auto& Row : Rows)
        {
            if (Row.Actor->IsA<ABrush>())
            {
                FImportObjectParams Params;
                Params.DestData = reinterpret_cast<uint8*>(Row.Actor);
                Params.SourceText = *Row.Body;
                Params.ObjectStruct = Row.Actor->GetClass();
                Params.SubobjectRoot = Row.Actor;
                Params.SubobjectOuter = Row.Actor;
                Params.Warn = GWarn;
                Params.ObjectRemapper = &Remapper;
                Params.bShouldCallEditChange = false;
                if (!ImportObjectProperties(Params))
                {
                    UE_LOG(LogTemp, Warning, TEXT("DAS_WORLD_IMPORT rejected: importing brush %s"), *Row.Actor->GetName());
                    return;
                }
                continue;
            }
            auto Params = ParamsFor(Row);
            if (!EditorUtilities::ImportObjectsPropertiesStep(Params))
            {
                UE_LOG(LogTemp, Warning, TEXT("DAS_WORLD_IMPORT rejected: importing properties of %s"), *Row.Actor->GetName());
                return;
            }
        }
        for (auto& Row : Rows)
            ForEachObjectWithOuter(Row.Actor, [](UObject* Object)
            {
                if (UTexture* Texture = Cast<UTexture>(Object))
                {
                    // Provider payloads and owning-texture pointers use Serialize,
                    // not reflected properties. T3D only restores TextureSourceData.
                    // Discard these incomplete caches; Landscape regenerates them
                    // from the restored source when saving/cooking the level.
                    TArray<UClass*> ImportedCaches;
                    if (const auto* UserData = Texture->GetAssetUserDataArray())
                        for (const UAssetUserData* Data : *UserData)
                            if (Data && Data->GetClass()->GetPathName().StartsWith(TEXT("/Script/Landscape.")))
                                ImportedCaches.Add(Data->GetClass());
                    for (UClass* Class : ImportedCaches)
                        Texture->RemoveUserDataOfClass(Class);
                    Texture->PostEditChange();
                }
            }, EGetObjectsFlags::IncludeNestedObjects);
        FAssetCompilingManager::Get().FinishAllCompilation();
        // Reflection imports relative transforms without SceneComponent setters.
        // Refresh every cached world transform before Landscape registers peers.
        for (auto& Row : Rows)
        {
            TArray<UObject*> Objects;
            GetObjectsWithOuter(Row.Actor, Objects);
            for (UObject* Object : Objects)
                if (USceneComponent* Component = Cast<USceneComponent>(Object))
                {
                    Component->PostEditImport();
                    Component->PostEditChange();
                    Component->UpdateComponentToWorld();
                }
            Row.Actor->UpdateComponentTransforms();
        }
        for (auto& Row : Rows)
        {
            if (ABrush* Brush = Cast<ABrush>(Row.Actor); Brush && Brush->Brush)
                FBSPOps::RebuildBrush(Brush->Brush);
            Row.Actor->PostEditImport();
            Row.Actor->PostEditChange();
            Row.Actor->ReregisterAllComponents();
        }
        World->GetPackage()->GetMetaData().SetValue(World, TEXT("DasWorldImportComplete"), TEXT("1"));
        World->MarkPackageDirty();
        UE_LOG(LogTemp, Display, TEXT("DAS_WORLD_IMPORT PASS actors=%d (dirty; save in editor)"), Rows.Num());
    }));

#include "AssetRegistry/AssetRegistryModule.h"
#include "Curves/CurveFloat.h"
#include "Misc/PackageName.h"
#include "UObject/Package.h"

// Python deliberately hides inline rich-curve storage. Preserve the complete
// native struct text, including interpolation/tangents, instead of resampling it.
static FAutoConsoleCommand DasCurveImportText(
    TEXT("Das.Curve.ImportText"), TEXT("Das.Curve.ImportText /Game/Curve runtime-curve.txt (dirty, no save)"),
    FConsoleCommandWithArgsDelegate::CreateLambda([](const TArray<FString>& Args)
    {
        FString Text;
        if (Args.Num() != 2 || !GEditor || GEditor->PlayWorld || !Args[0].StartsWith(TEXT("/Game/")) ||
            !FPackageName::IsValidLongPackageName(Args[0]) || !FFileHelper::LoadFileToString(Text, *Args[1]))
        {
            UE_LOG(LogTemp, Warning, TEXT("DAS_CURVE_IMPORT rejected: expected an editor curve path and struct text"));
            return;
        }
        FRuntimeFloatCurve Parsed;
        if (!FRuntimeFloatCurve::StaticStruct()->ImportText(*Text, &Parsed, nullptr, PPF_None, GWarn,
                TEXT("Das.Curve.ImportText")))
        {
            UE_LOG(LogTemp, Warning, TEXT("DAS_CURVE_IMPORT rejected: invalid RuntimeFloatCurve text"));
            return;
        }
        for (const auto& Key : Parsed.GetRichCurveConst()->GetCopyOfKeys())
            if (!FMath::IsFinite(Key.Time) || !FMath::IsFinite(Key.Value))
            {
                UE_LOG(LogTemp, Warning, TEXT("DAS_CURVE_IMPORT rejected: non-finite curve key"));
                return;
            }
        const FString Name = FPackageName::GetLongPackageAssetName(Args[0]);
        const FString ObjectPath = Args[0] + TEXT(".") + Name;
        UObject* Existing = LoadObject<UObject>(nullptr, *ObjectPath, nullptr, LOAD_NoWarn);
        if (Existing && !Existing->IsA<UCurveFloat>())
        {
            UE_LOG(LogTemp, Warning, TEXT("DAS_CURVE_IMPORT rejected: existing asset is not a CurveFloat"));
            return;
        }
        UPackage* Package = Existing ? Existing->GetOutermost() : CreatePackage(*Args[0]);
        UCurveFloat* Curve = Existing ? CastChecked<UCurveFloat>(Existing) :
            NewObject<UCurveFloat>(Package, *Name, RF_Public | RF_Standalone | RF_Transactional);
        Curve->Modify();
        Curve->FloatCurve = *Parsed.GetRichCurveConst();
        Curve->PostEditChange();
        if (!Existing)
            FAssetRegistryModule::AssetCreated(Curve);
        Package->MarkPackageDirty();
        UE_LOG(LogTemp, Display, TEXT("DAS_CURVE_IMPORT PASS %s (dirty; save in editor)"), *Curve->GetPathName());
    }));
