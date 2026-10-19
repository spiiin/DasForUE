# Interface event validation — 2026-10-09

- TestEditor.ps1: PASS, 476 checks, zero failures.
- TestDSLTypeRegistry.ps1 -AuthoredInterface -Cooked: PASS. Seven authored methods
  include scalar, zero-input and array/string event signatures. Five actual
  override event nodes exist across authored/native interface implementations.
- ProcessEvent scalar/array/Unicode dispatch and native Execute_* input/no-input
  calls change native Actor state. Array input ownership remains unchanged.
- Typed void adapter calls, existing return/out/ref graphs, metadata-only update,
  multiple-interface membership, save/reopen/reimport and actual game launch pass.
- TestDSLTypeRegistry.ps1 -AuthoredInterface: PASS after adding a body-reload probe.
  Changing AddScore from +amount to +amount*2 takes effect on the next event call
  without replacing the Blueprint class; restoring source and reopen pass.
- No reader/preprocessor/parser changes. General migrations/VM reentry stay
  deferred; unsupported event removal/rename is refused by stable-ID preflight.

Workspace evidence: work/interface-events-foundation.log,
work/interface-events-cooked.log and work/interface-events-reload.log.
