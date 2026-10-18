# Execution roles и native Async Physics Tick bridge

Реализован следующий инфраструктурный этап после
[исследования async handle](AsyncPhysicsHandleContract.md). Первоначальный этап добавил guards и native
lifecycle bridge. Теперь доступны isolated VM и scoped body/AddForce transport:
[ScopedPhysicsBody](ScopedPhysicsBody.md). Декларативная регистрация и привязка тела доступны через
[AsyncPhysicsDSL](AsyncPhysicsDSL.md).

## Зачем нужна роль

В frozen GT presim `IsInGameThread()` истинно, но solver ожидает окончания
callback и действует особый контракт доступа к физическим данным. Поэтому
обычная проверка игрового потока не защищает от Spawn, GT physics API,
таймеров, reflection, GC, загрузки ассетов или изменения компонентов в этой
фазе. Одного guard в generated wrapper тоже недостаточно: есть low-level externs.

`EDasExecutionRole` различает Gameplay и PhysicsPreSim. Native bridge задаёт
роль только на время callback и восстанавливает её через RAII при выходе и
ошибке. Дополнительно проверяется IsGameThreadFrozen solver-а — для выбранного
owner и зарегистрированных engine worlds. Это закрывает authored/static
callback без bridge TLS, смену WorldContext и отсутствие self. Проверка
использует API, присутствующий в Shipping; debug Chaos thread flags не дают
разрешение на операцию.

## Общая граница UE FFI

Все 579 регистраций UE externs теперь проходят `AddUEExtern` в
`Private/DasUEExternGuard.inl`. Фабрика сохраняет scalar/CMRES ABI, но передаёт
в interpreter и JIT builtin address pointer на проверяющую функцию. Она
отклоняет вызов в PhysicsPreSim до выполнения native тела. Проверка относится
и к функциям без Context/self, и к низкоуровневым вызовам.

Guard не считается pure при оптимизации. Foundation уже не предоставлял
собственную AOT-реализацию; у guarded externs дополнительно выставлен noAot,
чтобы прямой emitted cppName не обходил проверку. Флаги needCallerStackFrame
и существующие argument/result codecs сохранены.

На этом этапе **physics allowlist UE externs пуст**: запрещены даже value-helper
ue_add/ue_vector/UEName constructors. Обычная математика и локальные значения
самого daScript разрешены; будущие physics adapters получат отдельный явно
проверенный путь. Это не песочница для произвольного unsafe DAS, сторонних
библиотек или native C++ кода.

Входы обычной gameplay VM RunFile/RunCallback отклоняются в frozen physics
фазе до компиляции/работы с сессией. Это включает именованные authored methods
и static вызовы с другим owner. Публичного physics VM entry пока нет.

`Scripts/AuditUEExternGuards.py` проверяет исходники DasRuntime и запрещает
обходные addExtern/makeExtern/addInterop вне общей фабрики. Отчёт сохраняет
registration sites и source hashes. Он не распространяется на сторонние
плагины и обычные builtin-функции daScript.

## Native lifecycle bridge

`UDasAsyncPhysicsBridge` — transient final NotBlueprintable ActorComponent,
без UFUNCTION и без DAS extern для Create. Его доверенный C++ API принимает
native callback; этот этап не регистрирует произвольный скрипт в physics tick.

- Create выполняется на обычном GT для живого Actor/ActorComponent после
  BeginPlay в Game world с physics scene. Ссылки на script owner/world слабые;
  компонент принадлежит Actor через обычный component lifecycle. Есть лимит
  256 отслеживаемых bridge-компонентов; окончательный revoke уничтожает компонент.
- Callback допустим только при frozen solver своего мира, на GT, с конечными
  DeltaTime/SimTime, положительным DeltaTime и вне resimulation/shutdown.
  Вложенный native вызов bridge не может выдать второй scope.
- `FDasAsyncPhysicsStep` — native borrowed scope с уникальными epoch/revision,
  временем шага и закрытой weak-ссылкой на bridge. IsCurrent проверяет текущий
  TLS scope, роль, binding revision и owner lifetime. Копия недействительна
  после выхода, следующего callback, отмены или уничтожения binding.
  Это ещё не physics body handle и не переносится в DAS/JSON/IR.
- Revoke/Deactivate внутри callback немедленно запрещает дальнейшее использование
  scope. Штатный DeferRemoveAsyncPhysicsTick снимает physics tick безопасно;
  callback/context и компонент освобождаются позднее через обычную GT-фазу.
  FlushDeferred внутри callback ничего не разрушает. Другие callbacks того же
  engine tick продолжают работать. Deactivate не меняет engine IsActive посреди
  обхода physics callback set, что иначе могло бы привести к engine assert.
- Возврат false/ошибка обработчика отзывает подписку. Ошибка сохраняется в
  LastError; повторных пользовательских вызовов после неё нет. Native callback
  является доверенным кодом: не должен непосредственно разрушать владельца,
  запускать GC или выполнять неподдерживаемые UE операции внутри frozen фазы.
- Unregister/EndPlay/BeginDestroy компонента, потеря владельца и teardown мира
  прекращают dispatch. Runtime.ReleaseObject и явный script EndPlay отзывают
  owner bindings, в том числе без существующей VM-сессии.
- Успешно принятый script reload отзывает предыдущий binding; rejected reload
  сохраняет рабочую подписку. Начальная загрузка VM не считается reload.
  Начало class reinstance/default-update консервативно отзывает bindings;
  автоматического восстановления после migration этот этап не обещает.
- World cleanup и runtime shutdown снимают bindings и deferred ticker. Weak
  registry и сохранённый scope не удерживают уничтоженный Actor при GC.

Отдельная подготовленная interpreter VM с фиксированным mailbox теперь
реализована: см. [AsyncPhysicsVM](AsyncPhysicsVM.md). Ordinary UE externs в ней
по-прежнему запрещены. Четыре physics-only externs (input/output и body/force)
имеют отдельный scoped gate. Scoped body token и AddForce теперь реализованы:
[ScopedPhysicsBody](ScopedPhysicsBody.md).

## Целевые проверки

```powershell
./Scripts/TestExecutionRoles.ps1 -SkipBuild
```

После сборки smoke Editor выполнены 102 native/runtime checks: по 51 в sync и
fixed async 1/60. Проверены ordinary scalar/CMRES/property ABI, pure DAS в
заранее подготовленном тестовом context, 12 UE extern families, отсутствие
side effects, unscoped/foreign/no-self обходы и отказ gameplay VM entry до
компиляции. Проверены также epoch, отмена с peer callback, ошибка callback,
вложенный scope, rejected/accepted reload, ReleaseObject, явный script EndPlay,
component owner unregister, Destroy/GC, CDO/manual callback и world cleanup.

`IDasExecutionGuardProbe` — native test harness: он подготавливает отдельный
context заранее и не является продуктовым DAS physics callback API.

Один whole-module Collision preflight подтверждает работу обычного reflected
и struct/container ABI после смены общей registration factory. Покрытие
осталось 234/241, async handle по-прежнему заблокирован. DLL hashes стабильны
весь финальный прогон, engine errors/ensure отсутствуют.

Отчёты: `logs/execution-role-validation.json`,
`logs/execution-role-coverage.json`; runtime logs находятся в Build/ExecutionRoles.
Полные Foundation, PIE, packaged/Shipping, worker-thread DAS, network/resim и
новый body transport не проверялись. Коммит не создавался.
