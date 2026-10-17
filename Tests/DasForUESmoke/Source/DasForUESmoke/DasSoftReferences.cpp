#include "DasNativeHashProbeComponent.h"
#include "CoreMinimal.h"
#include "Containers/Ticker.h"
#include "DasRuntime.h"
#include "DasSchema.h"
#include "DasProbeActor.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "UObject/StructOnScope.h"
#include "UObject/UnrealType.h"
namespace {
int32 SoftCount(UObject* O,const TCHAR* N){auto* P=O?FindFProperty<FIntProperty>(O->GetClass(),N):nullptr;return P?P->GetPropertyValue_InContainer(O):MIN_int32;}
struct FSoftCooked {
 TWeakObjectPtr<UWorld> World;TWeakObjectPtr<ADasProbeActor> Owner;double Started=FPlatformTime::Seconds();bool Good=true;
 void Check(bool OK,const TCHAR* N){Good &= OK;UE_LOG(LogTemp,Display,TEXT("DAS_SOFT_REFERENCES_PACKAGED %s %s"),OK?TEXT("PASS"):TEXT("FAIL"),N);}
 bool Finish(){auto& R=FModuleManager::LoadModuleChecked<FDasRuntimeModule>(TEXT("DasRuntime"));if(auto* A=Owner.Get()){R.ReleaseObject(A);A->Destroy();}Check(R.PersistentContextCount(World.Get())==0,TEXT("owner_cleanup"));UE_LOG(LogTemp,Display,TEXT("DAS_SOFT_REFERENCES_PACKAGED_RESULT=%s"),Good?TEXT("PASS"):TEXT("FAIL"));FPlatformMisc::RequestExitWithStatus(false,Good?0:1);return false;}
 void CheckNativeCollections(ADasProbeActor* A){
  const FSoftObjectPath AssetPath(TEXT("/Game/NotLoaded/Asset.Asset")),ClassPath(TEXT("/Game/NotLoaded/Blueprint.Blueprint_C"));
  auto* Change=A->FindFunction(TEXT("ChangeAssets"));Check(Change!=nullptr,TEXT("array_native_method_exists"));
  if(Change){FStructOnScope Memory(Change);auto* Input=FindFProperty<FArrayProperty>(Change,TEXT("value"));auto* Previous=FindFProperty<FArrayProperty>(Change,TEXT("previous"));auto* Result=CastField<FArrayProperty>(Change->GetReturnProperty());
   Check(Input && Previous && Result && CastField<FSoftObjectProperty>(Input->Inner) && CastField<FSoftObjectProperty>(Result->Inner),TEXT("native_array_has_soft_element_pins"));
   if(Input && Previous && Result){FScriptArrayHelper Values(Input,Input->ContainerPtrToValuePtr<void>(Memory.GetStructMemory()));Values.AddValues(2);CastFieldChecked<FSoftObjectProperty>(Input->Inner)->SetPropertyValue(Values.GetRawPtr(0),FSoftObjectPtr(AssetPath));A->ProcessEvent(Change,Memory.GetStructMemory());FScriptArrayHelper Before(Previous,Previous->ContainerPtrToValuePtr<void>(Memory.GetStructMemory())),Returned(Result,Result->ContainerPtrToValuePtr<void>(Memory.GetStructMemory()));
    Check(Values.Num()==0 && Before.Num()==2 && Returned.Num()==2 && CastFieldChecked<FSoftObjectProperty>(Previous->Inner)->GetPropertyValue(Before.GetRawPtr(0)).ToSoftObjectPath()==AssetPath && CastFieldChecked<FSoftObjectProperty>(Result->Inner)->GetPropertyValue(Returned.GetRawPtr(1)).IsNull(),TEXT("native_Blueprint_array_return_ref_out"));
   }
  }
  auto* Echo=A->FindFunction(TEXT("EchoClasses"));Check(Echo!=nullptr,TEXT("map_native_method_exists"));
  if(Echo){FStructOnScope Memory(Echo);auto* Input=FindFProperty<FMapProperty>(Echo,TEXT("value"));auto* Result=CastField<FMapProperty>(Echo->GetReturnProperty());Check(Input && Result && CastField<FStrProperty>(Input->KeyProp) && CastField<FSoftClassProperty>(Input->ValueProp),TEXT("native_map_has_soft_class_value_pins"));
   if(Input && Result){FScriptMapHelper Values(Input,Input->ContainerPtrToValuePtr<void>(Memory.GetStructMemory()));int32 Index=Values.AddDefaultValue_Invalid_NeedsRehash();CastFieldChecked<FStrProperty>(Input->KeyProp)->SetPropertyValue(Values.GetKeyPtr(Index),TEXT("saved"));CastFieldChecked<FSoftClassProperty>(Input->ValueProp)->SetPropertyValue(Values.GetValuePtr(Index),FSoftObjectPtr(ClassPath));Values.Rehash();A->ProcessEvent(Echo,Memory.GetStructMemory());FScriptMapHelper Returned(Result,Result->ContainerPtrToValuePtr<void>(Memory.GetStructMemory()));bool Same=Returned.Num()==1;for(int32 I=0;I<Returned.GetMaxIndex();++I)if(Returned.IsValidIndex(I))Same &= CastFieldChecked<FStrProperty>(Result->KeyProp)->GetPropertyValue(Returned.GetKeyPtr(I))==TEXT("saved") && CastFieldChecked<FSoftClassProperty>(Result->ValueProp)->GetPropertyValue(Returned.GetValuePtr(I)).ToSoftObjectPath()==ClassPath;
    Check(Same && !ClassPath.ResolveObject(),TEXT("native_Blueprint_soft_class_map_return_without_load"));
   }
  }
  auto* Record=A->FindFunction(TEXT("EchoRecord"));Check(Record!=nullptr,TEXT("record_native_method_exists"));
  if(Record){FStructOnScope Memory(Record);auto* Input=FindFProperty<FStructProperty>(Record,TEXT("value"));auto* Result=CastField<FStructProperty>(Record->GetReturnProperty());Check(Input && Result && Input->Struct==Result->Struct,TEXT("native_record_method_constraints"));
   if(Input && Result){auto* Asset=CastField<FSoftObjectProperty>(FindDasStructField(Input->Struct,TEXT("Asset")));auto* Class=CastField<FSoftClassProperty>(FindDasStructField(Input->Struct,TEXT("ActorType")));Check(Asset && Class,TEXT("authored_record_has_native_soft_members"));if(Asset && Class){void* Data=Input->ContainerPtrToValuePtr<void>(Memory.GetStructMemory());Asset->SetPropertyValue_InContainer(Data,FSoftObjectPtr(AssetPath));Class->SetPropertyValue_InContainer(Data,FSoftObjectPtr(ClassPath));A->ProcessEvent(Record,Memory.GetStructMemory());const void* Out=Result->ContainerPtrToValuePtr<void>(Memory.GetStructMemory());Check(Asset->GetPropertyValue_InContainer(Out).ToSoftObjectPath()==AssetPath && Class->GetPropertyValue_InContainer(Out).ToSoftObjectPath()==ClassPath,TEXT("native_Blueprint_authored_soft_record_return_without_load"));}}
  }
 }
 bool Tick(float){auto* A=Owner.Get();auto* W=World.Get();if(!A || !W || !W->HasBegunPlay() || SoftCount(A,TEXT("Ready"))<4){if(FPlatformTime::Seconds()-Started<30 && (!A || A->LastDasError.IsEmpty()))return true;Check(false,TEXT("async_callbacks_completed"));if(A)UE_LOG(LogTemp,Error,TEXT("%s"),*A->LastDasError);return Finish();}
  Check(A->LastDasError.IsEmpty() && SoftCount(A,TEXT("Errors"))==0,TEXT("native_and_cooked_lazy_asset_loaded_only_on_request"));Check(SoftCount(A,TEXT("Ready"))==4 && SoftCount(A,TEXT("Rejected"))==2,TEXT("object_class_null_async_and_rejected_paths"));
  Check(SoftCount(A,TEXT("CollectionChecks"))==1,TEXT("soft_arrays_maps_authored_record_and_snapshot_isolation"));
  Check(SoftCount(A,TEXT("NativeChecks"))==1,TEXT("generated_native_soft_fields_methods_containers_ref_out"));
  auto* Native=A->FindComponentByClass<UDasNativeHashProbeComponent>();
  Check(Native && Native->SoftAsset.IsNull() && Native->SoftActorType.IsNull() && Native->SoftAssets.Num()==2 && Native->SoftAssets[0].IsNull() && Native->SoftAssets[1].ToSoftObjectPath().ToString()==TEXT("/Game/NotLoaded/Asset.Asset") && Native->SoftClasses.Num()==1 && Native->SoftClasses.FindRef(TEXT("enemy")).IsNull() && Native->SoftRecord.Asset.ToSoftObjectPath().ToString()==TEXT("/Game/NotLoaded/Asset.Asset") && Native->SoftRecord.ActorType.ToSoftObjectPath().ToString()==TEXT("/Game/NotLoaded/Blueprint.Blueprint_C") && Native->SoftRecord.Items.Num()==1 && Native->SoftRecord.Items[0].IsNull() && Native->SoftRecord.Classes.Num()==1 && Native->SoftRecord.Classes.FindRef(TEXT("enemy")).IsNull(),TEXT("generated_soft_writes_confirmed_in_native_Cpp_storage"));
  CheckNativeCollections(A);
  auto* P=FindFProperty<FSoftObjectProperty>(A->GetClass(),TEXT("Lazy"));auto* C=FindFProperty<FSoftClassProperty>(A->GetClass(),TEXT("ActorType"));Check(P && C && C->MetaClass==AActor::StaticClass(),TEXT("cooked_native_soft_fields"));
  auto* F=A->FindFunction(TEXT("Change"));if(F){FStructOnScope M(F);auto* Input=FindFProperty<FSoftObjectProperty>(F,TEXT("value"));auto* Previous=FindFProperty<FSoftObjectProperty>(F,TEXT("previous"));auto* Result=CastField<FSoftObjectProperty>(F->GetReturnProperty());Check(Input && Previous && Result,TEXT("native_soft_return_ref_out"));if(Input && Previous && Result){const FSoftObjectPath Path(TEXT("/Game/NotLoaded/Asset.Asset"));Input->SetPropertyValue_InContainer(M.GetStructMemory(),FSoftObjectPtr(Path));A->ProcessEvent(F,M.GetStructMemory());Check(Input->GetPropertyValue_InContainer(M.GetStructMemory()).IsNull() && Previous->GetPropertyValue_InContainer(M.GetStructMemory()).ToSoftObjectPath()==Path && Result->GetPropertyValue_InContainer(M.GetStructMemory()).ToSoftObjectPath()==Path,TEXT("unloaded_path_roundtrip"));}}else Check(false,TEXT("Change_exists"));
  auto* Echo=A->FindFunction(TEXT("EchoClass"));Check(Echo!=nullptr,TEXT("soft_class_method_exists"));if(Echo){FStructOnScope M(Echo);auto* Input=FindFProperty<FSoftClassProperty>(Echo,TEXT("value"));auto* Result=CastField<FSoftClassProperty>(Echo->GetReturnProperty());Check(Input && Result && Input->MetaClass==AActor::StaticClass() && Result->MetaClass==AActor::StaticClass(),TEXT("soft_class_method_constraints"));if(Input && Result){const FSoftObjectPath Path(TEXT("/Game/NotLoaded/Blueprint.Blueprint_C"));Input->SetPropertyValue_InContainer(M.GetStructMemory(),FSoftObjectPtr(Path));A->ProcessEvent(Echo,M.GetStructMemory());Check(Result->GetPropertyValue_InContainer(M.GetStructMemory()).ToSoftObjectPath()==Path && !Path.ResolveObject(),TEXT("unloaded_soft_class_return"));}}
  auto& Runtime=FModuleManager::LoadModuleChecked<FDasRuntimeModule>(TEXT("DasRuntime"));auto* Queue=A->FindFunction(TEXT("QueueForRelease"));Check(Queue!=nullptr,TEXT("queue_release_probe"));if(Queue){A->ProcessEvent(Queue,nullptr);Check(SoftCount(A,TEXT("Errors"))==0 && Runtime.SubscriptionCount(A)>0,TEXT("asset_request_pending_before_EndPlay"));}A->Destroy();Check(Runtime.SubscriptionCount(A)==0 && Runtime.PersistentContextCount(W)==0,TEXT("EndPlay_revokes_pending_completion"));return Finish();
 }
};
}
void StartDasSoftReferencesValidation(){auto S=MakeShared<FSoftCooked>();for(const auto& C:GEngine->GetWorldContexts())if(auto* W=C.World())if(W->IsGameWorld()){S->World=W;for(TActorIterator<ADasProbeActor> I(W);I;++I)if(I->GetFName()==TEXT("SoftReferences"))S->Owner=*I;}FTSTicker::GetCoreTicker().AddTicker(FTickerDelegate::CreateLambda([S](float D){return S->Tick(D);}));}
