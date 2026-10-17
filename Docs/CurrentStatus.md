# Актуальное состояние и функциональные приоритеты

Обновлено 2026-10-18. Это сводка по текущему коду и отчётам, а не процент
готовности к production. Ранние разделы README и записи Plan.md описывают
исторические этапы; ограничения нужно читать вместе с последующими расширениями.

## С чего начать

Готовый пример — [PlayableCharacter](PlayableCharacter.md), исходник
`Plugins/DasForUE/Content/Das/Scripts/dsl_playable_character.das`.
В нём уже есть настоящий Character, камера и текстовый компонент, defaults,
редактируемые input-ссылки, WASD, мышь, прыжок, lifecycle и освобождение input.
Нативные поля и отражённые методы вызываются обычным синтаксисом `self`.

```powershell
.\Scripts\CreatePlayableCharacter.ps1
# Если Editor уже собран:
.\Scripts\CreatePlayableCharacter.ps1 -SkipBuild
```

Команда создаёт свежую папку `/Game/DasExamples/Playable/<id>` с Character,
pickup, HealthComponent, Widget Blueprint HUD, шестью InputAction,
MappingContext, отдельным mesh, двумя NiagaraSystem и картой (15 ассетов). Интерфейс взаимодействия
хранится один раз в `/Game/DasExamples/Shared/Interactable`.
Путь записан в `logs/playable-example.json`.
Набор регрессионных Blueprint-ов при этом не создаётся. Повторный запуск создаёт
новую папку; изменение структуры существующего Blueprint требует его reimport.

## Что уже реализовано

| Область | Возможности и текущая граница |
| --- | --- |
| DSL | Class/struct/enum/interface, свойства и функции, аннотации и групповые объявления, defaults, компоненты и наследование. Это отдельная authoring-модель, а не прямое размещение объектов daScript в памяти UE. |
| Вызовы | UE ↔ daScript, return/ref/out, Blueprint Function Libraries и WorldContext, типизированные объектные и классовые ссылки. Произвольный C++ и любые virtual override автоматически не открываются. |
| Значения | Native enum/struct, массивы, карты и множества, объектные ссылки, FName/FText, soft object/class и поддержанные soft-контейнеры. Точные сочетания типов имеют явные ограничения. |
| Игровая логика | Actor/Component/Pawn/Character, Initialize/Construction/BeginPlay/Tick/EndPlay, default components и attachments, поиск/Spawn, input, timers, delegates, async/latent adapters. Отдельные примеры уже существуют. |
| Нативный API | Генерация по reflection для выбранных классов и поддержанных сигнатур. Сейчас семь игровых Engine receivers плюс отдельный ABI fixture; тип возвращаемой ссылки сам по себе не открывает все методы её класса. |
| Обновление | Logic reload с сохранением последней рабочей версии; часть structural/defaults migrations с предварительной проверкой и ограниченным откатом. Универсальная миграция любых типов и объектов не обещается. |

Подробности: [генерация нативного API](NativeGameplayGeneration.md),
[контейнеры](NativeGameplayHashContainers.md), [soft-ссылки](SoftReferences.md),
[Function Libraries](DSLFunctionLibraries.md), [композиция компонентов](DSLComponentComposition.md).

## Следующие функциональные шаги

1. **Сквозное взаимодействие с миром — реализовано.** В сцене есть
   [подбираемый предмет](PlayablePickup.md): mesh, сфера столкновений, реальное
   событие пересечения, счёт и восстановление по таймеру. Для генератора добавлены
   StaticMeshComponent/SphereComponent, для ссылок — публичные проверки
   валидности и идентичности. Дальнейшие API добавляются под следующую игровую
   механику, а не ради расширения списка поддержанных классов.
2. **Убирать низкоуровневый код по результатам этого примера.** Пользовательские
   методы должны использовать типизированные ссылки, обычные поля/методы и
   публичные managed helpers. Ручные call frames и внутренние `__ue_*` нужны
   только там, где ещё нет подходящего публичного API. Переименование helpers
   без улучшения возможностей само по себе не является задачей.
