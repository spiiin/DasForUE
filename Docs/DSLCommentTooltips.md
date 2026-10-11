# Comments as Unreal tooltips

Documentation comments immediately before reflected declarations become Unreal
`ToolTip` metadata. This uses the native `das::CommentReader` API behind daScript's
`[comment_reader]` mechanism, registered by `ue_foundation`. The parser and lexer
are unchanged. Comments come from the compiler's actual source buffer.

```das
// Счётчик игрока.
[ue_class(Blueprintable)]
class Counter {
    properties = ue_properties(EditAnywhere, BlueprintReadWrite) {
        // Текущие очки игрока.
        var Score:int=0
        // Скорость движения.
        var Speed:double=100.0lf
    }

    // Название объекта.
    @ue_property=(EditAnywhere, BlueprintReadWrite)
    Title:string="Example"

    // Возвращает сумму очков и входного значения.
    [ue_function(BlueprintCallable)]
    def Evaluate(input:int):int { return self.Score+input }
}
```

Supported targets: classes, interfaces and their annotated method stubs, reflected
methods, properties/components, enum types, struct types and struct fields.
Comments on individual enum entries are not collected as tooltips.

Use standalone `//`, `///`, `/* ... */` or `/** ... */` comments. Adjacent line
comments retain line breaks; block indentation and leading documentation `*`
markers are removed. UTF-8 text is preserved. Place comments above the declaration
and its annotations, or above each `var` inside a declaration group.

A blank line or intervening code breaks attachment. Trailing comments are ignored;
comment-like text inside strings is not documentation. A comment on a group label
is not a shared tooltip for all its members. Explicit `ToolTip` metadata, including
shared group metadata, takes priority over comments. Tooltips are limited to 4096
characters by the existing metadata contract.

Fixtures: `dsl_comment_tooltips.das` and `dsl_comment_types.das`. Foundation tests
check UTF-8, blocks, line groups, explicit overrides, detached/trailing comments,
interfaces and enum/struct metadata. The main Actor/Component example now documents
`Behavior` through a comment, with its resulting Unreal metadata checked by the
existing asset create/reopen test.

Validation (2026-10-11, UE 5.8.3): TestEditor.ps1 — editor build succeeded, DAS_FOUNDATION_RESULT=PASS failures=0; all eight comment-tooltip checks passed. TestActorComponent.ps1 — create and reopen passed, DAS_ACTOR_COMPONENT_PROBE=PASS cooked=False. Packaged execution was not repeated for this change.
