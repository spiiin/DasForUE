#include "DasDSLService.h"
#include "DasRuntime.h"
#include "DasActor.h"
#include "Modules/ModuleManager.h"
#include "Engine/Blueprint.h"
#include "Engine/BlueprintGeneratedClass.h"
#include "Engine/World.h"
#include "Engine/Level.h"
#include "Editor.h"
#include "Kismet2/KismetEditorUtilities.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "K2Node_VariableGet.h"
#include "K2Node_VariableSet.h"
#include "K2Node_FunctionEntry.h"
#include "K2Node_FunctionResult.h"
#include "K2Node_CallFunction.h"
#include "EdGraphSchema_K2.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "UObject/UnrealType.h"
#include "UObject/SavePackage.h"
#include "UObject/StructOnScope.h"
#include "UObject/MetaData.h"
#include "Misc/Parse.h"
#include "Misc/Paths.h"
#include "Misc/PackageName.h"
#include "Misc/FileHelper.h"
#include "HAL/FileManager.h"
#include <limits>
#if WITH_DEV_AUTOMATION_TESTS
void DasFailWidenAfterCompileForTesting(bool Enabled);
#endif
namespace
{
double ConvertWidenValue(UObject* Object, FName Name)
{
    if (!Object)
        return std::numeric_limits<double>::quiet_NaN();
    auto* P = Object->GetClass()->FindPropertyByName(Name);
    if (auto* D = CastField<FDoubleProperty>(P))
        return D->GetPropertyValue_InContainer(Object);
    if (auto* F = CastField<FFloatProperty>(P))
        return F->GetPropertyValue_InContainer(Object);
    if (auto* I = CastField<FIntProperty>(P))
        return I->GetPropertyValue_InContainer(Object);
    return std::numeric_limits<double>::quiet_NaN();
}
void SetConvertWidenValues(UObject* Object, int32 Score, float Rate)
{
    FindFProperty<FIntProperty>(Object->GetClass(), TEXT("Score"))->SetPropertyValue_InContainer(Object, Score);
    FindFProperty<FFloatProperty>(Object->GetClass(), TEXT("Rate"))->SetPropertyValue_InContainer(Object, Rate);
}
bool ConvertSaveWidenAsset(UObject* Asset)
{
    if (!Asset)
        return false;
    auto* P = Asset->GetOutermost();
    const FString File = FPackageName::LongPackageNameToFilename(
        P->GetName(),
        Asset->IsA<UWorld>() ? FPackageName::GetMapPackageExtension() : FPackageName::GetAssetPackageExtension());
    IFileManager::Get().MakeDirectory(*FPaths::GetPath(File), true);
    Asset->SetFlags(RF_Public | RF_Standalone);
    FSavePackageArgs Args;
    Args.TopLevelFlags = RF_Public | RF_Standalone;
    Args.SaveFlags = SAVE_NoError;
    return UPackage::SavePackage(P, Asset, *File, Args);
}
UBlueprint* ConvertWidenBP(const FString& Base, const TCHAR* Name)
{
    return LoadObject<UBlueprint>(nullptr, *(Base + Name + TEXT(".")+ Name));
}
ADasActor* ConvertWidenActor(UWorld* World, FName Name)
{
    if (World && World->PersistentLevel)
        for (auto& Entry : World->PersistentLevel->Actors)
            if (auto* A = Cast<ADasActor>(Entry.Get()))
                if (A->GetFName() == Name)
                    return A;
    return nullptr;
}
}

