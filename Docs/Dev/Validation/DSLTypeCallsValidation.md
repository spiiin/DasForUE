# DSL source type-reference validation — 2026-10-09

- TestEditor.ps1: PASS, 494 checks, zero failures. Includes type-token and
  short-string compilation, native names with a selected folder, wrong kinds,
  missing names, nonliteral marker expressions and wrong marker arity. Negative
  tests assert the intended semantic diagnostic. Enum local copy/reassignment
  exercises the corrected token value representation and external return ABI.
- TestDSLTypeRegistry.ps1 -TypeSyntax -Cooked: PASS. Authored renamed enum/struct
  names retain explicit registry IDs; source body constructors resolve correctly
  and native names work in the same registry scope. Editor create/reopen and game.
- TestDSLTypeRegistry.ps1 -Interfaces -Cooked: PASS. Twelve native interface call
  sites use ue_type tokens. Real Blueprint dispatch covers scalars, double arrays,
  multiple out/ref, Unicode maps and sets, failure/release checks, save/reopen/game.
- TestDSLTypeRegistry.ps1 -AuthoredInterface -Cooked: PASS. Authored interface type
  token and native short-string membership work during create/reopen and game;
  existing method/event dispatch, metadata and body reload checks remain passing.
- TestDSLTypeRegistry.ps1 -TypedObjects -Cooked: PASS. Class-token nullable casts
  and short-name class checks preserve null behavior; native type guards, forged
  wrapper rejection, object fields, GC/expiry and reopen/game probes remain passing.
- git diff --check: PASS. No ThirdParty changes. General migration/reentry work
  remains deferred; this is source reference lowering, not TScriptInterface values
  or generated typed interface method wrappers.

The new execution probe exposed an existing UEEnumValue local-copy crash. Fixed
the uint64 value representation, clone operation and matching native return carrier.
The native interface negative test also had a stale UInterface diagnostic check;
it now checks the shared resolver's Expected interface type error.

Evidence: workspace work/dsl-type-calls-foundation.log, the four
dsl-type-calls-<mode>.log transcripts and matching <mode>-cooked.log game logs.
Implementation and syntax: DSLTypeCalls.md. These changes are not committed.
