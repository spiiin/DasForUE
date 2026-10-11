options gen2
require ue_dsl

/**
 * Счётчик игрока.
 * Two documentation lines.
 */
[ue_class(module_id="doctips",Blueprintable)]
class DocCounter {
    properties=ue_properties(EditAnywhere,BlueprintReadWrite) {
        // Текущие очки.
        var Score:int=0
        /* Movement speed. */
        var Speed:double=100.0lf
    }
    // Название объекта.
    @ue_property=(EditAnywhere,BlueprintReadWrite)
    Title:string="http://example/ /* not a comment */"
    // This comment must lose to the explicit tooltip.
    @ue_property=(BlueprintReadOnly)
    @ToolTip="Explicit tooltip"
    Explicit:int=1
    // A detached comment.

    @ue_property=(BlueprintReadOnly)
    Detached:int=0
    @ue_property=(BlueprintReadOnly)
    Trailing:int=0 // Do not attach this to the next field.
    @ue_property=(BlueprintReadOnly)
    Next:int=0
    // Возвращает сумму.
    // Keeps line breaks.
    [ue_function(BlueprintCallable)]
    def Evaluate(input:int):int { return self.Score+input }
}

// Shared query contract.
[ue_interface(module_id="doctips",BlueprintType)]
class DocContract {
    // Reads the current score.
    [ue_function(BlueprintCallable)]
    def Query():int {}
}