3. **Отдельно закрывать production-возможности.** Shipping, полноценный AOT
   всего игрового пути, чистая машина, сеть/RPC/replication и расширенные
   миграции — самостоятельные этапы. GAS, StateTree и прочие пакеты
   добавляются под конкретную потребность; их наличие в плане не означает реализацию.

Оптимизации хранилища snapshot-ов и проверки исходников пока отложены.
Возвращаться к ним стоит при измеримой проблеме в игровом сценарии.

## Проверки при разработке

На итерации — необходимая сборка и целевые проверки изменённого пути.
Полный Foundation, PIE-набор и packaged build выполняются перед завершением
крупного этапа, по запросу либо при обоснованном риске для нескольких систем.
В отчёте отдельно перечисляются выполненные и отложенные проверки.

Для текущей версии: Editor Development, семь ассетов и семь сценариев
подбора в отдельном game-процессе — PASS; также прошла 31 проверка генератора.
Полные Foundation/PIE/packaged не запускались. NullRHI и программно поданный input
не подтверждают качество рендера или работу физических устройств.
Отчёт: `logs/pickup-validation.json`. Коммит не создавался.

Дополнение: целевая проверка pickup прошла, включая реальное пересечение,
восстановление в занятой области и отмену при Destroy. Семь сохранённых ассетов;
31 проверка генератора. Полные Foundation/PIE/packaged не запускались.
См. PlayablePickup.md и logs/pickup-validation.json.

## Автоматическое обнаружение native API — 2026-10-18

C++ список receiver-классов удалён. Генерация обнаруживает native-объявления
из входных DAS-модулей и их импортов; весь поддерживаемый API обнаруженных классов
экспортируется без выбора отдельных вызовов. Текущий реестр: 18 классов, 879 полей,
1873 метода. Команда генерации также собирает обновлённый реестр, если не указан
-SkipBuild/-Check. См. [процесс и ограничения](NativeGameplayDiscovery.md).
Точечные discovery/generator/reflection/runtime/pickup проверки прошли; полный
Foundation/PIE/packaged прогон не выполнялся.

## Async mesh pickup — 2026-10-18

MeshAsset в PlayablePickup — типизированная soft-ссылка. До completion предмет
скрыт и не собирается; ошибки оставляют его недоступным. LoadMesh заменяет старую
заявку, managed teardown отменяет её при Destroy/EndPlay. Сохранён отдельный mesh
пакет для проверки настоящей холодной загрузки. Все 12 точечных сценариев прошли
в чистом smoke-проекте с текущими DLL/DAS, без полного Foundation/PIE/package.
Неоднозначность копий Interactable при повторном экспорте затем устранена общим контрактом;
повторный экспорт и все 12 сценариев прошли в основном smoke-проекте.
Актуальные пути и границы проверки — в logs/pickup-validation.json.


## Трассировки, UMG и damage — 2026-10-18

Добавлен [игровой API и пример](GameplayServices.md): LineTrace и SphereTrace
с типизированным результатом и слабыми ссылками; создание Widget Blueprint HUD,
FText, кнопки и управление локальным вводом с очисткой по времени жизни контекста;
нативный ApplyDamage и авторский HealthComponent с событием Changed.
`ue_event(source="Health",...)` теперь может подписываться на свой компонент.
E взаимодействует с предметом по лучу камеры через Blueprint-интерфейс;
H наносит урон; Tab включает курсор. Обычный подбор при пересечении сохранён.

Целевые проверки и их границы: `logs/gameplay-features-validation.json` и
`logs/pickup-validation.json`. Это начальный срез соответствующих систем:
multi-hit/object queries, полный UMG API, point/radial damage, networking и
передача UETraceHit через отражённые свойства/методы пока не реализованы.
Полные Foundation/PIE/packaged проверки в этом изменении не выполнялись.
Коммит не создавался.

