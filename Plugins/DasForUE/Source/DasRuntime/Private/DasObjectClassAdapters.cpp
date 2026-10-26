#include "DasObjectClassAdapters.h"
#include "DasSchema.h"
#include "UObject/Interface.h"
#include "UObject/UnrealType.h"

class FDasObjectClassAdapter final : public IDasTypeAdapter
{
    bool Class;

public:
    explicit FDasObjectClassAdapter(bool IsClass) : Class(IsClass) {}
    EDasTypeKind GetKind() const override { return Class ? EDasTypeKind::Class : EDasTypeKind::Object; }
    void GetSupportSamples(TArray<FDasDeclaredType>& Out) const override
    {
        FDasDeclaredType Type;
        Type.Kind = Class ? TEXT("class") : TEXT("object");
        Type.TypePath = UObject::StaticClass()->GetPathName();
        Out.Add(Type);
        if (!Class)
        {
            Type.WeakReference = true;
            Out.Add(Type);
            Type.WeakReference = false;
            Type.TypePath = UInterface::StaticClass()->GetPathName();
            Out.Add(Type);
        }
    }
    bool UsesTypeIdentity() const override { return true; }
    bool SupportsContainerValue() const override { return !Class; }
    bool CanBeContainerKey(const FDasDeclaredType& Type) const override
    {
        auto* C = FindObject<UClass>(nullptr, *Type.TypePath);
        return !Class && C && !C->HasAnyClassFlags(CLASS_Interface);
    }
    bool MatchesProperty(const FProperty* Property) const override
    {
        return Class ? CastField<FClassProperty>(Property) != nullptr : DasReferencePropertyClass(Property) != nullptr;
    }
    TOptional<bool> ValidateDeclared(const FDasDeclaredType& Type, EDasPropertyUsage Usage,
                                     FString& Error) const override
    {
        auto* C = FindObject<UClass>(nullptr, *Type.TypePath);
        if (Type.WeakReference && C && C->HasAnyClassFlags(CLASS_Interface))
        {
            Error = TEXT("Weak storage requires a concrete object class");
            return false;
        }
        if (!C || (Class && C->HasAnyClassFlags(CLASS_Interface)))
        {
            Error = TEXT("Missing or incompatible loaded class");
            return false;
        }
        if (Usage == EDasPropertyUsage::Container && !Type.TypePath.StartsWith(TEXT("/Script/")))
        {
            Error = TEXT("Container references require native classes");
            return false;
        }
        return true;
    }
    TOptional<bool> DescribeProperty(const FProperty* P, FDasDeclaredType& Out, FString& Error) const override
    {
        const UClass* Base =
            Class ? CastFieldChecked<FClassProperty>(P)->MetaClass.Get() : DasReferencePropertyClass(P);
        if (!Base)
        {
            Error = TEXT("Reference property requires a base class");
            return false;
        }
        Out.TypePath = Base->GetPathName();
        Out.WeakReference = CastField<FWeakObjectProperty>(P) != nullptr;
        return true;
    }
    FProperty* MakeProperty(FFieldVariant Owner, FName Name, const FDasDeclaredType& T,
                            EDasPropertyUsage) const override
    {
        if (!FindObject<UClass>(nullptr, *T.TypePath))
            return nullptr;
        if (Class)
        {
            auto* P = new FClassProperty(Owner, Name);
            P->PropertyClass = UClass::StaticClass();
            P->MetaClass = FindObject<UClass>(nullptr, *T.TypePath);
            return P;
        }
        auto* C = FindObject<UClass>(nullptr, *T.TypePath);
        if (T.WeakReference)
        {
            auto* P = new FWeakObjectProperty(Owner, Name);
            P->PropertyClass = C;
            return P;
        }
        if (C->HasAnyClassFlags(CLASS_Interface))
        {
            auto* P = new FInterfaceProperty(Owner, Name);
            P->InterfaceClass = C;
            return P;
        }
        auto* P = new FObjectProperty(Owner, Name);
        P->PropertyClass = C;
        return P;
    }
};
TSharedRef<const IDasTypeAdapter> MakeDasObjectClassAdapter(bool Class)
{
    return MakeShared<FDasObjectClassAdapter>(Class);
}
