#include "DasTypeService.h"
#include "DasTypeRegistry.h"
#include "DasRuntime.h"
#include "DasActor.h"
#include "DasNumericLibrary.h"
#include "StructUtils/UserDefinedStruct.h"
#include "UserDefinedStructure/UserDefinedStructEditorData.h"
#include "Kismet2/StructureEditorUtils.h"
#include "Kismet2/KismetEditorUtilities.h"
#include "K2Node_MakeStruct.h"
#include "K2Node_CallFunction.h"
#include "Engine/Blueprint.h"
#include "Engine/World.h"
#include "Engine/Level.h"
#include "UObject/StructOnScope.h"
#include "Misc/Paths.h"
#include "Misc/FileHelper.h"
#include "Editor.h"
#include <limits>
void DasFailStructMigrationForTesting(int32);
bool RunDasStructNumericProbe(const FString& Script, UDasTypeRegistry* Registry, UBlueprint* BP, UWorld* World,
                              bool Create, FDasRuntimeModule& R)
{
    bool Pass = true;
    FString Error;
    auto Check = [&](bool V, const TCHAR* Name)
    {
        Pass &= V;
        UE_LOG(LogTemp, Display, TEXT("DAS_STRUCT_NUMERIC %s %s"), V ? TEXT("PASS"): TEXT("FAIL"), Name);
        if (!V)
            UE_LOG(LogTemp, Display, TEXT("Numeric struct detail: %s"), *Error);
    };
    auto* Type = CastChecked<UUserDefinedStruct>(Registry->Resolve(TEXT("direct.Settings"), false, Error));
    ADasActor* Actor = nullptr;
    auto Refresh = [&]()
    {
        for (auto& V : World->PersistentLevel->Actors)
            if (V && V->GetFName() == TEXT("TypedActor"))
                Actor = Cast<ADasActor>(V.Get());
    };
    Refresh();
    if (!Actor)
        return false;
    R.ReleaseObject(Actor);
    auto Memory = [&](UObject* O)
    {
        return FindFProperty<FStructProperty>(O->GetClass(), TEXT("Config"))->ContainerPtrToValuePtr<void>(O);
    };
    auto Read = [&](UObject* O)
    {
        auto* P = CastFieldChecked<FNumericProperty>(FindDasStructField(Type, TEXT("Score")));
        const void* V = P->ContainerPtrToValuePtr<void>(Memory(O));
        return P->IsInteger() ? double(P->GetSignedIntPropertyValue(V)) : P->GetFloatingPointPropertyValue(V);
    };
    auto Set = [&](double V)
    {
        auto* P = CastFieldChecked<FNumericProperty>(FindDasStructField(Type, TEXT("Score")));
        void* M = P->ContainerPtrToValuePtr<void>(Memory(Actor));
        if (P->IsInteger())
            P->SetIntPropertyValue(M, int64(V));
        else
            P->SetFloatingPointPropertyValue(M, V);
    };
    auto RunRead = [&]()
    {
        auto* F = Actor->FindFunction(TEXT("ReadConfigCount"));
        FStructOnScope A(F);
        {
            FEditorScriptExecutionGuard G;
            Actor->ProcessEvent(F, A.GetStructMemory());
        }
        return CastFieldChecked<FIntProperty>(F->GetReturnProperty())
            ->GetPropertyValue_InContainer(A.GetStructMemory());
    };
    auto MakeDefault = [&]()
    {
        auto* F = Actor->FindFunction(TEXT("MakeConfig"));
        FStructOnScope A(F);
        {
            FEditorScriptExecutionGuard G;
            Actor->ProcessEvent(F, A.GetStructMemory());
        }
        auto* S = CastFieldChecked<FStructProperty>(F->GetReturnProperty());
        auto* P = CastFieldChecked<FNumericProperty>(FindDasStructField(Type, TEXT("Score")));
        const void* V = P->ContainerPtrToValuePtr<void>(S->ContainerPtrToValuePtr<void>(A.GetStructMemory()));
        return P->IsInteger() ? double(P->GetSignedIntPropertyValue(V)) : P->GetFloatingPointPropertyValue(V);
    };
    auto MakeInput = [&]()
    {
        auto* F = Actor->FindFunction(TEXT("MakeConfigFromInput"));
        FStructOnScope A(F);
        FindFProperty<FIntProperty>(F, TEXT("Input"))->SetPropertyValue_InContainer(A.GetStructMemory(), 91);
        {
            FEditorScriptExecutionGuard G;
            Actor->ProcessEvent(F, A.GetStructMemory());
        }
        auto* S = CastFieldChecked<FStructProperty>(F->GetReturnProperty());
        auto* P = CastFieldChecked<FNumericProperty>(FindDasStructField(Type, TEXT("Score")));
        const void* V = P->ContainerPtrToValuePtr<void>(S->ContainerPtrToValuePtr<void>(A.GetStructMemory()));
        return P->IsInteger() ? double(P->GetSignedIntPropertyValue(V)) : P->GetFloatingPointPropertyValue(V);
    };
    auto AdapterCount = [&]()
    {
        int32 Count = 0;
        for (UEdGraph* G : BP->FunctionGraphs)
            for (UEdGraphNode* N : G->Nodes)
                if (auto* C = Cast<UK2Node_CallFunction>(N))
                    if (C->GetTargetFunction() &&
                        C->GetTargetFunction()->GetOwnerClass() == UDasNumericLibrary::StaticClass())
                        ++Count;
        return Count;
    };
    if (!Create)
    {
        Check(FindDasStructField(Type, TEXT("Score"))->IsA<FFloatProperty>() && Read(Actor) == 28 &&
                                 Read(BP->GeneratedClass->GetDefaultObject()) == 15 && RunRead() == 28 &&
                                 MakeDefault() == 88 && MakeInput() == 91 && AdapterCount() > 0,
                                 TEXT("reopened_numeric_layout_values_CDO_and_graph_adapters"));
        Check(FDasTypeService::MigrateStructFields(Script, Registry->GetPathName(), Error),
              TEXT("reopened_numeric_idempotent"));
        return Pass;
    }
    FString Source;
    FFileHelper::LoadFileToString(Source, *R.ScriptPath(Script));
    const FString Rules = FPaths::GetBaseFilename(Script) + TEXT("_values.json");
    auto Write = [&](const FString& S)
    {
        return FFileHelper::SaveStringToFile(S, *R.ScriptPath(Script),
                                             FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM);
    };
    auto Policy = [&](const TCHAR* I, const TCHAR* F)
    {
        return FFileHelper::SaveStringToFile(FString::Printf(
            TEXT("{\"schema_version\":1,\"rules\":[{\"field_id\":\"direct.Settings.Count\",\"operation\":\"convert\",\"integer_rule\":\"%s\",\"float_rule\":\"%s\"}]}"),
                I, F),
            *R.ScriptPath(Rules), FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM);
    };
    auto Typed = [&](const TCHAR* K)
    {
        if (FString(K) == TEXT("int"))
            return Source;
        return Source.Replace(
            TEXT("Score : int = 15"),
                *(FString(
                    TEXT("Score : ")) + K +
                    (FString(K) == TEXT("double")? TEXT(" = 15.0lf"): TEXT(" = 15.0"))))
                        .Replace(TEXT("result.Score += int(mode)"),
                            *(FString(TEXT("result.Score += ")) + K + TEXT("(int(mode))")))
                                .Replace(TEXT("self.Config.Score += amount"),
                                    *(FString(TEXT("self.Config.Score += ")) + K + TEXT("(amount)")))
                                        .Replace(TEXT("return self.Config.Score"),
                                            TEXT("return int(self.Config.Score)"))
                                                .Replace(TEXT("return ue_struct_get_int(self.Bundle, \"Items.0.Inner.Score\")"),
                                                    *(FString(TEXT("return int(ue_struct_get_")) + K +
                                                              TEXT("(self.Bundle, \"Items.0.Inner.Score\"))")))
                                                        .Replace(TEXT("ue_struct_with_int(self.Bundle, \"Items.0.Inner.Score\", value)"),
                                                            *(FString(TEXT("ue_struct_with_")) + K +
                                                                      TEXT("(self.Bundle, \"Items.0.Inner.Score\", ")+
                                                                           K + TEXT("(value))")));
    };
    const FGuid Guid = Type->GetCustomGuid();
    const FGuid Member = FStructureEditorUtils::GetVarDesc(Type)[0].VarGuid;
    const FString Double = Typed(TEXT("double"));
    Check(Write(Double) && !FDasTypeService::MigrateStructFields(Script, Registry->GetPathName(), Error),
          TEXT("numeric_type_change_without_rule_refused"));
    Check(Policy(TEXT("Exact"), TEXT("Exact")), TEXT("write_exact_conversion_policy"));
    Check(Write(Double.Replace(TEXT("Score : double = 15.0"), TEXT("Score : double = 16.0"))) &&
                !FDasTypeService::MigrateStructFields(Script, Registry->GetPathName(), Error, Rules) &&
                Error.Contains(TEXT("default")), TEXT("independent_default_change_refused"));
    Check(Write(Double), TEXT("restore_numeric_candidate"));
    const bool Dirty = World->GetOutermost()->IsDirty();
    const int32 BeforeAdapters = AdapterCount();
    for (int32 Stage = 1; Stage <= 4; ++Stage)
    {
        DasFailStructMigrationForTesting(Stage);
        const bool Failed = !FDasTypeService::MigrateStructFields(Script, Registry->GetPathName(), Error, Rules);
        DasFailStructMigrationForTesting(0);
        Refresh();
        Check(Failed &&
              Error.StartsWith(TEXT("Struct migration rolled back:")) &&
                               FindDasStructField(
                                   Type, TEXT("Score"))->IsA<FIntProperty>() && Read(Actor) == 27 && RunRead() == 27 &&
                                   MakeDefault() == 88 && MakeInput() == 91 && AdapterCount() == BeforeAdapters &&
                                   Type->GetCustomGuid() == Guid && World->GetOutermost()->IsDirty() == Dirty,
                                   TEXT("numeric_rollback_layout_values_defaults_links_adapters_and_dirty_flags"));
    }
    Check(FDasTypeService::MigrateStructFields(Script, Registry->GetPathName(), Error, Rules),
          TEXT("int_to_double_success"));
    Refresh();
    Check(FindDasStructField(Type, TEXT("Score"))->IsA<FDoubleProperty>() && Read(Actor) == 27 &&
                             Read(BP->GeneratedClass->GetDefaultObject()) == 15 && RunRead() == 27 &&
                             MakeDefault() == 88 && MakeInput() == 91 && AdapterCount() > BeforeAdapters &&
                             FStructureEditorUtils::GetVarDesc(Type)[0].VarGuid == Member,
                             TEXT("widening_preserves_ID_GUID_data_and_checked_graph"));
    Set(27.75);
    Check(Write(Typed(TEXT("int"))) &&
                Policy(TEXT("Exact"), TEXT("Exact")) &&
                            !FDasTypeService::MigrateStructFields(Script, Registry->GetPathName(), Error, Rules) &&
                            Error.Contains(TEXT("Non-integral")) && Read(Actor) == 27.75,
                                           TEXT("fractional_value_exact_refused_before_mutation"));
    Set(2147483648.0);
    Check(!FDasTypeService::MigrateStructFields(Script, Registry->GetPathName(), Error, Rules) &&
          Error.Contains(TEXT("range")), TEXT("int32_overflow_refused"));
    Set(std::numeric_limits<double>::infinity());
    Check(!FDasTypeService::MigrateStructFields(Script, Registry->GetPathName(), Error, Rules) &&
          Error.Contains(TEXT("Non-finite")), TEXT("nonfinite_value_refused"));
    Set(27.123456789012345);
    Check(Policy(TEXT("Ceil"), TEXT("Exact")), TEXT("write_explicit_integer_rounding"));
    const int32 DoubleAdapters = AdapterCount();
    for (int32 Stage = 1; Stage <= 4; ++Stage)
    {
        DasFailStructMigrationForTesting(Stage);
        const bool Failed = !FDasTypeService::MigrateStructFields(Script, Registry->GetPathName(), Error, Rules);
        DasFailStructMigrationForTesting(0);
        Refresh();
        Check(Failed &&
              Error.StartsWith(TEXT("Struct migration rolled back:")) &&
                               FindDasStructField(
                                   Type, TEXT("Score"))->IsA<FDoubleProperty>() && Read(Actor) == 27.123456789012345 &&
                                   Read(BP->GeneratedClass->GetDefaultObject()) == 15 && MakeDefault() == 88 &&
                                   MakeInput() == 91 && AdapterCount() == DoubleAdapters,
                                   TEXT("narrowing_rollback_restores_full_double_precision_and_adapters"));
    }
    Set(27.75);
    Check(Policy(TEXT("Ceil"), TEXT("Exact")) &&
                      FDasTypeService::MigrateStructFields(Script, Registry->GetPathName(), Error, Rules),
                 TEXT("double_to_int_explicit_Ceil_success"));
    Refresh();
    Check(Read(Actor) == 28 && RunRead() == 28 && MakeDefault() == 88, TEXT("rounded_values_and_composed_adapters"));
    Set(16777217);
    Check(Write(Typed(TEXT("float"))) &&
                Policy(TEXT("Exact"), TEXT("Exact")) &&
                            !FDasTypeService::MigrateStructFields(Script, Registry->GetPathName(), Error, Rules) &&
                            Error.Contains(TEXT("precision")), TEXT("float32_precision_loss_exact_refused"));
    Check(Policy(TEXT("Exact"), TEXT("RoundNearest")) &&
                      FDasTypeService::MigrateStructFields(Script, Registry->GetPathName(), Error, Rules),
                 TEXT("int_to_float_explicit_rounding_success"));
    Refresh();
    Check(Read(Actor) == 16777216, TEXT("float32_RoundNearest_value"));
    Set(28);
    Check(Type->GetCustomGuid() == Guid && FStructureEditorUtils::GetVarDesc(Type)[0].VarGuid == Member &&
          Read(BP->GeneratedClass->GetDefaultObject()) == 15 && RunRead() == 28 && MakeDefault() == 88 &&
          MakeInput() == 91 &&
          CastFieldChecked<FStrProperty>(
              FindDasStructField(Type, TEXT("Title")))->GetPropertyValue_InContainer(Memory(Actor)) ==
              TEXT("Saved 世界"), TEXT("final_float_layout_identity_CDO_Unicode_and_graph"));
    FDasScalarValue Amount;
    Amount.Kind = 0;
    Amount.IntValue = 3;
    auto Value = R.RunNamedFramePersistent(R.ScriptPath(Script), TEXT("ue_TypedActor_Bump"), {Amount},
                                                                      EDasMethodSignature::Int, Actor);
    Error = Value.Error;
    Check(Value.bSucceeded && Value.Value == 31 && Read(Actor) == 31, TEXT("updated_float_DSL_leaf_write"));
    R.ReleaseObject(Actor);
    Set(28);
    Check(!FDasTypeService::MigrateStructFields(Script, Registry->GetPathName(), Error, Rules) &&
          Error.Contains(TEXT("Unused")), TEXT("stale_convert_rule_refused"));
    Check(FDasTypeService::MigrateStructFields(Script, Registry->GetPathName(), Error),
          TEXT("numeric_idempotent_without_stale_rules"));
    return Pass;
}