## Повторный экспорт Interactable — 2026-10-18

Новый игровой пример использует один shared Interactable по каноническому пути;
повторный экспорт больше не создаёт одноимённый интерфейс в папке каждой карты.
Старые копии сохраняются. Ранее описанная блокировка основного smoke-проекта
устранена для этого примера; см. SharedInteractionInterface.md и целевой отчёт
logs/shared-interface-validation.json.


## Montage и animation events — 2026-10-18

Добавлен [managed Montage API](Montage.md), модуль `ue_animation.das` и пример
`dsl_montage_actor.das`: Play со скоростью, позицией и секцией; Stop, Cancel,
проверка активности; типизированный MontageEvent (Completed/Interrupted/BlendOut/
NotifyBegin/NotifyEnd). Поддержаны Montage Notify и Montage Notify Window.
ID подписки относится к конкретному native instance ID. Завершение, отмена,
Destroy/EndPlay и успешный hot reload снимают собственные обработчики; отклонённый
reload сохраняет рабочую версию. Успешный reload не перезапускает анимацию.

`Scripts/TestMontage.ps1 -SkipBuild`: восемь групп целевых сценариев — PASS,
включая настоящий Notify Window, отрицательные входы, повторный запуск,
ожидающие события, сохранение стороннего listener, принятый/отклонённый reload
и Destroy. Отчёт: `logs/montage-validation.json`.
Это проверка Editor Development в game-процессе без рендера; полноценный AnimBP,
видимые позы/root motion, сеть, полный Foundation/PIE/packaged не проверялись.
Коммит не создавался.

## Звук и Niagara — 2026-10-18

Добавлен [управляемый API эффектов](Effects.md), `ue_effects.das`.
Playable pickup и Character воспроизводят звук и Niagara при подборе и уроне.
Завершение пары освобождает компоненты и вызывает необязательный callback один
раз; stop, Destroy/EndPlay и успешный reload отзывают pending callbacks.
Отклонённый reload сохраняет эффект. Чужие компоненты не затрагиваются.
Семь целевых групп `TestEffects.ps1 -SkipBuild` — PASS, реальный RHI offscreen,
AudioComponent и CPU Niagara. Визуальное/слуховое качество, networking, полные
Foundation/PIE и packaged не проверялись. Без коммита.

## Завершение точечной проверки montage — 2026-10-18

Editor Development и TestMontage в основном smoke-проекте — PASS: десять игровых
групп и одна отрицательная authoring-проверка. Дополнительно подтверждены повторный
запуск с тем же именем из Completed и сохранение native playback другой группы
после play/stop/replace/cancel. Неверный тип аргумента callback отклоняется до
создания Blueprint. Базовые notify/stop/cancel/reload/Destroy проверки сохранены.
Отчёт: logs/montage-validation.json; ограничения и пример: Montage.md.
Полный Foundation/PIE/packaged и видимые позы/root motion не проверялись.

## AIController и MoveTo — 2026-10-18

Добавлен [управляемый AI API](AI.md): AIControllerRef, движение к Actor/позиции,
MoveResult (Success/Blocked/OffPath/Aborted/Invalid), необязательный completion,
проверка активности и отмена по имени. Cleanup учитывает UE request ID, сохраняет
более новое внешнее движение и чужие listeners. Destroy/EndPlay и успешный reload
отзывают движение и pending callback; отклонённый reload сохраняет рабочий запрос.
Отдельный `CreateAIEnemy.ps1` экспортирует playable demo с противником, который
обходит препятствие по сохранённому NavMesh и наносит урон игроку.
`TestAI.ps1 -SkipBuild`: восемь целевых групп — PASS, включая настоящее движение
и damage, queued completion/Destroy и принятый/отклонённый reload. Отчёт:
`logs/ai-validation.json`. Rendering/audio, сеть, полный Foundation/PIE и packaged
не проверялись. Без коммита. Behavior Tree/Perception/StateTree пока не добавлены.
