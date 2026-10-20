options gen2
module identity_game public
require ue_object_types
[ue_native_object(path="/Script/Engine.Actor")]
struct MinimalActorRef { handle:UObjectHandle }
var @ue_state Counter:int = 10
var @ue_state Ready:bool = false
var @ue_state Fraction:float = 0.0f
var @ue_state Weight:double = 0.0lf
var @ue_state Title:string = "Initial"
var Dependent:int = int(Counter)
[export]
def main(input:int):int {
    Counter += input
    return int(Counter) // REVISION
}
[export]
def mutate(input:int):int {
    Counter += 1
    Ready = true
    Fraction = 1.25f
    Weight = 2.5lf
    Title = "Сохранено 世界"
    return int(Counter)
}
[export]
def inspect(input:int):int {
    verify(Ready && Fraction == 1.25f && Weight == 2.5lf && Title == "Сохранено 世界")
    let text = ue_loctext(Welcome, "Привет 世界")
    verify(ue_text_namespace(text) == "identity_game")
    verify(ue_text_key(text) == "Welcome" && ue_text_to_string(text) == "Привет 世界")
    return Dependent
}
[export]
def cdo():UObjectHandle { return ue_class_default(type<ActorRef>).handle }
[export]
def reference():UObjectHandle { return ue_ref(type<ActorRef>, ue_self_object()).handle }
[export]
def null_reference():UObjectHandle { return ue_ref(type<ActorRef>, ue_null_object()).handle }

[export]
def minimal_reference():UObjectHandle { return ue_ref(type<MinimalActorRef>, ue_self_object()).handle }
