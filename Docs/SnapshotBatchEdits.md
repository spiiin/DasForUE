# Scoped snapshot edits

`ue_struct_edit(snapshot)` makes one private native root copy, invokes a synchronous
daScript block, validates the completed value and returns an immutable snapshot.
The ordinary scalar, nested-value, array, map and set adapters share this path;
outside an edit block their existing immutable behavior is unchanged.

```dascript
let original=ue_get_self_struct("Snapshot")
let changed=ue_struct_edit(original) $(var draft:UEStructValue) {
    draft=ue_struct_map_with_int(draft,"IntMap",0,10)
    draft=ue_struct_map_with_int(draft,"IntMap",1,20)
    draft=ue_struct_set_with_int(draft,"IntSet",7,true)
    draft=ue_struct_array_resize(draft,"Rows",2)
    draft=ue_struct_with_byte(draft,"Rows.1.Priority",uint8(42))
}
// Explicit publication to the Unreal object is a separate operation.
ue_set_self_struct("Snapshot",changed)
```

The block argument is a temporary context-local capability. Writes to that draft
modify the same buffer and return the same capability, so normal `with` expressions
can be reused. Reads and aliases inside the block see its current value. The source
and its aliases remain unchanged. Returning a result identical to an existing
snapshot may reuse that immutable snapshot's token.

The draft expires on both success and error. Saving its handle in a global does
not keep it valid; use the returned immutable result after the block. Obtaining a
nested snapshot still creates an independent value: edit it and explicitly assign
it back with `ue_struct_nested_with` or the appropriate map adapter.

## Errors and lifetime

An unhandled block error discards the draft. A failed write invalidates the draft
before touching native storage, because a map codec can insert a key before
rejecting its value. Catching that error inside the block cannot publish a partial
write; subsequent draft access or final publication fails. Full recursive snapshot
and math validation runs at publication. Existing type, index and container-size
checks remain in the individual adapters.

Unreal references inside active drafts participate in GC through the context's
`FGCObject` collector. Cleanup is explicit before daScript rethrow, since script
error recovery can bypass C++ destructors. Draft and immutable tokens share a
monotonic allocator, and drafts are excluded from immutable identity lookup and
snapshot reclamation. Immutable key-order caches are bypassed for draft reads.

Independent nested edit blocks are supported to a depth of eight. Starting another
edit of an active draft is rejected. Each independent block has its own result;
this is not a transaction across several snapshots. Calling Unreal functions,
writing object properties or assigning unrelated globals inside the block has its
normal side effects, which are not rolled back. The block itself does not write
the returned snapshot into a UObject property.

## Verification

Foundation tests exercise mixed writes, original-value isolation, changing key
orders, expired draft handles, nested scopes, handled and unhandled errors, partial
map insertion failure, draft GC roots and release after rollback. The native fixture
counts actual root copy assignments: one native capture plus one copy for an edit
block, regardless of how many supported writes it contains.

The performance probe compares `four_edits` with `batch_four_edits` on the same
fixture and checks checksums and copy counts in every sample. A batch of 64 groups
of four writes makes 65 root copies including the initial capture, compared with
257 for sequential immutable writes. This measures root copies, not every nested
buffer allocation or field copy. Timings also include final validation and
immutable identity lookup; see [Performance.md](Performance.md).

Editor and packaged Win64 Development builds, both Foundation suites and all
33 performance cases passed. Each timed case has nine samples. Cook/stage/archive
used the Entry map; execution used NullRHI, so this is not a rendered gameplay or
Shipping/AOT check. Frozen core/test/benchmark source hashes remained unchanged.
The reports are in `logs/batch-edit-*.json` and `logs/batch-edit-validation.json`.
