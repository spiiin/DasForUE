# Объектные параметры и out/inout

Общий UCallHandle теперь поддерживает объектные входы и выходы, чистые out и
изменяемые inout. Прежние ue_call_* adapters сохранены с прежними ограничениями.
Синтаксис будущего пользовательского DSL не меняется и не фиксируется.

## Направления параметров

Направления берутся из CPF_ReturnParm/ConstParm/OutParm/ReferenceParm конкретной
UFunction. Внутренний IR каждого скалярного листа хранит отдельно bInput и bOutput.

| Параметр UE | Заполнять перед invoke | Читать после invoke |
| --- | --- | --- |
| Обычный вход / const-reference | Да | Нет |
| Чистый out | Нет | Да |
| Изменяемая reference, включая UPARAM(ref) | Да | Да |
| ReturnValue | Нет | Да |

Все входные поля inout-структуры обязательны. Чистые out и return инициализирует
FStructOnScope; они не требуют фиктивных значений из скрипта. Запись в out/return,
чтение обычного входа как результата и незаполненный inout отклоняются.

Поддержаны скаляры, FString и вложенные структуры предыдущего этапа; отдельно
стоящие объектные параметры также могут быть input, out, inout или return.

## Объектный API

`ue_arg_object(call, path, handle)` задаёт объектный вход. Null передаётся как
`ue_null_object()`. Истёкший handle отличается от null и вызывает ошибку. Класс
объекта обязан быть совместим с FObjectProperty::PropertyClass.

`ue_result_object(call, path)` читает object return/out/inout и возвращает слабый
UObjectHandle с ожидаемым классом этого параметра. Null или уничтоженный объект
даёт null handle. Для проверки используются ue_object_valid/ue_object_is_a.

Обычный UObjectHandle не удерживает объект. Однако frame удерживает сильными
ссылками заданные объектные входы и полученные объектные выходы до release или
конца текущего исполнения скрипта. Это защищает не принадлежащую UObject память
FStructOnScope от висячих объектных ссылок после GC. Замена аргумента заменяет pin;
после ProcessEvent обновляются pins выходов/inout. Цель проверяется перед вызовом;
Destroy Actor нельзя отменить сильной ссылкой, уничтожаемые аргументы отклоняются.

Frames по-прежнему одноразовые и не живут между callback/Tick. Копия UCallHandle
обозначает тот же frame. После ue_release_call все копии недействительны. Слабый
object handle, полученный из результата, может использоваться далее, если объект
ещё удерживается Unreal или другим frame.

## Пример для Actor

```das
let call = ue_begin_call(ue_self_object(), "GetOwner")
ue_invoke_call(call)
let owner = ue_result_object(call, "ReturnValue")
let set = ue_begin_call(ue_self_object(), "SetOwner")
ue_arg_object(set, "NewOwner", owner)
ue_release_call(call)
ue_invoke_call(set)
ue_release_call(set)
```

Пример scalar/string inout находится в `Plugins/DasForUE/Content/Das/Scripts/inout_values.das`.
Он рассчитан на тестовый UDasObjectFixture с методом:

```cpp
int32 MutateValues(UPARAM(ref) int32& Count, UPARAM(ref) FString& Text,
                  bool& Accepted, FString& Description);
```

Скрипт заполняет Count/Text, вызывает функцию и читает изменённые Count/Text,
выходы Accepted/Description и ReturnValue. Строковые результаты копируются в память
daScript и остаются пригодными после release frame. Встроенные проверки также
покрывают inout структуры и обмен объектными ссылками.

## Ограничения

Объектные поля внутри структур пока отклоняются: для них нужен рекурсивный учёт
ссылок и GC-aware адаптер структуры. Также не поддержаны TSubclassOf/FClassProperty,
weak/soft/interface references, контейнеры, FText, static/RPC/latent calls
и AOT новых обёрток. Вызовы синхронные, на game thread; в bind_events запрещены.

Enum/FName уже добавлены — см. [EnumsNames.md](EnumsNames.md).
Следующий срез — развитие IR и генерация типизированных обёрток.
Объектные поля структур и контейнеры требуют отдельного решения по владению.

## Проверки

134 foundation-проверки прошли в Editor и packaged Win64 Development, оба процесса
завершились с кодом 0. Проверены scalar/string/struct/object inout, чистые out,
null, неверный класс и истёкший handle, ошибки направления, обязательные входы,
сохранение строк после release, GC при живых frames и очистка pins после выхода
из callback. Файл inout_values.das также запускается в обоих вариантах.

Objects PIE: 13 контрольных точек, одна успешная automation-проверка без warnings;
добавлены общий SetOwner/GetOwner и выходной pin Copied реальной Blueprint-функции.
Повторно прошла регрессия delegates PIE (одна успешная проверка без warnings).
Команды: TestEditor.ps1, TestObjectsPIE.ps1, TestDelegatesPIE.ps1, TestPackaged.ps1.
Журналы текущего этапа: C:\src\DasForUE2\logs\inout-*-console.log.
Проверка git diff --check прошла.
