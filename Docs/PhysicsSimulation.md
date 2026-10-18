# Collision / Physics: симуляция тела и PhysicsHandle

Целевой runner создаёт transient Game world с физической сценой Chaos,
одним подвижным BoxComponent и PhysicsHandleComponent. DAS вызывает существующие
сгенерированные ue_body_* / ue_handle_* wrappers. Native commandlet независимо
измеряет состояние тела после настоящих World ticks с шагом 1/60 секунды.
Fake movement и ручной вызов PhysicsHandle::TickComponent не используются.

Тестовый мир не имеет GameMode, поэтому commandlet явно маршрутизирует
NotifyBeginPlay после World::BeginPlay. Это включает component ticks обычным
lifecycle-путём. Иначе тело симулируется, но PhysicsHandle не обновляет цель.

## Сценарии

| Сценарий | Независимая проверка |
| --- | --- |
| Масса и velocity | масса 10 кг; linear velocity меняет позицию, angular velocity — ориентацию |
| Impulse | импульс 1000 при массе 10 создаёт скорость 100 см/с |
| Force | сила 1000 на 30 шагах создаёт скорость 50 см/с и перемещение |
| Damping | linear/angular damping уменьшают обе скорости после 60 шагов |
| Gravity | после 30 шагов вертикальные скорость и положение уменьшаются |
| Grab/move | grabbed component совпадает с телом; тело приближается к заданной цели |
| Location/rotation | обновлённая цель перемещает и поворачивает тело с сохранением локальной рамки захвата |
| Release | компонент становится null, повторный Release безопасен, тело снова движется свободно |

В DAS дополнительно проверяются getters, nullable output и отказ null receiver.
Force повторно задаётся перед каждым шагом: это непрерывное воздействие,
в отличие от однократного impulse. GrabComponentAtLocationWithRotation сохраняет
поворот тела относительно рамки захвата, поэтому native assertion учитывает
исходную ориентацию тела, а не приравнивает её к углу target.

## Запуск

```powershell
./Scripts/TestPhysicsSimulation.ps1
# Если editor target уже собран:
./Scripts/TestPhysicsSimulation.ps1 -SkipBuild
# Повторный runtime probe на конкретном готовом пакете:
./Scripts/TestPhysicsSimulation.ps1 -SkipBuild -Report <run/report.json>
```

Без Report runner повторяет генерацию и whole-module preflight Collision package,
без публикации. Без SkipBuild собирается только editor target smoke-проекта.
Точечный commandlet: DasPhysicsSimulation; DAS template:
Tests/PhysicsSimulation/probe.template.das. Новых ручных physics wrappers нет.

450 шагов моделируют 7,5 с; wall-clock ожидание для этого не нужно.
Runner проверяет восемь native measurements, финальный marker, отсутствие
engine errors/ensure и стабильность обеих загруженных DLL, fixture sources
и копии generated module. Итог: logs/physics-simulation-validation.json.
Детальные координаты и скорости записываются в runtime.log каждой попытки.

## Границы

NullRHI не отключает эту физическую симуляцию. Визуальная сцена, контакт с полом,
трение/restitution, skeletal/ragdoll, welding, network replication, async physics
и отдельные PhysicsConstraintComponent сценарии не проверяются этим runner.
У BoxComponent включены QueryAndPhysics и симуляция, но collision responses
установлены Ignore, чтобы измерения force/damping не зависели от столкновений.
Пользовательские карты и ассеты не изменяются. Полные Foundation/PIE/packaged
не запускаются.

## Результат 2026-10-18

Editor Development build и whole Collision module preflight — PASS.
Восемь native measurement groups и null receiver rejection — PASS.
На 450 шагах (7,5 с игрового времени) подтверждены:

- velocity 100 см/с дал перемещение 50 см за 0,5 с;
- impulse 1000 при массе 10 кг дал скорость 100 см/с;
- force 1000 за 0,5 с дал скорость 50 см/с и перемещение 12,917 см;
- damping 4 за 1 с уменьшил linear velocity до 1,593 см/с,
  angular velocity — до 1,434 град/с;
- gravity -980 за 0,5 с дала velocity -490 см/с и падение на 126,583 см;
- PhysicsHandle приблизил тело к двум разным target positions, сохранил
  component identity и повернул тело в ожидаемой локальной рамке;
- release вернул nullable component, повторный release прошёл,
  тело свободно переместилось на 30 см за 0,5 с при скорости 60 см/с.

Первые попытки исправили setup probe: явный BeginPlay для мира без GameMode,
имя generated target output и использование доступных vector operations.
Продакшен wrappers для body/handle менять не потребовалось.

Финальный runtime.log не содержит engine errors/ensure. DLL и fixture/module
hashes стабильны. Отчёт logs/physics-simulation-validation.json содержит путь
к точной попытке и хеши. Полные Foundation/PIE/packaged не запускались.
