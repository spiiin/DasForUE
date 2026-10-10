# Arrays of authored class references

Arrays of ue_object_ref views are supported in ue_class properties and reflected
method input/result/ref/out signatures:

```das
@ue_property=(EditAnywhere, BlueprintReadWrite)
Peers : array<CounterRef>

[ue_function(BlueprintCallable)]
def CopyPeers(values : array<CounterRef>) : array<CounterRef> {
    return <- clone(values)
}

push(self.Peers, peer)
self.Peers[0] = default<CounterRef>
var copy = api.CopyPeers(api.Peers)
api.Peers := copy
```

The source enum/struct registry rules are unchanged. Authored reference members
inside ue_struct remain deferred, including array members: that needs the separate
source bootstrap/dependency contract already required by scalar authored members.

Native storage/pins use the common DasProbeActor or DasScriptComponent host, as for
existing authored scalar fields/signatures. The recursive authoring model retains
the element's authored_id. Every non-null element is checked against this stable
class ID during natural packing and unpacking. C++/Blueprint can physically assign
another instance of the same host; a subsequent typed DAS read rejects it.

Source wrappers remain nominal types, including two wrappers for the same authored
class. Their converters are generated separately by source-qualified wrapper type,
even though the native carrier path is identical. Each converter still validates
the class ID. Self container-operation helpers use the same source-aware naming.

Getters/results are owned arrays. Null values round-trip, readonly writes reject,
and a failing element conversion does not publish native state. Ref/out values
are converted before caller copyback. To clear malformed native data, replace
the whole property with an empty typed array; read/modify operations must validate
the existing elements first.

Authored call contract checks now locate parameters by name instead of relying on
physical UFunction order. Blueprint may reorder input/out properties; source call
order remains unchanged. The mixed ref/out plus bool fixture tests this case.

Defaults remain empty. Concrete Blueprint-class pins, authored interface arrays,
authored ue_struct members, nonempty defaults, ownership extensions, Shipping/AOT
and structural migrations remain separate work.

Fixtures: ValueTarget.DirectPartners/AlternativePartners, CopyPartners/RefPartners
and ValueDriver.RejectNativeAuthoredArrays. Validation: DSLAuthoredReferenceArraysValidation.md.
