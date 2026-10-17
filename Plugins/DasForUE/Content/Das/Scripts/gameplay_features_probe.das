options gen2
require ue_gameplay_services
var saved:UETraceHit
[export]
def main(input:int):int {
    if(input==3) {
        let kind=ue_class_cast(type<UserWidgetClass>,ue_class_loaded(type<UserWidgetRef>,"/Script/DasRuntime.DasGameplayHUD"))
        let widget=ue_widget_create(kind,ue_pawn_controller(ue_character_self()))
        verify(ue_widget_show(widget),"show temporary widget")
        ue_widget_bind_click(widget,"Damage","HurtFromHUD")
        ue_widget_bind_click(widget,"Damage","HurtFromHUD")
        var invalid_text=false
        try { ue_widget_set_text(widget,"Damage",ue_text_invariant("wrong type")) } recover { invalid_text=true }
        var invalid_handler=false
        try { ue_widget_bind_click(widget,"Damage","OnHurt") } recover { invalid_handler=true }
        verify(invalid_text && invalid_handler,"invalid named widget/handler must fail")
        ue_widget_remove(widget)
        verify(!ue_widget_visible(widget),"removed widget visibility")
        var revoked=false
        try { ue_widget_set_text(widget,"Score",ue_text_invariant("revoked")) } recover { revoked=true }
        verify(revoked,"removed widget must revoke context ownership")
        return 1
    }
    if(input==2) { return (!ue_is_valid(ue_hit_actor(saved)) && !ue_is_valid(ue_hit_component(saved)) && saved.blocking_hit) ? 1 : 0 }
    let start=ue_vector(0.0lf,0.0lf,150.0lf)
    let finish=ue_vector(450.0lf,0.0lf,150.0lf)
    saved=ue_line_trace(start,finish)
    verify(saved.blocking_hit && !saved.start_penetrating && saved.distance>200.0lf && saved.distance<300.0lf && saved.normal.x< -0.8lf,"line trace scalar/math result")
    let actor=ue_hit_actor(saved)
    verify(ue_is_valid(actor) && ue_is_valid(ue_hit_component(saved)),"typed hit references")
    var ignored:array<ActorRef>
    push(ignored,actor)
    verify(!ue_line_trace(start,finish,ignored).blocking_hit,"ignored actor")
    let offset=ue_vector(0.0lf,100.0lf,150.0lf)
    let offset_end=ue_vector(450.0lf,100.0lf,150.0lf)
    verify(!ue_line_trace(offset,offset_end).blocking_hit,"line miss")
    verify(ue_sphere_trace(offset,offset_end,40.0lf).blocking_hit,"sphere sweep hit")
    var bad_radius=false
    try { let bad=ue_sphere_trace(start,finish,0.0lf) } recover { bad_radius=true }
    var bad_channel=false
    try { let bad=ue_line_trace(start,finish,unsafe(reinterpret<TraceChannel>(999))) } recover { bad_channel=true }
    var bad_initial_value=false
    try { let bad=ue_hit_actor(default<UETraceHit>) } recover { bad_initial_value=true }
    verify(bad_radius && bad_channel && bad_initial_value,"invalid trace inputs must fail")
    var negative=false
    try { ue_apply_damage(ue_self_actor(),-1.0f) } recover { negative=true }
    verify(negative,"negative damage must fail")
    return 1
}
