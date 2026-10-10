options gen2
module ue_reference_functions public
require ue_dsl public
require ue_scalar_refs public
// Ordinary functions: no ue_class or method annotations.
def apply_scalar_fields(actor:ScalarTargetRef):int {
  let alias = actor
  actor.Number += 5
  actor.Ratio = alias.Ratio + 0.5
  actor.Precise *= 2.0lf
  actor.Enabled = !alias.Enabled
  actor.Label = alias.Label + " changed"
  actor.Key = ue_name_value("Changed")
  return actor.Number + (actor.Ratio == 2.0 && actor.Precise == 4.5lf && !actor.Enabled && actor.Label == "Initial changed" && ue_names_equal(actor.Key,ue_name_value("Changed")) ? 100 : 0)
}
def read_scalar_field(actor:ScalarTargetRef):int {return actor.Number}
def cast_and_read(target:UObjectHandle):int {let actor=ue_ref(type<ScalarTargetRef>,target);return actor.Number}

def apply_value_fields(actor:ScalarTargetRef; peer:UObjectHandle):int {
  let original = actor.Payload
  actor.Payload = ue_struct_with_int(original,"Count",31)
  actor.Mode = ue_enum_value("EDasCallMode","Active")
  actor.Target = peer
  return ue_struct_get_int(actor.Payload,"Count") + (ue_enum_number(actor.Mode)==7 && ue_object_is_a(actor.Target,"Actor") && ue_struct_get_int(original,"Count")==10 ? 100 : 0)
}
def make_reference(target:UObjectHandle):ScalarTargetRef {return ue_ref(type<ScalarTargetRef>,target)}
