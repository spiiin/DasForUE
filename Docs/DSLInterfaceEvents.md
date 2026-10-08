# Interface event methods — 2026-10-09

Interfaces now support methods with a void result and no output/inout parameters.
These use real overriding UK2Node_Event nodes in an Actor Blueprint Event Graph.
They are not hidden return-bearing function graphs or direct calls to declaration
stubs. Event execution forwards owned input frames to the existing void DSL adapter.

```das
[ue_interface(module_id="game", BlueprintType)]
class ScoreEvents {
    [ue_function(BlueprintCallable, Category="Score")]
    def AddScore(amount:int):void {}

    [ue_function(BlueprintCallable)]
    def ClearScore():void {}

    [ue_function(BlueprintCallable)]
    def PushLabels(Values:array<string>; message:string):void {}
}
```

The implementing class uses the same signature with its actual body:

```das
[ue_class(module_id="game", Blueprintable),
 ue_implements(interface=ScoreEvents)]
class ScoreActor {
    @ue_property=(EditAnywhere, BlueprintReadWrite)
    Score : int = 0

    [ue_function(BlueprintCallable)]
    def AddScore(amount:int):void { self.Score += amount }

    [ue_function(BlueprintCallable)]
    def ClearScore():void { self.Score = 0 }
    // Implement the remaining declared methods as well.
}
```

Create the interface asset before applying the Actor class, as for existing
interface functions. Native UInterface BlueprintNativeEvent declarations with
void/no-output signatures use the same event backend. ProcessEvent and native
IDasEventInterface::Execute_* calls reach the Actor implementation. Input shape,
name/direction and native interface membership checks remain in force.

Methods with return/out/inout values continue using interface function graphs.
Primitive and the current supported container input types use the same pack/frame
transport as ordinary callable methods. Arrays/strings are copied into owned
values; no native pin/property pointer is cached across calls or reload.

Event nodes retain their stable member IDs. Reimport checks native function shape
and the owned event-to-adapter connection, updates source entry/file references
and preserves supported identity rules. Event removal/rename still requires a
structural policy and rejects; arbitrary custom graphs are not silently adopted.
Changing only the DSL body uses the existing runtime reload path without changing
the Blueprint class or restarting the editor.

This does not add arbitrary Blueprint override declarations, pure/static/net
methods, latent actions, event inheritance migration or VM reentry. A direct call
to the interface's declaration-only script stub still traps. Dispatch from an
already active daScript VM into a daScript-owned Actor is still deferred.

Examples: dsl_interface_definition.das and dsl_interface_implementation.das.
Validation: TestDSLTypeRegistry.ps1 -AuthoredInterface -Cooked and TestEditor.ps1.
