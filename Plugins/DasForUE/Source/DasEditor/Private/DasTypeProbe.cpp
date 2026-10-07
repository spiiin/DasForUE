#include "DasTypeService.h"
#include "DasProbeActor.h"
#include "Engine/Blueprint.h"
#include "Engine/BlueprintGeneratedClass.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "Kismet2/KismetEditorUtilities.h"
#include "EdGraphSchema_K2.h"
#include "DasRuntime.h"
#include "DasReflectedTypes.h"
#include "DasDefinedEnum.h"
#include "StructUtils/UserDefinedStruct.h"
#include "Kismet2/StructureEditorUtils.h"
#include "UObject/SavePackage.h"
#include "UObject/StrProperty.h"
#include "Misc/Parse.h"
#include "Misc/FileHelper.h"
#include "Misc/PackageName.h"
#include "Misc/Paths.h"
#include "Modules/ModuleManager.h"
#include "HAL/FileManager.h"
int32 RunDasTypeProbe(const FString& Params){
 if(FApp::GetProjectName()!=FString(TEXT("DasForUESmoke")))return 1;FString Id,Phase,E;if(!FParse::Value(*Params,TEXT("ProbeId="),Id) || Id.Len()!=32 || !FParse::Value(*Params,TEXT("Phase="),Phase))return 1;for(TCHAR C:Id)if(!FChar::IsHexDigit(C))return 1;const FString Folder=TEXT("/Game/DasFrameAssetProbe/Types_")+Id;bool Pass=true;auto Check=[&](bool V,const TCHAR* N){Pass &= V;UE_LOG(LogTemp,Display,TEXT("DAS_TYPES_TEST %s %s"),V?TEXT("PASS"):TEXT("FAIL"),N);};TArray<UObject*> Assets;Check(FDasTypeService::Apply(TEXT("dsl_type_declarations.das"),Folder,Assets,E),TEXT("apply_reflected_enum_struct"));if(!E.IsEmpty()){UE_LOG(LogTemp,Error,TEXT("%s"),*E);return 1;}if(Assets.Num()!=2)return 1;auto* Enum=Cast<UDasDefinedEnum>(Assets[0]);auto* Struct=Cast<UUserDefinedStruct>(Assets[1]);Check(Enum && Enum->GetValueByNameString(TEXT("Idle"))==7 && Enum->GetValueByNameString(TEXT("Active"))==42,TEXT("explicit_sparse_enum_values"));if(!Struct)return 1;const auto* Defaults=Struct->GetDefaultInstance();auto* Count=CastField<FIntProperty>(FStructureEditorUtils::GetPropertyByFriendlyName(Struct,TEXT("Count")));auto* Title=CastField<FStrProperty>(FStructureEditorUtils::GetPropertyByFriendlyName(Struct,TEXT("Title")));Check(Struct->Status==UDSS_UpToDate && Count && Count->GetPropertyValue_InContainer(Defaults)==10 && Title && Title->GetPropertyValue_InContainer(Defaults)==TEXT("Привет"),TEXT("compiled_struct_fields_and_owned_unicode_defaults"));const FString BlueprintPackage=Folder+TEXT("/BP_Types");UBlueprint* Blueprint=LoadObject<UBlueprint>(nullptr,*(BlueprintPackage+TEXT(".BP_Types")),nullptr,LOAD_NoWarn);if(!Blueprint){Blueprint=FKismetEditorUtilities::CreateBlueprint(ADasProbeActor::StaticClass(),CreatePackage(*BlueprintPackage),TEXT("BP_Types"),BPTYPE_Normal,UBlueprint::StaticClass(),UBlueprintGeneratedClass::StaticClass(),TEXT("DasTypeProbe"));FEdGraphPinType EnumPin;EnumPin.PinCategory=UEdGraphSchema_K2::PC_Byte;EnumPin.PinSubCategoryObject=Enum;FEdGraphPinType StructPin;StructPin.PinCategory=UEdGraphSchema_K2::PC_Struct;StructPin.PinSubCategoryObject=Struct;Check(FBlueprintEditorUtils::AddMemberVariable(Blueprint,TEXT("Mode"),EnumPin,TEXT("Active")) && FBlueprintEditorUtils::AddMemberVariable(Blueprint,TEXT("Config"),StructPin),TEXT("Blueprint_fields_accept_authored_types"));FKismetEditorUtilities::CompileBlueprint(Blueprint);}Check(Blueprint->Status!=BS_Error && FindFProperty<FByteProperty>(Blueprint->GeneratedClass,TEXT("Mode"))->Enum==Enum && FindFProperty<FByteProperty>(Blueprint->GeneratedClass,TEXT("Mode"))->GetPropertyValue_InContainer(Blueprint->GeneratedClass->GetDefaultObject())==42 && FindFProperty<FStructProperty>(Blueprint->GeneratedClass,TEXT("Config"))->Struct==Struct,TEXT("compiled_and_reopened_Blueprint_refers_to_enum_struct_identity"));if(Phase==TEXT("Create"))Assets.Add(Blueprint);
 TArray<UObject*> Again;Check(FDasTypeService::Apply(TEXT("dsl_type_declarations.das"),Folder,Again,E) && Again.Num()==2 && Again[0]==Assets[0] && Again[1]==Assets[1],TEXT("type_apply_idempotent"));if(!E.IsEmpty())UE_LOG(LogTemp,Display,TEXT("Idempotence: %s"),*E);
 auto& R=FModuleManager::LoadModuleChecked<FDasRuntimeModule>(TEXT("DasRuntime"));FString Source;FFileHelper::LoadFileToString(Source,*R.ScriptPath(TEXT("dsl_type_declarations.das")));const FString Temporary=TEXT("dsl_types_probe_")+Id+TEXT(".das");FFileHelper::SaveStringToFile(Source.Replace(TEXT("Active = 42"),TEXT("Active = 41")),*R.ScriptPath(Temporary),FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM);Check(!FDasTypeService::Apply(Temporary,Folder,Again,E) && Enum->GetValueByNameString(TEXT("Active"))==42,TEXT("changed_type_definition_refused_without_mutation"));IFileManager::Get().Delete(*R.ScriptPath(Temporary));
 if(Phase==TEXT("Create")){for(auto* A:Assets){const FString File=FPackageName::LongPackageNameToFilename(A->GetOutermost()->GetName(),FPackageName::GetAssetPackageExtension());IFileManager::Get().MakeDirectory(*FPaths::GetPath(File),true);FSavePackageArgs Args;Args.TopLevelFlags=RF_Public|RF_Standalone;Args.SaveFlags=SAVE_NoError;Check(UPackage::SavePackage(A->GetOutermost(),A,*File,Args),TEXT("save_authored_type_asset"));}}else if(Phase!=TEXT("Reopen"))return 1;UE_LOG(LogTemp,Display,TEXT("DAS_TYPES_RESULT=%s"),Pass?TEXT("PASS"):TEXT("FAIL"));return Pass?0:1;
}
