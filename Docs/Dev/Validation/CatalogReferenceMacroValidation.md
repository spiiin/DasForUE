# Catalog declaration and reference validation

2026-11-03, UE 5.8.3. Shared-agent job `catalog-entities-v5-20261103`. Reflection/macro suite: PASS. Overall job exit: 1.

- Reflection/macro suite: 101 checks, including 31 new source-catalog checks. Covers full IDs/unique short names, unknown/ambiguous identities, missing catalog, stale class IDs, wrong kinds and interface implementation validation, nullable values and property/signature transport.
- Header inventory: classes, interfaces, structs and enums; value-only catalogs, comment/string exclusion, explicit IDs, rename preserving identity, duplicate/empty/nonliteral identity rejection, interface declaration/reference identity deduplication.
- Real PIE regression run could not complete: the concurrently edited `RogueActionProjectileAttack.FireProjectile` fails compilation at `self.GetAimRotation()` / `self.GetAimPoint()` with `can't locate variable 'self'`. The projectile startup test times out; subsequent Overwhelm/point-impact suites are not reached. No gameplay PASS is claimed for this revision.
- Actual project startup generated 33 class records and 3 value/interface records, including all six object reference targets and all three authored interfaces used by the example.

Automatic inventory is opt-in and refreshes before editor compilation/reimport and PIE preparation. Invalid input preserves the previous catalog. The compiler validates declarations and bodies; reflected registries/assets retain their identity/layout checks. No directory scanning occurs in gameplay. Delegates, assets and runtime instances are outside this inventory.

## Runtime name-resolution audit (2026-11-03)

- Catalog JSON parsing is in compile-time `FUEAuthoredRefAnnotation::touch` / `DSLExpandReferenceType`, not gameplay reference conversion.
- `DSLAuthoredUnpack` emits an authored-ID constant; `ResolveAuthoredObject` checks the handle and a 256-site cache keyed by class/identity with GC/context epoch invalidation. A miss checks reflected class hierarchy identity in memory.
- Interface expansion resolves and validates the actual reflected path/ID before execution, then uses existing typed-reference transport.
- Prepared dependency hash validation and source-change invalidation can still read catalog bytes. This audit does not claim that program loading performs no filesystem I/O. No runtime behavior was changed by this documentation-only audit; the earlier 101-check receipt remains the functional validation.
