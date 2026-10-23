#include "DasDSLDiagnosticsCommandlet.h"
#include "DasDiagnosticEditor.h"
#include "DasRuntime.h"
#include "DasAuthoring.h"
#include "DasDSLService.h"
#include "Engine/Blueprint.h"
#include "Curves/CurveFloat.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "HAL/FileManager.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "Misc/ScopeExit.h"
#include "UObject/StrongObjectPtr.h"

UDasDSLDiagnosticsCommandlet::UDasDSLDiagnosticsCommandlet()
{
    IsClient = false;
    IsServer = false;
    IsEditor = true;
    LogToConsole = true;
}
int32 UDasDSLDiagnosticsCommandlet::Main(const FString&)
{
    int32 Checks = 0, Failures = 0;
    auto Check = [&](bool Good, const TCHAR* Name, const FString& Detail = FString())
    {
        ++Checks;
        if (!Good)
            ++Failures;
        UE_LOG(LogTemp, Display,
               TEXT("DAS_DSL_DIAGNOSTICS_%s %s %s"), Good ? TEXT("PASS"): TEXT("FAIL"), Name, Good ? TEXT(""): *Detail);
    };
    auto& Runtime = FModuleManager::LoadModuleChecked<FDasRuntimeModule>(TEXT("DasRuntime"));
    const FString Id = FGuid::NewGuid().ToString(EGuidFormats::Digits);
    const FString Name = TEXT("diagnostic_")+ Id;
    const FString Path = Runtime.ScriptPath(Name + TEXT(".das"));
    const FString ImportPath = Runtime.ScriptPath(Name + TEXT("_import.das"));
    TStrongObjectPtr<UObject> Owner(NewObject<UCurveFloat>());
    UBlueprint* BP = nullptr;
    ON_SCOPE_EXIT
    {
        Runtime.ReleaseObject(Owner.Get());
        IFileManager::Get().Delete(*Path, false, true);
        IFileManager::Get().Delete(*ImportPath, false, true);
        if (BP)
        {
            FAssetRegistryModule::AssetDeleted(BP);
            BP->GetOutermost()->SetDirtyFlag(false);
            BP->ClearFlags(RF_Public | RF_Standalone);
        }
    };
    auto Write = [&](const FString& Text)
    {
        return FFileHelper::SaveStringToFile(Text, *Path, FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM);
    };
    auto At = [&](const FString& Error, int32 Line, const FString& Entity = FString(), const FString& File = FString())
    {
        const auto Ds = FDasDiagnostic::Parse(Error);
        return Ds.ContainsByPredicate(
            [&](const auto& D)
            {
                return FPaths::IsSamePath(D.File, File.IsEmpty() ? Path : File) && D.Line == Line && D.Column > 0 &&
                       (Entity.IsEmpty() || D.Entity == Entity);
            });
    };
    Write(TEXT("options gen2\n[export]\ndef main(input:int):int {\n    return missing_value\n}\n"));
    auto Result = Runtime.RunFile(Path, 0);
    Check(!Result.bSucceeded && At(Result.Error, 4) &&
          Result.Error.Contains(TEXT("error[")), TEXT("compile_location_and_raw_details"), Result.Error);

    Write(TEXT("options gen2\nrequire ue_savegame\n[ue_class]\nclass Counter:SaveGame {\n    @ue_property=(BlueprintReadOnly,BlueprintReadWrite) Score:int=0\n}\n"));
    FString Error;
    FDasAuthoringModule Model;
    bool Read = Runtime.ReadAuthoringScript(Path, Model, Error);
    Check(!Read && At(Error, 5, TEXT("Counter.Score")) && Error.Contains(TEXT("Conflicting")),
                                                                         TEXT("annotation_property_source"), Error);

    Write(TEXT("options gen2\nrequire ue_foundation\n[export]\ndef main(input:int):int {\n    let value=ue_color(256,0,0,255)\n    return int(value.r)\n}\n"));
    Result = Runtime.RunFile(Path, 0);
    Check(!Result.bSucceeded && At(Result.Error, 5) &&
          Result.Error.Contains(TEXT("runtime")) && Result.Error.Contains(TEXT("Details:")),
                                                                          TEXT("native_cmres_call_site"), Result.Error);

    Write(TEXT("options gen2\nrequire ue_foundation\n[export]\ndef main(input:int):int {\n    try { let c=ue_color(256,0,0,255); return int(c.r) } recover {}\n    panic(\"after recover\")\n    return 0\n}\n"));
    Result = Runtime.RunFile(Path, 0);
    Check(!Result.bSucceeded && At(Result.Error, 6) &&
          Result.Error.Contains(TEXT("after recover")), TEXT("recover_does_not_reuse_stale_site"), Result.Error);

    const FString ImportCode =
        TEXT("options gen2\nmodule ")+ Name +
             TEXT("_import public\n[export]\ndef fail():int {\n    panic(\"import failure\")\n    return 0\n}\n");
    FFileHelper::SaveStringToFile(ImportCode, *ImportPath, FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM);
    Write(TEXT("options gen2\nrequire ")+ Name + TEXT("_import\n[export]\ndef main(input:int):int {return fail()}\n"));
    Result = Runtime.RunFile(Path, 0);
    Check(!Result.bSucceeded && At(Result.Error, 5, FString(), ImportPath), TEXT("imported_module_original_file"),
                                                                                 Result.Error);

    Write(TEXT("options gen2\n[export]\ndef main(input:int):int {\n    if (input==0) {\n        panic(\"old generation\")\n    }\n    return 42\n}\n"));
    const auto Warm = Runtime.RunPersistent(Path, 1, Owner.Get());
    Write(TEXT("options gen2\n[export]\ndef main(input:int):int {\n\n\n\n\n    return missing_new_generation\n}\n"));
    Result = Runtime.RunPersistent(Path, 0, Owner.Get());
    Check(Warm.bSucceeded && Warm.Value == 42 && !Result.bSucceeded && At(Result.Error, 5) && At(Result.ReloadError, 8),
          TEXT("failed_reload_keeps_old_vm_coordinates"), Result.Error + Result.ReloadError);
    Runtime.ReleaseObject(Owner.Get());

    Write(TEXT("options gen2\nrequire ue_savegame\n[ue_class]\nclass Counter:SaveGame {\n    [ue_function(BlueprintCallable)]\n    def Fail(input:int):int {\n        let c=ue_color(256,0,0,255)\n        return int(c.r)\n    }\n}\n"));
    Error.Reset();
    Model = {};
    Read = Runtime.ReadAuthoringScript(Path, Model, Error);
    bool Method = false;
    bool ImportFailure = false;
    if (Read && Model.Classes.Num() == 1)
    {
        UBlueprint* Unexpected = nullptr;
        FString Rejected;
        const bool Applied =
            FDasDSLService::Apply(Name + TEXT(".das"), TEXT("/Game/Tests/DasDSLDiagnostics/Rejected_")+ Id,
                                                            TEXT("not.a.class"), Unexpected, Rejected);
        const auto Ds = FDasDiagnostic::Parse(Rejected);
        ImportFailure = !Applied && !Unexpected &&
                        At(Rejected, 4, TEXT("Counter")) && Ds.Num() > 0 && Ds[0].Phase == TEXT("import");
        if (!ImportFailure)
            Error = Rejected;
    }
    if (Read && Model.Classes.Num() == 1 &&
        FDasDSLService::Apply(Name + TEXT(".das"), TEXT("/Game/Tests/DasDSLDiagnostics/BP_")+ Id, Model.Classes[0].Id,
                                                        BP, Error))
    {
        TStrongObjectPtr<UObject> Self(NewObject<UObject>(GetTransientPackage(), BP->GeneratedClass));
        const auto* M = Model.Classes[0].Methods.FindByPredicate([](const auto& V) { return V.Name == TEXT("Fail"); });
        if (M)
        {
            Result = Runtime.RunNamedPersistent(Path, M->EntryPoint, 0, Self.Get());
            Error = Result.Error;
            const auto Ds = FDasDiagnostic::Parse(Error);
            Method = !Result.bSucceeded && At(Error, 7, TEXT("Counter.Fail")) && Ds.Num() > 0 &&
                                              !Ds[0].Message.Contains(TEXT("__impl_"));
        }
        if (Method && M)
        {
            Write(TEXT("options gen2\nrequire ue_savegame\nrequire ")+ Name +
                TEXT("_import\n[ue_class]\nclass Counter:SaveGame {\n    [ue_function(BlueprintCallable)]\n    def Fail(input:int):int { return fail() }\n}\n"));
            const auto Imported = Runtime.RunNamedPersistent(Path, M->EntryPoint, 0, Self.Get());
            const auto ImportedDiagnostics = FDasDiagnostic::Parse(Imported.Error);
            Method = !Imported.bSucceeded && At(Imported.Error, 5, TEXT("Counter.Fail"), ImportPath) &&
                                                ImportedDiagnostics.Num() > 0 &&
                                                !ImportedDiagnostics[0].Entity.Contains(TEXT("ue_Counter_"));
            if (!Method)
                Error = Imported.Error;
        }
        Runtime.ReleaseObject(Self.Get());
    }
    Check(Method && ImportFailure, TEXT("authored_generated_method_and_import_diagnostics"), Error);

    Write(TEXT("options gen2\nrequire ue_savegame\n[ue_class]\nclass Counter:SaveGame {\n    [ue_function(BlueprintCallable)]\n    def Broken(input:int):int {\n        return self.MissingProperty\n    }\n}\n"));
    Error.Reset();
    Model = {};
    Read = Runtime.ReadAuthoringScript(Path, Model, Error);
    Check(!Read && At(Error, 7, TEXT("Counter.Broken")), TEXT("generated_lowering_reports_source_expression"), Error);

    FDasDiagnostic D;
    D.File = Path;
    D.Line = 7;
    D.Column = 9;
    D.Entity = TEXT("Counter.Fail");
    D.Phase = TEXT("runtime");
    D.Message = TEXT("example");
    bool Opened = false;
    auto Message = DasDiagnosticEditor::Message(D, FDasOpenDiagnosticSource::CreateLambda(
                                                       [&](const FString& File, int32 Line, int32 Col)
                                                       {
                                                           Opened =
                                                               FPaths::IsSamePath(File, Path) && Line == 7 && Col == 9;
                                                           return Opened;
                                                       }));
    for (const auto& Token : Message->GetMessageTokens())
        if (Token->GetType() == EMessageToken::Action)
            StaticCastSharedRef<FActionToken>(Token)->ExecuteAction();
    auto Stale = D;
    Stale.Line = 9999;
    auto Wrong = D;
    Wrong.File = FPaths::ChangeExtension(Path, TEXT("exe"));
    FDasDiagnostic Unknown;
    Unknown.File = Path;
    Unknown.Phase = TEXT("import");
    Unknown.Message = TEXT("no precise location");
    const auto Parsed = FDasDiagnostic::Parse(TEXT("prefix ")+ D.Format() + TEXT("\n")+ Unknown.Format());
    auto Windows = D;
    Windows.File = TEXT("C:\\Example Project\\counter.das");
    const auto WindowsParsed = FDasDiagnostic::Parse(TEXT("import failed: ")+ Windows.Format());
    Check(WindowsParsed.Num() == 1 && WindowsParsed[0].File == Windows.File && Opened &&
              !DasDiagnosticEditor::Navigate(Stale) && !DasDiagnosticEditor::Navigate(Wrong) && Parsed.Num() == 2 &&
              Parsed[1].Line == 0,
          TEXT("clickable_token_and_stale_source_rejection"));
    UE_LOG(LogTemp, Display,
           TEXT("DAS_DSL_DIAGNOSTICS_RESULT=%s checks=%d"), Failures ? TEXT("FAIL"): TEXT("PASS"), Checks);
    return Failures ? 1 : 0;
}
