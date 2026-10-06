#include "DasSchema.h"
#include "StructUtils/InstancedStruct.h"
#include "UObject/Class.h"
#include "UObject/UnrealType.h"
#include "UObject/StrProperty.h"
#include "Misc/OutputDevice.h"
bool ValidateDasValueStruct(const FString& Path,FString& Error){
    auto* Type=FindObject<UScriptStruct>(nullptr,*Path);if(!Path.StartsWith(TEXT("/Script/")) || !Type){Error=TEXT("Expected loaded native struct path");return false;}
    TSet<UScriptStruct*> Active;int32 Leaves=0;TFunction<bool(UScriptStruct*,int32)> Visit;
    Visit=[&](UScriptStruct* S,int32 Depth){if(Depth>16 || Active.Contains(S) || S->GetStructureSize()>1048576 || (S->GetCppStructOps() && S->GetCppStructOps()->HasAddStructReferencedObjects())){Error=TEXT("Unsupported recursive/opaque/native-reference struct");return false;}Active.Add(S);
        for(TFieldIterator<FProperty> It(S);It;++It){auto* P=*It;if(P->ArrayDim!=1){Error=TEXT("Fixed array struct fields unsupported");return false;}if(auto* N=CastField<FStructProperty>(P)){if(!Visit(N->Struct,Depth+1))return false;}
            else {if(!(CastField<FIntProperty>(P) || CastField<FFloatProperty>(P) || CastField<FDoubleProperty>(P) || CastField<FBoolProperty>(P) || CastField<FStrProperty>(P) || CastField<FNameProperty>(P) || CastField<FEnumProperty>(P) || (CastField<FByteProperty>(P) && CastField<FByteProperty>(P)->Enum)) || ++Leaves>1024){Error=TEXT("Struct supports scalar leaves; object/container/other fields unsupported");return false;}}}
        Active.Remove(S);return true;};return Visit(Type,0);
}

bool ValidateDasStructSnapshot(const FInstancedStruct& Value,FString& Error){
    if(!Value.IsValid()){Error=TEXT("Missing struct snapshot");return false;}if(!ValidateDasValueStruct(Value.GetScriptStruct()->GetPathName(),Error))return false;
    TFunction<bool(const UScriptStruct*,const uint8*)> Visit;
    Visit=[&](const UScriptStruct* Type,const uint8* Data){for(TFieldIterator<FProperty> It(Type);It;++It){const auto* P=*It;const uint8* Memory=P->ContainerPtrToValuePtr<uint8>(Data);
        if(auto* S=CastField<FStructProperty>(P)){if(!Visit(S->Struct,Memory))return false;continue;}
        const UEnum* Enum=nullptr;const FNumericProperty* Storage=nullptr;
        if(auto* E=CastField<FEnumProperty>(P)){Enum=E->GetEnum();Storage=E->GetUnderlyingProperty();}else if(auto* B=CastField<FByteProperty>(P)){Enum=B->Enum;Storage=B;}
        if(Enum){int32 Index=Enum->GetIndexByValue(Storage->GetSignedIntPropertyValue(Memory));if(Index==INDEX_NONE || Enum->GetNameStringByIndex(Index).EndsWith(TEXT("_MAX"))){Error=TEXT("Unknown or reserved enum value in struct field: ")+P->GetName();return false;}}
    }return true;};return Visit(Value.GetScriptStruct(),Value.GetMemory());
}

namespace {
bool ValidateStructDefaultFields(const UScriptStruct* Type,const FString& Text,FString& Error){
    int32 Pos=0;auto Space=[&](){while(Pos<Text.Len() && FChar::IsWhitespace(Text[Pos]))++Pos;};
    TFunction<bool(const UScriptStruct*)> Read;
    Read=[&](const UScriptStruct* S){
        Space();if(Pos>=Text.Len() || Text[Pos++]!=TEXT('('))return false;TSet<FName> Seen;
        for(;;){Space();if(Pos<Text.Len() && Text[Pos]==TEXT(')')){++Pos;return true;}
            int32 Begin=Pos;while(Pos<Text.Len() && (FChar::IsAlnum(Text[Pos]) || Text[Pos]==TEXT('_')))++Pos;
            if(Begin==Pos)return false;FName Name(*Text.Mid(Begin,Pos-Begin));auto* P=S->FindPropertyByName(Name);Space();
            if(!P || Seen.Contains(Name) || Pos>=Text.Len() || Text[Pos++]!=TEXT('='))return false;Seen.Add(Name);Space();
            if(auto* Nested=CastField<FStructProperty>(P)){if(!Read(Nested->Struct))return false;}
            else {
                Begin=Pos;if(Pos<Text.Len() && Text[Pos]==TEXT('"')){++Pos;bool Closed=false;while(Pos<Text.Len()){TCHAR Ch=Text[Pos++];if(Ch==TEXT('\\')){if(Pos>=Text.Len())return false;++Pos;}else if(Ch==TEXT('"')){Closed=true;break;}}if(!Closed)return false;}
                else {while(Pos<Text.Len() && Text[Pos]!=TEXT(',') && Text[Pos]!=TEXT(')')){if(Text[Pos]==TEXT('(') || Text[Pos]==TEXT('"'))return false;++Pos;}if(Text.Mid(Begin,Pos-Begin).TrimStartAndEnd().IsEmpty())return false;}
            }
            Space();if(Pos>=Text.Len())return false;if(Text[Pos]==TEXT(')')){++Pos;return true;}if(Text[Pos++]!=TEXT(','))return false;
            Space();if(Pos>=Text.Len() || Text[Pos]==TEXT(')'))return false;
        }
    };
    const bool Valid=Read(Type);Space();if(!Valid || Pos!=Text.Len()){Error=TEXT("Malformed, unknown or duplicate struct default field: ")+Type->GetPathName();return false;}return true;
}
}

bool ParseDasStructDefault(const FString& Path,const FString& Text,FInstancedStruct& Out,FString& Error){
    Out.Reset();if(!ValidateDasValueStruct(Path,Error))return false;
    FInstancedStruct Candidate;Candidate.InitializeAs(FindObject<UScriptStruct>(nullptr,*Path));
    if(!Text.IsEmpty()){
        if(Text.Len()>1048576){Error=TEXT("Struct default exceeds text limit");return false;}
        if(!ValidateStructDefaultFields(Candidate.GetScriptStruct(),Text,Error))return false;
        struct FDiagnostics: FOutputDevice {bool Seen=false;virtual void Serialize(const TCHAR*,ELogVerbosity::Type,const FName&) override {Seen=true;}} Diagnostics;
        const TCHAR* End=Candidate.GetScriptStruct()->ImportText(*Text,Candidate.GetMutableMemory(),nullptr,PPF_None,&Diagnostics,Path,false);
        if(!End || Diagnostics.Seen || !FString(End).TrimStartAndEnd().IsEmpty()){Error=TEXT("Invalid struct default text: ")+Path;return false;}
    }
    if(!ValidateDasStructSnapshot(Candidate,Error))return false;Out=MoveTemp(Candidate);return true;
}
