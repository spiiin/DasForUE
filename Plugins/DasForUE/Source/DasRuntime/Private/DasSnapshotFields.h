#pragma once
#include "DasSchema.h"
#include "UObject/UnrealType.h"

namespace DasSnapshotFields {
template<class Byte> struct TField {FProperty* Property;Byte* Memory;};

// Byte is uint8 or const uint8. A read path never casts away snapshot constness.
template<class Byte>
bool Find(const UScriptStruct* Type,Byte* Memory,const FString& Path,TField<Byte>& Out,FString& Error){
    TArray<FString> Parts;Path.ParseIntoArray(Parts,TEXT("."),false);
    if(Parts.IsEmpty()){Error=TEXT("empty struct field path");return false;}
    for(int32 I=0;I<Parts.Num();++I){
        auto* P=FindDasStructField(Type,Parts[I]);
        if(!P){Error=TEXT("struct field does not exist");return false;}
        Memory=P->template ContainerPtrToValuePtr<uint8>(Memory);
        if(I==Parts.Num()-1){Out={P,Memory};return true;}
        if(auto* Array=CastField<FArrayProperty>(P)){
            if(++I>=Parts.Num()){Error=TEXT("array path requires index");return false;}
            const FString& Index=Parts[I];
            if(Index.IsEmpty() || Index.Len()>4 || (Index.Len()>1 && Index[0]==TEXT('0'))){Error=TEXT("invalid struct array index");return false;}
            for(TCHAR Digit:Index)if(Digit<TEXT('0') || Digit>TEXT('9')){Error=TEXT("invalid struct array index");return false;}
            const int32 N=FCString::Atoi(*Index);FScriptArrayHelper Elements(Array,Memory);
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
}
