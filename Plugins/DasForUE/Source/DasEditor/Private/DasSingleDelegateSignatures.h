#pragma once
#include "Misc/PackageName.h"
#include "Misc/Paths.h"
#include "HAL/FileManager.h"
#include "UObject/SavePackage.h"
// A separate generated Blueprint asset owns serializable signature UFunctions.
// Separating packages avoids a circular load dependency on the authored class.
namespace
{
FName DasSingleSignatureName(const FDasFieldSchema& F)
{
    return FName(*(TEXT("__DasDelegate_")+ F.Name.ToString()));
}
FDasFieldSchema DasMethodDelegateField(const FString& Wire, const FString& Owner)
{
    FDasFieldSchema F;
    const FString Hash = FMD5::HashAnsiString(*Wire);
    F.Name = FName(*(TEXT("Type_")+ Hash));
    F.StableId = Owner + TEXT(".delegate.")+ Hash;
    F.Kind = EDasScalarKind::Delegate;
    F.DelegateWire = Wire;
    F.bInstanceEditable = false;
    return F;
}
TArray<FDasFieldSchema> DasAllSingleSignatures(const FDasClassSchema& Model)
{
    auto Fields = Model.Fields;
    TSet<FString> Seen;
    auto Add = [&](const FString& Wire)
    {
        if (!Seen.Contains(Wire))
        {
            Seen.Add(Wire);
            Fields.Add(DasMethodDelegateField(Wire, Model.StableId));
        }
    };
    for (const auto& M : Model.EffectiveMethods())
    {
        if (M.ReturnKind == EDasMethodSignature::Delegate)
            Add(M.ReturnClass);
        for (const auto& P : M.EffectiveParameters())
            if (P.Kind == EDasMethodSignature::Delegate)
                Add(P.ClassPath);
    }
    return Fields;
}
bool DasEnsureSingleSignatures(UBlueprint* BP, const FDasClassSchema& Model, FString& Error)
{
    const auto Fields = DasAllSingleSignatures(Model);
    const bool Needed = Fields.ContainsByPredicate([](const auto& F) { return F.Kind == EDasScalarKind::Delegate; });
    if (!Needed)
        return true;
    const FString HolderPackage = BP->GetOutermost()->GetName() + TEXT("_DelegateSignatures");
    const FName HolderName(*(BP->GetName() + TEXT("_DelegateSignatures")));
    auto* Holder =
        LoadObject<UBlueprint>(nullptr, *(HolderPackage + TEXT(".")+ HolderName.ToString()), nullptr, LOAD_NoWarn);
    if (Holder &&
        FString(Holder->GetOutermost()->GetMetaData().GetValue(Holder, TEXT("DasSignatureOwner"))) != Model.StableId)
    {
        Error = TEXT("Delegate signature asset conflicts with an unrelated asset");
        return false;
    }
    if (!Holder && FPackageName::DoesPackageExist(HolderPackage))
    {
        Error = TEXT("Delegate signature package already exists with another asset");
        return false;
    }
    if (!Holder)
        Holder = FKismetEditorUtilities::CreateBlueprint(
            UObject::StaticClass(), CreatePackage(*HolderPackage), HolderName, BPTYPE_Normal, UBlueprint::StaticClass(),
            UBlueprintGeneratedClass::StaticClass(), TEXT("DasDelegateSignatures"));
    if (!Holder)
    {
        Error = TEXT("Cannot create delegate signature holder");
        return false;
    }
    Holder->GetOutermost()->GetMetaData().SetValue(Holder, TEXT("DasSignatureOwner"), *Model.StableId);
    bool Added = false;
    for (const auto& F : Fields)
        if (F.Kind == EDasScalarKind::Delegate)
        {
            const FName Name = DasSingleSignatureName(F);
            if (auto* Existing =
                    Holder->FunctionGraphs.FindByPredicate([&](const auto& G) { return G && G->GetFName() == Name; }))
            {
                if (FString(Holder->GetOutermost()->GetMetaData().GetValue(*Existing, TEXT("DasDelegateId"))) !=
                            F.StableId)
                {
                    Error = TEXT("Delegate signature graph name conflicts with existing graph");
                    return false;
                }
                continue;
            }
            FDasAuthoringModule Definition;
            if (!FDasAuthoringModule::FromJson(F.DelegateWire, Definition, Error))
                return false;
            TStrongObjectPtr<UFunction> Signature(MakeDasDelegateSignature(Definition.Classes[0].Methods[0], false,
                                                                           GetTransientPackage(), NAME_None, Error));
            if (!Signature)
                return false;
            auto* G = FBlueprintEditorUtils::CreateNewGraph(Holder, Name, UEdGraph::StaticClass(),
                                                            UEdGraphSchema_K2::StaticClass());
            FBlueprintEditorUtils::AddFunctionGraph<UFunction>(Holder, G, true, Signature.Get());
            G->bEditable = false;
            G->bAllowDeletion = false;
            TArray<UK2Node_FunctionEntry*> Entries;
            G->GetNodesOfClass(Entries);
            if (Entries.Num() != 1)
            {
                Error = TEXT("Missing delegate signature entry");
                return false;
            }
            Entries[0]->SetExtraFlags(FUNC_Public | FUNC_BlueprintCallable);
            Entries[0]->MetaData.SetMetaData(TEXT("BlueprintInternalUseOnly"), FString(TEXT("true")));
            Holder->GetOutermost()->GetMetaData().SetValue(G, TEXT("DasDelegateId"), *F.StableId);
            Added = true;
        }
    if (Added)
    {
        FKismetEditorUtilities::CompileBlueprint(Holder);
        if (Holder->Status == BS_Error)
        {
            Error = TEXT("Delegate signature compilation failed");
            return false;
        }
        const FString File =
            FPackageName::LongPackageNameToFilename(HolderPackage, FPackageName::GetAssetPackageExtension());
        IFileManager::Get().MakeDirectory(*FPaths::GetPath(File), true);
        FSavePackageArgs Save;
        Save.TopLevelFlags = RF_Public | RF_Standalone;
        if (!UPackage::SavePackage(Holder->GetOutermost(), Holder, *File, Save))
        {
            Error = TEXT("Cannot save delegate signature asset");
            return false;
        }
    }
    return true;
}
FEdGraphPinType DasSingleDelegatePin(UBlueprint* BP, const FDasFieldSchema& F)
{
    FEdGraphPinType P;
    P.PinCategory = UEdGraphSchema_K2::PC_Delegate;
    auto* Holder = LoadObject<UBlueprint>(
        nullptr, *(BP->GetOutermost()->GetName() +
                   TEXT("_DelegateSignatures.")+ BP->GetName() + TEXT("_DelegateSignatures")), nullptr, LOAD_NoWarn);
    P.PinSubCategoryMemberReference.MemberParent = Holder ? Holder->GeneratedClass : nullptr;
    P.PinSubCategoryMemberReference.MemberName = DasSingleSignatureName(F);
    return P;
}
FEdGraphPinType DasMethodDelegatePin(UBlueprint* BP, const FString& Wire)
{
    return DasSingleDelegatePin(BP, DasMethodDelegateField(Wire, FString()));
}

}
