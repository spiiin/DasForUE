# Multi-level self validation — 2026-10-10

Scripts/TestMethodValues.ps1 -Cooked passed fresh asset creation/reopen, JSON
roundtrip, real PIE and packaged Win64 Development execution.

ValueTarget.MultiIndexContainers (-54) tests:
- Mixed int/int64 outer and inner indices, map keys, scalar/enum/math element fields.
- push/push_clone, insert/erase (bool result), pop/clear below an index.
- Whole nested-container clone replacement.
- Independent outer/inner invalid-index recovery with native values preserved.
- Non-finite math packing failure without publishing the modified root.
- Two side-effecting index expressions evaluated exactly once each.
- Unchanged sibling sentinel after successful and failed operations.

Compiler negatives reject readonly roots, invalid outer/inner key types,
wrong leaf types, another enum with identical values and readonly nested operations.
Marker: PASS multi_index_readonly_types_compile_errors.

Existing single-level indexed self/reference and direct class container fixtures
continue to pass. Actor/Component Create/Reopen, authored direct types Create/Reopen,
foundation smoke and generated native bindings -Check are regression coverage.

This does not establish Shipping/AOT coverage, new container shapes, live views,
mutable native projections or structural migration capabilities.
