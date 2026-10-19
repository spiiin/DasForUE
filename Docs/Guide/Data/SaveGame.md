# SaveGame

Declare a `SaveGame` class in the DSL, then create, save, load and delete slots from a script, synchronously or with async completion callbacks. Slots use Unreal's standard SaveGame serializer.

## Declaring a SaveGame class

```das
options gen2
require ue_savegame

[ue_class(Blueprintable)]
class PlayerSave : SaveGame {
    properties=ue_properties(EditAnywhere,BlueprintReadWrite) {
        // Current score stored in the save slot.
        var Score:int=10
        // Display name of the player.
        var PlayerName:string="Player"
    }
    [ue_function(BlueprintCallable)]
    def AddScore(amount:int):void { self.Score+=amount }
    [ue_function(BlueprintPure)]
    def GetScore():int { return self.Score }
}
```

Example: `Plugins/DasForUE/Content/Das/Scripts/savegame_profile.das`.

- Import it through the usual workflow ([Apply and hot reload](../Workflow/ApplyAndHotReload.md)); this creates a Blueprint derived from `UDasSaveGame` / `USaveGame`. Data-only classes need no methods.
- Fields use the common reflected field types; methods use the same checked call ABI as other authored classes; authored inheritance works.
- World lifecycle hooks, components and `ue_async_physics` are rejected. A SaveGame object has no game world.
- Every authored field gets the UE `SaveGame` property flag.
- For packaged games, save the imported Blueprint and include it, and its script, in the cook. Loading a slot needs its class and a compatible definition.

## Saving and loading

Call these from a persistent script owned by an object in a live game world (an Actor or widget):

```das
[ue_object_ref(script="savegame_profile.das",type="PlayerSave")]
struct PlayerSaveRef {handle:UObjectHandle}

let game=ue_savegame_create("/Game/Saves/BP_PlayerSave.BP_PlayerSave_C")
let profile=ue_ref(type<PlayerSaveRef>,game)
profile.Score=42
let success=ue_savegame_save(game,"Player1")
let restored=ue_savegame_load("Player1")
```

The class path refers to the imported Blueprint asset; `ue_savegame_create` also accepts an already resolved `UEClassHandle`.

| Function | Behavior |
| --- | --- |
| `ue_savegame_create(class_path or UEClassHandle):UObjectHandle` | New SaveGame object owned by this VM. |
| `ue_savegame_save(game, slot, user=0):bool` | Synchronous save. |
| `ue_savegame_load(slot, user=0):UObjectHandle` | Synchronous load; null on engine failure. |
| `ue_savegame_exists(slot, user=0):bool`, `ue_savegame_delete(slot, user=0):bool` | Slot queries. |
| `ue_savegame_valid(game):bool` | Does the handle still resolve to a SaveGame? |
| `ue_savegame_release(game):bool` | Drop this VM's ownership pin (does not destroy the object). |

- Created/loaded objects are weak checked handles, pinned by the VM until release or VM destruction. At most 32 per VM.
- Slot names are 1..128 ASCII letters, digits, underscore or hyphen, never file paths. User indices must be non-negative; the platform decides what they mean.

### What gets saved

UE's standard slot serializer stores **all non-transient properties**, not only those marked SaveGame. Object references are not deep copies of world actors or components: keep persistent state as values or durable asset/soft references and rebuild world objects after loading. There is no custom schema migration or save format versioning.

## Async save and load

```das
[export]
def on_saved(success:bool):void {
    if(success) {
        ue_savegame_load_async("load_profile","Player1","on_loaded")
    }
}

[export]
def on_loaded(game:UObjectHandle):void {
    if(ue_savegame_valid(game)) {
        let profile=ue_ref(type<PlayerSaveRef>,game)
        // profile.Score is available; the loaded object stays owned by this VM.
    }
}

// From an exported persistent entry:
ue_savegame_save_async("save_profile",game,"Player1","on_saved")
```

| Function | Behavior |
| --- | --- |
| `ue_savegame_save_async(request, game, slot, handler, user=0)` | Handler: exported `(bool):void`. |
| `ue_savegame_load_async(request, slot, handler, user=0)` | Handler: exported `(UObjectHandle):void`; null on failure. |
| `ue_savegame_pending(request):bool` | Is delivery still registered? |
| `ue_savegame_cancel(request):bool` | Cancel delivery. |

- Completion runs on the game thread through the managed callback queue. Reusing a pending request name fails; the registration is consumed before the handler runs, so the handler can start a new request with the same name. At most 32 pending deliveries per VM.
- UE snapshots the data before `AsyncSaveGameToSlot` returns, so changing the object afterwards does not affect the in-flight save.
- The load result is pinned before its callback and survives GC until released or the VM is destroyed.
- Cancelling, reloading/releasing the VM or cleaning up its world revokes delivery, but **native disk I/O continues**. A pending native save/load leases its slot until it actually completes, even if its callback was cancelled; sync save/load/delete and other managed async operations on a busy slot are refused. Slot leases are case-insensitive and conservative across user indices (some platforms ignore UserIndex). At most 256 native requests overall. Requests started outside this managed layer are not coordinated.

## Limitations

- No schema migration or versioning of save data.
- Cross-process/cooked loading and platform-specific save services follow Unreal's own behavior; the plugin adds no guarantees beyond the standard serializer.

Try it: `.\Scripts\TestSaveGame.ps1` runs a scenario that imports the class, writes/reads/deletes unique slots and exercises async save/load and cancellation.

See also: [Classes](../DSL/Classes.md), [Data assets](DataAssets.md), [Object and class references](../DSL/ObjectAndClassReferences.md), [Apply and hot reload](../Workflow/ApplyAndHotReload.md)
