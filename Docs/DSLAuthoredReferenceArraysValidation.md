# Authored class array validation — 2026-10-10

Scripts/TestMethodValues.ps1 -Cooked passed fresh asset creation/reopen, JSON
roundtrip, real PIE and packaged Win64 Development execution.

Fixtures cover authored class array fields, input/result/ref/out, owned copies,
null values, indexed replacement and readonly/nominal source compile failures.
Runtime failure groups -64..-67 cover wrong logical class IDs in forged element
views, invalid array arguments, and failed ref/out calls without caller updates.

Two different wrappers for the same authored class share one physical host/carrier
but receive independent source converters. Both round-trip without helper collisions.
The model records the authored element ID recursively.

The shared editor/PIE/game probe assigns ValueDriver into a ValueTarget array through
the common native Actor host, confirms typed reading rejects the wrong authored
ID, then clears it with an empty whole-field replacement. This tests the distinction
between native host identity and logical script class identity.

RefPartners keeps bool input after ref/out parameters in source. Contract checks
match native properties by name, preserving calls when Blueprint changes physical
input/out ordering. Both successful and failed execution are covered.

Markers: PASS authored_array_nominal_readonly_compile_errors,
PASS authored_array_element_identity_model,
PASS native_wrong_authored_array_id_rejected_and_cleared.

Regressions: Actor/Component Create/Reopen, authored direct types Create/Reopen,
foundation smoke and native binding generation -Check.
Authored struct members/interfaces, concrete pins, Shipping/AOT and structural
migration remain outside this slice.
