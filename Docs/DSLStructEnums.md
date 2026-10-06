# Enum-поля в UEStructValue

Общий reflected struct adapter поддерживает FEnumProperty и enum-backed
FByteProperty. Пример: DasNamedMode с EDasCallMode и FName Key, присутствующий
в binding export IR. Native enum values 0/7/42 сохраняются без перенумерации.

~~~das
[ue_function(BlueprintCallable,
             arg_value_struct="/Script/DasRuntime.DasNamedMode",
             return_struct="/Script/DasRuntime.DasNamedMode")]
def SetMode(value:UEStructValue; symbol:string):UEStructValue {
    return ue_struct_with_enum(value,"Mode",
                               "/Script/DasRuntime.EDasCallMode",symbol)
}
~~~

Чтение: `ue_struct_get_enum(value,fieldPath,enumTypePath):string`.
Обновление: `ue_struct_with_enum(value,fieldPath,enumTypePath,symbol):UEStructValue`.
With-helper создаёт независимую native копию, исходное значение не меняется.
Nested paths поддерживаются, например `Nested.Mode`.

## Правила

- Enum type path должен точно совпадать с enum поля; short type names и
  попытка подставить другую enum identity отклоняются.
- Принимаются registered short symbols (`Active`) и qualified symbols
  (`EDasCallMode::Active`), case-insensitive. Возвращается canonical short symbol.
- Числа, display labels и redirects вместо registered symbols не принимаются.
- Unknown symbols/values и symbols с suffix `_MAX` отклоняются. Правило
  одинаково в Editor/cooked runtime; editor-only Hidden metadata не используется
  как основание для проверки и само по себе не запрещает другое registered имя.
- Вся структура рекурсивно проверяется на invalid enum values при получении
  input snapshot и создании/копировании snapshot, а не только при enum getter.
  Поэтому повреждённый enum input не запускает body даже у метода, который
  непосредственно это поле не читает.
- Plain uint8 без UEnum, enum bitmask combinations без registered member,
  object/container fields и неизвестные leaf types остаются unsupported.

Полный пример: dsl_struct_enum_methods.das. Все IDs автоматически формируются.

~~~text
Das.DSL.Apply dsl_struct_enum_methods.das /Game/DasExamples/BP_EnumStruct enumvalues.EnumStructBridge
~~~

## Проверка и дальнейшая работа

Foundation tests: scoped enum/read/update, canonical/qualified names, immutable
input и сохранение FName, unknown/MAX/type mismatch, invalid snapshot до eval,
enum-backed byte и nested enum. Те же правила проверяются в packaged runtime.
TestDSLStructEnumPIE.ps1 проверяет настоящий ProcessEvent с typed struct pins,
reimport и GC. TestDSLStructEnumAsset.ps1 — сохранение и новый процесс.

Это enum leaves existing reflected structs. Authoring собственных UEnums,
enum properties нового Unreal-класса и direct typed enum method parameters
ещё не реализованы. Snapshot GC и ограничения live set остаются прежними.
Следующий этап — struct fields и literal defaults с отдельной migration policy.
