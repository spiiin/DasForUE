options gen2
options gc = true
require ue_api_umg_input
require ue_paint
var saved:UEPaintContext
var old_frame:UCallHandle
var brush:UObjectHandle
var font:UObjectHandle
def white():S_LinearColor_f4abb14b {return S_LinearColor_f4abb14b(f_r=1.0,f_g=1.0,f_b=1.0,f_a=1.0)}
def line(c:UEPaintContext):UEPaintContext {return ue_ui_DrawLine(c,ue_vector2d(5.0lf,5.0lf),ue_vector2d(30.0lf,25.0lf),white(),true,1.0)}
[export]
def setup(b:UObjectHandle;f:UObjectHandle):int {brush:=b;font:=f;return 1}
[export]
def warmup(c:UEPaintContext):void {}
[export]
def draw_all(c:UEPaintContext):void {
 saved:=c
 let box=ue_ui_DrawBox(c,ue_vector2d(2.0lf,2.0lf),ue_vector2d(15.0lf,12.0lf),C_SlateBrushAsset_c9451dbb_from(brush),white())
 let ln=line(box)
 var points:array<UEVector2D>
 push(points,ue_vector2d(10.0lf,10.0lf));push(points,ue_vector2d(40.0lf,40.0lf));push(points,ue_vector2d(60.0lf,15.0lf))
 let lines=ue_ui_DrawLines(ln,points,white(),true,2.0)
 let spline=ue_ui_DrawSpline(lines,ue_vector2d(10.0lf,20.0lf),ue_vector2d(10.0lf,0.0lf),ue_vector2d(80.0lf,60.0lf),ue_vector2d(0.0lf,10.0lf),white(),1.0)
 let text=ue_ui_DrawTextFormatted(spline,ue_text_invariant("Paint"),ue_vector2d(5.0lf,70.0lf),C_Font_684a8ec8_from(font),12.0,ue_name("Regular"),white())
 old_frame:=ue_begin_static_call("/Script/UMG.WidgetBlueprintLibrary","DrawLine","")
}
[export]
def outside(input:int):int {let l=line(saved);return 0}
[export]
def stale(c:UEPaintContext):void {let l=line(saved)}
[export]
def invalid_default(c:UEPaintContext):void {var empty:UEPaintContext;let l=line(empty)}
[export]
def panic_after_draw(c:UEPaintContext):void {let l=line(c);panic("deliberate paint failure")}
[export]
def wrong_signature(c:UEPaintContext):int {return 1}
[export]
def bad_thickness(c:UEPaintContext):void {let l=line(c);let bad=ue_ui_DrawLine(c,ue_vector2d(0.0lf,0.0lf),ue_vector2d(2.0lf,2.0lf),white(),true,-1.0)}
[export]
def bad_coordinate(c:UEPaintContext):void {let l=line(c);let bad=ue_ui_DrawLine(c,ue_vector2d(1.0e20lf,0.0lf),ue_vector2d(2.0lf,2.0lf),white(),true,1.0)}
[export]
def too_many_points(c:UEPaintContext):void {var points:array<UEVector2D>;resize(points,4097);let bad=ue_ui_DrawLines(c,points,white(),true,1.0)}
[export]
def too_many_commands(c:UEPaintContext):void {for(i in range(1025)){let l=line(c)}}
[export]
def stale_frame(c:UEPaintContext):void {ue_invoke_call(old_frame)}
[export]
def result_before_invoke(c:UEPaintContext):void {let f=ue_begin_static_call("/Script/UMG.WidgetBlueprintLibrary","DrawLine","");let x=__ue_result_paint_context(f,"context")}
[export]
def missing_context(c:UEPaintContext):void {let f=ue_begin_static_call("/Script/UMG.WidgetBlueprintLibrary","DrawLine","");ue_invoke_call(f)}
[export]
def empty_lines(c:UEPaintContext):void {var points:array<UEVector2D>;let x=ue_ui_DrawLines(c,points,white(),true,1.0)}
[export]
def destruct_after_draw(c:UEPaintContext):void {let x=line(c);let f=ue_begin_call(ue_self_object(),"DestructDuringPaintForTest");ue_invoke_call(f);ue_release_call(f)}
[export]
def draw_then_gc(c:UEPaintContext):void {let x=ue_ui_DrawBox(c,ue_vector2d(2.0lf,2.0lf),ue_vector2d(10.0lf,10.0lf),C_SlateBrushAsset_c9451dbb_from(brush),white());let f=ue_begin_call(ue_self_object(),"CollectPaintGarbageForTest");ue_invoke_call(f);ue_release_call(f)}

[export]
def nested_paint(c:UEPaintContext):void {
 let f=ue_begin_call(ue_self_object(),"NestedPaintForTest");ue_invoke_call(f)
 let rejected=ue_result_bool(f,"returnvalue");ue_release_call(f)
 if(!rejected){panic("reentrant paint was accepted")}
 let l=line(c)
}
