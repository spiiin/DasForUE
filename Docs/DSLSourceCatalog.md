# Authored source catalog and generated reference imports

Added 2026-10-10. **Not built or tested**: the commandlet, examples, runtime
resolution, reload and packaged staging have not been executed.

The source catalog maps authored Actor/Component class names and stable IDs to
local DAS source filenames. It is separate from Unreal reflected asset registries:
enum/struct asset lookup still uses the existing type_folder/type_registry rules.
Version 3 additionally inventories interface declarations and ready-made interface
reference modules. Version 2 inventories enum/struct source modules and generates their
public imports; existing version 1 catalogs remain readable.

Before:
```das
[ue_object_ref(script="counter_actor.das", type="CounterActor")]
struct CounterActorRef { handle:UObjectHandle }
```
After, with `ue_type_catalog.json` in the runtime Scripts folder:
```das
[ue_object_ref(type="CounterActor")]
struct CounterActorRef { handle:UObjectHandle }
```
An explicit catalog can be selected with `catalog="my_catalog.json"`. A stable
ID can be used instead of a simple class name. Unknown/ambiguous names fail;
resolution never chooses an arbitrary file. Explicit script remains supported,
but script and catalog together are rejected.

## Generate a catalog and all class reference wrappers

The new editor commandlet consumes an explicit comma-separated source list.
It reads the existing compiler-produced authoring model, not a regex parser of
DAS/C++. Source classes must already compile under their type/asset setup.

```powershell
UnrealEditor-Cmd.exe YourProject.uproject -run=DasSourceCatalog `
  -Scripts=counter_actor.das,score_component.das `
  -Output=ue_type_catalog.json -Module=ue_project_refs -unattended -nop4
```

Output goes to the runtime Scripts folder. The default output names are those
shown above. `-Check` compares existing outputs without writing them. Entries and
wrappers are sorted by stable ID; duplicate class names receive distinct generated
reference names with an ID hash suffix. Duplicate IDs/locations and reference-name
collisions are refused. A write failure returns failure; regenerate the pair before
use (the two output files are not an atomic filesystem transaction).

Then import the generated wrappers together:
```das
require ue_project_refs
let actor = ue_ref(type<CounterActorRef>, target)
actor.AddScore(5)
```

The generated module imports selected enum/struct modules publicly as well as
declaring the selected class reference wrappers. One user `require` exposes both:
```das
require ue_project_refs
// CounterState and CounterData come from ue_game_types.
let actor = ue_ref(type<CounterActorRef>, target)
actor.Apply(data)
```

By default, class compilation discovers imported dedicated enum/struct modules,
including transitive imports already loaded into the compiler library. `-TypeScripts`
can additionally select value modules not imported by the selected classes.
`-ExplicitTypeImports` disables discovery and uses only that explicit list. Their real DAS
module names come from compilation, and must match local source filenames for
the standard require loader. TypeScripts modules containing ue_class/ue_interface declarations
are refused; use InterfaceScripts for interface modules. Type-only catalogs are supported (omit -Scripts).
The generator does not scan unrelated source files. Discovery follows the compiled
module library for selected class roots; dependencies declared by the discovered
modules use normal DAS resolution. It inventories complete modules, not only types
mentioned in a particular method signature. Type-only generation still uses an
explicit TypeScripts root list.
Initial cyclic-source bootstrapping remains separate work. Interface inventories
are covered by version 3; generating their actual call bindings remains the existing
reflection binding generator’s responsibility. Module/type names and stable IDs are cataloged; reflected assets are not
created by this commandlet. Required asset setup still precedes class generation.

## Resolution and reload contract

- Catalog JSON schema version 3 (versions 1 and 2 remain readable); up to 1024 class
  entries, 256 value modules and 1024 inventoried value types, at most 1 MiB on read.
  At least one class or value module is required.
- Each class entry has `id`, `name`, `script` and generated `reference` name.
  Each value module has `module`, `script`, and `types` with `kind`/`id`/`name`.
  Duplicate module/source/ID and wrapper/value-name collisions fail before output.
  Generation over an input source and generated-module self-import are rejected.
- Catalog/source filenames stay local to Scripts; generated module identifiers
  use ASCII letters/digits/underscore. No arbitrary directory scanning/loading.
- Resolution canonicalizes the reference annotation to the existing source/name
  representation, so properties, methods, components and containers share it.
- The expected stable ID is retained in reserved compiler metadata and checked
  when the source declaration is resolved. A stale mapping cannot silently bind
  a different logical class. User-written reserved metadata is refused.
