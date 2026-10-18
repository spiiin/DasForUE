options gen2
require ue_data_tables
require datatable_item_types

// The items_table is loaded by the caller, for example via an authored asset reference.
def item_price(items_table:UObjectHandle;name:string):int {
    if (!ue_data_table_has_row(items_table,name)) {return -1}
    let item=ue_data_table_row(type<ItemRow>,items_table,name,"/Game/ItemTypes")
    return item.Price
}
