#include "DasCookedCallsCommandlet.h"
#include "DasDSLService.h"
#include "DasAuthoring.h"
#include "DasProbeActor.h"
#include "DasClassMethodsProbe.h"
#include "DasInitializerProbe.h"
#include "DasScriptComponent.h"
#include "DasRuntime.h"
#include "Modules/ModuleManager.h"
#include "Engine/Blueprint.h"
#include "Engine/BlueprintGeneratedClass.h"
#include "Engine/World.h"
#include "Kismet2/KismetEditorUtilities.h"
#include "UObject/SavePackage.h"
#include "UObject/Script.h"
#include "UObject/UnrealType.h"
#include "Misc/PackageName.h"
#include "Misc/Paths.h"
#include "Misc/Parse.h"
#include "HAL/FileManager.h"
#include "Misc/FileHelper.h"
#include "Misc/ScopeExit.h"
UDasCookedCallsCommandlet::UDasCookedCallsCommandlet(){IsClient=false;IsServer=false;IsEditor=true;LogToConsole=true;}
int32 UDasCookedCallsCommandlet::Main(const FString& Params){
 FEditorScriptExecutionGuard ScriptExecution; // Native probe intentionally executes Actor Blueprint graphs in an editor world.
 FString Id;FParse::Value(*Params,TEXT("ProbeId="),Id);FGuid Guid;if(!FGuid::ParseExact(Id,EGuidFormats::Digits,Guid))return 1;
 const FString Root=TEXT("/Game/DasCookedCalls/")+Id;FString Error;UBlueprint *Target=nullptr,*Driver=nullptr,*Library=nullptr;
 FString Phase;FParse::Value(*Params,TEXT("Phase="),Phase);
 if(Phase==TEXT("Reopen")){
  auto* BP=LoadObject<UBlueprint>(nullptr,*(Root+TEXT("/BP_Target.BP_Target")));if(!BP)return 1;
  auto* World=UWorld::CreateWorld(EWorldType::Editor,false);auto* Actor=World->SpawnActor<ADasProbeActor>(BP->GeneratedClass);FString Report;bool Good=RunDasClassMethodsProbe(Actor,Report);auto* Life=Actor?FindFProperty<FObjectProperty>(Actor->GetClass(),TEXT("Life")):nullptr;Good &= RunDasClassMethodsProbe(Life?Life->GetObjectPropertyValue_InContainer(Actor):nullptr,Report);UE_LOG(LogTemp,Display,TEXT("%s"),*Report);World->DestroyWorld(false);if(Good)UE_LOG(LogTemp,Display,TEXT("DAS_CLASS_METHODS_REOPEN=PASS"));return Good?0:1;
 }

 auto& Runtime=FModuleManager::LoadModuleChecked<FDasRuntimeModule>(TEXT("DasRuntime"));FString Original;if(!FFileHelper::LoadFileToString(Original,*Runtime.ScriptPath(TEXT("dsl_spawn_target.das"))))return 1;
 Original.ReplaceInline(TEXT("\r\n"),TEXT("\n"));
 const FString BadFile=Runtime.ScriptPath(TEXT("dsl_registered_class_negative_")+Id+TEXT(".das"));
 for(const auto& Pair:TArray<TPair<FString,FString>>{
  {TEXT("def EchoClass(value:ActorClass):ActorClass { self.SelectedClass=value; return self.SelectedClass }"),TEXT("def EchoClass(value:ObjectClass):ActorClass { self.SelectedClass=value; return self.SelectedClass }")},
  {TEXT("def EchoClass(value:ActorClass):ActorClass { self.SelectedClass=value; return self.SelectedClass }"),TEXT("def EchoClass(value:UEClassHandle):ActorClass { self.SelectedClass=value; return self.SelectedClass }")},
  {TEXT("def EchoClass(value:ActorClass):ActorClass { self.SelectedClass=value; return self.SelectedClass }"),TEXT("def EchoClass(value:ActorClass):ActorClass { return ue_class_loaded(type<ObjectRef>,\"/Script/Engine.Actor\") }")},
  {TEXT("[ue_function(BlueprintCallable)]\n    def EchoClass"),TEXT("[ue_function(BlueprintCallable,arg_value_class=\"/Script/CoreUObject.Object\")]\n    def EchoClass")}
 }){const FString Bad=Original.Replace(*Pair.Key,*Pair.Value);FDasAuthoringModule Model;FString Failure;if(Bad==Original || !FFileHelper::SaveStringToFile(Bad,*BadFile,FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM)){IFileManager::Get().Delete(*BadFile);return 1;}const bool Accepted=Runtime.ReadAuthoringScript(BadFile,Model,Failure);IFileManager::Get().Delete(*BadFile);if(Accepted || Failure.IsEmpty()){UE_LOG(LogTemp,Error,TEXT("Typed class negative source unexpectedly accepted"));return 1;}}
 for(const auto* BadType:{TEXT("int"),TEXT("ActorClass"),TEXT("UEClassHandle")}){const FString Bad=FString::Printf(TEXT("options gen2\nrequire ue_object_types\ntypedef BadClass = $UEClass<%s>\n"),BadType);FDasAuthoringModule Model;FString Failure;if(!FFileHelper::SaveStringToFile(Bad,*BadFile,FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM))return 1;const bool Accepted=Runtime.ReadAuthoringScript(BadFile,Model,Failure);IFileManager::Get().Delete(*BadFile);if(Accepted || !Failure.Contains(TEXT("UEClass"))){UE_LOG(LogTemp,Error,TEXT("Invalid UEClass<%s> accepted=%d error=%s"),BadType,Accepted,*Failure);return 1;}}
 UE_LOG(LogTemp,Display,TEXT("DAS_TYPED_CLASS_NEGATIVES=PASS"));
 if(!FDasDSLService::Apply(TEXT("dsl_spawn_target.das"),Root+TEXT("/BP_Target"),TEXT("spawntarget.SpawnTarget"),Target,Error) || !FDasDSLService::Apply(TEXT("dsl_spawn_driver.das"),Root+TEXT("/BP_Driver"),TEXT("spawndriver.SpawnDriver"),Driver,Error) || !FDasDSLService::Apply(TEXT("dsl_function_libraries.das"),Root+TEXT("/BP_Library"),TEXT("libraries.LibraryProbe"),Library,Error)){UE_LOG(LogTemp,Error,TEXT("Cooked call fixtures: %s"),*Error);return 1;}
 UBlueprint *Inherited=nullptr,*Skipped=nullptr,*Failure=nullptr;if(!FDasDSLService::Apply(TEXT("dsl_initialize_inherited.das"),Root+TEXT("/BP_Inherited"),TEXT("initinherited.InheritedInitializer"),Inherited,Error) || !FDasDSLService::Apply(TEXT("dsl_initialize_skipped.das"),Root+TEXT("/BP_Skipped"),TEXT("initskipped.SkippedInitializer"),Skipped,Error) || !FDasDSLService::Apply(TEXT("dsl_initialize_failure.das"),Root+TEXT("/BP_Failure"),TEXT("initfailure.FailingInitializer"),Failure,Error)){UE_LOG(LogTemp,Error,TEXT("Initializer fixtures: %s"),*Error);return 1;}
 FString InitReport;if(!RunDasInitializerLifetimeProbe(Target->GeneratedClass,Inherited->GeneratedClass,Skipped->GeneratedClass,Failure->GeneratedClass,InitReport)){UE_LOG(LogTemp,Error,TEXT("%s"),*InitReport);return 1;}UE_LOG(LogTemp,Display,TEXT("%s"),*InitReport);
 // A body-only reload must replace the VM without replaying successful Initialize.
 {TStrongObjectPtr<UWorld> ReloadWorld(UWorld::CreateWorld(EWorldType::Game,false));auto* A=ReloadWorld->SpawnActor<ADasProbeActor>(Target->GeneratedClass);auto* Life=FindFProperty<FObjectProperty>(A->GetClass(),TEXT("Life"));auto* C=Life?Life->GetObjectPropertyValue_InContainer(A):nullptr;auto& R=FModuleManager::LoadModuleChecked<FDasRuntimeModule>(TEXT("DasRuntime"));R.RunLifecycle(R.ScriptPath(TEXT("dsl_dynamic_lifecycle.das")),EDasCallback::Initialize,C);const auto Before=R.RunLifecycle(R.ScriptPath(TEXT("dsl_spawn_target.das")),EDasCallback::Initialize,A);FString Source;const FString File=R.ScriptPath(TEXT("dsl_spawn_target.das"));if(!FFileHelper::LoadFileToString(Source,*File))return 1;ON_SCOPE_EXIT{FFileHelper::SaveStringToFile(Source,*File,FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM);R.ReleaseObject(A);R.ReleaseObject(C);ReloadWorld->DestroyWorld(false);};const FString Changed=Source.Replace(TEXT("self.InitCount+=1; self.InitComponentsReady="),TEXT("self.InitCount+=100; self.InitComponentsReady="));if(Changed==Source || !FFileHelper::SaveStringToFile(Changed,*File,FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM))return 1;const auto Reloaded=R.RunLifecycle(File,EDasCallback::Initialize,A);auto* Count=FindFProperty<FIntProperty>(A->GetClass(),TEXT("InitCount"));if(!Before.bSucceeded || !Reloaded.bSucceeded || Reloaded.Revision<=Before.Revision || Count->GetPropertyValue_InContainer(A)!=3){UE_LOG(LogTemp,Error,TEXT("Initializer body reload failed: %s / %s"),*Before.Error,*Reloaded.Error);return 1;}UE_LOG(LogTemp,Display,TEXT("DAS_INITIALIZER_BODY_RELOAD=PASS"));}
 auto* Child=FKismetEditorUtilities::CreateBlueprint(Target->GeneratedClass,CreatePackage(*(Root+TEXT("/BP_Child"))),TEXT("BP_Child"),BPTYPE_Normal,UBlueprint::StaticClass(),UBlueprintGeneratedClass::StaticClass());if(!Child)return 1;FKismetEditorUtilities::CompileBlueprint(Child);
 auto* ClassField=FindFProperty<FClassProperty>(Driver->GeneratedClass,TEXT("SpawnClass"));if(!ClassField)return 1;
 ClassField->SetObjectPropertyValue_InContainer(Driver->GeneratedClass->GetDefaultObject(),Child->GeneratedClass);FindFProperty<FIntProperty>(Child->GeneratedClass,TEXT("EditableValue"))->SetPropertyValue_InContainer(Child->GeneratedClass->GetDefaultObject(),41);
 auto Save=[](UObject* O){const FString File=FPackageName::LongPackageNameToFilename(O->GetOutermost()->GetName(),O->IsA<UWorld>()?FPackageName::GetMapPackageExtension():FPackageName::GetAssetPackageExtension());IFileManager::Get().MakeDirectory(*FPaths::GetPath(File),true);FSavePackageArgs Args;Args.TopLevelFlags=RF_Public|RF_Standalone;return UPackage::SavePackage(O->GetOutermost(),O,*File,Args);};
 for(const auto* Name:{TEXT("BP_Inherited"),TEXT("BP_Skipped"),TEXT("BP_Failure"),TEXT("BP_DynamicLife"),TEXT("BP_Target"),TEXT("BP_Child"),TEXT("BP_Driver"),TEXT("BP_Library")}){auto* BP=FindObject<UBlueprint>(nullptr,*(Root+TEXT("/")+Name+TEXT(".")+Name));if(!BP || !Save(BP))return 1;}
 auto* World=UWorld::CreateWorld(EWorldType::Editor,false,TEXT("Map"),CreatePackage(*(Root+TEXT("/Map"))));World->SetFlags(RF_Public|RF_Standalone);World->GetOutermost()->SetPackageFlags(PKG_ContainsMap);
 auto Spawn=[&](UBlueprint* BP,const TCHAR* Name){FActorSpawnParameters P;P.Name=Name;auto* Actor=World->SpawnActor<ADasProbeActor>(BP->GeneratedClass,FTransform::Identity,P);if(Actor)Actor->Tags.Add(Name);return Actor;};
 auto* Probe=Spawn(Target,TEXT("ClassMethodProbe"));FString Report;bool Methods=RunDasClassMethodsProbe(Probe,Report);if(Probe){auto* Life=FindFProperty<FObjectProperty>(Probe->GetClass(),TEXT("Life"));Methods &= RunDasClassMethodsProbe(Life?Life->GetObjectPropertyValue_InContainer(Probe):nullptr,Report);FModuleManager::LoadModuleChecked<FDasRuntimeModule>(TEXT("DasRuntime")).ReleaseObject(Probe);Probe->Destroy();}UE_LOG(LogTemp,Display,TEXT("%s"),*Report);
 auto* Placed=Spawn(Inherited,TEXT("InitializerInherited"));if(!Placed)return 1;FindFProperty<FIntProperty>(Placed->GetClass(),TEXT("EditableValue"))->SetPropertyValue_InContainer(Placed,49);if(!Spawn(Skipped,TEXT("InitializerSkipped")) || !Spawn(Failure,TEXT("InitializerFailure")))return 1;
 const bool Good=Methods && Spawn(Driver,TEXT("CookedCallDriver")) && Spawn(Library,TEXT("CookedLibrary")) && Save(World);World->DestroyWorld(false);
 if(!Good)return 1;UE_LOG(LogTemp,Display,TEXT("DAS_COOKED_CALLS_EXPORT=PASS map=%s/Map"),*Root);return 0;
}
