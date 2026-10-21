#include "DasBlueprintRollback.h"
#include "DasContainerTypes.h"
#include "DasNumericGraphMigration.h"
#include "DasProbeActor.h"
#include "DasPropertyValues.h"
#include "DasSoftReferences.h"
#include "Engine/Blueprint.h"
#include "Engine/BlueprintGeneratedClass.h"
#include "Engine/World.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "Kismet2/KismetEditorUtilities.h"
#include "BlueprintCompilationManager.h"
#include "K2Node_Variable.h"
#include "K2Node_CallFunction.h"
#include "K2Node_FunctionEntry.h"
#include "UObject/UObjectHash.h"
#include "UObject/MetaData.h"
#include "UObject/StrProperty.h"
#include "Misc/PackageName.h"
#include "Modules/ModuleManager.h"
namespace
{
TMap<FName, FString> RollbackMetadata(UObject* O)
{
    const auto* M = FMetaData::GetMapForObject(O);
    return M ? *M : TMap<FName, FString>();
}
bool RollbackCapture(UObject* Object, const FDasFieldSchema& Field, FDasScalarValue& Out, FString& Error)
{
    auto* P = IsValid(Object) ? Object->GetClass()->FindPropertyByName(Field.Name) : nullptr;
    if (!P)
    {
        Error = TEXT("Missing rollback field: ")+ Field.Name.ToString();
        return false;
    }
    FDasScalarValue Value;
    if (!ReadDasPropertyValue(P, P->ContainerPtrToValuePtr<void>(Object), Value, Error))
        return false;
    if (Value.Kind != int32(Field.Kind))
    {
        Error = TEXT("Rollback field kind differs from declaration");
        return false;
    }
    Out = MoveTemp(Value);
    return true;
}
bool RollbackValue(UObject* Object, FName Name, const FDasScalarValue& Value)
{
    auto* P = IsValid(Object) ? Object->GetClass()->FindPropertyByName(Name) : nullptr;
    FString Error;
    return P && WriteDasPropertyValue(P, P->ContainerPtrToValuePtr<void>(Object), Value, Error);
}
}
struct FDasBlueprintRollback::FImpl
{
    struct FNode
    {
        TStrongObjectPtr<UK2Node> Node;
        FMemberReference Reference;
        decltype(UK2Node_FunctionEntry::MetaData) EntryMetadata;
    };
    struct FGraph
    {
        TStrongObjectPtr<UEdGraph> Graph;
        TMap<FName, FString> Metadata;
    };
    struct FBlueprint
    {
        TStrongObjectPtr<UBlueprint> BP;
        TStrongObjectPtr<UClass> OriginalClass;
        TArray<FBPVariableDescription> Variables;
        TArray<TObjectPtr<UEdGraph>> Functions, Uber, Delegates, Macros;
        FString Display, Description, Category;
        TMap<FName, FString> Metadata, ClassMetadata;
        TMap<FName, TMap<FName, FString>> FunctionMetadata;
    };
    struct FObject
    {
        TStrongObjectPtr<ADasProbeActor> Object;
        TArray<FDasScalarValue> Values;
        TMap<FString, FDasScalarValue> Archive;
    };
    TArray<FBlueprint> Blueprints;
    TArray<FGraph> Graphs;
    TArray<FNode> Nodes;
    TArray<FObject> Objects;
    TArray<FDasFieldSchema> Fields;
    TMap<UPackage*, bool> Dirty;
    TArray<TStrongObjectPtr<UObject>> Assets;
    FDasNumericGraphMigration Pins;
    FDasPropertyValueRoots ObjectPins;
    FDelegateHandle Handle;
    ~FImpl()
    {
        if (Handle.IsValid())
            FCoreUObjectDelegates::OnObjectsReplaced.Remove(Handle);
    }
};
FDasBlueprintRollback::FDasBlueprintRollback() : Impl(MakeUnique<FImpl>()) {}
FDasBlueprintRollback::~FDasBlueprintRollback() = default;
bool FDasBlueprintRollback::GatherScope(UBlueprint* Root, TArray<UBlueprint*>& BPs,
                                        TArray<TStrongObjectPtr<UObject>>& Assets, FString& Error, bool DefaultsOnly)
{
    BPs.Add(Root);
    auto& Registry = FModuleManager::LoadModuleChecked<FAssetRegistryModule>(TEXT("AssetRegistry")).Get();
    if (!Registry.IsSearchAllAssets())
        Registry.SearchAllAssets(true);
    else
        Registry.WaitForCompletion();
    if (Registry.IsLoadingAssets())
    {
        Error = TEXT("Asset scan must finish before structural migration");
        return false;
    }
    TArray<UBlueprint*> Dependents;
    FBlueprintEditorUtils::FindDependentBlueprints(Root, Dependents);
    TArray<UClass*> Children;
    GetDerivedClasses(Root->GeneratedClass, Children, true);
    for (auto* C : Children)
    {
        auto* B = Cast<UBlueprint>(C->ClassGeneratedBy);
        if (!B)
        {
            Error = TEXT("Unmanaged derived class in structural migration");
            return false;
        }
        Dependents.AddUnique(B);
    }
    TSet<FName> Packages;
    TArray<FName> Queue;
    auto Enqueue = [&](UObject* O)
    {
        if (!Packages.Contains(O->GetOutermost()->GetFName()))
        {
            Packages.Add(O->GetOutermost()->GetFName());
            Queue.Add(O->GetOutermost()->GetFName());
        }
    };
    Enqueue(Root);
    for (auto* B : Dependents)
    {
        if (!B->GetOutermost()->GetName().StartsWith(TEXT("/Game/")) &&
                                                     FPackageName::DoesPackageExist(B->GetOutermost()->GetName()))
        {
            Error = TEXT("Loaded dependent Blueprint is outside /Game scope");
            return false;
        }
        BPs.AddUnique(B);
        Assets.Emplace(B);
        Enqueue(B);
    }
    for (int32 I = 0; I < Queue.Num(); ++I)
    {
        if (Queue.Num() > 256)
        {
            Error = TEXT("Structural migration exceeds 256 packages");
            return false;
        }
        TArray<FName> References;
        Registry.GetReferencers(Queue[I], References);
        for (FName P : References)
            if (!Packages.Contains(P))
            {
                if (!P.ToString().StartsWith(TEXT("/Game/")) || Queue.Num() >= 256)
                {
                    Error = TEXT("Unsupported structural package scope: ")+ P.ToString();
                    return false;
                }
                TArray<FAssetData> Data;
                Registry.GetAssetsByPackageName(P, Data);
                if (Data.Num() != 1 || (Data[0].AssetClassPath != UBlueprint::StaticClass()->GetClassPathName() &&
                                        Data[0].AssetClassPath != UWorld::StaticClass()->GetClassPathName()))
                {
                    Error = TEXT("Structural migration supports ordinary Blueprint/map packages only");
                    return false;
                }
                auto* A = Data[0].GetAsset();
                if (!A)
                {
                    Error = TEXT("Failed to load structural dependency");
                    return false;
                }
                Assets.Emplace(A);
                Enqueue(A);
                if (auto* B = Cast<UBlueprint>(A))
                    BPs.AddUnique(B);
            }
    }
    for (auto* B : BPs)
    {
        if (!B->GeneratedClass || (B->Status != BS_UpToDate && B->Status != BS_UpToDateWithWarnings) ||
            (!DefaultsOnly && B != Root &&
             !B->GetOutermost()->GetMetaData().GetValue(B, TEXT("DasDSLSource")).IsEmpty()))
        {
            Error = TEXT("Compile dependent Blueprints; independent DSL sources need their own migration");
            return false;
        }
    }
    for (auto& A : Assets)
        if (auto* W = Cast<UWorld>(A.Get()))
            if (W->IsPartitionedWorld() || !W->GetStreamingLevels().IsEmpty())
            {
                Error = TEXT("Partitioned/streaming map migration needs a separate scope");
                return false;
            }
    BPs.Sort(
        [&](const UBlueprint& A, const UBlueprint& B)
        {
            if (&A == Root)
                return &B != Root;
            if (&B == Root)
                return false;
            auto Depth = [](const UClass* C)
            {
                int32 N = 0;
                while (C)
                {
                    ++N;
                    C = C->GetSuperClass();
                }
                return N;
            };
            return Depth(A.GeneratedClass) < Depth(B.GeneratedClass);
        });
    return true;
}
bool FDasBlueprintRollback::Prepare(UBlueprint* Root, const FDasClassSchema& Schema, const TArray<UBlueprint*>& BPs,
                                    const TArray<TStrongObjectPtr<UObject>>& Assets, FString& Error)
{
    // Soft references are captured like other owned values; only real containers are deferred.
    for (TFieldIterator<FProperty> It(Root->GeneratedClass, EFieldIteratorFlags::ExcludeSuper); It; ++It)
        if (DasContainerCarrierForProperty(*It) && !DasSoftCarrierForProperty(*It))
        {
            Error = TEXT("Direct container class rollback/migration is deferred");
            return false;
        }
    auto& S = *Impl;
    auto Remember = [&](UObject* O)
    {
        if (!S.Dirty.Contains(O->GetOutermost()))
            S.Dirty.Add(O->GetOutermost(), O->GetOutermost()->IsDirty());
    };
    for (auto* B : BPs)
    {
        if (!B || !B->GeneratedClass || (B->Status != BS_UpToDate && B->Status != BS_UpToDateWithWarnings))
        {
            Error = TEXT("Compile scoped Blueprints before rollback snapshot");
            return false;
        }
        FImpl::FBlueprint Image;
        Image.BP.Reset(B);
        Image.OriginalClass.Reset(B->GeneratedClass);
        Image.Variables = B->NewVariables;
        Image.Functions = B->FunctionGraphs;
        Image.Uber = B->UbergraphPages;
        Image.Delegates = B->DelegateSignatureGraphs;
        Image.Macros = B->MacroGraphs;
        Image.Display = B->BlueprintDisplayName;
        Image.Description = B->BlueprintDescription;
        Image.Category = B->BlueprintCategory;
        Image.Metadata = RollbackMetadata(B);
        Image.ClassMetadata = RollbackMetadata(B->GeneratedClass);
        for (TFieldIterator<UFunction> It(B->GeneratedClass, EFieldIteratorFlags::ExcludeSuper); It; ++It)
            Image.FunctionMetadata.Add(It->GetFName(), RollbackMetadata(*It));
        S.Blueprints.Add(MoveTemp(Image));
        Remember(B);
        TArray<UEdGraph*> Graphs;
        B->GetAllGraphs(Graphs);
        for (auto* G : Graphs)
        {
            FImpl::FGraph Graph;
            Graph.Graph.Reset(G);
            Graph.Metadata = RollbackMetadata(G);
            S.Graphs.Add(MoveTemp(Graph));
            for (auto& E : G->Nodes)
                if (auto* Node = Cast<UK2Node>(E.Get()))
                {
                    FImpl::FNode N;
                    N.Node.Reset(Node);
                    if (auto* V = Cast<UK2Node_Variable>(Node))
                        N.Reference = V->VariableReference;
                    else if (auto* C = Cast<UK2Node_CallFunction>(Node))
                        N.Reference = C->FunctionReference;
                    else if (auto* Entry = Cast<UK2Node_FunctionEntry>(Node))
                        N.EntryMetadata = Entry->MetaData;
                    else
                        continue;
                    S.Nodes.Add(MoveTemp(N));
                }
        }
    }
    for (auto& A : Assets)
    {
        S.Assets.Emplace(A.Get());
        Remember(A.Get());
    }
    for (const auto& F : Schema.Fields)
        if (Root->GeneratedClass->FindPropertyByName(F.Name))
            S.Fields.Add(F);
    TArray<UObject*> Objects;
    GetObjectsOfClass(Root->GeneratedClass, Objects, true, RF_NoFlags);
    for (auto* O : Objects)
        if (IsValid(O) && !O->GetClass()->HasAnyClassFlags(CLASS_NewerVersionExists) &&
            !O->GetClass()->GetName().StartsWith(TEXT("SKEL_")) &&
                                                 !O->GetClass()->GetName().StartsWith(TEXT("REINST_")))
        {
            auto* A = Cast<ADasProbeActor>(O);
            if (!A)
            {
                Error = TEXT("Rollback supports Actor adapter instances");
                return false;
            }
            if (auto* W = A->GetWorld())
                if (W->IsPartitionedWorld() || !W->GetStreamingLevels().IsEmpty())
                {
                    Error = TEXT("Unsupported loaded structural map scope");
                    return false;
                }
            FImpl::FObject Image;
            Image.Object.Reset(A);
            for (const auto& F : S.Fields)
            {
                FDasScalarValue Value;
                if (!RollbackCapture(O, F, Value, Error))
                    return false;
                Image.Values.Add(MoveTemp(Value));
            }
            Image.Archive = A->RetiredDasValues;
            S.Objects.Add(MoveTemp(Image));
            Remember(O);
        }
    TMap<FName, FEdGraphPinType> NoChanges;
    if (!S.Pins.Prepare(BPs, Root->GeneratedClass, NoChanges, EDasIntegerRule::Exact, EDasFloatRule::Exact, Error))
        return false;
    for (const auto& O : S.Objects)
    {
        for (const auto& V : O.Values)
            S.ObjectPins.Keep(V);
        S.ObjectPins.Keep(O.Archive);
    }
    S.Handle = FCoreUObjectDelegates::OnObjectsReplaced.AddLambda(
        [&S](const FCoreUObjectDelegates::FReplacementObjectMap& Map)
        {
            for (auto& O : S.Objects)
            {
                if (auto* New = Map.Find(O.Object.Get()))
                    O.Object.Reset(Cast<ADasProbeActor>(*New));
                for (auto& V : O.Values)
                    FDasPropertyValueRoots::Remap(V, Map);
                FDasPropertyValueRoots::Remap(O.Archive, Map);
            }
            S.ObjectPins.RemapPins(Map);
        });
    return true;
}
bool FDasBlueprintRollback::Finish(FString& Error)
{
    auto& S = *Impl;
    for (int32 I = 1; I < S.Blueprints.Num(); ++I)
    {
        auto* B = S.Blueprints[I].BP.Get();
        FKismetEditorUtilities::CompileBlueprint(B);
        FBlueprintCompilationManager::FlushCompilationQueueAndReinstance();
        if (!B->GeneratedClass || B->Status == BS_Error)
        {
            Error = TEXT("Dependent Blueprint compilation failed");
            return false;
        }
    }
    for (auto& B : S.Blueprints)
        B.BP->MarkPackageDirty();
    for (auto& O : S.Objects)
        if (O.Object.IsValid())
            O.Object->MarkPackageDirty();
    for (auto& A : S.Assets)
        A->MarkPackageDirty();
    return true;
}
bool FDasBlueprintRollback::Restore(FString& Error)
{
    auto& S = *Impl;
    for (auto& I : S.Blueprints)
    {
        auto* B = I.BP.Get();
        TArray<UEdGraph*> Current;
        B->GetAllGraphs(Current);
        for (auto* G : Current)
            if (!S.Graphs.ContainsByPredicate([&](const auto& Old) { return Old.Graph.Get() == G; }))
                FBlueprintEditorUtils::RemoveGraph(B, G, EGraphRemoveFlags::MarkTransient);
        B->NewVariables = I.Variables;
        B->FunctionGraphs = I.Functions;
        B->UbergraphPages = I.Uber;
        B->DelegateSignatureGraphs = I.Delegates;
        B->MacroGraphs = I.Macros;
        B->BlueprintDisplayName = I.Display;
        B->BlueprintDescription = I.Description;
        B->BlueprintCategory = I.Category;
        B->GetOutermost()->GetMetaData().SetObjectValues(B, I.Metadata);
    }
    for (auto& I : S.Graphs)
        I.Graph->GetOutermost()->GetMetaData().SetObjectValues(I.Graph.Get(), I.Metadata);
    for (auto& I : S.Nodes)
    {
        if (auto* V = Cast<UK2Node_Variable>(I.Node.Get()))
        {
            const FName Current = V->VariableReference.GetMemberName(), Old = I.Reference.GetMemberName();
            for (auto* P : V->Pins)
                if (P->PinName == Current)
                    P->PinName = Old;
            V->VariableReference = I.Reference;
        }
        else if (auto* C = Cast<UK2Node_CallFunction>(I.Node.Get()))
            C->FunctionReference = I.Reference;
        else if (auto* E = Cast<UK2Node_FunctionEntry>(I.Node.Get()))
            E->MetaData = I.EntryMetadata;
    }
    bool OK = true;
    FString PinError;
    OK &= S.Pins.Restore(PinError);
    for (auto& I : S.Blueprints)
    {
        auto* B = I.BP.Get();
        FBlueprintEditorUtils::MarkBlueprintAsStructurallyModified(B);
        FKismetEditorUtilities::CompileBlueprint(B);
        FBlueprintCompilationManager::FlushCompilationQueueAndReinstance();
        OK &= B->GeneratedClass && B->Status != BS_Error;
    }
    OK &= S.Pins.Restore(PinError);
    for (auto& I : S.Objects)
    {
        if (!I.Object.IsValid())
        {
            OK = false;
            continue;
        }
        for (int32 N = 0; N < S.Fields.Num(); ++N)
            OK &= RollbackValue(I.Object.Get(), S.Fields[N].Name, I.Values[N]);
        I.Object->RetiredDasValues = I.Archive;
    }
    for (auto& I : S.Blueprints)
    {
        auto* B = I.BP.Get();
        if (!B->GeneratedClass)
        {
            OK = false;
            continue;
        }
        B->GetOutermost()->GetMetaData().SetObjectValues(B->GeneratedClass, I.ClassMetadata);
        for (const auto& M : I.FunctionMetadata)
        {
            auto* F = B->GeneratedClass->FindFunctionByName(M.Key);
            if (F)
                F->GetOutermost()->GetMetaData().SetObjectValues(F, M.Value);
            else
                OK = false;
        }
        if (auto* C = Cast<UBlueprintGeneratedClass>(B->GeneratedClass))
            C->UpdateCustomPropertyListForPostConstruction();
        B->GetOutermost()->GetMetaData().SetObjectValues(B, I.Metadata);
    }
    if (OK)
        for (const auto& P : S.Dirty)
            P.Key->SetDirtyFlag(P.Value);
    else
        Error = TEXT("Blueprint rollback could not restore every graph/value/class");
    return OK;
}
