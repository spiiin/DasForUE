#include "DasContainerTypes.h"
#include "GameFramework/Actor.h"
#include "DasSchema.h"
#include "UObject/NoExportTypes.h"
#include "StructUtils/UserDefinedStruct.h"
#include "StructUtils/InstancedStruct.h"
#include "UObject/Class.h"
#include "UObject/UnrealType.h"
#include "UObject/StrProperty.h"
#include "Misc/OutputDevice.h"
FProperty* FindDasStructField(UScriptStruct* Type,const FString& Name){if(auto* P=Type->FindPropertyByName(*Name))return P;if(Type->IsA<UUserDefinedStruct>())for(TFieldIterator<FProperty> It(Type);It;++It)if(Type->GetAuthoredNameForField(*It)==Name)return *It;return nullptr;}
bool ValidateDasValueStruct(const FString& Path,FString& Error){
    auto* Type=FindObject<UScriptStruct>(nullptr,*Path);if(!Type || (!Path.StartsWith(TEXT("/Script/")) && !(Path.StartsWith(TEXT("/Game/")) && Type->IsA<UUserDefinedStruct>()))){Error=TEXT("Expected loaded native or authored user struct path");return false;}
    TSet<UScriptStruct*> Active;int32 Leaves=0;TFunction<bool(UScriptStruct*,int32)> Visit;
    Visit=[&](UScriptStruct* S,int32 Depth){if(Depth>16 || Active.Contains(S) || S->GetStructureSize()>1048576 || (S->GetCppStructOps() && S->GetCppStructOps()->HasAddStructReferencedObjects())){Error=TEXT("Unsupported recursive/opaque/native-reference struct");return false;}Active.Add(S);
        for(TFieldIterator<FProperty> It(S);It;++It){auto* P=*It;if(P->ArrayDim!=1){Error=TEXT("Fixed array struct fields unsupported");return false;}if(auto* N=CastField<FStructProperty>(P)){if(!Visit(N->Struct,Depth+1))return false;}
            else if(auto* Set=CastField<FSetProperty>(P)){if(auto* Enum=CastField<FEnumProperty>(Set->ElementProp);Enum && !Enum->GetUnderlyingProperty()->IsA<FByteProperty>()){Error=TEXT("Enum sets require byte-sized reflected storage");return false;}if((!CastField<FIntProperty>(Set->ElementProp) && !CastField<FStrProperty>(Set->ElementProp) && !CastField<FNameProperty>(Set->ElementProp) && !DasEnumPropertyType(Set->ElementProp) && !IsDasObjectHashProperty(Set->ElementProp)) || ++Leaves>1024){Error=TEXT("Snapshot sets require int/string/name/enum/object elements");return false;}}
            else if(auto* Map=CastField<FMapProperty>(P)){auto* V=CastField<FStructProperty>(Map->ValueProp);if(V && !Visit(V->Struct,Depth+1))return false;if((!CastField<FIntProperty>(Map->KeyProp) && !CastField<FStrProperty>(Map->KeyProp) && !CastField<FNameProperty>(Map->KeyProp) && !IsDasObjectHashProperty(Map->KeyProp)) || (!CastField<FIntProperty>(Map->ValueProp) && !CastField<FStrProperty>(Map->ValueProp) && !CastField<FFloatProperty>(Map->ValueProp) && !CastField<FDoubleProperty>(Map->ValueProp) && !CastField<FBoolProperty>(Map->ValueProp) && !CastField<FNameProperty>(Map->ValueProp) && !V && !DasEnumPropertyType(Map->ValueProp) && !DasReferencePropertyClass(Map->ValueProp)) || ++Leaves>1024){Error=TEXT("Snapshot maps require int/string/name keys and scalar/name/struct/enum/reference values");return false;}}
            else if(auto* Array=CastField<FArrayProperty>(P)){auto* Element=CastField<FStructProperty>(Array->Inner);if(Element){if(!Visit(Element->Struct,Depth+1))return false;}else if(!(CastField<FIntProperty>(Array->Inner) || CastField<FFloatProperty>(Array->Inner) || CastField<FDoubleProperty>(Array->Inner) || CastField<FBoolProperty>(Array->Inner) || CastField<FStrProperty>(Array->Inner) || CastField<FNameProperty>(Array->Inner) || DasEnumPropertyType(Array->Inner) || DasReferencePropertyClass(Array->Inner)) || ++Leaves>1024){Error=TEXT("Unsupported snapshot array element");return false;}}
            else {if(!(CastField<FIntProperty>(P) || CastField<FFloatProperty>(P) || CastField<FDoubleProperty>(P) || CastField<FBoolProperty>(P) || CastField<FStrProperty>(P) || CastField<FNameProperty>(P) || DasReferencePropertyClass(P) || CastField<FEnumProperty>(P) || (CastField<FByteProperty>(P) && CastField<FByteProperty>(P)->Enum)) || ++Leaves>1024){Error=TEXT("Struct supports scalar leaves; object/container/other fields unsupported");return false;}}}
        Active.Remove(S);return true;};return Visit(Type,0);
}

