# Declarative input binding validation

Shared build-agent job: `input-macro-v2-20261102`, UE 5.8.3, 2026-11-02.

- `dsl-reflection-macros`: PASS, 58 checks, including 11 input annotation checks. Exercises multiple triggers and rejects unknown/wrong-type action properties, unknown/missing/repeated triggers, unsupported/ref handler inputs, arguments to `ue_bind_inputs`, duplicate action/trigger pairs and an empty setup.
- `rogue-interaction`: PASS in real PIE, including E dispatch through the annotated controller.
- `rogue-actions`: PASS, 10 groups, including Shift sprint and session cleanup.
- `rogue-projectile-attack`: PASS, 7 groups, including LMB firing, windup, cooldown and cancellation.

The port character/controller use annotations. Generated calls reuse the existing checked Enhanced Input callback path; no runtime metadata scan or separate input dispatcher was introduced. Context acquisition and readiness remain explicit in script. InputAction and InputMappingContext assets retain their existing reflected properties/defaults.
