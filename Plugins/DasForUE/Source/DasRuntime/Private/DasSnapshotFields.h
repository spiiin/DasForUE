#pragma once
#include "DasSchema.h"
#include "UObject/UnrealType.h"
#include "UObject/FieldPath.h"

namespace DasSnapshotFields {
template<class Byte> struct TField {FProperty* Property;Byte* Memory;};

// No instance addresses or strong type references. Resolve lazily so a failure
// later in a path never hides an earlier array bounds error.
struct FPart {
    FString Name;
    int32 Index=INDEX_NONE;
    TWeakObjectPtr<const UScriptStruct> Type;
    FFieldPath Field;
};
struct FPath {
    TArray<FPart> Parts;
    explicit FPath(const FString& Path){
        if(Path.IsEmpty())return;
        TArray<FString> Names;Path.ParseIntoArray(Names,TEXT("."),false);
        for(auto& Name:Names){
            FPart& Part=Parts.AddDefaulted_GetRef();Part.Name=MoveTemp(Name);
            const auto& N=Part.Name;
            bool Valid=!N.IsEmpty() && N.Len()<=4 && (N.Len()==1 || N[0]!=TEXT('0'));
            for(TCHAR C:N)Valid &= C>=TEXT('0') && C<=TEXT('9');
            if(Valid)Part.Index=FCString::Atoi(*N);
        }
    }
};

// Byte is uint8 or const uint8. A read path never casts away snapshot constness.
template<class Byte>
bool Resolve(const UScriptStruct* Type,Byte* Memory,FPath& Path,TField<Byte>& Out,FString& Error,uint64* Lookups=nullptr){
    auto& Parts=Path.Parts;
    if(Parts.IsEmpty()){Error=TEXT("empty struct field path");return false;}
    for(int32 I=0;I<Parts.Num();++I){
        auto& Part=Parts[I];
        if(Part.Type.Get()!=Type || Part.Field.IsPathToFieldEmpty() || Part.Field.IsStale()){
            Part.Field.Reset();Part.Type=Type;
            if(Lookups)++*Lookups;
            // FString paths are untrusted script input; FName asserts on an
            // overlong segment instead of returning a missing property.
            if(Part.Name.Len()<NAME_SIZE)if(auto* Found=FindDasStructField(Type,Part.Name))Part.Field.Generate(Found);
        }
        auto* P=static_cast<FProperty*>(Part.Field.GetTyped(FProperty::StaticClass()));
        if(!P){Error=TEXT("struct field does not exist");return false;}
        Memory=P->template ContainerPtrToValuePtr<uint8>(Memory);
        if(I==Parts.Num()-1){Out={P,Memory};return true;}
        if(auto* Array=CastField<FArrayProperty>(P)){
            if(++I>=Parts.Num()){Error=TEXT("array path requires index");return false;}
            const int32 N=Parts[I].Index;
            if(N==INDEX_NONE){Error=TEXT("invalid struct array index");return false;}
            FScriptArrayHelper Elements(Array,Memory);
            if(!Elements.IsValidIndex(N)){Error=TEXT("struct array index out of range");return false;}
            Memory=Elements.GetRawPtr(N);
            if(I==Parts.Num()-1){Out={Array->Inner,Memory};return true;}
            auto* Element=CastField<FStructProperty>(Array->Inner);
            if(!Element){Error=TEXT("array element is not struct");return false;}
            Type=Element->Struct;continue;
        }
        auto* Struct=CastField<FStructProperty>(P);
        if(!Struct){Error=TEXT("path crosses non-struct field");return false;}
        Type=Struct->Struct;
    }
    Error=TEXT("invalid field path");return false;
}

template<class Byte>
bool Find(const UScriptStruct* Type,Byte* Memory,const FString& Path,TField<Byte>& Out,FString& Error){
    FPath Prepared(Path);return Resolve(Type,Memory,Prepared,Out,Error);
}

class FPathCache {
    struct FKey {
        TWeakObjectPtr<const UScriptStruct> Type;
        FString Path;
        bool operator==(const FKey& Other)const{return Type==Other.Type && Path.Equals(Other.Path,ESearchCase::CaseSensitive);}
        friend uint32 GetTypeHash(const FKey& Key){return HashCombine(GetTypeHash(Key.Type),GetTypeHash(Key.Path));}
    };
    TMap<FKey,FPath> Paths;
    uint64 Builds=0,Lookups=0;
public:
    static constexpr int32 MaxPaths=128,MaxPathLength=1024;
    template<class Byte>
    bool Find(const UScriptStruct* Type,Byte* Memory,const FString& Path,TField<Byte>& Out,FString& Error){
        FKey Key{Type,Path};
        if(auto* Prepared=Paths.Find(Key))return Resolve(Type,Memory,*Prepared,Out,Error,&Lookups);
        ++Builds;
        if(Path.Len()>MaxPathLength){FPath Scratch(Path);return Resolve(Type,Memory,Scratch,Out,Error,&Lookups);}
        for(auto It=Paths.CreateIterator();It;++It)if(!It.Key().Type.IsValid())It.RemoveCurrent();
        if(Paths.Num()>=MaxPaths)Paths.Reset();
        auto& Prepared=Paths.Add(MoveTemp(Key),FPath(Path));
        return Resolve(Type,Memory,Prepared,Out,Error,&Lookups);
    }
    int32 Num()const{return Paths.Num();}
    uint64 BuildCount()const{return Builds;}
    uint64 LookupCount()const{return Lookups;}
};
}