bool ValidateDasStructSnapshot(const FInstancedStruct& Value,FString& Error){
    if(!Value.IsValid()){Error=TEXT("Missing struct snapshot");return false;}if(!ValidateDasValueStruct(Value.GetScriptStruct()->GetPathName(),Error))return false;
    int64 ArrayBytes=0;int32 ArrayElements=0;TFunction<bool(const UScriptStruct*,const uint8*)> Visit;
    Visit=[&](const UScriptStruct* Type,const uint8* Data){
        if((Type==TBaseStructure<FVector2D>::Get() && reinterpret_cast<const FVector2D*>(Data)->ContainsNaN()) || (Type==TBaseStructure<FVector>::Get() && reinterpret_cast<const FVector*>(Data)->ContainsNaN()) || (Type==TBaseStructure<FRotator>::Get() && reinterpret_cast<const FRotator*>(Data)->ContainsNaN()) || (Type==TBaseStructure<FTransform>::Get() && (reinterpret_cast<const FTransform*>(Data)->ContainsNaN() || !reinterpret_cast<const FTransform*>(Data)->IsRotationNormalized()))){Error=TEXT("Invalid nested math snapshot value");return false;}
        for(TFieldIterator<FProperty> It(Type);It;++It){const auto* P=*It;const uint8* Memory=P->ContainerPtrToValuePtr<uint8>(Data);
        if(auto* S=CastField<FStructProperty>(P)){if(!Visit(S->Struct,Memory))return false;continue;}
        if(auto* Array=CastField<FArrayProperty>(P)){FScriptArrayHelper Elements(Array,Memory);ArrayElements+=Elements.Num();ArrayBytes+=int64(Elements.Num())*Array->Inner->GetElementSize();if(ArrayElements>4096 || ArrayBytes>16*1024*1024){Error=TEXT("Struct array snapshot exceeds element/byte limit");return false;}if(auto* Element=CastField<FStructProperty>(Array->Inner))for(int32 I=0;I<Elements.Num();++I)if(!Visit(Element->Struct,Elements.GetRawPtr(I)))return false;if(auto* C=DasReferencePropertyClass(Array->Inner))for(int32 I=0;I<Elements.Num();++I){auto* O=DasReferenceGet(Array->Inner,Elements.GetRawPtr(I));if(O && IsValid(O) && !DasReferenceMatches(O,C)){Error=TEXT("Reference array element class/interface mismatch");return false;}}if(auto* E=DasEnumPropertyType(Array->Inner))for(int32 I=0;I<Elements.Num();++I){const int32 Index=E->GetIndexByValue(DasEnumPropertyNumber(Array->Inner,Elements.GetRawPtr(I)));if(Index==INDEX_NONE || E->GetNameStringByIndex(Index).EndsWith(TEXT("_MAX"))){Error=TEXT("Unknown or reserved enum array element");return false;}}continue;}
        if(auto* Set=CastField<FSetProperty>(P)){FScriptSetHelper Values(Set,Memory);ArrayElements+=Values.Num();ArrayBytes+=int64(Values.Num())*Set->ElementProp->GetElementSize();if(ArrayElements>4096 || ArrayBytes>16*1024*1024){Error=TEXT("Snapshot container limit exceeded");return false;}
            if(IsDasObjectHashProperty(Set->ElementProp)){TSet<UObject*> Seen;for(int32 I=0;I<Values.GetMaxIndex();++I)if(Values.IsValidIndex(I)){auto* O=DasReferenceGet(Set->ElementProp,Values.GetElementPtr(I));if(Seen.Contains(O)){Error=TEXT("Duplicate object set identity");return false;}Seen.Add(O);if(O && (!IsValid(O) || (O->IsA<AActor>() && CastChecked<AActor>(O)->IsActorBeingDestroyed()) || !DasReferenceMatches(O,DasReferencePropertyClass(Set->ElementProp)))){Error=TEXT("Invalid or destroyed object set element");return false;}}}
            if(auto* E=DasEnumPropertyType(Set->ElementProp))for(int32 I=0;I<Values.GetMaxIndex();++I)if(Values.IsValidIndex(I)){const int32 Index=E->GetIndexByValue(DasEnumPropertyNumber(Set->ElementProp,Values.GetElementPtr(I)));if(Index==INDEX_NONE || E->GetNameStringByIndex(Index).EndsWith(TEXT("_MAX"))){Error=TEXT("Unknown or reserved enum set element");return false;}}
            continue;}
        if(auto* Map=CastField<FMapProperty>(P)){FScriptMapHelper Values(Map,Memory);ArrayElements+=Values.Num();ArrayBytes+=int64(Values.Num())*(Map->KeyProp->GetElementSize()+Map->ValueProp->GetElementSize());if(ArrayElements>4096 || ArrayBytes>16*1024*1024){Error=TEXT("Snapshot container limit exceeded");return false;}if(IsDasObjectHashProperty(Map->KeyProp)){TSet<UObject*> Seen;for(int32 I=0;I<Values.GetMaxIndex();++I)if(Values.IsValidIndex(I)){auto* O=DasReferenceGet(Map->KeyProp,Values.GetKeyPtr(I));if(Seen.Contains(O)){Error=TEXT("Duplicate map object identity");return false;}Seen.Add(O);if(O && (!IsValid(O) || (O->IsA<AActor>() && CastChecked<AActor>(O)->IsActorBeingDestroyed()) || !DasReferenceMatches(O,DasReferencePropertyClass(Map->KeyProp)))){Error=TEXT("Invalid or destroyed map object key");return false;}}}if(auto* V=CastField<FStructProperty>(Map->ValueProp))for(int32 I=0;I<Values.GetMaxIndex();++I)if(Values.IsValidIndex(I) && !Visit(V->Struct,Values.GetValuePtr(I)))return false;if(auto* E=DasEnumPropertyType(Map->ValueProp))for(int32 I=0;I<Values.GetMaxIndex();++I)if(Values.IsValidIndex(I)){const int32 Index=E->GetIndexByValue(DasEnumPropertyNumber(Map->ValueProp,Values.GetValuePtr(I)));if(Index==INDEX_NONE || E->GetNameStringByIndex(Index).EndsWith(TEXT("_MAX"))){Error=TEXT("Unknown or reserved enum map value");return false;}}if(auto* C=DasReferencePropertyClass(Map->ValueProp))for(int32 I=0;I<Values.GetMaxIndex();++I)if(Values.IsValidIndex(I)){auto* O=DasReferenceGet(Map->ValueProp,Values.GetValuePtr(I));if(O && IsValid(O) && !DasReferenceMatches(O,C)){Error=TEXT("Reference map value class/interface mismatch");return false;}}continue;}
        const UEnum* Enum=nullptr;const FNumericProperty* Storage=nullptr;
        if(auto* E=CastField<FEnumProperty>(P)){Enum=E->GetEnum();Storage=E->GetUnderlyingProperty();}else if(auto* B=CastField<FByteProperty>(P)){Enum=B->Enum;Storage=B;}
        if(Enum){int32 Index=Enum->GetIndexByValue(Storage->GetSignedIntPropertyValue(Memory));if(Index==INDEX_NONE || Enum->GetNameStringByIndex(Index).EndsWith(TEXT("_MAX"))){Error=TEXT("Unknown or reserved enum value in struct field: ")+P->GetName();return false;}}
    }return true;};return Visit(Value.GetScriptStruct(),Value.GetMemory());
}

