#include "DasSoftReferences.h"
#include "DasSchema.h"
#include "StructUtils/InstancedStruct.h"
#include "Misc/PackageName.h"
#include "Misc/SecureHash.h"
#include "UObject/Package.h"
namespace
{
TMap<FString, UScriptStruct*> SoftCarriers;
}
bool DasSoftTargetMatches(const UObject* Object, const UClass* Base, bool ClassReference)
{
    if (!Base || Base->HasAnyClassFlags(CLASS_Interface | CLASS_Deprecated | CLASS_NewerVersionExists))
        return false;
    if (!Object)
        return true;
    if (!IsValid(Object))
        return false;
    if (ClassReference)
    {
        auto* Class = Cast<UClass>(Object);
        return Class && !Class->HasAnyClassFlags(CLASS_Deprecated | CLASS_NewerVersionExists) && Class->IsChildOf(Base);
    }
    return !Object->IsA<UClass>() && Object->IsAsset() && Object->IsA(Base);
}
bool ValidateDasSoftPath(const FString& Text, FString& Error)
{
    Error.Reset();
    if (Text.IsEmpty())
        return true;
    for (TCHAR Ch : Text)
        if (Ch == 0)
        {
            Error = TEXT("Soft reference path contains NUL");
            return false;
        }
    if (Text.Len() > 1024 || !FPackageName::IsValidObjectPath(Text) ||
        !FSoftObjectPath(Text).GetSubPathString().IsEmpty())
    {
        Error = TEXT("Soft references require an asset object path without subobjects");
        return false;
    }
    return true;
}
FProperty* CreateDasSoftProperty(FFieldVariant Owner, FName Name, const FSoftObjectProperty* Prototype)
{
    if (auto* Class = CastField<FSoftClassProperty>(Prototype))
    {
        auto* P = new FSoftClassProperty(Owner, Name);
        P->PropertyClass = UClass::StaticClass();
        P->MetaClass = Class->MetaClass;
        P->SetPropertyFlags(CPF_UObjectWrapper);
        return P;
    }
    auto* P = new FSoftObjectProperty(Owner, Name);
    P->PropertyClass = Prototype->PropertyClass;
    P->SetPropertyFlags(CPF_UObjectWrapper);
    return P;
}
UScriptStruct* DasSoftCarrier(UClass* Base, bool ClassReference, FString& Error)
{
    check(IsInGameThread());
    Error.Reset();
    if (!Base || !Base->GetPathName().StartsWith(
                     TEXT("/Script/")) ||
                     Base->HasAnyClassFlags(CLASS_Interface | CLASS_Deprecated | CLASS_NewerVersionExists))
    {
        Error = TEXT("Soft references require a loaded native non-interface base");
        return nullptr;
    }
    const FString Name = (ClassReference
                          ? TEXT("DasSoftClass_"): TEXT("DasSoftObject_")) + FMD5::HashAnsiString(*Base->GetPathName()),
                          Path = TEXT("/Script/DasRuntime.")+ Name;
    if (auto* Entry = SoftCarriers.Find(Path))
        return *Entry;
    if (SoftCarriers.Num() >= 1024 || FindObject<UObject>(nullptr, *Path))
    {
        Error = TEXT("Soft carrier limit or name collision");
        return nullptr;
    }
    auto* S = NewObject<UScriptStruct>(CreatePackage(TEXT("/Script/DasRuntime")), *Name, RF_Transient);
    FSoftObjectProperty* P = nullptr;
    if (ClassReference)
    {
        auto* C = new FSoftClassProperty(S, TEXT("Values"));
        C->PropertyClass = UClass::StaticClass();
        C->MetaClass = Base;
        P = C;
    }
    else
    {
        P = new FSoftObjectProperty(S, TEXT("Values"));
        P->PropertyClass = Base;
    }
    P->SetPropertyFlags(CPF_UObjectWrapper);
    S->AddCppProperty(P);
    S->Bind();
    S->StaticLink(true);
    S->AddToRoot();
    SoftCarriers.Add(Path, S);
    return S;
}
FSoftObjectProperty* DasSoftProperty(const FString& Path)
{
    auto* Entry = SoftCarriers.Find(Path);
    return Entry ? FindFProperty<FSoftObjectProperty>(*Entry, TEXT("Values")) : nullptr;
}
UScriptStruct* DasSoftCarrierForProperty(const FProperty* Property)
{
    auto* P = CastField<FSoftObjectProperty>(Property);
    if (!P)
        return nullptr;
    auto* C = CastField<FSoftClassProperty>(P);
    FString Error;
    return DasSoftCarrier(C ? C->MetaClass.Get() : P->PropertyClass.Get(), C != nullptr, Error);
}
bool ValidateDasSoftValue(const FSoftObjectProperty* P, const void* Memory, FString& Error)
{
    const auto Value = P->GetPropertyValue(Memory);
    if (!ValidateDasSoftPath(Value.ToSoftObjectPath().ToString(), Error))
        return false;
    const auto* C = CastField<FSoftClassProperty>(P);
    if (!DasSoftTargetMatches(Value.Get(), C ? C->MetaClass.Get() : P->PropertyClass.Get(), C != nullptr))
    {
        Error = TEXT("Resolved soft reference has an incompatible object/class type");
        return false;
    }
    return true;
}
bool UpcastDasSoftValue(const FInstancedStruct& Source, const FString& TargetPath, FInstancedStruct& Out,
                        FString& Error)
{
    Error.Reset();
    auto* From = Source.IsValid() ? DasSoftProperty(Source.GetScriptStruct()->GetPathName()) : nullptr;
    auto* To = DasSoftProperty(TargetPath);
    if (!From || !To)
    {
        Error = TEXT("Unknown soft reference conversion type");
        return false;
    }
    auto* FromClass = CastField<FSoftClassProperty>(From);
    auto* ToClass = CastField<FSoftClassProperty>(To);
    if (bool(FromClass) != bool(ToClass))
    {
        Error = TEXT("Soft object/class conversion is forbidden");
        return false;
    }
    auto* Child = FromClass ? FromClass->MetaClass.Get() : From->PropertyClass.Get();
    auto* Parent = ToClass ? ToClass->MetaClass.Get() : To->PropertyClass.Get();
    if (!Child || !Parent || !Child->IsChildOf(Parent))
    {
        Error = TEXT("Soft references can only convert to a parent constraint");
        return false;
    }
    if (!ValidateDasStructSnapshot(Source, Error))
        return false;
    FInstancedStruct Candidate;
    Candidate.InitializeAs(FindObject<UScriptStruct>(nullptr, *TargetPath));
    To->SetPropertyValue(To->ContainerPtrToValuePtr<void>(Candidate.GetMutableMemory()),
                         From->GetPropertyValue(From->ContainerPtrToValuePtr<void>(Source.GetMemory())));
    if (!ValidateDasStructSnapshot(Candidate, Error))
        return false;
    Out = MoveTemp(Candidate);
    return true;
}
void ResetDasSoftCarriers()
{
    for (auto& Pair : SoftCarriers)
        Pair.Value->RemoveFromRoot();
    SoftCarriers.Empty();
}

