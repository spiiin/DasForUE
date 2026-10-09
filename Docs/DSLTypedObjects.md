# Typed UObject references in the authoring DSL

UEObject is a registered native daScript type macro. It validates a concrete
generated object wrapper and returns that existing nominal type. It does not
create a new pointer representation or assume native UObject binary layout.

## Declare reference aliases in a separate module

```das
options gen2
module game_object_types public
require ue_foundation public
require ue_generated public

typedef ActorRef = $UEObject<Actor>
typedef ObjectRef = $UEObject<Object>
```

Native short names such as Actor/Object are unambiguous typedef aliases exported
by GenerateBindings.py. Hash-qualified C_* wrapper names remain available and
unchanged. Ambiguous short names or collisions with existing generated symbols
are not exported. The selected reflection IR must include the native class.
Generated wrappers carry ue_native_object(path="/Script/...") metadata.

The bundled ue_object_types module already declares ActorRef and ObjectRef for
the current exported selection. It also verifies qualified macro syntax through
`$ue_foundation::UEObject<Object>`.

The separate module matters: the pinned parser treats unresolved type-macro
parameters as generic and forbids generic non-static class methods, before
function annotations can run. Importing a compiled alias module resolves the
macro first. Defining a macro alias immediately above the class in the same file
does not avoid that parser restriction. No reader, preprocessor or parser changes
are used. Use concrete imported aliases in all class method signatures.

## Author ordinary fields and methods

```das
options gen2
options gc
require ue_dsl
require ue_object_types

[ue_class(module_id="game", Blueprintable)]
class TargetHolder {
    @ue_property=(EditAnywhere, BlueprintReadWrite)
    Target : ActorRef

    [ue_function(BlueprintCallable)]
    def SetTarget(target:ActorRef):void { self.Target = target }

    [ue_function(BlueprintCallable)]
    def GetTarget():ActorRef { return self.Target }

    [ue_function(BlueprintCallable)]
    def Clear():void { self.Target = default<ActorRef> }
}
```

The frontend derives the native class constraint from the wrapper type for
properties, parameters and results. No arg_name_class or return_class annotation
is needed. A matching explicit constraint is permitted for compatibility; a
conflicting constraint rejects. Typed property defaults must be omitted and become
null native CDO defaults. Empty reference values can be constructed in method
bodies with default<ActorRef>.

ActorRef and ObjectRef are distinct nominal wrapper types. No implicit upcast or
downcast is introduced. Existing generated checked _from/_raw/_as_* conversions
remain available. The wrapper's internal handle is not proof that an object has
the declared class: generated bridge conversions and native frame/property
validation still check the actual object and reject forged, expired, foreign or
incompatible handles before publishing a typed result or field assignment.

Compiler-generated exported entry points retain the UObjectHandle ABI. Natural
typed implementations are private; native inputs are unpacked through generated
_from helpers and results/writes packed through _raw helpers.

## Ownership and limits

- A reflected field remains a strong FObjectProperty reference under Unreal GC.
  The wrapper/VM global holds only the existing weak, context-local handle token.
- The existing frame bridge pins live input objects while the call executes.
  Null is valid; a destroyed Actor or collected non-null handle rejects.
- This milestone covers loaded native /Script UObject classes exported in IR.
  Interface references, authored /Game class types, generic auto(T) arguments,
  typed object out/ref, fixed arrays and object containers are not added here.
- The original UObjectHandle + path annotations/initializers remain supported.
  Type macros are not used as arbitrary ordinary annotation arguments.
- This adds declaration/boundary typing, not an automatic high-level proxy for
  every native member; native member calls still use generated bindings.

Example: Plugins/DasForUE/Content/Das/Scripts/dsl_typed_objects.das.
Validation: Scripts/TestDSLTypeRegistry.ps1 -TypedObjects -Cooked,
Scripts/TestEditor.ps1, Scripts/TestBindingsGenerator.py and TestBindingsPolicy.py.


Reflected method bodies can also use `ue_cast_nullable_object(target.handle,
ue_type(Actor))` and `ue_object_is_a(handle, "Actor")` without spelling native
paths. Type aliases can identify the native class. The compiler resolves these
references; the checked weak-handle runtime ABI is unchanged. See DSLTypeCalls.md.
