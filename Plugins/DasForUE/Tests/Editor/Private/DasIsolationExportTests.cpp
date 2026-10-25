#include "DasActor.h"
#include "DasDSLService.h"
#include "Engine/Blueprint.h"
#include "Misc/AutomationTest.h"
#include "Misc/PackageName.h"
#include "Misc/Paths.h"
#include "HAL/FileManager.h"
#include "UObject/SavePackage.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FDasIsolationExportTest, "DasForUE.Hosts.ExportIsolation",
                                 EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FDasIsolationExportTest::RunTest(const FString& Parameters)
{
    if (FPaths::GetBaseFilename(FPaths::GetProjectFilePath()) != TEXT("DasRuntimeIsolation"))
    {
        AddInfo(TEXT("Export belongs to the DasRuntimeIsolation project; run runtime-isolation-packaged."));
        return true;
    }
    UBlueprint* BP = nullptr;
    FString Error;
    if (!FDasDSLService::Apply(TEXT("isolation_actor.das"), TEXT("/Game/RuntimeIsolation/BP_IsolationActor"),
                                                                 TEXT("isolation.IsolationActor"), BP, Error))
    {
        AddError(Error);
        return false;
    }
    if (!TestTrue(TEXT("Production host"), BP->ParentClass == ADasActor::StaticClass()))
        return false;
    const FString File = FPackageName::LongPackageNameToFilename(BP->GetOutermost()->GetName(),
                                                                 FPackageName::GetAssetPackageExtension());
    IFileManager::Get().MakeDirectory(*FPaths::GetPath(File), true);
    FSavePackageArgs Args;
    Args.TopLevelFlags = RF_Public | RF_Standalone;
    return TestTrue(TEXT("Save production asset for cooking"),
                         UPackage::SavePackage(BP->GetOutermost(), BP, *File, Args));
}
