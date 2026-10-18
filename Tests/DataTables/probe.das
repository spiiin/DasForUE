options gen2
require ue_data_tables
require datatable_item_types
require __EXAMPLE__
var remembered=ItemRow()
[export]
def start(input:int):int {
    let items_table=ue_self_object()
    let names=ue_data_table_names(items_table)
    if (length(names)!=2 || names[0]!="Potion" || names[1]!="Sword") {return -1}
    if (!ue_data_table_has_row(items_table,"potion") || ue_data_table_has_row(items_table,"Missing")) {return -2}
    if (item_price(items_table,"Sword")!=125 || item_price(items_table,"Missing")!=-1) {return -3}
    let row=ue_data_table_row(type<ItemRow>,items_table,"Potion","__FOLDER__")
    if (row.Label!="Health Potion" || row.Price!=25 || row.Weight!=0.5lf) {return -4}
    remembered=row
    var rejected=false
    try { let missing=ue_data_table_row(type<ItemRow>,items_table,"Missing","__FOLDER__");print("{missing.Price}") } recover {rejected=true}
    if (!rejected) {return -5}
    rejected=false
    try { let wrong=__ue_data_table_row(items_table,"Potion","/Script/CoreUObject.Vector");print("wrong accepted") } recover {rejected=true}
    if (!rejected) {return -6}
    return 1
}
[export]
def after_mutation(input:int):int {
    if (ue_data_table_has_row(ue_self_object(),"Potion")) {return -1}
    return (remembered.Label=="Health Potion" && remembered.Price==25 && remembered.Weight==0.5lf) ? 1 : -2
}
[export]
def empty_table(input:int):int {return length(ue_data_table_names(ue_self_object()))==0 ? 1 : -1}
