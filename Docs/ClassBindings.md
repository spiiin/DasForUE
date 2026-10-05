# Типизированные ссылки UCLASS — IR v3

Генератор создаёт отдельный nominal struct daScript для каждого класса receiver,
объектного параметра/результата и их предков. IR содержит массив classes:
type_path — полный путь UCLASS, parent — путь родителя или пустая строка.
Текущий выбор: 18 функций, 3 класса (Object, Actor, DasObjectFixture), 4 value-структуры.
Это привязки существующих классов Unreal; authoring DSL пользовательских классов
и его синтаксис этим этапом не определяются.

## Пример Actor

```das
options gen2
require ue_generated
require ue_foundation
[export]
def main(input:int):int {
    let actor = C_Actor_adf65d5b_from(ue_self_object())
    let owner = actor_GetOwner(actor) // также C_Actor_adf65d5b
    if (C_Actor_adf65d5b_valid(owner)) {
        let position = actor_K2_GetActorLocation(owner)
        return int(position.f_x)
    }
    return 0
}
```

## Преобразования и время жизни

Для каждого C_<имя>_<hash> генерируются функции:

- `_from(UObjectHandle)` — проверенное преобразование, допускающее null.
- `_raw(value)` — переход к низкоуровневому handle с повторной проверкой класса.
- `_null()` — безопасная пустая ссылка, в том числе для глобальной переменной.
- `_valid(value)` — false для null, уничтоженного объекта или неверного класса.
- `_as_<тип предка>(value)` — явное преобразование к каждому предку из IR.

Например, `C_Actor_adf65d5b_as_C_Object_55c3753a(actor)` возвращает Object.
Обратное преобразование: `C_Actor_adf65d5b_from(C_Object_55c3753a_raw(object))`.
Автоматического приведения между классами нет. Getter с объявленным результатом
UObject возвращает Object, даже если фактический объект — Actor: нужен явный cast.
Несовместимые receiver/аргументы отклоняются компилятором daScript.

Ссылка содержит слабый context-local token. Она не владеет UObject и не защищает
его от GC. Null разрешён в объектных аргументах и результатах, но не в receiver.
Истёкший ненулевой token вызывает ошибку при преобразовании или вызове и никогда
не превращается молча в null. Call frame удерживает объектные параметры и результаты
только до освобождения frame; возвращённая обёртка остаётся слабой.
Для передачи объекта через несколько callback его должен удерживать сам Unreal.

Поле `handle` открыто: struct можно собрать вручную. Это не граница безопасности.
Каждая сгенерированная обёртка повторно проверяет receiver и объектные аргументы;
проверки marshalling и актуальности сигнатуры также сохранены.
Низкоуровневые функции ue_foundation по-прежнему принимают UObjectHandle.

## Миграция и примеры

После перехода с IR v2 нужно запустить Scripts/GenerateBindings.ps1 и явно
преобразовать прежний receiver, например `C_Actor_adf65d5b_from(ue_self_object())`.
Объектные аргументы/выходы теперь тоже имеют конкретный тип класса.
generated_actor.das и generated_calls.das обновлены. class_roundtrip.das показывает
upcast/downcast, nullable input/result и object out/inout. class_persistent.das
демонстрирует слабую глобальную ссылку; class_weak_gc.das проверяет сборку объекта.
Эти примеры с fixture требуют тестового UDasObjectFixture и не запускаются на Actor.
class_bad_*, class_forged_receiver, class_null_receiver, class_raw_receiver и
class_expired_cast — намеренно ошибочные тесты.

## Границы и следующий шаг

Поддерживаются только загруженные классы из явного выбора API; зависимости-предки
экспортируются автоматически. Нет загрузки assets, class/TSubclassOf/soft/interface
адаптеров, сильного владения из скрипта, объектных полей value-структур или AOT.
Генератор проверяет полноту графа классов, циклы, дубликаты и коллизии имён.
Следующий срез: политика выбора reflected API и метаданных; затем defaults,
расширение охвата типов и AOT. Authoring DSL и hot reload остаются в плане.


## Проверки

172 foundation-проверки прошли в Editor и packaged Win64 Development (код 0).
Покрыты несовместимые статические типы, raw receiver, поддельная обёртка,
null receiver, nullable параметры/результаты, upcast/downcast, object out/inout,
истечение ссылок после GC внутри вызова и между вызовами persistent context.
Objects PIE прошёл: одна automation-проверка, 0 ошибок/тестов с предупреждениями;
сгенерированные Actor-обёртки исполняются на настоящем игровом Actor.
20 Python-тестов генератора прошли. GenerateBindings.ps1 -Check подтвердил
актуальность IR и кода; TestBindingsExport.ps1 — сохранность предыдущего файла
при отказе экспорта. git diff --check прошёл.

Команды: Scripts/TestEditor.ps1, TestObjectsPIE.ps1, TestPackaged.ps1,
GenerateBindings.ps1 -Check, TestBindingsGenerator.py, TestBindingsExport.ps1.
Журналы этапа: logs/classes-*.log; полные smoke logs: logs/smoke-editor.log
и logs/smoke-packaged.log; PIE report: logs/objects-pie-report/index.json.
