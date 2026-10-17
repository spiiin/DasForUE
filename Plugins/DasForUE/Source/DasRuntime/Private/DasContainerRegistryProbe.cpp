#include "DasContainerRegistry.h"
#include "DasPropertyTypes.h"
#include "DasDefinedEnum.h"
#include "UObject/Package.h"
#include "UObject/StrongObjectPtr.h"
#include "UObject/UnrealType.h"
#include "UObject/GarbageCollection.h"

bool CheckDasContainerRegistry(FString& Error)
{
    Error.Reset();
    const FString Prefix = TEXT("/Script/DasRuntime.DasRegistryProbe_") + FGuid::NewGuid().ToString(EGuidFormats::Digits);
    auto Leaf = [](const TCHAR* Kind, const FString& Path = FString()) { FDasDeclaredType T; T.Kind = Kind; T.TypePath = Path; return T; };
    auto Array = [](const FDasDeclaredType& T) { FDasDeclaredType A; A.Kind = TEXT("array"); A.Element = MakeShared<FDasDeclaredType>(T); return A; };
    auto Fail = [&](const TCHAR* Why) { Error = Why; return false; };
    FDasContainerRegistry Registry(2);
    const auto Ints = Array(Leaf(TEXT("int")));
    auto* First = Registry.GetOrCreate(Prefix + TEXT("_Ints"), Ints, Error);
    if (!First || Registry.GetOrCreate(Prefix + TEXT("_Ints"), Ints, Error) != First || Registry.Num() != 1)
        return Fail(TEXT("Registry did not reuse its owned carrier"));
    if (Registry.GetOrCreate(Prefix + TEXT("_Ints"), Array(Leaf(TEXT("double"))), Error) || Error.IsEmpty() || Registry.Num() != 1)
        return Fail(TEXT("Registry accepted a different shape under an occupied name"));
    TWeakObjectPtr<UScriptStruct> Weak(First);
    CollectGarbage(RF_NoFlags, true);
    if (!Weak.IsValid() || !First->IsRooted() || Registry.GetOrCreate(Prefix + TEXT("_Ints"), Ints, Error) != First)
        return Fail(TEXT("Registry ownership did not survive GC"));
    if (!Registry.GetOrCreate(Prefix + TEXT("_Doubles"), Array(Leaf(TEXT("double"))), Error) ||
        Registry.GetOrCreate(Prefix + TEXT("_Overflow"), Ints, Error) || Error.IsEmpty() || Registry.Num() != 2)
        return Fail(TEXT("Registry capacity is not enforced atomically"));
    Registry.Reset();
    if (Registry.Num() || First->IsRooted()) return Fail(TEXT("Registry reset retained its ownership"));
    if (Registry.GetOrCreate(Prefix + TEXT("_Ints"), Ints, Error) || Error.IsEmpty())
        return Fail(TEXT("Registry took over a still-live unowned carrier"));
    CollectGarbage(RF_NoFlags, true);
    if (Weak.IsValid()) return Fail(TEXT("Reset carrier remained rooted after GC"));
    if (!Registry.GetOrCreate(Prefix + TEXT("_Ints"), Ints, Error)) return false;
    if (Registry.GetOrCreate(TEXT("/Game/InvalidCarrier"), Ints, Error) || Error.IsEmpty() ||
        Registry.GetOrCreate(Prefix + TEXT("_Invalid"), Leaf(TEXT("int")), Error) || Error.IsEmpty() || Registry.Num() != 1)
        return Fail(TEXT("Invalid registry request changed its entries"));

    TStrongObjectPtr<UEnum> Collision(NewObject<UEnum>(CreatePackage(TEXT("/Script/DasRuntime")), *(Prefix.RightChop(FString(TEXT("/Script/DasRuntime.")).Len()) + TEXT("_Collision")), RF_Transient));
    if (Registry.GetOrCreate(Prefix + TEXT("_Collision"), Ints, Error) || Error.IsEmpty() || Registry.Num() != 1)
        return Fail(TEXT("Registry replaced an unrelated object occupying its path"));

    // Edit only private fixtures, never engine or user-authored definitions.
    TStrongObjectPtr<UScriptStruct> Source(NewObject<UScriptStruct>(CreatePackage(TEXT("/Script/DasRuntime")), *(TEXT("DasRegistrySource_") + FGuid::NewGuid().ToString(EGuidFormats::Digits)), RF_Transient));
    auto* Field = CreateDasProperty(Source.Get(), TEXT("Value"), Leaf(TEXT("int")), EDasPropertyUsage::Value, Error);
    if (!Field) return false;
    Source->AddCppProperty(Field); Source->Bind(); Source->StaticLink(true);
    FDasContainerRegistry Layouts;
    const auto Structs = Array(Leaf(TEXT("struct"), Source->GetPathName()));
    auto* Original = Layouts.GetOrCreate(Prefix + TEXT("_Structs"), Structs, Error);
    if (!Original) return false;
    TStrongObjectPtr<UScriptStruct> Outer(NewObject<UScriptStruct>(CreatePackage(TEXT("/Script/DasRuntime")), *(TEXT("DasRegistryOuter_") + FGuid::NewGuid().ToString(EGuidFormats::Digits)), RF_Transient));
    auto* NestedField = CreateDasProperty(Outer.Get(), TEXT("Rows"), Structs, EDasPropertyUsage::Value, Error);
    if (!NestedField) return false;
    Outer->AddCppProperty(NestedField); Outer->Bind(); Outer->StaticLink(true);
    const auto NestedStructs = Array(Leaf(TEXT("struct"), Outer->GetPathName()));
    auto* NestedCarrier = Layouts.GetOrCreate(Prefix + TEXT("_Nested"), NestedStructs, Error);
    if (!NestedCarrier) return false;
    const int32 OriginalSize = Source->GetPropertiesSize();
    Source->SetPropertiesSize(OriginalSize + 8);
    const bool RejectedSize = !Layouts.GetOrCreate(Prefix + TEXT("_Structs"), Structs, Error) && !Error.IsEmpty();
    const bool RejectedNested = !Layouts.GetOrCreate(Prefix + TEXT("_Nested"), NestedStructs, Error) && !Error.IsEmpty();
    Source->SetPropertiesSize(OriginalSize);
    if (!RejectedSize || !RejectedNested || Layouts.GetOrCreate(Prefix + TEXT("_Structs"), Structs, Error) != Original)
        return Fail(TEXT("Registry failed to detect struct layout drift or damaged its valid entry"));
    Field->SetPropertyFlags(CPF_BlueprintReadOnly);
    const bool RejectedFlags = !Layouts.GetOrCreate(Prefix + TEXT("_Structs"), Structs, Error) && !Error.IsEmpty();
    Field->ClearPropertyFlags(CPF_BlueprintReadOnly);
    if (!RejectedFlags) return Fail(TEXT("Registry ignored changed field flags"));

    TStrongObjectPtr<UDasDefinedEnum> Enum(NewObject<UDasDefinedEnum>(CreatePackage(*(TEXT("/Game/DasRegistry/") + FGuid::NewGuid().ToString(EGuidFormats::Digits))), TEXT("Definition"), RF_Transient));
    TArray<TPair<FName, int64>> Members = {{TEXT("Definition::Zero"), 0}, {TEXT("Definition::One"), 1}};
    Enum->SetEnums(Members, UEnum::ECppForm::EnumClass, UEnum::EUnderlyingType::uint8, EEnumFlags::None, UEnum::EAddMaxKeyIfMissing::No);
    Enum->MemberIds.Add(0, TEXT("zero")); Enum->MemberIds.Add(1, TEXT("one"));
    auto EnumType = Leaf(TEXT("enum"), Enum->GetPathName()); EnumType.EnumStorage = EDasEnumStorage::Byte;
    const auto Enums = Array(EnumType);
    auto* EnumCarrier = Layouts.GetOrCreate(Prefix + TEXT("_Enums"), Enums, Error);
    if (!EnumCarrier) return false;
    Enum->MemberIds[1] = TEXT("replaced-member");
    const bool RejectedId = !Layouts.GetOrCreate(Prefix + TEXT("_Enums"), Enums, Error) && !Error.IsEmpty();
    Enum->MemberIds[1] = TEXT("one");
    auto ChangedMembers = Members; ChangedMembers[1].Value = 2;
    Enum->SetEnums(ChangedMembers, UEnum::ECppForm::EnumClass, UEnum::EUnderlyingType::uint8, EEnumFlags::None, UEnum::EAddMaxKeyIfMissing::No);
    const bool RejectedEnum = !Layouts.GetOrCreate(Prefix + TEXT("_Enums"), Enums, Error) && !Error.IsEmpty();
    Enum->SetEnums(Members, UEnum::ECppForm::EnumClass, UEnum::EUnderlyingType::uint8, EEnumFlags::None, UEnum::EAddMaxKeyIfMissing::No);
    if (!RejectedId || !RejectedEnum || Layouts.GetOrCreate(Prefix + TEXT("_Enums"), Enums, Error) != EnumCarrier)
        return Fail(TEXT("Registry failed to detect enum definition drift"));
    Error.Reset(); return true;
}
