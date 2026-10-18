# Пакеты отражённого API

Профиль выбирает библиотеку целиком, экспортёр перечисляет её reflected API,
генератор строит замыкание зависимостей и проверяет каждую функцию, затем весь
пакет. Пакет использует существующий ProcessEvent backend, call frames и GC.
Новых C++ обёрток для отдельных функций нет. Native gameplay registry эта
команда не меняет.

## Первый пакет: Math

Профиль: `Bindings/Packages/CoreMath.profile.json`.
Модуль: `Plugins/DasForUE/Content/Das/Scripts/ue_api_math.das`.
Снимок UE 5.8.3, 2026-10-18:

| Результат | Уникальных функций |
| --- | ---: |
| Найдено в KismetMathLibrary | 741 |
| Сгенерировано и скомпилировано вместе | 619 |
| Отклонено reflection-экспортёром | 112 |
| Отклонено генератором | 10 |

Десять последних — Float/Vector/Quaternion SpringState и RuntimeFloatCurve API
с непрозрачными/пустыми для текущего генератора структурами. Они не удалены из
отчёта. Остальные причины экспортёра перечислены поимённо в summary JSON.
Ни одна функция не объявлена рабочей только потому, что прошла reflection.

```das
options gen2
require ue_api_math

[export]
def main(input:int):int {
    let position : UEVector = ue_vector(1.0lf,2.0lf,3.0lf)
    let doubled : UEVector = ue_math_Add_VectorVector(position,position)
    return int(doubled.x)
}
```

Профиль выбирает **canonical_math**: Vector2D → UEVector2D, Vector → UEVector,
Rotator → UERotator, Transform → UETransform. Это те же foundation-типы,
которые используют native gameplay API, defaults и `self`. Они используются
в аргументах, результатах, нескольких out, ref/inout, массивах и полях других
структур; дублирующих `S_Vector_*` и `S_Transform_*` в модуле нет. Пакет публично
импортирует foundation, поэтому явные типы и ue_vector/ue_rotator доступны
после одного require ue_api_math. При конфликте импортов используйте
квалификацию `ue_api_math::` для его обёрток.

Остальные native structs (в том числе Quat, LinearColor, Box) пока сохраняют
generated transport. Например, LinearColor всё ещё имеет `f_r`, а Vector — `x`.
Режим задаётся в профиле явно; standalone GenerateBindings.py включает его
флагом `--canonical-math`. Его прежний режим остаётся доступен для существующих
generated-модулей и воспроизводится существующими проверками.

Профиль явно выбирает `default_arguments=explicit`: все аргументы передаются
явно, в том числе имеющие CPP_Default. Для 74 параметров опубликованных функций
исходный default_text записан в полном отчёте. Это не установка новых значений
по умолчанию и не выполнение произвольных C++ выражений. Старый генератор и его
default arguments для существующих модулей не изменены.

## Воспроизведение

```powershell
.\Scripts\GenerateApiPackage.ps1 -SkipBuild
.\Scripts\GenerateApiPackage.ps1 -SkipBuild -Publish
```

Без SkipBuild сначала собирается commandlet. Без Publish результат остаётся
в уникальной папке Build/ApiPackages/ue_api_math. Экспорт загружает UE-модули из
профиля. Другой профиль задаётся через `-Profile`.

Перед публикацией весь generated DAS-модуль импортируется под уникальным
именем с хешем содержимого: установленная старая версия не может подменить
проверяемый модуль. Затем выполняется заданный профилем `[export] main(int):int`,
который должен вернуть 42. Проверяются scalar вызовы, Vector input/result,
несколько out, Normalize с ref input/output, вложенный Transform, LinearColor,
массив int и возвращённые index/value. Это целевой Editor NullRHI тест; все 619
функций компилируются, но не каждая выполняется. PIE, packaged и Shipping не
проверялись на этой итерации.

После успешного preflight и проверки хеша устанавливается только данный модуль.
Провал проверки сохраняет установленный модуль; причина находится в preflight.log
соответствующего запуска. Диагностики генерации и default adjustments находятся
в report.json. Последний успешный отчёт —
`Build/ApiPackages/ue_api_math/latest-report.json`; компактный публикуемый снимок —
`Bindings/Packages/ue_api_math.summary.json`.

Состояния отдельных кандидатов: generation_ready, export_blocked,
generation_blocked, policy_excluded. Их сумма равна candidate_count.
`generation_ready` означает прохождение Python-генерации; успешную компиляцию
и probe подтверждают отдельные state/preflight поля всего отчёта.
Исключения требуют причины; исчезновение выбранной библиотеки и межфункциональные
коллизии останавливают пакет. Instance receiver packages реализованы на следующем этапе: [World / Actor / Components](WorldApiPackage.md). Адаптеры непрозрачных
структур и канонизация остальных value types остаются в плане.


## Проверка canonical math

```powershell
.\Scripts\TestCanonicalMath.ps1 -SkipBuild
# С публикацией проверенного Math-модуля:
.\Scripts\TestCanonicalMath.ps1 -SkipBuild -Publish
```

120 Python-тестов, компиляция всего Math-модуля и его runtime probe;
Editor-only fixture проверяет вложенные math-поля, массивы Vector/Transform,
пустые массивы, возврат структуры с контейнерами и изменение inout.
Семь отрицательных сценариев проверяют неверные direction/type, незаданный
размер массива, выход за границы, пропущенный элемент, освобождённый frame
и нечисловой/бесконечный Vector. Итог: logs/canonical-math-validation.json.
Fixture не публикуется в пользовательский Math-пакет.

Четыре math-типа проходят через общий resolver проверенных call frames:
агрегатный доступ разрешается только при наличии соответствующих leaf-путей
в валидированной сигнатуре и корректном направлении. Array initialization и
границы проверяются обычным resolver. После записи отмечаются конкретные
leaf-пути, включая индексы; установка одного элемента не делает остальные
заполненными. Точный native struct identity и конечность значения проверяются
до записи; Transform дополнительно требует нормализованную rotation.
Его quaternion копируется целиком, без преобразования через Euler angles.
Сырые UE-адреса в DAS не передаются.


## World / Actor / Components

Добавлен [пакет receiver-методов и библиотек](WorldApiPackage.md): семь классов,
GameplayStatics и KismetSystemLibrary. Используются тот же generic backend и
canonical math. Его transient Game-world probes проверяют настоящий WorldContext.


## Native accessor и construction policy

[Общий слой](NativeAccessAndConstruction.md) выполняет getter/setter и сохраняет
construction guards в generated contracts. World теперь содержит 652 функции,
в том числе 88 с guard; Math сохраняет 619. Native поля доступны через
ue_native_gameplay, данный package generator по-прежнему выбирает функции.
