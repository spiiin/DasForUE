# Project script folder validation

2026-11-03, UE 5.8.3. Shared-agent job `rogue-script-folders-v3-20261103`, overall exit 1.

- Reflection/macro/source-path suite: PASS, 119 checks. New checks validate safe relative `.das` paths, traversal/empty-segment rejection, nested declaration inventory preserving IDs, short/explicit path lookup, module imports and a typed authored method reached through a nested catalog reference.
- Presentation did not complete. EXCEPTION: Rogue setup failed before PIE: [2026.11.03-02.14.59:434][ 29]LogTemp: Warning: DAS_DSL_AUTO_REIMPORT rejected BP_RogueCombatTarget: C:/src/ActionRoguelikeDas/Content/Das/Scripts/roguedemo/rogue_combat_target.das(47,85): [DAS:compile] RogueCombatTarget.RefreshStatus | field 'Health' not found in rogue_attribute_component::__ue_reference_f7b33ea74a16ded8e952dfe18bef9905 aka RogueAttributeComponentRef (+3 more on this line)
- Physical source groups: {'rogue': 26, 'roguedemo': 7, 'roguetest': 8}. No flat `.das` sources remain in the project Scripts root. Test templates were moved into `Tests/Das/roguetest`; preparation scripts and headless test staging were updated.
- Static validation: unique project filenames, all project module imports resolved, all catalog source paths existed, Python preparation scripts parsed. No handwritten native object wrappers or presentation-types imports remain.

The source filename index is built at loader startup and refreshed for reported hierarchy changes. Lookup does not scan directories or parse JSON. The catalog keeps relative folder paths; authored-reference annotations canonicalize to the unique module filename used by existing declaration/method/property keys. Stable IDs, import names and saved source names remain unchanged.
