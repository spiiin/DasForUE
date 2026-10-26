// Reload matrix: every declaration change of every entity kind applied with the automatic Reimport, in the
// editor and during PIE. Each cell ends migrated, lossy (applied but a value was lost), refused (nothing
// changed) or broken (an invariant failed). Outcomes are compared with Tests/ReloadMatrix/expected.json and
// written to logs/reload-matrix.json; Scripts/RenderReloadMatrix.py turns the expected file into
// Docs/Reference/ReloadMatrix.md.
#include "DasDSLService.h"
#include "DasDataAssetService.h"
#include "Engine/DataAsset.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "UObject/SavePackage.h"
#include "DasProbeInterface.h"
#include "DasRuntime.h"
#include "Dom/JsonObject.h"
#include "Editor.h"
#include "Engine/Blueprint.h"
#include "Engine/BlueprintGeneratedClass.h"
#include "Engine/SCS_Node.h"
#include "Engine/SimpleConstructionScript.h"
#include "Kismet2/KismetEditorUtilities.h"
#include "EngineUtils.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerController.h"
#include "HAL/FileManager.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"
#include "UObject/StrongObjectPtr.h"
#if WITH_DEV_AUTOMATION_TESTS
#include "Misc/AutomationTest.h"
#include "Tests/AutomationEditorCommon.h"
namespace DasReloadMatrix
{
struct FMatrixType
{
    const TCHAR* Name;
    const TCHAR* Require;
    const TCHAR* Type;
    const TCHAR* Default;
    const TCHAR* Default2;
    // Property text of an instance override (ImportText form).
    const TCHAR* Override;
    const TCHAR* RetypeType;
    const TCHAR* RetypeDefault;
};
// clang-format off
const FMatrixType MatrixTypes[] = {
    {TEXT("int"), nullptr, TEXT("int"), TEXT("10"), TEXT("20"), TEXT("77"), TEXT("float"), TEXT("10.0f")},
    {TEXT("float"), nullptr, TEXT("float"), TEXT("1.5f"), TEXT("2.5f"), TEXT("7.5"), TEXT("double"), TEXT("1.5lf")},
    {TEXT("bool"), nullptr, TEXT("bool"), TEXT("false"), TEXT("true"), TEXT("True"), TEXT("int"), TEXT("0")},
    {TEXT("string"), nullptr, TEXT("string"), TEXT("\"a\""), TEXT("\"b\""), TEXT("\"over\""), TEXT("UENameValue"),
     TEXT("ue_name_value(\"a\")")},
    {TEXT("name"), nullptr, TEXT("UENameValue"), TEXT("ue_name_value(\"a\")"), TEXT("ue_name_value(\"b\")"),
     TEXT("Over"), TEXT("string"), TEXT("\"a\"")},
    {TEXT("text"), TEXT("ue_text"), TEXT("FText"), TEXT("ue_text_invariant(\"a\")"),
     TEXT("ue_text_invariant(\"b\")"), TEXT("INVTEXT(\"over\")"), TEXT("string"), TEXT("\"a\"")},
    {TEXT("byte"), nullptr, TEXT("uint8"), TEXT("uint8(3)"), TEXT("uint8(4)"), TEXT("9"), TEXT("int"), TEXT("3")},
    {TEXT("vector"), nullptr, TEXT("UEVector"), TEXT("ue_vector(1.0lf,2.0lf,3.0lf)"),
     TEXT("ue_vector(4.0lf,5.0lf,6.0lf)"), TEXT("(X=7.000000,Y=7.000000,Z=7.000000)"), TEXT("UERotator"),
     TEXT("ue_rotator(1.0lf,2.0lf,3.0lf)")},
    {TEXT("soft"), TEXT("ue_pickup_components"), TEXT("SoftStaticMesh"),
     TEXT("ue_soft(type<SoftStaticMesh>,\"/Engine/BasicShapes/Sphere.Sphere\")"),
     TEXT("ue_soft(type<SoftStaticMesh>,\"/Engine/BasicShapes/Cone.Cone\")"), TEXT("/Engine/BasicShapes/Cube.Cube"),
     TEXT("string"), TEXT("\"a\"")},
    {TEXT("array"), nullptr, TEXT("array<int>"), TEXT("[1,2]"), TEXT("[3]"), TEXT("(7,8)"), TEXT("array<float>"),
     TEXT("[1.0f]")},
    {TEXT("set"), nullptr, TEXT("table<string>"), TEXT("{\"a\"}"), TEXT("{\"b\"}"), TEXT("(\"o\")"), TEXT("table<int>"),
     TEXT("{1}")},
    {TEXT("map"), nullptr, TEXT("table<string;int>"), TEXT("{\"a\"=>1}"), TEXT("{\"b\"=>2}"), TEXT("((\"o\", 7))"),
     TEXT("table<string;float>"), TEXT("{\"a\"=>1.0f}")},
};
// clang-format on
struct FMatrixKind
{
    const TCHAR* Name;
    const TCHAR* Parent;
    const TCHAR* Require;
    const TCHAR* Members;
};
// clang-format off
const FMatrixKind MatrixKinds[] = {
    {TEXT("actor"), TEXT("/Script/Engine.Actor"), nullptr, TEXT("")},
    {TEXT("composed"), TEXT("/Script/Engine.Actor"), TEXT("ue_scene_components"),
     TEXT("    @ue_component\n    @RootComponent\n    Pivot:SceneComponentRef\n")},
    {TEXT("pawn"), TEXT("/Script/Engine.Pawn"), nullptr, TEXT("")},
        {TEXT("character"), TEXT("/Script/Engine.Character"), nullptr, TEXT("")},
        {TEXT("child"), nullptr, nullptr, TEXT("")},
    // Instances are objects, not actors: saved DataAsset packages and an unsaved SaveGame object.
    {TEXT("data_asset"), TEXT("DataAsset"), nullptr, TEXT("")},
    {TEXT("primary_data_asset"), TEXT("PrimaryDataAsset"), nullptr, TEXT("")},
    {TEXT("save_game"), TEXT("SaveGame"), TEXT("ue_savegame"), TEXT("")},
    // An authored component inside an actor Blueprint (BP_Owner): instances are the components of placed owners.
    {TEXT("component"), TEXT("ActorComponent"), nullptr, TEXT("")},
    {TEXT("evented"), TEXT("/Script/Engine.Actor"), TEXT("ue_delegate_examples"),
     TEXT("    @ue_property=(BlueprintAssignable)\n    @ue_id=\"reloadmatrix.host.changed\"\n    Changed:FScoreEvent\n"
          "    @ue_property=(BlueprintReadWrite)\n    @ue_id=\"reloadmatrix.host.query\"\n    Query:FScoreQuery\n")},
};
// clang-format on
enum class EChange
{
    AddField,
    RemoveField,
    RenameField,
    RetypeField,
    ChangeDefault,
    MakeReadOnly,
    AddMethod,
    RemoveMethod,
    ChangeSignature,
    AddComponent,
    RemoveComponent,
    RenameComponent,
    ChangeComponentDefault,
    ChangeAttach,
    AddEvent,
    RemoveEvent,
    AddInterface,
    ParentAddField,
    Reparent,
};
const TCHAR* ChangeName(EChange C)
{
    // clang-format off
    static const TCHAR* Names[] = {TEXT("add_field"), TEXT("remove_field"), TEXT("rename_field"),
                                   TEXT("retype_field"), TEXT("change_default"), TEXT("make_read_only"),
                                   TEXT("add_method"), TEXT("remove_method"), TEXT("change_signature"),
                                   TEXT("add_component"), TEXT("remove_component"), TEXT("rename_component"),
                                   TEXT("change_component_default"), TEXT("change_attach"),
                                   TEXT("add_event"), TEXT("remove_event"), TEXT("add_interface"),
                                   TEXT("parent_add_field"), TEXT("reparent")};
    // clang-format on
    return Names[int32(C)];
}
bool FieldChange(EChange C)
{
    return C <= EChange::MakeReadOnly;
}
bool ComponentChange(EChange C)
{
    return C >= EChange::AddComponent && C <= EChange::ChangeAttach;
}
bool EventChange(EChange C)
{
    return C == EChange::AddEvent || C == EChange::RemoveEvent;
}
bool Evented(const FMatrixKind& K)
{
    return FCString::Strcmp(K.Name, TEXT("evented")) == 0;
}
bool HierarchyChange(EChange C)
{
    return C == EChange::ParentAddField || C == EChange::Reparent;
}
bool Child(const FMatrixKind& K)
{
    return FCString::Strcmp(K.Name, TEXT("child")) == 0;
}
// The authored parents of the child kind: MatrixBase (with Added for parent_add_field) and MatrixOther (the
// reparent target), one module per cell.
FString ParentScript(const TCHAR* Class, const TCHAR* Module, const FString& Suffix, bool bAdded)
{
    FString S = FString::Printf(TEXT("options gen2\nmodule %s_%s public\nrequire ue_dsl public\n"
                                     "[ue_class(module_id=\"reloadmatrixbase\",id=\"reloadmatrix.%s\",Blueprintable)]\n"
                                     "class %s : Actor {\n"
                                     "    @ue_property=(EditAnywhere,BlueprintReadWrite)\n"
                                     "    @ue_id=\"reloadmatrix.%s.basevalue\"\n    BaseValue:int=7\n"),
                                     Module, *Suffix, Class, Class, Class);
    if (bAdded)
        S += FString::Printf(
            TEXT("    @ue_property=(EditAnywhere,BlueprintReadWrite)\n    @ue_id=\"reloadmatrix.%s.added\"\n"
                                  "    Added:int=9\n"),
                 Class);
    return S + FString::Printf(TEXT("    [ue_function(id=\"reloadmatrix.%s.readbase\",BlueprintCallable)]\n"
                                    "    def ReadBase():int { return self.BaseValue }\n}\n"),
                                    Class);
}
bool AssetKind(const FMatrixKind& K)
{
    return K.Parent && !FString(K.Parent).StartsWith(TEXT("/Script/"));
}
bool ComponentKind(const FMatrixKind& K)
{
    return FCString::Strcmp(K.Name, TEXT("component")) == 0;
}
bool DataAssetKind(const FMatrixKind& K)
{
    return AssetKind(K) && FString(K.Parent).EndsWith(TEXT("DataAsset"));
}
bool Composed(const FMatrixKind& K)
{
    return FCString::Strcmp(K.Name, TEXT("composed")) == 0;
}
// The evented kind's event and delegate, with the event changes applied.
FString EventedMembers(const FMatrixKind& K, TOptional<EChange> Change)
{
    FString S = K.Members;
    if (!Evented(K) || !Change.IsSet())
        return S;
    if (*Change == EChange::RemoveEvent)
        S = S.Replace(TEXT("    @ue_property=(BlueprintAssignable)\n    @ue_id=\"reloadmatrix.host.changed\"\n    Changed:FScoreEvent\n"),
            TEXT(""));
    if (*Change == EChange::AddEvent)
        S += TEXT("    @ue_property=(BlueprintAssignable)\n    @ue_id=\"reloadmatrix.host.extra_event\"\n"
                  "    ExtraEvent:FScoreEvent\n");
    return S;
}
// Pivot (root), Child and Other attached to it; Child is the one the component changes touch.
FString ComposedMembers(TOptional<EChange> Change)
{
    auto Is = [&](EChange C)
    {
        return Change.IsSet() && *Change == C;
    };
    const TCHAR* Child = Is(EChange::RenameComponent) ? TEXT("Arm") : TEXT("Child");
    FString S = TEXT("    @ue_component\n    @RootComponent\n    Pivot:SceneComponentRef\n");
    if (!Is(EChange::RemoveComponent))
        S += FString::Printf(TEXT("    @ue_component\n    @Attach=\"%s\"\n    @ue_id=\"reloadmatrix.host.child\"\n"
                                  "    %s:SceneComponentRef\n"),
                                  Is(EChange::ChangeAttach) ? TEXT("Other") : TEXT("Pivot"), Child);
    S += TEXT("    @ue_component\n    @Attach=\"Pivot\"\n    @ue_id=\"reloadmatrix.host.other\"\n"
              "    Other:SceneComponentRef\n");
    if (Is(EChange::AddComponent))
        S += TEXT("    @ue_component\n    @Attach=\"Pivot\"\n    Extra:SceneComponentRef\n");
    S += TEXT("    defaults=ue_defaults() {\n");
    if (!Is(EChange::RemoveComponent))
        S += FString::Printf(TEXT("        self.%s.RelativeLocation=ue_vector(0.0lf,0.0lf,%s)\n"), Child,
                                  Is(EChange::ChangeComponentDefault) ? TEXT("60.0lf") : TEXT("50.0lf"));
    return S + TEXT("        self.Other.RelativeLocation=ue_vector(0.0lf,0.0lf,30.0lf)\n    }\n");
}

// The declaration of one cell before (Change unset) or after its change.
FString MatrixScript(const FMatrixKind& K, const FMatrixType& T, TOptional<EChange> Change, const FString& Suffix)
{
    const bool V2 = Change.IsSet();
    auto Is = [&](EChange C)
    {
        return V2 && *Change == C;
    };
    FString S = TEXT("options gen2\nrequire ue_dsl\n");
    for (const TCHAR* R : {K.Require, T.Require})
        if (R)
            S += FString::Printf(TEXT("require %s\n"), R);
    if (Child(K))
        S += Is(EChange::Reparent)
            ? FString::Printf(TEXT("require reload_matrix_other_%s\n[ue_class(module_id=\"reloadmatrix\","
                                        "id=\"reloadmatrix.host\",Blueprintable)]\nclass MatrixHost : MatrixOther {\n"),
                                   *Suffix)
            : FString::Printf(TEXT("require reload_matrix_base_%s\n[ue_class(module_id=\"reloadmatrix\","
                                        "id=\"reloadmatrix.host\",Blueprintable)]\nclass MatrixHost : MatrixBase {\n"),
                                   *Suffix);
    else if (AssetKind(K))
        S += FString::Printf(TEXT("[ue_class(module_id=\"reloadmatrix\",id=\"reloadmatrix.host\",Blueprintable)]\n"
                                  "class MatrixHost : %s {\n"),
                                  K.Parent);
    else
        S += FString::Printf(TEXT("[ue_class(module_id=\"reloadmatrix\",id=\"reloadmatrix.host\",parent=\"%s\","
                              "Blueprintable)%s]\nclass MatrixHost {\n%s"),
                                  K.Parent,
                                  Is(EChange::AddInterface)
                                  ? TEXT(",ue_implements(interface=\"/Script/DasRuntime.DasEventInterface\")")
                                  : TEXT(""), Composed(K) ? *ComposedMembers(Change) : *EventedMembers(K, Change));
    if (!Is(EChange::RemoveField))
        S += FString::Printf(
            TEXT("    @ue_property=(EditAnywhere,%s)\n    @ue_id=\"reloadmatrix.host.value\"\n    %s : %s = %s\n"),
                 Is(EChange::MakeReadOnly) ? TEXT("BlueprintReadOnly")
                                           : TEXT("BlueprintReadWrite"), Is(EChange::RenameField)
                                                  ? TEXT("Renamed")
                                                  : TEXT("Value"), Is(EChange::RetypeField) ? T.RetypeType : T.Type,
                                                         Is(EChange::RetypeField)     ? T.RetypeDefault
                                                         : Is(EChange::ChangeDefault) ? T.Default2
                                                                                      : T.Default);
    if (Is(EChange::AddField))
        S += TEXT("    @ue_property=(EditAnywhere,BlueprintReadWrite)\n    @ue_id=\"reloadmatrix.host.extra\"\n"
                  "    Extra : int = 5\n");
    S += TEXT("    @ue_property=(EditAnywhere,BlueprintReadWrite)\n    @ue_id=\"reloadmatrix.host.anchor\"\n"
              "    Anchor : int = 1\n");
    if (!Is(EChange::RemoveMethod))
        S += Is(EChange::ChangeSignature) ? TEXT("    [ue_function(id=\"reloadmatrix.host.get\",BlueprintCallable)]\n"
                        "    def Get(x:int):int { return self.Anchor + x }\n")
                                          : TEXT("    [ue_function(id=\"reloadmatrix.host.get\",BlueprintCallable)]\n"
                        "    def Get():int { return self.Anchor }\n");
    // Actor adapters need at least one method; Keep stays in every version.
    S += TEXT("    [ue_function(id=\"reloadmatrix.host.keep\",BlueprintCallable)]\n"
              "    def Keep():int { return 3 }\n");
    if (Is(EChange::AddInterface))
        S += TEXT("    [ue_function(BlueprintCallable)]\n    def Notify(Amount:int):void { self.Anchor += Amount }\n"
                  "    [ue_function(BlueprintCallable)]\n    def ResetCounter():void { self.Anchor = 1 }\n");
    if (Is(EChange::AddMethod))
        S += TEXT("    [ue_function(id=\"reloadmatrix.host.extra_method\",BlueprintCallable)]\n"
                  "    def ExtraMethod():int { return 2 }\n");
    return S + TEXT("}\n");
}

FString Text(UObject* O, const TCHAR* Name)
{
    FProperty* P = O ? O->GetClass()->FindPropertyByName(Name) : nullptr;
    FString Out;
    if (P)
        P->ExportTextItem_Direct(Out, P->ContainerPtrToValuePtr<void>(O), nullptr, O, PPF_None);
    return P ? Out : TEXT("<missing>");
}
bool Import(UObject* O, const TCHAR* Name, const TCHAR* Value)
{
    FProperty* P = O ? O->GetClass()->FindPropertyByName(Name) : nullptr;
    return P && P->ImportText_Direct(Value, P->ContainerPtrToValuePtr<void>(O), O, PPF_None);
}
USceneComponent* SceneComp(UObject* Object, const TCHAR* Name)
{
    auto* Actor = Cast<AActor>(Object);
    if (Actor)
        for (auto* C : Actor->GetComponents())
            if (C && C->GetFName() == Name)
                return Cast<USceneComponent>(C);
    return nullptr;
}
double CompZ(UObject* Actor, const TCHAR* Name)
{
    auto* C = SceneComp(Actor, Name);
    return C ? C->GetRelativeLocation().Z : -1.0;
}
AActor* FindActor(UWorld* World, FName Name)
{
    if (World)
        for (TActorIterator<AActor> It(World); It; ++It)
            if (It->GetFName() == Name)
                return *It;
    return nullptr;
}

struct FMatrixCell
{
    const FMatrixKind* Kind;
    const FMatrixType* Type;
    EChange Change;
    FString Key, ScriptName, ScriptPath, Package, Suffix;
    TStrongObjectPtr<UBlueprint> Blueprint, Owner;
    FName Placed, Plain;
    // Object paths of the instances of asset kinds (empty for actors).
    FString PlacedPath, PlainPath;
    FString Outcome, Message;
};
// The overridden (bPlaced) or plain instance: an actor in World, or the object of an asset kind.
UObject* Instance(const FMatrixCell& Cell, UWorld* World, bool bPlaced)
{
    const FString& Path = bPlaced ? Cell.PlacedPath : Cell.PlainPath;
    if (!Path.IsEmpty())
        return FindObject<UObject>(nullptr, *Path);
    AActor* Actor = FindActor(World, bPlaced ? Cell.Placed : Cell.Plain);
    if (Actor && ComponentKind(*Cell.Kind))
    {
        for (auto* C : Actor->GetComponents())
            if (C && C->GetFName() == TEXT("Comp"))
                return C;
        return nullptr;
    }
    return Actor;
}
TArray<FMatrixCell> MatrixCells()
{
    TArray<FMatrixCell> Cells;
    auto& Runtime = FModuleManager::LoadModuleChecked<FDasRuntimeModule>(TEXT("DasRuntime"));
    const FString Run = FGuid::NewGuid().ToString(EGuidFormats::Digits);
    for (const auto& K : MatrixKinds)
        for (const auto& T : MatrixTypes)
            for (int32 C = 0; C <= int32(EChange::Reparent); ++C)
            {
                const EChange Change = EChange(C);
                // Field changes for every type on plain actors; one representative type elsewhere.
                if ((FCString::Strcmp(K.Name, TEXT("actor")) != 0 || !FieldChange(Change)) &&
                     FCString::Strcmp(T.Name, TEXT("int")) != 0)
                    continue;
                // Component changes only on the composed actor.
                if ((ComponentChange(Change) && !Composed(K)) || (EventChange(Change) && !Evented(K)) ||
                    (HierarchyChange(Change) && !Child(K)) ||
                    (Change == EChange::AddInterface && FCString::Strcmp(K.Name, TEXT("actor")) != 0))
                    continue;
                FMatrixCell Cell{&K, &T, Change};
                Cell.Key = FString::Printf(TEXT("%s/%s/%s"), K.Name, T.Name, ChangeName(Change));
                const FString Id = FString::Printf(TEXT("%s_%s_%s"), K.Name, T.Name, ChangeName(Change));
                Cell.ScriptName = FString::Printf(TEXT("reload_matrix_%s_%s.das"), *Id, *Run.Left(8));
                Cell.ScriptPath = Runtime.ScriptPath(Cell.ScriptName);
                Cell.Suffix = FString::Printf(TEXT("%s_%s"), *Id, *Run.Left(8));
                Cell.Package = FString::Printf(TEXT("/Game/DasReloadMatrix/%s/%s/BP_Host"), *Run, *Id);
                Cell.Placed = FName(*(TEXT("Placed_")+ Id));
                Cell.Plain = FName(*(TEXT("Plain_")+ Id));
                Cells.Add(MoveTemp(Cell));
            }
    return Cells;
}
bool Write(const FString& Path, const FString& Text)
{
    return FFileHelper::SaveStringToFile(Text, *Path, FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM);
}
// Applies v1 and places an overridden and a plain instance in World.
FString ParentPath(const TCHAR* Module, const FMatrixCell& Cell)
{
    auto& Runtime = FModuleManager::LoadModuleChecked<FDasRuntimeModule>(TEXT("DasRuntime"));
    return Runtime.ScriptPath(FString::Printf(TEXT("%s_%s.das"), Module, *Cell.Suffix));
}
void DeleteCellScripts(const FMatrixCell& Cell)
{
    IFileManager::Get().Delete(*Cell.ScriptPath);
    if (Child(*Cell.Kind))
        for (const TCHAR* Module : {TEXT("reload_matrix_base"), TEXT("reload_matrix_other")})
            IFileManager::Get().Delete(*ParentPath(Module, Cell));
}
// Asset kinds: a DataAsset instance is a saved package (found by the migration scope through the asset
// registry); a SaveGame object lives only in memory.
bool PlaceObjects(FMatrixCell& Cell, UBlueprint* BP, FString& Error)
{
    const FString Folder = FPackageName::GetLongPackagePath(Cell.Package);
    for (const bool bPlaced : {true, false})
    {
        const FString Package = Folder / (bPlaced ? TEXT("Placed") : TEXT("Plain"));
        UObject* Object = nullptr;
        if (DataAssetKind(*Cell.Kind))
        {
            UDataAsset* Asset = nullptr;
            if (!FDasDataAssetService::Create(BP, Package, Asset, Error))
                return false;
            Object = Asset;
        }
        else
            Object = NewObject<UObject>(CreatePackage(*Package), BP->GeneratedClass,
                                        *FPackageName::GetLongPackageAssetName(Package), RF_Public | RF_Standalone);
        if (bPlaced &&
            (!Import(Object, TEXT("Value"), Cell.Type->Override) || !Import(Object, TEXT("Anchor"), TEXT("42"))))
        {
            Error = TEXT("Cannot override matrix instance values");
            return false;
        }
        (bPlaced ? Cell.PlacedPath : Cell.PlainPath) = Object->GetPathName();
        if (DataAssetKind(*Cell.Kind))
        {
            const FString File =
                FPackageName::LongPackageNameToFilename(Package, FPackageName::GetAssetPackageExtension());
            FSavePackageArgs Save;
            Save.TopLevelFlags = RF_Public | RF_Standalone;
            if (!UPackage::SavePackage(Object->GetOutermost(), Object, *File, Save))
            {
                Error = TEXT("Cannot save matrix DataAsset");
                return false;
            }
            FAssetRegistryModule::GetRegistry().ScanFilesSynchronous({File}, true);
        }
    }
    return true;
}
// The component kind: an actor Blueprint with the authored component, placed twice.
bool PlaceOwners(FMatrixCell& Cell, UBlueprint* Component, UWorld* World, FString& Error)
{
    const FString Package = FPackageName::GetLongPackagePath(Cell.Package) / TEXT("BP_Owner");
    UBlueprint* Owner = FKismetEditorUtilities::CreateBlueprint(
        AActor::StaticClass(), CreatePackage(*Package),
        TEXT("BP_Owner"), BPTYPE_Normal, UBlueprint::StaticClass(), UBlueprintGeneratedClass::StaticClass());
    if (!Owner || !Owner->SimpleConstructionScript)
    {
        Error = TEXT("Cannot create the owner Blueprint");
        return false;
    }
    USimpleConstructionScript* SCS = Owner->SimpleConstructionScript;
    SCS->AddNode(SCS->CreateNode(Component->GeneratedClass, TEXT("Comp")));
    FKismetEditorUtilities::CompileBlueprint(Owner);
    Cell.Owner.Reset(Owner);
    for (FName Name : {Cell.Placed, Cell.Plain})
    {
        FActorSpawnParameters Spawn;
        Spawn.Name = Name;
        if (!World->SpawnActor<AActor>(Owner->GeneratedClass, FTransform::Identity, Spawn))
        {
            Error = TEXT("Cannot place matrix owner");
            return false;
        }
    }
    UObject* Placed = Instance(Cell, World, true);
    if (!Import(Placed, TEXT("Value"), Cell.Type->Override) || !Import(Placed, TEXT("Anchor"), TEXT("42")))
    {
        Error = TEXT("Cannot override matrix component values");
        return false;
    }
    return true;
}
bool PrepareCell(FMatrixCell& Cell, UWorld* World, FString& Error)
{
    UBlueprint* BP = nullptr;
    if (Child(*Cell.Kind) &&
        (!Write(ParentPath(TEXT("reload_matrix_base"), Cell),
                           ParentScript(TEXT("MatrixBase"), TEXT("reload_matrix_base"), Cell.Suffix, false)) ||
                           !Write(ParentPath(TEXT("reload_matrix_other"), Cell),
                                             ParentScript(TEXT("MatrixOther"), TEXT("reload_matrix_other"), Cell.Suffix,
                                                                                    false))))
    {
        Error = TEXT("Cannot write parent scripts");
        return false;
    }
    if (!Write(Cell.ScriptPath, MatrixScript(*Cell.Kind, *Cell.Type, {}, Cell.Suffix)) ||
        !FDasDSLService::Apply(Cell.ScriptName, Cell.Package, TEXT("reloadmatrix.host"), BP, Error))
        return false;
    Cell.Blueprint.Reset(BP);
    if (ComponentKind(*Cell.Kind))
        return PlaceOwners(Cell, BP, World, Error);
    if (AssetKind(*Cell.Kind))
        return PlaceObjects(Cell, BP, Error);
    for (FName Name : {Cell.Placed, Cell.Plain})
    {
        FActorSpawnParameters Spawn;
        Spawn.Name = Name;
        if (!World->SpawnActor<AActor>(BP->GeneratedClass, FTransform::Identity, Spawn))
        {
            Error = TEXT("Cannot place matrix instance");
            return false;
        }
    }
    UObject* Placed = Instance(Cell, World, true);
    if (!Import(Placed, TEXT("Value"), Cell.Type->Override) || !Import(Placed, TEXT("Anchor"), TEXT("42")))
    {
        Error = TEXT("Cannot override matrix instance values");
        return false;
    }
    if (Composed(*Cell.Kind))
        if (auto* Child = SceneComp(Placed, TEXT("Child")))
        {
            Child->Modify();
            Child->SetRelativeLocation(FVector(0, 0, 80));
        }
    return true;
}
// Applies the cell's change and classifies the result on the instance Placed (Anchor = AnchorValue).
void RunCell(FMatrixCell& Cell, UWorld* World, const TCHAR* AnchorValue)
{
    UBlueprint* BP = Cell.Blueprint.Get();
    UObject* Placed = Instance(Cell, World, true);
    const FString ValueBefore = Text(Placed, TEXT("Value"));
    UClass* ClassBefore = BP->GeneratedClass;
    FString Error;
    const bool Applied =
        (Cell.Change == EChange::ParentAddField
         ? Write(ParentPath(TEXT("reload_matrix_base"), Cell),
                            ParentScript(TEXT("MatrixBase"), TEXT("reload_matrix_base"), Cell.Suffix, true))
                            : Write(Cell.ScriptPath, MatrixScript(*Cell.Kind, *Cell.Type, Cell.Change, Cell.Suffix))) &&
                 FDasDSLService::Reimport(BP, Error, EDasDefaultMigrationPolicy::Automatic, ComponentKind(*Cell.Kind));
    Cell.Message = Error;
    Placed = Instance(Cell, World, true);
    if (!Applied)
    {
        // A refusal changes nothing; a later cell must still see the old declaration.
        Cell.Outcome = Placed && BP->GeneratedClass == ClassBefore &&
                       Text(Placed, TEXT("Value")) == ValueBefore &&
                            Text(Placed, TEXT("Anchor")) == AnchorValue ? TEXT("refused") : TEXT("broken");
        return;
    }
    UObject* CDO = BP->GeneratedClass->GetDefaultObject();
    const bool AnchorKept = Placed && Text(Placed, TEXT("Anchor")) == AnchorValue;
    bool Shape = Placed != nullptr, Kept = AnchorKept;
    switch (Cell.Change)
    {
    case EChange::AddField:
        Shape &= Text(CDO, TEXT("Extra")) == TEXT("5") && Text(Placed, TEXT("Extra")) == TEXT("5");
        Kept &= Text(Placed, TEXT("Value")) == ValueBefore;
        break;
    case EChange::RemoveField:
        Shape &= Text(CDO, TEXT("Value")) == TEXT("<missing>");
        break;
    case EChange::RenameField:
        Shape &= Text(CDO, TEXT("Renamed")) != TEXT("<missing>") && Text(CDO, TEXT("Value")) == TEXT("<missing>");
        Kept &= Text(Placed, TEXT("Renamed")) == ValueBefore;
        break;
    case EChange::RetypeField:
        Shape &= Text(CDO, TEXT("Value")) != TEXT("<missing>");
        break;
    case EChange::ChangeDefault:
    {
        // The editor instance without an override follows the new default.
        UObject* Plain = Instance(Cell, GEditor->GetEditorWorldContext().World(), false);
        Shape &= Text(CDO, TEXT("Value")) != TEXT("<missing>");
        Kept &= Text(Placed, TEXT("Value")) == ValueBefore && Text(Plain, TEXT("Value")) == Text(CDO, TEXT("Value"));
        // During PIE the running copy without an override follows the new default as well.
        if (World->IsGameWorld())
            Kept &= Text(Instance(Cell, World, false), TEXT("Value")) == Text(CDO, TEXT("Value"));
        break;
    }
    case EChange::MakeReadOnly:
    {
        FProperty* P = BP->GeneratedClass->FindPropertyByName(TEXT("Value"));
        Shape &= P && P->HasAnyPropertyFlags(CPF_BlueprintReadOnly);
        Kept &= Text(Placed, TEXT("Value")) == ValueBefore;
        break;
    }
    case EChange::AddMethod:
        Shape &= BP->GeneratedClass->FindFunctionByName(TEXT("ExtraMethod")) != nullptr;
        break;
    case EChange::RemoveMethod:
        Shape &= BP->GeneratedClass->FindFunctionByName(TEXT("Get")) == nullptr;
        break;
    case EChange::ChangeSignature:
    {
        UFunction* F = BP->GeneratedClass->FindFunctionByName(TEXT("Get"));
        Shape &= F && F->NumParms == 2;
        break;
    }
    // Component changes: the placed instance moved Child to Z=80, the template has Z=50.
    case EChange::AddComponent:
        Shape &= SceneComp(Placed, TEXT("Extra")) != nullptr;
        Kept &= CompZ(Placed, TEXT("Child")) == 80.0;
        break;
    case EChange::RemoveComponent:
        Shape &= !SceneComp(Placed, TEXT("Child")) && SceneComp(Placed, TEXT("Other"));
        break;
    case EChange::RenameComponent:
        Shape &= SceneComp(Placed, TEXT("Arm")) && !SceneComp(Placed, TEXT("Child"));
        Kept &= CompZ(Placed, TEXT("Arm")) == 80.0;
        break;
    case EChange::ChangeComponentDefault:
        Shape &= CompZ(Instance(Cell, GEditor->GetEditorWorldContext().World(), false), TEXT("Child")) == 60.0;
        Kept &= CompZ(Placed, TEXT("Child")) == 80.0;
        break;
    case EChange::AddEvent:
        Shape &= CastField<FMulticastDelegateProperty>(BP->GeneratedClass->FindPropertyByName(TEXT("ExtraEvent"))) &&
                                                       BP->GeneratedClass->FindPropertyByName(TEXT("Changed"));
        break;
    case EChange::RemoveEvent:
        Shape &= !BP->GeneratedClass->FindPropertyByName(TEXT("Changed")) &&
                                                         BP->GeneratedClass->FindPropertyByName(TEXT("Query"));
        break;
    case EChange::AddInterface:
        Shape &= BP->GeneratedClass->ImplementsInterface(UDasEventInterface::StaticClass());
        break;
    // Hierarchies: the placed child keeps its own values and gets the parent's new field / new parent.
    case EChange::ParentAddField:
        Shape &= Text(Placed, TEXT("Added")) == TEXT("9");
        Kept &= Text(Placed, TEXT("Value")) == ValueBefore;
        break;
    case EChange::Reparent:
        Shape &= BP->ParentClass && BP->ParentClass->GetName().StartsWith(
                                        TEXT("BP_MatrixOther")) && Text(Placed, TEXT("BaseValue")) != TEXT("<missing>");
        Kept &= Text(Placed, TEXT("Value")) == ValueBefore;
        break;
    case EChange::ChangeAttach:
    {
        auto* Moved = SceneComp(Placed, TEXT("Child"));
        Shape &= Moved && Moved->GetAttachParent() == SceneComp(Placed, TEXT("Other"));
        Kept &= CompZ(Placed, TEXT("Child")) == 80.0;
        break;
    }
    }
    Cell.Outcome = !Shape ? TEXT("broken") : Kept ? TEXT("migrated") : TEXT("lossy");
    if (Cell.Outcome != TEXT("migrated"))
        Cell.Message = FString::Printf(
            TEXT("Value %s -> %s, Anchor %s, Child Z %.0f"), *ValueBefore,
                 *Text(Placed, TEXT("Value")),
                       *Text(Placed, TEXT("Anchor")),
                             CompZ(Placed, Cell.Change == EChange::RenameComponent ? TEXT("Arm") : TEXT("Child")));
}

FString RepoDir()
{
    return FPaths::ConvertRelativePathToFull(FPaths::ProjectDir() / TEXT("../.."));
}
// Records the outcomes of Mode and compares them with the expected file; returns the mismatches.
TArray<FString> RecordMatrix(const TCHAR* Mode, const TArray<FMatrixCell>& Cells)
{
    const FString ActualPath = RepoDir() / TEXT("logs/reload-matrix.json");
    TSharedPtr<FJsonObject> Actual;
    FString Existing;
    if (FFileHelper::LoadFileToString(Existing, *ActualPath))
        FJsonSerializer::Deserialize(TJsonReaderFactory<>::Create(Existing), Actual);
    if (!Actual.IsValid())
        Actual = MakeShared<FJsonObject>();
    auto Cells_ = MakeShared<FJsonObject>();
    for (const auto& Cell : Cells)
    {
        auto Row = MakeShared<FJsonObject>();
        Row->SetStringField(TEXT("outcome"), Cell.Outcome);
        Row->SetStringField(TEXT("message"), Cell.Message);
        Cells_->SetObjectField(Cell.Key, Row);
    }
    Actual->SetObjectField(Mode, Cells_);
    FString Out;
    FJsonSerializer::Serialize(Actual.ToSharedRef(), TJsonWriterFactory<>::Create(&Out));
    Write(ActualPath, Out);

    TArray<FString> Mismatches;
    FString ExpectedText;
    TSharedPtr<FJsonObject> Expected;
    if (!FFileHelper::LoadFileToString(
            ExpectedText, *(RepoDir() / TEXT("Tests/ReloadMatrix/expected.json"))) ||
            !FJsonSerializer::Deserialize(TJsonReaderFactory<>::Create(ExpectedText), Expected) || !Expected.IsValid())
    {
        Mismatches.Add(TEXT("Tests/ReloadMatrix/expected.json is missing; outcomes are in logs/reload-matrix.json"));
        return Mismatches;
    }
    const TSharedPtr<FJsonObject>* ModeCells = nullptr;
    Expected->TryGetObjectField(Mode, ModeCells);
    for (const auto& Cell : Cells)
    {
        const TSharedPtr<FJsonObject>* Row = nullptr;
        FString Want;
        if (!ModeCells || !(*ModeCells)->TryGetObjectField(Cell.Key, Row) ||
            !(*Row)->TryGetStringField(TEXT("outcome"), Want))
            Want = TEXT("<none>");
        if (Want != Cell.Outcome || Cell.Outcome == TEXT("broken"))
            Mismatches.Add(FString::Printf(TEXT("%s %s: expected %s, got %s (%s)"), Mode, *Cell.Key, *Want,
                                                *Cell.Outcome, *Cell.Message));
    }
    return Mismatches;
}
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FDasReloadMatrixEditorTest, "DasForUE.ReloadMatrix.Editor",
                                 EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FDasReloadMatrixEditorTest::RunTest(const FString&)
{
    UWorld* World = FAutomationEditorCommonUtils::CreateNewMap();
    TArray<DasReloadMatrix::FMatrixCell> Cells = DasReloadMatrix::MatrixCells();
    for (auto& Cell : Cells)
    {
        FString Error;
        if (!DasReloadMatrix::PrepareCell(Cell, World, Error))
        {
            Cell.Outcome = TEXT("broken");
            Cell.Message = TEXT("v1: ")+ Error;
            continue;
        }
        DasReloadMatrix::RunCell(Cell, World, TEXT("42"));
        UE_LOG(LogTemp, Display, TEXT("DAS_RELOAD_MATRIX editor %s %s %s"), *Cell.Key, *Cell.Outcome, *Cell.Message);
    }
    for (const auto& Cell : Cells)
        DeleteCellScripts(Cell);
    for (const FString& M : DasReloadMatrix::RecordMatrix(TEXT("editor"), Cells))
        AddError(M);
    return true;
}

namespace DasReloadMatrix
{
struct FMatrixPIEState
{
    TArray<FMatrixCell> Cells;
    bool Prepared = true;
};
class FMatrixPIECommand final : public IAutomationLatentCommand
{
    TSharedRef<FMatrixPIEState> S;
    FAutomationTestBase* Test;
    double Start = FPlatformTime::Seconds();
    bool Ending = false;

public:
    FMatrixPIECommand(TSharedRef<FMatrixPIEState> State, FAutomationTestBase* T) : S(State), Test(T) {}
    bool Update() override
    {
        if (Ending)
        {
            if (GEditor->PlayWorld && FPlatformTime::Seconds() - Start < 30)
                return false;
            for (const auto& Cell : S->Cells)
                DeleteCellScripts(Cell);
            for (const FString& M : RecordMatrix(TEXT("pie"), S->Cells))
                Test->AddError(M);
            return true;
        }
        UWorld* World = GEditor->PlayWorld;
        if (!World || !World->HasBegunPlay())
        {
            if (FPlatformTime::Seconds() - Start < 60)
                return false;
            Test->AddError(TEXT("PIE did not start"));
            return true;
        }
        for (auto& Cell : S->Cells)
        {
            if (!Cell.Outcome.IsEmpty())
                continue;
            // A value set in the running game must survive the change (game state).
            Import(Instance(Cell, World, true), TEXT("Anchor"), TEXT("55"));
            RunCell(Cell, World, TEXT("55"));
            UE_LOG(LogTemp, Display, TEXT("DAS_RELOAD_MATRIX pie %s %s %s"), *Cell.Key, *Cell.Outcome, *Cell.Message);
        }
        GEditor->RequestEndPlayMap();
        Ending = true;
        Start = FPlatformTime::Seconds();
        return false;
    }
};
}
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FDasReloadMatrixPIETest, "DasForUE.ReloadMatrix.PIE",
                                 EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FDasReloadMatrixPIETest::RunTest(const FString&)
{
    UWorld* World = FAutomationEditorCommonUtils::CreateNewMap();
    auto S = MakeShared<DasReloadMatrix::FMatrixPIEState>();
    S->Cells = DasReloadMatrix::MatrixCells();
    for (auto& Cell : S->Cells)
    {
        FString Error;
        if (!DasReloadMatrix::PrepareCell(Cell, World, Error))
        {
            Cell.Outcome = TEXT("broken");
            Cell.Message = TEXT("v1: ")+ Error;
        }
    }
    ADD_LATENT_AUTOMATION_COMMAND(FStartPIECommand(false));
    FAutomationTestFramework::Get().EnqueueLatentCommand(MakeShared<DasReloadMatrix::FMatrixPIECommand>(S, this));
    return true;
}

namespace DasReloadMatrix
{
// A Character possessed by the player keeps its controller when a declaration change reinstances it.
struct FPossessedState
{
    FMatrixCell Cell;
    bool Passed = true;
};
class FPossessedCommand final : public IAutomationLatentCommand
{
    TSharedRef<FPossessedState> S;
    FAutomationTestBase* Test;
    double Start = FPlatformTime::Seconds();
    bool Ending = false;

public:
    FPossessedCommand(TSharedRef<FPossessedState> State, FAutomationTestBase* T) : S(State), Test(T) {}
    bool Update() override
    {
        auto Check = [&](bool Value, const TCHAR* Name)
        {
            S->Passed &= Test->TestTrue(Name, Value);
            UE_LOG(LogTemp, Display, TEXT("DAS_POSSESSED_PIE %s %s"), Value ? TEXT("PASS") : TEXT("FAIL"), Name);
        };
        if (Ending)
        {
            if (GEditor->PlayWorld && FPlatformTime::Seconds() - Start < 30)
                return false;
            DeleteCellScripts(S->Cell);
            return true;
        }
        UWorld* World = GEditor->PlayWorld;
        APlayerController* PC = World ? World->GetFirstPlayerController() : nullptr;
        if (!World || !World->HasBegunPlay() || !PC)
        {
            if (FPlatformTime::Seconds() - Start < 60)
                return false;
            Check(false, TEXT("PIE_started"));
            return true;
        }
        auto Finish = [&]()
        {
            GEditor->RequestEndPlayMap();
            Ending = true;
            Start = FPlatformTime::Seconds();
            return false;
        };
        UBlueprint* BP = S->Cell.Blueprint.Get();
        auto* Pawn = Cast<APawn>(World->SpawnActor<AActor>(BP->GeneratedClass, FTransform(FVector(0, 0, 200))));
        if (!Pawn)
        {
            Check(false, TEXT("spawn_character"));
            return Finish();
        }
        PC->Possess(Pawn);
        Import(Pawn, TEXT("Anchor"), TEXT("55"));
        Check(PC->GetPawn() == Pawn, TEXT("possessed_before_change"));
        FString Error;
        Check(
            Write(S->Cell.ScriptPath, MatrixScript(*S->Cell.Kind, *S->Cell.Type, EChange::AddField, S->Cell.Suffix)) &&
                FDasDSLService::Reimport(BP, Error),
            TEXT("character_field_added_during_PIE"));
        if (!Error.IsEmpty())
            Test->AddInfo(Error);
        APawn* Now = PC->GetPawn();
        Check(Now && Now->GetClass() == BP->GeneratedClass && Now->GetController() == PC,
              TEXT("possession_kept_after_reinstance"));
        Check(Now && Text(Now, TEXT("Anchor")) ==
                          TEXT("55") && Text(Now, TEXT("Extra")) == TEXT("5"),
                                                                         TEXT("game_state_kept_and_new_field_present"));
        return Finish();
    }
};
}
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FDasReloadMatrixPossessedTest, "DasForUE.ReloadMatrix.PossessedCharacterPIE",
                                 EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FDasReloadMatrixPossessedTest::RunTest(const FString&)
{
    FAutomationEditorCommonUtils::CreateNewMap();
    auto S = MakeShared<DasReloadMatrix::FPossessedState>();
    for (auto& Cell : DasReloadMatrix::MatrixCells())
        if (Cell.Key == TEXT("character/int/add_field"))
            S->Cell = MoveTemp(Cell);
    UBlueprint* BP = nullptr;
    FString Error;
    if (!TestTrue(TEXT("apply_character"),
            DasReloadMatrix::Write(S->Cell.ScriptPath,
                                   DasReloadMatrix::MatrixScript(*S->Cell.Kind, *S->Cell.Type, {}, S->Cell.Suffix)) &&
                FDasDSLService::Apply(S->Cell.ScriptName, S->Cell.Package, TEXT("reloadmatrix.host"), BP, Error)))
    {
        AddError(Error);
        return false;
    }
    S->Cell.Blueprint.Reset(BP);
    ADD_LATENT_AUTOMATION_COMMAND(FStartPIECommand(false));
    FAutomationTestFramework::Get().EnqueueLatentCommand(MakeShared<DasReloadMatrix::FPossessedCommand>(S, this));
    return true;
}

void DasFailStructuralForTesting(int32 Stage);
// A failure after an interface was added, an event or a method removed or a signature changed restores the
// Blueprint exactly.
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FDasReloadMatrixRollbackTest, "DasForUE.ReloadMatrix.Rollback",
                                 EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FDasReloadMatrixRollbackTest::RunTest(const FString&)
{
    UWorld* World = FAutomationEditorCommonUtils::CreateNewMap();
    for (auto& Cell : DasReloadMatrix::MatrixCells())
    {
        if (Cell.Key != TEXT("actor/int/add_interface") &&
                             Cell.Key != TEXT("evented/int/remove_event") &&
                                              Cell.Key != TEXT("actor/int/remove_method") &&
                                                               Cell.Key != TEXT("actor/int/change_signature"))
            continue;
        FString Error;
        if (!TestTrue(*(Cell.Key + TEXT(" v1")), DasReloadMatrix::PrepareCell(Cell, World, Error)))
        {
            AddError(Error);
            continue;
        }
        UBlueprint* BP = Cell.Blueprint.Get();
        int32 Nodes = 0;
        TArray<UEdGraph*> Graphs;
        BP->GetAllGraphs(Graphs);
        for (UEdGraph* G : Graphs)
            Nodes += G->Nodes.Num();
        const int32 Interfaces = BP->ImplementedInterfaces.Num(), Dispatchers = BP->DelegateSignatureGraphs.Num();
        DasReloadMatrix::Write(Cell.ScriptPath,
                               DasReloadMatrix::MatrixScript(*Cell.Kind, *Cell.Type, Cell.Change, Cell.Suffix));
        DasFailStructuralForTesting(2);
        const bool Failed = !FDasDSLService::Reimport(BP, Error);
        DasFailStructuralForTesting(0);
        int32 NodesAfter = 0;
        Graphs.Reset();
        BP->GetAllGraphs(Graphs);
        for (UEdGraph* G : Graphs)
            NodesAfter += G->Nodes.Num();
        AddInfo(FString::Printf(TEXT("%s: failed=%d nodes %d->%d interfaces %d->%d dispatchers %d->%d changed=%d (%s)"),
                                     *Cell.Key, Failed, Nodes, NodesAfter, Interfaces, BP->ImplementedInterfaces.Num(),
                                     Dispatchers, BP->DelegateSignatureGraphs.Num(),
                                     BP->GeneratedClass->FindPropertyByName(TEXT("Changed")) != nullptr, *Error));
        TestTrue(*(Cell.Key + TEXT(" rolled back")),
                   Failed &&
                       Error.Contains(TEXT("rolled back")) && BP->ImplementedInterfaces.Num() == Interfaces &&
                                      BP->DelegateSignatureGraphs.Num() == Dispatchers && NodesAfter == Nodes &&
                                      !BP->GeneratedClass->ImplementsInterface(UDasEventInterface::StaticClass()) &&
                                      (Cell.Change != DasReloadMatrix::EChange::RemoveEvent ||
                                       BP->GeneratedClass->FindPropertyByName(TEXT("Changed"))) &&
                                       (BP->GeneratedClass->FindFunctionByName(
                                           TEXT("Get")) &&
                                           BP->GeneratedClass->FindFunctionByName(TEXT("Get"))->NumParms == 1));
        TestTrue(*(Cell.Key + TEXT(" applies after the failure")), FDasDSLService::Reimport(BP, Error));
        DeleteCellScripts(Cell);
    }
    return true;
}
#endif