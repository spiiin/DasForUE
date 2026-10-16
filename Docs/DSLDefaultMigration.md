# Explicit default migration

The first migration policy supports **default-only changes** for an existing DSL
Actor Blueprint. Reimport without a policy now selects the safe override-preserving merge; see [DSLDefaultsMigration.md](DSLDefaultsMigration.md). This page describes the explicit policy that preserves every existing instance value.

```text
Das.DSL.Reimport /Game/MyFolder/BP_Counter defaults=preserve_instances
```

Editor API: `FDasDSLService::Reimport(Blueprint, Error,
EDasDefaultMigrationPolicy::PreserveInstances)`.

The service validates the new script/model and the current asset against its
stored model before mutation. It refuses changes to field/method identity,
names, types, order, flags or metadata in the same migration. Apply other
supported declarations separately; rename/removal/type migrations remain future work.
Malformed/unknown struct defaults and stale or modified Blueprint declarations
are rejected before writing any defaults.

The policy updates this Blueprint's CDO and variable default descriptors, without
recompiling or replacing its class. It preserves **every existing instance value**,
including values equal to the old default. It does not infer whether the user
edited a value. Example: changing Count from 10 to 11 leaves an existing Count=10
actor at 10 and an overridden Count=77 actor at 77; a newly spawned actor gets 11.
Derived Blueprint CDOs also retain their existing values. This is not propagation
to all actors, derived Blueprints, maps or saved assets.

Supported defaults: int, float, bool, string, double, name and the supported
generic reflected structs. Structs use validated native copies. New defaults
persist across asset Save/Reopen and later Blueprint compilation. The canonical
DSL model is updated only after success; repeat reimport is idempotent. Assets
remain dirty until saved. Body-only script changes can accompany a default change;
declaration metadata/structure changes cannot.

Migration policy is editor tooling; runtime does not migrate actor state
automatically. Normal runtime last-good reload remains in effect while the new
default declaration has not been explicitly applied. After migration, the new
script can pass the owner/CDO checks without restarting the editor.

The low-level Blueprint backend accepts the policy only for existing fields and
methods. Call the DSL service for stored-model preflight and metadata consistency.
There is no complete Undo/rollback promise for all DSL import operations.

Tests: Scripts/TestDSLDefaultMigrationAsset.ps1 and
Scripts/TestDSLDefaultMigrationPIE.ps1. Each uses disposable GUID-scoped source
and asset fixtures, with no changes to user projects.
