# DasForUE2

daScript as a gameplay language for Unreal Engine 5.8. You write Unreal classes,
structs, enums and interfaces in daScript; the editor turns them into real
Blueprint assets, and their logic runs with hot reload in the editor, in PIE and
in packaged games.

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

## Requirements

- Windows x64, source-built Unreal Engine 5.8.3
- Visual Studio 2022 17.14 / MSVC 14.44, Windows SDK 10.0.22621.0
- Git, CMake, Python 3.12; a CPU with AVX2

daScript 0.6.4 is pinned in `dependencies.json` and built into `ThirdParty/daScript`.

## Quick start

```powershell
.\Scripts\BuildDependencies.ps1          # build the pinned daScript
.\Scripts\CreatePlayableCharacter.ps1     # build the editor and create the playable example map
.\Scripts\CreatePlayableCharacter.ps1 -SkipBuild   # if the Editor target is already built
```

The second command creates a Character with camera, input, HUD, pickup, health
component and effects under `/Game/DasExamples/Playable/<id>` and prints the map
path. Source: `Plugins/DasForUE/Content/Das/Scripts/dsl_playable_character.das`.

To use the plugin in your own project see
[Standalone project](Docs/Guide/GettingStarted/StandaloneProject.md).

## Documentation

- [Documentation index](Docs/index.md): capability overview and all pages
- [Install](Docs/Guide/GettingStarted/Install.md) and [Examples](Docs/Guide/GettingStarted/Examples.md)
- [DSL guide](Docs/Guide/DSL/Classes.md): classes, properties, functions, events, components, types
- [Gameplay](Docs/Guide/Gameplay/Input.md), [UMG](Docs/Guide/UMG/WidgetEvents.md), [Data](Docs/Guide/Data/DataAssets.md)
- [Apply and hot reload](Docs/Guide/Workflow/ApplyAndHotReload.md), [Migrations](Docs/Guide/Workflow/Migrations.md)
- [Generated API packages](Docs/Api/README.md) and [Reference](Docs/Reference/TypeSupport.md)
- [Limitations](Docs/Reference/Limitations.md)

## Status

Development builds on Win64 (editor, PIE, packaged). Not yet covered: Shipping,
AOT for the whole gameplay path, networking/replication, other platforms and
universal migration of arbitrary types. See [Limitations](Docs/Reference/Limitations.md)
and the [roadmap](Docs/Dev/Plans/Roadmap.md).

History of changes: [CHANGELOG.md](CHANGELOG.md). Contributor docs (architecture,
audits, testing, validation reports): [Docs/Dev](Docs/index.md#for-contributors).

## Third-party notices

daScript is BSD-3-Clause, copyright Gaijin Games Kft.; its license and dependency
notices remain in `ThirdParty/daScript`. Distribution must preserve applicable notices.
