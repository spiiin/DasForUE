#include "DasExtendedDefaultsCommandlet.h"
#include "DasDSLService.h"
#include "DasRuntime.h"
#include "DasAuthoring.h"
#include "DasExtendedDefaultsProbe.h"
#include "DasProbeActor.h"
#include "Engine/Blueprint.h"
#include "Engine/World.h"
#include "UObject/UnrealType.h"
#include "UObject/Script.h"
#include "UObject/SavePackage.h"
#include "Misc/PackageName.h"
#include "HAL/FileManager.h"

UDasExtendedDefaultsCommandlet::UDasExtendedDefaultsCommandlet()
{
    IsEditor = true;
    IsClient = false;
    IsServer = false;
    LogToConsole = true;
}
int32 UDasExtendedDefaultsCommandlet::Main(const FString& Params)
{
    FString Id, Phase;
    FParse::Value(*Params, TEXT("ProbeId="), Id);
    FParse::Value(*Params, TEXT("Phase="), Phase);
    FGuid Guid;
    if (!FGuid::ParseExact(Id, EGuidFormats::Digits, Guid))
        return 1;
    FEditorScriptExecutionGuard Guard;
    const FString Root = TEXT("/Game/DasExtendedDefaults/")+ Id;
    UBlueprint* BP = nullptr;
    FString Error;
    auto Save = [](UObject* O)
    {
        const FString File = FPackageName::LongPackageNameToFilename(
            O->GetOutermost()->GetName(),
            O->IsA<UWorld>() ? FPackageName::GetMapPackageExtension() : FPackageName::GetAssetPackageExtension());
        IFileManager::Get().MakeDirectory(*FPaths::GetPath(File), true);
        FSavePackageArgs Args;
        Args.TopLevelFlags = RF_Public | RF_Standalone;
        return UPackage::SavePackage(O->GetOutermost(), O, *File, Args);
    };
    auto& R = FModuleManager::LoadModuleChecked<FDasRuntimeModule>(TEXT("DasRuntime"));
    if (Phase == TEXT("Create"))
    {
        for (const TCHAR* Case :
             {TEXT("duplicate"), TEXT("unknown"), TEXT("expression"), TEXT("class"), TEXT("asset"), TEXT("missing")})
        {
            FDasAuthoringModule Invalid;
            FString Issue;
            const FString Script = FString(TEXT("dsl_extended_defaults_bad_")) + Case + TEXT(".das");
            if (R.ReadAuthoringScript(R.ScriptPath(Script), Invalid, Issue) || Issue.IsEmpty())
            {
                UE_LOG(LogTemp, Error, TEXT("Expected rejection for %s"), *Script);
                return 1;
            }
            UBlueprint* Rejected = nullptr;
            const FString RejectedPackage = Root + TEXT("/BP_Rejected_")+ Case;
            if (FDasDSLService::Apply(Script, RejectedPackage,
                                      TEXT("badextendeddefaults.BadDefaults"), Rejected, Issue) ||
                                      FindPackage(nullptr, *RejectedPackage))
            {
                UE_LOG(LogTemp, Error, TEXT("Rejected defaults created an asset: %s"), Case);
                return 1;
            }
            UE_LOG(LogTemp, Display, TEXT("PASS defaults_preflight_rejects_%s_before_asset_creation"), Case);
        }
        FDasAuthoringModule Model, Roundtrip;
        FString Wire;
        if (!R.ReadAuthoringScript(R.ScriptPath(TEXT("dsl_extended_defaults.das")), Model, Error) ||
                                   !Model.ToJson(Wire, Error) || !FDasAuthoringModule::FromJson(Wire, Roundtrip, Error))
        {
            UE_LOG(LogTemp, Error, TEXT("Defaults model: %s"), *Error);
            return 1;
        }
        if (!FDasDSLService::Apply(TEXT("dsl_extended_defaults.das"),
                                        Root + TEXT("/BP_Defaults"), TEXT("extendeddefaults.ExtendedDefaults"), BP,
                                                                          Error))
        {
            UE_LOG(LogTemp, Error, TEXT("Defaults import: %s"), *Error);
            return 1;
        }
        auto* Worker = LoadObject<UBlueprint>(nullptr, *(Root + TEXT("/BP_DefaultsWorker.BP_DefaultsWorker")));
        if (!Worker || !Save(Worker) || !Save(BP))
            return 1;
    }
    else
        BP = LoadObject<UBlueprint>(nullptr, *(Root + TEXT("/BP_Defaults.BP_Defaults")));
    if (!BP || !BP->GeneratedClass)
        return 1;
    bool Good = true;
    for (auto Type : {EWorldType::Editor, EWorldType::Game})
    {
        auto* W = UWorld::CreateWorld(Type, false);
        FString Report;
        Good &= RunDasExtendedDefaultsProbe(BP->GeneratedClass, W, Report);
        UE_LOG(LogTemp, Display, TEXT("%s"), *Report);
        W->DestroyWorld(false);
    }
    if (!Good)
        return 1;
    UBlueprint* Again = nullptr;
    if (!FDasDSLService::Apply(TEXT("dsl_extended_defaults.das"),
                                    Root + TEXT("/BP_Defaults"),
                                                TEXT("extendeddefaults.ExtendedDefaults"), Again, Error) || Again != BP)
    {
        UE_LOG(LogTemp, Error, TEXT("Unchanged defaults reimport: %s"), *Error);
        return 1;
    }
    if (Phase == TEXT("Create"))
    {
        auto* Map =
            UWorld::CreateWorld(EWorldType::Editor, false, FName(TEXT("Map")), CreatePackage(*(Root + TEXT("/Map"))));
        if (!Map)
            return 1;
        Map->SetFlags(RF_Public | RF_Standalone);
        Map->GetOutermost()->SetPackageFlags(PKG_ContainsMap);
        auto* Placed = Map->SpawnActor<ADasProbeActor>(BP->GeneratedClass);
        if (!Placed)
            return 1;
        Placed->Tags.Add(TEXT("ExtendedDefaultsFixture"));
        Good = Save(Map);
        Map->DestroyWorld(false);
    }
    if (!Good)
        return 1;
    UE_LOG(LogTemp, Display, TEXT("DAS_EXTENDED_DEFAULTS_RESULT=PASS phase=%s"), *Phase);
    return 0;
}