namespace {
bool ValidateStructDefaultFields(const UScriptStruct* Type,const FString& Text,FString& Error,FString& NativeText){
    struct FNameEdit{int32 Begin,Length;FString Name;};TArray<FNameEdit> Names;int32 Pos=0;auto Space=[&](){while(Pos<Text.Len() && FChar::IsWhitespace(Text[Pos]))++Pos;};
    TFunction<bool(const UScriptStruct*)> Read;
    Read=[&](const UScriptStruct* S){
        Space();if(Pos>=Text.Len() || Text[Pos++]!=TEXT('('))return false;TSet<FName> Seen;
        for(;;){Space();if(Pos<Text.Len() && Text[Pos]==TEXT(')')){++Pos;return true;}
            int32 Begin=Pos;while(Pos<Text.Len() && (FChar::IsAlnum(Text[Pos]) || Text[Pos]==TEXT('_')))++Pos;
            if(Begin==Pos)return false;const FString Name=Text.Mid(Begin,Pos-Begin);auto* P=FindDasStructField(const_cast<UScriptStruct*>(S),Name);if(P)Names.Add({Begin,Pos-Begin,P->GetName()});Space();
            if(!P || Seen.Contains(P->GetFName()) || Pos>=Text.Len() || Text[Pos++]!=TEXT('='))return false;Seen.Add(P->GetFName());Space();
            if(auto* Nested=CastField<FStructProperty>(P)){if(!Read(Nested->Struct))return false;}else if(CastField<FArrayProperty>(P) || CastField<FMapProperty>(P) || CastField<FSetProperty>(P)){Error=TEXT("Explicit struct array defaults require a separate policy");return false;}
            else {
                Begin=Pos;if(Pos<Text.Len() && Text[Pos]==TEXT('"')){++Pos;bool Closed=false;while(Pos<Text.Len()){TCHAR Ch=Text[Pos++];if(Ch==TEXT('\\')){if(Pos>=Text.Len())return false;++Pos;}else if(Ch==TEXT('"')){Closed=true;break;}}if(!Closed)return false;}
                else {while(Pos<Text.Len() && Text[Pos]!=TEXT(',') && Text[Pos]!=TEXT(')')){if(Text[Pos]==TEXT('(') || Text[Pos]==TEXT('"'))return false;++Pos;}if(Text.Mid(Begin,Pos-Begin).TrimStartAndEnd().IsEmpty())return false;}
            }
            Space();if(Pos>=Text.Len())return false;if(Text[Pos]==TEXT(')')){++Pos;return true;}if(Text[Pos++]!=TEXT(','))return false;
            Space();if(Pos>=Text.Len() || Text[Pos]==TEXT(')'))return false;
        }
    };
    const bool Valid=Read(Type);Space();if(!Valid || Pos!=Text.Len()){Error=TEXT("Malformed, unknown or duplicate struct default field: ")+Type->GetPathName();return false;}NativeText=Text;for(int32 I=Names.Num()-1;I>=0;--I){NativeText.RemoveAt(Names[I].Begin,Names[I].Length);NativeText.InsertAt(Names[I].Begin,Names[I].Name);}return true;
}
}

