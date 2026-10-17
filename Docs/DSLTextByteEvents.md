# FText и uint8 в делегатах, событиях и интерфейсах

`uint8` — настоящий беззнаковый байт (0…255), без enum-идентичности. `FText` — неизменяемое значение Unreal с сохранением истории локализации; оно не преобразуется в `string` при передаче.

```das
require ue_text

[ue_delegate]
struct TextQuery {
    signature:function<(value:FText):FText>
}

[ue_delegate]
struct ChangeValue {
    def signature(var number:uint8&; var title:FText&;
                  @ue_out var previous:uint8&):FText {}
}

[ue_multicast]
struct Changed {
    signature:function<(number:uint8; title:FText):void>
}

[ue_interface(BlueprintType)]
class ValueContract {
    [ue_function(BlueprintCallable)]
    def EchoText(value:FText):FText {}
    [ue_function(BlueprintCallable)]
    def Change(var number:uint8&; @ue_out var title:FText&):void {}
}
```

Привязка и вызовы используют обычные `ue_bind`, `Execute`, `AddUFunction`, `RemoveUFunction`, `Broadcast`, `Clear`. Сигнатуры обработчиков должны совпадать по типам и направлениям. Single-cast поддерживает результат и `ref/out`; multicast — `void` и `ref`, но не out-only. Массивы `array<uint8>` и `array<FText>` используют native snapshots.

Для подписки на отражённый native multicast подходят обычные обработчики:

```das
[ue_event(name="OnTitleChanged")]
def OnTitleChanged(value:FText):void {
    self.Title=value
}
```

Native FText можно передать по значению или `const FText&`; обработчик DSL принимает значение. Для byte используется только raw uint8, а не enum. Поддерживаются также native события с `const TArray<uint8>&` и `const TArray<FText>&`. Все подписки выполняются на игровом потоке и снимаются при освобождении runtime-контекста.

Примеры и регрессии: `ue_text_byte_delegates.das`, `dsl_text_byte_interface.das`, `dsl_text_byte_events.das`. Низкоуровневый `dsl_text_byte_interface_calls.das` проверяет именно interface-message dispatch, а не обходной прямой вызов метода объекта.

Interface-message dispatch теперь поддерживает DAS-host, включая self и ActorComponent. Ошибки и предел вложенности обрабатываются общим механизмом вызовов; см. [DSLInterfaceReentry.md](DSLInterfaceReentry.md).

Пустой FText без namespace/key считается тем же default независимо от invariant-флага: Blueprint CDO delta serialization может не сохранить отличие от исходного пустого значения. Это правило не применяется к именованным локализованным значениям. Непустые FText сравниваются по истории и идентичности, а не только по отображаемой строке.

`Scripts/GenerateBindings.py` принимает byte/text в IR интерфейсов, создаёт параметры `uint8` / `UETextValue`, массивы этих типов и обычные методы-обёртки. В byte-мосте используется проверяемый целочисленный ABI с преобразованием на границе обёртки; в FText-мосте остаётся native snapshot.

Проверено 2026-10-17: Editor и Win64 Development build, строгий gameplay PIE
(1 успешный тест, 0 warnings/failures/skips), packaged foundation (0 failures),
gameplay и playable-map smoke; всё под NullRHI. 94 проверки генератора прошли.
Экспорт реального Blueprint-интерфейса и генерация его обёрток также прошли.
Shipping и визуальная отрисовка не проверялись.

Журналы: `logs/text-byte-events.log`, `logs/text-byte-events-report/index.json`,
`logs/text-byte-events-runtime.log`, `logs/text-byte-events-foundation.log`,
`logs/text-byte-events-map.log`, `logs/text-byte-events-generator.log`,
`logs/text-byte-events-export.log`, `logs/text-byte-events-ir.json`,
`logs/text-byte-events-bindings.das` и `logs/text-byte-events-cooked-driver.log`.
