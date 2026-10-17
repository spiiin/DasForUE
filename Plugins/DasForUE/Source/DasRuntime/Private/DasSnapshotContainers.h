#pragma once
#include "DasSnapshotFields.h"
#include "DasPropertyTypes.h"
#include "DasContainerTypes.h"
#include "UObject/StrProperty.h"
#include <type_traits>

// Views borrow storage only for a synchronous operation. Mutable views require
// detached storage: callers publish the owned snapshot only after success.
namespace DasSnapshotContainers {
using EKind=EDasTypeKind;
bool CheckProperty(FProperty* Property,void* Memory,FString& Error);
inline bool Matches(const FProperty* P,EKind Kind){return Kind==EKind::Invalid || DasPropertyKind(P)==Kind;}
inline bool HashMatches(const FProperty* P,EKind Kind){
    if(!Matches(P,Kind))return false;
    if(Kind==EKind::Object)return IsDasObjectHashProperty(P);
    if(Kind==EKind::Enum)return P->GetElementSize()==1;
    return true;
}
template<class Helper> int32 SparseIndex(const Helper& Items,int32 Index){
    if(Index<0 || Index>=Items.Num())return INDEX_NONE;
    for(int32 I=0;I<Items.GetMaxIndex();++I)if(Items.IsValidIndex(I) && Index--==0)return I;
    return INDEX_NONE;
}
inline bool ExactStringKey(const FProperty* P,const void* Stored,const void* Key){
    auto* String=CastField<FStrProperty>(P);
    return !String || String->GetPropertyValue(Stored).Equals(String->GetPropertyValue(Key),ESearchCase::CaseSensitive);
}
template<class Helper,class KeyAt> void PrepareOrder(const Helper& Items,FProperty* Key,KeyAt Address,TArray<int32>& Out){
    Out.Reset(Items.Num());
    for(int32 I=0;I<Items.GetMaxIndex();++I)if(Items.IsValidIndex(I))Out.Add(I);
    // Match the existing sorted int/string interface. Other kinds keep sparse order.
    if(DasPropertyKind(Key)==EKind::Int)Out.Sort([&](int32 A,int32 B){return *static_cast<const int32*>(Address(A))<*static_cast<const int32*>(Address(B));});
    else if(DasPropertyKind(Key)==EKind::String)Out.Sort([&](int32 A,int32 B){return *static_cast<const FString*>(Address(A))<*static_cast<const FString*>(Address(B));});
}
template<class Byte> struct TMapView {
    static constexpr bool IsMap=true;
    FMapProperty* Property=nullptr;Byte* Memory=nullptr;
    int32 Num()const{return FScriptMapHelper(Property,Memory).Num();}
    void PrepareKeyOrder(TArray<int32>& Out)const{FScriptMapHelper Items(Property,Memory);PrepareOrder(Items,Property->KeyProp,[&](int32 I)->const void*{return Items.GetKeyPtr(I);},Out);}
    bool KeyAtSlot(int32 Slot,DasSnapshotFields::TField<const uint8>& Out,FString& Error)const{
        FScriptMapHelper Items(Property,Memory);if(!Items.IsValidIndex(Slot)){Error=TEXT("map key index out of range");return false;}
        Out={Property->KeyProp,Items.GetKeyPtr(Slot)};return true;
    }
    bool Find(const void* Key,DasSnapshotFields::TField<const uint8>& Out,FString& Error)const{
        FScriptMapHelper Items(Property,Memory);const int32 I=Items.FindMapIndexWithKey(Key);
        if(I==INDEX_NONE){Error=TEXT("snapshot map key not found");return false;}
        Out={Property->ValueProp,Items.GetValuePtr(I)};return true;
    }
    bool KeyAt(int32 Index,DasSnapshotFields::TField<const uint8>& Out,FString& Error)const{
        FScriptMapHelper Items(Property,Memory);const int32 I=SparseIndex(Items,Index);
        if(I==INDEX_NONE){Error=TEXT("map key index out of range");return false;}
        Out={Property->KeyProp,Items.GetKeyPtr(I)};return true;
    }
    template<class Visit> void ForEachKey(Visit Visitor)const{
        FScriptMapHelper Items(Property,Memory);
        for(int32 I=0;I<Items.GetMaxIndex();++I)if(Items.IsValidIndex(I))Visitor(Property->KeyProp,static_cast<const void*>(Items.GetKeyPtr(I)));
    }
    template<class Write> bool Upsert(const void* Key,Write Writer,FString& Error){
        static_assert(!std::is_const_v<Byte>,"Cannot modify a borrowed const snapshot");
        FScriptMapHelper Items(Property,Memory);int32 I=Items.FindMapIndexWithKey(Key);
        if(I==INDEX_NONE){
            if(Items.Num()>=4096){Error=TEXT("snapshot map size limit");return false;}
            I=Items.AddDefaultValue_Invalid_NeedsRehash();Property->KeyProp->CopyCompleteValue(Items.GetKeyPtr(I),Key);
        }else if(!ExactStringKey(Property->KeyProp,Items.GetKeyPtr(I),Key)){Error=TEXT("string map key collision under Unreal equality");return false;}
        const bool Written=Writer(Property->ValueProp,Items.GetValuePtr(I),Error);
        Items.Rehash();return Written;
    }
};
template<class Byte> bool Map(DasSnapshotFields::TField<Byte> Field,EKind Key,EKind Value,TMapView<Byte>& Out,FString& Error){
    auto* P=CastField<FMapProperty>(Field.Property);
    if(!P || !HashMatches(P->KeyProp,Key) || !Matches(P->ValueProp,Value)){Error=TEXT("snapshot map key/value type mismatch");return false;}
    Out={P,Field.Memory};return true;
}
template<class Byte> struct TSetView {
    static constexpr bool IsMap=false;
    FSetProperty* Property=nullptr;Byte* Memory=nullptr;
    int32 Num()const{return FScriptSetHelper(Property,Memory).Num();}
    void PrepareKeyOrder(TArray<int32>& Out)const{FScriptSetHelper Items(Property,Memory);PrepareOrder(Items,Property->ElementProp,[&](int32 I)->const void*{return Items.GetElementPtr(I);},Out);}
    bool KeyAtSlot(int32 Slot,DasSnapshotFields::TField<const uint8>& Out,FString& Error)const{
        FScriptSetHelper Items(Property,Memory);if(!Items.IsValidIndex(Slot)){Error=TEXT("set element index out of range");return false;}
        Out={Property->ElementProp,Items.GetElementPtr(Slot)};return true;
    }
    bool Contains(const void* Key)const{return FScriptSetHelper(Property,Memory).FindElementIndex(Key)!=INDEX_NONE;}
    bool ElementAt(int32 Index,DasSnapshotFields::TField<const uint8>& Out,FString& Error)const{
        FScriptSetHelper Items(Property,Memory);const int32 I=SparseIndex(Items,Index);
        if(I==INDEX_NONE){Error=TEXT("set element index out of range");return false;}
        Out={Property->ElementProp,Items.GetElementPtr(I)};return true;
    }
    template<class Visit> void ForEachKey(Visit Visitor)const{
        FScriptSetHelper Items(Property,Memory);
        for(int32 I=0;I<Items.GetMaxIndex();++I)if(Items.IsValidIndex(I))Visitor(Property->ElementProp,static_cast<const void*>(Items.GetElementPtr(I)));
    }
    bool Set(const void* Key,bool Present,FString& Error){
        static_assert(!std::is_const_v<Byte>,"Cannot modify a borrowed const snapshot");
        FScriptSetHelper Items(Property,Memory);const int32 I=Items.FindElementIndex(Key);
        if(!Present){if(I!=INDEX_NONE)Items.RemoveAt(I);return true;}
        if(I==INDEX_NONE && Items.Num()>=4096){Error=TEXT("snapshot set size limit");return false;}
        if(I!=INDEX_NONE && !ExactStringKey(Property->ElementProp,Items.GetElementPtr(I),Key)){Error=TEXT("string set key collision under Unreal equality");return false;}
        Items.AddElement(Key);return true;
    }
};
template<class Byte> bool Set(DasSnapshotFields::TField<Byte> Field,EKind Element,TSetView<Byte>& Out,FString& Error){
    auto* P=CastField<FSetProperty>(Field.Property);
    if(!P || !HashMatches(P->ElementProp,Element)){Error=TEXT("snapshot set element type mismatch");return false;}
    Out={P,Field.Memory};return true;
}
template<class Byte> struct TArrayView {
    FArrayProperty* Property=nullptr;Byte* Memory=nullptr;
    int32 Num()const{return FScriptArrayHelper(Property,Memory).Num();}
    bool ElementAt(int32 Index,DasSnapshotFields::TField<Byte>& Out,FString& Error)const{
        FScriptArrayHelper Items(Property,Memory);
        if(!Items.IsValidIndex(Index)){Error=TEXT("struct array index out of range");return false;}
        Out={Property->Inner,Items.GetRawPtr(Index)};return true;
    }
    bool Resize(int32 Count,FString& Error){
        static_assert(!std::is_const_v<Byte>,"Cannot modify a borrowed const snapshot");
        if(Count<0 || Count>4096 || int64(Count)*Property->Inner->GetElementSize()>16*1024*1024){Error=TEXT("struct array size out of range");return false;}
        UEnum* Enum=DasEnumPropertyType(Property->Inner);int64 Default=0;
        if(Enum){int32 First=INDEX_NONE;for(int32 I=0;I<Enum->NumEnums();++I)if(!Enum->GetNameStringByIndex(I).EndsWith(TEXT("_MAX"))){First=I;break;}
            if(First==INDEX_NONE){Error=TEXT("Enum array has no usable member");return false;}Default=Enum->GetValueByIndex(First);}
        FScriptArrayHelper Items(Property,Memory);const int32 Old=Items.Num();Items.Resize(Count);
        for(int32 I=Old;Enum && I<Count;++I)if(!SetDasEnumPropertyNumber(Property->Inner,Items.GetRawPtr(I),int32(Default))){Error=TEXT("Enum array default is invalid");return false;}
        return true;
    }
};
template<class Byte> bool Array(DasSnapshotFields::TField<Byte> Field,TArrayView<Byte>& Out,FString& Error){
    auto* P=CastField<FArrayProperty>(Field.Property);if(!P){Error=TEXT("expected snapshot array");return false;}
    Out={P,Field.Memory};return true;
}
// Only immutable snapshot identities may be used here. Slots never root a
// snapshot/object, nor retain native pointers across calls or Unreal GC.
class FKeyOrderCache {
    struct FKey {
        uint64 Token;FString Path;bool Map;
        bool operator==(const FKey& Other)const{return Token==Other.Token && Map==Other.Map && Path.Equals(Other.Path,ESearchCase::CaseSensitive);}
        friend uint32 GetTypeHash(const FKey& K){return HashCombine(GetTypeHash(K.Token),HashCombine(GetTypeHash(K.Path),GetTypeHash(K.Map)));}
    };
    TMap<FKey,TArray<int32>> Orders;TArray<int32> Scratch;int32 Slots=0;uint64 Builds=0;
public:
    static constexpr int32 MaxOrders=128,MaxSlots=65536,MaxPathLength=1024;
    template<class View> const TArray<int32>& Get(uint64 Token,const FString& Path,const View& Values){
        FKey Key{Token,Path,View::IsMap};
        if(const auto* Existing=Orders.Find(Key))return *Existing;
        ++Builds;
        if(Path.Len()>MaxPathLength){Values.PrepareKeyOrder(Scratch);return Scratch;}
        TArray<int32> Order;Values.PrepareKeyOrder(Order);
        if(Order.Num()>MaxSlots){Scratch=MoveTemp(Order);return Scratch;}
        if(Orders.Num()>=MaxOrders || Slots+Order.Num()>MaxSlots){Orders.Reset();Slots=0;}
        Slots+=Order.Num();return Orders.Add(MoveTemp(Key),MoveTemp(Order));
    }
    template<class Live> void Prune(Live IsLive){for(auto It=Orders.CreateIterator();It;++It)if(!IsLive(It.Key().Token)){Slots-=It.Value().Num();It.RemoveCurrent();}}
    int32 Num()const{return Orders.Num();}
    int32 SlotCount()const{return Slots;}
    uint64 BuildCount()const{return Builds;}
};
}
