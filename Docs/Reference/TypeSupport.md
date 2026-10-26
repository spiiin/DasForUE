# Type Support

This page summarizes which value kinds can appear in which positions of authored DSL code (classes, structs, interfaces, delegates, events and timers) and in generated native API. It is a map; the linked pages have the exact rules. A loaded, registered concrete enum/struct/class identity is required wherever a type is named, and "Yes" never means unrestricted signatures.

<!-- BEGIN GENERATED TYPE SUPPORT -->
## Registry-derived contracts

Generated from the live registry export in [Bindings/TypeSupport.json](../../Bindings/TypeSupport.json). Update with `GenerateTypeSupport.py --manifest logs/type-support-live.json`; verify with `--check`.

**Scope:** representative property descriptors checked by `ValidateDasPropertyType`. `Value`, `Snapshot member`, and `Delegate parameter` mean the three **property ABI usages**, not acceptance by every authored DSL parser, signature validator or callback. Container columns validate the complete composed descriptor with this sample as the child (the other map slot is `int`). Native class, interface and weak-reference samples remain separate.

`Yes` applies to the shown descriptor only. `No` has the exact validator reason in the JSON. `Unreported` means an adapter has no representative sample; it is never rendered as supported.

| Representative descriptor | Value | Snapshot member | Delegate parameter | Array element | Map value | Map key | Set element |
| --- | --- | --- | --- | --- | --- | --- | --- |
| <code>array&lt;int&gt;</code> | Yes | Yes | Yes | No | No | No | No |
| <code>bool</code> | Yes | Yes | Yes | Yes | Yes | No | No |
| <code>byte</code> | Yes | Yes | Yes | Yes | Yes | Yes | Yes |
| <code>class:/Script/CoreUObject.Object</code> | Yes | Yes | Yes | No | No | No | No |
| <code>double</code> | Yes | Yes | Yes | Yes | Yes | No | No |
| <code>enum:/Script/Engine.ECollisionChannel</code> | Yes | Yes | Yes | Yes | Yes | Yes | Yes |
| <code>float</code> | Yes | Yes | Yes | Yes | Yes | No | No |
| <code>int</code> | Yes | Yes | Yes | Yes | Yes | Yes | Yes |
| <code>map&lt;int, int&gt;</code> | Yes | Yes | Yes | No | No | No | No |
| <code>name</code> | Yes | Yes | Yes | Yes | Yes | Yes | Yes |
| <code>object:/Script/CoreUObject.Object</code> | Yes | Yes | Yes | Yes | Yes | Yes | Yes |
| <code>weak&lt;object:/Script/CoreUObject.Object&gt;</code> | Yes | Yes | Yes | No | No | No | No |
| <code>object:/Script/CoreUObject.Interface</code> | Yes | Yes | Yes | Yes | Yes | No | No |
| <code>set&lt;int&gt;</code> | Yes | Yes | Yes | No | No | No | No |
| <code>string</code> | Yes | Yes | Yes | Yes | Yes | Yes | Yes |
| <code>struct:/Script/CoreUObject.Color</code> | Yes | Yes | Yes | Yes | Yes | No | No |
| <code>struct:/Script/CoreUObject.LinearColor</code> | Yes | Yes | Yes | Yes | Yes | No | No |
| <code>struct:/Script/CoreUObject.Quat</code> | Yes | Yes | Yes | Yes | Yes | No | No |
| <code>struct:/Script/CoreUObject.Rotator</code> | Yes | Yes | Yes | Yes | Yes | No | No |
| <code>struct:/Script/CoreUObject.Transform</code> | Yes | Yes | Yes | Yes | Yes | No | No |
| <code>struct:/Script/CoreUObject.Vector</code> | Yes | Yes | Yes | Yes | Yes | No | No |
| <code>struct:/Script/CoreUObject.Vector2D</code> | Yes | Yes | Yes | Yes | Yes | No | No |
| <code>struct:/Script/Engine.ActorInstanceHandle</code> | Yes | No | Yes | Yes | Yes | No | No |
| <code>struct:/Script/Engine.BodyInstanceAsyncPhysicsTickHandle</code> | Yes | Yes | Yes | Yes | Yes | No | No |
| <code>struct:/Script/Engine.ConstraintInstanceAccessor</code> | Yes | No | Yes | Yes | Yes | No | No |
| <code>struct:/Script/Engine.HitResult</code> | Yes | No | Yes | Yes | Yes | No | No |
| <code>struct:/Script/EnhancedInput.InputActionValue</code> | Yes | Yes | Yes | Yes | Yes | No | No |
| <code>struct:/Script/GameplayTags.GameplayTag</code> | Yes | Yes | Yes | Yes | Yes | No | No |
| <code>struct:/Script/GameplayTags.GameplayTagContainer</code> | Yes | Yes | Yes | Yes | Yes | No | No |
| <code>struct:/Script/SlateCore.AnalogInputEvent</code> | Yes | No | Yes | Yes | Yes | No | No |
| <code>struct:/Script/SlateCore.CharacterEvent</code> | Yes | No | Yes | Yes | Yes | No | No |
| <code>struct:/Script/SlateCore.FocusEvent</code> | Yes | No | Yes | Yes | Yes | No | No |
| <code>struct:/Script/SlateCore.Geometry</code> | Yes | No | Yes | Yes | Yes | No | No |
| <code>struct:/Script/SlateCore.InputEvent</code> | Yes | No | Yes | Yes | Yes | No | No |
| <code>struct:/Script/SlateCore.KeyEvent</code> | Yes | No | Yes | Yes | Yes | No | No |
| <code>struct:/Script/SlateCore.NavigationEvent</code> | Yes | No | Yes | Yes | Yes | No | No |
| <code>struct:/Script/SlateCore.PointerEvent</code> | Yes | No | Yes | Yes | Yes | No | No |
| <code>struct:/Script/UMG.EventReply</code> | Yes | No | Yes | Yes | Yes | No | No |
| <code>struct:/Script/UMG.PaintContext</code> | Yes | No | Yes | Yes | Yes | No | No |
| <code>struct:/Script/UMG.WidgetAnimationHandle</code> | Yes | No | Yes | Yes | Yes | No | No |
| <code>struct:/Script/DasRuntime.DasSoftClass_f6005a7380ac4daaf37e8e855ef43965</code> | Yes | Yes | Yes | Yes | Yes | No | No |
| <code>struct:/Script/DasRuntime.DasSoftObject_f6005a7380ac4daaf37e8e855ef43965</code> | Yes | Yes | Yes | Yes | Yes | No | No |
| <code>text</code> | Yes | Yes | Yes | Yes | Yes | No | No |

