# Bulk world bindings

`ue_navigation` resolves the current game world's navigation system once per block.
The value passed to the block is a checked context-owned object handle, not a borrowed
native pointer. Calls validate its world and lifetime; retaining it does not bypass
these checks. A missing navigation system produces failed single queries or origin
fallbacks in the batch query.

```das
var locations : array<UEVector>
with_ue_navigation() $(navigation : NavigationSystemRef) {
    locations <- ue_navigation_random_points(navigation, origin, radius, count)
    // Custom script loops can also use the direct single-point overload:
    let point = ue_navigation_random_point(navigation, origin, radius)
}
```

The batch query returns exactly `count` positions. Failed queries use `origin`,
matching the example's previous policy. Negative/nonfinite inputs and counts over
1,000,000 are rejected. Query order and randomness remain native navigation behavior.

`ue_ism_add_instances` and `ue_ism_remove_instances` now transfer typed contiguous
arrays directly, without per-element reflected property paths. Add validates all
transforms and allocates result storage before changing the component. IDs remain
component-scoped stable `FPrimitiveInstanceId` values, not indices. Removal validates
all IDs and rejects duplicates before changing the component. Both operations require
a registered component in the active game world. No navigation or coin game rules
have moved into native code.

PIE coverage lives in ActionRoguelikeDas `rogue_bulk_world_probe.das` and the
`rogue-coin-profile` suite: scoped queries, batch size/radius, absent navigation,
zero/negative count, native instance transforms, stable IDs across removal, real
coin lever input, collection and loot, followed by before/after profiling.

Generated numeric and native math array carriers use checked contiguous copies for
pack/unpack. This includes int, int64, float, double, vectors, rotators, quaternions,
transforms and colors. Other containers are assembled in one scoped snapshot draft;
public snapshots retain their immutable semantics and object/class GC ownership.
