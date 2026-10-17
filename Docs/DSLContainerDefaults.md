# Container class defaults

Class properties accept ordinary gen2 array and table literals, with scalar
calculations and pure scalar helper functions in their elements/keys/values:

```das
def start_value(seed:int):int { return seed * 4 }

[ue_class(module_id="gameplay",Blueprintable)]
class Example : Actor {
    @ue_property=(EditAnywhere,BlueprintReadWrite)
    Seed:int=2
    @ue_property=(EditAnywhere,BlueprintReadWrite)
    Numbers:array<int> = [1,start_value(self.Seed)]
    @ue_property=(EditAnywhere,BlueprintReadWrite)
    Tags:table<string> = {"red","blue"}
    @ue_property=(EditAnywhere,BlueprintReadWrite)
    Weights:table<UENameValue;double> = {ue_name_value("Speed")=>2.5lf}

    defaults=ue_defaults() {
        self.Numbers=[self.Seed,start_value(self.Seed)]
    }
}
```

Use a space between the closing `>` and `=`; otherwise daScript lexes `>=`.
An assignment in ue_defaults takes precedence over the inline initializer.
Declaration property groups follow the same path. No reader macro is needed.

## Supported defaults

- Arrays of supported int/float/double/bool/string/name/enum/math values.
- Sets of supported int/string/name/enum keys.
- Maps with supported int/string/name/enum keys and scalar/name/enum/math values.
- Existing container types keep implicit empty defaults. Empty array/table
  literals are also supported, including for other supported carrier families.

Enum entries must use a member of their declared enum. FName values use
ue_name_value(string). Math values use existing ue_vector/ue_vector2d/
ue_rotator/ue_transform constructors. Scalar leaves may use self scalar source
defaults and audited helper calls. Source reads do not inspect manually edited
CDOs or live objects. Whole-container helper results, container indexing in
calculations, nonempty object-reference/object-key/opaque-struct defaults,
native field defaults and explicit inherited container override declarations
remain outside this slice. Parent default propagation uses the existing merge.

## Import, merge and persistence

The collector parses literal container construction as data and evaluates only
scalar leaves through the existing bounded evaluator. Looping, recursive,
unsafe, global-state and UObject/World calls remain rejected. Evaluator closures
and VM tokens are never saved. Resolved defaults use a JSON array in the existing
string default slot: flat items for arrays/sets and [key,value] pairs for maps.
Existing empty defaults and IR/JSON schema versions are unchanged.

The native parser validates JSON types, finite/range-correct numbers, enum
members, math values, FName lengths and duplicate set/map identities using
Unreal equality. Item, string, wire and snapshot limits apply before publication.
Duplicate FName spellings that compare equal are rejected. Invalid defaults
reject import before changing the working Blueprint.

Defaults merge treats each container as one value. A CDO/template/instance that
still equals the old source default receives the new value. A manually modified
container is preserved in full; individual keys/elements are not merged. Failed
publication uses the existing transaction/rollback. Native container defaults
are written with exact numeric text, including when merge refreshes Blueprint
variable metadata, so recompilation preserves double precision.

At runtime, objects use the saved Unreal CDO/template values. Re-evaluating source
for validation does not reapply defaults. No defaults evaluator runs at BeginPlay.

## Examples and checks

`dsl_computed_defaults.das` contains arrays, sets, string/FName/enum maps, vectors,
computed elements and a double precision fixture. TestComputedDefaults.ps1
checks initial import, helper-result changes, whole manual container overrides,
invalid entries, fresh source/runtime validation, save/reopen and Blueprint
recompile. The shared cooked probe checks the same values on saved and newly
spawned objects.


## Validation 2026-10-17

Passed Editor and Win64 Development builds, computed-defaults Create/Reopen/
Recompile, saved/fresh object runtime checks, invalid-source refusal and exact
double roundtrip. MethodValues Reopen passes on the earlier enum/object-container
map. Combined cook/stage/pak/archive and the packaged computed-defaults probe
pass; packaged foundation reports zero failures. All final commandlet and game
validation logs have no Error/Fatal entries. This change does not claim a new
dedicated container-defaults PIE automation run.

Map: `/Game/DasComputedDefaults/69b03d9493f14dd99efd97d34e24fe68/Map`.
Archive: `Build/ContainerDefaultsPackaged`.

Evidence in logs:

- `container-defaults-collection-build.log`, `container-defaults-world-build2.log`,
  `container-defaults-game-build.log`, `container-defaults-receipt.log`.
- `container-defaults-{Create,Reopen,Recompile}.log`,
  `container-defaults-final-suite2.log`.
- `container-defaults-method-regression.log`.
- `container-defaults-package.log`, `container-defaults-package-run.log`,
  `container-defaults-runtime-packaged.log`,
  `container-defaults-foundation-packaged.log`.

The ActorComponent and inherited/sibling scalar regressions also pass. The test
world is now strongly retained across compilation/GC; actor references are
refreshed before attaching the test component. Native struct integration uses
the current SDK's isFixedArray() API when checking by-value source fields.
Validation used Windows NullRHI; rendered gameplay, Shipping, network and other
platforms were not tested. Re-run TestComputedDefaults.ps1, optionally -Package.
