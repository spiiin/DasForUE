options gen2
require ue_dsl
[ue_object_ref(script="dsl_defaults_actor.das", type="DefaultActor")]
struct DefaultActorRef { handle:UObjectHandle }
[export]
def main():int {
    let target=ue_ref(type<DefaultActorRef>,ue_self_object())
    if (target.DefaultAdd()!=5 || target.DefaultAdd(10)!=8 || target.DefaultAdd(10,3)!=13) { return -201 }
    if (target.DefaultDouble()!=1.23456789012345lf || target.DefaultFloat()!=-0.125) { return -202 }
    if (target.DefaultBool() || target.DefaultText()!="Пример 世界" || target.DefaultEmpty()!="") { return -203 }
    return target.CheckDefaults()
}
