# Dynamic event operators

Worktree: `<repo>-ui`, branch `widget-layout`.

The normalizer recognizes `self.Event +=/-= self.Method` and declared component sources inside ordinary method bodies. It preserves compound assignments involving data fields. Hidden callback discovery promotes named methods through the existing path, and lowering reuses native/authored event signature validation plus an exact UE frame/layout check for direct UFunction dispatch. Invalid names, payloads and return types fail before Apply. The runtime checks the actual multicast property and handler again.

A single guarded extern mutates session-owned registrations. Source and target references are weak; no field pointers survive calls. Add/remove are idempotent for the source/event/method triple. Removing one handler preserves other registrations. Existing session teardown revokes direct bindings before replacing a VM; rejected candidates retain old bindings. Dynamic bindings are not replayed after successful reload: the owner must invoke its registration method again. Declarative `BindEvents` remains queued and automatically reinstalls its declarations.

The syntax supports events on self and declared native/authored components and handlers declared on the same class. It excludes arbitrary object expressions, owner contracts, inline blocks and inherited handler discovery. Direct handlers must have an exact supported value signature, without mutable ref/out. Construction, subscription collection and EndPlay updates are guarded; the extern uses the existing Unreal execution-role guard.

## Targeted validation

The single shared build agent completed this request:

```text
test-suite dsl-dynamic-ui-4829cdf231 dsl-event-checks,dsl-enum-names repo=<repo>-ui
```

`logs/agent/dsl-dynamic-ui-4829cdf231.done`: **0**. Editor build passed, followed by **66 targeted checks**:

- **51 event checks** (27 existing and 24 dynamic): compile-time unknown source/handler, wrong types, non-void and direct object-widening rejection; authored component validation from source; hidden handler metadata; Apply; conditional registration and idempotence; native scalar, authored tuple and component delivery; ordinary data compound assignment; selective removal; rejected-reload preservation; accepted-reload cleanup and explicit rebinding; harmless EndPlay removal, context release and Destroy cleanup.
- **15 enum-name regression checks**, including equality of the complete foundation extern baseline after adding one guarded function.

The focused runners freeze source and binary hashes during execution. The event runner additionally freezes both event fixtures, its runner and the extern baseline. Receipts are `logs/event-check-validation.json` and `logs/enum-names-validation.json`; local worktree hashes were rechecked before commit. Staged formatting and `git diff --check` passed.

All build/editor execution used the queue. Full suites, PIE and packaging were not run for this change.
