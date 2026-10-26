#include "DasSourceCatalogCommandlet.h"
#include "DasScriptHosts.h"
#include "DasSourceCatalog.h"
#include "DasAuthoring.h"
#include "DasReflectedTypes.h"
#include "DasRuntime.h"
#include "Modules/ModuleManager.h"
#include "Misc/Parse.h"
#include "Misc/FileHelper.h"
#include "Misc/SecureHash.h"
UDasSourceCatalogCommandlet::UDasSourceCatalogCommandlet()
{
    IsClient = false;
    IsServer = false;
    IsEditor = true;
    LogToConsole = true;
}
int32 UDasSourceCatalogCommandlet::Main(const FString& Params)
{
    FString Scripts, TypeScripts, InterfaceScripts,
        Output = TEXT("ue_type_catalog.json"), Module = TEXT("ue_project_refs");
    FParse::Value(*Params, TEXT("Scripts="), Scripts);
    FParse::Value(*Params, TEXT("TypeScripts="), TypeScripts);
    FParse::Value(*Params, TEXT("InterfaceScripts="), InterfaceScripts);
    FParse::Value(*Params, TEXT("Output="), Output);
    FParse::Value(*Params, TEXT("Module="), Module);
    auto Fail = [](const FString& E)
    {
        UE_LOG(LogTemp, Error, TEXT("DAS_SOURCE_CATALOG FAIL %s"), *E);
        return 1;
    };
    if (!FDasSourceCatalog::LocalFile(Output, TEXT("json")) ||
                                      !FDasSourceCatalog::LocalFile(Module + TEXT(".das"), TEXT("das")))
        return Fail(TEXT("Expected local output and module identifier"));
    TArray<FString> Files;
    if (!Scripts.IsEmpty())
        Scripts.ParseIntoArray(Files, TEXT(","), false);
    TArray<FString> TypeFiles;
    if (!TypeScripts.IsEmpty())
        TypeScripts.ParseIntoArray(TypeFiles, TEXT(","), false);
    TArray<FString> InterfaceFiles;
    if (!InterfaceScripts.IsEmpty())
        InterfaceScripts.ParseIntoArray(InterfaceFiles, TEXT(","), false);
    if ((Files.IsEmpty() && TypeFiles.IsEmpty() && InterfaceFiles.IsEmpty()) ||
        Files.Num() + TypeFiles.Num() + InterfaceFiles.Num() > 256)
        return Fail(TEXT("Expected Scripts/TypeScripts/InterfaceScripts (1..256 explicit sources)"));
    auto& R = FModuleManager::LoadModuleChecked<FDasRuntimeModule>(TEXT("DasRuntime"));
    FDasSourceCatalog Catalog;
    TSet<FString> Seen;
    FString Issue;
    const bool Discover = !FParse::Param(*Params, TEXT("ExplicitTypeImports"));
    auto MergeValueModule = [&](FDasCatalogValueModule Entry)
    {
        auto* Existing = Catalog.ValueModules.FindByPredicate(
            [&](const auto& M)
            {
                return M.Module.Equals(Entry.Module, ESearchCase::IgnoreCase) ||
                       M.Script.Equals(Entry.Script, ESearchCase::IgnoreCase);
            });
        if (!Existing)
        {
            Catalog.ValueModules.Add(MoveTemp(Entry));
            return true;
        }
        if (Existing->Module != Entry.Module || Existing->Script != Entry.Script)
        {
            Issue = TEXT("Conflicting discovered module/source mapping: ")+ Entry.Module;
            return false;
        }
        auto Sort = [](auto& Types)
        {
            Types.Sort([](const auto& A, const auto& B) { return A.Id < B.Id; });
        };
        Sort(Existing->Types);
        Sort(Entry.Types);
        if (Existing->Types.Num() != Entry.Types.Num())
        {
            Issue = TEXT("Changed catalog type inventory: ")+ Entry.Module;
            return false;
        }
        for (int32 I = 0; I < Entry.Types.Num(); ++I)
            if (Existing->Types[I].Id != Entry.Types[I].Id || Existing->Types[I].Name != Entry.Types[I].Name ||
                Existing->Types[I].Kind != Entry.Types[I].Kind)
            {
                Issue = TEXT("Conflicting catalog type inventory: ")+ Entry.Module;
                return false;
            }
        return true;
    };
    for (auto File : Files)
    {
        File = File.TrimStartAndEnd();
        if (!FDasSourceCatalog::LocalFile(File, TEXT("das")) || Seen.Contains(File.ToLower()))
            return Fail(TEXT("Invalid/duplicate source: ")+ File);
        Seen.Add(File.ToLower());
        FDasAuthoringModule Model;
        FDasSourceCatalog Discovered;
        if (!R.ReadAuthoringScript(R.ScriptPath(File), Model, Issue, Discover ? &Discovered : nullptr))
            return Fail(File + TEXT(": ")+ Issue);
        for (auto& M : Discovered.ValueModules)
            if (!MergeValueModule(MoveTemp(M)))
                return Fail(Issue);
        for (const auto& C : Model.Classes)
        {
            if (!DasActorNativeParent(C.NativeParent) && C.NativeParent != TEXT("/Script/Engine.ActorComponent"))
                return Fail(TEXT("Source catalog references require supported Actor/Component hosts"));
            FDasSourceCatalogEntry Entry;
            Entry.Id = C.Id;
            Entry.Name = C.Name;
            Entry.Script = File;
            Entry.Reference = C.Name + TEXT("Ref");
            Catalog.Entries.Add(MoveTemp(Entry));
        }
    }
    for (auto File : TypeFiles)
    {
        File = File.TrimStartAndEnd();
        if (!FDasSourceCatalog::LocalFile(File, TEXT("das")) || Seen.Contains(File.ToLower()))
            return Fail(TEXT("Invalid/duplicate value source: ")+ File);
        Seen.Add(File.ToLower());
        FDasReflectedTypes Model;
        FDasCatalogValueModule Entry;
        Entry.Script = File;
        if (!R.ReadReflectedTypes(R.ScriptPath(File), Model, Issue, &Entry.Module))
            return Fail(File + TEXT(": ")+ Issue);
        for (const auto& E : Model.Enums)
        {
            FDasCatalogValueType T;
            T.Kind = TEXT("enum");
            T.Id = E.Id;
            T.Name = E.Name;
            Entry.Types.Add(MoveTemp(T));
        }
        for (const auto& E : Model.Structs)
        {
            FDasCatalogValueType T;
            T.Kind = TEXT("struct");
            T.Id = E.Id;
            T.Name = E.Name;
            Entry.Types.Add(MoveTemp(T));
        }
        if (!MergeValueModule(MoveTemp(Entry)))
            return Fail(Issue);
    }
    for (auto File : InterfaceFiles)
    {
        File = File.TrimStartAndEnd();
        if (!FDasSourceCatalog::LocalFile(File, TEXT("das")) || Seen.Contains(File.ToLower()))
            return Fail(TEXT("Invalid/duplicate interface source: ")+ File);
        Seen.Add(File.ToLower());
        FDasSourceCatalog Imports;
        if (!R.ReadSourceCatalogImports(R.ScriptPath(File), Imports, Issue))
            return Fail(File + TEXT(": ")+ Issue);
        for (auto& M : Imports.ValueModules)
            if (!MergeValueModule(MoveTemp(M)))
                return Fail(Issue);
    }
    if (Seen.Contains((Module + TEXT(".das")).ToLower()))
        return Fail(TEXT("Generated module would overwrite an input source"));
    for (const auto& M : Catalog.ValueModules)
        if (M.Script.Equals(Module + TEXT(".das"), ESearchCase::IgnoreCase))
            return Fail(TEXT("Generated module would overwrite a discovered value source"));
    TMap<FString, int32> Counts;
    for (const auto& E : Catalog.Entries)
        ++Counts.FindOrAdd(E.Reference.ToLower());
    for (auto& E : Catalog.Entries)
        if (Counts[E.Reference.ToLower()] > 1)
            E.Reference += TEXT("_")+ FMD5::HashAnsiString(*E.Id).Left(8);
    FString Json, Source;
    if (!Catalog.ToJson(Json, Issue) || !Catalog.ReferenceModule(Module, Output, Source, Issue))
        return Fail(Issue);
    const FString CatalogPath = R.ScriptPath(Output), ModulePath = R.ScriptPath(Module + TEXT(".das"));
    if (FParse::Param(*Params, TEXT("Check")))
    {
        FString ExistingJson, ExistingSource;
        if (!FFileHelper::LoadFileToString(ExistingJson, *CatalogPath) ||
            !FFileHelper::LoadFileToString(ExistingSource, *ModulePath) || ExistingJson != Json ||
            ExistingSource != Source)
            return Fail(TEXT("Generated catalog/module differ or are missing"));
    }
    else
    {
        if (!FFileHelper::SaveStringToFile(Json, *CatalogPath, FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM) ||
            !FFileHelper::SaveStringToFile(Source, *ModulePath, FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM))
            return Fail(TEXT("Could not save catalog/module pair; regenerate before use"));
    }
    UE_LOG(LogTemp, Display,
           TEXT("DAS_SOURCE_CATALOG PASS classes=%d value_modules=%d catalog=%s module=%s"), Catalog.Entries.Num(),
                Catalog.ValueModules.Num(), *CatalogPath, *ModulePath);
    return 0;
}
