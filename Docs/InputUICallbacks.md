# Enhanced Input callbacks и UMG delegate-параметры

## Типизированный обработчик

```das
ue_enhanced_bind(self.MoveAction, "Triggered", "OnMove")

[ue_function(BlueprintCallable)]
def OnMove(value:InputActionValue):void {
    let axis = ue_input_axis2d(value)
    ue_character_move(ue_character_self(), float(axis.y), float(axis.x))
}
```

Такой же тип принимается обычным `[export] def handler(value:InputActionValue):void`.
Модуль `ue_enhanced_input` подключает `ue_input_value`. Сигнатура обработчика
определяет выбор транспорта: новый InputActionValue или прежние bool/float/
UEVector2D/UEVector. Оба вида используют одну таблицу owned input bindings.
Новый тип сохраняет kind и все оси native-значения, включая смену типа Action.
Подписка привязана к Action + Trigger; проверка контракта предшествует замене.
Применяются существующие EndPlay/release/reload и восстановление recipe.

Native proxy проверяет SourceAction и конечность данных, копирует native value
в owned snapshot очереди. Авторский метод получает его через общий struct bridge;
обычный экспорт — через numeric value. Нет указателей на callback stack или
хранения VM function pointers. Копию можно сохранить в DAS array.

## Параметры native single-cast delegates

```das
require ue_api_umg_input

let endpoint = ue_function_delegate(target, "ChooseNavigation")
ue_widget_SetNavigationRuleCustom(widget, direction, endpoint)
```

`target` — проверяемая объектная ссылка либо UObjectHandle; имя указывает
существующую UFunction с точной сигнатурой native delegate, включая return/out/ref.
Для снятия используются соответствующие native Unbind/Remove APIs или явное
`ue_function_delegate_null()` там, где native setter допускает пустой delegate.
FProperty kind и полный путь native signature проверяются в invocation-local
frame; общий DelegateHandler проверяет совместимость и layout UFunction.
Target удерживается до вызова; сохранённый UE delegate имеет native weak
семантику. Эти endpoints не являются подпиской на свободную DAS-функцию и
не получают автоматический Unbind от произвольного native Bind-метода.
Для такого API вызывающий использует соответствующий Unbind по lifecycle.
Managed input и managed кнопки HUD выполняют собственную автоматическую очистку.

Генератор поддерживает scalar input-only single-cast delegate-параметры.
Delegate outputs, контейнеры делегатов и произвольные C++/Slate callbacks остаются
отдельными задачами. Все десять UMG delegate-parameter failures теперь генерируются;
runtime-сценарий проверяет SetNavigationRuleCustom с возвратом UWidget и отказом
неправильного void-handler. Проверка одного setter не означает выполнение остальных.

## Игровой пример

`dsl_playable_character.das`: OnMove/OnLook принимают InputActionValue.
SetupHUD создаёт HUD текущего LocalPlayer, задаёт текст и managed Damage-кнопку.
Tab → OnToggleUI переключает GameOnly/GameAndUI с курсором; кнопка вызывает
HurtFromHUD. Используются прежние ownership и cleanup helpers, без второй UI-системы.

## Сокращённая проверка

`Scripts/TestInputUI.ps1` собирает Editor, публикует freshly generated UMG/Input
модуль после whole-module preflight и обновляет playable-пример. Затем один короткий
сценарий в отдельном `-game -NullRHI -DasInputUI` процессе проверяет:

- четыре формы typed native proxy payload и сохранённые копии;
- native UMG navigation delegate и отказ несовместимой сигнатуры;
- реальную LocalPlayer injection → авторский OnMove;
- managed HUD button, переключение UI/game и снятие input handles на release.

Это проверка native routing и состояния, без физической клавиатуры/мыши и
визуального rendering. Полные PIE/packaged и reload suites не запускаются.
Отчёт: `logs/input-ui-validation.json`.

Принятый прогон — PASS: Editor build, whole-module compile и один совместный
game-сценарий. Source/DLL fingerprint стабилен, Error/ensure нет. UMG/Input
опубликован с 333 generated functions; 110 export и 74 generation blockers
сохранены в package summary. Изменения не закоммичены.
