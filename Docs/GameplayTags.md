# GameplayTags

`require ue_gameplay_tags` provides `GameplayTag` and `GameplayTagContainer` as
owned value views over native `FGameplayTag` / `FGameplayTagContainer`. Authored
class and struct fields of these types become real UE tag properties, edited with
the normal tag picker; they are not JSON or string carriers.

```das
require ue_dsl
require ue_gameplay_tags

[ue_class(Blueprintable)]
class TaggedItem : Actor {
    properties=ue_properties(EditAnywhere,BlueprintReadWrite) {
        var Kind:GameplayTag
        var ItemTags:GameplayTagContainer
    }
    [ue_function(BlueprintCallable)]
    def ConfigureSword():void {
        self.Kind=ue_tag("Item.Weapon.Sword")
        self.ItemTags=ue_tags(self.Kind)
    }
    [ue_function(BlueprintPure)]
    def Matches(query:GameplayTag):bool {return ue_tags_has(self.ItemTags,query)}
}
```

Full example: `gameplaytag_items.das`; authored struct: `gameplaytag_item_types.das`.
Tags come from the project's dictionary, for example
`Examples/GameplayTags/DefaultGameplayTags.ini` copied to the project's `Config/`.

## API

- `ue_tag(name)`: registered tag; an unregistered name is a script error.
  `ue_find_tag(name)` returns an empty tag instead. `ue_tag()` is the empty tag.
- `ue_tag_valid(tag)`, `ue_tag_name(tag)`.
- `ue_tag_matches(tag, parent, exact=false)`: native `MatchesTag` / `MatchesTagExact`;
  `Item.Weapon.Sword` matches `Item.Weapon` unless `exact`.
- `ue_tags()`, `ue_tags(a)`, `ue_tags(a, b)`, `ue_tags(array<GameplayTag>)`.
- `ue_tags_add(tags, tag)`, `ue_tags_remove(tags, tag)`: replace the variable's value;
  copies made earlier are unaffected. Adding or removing an empty tag is an error.
- `ue_tags_count`, `ue_tags_has(tags, tag, exact=false)`,
  `ue_tags_has_any` / `ue_tags_has_all(tags, query, exact=false)` with native
  container semantics (`has_all` of an empty query is true, `has_any` is false).
- `ue_tags_names(tags)`: detached sorted `array<string>`.

Values may be stored in globals and survive GC. Tag fields, method parameters,
results and `@ue_ref` parameters use the generic struct frame path. Tag fields
in authored classes and structs use implicit empty defaults; initializers are
rejected. Writes that bypass this API (`ue_struct_with_*`, array resize on the
raw snapshot) are validated and rejected when they would produce an unregistered
tag or an inconsistent container.

Not covered yet: `GameplayTagQuery`, `IGameplayTagAssetInterface`, tag-change
events, declaring the dictionary from DSL, map values of tag type.

## Validation

`Scripts/TestGameplayTags.ps1` builds the smoke editor and runs the
`DasGameplayTags` commandlet with `Tests/GameplayTags/probe.das`: dictionary load,
authored struct with tag fields, authored Actor with tag fields and method
signatures, real native property types, matching/container/copy/guard checks
and reflected calls, native readback and retained values after GC (7 checks).
Report: `logs/gameplay-tags-validation.json`. PIE, packaged and cooked runs are
not part of this check.
