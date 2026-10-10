# Indexed container writes and reference mutations

Supported authored struct container members can now be changed directly, both in
ue_class methods and in ordinary functions using typed authored references:

```das
self.Settings.Samples[0] = 5.0lf
self.Settings.Samples[1l] += 3.0lf
self.Settings.Items[0].Count = 17
self.Settings.Items[0].Mode = MethodState.Idle
self.Settings.Items[0].Position.x += 4.0lf
self.Settings.Weights["score"] = 7.0lf
push(self.Settings.Samples, 2.0lf)
push(self.Settings.Samples, 4.0lf, 0l)
push_clone(self.Settings.Items, MethodPoint())
pop(self.Settings.Samples)
insert_clone(self.Settings.Weights, "copy", 8.5lf)
erase(self.Settings.Samples, 0l, 1l)

// The same operations work outside class methods.
def update(api : ValueTargetRef) {
    push(api.Settings.Samples, 2.0lf)
    push_clone(api.Settings.Items, MethodPoint())
    api.Settings.Items[0].Count += 1
    api.Settings.Weights["score"] = 7.5lf
    insert(api.Settings.Tags, "ready")
    let removed = erase(api.Settings.Weights, "score")
    clear(api.Settings.Tags)
}
```

Array indices accept int and int64; map keys retain their declared exact type.
push/push_clone accept one element and an optional insertion index. Array-to-array
push overloads and consuming moves are not provided by these adapters. pop keeps
the normal daScript void result. Set/map erase returns bool after a successful commit.

Receiver, indices and supplied values are evaluated once before the helper reads
native state. Each mutation works on an owned snapshot and validates/packs the
result before publishing the root UE property. Index or packing failures leave
that property unchanged. This is per-operation atomicity, not a transaction across
several statements; operand side effects are not rolled back.

Root and crossed struct member BlueprintReadOnly annotations are checked. Generated
helper parameters preserve nominal source types, including authored enums and
structs. Caller-supplied snapshots remain independent of native storage.

The reference visitor also publishes ordinary nested scalar/value member writes
instead of modifying a detached getter result. Both raw fields before accessor
generation and compiler-generated property getter calls are recognized.

## Scope

- Existing supported array/map/set shapes inside authored ue_struct properties.
- Multi-level self paths through supported struct/container shapes, including
  nested struct/math fields; see DSLMultiIndexSelf.md.
- Typed-reference paths use the resolved receiver/property source descriptors and
  can reconstruct nested indexed paths for supported declared container shapes.
- New element/value types, consuming move,
  live UE iterators and passing native members as mutable function parameters remain
  separate work. Use a local clone and whole-property assignment for those cases.

See also DSLSelfContainerOperations.md and DSLSelfContainerMembers.md.

Validation: [DSLIndexedContainersValidation](DSLIndexedContainersValidation.md).

Update: direct class container declarations are supported in [DSLClassContainers](DSLClassContainers.md).

Update: arrays of UEVector/UERotator/UETransform are supported in DSLMathArrays.md.

Update: authored enum arrays are supported in DSLEnumArrays.md.

Update: native object/interface arrays are supported in DSLReferenceArrays.md.
