# Контракт транспорта Slate input events

Статус: **detached snapshots и 33 ordinary functions реализованы**.
Актуальные API, проверки и границы: [InputEvents](InputEvents.md).
Ниже сохранено исходное проектирование и разбиение 74 отказов; будущие
navigation/reply/paint пункты остаются отдельными контрактами.

## Исходный дизайн перед реализацией

Исходный статус этого раздела: design only. Машинный контракт:
`Bindings/InputEvents.contract.json`. Покрытие UMG этим аудитом не увеличивается.
Проверенный снимок UE 5.8.3: 333 ready, 110 export failures, 74 generation failures.
Поимённый [аудит](UMGInputBlockers.md) воспроизводится из свежей reflection,
реального генератора и локальных исходников Engine.

## Разбиение 74 отказов

| Группа | Функций | Что требуется |
| --- | ---: | --- |
| Input events | 36 | 33 detached snapshot functions и 3 live navigation calls |
| Geometry | 17 | Отдельный owned transport native transform value |
| EventReply | 12 | Синхронный контракт команд маршрутизации события |
| PaintContext | 5 | Capability, действующая только внутри paint callback |
| WidgetAnimationHandle | 4 | Адаптер слабого состояния анимации |

Группы не пересекаются. По native types пересечения есть: например,
`DetectDragIfPressed` требует PointerEvent **и** EventReply и отнесён к EventReply.
Сами InputEvent-типы встречаются в 37 функциях, но самостоятельная группа — 36.
Остальные 110 export failures в эту задачу не входят; события UserWidget не
становятся обычными callable-функциями после устранения opaque structs.

## Почему нельзя просто разрешить пустой struct

В `SlateCore/Public/Input/Events.h` шесть reflected типов не имеют обычных
отражённых полей: FInputEvent, FKeyEvent, FAnalogInputEvent, FCharacterEvent,
FPointerEvent и FNavigationEvent. Они содержат native данные, наследование и
virtual методы. Размер, alignment и содержимое нельзя восстановить по пустому
IR. Нужен адаптер точных `/Script/SlateCore.*` identities, а не разрешение
всех непрозрачных структур.

Базовый FInputEvent содержит `const FWidgetPath* EventPath`. Его SetEventPath
сохраняет адрес переданного маршрута; GetWindow и FindGeometry разыменовывают
его. Обычная native-копия сохраняет этот адрес. Удержание копии события поэтому
не продлевает жизнь маршрута, SWidget или SWindow.

В данной версии FPointerEvent хранит `TSet<FKey> PressedButtons` **по значению**.
Нельзя описывать его как borrowed set. Копирование должно сохранить собственный
набор кнопок, а также все остальные данные события; обнулить требуется EventPath.

## Сохраняемый снимок

Предлагается один общий transport `slate_input_event_snapshot_v1` с шестью
точными видами native payload. Script получает типизированное immutable значение,
host хранит owned payload. Адреса native объектов и двоичный layout в DAS не
передаются. Default snapshot невалиден; integer/pointer constructors и lookalike
структуры не принимаются. Тип и направление каждого call-frame параметра
проверяются до native dispatch.

Capture создаёт собственную native-копию точного вида события и очищает EventPath
**на этой копии**, пока оригинальный callback ещё выполняется. Возможная стратегия
— производный bridge-тип с легальным доступом к protected EventPath, который
копирует базовое событие и обнуляет собственный указатель. Это предложение для
следующей реализации: оно ещё требует native copy/lifetime проверки. Cast чужого
объекта к bridge-типу, поиск offset, memcpy native layout и изменение оригинала
не допускаются.

Copy/clone, сохранение в DAS array/global и получение base view сохраняют detached
ownership. Снимок может пережить исходный callback, widget Destroy и reload: в нём
нет сильной ссылки на widget/world. Это сохранённые данные ввода, а не разрешение
повторно отправлять событие в Slate. Read-only data access и native calls остаются
под существующими gameplay/execution-role guards.

Нужно сохранить:

- modifiers, включая left/right и caps lock; repeat; исходные UserIndex,
  InputDeviceId и timestamp;
- FKey, character/key codes, analog value; у CharacterEvent — native TCHAR
  code unit, который не обязательно является целым Unicode scalar;
- pointer position, last position и CursorDelta **отдельно**, собственный набор
  кнопок, effecting key, pointer/touchpad indices, force/touch flags, wheel/gesture
  vector, type, phase и inversion;
- navigation type и genesis.

