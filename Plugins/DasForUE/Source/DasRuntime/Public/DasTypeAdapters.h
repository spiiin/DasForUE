#pragma once
#include "CoreMinimal.h"
#include "DasPropertyTypes.h"

// Type adapters (Docs/Dev/Design/TypeAdapters.md): one object owns the identity and the decisions of one
// value type. Subsystems ask the registry instead of keeping their own type predicates and path literals.
// Hooks exist only where a type has decisions; everything else takes the generic path of its kind.
enum class EDasAdapterDefault : uint8
{
    Explicit,
    ImplicitEmpty // authored fields take no initializer and export an empty default
};

class DASRUNTIME_API IDasTypeAdapter
{
public:
    virtual ~IDasTypeAdapter() = default;
    virtual EDasTypeKind GetKind() const = 0;
    // Struct adapters are found by this struct; kind adapters return nullptr.
    virtual const UScriptStruct* GetStruct() const { return nullptr; }
    // Dynamic carrier families participate without mutating the registry after startup.
    virtual bool IsReferenceValue() const { return false; }
    virtual bool IsStructFamily() const { return false; }
    virtual bool MatchesStructPath(const FString& Path) const { return false; }
    virtual FProperty* GetCarrierProperty(const FString& Path) const { return nullptr; }
    virtual const TCHAR* GetStructFieldDefault() const { return nullptr; }
    // Identity-bearing kinds (enum now; object/class later) retain their path in the descriptor.
    virtual bool UsesTypeIdentity() const { return false; }
    // An unset result leaves the operation to the generic builder/validator.
    virtual TOptional<bool> ValidateDeclared(const FDasDeclaredType&, EDasPropertyUsage, FString&) const { return {}; }
    virtual TOptional<bool> DescribeProperty(const FProperty*, FDasDeclaredType&, FString&) const { return {}; }
    // Kind adapters: whether a reflected property has this kind.
    virtual bool MatchesProperty(const FProperty* Property) const { return false; }

    // A leaf value is never expanded as an authored child struct.
    virtual bool IsLeafValue() const { return false; }
    // Math value structs (Vector, Vector2D, Rotator, Transform): text literal defaults, finite values.
    virtual bool IsMath() const { return false; }
    // Own call signature with a dedicated FDasScalarValue slot (Vector/Rotator/Transform keep their
    // persisted numbers); other structs travel as EDasMethodSignature::Struct snapshots.
    virtual TOptional<EDasMethodSignature> GetSignature() const { return {}; }
    // Value kind name ("vector", "rotator", ...) shared by constructor defaults and callback payload types,
    // or nullptr when the type has none.
    virtual const TCHAR* GetDefaultKind() const { return nullptr; }
    virtual EDasAdapterDefault GetDefaultPolicy() const { return EDasAdapterDefault::Explicit; }
    // nullptr leaves property creation to the generic builder of the kind.
    virtual FProperty* MakeProperty(FFieldVariant Owner, FName Name, const FDasDeclaredType& Type,
                                    EDasPropertyUsage Usage) const
    {
        return nullptr;
    }
    // Values of the struct in snapshots, nested values and arguments.
    virtual bool ValidateValue(const void* Data, FString& Error) const { return true; }
};

// daScript view of a type; defined with the DSL lowering (DasRuntime private).
class IDasScriptTypeFacet;

class DASRUNTIME_API FDasTypeAdapters
{
public:
    struct FEntry
    {
        TSharedRef<const IDasTypeAdapter> Adapter;
        const IDasScriptTypeFacet* Script = nullptr;
    };
    // Built-in adapters are registered on first use; the registry is read-only afterwards.
    static const FDasTypeAdapters& Get();
    void Register(TSharedRef<const IDasTypeAdapter> Adapter, const IDasScriptTypeFacet* Script = nullptr);

    const IDasTypeAdapter* Find(const FDasDeclaredType& Type) const;
    const IDasTypeAdapter* FindStruct(const UScriptStruct* Type) const;
    const IDasTypeAdapter* FindStructPath(const FString& Path) const;
    const IDasTypeAdapter* FindProperty(const FProperty* Property) const;
    const IDasTypeAdapter* FindDefaultKind(const FString& Kind) const;
    const IDasTypeAdapter* FindSignature(EDasMethodSignature Signature) const;
    TConstArrayView<FEntry> Entries() const { return EntryList; }

private:
    TArray<FEntry> EntryList;
    TMap<const UScriptStruct*, int32> ByStruct;
    TMap<FString, int32> ByPath;
    TMap<EDasTypeKind, int32> ByKind;
    TArray<int32> StructFamilies;
};

// Leaf value struct (GameplayTag, ...) by path, for authored type validation.
DASRUNTIME_API const IDasTypeAdapter* DasLeafValueAdapter(const FString& StructPath);
// Math value struct (Vector, Vector2D, Rotator, Transform) by path.
DASRUNTIME_API const IDasTypeAdapter* DasMathAdapter(const FString& StructPath);
#if DAS_WITH_TEST_PROBES
DASRUNTIME_API bool CheckDasTypeAdapters(FString& Error);
#endif
