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
