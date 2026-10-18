# Пакет UMG / Input

Профиль: `Bindings/Packages/UMGInput.profile.json`; модуль `ue_api_umg_input`.
Выбраны пять libraries и 20 receiver-классов. UE modules: Engine, InputCore,
SlateCore, Slate, UMG, EnhancedInput. Include=* выбирает весь локальный reflected
API каждого класса, без inherited копий. Это пакет вызовов; widget authoring,
автоматические overrides событий и подключение любых input delegates из этого
факта не следуют.

## Результат — UE 5.8.3, 2026-10-18

Из **517** кандидатов: **320** generation_ready, **110** export_blocked,
**87** generation_blocked, 0 policy_excluded. Весь staged DAS-модуль успешно
компилируется в UE. В этом снимке нет функций с UnsafeDuringActorConstruction;
общие construction/signature/world/receiver guards сохраняются.

| Класс / library | Ready | Export blocked | Generation blocked |
| --- | ---: | ---: | ---: |
| InputComponent | 0 | 10 | 0 |
| InputSettings | 11 | 1 | 0 |
| KismetInputLibrary | 21 | 1 | 31 |
| EnhancedInputComponent | 0 | 1 | 0 |
| EnhancedInputLibrary | 6 | 5 | 3 |
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
| UserWidget | 45 | 51 | 9 |
| Widget | 49 | 16 | 7 |
| WidgetBlueprintLibrary | 26 | 6 | 23 |
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
компилирует весь модуль. **Единственный runtime probe** создаёт reflected FKey
с KeyName=SpaceBar, передаёт его в `KismetInputLibrary.Key_IsKeyboardKey` через
сгенерированный wrapper и проверяет true. Это короткая проверка Name → FKey →
native call → bool, а не симуляция нажатия клавиши или rendering-тест.

Сборка успешна (target up to date), probe — PASS. Ошибок/ensure нет; DLL, профиль
и генераторы не менялись во время проверки. Отчёт: `logs/umg-input-validation.json`.
Полные Foundation/PIE/packaged, physical input, widget rendering и существующий
managed input callback suite не запускались. Выполнение остальных 319 wrappers
этим шагом не подтверждается.

## Backlog

Машинный список всех 197 отказов с классом/методом/причиной:
`Bindings/Packages/ue_api_umg_input.summary.json`. Отказы не скрыты ручными
exclusions и не посчитаны готовыми функциями.

- 77 generation failures: opaque/empty structs; сюда попадают формы Slate
  geometry/input events/event replies/paint state и InputActionValue, для которых
  нельзя выдумывать native layout из пустого reflection-описания.
- 10 generation failures: delegate-параметры. Нужен явный managed callback
  contract с владением и снятием; простая передача VM/function pointer не добавлена.
- 110 export failures: internal/protected/deprecated policy, soft references,
  non-callable объявления, Blueprint events и delegate signatures.

Сами signatures делегатов в backlog не означают, что существующий ue_event
backend перестал работать. Overrides событий UserWidget не становятся обычными
вызовами только потому, что экспортёр их перечислил. Эти категории нужно
расширять отдельно под конкретную потребность.

## Использование и публикация

Пакет пока staged в `Build/ApiPackages/ue_api_umg_input/<run>/`.
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
    let key=S_Key_a8a0a14c(f_keyname=ue_name("SpaceBar"))
    return ue_input_key_Key_IsKeyboardKey(key) ? 42 : -1
}
```

Это generated transport имени FKey; отдельный удобный пользовательский key DSL
этим пакетом не добавлялся. Поля/классы для project-based native discovery
по-прежнему генерируются существующим путём.
