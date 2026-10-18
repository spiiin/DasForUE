# Безопасные снимки Slate input events

Реализованы `UEInputEvent`, `UEKeyEvent`, `UEAnalogInputEvent`,
`UECharacterEvent`, `UEPointerEvent` и `UENavigationEvent`.
Подключение: `require ue_input_events`; generated UMG-пакет подключает его public.

Это opaque immutable handles к owned native-значениям в существующей ограниченной
таблице snapshots контекста. Native layout, указатели и поля токена в DAS не
экспортируются. Default handle невалиден; числовые constructors/casts и обращение
к Token запрещены. Тип хранится в debug/root metadata daScript, поэтому globals,
массивы и clone корректно участвуют в существующем сборщике snapshots.

## Захват и владение

Native `CaptureDasInputEvent(Type, Source, Out, Error)` принимает только шесть
точных UScriptStruct identities, проверяет thread/execution role/frozen physics,
finite values и предел 256 pressed keys, затем создаёт native-копию соответствующего
вида события. Производный bridge очищает **свой** protected EventPath; источник не
изменяется. В FInstancedStruct попадает обычный native base value с EventPath=null.
Нет offset hacks или копирования binary layout в DAS.

Нативные copy operations сохраняют modifiers, repeat, UserIndex, device ID,
timestamp, key/character codes, pointer positions и CursorDelta, gesture/touch
данные. PressedButtons — собственный TSet<FKey>, а не borrowed view. GetWindow,
FindGeometry и доступ к маршруту не открываются.

Ошибочный capture не заменяет Out. Входы вызовов проверяются повторно до
ProcessEvent. Результаты/projections публикуются как новые detached snapshots.
Copy/clone script handles разделяет immutable значение; lifetime определяется
живыми корнями контекста. Снимок не удерживает исходный widget, window или world.
ReleaseObject/reload освобождают таблицу старого контекста; handles не переносятся
между контекстами. Отдельная native-копия может быть безопасно захвачена заново.

## Транспорт и пользовательский код

Открыты ровно **33** функции из `Bindings/InputEvents.contract.json`: getters
KismetInputLibrary и пять value projections WidgetBlueprintLibrary. Generated
wrappers передают opaque event scalar через общий checked call frame. Native
exporter подтверждает `call_adapter=slate_input_event_snapshot_v1`; генератор
требует этот marker и точные transport identities, отвергает контейнерные формы.
На runtime дополнительно действует exact native function allowlist.

Публичные unwrap helpers принимают только уже проверенный UEStructValue:

```dascript
require ue_api_umg_input

[export]
def observe_key(raw:UEStructValue):int {
    let event=ue_key_event(raw)
    let key=ue_input_key_GetKey(event)
    let shift=ue_input_key_InputEvent_IsShiftDown(
        ue_ui_GetInputEventFromKeyEvent(event))
    return shift ? 1 : 0
}
```

Остальные helpers: ue_input_event, ue_analog_input_event, ue_character_event,
ue_pointer_event, ue_navigation_event. Они не создают события из чисел или строк.
Источником raw служит доверенный native capture/observer bridge. Общий scalar
verifier для произвольных structs не ослаблен: отдельный C++ метод
`FDasRuntimeModule::DispatchInputEventSnapshots` проверяет detached payloads и
вызывает observer с UEStructValue-параметрами и int-результатом. Он не отправляет
событие в Slate и не возвращает EventReply.

Эта итерация не добавляет UserWidget overrides, ue_function/ue_property event
types, defaults, сериализацию или автоматическую подписку на input. Для
синхронного UI routing всё ещё нужны отдельные Geometry/EventReply контракты.

## Закрытые операции и покрытие

Три Key_GetNavigationActionFromKey / Key_GetNavigationDirectionFromKey /
Key_GetNavigationDirectionFromAnalog остаются закрытыми: они зависят от текущего
Slate navigation config, analog меняет repeat state. DetectDragIfPressed также
не открывается этим codec — ему нужен EventReply/live event adapter.

UMG/Input: **366 ready (+33), 114 export blocked, 37 generation blocked**.
Четыре прежних generation failures перемещены в явные execution-contract отказы;
это не четыре дополнительно реализованные функции. Итого остаётся 41 отказ из
исходных 74: 37 непрозрачных Geometry/EventReply/PaintContext/animation shapes и
четыре scoped вызова. Прежние 110 export failures сохраняются отдельно.

## Целевые проверки

`Scripts/TestInputEvents.ps1 -Publish` собирает smoke Editor, проверяет codegen,
свежий export, компиляцию всего UMG module, exact allowlist и выполняет один
native/DAS fixture. Для готовой сборки есть `-SkipBuild`.

Проверяются source path/widget destruction, неизменность source, timestamp и
остальные контрольные metadata, собственные pressed keys, finite/size rejection
без partial result, все 33 getters/projections, globals/arrays/clone и явная
collection, default/wrong-kind/missing-input/navigation refusals, сохранность
после ошибок, reload и очистка таблицы. Отдельные compile probes запрещают
integer constructor, numeric cast и Token field.

Это targeted Editor NullRHI проверки; physical input, UserWidget reply routing,
полный Foundation/PIE/packaged не запускаются. Коммит не создаётся. Итоговый
отчёт: `logs/input-events-validation.json`; исходный контракт и ограничения:
[InputEventTransportContract](InputEventTransportContract.md).
