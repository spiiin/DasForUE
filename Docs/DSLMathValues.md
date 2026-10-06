# Native math values в DSL methods

Общий call frame поддерживает FVector, FRotator и FTransform по значению.

| daScript | Unreal struct | Frame kind |
|---|---|---|
| UEVector | /Script/CoreUObject.Vector (FVector) | 9 |
| UERotator | /Script/CoreUObject.Rotator (FRotator) | 10 |
| UETransform | /Script/CoreUObject.Transform (FTransform) | 11 |

Это distinct native handled types. Подмена одной структуры другой отклоняется
по AST и reflection identity, даже если размеры/число элементов совпадают.
В UE 5.8 эти vector/rotator values сохраняют double precision; сужения в float нет.

~~~das
options gen2
require ue_dsl
[ue_class(module_id="math",Blueprintable)]
class MathBridge {
    [ue_function(BlueprintCallable)]
    def Shift(value:UEVector; delta:double):UEVector {
        return ue_vector(value.x + delta, value.y, value.z)
    }
    [ue_function(BlueprintCallable)]
    def Apply(value:UETransform; position:UEVector):UEVector {
        return ue_transform_position(value,position)
    }
    [ue_function(BlueprintCallable)]
    def Build(translation:UEVector; rotation:UERotator; scale:UEVector):UETransform {
        return ue_transform(translation,rotation,scale)
    }
}
~~~

IDs остаются optional. Полный пример: dsl_math_methods.das.

~~~text
Das.DSL.Apply dsl_math_methods.das /Game/DasExamples/BP_Math math.MathBridge
~~~

## Helpers и ownership

- ue_vector(x:double;y:double;z:double) создаёт UEVector; fields x/y/z доступны.
- ue_rotator(pitch:double;yaw:double;roll:double) создаёт UERotator; соответствующие
  fields доступны. Углы — в градусах, как у Unreal FRotator.
- ue_transform(translation:UEVector;rotation:UERotator;scale:UEVector) создаёт
  transform через native Unreal constructor. Rotation хранится как quaternion.
- ue_transform_identity(), ue_transform_translation(), ue_transform_scale(),
  ue_transform_rotation(), ue_transform_position() используют Unreal math API.
- Инициализируйте local/global values явно helpers, особенно UETransform через
  identity/helper. Bare default native handled local не является обещанием
  identity transform. Arithmetic operators для этих types пока не добавлены.

Inputs копируются в отдельные aligned native slots до eval, outputs возвращаются
через CMRES storage и копируются в host-owned result. Адреса временных slots не
сохраняются. VM global, которому присвоен transform, получает независимую копию;
изменение host buffer и уничтожение context не изменяют уже возвращённые values.
При valid reload произвольные math globals пока не мигрируются.

Frame/native helpers отклоняют non-finite values, transform inputs/results с
ненормализованным quaternion и переполнение transformed position. Это проверяется
до небезопасного native math использования; invalid values не возвращаются в BP.
Смешанные scalar/object/math arguments сохраняют общий предел 16 inputs.

## Проверено и ограничения

TestDSLMathPIE.ps1 проверяет exact native struct pins и ProcessEvent для всех трёх
types, mixed transform/vector method, reimport и GC. TestDSLMathAsset.ps1 проверяет
Save/Reopen и native FTransform ProcessEvent в новом процессе. Foundation tests
проверяют precision, components, полный transform (rotation/translation/scale),
native TransformPosition semantics, global copy, context release, wrong struct,
non-finite vector и invalid quaternion. Те же ABI checks идут в packaged runtime.

Это method parameters/results, не authoring fields. User-defined/general reflected
structs, struct defaults/fields, nested structs/containers, out/ref, native virtual
adapters и production AOT остаются дальнейшими capabilities. Существующие
ue_generated::S_Vector/... wrappers не заменены и являются отдельными типами.
Следующий этап — общий reflected struct adapter и затем struct/object fields
с defaults, сериализацией и migration policy.
