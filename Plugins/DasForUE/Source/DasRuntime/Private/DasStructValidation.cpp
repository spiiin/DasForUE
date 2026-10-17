#include "DasValueTraversal.h"
#include "DasContainerTypes.h"
#include "DasDefaultRoots.h"
#include "Serialization/JsonSerializer.h"
#include "GameFramework/Actor.h"
#include "DasSchema.h"
#include "UObject/NoExportTypes.h"
#include "StructUtils/UserDefinedStruct.h"
#include "StructUtils/InstancedStruct.h"
#include "UObject/Class.h"
#include "UObject/UnrealType.h"
#include "UObject/StrProperty.h"
#include "UObject/TextProperty.h"
#include "Misc/OutputDevice.h"
FProperty* FindDasStructField(const UScriptStruct* Type,const FString& Name){if(auto* P=Type->FindPropertyByName(*Name))return P;if(Type->IsA<UUserDefinedStruct>())for(TFieldIterator<FProperty> It(Type);It;++It)if(Type->GetAuthoredNameForField(*It)==Name)return *It;return nullptr;}
bool ValidateDasValueStruct(const FString& Path,FString& Error){
    auto* Type=FindObject<UScriptStruct>(nullptr,*Path);if(!Type || (!Path.StartsWith(TEXT("/Script/")) && !(Path.StartsWith(TEXT("/Game/")) && Type->IsA<UUserDefinedStruct>()))){Error=TEXT("Expected loaded native or authored user struct path");return false;}
    TSet<UScriptStruct*> Active;int32 Leaves=0;TFunction<bool(UScriptStruct*,int32)> Visit;
    Visit=[&](UScriptStruct* S,int32 Depth){if(Depth>16 || Active.Contains(S) || S->GetStructureSize()>1048576 || (S->GetCppStructOps() && S->GetCppStructOps()->HasAddStructReferencedObjects())){Error=TEXT("Unsupported recursive/opaque/native-reference struct");return false;}Active.Add(S);
        for(TFieldIterator<FProperty> It(S);It;++It){auto* P=*It;if(P->ArrayDim!=1){Error=TEXT("Fixed array struct fields unsupported");return false;}if(auto* N=CastField<FStructProperty>(P)){if(!Visit(N->Struct,Depth+1))return false;}
            else if(auto* Set=CastField<FSetProperty>(P)){if(auto* Enum=CastField<FEnumProperty>(Set->ElementProp);Enum && !Enum->GetUnderlyingProperty()->IsA<FByteProperty>()){Error=TEXT("Enum sets require byte-sized reflected storage");return false;}if((!(CastField<FByteProperty>(Set->ElementProp) && !DasEnumPropertyType(Set->ElementProp)) && !CastField<FIntProperty>(Set->ElementProp) && !CastField<FStrProperty>(Set->ElementProp) && !CastField<FNameProperty>(Set->ElementProp) && !DasEnumPropertyType(Set->ElementProp) && !IsDasObjectHashProperty(Set->ElementProp)) || ++Leaves>1024){Error=TEXT("Snapshot sets require int/string/name/enum/object elements");return false;}}
            else if(auto* Map=CastField<FMapProperty>(P)){if((DasEnumPropertyType(Map->KeyProp) && Map->KeyProp->GetElementSize()!=1) || (CastField<FEnumProperty>(Map->KeyProp) && !CastFieldChecked<FEnumProperty>(Map->KeyProp)->GetUnderlyingProperty()->IsA<FByteProperty>())){Error=TEXT("Enum map keys require byte-sized reflected storage");return false;}auto* V=CastField<FStructProperty>(Map->ValueProp);if(V && !Visit(V->Struct,Depth+1))return false;if((!(CastField<FByteProperty>(Map->KeyProp) && !DasEnumPropertyType(Map->KeyProp)) && !CastField<FIntProperty>(Map->KeyProp) && !CastField<FStrProperty>(Map->KeyProp) && !CastField<FNameProperty>(Map->KeyProp) && !DasEnumPropertyType(Map->KeyProp) && !IsDasObjectHashProperty(Map->KeyProp)) || (!CastField<FTextProperty>(Map->ValueProp) && !(CastField<FByteProperty>(Map->ValueProp) && !DasEnumPropertyType(Map->ValueProp)) && !CastField<FIntProperty>(Map->ValueProp) && !CastField<FStrProperty>(Map->ValueProp) && !CastField<FFloatProperty>(Map->ValueProp) && !CastField<FDoubleProperty>(Map->ValueProp) && !CastField<FBoolProperty>(Map->ValueProp) && !CastField<FNameProperty>(Map->ValueProp) && !V && !DasEnumPropertyType(Map->ValueProp) && !DasReferencePropertyClass(Map->ValueProp)) || ++Leaves>1024){Error=TEXT("Snapshot maps require int/string/name/enum/object keys and scalar/name/struct/enum/reference values");return false;}}
            else if(auto* Array=CastField<FArrayProperty>(P)){auto* Element=CastField<FStructProperty>(Array->Inner);if(Element){if(!Visit(Element->Struct,Depth+1))return false;}else if(!(CastField<FTextProperty>(Array->Inner) || (CastField<FByteProperty>(Array->Inner) && !DasEnumPropertyType(Array->Inner)) || CastField<FIntProperty>(Array->Inner) || CastField<FFloatProperty>(Array->Inner) || CastField<FDoubleProperty>(Array->Inner) || CastField<FBoolProperty>(Array->Inner) || CastField<FStrProperty>(Array->Inner) || CastField<FNameProperty>(Array->Inner) || DasEnumPropertyType(Array->Inner) || DasReferencePropertyClass(Array->Inner)) || ++Leaves>1024){Error=TEXT("Unsupported snapshot array element");return false;}}
            else {if(!((CastField<FByteProperty>(P) && !CastField<FByteProperty>(P)->Enum) || CastField<FTextProperty>(P) || CastField<FIntProperty>(P) || CastField<FFloatProperty>(P) || CastField<FDoubleProperty>(P) || CastField<FBoolProperty>(P) || CastField<FStrProperty>(P) || CastField<FNameProperty>(P) || CastField<FClassProperty>(P) || DasReferencePropertyClass(P) || CastField<FEnumProperty>(P) || (CastField<FByteProperty>(P) && CastField<FByteProperty>(P)->Enum)) || ++Leaves>1024){Error=TEXT("Struct supports scalar leaves; object/container/other fields unsupported");return false;}}}
        Active.Remove(S);return true;};return Visit(Type,0);
}

