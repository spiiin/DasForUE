# Подбираемый предмет

Исходник игровой логики: `Plugins/DasForUE/Content/Das/Scripts/dsl_playable_pickup.das`.
Получатель очков: `dsl_playable_character.das`, поле `Score` и метод `AddScore`.

## Создание и запуск

```powershell
.\Scripts\CreatePlayableCharacter.ps1
# После сборки Editor можно пропустить её:
.\Scripts\CreatePlayableCharacter.ps1 -SkipBuild
```

Открыть карту из вывода команды или `logs/playable-example.json`, нажать Play.
WASD — движение, мышь — взгляд, Space — прыжок. Куб находится перед персонажем.
Карта создаётся в новой папке `/Game/DasExamples/Playable/<id>`; существующие
проекты и папки не удаляются. Экспортёр сохраняет Blueprint персонажа и предмета,
InputActions, MappingContext, отдельный PickupMesh, карту и зависимости персонажа.

Предмет содержит корневой `SphereComponent` радиусом 75 и дочерний
`StaticMeshComponent`. Mesh задан типизированной soft-ссылкой:

```das
var MeshAsset:SoftStaticMesh=ue_soft(type<SoftStaticMesh>,"/Engine/BasicShapes/Sphere.Sphere")
```

`SoftStaticMesh` — `$UESoftObject<StaticMeshRef>` из `ue_pickup_components`.
В `BeginPlay` вызывается `LoadMesh`, который запускает:

```das
ue_soft_load_async("mesh",self.MeshAsset,"OnMeshLoaded")
```

`OnMeshLoaded(mesh:StaticMeshRef)` получает типизированную ссылку без ручного cast.
До загрузки `Available=false`, `MeshLoading=true`, mesh отсутствует, визуальный
компонент скрыт и коллизии триггера выключены. Успешный callback устанавливает mesh,
`MeshReady=true`, видимость и `QueryOnly`. Пустая ссылка или ошибка загрузки
увеличивает `LoadFailures`; предмет остаётся недоступным, без DAS-исключения.

`LoadMesh` доступен из Blueprint: после изменения `MeshAsset` можно повторить
загрузку. Он отменяет предыдущую заявку с именем `mesh`, снимает таймер respawn
и снова закрывает подбор. `Restore` не может включить предмет до `MeshReady`.
Если игрок уже внутри сферы при включении коллизий, подбор может произойти сразу.
`MeshWasLoaded` показывает, был ли ресурс уже в памяти перед последней заявкой;
`MeshCallbacks` считает доставленные завершения, `LoadFailures` — неудачные.

Экспортёр примера сохраняет отдельный `PickupMesh.uasset` и назначает его путь
soft-свойству экземпляра на карте. Это копия Cube; пол использует исходный Cube.
CDO/карта не назначают pickup mesh в Visual и не держат его сильной ссылкой.
Поэтому свежий игровой процесс действительно загружает этот пакет асинхронно,
а не проверяет только completion уже загруженного вместе с полом ресурса.

`Collector` указывает на получателя `PlayableCharacter`. Экспортёр карты назначает
этого персонажа размещённому предмету. `Points` по умолчанию 10,
`RespawnSeconds` — 3 секунды; оба значения должны быть положительными.
Это однопользовательский пример с явно назначенным получателем, без репликации.

## Поведение daScript

```das
[ue_event(name="OnActorBeginOverlap")]
def OnOverlap(overlapped:ActorRef; other:ActorRef):void {
    self.OverlapEvents+=1
    if(!self.Available || !ue_is_valid(other) || !ue_is_valid(self.Collector)) { return }
    if(!ue_same_object(other,self.Collector)) { return }
    self.Available=false
    self.Trigger.SetCollisionEnabled(ECollisionEnabled.NoCollision)
    self.Visual.SetVisibility(false,true)
    self.Collector.AddScore(self.Points)
    self.Pickups+=1
    ue_timer_start("respawn",self.RespawnSeconds,"Restore",false)
}
```

Это настоящее sparse multicast-событие Unreal с двумя Actor-параметрами,
подключённое через существующий managed event/frame bridge. Подбор не опрашивается
в Tick. Visual не участвует в столкновениях; Trigger использует `OverlapOnlyPawn`
и `QueryOnly`. Посторонние Pawn игнорируются.

`Available` выключается до изменения столкновений и защищает от повторного
начисления. Метод `Restore` снимает старый именованный таймер, включает состояние,
видимость и столкновения. Если игрок всё ещё внутри сферы, новое пересечение
может сразу подобрать предмет снова; начисление происходит один раз за период.
При выходе и повторном входе восстановленный предмет также подбирается заново.
`AddScore` меняет свойство персонажа и строку его world-space TextRender-компонента.
Это не экранный HUD. `Pickups`, `Restores` и `OverlapEvents` — счётчики для наблюдения.

