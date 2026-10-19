# Gameplay Tags

`require ue_gameplay_tags` gives scripts `GameplayTag` and `GameplayTagContainer` as owned values over native `FGameplayTag` / `FGameplayTagContainer`, with hierarchical matching and container operations. Fields of these types in authored classes and structs become real UE tag properties, edited with the normal tag picker.

## Declaring tag fields

```das
options gen2
require ue_dsl
require ue_gameplay_tags

[ue_class(Blueprintable)]
class TaggedItem : Actor {
    properties=ue_properties(EditAnywhere,BlueprintReadWrite) {
        // Native UE tag and tag-container properties, editable in the normal tag picker.
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
    [ue_function(BlueprintCallable)]
    def Exchange(@ue_ref var value:GameplayTag&):void {
        let previous=self.Kind
        self.Kind=value
        value=previous
    }
}
```

Authored structs work the same way:

```das
[ue_struct(BlueprintType)]
struct ItemTagRecord {
    @ue_property=(EditAnywhere,BlueprintReadWrite)
    Kind:GameplayTag
    @ue_property=(EditAnywhere,BlueprintReadWrite)
    Tags:GameplayTagContainer
}
```

Examples: `Plugins/DasForUE/Content/Das/Scripts/gameplaytag_items.das` and `gameplaytag_item_types.das`.

- Tags can be used as fields, method parameters, results and `@ue_ref` parameters.
- Tag fields always default to empty; initializers on tag fields are rejected at compile time. Set values in Blueprint defaults, the Details panel, or code.
- Tag values may be stored in globals and survive GC.

## The tag dictionary

Tags come from the project's normal GameplayTags dictionary. The plugin does not declare tags from the DSL. The example dictionary is `Examples/GameplayTags/DefaultGameplayTags.ini`; copy it into your project's `Config/` folder:

```ini
[/Script/GameplayTags.GameplayTagsSettings]
ImportTagsFromConfig=True
+GameplayTagList=(Tag="Item.Weapon.Sword",DevComment="DasForUE item example")
+GameplayTagList=(Tag="Item.Consumable.Potion",DevComment="DasForUE item example")
```

## API

| Function | Behavior |
| --- | --- |
| `ue_tag(name)` | Registered tag; an unregistered name raises a script error. |
| `ue_find_tag(name)` | Like `ue_tag`, but returns an empty tag for an unregistered name. |
| `ue_tag()` | Empty tag. |
| `ue_tag_valid(tag)`, `ue_tag_name(tag)` | Validity and full name. |
| `ue_tag_matches(tag, parent, exact=false)` | Native `MatchesTag` / `MatchesTagExact`: `Item.Weapon.Sword` matches `Item.Weapon` unless `exact`. |
| `ue_tags()`, `ue_tags(a)`, `ue_tags(a, b)`, `ue_tags(array<GameplayTag>)` | Build a container. |
| `ue_tags_add(tags, tag)`, `ue_tags_remove(tags, tag)` | Replace the variable's value; earlier copies are unaffected. Adding or removing an empty tag is an error. |
| `ue_tags_count(tags)` | Number of tags. |
| `ue_tags_has(tags, tag, exact=false)` | Container contains (or matches) the tag. |
| `ue_tags_has_any(tags, query, exact=false)`, `ue_tags_has_all(tags, query, exact=false)` | Native container semantics: `has_all` of an empty query is `true`, `has_any` is `false`. |
| `ue_tags_names(tags)` | Detached, sorted `array<string>`. |

```das
let sword=ue_tag("Item.Weapon.Sword")
let weapon=ue_find_tag("Item.Weapon")
if(ue_tag_matches(sword,weapon)) {
    // hierarchical match
}
var tags=ue_tags(sword)
ue_tags_add(tags,ue_tag("Item.Consumable.Potion"))
print("{ue_tags_count(tags)} tags: {ue_tags_names(tags)}\n")
```

## Safety

Writes that bypass this API (for example `ue_struct_with_*` or resizing the raw snapshot) are validated, and rejected if they would produce an unregistered tag or an inconsistent container.

## Limitations

Not supported yet: `GameplayTagQuery`, `IGameplayTagAssetInterface`, tag-change events, declaring the dictionary from DSL, and maps whose values are tags.

Try it: `.\Scripts\TestGameplayTags.ps1` builds the editor and runs the tag scenario (dictionary, authored struct/Actor fields, matching, containers, reflected calls).

See also: [Structs](../DSL/Structs.md), [Classes](../DSL/Classes.md), [Built-in types](../DSL/BuiltinTypes.md), [DataTables and curves](DataTablesAndCurves.md)