void ConvertGraphs(UBlueprint* BP)
{
    auto Build = [&](FName Name, bool Setter, FName Field, bool Literal)
    {
        auto* G =
            FBlueprintEditorUtils::CreateNewGraph(BP, Name, UEdGraph::StaticClass(), UEdGraphSchema_K2::StaticClass());
        FBlueprintEditorUtils::AddFunctionGraph<UClass>(BP, G, true, nullptr);
        TArray<UK2Node_FunctionEntry*> Entries;
        G->GetNodesOfClass(Entries);
        auto* Entry = Entries[0];
        TArray<UK2Node_FunctionResult*> Results;
        G->GetNodesOfClass(Results);
        auto* Result = Results.IsEmpty() ? NewObject<UK2Node_FunctionResult>(G) : Results[0];
        if (Results.IsEmpty())
        {
            G->AddNode(Result);
            Result->CreateNewGuid();
            Result->PostPlacedNewNode();
            if (Result->Pins.IsEmpty())
                Result->AllocateDefaultPins();
        }
        Result->FunctionReference = Entry->FunctionReference;
        FEdGraphPinType Int;
        Int.PinCategory = UEdGraphSchema_K2::PC_Int;
        if (!Setter)
        {
            Result->CreateUserDefinedPin(UEdGraphSchema_K2::PN_ReturnValue, Int, EGPD_Input);
            auto* Get = NewObject<UK2Node_VariableGet>(G);
            Get->VariableReference.SetSelfMember(Field);
            G->AddNode(Get);
            Get->CreateNewGuid();
            Get->AllocateDefaultPins();
            G->GetSchema()->TryCreateConnection(Get->GetValuePin(),
                                                Result->FindPinChecked(UEdGraphSchema_K2::PN_ReturnValue));
            G->GetSchema()->TryCreateConnection(Entry->FindPinChecked(UEdGraphSchema_K2::PN_Then),
                                                Result->FindPinChecked(UEdGraphSchema_K2::PN_Execute));
        }
        else
        {
            if (!Literal)
            {
                FEdGraphPinType InputType = Int;
                if (Field == TEXT("Rate"))
                {
                    InputType.PinCategory = UEdGraphSchema_K2::PC_Real;
                    InputType.PinSubCategory = UEdGraphSchema_K2::PC_Float;
                }
                Entry->CreateUserDefinedPin(TEXT("Input"), InputType, EGPD_Output);
            }
            auto* Set = NewObject<UK2Node_VariableSet>(G);
            Set->VariableReference.SetSelfMember(Field);
            G->AddNode(Set);
            Set->CreateNewGuid();
            Set->AllocateDefaultPins();
            G->GetSchema()->TryCreateConnection(Entry->FindPinChecked(UEdGraphSchema_K2::PN_Then),
                                                Set->FindPinChecked(UEdGraphSchema_K2::PN_Execute));
            G->GetSchema()->TryCreateConnection(Set->FindPinChecked(UEdGraphSchema_K2::PN_Then),
                                                Result->FindPinChecked(UEdGraphSchema_K2::PN_Execute));
            if (Literal)
                G->GetSchema()->TrySetDefaultValue(*Set->FindPinChecked(Field), TEXT("-1.5"));
            else
                G->GetSchema()->TryCreateConnection(Entry->FindPinChecked(TEXT("Input")), Set->FindPinChecked(Field));
        }
    };
    Build(TEXT("ReadByGraph"), false, TEXT("Score"), false);
    Build(TEXT("WriteByGraph"), true, TEXT("Score"), false);
    Build(TEXT("SetLiteralRate"), true, TEXT("Rate"), true);
    Build(TEXT("WriteRateByGraph"), true, TEXT("Rate"), false);
    for (auto& Graph : BP->FunctionGraphs)
    {
        TArray<UK2Node_FunctionResult*> Results;
        Graph->GetNodesOfClass(Results);
        for (auto* Result : Results)
            if (Result->FindPin(UEdGraphSchema_K2::PN_Execute) &&
                Result->FindPinChecked(UEdGraphSchema_K2::PN_Execute)->LinkedTo.IsEmpty())
                Graph->RemoveNode(Result);
    }
    FBlueprintEditorUtils::MarkBlueprintAsStructurallyModified(BP);
    FKismetEditorUtilities::CompileBlueprint(BP);
}
int32 ConvertGraphRead(ADasActor* Actor)
{
    auto* F = Actor->FindFunction(TEXT("ReadByGraph"));
    FStructOnScope P(F);
    FEditorScriptExecutionGuard Guard;
    Actor->ProcessEvent(F, P.GetStructMemory());
    return CastField<FIntProperty>(F->GetReturnProperty())->GetPropertyValue_InContainer(P.GetStructMemory());
}
int32 NumericAdapterCount(UBlueprint* BP)
{
    TArray<UK2Node_CallFunction*> Nodes;
    FBlueprintEditorUtils::GetAllNodesOfClass(BP, Nodes);
    int32 Count = 0;
    for (auto* N : Nodes)
        if (N->GetTargetFunction() && N->GetTargetFunction()->GetOuterUClass() == UDasNumericLibrary::StaticClass())
            ++Count;
    return Count;
}

