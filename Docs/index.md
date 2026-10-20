# DasForUE2 Documentation

DasForUE2 integrates daScript (gen2 syntax) into Unreal Engine 5.8. You write
classes, structs, enums and interfaces in daScript, apply them as real Blueprint
assets, and run their logic with hot reload in the editor, PIE and packaged games.

## What is implemented

| Area | Capabilities and current boundary |
| --- | --- |
| DSL | Classes, structs, enums and interfaces; properties and functions; annotations and declaration groups; defaults, components and inheritance. This is a separate authoring model, not direct placement of daScript objects in UE memory. |
| Calls | UE ↔ daScript calls with return/ref/out, Blueprint Function Libraries and WorldContext, typed object and class references. Arbitrary C++ and arbitrary virtual overrides are not exposed automatically. |
| Values | Native enums and structs, arrays, maps and sets, object references, `FName`/`FText`, `GameplayTag`, soft object/class references and supported soft containers. Exact type combinations have explicit limits. |
| Gameplay | Actor, Component, Pawn and Character; Initialize/ConstructionScript/BeginPlay/Tick/EndPlay; default components and attachments; find and spawn; input; timers; delegates; async and latent adapters; traces, damage, AI, perception, animation, effects, UMG widgets and data assets. Working examples exist. |
| Native API | Generation from reflection for discovered classes and declarative API packages (Math, World, Collision/Physics, Pawn/AI/Navigation, Animation/Audio/Niagara, UMG/Input) with supported signatures. A returned reference type does not by itself expose every method of its class. |
| Updating | Logic reload that keeps the last working version; selected structural and defaults migrations with preflight checks and bounded rollback. Universal migration of arbitrary types and objects is not promised. |

Known gaps are collected in [Limitations](Reference/Limitations.md); changes over
time are in the [changelog](../CHANGELOG.md).

## Getting started

- [Install](Guide/GettingStarted/Install.md)
- [Examples](Guide/GettingStarted/Examples.md)
- [Standalone project](Guide/GettingStarted/StandaloneProject.md)
- [Standalone template](../Examples/Standalone/Template/README.md)

## DSL guide

- [Classes](Guide/DSL/Classes.md)
- [Structs](Guide/DSL/Structs.md)
- [Enums](Guide/DSL/Enums.md)
- [Built-in types](Guide/DSL/BuiltinTypes.md)
- [Containers](Guide/DSL/Containers.md)
- [Functions](Guide/DSL/Functions.md)
- [Defaults](Guide/DSL/Defaults.md)
- [Components](Guide/DSL/Components.md)
- [Inheritance](Guide/DSL/Inheritance.md)
- [Lifecycle](Guide/DSL/Lifecycle.md)
- [Self access](Guide/DSL/SelfAccess.md)
- [Interfaces](Guide/DSL/Interfaces.md)
- [Delegates and events](Guide/DSL/DelegatesAndEvents.md)
- [Object and class references](Guide/DSL/ObjectAndClassReferences.md)
- [Authored references](Guide/DSL/AuthoredReferences.md)
- [Soft references](Guide/DSL/SoftReferences.md)
- [Function libraries](Guide/DSL/FunctionLibraries.md)
- [Modules and type identity](Guide/DSL/ModulesAndTypeIdentity.md)

## Gameplay

- [Spawn and queries](Guide/Gameplay/SpawnAndQueries.md)
- [Timers](Guide/Gameplay/Timers.md)
- [Callback arguments](Guide/Gameplay/CallbackArguments.md)
- [Input](Guide/Gameplay/Input.md)
- [Gameplay services](Guide/Gameplay/GameplayServices.md)
- [GAS health and instant effects](Guide/Gameplay/GAS.md)
- [AI and perception](Guide/Gameplay/AIAndPerception.md)
- [Animation and effects](Guide/Gameplay/AnimationAndEffects.md)
- [Async physics](Guide/Gameplay/AsyncPhysics.md)

## UMG

- [Widget events](Guide/UMG/WidgetEvents.md)
- [Drag and drop](Guide/UMG/DragDrop.md)
- [Paint and animation](Guide/UMG/PaintAndAnimation.md)

## Data

- [Asset loading](Guide/Data/AssetLoading.md)
- [Data assets](Guide/Data/DataAssets.md)
- [DataTables and curves](Guide/Data/DataTablesAndCurves.md)
- [GameplayTags](Guide/Data/GameplayTags.md)
- [SaveGame](Guide/Data/SaveGame.md)

## Workflow

- [Apply and hot reload](Guide/Workflow/ApplyAndHotReload.md)
- [Migrations](Guide/Workflow/Migrations.md)

## Engine API packages

- [Overview](Api/README.md)
- [World / Actor / Components](Api/WorldActorComponents.md)
- [Collision / Physics](Api/CollisionPhysics.md)
- [Pawn / Character / AI / Navigation](Api/PawnCharacterAINavigation.md)
- [Animation / Audio / Niagara](Api/AnimationAudioNiagara.md)
- [UMG / Input](Api/UMGInput.md)

## Reference

- [Type support](Reference/TypeSupport.md)
- [Generated bindings](Reference/GeneratedBindings.md)
- [Native class bindings](Reference/NativeClassBindings.md)
- [Struct snapshots](Reference/StructSnapshots.md)
- [Low-level calls](Reference/LowLevelCalls.md)
- [Limitations](Reference/Limitations.md)

## For contributors

- [Testing](Dev/Testing.md)
- Plans: [Roadmap](Dev/Plans/Roadmap.md), [Improvement plan](Dev/Plans/ImprovementPlan.md), [API expansion plan](Dev/Plans/ApiExpansionPlan.md)
- Architecture: [Integration choices](Dev/Architecture/IntegrationChoices.md),
  [Authoring model](Dev/Architecture/AuthoringModel.md),
  [DSL frontend](Dev/Architecture/DSLFrontend.md),
  [Interop](Dev/Architecture/Interop.md),
  [Performance](Dev/Architecture/Performance.md),
  [Shared property types](Dev/Architecture/SharedPropertyTypes.md)
- Audits: [API coverage](Dev/Audits/ApiCoverage.md),
  [DSL syntax](Dev/Audits/DSLSyntaxAudit.md),
  [Nim macro audit](Dev/Audits/NimMacroAudit.md),
  [AngelScript functions/events](Dev/Audits/AngelScriptFunctionsEventsAudit.md)
- Design: [Native DSL syntax](Dev/Design/DSLNativeSyntax.md),
  [DSL syntax check](Dev/Design/DSLSyntaxCheck.md),
  [Async physics handle contract](Dev/Design/AsyncPhysicsHandleContract.md),
  [Input event transport contract](Dev/Design/InputEventTransportContract.md)
- API blockers: [Collision / Physics](Dev/Api/Blockers/CollisionPhysicsBlockers.md),
  [Pawn / AI / Navigation](Dev/Api/Blockers/PawnAINavigationBlockers.md),
  [UMG export](Dev/Api/Blockers/UMGExportBlockers.md),
  [UMG input](Dev/Api/Blockers/UMGInputBlockers.md)
- [Validation records](Dev/Validation/)
