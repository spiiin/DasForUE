options gen2
module ue_savegame public
require ue_dsl public

def ue_savegame_create(kind:UEClassHandle):UObjectHandle {return __ue_savegame_create(kind)}
def ue_savegame_create(class_path:string):UObjectHandle {return __ue_savegame_create_path(class_path)}
def ue_savegame_save(game:UObjectHandle; slot:string; user:int=0):bool {return __ue_savegame_save(game,slot,user)}
def ue_savegame_load(slot:string; user:int=0):UObjectHandle {return __ue_savegame_load(slot,user)}
def ue_savegame_exists(slot:string; user:int=0):bool {return __ue_savegame_exists(slot,user)}
def ue_savegame_delete(slot:string; user:int=0):bool {return __ue_savegame_delete(slot,user)}
def ue_savegame_release(game:UObjectHandle):bool {return __ue_savegame_release(game)}
def ue_savegame_valid(game:UObjectHandle):bool {return __ue_savegame_valid(game)}
// Completion: exported handler(success:bool):void / handler(game:UObjectHandle):void.
def ue_savegame_save_async(request:string; game:UObjectHandle; slot:string; handler:string; user:int=0):void {__ue_savegame_save_async(request,game,slot,user,handler)}
def ue_savegame_load_async(request:string; slot:string; handler:string; user:int=0):void {__ue_savegame_load_async(request,slot,user,handler)}
// Cancels delivery only. Native disk I/O continues and retains its slot lease.
def ue_savegame_cancel(request:string):bool {return __ue_savegame_cancel(request)}
def ue_savegame_pending(request:string):bool {return __ue_savegame_pending(request)}
