# Reference declaration macro validation

Shared-agent job `reference-macro-v1-20261102`, UE 5.8.3, 2026-11-02: PASS.

- `dsl-reflection-macros`: 70 checks, including 12 new reference macro cases. Native interface calls/ref-out/containers use a generated alias. Checks repeated aliases of one descriptor, authored nullable construction, wrong runtime authored identity, authored method return/property types, wrong argument types/arity, nonlocal source, unknown/empty class and wrong interface kind/identity.
- `rogue-interaction`: PASS, real PIE with the port's generated component/interface reference aliases.
- `rogue-projectile-attack`: PASS, 7 groups, real PIE; verifies action/component/reference calls in the attack path.
- `rogue-point-impact`: PASS, 3 groups, real projectile collision and authored interface damage dispatch.

All eight handle-wrapper declarations across the seven port `*_refs.das` modules use the new type macros. Unused handwritten RogueActionHost converters were removed. Alias modules and source dependencies remain explicit. The macros generate the existing annotated structure and use its validation; no runtime reference implementation or GC ownership policy was added.
