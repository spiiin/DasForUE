#include "DasPrepareProgramsCommandlet.h"
#include "DasRuntime.h"
#include "Misc/ConfigCacheIni.h"
#include "Misc/Parse.h"
#include "HAL/IConsoleManager.h"
#include "Engine/World.h"
#include "UObject/StrongObjectPtr.h"
UDasPrepareProgramsCommandlet::UDasPrepareProgramsCommandlet()
{
    IsEditor = true;
    IsClient = IsServer = false;
    LogToConsole = true;
}
int32 UDasPrepareProgramsCommandlet::Main(const FString& Params)
{
    auto& Runtime = FModuleManager::LoadModuleChecked<FDasRuntimeModule>(TEXT("DasRuntime"));
    // The build tool is the explicit compilation stage, even for a strict project.
    auto* Strict = IConsoleManager::Get().FindConsoleVariable(TEXT("das.RequirePreparedPrograms"));
    const int32 Previous = Strict->GetInt();
    const bool Verify = FParse::Param(*Params, TEXT("Verify"));
    Strict->Set(Verify ? 1 : 0, ECVF_SetByCode);
    TArray<FString> Scripts;
    FString List;
    if (FParse::Value(*Params, TEXT("Scripts="), List))
        List.ParseIntoArray(Scripts, TEXT(","), true);
    else
    {
        TArray<FString> Sections;
        GConfig->GetSectionNames(GGameIni, Sections);
        for (const auto& Section : Sections)
            if (Section == TEXT("DasForUE.Programs") || Section.StartsWith(TEXT("DasForUE.Programs /")))
            {
                TArray<FString> Part;
                GConfig->GetArray(*Section, TEXT("Scripts"), Part, GGameIni);
                for (const auto& Script : Part)
                    Scripts.AddUnique(Script);
            }
    }
    Runtime.ClearProgramCache();
    if (Verify)
    {
        TStrongObjectPtr<UWorld> World(UWorld::CreateWorld(EWorldType::Game, false));
        const auto Before = Runtime.ProgramCacheStats();
        FString Error;
        const bool OK = !Scripts.IsEmpty() && Runtime.PreparePrograms(World.Get(), Scripts, Error) &&
                        Runtime.ProgramCacheStats().Compilations == Before.Compilations &&
                        Runtime.ProgramCacheStats().PreparedLoads - Before.PreparedLoads == uint64(Scripts.Num());
        World->DestroyWorld(false);
        Strict->Set(Previous, ECVF_SetByCode);
        if (OK)
        {
            UE_LOG(LogTemp, Display, TEXT("DAS_PREPARE_VERIFY PASS programs=%d compilations=0"), Scripts.Num());
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("DAS_PREPARE_VERIFY FAIL %s"), *Error);
        }
        return OK ? 0 : 1;
    }
    bool OK = !Scripts.IsEmpty();
    for (const auto& Script : Scripts)
    {
        FString Error;
        if (!Runtime.ExportPreparedProgram(Script, Error))
        {
            UE_LOG(LogTemp, Error, TEXT("DAS_PREPARE_EXPORT_FAILED %s: %s"), *Script, *Error);
            OK = false;
            break;
        }
    }
    Strict->Set(Previous, ECVF_SetByCode);
    if (OK)
        UE_LOG(LogTemp, Display, TEXT("DAS_PREPARE_EXPORT PASS programs=%d"), Scripts.Num());
    return OK ? 0 : 1;
}
