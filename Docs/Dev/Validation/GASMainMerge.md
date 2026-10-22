# GAS integration into main

The merge combines main `bedf2a1` (including type adapters, cached access,
inherited callback dispatch, widget layouts and the InputActionValue call-bridge
fix) with GAS `0f8821f`. Both histories are retained by the merge commit.

Manual resolutions preserve both branches' additions in runtime includes,
DSL owner validation and public declarations. The playable Character uses the
main branch's typed HUD view and GAS-backed health values. No hand-written
GameplayTag special cases were reintroduced outside the adapters; reflection
registry data retains native identifier strings and is exempted narrowly from
that source-code lint.

Integration checks found and fixed redirected native-stderr handling in the
five old GAS runners, process-local cue-fixture route collisions after asset
tests, the API baseline's missing `__ue_gas_store_hit` entry, and missing explicit
cue search paths in the smoke project's DefaultGame.ini. Existing API
baseline records are unchanged. Test cue isolation does not delete saved assets.

## Build-agent validation

All builds and editor/game processes were run through the serialized build
agent against `<repo>-main`.

- `gasmerge-20261021b`: base/authored/scripted GAS, tasks/calculations,
  targeting, execution, cue hooks/assets and the end-to-end ability scenario
  passed. That job then exposed the old cue test's fixture collision.
- `gasmerge-20261021c`: corrected native cues passed (10 checks) and all
  Python tests passed (329). Foundation detected the single new API entry.
- `gasmerge-20261021e`: Foundation passed after deliberate baseline update.
- `gasmerge-20261021d`: real PIE, packaged Development, instrumented Shipping
  and MethodValues passed. Full automation had 36 successes plus one success
  with a cue-path warning; the strict gate correctly rejected that result.
- `gasmerge-20261021f`: packaged Development/Shipping and Foundation passed; the new DeveloperSettings cue section did not remove the warning in this engine build.
- `gasmerge-20261021g`: after setting explicit sample/test cue search paths through the supported AbilitySystemGlobals section,
  cached builds and recooked Development/Shipping archives, Foundation, full
  automation with zero warnings and inherited handlers passed. The targeted
  rerun reused the MethodValues/component fixtures verified in job d through
  the standard runner's NoDependencies mode; it did not relax warning checks.

Shipping still runs GAS/cue methods through the VM; its separate pure-DAS AOT
check requires real native execution without fallback. Full GAS AOT and
networking are outside this merge's acceptance scope.

Runtime logs, screenshots, binary/archive hashes and source hashes are recorded
in `logs/gas-main-merge-validation.json`. The record also verifies that the
selected adapter, guard, callback and access-cache files are unchanged from
main `bedf2a1`. Generated test assets are not committed.
