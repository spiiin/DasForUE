#include "DasTypeService.h"
#include "DasTypeRegistry.h"
#include "DasDefinedEnum.h"
#include "DasReflectedTypes.h"
#include "DasRuntime.h"
#include "Dom/JsonObject.h"
#include "Serialization/JsonSerializer.h"
#include "Misc/FileHelper.h"
#include "DasBlueprintRollback.h"
#include "DasActor.h"
#include "K2Node_SwitchEnum.h"
#include "K2Node_Select.h"
#include "K2Node_EnumLiteral.h"
#include "K2Node_GetNumEnumEntries.h"
#include "K2Node_CastByteToEnum.h"
#include "K2Node_ForEachElementInEnum.h"
#include "Kismet2/KismetEditorUtilities.h"
#include "BlueprintCompilationManager.h"
#include "Misc/ScopeExit.h"
#include "Engine/Blueprint.h"
#include "Engine/World.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "NodeDependingOnEnumInterface.h"
#include "K2Node_CallFunction.h"
#include "K2Node_Variable.h"
#include "K2Node_FunctionEntry.h"
#include "K2Node_FunctionResult.h"
#include "K2Node_Knot.h"
#include "EdGraph/EdGraph.h"
#include "EdGraph/EdGraphPin.h"
#include "StructUtils/UserDefinedStruct.h"
#include "Kismet2/StructureEditorUtils.h"
#include "UserDefinedStructure/UserDefinedStructEditorData.h"
#include "UObject/UObjectIterator.h"
#include "UObject/MetaData.h"
#include "UObject/StrProperty.h"
#include "UObject/StrongObjectPtr.h"
#include "Editor.h"
#include "Misc/Paths.h"
#include "Misc/PackageName.h"
#include "Modules/ModuleManager.h"
namespace
{
#if DAS_WITH_EDITOR_TESTS
int32 EnumAppendFailStage = 0;
#else
constexpr int32 EnumAppendFailStage = 0;
#endif
void SortTypes(FDasReflectedTypes& M)
{
    M.Enums.Sort([](const auto& A, const auto& B) { return A.Id < B.Id; });
    M.Structs.Sort([](const auto& A, const auto& B) { return A.Id < B.Id; });
}
struct FEnumImage
{
    UDasDefinedEnum* Type = nullptr;
    TArray<TPair<FName, int64>> Entries;
    TMap<FName, FString> Metadata;
    TMap<int64, FString> MemberIds;
};
bool GatherEnumScope(const TSet<UEnum*>& Types, TArray<TStrongObjectPtr<UObject>>& Assets, FString& Error)
{
    auto& AR = FModuleManager::LoadModuleChecked<FAssetRegistryModule>(TEXT("AssetRegistry")).Get();
    if (!AR.IsSearchAllAssets())
        AR.SearchAllAssets(true);
    else
        AR.WaitForCompletion();
    if (AR.IsLoadingAssets())
    {
        Error = TEXT("Finish asset scan before enum migration");
        return false;
    }
    TSet<FName> Seen;
    TArray<FName> Queue;
    for (auto* E : Types)
    {
        Seen.Add(E->GetOutermost()->GetFName());
        Queue.Add(E->GetOutermost()->GetFName());
    }
    for (int32 I = 0; I < Queue.Num(); ++I)
    {
        TArray<FName> Refs;
        AR.GetReferencers(Queue[I], Refs);
        for (FName P : Refs)
        {
            if (Seen.Contains(P))
                continue;
            if (Queue.Num() >= 256 || !P.ToString().StartsWith(TEXT("/Game/")))
            {
                Error = TEXT("Enum migration exceeds ordinary /Game scope");
                return false;
            }
            TArray<FAssetData> Data;
            AR.GetAssetsByPackageName(P, Data);
            if (Data.Num() != 1 || (Data[0].AssetClassPath != UBlueprint::StaticClass()->GetClassPathName() &&
                                    Data[0].AssetClassPath != UWorld::StaticClass()->GetClassPathName() &&
                                    Data[0].AssetClassPath != UDasTypeRegistry::StaticClass()->GetClassPathName()))
            {
                Error = TEXT("Enum migration supports ordinary Blueprint/map/catalog dependencies only");
                return false;
            }
            auto* A = Data[0].GetAsset();
            if (!A)
            {
                Error = TEXT("Could not load enum dependency");
                return false;
            }
            Assets.Emplace(A);
            Seen.Add(P);
            Queue.Add(P);
        }
    }
    // Include unsaved loaded Blueprints: they are absent from package dependency data.
    for (TObjectIterator<UBlueprint> It; It; ++It)
        if (!It->HasAnyFlags(RF_Transient | RF_ClassDefaultObject) &&
            It->GetOutermost()->GetName().StartsWith(TEXT("/Game/")))
            Assets.AddUnique(TStrongObjectPtr<UObject>(*It));
    for (const auto& A : Assets)
        if (auto* W = Cast<UWorld>(A.Get()))
            if (W->IsPartitionedWorld() || !W->GetStreamingLevels().IsEmpty())
            {
                Error = TEXT("Partitioned/streaming enum dependencies require a separate scope");
                return false;
            }
    return true;
}
bool PropertyUsesEnum(FProperty* P, const TSet<UEnum*>& Types, int32 Depth = 0)
{
    if (Depth > 16)
        return true;
    if (auto* B = CastField<FByteProperty>(P))
        return Types.Contains(B->Enum);
    if (auto* E = CastField<FEnumProperty>(P))
        return Types.Contains(E->GetEnum());
    if (auto* S = CastField<FStructProperty>(P))
    {
        if (S->Struct == FInstancedStruct::StaticStruct() || S->Struct == FDasScalarValue::StaticStruct())
            return true;
        for (TFieldIterator<FProperty> It(S->Struct); It; ++It)
            if (PropertyUsesEnum(*It, Types, Depth + 1))
                return true;
    }
    else if (auto* A = CastField<FArrayProperty>(P))
        return PropertyUsesEnum(A->Inner, Types, Depth + 1);
    else if (auto* M = CastField<FMapProperty>(P))
        return PropertyUsesEnum(M->KeyProp, Types, Depth + 1) || PropertyUsesEnum(M->ValueProp, Types, Depth + 1);
    else if (auto* Set = CastField<FSetProperty>(P))
        return PropertyUsesEnum(Set->ElementProp, Types, Depth + 1);
    return false;
}
bool StaticEnumStorage(FProperty* P, const TSet<UEnum*>& Types, int32 Depth = 0)
{
    if (Depth > 16)
        return true;
    if (auto* B = CastField<FByteProperty>(P))
        return Types.Contains(B->Enum);
    if (auto* E = CastField<FEnumProperty>(P))
        return Types.Contains(E->GetEnum());
    if (auto* S = CastField<FStructProperty>(P))
    {
        for (TFieldIterator<FProperty> It(S->Struct); It; ++It)
            if (StaticEnumStorage(*It, Types, Depth + 1))
                return true;
    }
    else if (auto* A = CastField<FArrayProperty>(P))
        return StaticEnumStorage(A->Inner, Types, Depth + 1);
    else if (auto* M = CastField<FMapProperty>(P))
        return StaticEnumStorage(M->KeyProp, Types, Depth + 1) || StaticEnumStorage(M->ValueProp, Types, Depth + 1);
    else if (auto* Set = CastField<FSetProperty>(P))
        return StaticEnumStorage(Set->ElementProp, Types, Depth + 1);
    return false;
}
bool CheckEnumData(FProperty* P, void* Memory, const TMap<UEnum*, TSet<int64>>& Values, int32& Budget, int32 Depth = 0)
{
    if (Depth > 16 || --Budget < 0)
        return false;
    if (auto* B = CastField<FByteProperty>(P))
    {
        if (const auto* V = Values.Find(B->Enum))
            return V->Contains(B->GetPropertyValue(Memory));
        return true;
    }
    if (auto* E = CastField<FEnumProperty>(P))
    {
        if (const auto* V = Values.Find(E->GetEnum()))
            return V->Contains(E->GetUnderlyingProperty()->GetSignedIntPropertyValue(Memory));
        return true;
    }
    auto Read = [&](FProperty* F, void* Base)
    {
        for (int32 I = 0; I < F->ArrayDim; ++I)
            if (!CheckEnumData(F, F->ContainerPtrToValuePtr<void>(Base, I), Values, Budget, Depth + 1))
                return false;
        return true;
    };
    if (auto* S = CastField<FStructProperty>(P))
    {
        if (S->Struct == FInstancedStruct::StaticStruct())
        {
            auto* V = static_cast<FInstancedStruct*>(Memory);
            if (V->IsValid())
                for (TFieldIterator<FProperty> It(V->GetScriptStruct()); It; ++It)
                    if (!Read(*It, V->GetMutableMemory()))
                        return false;
        }
        else
        {
            if (S->Struct == FDasScalarValue::StaticStruct())
            {
                const auto* V = static_cast<FDasScalarValue*>(Memory);
                if (V->Kind == 13)
                    if (const auto* Allowed = Values.Find(FindObject<UEnum>(nullptr, *V->ObjectClass)))
                        if (!Allowed->Contains(V->IntValue))
                            return false;
            }
            for (TFieldIterator<FProperty> It(S->Struct); It; ++It)
                if (!Read(*It, Memory))
                    return false;
        }
    }
    else if (auto* A = CastField<FArrayProperty>(P))
    {
        FScriptArrayHelper H(A, Memory);
        for (int32 I = 0; I < H.Num(); ++I)
            if (!CheckEnumData(A->Inner, H.GetRawPtr(I), Values, Budget, Depth + 1))
                return false;
    }
    else if (auto* M = CastField<FMapProperty>(P))
    {
        FScriptMapHelper H(M, Memory);
        for (int32 I = 0; I < H.GetMaxIndex(); ++I)
            if (H.IsValidIndex(I) && (!CheckEnumData(M->KeyProp, H.GetKeyPtr(I), Values, Budget, Depth + 1) ||
                                      !CheckEnumData(M->ValueProp, H.GetValuePtr(I), Values, Budget, Depth + 1)))
                return false;
    }
    else if (auto* Set = CastField<FSetProperty>(P))
    {
        FScriptSetHelper H(Set, Memory);
        for (int32 I = 0; I < H.GetMaxIndex(); ++I)
            if (H.IsValidIndex(I) && !CheckEnumData(Set->ElementProp, H.GetElementPtr(I), Values, Budget, Depth + 1))
                return false;
    }
    return true;
}
bool SupportedEnumNode(UEdGraphNode* N)
{
    return N->IsA<UK2Node_SwitchEnum>() || N->IsA<UK2Node_Select>() || N->IsA<UK2Node_EnumLiteral>() ||
           N->IsA<UK2Node_GetNumEnumEntries>() ||
           (N->IsA<UK2Node_CastByteToEnum>() && CastChecked<UK2Node_CastByteToEnum>(N)->bSafe) ||
           N->IsA<UK2Node_ForEachElementInEnum>();
}
void RebuildEnumNode(UK2Node* N)
{
    auto* D = CastChecked<INodeDependingOnEnumInterface>(N);
    D->ReloadEnum(D->GetEnum());
    N->ReconstructNode();
}
bool CheckBlueprint(UBlueprint* BP, const TSet<UEnum*>& Types, const FString& Script, FString& Error, bool RepairGraphs,
                    TArray<UK2Node*>& EnumNodes, TSet<UBlueprint*>& RepairBPs)
{
    bool Uses = false;
    for (const auto& V : BP->NewVariables)
        if (Types.Contains(Cast<UEnum>(V.VarType.PinSubCategoryObject.Get())))
            Uses = true;
    TArray<UEdGraph*> Graphs;
    BP->GetAllGraphs(Graphs);
    for (auto* G : Graphs)
        for (UEdGraphNode* N : G->Nodes)
        {
            if (auto* D = Cast<INodeDependingOnEnumInterface>(N))
                if (Types.Contains(D->GetEnum()))
                {
                    Uses = true;
                    if (RepairGraphs && SupportedEnumNode(N))
                    {
                        EnumNodes.Add(CastChecked<UK2Node>(N));
                        RepairBPs.Add(BP);
                    }
                    else
                    {
                        Error = TEXT("Enum-specific node reconstruction is not supported in append migration: ")+
                                     N->GetClass()->GetName();
                        return false;
                    }
                }
            for (auto* Pin : N->Pins)
                if (Pin && Types.Contains(Cast<UEnum>(Pin->PinType.PinSubCategoryObject.Get())))
                {
                    Uses = true;
                    if (!N->IsA<UK2Node_CallFunction>() && !N->IsA<UK2Node_Variable>() &&
                        !N->IsA<UK2Node_FunctionEntry>() && !N->IsA<UK2Node_FunctionResult>() &&
                        !N->IsA<UK2Node_Knot>() && !(RepairGraphs && SupportedEnumNode(N)))
                    {
                        Error = TEXT("Unsupported enum-dependent graph node");
                        return false;
                    }
                    auto* E = CastChecked<UEnum>(Pin->PinType.PinSubCategoryObject.Get());
                    if (!Pin->DefaultValue.IsEmpty())
                    {
                        const int32 Index = E->GetIndexByNameString(Pin->DefaultValue);
                        if (Index == INDEX_NONE || Index == E->NumEnums() - 1)
                        {
                            Error = TEXT("Unknown/reserved enum pin default");
                            return false;
                        }
                    }
                }
        }
    if (Uses)
    {
        const FString Source = BP->GetOutermost()->GetMetaData().GetValue(BP, TEXT("DasDSLSource"));
        if ((!Source.IsEmpty() && Source != Script) || !BP->GeneratedClass ||
            (BP->Status != BS_UpToDate && BP->Status != BS_UpToDateWithWarnings))
        {
            Error = FString::Printf(TEXT("Compile dependent Blueprint; independent DSL source requires coordinated type migration: %s status=%d source=%s expected=%s"),
                *BP->GetPathName(), int32(BP->Status), *Source, *Script);
            return false;
        }
    }
    return true;
}
}
#if DAS_WITH_EDITOR_TESTS
void DasFailEnumAppendForTesting(int32 Stage)
{
    EnumAppendFailStage = Stage;
}
#endif
static bool MigrateEnums(const FString& Script, const FString& RegistryPath, FString& Error, bool RepairGraphs,
                         bool RenameOnly, const TMap<FString, TMap<int64, int64>>* ValueRules = nullptr)
{
    Error.Reset();
    auto& R = FModuleManager::LoadModuleChecked<FDasRuntimeModule>(TEXT("DasRuntime"));
    if (!IsInGameThread() ||
        !RegistryPath.StartsWith(TEXT("/Game/")) || !FPackageName::IsValidObjectPath(RegistryPath) ||
                                 Script != FPaths::GetCleanFilename(Script) ||
                                 FPaths::GetExtension(Script) != TEXT("das") || (GEditor && GEditor->PlayWorld) ||
                                                                      !R.CanMigrateAuthoredTypes())
    {
        Error = TEXT("Enum append requires idle editor, stopped PIE and no script contexts");
        return false;
    }
    auto* Registry = LoadObject<UDasTypeRegistry>(nullptr, *RegistryPath, nullptr, LOAD_NoWarn);
    if (!Registry || !Registry->Validate(Error))
    {
        Error = TEXT("Expected valid type registry");
        return false;
    }
    TStrongObjectPtr<UDasTypeRegistry> Keep(Registry);
    FDasReflectedTypes Next, Old;
    if (!R.ReadReflectedTypes(R.ScriptPath(Script), Next, Error))
        return false;
    const FString OldWire = Registry->Bindings[0].Type->GetOutermost()->GetMetaData().GetValue(
        Registry->Bindings[0].Type, TEXT("DasTypesModel"));
    if (!FDasReflectedTypes::FromJson(OldWire, Old, Error))
        return false;
    SortTypes(Old);
    SortTypes(Next);
    if (Old.Id != Next.Id || Old.Enums.Num() != Next.Enums.Num() || Old.Structs.Num() != Next.Structs.Num() ||
        Registry->Bindings.Num() != Old.Enums.Num() + Old.Structs.Num())
    {
        Error = TEXT("Append migration cannot change type identities/count");
        return false;
    }
    TSet<UEnum*> Changed;
    TMap<UEnum*, TMap<FName, FName>> Renames;
    TMap<UEnum*, TSet<int64>> OldValues;
    TArray<FEnumImage> Images;
    FDasReflectedTypes Prefix = Next;
    const bool Remap = ValueRules != nullptr;
    TMap<UEnum*, TMap<int64, int64>> Remaps;
    TMap<UEnum*, TSet<FName>> Removed;
    bool Added = false;
    for (int32 I = 0; I < Old.Enums.Num(); ++I)
    {
        const auto& Before = Old.Enums[I];
        auto& After = Next.Enums[I];
        auto* E = Cast<UDasDefinedEnum>(Registry->Resolve(Before.Id, true, Error));
        if (!E || After.Id != Before.Id || (!Remap && After.Entries.Num() < Before.Entries.Num()) ||
            (RenameOnly && After.Entries.Num() != Before.Entries.Num()) || E->NumEnums() != Before.Entries.Num() + 1)
        {
            Error = TEXT("Enum migration requires unchanged type identities and valid old assets");
            return false;
        }
        After.Name = Before.Name;
        Prefix.Enums[I].Name = Before.Name;
        if (Remap)
        {
            int32 Last = -1;
            for (const auto& Entry : After.Entries)
            {
                const int32 Index = Before.Entries.IndexOfByPredicate([&](const auto& V) { return V.Id == Entry.Id; });
                if (Index <= Last || Index == INDEX_NONE || Before.Entries[Index].Name != Entry.Name)
                {
                    Error = TEXT("Value migration refuses addition/reorder/rename or changed member ID");
                    return false;
                }
                Last = Index;
            }
            const auto* Rules = ValueRules->Find(Before.Id);
            for (int32 J = 0; J < Before.Entries.Num(); ++J)
            {
                const auto& V = Before.Entries[J];
                if (E->GetNameStringByIndex(J) != V.Name || E->GetValueByIndex(J) != V.Value ||
                    E->MemberIds.FindRef(V.Value) != V.Id)
                {
                    Error = TEXT("Old enum asset/member identity drift");
                    return false;
                }
                const auto* Target = After.Entries.FindByPredicate([&](const auto& X) { return X.Id == V.Id; });
                const int64* Rule = Rules ? Rules->Find(V.Value) : nullptr;
                if (!Target || Target->Value != V.Value)
                {
                    if (!Rule || (Target && *Rule != Target->Value) ||
                        !After.Entries.ContainsByPredicate([&](const auto& X) { return X.Value == *Rule; }))
                    {
                        Error = TEXT("Changed/deleted value requires explicit valid replacement rule");
                        return false;
                    }
                    Remaps.FindOrAdd(E).Add(V.Value, *Rule);
                    if (!Target)
                    {
                        Removed.FindOrAdd(E).Add(FName(*V.Name));
                        const auto* Replacement =
                            After.Entries.FindByPredicate([&](const auto& X) { return X.Value == *Rule; });
                        Renames.FindOrAdd(E).Add(FName(*V.Name), FName(*Replacement->Name));
                    }
                }
                else if (Rule)
                {
                    Error = TEXT("Replacement rule supplied for unchanged value");
                    return false;
                }
            }
            if (Rules && Rules->Num() != Remaps.FindRef(E).Num())
            {
                Error = TEXT("Unused/unknown replacement rule");
                return false;
            }
        }
        else
        {
            for (int32 J = 0; J < Before.Entries.Num(); ++J)
                if ((!RenameOnly && After.Entries[J].Name != Before.Entries[J].Name) ||
                    After.Entries[J].Value != Before.Entries[J].Value || After.Entries[J].Id != Before.Entries[J].Id ||
                    (!E->MemberIds.IsEmpty() &&
                     E->MemberIds.FindRef(Before.Entries[J].Value) != Before.Entries[J].Id) ||
                    E->GetNameStringByIndex(J) != Before.Entries[J].Name ||
                    E->GetValueByIndex(J) != Before.Entries[J].Value)
                {
                    Error = TEXT("Enum append refuses removal, rename, reorder, renumber or asset drift");
                    return false;
                }
            if (RenameOnly)
                for (int32 J = 0; J < Before.Entries.Num(); ++J)
                    if (After.Entries[J].Name != Before.Entries[J].Name)
                        Renames.FindOrAdd(E).Add(FName(*Before.Entries[J].Name), FName(*After.Entries[J].Name));
        }
        Prefix.Enums[I].Entries = Before.Entries;
        if (After.Entries.Num() > Before.Entries.Num() || Renames.Contains(E) || Remaps.Contains(E))
        {
            Added = true;
            Changed.Add(E);
            auto& Values = OldValues.Add(E);
            for (const auto& V : Before.Entries)
                Values.Add(V.Value);
            FEnumImage Image;
            Image.Type = E;
            Image.MemberIds = E->MemberIds;
            for (int32 J = 0; J < E->NumEnums(); ++J)
                Image.Entries.Add({E->GetNameByIndex(J), E->GetValueByIndex(J)});
            if (const auto* M = FMetaData::GetMapForObject(E))
                Image.Metadata = *M;
            Images.Add(MoveTemp(Image));
        }
    }
    if (Remap)
        for (const auto& Rule : *ValueRules)
            if (!Old.Enums.ContainsByPredicate([&](const auto& E) { return E.Id == Rule.Key; }))
            {
                Error = TEXT("Unknown enum ID in replacement rules");
                return false;
            }
    for (int32 I = 0; I < Old.Structs.Num(); ++I)
    {
        if (Next.Structs[I].Id != Old.Structs[I].Id || !Registry->Resolve(Old.Structs[I].Id, false, Error))
        {
            Error = TEXT("Enum append cannot change struct identity");
            return false;
        }
        auto* Native = CastChecked<UUserDefinedStruct>(Registry->Resolve(Old.Structs[I].Id, false, Error));
        const auto& Vars = FStructureEditorUtils::GetVarDesc(Native);
        if (Native->Status != UDSS_UpToDate || Vars.Num() != Old.Structs[I].Properties.Num())
        {
            Error = TEXT("Stored struct layout drift blocks enum append");
            return false;
        }
        for (const auto& F : Old.Structs[I].Properties)
        {
            auto* P = FindDasStructField(Native, F.Name);
            const auto* V = Vars.FindByPredicate([&](const auto& X) { return X.FriendlyName == F.Name; });
            const auto* Memory = Native->GetDefaultInstance();
            const bool Same =
                F.Type.Kind ==
                TEXT("int")
                     ? CastField<FIntProperty>(P) &&
                           CastField<FIntProperty>(P)->GetPropertyValue_InContainer(Memory) == int32(F.Default.Number)
                     : F.Type.Kind ==
                           TEXT("float")
                                ? CastField<FFloatProperty>(P) &&
                                      CastField<FFloatProperty>(P)->GetPropertyValue_InContainer(Memory) ==
                                          float(F.Default.Number)
                                : F.Type.Kind ==
                                      TEXT("double")
                                           ? CastField<FDoubleProperty>(P) &&
                                                 CastField<FDoubleProperty>(P)->GetPropertyValue_InContainer(Memory) ==
                                                     F.Default.Number
                                           : F.Type.Kind ==
                                                 TEXT("bool")
                                                      ? CastField<FBoolProperty>(P) &&
                                                            CastField<FBoolProperty>(P)->GetPropertyValue_InContainer(
                                                                Memory) == F.Default.Boolean
                                                      : F.Type.Kind ==
                                                            TEXT("string")
                                                                 ? CastField<FStrProperty>(P) &&
                                                                       CastField<FStrProperty>(P)
                                                                               ->GetPropertyValue_InContainer(Memory) ==
                                                                           F.Default.String
                                                                 : CastField<FNameProperty>(P) &&
                                                                       CastField<FNameProperty>(P)
                                                                               ->GetPropertyValue_InContainer(Memory) ==
                                                                           FName(*F.Default.String);
            if (!Same || !V || V->MetaData.FindRef(TEXT("DasMemberId")) != F.Id)
            {
                Error = TEXT("Stored struct field/default/ID drift blocks enum append");
                return false;
            }
        }
        Next.Structs[I].Name = Old.Structs[I].Name;
        Prefix.Structs[I].Name = Old.Structs[I].Name;
    }
    FString A, B, NewWire;
    if (!Old.ToJson(A, Error) || !Prefix.ToJson(B, Error) || A != B || !Next.ToJson(NewWire, Error))
    {
        Error = TEXT("Enum append cannot change existing metadata or struct definitions");
        return false;
    }
    for (const auto& Binding : Registry->Bindings)
        if (Binding.Type->GetOutermost()->GetMetaData().GetValue(Binding.Type, TEXT("DasTypesModel")) != OldWire)
        {
            Error = TEXT("Catalog contains inconsistent stored type definitions");
            return false;
        }
    if (!Added)
        return true;
    TArray<TStrongObjectPtr<UObject>> Scope;
    if (!GatherEnumScope(Changed, Scope, Error))
        return false;
    TArray<UK2Node*> EnumNodes;
    TSet<UBlueprint*> RepairBPs;
    for (const auto& Asset : Scope)
        if (auto* BP = Cast<UBlueprint>(Asset.Get()))
            if (!CheckBlueprint(BP, Changed, Script, Error, RepairGraphs, EnumNodes, RepairBPs))
                return false;
    if (Remap)
        for (auto* Node : EnumNodes)
        {
            auto* D = CastChecked<INodeDependingOnEnumInterface>(Node);
            if (const auto* Names = Removed.Find(D->GetEnum()))
                for (FName Name : *Names)
                    if (auto* Pin = Node->FindPin(Name))
                    {
                        if ((Node->IsA<UK2Node_SwitchEnum>() && !Pin->LinkedTo.IsEmpty()) ||
                            (Node->IsA<UK2Node_Select>() &&
                             (!Pin->LinkedTo.IsEmpty() || Pin->DefaultValue != Pin->AutogeneratedDefaultValue)))
                        {
                            Error = TEXT("Removed branch/option has user logic or default override; resolve it before migration");
                            return false;
                        }
                    }
        }
    for (TObjectIterator<UObject> It(RF_NoFlags); It; ++It)
    {
        auto* O = *It;
        if (!IsValid(O) || O->GetClass()->HasAnyClassFlags(CLASS_NewerVersionExists) ||
            O->GetClass()->GetName().StartsWith(TEXT("SKEL_")) || O->GetClass()->GetName().StartsWith(TEXT("REINST_")))
            continue;
        int32 Budget = 65536;
        for (TFieldIterator<FProperty> P(O->GetClass()); P; ++P)
            if (PropertyUsesEnum(*P, Changed))
            {
                if (auto* W = O->GetTypedOuter<UWorld>())
                    if (W->IsPartitionedWorld() || !W->GetStreamingLevels().IsEmpty())
                    {
                        Error = TEXT("Loaded partitioned/streaming enum values require separate scope");
                        return false;
                    }
                for (int32 Index = 0; Index < P->ArrayDim; ++Index)
                    if (!CheckEnumData(*P, P->ContainerPtrToValuePtr<void>(O, Index), OldValues, Budget))
                    {
                        Error = TEXT("Existing enum value is unknown/reserved or exceeds traversal bounds: ")+
                                     O->GetPathName();
                        return false;
                    }
            }
    }


    auto RenameText = [&](UEnum* E, const FString& Value, bool Reverse = false)
    {
        const auto* Names = Renames.Find(E);
        if (!Names)
            return Value;
        for (const auto& Pair : *Names)
        {
            const FString From = (Reverse ? Pair.Value : Pair.Key).ToString(),
                          To = (Reverse ? Pair.Key : Pair.Value).ToString();
            if (Value == From)
                return To;
            if (Value == E->GetName() + TEXT("::")+ From)
                return E->GetName() + TEXT("::")+ To;
        }
        return Value;
    };
    auto RenameCase = [&](UEdGraphNode* Node, FName Name, bool Reverse = false)
    {
        if (!Node->IsA<UK2Node_SwitchEnum>() && !Node->IsA<UK2Node_Select>())
            return Name;
        auto* D = CastChecked<INodeDependingOnEnumInterface>(Node);
        return FName(*RenameText(D->GetEnum(), Name.ToString(), Reverse));
    };
    if (RenameOnly || Remap)
    {
        for (const auto& Asset : Scope)
            if (auto* BP = Cast<UBlueprint>(Asset.Get()))
                if (BP->GetOutermost()->GetMetaData().GetValue(BP, TEXT("DasDSLSource")) == Script)
                    RepairBPs.Add(BP);
    }
    TUniquePtr<FDasBlueprintRollback> GraphRollback;
    UBlueprint* GraphRoot = nullptr;
    TArray<UBlueprint*> GraphBPs;
    TArray<TStrongObjectPtr<UObject>> GraphAssets;
    struct FPinKey
    {
        UEdGraphNode* Node;
        FName Name;
        EEdGraphPinDirection Direction;
        FGuid Guid;
        UEdGraphPin* Find() const
        {
            for (auto* P : Node->Pins)
                if (P && ((Guid.IsValid() && P->PersistentGuid == Guid) ||
                          (P->PinName == Name && P->Direction == Direction)))
                    return P;
            return nullptr;
        }
    };
    TArray<TPair<FPinKey, FPinKey>> Links;
    struct FDefault
    {
        FPinKey Pin;
        FString Value;
        FText Text;
        UObject* Object;
    };
    TArray<FDefault> Defaults;
    for (auto* Node : EnumNodes)
        for (auto* Pin : Node->Pins)
            if (Pin)
                Defaults.Add({{Node, Pin->PinName, Pin->Direction, Pin->PersistentGuid},
                              Pin->DefaultValue,
                              Pin->DefaultTextValue,
                              Pin->DefaultObject});
    for (auto* Node : EnumNodes)
        for (auto* Pin : Node->Pins)
            if (Pin)
                for (auto* Other : Pin->LinkedTo)
                    Links.Add(TPair<FPinKey, FPinKey>(
                        FPinKey{Node, Pin->PinName, Pin->Direction, Pin->PersistentGuid},
                        FPinKey{Other->GetOwningNode(), Other->PinName, Other->Direction, Other->PersistentGuid}));
    if (!EnumNodes.IsEmpty() || RenameOnly || Remap)
    {
        if (RepairBPs.Num() != 1)
        {
            Error = TEXT("Enum node repair currently requires one DSL Actor root");
            return false;
        }
        GraphRoot = *RepairBPs.CreateConstIterator();
        if (!GraphRoot->GeneratedClass->IsChildOf(ADasActor::StaticClass()) ||
            GraphRoot->GetOutermost()->GetMetaData().GetValue(GraphRoot, TEXT("DasDSLSource")) != Script)
        {
            Error = TEXT("Enum node repair requires the imported DSL Actor root");
            return false;
        }
        FDasAuthoringModule Model;
        FDasClassSchema Schema;
        if (!FDasAuthoringModule::FromJson(
                GraphRoot->GetOutermost()->GetMetaData().GetValue(GraphRoot, TEXT("DasDSLModel")), Model, Error) ||
                !Model.ToActorSchema(GraphRoot->GetOutermost()->GetMetaData().GetValue(
                                         GraphRoot, TEXT("DasDSLClassId")), Schema, Error) ||
                                     !FDasBlueprintRollback::GatherScope(GraphRoot, GraphBPs, GraphAssets, Error))
            return false;
        GraphRollback = MakeUnique<FDasBlueprintRollback>();
        if (!GraphRollback->Prepare(GraphRoot, Schema, GraphBPs, GraphAssets, Error) ||
            !R.BeginPreserveContextsForReinstance(GraphRoot->GeneratedClass))
            return false;
    }

    if ((RenameOnly || Remap) && GraphRollback)
    {
        for (const auto& Asset : Scope)
            if (auto* BP = Cast<UBlueprint>(Asset.Get()))
            {
                bool Uses = false;
                for (const auto& V : BP->NewVariables)
                    Uses |= Changed.Contains(Cast<UEnum>(V.VarType.PinSubCategoryObject.Get()));
                TArray<UEdGraph*> Graphs;
                BP->GetAllGraphs(Graphs);
                for (auto* G : Graphs)
                    for (UEdGraphNode* Node : G->Nodes)
                        for (auto* Pin : Node->Pins)
                            if (Pin)
                                Uses |= Changed.Contains(Cast<UEnum>(Pin->PinType.PinSubCategoryObject.Get()));
                if (Uses && !GraphBPs.Contains(BP))
                {
                    R.EndPreserveContextsForReinstance();
                    Error = TEXT("Enum rename dependency is outside the Actor root rollback scope");
                    return false;
                }
            }
    }
    struct FSymbolImage
    {
        UEdGraphNode* Node;
        FName Name;
        EEdGraphPinDirection Direction;
        FString Default, Auto;
    };
    TArray<FSymbolImage> Symbols;
    if (Remap)
        for (auto* BP : GraphBPs)
        {
            TArray<UEdGraph*> Gs;
            BP->GetAllGraphs(Gs);
            for (auto* G : Gs)
                for (UEdGraphNode* N : G->Nodes)
                    for (auto* P : N->Pins)
                        if (P)
                            Symbols.Add({N, P->PinName, P->Direction, P->DefaultValue, P->AutogeneratedDefaultValue});
        }
    auto RewritePins = [&](bool Reverse)
    {
        for (auto* BP : GraphBPs)
        {
            for (auto& V : BP->NewVariables)
                if (auto* E = Cast<UEnum>(V.VarType.PinSubCategoryObject.Get()))
                    V.DefaultValue = RenameText(E, V.DefaultValue, Reverse);
            TArray<UEdGraph*> Graphs;
            BP->GetAllGraphs(Graphs);
            for (auto* G : Graphs)
                for (UEdGraphNode* Node : G->Nodes)
                    for (auto* Pin : Node->Pins)
                        if (Pin)
                        {
                            if (!Remap)
                                Pin->PinName = RenameCase(Node, Pin->PinName, Reverse);
                            if (auto* E = Cast<UEnum>(Pin->PinType.PinSubCategoryObject.Get()))
                            {
                                Pin->DefaultValue = RenameText(E, Pin->DefaultValue, Reverse);
                                Pin->AutogeneratedDefaultValue = RenameText(E, Pin->AutogeneratedDefaultValue, Reverse);
                            }
                        }
        }
    };

    struct FValueImage
    {
        TStrongObjectPtr<UObject> Object;
        FName Field;
        int64 Value;
        UBlueprint* CDOOwner = nullptr;
    };
    TArray<FValueImage> Data;
    FDelegateHandle DataHandle;
    if (Remap)
    {
        FDasAuthoringModule Model;
        FDasClassSchema Schema;
        if (!FDasAuthoringModule::FromJson(
                GraphRoot->GetOutermost()->GetMetaData().GetValue(GraphRoot, TEXT("DasDSLModel")), Model, Error) ||
                !Model.ToActorSchema(
                    GraphRoot->GetOutermost()->GetMetaData().GetValue(GraphRoot, TEXT("DasDSLClassId")), Schema, Error))
        {
            R.EndPreserveContextsForReinstance();
            return false;
        }
        for (TObjectIterator<UObject> It(RF_NoFlags); It; ++It)
        {
            auto* O = *It;
            if (!IsValid(O) || O->GetClass()->HasAnyClassFlags(CLASS_NewerVersionExists) ||
                O->GetClass()->GetName().StartsWith(TEXT("SKEL_")) ||
                                                    O->GetClass()->GetName().StartsWith(TEXT("REINST_")))
                continue;
            for (TFieldIterator<FProperty> P(O->GetClass()); P; ++P)
                if (StaticEnumStorage(*P, Changed))
                {
                    if ((*P)->GetClass() == FStructProperty::StaticClass() &&
                        (CastFieldChecked<FStructProperty>(*P)->Struct == FInstancedStruct::StaticStruct() ||
                         CastFieldChecked<FStructProperty>(*P)->Struct == FDasScalarValue::StaticStruct()))
                        continue;
                    auto* Byte = CastField<FByteProperty>(*P);
                    if (!Byte || !Changed.Contains(Byte->Enum) || P->ArrayDim != 1 ||
                        !O->IsA(GraphRoot->GeneratedClass) ||
                        !Schema.Fields.ContainsByPredicate(
                            [&](const auto& F) { return F.Name == P->GetFName() && F.Kind == EDasScalarKind::Enum; }))
                    {
                        R.EndPreserveContextsForReinstance();
                        Error = TEXT("Value remap currently supports declared scalar enum Actor fields only; container/nested/unrelated state requires separate policy");
                        return false;
                    }
                    Data.Add({TStrongObjectPtr<UObject>(O), P->GetFName(), Byte->GetPropertyValue_InContainer(O),
                              O->HasAnyFlags(RF_ClassDefaultObject) ? Cast<UBlueprint>(O->GetClass()->ClassGeneratedBy)
                                                                    : nullptr});
                }
        }
        for (TObjectIterator<ADasActor> It; It; ++It)
            for (const auto& Pair : It->RetiredDasValues)
            {
                const auto& V = Pair.Value;
                if ((V.Kind == 13 && Changed.Contains(FindObject<UEnum>(nullptr, *V.ObjectClass))) ||
                    (V.Kind == 12 && V.StructValue.IsValid() &&
                     [&]()
                     {
                         for (TFieldIterator<FProperty> P(V.StructValue.GetScriptStruct()); P; ++P)
                             if (StaticEnumStorage(*P, Changed))
                                 return true;
                         return false;
                     }()))
                {
                    R.EndPreserveContextsForReinstance();
                    Error = TEXT("Archived enum state requires a separate remap policy");
                    return false;
                }
            }
        DataHandle = FCoreUObjectDelegates::OnObjectsReplaced.AddLambda(
            [&](const FCoreUObjectDelegates::FReplacementObjectMap& Map)
            {
                for (auto& V : Data)
                    if (auto* New = Map.Find(V.Object.Get()))
                        V.Object.Reset(*New);
            });
    }
    auto WriteData = [&](bool Restore)
    {
        for (auto& V : Data)
        {
            if (V.CDOOwner && V.CDOOwner->GeneratedClass)
                V.Object.Reset(V.CDOOwner->GeneratedClass->GetDefaultObject());
            auto* P = FindFProperty<FByteProperty>(V.Object->GetClass(), V.Field);
            if (!P)
                return false;
            int64 Value = V.Value;
            if (!Restore)
                if (const auto* Map = Remaps.Find(P->Enum))
                    if (const auto* To = Map->Find(Value))
                        Value = *To;
            P->SetPropertyValue_InContainer(V.Object.Get(), uint8(Value));
        }
        return true;
    };
    ON_SCOPE_EXIT
    {
        if (DataHandle.IsValid())
            FCoreUObjectDelegates::OnObjectsReplaced.Remove(DataHandle);
        if (GraphRollback)
            R.EndPreserveContextsForReinstance();
    };
    TMap<UPackage*, bool> Dirty;
    TMap<UObject*, FString> Stored;
    for (const auto& Binding : Registry->Bindings)
    {
        Stored.Add(Binding.Type.Get(), OldWire);
        Dirty.Add(Binding.Type->GetOutermost(), Binding.Type->GetOutermost()->IsDirty());
    }
    auto Rollback = [&](const TCHAR* Issue)
    {
        bool Restored = true;
        for (auto& Image : Images)
        {
            Restored &= Image.Type->SetEnums(Image.Entries, UEnum::ECppForm::Namespaced, UEnum::EUnderlyingType::uint8,
                                             EEnumFlags::None, UEnum::EAddMaxKeyIfMissing::No);
            Image.Type->MemberIds = Image.MemberIds;
            Image.Type->GetOutermost()->GetMetaData().SetObjectValues(Image.Type, Image.Metadata);
        }
        for (const auto& Pair : Stored)
            Pair.Key->GetOutermost()->GetMetaData().SetValue(Pair.Key, TEXT("DasTypesModel"), *Pair.Value);
        if (GraphRollback)
        {
            if (RenameOnly)
                RewritePins(true);
            if (Remap)
                for (const auto& S : Symbols)
                    for (auto* P : S.Node->Pins)
                        if (P && P->PinName == S.Name && P->Direction == S.Direction)
                        {
                            P->DefaultValue = S.Default;
                            P->AutogeneratedDefaultValue = S.Auto;
                        }
            for (auto* Node : EnumNodes)
            {
                RebuildEnumNode(Node);
            }
            FString GraphError;
            Restored &= GraphRollback->Restore(GraphError);
            if (Remap)
                Restored &= WriteData(true);
        }
        for (const auto& Pair : Dirty)
            Pair.Key->SetDirtyFlag(Pair.Value);
        Error = Restored ? FString(TEXT("Enum append rolled back: ")) + Issue : TEXT("Enum append rollback failed");
        return false;
    };
    if (RenameOnly)
    {
        RewritePins(false);
        for (auto& Link : Links)
        {
            Link.Key.Name = RenameCase(Link.Key.Node, Link.Key.Name);
            Link.Value.Name = RenameCase(Link.Value.Node, Link.Value.Name);
        }
        for (auto& Default : Defaults)
        {
            Default.Pin.Name = RenameCase(Default.Pin.Node, Default.Pin.Name);
            auto* Pin = Default.Pin.Find();
            if (Pin)
                if (auto* E = Cast<UEnum>(Pin->PinType.PinSubCategoryObject.Get()))
                    Default.Value = RenameText(E, Default.Value);
        }
    }
    if (Remap)
    {
        RewritePins(false);
        Defaults.RemoveAll(
            [&](const auto& D)
            {
                auto* Dep = Cast<INodeDependingOnEnumInterface>(D.Pin.Node);
                const auto* Gone = Dep ? Removed.Find(Dep->GetEnum()) : nullptr;
                return Gone && Gone->Contains(D.Pin.Name) &&
                       (D.Pin.Node->IsA<UK2Node_SwitchEnum>() || D.Pin.Node->IsA<UK2Node_Select>());
            });
        for (auto& D : Defaults)
            if (auto* Pin = D.Pin.Find())
                if (auto* E = Cast<UEnum>(Pin->PinType.PinSubCategoryObject.Get()))
                    D.Value = RenameText(E, D.Value);
    }
    for (const auto& D : Next.Enums)
    {
        auto* E = CastChecked<UDasDefinedEnum>(Registry->Resolve(D.Id, true, Error));
        if (!Changed.Contains(E))
            continue;
        TArray<TPair<FName, int64>> Entries;
        for (const auto& V : D.Entries)
            Entries.Add({FName(*(E->GetName() + TEXT("::")+ V.Name)), V.Value});
        if (!E->SetEnums(Entries, UEnum::ECppForm::Namespaced, UEnum::EUnderlyingType::uint8, EEnumFlags::None,
                         UEnum::EAddMaxKeyIfMissing::Yes))
            return Rollback(TEXT("Could not append enum members"));
        E->MemberIds.Empty();
        for (const auto& V : D.Entries)
            E->MemberIds.Add(V.Value, V.Id);
        for (int32 I = 0; I < D.Entries.Num(); ++I)
            E->SetMetaData(TEXT("DisplayName"), *D.Entries[I].Name, I);
        E->SetMetaData(TEXT("Hidden"), TEXT("true"), E->NumEnums() - 1);
        if (Remap && !WriteData(false))
            return Rollback(TEXT("Could not remap scalar enum data"));
        if (EnumAppendFailStage == 1)
            return Rollback(TEXT("Injected after enum mutation"));
    }
    if (GraphRollback)
    {
        for (auto* Node : EnumNodes)
        {
            RebuildEnumNode(Node);
        }
        if (EnumAppendFailStage == 3)
            return Rollback(TEXT("Injected after enum node reconstruction"));
        FBlueprintEditorUtils::MarkBlueprintAsStructurallyModified(GraphRoot);
        FKismetEditorUtilities::CompileBlueprint(GraphRoot);
        FBlueprintCompilationManager::FlushCompilationQueueAndReinstance();
        if (!GraphRoot->GeneratedClass || GraphRoot->Status == BS_Error)
            return Rollback(TEXT("Enum node Blueprint compilation failed"));
        if (Remap && !WriteData(false))
            return Rollback(TEXT("Could not restore mapped values after compilation"));
        if (!GraphRollback->Finish(Error))
            return Rollback(TEXT("Dependent enum node Blueprint compilation failed"));
        for (const auto& Link : Links)
        {
            auto* FromPin = Link.Key.Find();
            auto* ToPin = Link.Value.Find();
            if (!FromPin || !ToPin || !FromPin->LinkedTo.Contains(ToPin))
                return Rollback(TEXT("Existing enum node link was not preserved"));
        }
        for (const auto& Default : Defaults)
        {
            auto* Pin = Default.Pin.Find();
            if (!Pin || Pin->DefaultValue != Default.Value || !Pin->DefaultTextValue.EqualTo(Default.Text) ||
                Pin->DefaultObject != Default.Object)
                return Rollback(TEXT("Existing enum node default was not preserved"));
        }
        if (EnumAppendFailStage == 4)
            return Rollback(TEXT("Injected after enum node compilation"));
    }
    bool NeedsAuthoring = false;
    for (const auto& Asset : Scope)
        if (auto* BP = Cast<UBlueprint>(Asset.Get()))
            NeedsAuthoring |= BP->GetOutermost()->GetMetaData().GetValue(BP, TEXT("DasDSLSource")) == Script;
    if (NeedsAuthoring)
    {
        FDasAuthoringModule Authoring;
        FString ClassWire;
        if (!R.ReadAuthoringScript(R.ScriptPath(Script), Authoring, Error) || !Authoring.ToJson(ClassWire, Error))
            return Rollback(TEXT("Updated DSL source does not compile"));
        for (const auto& Asset : Scope)
            if (auto* BP = Cast<UBlueprint>(Asset.Get()))
                if (BP->GetOutermost()->GetMetaData().GetValue(BP, TEXT("DasDSLSource")) == Script)
                {
                    FString Expected = BP->GetOutermost()->GetMetaData().GetValue(BP, TEXT("DasDSLModel"));
                    if (RenameOnly || Remap)
                    {
                        FDasAuthoringModule Previous;
                        if (!FDasAuthoringModule::FromJson(Expected, Previous, Error))
                            return Rollback(TEXT("Missing old class model"));
                        for (auto& C : Previous.Classes)
                            for (auto& P : C.Properties)
                                if (P.Type.Kind == TEXT("enum"))
                                    P.Default.String =
                                        RenameText(FindObject<UEnum>(nullptr, *P.Type.TypePath), P.Default.String);
                        if (!Previous.ToJson(Expected, Error))
                            return Rollback(TEXT("Could not normalize renamed class default"));
                    }
                    if (Expected != ClassWire)
                        return Rollback(TEXT("Class declarations must remain unchanged during enum append"));
                    if (RenameOnly || Remap)
                        BP->GetOutermost()->GetMetaData().SetValue(BP, TEXT("DasDSLModel"), *ClassWire);
                }
    }
    for (const auto& Binding : Registry->Bindings)
    {
        Binding.Type->GetOutermost()->GetMetaData().SetValue(Binding.Type, TEXT("DasTypesModel"), *NewWire);
        Binding.Type->MarkPackageDirty();
    }
    if (EnumAppendFailStage == 2)
        return Rollback(TEXT("Injected after model update"));
    UE_LOG(LogTemp, Display,
           TEXT("DAS_ENUM_APPEND PASS %d enums; existing graph/value storage unchanged"), Changed.Num());
    return true;
}

