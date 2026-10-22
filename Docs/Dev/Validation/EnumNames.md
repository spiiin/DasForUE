# Typed input and collision names — Phase 3.3

Worktree: `<repo>-ui`, branch `widget-layout`.

`require ue_collision_profiles` exposes a project-configured `CollisionProfile` enum. Its checked conversion returns the existing context-owned FName transport, usable by native self methods; generated wrappers use `UEName(value=ue_name_text(name))`. IDs hash case-normalized UTF-8 names, independent of configuration order. Label normalization handles punctuation, leading digits, keywords and collisions without confusing the original names. The enum is configuration metadata, not a reflected UEnum or property storage type. Restart the editor/game process after configuration edits to refresh symbols; conversion also rejects removed profiles and forged IDs.

Enhanced Input retains string overloads, with `DAS_DSL_STRING_TRIGGER` for literal strings in user source. Typed `TriggerEvent` values reject misspellings at compile time. Dynamic strings and binding modules are excluded from the lint.

The pickup example uses a typed collision profile. Test-only collision profiles are configured in this worktree's smoke project, including two names normalizing to the same label and a daScript keyword.

## Targeted checks

Request through the single shared build agent:

```text
test-suite dsl33-ui-8fb2a095413f dsl-enum-names,dsl-inline-callbacks repo=<repo>-ui
```

Build agent exit code: **0**. Both focused suites passed:

- **15 enum-name checks:** typed helpers and trigger overloads compile; extern baseline matches; unknown profiles, integers, strings and misspelled triggers are rejected; every loaded project profile has a symbol; normalized labels remain distinct; native component FName roundtrip, custom profiles, forged IDs, escaped/keyword names and the migrated pickup compile probe pass. The runner also requires the legacy string-trigger warning.
- **26 inline callback regression checks:** Apply, runtime timer/input delivery, stable generated identities, forbidden captures, hot reload, rejected-reload preservation and cleanup.

Early runs exposed errors in the new test harness (reflection FName conversion, minimum authored-class requirements and VM isolation). These were corrected before the final passing run. Staged formatting and `git diff --check` passed.

The enum-name runner freezes plugin sources, script modules, fixtures, smoke configuration, extern baseline, runner and engine/project binary/module manifests during execution. Receipts: `logs/enum-names-validation.json`, `logs/inline-callback-validation.json`. The commandlet saves the actual foundation manifest to `logs/enum-names-externs.txt`.

PIE, packaging and full suites are outside this change's checks. All build/editor work goes through the existing build-agent queue.
