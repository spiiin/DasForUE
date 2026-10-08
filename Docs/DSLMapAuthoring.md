# Owned int-to-int map fields

The initial map field uses daScript's native table syntax:

```das
[ue_struct(id="sample.Scores", module_id="sample", BlueprintType)]
struct Scores {
    @ue_property=(EditAnywhere, BlueprintReadWrite)
    Values : table<int; int>
}
```

It creates TMap<int32,int32> storage through a typed FMapProperty. The enclosing
struct may be an Actor property, typed method parameter/result and ProcessEvent
value. Defaults are implicitly empty. Generated helpers pack using readonly
keys/values iterators and unpack by inserting native key/value pairs into a new
owned table. A repeated key updates its value rather than creating a duplicate.

```das
var data <- clone(self.Config)
data.Values[-5] = 25
self.Config <- data

var local <- clone(self.Config)
var total = 0
for (value in values(local.Values)) { total += value }
```

Keep a local owned value alive during iteration. keys(self.Config.Values) and
values(self.Config.Values) reject at compile time because self reads produce a
temporary snapshot. daScript table indexing can insert, and iterator traversal
locks the table: use values iteration or readonly get access, not indexing a locked
table inside a keys loop. Map iteration is not an ordering contract.

Low-level owned APIs: ue_struct_map_size(value,path),
ue_struct_map_key(value,path,ordinal), ue_struct_map_get_int(value,path,key),
ue_struct_map_with_int(value,path,key,newValue). Native key enumeration is sorted
by signed int key for deterministic ordinal access; daScript tables need not keep
that order. Missing keys and out-of-range ordinals return script errors. with
returns a new snapshot, and native Actor state changes only after root assignment.

Maps share the existing 4096 total container element and 16 MiB fixed element byte
bounds with arrays; the map-specific initial storage is two int32 values per pair.
Duplicate updates remain legal at the per-map size limit. Values are owned and no
native key/value pointer escapes. Current packing composes snapshot copies and is
not optimized for bulk maps.

Example: Content/Das/Scripts/dsl_map_authoring.das in the plugin. Replace REGISTRY
with your registry object path. Test:
Scripts/TestDSLTypeRegistry.ps1 -AuthoredMap -Cooked.

This milestone intentionally supports only table<int;int> inside ue_struct with
empty defaults. Other keys/values, nested map containers, set, standalone map
signatures, nonempty defaults and direct self-container mutation remain later work.
Migration development remains deferred: catalogs with maps reject structural
migration before candidate publication/native changes. Existing policies continue
for their earlier supported storage.

Next: more map key/value types, set functionality and standalone container
signatures; out/ref and interfaces follow. Key conversion/collision migrations and
container graph repair remain a separate deferred block.


## String key/value extension

The supported combinations are now table<int;int>, table<int;string>,
table<string;int> and table<string;string>. Native layouts are typed TMap<int32/
FString,int32/FString>. Strings are copied through owned FString and daScript
string storage; Unicode, empty values/keys and quoted text survive typed calls,
map save/reopen and cook. Defaults remain empty, and the earlier iterator/local
clone rules and container element bounds still apply.

String keys use the native reflected property equality. In the tested UE backend,
Case and case compare as equivalent, unlike distinct keys in a daScript table.
Packing two different spellings that native equality considers equal now returns
an explicit key-collision error before committing the root snapshot to the Actor.
Exact duplicate spelling still updates its value. Choose unambiguous keys or an
explicit application-level normalization policy; the bridge does not silently
discard an entry or rename keys. Existing key lookup follows native equality.

String-key ordinal enumeration sorts stored FString keys deterministically. This
does not turn daScript table iteration order into a stable contract. Native element
byte accounting includes fixed FString storage rather than total text heap bytes;
these limits are not an untrusted-script memory sandbox.

New low-level names: ue_struct_map_key_string, ue_struct_map_get_int_string,
ue_struct_map_get_string_int, ue_struct_map_get_string_string and matching
ue_struct_map_with_<key>_<value> functions. Existing int-int names are unchanged.
Returned strings belong to the daScript context; native string pointers do not escape.

Example: Content/Das/Scripts/dsl_string_maps.das. Test:
Scripts/TestDSLTypeRegistry.ps1 -StringMap -Cooked. It covers all four combinations,
Unicode, empty strings, duplicate updates, native-equality collision refusal,
typed return and native data preservation on a failed commit.

Set, other map key/value types, standalone signatures and migration logic remain
later work. The existing early map-migration refusal is retained.


## Scalar and name values

Maps inside ue_struct now accept int or string keys with any of these values:
int, float, double, bool, string, UENameValue. Native value properties are
FIntProperty, FFloatProperty, FDoubleProperty, FBoolProperty, FStrProperty and
FNameProperty respectively. There are 12 supported key/value combinations.
Float/double are distinct native types; double uses the real/double Blueprint
terminal subtype and retains its precision through typed calls and serialization.

```das
@ue_property=(EditAnywhere, BlueprintReadWrite)
Weights : table<string; double>
@ue_property=(EditAnywhere, BlueprintReadWrite)
Enabled : table<int; bool>
@ue_property=(EditAnywhere, BlueprintReadWrite)
Labels : table<string; UENameValue>
```

FName values use context-owned UENameValue tokens. Use ue_name_value(text) to
construct and ue_names_equal(a,b) to compare identity. Empty text becomes native
NAME_None, and numbered names retain their native FName identity. These are
values only: FName keys remain a later extension. Native references do not escape.

Generated pack/unpack helpers and low-level APIs are fully typed. New names are
ue_struct_map_get_<key>_<value> and ue_struct_map_with_<key>_<value>, where key
is int/string and value is float/double/bool/name. Get rejects an absent key or
wrong native value type. With returns an independent snapshot and an exact key
updates its value. The prior string-key collision rule, empty defaults and
shared container limits still apply. Mutate a local clone and assign the whole
Actor property; iterators over temporary self reads remain unsupported.

Example: Plugins/DasForUE/Content/Das/Scripts/dsl_scalar_maps.das. Replace REGISTRY
before applying the class. Validation:
Scripts/TestDSLTypeRegistry.ps1 -ScalarMap -AuthoredSet -Cooked.

Enums, structs, objects and nested containers as map values, more key types,
nonempty defaults and standalone DSL container signatures remain separate work.
Map structural migration remains deferred.
