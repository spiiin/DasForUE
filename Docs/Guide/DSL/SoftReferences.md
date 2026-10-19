# Soft References

Soft references name an asset or class by path without loading it. In the DSL they are typed values, `$UESoftObject<T>` and `$UESoftClass<T>`. They become real `FSoftObjectProperty` / `FSoftClassProperty` fields and pins, so the asset picker, serialization and cook dependency tracking all work as they do in Unreal.

## Declaring soft types

`require ue_soft_references` for the shared aliases:

| Alias | Definition | Unreal type |
| --- | --- | --- |
| `SoftObject` | `$UESoftObject<ObjectRef>` | `TSoftObjectPtr<UObject>` |
| `SoftActorClass` | `$UESoftClass<ActorRef>` | `TSoftClassPtr<AActor>` |

To constrain the asset type or the class base, define your own alias with a narrower native object wrapper:

```das
options gen2
module game_soft_types public
require ue_soft_references public
require ue_default_assets public

typedef SoftCurve = $UESoftObject<CurveAssetRef>
typedef SoftMeshActorClass = $UESoftClass<SoftMeshActorRef>
```

The constraint must be a loaded native class that is not an interface. The referenced asset or class itself can be a Blueprint. Identical aliases resolve to the same nominal type. The generated native API modules emit the aliases they need. Shared aliases usually go in an imported module, but `ue_soft(type<T>, ...)` also accepts a soft typedef declared in the same file.

## Example

```das
options gen2
require ue_dsl
require ue_soft_references

[ue_class(Blueprintable)]
class Example : Actor {
    properties = ue_properties(EditAnywhere, BlueprintReadWrite) {
        var Asset:SoftObject = ue_soft(type<SoftObject>, "/Engine/BasicShapes/Cube.Cube")
        var ActorType:SoftActorClass = ue_soft(type<SoftActorClass>, "/Script/Engine.StaticMeshActor")
        var Assets:array<SoftObject>
        var Classes:table<string; SoftActorClass>
    }

    def BeginPlay():void {
        let path = ue_soft_path(self.Asset)      // no load
        let existing = ue_soft_get(self.Asset)   // only if already loaded, else null
        let loaded = ue_soft_load(self.Asset)    // explicit blocking load
        ue_soft_load_async("asset", self.Asset, "AssetReady")
    }

    [ue_function(BlueprintCallable)]
    def AssetReady(value:ObjectRef):void { /* use value */ }

    [ue_function(BlueprintCallable)]
    def Change(var value:SoftObject&; @ue_out var previous:SoftObject&):SoftObject {
        previous = value
        value = default<SoftObject>
        return previous
    }
}
```

Paths are object paths that include the object name (`/Game/Items/Sword.Sword`, or `/Game/Actors/Enemy.Enemy_C` for a Blueprint class). Export-text strings such as `StaticMesh'...'` are not needed. Subobject paths are rejected.

## API

| Function | Loads? | Result |
| --- | --- | --- |
| `ue_soft(type<Alias>, path)` | No | A soft value. Can be used as a property default. |
| `default<Alias>` | No | Empty path (null) |
| `ue_soft_path(v)` | No | The path string |
| `ue_soft_is_null(v)` / `ue_soft_is_loaded(v)` | No | bool |
| `ue_soft_get(v)` | No | Typed nullable reference if the asset is already loaded, otherwise null |
| `ue_soft_load(v)` | Yes, blocking | Typed nullable reference. A missing asset gives null. |
| `ue_soft_load_async(name, v, "Method")` | Yes, async | Calls `Method` later |
| `ue_soft_upcast(type<Parent>, v)` | No | The same path as a parent-typed soft value |

Copying, default construction, serialization and returning soft values from methods never load anything. A resolved object of an incompatible type is rejected. For an unloaded path, the type cannot be known until the asset is resolved.

### Asynchronous loading

`ue_soft_load_async(name, value, "Method")` requires an annotated `void` instance method that takes the matching typed object or class reference. The callback receives a regular reference, not the soft wrapper.

