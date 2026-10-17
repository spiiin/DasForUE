options gen2
require ue_soft_upcast_types
def replace(var value:SoftObject):void { value=default<SoftObject> }
[export]
def main(input:int):int {
    var curve=default<SoftCurve>
    replace(curve)
    return input
}
