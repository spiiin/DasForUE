#include "DasPropertyTypes.h"
#include "DasContainerTypes.h"
#include "DasDefaultValueProbe.h"
#include "Engine/EngineTypes.h"
#include "GameFramework/Actor.h"
#include "UObject/Interface.h"
#include "UObject/StrongObjectPtr.h"
#include "UObject/UnrealType.h"
#include "UObject/Package.h"
#include "UObject/TextProperty.h"

bool CheckDasPropertyTypes(FString& Error){
 auto Leaf=[](const TCHAR* Kind,const FString& Path=FString(),EDasEnumStorage Storage=EDasEnumStorage::Automatic){FDasDeclaredType T;T.Kind=Kind;T.TypePath=Path;T.EnumStorage=Storage;return T;};
 auto Shape=[](const TCHAR* Kind,const FDasDeclaredType& First,const FDasDeclaredType& Second=FDasDeclaredType()){FDasDeclaredType T;T.Kind=Kind;if(T.GetKind()==EDasTypeKind::Map){T.Key=MakeShared<FDasDeclaredType>(First);T.Value=MakeShared<FDasDeclaredType>(Second);}else T.Element=MakeShared<FDasDeclaredType>(First);return T;};
 const FString EnumPath=StaticEnum<ESpawnActorCollisionHandlingMethod>()->GetPathName();
 auto ByteEnum=Leaf(TEXT("enum"),EnumPath,EDasEnumStorage::Byte),StrongEnum=Leaf(TEXT("enum"),EnumPath,EDasEnumStorage::Enum8);
 const auto Object=Leaf(TEXT("object"),UObject::StaticClass()->GetPathName()),Interface=Leaf(TEXT("object"),UInterface::StaticClass()->GetPathName());
 TArray<FDasDeclaredType> Values;for(const TCHAR* K:{TEXT("byte"),TEXT("text"),TEXT("int"),TEXT("float"),TEXT("double"),TEXT("bool"),TEXT("string"),TEXT("name")})Values.Add(Leaf(K));Values.Append({ByteEnum,StrongEnum,Object,Interface,Leaf(TEXT("struct"),TBaseStructure<FVector>::Get()->GetPathName()),Leaf(TEXT("struct"),FDasDefaultNestedProbe::StaticStruct()->GetPathName())});
 auto Roundtrip=[&](const FDasDeclaredType& Type){
  TStrongObjectPtr<UScriptStruct> A(NewObject<UScriptStruct>(GetTransientPackage())),B(NewObject<UScriptStruct>(GetTransientPackage()));
  auto* P=CreateDasProperty(A.Get(),TEXT("Value"),Type,EDasPropertyUsage::Value,Error);if(!P)return false;A->AddCppProperty(P);A->Bind();A->StaticLink(true);
  FDasDeclaredType Description;if(!DescribeDasProperty(P,Description,Error))return false;
  auto* Copy=CreateDasProperty(B.Get(),TEXT("Value"),Description,EDasPropertyUsage::Value,Error);if(!Copy)return false;B->AddCppProperty(Copy);B->Bind();B->StaticLink(true);
  FDasDeclaredType Again;if(!DescribeDasProperty(Copy,Again,Error) || Description.StorageIdentity()!=Again.StorageIdentity() || !P->SameType(Copy) || P->GetElementSize()!=Copy->GetElementSize() || A->GetMinAlignment()!=B->GetMinAlignment()){Error=TEXT("Property description/build roundtrip mismatch: ")+Type.Kind;return false;}
  if(auto* Set=CastField<FSetProperty>(P);Set && !Set->ElementProp->HasAnyPropertyFlags(CPF_HasGetValueTypeHash)){Error=TEXT("Missing set hash flag");return false;}
  if(auto* Map=CastField<FMapProperty>(P);Map && !Map->KeyProp->HasAnyPropertyFlags(CPF_HasGetValueTypeHash)){Error=TEXT("Missing map hash flag");return false;}
  if(Type.GetKind()==EDasTypeKind::Array || Type.GetKind()==EDasTypeKind::Map || Type.GetKind()==EDasTypeKind::Set){auto* Carrier=DasContainerCarrierForProperty(P);auto* CarrierP=Carrier?Carrier->FindPropertyByName(TEXT("Values")):nullptr;if(!CarrierP || !P->SameType(CarrierP)){Error=TEXT("Factory/native carrier identity mismatch: ")+Description.StorageIdentity();return false;}}
  return true;
 };
 for(const auto& Type:Values)if(!Roundtrip(Type))return false;
 if(!Roundtrip(Leaf(TEXT("class"),AActor::StaticClass()->GetPathName())))return false;
 for(const auto& Type:Values)if(!Roundtrip(Shape(TEXT("array"),Type)))return false;
 TArray<FDasDeclaredType> Keys={Leaf(TEXT("int")),Leaf(TEXT("byte")),Leaf(TEXT("string")),Leaf(TEXT("name")),ByteEnum,StrongEnum,Object};
 for(const auto& Key:Keys){if(!Roundtrip(Shape(TEXT("set"),Key)))return false;for(const auto& Value:Values)if(!Roundtrip(Shape(TEXT("map"),Key,Value)))return false;}
 if(ByteEnum.StorageIdentity()==StrongEnum.StorageIdentity()){Error=TEXT("Enum storage identities collapsed");return false;}
 TStrongObjectPtr<UScriptStruct> Owner(NewObject<UScriptStruct>(GetTransientPackage()));
 auto Reject=[&](const FDasDeclaredType& T,EDasPropertyUsage Usage){FString Why;auto* Unexpected=CreateDasProperty(Owner.Get(),TEXT("Rejected"),T,Usage,Why);if(Unexpected){delete Unexpected;Error=TEXT("Unsupported factory type accepted: ")+T.Kind;return false;}if(Why.IsEmpty() || Owner->ChildProperties){Error=TEXT("Factory failure changed owner or omitted diagnostic");return false;}return true;};
 auto Invalid=Leaf(TEXT("unknown"));if(!Reject(Invalid,EDasPropertyUsage::Value))return false;
 Invalid=Leaf(TEXT("int"),TEXT("/Script/Engine.Actor"));if(!Reject(Invalid,EDasPropertyUsage::Value))return false;
 Invalid=Shape(TEXT("array"),Shape(TEXT("array"),Leaf(TEXT("int"))));if(!Reject(Invalid,EDasPropertyUsage::Container))return false;
 if(!Reject(Shape(TEXT("set"),Interface),EDasPropertyUsage::Container) || !Reject(Shape(TEXT("map"),Interface,Object),EDasPropertyUsage::Container))return false;
 if(!Reject(Shape(TEXT("map"),Leaf(TEXT("int")),Object),EDasPropertyUsage::DelegateParameter) || !Reject(Leaf(TEXT("class"),AActor::StaticClass()->GetPathName()),EDasPropertyUsage::DelegateParameter))return false;
 if(!Reject(Leaf(TEXT("object"),TEXT("/Script/Engine.MissingFactoryClass")),EDasPropertyUsage::Value))return false;
 for(const TCHAR* K:{TEXT("byte"),TEXT("text"),TEXT("int"),TEXT("float"),TEXT("double"),TEXT("bool"),TEXT("string"),TEXT("name")}){auto T=Leaf(K);FString Why;auto* P=CreateDasProperty(Owner.Get(),TEXT("Input"),T,EDasPropertyUsage::DelegateParameter,Why);if(!P){Error=Why;return false;}delete P;}
 Error.Reset();return true;
}