При capture нельзя вычислять заново UserIndex по текущему device mapper,
timestamp по текущему времени или CursorDelta как разность координат: это может
изменить данные native-события. Внутренние uint32 indices и uint64 timestamp
сохраняются без сужения; существующие reflected getters с legacy int32 result
сохраняют объявленную UE семантику результата.

Nonfinite analog/позиции/deltas/force отклоняются. Планируемый предел — 256
pressed keys: проверка размера до deep copy/allocation, без тихого обрезания.
Валидация завершается до публикации результата; ошибка не оставляет partial
snapshot или частично записанные outputs.

В первую реализацию входят scalar input/result/out transports, 33 точные
functions из contract allowlist, существующий FKey transport и пять native
преобразований: Key→Input, Analog→Key, Character→Input, Pointer→Input,
Navigation→Input. Результат преобразования — новый detached value с точной
семантикой native slicing, без downcast. Авторские reflected ue_property,
defaults и JSON-сериализация событий пока не входят в этот контракт.

## Три вызова навигации

Key_GetNavigationActionFromKey, Key_GetNavigationDirectionFromKey и
Key_GetNavigationDirectionFromAnalog обращаются к FSlateApplication и выбранному
navigation config. Analog-вариант в NavigationConfig.cpp меняет per-user repeat
state, LastNavigationTime и Repeats. Повторная обработка старого снимка может
изменить поведение живого ввода.

Эти три функции пока не входят в allowlist обычных snapshot-вызовов. Следующему
адаптеру понадобятся initialized Slate, текущий widget/user, session/revision,
callback epoch и явная семантика повторного вызова. После выхода/ошибки callback,
Destroy/reload capability недействительна. Один IsInGameThread без проверки роли
и живого callback недостаточен. Точный алгоритм navigation evaluation/replay в
этой задаче не реализуется.

## Что не является обычным input snapshot

FEventReply содержит FReply с командами capture/focus/drag. Его нельзя превратить
в сохраняемый ответ без проверки targets и текущего синхронного события.
OnKeyDown/OnMouseMove и подобные UserWidget overrides должны вернуть ответ до
возврата в Slate. Отложенный наблюдатель может получить detached данные, но не
решать позже судьбу исходного события. Поэтому существующий queued event backend
не подключается к reply-returning overrides автоматически. Для будущего bridge
missing handler/error должен возвращать Unhandled, без старого cached reply.

FPaintContext содержит ссылки на draw list, geometry и style, а также меняемый
MaxLayer: это scope-only paint capability. FGeometry требует отдельного
транспорта transforms и правил устаревшего layout, а не указателя на widget.
FWidgetAnimationHandle содержит weak animation state; IsValid не гарантирует,
что завершившаяся анимация ещё имеет доступный state. Для него нужен отдельный
weak lifecycle adapter. Эти ограничения не закрываются input-event transport.

## Следующая реализация и целевые проверки

1. Owned native payload для шести видов, detach/clone/projections и exact identity.
2. Общий scalar call-frame codec и generator registration; открыть только 33
   функции allowlist. Не ослаблять общий запрет opaque structs.
3. Native/DAS probes: сравнить поля/getters с native; уничтожить исходный path и
   widget, затем прочитать clone/array/base views; проверить GC, wrong kind,
   default, foreign context, nonfinite и size limits. Проверить отсутствие записи
   при отказе и сохранение timestamp/device/user/cursor values.
4. Отдельно проектировать live navigation и synchronous UserWidget/EventReply
   bridge. После этого — Geometry, PaintContext и animation handle по их контрактам.

Данная задача выполнила **аудит**, не эти runtime-проверки. Выполнены 16 unit tests
аудита/контракта, один fresh reflection export, повтор реальной генерации и
проверка anchors/hashes девяти native source files. DLL/BuildId стабильны.
Группы 36+17+12+5+4 подтверждены; coverage остался 333/110/74.

Воспроизведение: `Scripts/AuditUMGInput.ps1`; отчёт
`logs/umg-input-audit-validation.json`, подробные machine evidence:
`Bindings/Packages/ue_api_umg_input.audit.json`. Сборка, physical input,
Foundation/PIE/packaged не запускались; коммит не создавался.


## Подтверждение реализации

38 native/runtime checks, все 33 generated functions, три compile-time token
refusals и 122 целевых Python checks — PASS. Собственные pressed keys,
оригинальный path/widget lifetime, clone/GC, reload и table release подтверждены.
UMG опубликован: 366 ready / 114 export blocked / 37 generation blocked.
Три navigation calls и DetectDragIfPressed остаются закрытыми.
