options gen2
module ue_gameplay_services public
require ue_character_control public
require ue_text public

// Native collision channels; custom project trace channels use their configured numbers.
enum TraceChannel {
    WorldStatic=0
    WorldDynamic=1
    Pawn=2
    Visibility=3
    Camera=4
    PhysicsBody=5
    Vehicle=6
    Destructible=7
    GameTrace1=14
    GameTrace2=15
    GameTrace3=16
    GameTrace4=17
    GameTrace5=18
    GameTrace6=19
    GameTrace7=20
    GameTrace8=21
    GameTrace9=22
    GameTrace10=23
    GameTrace11=24
    GameTrace12=25
    GameTrace13=26
    GameTrace14=27
    GameTrace15=28
    GameTrace16=29
    GameTrace17=30
    GameTrace18=31
}
[ue_native_object(path="/Script/Engine.PrimitiveComponent")]
struct TraceComponentRef { handle:UObjectHandle }
def TraceComponentRef_from(value:UObjectHandle):TraceComponentRef { return TraceComponentRef(handle=ue_cast_nullable_object(value,"/Script/Engine.PrimitiveComponent")) }

def ue_line_trace(start:UEVector; finish:UEVector; var ignored:array<ActorRef>; channel:TraceChannel=TraceChannel.Visibility; complex:bool=false; ignore_self:bool=true):UETraceHit {
    var handles:array<UObjectHandle>
    for(actor in ignored) { push(handles,actor.handle) }
    return __ue_trace_world(start,finish,0.0lf,int(channel),handles,complex,ignore_self)
}
def ue_line_trace(start:UEVector; finish:UEVector; channel:TraceChannel=TraceChannel.Visibility; complex:bool=false; ignore_self:bool=true):UETraceHit {
    var ignored:array<ActorRef>
    return ue_line_trace(start,finish,ignored,channel,complex,ignore_self)
}
def ue_sphere_trace(start:UEVector; finish:UEVector; radius:double; var ignored:array<ActorRef>; channel:TraceChannel=TraceChannel.Visibility; complex:bool=false; ignore_self:bool=true):UETraceHit {
    verify(radius>0.0lf,"Sphere trace radius must be positive")
    var handles:array<UObjectHandle>
    for(actor in ignored) { push(handles,actor.handle) }
    return __ue_trace_world(start,finish,radius,int(channel),handles,complex,ignore_self)
}
def ue_sphere_trace(start:UEVector; finish:UEVector; radius:double; channel:TraceChannel=TraceChannel.Visibility; complex:bool=false; ignore_self:bool=true):UETraceHit {
    var ignored:array<ActorRef>
    return ue_sphere_trace(start,finish,radius,ignored,channel,complex,ignore_self)
}
def ue_hit_actor(hit:UETraceHit):ActorRef { return ue_ref(type<ActorRef>,__ue_trace_actor(hit)) }
def ue_hit_component(hit:UETraceHit):TraceComponentRef { return TraceComponentRef_from(__ue_trace_component(hit)) }
def ue_self_actor():ActorRef {
    let object=ue_self_object()
    if(ue_object_is_a(object,"/Script/Engine.Actor")) { return ue_ref(type<ActorRef>,object) }
    return ue_ref(type<ActorRef>,ue_component_owner(object))
}
def ue_apply_damage(target:ActorRef; amount:float; source:ActorRef=default<ActorRef>; instigator:PlayerControllerRef=default<PlayerControllerRef>):float {
    return __ue_apply_damage(target.handle,amount,source.handle,instigator.handle)
}

[ue_native_object(path="/Script/UMG.UserWidget")]
struct UserWidgetRef { handle:UObjectHandle }
def UserWidgetRef_from(value:UObjectHandle):UserWidgetRef { return UserWidgetRef(handle=ue_cast_nullable_object(value,"/Script/UMG.UserWidget")) }
typedef UserWidgetClass = $UEClass<UserWidgetRef>
def ue_widget_create(kind:UserWidgetClass; player:PlayerControllerRef):UserWidgetRef { var handle=ue_class_handle(kind); return ue_ref(type<UserWidgetRef>,__ue_widget_create(handle,player.handle)) }
def ue_widget_show(widget:UserWidgetRef; z_order:int=0):bool { return __ue_widget_show(widget.handle,z_order) }
def ue_widget_visible(widget:UserWidgetRef):bool { return __ue_widget_visible(widget.handle) }
def ue_widget_remove(widget:UserWidgetRef):void { __ue_widget_remove(widget.handle) }
def ue_widget_set_text(widget:UserWidgetRef; name:string; text:UETextValue):void { __ue_widget_text(widget.handle,name,text) }
def ue_widget_bind_click(widget:UserWidgetRef; name:string; handler:string):void { __ue_widget_click(widget.handle,name,handler) }
def ue_widget_input(widget:UserWidgetRef; enabled:bool):void { __ue_widget_input(widget.handle,enabled) }
