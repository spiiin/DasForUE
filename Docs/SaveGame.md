# Authoring SaveGame classes

```das
options gen2
require ue_savegame

[ue_class(Blueprintable)]
class PlayerSave : SaveGame {
    properties=ue_properties(EditAnywhere,BlueprintReadWrite) {
        // Current score stored in the save slot.
        var Score:int=10
        var PlayerName:string="Player"
    }
    [ue_function(BlueprintCallable)]
    def AddScore(amount:int):void {self.Score+=amount}
}
```

Import through the usual authoring workflow to create a Blueprint derived from
`UDasSaveGame` / `USaveGame`. Data-only classes do not need a dummy method.
Authored inheritance and the existing field types use the common reflected
backend. Methods use the same checked frame ABI as other authored hosts.
World lifecycle hooks, components and async physics hooks are not supported on
SaveGame classes. SaveGame objects do not provide a game world.

The imported Blueprint must be saved and included in the cook for a packaged
application, alongside its authoring script. Loading a slot requires its class
and compatible definition to be available. Saving does not cook or import a class.

## Slots and object references

Call the following helpers from a persistent script owned by an object in a live
game world (for example an Actor or widget):

```das
let game=ue_savegame_create("/Game/Saves/BP_PlayerSave.BP_PlayerSave_C")
// An already resolved UEClassHandle is also accepted by ue_savegame_create.
let profile=ue_ref(type<PlayerSaveRef>,game)
profile.Score=42
let success=ue_savegame_save(game,"Player1")
let restored=ue_savegame_load("Player1")
```

`PlayerSaveRef` is the ordinary authored reference view:

```das
[ue_object_ref(script="savegame_profile.das",type="PlayerSave")]
struct PlayerSaveRef {handle:UObjectHandle}
```

The path above refers to the imported Blueprint asset; the example source file
is `Plugins/DasForUE/Content/Das/Scripts/savegame_profile.das`.

- `ue_savegame_exists(slot,user=0)` and `ue_savegame_delete(slot,user=0)` return bool.
- `ue_savegame_load(slot,user=0)` returns a null handle on engine load failure.
- `ue_savegame_valid(game)` checks whether a handle still resolves to a SaveGame.
- `ue_savegame_release(game)` drops this VM's ownership pin, not the object itself.
- Created/loaded objects have weak checked handles and are pinned by their VM
  until release or VM destruction. Each VM owns at most 32 such objects.
- Slot names are 1..128 ASCII letters/digits/underscore/hyphen, never file paths.
  User indices must be nonnegative; the platform decides their interpretation.

Every authored field on a SaveGame Blueprint receives the UE SaveGame property
flag. These helpers use **UE's standard slot serializer**, which stores all
non-transient properties, not only properties marked SaveGame. Object references
are not deep copies of world actors/components. Keep persistent state as values
or durable asset/soft references and reconstruct world objects after loading.
No custom schema migration or save format versioning is introduced here.

## Asynchronous completion

```das
[export]
def on_saved(success:bool):void {
    if (success) {
        ue_savegame_load_async("load_profile","Player1","on_loaded")
    }
}

[export]
def on_loaded(game:UObjectHandle):void {
    if (ue_savegame_valid(game)) {
        let profile=ue_ref(type<PlayerSaveRef>,game)
        // profile.Score is available; the loaded object remains owned by this VM.
    }
}

// From an exported persistent entry:
// ue_savegame_save_async("save_profile",game,"Player1","on_saved")
```

Save completion is an exported `(bool):void`; load completion is exported
`(UObjectHandle):void` and receives null on failure. Completion runs on the game
thread through the managed callback queue. Reusing a pending request name fails;
the completed registration is consumed before the handler runs, so the handler
can start a new request under that name. The per-VM limit is 32 pending deliveries.

`ue_savegame_pending(request)` queries delivery registration and
`ue_savegame_cancel(request)` cancels delivery. Reloading/releasing the VM or
cleaning up its world also revokes delivery. **Native disk I/O continues.** A
pending native save/load leases its slot until actual native completion, even
when its callback has been cancelled. Sync save/load/delete and other managed
async operations reject a busy slot. Leases are case insensitive and conservative
across user indices because some platforms ignore UserIndex. Native requests are
bounded to 256 overall. Requests started outside this managed layer cannot be
serialized by it.

UE snapshots save data before `AsyncSaveGameToSlot` returns. Mutating the source
object afterwards does not change that in-flight snapshot. The load result is
pinned before its callback, so a retained handle survives later garbage collection
until explicit release or VM destruction.

## Focused validation

`Scripts/TestSaveGame.ps1` builds the smoke Editor target and runs one
`DasSaveGame` commandlet scenario, with no PIE/packaged run. It imports the author
class in memory, exercises defaults and methods, writes/reads/deletes unique real
slots, checks async save/load, null failure, cancellation and post-callback GC.
It records source/DLL hashes in `logs/savegame-validation.json` and rejects moving
sources or DLLs. Test slots and temporary probe scripts are cleaned up; no user's
slots are selected. This scenario does not certify cross-process/cooked loading,
platform-specific save services or schema migration.
