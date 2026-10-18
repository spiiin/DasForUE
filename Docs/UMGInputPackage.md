# Пакет UMG / Input

Профиль: `Bindings/Packages/UMGInput.profile.json`; модуль `ue_api_umg_input`.
Выбраны пять libraries и 20 receiver-классов. UE modules: Engine, InputCore,
SlateCore, Slate, UMG, EnhancedInput. Include=* выбирает весь локальный reflected
API каждого класса, без inherited копий. Это пакет вызовов; widget authoring,
автоматические overrides событий и подключение любых input delegates из этого
факта не следуют.

## Актуальное расширение: owned input events

**366 ready, 114 export blocked, 37 generation blocked**. Реализованы 33
functions через [owned input events](InputEvents.md), пакет установлен и
проверен обычным require. Четыре прежних generation failures перенесены в
явные execution-contract отказы: три navigation calls и DetectDragIfPressed.
Поимённый текущий [backlog](UMGInputBlockers.md). Ниже сохранён снимок до
этого расширения и его отдельные предыдущие проверки.

## Результат до input-event transport — UE 5.8.3, 2026-10-18

Из **517** кандидатов: **333** generation_ready, **110** export_blocked,
**74** generation_blocked, 0 policy_excluded. Весь staged DAS-модуль успешно
компилируется в UE. В этом снимке нет функций с UnsafeDuringActorConstruction;
общие construction/signature/world/receiver guards сохраняются.

| Класс / library | Ready | Export blocked | Generation blocked |
| --- | ---: | ---: | ---: |
| InputComponent | 0 | 10 | 0 |
| InputSettings | 11 | 1 | 0 |
| KismetInputLibrary | 21 | 1 | 31 |
| EnhancedInputComponent | 0 | 1 | 0 |
| EnhancedInputLibrary | 9 | 5 | 0 |
| EnhancedInputLocalPlayerSubsystem | 0 | 4 | 0 |
| EnhancedInputWorldSubsystem | 2 | 0 | 0 |
| InputMappingContext | 4 | 1 | 0 |
| Button | 8 | 0 | 0 |
| CanvasPanel | 1 | 0 | 0 |
| CanvasPanelSlot | 16 | 2 | 0 |
| ContentWidget | 3 | 0 | 0 |
| EditableTextBox | 11 | 3 | 0 |
| Image | 12 | 2 | 0 |
| InputKeySelector | 8 | 2 | 0 |
| PanelWidget | 10 | 0 | 0 |
| ProgressBar | 3 | 0 | 0 |
| ScrollBox | 30 | 1 | 0 |
| SlateBlueprintLibrary | 4 | 4 | 12 |
| Slider | 10 | 0 | 0 |
| TextBlock | 18 | 0 | 0 |
| UserWidget | 50 | 51 | 4 |
| Widget | 53 | 16 | 3 |
| WidgetBlueprintLibrary | 27 | 6 | 22 |
| WidgetLayoutLibrary | 22 | 0 | 2 |

Ready включает обычные Widget setters/getters, панели и canvas slots, TextBlock,
Button, Image, progress/slider/scroll, часть layout, FKey utilities и доступные
reflected InputSettings / EnhancedInput методы. Нулевой ready у InputComponent,
EnhancedInputComponent и LocalPlayerSubsystem указан явно: выбранный класс не
означает автоматическую поддержку его неотражённого или interface API.
InputAction не выбран receiver: собственных UFUNCTION нет, но объектные ссылки
на него могут входить в замыкание зависимостей других функций.

Модуль использует canonical_math и explicit defaults. Существующие managed
`ue_enhanced_input`, UI helpers и подписки сохраняются. Нового C++ dispatcher,
raw vtable вызовов и отдельного Input event loop нет.

## Сокращённая проверка

```powershell
./Scripts/TestUMGInputPackage.ps1
./Scripts/TestUMGInputPackage.ps1 -SkipBuild
```

Runner вызывает Editor build, экспортирует reflection, генерирует пакет и
компилирует весь модуль. **Единственный runtime probe** проверяет типизированный
InputActionValue: Bool/Axis1D/2D/3D, native Make/Break/ToString, обнуление лишних
осей и owned copy в array. Это не симуляция нажатия клавиши или rendering-тест.
Первый пакет проверял FKey(SpaceBar); текущий runner заменил этот probe новым.

