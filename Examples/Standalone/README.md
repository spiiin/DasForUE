# Самостоятельный UE-проект с daScript

Шаблон `Examples/Standalone/Template` создаёт отдельный проект `DasStarter`:
собственные C++ targets, игровые DAS-файлы, Blueprint-ресурсы и карта.
Он подключает плагин из текущего checkout через `AdditionalPluginDirectories`;
это пример интеграции исходников, а не готовый бинарный дистрибутив плагина.
Модули и ресурсы `Tests/DasForUESmoke` ему не нужны.

## Создание и точечная проверка

Нужны Windows x64, подготовленный исходный UE 5.8 и зависимости daScript
из рабочего checkout DasForUE2. Команды выполняются из корня DasForUE2:

```powershell
./Scripts/NewDasProject.ps1 -Destination C:/src/DasStarter
./Scripts/TestStandalone.ps1 -Project C:/src/DasStarter/DasStarter.uproject
```

Можно указать `-Engine C:/path/to/UnrealEngine` в обоих скриптах.
Существующая папка назначения отклоняется без перезаписи.
Имя проекта внутри шаблона — `DasStarter`; параметр Destination выбирает папку.
Python сейчас вызывается по пути `C:/Python312/python.exe`, как и в других
инструментах checkout; при иной установке Python нужно поправить скрипты.

TestStandalone собирает editor target, обнаруживает используемые native-классы,
генерирует их полные доступные привязки и пересобирает runtime с новым реестром.
Затем project commandlet создаёт Blueprint и карту, а отдельный игровой процесс
с NullRHI проверяет possession, авторский компонент, native collision, вызов
авторского метода, таймер восстановления предмета и reload импортированного
проектного модуля с сохранением состояния. Изменённый тестом исходник
восстанавливается, его хеш проверяется. Повторная генерация должна быть идентичной.
Отчёт: `<project>/Saved/DasValidation/result.json`; журналы рядом.
`-SkipBuild` пропускает генерацию/сборку и требует уже актуальные DLL и bindings.

Проверка не запускает общий Foundation, полный набор PIE, packaged Development
или визуальную проверку и ввод с физической клавиатуры/мыши.

## Открытие примера

После успешной подготовки откройте `C:/src/DasStarter/DasStarter.uproject`
в редакторе и запустите `/Game/Starter/StarterMap` через Play.
WASD — перемещение, мышь — поворот камеры. На площадке расположен куб-предмет:
при пересечении персонажем он добавляет Score, скрывается и возвращается по таймеру.
Score доступен как Blueprint-свойство персонажа; интерфейс счёта в шаблон не входит.

## Где лежит код

| Файл в новом проекте | Назначение |
| --- | --- |
| `Content/Das/Scripts/starter_character.das` | Character, камера, ввод и Score |
| `Content/Das/Scripts/starter_component.das` | Авторский ActorComponent с состоянием |
| `Content/Das/Scripts/starter_pickup.das` | Sphere/StaticMesh, overlap, таймер |
| `Content/Das/Scripts/starter_refs.das` | Типизированные ссылки на авторские классы |
| `Content/Das/Scripts/starter_values.das` | Обычный импортируемый DAS-модуль, проверка reload |
| `Source/DasStarterEditor/DasStarterCommandlet.cpp` | Создание Blueprint и карты |
| `Source/DasStarter/DasStarter.cpp` | Игровой модуль и probe, активный только с `-DasStarterProbe` |
| `Bindings/NativeGameplay.sources.json` | Входные модули и каталоги native-генератора |

## Подключение и генерация

Runtime ищет именованные игровые скрипты сначала в
`<project>/Content/Das/Scripts`, затем в каталоге скриптов плагина.
Обычный `require starter_values` использует тот же приоритет; stdlib остаётся
в daScript root плагина. Имена проектных модулей должны быть уникальны:
совпадение намеренно перекрывает модуль плагина. Разрешение относительных путей
и `daslib` сохраняет стандартное поведение daScript.

```powershell
./Scripts/GenerateNativeGameplay.ps1 -Project C:/src/DasStarter/DasStarter.uproject
```

Выходные файлы принадлежат проекту:

- `Bindings/NativeGameplay.json` — reflected IR.
- `Source/DasGenerated/DasProjectNativeGameplay.generated.h` — native runtime registry.
- `Content/Das/Scripts/ue_native_gameplay.das` — типизированные DAS-обёртки.

Глобальные generated-файлы плагина эта команда не заменяет.
Runtime.Build.cs выбирает проектный реестр, если заголовок существует,
и добавляет проектные скрипты в runtime dependencies для staging.
Это не подтверждение работоспособности packaged-сборки: она требует отдельной проверки.
Поддерживается один реестр на собираемый project target. В этой конфигурации
UBT размещает DLL внешнего плагина в `<project>/Binaries/Win64`, отдельно от
smoke-проекта. Исходники самого плагина остаются общими для checkout.

Для генерации карты отдельно после сборки:

```powershell
& C:/src/unreal/UnrealEngine/Engine/Binaries/Win64/UnrealEditor-Cmd.exe `
  C:/src/DasStarter/DasStarter.uproject -run=DasStarter -unattended -NullRHI
```

Commandlet обновляет только собственные ресурсы `/Game/Starter`.
Его не следует запускать поверх вручную изменённой StarterMap без сохранения копии.
Изменения тела DAS и импортов подхватываются runtime reload; изменения схемы
авторских типов требуют reimport, изменения используемых native API — генерации
и C++-сборки. Видимый editor PIE reload в этой точечной проверке не проверяется.

## Результат точечной проверки 2026-10-18

`C:/src/DasStarter/DasStarter.uproject`: Editor Development, экспорт карты и
трёх Blueprint, четыре игровых проверки и воспроизводимость native generation —
PASS. Обнаружено 17 native-классов. Защита существующей папки — PASS.
Глобальные generated-файлы плагина сохранили хеши. Остальные виды проверки,
перечисленные выше, не выполнялись. Отчёт: `logs/standalone-validation.json`
в checkout; подробные игровые журналы — в проектном Saved/DasValidation.

## Освещение карты

Генератор создаёт Movable DirectionalLight: карте не требуется запекание света.
Для обновления только освещения уже существующей StarterMap (без пересоздания
объектов) после сборки editor target запустите:

```powershell
& C:/src/unreal/UnrealEngine/Engine/Binaries/Win64/UnrealEditor-Cmd.exe `
  C:/src/DasStarter/DasStarter.uproject -run=DasStarter -LightingOnly -unattended -NullRHI
```

2026-10-18: существующая карта обновлена, offscreen D3D12/SM5 запуск подтвердил
`NumLightingUnbuiltObjects=0`. Экранные предупреждения не отключались.
Отчёт: `<project>/Saved/DasValidation/lighting-validation.json`.
