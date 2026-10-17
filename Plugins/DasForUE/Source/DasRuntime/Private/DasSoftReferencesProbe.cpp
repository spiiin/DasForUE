#include "DasSoftReferences.h"
#include "DasPropertyTypes.h"
#include "DasPropertyValues.h"
#include "DasContainerTypes.h"
#include "DasContainerRegistry.h"
#include "DasSchema.h"
#include "Engine/DataAsset.h"
#include "Curves/CurveFloat.h"
#include "GameFramework/Actor.h"
#include "UObject/Package.h"
#include "UObject/GarbageCollection.h"
bool CheckDasSoftReferences(FString& Error){
 auto Fail=[&](const TCHAR* Message){Error=Message;return false;};
 auto* S=DasSoftCarrier(UCurveFloat::StaticClass(),false,Error);if(!S)return false;
 auto* P=DasSoftProperty(S->GetPathName());FDasDeclaredType T;if(!DescribeDasProperty(P,T,Error))return false;
 FString Fingerprint;if(!DasPropertyLayoutFingerprint(T,Fingerprint))return Fail(TEXT("Soft type fingerprint failed"));
 const FSoftObjectPath Path(TEXT("/Game/NotLoaded/Asset.Asset"));FSoftObjectPtr Source(Path);FDasScalarValue Value;
 if(!ReadDasPropertyValue(P,&Source,Value,Error))return false;
 FSoftObjectPtr Destination;if(!WriteDasPropertyValue(P,&Destination,Value,Error) || Destination.ToSoftObjectPath()!=Path || Path.ResolveObject())return Fail(TEXT("Unloaded soft roundtrip loaded or lost the asset"));
 FInstancedStruct Defaults;if(!ParseDasStructDefault(S->GetPathName(),TEXT("{\"Values\":\"/Game/NotLoaded/Asset.Asset\"}"),Defaults,Error))return false;
 if(P->GetPropertyValue(P->ContainerPtrToValuePtr<void>(Defaults.GetMemory())).ToSoftObjectPath()!=Path)return Fail(TEXT("Soft default lost asset path"));
 if(ValidateDasSoftPath(TEXT("/Game/Asset.Asset:Subobject"),Error) || ValidateDasSoftPath(TEXT("bad path"),Error))return Fail(TEXT("Invalid soft path accepted"));
 auto* ClassCarrier=DasSoftCarrier(AActor::StaticClass(),true,Error);if(!ClassCarrier)return false;auto* Class=DasSoftProperty(ClassCarrier->GetPathName());FSoftObjectPtr Wrong{FSoftObjectPath(UDataAsset::StaticClass())};
 if(ValidateDasSoftValue(Class,&Wrong,Error))return Fail(TEXT("Wrong soft class base accepted"));
 // Common factory must create arrays/map VALUES of actual soft properties.
 for(bool ClassReference:{false,true}){
  auto* LeafCarrier=ClassReference?ClassCarrier:S;FDasDeclaredType Leaf;Leaf.Kind=TEXT("struct");Leaf.TypePath=LeafCarrier->GetPathName();
  FDasDeclaredType ArrayType;ArrayType.Kind=TEXT("array");ArrayType.Element=MakeShared<FDasDeclaredType>(Leaf);auto* ArrayCarrier=DasContainerCarrier(ArrayType);auto* Array=ArrayCarrier?FindFProperty<FArrayProperty>(ArrayCarrier,TEXT("Values")):nullptr;
  if(!Array || !CastField<FSoftObjectProperty>(Array->Inner) || bool(CastField<FSoftClassProperty>(Array->Inner))!=ClassReference)return Fail(TEXT("Soft array became wrapper-struct storage"));
  FInstancedStruct ArrayValue;if(!ParseDasStructDefault(ArrayCarrier->GetPathName(),TEXT("[\"/Game/NotLoaded/Asset.Asset\",\"\"]"),ArrayValue,Error))return false;
  FScriptArrayHelper Elements(Array,Array->ContainerPtrToValuePtr<void>(ArrayValue.GetMemory()));if(Elements.Num()!=2 || CastFieldChecked<FSoftObjectProperty>(Array->Inner)->GetPropertyValue(Elements.GetRawPtr(0)).ToSoftObjectPath()!=Path)return Fail(TEXT("Soft array default changed path"));
  for(const TCHAR* Kind:{TEXT("int"),TEXT("string"),TEXT("name"),TEXT("byte"),TEXT("object")}){FDasDeclaredType MapType;MapType.Kind=TEXT("map");auto Key=MakeShared<FDasDeclaredType>();Key->Kind=Kind;if(Key->Kind==TEXT("object"))Key->TypePath=UObject::StaticClass()->GetPathName();MapType.Key=Key;MapType.Value=MakeShared<FDasDeclaredType>(Leaf);auto* MapCarrier=DasContainerCarrier(MapType);auto* Map=MapCarrier?FindFProperty<FMapProperty>(MapCarrier,TEXT("Values")):nullptr;
   if(!Map || !CastField<FSoftObjectProperty>(Map->ValueProp) || bool(CastField<FSoftClassProperty>(Map->ValueProp))!=ClassReference)return Fail(TEXT("Soft map value factory failed"));FDasDeclaredType Described;if(!DescribeDasProperty(Map,Described,Error) || Described.StorageIdentity()!=MapType.StorageIdentity())return Fail(TEXT("Soft map descriptor roundtrip failed"));
  }
  FDasDeclaredType SetType;SetType.Kind=TEXT("set");SetType.Element=MakeShared<FDasDeclaredType>(Leaf);if(ValidateDasPropertyType(SetType,EDasPropertyUsage::Container,Error))return Fail(TEXT("Soft set hashing was admitted without a policy"));
 }
 FDasPropertyValueRoots Roots;TWeakObjectPtr<UCurveFloat> Weak;
 const FString PackageName=TEXT("/Game/DasSoftReferences/GC_")+FGuid::NewGuid().ToString(EGuidFormats::Digits);
 auto* Asset=NewObject<UCurveFloat>(CreatePackage(*PackageName),TEXT("SoftOnly"),RF_Public);Weak=Asset;FSoftObjectPtr Live(Asset);
 FDasScalarValue Captured;if(!ReadDasPropertyValue(P,&Live,Captured,Error))return false;const auto Original=Live.ToSoftObjectPath();Roots.Keep(MoveTemp(Captured));
 Asset=nullptr;CollectGarbage(RF_NoFlags);
 if(Weak.IsValid())return Fail(TEXT("Soft snapshot retained asset strongly"));
 FSoftObjectPtr After;if(!WriteDasPropertyValue(P,&After,Roots.Values[0],Error) || After.ToSoftObjectPath()!=Original)return Fail(TEXT("GC lost soft path"));
 Error.Reset();return true;
}