bool ParseDasStructDefault(const FString& Path,const FString& Text,FInstancedStruct& Out,FString& Error){
    Out.Reset();if(!ValidateDasValueStruct(Path,Error))return false;
    FInstancedStruct Candidate;Candidate.InitializeAs(FindObject<UScriptStruct>(nullptr,*Path));
    if(!Text.IsEmpty()){
        if(Text.Len()>1048576){Error=TEXT("Struct default exceeds text limit");return false;}
        FString NativeText;if(!ValidateStructDefaultFields(Candidate.GetScriptStruct(),Text,Error,NativeText))return false;
        struct FDiagnostics: FOutputDevice {bool Seen=false;virtual void Serialize(const TCHAR*,ELogVerbosity::Type,const FName&) override {Seen=true;}} Diagnostics;
        const TCHAR* End=Candidate.GetScriptStruct()->ImportText(*NativeText,Candidate.GetMutableMemory(),nullptr,PPF_None,&Diagnostics,Path,false);
        if(!End || Diagnostics.Seen || !FString(End).TrimStartAndEnd().IsEmpty()){Error=TEXT("Invalid struct default text: ")+Path;return false;}
    }
    if(!ValidateDasStructSnapshot(Candidate,Error))return false;Out=MoveTemp(Candidate);return true;
}
