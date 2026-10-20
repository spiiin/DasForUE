# Inherited handler references

Worktree `C:/src/DasForUE2-handlers`, branch `inherited-handlers`, based on main merge `52fe60b`.

One nearest-declaration traversal is shared by managed callback validators, typed delegate references, multi-capture timers and dynamic event checks. The traversal crosses authored parents and their script modules and preserves ambiguity/static/signature errors without falling back to a less-derived method. Hidden callback overrides inherit the internal event contract automatically. Script-only inherited helpers receive a child-owned forwarding adapter through existing Super lowering; parent modules and their emitted models are not mutated.

Only methods actually named in managed callback positions receive dispatch adapters; unused inherited signatures do not force extra type imports. Managed callbacks use generated native-reflection dispatch adapters with exact-contract evidence, sharing the method/free-handler ABI adapter. The original method exports remain the implementations called by Unreal thunks; dispatch exports resolve the actual object's method, which avoids freezing base registrations to a base VM entry and keeps Super non-recursive. Inline callbacks retain their existing entry identities and reload transfer rules. Callback targets become virtual by default without changing explicit Blueprint visibility; `BlueprintEvent=false` stays final. Unrelated visible functions retain their declared policy. Existing Blueprints with final hidden handlers need new asset paths because the current Apply preflight rejects callable/event role migration.

Targeted build-agent validation completed on 2026-10-20:

| Agent request | Passed checks |
| --- | --- |
| `inherit-finish-20261020` | Editor build, Foundation (zero failures), operation keys (26), inherited handlers (34), hidden callbacks (25), handler references (34). The event suite exposed anonymous-module argument type lookup; its failed result was not accepted. |
| `inherit-events-20261020` | Exit 0: rebuilt the final source, inherited handlers (34), event checks (51), inline callbacks (26). |

The final correction resolves anonymous `::Type` declarations in the in-progress module rather than the dependency group. Compiler self/Super views are excluded from public component-view selection; actual public-view ambiguity is still rejected. The existing dynamic event fixtures exercise both cases.

The 34 new checks cover imported three-level and same-file (without lifecycle) lookup, hidden and visible override contracts, unknown methods and incompatible parameter/arity/ref-out overrides, Apply of base/middle/leaf, base-registered timers and input dispatching to the leaf, scalar/multi-capture payloads, delegate results and ref/out with Super, Bind/Add/Remove identity, inherited widget-handler reflection, rejected and accepted reload, and owner cleanup. Widget binding is compile checked; runtime reflection of its inherited handler is checked without a visible UI.

Final inherited/event/inline receipts were checked against their local file SHA256 maps after the run. Earlier Foundation/hidden/reference/key checks predate only the final anonymous-module lookup correction; they were not repeated after that isolated fix.

 The new runner freezes plugin sources, script modules, all three imported fixtures, test runner, extern baseline and binaries/manifests; it writes `logs/inherited-handlers-validation.json`. Full automation suites, PIE and packaged builds were not run for this change.
