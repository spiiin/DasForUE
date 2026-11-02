# Typed interface calls - 2026-11-02

Target: UE 5.8.3 source, Win64 Development, daScript 0.6.4. Jobs ran serially
through the shared build agent against C:/src/DasForUE2-main.

- typed-interface-v4-20261102: reflection macros PASS, 36 checks. Fresh interface
  views have no generated method wrappers. Member dispatch, explicit ue_call,
  ref/out, array results, same-class native view arguments and typed class-to-native
  class-handle arguments execute successfully. Wrong names/types, raw receivers,
  stale static signatures and ambiguous bindings remain rejected. The isolated
  game world explicitly prepares its program before invoking gameplay methods.
- typed-interface-port-v6-20261102: real PIE presentation PASS (35 s, eight groups),
  effects PASS (34 s, six groups), point impact PASS (33 s, three groups). Exercises
  direct mesh/AnimInstance setters, action-host interfaces with gameplay tags,
  ActorComponent self calls, imported CollisionHit packing and physics impacts.
- typed-calls-final-20261102: Foundation PASS (45 s), generic parents PASS (17 s),
  native catalogue regeneration check PASS (7 s), interface contracts PASS (34 s),
  interaction PASS (26 s), projectile attack PASS (34 s), Overwhelm PASS (37 s).

Strict PIE harnesses inspect logs after process exit for runtime/timer/teardown
errors. Native declaration discovery adds ActorComponent and refreshes pending
SpringArm/owned-object reflection metadata; generated artifacts were not patched
by hand. No gameplay code moved into C++.
