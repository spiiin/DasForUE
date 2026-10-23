options gen2
require ue_foundation
require ue_time
require ue_math

// AOT against the interpreter (DasInteropProbe.inl, C3): input = iterations * 10 + mode; the results are folded
// with XOR so neither build can drop the loop. Self is a UDasObjectFixture with health == 7. Modes 4 and 5 call
// native binds through the guarded bridge: 4 a scalar one, 5 engine values (FVector by const reference and by
// value, a date, a string result) - six calls an iteration.
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
    } elif (mode==3) {
        for (i in range(0,count)) { acc^=ue_state_get_int("interop_missing",i) }
    } elif (mode==4) {
        for (i in range(0,count)) { acc^=days_in_year(2023)+i }
    } else {
        // The string result once (24 characters), not in the loop: each call allocates it on the context heap.
        let iso=length(make_date_time(2024,2,29).to_iso8601())
        for (i in range(0,count)) {
            let turned=forward_vector().rotate_angle_axis(180.0lf,up_vector())
            acc^=int(turned.size_2d()+0.5lf)+make_date_time(2024,2,29).get_day()+iso+i
        }
    }
    return acc
}
