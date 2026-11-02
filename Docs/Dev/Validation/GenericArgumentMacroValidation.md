# Generic argument macro - 2026-11-02

UE 5.8.3 source / Win64 Development / daScript 0.6.4. Shared-agent jobs:

- generic-ue-arg-v3-20261102: reflection macro suite PASS (47 checks, 17 s).
  New checks cover scalars with single evaluation, dynamic field names evaluated
  once, detached CollisionHit roundtrip (nested vector, FName, byte and Actor),
  input unchanged after the native copy is mutated, arrays, math values and typed
  class arguments. Invalid frame/name types and plain DAS structs are compile
  errors. An incompatible native property type is rejected before invocation;
  the fixture's native side-effect counter remains unchanged.
- Same job: real-PIE point impact PASS (59 s, three groups), including actual
  point-damage payloads, physics impulse, zero-damage/zero-impulse cases and
  post-exit runtime/teardown log checks.
- generic-ue-arg-final-20261102: Foundation PASS (20 s), projectile attack PASS
  (55 s, seven groups), Overwhelm PASS (58 s, seven groups).

The macro reuses existing codecs and generated packers. No alternate serializer
or runtime reflection walk was introduced. ue_arg_collision_hit was removed,
with no compatibility alias. The generic point-damage wrapper uses ue_arg directly.
