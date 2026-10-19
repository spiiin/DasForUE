# Async physics body handle: исследование и контракт

Актуальный статус: dedicated VM, scoped body token и безопасный AddForce реализованы;
см. [ScopedPhysicsBody](ScopedPhysicsBody.md). Ниже сохранено исходное исследование.
В runtime добавлен явный отказ для этого типа в обычных reflected call frames.
Это отдельная задача от snapshot/constraint transport: lifetime-проверок и
`IsInGameThread()` здесь недостаточно.

## Что находится в UE 5.8.3

`FBodyInstanceAsyncPhysicsTickHandle` — native struct с сырым
`FPhysicsActorHandle Proxy`. У него нет reflected-полей, weak owner, поколения
или owning reference. `IsValid()` проверяет `Proxy` и вызывает
`Proxy->GetPhysicsThreadAPI()`. У сохранённой после удаления proxy копии даже
проверка IsValid может обратиться к освобождённой памяти. Такое обращение
не выполнялось в тесте: вывод следует из native реализации.

`UPrimitiveComponent.GetBodyInstanceAsyncPhysicsTickHandle(BoneName, bGetWelded,
Index)` вызывает virtual GetBodyInstance и возвращает его ActorHandle.
Обычный PrimitiveComponent может выбрать WeldParent; SkeletalMesh выбирает
body по bone/root; InstancedStaticMesh использует instance index. Имя компонента
или совпадающий индекс не гарантируют, что тело осталось прежним.

По текстовому поиску в доступных C++-исходниках Engine/Source и Engine/Plugins
найден один потребитель — `UPhysicsThreadLibrary.AddForce(handle, force,
bAccelChange)`. Загруженная reflection отдельно подтверждает один getter и
одного consumer. Это не аудит бинарных Blueprint-ассетов, внешних проектов
или неустановленных плагинов; rg использует обычные правила исключений.
AddForce private в C++, но BlueprintCallable: native-проба вызывает его
штатным ProcessEvent с заполненным frame **внутри** корректного physics callback.

При bAccelChange=false сила записывается напрямую; при true умножается на
внутреннюю массу тела. Chaos принимает её для Dynamic/Sleeping и пробуждает
Sleeping body. Для постоянной силы вызов нужен на каждом physics step;
заранее умножать force на DeltaTime не нужно — интегрирование делает solver.

### Фаза важнее физического OS-потока

Штатный FAsyncPhysicsTickCallback зарегистрирован как Presimulate |
RunOnFrozenGameThread. Solver ждёт physics-задачи, запускает callback на GT
и затем продолжает simulation. В этой фазе IsInGameThread()==true, однако
Chaos IsInPhysicsThreadContext()==true, IsInGameThreadContext()==false.
Это подтверждено и в sync, и в fixed async режиме native-пробой.

SingleParticlePhysicsProxy различает external GT API и internal physics API.
В frozen фазе обычные GT physics accessors недопустимы. Debug helpers из
Framework/Threading.h не являются shipping-контрактом: собственный scope token
и allowlist привязки обязательны независимо от PHYSICS_THREAD_CONTEXT.

### Доказательства в локальных исходниках

- BodyInstance.h:336 и BodyInstance.cpp:5129 — raw storage и IsValid.
- PrimitiveComponent.h:2340 и PrimitiveComponentPhysics.cpp:1259 — getter и
  назначение для Async Physics Tick; SkeletalMeshComponentPhysics.cpp:2184 и
  InstancedStaticMesh.cpp:2583 — разные selectors.
- Physics/Experimental/PhysicsThreadLibrary.h:32 — единственный найденный consumer.
- PhysScene_Chaos.cpp:146 — callback options; PhysicsSolverBase.cpp:148 и
  PhysicsSolverBase.h:131 — frozen GT scope и task scheduling.
- PhysicsProxy/SingleParticlePhysicsProxy.h — GetPhysicsThreadAPI, AddForce,
  VerifyContext; Chaos/Framework/PhysicsProxyBase.h — timestamp и MarkDeleted.

Полные engine-relative пути, номера строк, source matches и SHA256 одиннадцати
файлов: `logs/async-physics-source-audit.json`. Воспроизведение:
`Scripts/AuditAsyncPhysics.py --engine <UE> --output <report.json>`.

## Выбранный безопасный контракт daScript

Это требования к следующей реализации, а не уже доступный API/DSL-синтаксис.

1. **Только engine-owned presim scope.** Нативный bridge регистрирует callback
   через обычный lifecycle Unreal. Он выдаёт временный execution capability:
   session/binding revision, World/solver identity и уникальный callback epoch.
   Обычные main/Tick/UFunction/events/timers, ConstructionScript, bind_events,
   EndPlay и editor world не могут создать такой scope. Произвольный пользовательский
   вызов функции с именем AsyncPhysicsTick ничего не разрешает.
2. **Handle заимствован до конца этого callback.** В DAS передаётся opaque
   token; в закрытой записи — weak component/owner, body selector и generation
   marker. Сырой Proxy/FBodyInstance/internal body pointer в DAS, JSON или IR
   не попадает. Токен из прошлого callback, другого VM/session/World/solver,
   pending frame или reload revision отклоняется. Даже при записи токена
   в DAS global/контейнер он перестаёт действовать после выхода из callback.
   ue_property, ue_function input/results и сериализация этого токена запрещены.
3. **Только свежее разрешение.** Перед каждым использованием проверяются
   owner/lifecycle, регистрация, совпадение World/solver и selector. По текущему
   body получается текущий proxy; его generation marker сравнивается с
   захваченным. Только затем создаётся временный native handle и проверяется
   наличие internal body. Старый native handle и его IsValid не используются.
   Shared timestamp сам по себе не продлевает жизнь proxy/компонента.
