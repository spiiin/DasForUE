#include "Modules/ModuleManager.h"
#include "HAL/IConsoleManager.h"
#include "DasDSLService.h"
#include "Engine/Blueprint.h"
#include "Misc/PackageName.h"
class FDasEditorModule final : public IModuleInterface {
    IConsoleObject* ApplyCommand=nullptr;IConsoleObject* ReimportCommand=nullptr;
public:
    virtual void StartupModule() override {
        ApplyCommand=IConsoleManager::Get().RegisterConsoleCommand(TEXT("Das.DSL.Apply"),TEXT("Das.DSL.Apply ScriptName.das /Game/Folder/BP_Name ClassStableId"),FConsoleCommandWithArgsDelegate::CreateLambda([](const TArray<FString>& Args){
            FString Error;UBlueprint* BP=nullptr;
            if(Args.Num()!=3 || !FDasDSLService::Apply(Args[0],Args[1],Args[2],BP,Error)) UE_LOG(LogTemp,Warning,TEXT("DAS_DSL_APPLY rejected: %s"),Args.Num()!=3?TEXT("Expected script, asset package and class ID"):*Error);
        }),ECVF_Default);
        ReimportCommand=IConsoleManager::Get().RegisterConsoleCommand(TEXT("Das.DSL.Reimport"),TEXT("Das.DSL.Reimport /Game/Folder/BP_Name"),FConsoleCommandWithArgsDelegate::CreateLambda([](const TArray<FString>& Args){
            FString Error;UBlueprint* BP=nullptr;
            if(Args.Num()==1) BP=LoadObject<UBlueprint>(nullptr,*(Args[0]+TEXT(".")+FPackageName::GetLongPackageAssetName(Args[0])),nullptr,LOAD_NoWarn);
            if(Args.Num()!=1 || !FDasDSLService::Reimport(BP,Error)) UE_LOG(LogTemp,Warning,TEXT("DAS_DSL_REIMPORT rejected: %s"),Args.Num()!=1?TEXT("Expected asset package"):*Error);
        }),ECVF_Default);
    }
    virtual void ShutdownModule() override {if(ApplyCommand) IConsoleManager::Get().UnregisterConsoleObject(ApplyCommand);if(ReimportCommand) IConsoleManager::Get().UnregisterConsoleObject(ReimportCommand);}
};
IMPLEMENT_MODULE(FDasEditorModule,DasEditor)
