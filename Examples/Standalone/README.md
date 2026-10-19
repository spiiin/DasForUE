# DasStarter: a standalone UE project with daScript

The template in `Examples/Standalone/Template` creates a separate Unreal project, `DasStarter`, with
its own C++ targets, gameplay `.das` scripts, Blueprint assets and a map. It references the plugin
from your DasForUE2 checkout through `AdditionalPluginDirectories`, so it is a source integration
example, not a binary plugin distribution. It does not need anything from `Tests/DasForUESmoke`.

## Requirements

- Windows x64 and a source-built UE 5.8 (see [Install](../../Docs/Guide/GettingStarted/Install.md)).
- The daScript dependency already prepared in the checkout (`Scripts/BuildDependencies.ps1`).
- Python at `C:/Python312/python.exe` (the path used by the checkout's tools; edit the scripts if your
  Python is installed elsewhere).

## Create the project

Run from the DasForUE2 checkout root:

```powershell
./Scripts/NewDasProject.ps1 -Destination C:/src/DasStarter
./Scripts/GenerateNativeGameplay.ps1 -Project C:/src/DasStarter/DasStarter.uproject
& C:/src/unreal/UnrealEngine/Engine/Binaries/Win64/UnrealEditor-Cmd.exe `
  C:/src/DasStarter/DasStarter.uproject -run=DasStarter -unattended -NullRHI
```

1. `NewDasProject.ps1` copies the template to `-Destination` and points the project at the
   checkout's `Plugins` folder. An existing destination folder is refused, never overwritten. The
   project name inside is always `DasStarter`; `-Destination` only chooses the folder.
2. `GenerateNativeGameplay.ps1 -Project` discovers the native classes the scripts use, generates
   their bindings into the project and builds the editor target.
3. The `DasStarter` commandlet creates the Blueprints and `/Game/Starter/StarterMap`.

All scripts accept `-Engine C:/path/to/UnrealEngine` (default `C:\src\unreal\UnrealEngine`).

The commandlet only rewrites its own assets in `/Game/Starter`. Do not run it over a StarterMap
you edited by hand without keeping a copy.

## Play

Open `C:/src/DasStarter/DasStarter.uproject` in the editor and press Play on
`/Game/Starter/StarterMap`.

- WASD: move. Mouse: look.
- The cube on the platform is a pickup: touching it adds to `Score`, hides it, and it comes back on a
  timer. `Score` is a Blueprint property of the character; the template has no score UI.

The map uses a Movable DirectionalLight, so no light baking is needed. To refresh only the lighting of
an existing StarterMap without recreating objects:

```powershell
& C:/src/unreal/UnrealEngine/Engine/Binaries/Win64/UnrealEditor-Cmd.exe `
  C:/src/DasStarter/DasStarter.uproject -run=DasStarter -LightingOnly -unattended -NullRHI
```

## Where the code lives

| File in the new project | Purpose |
| --- | --- |
| `Content/Das/Scripts/starter_character.das` | Character, camera, input and `Score` |
| `Content/Das/Scripts/starter_component.das` | Authored ActorComponent with state |
| `Content/Das/Scripts/starter_pickup.das` | Sphere/StaticMesh, overlap event, respawn timer |
| `Content/Das/Scripts/starter_refs.das` | Typed references to the authored classes |
| `Content/Das/Scripts/starter_values.das` | Ordinary imported daScript module |
| `Source/DasStarterEditor/DasStarterCommandlet.cpp` | Creates the Blueprints and the map |
| `Source/DasStarter/DasStarter.cpp` | Game module (its test probe runs only with `-DasStarterProbe`) |
| `Bindings/NativeGameplay.sources.json` | Entry modules and script folders for the native generator |

Put your own gameplay code in the project's `Content/Das/Scripts`, not in the plugin's Scripts folder.

## Script lookup and native bindings

Named gameplay scripts are looked up first in `<project>/Content/Das/Scripts`, then in the plugin's
Scripts folder. A plain `require starter_values` follows the same order; the daScript standard
library stays in the plugin. Keep project module names unique: a project module with the same name as
a plugin module deliberately overrides it. Relative paths and `daslib` resolve as in standard
daScript.

`GenerateNativeGameplay.ps1 -Project <uproject>` writes project-owned outputs and never replaces the
plugin's global generated files:

- `Bindings/NativeGameplay.json` — reflected IR;
- `Source/DasGenerated/DasProjectNativeGameplay.generated.h` — native runtime registry;
- `Content/Das/Scripts/ue_native_gameplay.das` — typed daScript wrappers.

The plugin's `Runtime.Build.cs` uses the project registry when that header exists and adds the
project scripts to runtime dependencies for staging. One registry per built project target is
supported. UBT places the external plugin's DLLs in `<project>/Binaries/Win64`; the plugin sources
themselves stay shared with the checkout.

## Iterating

| You changed | What to do |
| --- | --- |
| A method body or an imported module | Nothing: runtime hot reload picks it up on the next call |
| An authored class/struct/enum declaration | Reimport the asset (`Das.DSL.Reimport`, see [Migrations](../../Docs/Guide/Workflow/Migrations.md)) |
| The set of native APIs the scripts use | Rerun `GenerateNativeGameplay.ps1 -Project …` (regenerates and rebuilds C++) |

Packaged builds of the standalone project have not been verified yet.

See also: [Standalone project guide](../../Docs/Guide/GettingStarted/StandaloneProject.md) ·
[Apply and hot reload](../../Docs/Guide/Workflow/ApplyAndHotReload.md) ·
[Generated bindings](../../Docs/Reference/GeneratedBindings.md)
