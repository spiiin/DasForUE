# ue_api_collision: отчёт генерации

Движок: `5.8.3-0+UE5`. Состояние: `ue_preflight_passed`.

| Класс | Готовы | Export blockers | Generation blockers | Policy exclusions |
| --- | ---: | ---: | ---: | ---: |
| GameplayStatics | 3 | 7 | 0 | 0 |
| KismetSystemLibrary | 12 | 26 | 0 | 0 |
| PhysicsConstraintComponent | 41 | 4 | 0 | 0 |
| PhysicsHandleComponent | 13 | 1 | 0 | 0 |
| PrimitiveComponent | 74 | 3 | 1 | 0 |

Всего 185 кандидатов, 185 уникальных объявлений. Construction guarded: 41.
Из 143 готовых объявлений 89 уже сгенерированы в baseline-пакетах; 54 отсутствуют в них. Это не число новых возможностей всей привязки.
Baseline: ue_api_world, ue_api_math.

## Группы блокеров

| Группа | Уникальных объявлений |
| --- | ---: |
| generation_shape | 1 |
| special_call_adapter | 1 |
| type_or_abi | 34 |
| visibility_or_deprecation_policy | 6 |

## Подсказки отражённого native-аудита

Это отдельная policy-оценка тех же объявлений. Она уточняет пути проблемных полей, но не перечисляет все будущие generic ABI blockers. Устранение одного типа не гарантирует прохождения функции.

| Тип | Уникальных объявлений с проблемой |
| --- | ---: |
| `StructProperty: FBodyInstanceAsyncPhysicsTickHandle` | 1 |
| `UInt32Property: uint32` | 1 |
| `WeakObjectProperty: TWeakObjectPtr<UObject>` | 34 |
| `WeakObjectProperty: TWeakObjectPtr<UPhysicalMaterial>` | 33 |
| `WeakObjectProperty: TWeakObjectPtr<UPrimitiveComponent>` | 33 |

Числа типов пересекаются: несколько weak-полей одного HitResult считаются одним объявлением для каждого типа.

## Поимённый backlog

