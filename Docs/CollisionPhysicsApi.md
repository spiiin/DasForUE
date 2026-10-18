# Профиль Collision / Physics

Первый проход систематической генерации для UE 5.8.3. Профиль:
`Bindings/Packages/CollisionPhysics.profile.json`, staged module `ue_api_collision`.
На этом этапе модуль проверяется в Build/ApiPackages, но не устанавливается
в Content/Das/Scripts. Действующие World/Math и native registry не заменяются.

## Область

- KismetSystemLibrary: Line/Sphere/Box/Capsule Trace, overlap и collision API.
- GameplayStatics: HitResult, projectile prediction и collision helpers.
- PrimitiveComponent: collision/overlap, physics, force/impulse, mass, damping,
  gravity, velocity, physical materials, rigid body, move-ignore и closest point.
- PhysicsConstraintComponent и PhysicsHandleComponent: все локальные отражённые
  объявления, включая диагностируемые deprecated/служебные методы.

Унаследованные копии не включаются. Методы Actor/SceneComponent не дублируются
на constraint/handle receiver; они остаются в World. Неотражённый C++, Chaos
solver API, async callbacks, delegates и расширения дополнительных плагинов
не являются областью этого первого профиля. Рендерные materials и StackTrace
не включены. Профиль не является обещанием покрытия всей физики Unreal.

## Результат первого прохода

185 кандидатов, 143 обёртки прошли генерацию и совместную компиляцию модуля;
41 export blocker и один generation blocker. Construction guard сохранён
для 41 готовой функции. Из 143 объявлений 89 уже есть в Math/World,
54 отсутствуют в этих пакетах: 41 constraint и 13 handle. Наличие альтернативных
native/managed helpers не проверяется этим числом.

Поимённый [отчёт блокеров](CollisionPhysicsBlockers.md) воспроизводится из
свежего generic export и отдельного native reflection inventory. Машинный
снимок: `Bindings/Packages/ue_api_collision.summary.json`. В нём сохранены
объявления, стадии, причины, пути type issues, хеши evidence и baseline-пакетов.
Native-policy hints уточняют диагностику, но не заменяют generic backend.

## Что снимать дальше

1. **Общий transport collision results.** 33 выбранных объявления используют
   HitResult: 24 trace/sweep, семь GameplayStatics helpers и два component overlap.
   Reflected поля PhysMaterial/Component и вложенный ReferenceObject являются
   weak references. Их нельзя бездумно сделать сильными: результат попадания
   не должен удерживать уничтоженный Actor/Component.
2. **ActorInstanceHandle.** Простого разрешения weak-полей недостаточно:
   generic validator отдельно отклоняет этот тип, поскольку отражённые поля
   не описывают всю его native-семантику. Нужен проверенный codec/adapter
   для HitResult с разрешением Actor через UE, включая invalid/expired cases.
   Это ограничение видно в DasMarshalling.inl и не является новым runtime-тестом.
3. **Constraint handle.** GetConstraint дополнительно требует weak owner,
   uint32 index и отдельного решения о lifetime/допустимых операциях handle.
4. **Async physics tick handle.** GetBodyInstanceAsyncPhysicsTickHandle
   экспортируется, но непрозрачная структура требует адаптера; отражения
   недостаточно. SetCollisionProfileNameProperty — отдельный CustomThunk.

Ещё шесть отказов относятся к deprecated или служебным, не BlueprintCallable
объявлениям. Они не являются шестью недостающими кодеками и не требуют
ослабления visibility policy.

В привязке уже есть managed `ue_line_trace`, `ue_sphere_trace`, `UETraceHit`
и overlap helpers. Отказ generic wrappers не означает отсутствия trace вообще.
Следующий этап должен использовать существующий collision-result подход
и общие type descriptors, а не добавлять ручную обёртку для каждой из 33 функций.
После добавления transport нужен повторный экспорт: устранение первого
блокера может обнаружить последующие ограничения структуры/массива.

## Целевые проверки

```powershell
.\Scripts\TestCollisionApiPackage.ps1 -SkipBuild
```

Без SkipBuild сначала собирается тестовый Editor target. Для сравнения нужны
успешные полные отчёты World и Math в Build/ApiPackages/*/latest-report.json.

Runner выполняет 17 тестов пакетного генератора и шесть тестов отчёта;
проверяет компиляцию всего staged-модуля, damping/gravity и collision settings
на зарегистрированном BoxComponent в transient Game world. SphereOverlapActors
проверяется на попадание, промах и исключение владельца. BoxOverlapComponents
проверяет массив объектов и identity root-компонента. Затем проверяются
отсутствующий WorldContext, null и неправильный receiver, совместный импорт
Collision/World/Math с квалифицированными именами.

Миры уничтожаются после probe; ассеты и открытый редактор не затрагиваются.
Это Editor NullRHI: физическая симуляция/импульс, trace/sweep runtime, PIE,
packaged/Shipping и полный Foundation здесь не проверяются. Generated означает
проверенную компиляцию, а не выполнение каждой функции пакета.

Последний результат: `logs/collision-api-package-validation.json`.
Общий отчёт: `Build/ApiPackages/ue_api_collision/latest-report.json`.
Каждый запуск хранит reflection.json, inventory.json и отдельные probe logs.

Последний целевой прогон: PASS — 23 Python-теста, весь модуль, overlap/settings,
три ожидаемых отказа и совместный импорт World/Math. Сборка тестового проекта
согласована после параллельного изменения FDasDeclaredType. Коммит не создавался.

Реальная body simulation и PhysicsHandle теперь проверяются отдельно:
[PhysicsSimulation](PhysicsSimulation.md), runner Scripts/TestPhysicsSimulation.ps1.
Восемь native measurements и null receiver — PASS; 450 Chaos steps.
Это дополняет query probes, не меняя их прежнюю область проверки.
