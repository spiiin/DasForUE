#include "DasPropertyTypes.h"
#include "DasSoftReferences.h"
#include "DasDefinedEnum.h"
#include "UObject/UnrealType.h"
#include "UObject/StrProperty.h"
#include "UObject/TextProperty.h"

namespace {
struct FKindName {const TCHAR* Name;EDasTypeKind Kind;};
const FKindName Names[]={
 {TEXT("void"),EDasTypeKind::Void},{TEXT("byte"),EDasTypeKind::Byte},{TEXT("text"),EDasTypeKind::Text},
 {TEXT("int"),EDasTypeKind::Int},{TEXT("float"),EDasTypeKind::Float},{TEXT("double"),EDasTypeKind::Double},
 {TEXT("bool"),EDasTypeKind::Bool},{TEXT("string"),EDasTypeKind::String},{TEXT("name"),EDasTypeKind::Name},
 {TEXT("object"),EDasTypeKind::Object},{TEXT("class"),EDasTypeKind::Class},{TEXT("struct"),EDasTypeKind::Struct},
 {TEXT("enum"),EDasTypeKind::Enum},{TEXT("array"),EDasTypeKind::Array},{TEXT("set"),EDasTypeKind::Set},
 {TEXT("map"),EDasTypeKind::Map},{TEXT("delegate"),EDasTypeKind::Delegate},{TEXT("event"),EDasTypeKind::Event}
};
bool HashKey(EDasTypeKind K){return K==EDasTypeKind::Int || K==EDasTypeKind::Byte || K==EDasTypeKind::String || K==EDasTypeKind::Name || K==EDasTypeKind::Enum || K==EDasTypeKind::Object;}
bool Container(EDasTypeKind K){return K==EDasTypeKind::Array || K==EDasTypeKind::Set || K==EDasTypeKind::Map;}
bool Validate(const FDasDeclaredType& T,EDasPropertyUsage Usage,FString& Error,int32 Depth,int32& Nodes){
 auto Fail=[&](const TCHAR* Why){Error=FString(Why)+TEXT(": ")+T.Kind+TEXT(" ")+T.TypePath;return false;};
 if(Depth>16 || ++Nodes>1024)return Fail(TEXT("Property type exceeds depth/node limit"));
 const auto K=T.GetKind();
 if(K==EDasTypeKind::Invalid || K==EDasTypeKind::Void || K==EDasTypeKind::Delegate || K==EDasTypeKind::Event)return Fail(TEXT("Unsupported property kind"));
 if(K!=EDasTypeKind::Enum && T.EnumStorage!=EDasEnumStorage::Automatic)return Fail(TEXT("Enum storage on a non-enum type"));
 if(!T.AuthoredId.IsEmpty() && K!=EDasTypeKind::Object)return Fail(TEXT("Authored object identity on a non-object type"));
 if(!T.DelegateWire.IsEmpty())return Fail(TEXT("Unexpected delegate signature"));
 if(Container(K)){
  if(!T.TypePath.IsEmpty())return Fail(TEXT("Container type_path must be empty"));
  if(K==EDasTypeKind::Map){if(!T.Key || !T.Value || T.Element || !HashKey(T.Key->GetKind()) || Container(T.Value->GetKind()) || T.Value->GetKind()==EDasTypeKind::Class)return Fail(TEXT("Unsupported map shape"));
   if(!Validate(*T.Key,Usage,Error,Depth+1,Nodes) || !Validate(*T.Value,Usage,Error,Depth+1,Nodes))return false;
   if(T.Key->GetKind()==EDasTypeKind::Object){auto* C=FindObject<UClass>(nullptr,*T.Key->TypePath);if(C->HasAnyClassFlags(CLASS_Interface))return Fail(TEXT("Interface object keys are unsupported"));}
  }else{if(!T.Element || T.Key || T.Value || Container(T.Element->GetKind()) || T.Element->GetKind()==EDasTypeKind::Class || (K==EDasTypeKind::Set && !HashKey(T.Element->GetKind())))return Fail(TEXT("Unsupported element shape"));
   if(!Validate(*T.Element,Usage,Error,Depth+1,Nodes))return false;
   if(K==EDasTypeKind::Set && T.Element->GetKind()==EDasTypeKind::Object){auto* C=FindObject<UClass>(nullptr,*T.Element->TypePath);if(C->HasAnyClassFlags(CLASS_Interface))return Fail(TEXT("Interface object sets are unsupported"));}
  }return true;
 }
 if(T.Element || T.Key || T.Value)return Fail(TEXT("Leaf has container members"));
 if(K==EDasTypeKind::Object || K==EDasTypeKind::Class){auto* C=FindObject<UClass>(nullptr,*T.TypePath);if(!C || (K==EDasTypeKind::Class && C->HasAnyClassFlags(CLASS_Interface)))return Fail(TEXT("Missing or incompatible loaded class"));if(Usage==EDasPropertyUsage::Container && !T.TypePath.StartsWith(TEXT("/Script/")))return Fail(TEXT("Container references require native classes"));return true;}
 if(K==EDasTypeKind::Struct){auto* S=FindObject<UScriptStruct>(nullptr,*T.TypePath);if(!S)return Fail(TEXT("Missing loaded struct"));return Usage!=EDasPropertyUsage::Container || ValidateDasValueStruct(T.TypePath,Error);}
 if(K==EDasTypeKind::Enum){auto* E=FindObject<UEnum>(nullptr,*T.TypePath);if(!E)return Fail(TEXT("Missing loaded enum"));for(int32 I=0;I<E->NumEnums();++I){if(E->GetNameStringByIndex(I).EndsWith(TEXT("_MAX")))continue;const auto N=E->GetValueByIndex(I);if(N<0 || N>254)return Fail(TEXT("Enum property requires values in 0..254"));}return true;}
 return T.TypePath.IsEmpty()?true:Fail(TEXT("Primitive has a type identity"));
}
bool Describe(const FProperty* P,FDasDeclaredType& Out,FString& Error,int32 Depth,int32& Nodes){
 if(!P || P->ArrayDim!=1 || Depth>16 || ++Nodes>1024){Error=TEXT("Unsupported fixed array or property nesting");return false;}
 Out={};const auto K=DasPropertyKind(P);Out.Kind=DasTypeKindName(K);
 if(K==EDasTypeKind::Invalid){Error=TEXT("Unsupported reflected property: ")+P->GetClass()->GetName();return false;}
 if(auto* A=CastField<FArrayProperty>(P)){auto Child=MakeShared<FDasDeclaredType>();Out.Element=Child;return Describe(A->Inner,*Child,Error,Depth+1,Nodes);}
 if(auto* S=CastField<FSetProperty>(P)){auto Child=MakeShared<FDasDeclaredType>();Out.Element=Child;return Describe(S->ElementProp,*Child,Error,Depth+1,Nodes);}
 if(auto* M=CastField<FMapProperty>(P)){auto Key=MakeShared<FDasDeclaredType>(),Value=MakeShared<FDasDeclaredType>();Out.Key=Key;Out.Value=Value;return Describe(M->KeyProp,*Key,Error,Depth+1,Nodes) && Describe(M->ValueProp,*Value,Error,Depth+1,Nodes);}
 if(auto* E=CastField<FEnumProperty>(P)){if(!E->GetUnderlyingProperty() || !E->GetUnderlyingProperty()->IsA<FByteProperty>()){Error=TEXT("Only byte-sized enum storage is supported");return false;}Out.TypePath=E->GetEnum()->GetPathName();Out.EnumStorage=EDasEnumStorage::Enum8;}
 else if(auto* B=CastField<FByteProperty>(P);B && B->Enum){Out.TypePath=B->Enum->GetPathName();Out.EnumStorage=EDasEnumStorage::Byte;}
 else if(auto* Soft=DasSoftCarrierForProperty(P))Out.TypePath=Soft->GetPathName();
 else if(auto* C=CastField<FClassProperty>(P))Out.TypePath=C->MetaClass->GetPathName();
 else if(auto* S=CastField<FStructProperty>(P))Out.TypePath=S->Struct->GetPathName();
 else if(auto* ReferenceClass=DasReferencePropertyClass(P))Out.TypePath=ReferenceClass->GetPathName();
 if(K==EDasTypeKind::Delegate || K==EDasTypeKind::Event){Error=TEXT("Delegate signature requires its explicit authoring contract");return false;}
 return true;
}
FProperty* Build(FFieldVariant Owner,FName Name,const FDasDeclaredType& T,EDasPropertyUsage Usage){
 switch(T.GetKind()){
 case EDasTypeKind::Byte:return new FByteProperty(Owner,Name);
 case EDasTypeKind::Text:return new FTextProperty(Owner,Name);
 case EDasTypeKind::Int:return new FIntProperty(Owner,Name);
 case EDasTypeKind::Float:return new FFloatProperty(Owner,Name);
 case EDasTypeKind::Double:return new FDoubleProperty(Owner,Name);
 case EDasTypeKind::Bool:{auto* P=new FBoolProperty(Owner,Name);P->SetBoolSize(sizeof(bool),true);return P;}
 case EDasTypeKind::String:return new FStrProperty(Owner,Name);
 case EDasTypeKind::Name:return new FNameProperty(Owner,Name);
 case EDasTypeKind::Struct:{if(auto* Soft=DasSoftProperty(T.TypePath))return CreateDasSoftProperty(Owner,Name,Soft);auto* P=new FStructProperty(Owner,Name);P->Struct=FindObject<UScriptStruct>(nullptr,*T.TypePath);return P;}
 case EDasTypeKind::Object:{auto* C=FindObject<UClass>(nullptr,*T.TypePath);if(C->HasAnyClassFlags(CLASS_Interface)){auto* P=new FInterfaceProperty(Owner,Name);P->InterfaceClass=C;return P;}auto* P=new FObjectProperty(Owner,Name);P->PropertyClass=C;return P;}
 case EDasTypeKind::Class:{auto* P=new FClassProperty(Owner,Name);P->PropertyClass=UClass::StaticClass();P->MetaClass=FindObject<UClass>(nullptr,*T.TypePath);return P;}
 case EDasTypeKind::Enum:{auto* E=FindObject<UEnum>(nullptr,*T.TypePath);if(DasResolvedEnumStorage(T)==EDasEnumStorage::Enum8){auto* P=new FEnumProperty(Owner,Name);P->SetEnum(E);P->AddCppProperty(new FByteProperty(P,TEXT("UnderlyingType")));return P;}auto* P=new FByteProperty(Owner,Name);P->Enum=E;return P;}
 case EDasTypeKind::Array:{auto* P=new FArrayProperty(Owner,Name);P->Inner=Build(P,Usage==EDasPropertyUsage::DelegateParameter?TEXT("Inner"):TEXT("Values"),*T.Element,Usage);return P;}
 case EDasTypeKind::Set:{auto* P=new FSetProperty(Owner,Name);P->ElementProp=Build(P,TEXT("Element"),*T.Element,Usage);P->ElementProp->SetPropertyFlags(CPF_HasGetValueTypeHash);return P;}
 case EDasTypeKind::Map:{auto* P=new FMapProperty(Owner,Name);P->KeyProp=Build(P,TEXT("Key"),*T.Key,Usage);P->KeyProp->SetPropertyFlags(CPF_HasGetValueTypeHash);P->ValueProp=Build(P,TEXT("Value"),*T.Value,Usage);return P;}
 default:return nullptr;
 }
}
}
EDasTypeKind DasTypeKind(const FString& Name){for(const auto& Entry:Names)if(Name==Entry.Name)return Entry.Kind;return EDasTypeKind::Invalid;}
EDasEnumStorage DasResolvedEnumStorage(const FDasDeclaredType& Type){
 if(Type.GetKind()!=EDasTypeKind::Enum)return EDasEnumStorage::Automatic;
 if(Type.EnumStorage!=EDasEnumStorage::Automatic)return Type.EnumStorage;
 const auto* Enum=FindObject<UEnum>(nullptr,*Type.TypePath);
 return Enum && !Enum->IsA<UDasDefinedEnum>() && Enum->GetCppForm()==UEnum::ECppForm::EnumClass?EDasEnumStorage::Enum8:EDasEnumStorage::Byte;
}
FString DasTypeKindName(EDasTypeKind Kind){for(const auto& Entry:Names)if(Kind==Entry.Kind)return Entry.Name;return {};}
FString FDasDeclaredType::StorageIdentity() const {
 TFunction<FString(const FDasDeclaredType&,int32)> StorageKey;
 StorageKey=[&](const FDasDeclaredType& T,int32 Depth){if(Depth>16)return FString(TEXT("invalid-depth"));FString S=T.Kind+FString::Printf(TEXT("[%d]:%d:"),int32(T.EnumStorage),T.TypePath.Len())+T.TypePath;if(T.Element)S+=TEXT("<")+StorageKey(*T.Element,Depth+1)+TEXT(">");if(T.Key)S+=TEXT("{")+StorageKey(*T.Key,Depth+1)+TEXT(";")+(T.Value?StorageKey(*T.Value,Depth+1):TEXT("missing"))+TEXT("}");return S;};return StorageKey(*this,0);
}
EDasTypeKind DasPropertyKind(const FProperty* P){
 if(!P || P->ArrayDim!=1)return EDasTypeKind::Invalid;
 if(CastField<FArrayProperty>(P))return EDasTypeKind::Array;if(CastField<FSetProperty>(P))return EDasTypeKind::Set;if(CastField<FMapProperty>(P))return EDasTypeKind::Map;
 if(auto* B=CastField<FByteProperty>(P))return B->Enum?EDasTypeKind::Enum:EDasTypeKind::Byte;
 if(CastField<FEnumProperty>(P))return EDasTypeKind::Enum;if(CastField<FTextProperty>(P))return EDasTypeKind::Text;
 if(CastField<FIntProperty>(P))return EDasTypeKind::Int;if(CastField<FFloatProperty>(P))return EDasTypeKind::Float;if(CastField<FDoubleProperty>(P))return EDasTypeKind::Double;if(CastField<FBoolProperty>(P))return EDasTypeKind::Bool;
 if(CastField<FStrProperty>(P))return EDasTypeKind::String;if(CastField<FNameProperty>(P))return EDasTypeKind::Name;if(CastField<FClassProperty>(P))return EDasTypeKind::Class;
 if(CastField<FSoftObjectProperty>(P))return EDasTypeKind::Struct;
 if(DasReferencePropertyClass(P))return EDasTypeKind::Object;if(CastField<FStructProperty>(P))return EDasTypeKind::Struct;if(CastField<FDelegateProperty>(P))return EDasTypeKind::Delegate;if(CastField<FMulticastDelegateProperty>(P))return EDasTypeKind::Event;return EDasTypeKind::Invalid;
}
bool DescribeDasProperty(const FProperty* P,FDasDeclaredType& Out,FString& Error){Error.Reset();FDasDeclaredType Result;int32 Nodes=0;if(!Describe(P,Result,Error,0,Nodes)){Out={};Out.Kind.Reset();return false;}Out=MoveTemp(Result);return true;}
bool ValidateDasPropertyType(const FDasDeclaredType& Type,EDasPropertyUsage Usage,FString& Error){Error.Reset();int32 Nodes=0;return Validate(Type,Usage,Error,0,Nodes);}
FProperty* CreateDasProperty(FFieldVariant Owner,FName Name,const FDasDeclaredType& Type,EDasPropertyUsage Usage,FString& Error){if(!IsInGameThread()){Error=TEXT("Property creation requires the game thread");return nullptr;}if(!ValidateDasPropertyType(Type,Usage,Error))return nullptr;return Build(Owner,Name,Type,Usage);}
