// Reload matrix: every declaration change of every entity kind applied with the automatic Reimport, in the
// editor and during PIE. Each cell ends migrated, lossy (applied but a value was lost), refused (nothing
// changed) or broken (an invariant failed). Outcomes are compared with Tests/ReloadMatrix/expected.json and
// written to logs/reload-matrix.json; Scripts/RenderReloadMatrix.py turns the expected file into
// Docs/Reference/ReloadMatrix.md.
#include "DasDSLService.h"
#include "DasRuntime.h"
#include "Dom/JsonObject.h"
#include "Editor.h"
#include "Engine/Blueprint.h"
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
};
const TCHAR* ChangeName(EChange C)
{
    // clang-format off
    static const TCHAR* Names[] = {TEXT("add_field"), TEXT("remove_field"), TEXT("rename_field"),
                                   TEXT("retype_field"), TEXT("change_default"), TEXT("make_read_only"),
                                   TEXT("add_method"), TEXT("remove_method"), TEXT("change_signature")};
    // clang-format on
    return Names[int32(C)];
}
bool FieldChange(EChange C)
{
    return C <= EChange::MakeReadOnly;
}

// The declaration of one cell before (Change unset) or after its change.
FString MatrixScript(const FMatrixKind& K, const FMatrixType& T, TOptional<EChange> Change)
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
    S += FString::Printf(TEXT("[ue_class(module_id=\"reloadmatrix\",id=\"reloadmatrix.host\",parent=\"%s\","
                              "Blueprintable)]\nclass MatrixHost {\n%s"),
                              K.Parent, K.Members);
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
    FString Key, ScriptName, ScriptPath, Package;
    TStrongObjectPtr<UBlueprint> Blueprint;
    FName Placed, Plain;
    FString Outcome, Message;
};
TArray<FMatrixCell> MatrixCells()
{
    TArray<FMatrixCell> Cells;
    auto& Runtime = FModuleManager::LoadModuleChecked<FDasRuntimeModule>(TEXT("DasRuntime"));
    const FString Run = FGuid::NewGuid().ToString(EGuidFormats::Digits);
    for (const auto& K : MatrixKinds)
        for (const auto& T : MatrixTypes)
            for (int32 C = 0; C <= int32(EChange::ChangeSignature); ++C)
            {
                const EChange Change = EChange(C);
                // Field changes for every type on plain actors; one representative type elsewhere.
                if ((FCString::Strcmp(K.Name, TEXT("actor")) != 0 || !FieldChange(Change)) &&
                     FCString::Strcmp(T.Name, TEXT("int")) != 0)
                    continue;
                FMatrixCell Cell{&K, &T, Change};
                Cell.Key = FString::Printf(TEXT("%s/%s/%s"), K.Name, T.Name, ChangeName(Change));
                const FString Id = FString::Printf(TEXT("%s_%s_%s"), K.Name, T.Name, ChangeName(Change));
                Cell.ScriptName = FString::Printf(TEXT("reload_matrix_%s_%s.das"), *Id, *Run.Left(8));
                Cell.ScriptPath = Runtime.ScriptPath(Cell.ScriptName);
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
bool PrepareCell(FMatrixCell& Cell, UWorld* World, FString& Error)
{
    UBlueprint* BP = nullptr;
    if (!Write(Cell.ScriptPath, MatrixScript(*Cell.Kind, *Cell.Type, {})) ||
        !FDasDSLService::Apply(Cell.ScriptName, Cell.Package, TEXT("reloadmatrix.host"), BP, Error))
        return false;
    Cell.Blueprint.Reset(BP);
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
    AActor* Placed = FindActor(World, Cell.Placed);
    if (!Import(Placed, TEXT("Value"), Cell.Type->Override) || !Import(Placed, TEXT("Anchor"), TEXT("42")))
    {
        Error = TEXT("Cannot override matrix instance values");
        return false;
    }
    return true;
}
// Applies the cell's change and classifies the result on the instance Placed (Anchor = AnchorValue).
void RunCell(FMatrixCell& Cell, UWorld* World, const TCHAR* AnchorValue)
{
    UBlueprint* BP = Cell.Blueprint.Get();
    AActor* Placed = FindActor(World, Cell.Placed);
    const FString ValueBefore = Text(Placed, TEXT("Value"));
    UClass* ClassBefore = BP->GeneratedClass;
    FString Error;
    const bool Applied = Write(Cell.ScriptPath, MatrixScript(*Cell.Kind, *Cell.Type, Cell.Change)) &&
                         FDasDSLService::Reimport(BP, Error);
    Cell.Message = Error;
    Placed = FindActor(World, Cell.Placed);
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
        AActor* Plain = FindActor(GEditor->GetEditorWorldContext().World(), Cell.Plain);
        Shape &= Text(CDO, TEXT("Value")) != TEXT("<missing>");
        Kept &= Text(Placed, TEXT("Value")) == ValueBefore && Text(Plain, TEXT("Value")) == Text(CDO, TEXT("Value"));
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
    }
    Cell.Outcome = !Shape ? TEXT("broken") : Kept ? TEXT("migrated") : TEXT("lossy");
    if (Cell.Outcome != TEXT("migrated"))
        Cell.Message = FString::Printf(TEXT("Value %s -> %s, Anchor %s"), *ValueBefore,
                                            *Text(Placed, TEXT("Value")), *Text(Placed, TEXT("Anchor")));
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
        IFileManager::Get().Delete(*Cell.ScriptPath);
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
                IFileManager::Get().Delete(*Cell.ScriptPath);
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
            Import(FindActor(World, Cell.Placed), TEXT("Anchor"), TEXT("55"));
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
            IFileManager::Get().Delete(*S->Cell.ScriptPath);
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
        Check(Write(S->Cell.ScriptPath, MatrixScript(*S->Cell.Kind, *S->Cell.Type, EChange::AddField)) &&
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
    if (!TestTrue(
            TEXT("apply_character"),
                 DasReloadMatrix::Write(S->Cell.ScriptPath,
                                        DasReloadMatrix::MatrixScript(*S->Cell.Kind, *S->Cell.Type, {})) &&
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
#endif