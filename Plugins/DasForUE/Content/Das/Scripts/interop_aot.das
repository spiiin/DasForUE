options gen2
require ue_foundation

// AOT against the interpreter (DasInteropProbe.inl, C3): input = iterations * 10 + mode; the results are folded
// with XOR so neither build can drop the loop. Self is a UDasObjectFixture with health == 7.
def leaf(value:int):int { return value+1 }

[export]
def main(input:int):int {
    let mode=input % 10
    let count=input / 10
    var acc=0
    if (mode==0) {
        for (i in range(0,count)) { acc^=i & 7 }
    } elif (mode==1) {
        for (i in range(0,count)) { acc^=leaf(i) }
    } elif (mode==2) {
        for (i in range(0,count)) { acc^=ue_get_self_int("health")+i }
    } else {
        for (i in range(0,count)) { acc^=ue_state_get_int("interop_missing",i) }
    }
    return acc
}
