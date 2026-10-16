# Method parameter defaults

Authored Actor and ActorComponent methods support ordinary daScript defaults:

```das
options gen2
require ue_dsl

[ue_class]
class Counter : Actor {
    [ue_function(BlueprintPure)]
    def Add(left:int=7; right:int=-2):int { return left+right }

    [ue_function(BlueprintCallable)]
    def Example():int {
        return self.Add()+self.Add(10)+self.Add(10,3) // 5+8+13
    }
}
```

The same omitted-argument syntax works on typed authored references, including
from ordinary DAS functions. A disconnected Blueprint input pin starts with the
declared value. Defaults persist through saving, reopening and recompilation.
Explicitly supplied arguments keep their usual meaning. ProcessEvent callers
must still provide the complete native parameter storage.

Supported defaults are trailing input literals of type int, float, double, bool
and string. Negative numeric literals, Unicode and empty strings are supported.
Defaults for out/ref, WorldContext, complex types and computed expressions are
rejected. The instance and static-library collectors share the literal reader
and IR validation. Generated native ABI entry points still require all arguments;
source-level and reference-call functions fill omitted arguments before entering
that ABI.

Changing a literal default requires declaration reimport to refresh Blueprint
metadata. Existing caller pin values are not automatically migrated. Ordinary
function inheritance and BlueprintEvent dispatch retain their existing rules;
defaults are call-site values, not a replacement for virtual dispatch.

Examples and checks:

- `dsl_defaults_actor.das` and `dsl_defaults_component.das`: scalar defaults and
  same-object calls.
- `dsl_defaults_actor_calls.das` and `dsl_defaults_component_calls.das`: typed
  reference calls from a free function.
- `dsl_defaults_child.das` and `dsl_defaults_child_calls.das`: inherited virtual
  calls and Super.
- `Scripts/TestStaticLibrary.ps1`: the shared literal/default regression suite
  includes Actor and ActorComponent consumers, negative declarations, saving,
  reopening and Blueprint recompilation.

Validation: strict DasForUE.Gameplay.RuntimePIE regression PASS (1 success,
0 failures, 0 warning tests; instance-defaults-pie.log and
instance-defaults-pie-report/index.json). The original stage covered saved/reopened commandlet consumers. The game-world
and packaged checks below extend that coverage.


## Game-world and packaged validation

`RunDasInstanceDefaultsProbe` runs the same native probe inside RuntimePIE and
the Win64 Development packaged gameplay executable. It creates an authored Actor,
an authored ActorComponent and an inherited Actor after the world has begun play.
The component is dynamically registered on the Actor and must also begin play.

Both hosts exercise their own methods, typed references from free DAS functions,
and compiled Blueprint consumers with disconnected input pins. Checks include
omitted, partial and explicit arguments; float/double precision; bool, Unicode
and empty-string literals; virtual calls with base/derived call-site defaults;
and Super calls. The probe releases its persistent contexts and destroys all of
its temporary objects. PIE's existing context-lifetime assertions also pass.

One editor fixture helper prepares the same source classes and Blueprint consumers
for both paths. Reflected class references on the map's native carrier retain
the consumer classes and their parent hierarchies through cook and GC. The
packaged process loads those dependencies and runs without an editor module.

Validated 2026-10-17: Editor build, strict RuntimePIE (one successful test, zero
failures or warning tests), Win64 Development build/cook/stage/Pak/archive,
packaged foundation (`failures=0`) and packaged gameplay, all PASS under NullRHI.
Run `Scripts/TestGameplayRuntimePIE.ps1` and `Scripts/TestCookedGameplay.ps1`.
Logs: `instance-defaults-game-pie.log`, `instance-defaults-game-pie-report.json`,
`instance-defaults-game-cooked.log`, `cooked-gameplay-runtime.log`,
`smoke-packaged.log`. Shipping and rendered gameplay were not exercised.
