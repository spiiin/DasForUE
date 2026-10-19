# Natural method value validation — 2026-10-10

Source Unreal Engine 5.8.3; pinned daScript 0.6.4; Win64 Development.

TestMethodValues.ps1 -Cooked: PASS, including:

- Create and Reopen of GUID-scoped reflected enums/structs, an explicit registry,
  target/driver Blueprints and a test map.
- Natural enum and nested struct inputs/results in an ordinary DAS function.
- Enum and nested struct ref/out copyback, string contents including Unicode,
  independent previous values, and failed-callee recovery preserving ref data.
- Same-valued but distinct enum argument and wrong struct argument produce
  actual compilation errors. The valid driver compiles and executes.
- Actual PIE: succeeded=1, succeededWithWarnings=0, failed=0, notRun=0;
  natural calls and recovery pass, EndPIE releases persistent contexts.
- Cooked packaged executable: DAS_METHOD_VALUES_RESULT=PASS.

The first packaging attempt exposed an outdated runtime dependency list: the
fresh generated DAS files were absent from the game receipt. The test now removes
only its own game's cached Makefile.bin, regenerates rules/metadata and checks
both GUID source paths in the receipt before cooking. The repeated packaged run
passes through Pak/UFS file access.

Regression validation:

- TestActorComponent.ps1: Create/Reopen PASS with the previous common method,
  primitive/opaque value, reentry, recovery and property checks.
- TestEditor.ps1: DAS_FOUNDATION_RESULT=PASS, failures=0.
- TestBindingsGenerator.py: 84 tests, OK.
- GenerateBindings.ps1 -Check: PASS; native IR/module need no replacement.
- git diff --check: PASS. ThirdParty/daScript remains unchanged.

Logs: logs/method-values-Create.log, method-values-Reopen.log,
method-values-pie.log, method-values-cooked.log, actor-component-Create.log,
actor-component-Reopen.log, smoke-editor.log and bindings-export.log.
Automation report: logs/method-values-pie-report/index.json.

No general migration, container method adapter, natural native-object/math method
signature mapping or automatic reference-import discovery is included in this step.