// A family owns all dynamically constrained soft object/class carriers. Only carriers created by
// DasSoftCarrier are recognised; a matching-looking name or arbitrary one-field struct is insufficient.
class FDasSoftReferenceAdapter final : public IDasTypeAdapter
{
public:
    EDasTypeKind GetKind() const override { return EDasTypeKind::Struct; }
    void GetSupportSamples(TArray<FDasDeclaredType>& Out) const override
    {
        for (bool ClassReference : {false, true})
        {
            FString Error;
            if (const auto* Carrier = DasSoftCarrier(UObject::StaticClass(), ClassReference, Error))
            {
                FDasDeclaredType Type;
                Type.Kind = TEXT("struct");
                Type.TypePath = Carrier->GetPathName();
                Out.Add(MoveTemp(Type));
            }
        }
    }
    bool IsReferenceValue() const override { return true; }
    bool IsStructFamily() const override { return true; }
    bool MatchesStructPath(const FString& Path) const override { return DasSoftProperty(Path) != nullptr; }
    bool MatchesProperty(const FProperty* Property) const override
    {
        return CastField<FSoftObjectProperty>(Property) != nullptr;
    }
    bool IsLeafValue() const override { return true; }
    EDasAdapterDefault GetDefaultPolicy() const override { return EDasAdapterDefault::ImplicitEmpty; }
    const TCHAR* GetStructFieldDefault() const override { return TEXT("None"); }
    FProperty* GetCarrierProperty(const FString& Path) const override { return DasSoftProperty(Path); }
    FProperty* MakeProperty(FFieldVariant Owner, FName Name, const FDasDeclaredType& Type,
                            EDasPropertyUsage) const override
    {
        const auto* Prototype = DasSoftProperty(Type.TypePath);
        return Prototype ? CreateDasSoftProperty(Owner, Name, Prototype) : nullptr;
    }
};
TSharedRef<const IDasTypeAdapter> MakeDasSoftReferenceAdapter()
{
    return MakeShared<FDasSoftReferenceAdapter>();
}
