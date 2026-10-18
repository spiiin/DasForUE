options gen2
require ue_savegame
[ue_object_ref(script="savegame_profile.das",type="PlayerSave")]
struct ProfileRef {handle:UObjectHandle}
var phase:int=0
var retained:UObjectHandle
[export]
def saved(success:bool):void {
    if (!success) {phase=-10;return}
    ue_savegame_load_async("read","__SLOT_ASYNC__","loaded")
}
[export]
def loaded(game:UObjectHandle):void {
    if (!ue_savegame_valid(game)) {phase=-11;return}
    retained=game
    let profile=ue_ref(type<ProfileRef>,game)
    if (profile.Score!=42 || profile.PlayerName!="Test player") {phase=-12;return}
    phase=1
}
[export]
def missing(game:UObjectHandle):void {
    phase=ue_savegame_valid(game) ? -13 : 2
}
[export]
def cancelled(success:bool):void {phase=-14}
[export]
def start(input:int):int {
    let game=ue_savegame_create("__CLASS__")
    let profile=ue_ref(type<ProfileRef>,game)
    if (profile.Score!=10) {return -1}
    profile.Score=40
    profile.PlayerName="Test player"
    profile.AddScore(2)
    if (profile.GetScore()!=42) {return -2}
    if (!ue_savegame_save(game,"__SLOT_SYNC__")) {return -3}
    if (!ue_savegame_exists("__SLOT_SYNC__")) {return -4}
    let restored=ue_savegame_load("__SLOT_SYNC__")
    let copy=ue_ref(type<ProfileRef>,restored)
    if (copy.Score!=42 || copy.PlayerName!="Test player") {return -5}
    if (!ue_savegame_release(restored)) {return -6}
    if (!ue_savegame_delete("__SLOT_SYNC__")) {return -7}
    ue_savegame_save_async("write",game,"__SLOT_ASYNC__","saved")
    profile.Score=999
    return 1
}
[export]
def status(input:int):int {return phase}
[export]
def retained_after_gc(input:int):int {
    let profile=ue_ref(type<ProfileRef>,retained)
    return profile.Score==42 && profile.PlayerName=="Test player" ? 1 : 0
}
[export]
def load_missing(input:int):int {
    ue_savegame_load_async("missing","__SLOT_MISSING__","missing")
    return 1
}
[export]
def cancel_write(input:int):int {
    let game=ue_savegame_create("__CLASS__")
    ue_savegame_save_async("cancel",game,"__SLOT_CANCEL__","cancelled")
    if (!ue_savegame_pending("cancel") || !ue_savegame_cancel("cancel")) {return -1}
    return ue_savegame_pending("cancel") ? -2 : 1
}
