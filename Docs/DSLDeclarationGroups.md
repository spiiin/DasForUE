# Declaration groups

The pinned daScript gen2 grammar accepts a trailing block on a call. DasForUE
consumes such calls in the field initializer of `ue_class` / `ue_struct` before
type inference. No reader macro, source preprocessing or parser patch is used.

```das
[ue_class(Blueprintable)]
class Example {
    properties = ue_properties(EditAnywhere, BlueprintReadWrite, Category="State") {
        var Score:int=0
        var Speed:double=100.0lf
    }
    // Title shown for this object.
    @ue_property=(EditAnywhere, BlueprintReadWrite)
    @Category="State"
    Title:string="Example"

    status = ue_properties(BlueprintReadOnly) {
        var Ready:bool=false
    }
    [ue_function(BlueprintCallable)]
    def GetScore():int { return self.Score }
}
```

`Score`, `Speed`, `Title` and `Ready` become ordinary fields. `properties` and
`status` are arbitrary group labels, not object properties. Access remains
`self.Score` or `actor.Score`; group labels do not enter stable IDs or runtime IR.
Several groups and individually annotated fields can coexist. Names must remain
unique across all declarations and group labels.

The flags have the same meaning and restrictions as individual `ue_property`
annotations. Category, DisplayName and ToolTip are literal string named arguments.
Fields needing their own tooltip or other metadata can use ordinary standalone
declarations, as Title does above. Per-field annotations are retained; per-field
string metadata overrides the group
value. Conflicting read/visibility flags are errors, not silent overrides.

Groups contain only explicitly typed mutable `var` declarations. Defaults are
validated by the ordinary property collector. Executable statements, inferred
types, references, empty blocks and nested groups are refused. Group initializers
are compile-time declaration syntax; their blocks never execute.

## Component fields and structs

```das
components = ue_components(VisibleAnywhere, BlueprintReadOnly, Category="Components") {
    var Behavior:ScoreComponent
}
```

`ue_components` additionally marks every field as `ue_component`; the existing
authored reference, ownership and SCS creation rules apply. It is supported only
in Actor classes. Component defaults must be omitted, as with individual component
fields. `ue_properties` also works inside a plain `ue_struct`.

Examples converted to groups:

- `dsl_actor_component_owner.das` / `dsl_component_behavior.das`.
- `dsl_gameplay_actor.das` / `dsl_gameplay_behavior.das`.

## Functions and other declarations

This does **not** support `functions = ue_functions(...) { def Query() ... }`:
the daScript parser rejects `def` inside an expression block before an AST macro
can run. Ordinary function/event/lifecycle annotations remain the supported form.
Enum/class/interface declarations cannot be placed in these expression blocks
either. Supporting those groups needs a separately valid syntax; they are not
silently treated as properties.

Tests compare grouped and individual authoring JSON after normalizing only the
source filename, including IDs, types, defaults and metadata. Foundation tests
also cover grouped struct collection and invalid groups. Actor/Component asset and
PIE tests exercise the converted main example.

Comments immediately above reflected declarations provide default `ToolTip` metadata; see [comment tooltips](DSLCommentTooltips.md). Explicit metadata takes priority.
