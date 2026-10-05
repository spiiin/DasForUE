# Enum и FName в общем UFunction API

Поддержаны FEnumProperty, FByteProperty с UEnum (включая TEnumAsByte) и FNameProperty.
Они работают как input, return, out/inout и как поля поддерживаемых вложенных структур.
IR теперь хранит путь enum-типа для каждого такого скалярного листа.

## Enum

```das
let c = ue_begin_call(ue_self_object(), "SetTickGroup")
ue_arg_enum(c, "NewTickGroup", "/Script/Engine.ETickingGroup", "TG_PostPhysics")
ue_invoke_call(c)
ue_release_call(c)
```

Это пример для Actor: он действительно меняет tick group на PostPhysics.
Для чтения используется `ue_result_enum(call, path, enum_type_path)`, возвращающий
имя элемента как строку. Type path должен соответствовать UEnum конкретного поля:
совпадения числового значения или короткого имени другого enum недостаточно.

Имена элементов сравниваются без учёта регистра с зарегистрированными короткими
либо полными символами. DisplayName, локализованные подписи, числовые строки,
переадресации старых имён и произвольные комбинации bitflags не интерпретируются.
Для bitflags можно передать отдельный явно объявленный элемент; синтез масок пока
не поддержан. При числовых alias результат нормализуется к первому имени значения.

Символы с окончанием `_MAX` зарезервированы и отклоняются на входе и выходе.
Это единая политика Editor/package, не зависящая от editor-only Hidden metadata.
Прочие Hidden-элементы специально не фильтруются: политика экспорта метаданных ещё
не реализована. Слишком длинные символы отклоняются до операций с таблицей FName.

`ue_arg_int` не заменяет enum setter. Неизвестный элемент или чужой тип отклоняются
до ProcessEvent. Если сама UE-функция вернула необъявленное значение, ошибка возникает
при чтении результата; выполненные действия функции не откатываются.

## FName

`ue_arg_name(call, path, text)` создаёт FName в памяти параметров, а
`ue_result_name(call, path)` возвращает копию FName::ToString() в памяти daScript.
Возвращённая строка живёт независимо от освобождения frame.

Пустая строка и `None` означают NAME_None. Числовые суффиксы и Unicode обрабатывает
Unreal; равенство имён не зависит от регистра. Текстовый регистр результата может
зависеть от таблицы имён и конфигурации сборки, поэтому исходное написание не является
контрактом. Имена длиной NAME_SIZE и более (в TCHAR) отклоняются до создания FName.
FName интернируется Unreal и предназначен для идентификаторов, а не потока уникальных
пользовательских текстов. FString и FName остаются разными типами параметров:
ue_arg_string не записывает FName и наоборот.

```das
let c = ue_begin_call(ue_self_object(), "ActorHasTag")
ue_arg_name(c, "Tag", "QuestTarget_17")
ue_invoke_call(c)
let found = ue_result_bool(c, "ReturnValue")
ue_release_call(c)
```

## Пример и проверки

`Plugins/DasForUE/Content/Das/Scripts/enum_names.das` показывает enum/FName return,
out/inout и чтение строк после release. Он рассчитан на тестовый UDasObjectFixture;
примеры выше используют обычные функции Actor.

152 foundation-проверки прошли в Editor и packaged Win64 Development, оба процесса
завершились с кодом 0. Проверены enum class, legacy TEnumAsByte, разреженные значения,
enum/FName в структурах, return/out/inout, None, Unicode, регистр, числовой суффикс,
неверные типы/элементы/направления, зарезервированный _MAX, слишком длинный ввод
и необъявленное значение, возвращённое UE. Пример enum_names.das запущен в обоих
вариантах, строки проверены после освобождения frame.

Objects PIE: 16 контрольных точек; одна успешная automation-проверка без warnings.
Проверены настоящий SetTickGroup, ActorHasTag и FName input/output в созданной
Blueprint-функции, а также прежние вызовы, объектные ссылки, подписки и Destroy/GC.
Команды: Scripts/TestEditor.ps1, TestObjectsPIE.ps1, TestPackaged.ps1.
Журналы: C:\src\DasForUE2\logs\enum-name-*-console.log.
Проверка git diff --check прошла.

## Следующие работы

Проверка типов enum выполняется в runtime; низкоуровневый API сохраняется. Для явно выбранных функций уже добавлены
экспорт IR, enum daScript и генератор типизированных обёрток —
см. [GeneratedBindings.md](GeneratedBindings.md).

Контейнеры, объектные поля структур, FText, weak/soft/interface references,
static/RPC/latent calls и AOT новых API остаются отдельными задачами.
Пользовательский DSL классов и его hot reload остаются в плане; синтаксис не выбран.
