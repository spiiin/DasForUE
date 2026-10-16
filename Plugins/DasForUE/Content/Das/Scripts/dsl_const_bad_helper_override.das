options gen2
require ue_inheritance_base
[ue_class(Blueprintable)]
class BadHelperConst : BaseCounter {
    def override LocalRead():int { self.Score+=1;return self.Score }
}
