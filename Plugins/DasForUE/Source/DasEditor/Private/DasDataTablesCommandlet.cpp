#include "DasDataTablesCommandlet.h"
#include "DasTypeService.h"
#include "DasRuntime.h"
#include "DasSchema.h"
#include "UObject/Package.h"
#include "Misc/Paths.h"
#include "Engine/DataTable.h"
#include "UObject/StrongObjectPtr.h"
#include "UObject/GarbageCollection.h"
#include "UObject/UnrealType.h"
#include "StructUtils/UserDefinedStruct.h"
#include "StructUtils/InstancedStruct.h"
#include "Misc/FileHelper.h"
#include "Misc/ScopeExit.h"
#include "HAL/FileManager.h"

UDasDataTablesCommandlet::UDasDataTablesCommandlet(){IsClient=false;IsServer=false;IsEditor=true;LogToConsole=true;}
int32 UDasDataTablesCommandlet::Main(const FString& Params){
 auto& R=FModuleManager::LoadModuleChecked<FDasRuntimeModule>(TEXT("DasRuntime"));FString Error;
 int32 Checks=0,Failures=0;auto Check=[&](bool Good,const TCHAR* Label){++Checks;if(!Good)++Failures;UE_LOG(LogTemp,Display,TEXT("DAS_DATA_TABLES_%s %s"),Good?TEXT("PASS"):TEXT("FAIL"),Label);return Good;};
 const FString Tag=FGuid::NewGuid().ToString(EGuidFormats::Digits),Folder=TEXT("/Game/Tests/DasDataTables/")+Tag;
 const FString ExampleFile=TEXT("datatable_example_probe_")+Tag+TEXT(".das"),ProbeFile=TEXT("datatable_runtime_probe_")+Tag+TEXT(".das");
 const FString ExamplePath=R.ScriptPath(ExampleFile),ProbePath=R.ScriptPath(ProbeFile);
 ON_SCOPE_EXIT{IFileManager::Get().Delete(*ExamplePath,false,true);IFileManager::Get().Delete(*ProbePath,false,true);};
 TArray<UObject*> Types;if(!Check(FDasTypeService::Apply(TEXT("datatable_item_types.das"),Folder,Types,Error) && Types.Num()==1,TEXT("authored_item_row_schema"))){UE_LOG(LogTemp,Display,TEXT("%s"),*Error);return 1;}
 auto* RowType=Cast<UScriptStruct>(Types[0]);if(!RowType)return 1;
 TStrongObjectPtr<UDataTable> Table(NewObject<UDataTable>());Table->RowStruct=RowType;
 ON_SCOPE_EXIT{R.ReleaseObject(Table.Get());for(auto* T:Types){T->GetOutermost()->SetDirtyFlag(false);T->ClearFlags(RF_Standalone);}};
 FInstancedStruct Row;Row.InitializeAs(RowType);
 auto* Label=CastField<FStrProperty>(FindDasStructField(RowType,TEXT("Label")));auto* Price=CastField<FIntProperty>(FindDasStructField(RowType,TEXT("Price")));auto* Weight=CastField<FDoubleProperty>(FindDasStructField(RowType,TEXT("Weight")));
 if(!Check(Label && Price && Weight,TEXT("reflected_item_fields")))return 1;
 auto Set=[&](const TCHAR* Name,int32 Cost,double Mass){Label->SetPropertyValue_InContainer(Row.GetMutableMemory(),Name);Price->SetPropertyValue_InContainer(Row.GetMutableMemory(),Cost);Weight->SetPropertyValue_InContainer(Row.GetMutableMemory(),Mass);};
 Set(TEXT("Health Potion"),25,.5);Table->AddRow(TEXT("Potion"),Row.GetMemory(),RowType);Set(TEXT("Iron Sword"),125,3.);Table->AddRow(TEXT("Sword"),Row.GetMemory(),RowType);
 FString Example,Probe,Fixture;if(!FParse::Value(*Params,TEXT("Fixture="),Fixture) || !FFileHelper::LoadFileToString(Probe,*Fixture) || !FFileHelper::LoadFileToString(Example,*R.ScriptPath(TEXT("datatable_items.das"))))return 2;
 Example.ReplaceInline(TEXT("/Game/ItemTypes"),*Folder);Probe.ReplaceInline(TEXT("__FOLDER__"),*Folder);Probe.ReplaceInline(TEXT("__EXAMPLE__"),*FPaths::GetBaseFilename(ExampleFile));
 if(!FFileHelper::SaveStringToFile(Example,*ExamplePath,FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM) || !FFileHelper::SaveStringToFile(Probe,*ProbePath,FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM))return 2;
 auto Call=[&](const TCHAR* Entry){auto V=R.RunNamedPersistent(ProbePath,Entry,0,Table.Get());if(!V.bSucceeded)UE_LOG(LogTemp,Display,TEXT("DataTable VM: %s"),*V.Error);return V;};
 const auto Read=Call(TEXT("start"));Check(Read.bSucceeded && Read.Value==1,TEXT("typed_rows_names_missing_and_wrong_type"));
 Table->RemoveRow(TEXT("Potion"));CollectGarbage(RF_NoFlags);const auto Retained=Call(TEXT("after_mutation"));Check(Retained.bSucceeded && Retained.Value==1,TEXT("owned_row_survives_native_removal_and_gc"));
 Table->EmptyTable();const auto Empty=Call(TEXT("empty_table"));Check(Empty.bSucceeded && Empty.Value==1,TEXT("empty_name_list"));
 UE_LOG(LogTemp,Display,TEXT("DAS_DATA_TABLES_RESULT=%s checks=%d"),Failures?TEXT("FAIL"):TEXT("PASS"),Checks);return Failures?1:0;
}