### Signature, callback and default type gates

These columns query `QueryDasTypeUse` at the boundaries listed below. They are not acceptance by every authored DSL parser or a guarantee for arbitrary literals/expressions. `Context` requires the concrete native property, owning function, direction and nesting depth; the adapter gate is applied before a call. `No` reasons are retained in JSON.

| Descriptor | <code>authored_in</code> | <code>authored_out</code> | <code>authored_ref</code> | <code>authored_return</code> | <code>interface_in</code> |
| --- | --- | --- | --- | --- | --- |
| <code>array&lt;int&gt;</code> | Yes | Yes | Yes | Yes | Yes |
| <code>bool</code> | Yes | Yes | Yes | Yes | Yes |
| <code>byte</code> | Yes | Yes | Yes | Yes | Yes |
| <code>class:/Script/CoreUObject.Object</code> | Yes | Yes | Yes | Yes | Yes |
| <code>double</code> | Yes | Yes | Yes | Yes | Yes |
| <code>enum:/Script/Engine.ECollisionChannel</code> | Yes | Yes | Yes | Yes | Yes |
| <code>float</code> | Yes | Yes | Yes | Yes | Yes |
| <code>int</code> | Yes | Yes | Yes | Yes | Yes |
| <code>map&lt;int, int&gt;</code> | Yes | Yes | Yes | Yes | Yes |
| <code>name</code> | Yes | Yes | Yes | Yes | Yes |
| <code>object:/Script/CoreUObject.Object</code> | Yes | Yes | Yes | Yes | Yes |
| <code>weak&lt;object:/Script/CoreUObject.Object&gt;</code> | Yes | Yes | Yes | Yes | Yes |
| <code>object:/Script/CoreUObject.Interface</code> | Yes | Yes | Yes | Yes | Yes |
| <code>set&lt;int&gt;</code> | Yes | Yes | Yes | Yes | Yes |
| <code>string</code> | Yes | Yes | Yes | Yes | Yes |
| <code>struct:/Script/CoreUObject.Color</code> | Yes | Yes | Yes | Yes | Yes |
| <code>struct:/Script/CoreUObject.LinearColor</code> | Yes | Yes | Yes | Yes | Yes |
| <code>struct:/Script/CoreUObject.Quat</code> | Yes | Yes | Yes | Yes | Yes |
| <code>struct:/Script/CoreUObject.Rotator</code> | Yes | Yes | Yes | Yes | Yes |
| <code>struct:/Script/CoreUObject.Transform</code> | Yes | Yes | Yes | Yes | Yes |
| <code>struct:/Script/CoreUObject.Vector</code> | Yes | Yes | Yes | Yes | Yes |
| <code>struct:/Script/CoreUObject.Vector2D</code> | Yes | Yes | Yes | Yes | Yes |
| <code>struct:/Script/Engine.ActorInstanceHandle</code> | No | No | No | No | No |
| <code>struct:/Script/Engine.BodyInstanceAsyncPhysicsTickHandle</code> | Yes | Yes | Yes | Yes | Yes |
| <code>struct:/Script/Engine.ConstraintInstanceAccessor</code> | No | No | No | No | No |
| <code>struct:/Script/Engine.HitResult</code> | No | No | No | No | No |
| <code>struct:/Script/EnhancedInput.InputActionValue</code> | Yes | Yes | Yes | Yes | Yes |
| <code>struct:/Script/GameplayTags.GameplayTag</code> | Yes | Yes | Yes | Yes | Yes |
| <code>struct:/Script/GameplayTags.GameplayTagContainer</code> | Yes | Yes | Yes | Yes | Yes |
| <code>struct:/Script/SlateCore.AnalogInputEvent</code> | No | No | No | No | No |
| <code>struct:/Script/SlateCore.CharacterEvent</code> | No | No | No | No | No |
| <code>struct:/Script/SlateCore.FocusEvent</code> | No | No | No | No | No |
| <code>struct:/Script/SlateCore.Geometry</code> | No | No | No | No | No |
| <code>struct:/Script/SlateCore.InputEvent</code> | No | No | No | No | No |
| <code>struct:/Script/SlateCore.KeyEvent</code> | No | No | No | No | No |
| <code>struct:/Script/SlateCore.NavigationEvent</code> | No | No | No | No | No |
| <code>struct:/Script/SlateCore.PointerEvent</code> | No | No | No | No | No |
| <code>struct:/Script/UMG.EventReply</code> | No | No | No | No | No |
| <code>struct:/Script/UMG.PaintContext</code> | No | No | No | No | No |
| <code>struct:/Script/UMG.WidgetAnimationHandle</code> | No | No | No | No | No |
| <code>struct:/Script/DasRuntime.DasSoftClass_f6005a7380ac4daaf37e8e855ef43965</code> | Yes | Yes | Yes | Yes | Yes |
| <code>struct:/Script/DasRuntime.DasSoftObject_f6005a7380ac4daaf37e8e855ef43965</code> | Yes | Yes | Yes | Yes | Yes |
| <code>text</code> | Yes | Yes | Yes | Yes | Yes |

