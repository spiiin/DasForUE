options gen2
require dsl_native_defaults public
[ue_class(module_id="nativedefaults",Blueprintable)]
class NativeDefaultsChild : NativeDefaults {
    defaults=ue_defaults() {
        self.InitialLifeSpan=20.0f
        self.RootComponent.RelativeScale3D=ue_vector(1.0lf,2.0lf,3.0lf)
    }
}
[ue_class(module_id="nativedefaults",Blueprintable)]
class NativeDefaultsComputedChild : NativeDefaults {
    defaults=ue_defaults() {
        self.InitialLifeSpan=self.InitialLifeSpan+1.0f
    }
}
