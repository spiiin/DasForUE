# Typed-reference and gameplay recheck

2026-11-03, UE 5.8.3. All listed shared-agent jobs completed with exit 0.

| Job | Validation |
| --- | --- |
| `rogue-recheck-v1-20261103` | Reflection/reference macros 119; projectile attack 7; presentation 8; effects 6; Overwhelm 7; point impact 3 |
| `rogue-recheck-handlers-v2-20261103` | Inherited callback handlers and reload 35 |
| `rogue-recheck-ai-v3-20261103` | Real-PIE AI 7 |

The earlier missing `Health`/method failures are no longer reproduced. Current accessor/method generation uses module-local private helpers, checks visibility before reusing helpers, and reconstructs imported private helper calls locally rather than depending on sibling modules. This recheck validates that shared working-tree implementation, including typed native/authored aliases and grouped source paths.

Two separate fixture failures were corrected:

- The inherited-handler test now prepares initial programs and edited source candidates outside gameplay, matching the editor preparation queue. Invalid reload candidates remain visible as ReloadError while the previous VM keeps working. All reload assertions pass.
- AI setup used Python `set_editor_property` to write HealthMax/Health on a cached ActorComponent. Editor property notification reconstructed the component: diagnostics showed the cached component owner became None while native damage broadcasts still arrived (5 observed broadcasts including the direct probe). The fixture therefore observed a detached component. Setup now invokes the test-only DAS `ConfigureHealth` helper through reflected typed runtime access and asserts both component identity/owner and initial live values. The health component is not reconstructed, real AI projectile damage lowers player health, and all sight/hearing/stun/occlusion/memory/death assertions pass.

No gameplay compilation, weakened type checks or manual damage workaround was introduced. The health setup helper is confined to roguetest and its canonical test template. AI damage continues through actual Unreal collision, ApplyDamage and the managed owner-event handler.