#include "Engine/StaticMeshActor.h"
bool CheckDasSoftUpcasts(FString& Error){
 auto Fail=[&](const TCHAR* Why){Error=Why;return false;};
 auto Make=[&](UClass* Base,bool Class,const FSoftObjectPath& Path){FInstancedStruct V;auto* S=DasSoftCarrier(Base,Class,Error);if(S){V.InitializeAs(S);auto* P=DasSoftProperty(S->GetPathName());P->SetPropertyValue(P->ContainerPtrToValuePtr<void>(V.GetMutableMemory()),FSoftObjectPtr(Path));}return V;};
 auto PathOf=[](const FInstancedStruct& V){auto* P=DasSoftProperty(V.GetScriptStruct()->GetPathName());return P->GetPropertyValue(P->ContainerPtrToValuePtr<void>(V.GetMemory())).ToSoftObjectPath();};
 for(bool Class:{false,true}){
  const FSoftObjectPath Path(Class?TEXT("/Game/NotLoaded/Enemy.Enemy_C"):TEXT("/Game/NotLoaded/Curve.Curve"));
  auto Source=Make(Class?AStaticMeshActor::StaticClass():UCurveFloat::StaticClass(),Class,Path);
  auto* Target=DasSoftCarrier(Class?AActor::StaticClass():UObject::StaticClass(),Class,Error);if(!Source.IsValid() || !Target)return false;
  FInstancedStruct Out;if(!UpcastDasSoftValue(Source,Target->GetPathName(),Out,Error) || PathOf(Out)!=Path || PathOf(Source)!=Path || Path.ResolveObject())return Fail(TEXT("Soft parent conversion lost or loaded path"));
  auto Parent=Make(Class?AActor::StaticClass():UObject::StaticClass(),Class,FSoftObjectPath());const auto SavedType=Out.GetScriptStruct();const auto SavedPath=PathOf(Out);
  if(UpcastDasSoftValue(Parent,Source.GetScriptStruct()->GetPathName(),Out,Error) || Out.GetScriptStruct()!=SavedType || PathOf(Out)!=SavedPath)return Fail(TEXT("Null downcast accepted or failure modified output"));
  auto* Other=DasSoftCarrier(UDataAsset::StaticClass(),Class,Error);if(!Other || UpcastDasSoftValue(Source,Other->GetPathName(),Out,Error))return Fail(TEXT("Unrelated soft conversion accepted"));
  auto* Opposite=DasSoftCarrier(UObject::StaticClass(),!Class,Error);if(!Opposite || UpcastDasSoftValue(Source,Opposite->GetPathName(),Out,Error))return Fail(TEXT("Soft object/class conversion accepted"));
  if(UpcastDasSoftValue(Source,TEXT("/Script/DasRuntime.UnknownSoftType"),Out,Error))return Fail(TEXT("Unknown soft constraint accepted"));
  if(!UpcastDasSoftValue(Source,Source.GetScriptStruct()->GetPathName(),Out,Error) || PathOf(Out)!=Path)return Fail(TEXT("Soft identity conversion failed"));
  auto Empty=Make(Class?AStaticMeshActor::StaticClass():UCurveFloat::StaticClass(),Class,FSoftObjectPath());if(!UpcastDasSoftValue(Empty,Target->GetPathName(),Out,Error) || !PathOf(Out).IsNull())return Fail(TEXT("Soft null widening failed"));
 }
 // Resolving a loaded source retains its weak target and path under the parent constraint.
 auto* Asset=NewObject<UCurveFloat>(CreatePackage(*(TEXT("/Game/DasSoftReferences/Upcast_")+FGuid::NewGuid().ToString(EGuidFormats::Digits))),TEXT("Curve"),RF_Public);
 auto Loaded=Make(UCurveFloat::StaticClass(),false,FSoftObjectPath(Asset));auto* Root=DasSoftCarrier(UObject::StaticClass(),false,Error);FInstancedStruct Out;
 if(!Root || !UpcastDasSoftValue(Loaded,Root->GetPathName(),Out,Error) || PathOf(Out).ResolveObject()!=Asset)return Fail(TEXT("Loaded soft upcast lost its target"));
 Error.Reset();return true;
}