| Функция | Стадия / причина | Пути проблемных полей (native policy) |
| --- | --- | --- |
| GameplayStatics.Blueprint_PredictProjectilePath_Advanced | export_blocked: Actor instance handles require the collision result adapter | `PredictResult.HitResult.PhysMaterial`, `PredictResult.HitResult.HitObjectHandle.ReferenceObject`, `PredictResult.HitResult.Component` |
| GameplayStatics.Blueprint_PredictProjectilePath_ByObjectType | export_blocked: Actor instance handles require the collision result adapter | `OutHit.PhysMaterial`, `OutHit.HitObjectHandle.ReferenceObject`, `OutHit.Component` |
| GameplayStatics.Blueprint_PredictProjectilePath_ByTraceChannel | export_blocked: Actor instance handles require the collision result adapter | `OutHit.PhysMaterial`, `OutHit.HitObjectHandle.ReferenceObject`, `OutHit.Component` |
| GameplayStatics.BreakHitResult | export_blocked: Actor instance handles require the collision result adapter | `Hit.PhysMaterial`, `Hit.HitObjectHandle.ReferenceObject`, `Hit.Component` |
| GameplayStatics.FindCollisionUV | export_blocked: Actor instance handles require the collision result adapter | `Hit.PhysMaterial`, `Hit.HitObjectHandle.ReferenceObject`, `Hit.Component` |
| GameplayStatics.GetSurfaceType | export_blocked: Actor instance handles require the collision result adapter | `Hit.PhysMaterial`, `Hit.HitObjectHandle.ReferenceObject`, `Hit.Component` |
| GameplayStatics.MakeHitResult | export_blocked: Actor instance handles require the collision result adapter | `ReturnValue.PhysMaterial`, `ReturnValue.HitObjectHandle.ReferenceObject`, `ReturnValue.Component` |
| KismetSystemLibrary.BoxTraceMulti | export_blocked: Actor instance handles require the collision result adapter | `OutHits[].PhysMaterial`, `OutHits[].HitObjectHandle.ReferenceObject`, `OutHits[].Component` |
| KismetSystemLibrary.BoxTraceMultiByProfile | export_blocked: Actor instance handles require the collision result adapter | `OutHits[].PhysMaterial`, `OutHits[].HitObjectHandle.ReferenceObject`, `OutHits[].Component` |
| KismetSystemLibrary.BoxTraceMultiForObjects | export_blocked: Actor instance handles require the collision result adapter | `OutHits[].PhysMaterial`, `OutHits[].HitObjectHandle.ReferenceObject`, `OutHits[].Component` |
| KismetSystemLibrary.BoxTraceSingle | export_blocked: Actor instance handles require the collision result adapter | `OutHit.PhysMaterial`, `OutHit.HitObjectHandle.ReferenceObject`, `OutHit.Component` |
| KismetSystemLibrary.BoxTraceSingleByProfile | export_blocked: Actor instance handles require the collision result adapter | `OutHit.PhysMaterial`, `OutHit.HitObjectHandle.ReferenceObject`, `OutHit.Component` |
| KismetSystemLibrary.BoxTraceSingleForObjects | export_blocked: Actor instance handles require the collision result adapter | `OutHit.PhysMaterial`, `OutHit.HitObjectHandle.ReferenceObject`, `OutHit.Component` |
| KismetSystemLibrary.CapsuleTraceMulti | export_blocked: Actor instance handles require the collision result adapter | `OutHits[].PhysMaterial`, `OutHits[].HitObjectHandle.ReferenceObject`, `OutHits[].Component` |
| KismetSystemLibrary.CapsuleTraceMultiByProfile | export_blocked: Actor instance handles require the collision result adapter | `OutHits[].PhysMaterial`, `OutHits[].HitObjectHandle.ReferenceObject`, `OutHits[].Component` |
| KismetSystemLibrary.CapsuleTraceMultiForObjects | export_blocked: Actor instance handles require the collision result adapter | `OutHits[].PhysMaterial`, `OutHits[].HitObjectHandle.ReferenceObject`, `OutHits[].Component` |
| KismetSystemLibrary.CapsuleTraceSingle | export_blocked: Actor instance handles require the collision result adapter | `OutHit.PhysMaterial`, `OutHit.HitObjectHandle.ReferenceObject`, `OutHit.Component` |
| KismetSystemLibrary.CapsuleTraceSingleByProfile | export_blocked: Actor instance handles require the collision result adapter | `OutHit.PhysMaterial`, `OutHit.HitObjectHandle.ReferenceObject`, `OutHit.Component` |
| KismetSystemLibrary.CapsuleTraceSingleForObjects | export_blocked: Actor instance handles require the collision result adapter | `OutHit.PhysMaterial`, `OutHit.HitObjectHandle.ReferenceObject`, `OutHit.Component` |
| KismetSystemLibrary.GetCollisionProfileNames | export_blocked: Expected loaded class and local BlueprintCallable instance function: /Script/Engine.KismetSystemLibrary.GetCollisionProfileNames | — |
| KismetSystemLibrary.LineTraceMulti | export_blocked: Actor instance handles require the collision result adapter | `OutHits[].PhysMaterial`, `OutHits[].HitObjectHandle.ReferenceObject`, `OutHits[].Component` |
| KismetSystemLibrary.LineTraceMultiByProfile | export_blocked: Actor instance handles require the collision result adapter | `OutHits[].PhysMaterial`, `OutHits[].HitObjectHandle.ReferenceObject`, `OutHits[].Component` |
| KismetSystemLibrary.LineTraceMultiForObjects | export_blocked: Actor instance handles require the collision result adapter | `OutHits[].PhysMaterial`, `OutHits[].HitObjectHandle.ReferenceObject`, `OutHits[].Component` |
| KismetSystemLibrary.LineTraceSingle | export_blocked: Actor instance handles require the collision result adapter | `OutHit.PhysMaterial`, `OutHit.HitObjectHandle.ReferenceObject`, `OutHit.Component` |
| KismetSystemLibrary.LineTraceSingleByProfile | export_blocked: Actor instance handles require the collision result adapter | `OutHit.PhysMaterial`, `OutHit.HitObjectHandle.ReferenceObject`, `OutHit.Component` |
| KismetSystemLibrary.LineTraceSingleForObjects | export_blocked: Actor instance handles require the collision result adapter | `OutHit.PhysMaterial`, `OutHit.HitObjectHandle.ReferenceObject`, `OutHit.Component` |
| KismetSystemLibrary.SetCollisionProfileNameProperty | export_blocked: requires_special_call_adapter | — |
| KismetSystemLibrary.SphereTraceMulti | export_blocked: Actor instance handles require the collision result adapter | `OutHits[].PhysMaterial`, `OutHits[].HitObjectHandle.ReferenceObject`, `OutHits[].Component` |
| KismetSystemLibrary.SphereTraceMultiByProfile | export_blocked: Actor instance handles require the collision result adapter | `OutHits[].PhysMaterial`, `OutHits[].HitObjectHandle.ReferenceObject`, `OutHits[].Component` |
| KismetSystemLibrary.SphereTraceMultiForObjects | export_blocked: Actor instance handles require the collision result adapter | `OutHits[].PhysMaterial`, `OutHits[].HitObjectHandle.ReferenceObject`, `OutHits[].Component` |
| KismetSystemLibrary.SphereTraceSingle | export_blocked: Actor instance handles require the collision result adapter | `OutHit.PhysMaterial`, `OutHit.HitObjectHandle.ReferenceObject`, `OutHit.Component` |
| KismetSystemLibrary.SphereTraceSingleByProfile | export_blocked: Actor instance handles require the collision result adapter | `OutHit.PhysMaterial`, `OutHit.HitObjectHandle.ReferenceObject`, `OutHit.Component` |
| KismetSystemLibrary.SphereTraceSingleForObjects | export_blocked: Actor instance handles require the collision result adapter | `OutHit.PhysMaterial`, `OutHit.HitObjectHandle.ReferenceObject`, `OutHit.Component` |
| PhysicsConstraintComponent.GetConstraint | export_blocked: unsupported UFunction or struct field type | `ReturnValue.Owner`, `ReturnValue.Index` |
| PhysicsConstraintComponent.ReInitConstraintAndApplyPhysicsState | export_blocked: Expected loaded class and local BlueprintCallable instance function: /Script/Engine.PhysicsConstraintComponent.ReInitConstraintAndApplyPhysicsState | — |
| PhysicsConstraintComponent.SetAngularOrientationDrive | export_blocked: deprecated_function | — |
| PhysicsConstraintComponent.SetAngularVelocityDrive | export_blocked: deprecated_function | — |
| PhysicsHandleComponent.GrabComponent | export_blocked: deprecated_function | — |
| PrimitiveComponent.GetBodyInstanceAsyncPhysicsTickHandle | generation_blocked: Opaque/empty structs need an adapter | `ReturnValue` |
| PrimitiveComponent.HandlePostMigrationPhysicsStateChanged | export_blocked: Expected loaded class and local BlueprintCallable instance function: /Script/Engine.PrimitiveComponent.HandlePostMigrationPhysicsStateChanged | — |
| PrimitiveComponent.K2_BoxOverlapComponent | export_blocked: Actor instance handles require the collision result adapter | `OutHit.PhysMaterial`, `OutHit.HitObjectHandle.ReferenceObject`, `OutHit.Component` |
| PrimitiveComponent.K2_SphereOverlapComponent | export_blocked: Actor instance handles require the collision result adapter | `OutHit.PhysMaterial`, `OutHit.HitObjectHandle.ReferenceObject`, `OutHit.Component` |
