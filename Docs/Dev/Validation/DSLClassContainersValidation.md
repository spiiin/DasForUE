# Direct class container validation — 2026-10-10

`Scripts/TestMethodValues.ps1 -Cooked` verifies fresh assets, JSON type roundtrip,
editor reopen, real PIE and a staged Win64 Development game.

DirectSamples/DirectPoints/DirectWeights/DirectTags are checked as native
FArrayProperty/FMapProperty/FSetProperty fields, rather than struct-valued fields.
The runtime fixture exercises self/reference operations, indexed compound writes,
authored struct/enum elements, independent getter copies, whole-field replacement
and clearing. An invalid index leaves the earlier native array value unchanged.

Compiler negatives cover readonly mutations and whole-field replacement, wrong
element/container types, nested-array shapes without carriers and explicit
nonempty defaults. Runtime failure groups are -51, -52 and -53.

Body-only Apply keeps the same generated class and reports no Blueprint recompile.
A declaration metadata change is refused by the direct-container migration guard
before changing the class. This does not establish structural migration support.

Markers:
- PASS direct_container_readonly_types_defaults_compile_errors
- PASS direct_container_native_properties
- PASS direct_container_body_only_reimport
- PASS direct_container_declaration_change_refused
- DAS_METHOD_VALUES_PIE_RESULT=PASS
- DAS_METHOD_VALUES_PROBE=PASS cooked=True

Regressions: Actor/Component Create/Reopen, direct authored types Create/Reopen,
foundation smoke and native binding generation with -Check.
Shipping and AOT coverage for these new helpers remain separate work.
