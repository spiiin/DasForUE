# Result propagation and native input lifecycle validation

2026-11-03, UE 5.8 Windows editor/standalone PIE, shared build agent.

- result-try: PASS, 8 groups including seven compile-negative cases.
- dsl-inherited-handlers: PASS, 35 checks, including inherited Result helper with
  reflected table and module-private container/concrete-generic dependencies.
- dsl-reflection-macros: PASS, 119 checks; script-staging: PASS.
- ActionRoguelike actions/presentation/Overwhelm: PASS, 10/8/7 groups.
- rogue-input: PASS in two fresh PIE/editor runs, 4 groups each. Native
  SetupInputComponent/SetupPlayerInputComponent, initial startup,
  UnPossessed cleanup, held sprint cancellation, same/different pawn possession,
  no duplicated interaction/fire and no input to the old pawn were exercised.
- All 92 example/fixture modules compile; strict runtime/teardown gates passed.

Input hooks are optional zero-argument reflected events on ADasPlayerController,
ADasPawn and ADasCharacter. The script annotation is ue_function(BlueprintEvent).
ue_bind_inputs/ue_unbind_inputs use the same checked annotation expansion.
Inherited implementation dependencies are copied privately into the consuming
module, preserving concrete generic arguments without widening visibility.

Agent receipts: rogue-input-hooks-v5-20261103,
rogue-input-macros-v1-20261103, rogue-input-hooks-v10-20261103,
rogue-input-hooks-v11-20261103. See docs/Guide/ResultPropagation.md and
docs/Guide/Gameplay/Input.md for API/lifetime contracts.
