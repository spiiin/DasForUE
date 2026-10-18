options gen2
require ue_enhanced_input
require ue_api_umg_input

var seen=0
var total=0
var saved:array<InputActionValue>
[export]
def on_value(value:InputActionValue):void {
    let kind=ue_input_type(value)
    if(kind==InputValueType.Boolean) { verify(ue_input_bool(value),"button payload"); seen=seen | 1 }
    elif(kind==InputValueType.Axis1D) { verify(ue_input_axis1d(value)==0.5lf,"axis1 payload"); seen=seen | 2 }
    elif(kind==InputValueType.Axis2D) { verify(ue_input_axis2d(value).y==4.0lf,"axis2 payload"); seen=seen | 4 }
    else { verify(ue_input_axis3d(value).z==3.0lf,"axis3 payload"); seen=seen | 8 }
    push(saved,value)
    total+=1
}
[export]
def main(input:int):int {
    if(input==0) {
        ue_enhanced_bind(InputActionRef_from(ue_get_self_object("Action")),"Triggered","on_value")
        let widget=ue_ref(type<Widget>,ue_get_self_object("Widget"))
        var refused=false
        try {
            ue_widget_SetNavigationRuleCustom(widget,E_EUINavigation_e4554eeb.v_Right,ue_function_delegate(ue_self_object(),"WrongNavigation"))
        } recover { refused=true }
        verify(refused,"mismatched native delegate must be rejected")
        ue_widget_SetNavigationRuleCustom(widget,E_EUINavigation_e4554eeb.v_Right,ue_function_delegate(ue_self_object(),"ChooseNavigation"))
        return 42
    }
    verify(seen==15 && total==4 && length(saved)==4,"all callback shapes delivered once")
    verify(ue_input_axis2d(saved[2]).y==4.0lf,"retained owned callback copy")
    return 42
}
