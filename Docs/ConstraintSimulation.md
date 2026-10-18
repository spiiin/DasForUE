# Два симулируемых тела с PhysicsConstraint

Точечный commandlet DasConstraintSimulation создаёт transient Game world с
Chaos physics scene, двумя отдельными Actor/BoxComponent массой 1 кг и общим
PhysicsConstraintComponent. Оба тела динамические. Для изоляции степеней свободы
их центры изначально совпадают; gravity, damping и collision responses отключены.
Это проверка joint solver без контактов, пола и взаимного трения тел.

Все настройки, impulses и angular velocities задаёт phased DAS probe через
сгенерированные ue_constraint_*, ue_joint_* и ue_body_* wrappers. Native код
создаёт fixture, возвращает её в исходное состояние между независимыми случаями,
шагает World с dt=1/60 с и независимо измеряет положение/угол рамок constraint.
Напрямую TickComponent и ручные solver impulses для проверки не вызываются.

Мир без GameMode явно маршрутизирует NotifyBeginPlay. В промежуточных dynamic
случаях Reset не вызывается: настройки меняются на живой движущейся связи.

## Сценарии и результат 2026-10-18

| Проверка | Измеренный результат |
| --- | --- |
| Locked linear | impulse одному телу переместил оба на 25 см; относительное смещение 0 |
| Free X | второе тело ушло на 50 см, первое осталось на месте |
| Limited X 25 см | относительное смещение 25 см |
| Locked angular | вращение передалось обоим телам; относительный угол 0 |
| Free twist X | относительный угол около 45 градусов |
| Limited twist 20 градусов | итоговый относительный угол около 17,96 градусов |
| BreakConstraint | связь broken, тела расходятся на 50 см; старый handle отклоняется |
| Recreate | те же тела и broken=false, но новое физическое поколение: старые snapshot/pending setter отклоняются, свежий handle работает |
| Dynamic X: до изменения | Free X дал 30 см относительного смещения |
| Dynamic X: после изменения | текущая связь ограничена 10 см; итоговое смещение 10 см |
| Dynamic twist: до изменения | Free twist дал относительный угол около 27 градусов |
| Dynamic twist: после изменения | лимит 10 градусов; итоговый угол около 5,97 градусов |

Измерения проверяются по завершении этапов; тест не обещает отсутствие transient
overshoot на каждом шаге и не является долгим stress-тестом устойчивости solver.
Angular case проверяет twist X; отдельные swing/cone, drives/motors, projection,
plasticity и автоматический force-threshold break остаются за областью этого probe.
Break здесь вызывается явно из DAS.

После recreation pending setter пытается выключить DisableCollision. Вызов
отклоняется до native dispatch; fresh getter подтверждает отсутствие побочного
эффекта. Новая связь затем реально удерживает тела во время следующих World ticks.
Проверяются также GetConstrainedComponents identity и library getters/ref-out.

## Воспроизведение

```powershell
./Scripts/TestConstraintSimulation.ps1
# После сборки smoke Editor target:
./Scripts/TestConstraintSimulation.ps1 -SkipBuild
# На конкретном готовом пакете:
./Scripts/TestConstraintSimulation.ps1 -SkipBuild -Report <run/report.json>
```

Без Report runner заново экспортирует Collision package и выполняет whole-module
preflight без публикации. Он требует runtime с checked UEConstraintHandle transport
и профиль, включающий ConstraintInstanceBlueprintLibrary. Сам transport отдельно
описан в ConstraintTransport.md; существующий TestConstraintTransport.ps1 проверяет
его lifetime/GC/foreign-context сценарии, которые здесь полностью не повторяются.

DAS source: Tests/ConstraintSimulation/probe.template.das.
Итог: logs/constraint-simulation-validation.json. В нём сохранены точные пути
package report и runtime.log, результаты 12 этапов и хеши DLL/fixture/module.

Editor Development build, whole module preflight и все 12 этапов — PASS.
516 шагов соответствуют 8,6 с игрового времени. Engine errors/ensure отсутствуют;
обе загруженные DLL, fixture sources и generated module стабильны во время теста.
Новых ручных production wrappers не понадобилось. Пользовательские карты/ассеты
не изменяются. Полные Foundation, PIE и packaged не запускались.
