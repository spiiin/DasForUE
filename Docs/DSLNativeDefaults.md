# Defaults for native host properties and built-in components

The existing `ue_defaults` syntax can target editable reflected properties on the
native Actor/ActorComponent base, without redeclaring those fields:

```dascript
[ue_class(module_id="gameplay",Blueprintable)]
class TimedActor : Actor {
    defaults=ue_defaults() {
        self.InitialLifeSpan=10.0f*3.0f
        self.bHidden=false
        self.RootComponent.RelativeScale3D=ue_vector(1.0lf,2.0lf,3.0lf)
    }
}
```

`RootComponent` here identifies the fixed default subobject referenced by the
native adapter CDO (`ProbeRoot` for the current Actor host). It does not create
an SCS component or replace the component reference. Targets must have the same
native class, name, outer and default-subobject identity on the authored CDO.
Editable native ActorComponent defaults, such as `bAutoActivate`, work too.

IR has optional `native_defaults` groups containing a component-reference name
(empty for the host) and the existing typed literal map. Own DSL `properties`
remain unchanged; no duplicate UPROPERTY/NewVariables entries are generated.
Native host bookkeeping fields such as AuthoredTypeId are not projected. Native
lookup starts at Engine.Actor/ActorComponent, not the bridge implementation class.
Native names use the exact Unreal reflection spelling, matching daScript case
sensitivity; case aliases cannot bypass descendant override protection.
Unknown, non-editable, edit-const, template-disabled and unsupported targets reject
before creating a Blueprint asset. The shared editable-value parser supports the
existing scalar/math/struct-text/strong asset-class defaults; containers and native
enum defaults are not added by this stage.

Pure calculations can read editable native scalar source defaults. The initial
value comes from the native host CDO, then authored parent source defaults and
earlier assignments take precedence. These are source values, not manual editor
overrides. Native component-field reads inside calculations and ordinary
`self.NativeProperty` access in gameplay methods are separate functionality;
this stage adds defaults destinations.

Initial import applies owned overrides to the new CDO/native subobjects only.
Authored descendants inherit parent values and may override them. Cooked source
fingerprints are scoped by authored class/target/property. Runtime source checks
validate the imported contract while permitting manual CDO values. A removed or
changed source default requires reimport.

Safe reimport uses the existing staged three-way merge for native host CDOs,
native component templates and their archetype descendants. Manual values and
manual struct members are preserved. Removing a source key returns unchanged
values to the actual immediate parent default. Explicit child defaults block
parent propagation even if equal to the previous parent value. Existing rollback,
world/idle/dependency guards apply; the merge does not reinstance or rerun gameplay.
Save packages after a successful reimport to persist the changes.

Native component defaults and authored SCS composition cannot be mixed in one
effective class yet: changing a root/subobject identity needs a separate policy.
The supported hosts remain Actor and ActorComponent. Character/Pawn adapters,
arbitrary native parents, component replacement and deep nested destinations are
future work.

Examples: `dsl_native_defaults.das`, `dsl_native_defaults_child.das`,
`dsl_native_defaults_component.das`. Run `Scripts/TestNativeDefaults.ps1`, or add
`-Package` for the Win64 Development NullRHI probe.


Final validation: TestNativeDefaults.ps1 -SkipBuild -Package passed all
Create/Reopen/Recompile phases and the Win64 Development NullRHI runtime probe.
Cooked actor/component CDO values, manual instance and struct-member overrides,
authored descendants, source contracts and fresh Spawn all passed. No engine
errors/ensures were reported by the final packaged probe. Editor and packaged
foundation each report zero failures. Existing computed defaults, inherited
defaults and safe-merge Create/Reopen/Recompile/Unopened regressions also passed.
Case aliases are rejected before asset creation. Shipping and rendered gameplay
were not exercised.

Logs: logs/native-defaults-final-suite.log, native-defaults-packaged.log,
native-defaults-foundation-editor.log and smoke-packaged.log. Packaging now uses
AutomationTool's -WaitForUATMutex so concurrent UAT runs serialize instead of
failing or stopping another run.
