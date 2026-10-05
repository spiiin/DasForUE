// Experimental annotation spelling, NOT the proposed public DSL syntax.
options gen2
module ue_schema_probe public
require daslib/ast
require daslib/rtti
require daslib/ast_boost
require daslib/templates_boost
require ue_foundation public

[structure_macro(name="ue_probe")]
class ProbeTypeMacro : AstStructureAnnotation {
    def override apply(var st : StructurePtr; var group : ModuleGroup;
                       args : AnnotationArgumentList; var errors : das_string) : bool {
        if (length(args) != 0) {
            errors := "ue_probe does not accept arguments in this feasibility test"
            return false
        }
        var fields = ""
        for (field in st.fields) {
            if (field._type.baseType != Type.tFloat && field._type.baseType != Type.tInt) {
                errors := "ue_probe currently accepts only explicitly typed int/float fields"
                return false
            }
            let kind = field._type.baseType == Type.tFloat ? "float" : "int"
            fields += "{field.name}:{kind};"
        }
        let functionName = "emit_{st.name}"
        let typeName = string(st.name)
        var fn = qmacro_function(functionName) $() : int {
            return ue_emit_type($v(typeName), "Actor", $v(fields))
        }
        fn.flags.generated = true
        fn.body |> force_at(st.at)
        add_function(st._module, fn)
        return true
    }
}
