# Your Own Project

The recommended way to start a game with DasForUE2 is the `DasStarter` template: a separate Unreal
project that uses the plugin from your checkout, keeps its gameplay scripts in its own
`Content/Das/Scripts`, and generates its own native bindings. This page explains the workflow; the
step-by-step commands are in the [DasStarter README](../../../Examples/Standalone/README.md).

## Before you start

Complete [Install](Install.md) up to `Scripts/BuildDependencies.ps1`: you need a source-built UE 5.8
and the prepared daScript dependency in the checkout. The project generator also uses Python at
`C:/Python312/python.exe`.

## Create, generate, export

```powershell
./Scripts/NewDasProject.ps1 -Destination C:/src/DasStarter
./Scripts/GenerateNativeGameplay.ps1 -Project C:/src/DasStarter/DasStarter.uproject
& C:/src/unreal/UnrealEngine/Engine/Binaries/Win64/UnrealEditor-Cmd.exe `
  C:/src/DasStarter/DasStarter.uproject -run=DasStarter -unattended -NullRHI
```

| Step | Result |
| --- | --- |
| `NewDasProject.ps1 -Destination <folder>` | Copies `Examples/Standalone/Template`; refuses an existing folder |
| `GenerateNativeGameplay.ps1 -Project <uproject>` | Project-local native bindings + editor build |
| `-run=DasStarter` commandlet | Blueprints and `/Game/Starter/StarterMap` (only its own `/Game/Starter` assets) |

Pass `-Engine <path>` if your engine is not at `C:\src\unreal\UnrealEngine`. Then open the project
and press Play: WASD moves, mouse looks, and the cube pickup adds `Score` and respawns on a timer.

## How the project is wired

The generated `DasStarter.uproject` lists the checkout's `Plugins` folder in
`AdditionalPluginDirectories` and enables the plugins it needs:

```json
"Plugins": [
  { "Name": "DasForUE", "Enabled": true },
  { "Name": "EnhancedInput", "Enabled": true },
  { "Name": "Niagara", "Enabled": true }
]
```

To use DasForUE2 in a different project, apply the same idea: add the checkout's `Plugins` folder to
`AdditionalPluginDirectories`, enable `DasForUE` (and `EnhancedInput` if you use input), create
`Content/Das/Scripts`, and add a `Bindings/NativeGameplay.sources.json` listing your entry modules and
script folders before running `GenerateNativeGameplay.ps1 -Project`.

### Script lookup

- Named scripts (for `Das.DSL.Apply`, components, widgets, …) are found in
  `<project>/Content/Das/Scripts` first, then in `Plugins/DasForUE/Content/Das/Scripts`.
- `require my_module` uses the same order. A project module with the same name as a plugin module
  overrides it, so keep names unique.
- The daScript standard library and `daslib` resolve from the plugin as in standard daScript.

### Project-local native bindings

`GenerateNativeGameplay.ps1 -Project` discovers which native Unreal classes your scripts use and
generates all of their supported reflected members:

| Output | Content |
| --- | --- |
| `Bindings/NativeGameplay.json` | Reflected IR |
| `Source/DasGenerated/DasProjectNativeGameplay.generated.h` | Native runtime registry |
| `Content/Das/Scripts/ue_native_gameplay.das` | Typed daScript wrappers (`require ue_native_gameplay`) |

The plugin's global generated files are not touched. The runtime module picks the project registry
when the header exists; one registry per built project target is supported. UBT puts the plugin's DLLs
in `<project>/Binaries/Win64`.

## Iterating on your game

| Change | What to do |
| --- | --- |
| Method bodies, imported helper modules | Nothing — hot reload on the next call ([details](../Workflow/ApplyAndHotReload.md)) |
| Authored class/struct/enum declarations | `Das.DSL.Reimport` / `Das.DSL.Types.*` ([Migrations](../Workflow/Migrations.md)) |
| Using a native class or API not used before | Rerun `GenerateNativeGameplay.ps1 -Project …` (regenerates and rebuilds C++) |

Rerunning the `DasStarter` commandlet recreates its own StarterMap and Blueprints; keep a copy if you
edited them by hand.

## Limitations

- The template uses a source-built engine and the plugin sources from the checkout; a binary plugin
  distribution is not provided.
- Packaged builds of the standalone project have not been verified yet.

An automated check of the template is described in [Testing](../../Dev/Testing.md).

See also: [Install](Install.md) · [Examples](Examples.md) ·
[Generated bindings](../../Reference/GeneratedBindings.md) ·
[Modules and type identity](../DSL/ModulesAndTypeIdentity.md)

## Packaged scripts

Plugin fixtures and examples are excluded by default. See [Script staging](ScriptStaging.md) for runtime manifests, explicit example imports and project data files.