| Descriptor | <code>interface_out</code> | <code>interface_ref</code> | <code>interface_return</code> | <code>delegate_in</code> | <code>delegate_out</code> |
| --- | --- | --- | --- | --- | --- |
| <code>array&lt;int&gt;</code> | Yes | Yes | Yes | Yes | Yes |
| <code>bool</code> | Yes | Yes | Yes | Yes | Yes |
| <code>byte</code> | Yes | Yes | Yes | Yes | Yes |
| <code>class:/Script/CoreUObject.Object</code> | Yes | Yes | Yes | Yes | Yes |
| <code>double</code> | Yes | Yes | Yes | Yes | Yes |
| <code>enum:/Script/Engine.ECollisionChannel</code> | Yes | Yes | Yes | Yes | Yes |
| <code>float</code> | Yes | Yes | Yes | Yes | Yes |
| <code>int</code> | Yes | Yes | Yes | Yes | Yes |
| <code>map&lt;int, int&gt;</code> | Yes | Yes | Yes | Yes | Yes |
| <code>name</code> | Yes | Yes | Yes | Yes | Yes |
| <code>object:/Script/CoreUObject.Object</code> | Yes | Yes | Yes | Yes | Yes |
| <code>weak&lt;object:/Script/CoreUObject.Object&gt;</code> | Yes | Yes | Yes | Yes | Yes |
| <code>object:/Script/CoreUObject.Interface</code> | Yes | Yes | Yes | Yes | Yes |
| <code>set&lt;int&gt;</code> | Yes | Yes | Yes | Yes | Yes |
| <code>string</code> | Yes | Yes | Yes | Yes | Yes |
| <code>struct:/Script/CoreUObject.Color</code> | Yes | Yes | Yes | Yes | Yes |
| <code>struct:/Script/CoreUObject.LinearColor</code> | Yes | Yes | Yes | Yes | Yes |
| <code>struct:/Script/CoreUObject.Quat</code> | Yes | Yes | Yes | Yes | Yes |
| <code>struct:/Script/CoreUObject.Rotator</code> | Yes | Yes | Yes | Yes | Yes |
| <code>struct:/Script/CoreUObject.Transform</code> | Yes | Yes | Yes | Yes | Yes |
| <code>struct:/Script/CoreUObject.Vector</code> | Yes | Yes | Yes | Yes | Yes |
| <code>struct:/Script/CoreUObject.Vector2D</code> | Yes | Yes | Yes | Yes | Yes |
| <code>struct:/Script/Engine.ActorInstanceHandle</code> | No | No | No | Yes | Yes |
| <code>struct:/Script/Engine.BodyInstanceAsyncPhysicsTickHandle</code> | Yes | Yes | Yes | Yes | Yes |
| <code>struct:/Script/Engine.ConstraintInstanceAccessor</code> | No | No | No | Yes | Yes |
| <code>struct:/Script/Engine.HitResult</code> | No | No | No | Yes | Yes |
| <code>struct:/Script/EnhancedInput.InputActionValue</code> | Yes | Yes | Yes | Yes | Yes |
| <code>struct:/Script/GameplayTags.GameplayTag</code> | Yes | Yes | Yes | Yes | Yes |
| <code>struct:/Script/GameplayTags.GameplayTagContainer</code> | Yes | Yes | Yes | Yes | Yes |
| <code>struct:/Script/SlateCore.AnalogInputEvent</code> | No | No | No | Yes | Yes |
| <code>struct:/Script/SlateCore.CharacterEvent</code> | No | No | No | Yes | Yes |
| <code>struct:/Script/SlateCore.FocusEvent</code> | No | No | No | Yes | Yes |
| <code>struct:/Script/SlateCore.Geometry</code> | No | No | No | Yes | Yes |
| <code>struct:/Script/SlateCore.InputEvent</code> | No | No | No | Yes | Yes |
| <code>struct:/Script/SlateCore.KeyEvent</code> | No | No | No | Yes | Yes |
| <code>struct:/Script/SlateCore.NavigationEvent</code> | No | No | No | Yes | Yes |
| <code>struct:/Script/SlateCore.PointerEvent</code> | No | No | No | Yes | Yes |
| <code>struct:/Script/UMG.EventReply</code> | No | No | No | Yes | Yes |
| <code>struct:/Script/UMG.PaintContext</code> | No | No | No | Yes | Yes |
| <code>struct:/Script/UMG.WidgetAnimationHandle</code> | No | No | No | Yes | Yes |
| <code>struct:/Script/DasRuntime.DasSoftClass_f6005a7380ac4daaf37e8e855ef43965</code> | Yes | Yes | Yes | Yes | Yes |
| <code>struct:/Script/DasRuntime.DasSoftObject_f6005a7380ac4daaf37e8e855ef43965</code> | Yes | Yes | Yes | Yes | Yes |
| <code>text</code> | Yes | Yes | Yes | Yes | Yes |

