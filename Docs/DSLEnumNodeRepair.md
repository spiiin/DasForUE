# Standard enum-node repair

`Das.DSL.Types.ExtendEnums Script.das /Game/Registry.Registry graphs=repair`
now supports the standard Switch on Enum, Enum Literal, Select, Get Num Enum
Entries, safe Byte to Enum and For Each Element in Enum nodes for authored enums.
The existing single imported DSL Actor root / ordinary dependent Blueprint/map
scope, idle editor and stopped PIE requirements still apply.

Each supported node reloads its enum definition through Unreal's native
INodeDependingOnEnumInterface, reconstructs pins and compiles with its root and
ordinary dependent classes. The operation checks all existing links and defaults
after compilation. The same rollback restores old enum/member identities, node
definitions/pins, classes, Actor data and dirty flags after failure.

Sparse values are handled as values, not ordinal indexes:

- Literal Active returns 42 before/after append.
- Select retains Idle/Active option defaults 70/420 and adds a new option for Paused.
- Count is regenerated from 2 to 3. Unreal compiles it to a literal, so refreshing
  the enum UObject alone would not update old Blueprint bytecode.
- ForEach iterates indexes but calls GetEnumeratorValueFromIndex to produce 7/42/
  200. MAX is excluded; the test sums 49 before append and 249 afterwards.
- Safe Byte to Enum uses Unreal GetValidValue: valid values stay unchanged, an
  unknown value returns the reserved MAX (43 before / 201 after append). This is
  Unreal's native cast behavior, not a throwing daScript conversion. The binding
  boundary still rejects MAX/invalid values before writes. Unsafe cast nodes are
  outside the supported repair policy.

Unconnected new Select/Switch options retain Unreal's normal defaults. Migration
does not invent handlers or rewrite user logic. Member rename/removal/renumber and
multi-root coordinated repair are later policies.

Test: Scripts/TestDSLTypeRegistry.ps1 -EnumAppend -EnumGraphs -EnumNodes -Cooked.
The executable fixture graphs are built in Source/DasEditor/Private/
DasEnumNodesProbe.cpp. Accumulation uses a Blueprint function-local variable so
the fixture remains compatible with ordinary DSL reimport. Before/after append,
rollback, save/reopen and cooked invocation are verified. See
DSLEnumNodeRepairValidation.md.