4. **Узкий первый body scope.** Сначала unwelded single-body primitives;
   выбранный BI должен принадлежать этому компоненту. Welding, skeletal/ragdoll,
   instance-index bodies, geometry collections и cluster unions закрыты до
   самостоятельных selector/lifetime-тестов. RecreatePhysicsState, смена
   выбранного body, Destroy/unregister и teardown отзывают capability.
   Dynamic/Sleeping допускаются; static/kinematic/not-ready — нет.
5. **Узкий набор операций.** Первая операция — AddForce с конечным XYZ и
   bAccelChange; проверяются также масса и конечность результата force*mass.
   Никаких произвольных ProcessEvent/native callbacks через этот handle.
   В роли PhysicsPreSim допускаются только специально разрешённые physics
   adapters и проверенные операции над локальными значениями. Обычные UObject/
   reflection/property access, GT physics API, Spawn/Destroy, GC, asset loading,
   timers/delegates/UI и переключение миров должны иметь запрет при входе,
   включая низкоуровневые externs. Одной проверки generated wrapper недостаточно.
6. **Изолированное выполнение.** Program компилируется и VM создаётся заранее
   на обычном GT; physics callback не компилирует скрипт и не вызывает reentry
   в занятую gameplay VM. Отдельный context и собственные execution-role scopes
   обязательны; state/input/output передаются как ограниченные value records.
   Нужны ограничения числа handles/calls и размера буферов. Замер времени после
   возврата не является гарантией прерывания бесконечного DAS-кода.
7. **Очистка и reload.** Epoch отзывается при любом выходе/ошибке. Destroy,
   unregister, teardown и hot reload отзывают binding revision и очищают
   handles/frames; старые callbacks больше не получают разрешение на операции.
   Изменять список callbacks во время его обхода нельзя: фактический unregister
   откладывается до безопасной GT-фазы. Нельзя ждать GT задачу из frozen callback
   или блокировать solver ожиданием VM/async task. Ошибка не откатывает уже
   применённые силы.
8. **За пределами первой версии.** Network prediction, rewind/resimulation,
   replication/determinism, worker-thread DAS и arbitrary Chaos API не обещаются.
   Режим resimulation нужно явно распознать/отклонить до отдельной поддержки.

Альтернатива для обычного gameplay — уже доступные ue_body_AddForce/impulse
и прочие GT wrappers. Очередь value-команд с GT на physics step тоже возможна,
но это отдельная семантика: она не заменяет callback-time управление
внутренним состоянием и не делает native async handle обычным snapshot.

## Что изменено и проверено сейчас

В ValidateCallField тип BodyInstanceAsyncPhysicsTickHandle явно отклоняется
до выделения call frame с сообщением о необходимости scoped frozen-game-thread
adapter. Это закрывает и getter, и AddForce, в том числе прямые low-level DAS
calls; никакого общего разрешения empty struct ABI не добавлено.

Collision остаётся **234/241 ready**. Теперь 7 export blockers и 0 generation
blockers: async handle переведён из случайного «Opaque/empty struct» в явную
группу execution_context. Шесть остальных отказов — прежние deprecated/
служебные объявления. Новых рабочих physics wrappers этим этапом не добавлено.

Runner `Scripts/TestAsyncPhysicsAudit.ps1 -SkipBuild`:

- 7 Python report tests; источник и загруженная reflection проверяются отдельно.
- По 12 native/runtime checks в sync и fixed async (1/60 с): presim phase,
  native AddForce/acceleration и независимые velocity measurements,
  generation marker/recreate, callback после recreate, два DAS rejection paths,
  unregister и Destroy/GC при сохранённых timestamp markers.
- Перед измерением fixed async даётся завершиться команде сброса скорости;
  после применения силы — публикации результата. Иначе внешний velocity reset
  способен перекрыть первый подсчитанный шаг силы.
- Весь staged Collision module проходит preflight после введения guard;
  отчёт блокеров обновляется из свежего export/native inventory.

Итог: `logs/async-physics-audit-validation.json`; source evidence и оба native
лога перечислены в нём. Проверяется native callback/consumer и отказ обычного
DAS, **не работа ещё не реализованного scoped DAS transport**. На этом этапе
не проверены skeletal/welding/network/resim, произвольный worker thread,
полные Foundation/PIE/packaged. Пакет staged; коммит не создаётся.

## Следующий этап реализации

Сначала execution-role guard для всех UE externs и lifecycle native bridge,
затем изолированный PhysicsPreSim VM, scoped body token и единственный
AddForce adapter. После этого — целевые тесты выхода/ошибки/reload, чужого
scope/epoch/World, отсутствия reentry, body recreation и реального fixed async.
Генератор должен экспортировать такие функции в отдельный physics-only профиль
с явным execution-domain contract, а не открывать их обычному Collision module.
Синтаксис DSL и публичные имена выбираются отдельно.


## Инфраструктурный этап реализован

Execution roles, общая FFI guard factory и native lifecycle bridge теперь
реализованы и прошли 102 целевые проверки. Актуальные ограничения и следующий
этап — [ExecutionRolesAndAsyncBridge](ExecutionRolesAndAsyncBridge.md).
Публичный DAS callback и scoped body/AddForce transport по-прежнему закрыты.


## Scoped transport реализован

Dedicated VM и native registration доступны; две physics-only функции теперь
позволяют Acquire/AddForce с ограниченной областью действия. Детали, ограничения
и свежие проверки: [ScopedPhysicsBody](ScopedPhysicsBody.md). Raw отражённый
handle остаётся закрыт, а декларативный registration DSL ещё не добавлен.
