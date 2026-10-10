# Vector2D and Enhanced Input Axis2D

Added 2026-10-10. **Not built or tested**. Source/ABI, UHT, property/call/frame,
ref/out, input/events/timers, PIE and packaged execution have not been verified.

```das
Position:UEVector2D=ue_vector2d(0.0lf,0.0lf)

[ue_function(BlueprintCallable)]
def OnMove(value:UEVector2D):void { self.Position = value }
```

UEVector2D is copied FVector2D storage with x/y double fields, constructed by
ue_vector2d(x,y). Native reflected type is /Script/CoreUObject.Vector2D; Blueprint
pins remain actual Vector2D. Literal/zero defaults, class/struct fields, existing
typed-reference accessors, input/result/ref/out and math array/map carriers use
the common checked struct snapshot transport. Existing ABI kind numbers remain
unchanged: Vector2D uses kind 12 with exact native struct identity.

Generated method wrappers pack/unpack the source math value at the transport
boundary. It is not padded to FVector or confused with Transform. Snapshot math
validation rejects NaN/Inf. Natural x/y member access is exposed by the annotation;
common nested mutation paths include the Vector2D x/y whitelist.

Enhanced Input Axis2D now produces a native Vector2D snapshot and requires an
annotated void callback(value:UEVector2D). The same typed InputActionRef API applies:
```das
ue_enhanced_bind(self.MoveAction,"Triggered","OnMove")
```
Assign an Axis2D InputAction and configure the project's component/Mapping Context
using the existing input setup. No new asset or player configuration was created.

Dynamic multicast Vector2D value/const-ref signals have dedicated native proxies,
carrying exact snapshots into copied source callback values. Timer and next-tick
captures add the same Vector2D shape with exact callback contract. The probe host
provides OnDasVector2DSignal/OnDasVector2DRefSignal plus emit methods for later checks.

Example: dsl_vector2d.das includes fields/containers, method echo, directed params,
timed assignment, Axis2D binding and a const-ref event callback. It has not been
imported or executed. Generic reflection-generator coverage/AOT export for native
Vector2D needs its own review; this stage extends the source DSL/runtime transport.

Pending checks: build/UHT; constructor/default/native name and precise double layout;
property/readonly/nested access and container converters; input/result/ref/out and
failed copyback; exact method/frame signatures; NaN/Inf rejection; dynamic proxy
compatibility; captured values; all Enhanced Input types including Axis2D; regression
of prior math types; owned lifetime/GC; actual PIE and packaged Development.