bool FDasTypeService::ExtendEnums(const FString& Script, const FString& RegistryPath, FString& Error, bool RepairGraphs)
{
    return MigrateEnums(Script, RegistryPath, Error, RepairGraphs, false);
}
bool FDasTypeService::RenameEnumMembers(const FString& Script, const FString& RegistryPath, FString& Error)
{
    return MigrateEnums(Script, RegistryPath, Error, true, true);
}

bool FDasTypeService::MigrateEnumValues(const FString& Script, const FString& RegistryPath, const FString& RulesFile,
                                        FString& Error)
{
    if (RulesFile != FPaths::GetCleanFilename(RulesFile) || FPaths::GetExtension(RulesFile) != TEXT("json"))
    {
        Error = TEXT("Expected local rules.json in plugin Scripts");
        return false;
    }
    FString Text;
    auto& R = FModuleManager::LoadModuleChecked<FDasRuntimeModule>(TEXT("DasRuntime"));
    TSharedPtr<FJsonObject> J;
    if (!FFileHelper::LoadFileToString(Text, *R.ScriptPath(RulesFile)) || Text.Len() > 65536 ||
        !FJsonSerializer::Deserialize(TJsonReaderFactory<>::Create(Text), J) || !J || J->Values.Num() != 2 ||
        !J->HasTypedField<EJson::Number>(TEXT("schema_version")) || J->GetNumberField(TEXT("schema_version")) != 1)
    {
        Error = TEXT("Invalid enum replacement rules/version");
        return false;
    }
    const TArray<TSharedPtr<FJsonValue>>* Rows = nullptr;
    if (!J->TryGetArrayField(TEXT("rules"), Rows) || Rows->IsEmpty() || Rows->Num() > 255)
    {
        Error = TEXT("Expected 1..255 replacement rules");
        return false;
    }
    TMap<FString, TMap<int64, int64>> Rules;
    for (const auto& Row : *Rows)
    {
        const TSharedPtr<FJsonObject>* Parsed = nullptr;
        if (!Row->TryGetObject(Parsed))
        {
            Error = TEXT("Expected rule object");
            return false;
        }
        const auto Obj = *Parsed;
        FString Id;
        double From, To;
        if (!Obj || Obj->Values.Num() != 3 ||
            !Obj->TryGetStringField(
                TEXT("enum_id"), Id) ||
                !Obj->TryGetNumberField(
                    TEXT("from"), From) ||
                    !Obj->TryGetNumberField(TEXT("to"), To) || !FMath::IsFinite(From) || !FMath::IsFinite(To) ||
                                            From < 0 || From > 254 || To < 0 || To > 254 ||
                                            FMath::FloorToDouble(From) != From || FMath::FloorToDouble(To) != To ||
                                            Rules.FindOrAdd(Id).Contains(int64(From)))
        {
            Error = TEXT("Invalid or duplicate replacement rule");
            return false;
        }
        Rules.FindOrAdd(Id).Add(int64(From), int64(To));
    }
    return MigrateEnums(Script, RegistryPath, Error, true, false, &Rules);
}
