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
 UE_LOG(LogTemp,Display,TEXT("DAS_AUTHORED_DELEGATES_RESULT=PASS"));return 0;
}
