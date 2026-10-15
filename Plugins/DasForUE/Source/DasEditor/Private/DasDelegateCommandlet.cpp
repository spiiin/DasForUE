#include "DasDelegateCommandlet.h"
#include "DasDSLService.h"
#include "DasProbeActor.h"
#include "DasRuntime.h"
#include "Engine/Blueprint.h"
#include "Engine/World.h"
#include "UObject/StrongObjectPtr.h"
#include "UObject/UnrealType.h"
#include "UObject/MetaData.h"
#include "Kismet2/KismetEditorUtilities.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "K2Node_FunctionEntry.h"
#include "K2Node_CallFunction.h"
#include "EdGraphSchema_K2.h"
#include "Misc/ScopeExit.h"
#include "Misc/PackageName.h"
#include "Misc/Paths.h"
#include "HAL/FileManager.h"
#include "UObject/SavePackage.h"
#include "UObject/Script.h"
#include "UObject/StructOnScope.h"

namespace {
UBlueprint* MakeDelegateListenerBlueprint(){
 auto* BP=FKismetEditorUtilities::CreateBlueprint(UDasDelegateValidationReceiver::StaticClass(),GetTransientPackage(),MakeUniqueObjectName(GetTransientPackage(),UBlueprint::StaticClass(),TEXT("BP_DelegateListener")),BPTYPE_Normal,UBlueprint::StaticClass(),UBlueprintGeneratedClass::StaticClass(),TEXT("DelegateValidation"));if(!BP)return nullptr;
 auto* G=FBlueprintEditorUtils::CreateNewGraph(BP,TEXT("BlueprintHandler"),UEdGraph::StaticClass(),UEdGraphSchema_K2::StaticClass());FBlueprintEditorUtils::AddFunctionGraph<UClass>(BP,G,true,nullptr);
 TArray<UK2Node_FunctionEntry*> Entries;G->GetNodesOfClass(Entries);if(Entries.Num()!=1)return nullptr;auto* Entry=Entries[0];Entry->AddExtraFlags(FUNC_BlueprintCallable|FUNC_Public);
 FEdGraphPinType Type;Type.PinCategory=UEdGraphSchema_K2::PC_Int;auto* Counter=Entry->CreateUserDefinedPin(TEXT("Counter"),Type,EGPD_Output);if(!Counter)return nullptr;
 auto* Call=NewObject<UK2Node_CallFunction>(G);Call->SetFromFunction(UDasDelegateValidationReceiver::StaticClass()->FindFunctionByName(TEXT("Receive")));G->AddNode(Call,false,false);Call->CreateNewGuid();Call->PostPlacedNewNode();Call->AllocateDefaultPins();const auto* Schema=GetDefault<UEdGraphSchema_K2>();if(!Schema->TryCreateConnection(Entry->FindPinChecked(UEdGraphSchema_K2::PN_Then),Call->GetExecPin()) || !Schema->TryCreateConnection(Counter,Call->FindPinChecked(TEXT("Counter"))))return nullptr;
 FBlueprintEditorUtils::MarkBlueprintAsStructurallyModified(BP);FKismetEditorUtilities::CompileBlueprint(BP);return BP->Status==BS_Error?nullptr:BP;
}
}
UDasDelegateCommandlet::UDasDelegateCommandlet(){IsClient=false;IsServer=false;IsEditor=true;LogToConsole=true;}
int32 UDasDelegateCommandlet::Main(const FString& Params){
 FString Id,Phase;FParse::Value(*Params,TEXT("ProbeId="),Id);FParse::Value(*Params,TEXT("Phase="),Phase);FGuid Guid;if(!FGuid::ParseExact(Id,EGuidFormats::Digits,Guid))return 1;
 FEditorScriptExecutionGuard ScriptExecution;
 const FString Package=TEXT("/Game/DasDelegates/")+Id+TEXT("/BP_DelegateExample");UBlueprint* BP=nullptr;FString Error;
 if(Phase==TEXT("Create")){if(!FDasDSLService::Apply(TEXT("dsl_delegate_actor.das"),Package,TEXT("delegateexample.DelegateExample"),BP,Error)){UE_LOG(LogTemp,Error,TEXT("%s"),*Error);return 1;}}
 else BP=LoadObject<UBlueprint>(nullptr,*(Package+TEXT(".BP_DelegateExample")));
 if(!BP || !BP->GeneratedClass || BP->Status==BS_Error || BP->DelegateSignatureGraphs.Num()!=1)return 1;TStrongObjectPtr<UBlueprint> KeepBP(BP);
 auto* Prop=FindFProperty<FMulticastDelegateProperty>(BP->GeneratedClass,TEXT("OnExampleEvent"));
 if(!Prop || !Prop->HasAllPropertyFlags(CPF_BlueprintAssignable|CPF_BlueprintCallable) || Prop->SignatureFunction->NumParms!=1 || Prop->GetMetaData(TEXT("ToolTip"))!=TEXT("Broadcasts each change to Blueprint and DAS handlers."))return 1;
 UE_LOG(LogTemp,Display,TEXT("PASS authored_event_dispatcher_graph_flags_tooltip_%s"),*Phase);
 if(Phase==TEXT("Create")){const FString File=FPackageName::LongPackageNameToFilename(Package,FPackageName::GetAssetPackageExtension());IFileManager::Get().MakeDirectory(*FPaths::GetPath(File),true);FSavePackageArgs Save;Save.TopLevelFlags=RF_Public|RF_Standalone;if(!UPackage::SavePackage(BP->GetOutermost(),BP,*File,Save))return 1;}

 auto* World=UWorld::CreateWorld(EWorldType::Game,false);if(!World)return 1;
 ON_SCOPE_EXIT{World->DestroyWorld(false);};auto* Actor=World->SpawnActor<ADasProbeActor>(BP->GeneratedClass);if(!Actor)return 1;
 TStrongObjectPtr<UBlueprint> ListenerBP(MakeDelegateListenerBlueprint());if(!ListenerBP)return 1;TStrongObjectPtr<UDasDelegateValidationReceiver> External(NewObject<UDasDelegateValidationReceiver>(GetTransientPackage(),ListenerBP->GeneratedClass));FScriptDelegate Binding;Binding.BindUFunction(External.Get(),TEXT("BlueprintHandler"));Prop->AddDelegate(Binding,Actor,Prop->ContainerPtrToValuePtr<void>(Actor));
 auto Call=[&](const TCHAR* Name,int32 Value=0){if(auto* F=Actor->FindFunction(Name)){if(F->NumParms)Actor->ProcessEvent(F,&Value);else Actor->ProcessEvent(F,nullptr);return true;}return false;};
 if(!Call(TEXT("BindHandlers")) || !Call(TEXT("BindHandlers")) || !Call(TEXT("Emit"),4))return 1;
 auto* Score=FindFProperty<FIntProperty>(Actor->GetClass(),TEXT("Score"));if(!Score || Score->GetPropertyValue_InContainer(Actor)!=12 || External->Count!=4){UE_LOG(LogTemp,Error,TEXT("Expected Score=12 External=4, got %d/%d"),Score?Score->GetPropertyValue_InContainer(Actor):-1,External->Count);UE_LOG(LogTemp,Error,TEXT("DAS error: %s"),*Actor->LastDasError);return 1;}
 if(!Call(TEXT("UnbindSecond")) || !Call(TEXT("Emit"),1) || Score->GetPropertyValue_InContainer(Actor)!=13 || External->Count!=5)return 1;
 const auto* Bound=Prop->GetMulticastDelegate(Prop->ContainerPtrToValuePtr<void>(Actor));if(!Bound || !Bound->Contains(External.Get(),TEXT("BlueprintHandler")))return 1;
 FMulticastScriptDelegate Snapshot=*Bound;struct FArgs{int32 Counter;};FArgs Args{2};Snapshot.ProcessDelegate<UObject>(&Args);if(Score->GetPropertyValue_InContainer(Actor)!=15 || External->Count!=7)return 1;
 auto& Runtime=FModuleManager::LoadModuleChecked<FDasRuntimeModule>(TEXT("DasRuntime"));const auto Ref=Runtime.RunPersistent(Runtime.ScriptPath(TEXT("delegate_reference_probe.das")),5,Actor);if(!Ref.bSucceeded || Ref.Value!=15 || External->Count!=12){UE_LOG(LogTemp,Error,TEXT("Event accessor failed: %s %d/%d"),*Ref.Error,Ref.Value,External->Count);return 1;}UE_LOG(LogTemp,Display,TEXT("PASS authored_event_typed_foreign_accessor_and_Blueprint_handler_%s"),*Phase);
 Runtime.ReleaseObject(Actor);Actor->Destroy();
 UE_LOG(LogTemp,Display,TEXT("PASS authored_event_DAS_and_external_UFunction_multi_handlers_unique_bind_remove_native_broadcast_%s"),*Phase);
 const FString SinglePackage=FPackageName::GetLongPackagePath(Package)+TEXT("/BP_SingleDelegate");UBlueprint* SingleBP=nullptr;
 if(Phase==TEXT("Create")){if(!FDasDSLService::Apply(TEXT("dsl_single_delegate_actor.das"),SinglePackage,TEXT("singledelegate.SingleDelegateActor"),SingleBP,Error)){UE_LOG(LogTemp,Error,TEXT("Single delegate import: %s"),*Error);return 1;}const FString File=FPackageName::LongPackageNameToFilename(SinglePackage,FPackageName::GetAssetPackageExtension());FSavePackageArgs Save;Save.TopLevelFlags=RF_Public|RF_Standalone;if(!UPackage::SavePackage(SingleBP->GetOutermost(),SingleBP,*File,Save))return 1;}else SingleBP=LoadObject<UBlueprint>(nullptr,*(SinglePackage+TEXT(".BP_SingleDelegate")));
 if(!SingleBP || !SingleBP->GeneratedClass)return 1;TStrongObjectPtr<UBlueprint> KeepSingle(SingleBP);auto* SingleActor=World->SpawnActor<ADasProbeActor>(SingleBP->GeneratedClass);auto* Callback=FindFProperty<FDelegateProperty>(SingleActor->GetClass(),TEXT("Callback"));if(!Callback || !Callback->HasAnyPropertyFlags(CPF_BlueprintVisible))return 1;
 auto InvokeSingle=[&](const TCHAR* Name,int32 Value=0){auto* F=SingleActor->FindFunction(Name);struct FArgs{int32 Value,ReturnValue=0;};FArgs A{Value};if(F)SingleActor->ProcessEvent(F,F->NumParms?&A:nullptr);return A.ReturnValue;};InvokeSingle(TEXT("Bind"));if(InvokeSingle(TEXT("Invoke"),3)!=3)return 1;InvokeSingle(TEXT("Clear"));if(Callback->GetPropertyValue_InContainer(SingleActor).IsBound() || InvokeSingle(TEXT("InvokeCopy"),4)!=7)return 1;
 const auto Foreign=Runtime.RunPersistent(Runtime.ScriptPath(TEXT("single_delegate_reference_probe.das")),5,SingleActor);if(!Foreign.bSucceeded || Foreign.Value!=12){UE_LOG(LogTemp,Error,TEXT("Single delegate foreign accessor: %s (%d)"),*Foreign.Error,Foreign.Value);return 1;}auto* Echo=SingleActor->FindFunction(TEXT("EchoCallback"));auto* EchoInput=Echo?FindFProperty<FDelegateProperty>(Echo,TEXT("callback")):nullptr;auto* EchoOutput=Echo?CastField<FDelegateProperty>(Echo->GetReturnProperty()):nullptr;
 if(!EchoInput || !EchoOutput || !EchoInput->SignatureFunction->IsSignatureCompatibleWith(EchoOutput->SignatureFunction))return 1;
 FStructOnScope EchoArgs(Echo);FScriptDelegate NativeBinding;NativeBinding.BindUFunction(SingleActor,TEXT("Handler"));EchoInput->SetPropertyValue_InContainer(EchoArgs.GetStructMemory(),NativeBinding);SingleActor->ProcessEvent(Echo,EchoArgs.GetStructMemory());auto Returned=EchoOutput->GetPropertyValue_InContainer(EchoArgs.GetStructMemory());if(Returned.GetUObject()!=SingleActor || Returned.GetFunctionName()!=TEXT("Handler")){UE_LOG(LogTemp,Error,TEXT("Delegate native roundtrip failed: %s"),*SingleActor->LastDasError);return 1;}
 const auto Methods=Runtime.RunNamedPersistent(Runtime.ScriptPath(TEXT("single_delegate_reference_probe.das")),TEXT("method_probe"),2,SingleActor);if(!Methods.bSucceeded || Methods.Value!=14){UE_LOG(LogTemp,Error,TEXT("Delegate method accessor failed: %s %d"),*Methods.Error,Methods.Value);return 1;}
 auto* Peer=World->SpawnActor<ADasProbeActor>(SingleBP->GeneratedClass);if(!Peer)return 1;
 auto* Execute=Peer->FindFunction(TEXT("ExecuteCallback"));auto* InputDelegate=Execute?FindFProperty<FDelegateProperty>(Execute,TEXT("callback")):nullptr;auto* InputNumber=Execute?FindFProperty<FIntProperty>(Execute,TEXT("value")):nullptr;auto* ReturnNumber=Execute?CastField<FIntProperty>(Execute->GetReturnProperty()):nullptr;if(!InputDelegate || !InputNumber || !ReturnNumber)return 1;
 FStructOnScope CrossArgs(Execute);InputDelegate->SetPropertyValue_InContainer(CrossArgs.GetStructMemory(),Returned);InputNumber->SetPropertyValue_InContainer(CrossArgs.GetStructMemory(),3);Peer->ProcessEvent(Execute,CrossArgs.GetStructMemory());auto* PeerScore=FindFProperty<FIntProperty>(Peer->GetClass(),TEXT("Score"));if(ReturnNumber->GetPropertyValue_InContainer(CrossArgs.GetStructMemory())!=17 || !PeerScore || PeerScore->GetPropertyValue_InContainer(Peer)!=0 || !Peer->LastDasError.IsEmpty()){UE_LOG(LogTemp,Error,TEXT("Cross-context delegate parameter failed: %s"),*Peer->LastDasError);return 1;}Runtime.ReleaseObject(Peer);Peer->Destroy();
 FStructOnScope EmptyArgs(Echo);SingleActor->ProcessEvent(Echo,EmptyArgs.GetStructMemory());if(EchoOutput->GetPropertyValue_InContainer(EmptyArgs.GetStructMemory()).IsBound())return 1;
 UE_LOG(LogTemp,Display,TEXT("PASS single_delegate_method_native_roundtrip_and_DAS_parameter_result_%s"),*Phase);
 int32 RefResult=0;auto* RefFunction=SingleActor->FindFunction(TEXT("ProbeRefOut"));if(!RefFunction)return 1;SingleActor->ProcessEvent(RefFunction,&RefResult);if(RefResult!=30){UE_LOG(LogTemp,Error,TEXT("Authored ref/out delegate: %d %s"),RefResult,*SingleActor->LastDasError);return 1;}UE_LOG(LogTemp,Display,TEXT("PASS delegate_saved_ref_out_signature_and_multicast_ref_%s"),*Phase);
 Runtime.ReleaseObject(SingleActor);SingleActor->Destroy();UE_LOG(LogTemp,Display,TEXT("PASS single_delegate_field_reflection_bind_copy_clear_foreign_accessor_%s"),*Phase);
 int32 ArrayResult=0;auto* ArrayOwner=World->SpawnActor<ADasProbeActor>(SingleBP->GeneratedClass);if(!ArrayOwner)return 1;auto* ArrayFn=ArrayOwner->FindFunction(TEXT("ProbeRefArray"));if(!ArrayFn)return 1;ArrayOwner->ProcessEvent(ArrayFn,&ArrayResult);if(ArrayResult!=1){UE_LOG(LogTemp,Error,TEXT("Delegate array ref/out: %s"),*ArrayOwner->LastDasError);return 1;}Runtime.ReleaseObject(ArrayOwner);ArrayOwner->Destroy();UE_LOG(LogTemp,Display,TEXT("PASS delegate_array_ref_out_%s"),*Phase);
 const FString ChildPackage=FPackageName::GetLongPackagePath(Package)+TEXT("/BP_ChildCounter");UBlueprint* ChildBP=nullptr;
 if(Phase==TEXT("Create")){if(!FDasDSLService::Apply(TEXT("dsl_inheritance_child.das"),ChildPackage,TEXT("inheritance.ChildCounter"),ChildBP,Error)){UE_LOG(LogTemp,Error,TEXT("Inheritance import: %s"),*Error);return 1;}for(auto* SaveBP:{ChildBP,Cast<UBlueprint>(ChildBP->ParentClass->ClassGeneratedBy)}){if(!SaveBP)return 1;const FString File=FPackageName::LongPackageNameToFilename(SaveBP->GetOutermost()->GetName(),FPackageName::GetAssetPackageExtension());FSavePackageArgs Save;Save.TopLevelFlags=RF_Public|RF_Standalone;if(!UPackage::SavePackage(SaveBP->GetOutermost(),SaveBP,*File,Save))return 1;}}
 else ChildBP=LoadObject<UBlueprint>(nullptr,*(ChildPackage+TEXT(".BP_ChildCounter")));
 if(!ChildBP || !ChildBP->GeneratedClass || ChildBP->NewVariables.Num()!=1 || !Cast<UBlueprint>(ChildBP->ParentClass->ClassGeneratedBy))return 1;auto* Child=World->SpawnActor<ADasProbeActor>(ChildBP->GeneratedClass);if(!Child)return 1;auto* Probe=Child->FindFunction(TEXT("Probe"));struct FChildArgs{int32 Input=2,ReturnValue=0;};FChildArgs ChildArgs;if(!Probe)return 1;Child->ProcessEvent(Probe,&ChildArgs);if(ChildArgs.ReturnValue!=27 || Child->AuthoredTypeId!=TEXT("inheritance.ChildCounter")){UE_LOG(LogTemp,Error,TEXT("Inheritance result %d: %s"),ChildArgs.ReturnValue,*Child->LastDasError);return 1;}const auto AsBase=Runtime.RunPersistent(Runtime.ScriptPath(TEXT("inheritance_reference_probe.das")),1,Child);if(!AsBase.bSucceeded || AsBase.Value!=16){UE_LOG(LogTemp,Error,TEXT("Base reference on authored child: %s %d"),*AsBase.Error,AsBase.Value);return 1;}
 Runtime.ReleaseObject(Child);Child->Destroy();UE_LOG(LogTemp,Display,TEXT("PASS authored_inheritance_fields_override_Super_%s"),*Phase);
 const FString LifePackage=FPackageName::GetLongPackagePath(Package)+TEXT("/BP_ChildLifeActor");UBlueprint* LifeBP=nullptr;
 if(Phase==TEXT("Create")){
  if(!FDasDSLService::Apply(TEXT("dsl_life_actor_child.das"),LifePackage,TEXT("lifefamily.ChildLifeActor"),LifeBP,Error)){UE_LOG(LogTemp,Error,TEXT("Lifecycle import: %s"),*Error);return 1;}
  TArray<UBlueprint*> Family{LifeBP,Cast<UBlueprint>(LifeBP->ParentClass->ClassGeneratedBy)};
  auto* BehaviorProperty=FindFProperty<FObjectProperty>(LifeBP->GeneratedClass,TEXT("Behavior"));
  auto* BehaviorClass=BehaviorProperty?BehaviorProperty->PropertyClass.Get():nullptr;
  if(BehaviorClass){Family.Add(Cast<UBlueprint>(BehaviorClass->ClassGeneratedBy));Family.Add(Cast<UBlueprint>(BehaviorClass->GetSuperClass()->ClassGeneratedBy));}
  for(auto* FamilyBP:Family){if(!FamilyBP)return 1;const FString File=FPackageName::LongPackageNameToFilename(FamilyBP->GetOutermost()->GetName(),FPackageName::GetAssetPackageExtension());FSavePackageArgs Save;Save.TopLevelFlags=RF_Public|RF_Standalone;if(!UPackage::SavePackage(FamilyBP->GetOutermost(),FamilyBP,*File,Save))return 1;}
 }else LifeBP=LoadObject<UBlueprint>(nullptr,*(LifePackage+TEXT(".BP_ChildLifeActor")));
 if(!LifeBP || !LifeBP->GeneratedClass)return 1;
 auto* Life=World->SpawnActor<ADasProbeActor>(LifeBP->GeneratedClass);if(!Life)return 1;
 auto IntField=[](UObject* Object,const TCHAR* Name){auto* Field=Object?FindFProperty<FIntProperty>(Object->GetClass(),Name):nullptr;return Field?Field->GetPropertyValue_InContainer(Object):MIN_int32;};
 const auto BeginResult=Runtime.RunLifecycle(Runtime.ScriptPath(TEXT("dsl_life_actor_child.das")),EDasCallback::BeginPlay,Life);
 const auto TickResult=Runtime.RunLifecycle(Runtime.ScriptPath(TEXT("dsl_life_actor_child.das")),EDasCallback::Tick,Life,0.1f);
 const auto BeginAgain=Runtime.RunLifecycle(Runtime.ScriptPath(TEXT("dsl_life_actor_child.das")),EDasCallback::BeginPlay,Life);
 const auto EndResult=Runtime.RunLifecycle(Runtime.ScriptPath(TEXT("dsl_life_actor_child.das")),EDasCallback::EndPlay,Life,0.f,0);
 if(!BeginResult.bSucceeded || !TickResult.bSucceeded || !BeginAgain.bSucceeded || !EndResult.bSucceeded || IntField(Life,TEXT("BaseBegins"))!=1 || IntField(Life,TEXT("ChildBegins"))!=1 || IntField(Life,TEXT("BaseTicks"))!=1 || IntField(Life,TEXT("BaseEnds"))!=1 || IntField(Life,TEXT("ChildEnds"))!=1){UE_LOG(LogTemp,Error,TEXT("Lifecycle chain: %s / %s / %s"),*BeginResult.Error,*TickResult.Error,*EndResult.Error);return 1;}
 Runtime.ReleaseObject(Life);Life->Destroy();
 UE_LOG(LogTemp,Display,TEXT("PASS inherited_lifecycle_saved_chain_once_%s"),*Phase);
 UBlueprint* TickOnlyBP=nullptr;
 if(!FDasDSLService::Apply(TEXT("dsl_life_tick_child.das"),FPackageName::GetLongPackagePath(Package)+TEXT("/BP_TickOnlyChild"),TEXT("lifetick.TickOnlyChild"),TickOnlyBP,Error)){UE_LOG(LogTemp,Error,TEXT("Tick-only import: %s"),*Error);return 1;}
 auto* TickOnly=World->SpawnActor<ADasProbeActor>(TickOnlyBP->GeneratedClass);if(!TickOnly)return 1;
 const auto TickOnlyResult=Runtime.RunLifecycle(Runtime.ScriptPath(TEXT("dsl_life_tick_child.das")),EDasCallback::Tick,TickOnly,0.1f);
 if(!TickOnlyResult.bSucceeded || IntField(TickOnly,TEXT("Ticks"))!=1){UE_LOG(LogTemp,Error,TEXT("Tick-only chain: %s"),*TickOnlyResult.Error);return 1;}
 TickOnly->EmitDasSignal(3);
 if(IntField(TickOnly,TEXT("BaseSignals"))!=3){UE_LOG(LogTemp,Error,TEXT("Inherited event-only initialization failed"));return 1;}
 const auto EventOnlyEnd=Runtime.RunLifecycle(Runtime.ScriptPath(TEXT("dsl_life_tick_child.das")),EDasCallback::EndPlay,TickOnly,0.f,0);
 if(!EventOnlyEnd.bSucceeded || Runtime.SubscriptionCount(TickOnly)!=0)return 1;
 TickOnly->EmitDasSignal(5);if(IntField(TickOnly,TEXT("BaseSignals"))!=3)return 1;
 UE_LOG(LogTemp,Display,TEXT("PASS inherited_event_without_parent_BeginPlay_EndPlay_cleanup_%s"),*Phase);
 Runtime.ReleaseObject(TickOnly);TickOnly->Destroy();
 UE_LOG(LogTemp,Display,TEXT("PASS inherited_tick_without_BeginPlay_or_parent_method_%s"),*Phase);
 UBlueprint* HelperBP=nullptr;const FString HelperPackage=FPackageName::GetLongPackagePath(Package)+TEXT("/BP_GrandCounter");
 if(Phase==TEXT("Create")){if(!FDasDSLService::Apply(TEXT("dsl_inheritance_grandchild.das"),HelperPackage,TEXT("inheritance.GrandCounter"),HelperBP,Error)){UE_LOG(LogTemp,Error,TEXT("Helper inheritance import: %s"),*Error);return 1;}const FString File=FPackageName::LongPackageNameToFilename(HelperPackage,FPackageName::GetAssetPackageExtension());FSavePackageArgs Save;Save.TopLevelFlags=RF_Public|RF_Standalone;if(!UPackage::SavePackage(HelperBP->GetOutermost(),HelperBP,*File,Save))return 1;}
 else HelperBP=LoadObject<UBlueprint>(nullptr,*(HelperPackage+TEXT(".BP_GrandCounter")));
 if(!HelperBP || !HelperBP->GeneratedClass)return 1;auto* HelperActor=World->SpawnActor<ADasProbeActor>(HelperBP->GeneratedClass);if(!HelperActor)return 1;
 // A fresh editor-world instance executes construction after SCS, and reruns use edited values.
 TStrongObjectPtr<UWorld> ConstructionWorld(UWorld::CreateWorld(EWorldType::Editor,false));
 auto* Constructed=ConstructionWorld->SpawnActor<ADasProbeActor>(HelperBP->GeneratedClass);
 if(!Constructed || !Constructed->LastDasError.IsEmpty() || IntField(Constructed,TEXT("ConstructionValue"))!=23 || IntField(Constructed,TEXT("ConstructionTrace"))!=12)return 1;
 struct FReadScore{int32 Result=0;} ReadScore;auto* Reader=HelperActor->FindFunction(TEXT("ReadScore"));if(!Reader)return 1;HelperActor->ProcessEvent(Reader,&ReadScore);if(ReadScore.Result!=10)return 1;
 const int32 ContextsBefore=Runtime.PersistentContextCount();if(ContextsBefore<1)return 1;
 auto* ScoreProperty=FindFProperty<FIntProperty>(Constructed->GetClass(),TEXT("Score"));ScoreProperty->SetPropertyValue_InContainer(Constructed,20);
 Constructed->RerunConstructionScripts();
 if(!Constructed->LastDasError.IsEmpty() || IntField(Constructed,TEXT("ConstructionValue"))!=43 || IntField(Constructed,TEXT("ConstructionTrace"))!=12 || Runtime.PersistentContextCount()!=ContextsBefore || Runtime.SubscriptionCount(Constructed)!=0)return 1;
 if(IntField(HelperBP->GeneratedClass->GetDefaultObject(),TEXT("ConstructionValue"))!=0)return 1;
 UBlueprint* FailureBP=nullptr;
 if(!FDasDSLService::Apply(TEXT("dsl_construction_failure.das"),FPackageName::GetLongPackagePath(Package)+TEXT("/BP_ConstructionFailure"),TEXT("construction.ConstructionFailure"),FailureBP,Error)){UE_LOG(LogTemp,Error,TEXT("Construction failure fixture import: %s"),*Error);return 1;}
 auto* FailedConstruction=ConstructionWorld->SpawnActor<ADasProbeActor>(FailureBP->GeneratedClass);
 if(!FailedConstruction || !FailedConstruction->LastDasError.Contains(TEXT("managed callbacks are forbidden during ConstructionScript")) || IntField(FailedConstruction,TEXT("ConstructionTrace"))!=1)return 1;
 FailedConstruction->OnConstruction(FTransform::Identity);
 if(IntField(FailedConstruction,TEXT("ConstructionTrace"))!=2 || Runtime.SubscriptionCount(FailedConstruction)!=0 || Runtime.PersistentContextCount()!=ContextsBefore)return 1;
 FailedConstruction->Destroy();
 Constructed->Destroy();ConstructionWorld->DestroyWorld(false);
 UE_LOG(LogTemp,Display,TEXT("PASS editor_construction_repeat_Super_instance_edits_no_CDO_no_VM_leak_%s"),*Phase);
 if(IntField(HelperActor,TEXT("ConstructionValue"))!=23 || IntField(HelperActor,TEXT("ConstructionTrace"))!=12 || !HelperActor->LastDasError.IsEmpty())return 1;
 struct FHelperArgs{int32 Input=2,Result=0;};FHelperArgs HA;auto* HelperFunction=HelperActor->FindFunction(TEXT("LocalProbe"));if(!HelperFunction)return 1;HelperActor->ProcessEvent(HelperFunction,&HA);
 if(HA.Result!=21 || IntField(HelperActor,TEXT("Score"))!=12 || HelperActor->FindFunction(TEXT("Offset")) || HelperActor->FindFunction(TEXT("Template"))){UE_LOG(LogTemp,Error,TEXT("Helpers: result=%d score=%d error=%s"),HA.Result,IntField(HelperActor,TEXT("Score")),*HelperActor->LastDasError);return 1;}
 Runtime.ReleaseObject(HelperActor);HelperActor->Destroy();UE_LOG(LogTemp,Display,TEXT("PASS three_level_same_context_helpers_override_Super_%s"),*Phase);
 UE_LOG(LogTemp,Display,TEXT("DAS_AUTHORED_DELEGATES_RESULT=PASS"));return 0;
}
