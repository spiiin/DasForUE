# Точечные navigation-запросы на Recast NavMesh

Runner: `Scripts/TestNavigationQueries.ps1`; fixture: `DasNavigationCommandlet.cpp`;
DAS: `Tests/Navigation/probe.template.das`.

```powershell
./Scripts/TestNavigationQueries.ps1
# Уже собранный Editor:
./Scripts/TestNavigationQueries.ps1 -SkipBuild
# Повторить на конкретном успешном staged пакете:
./Scripts/TestNavigationQueries.ps1 -SkipBuild -Report <run/report.json>
```

Без Report runner заново экспортирует и генерирует полный Pawn/AI пакет,
проверяет компиляцию staged модуля, затем запускает только navigation commandlet.
Content и карты не сохраняются; опубликованный DAS API не заменяется.

## Сцена и контракт проверки

В отдельном процессе создаётся transient мир с полом 40×40 м, препятствием
2×6 м между Start=(-600,0,50) и Goal=(600,0,50), а также отдельной площадкой
в (3000,0,50), отделённой от пола зазором. Сначала Recast строит настоящий
NavMesh в EditorMode. Commandlet завершает подготовку assets и обслуживает
штатный delayed unlock через core ticker, поскольку обычного editor frame loop
здесь нет. После синхронного завершения сборки мир переводится в Game,
акторы проходят BeginPlay, и запросы выполняются из DAS с live WorldContext.

Native baseline подтверждает покрытие обеих площадок и полный путь в обход
препятствия. Далее используются только сгенерированные `ue_navigation_*` wrappers:

- project: успешная проекция точки на пол;
- random navigable/reachable: минимум восемь успешных выборок каждого вида,
  проверка радиуса 150 см и повторной проекции результатов на NavMesh;
- ограничение — 64 попытки: false у отдельного случайного запроса допустим,
  но недостаток успешных выборок приводит к отказу всей проверки;
- navigation raycast: препятствие блокирует прямую, соседний свободный участок нет;
- synchronous location path и Actor-goal path: valid, не partial, длиннее прямой;
- GetPathCost/Length: Success enum, положительные out double;
- до отдельной navigable-площадки: valid partial path вместо полного;
- проекция далеко за границами NavMesh: false.

Нельзя требовать равенства `NavigationPath.GetPathLength` и отдельного
`NavigationSystem.GetPathLength`: второй native API использует оценку длины
по corridor. В тестовой сцене наблюдались соответственно 1410.962 и 1714.689 см,
cost 1878.951; прямое расстояние между заданными точками — 1200 см.

Runner проверяет все шесть маркеров, отсутствие engine Error/ensure и стабильность
DLL, fixture/template и generated module SHA256. Финальный отчёт сохраняется
в `logs/navigation-validation.json`, полные логи — в соответствующем staged run.

Это проверка запросов к настоящему NavMesh, а не движения Character/AI по пути.
Полный PIE, packaged, сеть, BehaviorTree/EQS и perception stimuli не запускаются.
Изменений runtime backend для успешных запросов не потребовалось.