| Descriptor | <code>delegate_ref</code> | <code>delegate_return</code> | <code>multicast_in</code> | <code>multicast_ref</code> | <code>queued_event</code> |
| --- | --- | --- | --- | --- | --- |
| <code>array&lt;int&gt;</code> | Yes | Yes | Yes | Yes | Yes |
| <code>bool</code> | Yes | Yes | Yes | Yes | Yes |
| <code>byte</code> | Yes | Yes | Yes | Yes | Yes |
| <code>class:/Script/CoreUObject.Object</code> | Yes | Yes | Yes | Yes | Yes |
| <code>double</code> | Yes | Yes | Yes | Yes | Yes |
| <code>enum:/Script/Engine.ECollisionChannel</code> | Yes | Yes | Yes | Yes | Yes |
| <code>float</code> | Yes | Yes | Yes | Yes | Yes |
| <code>int</code> | Yes | Yes | Yes | Yes | Yes |
| <code>map&lt;int, int&gt;</code> | Yes | Yes | Yes | Yes | Yes |
| <code>name</code> | Yes | Yes | Yes | Yes | Yes |
| <code>object:/Script/CoreUObject.Object</code> | Yes | Yes | Yes | Yes | Yes |
| <code>weak&lt;object:/Script/CoreUObject.Object&gt;</code> | Yes | Yes | Yes | Yes | Yes |
| <code>object:/Script/CoreUObject.Interface</code> | Yes | Yes | Yes | Yes | Yes |
| <code>set&lt;int&gt;</code> | Yes | Yes | Yes | Yes | Yes |
| <code>string</code> | Yes | Yes | Yes | Yes | Yes |
| <code>struct:/Script/CoreUObject.Color</code> | Yes | Yes | Yes | Yes | Yes |
| <code>struct:/Script/CoreUObject.LinearColor</code> | Yes | Yes | Yes | Yes | Yes |
| <code>struct:/Script/CoreUObject.Quat</code> | Yes | Yes | Yes | Yes | Yes |
| <code>struct:/Script/CoreUObject.Rotator</code> | Yes | Yes | Yes | Yes | Yes |
| <code>struct:/Script/CoreUObject.Transform</code> | Yes | Yes | Yes | Yes | Yes |
| <code>struct:/Script/CoreUObject.Vector</code> | Yes | Yes | Yes | Yes | Yes |
| <code>struct:/Script/CoreUObject.Vector2D</code> | Yes | Yes | Yes | Yes | Yes |
| <code>struct:/Script/Engine.ActorInstanceHandle</code> | Yes | Yes | Yes | Yes | No |
| <code>struct:/Script/Engine.BodyInstanceAsyncPhysicsTickHandle</code> | Yes | Yes | Yes | Yes | Yes |
| <code>struct:/Script/Engine.ConstraintInstanceAccessor</code> | Yes | Yes | Yes | Yes | No |
| <code>struct:/Script/Engine.HitResult</code> | Yes | Yes | Yes | Yes | No |
| <code>struct:/Script/EnhancedInput.InputActionValue</code> | Yes | Yes | Yes | Yes | Yes |
| <code>struct:/Script/GameplayTags.GameplayTag</code> | Yes | Yes | Yes | Yes | Yes |
| <code>struct:/Script/GameplayTags.GameplayTagContainer</code> | Yes | Yes | Yes | Yes | Yes |
| <code>struct:/Script/SlateCore.AnalogInputEvent</code> | Yes | Yes | Yes | Yes | No |
| <code>struct:/Script/SlateCore.CharacterEvent</code> | Yes | Yes | Yes | Yes | No |
| <code>struct:/Script/SlateCore.FocusEvent</code> | Yes | Yes | Yes | Yes | No |
| <code>struct:/Script/SlateCore.Geometry</code> | Yes | Yes | Yes | Yes | No |
| <code>struct:/Script/SlateCore.InputEvent</code> | Yes | Yes | Yes | Yes | No |
| <code>struct:/Script/SlateCore.KeyEvent</code> | Yes | Yes | Yes | Yes | No |
| <code>struct:/Script/SlateCore.NavigationEvent</code> | Yes | Yes | Yes | Yes | No |
| <code>struct:/Script/SlateCore.PointerEvent</code> | Yes | Yes | Yes | Yes | No |
| <code>struct:/Script/UMG.EventReply</code> | Yes | Yes | Yes | Yes | No |
| <code>struct:/Script/UMG.PaintContext</code> | Yes | Yes | Yes | Yes | No |
| <code>struct:/Script/UMG.WidgetAnimationHandle</code> | Yes | Yes | Yes | Yes | No |
| <code>struct:/Script/DasRuntime.DasSoftClass_f6005a7380ac4daaf37e8e855ef43965</code> | Yes | Yes | Yes | Yes | Yes |
| <code>struct:/Script/DasRuntime.DasSoftObject_f6005a7380ac4daaf37e8e855ef43965</code> | Yes | Yes | Yes | Yes | Yes |
| <code>text</code> | Yes | Yes | Yes | Yes | Yes |

| Descriptor | <code>timer</code> | <code>property_default</code> | <code>struct_member_default</code> | <code>parameter_default</code> | <code>computed_default_leaf</code> |
| --- | --- | --- | --- | --- | --- |
| <code>array&lt;int&gt;</code> | Yes | Yes | Yes | No | No |
| <code>bool</code> | Yes | Yes | Yes | Yes | Yes |
| <code>byte</code> | Yes | Yes | Yes | Yes | No |
| <code>class:/Script/CoreUObject.Object</code> | Yes | Yes | No | No | No |
| <code>double</code> | Yes | Yes | Yes | Yes | Yes |
| <code>enum:/Script/Engine.ECollisionChannel</code> | Yes | Yes | Yes | No | No |
| <code>float</code> | Yes | Yes | Yes | Yes | Yes |
| <code>int</code> | Yes | Yes | Yes | Yes | Yes |
| <code>map&lt;int, int&gt;</code> | Yes | Yes | Yes | No | No |
| <code>name</code> | Yes | Yes | Yes | No | No |
| <code>object:/Script/CoreUObject.Object</code> | Yes | Yes | Yes | No | No |
| <code>weak&lt;object:/Script/CoreUObject.Object&gt;</code> | Yes | Yes | Yes | No | No |
| <code>object:/Script/CoreUObject.Interface</code> | Yes | Yes | Yes | No | No |
| <code>set&lt;int&gt;</code> | Yes | Yes | Yes | No | No |
| <code>string</code> | Yes | Yes | Yes | Yes | Yes |
| <code>struct:/Script/CoreUObject.Color</code> | Yes | Yes | Yes | No | No |
| <code>struct:/Script/CoreUObject.LinearColor</code> | Yes | Yes | Yes | No | No |
| <code>struct:/Script/CoreUObject.Quat</code> | Yes | Yes | Yes | No | No |
| <code>struct:/Script/CoreUObject.Rotator</code> | Yes | Yes | Yes | No | No |
| <code>struct:/Script/CoreUObject.Transform</code> | Yes | Yes | Yes | No | No |
| <code>struct:/Script/CoreUObject.Vector</code> | Yes | Yes | Yes | No | No |
| <code>struct:/Script/CoreUObject.Vector2D</code> | Yes | Yes | Yes | No | No |
| <code>struct:/Script/Engine.ActorInstanceHandle</code> | No | No | No | No | No |
| <code>struct:/Script/Engine.BodyInstanceAsyncPhysicsTickHandle</code> | Yes | Yes | Yes | No | No |
| <code>struct:/Script/Engine.ConstraintInstanceAccessor</code> | No | Yes | Yes | No | No |
| <code>struct:/Script/Engine.HitResult</code> | No | Yes | Yes | No | No |
| <code>struct:/Script/EnhancedInput.InputActionValue</code> | Yes | Yes | Yes | No | No |
| <code>struct:/Script/GameplayTags.GameplayTag</code> | Yes | Yes | Yes | No | No |
| <code>struct:/Script/GameplayTags.GameplayTagContainer</code> | Yes | Yes | Yes | No | No |
| <code>struct:/Script/SlateCore.AnalogInputEvent</code> | No | No | No | No | No |
| <code>struct:/Script/SlateCore.CharacterEvent</code> | No | No | No | No | No |
| <code>struct:/Script/SlateCore.FocusEvent</code> | No | No | No | No | No |
| <code>struct:/Script/SlateCore.Geometry</code> | No | No | No | No | No |
| <code>struct:/Script/SlateCore.InputEvent</code> | No | No | No | No | No |
| <code>struct:/Script/SlateCore.KeyEvent</code> | No | No | No | No | No |
| <code>struct:/Script/SlateCore.NavigationEvent</code> | No | No | No | No | No |
| <code>struct:/Script/SlateCore.PointerEvent</code> | No | No | No | No | No |
| <code>struct:/Script/UMG.EventReply</code> | No | No | No | No | No |
| <code>struct:/Script/UMG.PaintContext</code> | No | No | No | No | No |
| <code>struct:/Script/UMG.WidgetAnimationHandle</code> | No | No | No | No | No |
| <code>struct:/Script/DasRuntime.DasSoftClass_f6005a7380ac4daaf37e8e855ef43965</code> | Yes | Yes | Yes | No | No |
| <code>struct:/Script/DasRuntime.DasSoftObject_f6005a7380ac4daaf37e8e855ef43965</code> | Yes | Yes | Yes | No | No |
| <code>text</code> | Yes | Yes | Yes | Yes | No |

