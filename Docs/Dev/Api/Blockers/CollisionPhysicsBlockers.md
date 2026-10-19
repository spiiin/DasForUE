# ue_api_collision: отчёт генерации

Движок: `5.8.3-0+UE5`. Состояние: `published`.

| Класс | Готовы | Export blockers | Generation blockers | Policy exclusions |
| --- | ---: | ---: | ---: | ---: |
| ConstraintInstanceBlueprintLibrary | 56 | 0 | 0 | 0 |
| GameplayStatics | 10 | 0 | 0 | 0 |
| KismetSystemLibrary | 37 | 0 | 0 | 1 |
| PhysicsConstraintComponent | 42 | 0 | 0 | 3 |
| PhysicsHandleComponent | 13 | 0 | 0 | 1 |
| PrimitiveComponent | 76 | 0 | 0 | 2 |

Всего 241 кандидатов, 241 уникальных объявлений. Construction guarded: 44.
Из 234 готовых объявлений 89 уже сгенерированы в baseline-пакетах; 145 отсутствуют в них. Это не число новых возможностей всей привязки.
Baseline: ue_api_world, ue_api_math.

## Группы блокеров

| Группа | Уникальных объявлений |
| --- | ---: |

## Подсказки отражённого native-аудита

Это отдельная policy-оценка тех же объявлений. Она уточняет пути проблемных полей, но не перечисляет все будущие generic ABI blockers. Устранение одного типа не гарантирует прохождения функции.

| Тип | Уникальных объявлений с проблемой |
| --- | ---: |
| `StructProperty: FBodyInstanceAsyncPhysicsTickHandle` | 1 |

Числа типов пересекаются: несколько weak-полей одного HitResult считаются одним объявлением для каждого типа.

## Поимённый backlog

| Функция | Стадия / причина | Пути проблемных полей (native policy) |
| --- | --- | --- |
| — | Активных технических блокеров в выбранном профиле нет | — |

## Документированные исключения

| Функция | Причина политики | Альтернатива | Исходный export diagnostic |
| --- | --- | --- | --- |
| KismetSystemLibrary.GetCollisionProfileNames | not_blueprint_callable: editor customization helper | Use collision profile names configured in Project Settings; not an ordinary reflected call. | Expected loaded class and local BlueprintCallable instance function: /Script/Engine.KismetSystemLibrary.GetCollisionProfileNames |
| PhysicsConstraintComponent.ReInitConstraintAndApplyPhysicsState | not_blueprint_callable: engine physics-state maintenance | Use the public constraint setup and parameter API. | Expected loaded class and local BlueprintCallable instance function: /Script/Engine.PhysicsConstraintComponent.ReInitConstraintAndApplyPhysicsState |
| PhysicsConstraintComponent.SetAngularOrientationDrive | deprecated_function | SetOrientationDriveTwistAndSwing / SetOrientationDriveSLERP | deprecated_function |
| PhysicsConstraintComponent.SetAngularVelocityDrive | deprecated_function | SetAngularVelocityDriveTwistAndSwing / SetAngularVelocityDriveSLERP | deprecated_function |
| PhysicsHandleComponent.GrabComponent | deprecated_function | GrabComponentAtLocation / GrabComponentAtLocationWithRotation | deprecated_function |
| PrimitiveComponent.GetBodyInstanceAsyncPhysicsTickHandle | raw_async_handle_forbidden: scoped execution capability required | ue_async_physics + ue_physics_body + ue_physics_add_force; Docs/AsyncPhysicsDSL.md | async physics handles require a scoped frozen-game-thread physics callback adapter  |
| PrimitiveComponent.HandlePostMigrationPhysicsStateChanged | not_blueprint_callable: engine migration hook | Engine-managed lifecycle; no public scripting equivalent required. | Expected loaded class and local BlueprintCallable instance function: /Script/Engine.PrimitiveComponent.HandlePostMigrationPhysicsStateChanged |

## Подключение и зависимости

```dascript
require ue_api_collision
```

daScript: ue_foundation. UE modules: Engine.

Готовность генерации не означает проверку каждой функции во всех режимах; packaged/Shipping имеют отдельную проверку.
