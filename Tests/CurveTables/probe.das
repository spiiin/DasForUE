options gen2
require ue_curve_tables
require curvetable_items
var previous_names:array<string>
[export]
def read(input:int):int {
    let curves=ue_self_object()
    previous_names <- ue_curve_table_names(curves)
    if (length(previous_names)!=3 || previous_names[0]!="Damage" || previous_names[1]!="Empty" || previous_names[2]!="StoredDefault") {return -1}
    if (!ue_curve_table_has_row(curves,"damage") || ue_curve_table_has_row(curves,"Missing")) {return -2}
    let expected=input==0 ? 15.0 : 3.125
    let delta=item_damage(curves,0.5)-expected
    if (delta>0.0001 || delta< -0.0001) {return -3}
    if (ue_curve_table_eval(curves,"Damage",0.0)!=(input==0 ? 10.0 : 0.0)) {return -4}
    if (ue_curve_table_eval(curves,"Damage",2.0)!=(input==0 ? 30.0 : 20.0)) {return -5}
    if (ue_curve_table_eval(curves,"Damage",-1.0)!=0.0 || ue_curve_table_eval(curves,"Damage",3.0)!=(input==0 ? 40.0 : 20.0)) {return -6}
    if (ue_curve_table_eval(curves,"Empty",1.0,-3.5)!=-3.5 || ue_curve_table_eval(curves,"Empty",1.0)!=0.0) {return -7}
    if (ue_curve_table_eval(curves,"StoredDefault",1.0,-3.5)!=7.0) {return -8}
    var rejected=false
    try { ue_curve_table_eval(curves,"Missing",1.0) } recover {rejected=true}
    if (!rejected) {return -9}
    rejected=false
    try { ue_curve_table_eval(curves,"None",1.0) } recover {rejected=true}
    if (!rejected) {return -10}
    return 1
}
[export]
def after_clear(input:int):int {
    if (length(ue_curve_table_names(ue_self_object()))!=0 || ue_curve_table_has_row(ue_self_object(),"Damage")) {return -1}
    return (length(previous_names)==3 && previous_names[0]=="Damage") ? 1 : -2
}
[export]
def wrong_owner(input:int):int {
    var rejected=false
    try { ue_curve_table_eval(ue_self_object(),"Damage",1.0) } recover {rejected=true}
    return rejected ? 1 : -1
}
