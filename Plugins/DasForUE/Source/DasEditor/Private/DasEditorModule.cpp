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
        ReimportCommand=IConsoleManager::Get().RegisterConsoleCommand(TEXT("Das.DSL.Reimport"),TEXT("Das.DSL.Reimport /Game/Folder/BP_Name [defaults=preserve_instances | fields=rename_by_id | fields=rename_with_dependencies | fields=widen_numeric | fields=widen_numeric_with_dependencies]"),FConsoleCommandWithArgsDelegate::CreateLambda([](const TArray<FString>& Args){
            FString Error;UBlueprint* BP=nullptr;
            if(Args.Num()==1 || Args.Num()==2) BP=LoadObject<UBlueprint>(nullptr,*(Args[0]+TEXT(".")+FPackageName::GetLongPackageAssetName(Args[0])),nullptr,LOAD_NoWarn);
            const bool Valid=Args.Num()==1 || (Args.Num()==2 && (Args[1]==TEXT("defaults=preserve_instances") || Args[1]==TEXT("fields=rename_by_id") || Args[1]==TEXT("fields=rename_with_dependencies") || Args[1]==TEXT("fields=widen_numeric") || Args[1]==TEXT("fields=widen_numeric_with_dependencies")));
            if(Valid && Args.Num()==2 && (Args[1]==TEXT("fields=widen_numeric") || Args[1]==TEXT("fields=widen_numeric_with_dependencies"))){if(!FDasDSLService::ReimportWidenedNumericFields(BP,Error,Args[1]==TEXT("fields=widen_numeric_with_dependencies")))UE_LOG(LogTemp,Warning,TEXT("DAS_DSL_WIDEN rejected: %s"),*Error);return;}
            if(!Valid || !(Args.Num()==2 && (Args[1]==TEXT("fields=rename_by_id") || Args[1]==TEXT("fields=rename_with_dependencies"))?FDasDSLService::ReimportRenamedFields(BP,Error,Args[1]==TEXT("fields=rename_with_dependencies")):FDasDSLService::Reimport(BP,Error,Args.Num()==2?EDasDefaultMigrationPolicy::PreserveInstances:EDasDefaultMigrationPolicy::Refuse))) UE_LOG(LogTemp,Warning,TEXT("DAS_DSL_REIMPORT rejected: %s"),!Valid?TEXT("Expected asset package and optional defaults=preserve_instances or fields=rename_by_id"):*Error);
        }),ECVF_Default);
    }
    virtual void ShutdownModule() override {if(ApplyCommand) IConsoleManager::Get().UnregisterConsoleObject(ApplyCommand);if(ReimportCommand) IConsoleManager::Get().UnregisterConsoleObject(ReimportCommand);}
};
IMPLEMENT_MODULE(FDasEditorModule,DasEditor)
