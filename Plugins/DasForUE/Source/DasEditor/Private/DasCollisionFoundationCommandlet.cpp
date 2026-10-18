#include "DasCollisionFoundationCommandlet.h"
#include "DasWeakCollisionFixture.h"
#include "DasRuntime.h"
#include "DasSchema.h"
#include "DasPropertyTypes.h"
#include "DasPropertyValues.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "Components/BoxComponent.h"
#include "UObject/StrongObjectPtr.h"
#include "UObject/UnrealType.h"
#include "UObject/GarbageCollection.h"
#include "Misc/ScopeExit.h"
UDasCollisionFoundationCommandlet::UDasCollisionFoundationCommandlet(){IsClient=false;IsServer=false;IsEditor=true;LogToConsole=true;}
int32 UDasCollisionFoundationCommandlet::Main(const FString& Params){
 int32 Failures=0;auto Check=[&](bool Good,const TCHAR* Name){UE_LOG(LogTemp,Display,TEXT("DAS_COLLISION_%s %s"),Good?TEXT("PASS"):TEXT("FAIL"),Name);if(!Good)++Failures;return Good;};
 TStrongObjectPtr<UWorld> World(UWorld::CreateWorld(EWorldType::Game,false));World->InitializeActorsForPlay(FURL());
 auto* Owner=World->SpawnActor<AActor>();auto* Fixture=NewObject<UDasWeakCollisionFixture>(Owner);Owner->AddInstanceComponent(Fixture);Fixture->RegisterComponent();
 auto& Runtime=FModuleManager::LoadModuleChecked<FDasRuntimeModule>(TEXT("DasRuntime"));
 ON_SCOPE_EXIT{Runtime.ReleaseObject(Fixture);World->DestroyWorld(false);};
 FString Error;auto* Property=FindFProperty<FWeakObjectProperty>(Fixture->GetClass(),TEXT("WeakTarget"));
 FDasDeclaredType Type;Check(DescribeDasProperty(Property,Type,Error) && Type.Kind==TEXT("object") && Type.WeakReference && Type.TypePath==TEXT("/Script/CoreUObject.Object"),TEXT("weak_property_descriptor"));
 FDasDeclaredType Strong=Type;Strong.WeakReference=false;
 TStrongObjectPtr<UClass> Carrier(NewObject<UClass>());auto* Rebuilt=CreateDasProperty(Carrier.Get(),TEXT("RebuiltWeak"),Type,EDasPropertyUsage::Value,Error);
 if(Rebuilt)Carrier->AddCppProperty(Rebuilt);
 Check(CastField<FWeakObjectProperty>(Rebuilt) && Rebuilt->SameType(Property) && Type.StorageIdentity()!=Strong.StorageIdentity() && Type.Identity()==Strong.Identity(),TEXT("weak_factory_and_physical_storage_identity"));
 auto* Value=NewObject<UDasWeakCollisionFixture>();TWeakObjectPtr<UObject> Weak=Value;
 Check(DasReferenceSet(Property,Property->ContainerPtrToValuePtr<void>(Fixture),Value) && DasReferenceGet(Property,Property->ContainerPtrToValuePtr<void>(Fixture))==Value,TEXT("weak_read_write"));
 CollectGarbage(RF_NoFlags);Value=nullptr;FDasScalarValue Read;
 Check(!Weak.IsValid() && ReadDasPropertyValue(Property,Property->ContainerPtrToValuePtr<void>(Fixture),Read,Error) && !Read.ObjectValue,TEXT("weak_GC_becomes_null"));
 auto* Array=FindFProperty<FArrayProperty>(Fixture->GetClass(),TEXT("WeakArray"));Check(!DescribeDasProperty(Array,Type,Error),TEXT("weak_container_cannot_become_strong_carrier"));
 TStrongObjectPtr<UClass> TempClass(NewObject<UClass>());auto* WrongProperty=new FWeakObjectProperty(TempClass.Get(),TEXT("ActorOnly"));WrongProperty->PropertyClass=AActor::StaticClass();TempClass->AddCppProperty(WrongProperty);
 TWeakObjectPtr<UObject> Storage;auto* Wrong=NewObject<UDasWeakCollisionFixture>();Check(!DasReferenceSet(WrongProperty,&Storage,Wrong) && Storage.IsExplicitlyNull(),TEXT("weak_wrong_class_rejected"));
 auto* Target=World->SpawnActor<AActor>();auto* Box=NewObject<UBoxComponent>(Target);Target->SetRootComponent(Box);Target->AddInstanceComponent(Box);Box->SetBoxExtent(FVector(50));Box->SetCollisionEnabled(ECollisionEnabled::QueryOnly);Box->SetCollisionObjectType(ECC_WorldStatic);Box->SetCollisionResponseToAllChannels(ECR_Block);Box->RegisterComponent();Target->SetActorLocation(FVector(300,0,0));
 const auto Run=Runtime.RunFile(Runtime.ScriptPath(TEXT("collision_foundation_probe.das")),0,false,Fixture);
 if(!Run.bSucceeded)UE_LOG(LogTemp,Error,TEXT("%s"),*Run.Error);
 Check(Run.bSucceeded && Run.Value==42,TEXT("DAS_generated_weak_frame_GC_trace_fields_and_overlap"));
 UE_LOG(LogTemp,Display,TEXT("DAS_COLLISION_RESULT=%s"),Failures?TEXT("FAIL"):TEXT("PASS"));return Failures?1:0;
}
