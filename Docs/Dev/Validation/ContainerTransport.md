# General container transports and authored class bases

Validated on 2026-11-02 through build agent job `container-transport-20261102k`.

- Editor Development build: PASS (56 s including agent overhead).
- One automation scenario, `DasForUE.Runtime.ContainerTransport`: PASS, no errors or warnings (0.54 s test body; 17 s editor process).
- The validation receipt's 605 source/test SHA-256 hashes match the final implementation.
- Existing extern signatures are preserved; the generated manifest adds three class snapshot/map functions.

## Coverage

The scenario imports a real DSL ScriptObject base, creates a Blueprint child, and
checks 18 array/map descriptor combinations with class/object/scalar/text values.
Every accepted shape has a reusable carrier with an exact reflected shape and
property-to-carrier roundtrip. Nested and weak arrays and unsupported hash keys
remain rejected. Class defaults accept the child/null and refuse an unrelated class.
The class snapshot survives GC.

An imported `UEClass<ActionRef>` alias resolves to the applied Blueprint base, not
to its native host. A DSL Actor declares class array/map/scalar properties. Actual
UFunction calls in a started synthetic game world write/read those fields and
return the child class through the frame bridge; LastDasError remains empty.

## Design and bounds

New combinations are built through the shared property adapters. Legacy carrier
names remain available. The versioned generic recipe is self-contained and bounded
by FName length and registry capacity. Snapshot shape validation and signature
checks share the same adapters; recursive/opaque layouts retain their guards.

Authored bases must be applied/loaded before alias resolution. Missing/ambiguous
source bindings are compile errors; an explicit class asset alias can disambiguate.
Class map keys/set elements, nested/weak containers and class-list DSL initializers
are not added. Existing live carrier definition drift is refused; this does not add
automatic structural Blueprint migration. Full PIE, packaged and AOT were not run.

## Integration

Worktree: `C:/src/DasForUE2-container-transport`, branch `codex/container-transport`.
The private baseline `efdc523` snapshots prerequisite work from the other chat on
main `71ffb39`. Integrate the feature delta only; do not merge the private baseline
snapshot wholesale. The later integration into main preserves the other chat's
uncommitted work; its combined build has not been repeated as part of the transfer.

Local evidence: `logs/container-transport-validation.json`,
`logs/container-transport-report/index.json`, and shared build-agent job log/done.
