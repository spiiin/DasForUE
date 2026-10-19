# Приёмка общего HitResult transport

Адаптер реализован в соседнем чате; этот документ задаёт независимую проверку
интеграции в пакет Collision / Physics. Текущие safe views UETraceHit сами
по себе не доказывают работу сгенерированных native trace/sweep wrappers.

## Генерация

После завершения адаптера и его сборки повторить экспорт и генерацию полного
CollisionPhysics.profile.json без Publish. Проверить свежий report.json:

```powershell
python Scripts/CheckCollisionHitResultCoverage.py <fresh-report.json>
```

Проверка требует все 33 объявления: 24 Line/Sphere/Box/Capsule Single/Multi
в вариантах channel/profile/object types, семь GameplayStatics helpers,
два component overlap. Она требует успешную компиляцию всего пакета в UE,
но не заявляет runtime-проверку каждого метода.

Runner TestCollisionApiPackage.ps1 пока содержит старое требование отказа
LineTraceSingle; после адаптера это требование нужно удалить и заменить
успешной проверкой покрытия. Не считать старый PASS приёмкой адаптера.

## Целевые runtime-сценарии

| Группа | Проверяемое поведение |
| --- | --- |
| Line single | hit/miss, blocking flag, distance/time, endpoints, Actor/Component identity |
| Sphere/box/capsule sweep | реальные попадания, impact/location/normal, начальное проникновение |
| Multi | массив native HitResult, несколько объектов, порядок и сохранение identity |
| Filters | ignored Actor, channel/object types/profile, ignore self |
| Roundtrip | передать выданный HitResult в BreakHitResult/GetSurfaceType; сохранить native state |
| Nested/arrays | projectile result со вложенным HitResult и multi-массивы |
| Lifetime | удержать результат и frame, Destroy/GC, weak Actor/Component/PhysMaterial не удерживают объект |
| Rejections | чужой scope, неправильный тип, освобождённый frame; существующие WorldContext guards |

FindCollisionUV требует подходящего mesh и настроек UV collision: успешная
генерация не считается положительной runtime-проверкой UV на BoxComponent.
PhysicalMaterial может законно отсутствовать; положительную identity-проверку
делать на fixture с явно установленным материалом.

Проверять в transient Game world, без изменения пользовательских карт.
Зафиксировать хеши исходников и DLL до/после прогона. Во время чужой сборки
не запускать runtime-приёмку и не объявлять изменяющуюся версию проверенной.
Полные Foundation/PIE/packaged на этом этапе не запускаются.

## Независимая runtime-матрица

`Scripts/TestCollisionTraceMatrix.ps1 -Report <fresh-run/report.json>`
создаёт и выполняет probe для всех 24 trace/sweep wrappers: 96 запросов
(hit/miss/ignored actor/ignore-self) и 24 roundtrip через BreakHitResult.
Режимы channel, object types и profile проверяются для каждой формы и Single/Multi.
Матрица использует один зарегистрированный BoxComponent существующего
GameWorld fixture. Multi-object ordering и GC проверяются отдельно адаптером;
самостоятельная матрица их не заявляет.

`-Regenerate` предварительно повторяет экспорт/генерацию без сборки и публикации.
Запускать только после окончания сборки адаптера. Runner сравнивает исходники
и обе DLL до/после проверки. Результат: logs/collision-trace-matrix-validation.json.



## Результат 2026-10-18

Повторная генерация и компиляция всего Collision пакета — PASS, 175 обёрток.
Матрица — PASS: все 24 варианта, 96 запросов и 24 BreakHitResult roundtrip.
Свежий пакет: Build/ApiPackages/ue_api_collision/451943800bc04f368b6aeacf4776111b/report.json.
Итоговый отчёт: logs/collision-trace-matrix-validation.json.
Исходники и фактически загруженные DLL smoke-проекта стабильны во время прогона.

Полная проверка группы HitResult: 32/33 готовы. Оставшийся
Blueprint_PredictProjectilePath_Advanced проходит export, но не генерацию:
Unsupported struct array member (массив ActorsToIgnore в PredictParams).
Это отдельный пробел генератора для object arrays внутри struct, а не повод
ослаблять запрет flattening ActorInstanceHandle. Полный диагностический снимок:
logs/collision-hitresult-coverage-after.json. Флаг --traces-only checker проверяет
24 trace/sweep отдельно и проходит.

Целевой прогон адаптера в соседнем чате сохранён в
logs/hit-transport-validation.json: nested/array/ref, multi-hit, snapshot/call-frame
GC, Destroy и ожидаемые отказы — PASS. Независимая матрица не повторяет эти GC
сценарии и не заявляет физическую симуляцию или положительную FindCollisionUV.
Полные Foundation/PIE/packaged не запускались; новый коммит не создавался.

## PredictProjectilePath_Advanced: blocker закрыт — 2026-10-18

Генератор разрешает ObjectProperty/ClassProperty arrays в полях struct так же,
как в параметрах UFunction. Сохраняются проверка native storage marker и класса,
nullable object reads, контроль направления и существующий frame marshalling.
WeakObjectProperty arrays по-прежнему не подменяются сильными ссылками.
Новый native ABI или ручной wrapper для Advanced не понадобился.

Повторная генерация и весь Collision module preflight — PASS: 176 wrappers,
все 33/33 объявления HitResult. Пять новых regression tests, 96 существующих
generator tests и восемь HitResult generator tests — PASS (109 суммарно).

Целевой runtime probe: hit, nested HitResult/PathData, ignored Actor внутри
PredictParams, clear массива, null element, дубликаты и spatial miss — PASS.
Две загруженные DLL стабильны. Native rebuild, full Foundation/PIE/packaged
не запускались. Collision package остаётся staged; новый коммит не создавался.

Воспроизведение: Scripts/TestStructReferenceArraysGenerator.py;
Scripts/GenerateApiPackage.ps1 -Profile Bindings/Packages/CollisionPhysics.profile.json -SkipBuild;
Scripts/TestProjectileAdvanced.ps1 -Report <fresh-run/report.json>.
Probe: Tests/ProjectileAdvanced/probe.template.das.
Отчёт: logs/projectile-advanced-validation.json;
coverage: logs/collision-hitresult-coverage-after.json.
