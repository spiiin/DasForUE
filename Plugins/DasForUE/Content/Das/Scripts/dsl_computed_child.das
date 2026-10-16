options gen2
require dsl_computed_defaults public
[ue_class(module_id="computeddefaults",Blueprintable)]
class ComputedChild : ComputedDefaults {
    defaults=ue_defaults() {
        self.Score=default_score(20)+self.Seed
        self.Derived=self.Score+2
    }
}

[ue_class(module_id="computeddefaults",Blueprintable)]
class ComputedSibling : ComputedDefaults {
    defaults=ue_defaults() {
        self.Derived=self.Score+1
    }
}
