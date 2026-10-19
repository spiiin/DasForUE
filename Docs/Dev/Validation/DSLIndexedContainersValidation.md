# Indexed container validation — 2026-10-10

`Scripts/TestMethodValues.ps1 -Cooked` passed:

- Fresh reflected assets and JSON declaration identity roundtrip; editor reopen.
- Real PIE automation: `DAS_METHOD_VALUES_PIE_RESULT=PASS`.
- Win64 Development build, cook, staging and packaged execution:
  `DAS_METHOD_VALUES_PROBE=PASS cooked=True`.
- Self array/map indexed writes, compound writes, authored enum and nested math
  element fields; push/push_clone, positional insertion, pop, insert_clone and
  int64 index/insertion/erase overloads.
- Equivalent mutations from ordinary DAS functions through ValueTargetRef,
  including bool erase results and clear/pop.
- Out-of-range index recovery and non-finite math packing rejection preserve
  native values, including unrelated fields in the same root snapshot.
- Readonly roots, wrong element/value/key and wrong nominal enum types fail
  compilation. Foreign readonly container operations produce a persistent
  compiler diagnostic instead of silently changing a detached getter snapshot.

Markers: `PASS indexed_container_types_readonly_compile_errors`,
`PASS foreign_container_types_readonly_compile_errors`.
Runtime fixture failure groups: -49 (self), -50 (typed reference).

Validation scope is the supported container shapes and Win64 Development; it
does not establish Shipping/AOT coverage for the new helpers or new types.