| Descriptor | <code>synchronous_callback</code> | <code>native_call</code> |
| --- | --- | --- |
| <code>array&lt;int&gt;</code> | No | No |
| <code>bool</code> | No | No |
| <code>byte</code> | No | No |
| <code>class:/Script/CoreUObject.Object</code> | No | No |
| <code>double</code> | No | No |
| <code>enum:/Script/Engine.ECollisionChannel</code> | No | No |
| <code>float</code> | No | No |
| <code>int</code> | No | No |
| <code>map&lt;int, int&gt;</code> | No | No |
| <code>name</code> | No | No |
| <code>object:/Script/CoreUObject.Object</code> | No | No |
| <code>weak&lt;object:/Script/CoreUObject.Object&gt;</code> | No | No |
| <code>object:/Script/CoreUObject.Interface</code> | No | No |
| <code>set&lt;int&gt;</code> | No | No |
| <code>string</code> | No | No |
| <code>struct:/Script/CoreUObject.Color</code> | No | No |
| <code>struct:/Script/CoreUObject.LinearColor</code> | No | No |
| <code>struct:/Script/CoreUObject.Quat</code> | No | No |
| <code>struct:/Script/CoreUObject.Rotator</code> | No | No |
| <code>struct:/Script/CoreUObject.Transform</code> | No | No |
| <code>struct:/Script/CoreUObject.Vector</code> | No | No |
| <code>struct:/Script/CoreUObject.Vector2D</code> | No | No |
| <code>struct:/Script/Engine.ActorInstanceHandle</code> | No | No |
| <code>struct:/Script/Engine.BodyInstanceAsyncPhysicsTickHandle</code> | No | No |
| <code>struct:/Script/Engine.ConstraintInstanceAccessor</code> | No | Context |
| <code>struct:/Script/Engine.HitResult</code> | No | Context |
| <code>struct:/Script/EnhancedInput.InputActionValue</code> | Context | Context |
| <code>struct:/Script/GameplayTags.GameplayTag</code> | No | No |
| <code>struct:/Script/GameplayTags.GameplayTagContainer</code> | No | No |
| <code>struct:/Script/SlateCore.AnalogInputEvent</code> | Context | Context |
| <code>struct:/Script/SlateCore.CharacterEvent</code> | Context | Context |
| <code>struct:/Script/SlateCore.FocusEvent</code> | Context | Context |
| <code>struct:/Script/SlateCore.Geometry</code> | Context | Context |
| <code>struct:/Script/SlateCore.InputEvent</code> | Context | Context |
| <code>struct:/Script/SlateCore.KeyEvent</code> | Context | Context |
| <code>struct:/Script/SlateCore.NavigationEvent</code> | Context | Context |
| <code>struct:/Script/SlateCore.PointerEvent</code> | Context | Context |
| <code>struct:/Script/UMG.EventReply</code> | Context | Context |
| <code>struct:/Script/UMG.PaintContext</code> | Context | Context |
| <code>struct:/Script/UMG.WidgetAnimationHandle</code> | No | Context |
| <code>struct:/Script/DasRuntime.DasSoftClass_f6005a7380ac4daaf37e8e855ef43965</code> | No | No |
| <code>struct:/Script/DasRuntime.DasSoftObject_f6005a7380ac4daaf37e8e855ef43965</code> | No | No |
| <code>text</code> | No | No |

| Context | Actual validation boundary |
| --- | --- |
| <code>authored_in</code> | authored method lowering ABI |
| <code>authored_out</code> | authored method lowering ABI |
| <code>authored_ref</code> | authored method lowering ABI |
| <code>authored_return</code> | authored method lowering ABI |
| <code>interface_in</code> | interface method lowering ABI; nominal signature matching remains required |
| <code>interface_out</code> | interface method lowering ABI; nominal signature matching remains required |
| <code>interface_ref</code> | interface method lowering ABI; nominal signature matching remains required |
| <code>interface_return</code> | interface method lowering ABI; nominal signature matching remains required |
| <code>delegate_in</code> | delegate reflected signature factory |
| <code>delegate_out</code> | delegate reflected signature factory |
| <code>delegate_ref</code> | delegate reflected signature factory |
| <code>delegate_return</code> | delegate reflected signature factory |
| <code>multicast_in</code> | multicast signature factory; no return or out-only parameters |
| <code>multicast_ref</code> | synchronous multicast signature; queued subscribers cannot mutate refs |
| <code>queued_event</code> | owned queued payload; native receiver/signature and delivery value checks remain required |
| <code>timer</code> | timer payload; scheduling, direction and delivery value checks remain required |
| <code>property_default</code> | explicit property initializer type gate; literal/path validation follows |
| <code>struct_member_default</code> | struct member type gate; implicit/explicit and symbol-table checks follow |
| <code>parameter_default</code> | scalar input literal; ordering/world-context and literal validation follow |
| <code>computed_default_leaf</code> | scalar evaluator leaf; constructors/containers compose leaves; expression audit follows |
| <code>synchronous_callback</code> | specialized synchronous callback role; concrete frame and lifetime scope required |
| <code>native_call</code> | registered native transport; exact FProperty/function/direction/depth required |

### Registered script facets and default policies

These are adapter/facet declarations, not an independent support table. `—` means no dedicated hook is declared; generic lowering may still apply. View flags describe specialized view uses, not complete field/signature support. `explicit`/`implicit_empty` is the adapter default policy, not a claim that a particular property, parameter or computed-default expression is accepted.

