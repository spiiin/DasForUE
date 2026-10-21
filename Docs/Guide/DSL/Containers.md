# Containers

Arrays, maps and sets are written with the normal daScript container syntax and become real Unreal `TArray`, `TMap` and `TSet` properties and pins. The values you handle in script are always owned copies: reading a container gives you a snapshot, and writes are validated and then committed to the Unreal property.

## Syntax

| daScript | Unreal | Notes |
| --- | --- | --- |
| `array<T>` | `TArray<T>` / `FArrayProperty` | Dynamic arrays only. Fixed C++ arrays are not supported. |
| `table<K; V>` | `TMap<K, V>` / `FMapProperty` | |
| `table<K>` (same as `table<K; void>`) | `TSet<K>` / `FSetProperty` | A key-only table is a set. |

Containers work in:

- `ue_class` properties (Actor and ActorComponent classes),
- `ue_struct` members (with a narrower set of key types, see [below](#containers-inside-ue_struct)),
- `ue_function` parameters, results, mutable `var x:T&` and `@ue_out var x:T&` parameters, and authored interface methods,
- native reflected fields accessed through `self`, and typed references to other objects.

Several containers and scalars can share one method. The usual limit of 16 parameters applies. Blueprint shows ordinary native container pins. The internal carrier structs used to transport snapshots are private and never appear as user assets or pins.

## Supported element types

| Element type | Array element | Map value | Map key / set element |
| --- | --- | --- | --- |
| `int`, `uint8`, `string`, `UENameValue` | Yes | Yes | Yes |
| `float`, `double`, `bool` | Yes | Yes | No |
| `FText` | Yes | Yes | No. Text has no supported Unreal key hash. |
| `UEVector`, `UEVector2D`, `UERotator`, `UETransform` | Yes | Yes | No |
| Authored `ue_enum`, native `ue_native_enum` | Yes | Yes | Yes |
| Authored `ue_struct` | Yes | Yes | No |
| Typed object / interface references (`ActorRef`, generated wrappers) | Yes | Yes | Use an object key type (`ActorKey`), see below |
| Soft object / class references | Yes | Yes | No |
| `UEStructValue` (generic native struct) | Native self fields only | No | No |
| Class references, delegates, other containers | No | No | No |

Nested containers (`array<array<T>>`, a map whose values are arrays, and so on) are rejected. A struct element can contain containers, though: `array<Group>` where `Group` has an `array<double>` member works.

```das
options gen2
require ue_dsl

[ue_class(Blueprintable)]
class Counter {
    @ue_property=(EditAnywhere, BlueprintReadWrite)
    Scores : array<int>
    @ue_property=(EditAnywhere, BlueprintReadWrite)
    Weights : table<string; double>
    @ue_property=(EditAnywhere, BlueprintReadWrite)
    Tags : table<UENameValue>
    @ue_property=(EditAnywhere, BlueprintReadWrite)
    Positions : array<UEVector>

    [ue_function(BlueprintCallable)]
    def AddScore(value:int):void {
        push(self.Scores, value)
        self.Weights["last"] = double(value)
        insert(self.Tags, ue_name_value("Changed"))
        push(self.Positions, ue_vector(1.0lf, 2.0lf, 3.0lf))
        self.Positions[0].x += 4.0lf
    }

    [ue_function(BlueprintCallable)]
    def Echo(input:table<int; double>):table<int; double> {
        var result <- clone(input)
        result[-3] = 1.23456789012345lf
        return <- result
    }
}
```

## Reading, writing and ownership

- **Reads return owned copies.** `var copy <- api.Weights` or `var copy = self.Scores` gives you a detached container. Editing it does not change the property until you assign it back.
- **Whole assignment** uses clone assignment: `self.Scores := values` or `api.Scores := copy`.
- **Direct operations on properties** are committed for you, both through `self` and through a typed reference. These include indexed assignment and compound operators, `push`, `push_clone` (with an optional insert index), `pop`, `erase`, `insert`, `insert_clone`, `clear` and `key_exists`. They also work through several levels: `self.Groups[0].Scores[1] += 3.0lf`, `self.Groups[0].Weights["score"] = 8.0lf`. Indexed access assumes the element exists. Array indices accept `int` and `int64`. See [Self access](SelfAccess.md) for the full list of operations.
- **Atomicity.** Each operation works on a snapshot, validates the result and only then publishes the property. A failed index, a failed packing step or a validation error leaves the property unchanged. A function with several statements is not a transaction, though: earlier successful statements stay applied.
- **Methods** receive owned containers. Clone an input before changing it, and use the move form for results (`return <- result`). Unreal marks array inputs as by-ref; the adapter still copies them, so script code never changes the caller's array.
- **Read-only.** `BlueprintReadOnly` or `readonly` blocks whole assignment, indexed writes and every container operation.
- **Order.** Map and set iteration order is not a contract. Look values up by key.

### Iteration

Iterate over a local copy, not over a temporary read from `self`:

```das
var local <- clone(self.Config)
var total = 0
for (value in values(local.Values)) { total += value }
for (name in keys(local.Names)) { /* read only */ }
```

`keys(self.Config.Values)` and `values(...)` over a self read are rejected at compile time. daScript locks a table while you iterate it, so do not mutate the same table inside its loop. Indexing a table can insert a key, so use `key_exists` or read-only access instead.

### String and name keys

String keys and set elements follow Unreal's native equality, which ignores case. If two spellings that are distinct in daScript (`"Case"` and `"case"`) would collide natively, packing fails with an explicit key-collision error and the property keeps its previous contents. Entries are never silently dropped or renamed. An exact duplicate just updates the value. `UENameValue` keys follow `FName` rules: case is ignored, `None` and the empty name are the same key, and number suffixes count.

### Object keys

daScript table keys cannot be reference wrappers, so object sets and object-key maps use a nominal key type, `$UEObjectKey<Reference>`. The `ue_object_keys` module predefines `ActorKey` and `ObjectKey`:

```das
require ue_object_keys

[ue_class(Blueprintable)]
class ActorRegistry {
    @ue_property=(EditAnywhere, BlueprintReadWrite)
    Actors:table<ActorKey>
    @ue_property=(EditAnywhere, BlueprintReadWrite)
    Scores:table<ActorKey; int>

    [ue_function(BlueprintCallable)]
    def Remember(actor:ActorRef):void {
        let key = ue_key(actor)
        insert(self.Actors, key)
        self.Scores[key] = 10
    }
}
```

- `ue_key(ref)` infers the key type. `ue_key(type<ActorKey>, value)` is an explicit checked conversion. `ue_key_ref(key)` gives back the typed reference, `ue_key_valid(key)` checks that the object is alive, and `default<ActorKey>` is the null key.
- Declare custom key aliases in an imported module (`typedef EnemyKey = $UEObjectKey<EnemyRef>`), so they resolve before the class annotation runs.
- Keys are opaque identities owned by the context, not pointers or persistent IDs. Two references to the same object give the same key. Native containers hold strong references, but the script's key table is weak. If a stored collection contains a destroyed object, reading it fails. To recover, assign an empty table to the property.
- Interface views, class references, soft/weak references and raw `UObjectHandle` are not accepted as keys. Up to 65,536 object identities are allowed per context.

## Containers inside `ue_struct`

Authored struct members support containers with a narrower set of key types:

| Shape | Supported |
| --- | --- |
| `array<T>` | int, float, double, bool, string, `UENameValue`, `uint8`, `FText`, math types, authored enums, authored structs, typed native object/interface refs, soft refs |
| `table<K; V>` | Keys: `int`, `string`, `uint8`. Values: int, float, double, bool, string, `UENameValue`, `uint8`, `FText`, math types, authored structs, authored enums, typed native object/interface refs, soft refs |
| `table<K>` (set) | `int`, `string`, `uint8` |

Struct member containers always start empty, and initializers are rejected. References to authored classes (`ue_object_ref` views) are not allowed in struct members. Enum-key maps, name-key maps and object-key containers are available as `ue_class` properties and in method signatures, but not as `ue_struct` members.

```das
[ue_struct(module_id="structmaps", BlueprintType)]
struct MapCatalog {
    @ue_property=(EditAnywhere, BlueprintReadWrite)
    Entries:table<string; MapEntry>
    @ue_property=(EditAnywhere, BlueprintReadWrite)
    Positions:table<int; UEVector>
    @ue_property=(EditAnywhere, BlueprintReadWrite)
    Ids:table<int>
}
```

Inside a method you can also change struct-member containers directly (`self.Entries[name].Score += amount`, `push(self.Entries[name].Samples, 1.0lf)`).

## Defaults

`ue_class` container properties accept array and table literals, including computed scalar elements and `ue_defaults()` overrides, for example `Tags:table<string> = {"red","blue"}`. Write a space between `>` and `=`. Otherwise daScript reads `>=`. Defaults with object references or object keys are not supported. Containers in `ue_struct` members always start empty. See [Defaults](Defaults.md).

## Limits

| Limit | Value |
| --- | --- |
| Total elements per snapshot, including containers nested in struct elements | 4,096 |
| Fixed element storage per snapshot (FString heap not counted) | 16 MiB |
| Elements per container in a native call frame | 65,536 |
| Call-frame traversal depth / expanded nodes | 16 / 1,048,576 |
| Cached carrier shapes (struct arrays, maps, object-hash combinations) | 1,024 each |

These limits keep the data transport bounded. They are not a memory sandbox for untrusted scripts. Packing copies snapshots per operation, which is fine for gameplay-sized containers but not tuned for very large bulk data.

## Reload and migration

A class with container properties is reimported like any class: new fields and methods (container fields
included) are added, container defaults are merged with instance values, and instance contents are kept.
Removing, renaming or retyping an existing container property, or changing a struct that contains
containers, is refused before any change. See [Migrations](../Workflow/Migrations.md) and the
[reload matrix](../../Reference/ReloadMatrix.md).

Examples, all in `Plugins/DasForUE/Content/Das/Scripts/`: `dsl_container_calls.das`, `dsl_array_authoring.das`, `dsl_primitive_arrays.das`, `dsl_map_authoring.das`, `dsl_string_maps.das`, `dsl_scalar_maps.das`, `dsl_set_authoring.das`, `ue_struct_map_types.das` + `dsl_struct_maps.das` + `dsl_reflected_maps.das`, `dsl_text_byte_containers.das`, `ue_object_keys.das`, `dsl_method_value_driver.das`. Fixtures that contain `REGISTRY` need your own registry object path in its place.

See also: [Built-in types](BuiltinTypes.md) · [Structs](Structs.md) · [Enums](Enums.md) · [Soft references](SoftReferences.md) · [Self access](SelfAccess.md) · [Defaults](Defaults.md) · [Object and class references](ObjectAndClassReferences.md) · [Struct snapshots](../../Reference/StructSnapshots.md)
