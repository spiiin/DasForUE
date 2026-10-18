options gen2
require ue_curve_tables

// Time can also be an item level, charge amount or other curve input.
def item_damage(curves:UObjectHandle;level:float):float {
    if (!ue_curve_table_has_row(curves,"Damage")) {return 0.0}
    return ue_curve_table_eval(curves,"Damage",level)
}
