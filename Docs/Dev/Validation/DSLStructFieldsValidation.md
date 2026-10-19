# Struct fields/defaults validation — 2026-10-06

UE 5.8.3 / pinned daScript 0.6.4 on Windows Win64 Development.

- TestEditor.ps1: 373 foundation checks, PASS, failures=0.
- TestPackaged.ps1: build/cook/stage/archive and packaged launch, 373 foundation checks, PASS, failures=0.
- TestDSLStructFieldsAsset.ps1: Create and Reopen in separate processes, PASS. Native struct defaults/readonly flags, ProcessEvent self read/write, incompatible assignment rejection without mutation, default change rejection, actor override and CDO preservation after reinstance, saved Blueprint reopen.
- TestDSLStructFieldsPIE.ps1: real PIE, Automation succeeded=1, succeededWithWarnings=0, failed=0, notRun=0. Default, self access, body-only reimport, rejected default mutation, adding a field/reinstance, actor override/CDO preservation and GC.
- git diff --check: passed.

Default import regression: Unreal silently skips unknown properties. A reflected field-name/duplicate/shape preflight now runs before ImportText. Unknown fields, trailing data and invalid enum values are tested. No source preprocessor or reader macro added.

These tests do not establish production AOT/JIT support or migration semantics. Existing default/type changes still require an explicit future migration policy.