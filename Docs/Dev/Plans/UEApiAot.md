# Guarded UE API AOT bridge

## First increment

`AddUEExtern` derives an AOT ABI description from the C++ function signature.
Functions using scalar values, UTF-8 strings and exactly one `das::Context*`
can emit `dasue::aot::Call<key, return, arguments...>`. Unsupported types,
references, managed snapshots and callback/block signatures remain `noAot`.
The script API and DSL accessor syntax stay unchanged.

The key includes the script function name and native ABI. Registration rejects
collisions; missing/stale keys fail with a regenerate/rebuild diagnostic.
The registry stores the same `TCheckedUEExtern::Invoke` pointer used by the VM.
Generated code never calls the original unguarded implementation directly.

Before resolving that pointer, the bridge checks the execution role and the
active game-thread context. The original accessor then enforces owner lifetime,
field type, write permission and bind-event restrictions. The physics-data-only
module remains isolated and has no AOT implementation in this increment.

## Focused acceptance

Queue through the shared build agent:

```text
test-suite ueaot1 ue-aot repo=C:\src\DasForUE2-main
```

The suite builds the emitter, generates `Private/Generated/UEGuardAot.cpp`
inside Unreal (so the native UE module is registered), then links that output
and performs one short runtime commandlet check. Current daScript emits C++
through `daslib/aot_cpp`; the build-time driver is `ue_aot_emit.das`.

The check requires linked native execution (`bUsedAot`), exercises scalar field
reads/writes and strings, and rejects missing/read-only/wrong-type fields,
missing/expired owners, absent/foreign contexts, worker-thread calls and
PhysicsPreSim/isolated-physics entry. It also checks recovery after refusal and
that unsupported object-handle ABI still stays closed. The runner audits the
common guarded-registration source contract. Full Foundation/PIE/packaged suites
are deliberately outside this focused increment.

## Next increments

- Owned object/value/snapshot ABI and generated wrapper annotations.
- Callbacks and authored methods, including ability and cue hooks.
- Per-project generation, dependency freshness and build/link integration.
- Whole-map Shipping acceptance that forbids gameplay VM fallback.

This first increment does not establish full GAS AOT or expose raw UObject
pointers/layout to generated code.

## Verified first increment

Build-agent job `ueaot-20261021c` completed with exit 0 on 2026-10-21:
generated and linked native C++, 15/15 focused runtime checks, and the guarded
registration source audit passed. The registry contained 182 eligible extern
ABIs; this is an eligibility count, not an independent runtime test of every API.
The emitter initially needed explicit AST/RTTI imports and a mutable borrowed
context pointer. The successful receipt and source/binary hashes are in
`logs/ue-aot-validation.json`, runtime details in `logs/ue-aot-runtime.log`.

The suite's manifest entry follows `build`, ensuring a fresh checkout builds
the generator before using it. Its explicit second build links the newly
emitted C++ before the runtime probe. No full PIE/packaged test was added.
