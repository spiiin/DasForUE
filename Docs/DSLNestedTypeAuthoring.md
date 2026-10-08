# Nested authored struct declarations

```das
[ue_struct(id="sample.Leaf", module_id="sample", BlueprintType)]
struct Leaf {
    @ue_property=(EditAnywhere, BlueprintReadWrite)
    Count : int = 10
}

[ue_struct(id="sample.Branch", module_id="sample", BlueprintType)]
struct Branch {
    @ue_property=(EditAnywhere, BlueprintReadWrite)
    Inner : Leaf = Leaf()
}
```

Use Branch directly as a ue_class property, argument or result, with type_registry
or type_folder as in the earlier direct-type workflow. An owned class property uses
`Config : Branch = Branch()`. Methods can read/write scalar leaves, including
`self.Config.Inner.Count += amount`. The native Actor is updated, rather than a
temporary unpacked copy. Local by-value structs can be modified normally and
returned through the recursive bridge.

Nested fields currently use empty child constructors. Defaults come from child
declarations. Explicit nested initializer overrides, containers, enum/object
members, inheritance, references and struct methods remain outside this milestone.
Scalar fields retain the earlier supported types/defaults/metadata. All reflected
declarations must belong to the same module and referenced types must be present.

The full model validates missing references and by-value cycles before native
creation; each tree is bounded to depth 16 and 1024 scalar leaves. Existing limits
of 256 reflected types and 64 fields per struct remain. Native runtime validation
also checks struct size/unsupported fields. The editor creates children before
parents and checks nested native pin identity/defaults on idempotent application.
Asset creation failures clean only assets created in that operation.

Nested member references in the project's private IR use synthetic type_path
`/DasTypes/<stable-ID-hash>.Type`. This is a stable declaration reference, not an
Unreal asset path. The factory resolves it through the module model and creates
the actual FStructProperty referring to the child asset. Runtime class/parameter
paths resolve through the type registry. A source type alias therefore does not
change the stored native type when its stable ID is preserved.

Per-annotation metadata may contain an unresolved reference until the compiler
collects the complete module; only the internal annotation reader/writer opts into
that mode. Public/default model validation requires a closed, valid type graph.
Transport JSON remains version 2, with the added nested struct field shape.

Generated pack/unpack helpers recursively walk the source types and native scalar
paths, such as Left.Inner.Count. They create owned by-value daScript results and
native snapshots; no reference into native struct memory escapes to daScript.
Deep self scalar assignments and compound assignments commit one leaf through the
existing dotted-path scalar setter. Readonly checks apply along the entire path.

Assignment of a whole nested member through self (for example self.Config.Inner =
other.Inner) currently rejects explicitly. Copy Config into a local value, edit it,
then assign self.Config = value. Whole self-property assignment and typed method
parameters/results use recursive pack/unpack. This avoids silently modifying an
unpacked temporary or supplying an incompatible nested snapshot.

Example: Plugins/DasForUE/Content/Das/Scripts/dsl_nested_authoring.das. The fixture's
REGISTRY placeholder is replaced by the isolated test runner; substitute your own
registry object path when using the example manually.

Test: Scripts/TestDSLTypeRegistry.ps1 -AuthoredNested -Cooked. It creates a three-type
Leaf/Branch/Tree hierarchy, verifies native order/defaults, deep self writes,
typed frames, local mutation/return, ProcessEvent, source aliases, safe rejection
of unsupported nested writes, save/reopen and actual packaged invocation/GC.

The existing migration service continues to handle scalar-leaf definition changes
stored inside wrapper trees. Adding/removing/retyping nested child members in an
outer definition needs a future graph/definition migration policy.

Next: container definitions and state migration, then broader nested member writes,
split/custom Blueprint node repair, multiple roots and active-PIE type reload.
