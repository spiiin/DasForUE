#include "UObject/TextProperty.h"
#include "DasDefinedEnum.h"
#include "DasReflectedTypes.h"
#include "DasContainerTypes.h"
#include "DasSoftReferences.h"
#include "DasAuthoring.h"
#include "DasPropertyTypes.h"
#include "DasContainerRegistry.h"
#include "UObject/UnrealType.h"

#include "DasSchema.h"
#include "StructUtils/UserDefinedStruct.h"
#include "UObject/Package.h"

bool IsDasObjectHashProperty(const FProperty* P)
{
    auto* Object = CastField<FObjectProperty>(P);
    return Object && !CastField<FClassProperty>(P) && Object->PropertyClass &&
           !Object->PropertyClass->HasAnyClassFlags(CLASS_Interface);
}

namespace
{

FDasDeclaredType CarrierLeaf(const TCHAR* Kind, const FString& Path = FString(), bool Strong = false)
{
    FDasDeclaredType T;
    T.Kind = Kind;
    T.TypePath = Path;
    if (T.GetKind() == EDasTypeKind::Enum)
        T.EnumStorage = Strong ? EDasEnumStorage::Enum8 : EDasEnumStorage::Byte;
    return T;
}
FDasDeclaredType CarrierField(const FProperty* Property)
{
    FDasDeclaredType T;
    FString Error;
    if (!DescribeDasProperty(Property, T, Error))
        T.Kind.Reset();
    return T;
}
FDasDeclaredType CarrierShape(const TCHAR* Kind, const FDasDeclaredType& First,
                              const FDasDeclaredType& Second = FDasDeclaredType())
{
    FDasDeclaredType T;
    T.Kind = Kind;
    if (T.GetKind() == EDasTypeKind::Map)
    {
        T.Key = MakeShared<FDasDeclaredType>(First);
        T.Value = MakeShared<FDasDeclaredType>(Second);
    }
    else
        T.Element = MakeShared<FDasDeclaredType>(First);
    return T;
}
// Native UHT carriers are registered once. Infer their shape from Values so
// declarations and reflection cannot acquire separate primitive-type tables.
struct FStaticCarrierEntry
{
    UScriptStruct* Struct;
    FProperty* Values;
    FString Path, StorageIdentity;
};
const TArray<FStaticCarrierEntry>& StaticCarriers()
{
    static const TArray<FStaticCarrierEntry> Entries = []()
    {
        TArray<FStaticCarrierEntry> Result;
        auto Add = [&](UScriptStruct* S)
        {
            auto* Values = S->FindPropertyByName(TEXT("Values"));
            FDasDeclaredType Type;
            FString Error;
            const bool bValid = DescribeDasProperty(Values, Type, Error);
            checkf(bValid, TEXT("Invalid static carrier %s: %s"), *S->GetPathName(), *Error);
            if (!bValid)
                return;
            Result.Add({S, Values, S->GetPathName(), Type.StorageIdentity()});
        };
        Add(FDasArrayByteCarrier::StaticStruct());
        Add(FDasArrayTextCarrier::StaticStruct());
        Add(FDasSetByteCarrier::StaticStruct());
        Add(FDasMapIntByteCarrier::StaticStruct());
        Add(FDasMapIntTextCarrier::StaticStruct());
        Add(FDasMapStringByteCarrier::StaticStruct());
        Add(FDasMapStringTextCarrier::StaticStruct());
        Add(FDasSetNameCarrier::StaticStruct());
        Add(FDasArrayIntCarrier::StaticStruct());
        Add(FDasArrayFloatCarrier::StaticStruct());
        Add(FDasArrayDoubleCarrier::StaticStruct());
        Add(FDasArrayBoolCarrier::StaticStruct());
        Add(FDasArrayStringCarrier::StaticStruct());
        Add(FDasArrayNameCarrier::StaticStruct());
        Add(FDasSetIntCarrier::StaticStruct());
        Add(FDasSetStringCarrier::StaticStruct());
        Add(FDasMapIntIntCarrier::StaticStruct());
        Add(FDasMapIntFloatCarrier::StaticStruct());
        Add(FDasMapIntDoubleCarrier::StaticStruct());
        Add(FDasMapIntBoolCarrier::StaticStruct());
        Add(FDasMapIntStringCarrier::StaticStruct());
        Add(FDasMapIntNameCarrier::StaticStruct());
        Add(FDasMapStringIntCarrier::StaticStruct());
        Add(FDasMapStringFloatCarrier::StaticStruct());
        Add(FDasMapStringDoubleCarrier::StaticStruct());
        Add(FDasMapStringBoolCarrier::StaticStruct());
        Add(FDasMapStringStringCarrier::StaticStruct());
        Add(FDasMapStringNameCarrier::StaticStruct());
        return Result;
    }();
    return Entries;
}

FDasContainerRegistry CarrierRegistry;
UScriptStruct* RegisteredCarrier(const FString& Path, const FDasDeclaredType& Type)
{
    FString Error;
    return CarrierRegistry.GetOrCreate(Path, Type, Error);
}


const FString StructArrayPrefix = TEXT("/Script/DasRuntime.DasStructArray_");

FString StructArrayPath(UScriptStruct* Element)
{
    const FTCHARToUTF8 Text(*Element->GetPathName());
    const TCHAR* Digits = TEXT("0123456789ABCDEF");
    FString Path = StructArrayPrefix;
    for (int32 I = 0; I < Text.Length(); ++I)
    {
        const uint8 Byte = uint8(Text.Get()[I]);
        Path.AppendChar(Digits[Byte >> 4]);
        Path.AppendChar(Digits[Byte & 15]);
    }
    return Path;
}
UScriptStruct* StructArrayCarrier(UScriptStruct* Element)
{
    if (!IsInGameThread() || !IsValid(Element) ||
        (!Element->GetPathName().StartsWith(TEXT("/Script/")) && (!Element->IsA<UUserDefinedStruct>() ||
                                                                  !Element->GetPathName().StartsWith(TEXT("/Game/")))))
        return nullptr;
    FString Error;
    if (!ValidateDasValueStruct(Element->GetPathName(), Error))
        return nullptr;
    const FString Path = StructArrayPath(Element);
    if (Path.Len() >= NAME_SIZE)
        return nullptr;
    return RegisteredCarrier(Path, CarrierShape(TEXT("array"), CarrierLeaf(TEXT("struct"), Element->GetPathName())));
}
UScriptStruct* ResolveStructArrayCarrier(const FString& Path)
{
    if (!Path.StartsWith(StructArrayPrefix) || Path.Len() >= NAME_SIZE)
        return nullptr;
    const FString Hex = Path.RightChop(StructArrayPrefix.Len());
    if (Hex.IsEmpty() || Hex.Len() % 2)
        return nullptr;
    TArray<ANSICHAR> Bytes;
    auto Digit = [](TCHAR C)
    {
        return C >= '0' && C <= '9' ? int32(C - '0') : C >= 'A' && C <= 'F' ? int32(C - 'A' + 10) : -1;
    };
    for (int32 I = 0; I < Hex.Len(); I += 2)
    {
        const int32 High = Digit(Hex[I]), Low = Digit(Hex[I + 1]);
        if (High < 0 || Low < 0 || !(High || Low))
            return nullptr;
        Bytes.Add(ANSICHAR((High << 4) | Low));
    }
    Bytes.Add(0);
    const FString ElementPath = UTF8_TO_TCHAR(Bytes.GetData());
    if (!ElementPath.StartsWith(TEXT("/Game/")) && !ElementPath.StartsWith(TEXT("/Script/")))
        return nullptr;
    auto* Element = FindObject<UScriptStruct>(nullptr, *ElementPath);
    if (!Element)
        Element = LoadObject<UScriptStruct>(nullptr, *ElementPath, nullptr, LOAD_NoWarn);
    auto* Carrier = StructArrayCarrier(Element);
    return Carrier && Carrier->GetPathName() == Path ? Carrier : nullptr;
}

const FString StructMapPrefix = TEXT("/Script/DasRuntime.DasStructMap_");
UScriptStruct* StructMapCarrier(UScriptStruct* Element, bool StringKey)
{
    if (!IsInGameThread() || !IsValid(Element) ||
        (!Element->GetPathName().StartsWith(TEXT("/Script/")) && (!Element->IsA<UUserDefinedStruct>() ||
                                                                  !Element->GetPathName().StartsWith(TEXT("/Game/")))))
        return nullptr;
    FString Error;
    if (!ValidateDasValueStruct(Element->GetPathName(), Error))
        return nullptr;
    const FTCHARToUTF8 Text(*Element->GetPathName());
    const TCHAR* Digits = TEXT("0123456789ABCDEF");
    FString Path = StructMapPrefix + (StringKey ? TEXT("S_"): TEXT("I_"));
    for (int32 I = 0; I < Text.Length(); ++I)
    {
        const uint8 Byte = uint8(Text.Get()[I]);
        Path.AppendChar(Digits[Byte >> 4]);
        Path.AppendChar(Digits[Byte & 15]);
    }
    if (Path.Len() >= NAME_SIZE)
        return nullptr;
    return RegisteredCarrier(
        Path, CarrierShape(TEXT("map"), CarrierLeaf(StringKey ? TEXT("string"): TEXT("int")),
                                                    CarrierLeaf(TEXT("struct"), Element->GetPathName())));
}
UScriptStruct* ResolveStructMapCarrier(const FString& Path)
{
    if (!Path.StartsWith(StructMapPrefix) || Path.Len() >= NAME_SIZE)
        return nullptr;
    FString Encoded = Path.RightChop(StructMapPrefix.Len());
    const bool StringKey = Encoded.StartsWith(TEXT("S_"));
    if (!StringKey && !Encoded.StartsWith(TEXT("I_")))
        return nullptr;
    const FString Hex = Encoded.RightChop(2);
    if (Hex.IsEmpty() || Hex.Len() % 2)
        return nullptr;
    auto Digit = [](TCHAR C)
    {
        return C >= '0' && C <= '9' ? int32(C - '0') : C >= 'A' && C <= 'F' ? int32(C - 'A' + 10) : -1;
    };
    TArray<ANSICHAR> Bytes;
    for (int32 I = 0; I < Hex.Len(); I += 2)
    {
        const int32 H = Digit(Hex[I]), L = Digit(Hex[I + 1]);
        if (H < 0 || L < 0 || !(H || L))
            return nullptr;
        Bytes.Add(ANSICHAR((H << 4) | L));
    }
    Bytes.Add(0);
    const FString ElementPath = UTF8_TO_TCHAR(Bytes.GetData());
    if (!ElementPath.StartsWith(TEXT("/Game/")) && !ElementPath.StartsWith(TEXT("/Script/")))
        return nullptr;
    auto* Element = FindObject<UScriptStruct>(nullptr, *ElementPath);
    if (!Element)
        Element = LoadObject<UScriptStruct>(nullptr, *ElementPath, nullptr, LOAD_NoWarn);
    auto* S = StructMapCarrier(Element, StringKey);
    return S && S->GetPathName() == Path ? S : nullptr;
}


const FString ReflectedMapPrefix = TEXT("/Script/DasRuntime.DasReflectedMap_");
UScriptStruct* ReflectedMapCarrier(UObject* Element, bool StringKey, bool Strong = false)
{
    if (!IsInGameThread() || !IsValid(Element))
        return nullptr;
    auto* Enum = Cast<UEnum>(Element);
    auto* Authored = Cast<UDasDefinedEnum>(Element);
    auto* Class = Cast<UClass>(Element);
    if (Enum    ? (!Element->GetPathName().StartsWith(TEXT("/Script/")) &&
                                                   (!Authored || !Element->GetPathName().StartsWith(TEXT("/Game/"))))
                : !Class || !Element->GetPathName().StartsWith(TEXT("/Script/")))
        return nullptr;
    FString Path = ReflectedMapPrefix + (StringKey ? TEXT("S_"): TEXT("I_")) +
                                         (Enum ? (Strong ? TEXT("N_"): TEXT("E_")) : TEXT("C_"));
    const FTCHARToUTF8 Text(*Element->GetPathName());
    const TCHAR* Digits = TEXT("0123456789ABCDEF");
    for (int32 I = 0; I < Text.Length(); ++I)
    {
        const uint8 B = uint8(Text.Get()[I]);
        Path.AppendChar(Digits[B >> 4]);
        Path.AppendChar(Digits[B & 15]);
    }
    if (Path.Len() >= NAME_SIZE)
        return nullptr;
    return RegisteredCarrier(
        Path,
        CarrierShape(
            TEXT("map"), CarrierLeaf(StringKey ? TEXT("string"): TEXT("int")),
                                     CarrierLeaf(Enum ? TEXT("enum"): TEXT("object"), Element->GetPathName(), Strong)));
}
UScriptStruct* ResolveReflectedMapCarrier(const FString& Path)
{
    if (!Path.StartsWith(ReflectedMapPrefix) || Path.Len() >= NAME_SIZE)
        return nullptr;
    const FString Text = Path.RightChop(ReflectedMapPrefix.Len());
    const bool StringKey = Text.StartsWith(TEXT("S_"));
    if (!StringKey && !Text.StartsWith(TEXT("I_")))
        return nullptr;
    const FString Kind = Text.Mid(2, 2);
    if (Kind != TEXT("E_") && Kind != TEXT("N_") && Kind != TEXT("C_"))
        return nullptr;
    const FString Hex = Text.RightChop(4);
    if (Hex.IsEmpty() || Hex.Len() % 2)
        return nullptr;
    TArray<ANSICHAR> Bytes;
    auto Digit = [](TCHAR C)
    {
        return C >= '0' && C <= '9' ? int32(C - '0') : C >= 'A' && C <= 'F' ? int32(C - 'A' + 10) : -1;
    };
    for (int32 I = 0; I < Hex.Len(); I += 2)
    {
        int32 H = Digit(Hex[I]), L = Digit(Hex[I + 1]);
        if (H < 0 || L < 0 || !(H || L))
            return nullptr;
        Bytes.Add(ANSICHAR((H << 4) | L));
    }
    Bytes.Add(0);
    const FString ElementPath = UTF8_TO_TCHAR(Bytes.GetData());
    UObject* Element = nullptr;
    if (Kind == TEXT("E_") || Kind == TEXT("N_"))
    {
        if (!ElementPath.StartsWith(TEXT("/Script/")) && !ElementPath.StartsWith(TEXT("/Game/")))
            return nullptr;
        Element = FindObject<UEnum>(nullptr, *ElementPath);
        if (!Element)
            Element = LoadObject<UEnum>(nullptr, *ElementPath, nullptr, LOAD_NoWarn);
    }
    else
    {
        if (!ElementPath.StartsWith(TEXT("/Script/")))
            return nullptr;
        Element = FindObject<UClass>(nullptr, *ElementPath);
        if (!Element)
            Element = LoadObject<UClass>(nullptr, *ElementPath, nullptr, LOAD_NoWarn);
    }
    auto* S = ReflectedMapCarrier(Element, StringKey, Kind == TEXT("N_"));
    return S && S->GetPathName() == Path ? S : nullptr;
}

const FString StrongEnumArrayPrefix = TEXT("/Script/DasRuntime.DasStrongEnumArray_");
const FString EnumArrayPrefix = TEXT("/Script/DasRuntime.DasEnumArray_");
UScriptStruct* EnumArrayCarrier(UEnum* Element, bool Strong = false)
{
    if (!IsInGameThread() || !IsValid(Element) ||
        (!Element->GetPathName().StartsWith(TEXT("/Script/")) && (!Element->IsA<UDasDefinedEnum>() ||
                                                                  !Element->GetPathName().StartsWith(TEXT("/Game/")))))
        return nullptr;
    const FTCHARToUTF8 Text(*Element->GetPathName());
    const TCHAR* Digits = TEXT("0123456789ABCDEF");
    FString Path = Strong ? StrongEnumArrayPrefix : EnumArrayPrefix;
    for (int32 I = 0; I < Text.Length(); ++I)
    {
        const uint8 Byte = uint8(Text.Get()[I]);
        Path.AppendChar(Digits[Byte >> 4]);
        Path.AppendChar(Digits[Byte & 15]);
    }
    if (Path.Len() >= NAME_SIZE)
        return nullptr;
    return RegisteredCarrier(Path,
                             CarrierShape(TEXT("array"), CarrierLeaf(TEXT("enum"), Element->GetPathName(), Strong)));
}
UScriptStruct* ResolveEnumArrayCarrier(const FString& Path)
{
    const bool Strong = Path.StartsWith(StrongEnumArrayPrefix);
    if ((!Strong && !Path.StartsWith(EnumArrayPrefix)) || Path.Len() >= NAME_SIZE)
        return nullptr;
    const FString Hex = Path.RightChop(Strong ? StrongEnumArrayPrefix.Len() : EnumArrayPrefix.Len());
    if (Hex.IsEmpty() || Hex.Len() % 2)
        return nullptr;
    auto Digit = [](TCHAR C)
    {
        return C >= '0' && C <= '9' ? int32(C - '0') : C >= 'A' && C <= 'F' ? int32(C - 'A' + 10) : -1;
    };
    TArray<ANSICHAR> Bytes;
    for (int32 I = 0; I < Hex.Len(); I += 2)
    {
        const int32 H = Digit(Hex[I]), L = Digit(Hex[I + 1]);
        if (H < 0 || L < 0 || !(H || L))
            return nullptr;
        Bytes.Add(ANSICHAR((H << 4) | L));
    }
    Bytes.Add(0);
    const FString ElementPath = UTF8_TO_TCHAR(Bytes.GetData());
    if (!ElementPath.StartsWith(TEXT("/Game/")) && !ElementPath.StartsWith(TEXT("/Script/")))
        return nullptr;
    auto* Element = FindObject<UEnum>(nullptr, *ElementPath);
    if (!Element)
        Element = LoadObject<UEnum>(nullptr, *ElementPath, nullptr, LOAD_NoWarn);
    auto* S = EnumArrayCarrier(Element, Strong);
    return S && S->GetPathName() == Path ? S : nullptr;
}

// Keep byte enums and enum-class storage distinct, as for native enum arrays.
const FString EnumSetPrefix = TEXT("/Script/DasRuntime.DasEnumSet_");
const FString StrongEnumSetPrefix = TEXT("/Script/DasRuntime.DasStrongEnumSet_");
UScriptStruct* EnumSetCarrier(UEnum* Enum, bool Strong = false)
{
    auto* Base = EnumArrayCarrier(Enum, Strong);
    if (!Base)
        return nullptr;
    auto* Array = CastFieldChecked<FArrayProperty>(Base->FindPropertyByName(TEXT("Values")));
    const FString Path = (Strong ? StrongEnumSetPrefix : EnumSetPrefix) +
                         Base->GetPathName().RightChop(Strong ? StrongEnumArrayPrefix.Len() : EnumArrayPrefix.Len());
    if (Path.Len() >= NAME_SIZE)
        return nullptr;
    return RegisteredCarrier(Path, CarrierShape(TEXT("set"), CarrierField(Array->Inner)));
}
UScriptStruct* ResolveEnumSetCarrier(const FString& Path)
{
    const bool Strong = Path.StartsWith(StrongEnumSetPrefix);
    if (!Strong && !Path.StartsWith(EnumSetPrefix))
        return nullptr;
    auto* Base = ResolveEnumArrayCarrier((Strong ? StrongEnumArrayPrefix : EnumArrayPrefix) +
                                         Path.RightChop(Strong ? StrongEnumSetPrefix.Len() : EnumSetPrefix.Len()));
    auto* Array = Base ? CastField<FArrayProperty>(Base->FindPropertyByName(TEXT("Values"))) : nullptr;
    auto* S = Array ? EnumSetCarrier(DasEnumPropertyType(Array->Inner), Strong) : nullptr;
    return S && S->GetPathName() == Path ? S : nullptr;
}

const FString ReferenceArrayPrefix = TEXT("/Script/DasRuntime.DasReferenceArray_");
UScriptStruct* ReferenceArrayCarrier(UClass* Element)
{
    if (!IsInGameThread() || !IsValid(Element) || !Element->GetPathName().StartsWith(TEXT("/Script/")))
        return nullptr;
    const FTCHARToUTF8 Text(*Element->GetPathName());
    const TCHAR* Digits = TEXT("0123456789ABCDEF");
    FString Path = ReferenceArrayPrefix;
    for (int32 I = 0; I < Text.Length(); ++I)
    {
        const uint8 Byte = uint8(Text.Get()[I]);
        Path.AppendChar(Digits[Byte >> 4]);
        Path.AppendChar(Digits[Byte & 15]);
    }
    if (Path.Len() >= NAME_SIZE)
        return nullptr;
    return RegisteredCarrier(Path, CarrierShape(TEXT("array"), CarrierLeaf(TEXT("object"), Element->GetPathName())));
}
UScriptStruct* ResolveReferenceArrayCarrier(const FString& Path)
{
    if (!Path.StartsWith(ReferenceArrayPrefix) || Path.Len() >= NAME_SIZE)
        return nullptr;
    const FString Hex = Path.RightChop(ReferenceArrayPrefix.Len());
    if (Hex.IsEmpty() || Hex.Len() % 2)
        return nullptr;
    auto Digit = [](TCHAR C)
    {
        return C >= '0' && C <= '9' ? int32(C - '0') : C >= 'A' && C <= 'F' ? int32(C - 'A' + 10) : -1;
    };
    TArray<ANSICHAR> Bytes;
    for (int32 I = 0; I < Hex.Len(); I += 2)
    {
        const int32 H = Digit(Hex[I]), L = Digit(Hex[I + 1]);
        if (H < 0 || L < 0 || !(H || L))
            return nullptr;
        Bytes.Add(ANSICHAR((H << 4) | L));
    }
    Bytes.Add(0);
    const FString ClassPath = UTF8_TO_TCHAR(Bytes.GetData());
    if (!ClassPath.StartsWith(TEXT("/Script/")))
        return nullptr;
    auto* C = FindObject<UClass>(nullptr, *ClassPath);
    if (!C)
        C = LoadObject<UClass>(nullptr, *ClassPath, nullptr, LOAD_NoWarn);
    auto* S = ReferenceArrayCarrier(C);
    return S && S->GetPathName() == Path ? S : nullptr;
}

const FString NameMapPrefix = TEXT("/Script/DasRuntime.DasNameKeyMap_");
UScriptStruct* NameMapFromBase(FMapProperty* Base, const FString& BasePath)
{
    if (!IsInGameThread() || !Base || !Base->KeyProp->IsA<FIntProperty>())
        return nullptr;
    FString Path = NameMapPrefix;
    const FTCHARToUTF8 Text(*BasePath);
    const TCHAR* Digits = TEXT("0123456789ABCDEF");
    for (int32 I = 0; I < Text.Length(); ++I)
    {
        const uint8 B = uint8(Text.Get()[I]);
        Path.AppendChar(Digits[B >> 4]);
        Path.AppendChar(Digits[B & 15]);
    }
    if (Path.Len() >= NAME_SIZE)
        return nullptr;
    return RegisteredCarrier(Path, CarrierShape(TEXT("map"), CarrierLeaf(TEXT("name")), CarrierField(Base->ValueProp)));
}
UScriptStruct* NameMapCarrier(const FDasDeclaredType& Type, bool Strong = false)
{
    FDasDeclaredType Proxy = Type;
    auto Key = MakeShared<FDasDeclaredType>();
    Key->Kind = TEXT("int");
    Proxy.Key = Key;
    auto* Base = Strong && Type.Value &&
                 Type.Value->Kind ==
                     TEXT("enum")? ReflectedMapCarrier(FindObject<UEnum>(nullptr, *Type.Value->TypePath), false, true)
                                 : DasContainerCarrier(Proxy);
    return Base
        ? NameMapFromBase(CastField<FMapProperty>(Base->FindPropertyByName(TEXT("Values"))), Base->GetPathName())
        : nullptr;
}
UScriptStruct* ResolveNameMap(const FString& Path)
{
    if (!Path.StartsWith(NameMapPrefix) || Path.Len() >= NAME_SIZE)
        return nullptr;
    const FString Hex = Path.RightChop(NameMapPrefix.Len());
    if (Hex.IsEmpty() || Hex.Len() % 2)
        return nullptr;
    auto Digit = [](TCHAR C)
    {
        return C >= '0' && C <= '9' ? int32(C - '0') : C >= 'A' && C <= 'F' ? int32(C - 'A' + 10) : -1;
    };
    TArray<ANSICHAR> Bytes;
    for (int32 I = 0; I < Hex.Len(); I += 2)
    {
        const int32 H = Digit(Hex[I]), L = Digit(Hex[I + 1]);
        if (H < 0 || L < 0 || !(H || L))
            return nullptr;
        Bytes.Add(ANSICHAR((H << 4) | L));
    }
    Bytes.Add(0);
    const FString BasePath = UTF8_TO_TCHAR(Bytes.GetData());
    if (BasePath.StartsWith(NameMapPrefix))
        return nullptr;
    auto* S = NameMapFromBase(CastField<FMapProperty>(DasContainerProperty(BasePath)), BasePath);
    return S && S->GetPathName() == Path ? S : nullptr;
}


const FString ObjectSetPrefix = TEXT("/Script/DasRuntime.DasObjectSet_");
const FString ObjectMapPrefix = TEXT("/Script/DasRuntime.DasObjectMap_");
FString ObjectHashEncode(const FString& Text)
{
    const FTCHARToUTF8 Bytes(*Text);
    const TCHAR* Digits = TEXT("0123456789ABCDEF");
    FString Hex;
    for (int32 I = 0; I < Bytes.Length(); ++I)
    {
        uint8 B = uint8(Bytes.Get()[I]);
        Hex.AppendChar(Digits[B >> 4]);
        Hex.AppendChar(Digits[B & 15]);
    }
    return Hex;
}
bool ObjectHashDecode(const FString& Hex, FString& Text)
{
    if (Hex.IsEmpty() || Hex.Len() % 2 || Hex.Len() >= 2 * NAME_SIZE)
        return false;
    auto Digit = [](TCHAR C)
    {
        return C >= '0' && C <= '9' ? int32(C - '0') : C >= 'A' && C <= 'F' ? int32(C - 'A' + 10) : -1;
    };
    TArray<ANSICHAR> Bytes;
    for (int32 I = 0; I < Hex.Len(); I += 2)
    {
        int32 H = Digit(Hex[I]), L = Digit(Hex[I + 1]);
        if (H < 0 || L < 0 || !(H || L))
            return false;
        Bytes.Add(ANSICHAR((H << 4) | L));
    }
    Bytes.Add(0);
    Text = UTF8_TO_TCHAR(Bytes.GetData());
    return ObjectHashEncode(Text) == Hex;
}
bool ObjectHashClass(UClass* Class)
{
    return IsInGameThread() && IsValid(Class) &&
           Class->GetPathName().StartsWith(
               TEXT("/Script/")) &&
               !Class->HasAnyClassFlags(CLASS_Interface | CLASS_Deprecated | CLASS_NewerVersionExists);
}
UScriptStruct* ObjectSetCarrier(UClass* Class)
{
    if (!ObjectHashClass(Class))
        return nullptr;
    const FString Path = ObjectSetPrefix + ObjectHashEncode(Class->GetPathName());
    if (Path.Len() >= NAME_SIZE)
        return nullptr;
    return RegisteredCarrier(Path, CarrierShape(TEXT("set"), CarrierLeaf(TEXT("object"), Class->GetPathName())));
}
UScriptStruct* ObjectMapFromBase(UClass* Class, FMapProperty* Base, const FString& BasePath)
{
    if (!ObjectHashClass(Class) || !Base || !Base->KeyProp->IsA<FIntProperty>())
        return nullptr;
    const FString Path = ObjectMapPrefix + ObjectHashEncode(Class->GetPathName() + TEXT("|")+ BasePath);
    if (Path.Len() >= NAME_SIZE)
        return nullptr;
    return RegisteredCarrier(Path, CarrierShape(TEXT("map"), CarrierLeaf(TEXT("object"), Class->GetPathName()),
                                                                         CarrierField(Base->ValueProp)));
}
UScriptStruct* ObjectMapCarrier(const FDasDeclaredType& Type, int StrongOverride = -1)
{
    if (!Type.Key || !Type.Value)
        return nullptr;
    auto* Class = FindObject<UClass>(nullptr, *Type.Key->TypePath);
    FDasDeclaredType Proxy = Type;
    auto Key = MakeShared<FDasDeclaredType>();
    Key->Kind = TEXT("int");
    Proxy.Key = Key;
    UScriptStruct* Base = nullptr;
    if (Type.Value->Kind == TEXT("enum"))
    {
        auto* E = FindObject<UEnum>(nullptr, *Type.Value->TypePath);
        bool Strong = StrongOverride >= 0
                          ? StrongOverride != 0
                          : E && !E->IsA<UDasDefinedEnum>() && E->GetCppForm() == UEnum::ECppForm::EnumClass;
        Base = ReflectedMapCarrier(E, false, Strong);
    }
    else
        Base = DasContainerCarrier(Proxy);
    return Base ? ObjectMapFromBase(Class, CastField<FMapProperty>(Base->FindPropertyByName(TEXT("Values"))),
                                                                   Base->GetPathName())
                                    : nullptr;
}
UScriptStruct* ResolveObjectHashCarrier(const FString& Path)
{
    const bool Set = Path.StartsWith(ObjectSetPrefix);
    if (!Set && !Path.StartsWith(ObjectMapPrefix))
        return nullptr;
    if (Path.Len() >= NAME_SIZE)
        return nullptr;
    FString Text;
    if (!ObjectHashDecode(Path.RightChop(Set ? ObjectSetPrefix.Len() : ObjectMapPrefix.Len()), Text))
        return nullptr;
    FString ClassPath = Text, BasePath;
    if (!Set && !Text.Split(TEXT("|"), &ClassPath, &BasePath))
        return nullptr;
    if (!ClassPath.StartsWith(TEXT("/Script/")))
        return nullptr;
    auto* Class = FindObject<UClass>(nullptr, *ClassPath);
    if (!Class)
        Class = LoadObject<UClass>(nullptr, *ClassPath, nullptr, LOAD_NoWarn);
    if (!Set && (BasePath.StartsWith(ObjectMapPrefix) || BasePath.StartsWith(ObjectSetPrefix)))
        return nullptr;
    auto* S = Set ? ObjectSetCarrier(Class)
                  : ObjectMapFromBase(Class, CastField<FMapProperty>(DasContainerProperty(BasePath)), BasePath);
    return S && S->GetPathName() == Path ? S : nullptr;
}


// Clone an int-key map's value and a validated enum array's byte/enum key.
const FString EnumMapPrefix = TEXT("/Script/DasRuntime.DasEnumKeyMap_");
UScriptStruct* EnumMapFromBase(UEnum* Enum, bool Strong, FMapProperty* Base, const FString& BasePath)
{
    auto* KeyCarrier = EnumArrayCarrier(Enum, Strong);
    if (!KeyCarrier || !Base || !Base->KeyProp->IsA<FIntProperty>())
        return nullptr;
    auto* Element = CastFieldChecked<FArrayProperty>(KeyCarrier->FindPropertyByName(TEXT("Values")))->Inner;
    const FString Path =
        EnumMapPrefix + (Strong ? TEXT("N_"): TEXT("E_")) + ObjectHashEncode(Enum->GetPathName() + TEXT("|")+ BasePath);
    if (Path.Len() >= NAME_SIZE)
        return nullptr;
    return RegisteredCarrier(Path, CarrierShape(TEXT("map"), CarrierField(Element), CarrierField(Base->ValueProp)));
}
UScriptStruct* EnumMapCarrier(const FDasDeclaredType& Type, int KeyStrong = -1, int ValueStrong = -1)
{
    if (!Type.Key || !Type.Value)
        return nullptr;
    auto* E = FindObject<UEnum>(nullptr, *Type.Key->TypePath);
    if (!E)
        return nullptr;
    bool Strong =
        KeyStrong >= 0 ? KeyStrong != 0 : !E->IsA<UDasDefinedEnum>() && E->GetCppForm() == UEnum::ECppForm::EnumClass;
    FDasDeclaredType Proxy = Type;
    auto Key = MakeShared<FDasDeclaredType>();
    Key->Kind = TEXT("int");
    Proxy.Key = Key;
    UScriptStruct* Base = nullptr;
    if (Type.Value->Kind == TEXT("enum"))
    {
        auto* V = FindObject<UEnum>(nullptr, *Type.Value->TypePath);
        bool VS = ValueStrong >= 0 ? ValueStrong != 0
                                   : V && !V->IsA<UDasDefinedEnum>() && V->GetCppForm() == UEnum::ECppForm::EnumClass;
        Base = ReflectedMapCarrier(V, false, VS);
    }
    else
        Base = DasContainerCarrier(Proxy);
    return Base ? EnumMapFromBase(
                      E, Strong, CastField<FMapProperty>(Base->FindPropertyByName(TEXT("Values"))), Base->GetPathName())
                      : nullptr;
}
UScriptStruct* ResolveEnumMapCarrier(const FString& Path)
{
    if (!Path.StartsWith(EnumMapPrefix) || Path.Len() >= NAME_SIZE)
        return nullptr;
    FString Rest = Path.RightChop(EnumMapPrefix.Len());
    const bool Strong = Rest.StartsWith(TEXT("N_"));
    if (!Strong && !Rest.StartsWith(TEXT("E_")))
        return nullptr;
    FString Text, EnumPath, BasePath;
    if (!ObjectHashDecode(Rest.RightChop(2), Text) ||
        !Text.Split(TEXT("|"), &EnumPath, &BasePath) ||
                    (!EnumPath.StartsWith(TEXT("/Script/")) && !EnumPath.StartsWith(TEXT("/Game/"))) ||
                      BasePath.StartsWith(EnumMapPrefix) || BasePath.StartsWith(ObjectMapPrefix) ||
                      BasePath.StartsWith(NameMapPrefix))
        return nullptr;
    auto* E = FindObject<UEnum>(nullptr, *EnumPath);
    if (!E)
        E = LoadObject<UEnum>(nullptr, *EnumPath, nullptr, LOAD_NoWarn);
    auto* S = EnumMapFromBase(E, Strong, CastField<FMapProperty>(DasContainerProperty(BasePath)), BasePath);
    return S && S->GetPathName() == Path ? S : nullptr;
}


const FString ByteMapPrefix = TEXT("/Script/DasRuntime.DasByteKeyMap_");
UScriptStruct* ByteMapFromBase(FMapProperty* Base, const FString& BasePath)
{
    if (!IsInGameThread() || !Base || !Base->KeyProp->IsA<FIntProperty>())
        return nullptr;
    const FString Path = ByteMapPrefix + ObjectHashEncode(BasePath);
    if (Path.Len() >= NAME_SIZE)
        return nullptr;
    return RegisteredCarrier(Path, CarrierShape(TEXT("map"), CarrierLeaf(TEXT("byte")), CarrierField(Base->ValueProp)));
}
UScriptStruct* ByteMapCarrier(const FDasDeclaredType& Type)
{
    if (!Type.Value)
        return nullptr;
    FDasDeclaredType Proxy = Type;
    auto K = MakeShared<FDasDeclaredType>();
    K->Kind = TEXT("int");
    Proxy.Key = K;
    auto* Base = DasContainerCarrier(Proxy);
    return Base
        ? ByteMapFromBase(CastField<FMapProperty>(Base->FindPropertyByName(TEXT("Values"))), Base->GetPathName())
        : nullptr;
}
UScriptStruct* ResolveByteMap(const FString& Path)
{
    FString BasePath;
    if (Path.Len() >= NAME_SIZE || !ObjectHashDecode(Path.RightChop(ByteMapPrefix.Len()), BasePath) ||
        BasePath.StartsWith(ByteMapPrefix))
        return nullptr;
    auto* S = ByteMapFromBase(CastField<FMapProperty>(DasContainerProperty(BasePath)), BasePath);
    return S && S->GetPathName() == Path ? S : nullptr;
}


}
void ResetDasContainerCarriers()
{
    CarrierRegistry.Reset();
    ResetDasSoftCarriers();
}
UScriptStruct* DasContainerCarrier(const FDasDeclaredType& Type)
{
    if (Type.Kind == TEXT("struct") && DasSoftProperty(Type.TypePath))
        return FindObject<UScriptStruct>(nullptr, *Type.TypePath);
    FString Error;
    if (!ValidateDasPropertyType(Type, EDasPropertyUsage::Container, Error))
        return nullptr;
    const FString Identity = Type.StorageIdentity();
    for (const auto& Entry : StaticCarriers())
        if (Entry.StorageIdentity == Identity)
            return Entry.Struct;
    auto Storage = [](const TSharedPtr<const FDasDeclaredType>& T)
    {
        return !T || T->GetKind() != EDasTypeKind::Enum               ? -1
               : DasResolvedEnumStorage(*T) == EDasEnumStorage::Enum8 ? 1
                                                                      : 0;
    };
    if (Type.Kind == TEXT("map") && Type.Key && Type.Key->Kind == TEXT("byte"))
        return ByteMapCarrier(Type);

    if (Type.Kind == TEXT("map") && Type.Key && Type.Key->Kind == TEXT("enum"))
        return EnumMapCarrier(Type, Storage(Type.Key), Storage(Type.Value));
    if (Type.Kind == TEXT("set") && Type.Element && Type.Element->Kind == TEXT("object"))
        return ObjectSetCarrier(FindObject<UClass>(nullptr, *Type.Element->TypePath));
    if (Type.Kind == TEXT("map") && Type.Key && Type.Key->Kind == TEXT("object"))
        return ObjectMapCarrier(Type, Storage(Type.Value));
    if (Type.Kind == TEXT("set") && Type.Element && Type.Element->Kind == TEXT("enum"))
    {
        auto* Enum = FindObject<UEnum>(nullptr, *Type.Element->TypePath);
        // Kismet emits FEnumProperty for native enum classes, but FByteProperty
        // for regular/native-byte enums and authored enums.
        return EnumSetCarrier(Enum, Storage(Type.Element) >= 0 ? Storage(Type.Element) != 0
                                                               : Enum && !Enum->IsA<UDasDefinedEnum>() &&
                                                                     Enum->GetCppForm() == UEnum::ECppForm::EnumClass);
    }
    if (Type.Kind == TEXT("map") && Type.Key && Type.Key->Kind == TEXT("name") && Type.Value)
        return NameMapCarrier(Type, Storage(Type.Value) == 1);
    if (Type.Kind == TEXT("map") && Type.Key && Type.Value &&
                          (Type.Key->Kind == TEXT("int") || Type.Key->Kind == TEXT("string")))
    {
        if (Type.Value->Kind == TEXT("enum"))
            return ReflectedMapCarrier(FindObject<UEnum>(nullptr, *Type.Value->TypePath),
                                       Type.Key->Kind == TEXT("string"), Storage(Type.Value) == 1);
        if (Type.Value->Kind == TEXT("object"))
            return ReflectedMapCarrier(FindObject<UClass>(nullptr, *Type.Value->TypePath),
                                       Type.Key->Kind == TEXT("string"));
    }
    if (Type.Kind == TEXT("map") && Type.Key && Type.Value &&
                          (Type.Key->Kind == TEXT("int") || Type.Key->Kind == TEXT("string")) &&
                                                  Type.Value->Kind == TEXT("struct"))
        return StructMapCarrier(FindObject<UScriptStruct>(nullptr, *Type.Value->TypePath),
                                Type.Key->Kind == TEXT("string"));
    if (Type.Kind == TEXT("array") && Type.Element && Type.Element->Kind == TEXT("object"))
        return ReferenceArrayCarrier(FindObject<UClass>(nullptr, *Type.Element->TypePath));
    if (Type.Kind == TEXT("array") && Type.Element && Type.Element->Kind == TEXT("enum"))
        return EnumArrayCarrier(FindObject<UEnum>(nullptr, *Type.Element->TypePath), Storage(Type.Element) == 1);
    if (Type.Kind == TEXT("array") && Type.Element && Type.Element->Kind == TEXT("struct"))
        return StructArrayCarrier(FindObject<UScriptStruct>(nullptr, *Type.Element->TypePath));
    return nullptr;
}
FProperty* DasContainerProperty(const FString& Path)
{
    if (auto* Soft = DasSoftProperty(Path))
        return Soft;
    for (const auto& Entry : StaticCarriers())
        if (Entry.Path == Path)
            return Entry.Values;
    if (Path.StartsWith(ByteMapPrefix))
    {
        auto* S = ResolveByteMap(Path);
        return S ? S->FindPropertyByName(TEXT("Values")) : nullptr;
    }

    if (Path.StartsWith(EnumMapPrefix))
    {
        auto* S = ResolveEnumMapCarrier(Path);
        return S ? S->FindPropertyByName(TEXT("Values")) : nullptr;
    }
    if (Path.StartsWith(ObjectSetPrefix) || Path.StartsWith(ObjectMapPrefix))
    {
        auto* S = ResolveObjectHashCarrier(Path);
        return S ? S->FindPropertyByName(TEXT("Values")) : nullptr;
    }
    if (Path.StartsWith(EnumSetPrefix) || Path.StartsWith(StrongEnumSetPrefix))
    {
        auto* S = ResolveEnumSetCarrier(Path);
        return S ? S->FindPropertyByName(TEXT("Values")) : nullptr;
    }
    if (Path.StartsWith(NameMapPrefix))
    {
        auto* S = ResolveNameMap(Path);
        return S ? S->FindPropertyByName(TEXT("Values")) : nullptr;
    }
    if (Path.StartsWith(ReflectedMapPrefix))
    {
        auto* S = ResolveReflectedMapCarrier(Path);
        return S ? S->FindPropertyByName(TEXT("Values")) : nullptr;
    }
    if (Path.StartsWith(StructMapPrefix))
    {
        auto* S = ResolveStructMapCarrier(Path);
        return S ? S->FindPropertyByName(TEXT("Values")) : nullptr;
    }
    if (Path.StartsWith(ReferenceArrayPrefix))
    {
        auto* S = ResolveReferenceArrayCarrier(Path);
        return S ? S->FindPropertyByName(TEXT("Values")) : nullptr;
    }
    if (Path.StartsWith(EnumArrayPrefix) || Path.StartsWith(StrongEnumArrayPrefix))
    {
        auto* S = ResolveEnumArrayCarrier(Path);
        return S ? S->FindPropertyByName(TEXT("Values")) : nullptr;
    }
    if (Path.StartsWith(StructArrayPrefix))
    {
        auto* S = ResolveStructArrayCarrier(Path);
        return S ? S->FindPropertyByName(TEXT("Values")) : nullptr;
    }
    return nullptr;
}
UScriptStruct* DasContainerCarrierForProperty(const FProperty* Property)
{
    if (auto* Soft = DasSoftCarrierForProperty(Property))
        return Soft;
    FDasDeclaredType Type;
    FString Error;
    if (!DescribeDasProperty(Property, Type, Error) ||
        !ValidateDasPropertyType(Type, EDasPropertyUsage::Container, Error))
        return nullptr;
    auto* Carrier = DasContainerCarrier(Type);
    auto* Values = Carrier ? Carrier->FindPropertyByName(TEXT("Values")) : nullptr;
    return Values && Values->SameType(Property) && Values->GetElementSize() == Property->GetElementSize() &&
                   Values->GetMinAlignment() == Property->GetMinAlignment()
               ? Carrier
               : nullptr;
}