| Adapter | View module | Specialized view uses | Frame / method bridges | Default policy | Default kind |
| --- | --- | --- | --- | --- | --- |
| <code>array</code> | — | — | — / — | <code>explicit</code> | — |
| <code>bool</code> | — | — | — / — | <code>explicit</code> | — |
| <code>byte</code> | — | — | — / — | <code>explicit</code> | — |
| <code>class</code> | <code>ue_native_gameplay</code> | native_property, self_property, authored_reference | — / — | <code>explicit</code> | — |
| <code>double</code> | — | — | — / — | <code>explicit</code> | — |
| <code>enum</code> | <code>ue_native_gameplay</code> | native_property | — / — | <code>explicit</code> | — |
| <code>float</code> | — | — | — / — | <code>explicit</code> | — |
| <code>int</code> | — | — | — / — | <code>explicit</code> | — |
| <code>map</code> | — | — | — / — | <code>explicit</code> | — |
| <code>name</code> | <code>ue_foundation</code> | native_property, self_property, authored_reference | — / — | <code>explicit</code> | — |
| <code>object</code> | <code>ue_native_gameplay</code> | native_property, self_property, authored_reference | — / — | <code>explicit</code> | — |
| <code>set</code> | — | — | — / — | <code>explicit</code> | — |
| <code>string</code> | — | — | — / — | <code>explicit</code> | — |
| <code>struct:/Script/CoreUObject.Color</code> | <code>ue_foundation</code> | native_property, self_property, authored_reference | — / — | <code>explicit</code> | <code>color</code> |
| <code>struct:/Script/CoreUObject.LinearColor</code> | <code>ue_foundation</code> | native_property, self_property, authored_reference | — / — | <code>explicit</code> | <code>linear_color</code> |
| <code>struct:/Script/CoreUObject.Quat</code> | <code>ue_foundation</code> | native_property, self_property, authored_reference | — / — | <code>explicit</code> | <code>quat</code> |
| <code>struct:/Script/CoreUObject.Rotator</code> | <code>ue_foundation</code> | native_property, self_property, authored_reference | <code>rotator</code> / <code>rotator</code> | <code>explicit</code> | <code>rotator</code> |
| <code>struct:/Script/CoreUObject.Transform</code> | <code>ue_foundation</code> | native_property, self_property, authored_reference | <code>transform</code> / <code>transform</code> | <code>explicit</code> | <code>transform</code> |
| <code>struct:/Script/CoreUObject.Vector</code> | <code>ue_foundation</code> | native_property, self_property, authored_reference | <code>vector</code> / <code>vector</code> | <code>explicit</code> | <code>vector</code> |
| <code>struct:/Script/CoreUObject.Vector2D</code> | <code>ue_foundation</code> | native_property, self_property, authored_reference | — / — | <code>explicit</code> | <code>vector2d</code> |
| <code>struct:/Script/Engine.ActorInstanceHandle</code> | — | — | — / — | <code>explicit</code> | — |
| <code>struct:/Script/Engine.BodyInstanceAsyncPhysicsTickHandle</code> | — | — | — / — | <code>explicit</code> | — |
| <code>struct:/Script/Engine.ConstraintInstanceAccessor</code> | — | — | — / — | <code>explicit</code> | — |
| <code>struct:/Script/Engine.HitResult</code> | — | — | — / — | <code>explicit</code> | — |
| <code>struct:/Script/EnhancedInput.InputActionValue</code> | <code>ue_foundation</code> | — | — / <code>input_value</code> | <code>explicit</code> | — |
| <code>struct:/Script/GameplayTags.GameplayTag</code> | <code>ue_gameplay_tags</code> | native_property | — / — | <code>implicit_empty</code> | — |
| <code>struct:/Script/GameplayTags.GameplayTagContainer</code> | <code>ue_gameplay_tags</code> | native_property | — / — | <code>implicit_empty</code> | — |
| <code>struct:/Script/SlateCore.AnalogInputEvent</code> | — | — | — / — | <code>explicit</code> | — |
| <code>struct:/Script/SlateCore.CharacterEvent</code> | — | — | — / — | <code>explicit</code> | — |
| <code>struct:/Script/SlateCore.FocusEvent</code> | — | — | — / — | <code>explicit</code> | — |
| <code>struct:/Script/SlateCore.Geometry</code> | — | — | — / — | <code>explicit</code> | — |
| <code>struct:/Script/SlateCore.InputEvent</code> | — | — | — / — | <code>explicit</code> | — |
| <code>struct:/Script/SlateCore.KeyEvent</code> | — | — | — / — | <code>explicit</code> | — |
| <code>struct:/Script/SlateCore.NavigationEvent</code> | — | — | — / — | <code>explicit</code> | — |
| <code>struct:/Script/SlateCore.PointerEvent</code> | — | — | — / — | <code>explicit</code> | — |
| <code>struct:/Script/UMG.EventReply</code> | — | — | — / — | <code>explicit</code> | — |
| <code>struct:/Script/UMG.PaintContext</code> | — | — | — / — | <code>explicit</code> | — |
| <code>struct:/Script/UMG.WidgetAnimationHandle</code> | — | — | — / — | <code>explicit</code> | — |
| <code>struct:family</code> | <code>ue_native_gameplay</code> | native_property | — / — | <code>implicit_empty</code> | — |
| <code>text</code> | <code>ue_foundation</code> | native_property, self_property, authored_reference | — / — | <code>explicit</code> | — |

### Native transports and lifetime policies

Transport identifiers and native-call constraints come from the owning adapter. A property ABI `Yes` above does not bypass this call gate or grant a scoped handle reflected storage. Opaque handles keep their dedicated entry points; registration does not add generic DSL views.

