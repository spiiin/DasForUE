# Массивы value-структур, enum и UObject — IR v4

TArray расширен на отражённые value-структуры, enum и типизированные UObject-ссылки.
Поддерживаются const/input, return, чистый out и inout. Результаты остаются
array<T> daScript и перемещаются через `<-`; вход скрипта не изменяется на месте.
Текущий выбор содержит 36 функций, включая настоящий Actor.GetAttachedActors.

```das
let self = C_DasObjectFixture_5f019a81_from(ue_self_object())
let payload = S_DasCallPayload_a9a75418(
    f_metrics = S_DasCallMetrics_8fde8e48(f_ratio = 1.5),
    f_count = 2, f_label = "Привет", f_benabled = true, f_weight = 1.25lf)
let input <- [payload]
let result <- fixture_TransformPayloads(self, input)
verify(result[0].f_count == 6 && input[0].f_count == 2)

let objects <- fixture_MakeFixtureObjects(self)
verify(C_DasObjectFixture_5f019a81_valid(objects[0]))
```

Это fixture-пример. Для настоящего Actor:

```das
let actor = C_Actor_adf65d5b_from(ue_self_object())
let children <- actor_GetAttachedActors(actor)
for (child in children) {
    if (C_Actor_adf65d5b_valid(child)) {
        let position = actor_K2_GetActorLocation(child)
    }
}
```

Полные проверяемые сценарии: generated_complex_arrays.das, array_gc_*.das и
generated_actor.das в Content/Das/Scripts плагина. generated_actor.das предназначен
для Objects PIE fixture: ожидает один прикреплённый Actor, owner у ребёнка и тег
DasEnumTag_17. Отрицательные generated_complex_arrays_bad_*, forged/expired
и array_gc_exception_fixture специально проверяют отказ/очистку.

## Структуры и проверка типов

Поля структур рекурсивно записываются и читаются по пути values.0.metrics.ratio.
Поддержаны вложенные value-структуры, строки, FName, скаляры и enum. Каждый входной
лист каждого элемента должен быть записан после последнего ue_arg_array: неполный
ввод отклоняется до ProcessEvent. Повторное использование результата и пустые
массивы сохраняют прежнюю семантику. Компилятор daScript отклоняет массив другого
nominal struct-типа, даже если его поля похожи.

Для object-массива тип элемента соответствует объявленному UCLASS, например
array<C_Actor_adf65d5b>. Между массивами Actor/Fixture/Object нет неявного приведения:
элементы нужно преобразовать явно. Runtime повторно проверяет класс и время жизни
каждого элемента. Null разрешён; истёкший ненулевой handle вызывает ошибку.
Открытое поле handle у wrapper не позволяет обойти эти проверки.

## Владение и GC

Каждый переданный UObject удерживается отдельным strong pin в call frame до его
освобождения. Это защищает входные элементы, если UE-функция вызывает GC.
После ProcessEvent все object return/out/inout элементы получают pins до возврата
в VM. Поэтому скрипт может вызвать GC до чтения результатов из raw frame.

Сброс входного массива освобождает его старые pins. Замена inout сначала создаёт
pins новых выходов, затем освобождает прежние ссылки, включая элементы, удалённые
из массива. Освобождение frame, выход из callback и ошибка VM освобождают все pins.
Новые UObject, создаваемые внутри UE-функции, должны пережить её собственное
выполнение по обычным правилам UE; плагин начинает удерживать выходы после
возврата из ProcessEvent.

Возвращённые значения daScript содержат только слабые context-local handles.
Ни array<T>, ни глобальная переменная persistent-контекста не владеют UObject.
После освобождения frame объект может быть собран GC, если сам Unreal его не
удерживает. Методы `_valid` возвращают false, повторная передача истёкшего элемента
отклоняется. Pin также не отменяет Destroy у Actor: проверки живости сохраняются.

## IR v4

Изменение схемы явно версионировано. Генератор теперь требует schema_version=4;
для старого IR нужно выполнить Scripts/GenerateBindings.ps1.
В parameters[].element сохраняется полный тип элемента и identity структуры,
класса или enum. В fields остаётся корневой узел массива, к нему добавлены
проверенные шаблоны листьев с `*` вместо динамического индекса:

```json
[
  {"path":"values", "kind":"array", "type_path":"StructProperty", "input":true, "output":false},
  {"path":"values.*.metrics.ratio", "kind":"float", "type_path":"", "input":true, "output":false},
  {"path":"values.*.count", "kind":"int", "type_path":"", "input":true, "output":false}
]
```

Пример сокращён. Struct definitions остаются общими и содержат все остальные поля.
Enum-листья сохраняют type_path и members; object-листья — class type_path,
классы и их предки добавляются автоматически. Генератор сверяет дерево типов
с полным набором листьев; wildcard не является доступным индексом raw API.
Runtime signature guard уже включает inner property и поля структуры/identity.

## Границы и дальше

Массивы внутри структур, вложенные массивы, поля UObject внутри value-структур,
opaque structs с собственными native GC-references, fixed arrays, TSubclassOf,
interface/soft/weak property adapters и defaults массивов остаются отдельными
задачами. Поддержка direct UObject-элемента не означает поддержку object-поля
внутри struct. Лимит прежний: 65 536 элементов на массив; слишком большой выход
обнаруживается после выполнения UE-функции и не откатывает её побочные эффекты.

Следующий этап по порядку: TMap/TSet поддерживаемых значений. Затем модель
authoring DSL и макросы с выбранным пользователем синтаксисом; hot reload методов,
затем состава классов с переносом состояния; AOT и production packaging.
Модель DSL можно поставить перед оставшимися контейнерами как отдельный приоритет.


## Верификация

212 foundation-проверок прошли в Editor и 212 — в packaged Win64 Development;
оба процесса завершились с кодом 0. Покрыты вложенные value-структуры и Unicode,
enum/FName, типовая безопасность, null/истёкшие UObject, input GC во время
ProcessEvent, удержание output до чтения, сброс/замена inout-pins, освобождение
после ошибки VM и слабые массивы в глобальных переменных persistent-контекста.
44 Python-теста генератора и 9 интеграционных сценариев политики экспорта прошли.
GenerateBindings.ps1 -Check подтвердил актуальность IR v4/кода; прежний отказ
экспортёра с сохранением предыдущего файла прошёл. git diff --check прошёл.
Objects PIE: 18 контрольных точек, 1 успешный automation-тест, 0 failed и
0 succeededWithWarnings. GetAttachedActors вернул настоящего прикреплённого Actor;
его типизированная ссылка использована в дальнейших вызовах GetOwner/ActorHasTag.
Журналы: logs/complex-arrays-*.log, logs/smoke-editor.log,
logs/smoke-packaged.log и logs/objects-pie-report/index.json.
