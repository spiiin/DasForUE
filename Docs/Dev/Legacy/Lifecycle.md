# BeginPlay, Tick и EndPlay в daScript

Реализация находится в `C:\src\DasForUE2`. Это фиксированный контракт native Actor
адаптера, а не окончательный пользовательский DSL.

## Подключение

У Actor, наследующего `ADasProbeActor`, задайте свойство `LifecycleScript` до начала
игры. Относительное имя, например `lifecycle_actor.das`, ищется в
`Plugins/DasForUE/Content/Das/Scripts`. Абсолютный путь также поддерживается.
Путь фиксируется при BeginPlay; изменение самого файла подхватывается при Tick.
Замена свойства на другой путь в уже работающем Actor пока не поддерживается.

Три обработчика необязательны и должны быть экспортированы:

```das
options gen2
require ue_foundation

[export]
def begin_play() {
    ue_state_set_int("begins", ue_state_get_int("begins", 0) + 1)
}

[export]
def tick(delta_time:float) {
    ue_state_set_int("ticks", ue_state_get_int("ticks", 0) + 1)
}

[export]
def end_play(reason:int) {
    // Освобождение ресурсов скрипта; контекст пока ещё существует.
}
```

`main` для lifecycle-скрипта не требуется. Если он присутствует, сохраняется контракт
`main(input:int):int` для обычного `ExecuteDas`. У обработчиков возвращаемый тип —
`void`; неверная сигнатура любого присутствующего обработчика отклоняет новую версию.

Полный проверенный пример:
`C:\src\DasForUE2\Plugins\DasForUE\Content\Das\Scripts\lifecycle_actor.das`.
Он рассчитан на класс с Blueprint-visible полями `health:int` и `speed:float`:
BeginPlay увеличивает health, Tick прибавляет DeltaSeconds к speed, EndPlay записывает
результат завершения. Такие поля создаёт существующий пример `schema_v2.das`.

## Порядок событий и reload

- После `Super::BeginPlay` Actor вызывает `begin_play`; затем Unreal вызывает
  `tick` каждый игровой Tick и передаёт настоящий `float DeltaSeconds`.
- Каждый Actor использует собственный постоянный контекст. Editor world
  не получает игровые callbacks; это проверено отдельно от PIE world.
- При обновлении логики переносится именованное int32-state, а отметка уже
  доставленного BeginPlay остаётся в сессии. BeginPlay повторно не вызывается.
- Если исходник изначально не компилируется, контекст не запускается. После
  исправления первый успешный Tick доставляет BeginPlay, затем Tick.
- Если сам BeginPlay завершился с runtime panic, он не повторяется автоматически:
  уже выполненные записи не откатываются. Следующие Tick могут выполняться.
- Ошибочный кандидат оставляет предыдущие callbacks рабочими. `LastDasError`
  доступен на Actor для просмотра диагностики; одинаковая ошибка не печатается
  заново на каждом Tick.
- EndPlay использует последнюю принятую версию и не компилирует новый исходник
  во время завершения. Он вызывается до `Super::EndPlay` и освобождения контекстов.
  При Destroy базовый `Destroyed` сначала маршрутизирует EndPlay, затем выполняется
  окончательная очистка.
- Внутри EndPlay разрешён доступ к `self`, даже когда Actor уже помечен как
  уничтожаемый. Для обычных вызовов такой доступ по-прежнему отклоняется.
- Контекст освобождается и при ошибке EndPlay. Повторная доставка EndPlay для той же
  сессии подавляется; вызовы Tick после завершения отклоняются.

`reason` соответствует числовому `EEndPlayReason::Type` текущего UE:
`0` Destroyed, `1` LevelTransition, `2` EndPlayInEditor, `3` RemovedFromWorld,
`4` Quit. Сгенерированный enum для daScript появится с расширением bindings.

## Проверки

`Scripts\TestEditor.ps1`: 66 PASS. Новые проверки охватывают однократный BeginPlay,
float delta, все три неверные сигнатуры, reload без повторного BeginPlay, EndPlay
на рабочей версии, необязательные обработчики без main, восстановление после
первичной ошибки компиляции и отсутствие повторного BeginPlay после panic.

`Scripts\TestLifecyclePIE.ps1`: настоящий PIE с двумя Actor, событиями от движка,
изменением исходника между игровыми кадрами, fallback, Destroy и EndPIE.
17 контрольных точек; Unreal Automation — 1 success, 0 failed,
0 succeededWithWarnings. EndPlay проверяется наблюдателем в настоящем
`AActor::OnEndPlay`, до удаления объекта.

Регрессии прошли: structural reload в PIE, сохранение и открытие assets в новом
процессе. Packaged Win64 Development: build/cook/package и запуск завершились
с кодом 0, все 66 foundation-проверок прошли. Автоматическую доставку событий
Actor движком проверяет PIE-тест; packaged suite проверяет runtime-контракт callbacks.

Команды регрессий: `Scripts\TestReloadPIE.ps1`, `Scripts\TestReload.ps1`,
`Scripts\TestPackaged.ps1`. Журналы: `C:\src\DasForUE2\logs`.

При проверке упаковки обнаружен и исправлен устаревший staging receipt: UBT
использовал закешированный список RuntimeDependencies и пропускал новые `.das`.
`TestPackaged.ps1` теперь передаёт `-NoUBTMakefiles`, чтобы заново раскрывать список
файлов перед упаковкой. В новом receipt присутствуют lifecycle_actor.das и
persistent_counter.das.

## Что ещё остаётся

Обновление тела скрипта поддержано. Перенос lifecycle-сессии и состояния при замене
самого UObject через Blueprint structural reinstancing пока не реализован;
для такого изменения следует остановить PIE. Проверка файлов и компиляция пока
синхронные на game thread, отдельные для каждой пары owner/script.

Пользовательский DSL, произвольные UFunction-вызовы, delegates/подписки,
таймеры и async остаются отдельными этапами. Следующий функциональный срез —
управляемые подписки на события Unreal с очисткой при reload и EndPlay.


Дополнение: управляемые multicast-подписки теперь реализованы (Docs/Delegates.md).
Вложенный Destroy из чужого обработчика во время VM-вызова отзывает подписки
и безопасно откладывает освобождение VM. Повторная доставка скриптового EndPlay
внутрь уже активной VM пока не поддерживается; обычный Destroy/EndPIE вне VM
сохраняет описанное поведение. Это отдельно отмечено в текущем плане.
