options gen2
require ue_dsl
require @COLLISION@

def assign_profile() {
    let object=@OBJECT@_from(ue_self_object())
    var profile:@PROFILE@
    profile.f_name=UEName(value="BlockAll")
    ue_collision_SetCollisionProfileNameProperty(object,UEName(value="Profile"),profile)
}

[export]
def main(input:int):int {
    if(input==0 || input==18) { assign_profile(); return 42 }
    let self=ue_self_object()
    var target=self
    if(input==9) { target=ue_null_object() }
    if(input==10 || input==11 || input==12) { target=ue_get_object(self,"OtherActor") }
    var name="Profile"
    if(input==1) { name="Counter" }
    if(input==2) { name="Direction" }
    if(input==3) { name="DoesNotExist" }
    if(input==4) { name="Readonly" }
    if(input==5) { name="Hidden" }
    if(input==6) { name="Fixed" }
    if(input==7) { name="Profiles" }
    if(input==8) { name="" }
    if(input==13) { name="Semantic" }
    if(input==16) {
        let unsupported=ue_begin_static_call("/Script/Engine.KismetSystemLibrary","SetStructurePropertyByName","")
        ue_release_call(unsupported)
        return 42
    }
    if(input==12 || input==14 || input==15) {
        let call=ue_begin_static_call("/Script/Engine.KismetSystemLibrary","SetCollisionProfileNameProperty","")
        ue_arg_object(call,"object",target)
        ue_arg_name(call,"propertyname",name)
        if(input==15) { ue_arg_string(call,"value.name","BlockAll") }
        if(input!=14) { ue_arg_name(call,"value.name","BlockAll") }
        if(input==12) {
            let destroy=ue_begin_call(self,"DestroyOther")
            ue_invoke_call(destroy)
            ue_release_call(destroy)
        }
        ue_invoke_call(call)
        ue_release_call(call)
        return 42
    }
    var profile:@PROFILE@
    profile.f_name=UEName(value="BlockAll")
    ue_collision_SetCollisionProfileNameProperty(@OBJECT@_from(target),UEName(value=name),profile)
    return 42
}

[ue_class(module_id="collisionprofileprobe")]
class GuardProbe {
    def ConstructionScript() { assign_profile() }
}
[export]
def begin_play() { }
[export]
def end_play(reason:int) { assign_profile() }