- Requests are named managed subscriptions that share `ue_asset_cancel(name)`, `ue_asset_cancel_all()` and `ue_asset_is_pending(name)` with other asset loads. If you start a request with the same name, the new contract is validated before the old request is replaced.
- The callback is always deferred through the owner's queue, even for an already loaded asset or an empty path (empty completes with null). "Deferred" means after the VM call returns. It is not promised for the next frame.
- Cancellation, owner release/EndPlay and script reload revoke pending delivery.
- Holding the resulting object reference does not keep the asset loaded once the request ends. Use normal Unreal ownership when you need the asset to stay resident.

See [Asset loading](../Data/AssetLoading.md) for the general asset-loading API.

## Where soft references can be used

| Position | Supported |
| --- | --- |
| `ue_class` properties, with literal `ue_soft(...)` defaults (also inside array/map literals) | Yes |
| `ue_struct` members | Yes, always empty. Initializers are rejected. |
| Method parameters, results, `ref`, `@ue_out` | Yes |
| `array<Soft...>` and map values `table<K; Soft...>` with the usual key types (int, string, name, byte, object keys) | Yes, in classes, structs and signatures |
| Map keys and set elements | No |
| Generated native fields and methods (`FSoftObjectProperty` / `FSoftClassProperty`), including native structs that contain soft fields, arrays and maps | Yes |

Soft values in containers follow the normal [container](Containers.md) rules: indexed writes, `push` and map insertion commit to the property, and reads return detached copies. An authored struct can bundle soft leaves:

```das
[ue_struct]
struct SoftRecord {
    @ue_property=(EditAnywhere, BlueprintReadWrite)
    Asset:SoftObject
    @ue_property=(EditAnywhere, BlueprintReadWrite)
    ActorType:SoftActorClass
    @ue_property=(EditAnywhere, BlueprintReadWrite)
    Items:array<SoftObject>
    @ue_property=(EditAnywhere, BlueprintReadWrite)
    Named:table<string; SoftActorClass>
}
```

Apply authored struct types before the class, as for any [struct](Structs.md#creating-the-assets).

Generated native APIs use the same values:

```das
require ue_native_gameplay

def use_asset(api:NativeHashProbeRef) {
    api.SoftAsset = ue_soft(type<SoftObject>, "/Engine/BasicShapes/Cube.Cube")
    let asset = api.EchoSoftAsset(api.SoftAsset)
    var items:array<SoftObject>
    push(items, asset)
    api.SoftAssets := items
}
```

## Parent conversions

Soft references widen implicitly along the native class hierarchy. The path is kept and nothing is loaded:

```das
require ue_soft_upcast_types

let curve = ue_soft(type<SoftCurve>, "/Game/Curves/Speed.Speed")
let asset:SoftObject = curve                        // implicit widening
let same = ue_soft_upcast(type<SoftObject>, curve)  // explicit, e.g. to pick an overload

let child = ue_soft(type<SoftMeshActorClass>, "/Game/Actors/Enemy.Enemy_C")
let actor:SoftActorClass = child
```

- Implicit widening works in variable initialization, assignment, function inputs and returns, generated native property setters and native method inputs. An exact overload wins. Otherwise the nearest unique parent overload is chosen. The source expression is evaluated once.
- Only object-to-object and class-to-class widening is allowed. Downcasts, unrelated constraints and object/class mixing are compile errors, including for null values. The low-level bridge checks the same rules again at runtime.
- `ref`/`out` arguments, arrays and maps are invariant. Create a parent-typed variable, or convert the elements explicitly.

## Ownership

A soft value owns only its path and never roots the asset, even inside a stored struct snapshot. Detached collections and records stay independent of the property they were read from. Invalid paths and resolved values with the wrong base are rejected before a snapshot is published.

Network replication of soft references and structural migration of soft fields are not supported.

Examples, all in `Plugins/DasForUE/Content/Das/Scripts/`: `dsl_soft_references.das`, `ue_soft_references.das`, `ue_soft_record.das`, `ue_soft_upcast_types.das`, `native_soft_probe.das`. Try it: `Scripts/TestSoftReferences.ps1`.

See also: [Object and class references](ObjectAndClassReferences.md) · [Asset loading](../Data/AssetLoading.md) · [Containers](Containers.md) · [Built-in types](BuiltinTypes.md) · [Defaults](Defaults.md)
