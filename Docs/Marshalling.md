# Общий маршаллинг UFunction

Добавлен первый общий путь вызова отражённых функций из daScript. Он работает с
нативными и Blueprint-функциями через ProcessEvent. Это низкоуровневый API будущих
генерируемых обёрток; синтаксис пользовательского DSL ещё не выбран.

## Модель вызова

`ue_begin_call(object, "Function")` возвращает непрозрачный `UCallHandle` и строит
IR сигнатуры: пути скалярных полей, типы и направления input/return. Адреса FProperty
не входят в этот IR. Вложенные структуры разворачиваются в пути `Payload.Metrics.Ratio`.
Форма и тип каждой структуры берутся из конкретной UFunction; передать память чужой
структуры или подменить тип по совпадению размера невозможно.

Перед выделением параметров проверяется вся сигнатура, включая возвращаемое значение.
Затем скрипт задаёт все входные скаляры, вызывает функцию один раз и читает результат.
Правила out/inout и объектных параметров описаны в [InOut.md](InOut.md).
Входные поля структур тоже нужно заполнить явно: автоматической подстановки C++ defaults нет.

| API | Назначение |
| --- | --- |
| `ue_begin_call(object, name)` | Создать и проверить вызов |
| `ue_arg_int/float/double/bool/string/object(call, path, value)` | Заполнить входной скаляр или поле структуры |
| `ue_invoke_call(call)` | Проверить полноту параметров и выполнить ProcessEvent |
| `ue_result_int/float/double/bool/string/object(call, path)` | Прочитать возвращаемое значение или его поле |
| `ue_release_call(call)` | Освободить временные данные; все копии handle станут недействительны |

Поддержаны int32, float32, float64, bool, FString, вложенные UScriptStruct с такими
отражёнными полями, несколько входов, void или одно возвращаемое значение.
Разрешены обычные входы по значению и const-reference. Bool читается через FBoolProperty,
включая его битовое представление; строки копируются UTF-8 ↔ FString.

## Пример для Actor

```das
options gen2
require ue_foundation

[export]
def main(input:int):int {
    let call = ue_begin_call(ue_self_object(), "K2_GetActorLocation")
    ue_invoke_call(call)
    let x = ue_result_double(call, "ReturnValue.X")
    ue_release_call(call)
    return int(x)
}
```

Этот пример сохранён как `Content/Das/Scripts/actor_location.das` в плагине и подходит
для вызова ExecuteDas на ADasProbeActor. Функция движка возвращает настоящий FVector.
Полный пример нескольких аргументов и вложенной структуры — `marshalling.das`;
он запускается тестами с приватным UDasObjectFixture, не с обычным Actor.

## Время жизни и ошибки

Параметры и результат находятся в FStructOnScope, который выполняет корректные
Initialize/Destroy операций UE, включая FString и вложенные структуры. Строка результата
копируется в память daScript и остаётся пригодной после ue_release_call.
Вызов хранит слабую ссылку на цель и удерживает её только на время ProcessEvent.

Call frames действуют только в пределах текущего исполнения скрипта/callback.
При выходе, включая ошибку, все оставшиеся frames освобождаются. Их нельзя сохранять
для следующего Tick или переносить через reload. Одновременно разрешено не более 256
frames; для длинных циклов нужно явно освобождать их. UFunction удерживается на время
frame, перед dispatch проверяется, что на объекте осталась та же функция.

Неполные входы, неверные типы, неизвестные имена и неподдерживаемая сигнатура приводят
к ошибке до ProcessEvent. Чтение неверного типа результата — ошибка уже после вызова:
побочные эффекты выполненной UE-функции не откатываются. Повторный invoke запрещён.
Подготовка и выполнение вызовов запрещены внутри bind_events. Вложенные события
по-прежнему доставляются после выхода из VM через существующую очередь.

## Границы текущего среза

Общий API теперь поддерживает UObject-параметры/результаты и изменяемые out/ref:
см. [InOut.md](InOut.md) для направлений и времени жизни сильных ссылок frame.
Прежние object adapters сохраняются. Enum и FName теперь поддержаны — см. [EnumsNames.md](EnumsNames.md).
Не поддержаны массивы/TArray/TMap/TSet,
FText, soft/weak/interface references, static/RPC/latent вызовы. Для непрозрачного
нативного состояния структуры нужен специальный адаптер: доступны только отражённые
поля; структуры с AddStructReferencedObjects отклоняются. Структурный reload во время
самого синхронного ProcessEvent не поддерживается.

AOT для новых API, сериализуемый полный IR с метаданными и генерация daScript-обёрток
остаются следующими работами. Сейчас проверки типов аргументов выполняются в runtime.

## Проверка

120 foundation-проверок прошли в Editor и упакованном Win64 Development приложении.
Оба процесса завершились с кодом 0. Покрыты несколько аргументов, const-reference,
вложенные структуры, Unicode, bool/float/double/int, владение строкой после release,
600 вызовов с освобождением frames, лимит frames и ошибки без лишних вызовов UE.

Objects PIE: 10 контрольных точек, одна успешная automation-проверка без warnings.
Общий frame вызвал настоящую Blueprint Echo и прочитал FVector из K2_GetActorLocation.
Сохранилась проверка прежних object adapters, подписок и Destroy/GC.
Повторно прошли delegates/lifecycle/structural PIE, save/reopen в новом процессе.
Команды: Scripts/TestEditor.ps1, TestObjectsPIE.ps1, TestDelegatesPIE.ps1,
TestLifecyclePIE.ps1, TestReloadPIE.ps1, TestReload.ps1, TestPackaged.ps1.
Журналы находятся в C:\src\DasForUE2\logs; новые прогоны имеют префикс marshalling-.


Упаковка: проверка target receipt обнаружила отсутствие новых .das файлов и остановила
прогон. Уточнение по исходникам UE 5.8.3: BuildMode.cs использует bUseUBTMakefiles для
предварительной загрузки, но синхронная ветка далее всё равно вызывает LoadMakefile.
Поэтому одного -NoUBTMakefiles недостаточно. TestPackaged.ps1 теперь перед сборкой
удаляет только генерируемый Tests/DasForUESmoke/Intermediate/Build/Win64/x64/
DasForUESmoke/Development/Makefile.bin. После пересоздания проверяется наличие всех
.das в receipt, затем выполняются cook/stage и тесты. Исходники движка не изменялись.
