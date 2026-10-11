# Экспорт IR и генерация обёрток — первый срез

Цепочка: `Bindings/Selection.json` → UE reflection → `Bindings/Api.json` →
`Scripts/GenerateBindings.py` → `Content/Das/Scripts/ue_generated.das` в плагине.
Всё основано на загруженной reflection Unreal; исходный C++ UE не парсится.
Синтаксис DSL пользовательских классов этот этап не определяет.

## Выбранный API

Selection.json задаёт полный путь класса, имя функции и имя обёртки. Сейчас включены
21 функция: ActorHasTag, SetTickGroup, GetOwner, SetOwner, K2_GetActorLocation,
SetActorRelativeScale3D, GetActorRelativeScale3D и
четырнадцать функций UDasObjectFixture для проверки скаляров, строк, enum/FName и структур.
Тестовые функции нужны для проверок; они не являются частью API обычного Actor.
Экспортер не обходит автоматически весь движок и не загружает произвольные assets.

IR имеет schema_version=5 и содержит версию движка, receiver class, имя функции,
alias, контрольное описание сигнатуры и поля параметров. Именованные структуры находятся в structs, параметры верхнего уровня — в parameters.
Массив classes содержит type_path и parent для receiver, object-параметров и предков.
Поля структур также представлены путями скалярных листьев для проверки согласованности. Каждый лист содержит kind, input/output и type_path для
enum/объектного класса. Enum содержит символы и десятичные значения в строках, чтобы
JSON не терял точность int64. Полная схема UHT/метаданных пока не экспортируется.

Экспортер использует ту же проверку поддерживаемых параметров, что и runtime.
При отсутствии класса/функции, неподходящих flags или неподдерживаемом параметре
экспорт завершается ошибкой, не записывая частично собранный JSON.

## Использование

```powershell
C:\src\DasForUE2\Scripts\GenerateBindings.ps1
C:\src\DasForUE2\Scripts\GenerateBindings.ps1 -Check
```

Команда собирает editor target, запускает DasExport, генерирует оба результата
в Build/Bindings и только после успешной генерации копирует их в рабочие файлы.
`-Check` сравнивает заново полученный IR и код с сохранёнными файлами и возвращает
ошибку при расхождении, не обновляя их. Копирование двух файлов не является
транзакцией файловой системы. Python по умолчанию C:\Python312\python.exe;
путь можно переопределить параметром -Python, движок — -Engine.

Отдельно генератор запускается без Unreal, используя уже экспортированный IR:

```powershell
python Scripts/GenerateBindings.py Bindings/Api.json Plugins/DasForUE/Content/Das/Scripts/ue_generated.das --check
python Scripts/TestBindingsGenerator.py
```

В первом срезе aliases должны начинаться с actor_, fixture_ либо ue_. Генератор
отклоняет коллизии имён, неподдерживаемые kinds, неизвестную версию схемы и разные
определения одного enum. Генерация детерминирована, без временных меток.

## Код скрипта

```das
options gen2
require ue_generated
require ue_foundation

[export]
def main(input:int):int {
    let actor = C_Actor_adf65d5b_from(ue_self_object())
    let location = actor_K2_GetActorLocation(actor)
    if (actor_ActorHasTag(actor, ue_name("QuestTarget"))) {
        return int(location.f_x)
    }
    return 0
}
```

Обёртка сама проверяет класс receiver, создаёт call frame, проверяет его сигнатуру,
заполняет параметры, вызывает ProcessEvent, читает результаты и освобождает frame.
При исключении оставшиеся frames очищает существующий механизм выхода из VM.
Один выход возвращается непосредственно. Несколько выходных параметров возвращаются
структурой `<alias>_Result` с полями `out_<path>`; точки в пути заменяются на `_`.
Inout передаётся как входное значение и возвращается в result, аргумент скрипта
на месте не изменяется. Входные и выходные структуры теперь представлены целыми значениями —
см. [StructBindings.md](StructBindings.md).

FName представлен отдельной структурой UEName с полем value:string; конструктор
`ue_name(...)` делает переход явным. Enum представлен настоящим enum daScript с
именем из пути UE-типа и короткого SHA-256 суффикса; элементы имеют префикс v_.
Числовые ordinals daScript внутренние: преобразование в UE использует символы,
поэтому cast enum в int не даёт обещания получить числовое значение Unreal.

`generated_calls.das` — исполняемый пример на тестовом UDasObjectFixture.
`generated_actor.das` — пример на Actor, используемый PIE-тестом; он ожидает тег
DasEnumTag_17 и устанавливает tick group PrePhysics.
`generated_bad_*.das` и `generated_wrong_enum.das` — намеренно ошибочные тесты,
а не примеры для обычного запуска.

## Проверки до вызова

daScript отклоняет string вместо UEName, int вместо enum и другой enum-тип.
Для UCLASS генерируются отдельные типы ссылок и проверяемые casts; см. ClassBindings.md.
Контрольная сигнатура включает типы, направления, имена полей и struct/enum/object
identity. Если обёртка устарела, ue_expect_signature завершает выполнение до
ProcessEvent. Для enum изменения набора символов также видны в экспортированном IR
и выявляются режимом -Check; runtime не хранит полный старый список символов.

## Границы

Поддерживаемые default arguments описаны в Defaults.md. Нет перегрузок, произвольного metadata
policy (базовые правила отбора реализованы в BindingPolicy.md), наследования DSL-классов или AOT этих обёрток. Поддержка containers,
объектных полей структур и RPC/latent API описывается отдельными этапами. Статические вызовы Blueprint Function Libraries реализованы — см. DSLFunctionLibraries.md.
Hidden enum-символы кроме _MAX пока не фильтруются; это не готовая политика экспорта
публичного API движка. На данном этапе требуется явный небольшой выбор функций.

## Верификация первого среза

158 foundation-проверок прошли в Editor и packaged Win64 Development; оба процесса
завершились с кодом 0. Положительный пример выполняет сгенерированные обёртки для
скаляров, enum, FName, inout и структур. Отрицательные примеры подтверждают конкретные
ошибки компиляции типов. Runtime-проверки подтверждают отказ неверного receiver и
устаревшей сигнатуры без побочных эффектов UE-функции.

Objects PIE: 17 контрольных точек, одна успешная automation-проверка без warnings.
Все пять Actor-обёрток исполнились на настоящем игровом Actor. Режим генерации -Check
прошёл; 8 Python-тестов генератора прошли. TestBindingsExport.ps1 подтвердил, что
неподдерживаемая сигнатура K2_GetComponentsByClass отклоняется, сохраняя предыдущий
выходной файл даже после успешной обработки первой функции списка.

Команды: GenerateBindings.ps1 -Check, TestBindingsGenerator.py,
TestBindingsExport.ps1, TestEditor.ps1, TestObjectsPIE.ps1, TestPackaged.ps1.
Журналы этапа: C:\src\DasForUE2\logs\bindings-*-console.log.
Проверка git diff --check прошла.

Именованные структуры реализованы отдельным следующим этапом — см. StructBindings.md.
Типизированные ссылки реализованы в ClassBindings.md. Базовая политика выбора API реализована в BindingPolicy.md; следующий шаг — defaults.

Типизированные defaults реализованы следующим этапом: см. Defaults.md.

TArray скаляров реализован следующим этапом: Arrays.md. Текущий выбор — 27 функций.

TArray структур, enum и UObject реализован в IR v4: ComplexArrays.md. Текущий выбор — 36 функций.

Scalar TMap/TSet реализован в IR v5: HashContainers.md. Текущий выбор — 51 функция.
