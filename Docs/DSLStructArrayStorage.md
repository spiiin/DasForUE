# Arrays of struct: snapshots and field migration

The owned UEStructValue snapshot now supports dynamic arrays of supported structs
inside native/user struct trees. This is storage/interoperability support; it does
not yet add array fields to `[ue_struct]` or array types to ue_class declarations.
Use an existing Unreal user struct wrapper as the current UEStructValue property.

Scalar access accepts explicit zero-based indexes in dotted paths:

```das
let count = ue_struct_get_int(self.Bundle, "Items.0.Inner.Count")
self.Bundle = ue_struct_with_int(self.Bundle, "Items.0.Inner.Count", count + 1)
```

with-functions return a new owned snapshot; assign it to self to commit the change.
Indexes must be decimal, nonnegative, canonical (no leading zero except 0), at
most four digits and within array bounds. A path must continue to a struct member.
The current API reads/writes scalar leaves; it does not resize the array or return
an element reference. Wrong scalar accessors reject, so source operations must be
updated when a field changes from int to float/double.

Snapshot validation recursively checks array element structs, including enum
member validity. Each snapshot is bounded to 4096 total array elements and 16 MiB
of array element storage, in addition to existing depth/type/struct-size limits.
Object references, primitive arrays, direct arrays-of-arrays, map and set fields
remain outside this snapshot policy. Structs containing further arrays of struct
may nest within the same depth/element/byte limits.

MigrateStructFields now captures arrays in wrapper trees independently of native
layout: array lengths plus per-index paths and stable-ID scalar values. Element
order, independent overrides, nested sibling fields and Unicode are preserved.
After native compilation, arrays allocate the new element layout and initialize
elements before restoring old values. Added fields use new defaults; drop and
numeric rules apply to all occurrences. Original values/lengths remain available
for rollback, and projected restoration allocations obey the byte bound as well.
No old-layout FInstancedStruct snapshot is kept for changing arrays across compile.

The first migration scope uses empty native wrapper/CDO array defaults. Nonempty
wrapper array defaults and explicit text defaults are refused and need a separate
policy. Normal instances may contain populated arrays. Arrays must be inside an
owned struct field on the established Actor/CDO family; direct Actor array fields,
unrelated storage, retired snapshots, container graph repair and active PIE remain
outside this scope. Wrapper Make/Break/Set/custom and split-pin nodes retain the
earlier refusal policy.

Tests: Scripts/TestDSLTypeRegistry.ps1 -StructArray with optional -StructNumeric or
-StructDrop; add -Cooked for packaged execution. The fixture contains a three-item
array of envelopes with inner authored struct values 31/38/45, independent titles
and labels, plus an empty array. It verifies add/rename, conversion/removal, rollback,
index reads/writes, bounds/limit rejection, source accessor updates, save/reopen and
actual cooked map values/callbacks.

Next: DSL array declarations and recursive dynamic pack/unpack, then set/map storage
and explicit policies for key collisions/reindexing, defaults and graph repair.
