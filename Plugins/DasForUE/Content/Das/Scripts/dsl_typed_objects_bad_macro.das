options gen2
require ue_dsl
typedef Bad = $UEObject<int>
[ue_class(module_id="badtyped", Blueprintable)]
class BadTarget {
    @ue_property
    Target : Bad
}