Таймер, подписка и async-заявка принадлежат предмету и снимаются managed teardown
при EndPlay/Destroy; завершение отменённой заявки не вызывает метод уничтоженного владельца. Отдельный C++ Tick для игровой логики не добавлен.

## Публичный API

В нативный генератор добавлены receiver-классы StaticMeshComponent и SphereComponent,
включая подходящие унаследованные методы. Канонические ссылки находятся в
`ue_pickup_components.das`; генерация сохраняет существующие фильтры сигнатур.
Изменены reflection manifest, скомпилированный registry и сгенерированный модуль.
Пользовательский код вызывает обычные `SetStaticMesh`, `SetCollisionEnabled`,
`SetSphereRadius`, `SetVisibility`; ручных call frames в классе предмета нет.

`ue_is_valid(ref)` проверяет слабую UObject-ссылку без ручного `.handle`.
`ue_same_object(a,b)` сравнивает две живые UObject-ссылки, в том числе разные
типизированные представления одного объекта. Пустые/истёкшие ссылки дают false;
идентификаторы и указатели наружу не выдаются. Для классов и soft-ссылок остаются
их специальные функции проверки.

## Целевые проверки

```powershell
.\Scripts\TestPickup.ps1 -SkipBuild
```

Без `-SkipBuild` runner сначала собирает Editor. Затем он проверяет генератор,
создаёт свежую сцену и запускает её отдельным процессом Editor `-game -DasPickup`.
Геометрические пересечения создаются движком после перемещения Character, без
ручного вызова обработчика daScript. Только проверка дублирующего события
дополнительно вызывает настоящий Unreal delegate во время cooldown.

Первые семь сценариев: посторонний Pawn, реальный подбор и очки/mesh/collision,
защита от дублирования, восстановление таймером, повторный вход, восстановление
в занятой области и уничтожение с активным таймером/подпиской. В тесте задержка
сокращается до 0.15 секунды; сохранённый пользовательский default остаётся 3 секунды.

Результат 2026-10-17: PASS; 31 проверка генератора, воспроизводимость артефактов,
Editor Development, сохранённые семь ассетов и отдельный game-процесс.
Отчёт: `logs/pickup-validation.json`, журнал: `logs/pickup-game.log`.
Полные Foundation/PIE и packaged build не запускались. NullRHI и программное
перемещение не проверяют качество рендера или физические устройства ввода.
Коммит не создавался.

Текущая карта: `/Game/DasExamples/Playable/f11090b4a75e4fe79c1836d70c429fc4/PlayableMap`.

## Асинхронная загрузка — 2026-10-18

Точечный runner дополнительно проверяет холодную загрузку отдельного mesh-пакета,
запрет подбора/ручного Restore во время загрузки, замену ещё не завершённой заявки,
пустую ссылку, отсутствующий ресурс и уничтожение до доставки callback.
Отсутствующий ресурс намеренно выдаёт сообщение загрузчика Unreal; ожидаемый
результат игрового callback — пустая ссылка и безопасное недоступное состояние.
Проверка уничтожения удерживает сам уничтоженный Actor от GC и убеждается, что
счётчик callback остаётся нулевым после прохождения отложенных завершений.

Исторический отчёт о семи ассетах выше предшествует soft-mesh расширению.
Актуальные карта, список ассетов и проверок находятся в
`logs/pickup-validation.json`; `logs/playable-example.json` содержит последнюю карту.

Проверено 2026-10-18: Editor Development собран; 34 проверки генератора и все
12 сценариев pickup прошли. Для игрового прогона использован чистый smoke-проект
с теми же Editor DLL и текущими исходниками DAS. Повторный экспорт в основной
smoke-проект встретил несколько ранее сохранённых копий Interactable с одним
stable ID; это отдельное ограничение общей сцены, не исправление async API.
Параметр `-Project <path.uproject>` у CreatePlayableCharacter/TestPickup позволяет
выбрать проект проверки. Точный путь использованного проекта записан в отчёте.
Полные Foundation/PIE/packaged проверки не выполнялись; коммит не создавался.

## Общий интерфейс и основной smoke-проект — 2026-10-18

Проблема повторного экспорта с копиями Interactable устранена: новый пример
использует один интерфейс из `/Game/DasExamples/Shared`. Старые карты и интерфейсы
сохранены; неоднозначный глобальный выбор не используется для этого контракта.
См. [причину, решение и проверку повторного экспорта](SharedInteractionInterface.md).
Первый изолированный async-прогон выше описывает состояние до этого исправления.
Актуальная проверка основного проекта — `logs/shared-interface-validation.json`.
