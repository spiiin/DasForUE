# Типизированная схема, editor-сервис и доступ к self

Реализовано в DasForUE2, проверено на UE 5.8.3 / Win64 Development, 2026-10-05.

## Где смотреть код

Все пути ниже относительно <repo>.

- `Plugins/DasForUE/Source/DasRuntime/Public/DasSchema.h`: минимальная типизированная
  модель класса, scalar-полей, defaults и метода.
- `Plugins/DasForUE/Source/DasRuntime/Private/DasSchema.cpp`: проверка схемы и
  преобразование временного строкового формата макроса в IR. Не зависит от редактора.
- `Plugins/DasForUE/Source/DasEditor/Public/DasBlueprintService.h`: API editor backend.
- `Plugins/DasForUE/Source/DasEditor/Private/DasBlueprintService.cpp`: создание полей
  и метода, preflight и запуск штатного Blueprint compiler/reinstancing.
- `Plugins/DasForUE/Source/DasRuntime/Private/DasRuntime.cpp`: доступ к self и свойствам.
- `Plugins/DasForUE/Content/Das/Scripts/self_properties.das`: работающий пример чтения
  float/int и записи новых значений в текущий UObject/Actor.
- `Plugins/DasForUE/Tests/Editor/Private/DasReloadCommandlet.cpp`: сценарии проверки;
  логика генерации теперь вызывается через сервис, а не реализована внутри теста.

## Пример daScript

```das
options gen2
require ue_foundation
[export]
def main(input : int) : int {
    let speed = ue_get_self_float("speed")
    assert(speed > 0.0)
    ue_set_self_float("speed", speed + 1.0)
    let health = ue_get_self_int("health") + input
    ue_set_self_int("health", health)
    return ue_get_self_int("health")
}
```

Этот скрипт вызывается с self, содержащим scalar-свойства speed и health.
ADasProbeActor::ExecuteDas передаёт свой this как TWeakObjectPtr<UObject> в runtime.
Для вызова из другого C++ кода используется RunFile(Path, Input, false, WeakSelf).
Запуск примера без self ожидаемо возвращает диагностическую ошибку.
Названия низкоуровневых функций не фиксируют будущий синтаксис пользовательского DSL.

## Границы доступа к объектам

- Скрипту не передаётся сырой адрес UObject. Он работает с текущим self, доступным
  через слабую ссылку в контексте конкретного вызова.
- Слабая ссылка не удерживает объект от GC. Проверяются serial/lifetime UE,
  валидность UObject и состояние уничтожения Actor.
- Все операции разрешены только в активном runtime-контексте на game thread;
  compiler/macro context не получает доступ к объекту.
- Для каждого обращения заново находится FProperty текущего класса. Указатель
  на свойство не сохраняется между вызовами и не переживает reinstancing.
- Поддержаны ровно scalar int32 и float32. Массивы, другой числовой тип и отсутствующее
  свойство отклоняются. Чтение требует BlueprintVisible; запись дополнительно
  запрещена для BlueprintReadOnly.
- Ошибки возвращаются в FDasExecutionResult; восстановление после аппаратных ошибок
  памяти или UE fatal assertions этим механизмом не обещается.

Это первый слой self-доступа, не универсальные script-side object handles.
Передача произвольных UObject-ссылок между скриптами, контейнеры и ownership policies
остаются следующими задачами. Контекст пока создаётся заново на каждый вызов.

## Editor API

1. FDasBlueprintService::ReadSchema исполняет существующий macro probe и возвращает
   FDasClassSchema. При ошибке выходная схема не заменяется.
2. Вызывающая сторона задаёт typed defaults и разрешения instance editing.
   В тестах speed=10 и health=100 теперь задаются только фикстурой, а не backend.
3. FDasBlueprintService::Apply применяет схему к Blueprint на базе ADasProbeActor.
   Backend получает типизированные данные и не разбирает строку полей.

Текущий IR описывает Actor-derived class, int32/float32-поля и один метод int32 → int32.
Принятие окончательного синтаксиса DSL, произвольные сигнатуры, metadata-модель,
стабильные migration IDs и versioned schema storage ещё впереди.

Preflight отказывается от удаления/смены типа существующего поля, изменения default
или instance editability без политики миграции. Defaults сравниваются с CDO как
типизированные значения, поскольку Blueprint compiler может менять их строковую запись.
Полный rollback после ошибки Blueprint-компилятора уже после начала мутации пока
не реализован. Метод вызывается через существующий native adapter, а не произвольный
C++ virtual override.

## Проверки

```powershell
.\Scripts\TestEditor.ps1
.\Scripts\TestReload.ps1
.\Scripts\TestReloadPIE.ps1
.\Scripts\TestPackaged.ps1
```

- 21 foundation-проверка PASS в editor commandlet и packaged game. Включены
  положительный self-доступ, отсутствие self/поля, несовпадение типа, read-only,
  освобождение объекта сборщиком мусора и повторный вызов с истёкшей слабой ссылкой.
- Reload и загрузка карты в новом процессе: PASS, включая чтение и изменение
  скриптом свойств заново созданного класса.
- Настоящая PIE-сессия: PASS, Unreal Automation сообщает 1 success, 0 failures,
  0 succeededWithWarnings. Проверены запись в игровой экземпляр после reload,
  неизменность редакторского экземпляра и отказ после Actor::Destroy.
- Все тестовые процессы завершились с кодом 0. Логи находятся в logs/.

PIE означает Play In Editor: игру, запущенную внутри редактора. Здесь тест проверяет
обновление класса во время игры, а не только загрузку assets в commandlet.

## Что осталось

Persistent contexts и кеш компиляции, безопасные произвольные object handles,
UFunction-вызовы и сложные параметры, callbacks BeginPlay/EndPlay и подписки,
полная metadata/DSL модель, file watcher, undo/redo и cook parity сгенерированных
классов. Новый self-доступ проверен в интерпретаторе; AOT для этих bindings отдельно
не проверялся. Прежний отдельный AOT smoke test продолжает проходить.
