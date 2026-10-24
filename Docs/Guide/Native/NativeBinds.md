# Native C++ binds

A native bind is a C++ function or method that scripts call directly: one typed call, no reflection, no
`ProcessEvent` and no call frame. Binds are written by hand in a game or plugin module with `DAS_NATIVE_BINDS`
(`DasNativeBinds.h`); scripts see them as an ordinary daScript module.

Use them for hot code and for C++ that reflection cannot reach: methods that are not `UFUNCTION`s, `FMath`,
operators of math types, your own game classes. Reflection stays the general path: every `UFUNCTION` and
`UPROPERTY` is still reachable through [native class bindings](../../Reference/NativeClassBindings.md) and the
[generated bindings](../../Reference/GeneratedBindings.md).

The cost on this project's test machine: a native method call costs about 100–300 ns, a reflected `UFUNCTION`
call 10–40 µs ([Performance](../../Dev/Architecture/Performance.md#native-binds--2026-10-23)).

The plugin ships three native modules: `ue_engine` (actors, components, world), `ue_math` (`FMath`, math types)
and `ue_time` (`FDateTime`, `FTimespan`). Their functions are listed in the
[native modules reference](../../Reference/NativeModules.md).

## A module

The example below is the test module of the smoke project
(`Tests/DasForUESmoke/Source/DasForUESmoke/NativeBindsExample.cpp`). `AExampleTarget` is an ordinary `AActor`
subclass; its methods need no `UFUNCTION`.

<!-- snippet: Tests/DasForUESmoke/Source/DasForUESmoke/NativeBindsExample.cpp#module -->
```cpp
// A free function: arguments and results are ordinary C++ types.
static int32 DamageFor(int32 Base, float Multiplier)
{
    return FMath::RoundToInt(Base * Multiplier);
}
// A method can also be a function whose first parameter is the receiver.
static void Heal(AExampleTarget& Target, int32 Amount)
{
    if (Amount < 0)
        DasNativeError(TEXT("heal amount must not be negative")); // a script error after the call returns
    else
        Target.Health += Amount;
}
// Extends a type another module declared: every AActor (and AExampleTarget) gets this method.
static double DistanceToOrigin(const AActor& Actor)
{
    return Actor.GetActorLocation().Length();
}

DAS_NATIVE_BINDS(ExampleGame, "example_game")
{
    Binds.Function<&DamageFor>(TEXT("damage_for"), EDasBind::Pure);
    Binds.Class<AExampleTarget>(TEXT("AExampleTarget")) // the script type, named as in C++
        .Method<&AExampleTarget::GetHealth>(TEXT("get_health"))
        .Method<&AExampleTarget::IsAlive>(TEXT("is_alive"))
        .Method<&AExampleTarget::ApplyDamage>(TEXT("apply_damage"))
        .Method<&Heal>(TEXT("heal"));
    Binds.Class<AActor>().Method<&DistanceToOrigin>(TEXT("distance_to_origin"));
}
```

`DAS_NATIVE_BINDS(Id, "name")` registers the module when its code module loads; the script module `example_game`
is built before the next script compilation. A script uses it with `require`:

<!-- snippet: Tests/DasForUESmoke/Content/Das/Scripts/native_binds_example.das#script -->
```das
let target = as_AExampleTarget(ue_self_object()) // null when self is not an AExampleTarget
if (!target.valid()) { return 1 }
target.apply_damage(damage_for(10, 1.5))         // 15
target.heal(5)
if (target.get_health() != 90 || !target.is_alive()) { return 2 }
// Methods of the ancestor types: ue_engine's AActor and the one example_game adds to it.
let location = target.get_actor_location()
if (abs(target.distance_to_origin() - 500.0lf) > 0.001lf || location.x != 300.0lf) { return 3 }
// A typed reference converts to its ancestors and back.
let actor : AActor = as_AActor(target)
if (as_AExampleTarget(actor) != target) { return 4 }
```

A registration error (an unsupported name, a method of a class without a script type, a bad `Reflects`) makes the
whole module unavailable; `DasNativeModuleStatus(TEXT("example_game"))` returns the reason, the log has it too,
and `require example_game` fails to compile.

## Functions

`Binds.Function<&Fn>(TEXT("name"), Flags)` binds a free function. The address must be a template argument: wrap
functions imported from another DLL (engine functions) in a small lambda or function of your module, as
`ue_engine` does.

| C++ | Script |
| --- | --- |
| `bool`, `int32`, `int64`, `float`, `double`, `uint8` | `bool`, `int`, `int64`, `float`, `double`, `uint8` |
| `FString` | `string` |
| `FName`, `FText` | `UENameValue`, `UETextValue` |
| `FVector`, `FVector2D`, `FRotator`, `FQuat`, `FTransform`, `FLinearColor`, `FColor` | `UEVector`, `UEVector2D`, `UERotator`, `UEQuat`, `UETransform`, `UELinearColor`, `UEColor` |
| `FDateTime`, `FTimespan` | `UEDateTime`, `UETimespan` |
| `T*` (`T` a `UObject` class) | the script type of `T`, or `UObjectHandle` |

A const reference counts as its value type. Up to 16 parameters. Other types do not compile
(`static_assert` names the supported list).

Flags:

- `EDasBind::Pure` — no side effects and no game state: the compiler may evaluate a call with constant arguments
  while it compiles. Not allowed with object parameters or results.
- `EDasBind::NonNullArgs` — a null or expired object argument is a script error instead of `nullptr`.

A function name may also be an operator: `+ - * / % == != < <= > >=` (`-` with one parameter is unary minus).
Overloads with different parameter types may share a name.

`DasNativeError(TEXT("..."))` inside a bound function fails the script call with `<function>: <message>` after
the function returns; the first message of a call wins.

## Classes and methods

- `Binds.Class<T>(TEXT("Name"))` declares the script type `Name` for `T`. One module owns a class.
- `.Method<&T::Fn>(TEXT("name"))` binds a member function of `T` or of its base, or a function whose first
  parameter is `T*` / `T&`. Overloaded members are chosen with `static_cast`.
- `Binds.Class<T>()` without a name adds methods to a type that another module declared (`distance_to_origin` on
  `AActor` above).
- A type has the methods of its ancestor types: `target.get_actor_location()` above calls `ue_engine`'s `AActor`
  method on an `AExampleTarget`.

Every class type also gets `as_<Name>(x)` (from `UObjectHandle`, an ancestor or a descendant type; null when the
object is not a `<Name>`), `x.handle()`, `x.valid()`, `==` and `!=`. `var x : <Name>` is a null reference.

References are weak: a script does not keep objects alive, and a destroyed or collected object reads as null. A
null or expired receiver is a script error (`AExampleTarget.get_health: receiver is null or expired`); a null or
expired argument reaches C++ as `nullptr` unless the bind is `NonNullArgs`.

### Name types as in C++

Name a script type after its C++ class: `AExampleTarget`, like `ue_engine`'s `AActor`, `APawn`,
`UActorComponent`, `USceneComponent`, `UWorld`, `UGameInstance`. Short names clash: `Actor`, `Pawn` and
`ActorComponent` are the DSL base classes of `ue_dsl` (`class MyActor : Actor`), and the binding generator gives
other classes short aliases (`typedef Light = C_Light_<hash>`). In a script that requires both modules a shared
name is ambiguous in a type annotation (`var a : Actor`). The generator leaves out the alias for any name a native
module declares; pass your project's source folders with `--native-binds-source`.

## Gameplay calls through binds (`Reflects`)

Gameplay scripts call `UFUNCTION`s of native references with member syntax: `self.Trigger.K2_GetComponentLocation()`.
That goes through reflection. A method that does what a `UFUNCTION` does can declare it:

```cpp
Actor.Method<GetActorLocation>(TEXT("get_actor_location")).Reflects(TEXT("K2_GetActorLocation"));
```

Then the same gameplay call runs the bind: same syntax, same receiver checks (a live receiver in the owner's
world, no calls in `bind_events` / `EndPlay`, its class), no call frame. `ue_engine` reflects about 37
`UFUNCTION`s of `AActor`, `APawn`, `UActorComponent` and `USceneComponent`; the reference lists them.

`Reflects` is checked when the module is built: the `UFUNCTION` exists, is `BlueprintCallable` and native, is not
an event, static, network or editor-only function, has the same result and only input parameters of the same
types; the bind is not `NonNullArgs` (reflection passes `nullptr`); one bind per `UFUNCTION`. A mismatch makes the
module unavailable with the reason.

The gameplay call stays on reflection when the `UFUNCTION` requires authority or is unsafe in construction
scripts (the reflected call checks these), when its script types differ from the bind's (object parameters and
results: gameplay references are script structs), or when the script sets `options ue_native_routes = false`.

## AOT

Scripts compiled ahead of time call binds whose types all have an AOT ABI — scalars, strings, math values, dates
and time spans — through a guarded bridge. Binds with objects, `FName`, `FText` and operators stay closed: a
script function that calls one stays in the interpreter, the rest of the file is still compiled.

## Reload and unloading

A module that unloads (a game module rebuilt by Live Coding) only marks its binds unavailable: compiled calls
report `Native module '<name>' was unloaded` instead of running unloaded code. The next compilation picks up the
module registered by the new code.

## Inspecting modules

The `DasNativeBindsManifest` commandlet writes `logs/native-binds-manifest.json` (every module, script type and
function with its flags) and fails on a registration error. `Scripts/NativeModulesReference.py` turns it into the
[native modules reference](../../Reference/NativeModules.md); `Scripts/NativeBindsCoverage.py` compares the plugin
modules with the AngelScript binds ([coverage](../../Dev/Architecture/NativeBindsCoverage.md)).
