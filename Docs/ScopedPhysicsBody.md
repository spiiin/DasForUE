# Scoped body token и Async Physics AddForce

Для отдельной physics VM доступны:

```dascript
options gen2
require ue_foundation
[export]
def physics_tick(dt:float; time:float) {
    let body=ue_physics_body(0)
    // Сила в мировых координатах; не умножать на dt.
    ue_physics_add_force(body,1000.0lf,0.0lf,0.0lf,false)
    // true означает ускорение: native adapter умножает значение на массу.
}
```

На обычном game thread после BeginPlay и готовности physics state:

```cpp
auto VM = CreateDasAsyncPhysicsVM(Path, Actor, Error);
if (VM && !VM->BindBody(0, Mesh, Error)) { VM->Revoke(); }
// VM->ClearBody(0, Error) снимает разрешение на тело, сохраняя саму VM.
```

Разрешение выдаёт native host через `BindBody(slot, component, error)`. Поддержаны
слоты 0..7, только компоненты того же Actor, что владеет VM. Неправильная
перепривязка не заменяет существующий слот. Одинаковое тело нельзя разрешить
через несколько слотов. Native handles и permissions не удерживают Actor или
Component при GC. В mailbox разрешённых компонентов/указателей нет.

`ue_physics_body(slot)` возвращает opaque `UEPhysicsBodyToken`. В DAS не
экспортированы поля, Proxy, FBodyInstance или internal particle. Для каждого
VM callback действует своя фиксированная таблица, идентификаторы монотонны и
не переиспользуются между VM, шагами, revision и reload. Token можно скопировать
в локальную переменную/контейнер, но прошлый token не подходит следующему шагу.
В `ue_property`, `ue_function` параметрах/результатах и JSON/IR авторинга этот
тип не поддержан. Числовой cast/конструктор не экспортированы. Обычные gameplay Tick/events/timers и globals initializer
не могут приобрести token или применить physics force.

Это нужно, потому что raw `FBodyInstanceAsyncPhysicsTickHandle` хранит сырой
Proxy: после пересоздания тела даже его IsValid может обратиться к освобождённой
памяти. Здесь хранится weak Component и shared deletion/generation marker,
который не продлевает жизнь Proxy. Перед acquire, каждым AddForce и commit
сначала проверяется marker, затем заново выбирается текущий member BodyInstance
и Proxy. Старый Proxy никогда не разыменовывается. Дальше проверяются marker
identity, принадлежность World/solver, текущий engine-issued step, отсутствие
resimulation и Dynamic/Sleeping internal particle.

## Первый поддержанный набор тел

Box, Sphere, Capsule и StaticMesh с единственным собственным BodyInstance,
без welding и welded children. Native selector проверяется при BindBody на
обычном GT; в frozen callback произвольный virtual GetBodyInstance не вызывается.
Skeletal/ragdoll, InstancedStaticMesh/HISM, geometry collections/cluster unions,
static/kinematic и неготовые тела отклоняются. Custom selector, возвращающий
другой BI вместо известного member, также отклоняется.

Body recreation делает старое permission недействительным. Явный BindBody на
обычном GT может выдать новое permission; после отключения VM по ошибке нужно
создать новую VM. Destroy/unregister/teardown/reload отзывают разрешение через
bridge lifecycle либо при следующем использовании weak body. Если скрипт не
обращается к телу, ошибку из-за его отсутствия специально не создаём.

## Числовые проверки и применение

За шаг допускается максимум 64 вызова AddForce на VM. Проверяются конечность
XYZ, положительная конечная масса/inverse mass, force*mass в AccelChange,
суммарная сила и результирующие acceleration/velocity. Операции накапливаются
в фиксированном массиве из восьми записей. После успешного physics_tick все
тела повторно проверяются, затем силы применяются к internal API в том же
engine preSim callback, по одному суммарному AddForce на тело. Дополнительной
задержки на следующий шаг нет. Chaos выполняет пробуждение и интегрирование.
Прямого ProcessEvent/private UPhysicsThreadLibrary или GT AddForce здесь нет.

Ошибка скрипта, неверный/чужой/старый token, переполнение или превышение лимита
отбрасывает силы **этой VM за текущий callback**, отменяет output mailbox и
отключает binding. Перехват ошибки через DAS try/recover не снимает poisoned
transaction. Силы прошлых шагов и других callbacks не откатываются.

Все обычные UE externs продолжают иметь запрет в isolated VM. Две новые функции
проходят существующий physics-only FFI gate с проверкой точного Context и
engine-issued step; всего таких externs четыре с input/output. Raw reflected
getter и PhysicsThreadLibrary.AddForce остаются закрыты обычному transport.

DSL-декларация registration теперь доступна: [AsyncPhysicsDSL](AsyncPhysicsDSL.md).
Пока нет network rewind/resimulation, physics
worker-thread VM или жёсткого deadline выполнения DAS. Это проверенный UE
capability boundary для доверенного скрипта, не sandbox произвольных native
modules. Правила памяти отдельной VM: [AsyncPhysicsVM](AsyncPhysicsVM.md).

## Целевые проверки

`Scripts/TestScopedPhysicsBody.ps1` собирает smoke Editor и запускает fixture
в sync и fixed async 1/60. Проверяет фактическую скорость от DAS силы и
AccelChange, sleeping wake, XYZ/копирование token, отказ чужого owner/World,
scope/epoch, malformed/default token, limits, arithmetic overflow, ошибки без
частичных сил, реальный weld, body recreation/rebind, unregister/Destroy/GC,
обычный gameplay/init/reflected-authoring отказ и accepted reload.
Включена только регрессия isolated VM/mailbox и execution-role/native bridge.
Runner проверяет отсутствие engine errors/ensure и стабильность DLL SHA256.
Отчёт: `logs/scoped-body-validation.json`. Полные Foundation/PIE/packaged не
запускаются; коммит не создаётся.


Финальный прогон: **80 scoped body checks + 66 VM checks + 102 role/lifecycle
checks = 248 PASS**, sync и fixed async 1/60. Проверены также отсутствие
числового cast и integer constructor у token. Force-проба: при массе 10 и
силе 1000 за 0,5 секунды получена скорость 50; AccelChange и sleeping wake
подтверждены отдельным измерением. Реальная welding fixture использует
`FAttachmentTransformRules(EAttachmentRule::KeepWorld,true)`; обычный
KeepWorldTransform без weld flag не прикрепляет симулируемый child.
Runtime/Editor DLL SHA256 стабильны за весь финальный прогон; engine errors
и ensure отсутствуют. Логи scoped проверки:
`Build/ScopedPhysicsBody/e739d833e63546ac8fdb9b3dddcbf49a`.
Коммит не создавался, полные Foundation/PIE/packaged не запускались.
