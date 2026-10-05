# Именованные структуры в IR и daScript

IR обновлён до schema_version=2. Общие описания UScriptStruct находятся в `structs`,
а `parameters` каждой функции сохраняют исходные параметры верхнего уровня:
тип, направление и ссылку на определение структуры. Поля `fields` с развёрнутыми
скалярными путями сохранены для проверки согласованности с runtime-маршаллингом.
Генератор сравнивает оба представления и отклоняет расхождения.

Определение структуры содержит полный type_path, cpp_type и её именованные поля.
Один UE-тип объявляется в daScript один раз; вложенные типы выводятся раньше
использующих их структур. Имя имеет форму S_<UEName>_<hash>, где hash получен из
полного пути типа. У полей префикс f_, чтобы избежать конфликтов с ключевыми
словами daScript (например, label). Идентичность определяется типом, не набором полей.

## FVector целиком

```das
options gen2
require ue_generated
require ue_foundation

[export]
def main(input:int):int {
    let actor = ue_self_object()
    let scale = S_Vector_4bd4f956(
        f_x = double(2), f_y = double(3), f_z = double(4))
    actor_SetActorRelativeScale3D(actor, scale)
    let observed = actor_GetActorRelativeScale3D(actor)
    verify(observed.f_x == scale.f_x)
    let location = actor_K2_GetActorLocation(actor)
    return int(location.f_x)
}
```

S_Vector_4bd4f956 соответствует `/Script/CoreUObject.Vector` (FVector данной
конфигурации UE, с double-полями). Это значение daScript; адрес памяти UE в него
не попадает. Обёртка сама записывает/читает поля через проверенный call frame.

## Вложенные структуры и inout

```das
let source = S_DasCallPayload_a9a75418(
    f_metrics = S_DasCallMetrics_8fde8e48(f_ratio = 1.5),
    f_count = 2, f_label = "data", f_benabled = false, f_weight = double(2.5))
let transformed = fixture_TransformPayload(self, source, 3, false, "!")
verify(transformed.f_count == 5)
verify(transformed.f_metrics.f_ratio == 4.5)

let changed = fixture_MutatePayload(self, source)
verify(changed.out_payload.f_count == 6)
verify(changed.out_copy.f_label == "data!")
verify(source.f_count == 2 && source.f_label == "data")
```

Последний пример требует UDasObjectFixture, не обычного Actor. Inout возвращается
в result как новое значение; аргумент скрипта на месте не изменяется. Несколько
выходных параметров дают `<alias>_Result`, где каждое поле соответствует одному
целому параметру. Один выход, включая struct return, возвращается непосредственно.
Результат можно передать в следующую обёртку после освобождения первого frame.
Строки и FName внутри результатов копируются в память daScript; enum остаются
отдельными сгенерированными типами. Вложенность сохраняется.

## Миграция с первого генератора

Запустить `Scripts/GenerateBindings.ps1` и обновить вызывающий код. Версия IR 1
намеренно отклоняется новым генератором. Низкоуровневые ue_arg_*/ue_result_* не
меняются; меняются сигнатуры сгенерированных обёрток со структурами:

- `actor_K2_GetActorLocation(...).out_returnvalue_x` → `... .f_x`.
- Несколько скалярных аргументов Payload → один S_DasCallPayload_a9a75418.
- `out_copy_mode` → `out_copy.f_mode` для структурного выходного параметра.

Готовые примеры generated_calls.das и generated_actor.das уже переведены.
Selection.json теперь содержит 14 функций: добавлены методы чтения/записи масштаба
Actor и MutatePayload для проверки структурного inout.

## Границы

Это структуры-значения для передачи в функции, без методов C++/операторов,
наследования или размещения в памяти Unreal. Значения по умолчанию относятся к
daScript; C++ constructors/defaults UScriptStruct не экспортируются. Для важных
полей следует явно задавать значения. Optional/default arguments функций также
ещё не генерируются.

Не поддержаны объектные поля структур, контейнеры, непрозрачное нативное состояние,
пустые структуры без отражённых полей и рекурсивные графы значений. Генератор
проверяет отсутствующие/дублирующиеся типы, циклы, глубину вложенности, коллизии
полей и согласованность параметров с проверенными скалярными листьями.
Class-specific UObject-типы, AOT и синтаксис пользовательского DSL не добавлялись.

## Верификация

161 foundation-проверка прошла в Editor и packaged Win64 Development; оба процесса
завершились с кодом 0. Проверены вложенные строки/enum/FName, целый struct return,
struct out/inout, сохранность входа и повторная передача результата в следующий вызов.
Компилятор отклонил чужой тип, пользовательский FakeVector с такими же полями и старый
вызов с развёрнутыми аргументами.

Objects PIE: 17 контрольных точек, одна успешная automation-проверка без warnings.
Пример generated_actor.das записал и прочитал масштаб настоящего Actor как FVector
и получил позицию целой структурой. Прежние сценарии объектных ссылок/событий/GC прошли.
Все 15 Python-тестов генератора прошли; GenerateBindings.ps1 -Check подтвердил
актуальность IR и кода. TestBindingsExport.ps1 подтвердил сохранность старого файла
при неподдерживаемой сигнатуре. git diff --check прошёл.

Команды: GenerateBindings.ps1 -Check, TestBindingsGenerator.py,
TestBindingsExport.ps1, TestEditor.ps1, TestObjectsPIE.ps1, TestPackaged.ps1.
Журналы: C:\src\DasForUE2\logs\structs-*-console.log.

Типизированные UObject-ссылки реализованы следующим этапом: ClassBindings.md.
Метаданные и расширение выбранного API остаются в плане.
