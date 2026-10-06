# Применение и hot reload DSL в Unreal Editor

## Команды

В консоли Unreal Editor (в том числе Output Log):

~~~text
Das.DSL.Apply dsl_counter_v1.das /Game/DasExamples/BP_Counter example.counter
Das.DSL.Reimport /Game/DasExamples/BP_Counter
~~~

Apply читает `.das` из `Plugins/DasForUE/Content/Das/Scripts`, собирает authoring
модель и проверяет все methods, затем создаёт или обновляет Blueprint. Последний
аргумент — stable class ID, а не имя класса. Можно явно применить вторую версию:

~~~text
Das.DSL.Apply dsl_counter_v2.das /Game/DasExamples/BP_Counter example.counter
~~~

Asset остаётся dirty: сохранить его нужно обычной командой Save. Apply не сохраняет
и не перезаписывает файлы автоматически. Путь asset должен начинаться с /Game/;
существующий asset другого типа не заменяется. Исходник задаётся только basename
`.das`, произвольные внешние paths не поддерживаются этой командой.

Blueprint хранит DasDSLSource/DasDSLClassId и canonical authoring model в package
metadata. Reimport использует сохранённую привязку; она проверена после открытия
asset в новом Editor-процессе. Обычный JSON authoring API продолжает работать.

## Поведение reload

- Изменение только тела метода обнаруживает существующий runtime source tracker
  при следующем вызове. Actor и его Unreal-поля остаются прежними. Reimport с
  неизменной моделью проверяет текущий Blueprint и не компилирует его заново.
- Добавление полей/методов применяется командой Apply/Reimport, включая работающий
  PIE. Blueprint reinstance обновляет экземпляры и ссылки; overrides сохраняются,
  новые поля получают defaults. Editor-world и PIE-world остаются раздельными.
- Неприменённая структурная декларация не заменяет работающий runtime context.
  Runtime проверяет типы и readonly properties; для Actor adapter также CDO
  defaults и reflected method signatures. Canonical declaration сравнивается с
  текущим context. Диагностика предлагает apply/reimport Blueprint.
- Syntax/compile errors и неподдержанные migration changes отклоняются до обычной
  mutation Blueprint. Уже работающий context продолжает выполнять последнюю
  допустимую версию. У свежего Actor без рабочей версии несовместимый source
  завершается диагностикой, fallback у него ещё нет.
- Исправленный source снова принимается автоматически при следующем вызове.
  Destroy/EndPIE освобождают owner contexts штатным runtime lifecycle.

Canonical JSON декларации теперь также хранится в compiled context для сравнения
версий. Это metadata схемы, а не копия значений Actor. В generated `.das` JSON
не добавляется. VM globals при смене context проходят обычную инициализацию;
существующий PersistentInts переносится. Универсальной миграции произвольного
состояния daScript VM пока нет.

## Проверено

TestDSLPIE.ps1 запускает настоящий Editor/PIE, проверяет console Apply/Reimport,
body replacement без смены Actor identity, syntax fallback, блокировку
неприменённых деклараций, structural reinstance с Score override и Bonus default,
Actor references, изоляцию миров, отказ от default migration, recovery, GC и EndPIE.
TestDSL.ps1 проверяет импорт, asset save/reopen и сохранённую source binding.
Runtime reload guards дополнительно входят в Editor/packaged foundation tests.

## Ограничения и следующий этап

Это ручной editor import/reimport, без автоматического watcher, toolbar UI или
стандартного UE AssetImportData factory. Ограничения типов/signatures первого
DSL frontend сохраняются: Actor adapter, int/float/bool поля, 0–16 ordered int/float/bool inputs и int/float/bool/void result.
Removal/rename/type/default/editability changes требуют отдельной migration policy.
Полного rollback после неожиданной ошибки Blueprint compiler уже после mutation
ещё нет; такие ошибки сообщаются явно, без утверждения об успешном применении.

Следующий этап: более общие signatures и типы полей/методов с capability checks;
автоматический watcher/editor UI можно добавить поверх этого import service.
