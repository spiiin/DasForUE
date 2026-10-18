# Типизированный InputActionValue

Модуль `ue_input_value` публично подключается из `ue_enhanced_input` и из
generated-модуля, если его сигнатуры используют native InputActionValue.

```das
require ue_input_value

let jump:InputActionValue = ue_input_value(true)
let throttle = ue_input_value(0.5)
let move = ue_input_value(ue_vector2d(1.0lf, 0.0lf))
let look = ue_input_value(ue_vector(0.0lf, 1.0lf, 0.0lf))

let pressed = ue_input_bool(jump)
let amount = ue_input_axis1d(throttle)
let direction = ue_input_axis2d(move)
let rotation = ue_input_axis3d(look)
let is2D = ue_input_type(move) == InputValueType.Axis2D
```

`ue_input_value` перегружен для bool, float, double, UEVector2D и UEVector.
`InputValueType`: Boolean, Axis1D, Axis2D, Axis3D. Axis1D getter возвращает double
после native float-конверсии, как EnhancedInputLibrary. Bool использует UE
IsNonZero с native tolerance; это не простое сравнение X с нулём. Чтение другого
числа осей разрешено, как UE Get<T>(): недостающие компоненты равны нулю.

Пример: `Plugins/DasForUE/Content/Das/Scripts/input_action_values.das`.

## Контракт

FInputActionValue не содержит reflected-полей. Адаптер хранит собственные
числовые компоненты и kind, а native-значение создаёт конструктором UE.
Скрипт не получает raw pointer, native layout или token в таблице context.
Копирование и хранение в DAS array сохраняют независимое значение.
Поля carrier не доступны скрипту; внешние helpers проверяют kind и конечность
компонентов. UE-конструктор обнуляет неиспользуемые оси.

Экспорт помечает точную native-идентичность
`/Script/EnhancedInput.InputActionValue` transport-ом `input_action_value_v1`.
Генератор использует тот же InputActionValue в аргументах и результатах.
Существующий invocation-local frame сохраняет проверку сигнатур, направления
и assignment; преобразование копирует данные через FInputActionValue API.
Fixed native arrays отклоняются. Нового call dispatcher нет.

## Границы

Value API и reflected-call transport дополнены типизированными Enhanced Input
callbacks, включая `[ue_function]` методы: см. [Input/UI callbacks](InputUICallbacks.md).
Прежние bool/float/vector обработчики сохранены. Авторское UPROPERTY, таймеры
и произвольные native virtual overrides не получают новую поддержку автоматически.

Проверка: `Scripts/TestUMGInputPackage.ps1` — Editor build, генерация и компиляция
целого UMG/Input модуля, один runtime main. Он проверяет четыре вида значений,
native Make/Break/ToString, обнуление лишних осей и owned copy в array.
Полный PIE/packaged, rendering и физический ввод не входят в эту проверку.
Результат: `logs/umg-input-validation.json`.
