options gen2
options gc = true
require _MODULE_
require ue_geometry
var saved:array<UEGeometry>
var widgetSnapshots:array<UEGeometry>
var checks:int=0
def expect_geometry(good:bool):void { if(!good){panic("Geometry mismatch at {checks}")};checks+=1 }
def near_geometry(v:UEVector2D;x:double;y:double):bool {return v.x-x<0.0001lf && v.x-x> -0.0001lf && v.y-y<0.0001lf && v.y-y> -0.0001lf}
[export]
def accept_geometry(raw:UEStructValue):int {
 checks=0
 let g=ue_geometry(raw)
 expect_geometry(near_geometry(ue_ui_geometry_LocalToAbsolute(g,ue_vector2d(4.0lf,6.0lf)),22.0lf,26.0lf))
 expect_geometry(near_geometry(ue_ui_geometry_AbsoluteToLocal(g,ue_vector2d(22.0lf,26.0lf)),4.0lf,6.0lf))
 expect_geometry(near_geometry(ue_ui_geometry_Vector_LocalToAbsolute(g,ue_vector2d(4.0lf,6.0lf)),12.0lf,6.0lf))
 expect_geometry(near_geometry(ue_ui_geometry_Vector_AbsoluteToLocal(g,ue_vector2d(12.0lf,6.0lf)),4.0lf,6.0lf))
 expect_geometry(ue_ui_geometry_Scalar_LocalToAbsolute(g,2.0)==6.0)
 expect_geometry(ue_ui_geometry_Scalar_AbsoluteToLocal(g,6.0)==2.0)
 expect_geometry(near_geometry(ue_ui_geometry_GetLocalSize(g),100.0lf,50.0lf))
 expect_geometry(near_geometry(ue_ui_geometry_GetAbsoluteSize(g),300.0lf,50.0lf))
 expect_geometry(near_geometry(ue_ui_geometry_GetLocalTopLeft(g),10.0lf,20.0lf))
 expect_geometry(ue_ui_geometry_IsUnderLocation(g,ue_vector2d(22.0lf,26.0lf)) && !ue_ui_geometry_IsUnderLocation(g,ue_vector2d(-10.0lf,-10.0lf)))
 let viewport=ue_ui_geometry_LocalToViewport(g,ue_vector2d(4.0lf,6.0lf))
 expect_geometry(near_geometry(viewport.out_pixelposition,0.0lf,0.0lf) && near_geometry(viewport.out_viewportposition,0.0lf,0.0lf))
 let local=ue_ui_geometry_ScreenToWidgetLocal(g,ue_vector2d(4.0lf,6.0lf),false)
 expect_geometry(near_geometry(local,-10.0lf/3.0lf,-20.0lf))
 let host=ue_ui_layout_GetViewportWidgetGeometry()
 if(!near_geometry(ue_ui_geometry_GetLocalSize(host),0.0lf,0.0lf)){panic("headless host geometry mismatch")}
 push(saved,g)
 saved <- clone(saved)
 ue_struct_collect()
 return checks
}
[export]
def widget_geometry(raw:UObjectHandle):int {
 let w=C_Widget_f2421369_from(raw)
 push(widgetSnapshots,ue_widget_GetCachedGeometry(w))
 push(widgetSnapshots,ue_widget_GetTickSpaceGeometry(w))
 push(widgetSnapshots,ue_widget_GetPaintSpaceGeometry(w))
 for(g in widgetSnapshots){if(!near_geometry(ue_ui_geometry_GetLocalSize(g),0.0lf,0.0lf)){panic("unlaid-out widget size mismatch")}}
 return length(widgetSnapshots)
}
[export]
def player_geometry(raw:UObjectHandle):int {
 let g=ue_ui_layout_GetPlayerScreenWidgetGeometry(C_PlayerController_fb3d8846_from(raw))
 if(!near_geometry(ue_ui_geometry_GetLocalSize(g),0.0lf,0.0lf)){panic("headless player geometry mismatch")}
 return 1
}
[export]
def read_saved(input:int):int {
 ue_struct_collect()
 if(length(saved)==0){return -1}
 if(!near_geometry(ue_ui_geometry_LocalToAbsolute(saved[0],ue_vector2d(4.0lf,6.0lf)),22.0lf,26.0lf)){return -2}
 for(g in widgetSnapshots){if(!near_geometry(ue_ui_geometry_GetLocalSize(g),0.0lf,0.0lf)){return -3}}
 return 1
}
[export]
def bad_default(input:int):int {var g:UEGeometry;let s=ue_ui_geometry_GetLocalSize(g);return int(s.x)}
[export]
def bad_unassigned(input:int):int {let c=ue_begin_static_call("/Script/UMG.SlateBlueprintLibrary","GetLocalSize","");ue_invoke_call(c);return 0}
[export]
def bad_overflow(input:int):int {let s=ue_ui_geometry_LocalToAbsolute(saved[0],ue_vector2d(3.0e38lf,3.0e38lf));return int(s.x)}
[export]
def bad_player(input:int):int {var p:C_PlayerController_fb3d8846;let g=ue_ui_layout_GetPlayerScreenWidgetGeometry(p);return 0}
[export]
def inverse_geometry(raw:UEStructValue):int {let s=ue_ui_geometry_AbsoluteToLocal(ue_geometry(raw),ue_vector2d(1.0lf,1.0lf));return int(s.x)}
[export]
def forward_geometry(raw:UEStructValue):int {let s=ue_ui_geometry_LocalToAbsolute(ue_geometry(raw),ue_vector2d(1.0lf,1.0lf));if(s.x==0.0lf && s.y==0.0lf){return 1};return 0}
