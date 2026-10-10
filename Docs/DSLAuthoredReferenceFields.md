# Authored class reference fields

ue_class properties can now use an imported ue_object_ref view of an authored
Actor/ActorComponent class. Omit initializers for null defaults:

```das
@ue_property=(EditAnywhere, BlueprintReadWrite)
Partner:ValueTargetRef
@ue_property=(EditAnywhere, BlueprintReadOnly)
LockedPartner:ValueTargetRef
```

```das
api.Partner = target
let other = api.Partner
api.Partner = default<ValueTargetRef>
// Inside the owning class method:
self.Partner = value
```

Self references are supported: collection publishes the class identity/native
parent before fields, avoiding a Blueprint-class bootstrap dependency. Source
imports and explicit reference script/type declarations remain unchanged.

The object type IR carries authored_id in addition to the physical host path.
As for current authored method signatures, physical Blueprint pins/storage use
the common DasProbeActor or DasScriptComponent host. Packing and unpacking check
the value's actual authored stable ID and liveness; null is allowed. A forged
view holding a live different authored class with the same host is rejected.
Source-typed self setter helpers and common property operators reject wrong
wrapper types before extracting handles. Readonly fields have no setter.

The field is an ordinary Unreal object property with ordinary ownership/GC
semantics; it does not instantiate a component or create a new object. DAS views
remain weak checked handles. Blueprint code can assign generic host values;
reading through the typed DAS view checks the expected authored identity.
Concrete generated Blueprint class pins/pickers remain separate work.

Ordinary Apply rejects changing an existing field's authored_id, even when the
physical host path is unchanged; identity migration remains deferred. New object
properties use existing null-default creation and snapshot/cook persistence.

This stage covers class properties. Authored-reference ue_struct members,
component instancing/default creation and automatic imports remain separate.
Sample field Partner avoids the inherited Peer property of the test host.
Validation: [DSLAuthoredReferenceFieldsValidation.md](DSLAuthoredReferenceFieldsValidation.md).
