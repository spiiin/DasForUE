> Current implementation: [shared reflected defaults](DSLDefaultValues.md).
> The validation notes below also retain evidence from earlier incremental slices.

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
- Sets of supported int/string/name/enum/object keys, including asset defaults.
- Maps with supported int/string/name/enum/object keys and scalar/name/enum/math/asset/structured values.
- Existing container types keep implicit empty defaults. Empty array/table
  literals are also supported, including for other supported carrier families.

Enum entries must use a member of their declared enum. FName values use
ue_name_value(string). Math values use existing ue_vector/ue_vector2d/
ue_rotator/ue_transform constructors. Scalar leaves may use self scalar source
defaults and audited helper calls. Source reads do not inspect manually edited
CDOs or live objects. Whole-container helper results, container indexing in
calculations, opaque-struct defaults,
explicit inherited container override declarations
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

## Asset reference defaults

Native object-reference arrays and FName-to-object maps accept top-level assets:

```das
@ue_property=(EditAnywhere,BlueprintReadWrite)
Assets:array<CurveAssetRef> = [
    ue_asset_default("/Game/DasAssetDefaults/ArrayAsset.ArrayAsset"),
    ue_asset_default("")
]
@ue_property=(EditAnywhere,BlueprintReadWrite)
AssetMap:table<UENameValue;CurveAssetRef> = {
    ue_name_value("Main")=>ue_asset_default(default_map_asset()),
    ue_name_value("NoneValue")=>ue_asset_default("")
}
```

CurveAssetRef is a native UObject wrapper constrained to /Script/Engine.CurveFloat
(see ue_default_assets.das). ObjectRef also works when no narrower constraint is
needed. These expressions may also appear in ue_defaults assignments. Paths may
use audited scalar helpers; an empty path means a null object reference. The
marker is declaration data, not a runtime asset-loading function.

For table assignments inside ue_defaults, parenthesize the literal: self.AssetMap=({ue_name_value("Main")=>ue_asset_default(path)}). The gen2 grammar reserves a bare opening brace at statement level for blocks.

The wire leaf is {"asset":"/Game/Package.Object"}, including {"asset":""} for
null. A plain string is not accepted as a reference leaf. Loading validates the
declared native class/interface, top-level object path and asset identity.
Classes, CDOs, live actors, subobjects and missing/incompatible assets are
rejected. Asset sets, object map keys and other supported key families now use the shared
codec too. Authored reference types remain deferred for nonempty defaults. FName duplicates
are rejected under Unreal equality, including differently cased spellings.

FDasDefaultRoots reports the references in staged FInstancedStruct values to GC.
The parser retains its candidate while loading subsequent assets, and Blueprint
import retains parsed containers through property creation and compilation.
Migration uses its existing rooted staging plan. Published containers are real
reflected UPROPERTY values on the CDO/templates/instances, so GC, serialization
and cooking follow Unreal's normal hard-reference dependency handling. There is
no permanent global asset root. Whole-container manual-override merge semantics
also apply to these references.

TestComputedDefaults creates two small UCurveFloat assets referenced only by
the containers. It verifies typed paths, null entries, helper paths, source
changes, manual empty-container preservation, invalid source refusal, GC,
save/reopen/recompile and fresh instances. A separate GC-only asset proves a
rooted default snapshot retains its object and releases it when cleared. The
packaged probe reads both curve payloads through saved/new object properties
after forced GC, without separately loading those assets by path.

### Asset-container validation (2026-10-17)

Passed Editor and Win64 Development builds; Create/Reopen/Recompile; typed and
invalid-path checks; whole manual-container preservation; snapshot retain/release
GC checks; saved CDO/instance GC; fresh Spawn. Cook/stage/pak/IoStore/archive and
the packaged defaults probe pass. Packaged foundation reports PASS failures=0.
Final commandlet and packaged game logs have no Error/Fatal entries.

Map: /Game/DasComputedDefaults/c7a18407288d49e78e237c03fa49509c/Map.
Archive: Build/AssetContainerDefaultsPackaged.
Evidence: logs/asset-container-defaults-{Create,Reopen,Recompile}.log,
asset-container-defaults-editor-build4.log, asset-container-defaults-receipt.log,
asset-container-defaults-package.log, asset-container-defaults-package-run.log,
asset-container-defaults-runtime-packaged.log,
asset-container-defaults-foundation-packaged.log.

The actual IoStore index (logs/asset-container-defaults-iostore.csv) contains
DasAssetDefaults/ArrayAsset.uasset and DasAssetDefaults/MapAsset.uasset and omits
DasAssetDefaults/GCOnly.uasset. Thus the two published container references cook
the required assets without cooking the unrelated snapshot-only fixture.
Validation used Windows Development with NullRHI; no new dedicated PIE or Shipping
run is claimed for this slice.