void DasCreateNumericGraphFixture(UBlueprint* B)
{
    ConvertGraphs(B);
}
int32 DasReadNumericGraphFixture(ADasActor* A)
{
    return ConvertGraphRead(A);
}
int32 DasNumericGraphAdapterCount(UBlueprint* B)
{
    return NumericAdapterCount(B);
}
int32 RunDasNumericConvertProbe(const FString& Params)
{
    if (FApp::GetProjectName() != FString(TEXT("DasForUESmoke")))
        return 1;
    FString Id, Phase, Error;
    if (!FParse::Value(*Params, TEXT("ProbeId="), Id) || Id.Len() != 32 ||
                       !FParse::Value(*Params, TEXT("Phase="), Phase))
        return 1;
    for (TCHAR C : Id)
        if (!FChar::IsHexDigit(C))
            return 1;
    const FString Base = TEXT("/Game/DasFrameAssetProbe/Convert_")+ Id +
                              TEXT("/"), Script = TEXT("dsl_convert_")+ Id + TEXT(".das");
    auto& Runtime = FModuleManager::LoadModuleChecked<FDasRuntimeModule>(TEXT("DasRuntime"));
    bool Passed = true;
    FDasNumericMigrationOptions Options;
    Options.IncludeDependencies = true;
    Options.Integers = EDasIntegerRule::NearestAway;
    auto Check = [&](bool Value, const TCHAR* Name)
    {
        Passed &= Value;
        UE_LOG(LogTemp, Display, TEXT("DAS_CONVERT_TEST %s %s"), Value ? TEXT("PASS"): TEXT("FAIL"), Name);
    };
    UBlueprint* Parent = nullptr;
    UBlueprint* Child = nullptr;
    UWorld* World = nullptr;
    if (Phase == TEXT("NarrowFloat") || Phase == TEXT("ReopenFloat"))
    {
        Parent = ConvertWidenBP(Base, TEXT("BP_Parent"));
        if (!Parent)
            return 1;
        if (Phase == TEXT("NarrowFloat"))
        {
            FString Source;
            FFileHelper::LoadFileToString(Source, *Runtime.ScriptPath(Script));
            Source = Source.Replace(
                TEXT("Score : double = 5.0lf"),
                     TEXT("Score : float = 5.0")).Replace(TEXT("Rate : int = 2"), TEXT("Rate : float = 2.0"));
            Check(FFileHelper::SaveStringToFile(Source, *Runtime.ScriptPath(Script),
                                                FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM),
                  TEXT("write_float_target_fields"));
            UClass* Before = Parent->GeneratedClass;
            Check(!FDasDSLService::ReimportConvertedNumericFields(Parent, Options, Error) &&
                      Parent->GeneratedClass == Before,
                  TEXT("double_to_float_requires_precision_round_policy"));
            Options.Floats = EDasFloatRule::RoundNearest;
#if WITH_DEV_AUTOMATION_TESTS
            const int32 OldAdapters = NumericAdapterCount(Parent);
            DasFailWidenAfterCompileForTesting(true);
            const bool Failed = !FDasDSLService::ReimportConvertedNumericFields(Parent, Options, Error);
            DasFailWidenAfterCompileForTesting(false);
            Check(Failed && Error.StartsWith(TEXT("Numeric migration rolled back:")) &&
                                             FindFProperty<FDoubleProperty>(
                                                 Parent->GeneratedClass, TEXT("Score")) &&
                                                 NumericAdapterCount(Parent) == OldAdapters,
                                                 TEXT("narrowing_rollback_preserves_existing_adapter_graph"));
#endif
            const bool Converted = FDasDSLService::ReimportConvertedNumericFields(Parent, Options, Error);
            Check(Converted, TEXT("explicit_float_round_migration"));
            if (!Converted)
            {
                UE_LOG(LogTemp, Error, TEXT("%s"), *Error);
                return 1;
            }
        }
        Child = ConvertWidenBP(Base, TEXT("BP_Child"));
        World = LoadObject<UWorld>(nullptr, *(Base + TEXT("Map.Map")));
        auto* A = ConvertWidenActor(World, TEXT("ParentActor"));
        auto* B = ConvertWidenActor(World, TEXT("ChildActor"));
        Check(FindFProperty<FFloatProperty>(
            Parent->GeneratedClass, TEXT("Score")) &&
            FindFProperty<FFloatProperty>(
                Parent->GeneratedClass, TEXT("Rate")) &&
                ConvertWidenValue(A, TEXT("Score")) == double(float(MIN_int32)) &&
                                  ConvertWidenValue(B, TEXT("Score")) == double(float(MAX_int32)) &&
                                                    ConvertWidenValue(B, TEXT("Rate")) == -13,
                                                                      TEXT("float_fields_explicit_round_persist"));
        Check(NumericAdapterCount(Parent) >= 3 && A && ConvertGraphRead(A) == MIN_int32,
              TEXT("float_graph_links_adapt_and_execute"));
        if (Phase == TEXT("NarrowFloat"))
            Check(ConvertSaveWidenAsset(Parent) && ConvertSaveWidenAsset(Child) && ConvertSaveWidenAsset(World),
                  TEXT("save_float_scope"));
        if (World && World->IsInitialized())
            World->DestroyWorld(false);
        UE_LOG(LogTemp, Display, TEXT("DAS_NUMERIC_CONVERT_RESULT=%s"), Passed ? TEXT("PASS"): TEXT("FAIL"));
        return Passed ? 0 : 1;
    }
    if (Phase == TEXT("Create"))
    {
        FString Source;
        Check(FFileHelper::LoadFileToString(
            Source, *Runtime.ScriptPath(TEXT("dsl_widen_numeric.das"))) &&
            FFileHelper::SaveStringToFile(Source, *Runtime.ScriptPath(Script),
                                          FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM),
            TEXT("create_widen_source"));
        Check(FDasDSLService::Apply(Script, Base + TEXT("BP_Parent"), TEXT("widen.WidenFields"), Parent, Error),
                                                TEXT("create_widen_parent"));
        if (!Parent)
            return 1;
        ConvertGraphs(Parent);
        Child = FKismetEditorUtilities::CreateBlueprint(
            Parent->GeneratedClass, CreatePackage(*(Base + TEXT("BP_Child"))),
                                                  TEXT("BP_Child"), BPTYPE_Normal, UBlueprint::StaticClass(),
                                                       UBlueprintGeneratedClass::StaticClass(), TEXT("DasWidenProbe"));
        FAssetRegistryModule::AssetCreated(Child);
        SetConvertWidenValues(Child->GeneratedClass->GetDefaultObject(), 23, 2.25f);
        World = UWorld::CreateWorld(EWorldType::Editor, false, TEXT("Map"), CreatePackage(*(Base + TEXT("Map"))));
        World->SetFlags(RF_Public | RF_Standalone);
        World->GetOutermost()->SetPackageFlags(PKG_ContainsMap);
        FAssetRegistryModule::AssetCreated(World);
        FActorSpawnParameters Spawn;
        Spawn.Name = TEXT("ParentActor");
        SetConvertWidenValues(World->SpawnActor<ADasActor>(Parent->GeneratedClass, FTransform::Identity, Spawn),
                              MIN_int32, 0.1f);
        Spawn.Name = TEXT("ChildActor");
        SetConvertWidenValues(World->SpawnActor<ADasActor>(Child->GeneratedClass, FTransform::Identity, Spawn),
                              MAX_int32, -13.25f);
        Check(ConvertSaveWidenAsset(Parent) && ConvertSaveWidenAsset(Child) && ConvertSaveWidenAsset(World),
              TEXT("save_original_numeric_scope"));
    }
    else if (Phase == TEXT("Migrate") || Phase == TEXT("Reopen"))
    {
        Parent = ConvertWidenBP(Base, TEXT("BP_Parent"));
        if (!Parent)
            return 1;
        if (Phase == TEXT("Migrate"))
        {
            Check(!FindObject<UBlueprint>(nullptr, *(Base + TEXT("BP_Child.BP_Child"))) &&
                                          !FindObject<UWorld>(nullptr, *(Base + TEXT("Map.Map"))),
                                                              TEXT("numeric_dependencies_initially_unloaded"));
            FString Source;
            FFileHelper::LoadFileToString(Source, *Runtime.ScriptPath(Script));
            const FString Changed = Source.Replace(
                TEXT("Score : int = 5"),
                     TEXT("Score : double = 5.0lf")).Replace(TEXT("Rate : float = 1.5"), TEXT("Rate : int = 2"));
            auto Write = [&](const FString& Text)
            {
                return FFileHelper::SaveStringToFile(Text, *Runtime.ScriptPath(Script),
                                                     FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM);
            };
            Check(Write(Changed), TEXT("write_numeric_widening"));
            UClass* Before = Parent->GeneratedClass;
            Check(!FDasDSLService::Reimport(Parent, Error) && Parent->GeneratedClass == Before,
                  TEXT("implicit_type_change_refused"));
            Check(!FDasDSLService::ReimportWidenedNumericFields(Parent, Error) && Parent->GeneratedClass == Before,
                  TEXT("isolated_widening_refuses_saved_dependencies"));
            Check(Write(Changed.Replace(TEXT("Rate : int = 2"), TEXT("Rate : int = 3"))) &&
                                        !FDasDSLService::ReimportConvertedNumericFields(Parent, Options, Error) &&
                                        Parent->GeneratedClass == Before,
                        TEXT("combined_default_change_refused"));
            Write(Changed);
            World = LoadObject<UWorld>(nullptr, *(Base + TEXT("Map.Map")));
            auto* A = ConvertWidenActor(World, TEXT("ParentActor"));
            auto* Rate = FindFProperty<FFloatProperty>(A->GetClass(), TEXT("Rate"));
            Rate->SetPropertyValue_InContainer(A, std::numeric_limits<float>::quiet_NaN());
            Check(!FDasDSLService::ReimportConvertedNumericFields(Parent, Options, Error) &&
                      Parent->GeneratedClass == Before,
                  TEXT("nonfinite_instance_refused_before_mutation"));
            Rate->SetPropertyValue_InContainer(A, 0.1f);
            const FString OldModel = Parent->GetOutermost()->GetMetaData().GetValue(Parent, TEXT("DasDSLModel"));
#if WITH_DEV_AUTOMATION_TESTS
            DasFailWidenAfterCompileForTesting(true);
            const bool Failed = !FDasDSLService::ReimportConvertedNumericFields(Parent, Options, Error);
            DasFailWidenAfterCompileForTesting(false);
            Check(Failed && Error.StartsWith(TEXT("Numeric migration rolled back:")) &&
                                             FindFProperty<FIntProperty>(
                                                 Parent->GeneratedClass, TEXT("Score")) &&
                                                 FindFProperty<FFloatProperty>(
                                                     Parent->GeneratedClass, TEXT("Rate")) &&
                                                     Parent->GetOutermost()->GetMetaData().GetValue(
                                                         Parent, TEXT("DasDSLModel")) == OldModel,
                                                         TEXT("post_compile_failure_rolls_back_types_and_model"));
            A = ConvertWidenActor(World, TEXT("ParentActor"));
            auto* B = ConvertWidenActor(World, TEXT("ChildActor"));
            Child = ConvertWidenBP(Base, TEXT("BP_Child"));
            Check(ConvertWidenValue(
                A, TEXT("Score")) == MIN_int32 &&
                ConvertWidenValue(A, TEXT("Rate")) == static_cast<double>(0.1f) &&
                                  ConvertWidenValue(B, TEXT("Score")) == MAX_int32 && Child &&
                                                    ConvertWidenValue(
                                                        Child->GeneratedClass->GetDefaultObject(), TEXT("Score")) == 23,
                                                        TEXT("rollback_preserves_instances_and_child_CDO"));
            Check(NumericAdapterCount(Parent) == 0 && ConvertGraphRead(A) == MIN_int32,
                  TEXT("rollback_restores_nodes_pins_and_connections"));
#endif
            const bool Migrated = FDasDSLService::ReimportConvertedNumericFields(Parent, Options, Error);
            Check(Migrated, TEXT("exact_numeric_migration"));
            if (!Migrated)
            {
                UE_LOG(LogTemp, Error, TEXT("Convert: %s"), *Error);
                return 1;
            }
        }
        Child = ConvertWidenBP(Base, TEXT("BP_Child"));
        World = LoadObject<UWorld>(nullptr, *(Base + TEXT("Map.Map")));
        Check(Child && World &&
              FindFProperty<FDoubleProperty>(Parent->GeneratedClass, TEXT("Score")) &&
                                             FindFProperty<FIntProperty>(Parent->GeneratedClass, TEXT("Rate")),
                                                                         TEXT("double_fields_persist_on_reopen"));
        Check(ConvertWidenValue(Parent->GeneratedClass->GetDefaultObject(), TEXT("Score")) == 5 &&
                                ConvertWidenValue(
                                    Parent->GeneratedClass->GetDefaultObject(), TEXT("Rate")) == 2 && Child &&
                                    ConvertWidenValue(Child->GeneratedClass->GetDefaultObject(), TEXT("Score")) == 23 &&
                                                      ConvertWidenValue(Child->GeneratedClass->GetDefaultObject(),
                                                                        TEXT("Rate")) == 2,
                                                                        TEXT("promoted_root_child_defaults_exact"));
        auto* A = ConvertWidenActor(World, TEXT("ParentActor"));
        auto* B = ConvertWidenActor(World, TEXT("ChildActor"));
        Check(ConvertWidenValue(
            A, TEXT("Score")) == MIN_int32 &&
            ConvertWidenValue(A, TEXT("Rate")) == 0 &&
                              ConvertWidenValue(B, TEXT("Score")) == MAX_int32 &&
                                                ConvertWidenValue(B, TEXT("Rate")) == -13,
                                                                  TEXT("promoted_map_extremes_and_fraction_exact"));
        if (B)
        {
            auto* F = B->FindFunction(TEXT("ReadScore"));
            FStructOnScope Frame(F);
            FEditorScriptExecutionGuard Guard;
            B->ProcessEvent(F, Frame.GetStructMemory());
            Check(CastField<FDoubleProperty>(F->GetReturnProperty())
                          ->GetPropertyValue_InContainer(Frame.GetStructMemory()) == MAX_int32,
                  TEXT("inherited_DSL_reads_promoted_value"));
        }
        Check(NumericAdapterCount(Parent) >= 2 && A && ConvertGraphRead(A) == MIN_int32,
              TEXT("checked_graph_adapters_execute_and_persist"));
        if (Phase == TEXT("Migrate") && A)
        {
            auto* Setter = A->FindFunction(TEXT("WriteByGraph"));
            FStructOnScope P(Setter);
            FindFProperty<FIntProperty>(Setter, TEXT("Input"))->SetPropertyValue_InContainer(P.GetStructMemory(), 42);
            FEditorScriptExecutionGuard Guard;
            A->ProcessEvent(Setter, P.GetStructMemory());
            Check(ConvertWidenValue(A, TEXT("Score")) == 42, TEXT("setter_incoming_numeric_adapter"));
            FindFProperty<FDoubleProperty>(A->GetClass(),
                                           TEXT("Score"))->SetPropertyValue_InContainer(A, double(MIN_int32));
            A->ProcessEvent(A->FindFunction(TEXT("SetLiteralRate")), nullptr);
            Check(ConvertWidenValue(A, TEXT("Rate")) == -2, TEXT("setter_literal_uses_explicit_rounding"));
            FindFProperty<FIntProperty>(A->GetClass(), TEXT("Rate"))->SetPropertyValue_InContainer(A, 0);
        }
        if (Phase == TEXT("Migrate"))
            Check(ConvertSaveWidenAsset(Parent) && ConvertSaveWidenAsset(Child) && ConvertSaveWidenAsset(World),
                  TEXT("save_promoted_scope"));
        UClass* Before = Parent->GeneratedClass;
        Check(FDasDSLService::ReimportConvertedNumericFields(Parent, Options, Error) &&
                  Parent->GeneratedClass == Before,
              TEXT("widen_repeat_is_idempotent"));
    }
    else
        return 1;
    if (World && World->IsInitialized())
        World->DestroyWorld(false);
    UE_LOG(LogTemp, Display, TEXT("DAS_NUMERIC_CONVERT_RESULT=%s"), Passed ? TEXT("PASS"): TEXT("FAIL"));
    return Passed ? 0 : 1;
}