- Catalog bytes (including a missing catalog) join persistent compile dependency
  tracking. Source files use the existing dependency path. Last-good behavior
  remains governed by the existing runtime compile/reload machinery.
- Runtime build rules already stage all files under Content/Das, including JSON.
  Packaged behavior still needs verification.

Examples: `ue_example_source_catalog.json`, `ue_example_catalog_refs.das`,
`ue_struct_map_actor_refs.das` and `dsl_catalog_access.das`. Examples use a named
catalog to avoid installing a project-wide default into unrelated fixtures.

Pending verification: build/UHT; simple-name and ID lookup; unknown/ambiguous names;
duplicate IDs/locations/wrapper names; bad filenames and malformed/oversized JSON;
stale IDs; source cycles; canonical accessor/component keys; empty explicit script;
generate/check determinism; catalog edits/deletion/restoration and last-good reload;
ordinary field/method access, arrays/maps, PIE and packaged UFS reads.

## Value import extension (validation pending)

No build or tests were run for the version 2 extension. The examples now use one
require for math handles, authored value types and class wrappers. Inventory is
generated from compiler models; it is not an additional independent definition
of the enum/struct layout. Native asset/type registry validation remains unchanged.

Additional pending checks: version 1 compatibility; value-only catalogs; mismatched
module filenames; dedicated module refusal; duplicate IDs/module exports and wrapper
collisions; deterministic public imports; transitive symbol visibility; source/cook
dependency tracking; generated-module self-import and input overwrite prevention.

## Compiler-based import discovery (validation pending)

For class-root generation, enum/struct modules no longer need to be repeated in
TypeScripts. Inventory comes from compiled ue_enum/ue_struct annotations and their
source locations, not textual require parsing. Repeated dependencies across class
roots and explicit selections merge only if module, source and type inventory agree.
Conflicts fail before output. Imports are sorted independently of traversal order.

Discovery retains the existing local Scripts scope and requires module filenames
to match their declared DAS module names. Modules mixing authored values/interfaces with
ue_class are refused for consolidated imports; use dedicated value
modules or choose ExplicitTypeImports for a manual boundary. Referenced class source
files compiled separately by ue_object_ref are not automatically cataloged as class
roots; explicitly select the classes whose wrappers should be generated.

Additional pending checks: direct and transitive value imports; shared dependencies
across multiple class sources; manual/discovered merge; conflicting module/file/type
inventory; private/public import visibility; generated output determinism; invalid
source locations; mixed declaration modules; unchanged outputs after refusal.
No build, commandlet generation, tests or packaged execution were run for this stage.

## Interface declaration and binding imports (validation pending)

Version 3 adds `interface` and `interface_ref` inventory kinds. Class-root automatic
discovery sees both in the compiler library. `-InterfaceScripts=ue_contract.das`
can explicitly select a declaration or existing generated binding module, and also
collects its enum/struct dependencies. This supports interface-only generation.

Declaration modules may contain enum/struct/interface definitions together, but
must not contain ue_class. ReadReflectedTypes/TypeScripts keeps its dedicated
enum/struct contract. Existing ue_interface_ref annotations still require the
loaded reflected interface asset and exact stable ID; the catalog does not bypass
these checks or invent asset paths.

An interface declaration and its reference views can share the same logical ID.
Duplicate interface definitions are refused, as are collisions with enum/struct/class
IDs. Multiple nominal interface views are permitted if their exported names differ.

If a module contains only interface declarations and every declared ID has a selected
ready-made reference view, the aggregate imports the binding modules and omits that
declaration-only module. This avoids exporting same-named declaration and runtime
wrapper types together. The complete source inventory stays in the manifest. Other
duplicate public type names are refused before output; use distinct source names or
separate aggregate modules. Mixed declaration/value modules cannot be partially
imported and will fail on such collisions.

The catalog **does not generate interface method adapters**. Generate those through
the existing reflection/GenerateBindings workflow, then select their module with
InterfaceScripts or import it from a selected class source. Importing only a
declaration makes the definition visible; it does not create a callable UE interface
factory or method wrapper.

Example `ue_catalog_contract.das` is a declaration-only module depending on MapEntry.
The example manifest and aggregate now include it. Asset import, binding generation,
and example compilation have not been run. No build/tests were performed.

Pending checks: v1/v2 compatibility; interface-only and mixed value/interface modules;
declaration/reference ID pairing; aliases; duplicate definitions and public names;
preferred binding exports; absent/stale interface assets; method factory/calls through
real generated bindings; transitive dependencies; deterministic output and packaged
loading. Broader source-bootstrap and interface container extensions remain separate.