Сборка успешна, probe — PASS. Ошибок/ensure нет; DLL, профиль, генераторы и
adapter sources не менялись во время проверки. Отчёт: `logs/umg-input-validation.json`.
Полные Foundation/PIE/packaged, physical input, widget rendering и существующий
managed input callback suite не запускались. Остальные wrappers этим шагом
в runtime не проверялись. Синтаксис и границы: [InputActionValue](InputActionValue.md).

## Backlog

Машинный список всех 184 отказов с классом/методом/причиной:
`Bindings/Packages/ue_api_umg_input.summary.json`. Отказы не скрыты ручными
exclusions и не посчитаны готовыми функциями.

- 74 generation failures: opaque/empty structs; сюда попадают формы Slate
  geometry/input events/event replies/paint state, для которых
  нельзя выдумывать native layout из пустого reflection-описания.
- Все 10 прежних delegate-parameter failures теперь генерируются через
  UEFunctionDelegate и проверяемый UFunction endpoint. Native weak lifetime и
  явный Unbind сохраняются; это не автоматическая managed-подписка на DAS closure.
  См. [Input/UI callbacks](InputUICallbacks.md).
- 110 export failures: internal/protected/deprecated policy, soft references,
  non-callable объявления, Blueprint events и delegate signatures.

Сами signatures делегатов в backlog не означают, что существующий ue_event
backend перестал работать. Overrides событий UserWidget не становятся обычными
вызовами только потому, что экспортёр их перечислил. Эти категории нужно
расширять отдельно под конкретную потребность.

## Использование и публикация

Пакет опубликован в plugin scripts после preflight; staged snapshots сохраняются
в `Build/ApiPackages/ue_api_umg_input/<run>/`.
Для установки обычного модуля существующий конвейер поддерживает:

```powershell
./Scripts/GenerateApiPackage.ps1 -Profile ./Bindings/Packages/UMGInput.profile.json -SkipBuild -Publish
```

После публикации короткий пример:

```das
options gen2
require ue_api_umg_input
[export]
def main(input:int):int {
    let move=ue_input_value(ue_vector2d(1.0lf,0.0lf))
    let parts=ue_enhanced_api_BreakInputActionValue(move)
    return parts.out_x==1.0lf ? 42 : -1
}
```

Сам `ue_input_value` доступен без публикации generated-пакета.
Поля/классы для project-based native discovery по-прежнему генерируются
существующим путём.

## Typed callbacks и UI-сценарий — 2026-10-18

После Editor build один короткий `-game -NullRHI -DasInputUI` сценарий — PASS:
четыре InputActionValue payload через native proxy и owned копии, navigation
delegate с return UWidget и отказом несовместимого handler, настоящая LocalPlayer
action injection → авторский OnMove, managed HUD click, UI/Game input и release.
Полный generated-модуль компилируется также при импорте в этом сценарии.
DLL/исходники стабильны, Error/ensure нет. Отчёт `logs/input-ui-validation.json`.
Остальные native delegate setters/animation/FieldNotify здесь не выполнялись;
полные PIE/packaged, физический ввод, rendering и reload suites не запускались.


## Аудит оставшихся 74 opaque failures

Свежий reflection/generator/source audit: 36 input events, 17 Geometry,
12 EventReply, 5 PaintContext и 4 WidgetAnimationHandle. Для input events
выделены 33 detached snapshot candidates и три live navigation calls;
DetectDragIfPressed также требует EventReply. Transport пока не реализован,
coverage не увеличен. [Поимённый аудит](UMGInputBlockers.md),
[контракт](InputEventTransportContract.md); воспроизведение Scripts/AuditUMGInput.ps1.
16 unit tests и fresh export — PASS, без сборки/PIE/packaged и коммита.


Текущий input-event этап: 38 native/runtime checks + 3 compile refusals, 33
functions exercised и 122 Python checks — PASS. Scope и воспроизведение:
[InputEvents](InputEvents.md), Scripts/TestInputEvents.ps1 -Publish.