| Adapter | Transport / handle | Lifetime | Native call constraint | Reflected storage constraint |
| --- | --- | --- | --- | --- |
| <code>struct:/Script/Engine.ActorInstanceHandle</code> | — / — | <code>rejected_internal_handle</code> | Actor instance handles require the collision result adapter | Actor instance handles require the collision result adapter |
| <code>struct:/Script/Engine.BodyInstanceAsyncPhysicsTickHandle</code> | — / — | <code>rejected_internal_handle</code> | async physics handles require a scoped frozen-game-thread physics callback adapter | Generic validation |
| <code>struct:/Script/Engine.ConstraintInstanceAccessor</code> | <code>constraint_component_v1</code> / <code>UEConstraintHandle</code> | <code>component_owned_capability</code> | constraint accessors require scalar component-owned getter/library contract | Generic validation |
| <code>struct:/Script/Engine.HitResult</code> | <code>hit_result_v1</code> / <code>UEHitResult</code> | <code>owned_collision_snapshot</code> | fixed hit result arrays need an adapter | Generic validation |
| <code>struct:/Script/EnhancedInput.InputActionValue</code> | <code>input_action_value_v1</code> / <code>InputActionValue</code> | <code>owned_input_value</code> | fixed InputActionValue arrays need an adapter | Generic validation |
| <code>struct:/Script/SlateCore.AnalogInputEvent</code> | <code>slate_input_event_snapshot_v1</code> / <code>UEAnalogInputEvent</code> | <code>detached_snapshot</code> | input events require exact scalar snapshot function allowlist; navigation/reply need live adapters | Input events require detached snapshot transport, not reflected storage/defaults |
| <code>struct:/Script/SlateCore.CharacterEvent</code> | <code>slate_input_event_snapshot_v1</code> / <code>UECharacterEvent</code> | <code>detached_snapshot</code> | input events require exact scalar snapshot function allowlist; navigation/reply need live adapters | Input events require detached snapshot transport, not reflected storage/defaults |
| <code>struct:/Script/SlateCore.FocusEvent</code> | <code>slate_input_event_snapshot_v1</code> / <code>UEFocusEvent</code> | <code>detached_snapshot</code> | input events require exact scalar snapshot function allowlist; navigation/reply need live adapters | Input events require detached snapshot transport, not reflected storage/defaults |
| <code>struct:/Script/SlateCore.Geometry</code> | <code>slate_geometry_snapshot_v1</code> / <code>UEGeometry</code> | <code>owned_snapshot</code> | Geometry requires exact scalar snapshot function allowlist | Geometry requires owned snapshot transport, not reflected storage/defaults |
| <code>struct:/Script/SlateCore.InputEvent</code> | <code>slate_input_event_snapshot_v1</code> / <code>UEInputEvent</code> | <code>detached_snapshot</code> | input events require exact scalar snapshot function allowlist; navigation/reply need live adapters | Input events require detached snapshot transport, not reflected storage/defaults |
| <code>struct:/Script/SlateCore.KeyEvent</code> | <code>slate_input_event_snapshot_v1</code> / <code>UEKeyEvent</code> | <code>detached_snapshot</code> | input events require exact scalar snapshot function allowlist; navigation/reply need live adapters | Input events require detached snapshot transport, not reflected storage/defaults |
| <code>struct:/Script/SlateCore.NavigationEvent</code> | <code>slate_input_event_snapshot_v1</code> / <code>UENavigationEvent</code> | <code>detached_snapshot</code> | input events require exact scalar snapshot function allowlist; navigation/reply need live adapters | Input events require detached snapshot transport, not reflected storage/defaults |
| <code>struct:/Script/SlateCore.PointerEvent</code> | <code>slate_input_event_snapshot_v1</code> / <code>UEPointerEvent</code> | <code>detached_snapshot</code> | input events require exact scalar snapshot function allowlist; navigation/reply need live adapters | Input events require detached snapshot transport, not reflected storage/defaults |
| <code>struct:/Script/UMG.EventReply</code> | <code>scoped_event_reply_v1</code> / <code>UEEventReply</code> | <code>synchronous_callback_scope</code> | EventReply requires exact scalar synchronous callback adapter | EventReply requires a synchronous callback, not reflected storage/defaults |
| <code>struct:/Script/UMG.PaintContext</code> | <code>scoped_paint_context_v1</code> / <code>UEPaintContext</code> | <code>synchronous_paint_scope</code> | PaintContext requires exact scalar synchronous OnPaint adapter | PaintContext requires synchronous OnPaint, not reflected storage/defaults |
| <code>struct:/Script/UMG.WidgetAnimationHandle</code> | <code>weak_widget_animation_v1</code> / <code>UEWidgetAnimationHandle</code> | <code>weak_widget_context_capability</code> | WidgetAnimation requires exact scalar producer result | WidgetAnimation requires a weak context capability, not reflected storage/defaults |

Coverage: **40 registered adapters, 43 representative descriptors**.

**AOT:** per binding; not inferred from a type adapter. A scalar-compatible binding can be AOT eligible while an opaque overload of the same operation is closed. A type-level `Yes` would be misleading.

**Not inferred by this registry export:** complete DSL syntax, expression evaluation, literal validity and declaration graph resolution; source receiver/signature compatibility, payload budgets and delivery liveness; generated native API and unregistered engine transports.

<!-- END GENERATED TYPE SUPPORT -->

The authored and generated-API summaries below describe end-to-end public surfaces. The generated
tables above cover the shared validation gates; source syntax, declaration graphs, literal validity,
native receiver contracts and per-binding API/AOT checks can impose additional restrictions.

## Authored DSL

| Value kind | Field | Param in | Param out / ref | Return | Array element | Map key / set element | Map value | Event payload (`ue_event`, delegates) | Timer argument | Default value |
| --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- |
| `int`, `float`, `double`, `bool`, `string` | Yes | Yes | Yes | Yes | Yes | `int`, `string` (not float/bool) | Yes | Yes | Yes | Yes (property and parameter literal) |
| `uint8` (byte) | Yes | Yes | Yes | Yes | Yes | Yes | Yes | Yes | Yes | Yes (literal 0–255) |
| FName (`UENameValue`) | Yes | Yes | Yes | Yes | Yes | Yes | Yes | Yes | Yes | Property only |
| FText (`UETextValue`) | Yes | Yes | Yes | Yes | Yes | No | Yes | Yes | Yes | Yes (`ue_text`/`ue_text_invariant` literal) |
| `UEVector2D`, `UEVector`, `UERotator`, `UETransform` | Yes | Yes | Yes | Yes | Yes | No | Yes | Yes | Yes | Property only |
| Enum (native or authored, concrete) | Yes (byte storage) | Yes | Yes | Yes | Yes | Yes | Yes | Yes, exact type | Yes, exact type | Property only (named member) |
| Value struct (native or authored) | Yes, supported fields | Yes | Yes | Yes | Yes | No | Yes | Yes, registered layout | Yes, registered layout | Property only |
| Object / interface reference | Yes | Yes | Yes | Yes | Yes | Objects via `UEObjectKey`; not interfaces | Yes | Yes | Yes | Asset paths only (null or top-level asset) |
| Class reference | Yes | Yes | Yes | Yes | Limited (see notes) | No | No | No | No | Yes (`{"class":...}` constrained by MetaClass) |
| Soft object / soft class | Yes | Yes | Yes | Yes | Yes | No | Yes | No | No | Empty, or literal in standalone array/map properties |
| `array` / `table` (map, set) | Yes | Yes | Yes | Yes | No nesting | No nesting | No nesting | Supported carrier shapes | Authored class callbacks | Yes |
| Delegate / event | Authored delegate fields, multicast event properties | Single delegates | No | Single delegates | No | No | No | Subscription API, not a payload | No | No |
| Nested containers, fixed C++ arrays, raw pointers | No | No | No | No | No | No | No | No | No | No |

