options gen2
require ue_soft_upcast_types
def replace(var value:SoftObject&):void { value=default<SoftObject> }
def ambiguous(a:SoftObject; b:SoftCurve):int { return 1 }
def ambiguous(a:SoftCurve; b:SoftObject):int { return 2 }
[export]
def main(input:int):int {
    var value=default<SoftCurve>;replace(value)
    return input
}
