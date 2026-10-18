# Object/class arrays внутри структур

Generic generator поддерживает strong UObject references и UClass/TSubclassOf
arrays в полях отражённых структур, включая struct внутри struct и массив
структур с собственными массивами ссылок. Тот же backend используется для
параметров/результатов UFunction; отдельных ручных игровых wrappers не добавлено.

Проверка array storage общая для top-level параметров и struct members:
ObjectProperty/ClassProperty не взаимозаменяемы, неизвестные классы/метаклассы
отклоняются. WeakObjectProperty и marker weak_reference=true не могут незаметно
стать strong arrays. Поддержка прямых weak reference containers не расширялась.

Native frame проверяет класс объекта, MetaClass класса, размеры/индексы,
направление и заполнение каждого элемента. null и дубликаты допустимы.
Сохраняется обычное strong reference поведение native массивов на время вызова.
Возвращённые DAS object views остаются checked handles; результаты содержат
собственные DAS-массивы и не ссылаются на память освобождённого call frame.

## Advanced projectile prediction

В FPredictProjectilePathParams поле ActorsToIgnore больше не блокирует
Blueprint_PredictProjectilePath_Advanced. Generated call передаёт всю структуру,
возвращает FPredictProjectilePathResult с массивом PathData и opaque UEHitResult.
Чтение попадания сохраняет transport/lifetime contract из [HitResultTransport](HitResultTransport.md).

Collision profile: **176/185** ready wrappers, **8 export + 1 generation** blockers.
Все **33/33** выбранных объявлений с HitResult проходят генерацию и компиляцию
всего модуля. Это не утверждение, что выполнена каждая из 176 функций.
Оставшиеся blockers: constraint handle, opaque async physics handle,
CustomThunk setter и шесть deprecated/служебных методов.

## Целевые проверки

```powershell
.\Scripts\TestStructReferenceArrays.ps1 -SkipBuild
```

Без SkipBuild собирается Editor target с fixture. Девять тестов reference-array
генерации + 96 базовых + восемь HitResult — **113 PASS**. Вся генерация Collision,
общая компиляция DAS-модуля и его основной probe — PASS.

Advanced runtime probe: hit, ActorsToIgnore, clear/null/duplicates, spatial miss,
PathData и вложенный HitResult. Отдельная reflected fixture проверяет возврат
object/class arrays, Actor/Pawn MetaClass, null, дубликаты, пустые массивы,
struct arrays с вложенными reference arrays и независимость результата от
изменений входа. Wrong object, wrong MetaClass и отсутствующий элемент
отклоняются. Проверены неизменные SHA256 Runtime/Editor DLL во время проверки.

Probes: `Tests/ProjectileAdvanced/probe.template.das`,
`Tests/StructReferenceArrays/probe.template.das`.
Отчёты: `logs/projectile-advanced-validation.json`,
`logs/struct-reference-arrays-validation.json`.
Generated modules/IR и runtime logs остаются в Build/ApiPackages/ue_api_collision.
Пакет staged; публикация, full Foundation/PIE/packaged и новый коммит не выполнялись.
