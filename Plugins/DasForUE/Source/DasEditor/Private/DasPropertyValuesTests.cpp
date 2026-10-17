#include "DasPropertyValues.h"
#include "DasPropertyTypes.h"
#include "DasContainerTypes.h"
#include "DasSchema.h"
#include "DasDefaultValueProbe.h"
#include "DasProbeInterface.h"
#include "Components/SceneComponent.h"
#include "Engine/EngineTypes.h"
#include "UObject/Package.h"
#include "UObject/StrongObjectPtr.h"
#include "UObject/StructOnScope.h"
#include "UObject/UnrealType.h"
#include "UObject/StrProperty.h"
#include "UObject/TextProperty.h"
#include "Misc/AutomationTest.h"
#include <limits>

#if WITH_DEV_AUTOMATION_TESTS
namespace {
void SeedPropertyValue(FProperty* P,void* Data,int32 Seed,UObject* First,UObject* Second){
 if(auto* Class=CastField<FClassProperty>(P))Class->SetObjectPropertyValue(Data,USceneComponent::StaticClass());
 else if(DasReferencePropertyClass(P))DasReferenceSet(P,Data,DasReferencePropertyClass(P)->HasAnyClassFlags(CLASS_Interface)?nullptr:Seed%2?First:Second);
 else if(auto* Enum=DasEnumPropertyType(P))SetDasEnumPropertyNumber(P,Data,Enum->GetValueByIndex(Seed%2));
 else if(auto* Byte=CastField<FByteProperty>(P))Byte->SetPropertyValue(Data,Seed%2?255:0);
 else if(auto* Text=CastField<FTextProperty>(P))Text->SetPropertyValue(Data,FText::ChangeKey(TEXT("PropertyValues"),TEXT("Localized"),FText::FromString(TEXT("Owned 世界"))));
 else if(auto* Int=CastField<FIntProperty>(P))Int->SetPropertyValue(Data,Seed*17);
 else if(auto* Float=CastField<FFloatProperty>(P))Float->SetPropertyValue(Data,Seed*1.25f);
 else if(auto* Double=CastField<FDoubleProperty>(P))Double->SetPropertyValue(Data,Seed*1.23456789012345);
 else if(auto* Bool=CastField<FBoolProperty>(P))Bool->SetPropertyValue(Data,Seed%2!=0);
 else if(auto* String=CastField<FStrProperty>(P))String->SetPropertyValue(Data,FString::Printf(TEXT("Owned 世界 %d"),Seed));
 else if(auto* Name=CastField<FNameProperty>(P))Name->SetPropertyValue(Data,FName(*FString::Printf(TEXT("Value_%d"),Seed)));
 else if(auto* Struct=CastField<FStructProperty>(P)){for(TFieldIterator<FProperty> It(Struct->Struct);It;++It)SeedPropertyValue(*It,It->ContainerPtrToValuePtr<void>(Data),Seed,First,Second);}
 else if(auto* Array=CastField<FArrayProperty>(P)){FScriptArrayHelper Items(Array,Data);Items.Resize(2);for(int32 I=0;I<2;++I)SeedPropertyValue(Array->Inner,Items.GetRawPtr(I),Seed+I,First,Second);}
 else if(auto* Set=CastField<FSetProperty>(P)){FScriptSetHelper Items(Set,Data);for(int32 I=0;I<2;++I){const int32 At=Items.AddDefaultValue_Invalid_NeedsRehash();SeedPropertyValue(Set->ElementProp,Items.GetElementPtr(At),Seed+I,First,Second);}Items.Rehash();}
 else if(auto* Map=CastField<FMapProperty>(P)){FScriptMapHelper Items(Map,Data);for(int32 I=0;I<2;++I){const int32 At=Items.AddDefaultValue_Invalid_NeedsRehash();SeedPropertyValue(Map->KeyProp,Items.GetKeyPtr(At),Seed+I,First,Second);SeedPropertyValue(Map->ValueProp,Items.GetValuePtr(At),Seed+I,First,Second);}Items.Rehash();}
}
}
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FDasPropertyValuesTest,"DasForUE.Types.PropertyValues",EAutomationTestFlags::EditorContext|EAutomationTestFlags::EngineFilter)
bool FDasPropertyValuesTest::RunTest(const FString&){
 auto Leaf=[](const TCHAR* Kind,const FString& Path=FString()){FDasDeclaredType T;T.Kind=Kind;T.TypePath=Path;return T;};
 auto Shape=[](const TCHAR* Kind,const FDasDeclaredType& A,const FDasDeclaredType& B=FDasDeclaredType()){FDasDeclaredType T;T.Kind=Kind;if(T.GetKind()==EDasTypeKind::Map){T.Key=MakeShared<FDasDeclaredType>(A);T.Value=MakeShared<FDasDeclaredType>(B);}else T.Element=MakeShared<FDasDeclaredType>(A);return T;};
 TArray<FDasDeclaredType> Leaves;for(const TCHAR* K:{TEXT("byte"),TEXT("text"),TEXT("int"),TEXT("float"),TEXT("double"),TEXT("bool"),TEXT("string"),TEXT("name")})Leaves.Add(Leaf(K));
 auto Enum=Leaf(TEXT("enum"),StaticEnum<ESpawnActorCollisionHandlingMethod>()->GetPathName());Enum.EnumStorage=EDasEnumStorage::Byte;auto StrongEnum=Enum;StrongEnum.EnumStorage=EDasEnumStorage::Enum8;
 auto Object=Leaf(TEXT("object"),UObject::StaticClass()->GetPathName());
 Leaves.Append({Enum,StrongEnum,Object,Leaf(TEXT("object"),UDasProbeInterface::StaticClass()->GetPathName()),Leaf(TEXT("struct"),TBaseStructure<FVector>::Get()->GetPathName()),Leaf(TEXT("struct"),FDasDefaultNestedProbe::StaticStruct()->GetPathName())});
 TArray<FDasDeclaredType> Types=Leaves;Types.Add(Leaf(TEXT("class"),UObject::StaticClass()->GetPathName()));
 for(const auto& T:Leaves){Types.Add(Shape(TEXT("array"),T));for(const auto& K:{Leaf(TEXT("int")),Leaf(TEXT("string")),Leaf(TEXT("name")),Object,Enum})Types.Add(Shape(TEXT("map"),K,T));if(T.GetKind()==EDasTypeKind::Int || T.GetKind()==EDasTypeKind::Byte || T.GetKind()==EDasTypeKind::String || T.GetKind()==EDasTypeKind::Name || T.GetKind()==EDasTypeKind::Enum || (T.GetKind()==EDasTypeKind::Object && T.TypePath==Object.TypePath))Types.Add(Shape(TEXT("set"),T));}
 TStrongObjectPtr<UObject> First(NewObject<USceneComponent>()),Second(NewObject<USceneComponent>());int32 Count=0;FString Error;
 for(const auto& Type:Types){
  TStrongObjectPtr<UScriptStruct> Owner(NewObject<UScriptStruct>(GetTransientPackage()));auto* P=CreateDasProperty(Owner.Get(),TEXT("Value"),Type,EDasPropertyUsage::Value,Error);
  if(!P){AddError(Error);return false;}Owner->AddCppProperty(P);Owner->Bind();Owner->StaticLink(true);
  FStructOnScope Source(Owner.Get()),Dest(Owner.Get()),Expected(Owner.Get());void* A=P->ContainerPtrToValuePtr<void>(Source.GetStructMemory());void* B=P->ContainerPtrToValuePtr<void>(Dest.GetStructMemory());void* E=P->ContainerPtrToValuePtr<void>(Expected.GetStructMemory());
  SeedPropertyValue(P,A,3,First.Get(),Second.Get());P->CopyCompleteValue(E,A);
  FDasScalarValue Snapshot;if(!ReadDasPropertyValue(P,A,Snapshot,Error)){AddError(Type.StorageIdentity()+TEXT(": ")+Error);return false;}
  if(Snapshot.Kind==int32(EDasMethodSignature::Struct) && (!Snapshot.StructValue.IsValid() || Snapshot.ObjectClass!=Snapshot.StructValue.GetScriptStruct()->GetPathName())){AddError(TEXT("Snapshot transport lost concrete type identity"));return false;}
  if(Type.GetKind()==EDasTypeKind::Class && (Snapshot.ClassValue!=USceneComponent::StaticClass() || Snapshot.ObjectValue)){AddError(TEXT("Class transport must use ClassValue"));return false;}
  P->ClearValue(A);if(!WriteDasPropertyValue(P,B,Snapshot,Error) || !P->Identical(B,E,PPF_DeepComparison)){AddError(TEXT("Owned read/write mismatch: ")+Type.StorageIdentity()+TEXT(" ")+Error);return false;}
  FDasScalarValue Wrong=Snapshot;Wrong.Kind=int32(EDasMethodSignature::Void);
  if(WriteDasPropertyValue(P,B,Wrong,Error) || !P->Identical(B,E,PPF_DeepComparison)){AddError(TEXT("Failed write changed destination"));return false;}
  FDasScalarValue Sentinel;Sentinel.Kind=99;Sentinel.StringValue=TEXT("unchanged");
  if(ReadDasPropertyValue(P,nullptr,Sentinel,Error) || Sentinel.Kind!=99 || Sentinel.StringValue!=TEXT("unchanged")){AddError(TEXT("Failed read changed output"));return false;}
  if(Type.GetKind()==EDasTypeKind::Enum || Type.GetKind()==EDasTypeKind::Object || Type.GetKind()==EDasTypeKind::Class){Wrong=Snapshot;Wrong.ObjectClass=TEXT("/Script/Missing.Type");if(WriteDasPropertyValue(P,B,Wrong,Error) || !P->Identical(B,E,PPF_DeepComparison)){AddError(TEXT("Wrong reference/enum identity accepted"));return false;}}
  P->ClearValue(B);if(!WriteDasPropertyValue(P,B,Snapshot,Error) || !P->Identical(B,E,PPF_DeepComparison)){AddError(TEXT("Snapshot not reusable"));return false;}
  if(Snapshot.Kind==int32(EDasMethodSignature::Struct)){
   const auto* SnapshotMemory=Snapshot.StructValue.GetMemory();P->ClearValue(B);
   if(!WriteDasPropertySnapshot(P,B,Snapshot.StructValue,Error) || !P->Identical(B,E,PPF_DeepComparison) || Snapshot.StructValue.GetMemory()!=SnapshotMemory){AddError(TEXT("Borrowed snapshot write changed source or destination value"));return false;}
   FInstancedStruct WrongType;WrongType.InitializeAs(TBaseStructure<FIntPoint>::Get());
   if(WriteDasPropertySnapshot(P,B,WrongType,Error) || !P->Identical(B,E,PPF_DeepComparison)){AddError(TEXT("Rejected borrowed snapshot write changed destination"));return false;}
  }
  ++Count;
 }
 // Values outside UPROPERTY memory remain alive only through the explicit root.
 FDasPropertyValueRoots Roots;TWeakObjectPtr<UObject> Weak;FDasScalarValue Nested;
 {
  UObject* Item=NewObject<USceneComponent>();Weak=Item;
  FDasDefaultBundleProbe Bundle;Bundle.Nested.Assets.Add(Item);Bundle.Nested.Lookup.Add(TEXT("Keep"),Item);Bundle.Rows.Add(Bundle.Nested);Bundle.Catalog.Add(TEXT("Row"),Bundle.Nested);
  Nested.Kind=int32(EDasMethodSignature::Struct);Nested.StructValue.InitializeAs<FDasDefaultBundleProbe>(Bundle);Roots.Keep(Nested);Nested=FDasScalarValue();
 }
 CollectGarbage(RF_NoFlags);if(!TestTrue(TEXT("Nested snapshot roots retain references"),Weak.IsValid()))return false;
 auto& Bundle=Roots.Values[0].StructValue.GetMutable<FDasDefaultBundleProbe>();UObject* Replacement=Second.Get();FCoreUObjectDelegates::FReplacementObjectMap Replacements;Replacements.Add(Weak.Get(),Replacement);Roots.RemapPins(Replacements);
 TestTrue(TEXT("Nested references remap"),Bundle.Nested.Assets[0]==Replacement && Bundle.Nested.Lookup.FindChecked(TEXT("Keep"))==Replacement && Bundle.Rows[0].Assets[0]==Replacement && Bundle.Catalog.FindChecked(TEXT("Row")).Lookup.FindChecked(TEXT("Keep"))==Replacement);
 CollectGarbage(RF_NoFlags);TestFalse(TEXT("Replaced references released"),Weak.IsValid());
 // Key hashes must be rebuilt after replacing object identities.
 for(const TCHAR* Kind:{TEXT("set"),TEXT("map")}){
  const auto Type=Shape(Kind,Object,Leaf(TEXT("int")));TStrongObjectPtr<UScriptStruct> Owner(NewObject<UScriptStruct>(GetTransientPackage()));auto* P=CreateDasProperty(Owner.Get(),TEXT("Value"),Type,EDasPropertyUsage::Value,Error);if(!P){AddError(Error);return false;}Owner->AddCppProperty(P);Owner->Bind();Owner->StaticLink(true);FStructOnScope Storage(Owner.Get());void* Data=P->ContainerPtrToValuePtr<void>(Storage.GetStructMemory());
  SeedPropertyValue(P,Data,3,First.Get(),Second.Get());
  FDasScalarValue Value;if(!ReadDasPropertyValue(P,Data,Value,Error)){AddError(Error);return false;}
  FDasScalarValue Duplicate=Value;
  auto* DuplicateProperty=Duplicate.StructValue.GetScriptStruct()->FindPropertyByName(TEXT("Values"));
  void* DuplicateData=DuplicateProperty->ContainerPtrToValuePtr<void>(Duplicate.StructValue.GetMutableMemory());
  if(auto* Set=CastField<FSetProperty>(DuplicateProperty)){
   FScriptSetHelper Items(Set,DuplicateData);DasReferenceSet(Set->ElementProp,Items.GetElementPtr(1),DasReferenceGet(Set->ElementProp,Items.GetElementPtr(0)));
  }else{
   auto* MapProperty=CastFieldChecked<FMapProperty>(DuplicateProperty);FScriptMapHelper Items(MapProperty,DuplicateData);DasReferenceSet(MapProperty->KeyProp,Items.GetKeyPtr(1),DasReferenceGet(MapProperty->KeyProp,Items.GetKeyPtr(0)));
  }
  TestFalse(TEXT("Duplicate object hash identity rejected"),ValidateDasStructSnapshot(Duplicate.StructValue,Error));
  TestTrue(TEXT("Duplicate rejection preserves original snapshot"),ValidateDasStructSnapshot(Value.StructValue,Error));
  // Leave a sparse hole before the surviving entry. Neither validation nor
  // replacement may visit removed storage or stop at Num() instead of MaxIndex.
  auto* SnapshotProperty=Value.StructValue.GetScriptStruct()->FindPropertyByName(TEXT("Values"));
  void* SnapshotData=SnapshotProperty->ContainerPtrToValuePtr<void>(Value.StructValue.GetMutableMemory());
  if(auto* Set=CastField<FSetProperty>(SnapshotProperty))FScriptSetHelper(Set,SnapshotData).RemoveAt(0);
  else FScriptMapHelper(CastFieldChecked<FMapProperty>(SnapshotProperty),SnapshotData).RemoveAt(0);
  TestTrue(TEXT("Sparse snapshot validates"),ValidateDasStructSnapshot(Value.StructValue,Error));
  TStrongObjectPtr<UObject> NewObjectKey(NewObject<USceneComponent>());FCoreUObjectDelegates::FReplacementObjectMap Map;Map.Add(Second.Get(),NewObjectKey.Get());FDasPropertyValueRoots::Remap(Value,Map);if(!WriteDasPropertyValue(P,Data,Value,Error)){AddError(Error);return false;}UObject* Key=NewObjectKey.Get();
  if(auto* Set=CastField<FSetProperty>(P)){FScriptSetHelper Items(Set,Data);TestTrue(TEXT("Set remap rehashes object key"),Items.FindElementIndex(&Key)!=INDEX_NONE);}else {FScriptMapHelper Items(CastFieldChecked<FMapProperty>(P),Data);TestTrue(TEXT("Map remap rehashes object key"),Items.FindMapIndexWithKey(&Key)!=INDEX_NONE);}
 }
 // The element budget is shared by all nested containers, including the array
 // holding a struct itself. Read-only validation must leave the snapshot intact.
 FInstancedStruct Budget;Budget.InitializeAs<FDasDefaultBundleProbe>();
 auto& Limited=Budget.GetMutable<FDasDefaultBundleProbe>();Limited.Nested.Positions.Init(FVector::ZeroVector,2047);
 Limited.Rows.AddDefaulted_GetRef().Positions.Init(FVector::ZeroVector,2048);
 TestTrue(TEXT("Aggregate nested element limit accepts 4096"),ValidateDasStructSnapshot(Budget,Error));
 Limited.Nested.Positions.Add(FVector::ZeroVector);
 TestFalse(TEXT("Aggregate nested element limit rejects 4097"),ValidateDasStructSnapshot(Budget,Error));
 TestEqual(TEXT("Validation leaves oversized input intact"),Limited.Nested.Positions.Num(),2048);
 Limited.Nested.Positions.Reset();Limited.Rows.Reset();
 auto& BadMath=Limited.Catalog.Add(TEXT("BadMath")).Positions.AddDefaulted_GetRef();BadMath.X=std::numeric_limits<double>::quiet_NaN();
 TestFalse(TEXT("Math validation reaches map struct array element"),ValidateDasStructSnapshot(Budget,Error));
 BadMath=FVector::ZeroVector;
 TestTrue(TEXT("Valid nested math recovers"),ValidateDasStructSnapshot(Budget,Error));
 FDasScalarValue Unchanged;Unchanged.Kind=99;int32 Int=7;TestFalse(TEXT("Null property rejected"),ReadDasPropertyValue(nullptr,&Int,Unchanged,Error));TestEqual(TEXT("Null read atomic"),Unchanged.Kind,99);
 UE_LOG(LogTemp,Display,TEXT("DAS_PROPERTY_VALUES_RESULT=PASS combinations=%d"),Count);return !HasAnyErrors();
}
#endif
