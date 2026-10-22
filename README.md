<p align="center">
  <img src="Docs/Images/dasforue-logo.webp" alt="DasForUE" width="280">
</p>

<p align="center">
  <b>daScript (daslang) as a gameplay language for Unreal Engine 5</b><br>
  Write Unreal classes in a fast scripting language, get real Blueprint assets, and change them live.
</p>

<p align="center">
  <a href="https://www.youtube.com/watch?v=7lqbYKtIZ3c">▶ Hot reload demo (video)</a> ·
  <a href="Docs/index.md">Documentation</a> ·
  <a href="Docs/Guide/GettingStarted/Install.md">Install</a> ·
  <a href="CHANGELOG.md">Changelog</a>
</p>

---

DasForUE embeds [daScript](https://github.com/GaijinEntertainment/daScript) into Unreal Engine 5.8. You declare
actors, components, structs, enums, interfaces, data assets and GAS definitions in daScript; the editor turns them
into ordinary Blueprint assets that designers can extend, place and edit. Method bodies hot reload while you play,
and saving a script migrates its Blueprints, placed actors and data in place.

[![Hot reload demo](https://img.youtube.com/vi/7lqbYKtIZ3c/hqdefault.jpg)](https://www.youtube.com/watch?v=7lqbYKtIZ3c)

## A taste of the DSL

An actor with editable properties, a lifecycle hook and a Blueprint-callable function:

```das
options gen2
require ue_dsl
require ue_native_gameplay public

[ue_class(Blueprintable)]
class Counter : Actor {
    state=ue_properties(EditAnywhere,BlueprintReadWrite) {
        var Score:int=0
    }
    def BeginPlay():void { self.Score=0 }

    [ue_function(BlueprintCallable)]
    def AddScore(amount:int):void { self.Score+=amount }
}
```

A component that reacts to an engine event and broadcasts its own event to Blueprints:

```das
[ue_class(Blueprintable)]
class HealthComponent : ActorComponent {
    settings=ue_properties(EditAnywhere,BlueprintReadWrite) {
        var Maximum:float=100.0f
    }
    state=ue_properties(BlueprintReadWrite) {
        var Current:float=100.0f
    }
    events=ue_properties(BlueprintAssignable) { var Changed:HealthChangedSignal }

    def BeginPlay():void { self.Current=self.Maximum }

    [ue_event(source=owner,source_type=ActorRef,name=OnTakeAnyDamage)]
    def OnDamage(damaged:ActorRef; amount:float; kind:ObjectRef; instigator:ObjectRef; source:ActorRef):void {
        if(amount>=self.Current) { self.Current=0.0f } else { self.Current-=amount }
        self.Changed.Broadcast(self.Current,self.Maximum)
    }
}
```

Structs and enums become Blueprint types; GAS attribute sets and effects become real GAS assets:

```das
[ue_struct(id="direct.Settings", module_id="direct", BlueprintType)]
struct GameSettings {
    @ue_property=(EditAnywhere, BlueprintReadWrite)
    Count : int = 15
    @ue_property=(EditAnywhere, BlueprintReadWrite)
    Title : string = "Initial"
}

[ue_attribute_set]
class CombatAttributes {
    @min=0
    @max="MaxHealth"
    Health:float=100.0f
    @min=1
    MaxHealth:float=100.0f
}

[ue_gameplay_effect(attributes="CombatAttributes")]
class PotionHeal {
    Health:float=25.0f
}
```

More: [Examples](Docs/Guide/GettingStarted/Examples.md) and the scripts in
`Plugins/DasForUE/Content/Das/Scripts` (the playable character, AI enemy, UMG, GAS).

## What works today

- **Two-way daScript ↔ Blueprint.** Blueprints call daScript functions, read and write its properties and bind to
  its events; daScript calls engine and Blueprint functions, raises delegates and handles engine events.
- **Hot reload.** Edit method bodies while playing in the editor (PIE) and see the change on the next call. Saving a
  script also reimports its declaration: new and removed fields (values archived), renames, numeric retypes,
  methods, components, events, interfaces and parent classes migrate the class, its placed actors and saved data in
  place, with a rollback on failure. Changes that touch other assets ask first. See the
  [reload matrix](Docs/Reference/ReloadMatrix.md) for every case.
- **A DSL for most Unreal entities:** actors, pawns and characters, components (native and authored), structs,
  enums, interfaces, delegates and events, function libraries, DataAssets and Primary Assets, DataTables, SaveGame,
  UMG widgets, and GAS (attribute sets, effects, abilities, calculations, cues).
- **Core gameplay modules:** input (Enhanced Input), spawning and queries, timers, collision and physics, animation
  and effects (Niagara), AI and perception, UMG, GAS, generated bindings for a large part of the engine API.
- **Editor, PIE and packaged Development builds** on Win64.

## Planned

- **daScript ↔ C++:** call project C++ from scripts and scripts from C++ without Blueprint glue; AOT for the whole
  gameplay path.
- **Performance:** fewer marshalling layers on hot calls, AOT/JIT where it pays, profiling hooks.
- **Closing gaps in the bindings:** the remaining refused reload cases, binding blockers listed in
  [Limitations](Docs/Reference/Limitations.md).
- **Broader engine coverage:** more engine modules and plugins (Mass, StateTree, Chaos vehicles, audio, sequencer, …).
- **Networking:** replicated properties and RPCs.
- **More platforms and configurations:** Shipping builds, Linux and macOS.
- **Docs and samples:** a step-by-step tutorial and a small sample game.

## Requirements

- Windows x64, source-built Unreal Engine 5.8.3
- Visual Studio 2022 17.14 / MSVC 14.44, Windows SDK 10.0.22621.0
- Git, CMake, Python 3.12; a CPU with AVX2

daScript 0.6.4 is pinned in `dependencies.json` and built into `ThirdParty/daScript`.

## Quick start

```powershell
git clone https://github.com/spiiin/DasForUE.git
cd DasForUE
.\Scripts\BuildDependencies.ps1          # build the pinned daScript
.\Scripts\CreatePlayableCharacter.ps1     # build the editor and create the playable example map
```

The second command creates a Character with camera, input, HUD, pickup, health component and effects under
`/Game/DasExamples/Playable/<id>` and prints the map path. To use the plugin in your own project, see
[Standalone project](Docs/Guide/GettingStarted/StandaloneProject.md).

## Documentation

- [Documentation index](Docs/index.md) and [Install](Docs/Guide/GettingStarted/Install.md)
- [DSL guide](Docs/Guide/DSL/Classes.md): classes, properties, functions, events, components, types
- [Gameplay](Docs/Guide/Gameplay/Input.md), [UMG](Docs/Guide/UMG/WidgetEvents.md), [Data](Docs/Guide/Data/DataAssets.md),
  [GAS](Docs/Guide/Gameplay/GAS.md)
- [Apply and hot reload](Docs/Guide/Workflow/ApplyAndHotReload.md), [Migrations](Docs/Guide/Workflow/Migrations.md)
- [Generated API packages](Docs/Api/README.md), [Reference](Docs/Reference/TypeSupport.md),
  [Limitations](Docs/Reference/Limitations.md)

## License

MIT, see [LICENSE](LICENSE). daScript is BSD-3-Clause, copyright Gaijin Games Kft.; its license and dependency
notices remain in `ThirdParty/daScript`. Distribution must preserve applicable notices.
