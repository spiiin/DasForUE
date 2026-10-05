# TMap и TSet: первый срез — IR v5

Поддержаны верхнеуровневые TMap/TSet со скалярными типами: int32, float, double,
bool, FString, FName и отражённый enum, если UE предоставляет хеш для ключа/элемента.
Работают input/const-ref, return, чистые out и inout. Текущий выбор содержит
51 функцию; map/set функции представлены реальным UObject fixture API.

## Представление daScript

Для каждой комбинации типов генерируется отдельный nominal wrapper. Map содержит
entries:array<Entry>, где Entry имеет типизированные key и value. Set содержит
values:array<T>. Идентификатор зависит от kind/identity членов; одинаковые типы
в разных функциях используют один wrapper. Имена проверяются на коллизии.

```das
let self = C_DasObjectFixture_5f019a81_from(ue_self_object())
var map : H_Map_int_string_aa6f8c57
map.entries <- [
    H_Map_int_string_aa6f8c57_Entry(key=1, value="hello"),
    H_Map_int_string_aa6f8c57_Entry(key=2, value="Привет")]
let result <- fixture_TransformMap(self, map)

var set : H_Set_int_bf4eb51c
set.values <- [1, 2, 3]
let changed <- fixture_TransformSet(self, set)
```

Полный исполняемый пример: generated_hash_containers.das. Он показывает поиск
по entries, membership по values, пустые контейнеры, повторную передачу результатов,
inout/out и сохранность входа. generated_hash_numbers.das проверяет float/double.
Эти примеры требуют UDasObjectFixture. Это представление для передачи коллекций;
оно пока не предоставляет полноценный Map/Set API daScript с insert/remove/find.
Порядок элементов на выходе не гарантирован: проверки сравнивают содержимое.
При изменении открытых entries/values уникальность проверяется при передаче в UE.

Такой wrapper сохраняет отдельные UEName и enum-типы, включая ключи, и не
приравнивает контейнер к обычному array<T> или другому Map/Set. Array-данные и
wrapper с ними перемещаются через `<-`. Возвращённые строки принадлежат daScript;
изменение inout возвращает новую коллекцию и не меняет вход скрипта на месте.

## Уникальность и числовые ключи

Повторный ключ Map и повторный элемент Set отклоняются до ProcessEvent.
Последнее значение не перезаписывает предыдущую пару молча. Проверка использует
FProperty.GetValueTypeHash и Identical, включая UE-семантику FName для регистра,
None и числового суффикса. Например, Mixed_17 и mixed_17 — один FName-ключ.
Для FString регистр остаётся существенным по правилам Unreal.

Float/double ключи и элементы Set должны быть конечными. NaN/Infinity дают ошибку;
Map values этого ограничения не получают. Входные +0 и -0 нормализуются в +0 и
считаются дубликатами. Вход скрипта при этом не изменяется. Native outputs с
неконечным ключом/элементом отклоняются после выполнения UE-функции.

## Runtime и время жизни

ue_arg_hash(call, path, count) создаёт отдельные initialized staging-values.
Map slots задаются путями values.0.key/values.0.value, Set — values.0 и т. д.,
используя обычные ue_arg_int/string/name/enum/float/... API. Ключи живого UE hash
container во время заполнения не меняются. После проверки полноты и уникальности
FScriptMapHelper.AddPair/FScriptSetHelper.AddElement формируют корректный UE container.

После ProcessEvent ue_result_hash_size и обычные result-getters читают логические
слоты. Runtime строит индекс только живых элементов, учитывая sparse holes и
native удаление/добавление. Индексы — позиции передачи, а не стабильные ключи.
Сброс требует повторной записи всех slots; размер 0 задаётся явно. Проверяются
тип, направление, индексы, переполнение, фаза и released frames.

Лимит — 65 536 пар/элементов на контейнер. Слишком большой вход отвергается до
создания staging-values и вызова UFunction. Слишком большой выход выявляется
после ProcessEvent, поэтому побочные эффекты функции уже могли произойти.
При такой ошибке frame помечается failed и читать результаты нельзя, в том числе
после try/recover. Frame можно освободить и создать новый.

Staging-values и UE frame владеют своими скалярами/строками; их деструкторы
выполняются при освобождении frame или выходе из VM после ошибки. Raw-указатели
и native hash layout в скрипт не передаются. UObject/struct values этого среза
не поддерживаются, поэтому дополнительных GC-pins для Map/Set здесь нет.

## IR v5 и ограничения

Схема явно обновлена до v5; старый IR нужно перегенерировать GenerateBindings.ps1.
Map node содержит key/value, Set node — element. type_path Map состоит из
имён FProperty ключа/значения через `;`; Set использует имя element property.
В fields присутствует корень контейнера и wildcard-листья `values.*.key`,
`values.*.value` или `values.*`. Enum identity/members и metadata экспортируются
как прежде. Генератор сверяет дерево и листья, виды и marker-типы. Runtime
signature guard включает описание key/value/element свойств.

Нет Map/Set структур или UObject, контейнеров внутри структур/других контейнеров,
fixed arrays и defaults Map/Set. Hash flag Unreal обязателен; произвольные типы
с пользовательским equality/hash требуют отдельного адаптера. Политика общего
публичного UE API и AOT остаются последующими задачами.

Следующий основной этап — модель authoring DSL: классы, наследование, свойства,
функции, metadata и связь со скриптовыми реализациями. Далее синтаксические макросы
с выбранным пользователем синтаксисом, hot reload методов и структурных изменений,
AOT. Расширение Map/Set на структуры/UObject остаётся отдельным пунктом контейнеров.


## Верификация

239 foundation-проверок прошли в Editor и 239 — в packaged Win64 Development,
оба процесса завершились с кодом 0. Покрыты map/set input, return, out/inout,
Unicode, Name/enum, double precision, пустота, повторное использование результата,
native remove/add, сохранность входа, дубли, FName case equality, signed zero,
NaN key, неполные slots, индексы/переполнение/лимит, направления и released frames.
Try/recover-тест подтвердил блокировку результатов failed frame и возможность
продолжить новым вызовом. 51 Python-тест генератора прошёл, включая несогласованные
map/set markers, отсутствующие члены, неподдерживаемые виды и коллизии имён.
GenerateBindings.ps1 -Check, TestBindingsExport.ps1 и git diff --check прошли.
Регрессионный Objects PIE прошёл (1 success, 0 failed, 0 succeededWithWarnings);
map/set сценарии выполняются на UObject fixture в Editor и packaged smoke suite.
Журналы: logs/hash-*.log, logs/smoke-editor.log и logs/smoke-packaged.log.
