# Install

DasForUE2 is a source plugin that embeds a pinned daScript 0.6.4 compiler/runtime in Unreal Engine
5.8. This page covers building the dependency, verifying the build with the included smoke project,
enabling the plugin, and creating the playable example.

## Prerequisites

| Requirement | Version |
| --- | --- |
| OS | Windows x64 |
| Unreal Engine | Source-built UE 5.8.3 |
| Compiler | Visual Studio 2022 17.14 / MSVC 14.44 |
| Windows SDK | 10.0.22621.0 |
| Tools | Git, CMake; Python 3.12 at `C:/Python312/python.exe` for the generator scripts |
| CPU | AVX2 (upstream CMake selects SIMD from the build host; other CPUs are not validated) |

The default engine location is `C:\src\unreal\UnrealEngine`; scripts that need the engine accept
`-Engine <path>`.

## 1. Get the source and build the daScript dependency

Clone the repository and run the scripts from its root:

```powershell
git clone https://github.com/spiiin/DasForUE.git C:\src\DasForUE
cd C:\src\DasForUE
```

The scripts are PowerShell files; if Windows refuses to run them ("running scripts is disabled on this
system"), allow local scripts for your user once with
`Set-ExecutionPolicy -Scope CurrentUser RemoteSigned`. Then build the dependency:

```powershell
.\Scripts\BuildDependencies.ps1
# If Visual Studio / Build Tools are not in C:\BuildTools\2022:
.\Scripts\BuildDependencies.ps1 -VisualStudio "C:\Program Files\Microsoft Visual Studio\2022\Community"
```

The script clones daScript into `ThirdParty/daScript` at the exact commit pinned in
`dependencies.json` and builds it. It refuses to continue if an existing checkout there is at a
different revision. Your own daScript checkouts elsewhere are never used or modified. It also copies
`daslib` into `Plugins/DasForUE/Content/Das/daslib` (a build output, not committed) and regenerates
`SmokeAot.cpp`. The first run takes a while: a full clone of daScript plus a CMake build.

## 2. Verify with the smoke project

The checkout includes an isolated test project, `Tests\DasForUESmoke\DasForUESmoke.uproject`, where
only this plugin is explicitly enabled. Build it and run the foundation checks in the editor and in a
packaged game:

```powershell
.\Scripts\TestEditor.ps1
.\Scripts\TestPackaged.ps1
# With a different engine:
.\Scripts\TestEditor.ps1 -Engine D:\UE\UnrealEngine
```

Packaged output goes to `Build/Packaged/Windows/`. More test scripts are described in
[Testing](../../Dev/Testing.md).

## 3. Enable the plugin in a project

The plugin lives in `Plugins/DasForUE`. A project uses it by pointing `AdditionalPluginDirectories`
at the checkout's `Plugins` folder and enabling it:

```json
{
  "FileVersion": 3,
  "AdditionalPluginDirectories": [ "C:/src/DasForUE/Plugins" ],
  "Plugins": [
    { "Name": "DasForUE", "Enabled": true },
    { "Name": "EnhancedInput", "Enabled": true }
  ]
}
```

The easiest way to get a correctly configured project is the template:
`.\Scripts\NewDasProject.ps1 -Destination C:/src/MyGame` — see
[Your own project](StandaloneProject.md). Game scripts go to `<project>/Content/Das/Scripts`;
the plugin's own scripts and modules are in `Plugins/DasForUE/Content/Das/Scripts`.

## 4. Create the playable example

The quickest way to see the plugin working is the playable Character example in the smoke project:

```powershell
.\Scripts\CreatePlayableCharacter.ps1
# If the Editor target is already built:
.\Scripts\CreatePlayableCharacter.ps1 -SkipBuild
```

It builds the editor target (unless `-SkipBuild`) and creates a fresh folder
`/Game/DasExamples/Playable/<id>` with a Character, a pickup, a HealthComponent, a Widget Blueprint
HUD, six InputActions, a MappingContext, a pickup mesh, two NiagaraSystems and a map. The shared
interaction interface is kept once in `/Game/DasExamples/Shared/Interactable`. The command prints the
map path; open it in the editor and press Play. Each run creates a new folder; existing ones are left
alone. `-Project <path.uproject>` selects a different project.

What the example contains and its controls: [Examples](Examples.md).

## Notes

- daScript is BSD-3-Clause (copyright Gaijin Games Kft.); its license and dependency notices are in
  `ThirdParty/daScript` and must be preserved when distributing.
- Only Win64 Development (editor and packaged) is covered; Shipping and other platforms are not.

See also: [Your own project](StandaloneProject.md) · [Examples](Examples.md) ·
[Apply and hot reload](../Workflow/ApplyAndHotReload.md) · [Limitations](../../Reference/Limitations.md)