bool ValidateDasStructSnapshot(const FInstancedStruct& Value,FString& Error){
    if(!Value.IsValid()){Error=TEXT("Missing struct snapshot");return false;}
    if(!ValidateDasValueStruct(Value.GetScriptStruct()->GetPathName(),Error))return false;
    struct FValidation {
        FString& Error;
        int64 Bytes=0, Elements=0;
        struct FKeys {TSet<UObject*> Objects;TSet<int64> Enums;};
        TArray<FKeys> Containers;
        bool Struct(const UScriptStruct* Type,const uint8* Data){
            if((Type==TBaseStructure<FVector2D>::Get() && reinterpret_cast<const FVector2D*>(Data)->ContainsNaN()) || (Type==TBaseStructure<FVector>::Get() && reinterpret_cast<const FVector*>(Data)->ContainsNaN()) || (Type==TBaseStructure<FRotator>::Get() && reinterpret_cast<const FRotator*>(Data)->ContainsNaN()) || (Type==TBaseStructure<FTransform>::Get() && (reinterpret_cast<const FTransform*>(Data)->ContainsNaN() || !reinterpret_cast<const FTransform*>(Data)->IsRotationNormalized()))){Error=TEXT("Invalid nested math snapshot value");return false;}
            return true;
        }
        bool Begin(FProperty* P,const uint8*,int32 Count,int64 Size){
            Elements+=Count;Bytes+=Size;
            if(Elements>4096 || Bytes>16*1024*1024){Error=CastField<FArrayProperty>(P)?TEXT("Struct array snapshot exceeds element/byte limit"):TEXT("Snapshot container limit exceeded");return false;}
            Containers.AddDefaulted();return true;
        }
        void End(FProperty*,const uint8*){Containers.Pop();}
        bool Leaf(FProperty* P,const uint8* Data,DasValueTraversal::ERole Role){
            using ERole=DasValueTraversal::ERole;
            const bool Key=Role==ERole::MapKey, Set=Role==ERole::SetElement;
            if((Key || Set) && IsDasObjectHashProperty(P)){
                auto* O=DasReferenceGet(P,Data);auto& Seen=Containers.Last().Objects;
                if(Seen.Contains(O)){Error=Set?TEXT("Duplicate object set identity"):TEXT("Duplicate map object identity");return false;}
                Seen.Add(O);
                if(O && (!IsValid(O) || (O->IsA<AActor>() && CastChecked<AActor>(O)->IsActorBeingDestroyed()) || !DasReferenceMatches(O,DasReferencePropertyClass(P)))){Error=Set?TEXT("Invalid or destroyed object set element"):TEXT("Invalid or destroyed map object key");return false;}
            }else if(Role==ERole::ArrayElement || Role==ERole::MapValue){
                if(auto* C=DasReferencePropertyClass(P)){
                    auto* O=DasReferenceGet(P,Data);
                    if(O && IsValid(O) && !DasReferenceMatches(O,C)){Error=Role==ERole::ArrayElement?TEXT("Reference array element class/interface mismatch"):TEXT("Reference map value class/interface mismatch");return false;}
                }
            }
            if(auto* Enum=DasEnumPropertyType(P)){
                const int64 Number=DasEnumPropertyNumber(P,Data);const int32 Index=Enum->GetIndexByValue(Number);
                const bool Invalid=Index==INDEX_NONE || Enum->GetNameStringByIndex(Index).EndsWith(TEXT("_MAX"));
                bool Duplicate=false;
                if(Key){auto& Seen=Containers.Last().Enums;Duplicate=Seen.Contains(Number);Seen.Add(Number);}
                if(Invalid || Duplicate){
                    switch(Role){
                    case ERole::ArrayElement:Error=TEXT("Unknown or reserved enum array element");break;
                    case ERole::SetElement:Error=TEXT("Unknown or reserved enum set element");break;
                    case ERole::MapKey:Error=TEXT("Unknown, reserved or duplicate enum map key");break;
                    case ERole::MapValue:Error=TEXT("Unknown or reserved enum map value");break;
                    default:Error=TEXT("Unknown or reserved enum value in struct field: ")+P->GetName();break;
                    }
                    return false;
                }
            }
            return true;
        }
    } Policy{Error};
    return DasValueTraversal::Struct(Value.GetScriptStruct(),Value.GetMemory(),Policy);
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

#include "DasDefaultValue.inl"

bool ParseDasStructDefault(const FString& Path,const FString& Text,FInstancedStruct& Out,FString& Error){
    if(DasContainerProperty(Path) || Text.TrimStart().StartsWith(TEXT("{")))return ParseDasTypedDefault(Path,Text,Out,Error);
    Out.Reset();if(!ValidateDasValueStruct(Path,Error))return false;
    FDasDefaultRoots Roots;auto& Candidate=Roots.Values.AddDefaulted_GetRef();Candidate.InitializeAs(FindObject<UScriptStruct>(nullptr,*Path));
    if(!Text.IsEmpty()){
        if(Text.Len()>1048576){Error=TEXT("Struct default exceeds text limit");return false;}
        FString NativeText;if(!ValidateStructDefaultFields(Candidate.GetScriptStruct(),Text,Error,NativeText))return false;
        struct FDiagnostics: FOutputDevice {bool Seen=false;virtual void Serialize(const TCHAR*,ELogVerbosity::Type,const FName&) override {Seen=true;}} Diagnostics;
        const TCHAR* End=Candidate.GetScriptStruct()->ImportText(*NativeText,Candidate.GetMutableMemory(),nullptr,PPF_None,&Diagnostics,Path,false);
        if(!End || Diagnostics.Seen || !FString(End).TrimStartAndEnd().IsEmpty()){Error=TEXT("Invalid struct default text: ")+Path;return false;}
    }
    if(!ValidateDasStructSnapshot(Candidate,Error))return false;Out=MoveTemp(Candidate);return true;
}