### Notes by position

- **Fields.** Become real reflected `UPROPERTY`s with normal Blueprint pins, serialization and cook rules. Writes through `self` validate type and `BlueprintReadOnly`. See [Structs](../Guide/DSL/Structs.md), [Containers](../Guide/DSL/Containers.md), [Self access](../Guide/DSL/SelfAccess.md).
- **Parameters and returns.** Up to 16 parameters per method or delegate frame. `var x:T&` is inout, `@ue_out var x:T&` is output-only; const references with direction markers are rejected. Output-only values start as the type's default (zero/empty, null reference, first valid enum member, struct default, zero vector/rotator, identity transform). See [Functions](../Guide/DSL/Functions.md).
- **Parameter defaults.** Only trailing input literals on authored methods: `int`, `float`, `double`, `bool`, `string`, plus byte (`uint8(n)`) and text (`ue_loctext(...)`, `ue_text_invariant(...)`) literals. Out/ref, `WorldContext`, complex types and computed expressions are rejected; authored interface stubs have no defaults. See [Defaults](../Guide/DSL/Defaults.md).
- **Property defaults.** Use one recursive materializer for scalars, names, text, enums, math, structs, assets, classes and containers (array/set/map literals, `ue_defaults()` blocks and audited computed scalar leaves). Persisted object references must be top-level assets or classes, not actors or subobjects. Nonempty soft-reference members inside authored structs are rejected. Shared limits: depth 16, 8192 typed nodes, 4096 container entries, 16 MiB, 1 MiB input JSON.
- **Hash keys / set elements.** Restricted to byte, int, string, FName, supported enums and object keys. Float, bool, FText, struct, math, class, soft and interface keys are rejected. FName keys follow Unreal equality (case-insensitive). Containers inside an authored `ue_struct` use a narrower key policy (int, string, `uint8`); Actor class fields support all key families.
- **Class references.** Supported for class fields, method parameters and results, and inside generated struct arrays of native API calls. Containers of class references, class references in the authored interface DSL, delegates, events and timers are not supported.
- **Delegates.** Single delegates allow return/ref/out in their own signature; multicast signatures return void and allow in/ref, not out-only. Delegate values cannot be nested in delegate signatures or passed by ref/out. See [Delegates and events](../Guide/DSL/DelegatesAndEvents.md).
- **Event payloads.** Native `ue_event` subscriptions accept by-value and native const-ref parameters; mutable native ref/out subscriptions are not supported. Native struct payloads must come from the canonical generated registry. See [Callback arguments](../Guide/Gameplay/CallbackArguments.md).
- **Timer arguments and queued callbacks.** Return void and take 0–16 by-value arguments, without ref/out or defaults. Scalar timer overloads also work with ordinary exported functions; automatic array packing is done for authored class callbacks. Invalid replacement is rejected before the existing timer is removed. See [Timers](../Guide/Gameplay/Timers.md).
- **Payload validation.** Events and timers share the same owning-copy rules: exact snapshot identity, finite numbers and math, text and array budgets, and object lifetime checked before delivery.

## Generated native API

| Value kind | [`ue_generated`](GeneratedBindings.md) | [Native class bindings](NativeClassBindings.md) | [API packages](../Api/README.md) | [Frame transport (developer)](../Dev/Runtime/LowLevelCalls.md) |
| --- | --- | --- | --- | --- |
| Scalars, FString | Yes | Yes | Yes | Yes |
| `uint8` | — | Yes (0..255 checked) | Yes | Yes (0..255 checked) |
| FName | `UEName` | `UENameValue` | `UEName` | Yes |
| FText | — | `UETextValue` | `UETextValue` | — |
| Enum | `E_*` enums | `[ue_native_enum]` types (0..254) | `E_*` enums | by symbol |
| Vector / Rotator / Transform | `S_*` structs | canonical math | canonical math | field paths |
| Other value structs | `S_*` structs | supported generated structs | `S_*` structs | field paths |
| Object reference | `C_*` views | typed refs, automatic upcasts | `C_*` views + typedefs | `UObjectHandle` |
| Class reference | — | Yes | Yes | Yes |
| Soft object / class | — | Yes | No generic ABI (dedicated adapters only) | — |
| Weak object fields | — | — | Yes (no strong pin) | — |
| `TArray` | Yes | Yes | Yes | Yes |
| `TMap` / `TSet` | `H_*` wrappers (scalar/enum) | `table` (scalar/name/enum/object keys) | Scalar/enum shapes | Yes |
| Default arguments | Typed, from `CPP_Default` | Trailing scalars and enums | None (explicit) | None |

Opaque engine values with dedicated transports in packages: `UEHitResult`, `UEConstraintHandle`, Geometry, input events, `UEEventReply`, PaintContext, WidgetAnimation handles and `InputActionValue`. See the [API package pages](../Api/README.md).

Not supported anywhere: nested containers, fixed C++ arrays, raw pointers, arbitrary C++ types without reflection, interface keys and weak-object containers.

See also: [Limitations](Limitations.md) · [Built-in types](../Guide/DSL/BuiltinTypes.md) · [Enums](../Guide/DSL/Enums.md) · [Soft references](../Guide/DSL/SoftReferences.md) · [Struct snapshots](StructSnapshots.md)
