# Collision / Physics: weak references и результаты запросов

Первый срез добавляет native weak-хранилище в общий backend и расширяет
проверяемые результаты столкновений. Это ещё не полный пакет Physics API.

## Weak UObject properties

`FWeakObjectProperty` использует тот же номинальный тип объектной ссылки,
что strong-поле такого же native-класса. Для DAS не требуется отдельная
обёртка только потому, что native-поле объявлено как `TWeakObjectPtr<T>`.
Чтение/запись идёт через FWeakObjectProperty, с проверкой класса.
Просроченная ссылка и ссылка на уничтожаемый Actor/Component читаются как null.

`FDasDeclaredType.WeakReference` хранит физический вид свойства, как EnumStorage:
это не новый пользовательский DSL-синтаксис. Фабрика восстанавливает именно
weak-свойство; StorageIdentity различает weak/strong, Identity оставляет общий
номинальный объектный тип. Reflection IR содержит необязательный boolean
`weak_reference=true` у object-поля. Оба генератора проверяют этот marker.

Call frames и snapshot marshalling не добавляют strong pins для weak-листьев.
Поэтому удержание frame/снимка не продлевает жизнь объекта. Это относится и
к reflected struct с weak-полями. Корректность проверяется настоящим UE GC
внутри native-вызова, пока frame остаётся живым.

Прямые `array/set/map` с weak-элементами/значениями пока отклоняются:
существующие object carriers хранят strong-ссылки. Они не должны незаметно
менять native storage. Отдельный carrier для таких контейнеров — следующий
типовой шаг; массивы struct с собственными weak-полями используют storage struct.

## Hit result

`UETraceHit`, возвращаемый существующими `ue_line_trace` и `ue_sphere_trace`,
дополнен данными FHitResult:

- `trace_start`, `trace_end`, `penetration_depth`;
- `item`, `my_item`, `element_index`;
- `ue_hit_bone_name(hit)` и `ue_hit_my_bone_name(hit)` — UENameValue;
- `ue_hit_physical_material(hit)` — nullable PhysicalMaterialRef;
- `ue_hit_physics_owner(hit)` — nullable Object.

Существующие blocking_hit/start_penetrating, distance/time, location/impact_point,
normal/impact_normal, face_index, ue_hit_actor и ue_hit_component сохранены.
Query params теперь запрашивают physical material. Возвращение материала
зависит от коллайдера и типа запроса; отсутствие представлено null.

```das
let hit=ue_sphere_trace(start,finish,20.0lf)
if(hit.blocking_hit) {
    let actor=ue_hit_actor(hit)
    let material=ue_hit_physical_material(hit)
    let bone=ue_hit_bone_name(hit)
    print("penetration={hit.penetration_depth}, item={hit.item}\n")
}
```

## Overlap result

```das
require ue_gameplay_services

let results <- ue_sphere_overlap(center,75.0lf)
for(result in results) {
    let actor=ue_overlap_actor(result)
    let component=ue_overlap_component(result)
    let physics_owner=ue_overlap_physics_owner(result)
    print("blocking={result.blocking_hit}, item={result.item_index}\n")
}
```

UEOverlapResult содержит blocking_hit, item_index через native GetItemIndex(),
my_item_index и checked getters для Actor/Component/PhysicsObjectOwner.
Есть перегрузка с `var ignored:array<ActorRef>`, TraceChannel и ignore_self.
Проверяются игровой мир, фаза lifecycle, позиция/радиус/канал, принадлежность
ignored Actors миру, количество входных и выходных объектов (до 4096).
Это channel-based overlap, не object-type/profile query и не deduplicated actors.

## Lifetime и границы

Числовые поля — копии. Объектные getters проверяют scope результата и возвращают
null после уничтожения; внутренние токены не раскрываются. Доступ к actor
instance handle использует GetCachedActor: чтение не должно создавать или
подгружать dehydrated Actor. В таком случае actor может быть null при живом component.

Сырые Chaos physics handles и private FActorInstanceHandle state в DAS не
передаются. Нельзя безопасно создать эти структуры только из reflected полей:
автоматический flatten ActorInstanceHandle остаётся явно запрещённым.
HitResult/OverlapResult здесь представлены curated value views, а не полными
авторскими DAS records. Подключение этих native structs к общему пакетному
генератору trace/sweep signatures требует следующего отдельного adapter этапа.

## Точечная проверка

```powershell
./Scripts/TestCollisionFoundation.ps1
# При уже собранном актуальном editor target:
./Scripts/TestCollisionFoundation.ps1 -SkipBuild
```

Шесть generator cases, native descriptor/factory/storage identity, weak get/set,
реальный GC, отказ неправильного класса и неподдержанного weak-container,
generated weak struct call frame, настоящие line/sphere trace и sphere overlap,
глубина проникновения, неверный радиус, scope и ссылки после Destroy.
Runtime выполняется в transient Game world через commandlet с NullRHI.
Foundation/full PIE/packaged и визуальные проверки этим скриптом не запускаются.
Отчёт: logs/collision-foundation-validation.json; логи с тем же префиксом.

## Результат 2026-10-18

Editor Development — PASS. Шесть generator cases и семь runtime-групп — PASS.
Отдельно подтверждён реальный GC при удерживаемом выходном frame: weak output
не удерживает объект и читается как null после GC. Проверены настоящие trace,
ненулевая penetration depth, overlap identities и пустые ссылки после Destroy.
Экспорт сохранил weak marker; DLL не менялась во время проверки. В финальном
runtime log нет ошибок/ensure. Полные Foundation/PIE/packaged не запускались.
Проверенная версия сохранена в Git.


## Подключение generated API

Следующий этап [HitResult transport](HitResultTransport.md) выполнен: native
FHitResult переносится как opaque UEHitResult через checked call frames,
включая arrays/ref/out/nested. Поля ActorInstanceHandle всё ещё не flatten'ятся;
вместо этого сохраняется полная native copy и используется GetCachedActor view.
