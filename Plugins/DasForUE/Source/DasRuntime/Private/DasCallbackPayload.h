#pragma once
#include "DasPropertyValues.h"
#include "DasCallbackTypes.h"

// A GC-cleared, originally non-null Actor must not become a valid null capture.
// Enumerate through the common GC traversal, retaining only weak identities.
struct FDasCallbackIdentities final : FReferenceCollector {
    TArray<TWeakObjectPtr<UObject>> Objects;
    virtual bool IsIgnoringArchetypeRef() const override {return false;}
    virtual bool IsIgnoringTransient() const override {return false;}
    virtual void HandleObjectReference(UObject*& Object,const UObject*,const FProperty*) override {if(Object)Objects.Add(Object);}
    bool IsAlive() const {for(const auto& Object:Objects)if(!Object.IsValid() || !DasCallbackObjectAlive(Object.Get()))return false;return true;}
};
// Stable heap owner shared by the pending slot and in-flight delivery. Reuse
// the common reflected-value GC traversal instead of a second reference walker.
class FDasCallbackPayload final {
    FDasPropertyValueRoots Roots;
    FDasCallbackIdentities Identities;
public:
    explicit FDasCallbackPayload(const FDasScalarValue& In){Roots.Keep(In);Roots.AddReferencedObjects(Identities);}
    const FDasScalarValue& GetValue() const {return Roots.Values[0];}
    bool IsAlive() const {return Identities.IsAlive();}
};
