#include "DasInterfaceService.h"
#include "DasRuntime.h"
#include "DasAuthoring.h"
#include "DasContainerTypes.h"
#include "DasAuthoredInterfaceClass.h"
#include "Engine/Blueprint.h"
#include "UObject/Interface.h"
#include "UObject/MetaData.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "Kismet2/KismetEditorUtilities.h"
#include "K2Node_FunctionEntry.h"
#include "K2Node_FunctionResult.h"
#include "EdGraphSchema_K2.h"
#include "Misc/PackageName.h"
#include "Misc/Paths.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "Modules/ModuleManager.h"
#include "KismetCompiler.h"
#include "Kismet2/KismetReinstanceUtilities.h"
namespace {
void ApplyInterfaceMethodMetadata(UBlueprint* BP,const FDasDeclaredClass& Definition){
 for(const auto& M:Definition.Methods){for(UEdGraph* G:BP->FunctionGraphs)if(G->GetFName()==FName(*M.Name)){TArray<UK2Node_FunctionEntry*> Entries;G->GetNodesOfClass(Entries);for(auto* Entry:Entries){for(const auto* Key:{TEXT("Category"),TEXT("DisplayName"),TEXT("ToolTip")})Entry->MetaData.RemoveMetaData(Key);for(const auto& Pair:M.Metadata)Entry->MetaData.SetMetaData(*Pair.Key,FString(Pair.Value));}}
  if(auto* Method=BP->GeneratedClass->FindFunctionByName(*M.Name)){for(const auto* Key:{TEXT("Category"),TEXT("DisplayName"),TEXT("ToolTip")})Method->RemoveMetaData(Key);for(const auto& Pair:M.Metadata)Method->SetMetaData(*Pair.Key,*Pair.Value);}
 }
 for(const auto* Key:{TEXT("Category"),TEXT("DisplayName"),TEXT("ToolTip")})BP->GeneratedClass->RemoveMetaData(Key);for(const auto& Pair:Definition.Metadata)BP->GeneratedClass->SetMetaData(*Pair.Key,*Pair.Value);
}
FEdGraphPinType AuthoredInterfacePin(const FDasDeclaredType& T){FEdGraphPinType P;if(auto* Carrier=DasContainerCarrier(T)){GetDefault<UEdGraphSchema_K2>()->ConvertPropertyToPinType(Carrier->FindPropertyByName(TEXT("Values")),P);return P;}if(T.Kind==TEXT("object")){P.PinSubCategoryObject=FindObject<UClass>(nullptr,*T.TypePath);P.PinCategory=CastChecked<UClass>(P.PinSubCategoryObject.Get())->HasAnyClassFlags(CLASS_Interface)?UEdGraphSchema_K2::PC_Interface:UEdGraphSchema_K2::PC_Object;return P;}if(T.Kind==TEXT("enum")){P.PinCategory=UEdGraphSchema_K2::PC_Byte;P.PinSubCategoryObject=FindObject<UEnum>(nullptr,*T.TypePath);return P;}if(T.Kind==TEXT("struct")){P.PinCategory=UEdGraphSchema_K2::PC_Struct;P.PinSubCategoryObject=FindObject<UScriptStruct>(nullptr,*T.TypePath);return P;}P.PinCategory=T.Kind==TEXT("int")?UEdGraphSchema_K2::PC_Int:T.Kind==TEXT("bool")?UEdGraphSchema_K2::PC_Boolean:T.Kind==TEXT("string")?UEdGraphSchema_K2::PC_String:T.Kind==TEXT("name")?UEdGraphSchema_K2::PC_Name:UEdGraphSchema_K2::PC_Real;if(T.Kind==TEXT("float") || T.Kind==TEXT("double"))P.PinSubCategory=T.Kind==TEXT("float")?UEdGraphSchema_K2::PC_Float:UEdGraphSchema_K2::PC_Double;return P;}
}
class FDasAuthoredInterfaceCompiler : public FKismetCompilerContext {
public:
 FDasAuthoredInterfaceCompiler(UBlueprint* BP,FCompilerResultsLog& Log,const FKismetCompilerOptions& Options):FKismetCompilerContext(BP,Log,Options){}
 virtual void SpawnNewClass(const FString& Name)override {NewClass=FindObject<UDasAuthoredInterfaceClass>(Blueprint->GetOutermost(),*Name);if(!NewClass)NewClass=NewObject<UDasAuthoredInterfaceClass>(Blueprint->GetOutermost(),*Name,RF_Public|RF_Transactional);else{NewClass->ClassGeneratedBy=Blueprint;FBlueprintCompileReinstancer::Create(NewClass);}}
};
void FDasInterfaceService::RegisterCompiler(){FKismetCompilerContext::RegisterCompilerForBP(UDasAuthoredInterfaceBlueprint::StaticClass(),[](UBlueprint* BP,FCompilerResultsLog& Log,const FKismetCompilerOptions& Options){return MakeShared<FDasAuthoredInterfaceCompiler>(BP,Log,Options);});}
bool FDasInterfaceService::Apply(const FString& Script,const FString& Folder,TArray<UBlueprint*>& Out,FString& Error){
 Out.Reset();Error.Reset();if(!IsInGameThread() || Script!=FPaths::GetCleanFilename(Script) || FPaths::GetExtension(Script)!=TEXT("das") || !Folder.StartsWith(TEXT("/Game/")) || !FPackageName::IsValidLongPackageName(Folder)){Error=TEXT("Expected local interface source and /Game folder");return false;}
 auto& R=FModuleManager::LoadModuleChecked<FDasRuntimeModule>(TEXT("DasRuntime"));FDasAuthoringModule Model;FString Wire;if(!R.ReadAuthoredInterfaces(R.ScriptPath(Script),Model,Error) || !Model.ToJson(Wire,Error))return false;
 struct FTarget {const FDasDeclaredClass* Definition;FString Package;UBlueprint* Existing;};TArray<FTarget> Targets;
 for(const auto& C:Model.Classes){const FString Package=Folder+TEXT("/")+C.Name;auto* Object=LoadObject<UObject>(nullptr,*(Package+TEXT(".")+C.Name),nullptr,LOAD_NoWarn);auto* BP=Cast<UBlueprint>(Object);if((Object && !BP) || (!Object && FPackageName::DoesPackageExist(Package))){Error=TEXT("Authored interface package conflict");return false;}if(BP){auto* Class=Cast<UDasAuthoredInterfaceClass>(BP->GeneratedClass);if(BP->BlueprintType!=BPTYPE_Interface || !Class || Class->StableId!=C.Id){Error=TEXT("Changed interface definition requires a later structural policy");return false;}if(Class->ContractJson!=Wire){FDasAuthoringModule Previous;if(!FDasAuthoringModule::FromJson(Class->ContractJson,Previous,Error) || Previous.Id!=Model.Id){Error=TEXT("Interface stored contract identity is invalid");return false;}const auto* Old=Previous.Classes.FindByPredicate([&](const auto& Item){return Item.Id==C.Id;});if(!Old || FDasAuthoringModule::Compare(*Old,C).bStructural){Error=TEXT("Changed interface definition requires a later structural policy");return false;}}
 for(const auto& M:C.Methods){UEdGraph* Graph=nullptr;for(UEdGraph* Candidate:BP->FunctionGraphs)if(Candidate->GetFName()==FName(*M.Name))Graph=Candidate;TArray<UK2Node_FunctionEntry*> Entries;if(Graph)Graph->GetNodesOfClass(Entries);if(Entries.Num()!=1 || !Class->FindFunctionByName(*M.Name)){Error=TEXT("Existing interface signature graph is missing");return false;}}}Targets.Add({&C,Package,BP});}
 TArray<UBlueprint*> Created;auto Fail=[&](const TCHAR* Why){for(auto* BP:Created){BP->ClearFlags(RF_Public|RF_Standalone);BP->SetFlags(RF_Transient);const FName RejectedName=MakeUniqueObjectName(GetTransientPackage(),BP->GetClass(),TEXT("RejectedDasInterface"));BP->Rename(*RejectedName.ToString(),GetTransientPackage(),REN_DontCreateRedirectors|REN_DoNotDirty);}Out.Reset();Error=Why;return false;};
 for(const auto& Target:Targets){if(Target.Existing){auto* Existing=CastChecked<UDasAuthoredInterfaceBlueprint>(Target.Existing);auto* Class=CastChecked<UDasAuthoredInterfaceClass>(Existing->GeneratedClass);if(Class->ContractJson!=Wire || Existing->StableId!=Target.Definition->Id){ApplyInterfaceMethodMetadata(Existing,*Target.Definition);Class->ContractJson=Wire;Existing->StableId=Target.Definition->Id;Existing->GetOutermost()->GetMetaData().SetValue(Existing,TEXT("DasInterfaceSource"),*Script);Existing->MarkPackageDirty();}Out.Add(Target.Existing);continue;}const auto& C=*Target.Definition;auto* BP=FKismetEditorUtilities::CreateBlueprint(UInterface::StaticClass(),CreatePackage(*Target.Package),*C.Name,BPTYPE_Interface,UDasAuthoredInterfaceBlueprint::StaticClass(),UDasAuthoredInterfaceClass::StaticClass());if(!BP)return Fail(TEXT("Authored interface creation failed"));Created.Add(BP);
  for(const auto& M:C.Methods){auto* G=FBlueprintEditorUtils::CreateNewGraph(BP,*M.Name,UEdGraph::StaticClass(),UEdGraphSchema_K2::StaticClass());FBlueprintEditorUtils::AddFunctionGraph<UClass>(BP,G,false,nullptr);TArray<UK2Node_FunctionEntry*> Entries;TArray<UK2Node_FunctionResult*> Results;G->GetNodesOfClass(Entries);G->GetNodesOfClass(Results);if(Entries.Num()!=1)return Fail(TEXT("Interface entry missing"));auto* Entry=Entries[0];Entry->AddExtraFlags(FUNC_BlueprintCallable|FUNC_BlueprintEvent|FUNC_Event|FUNC_Public);if(M.bPure)Entry->AddExtraFlags(FUNC_BlueprintPure);if(M.bConst)Entry->AddExtraFlags(FUNC_Const);Entry->MetaData.SetMetaData(TEXT("DasFunctionConst"),FString(M.bConst?TEXT("true"):TEXT("false")));Entry->MetaData.SetMetaData(TEXT("DasFunctionKind"),FString(TEXT("Event")));Entry->MetaData.SetMetaData(TEXT("DasFunctionPure"),FString(M.bPure?TEXT("true"):TEXT("false")));for(const auto& Pair:M.Metadata)Entry->MetaData.SetMetaData(*Pair.Key,FString(Pair.Value));UK2Node_FunctionResult* Result=Results.Num()?Results[0]:nullptr;if(!Result){FGraphNodeCreator<UK2Node_FunctionResult> Creator(*G);Result=Creator.CreateNode();Creator.Finalize();}G->GetSchema()->TryCreateConnection(Entry->FindPinChecked(UEdGraphSchema_K2::PN_Then),Result->FindPinChecked(UEdGraphSchema_K2::PN_Execute));
   for(const auto& P:M.Parameters){auto Pin=AuthoredInterfacePin(P.Type);if(P.Direction==TEXT("out"))Result->CreateUserDefinedPin(*P.Name,Pin,EGPD_Input);else{Pin.bIsReference=P.Direction==TEXT("inout");Pin.bIsConst=P.Direction==TEXT("in") && P.Type.Kind==TEXT("array");Entry->CreateUserDefinedPin(*P.Name,Pin,EGPD_Output);}}
   if(M.ReturnType.Kind!=TEXT("void"))Result->CreateUserDefinedPin(UEdGraphSchema_K2::PN_ReturnValue,AuthoredInterfacePin(M.ReturnType),EGPD_Input);
   BP->GetOutermost()->GetMetaData().SetValue(G,TEXT("DasMemberId"),*M.Id);
  }
  FKismetEditorUtilities::CompileBlueprint(BP);if(BP->Status==BS_Error)return Fail(TEXT("Authored interface compilation failed"));auto* Class=Cast<UDasAuthoredInterfaceClass>(BP->GeneratedClass);if(!Class || !Class->HasAnyClassFlags(CLASS_Interface))return Fail(TEXT("Generated authored interface class mismatch"));Class->StableId=C.Id;Class->ContractJson=Wire;ApplyInterfaceMethodMetadata(BP,C);CastChecked<UDasAuthoredInterfaceBlueprint>(BP)->StableId=C.Id;auto& Meta=BP->GetOutermost()->GetMetaData();Meta.SetValue(BP,TEXT("DasTypeId"),*C.Id);Meta.SetValue(BP,TEXT("DasInterfaceSource"),*Script);BP->MarkPackageDirty();Out.Add(BP);
 }
 for(auto* BP:Created)FAssetRegistryModule::AssetCreated(BP);return true;
}
