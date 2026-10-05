# Эксперимент: создание класса и structural reload

## Реализованный механизм

`daScript structure macro → FDasTypeDeclaration → UBlueprint → UBlueprintGeneratedClass`.
Внутренний Blueprint backend использует штатный компилятор и reinstancing Unreal.
Пользовательскому скрипту не требуется описывать Blueprint-граф вручную.
Это настоящий Actor-derived UClass с FProperty и UFunction, но не вручную
синтезированный native UClass и не копия механизма NimForUE.

Native adapter `ADasProbeActor` предоставляет корневой SceneComponent, ссылку Peer
и вызов daScript. Editor commandlet создаёт Blueprint-родителя, его обычного
Blueprint-наследника и карту. Поля берутся из результата синтаксического макроса.
Функция Evaluate имеет фиксированную тестовую сигнатуру `int → int` и граф,
вызывающий native adapter; её тело выполняется в daScript через ProcessEvent.
Произвольные сигнатуры и объявления методов DSL ещё не реализованы. Скрипт получает только int-параметр; доступ к self/UObject и его свойствам из daScript относится к следующему слою безопасной привязки.

## Сценарий

1. `schema_v1.das` задаёт поле speed; созданный родитель имеет default 10.
2. Blueprint-наследник переопределяет default на 20.
3. Размещённый экземпляр переопределяет speed на 37, получает положение
   (11, 22, 33) и двустороннюю ссылку с другим Actor.
4. Evaluate(40) возвращает 41.
5. `reload_v2.das` добавляет health с тестовым default 100 и меняет результат
   Evaluate(40) на 42. Обновление происходит в том же процессе редактора.
6. Проверяется уведомление OnObjectsReplaced именно для исходного экземпляра,
   обновление Blueprint-наследника, defaults/override, ссылки и transform.
7. Ошибочный скрипт отклоняется до изменения Blueprint; рабочая версия сохраняется.
8. Выполняется GC; сохранённые классы и карта открываются в другом процессе
   без повторного запуска генератора схемы, и данные/вызовы проверяются заново.

Значения defaults в этом эксперименте заданы генератором тестовой фикстуры.
Они не означают принятия синтаксиса defaults для пользовательского DSL.

## Запуск

```powershell
.\Scripts\TestReload.ps1
.\Scripts\TestReloadPIE.ps1
```

Сценарий собирает editor target и запускает два отдельных commandlet-процесса.
Каждый должен завершиться с кодом 0 и собственным `DAS_RELOAD_RESULT=PASS`.
Новый набор тестовых assets создаётся в
`Tests/DasForUESmoke/Content/DasReloadProbe/Run_<GUID>/`.
Это отдельный тестовый проект; другие пользовательские карты не открываются
и не меняются. Сгенерированные assets исключены из Git; сохранённый набор остаётся
доступен для ручного осмотра в Content Browser.

Журналы: `logs/reload-create.log`, `logs/reload-reopen.log`.
`logs/reload-root.txt` содержит package path последнего сохранённого набора.

Второй сценарий запускает настоящий Unreal Automation test `DasForUE.Reload.PIE`
в отдельном редакторе с NullRHI. Он дожидается BeginPlay, меняет speed в игровом
мире с 37 на 73, обновляет класс и проверяет, что игровое значение 73 и
редакторское 37 остаются независимыми. Проверяются reinstancing, новое поле и метод,
ссылки внутри мира, transform, GC и редакторский экземпляр после EndPIE.
Журнал: `logs/reload-pie.log`; отчёт: `logs/reload-pie-report/index.json`.

## Результат — 2026-10-05

- 26 контрольных точек создания, обновления и сохранения: PASS, код процесса 0.
- 12 контрольных точек повторного открытия в новом процессе: PASS, код процесса 0.
- Настоящая PIE-сессия: 15 контрольных точек PASS; Unreal Automation сообщает
  один успешный тест, без ошибок и предупреждений, код процесса 0.
- Регрессия основания: все 12 тестов PASS в editor commandlet и packaged game.

## Найденные особенности UE

- AddMemberVariable по умолчанию ставит DisableEditOnInstance. Для пользовательских
  overrides нужно явно разрешить instance editing, иначе construction/reinstancing
  может сбросить значение к default.
- После LoadObject карты компоненты необходимо зарегистрировать: до этого
  кэшированный world transform ещё не отражает сохранённые относительные координаты.
- Для создания graph nodes используется FGraphNodeCreator, предотвращающий повторную
  генерацию пинов при PostPlacedNewNode/ReconstructNode.

## Границы результата

- Проверены editor world в headless commandlet и одна настоящая PIE-сессия.
  Несколько сетевых клиентов, открытые Blueprint-окна, selection/undo,
  file watcher и пользовательский интерфейс reload не проверены.
- В PIE проверен флаг HasActorBegunPlay после замены, но не количество вызовов
  пользовательских BeginPlay/EndPlay callbacks и не перенос подписок/задач.
- Поддержаны только добавления int/float-полей. Удаление и смена типа существующего
  поля отклоняются; миграций enum/struct/components/DataTable пока нет.
- Отказ ошибочного скрипта происходит до изменения класса. Полный rollback ошибки
  внутри Blueprint-компилятора после начала мутации не реализован.
- Каждый вызов probe заново компилирует скрипт: это проверка механизма, а не готовая
  производительная runtime architecture. Далее нужны persistent contexts и ownership.
- Native виртуальные функции, произвольное наследование C++, сетевой reload и
  Shipping/cook сгенерированных классов не проверены. Packaged foundation smoke
  проверяет только ранее реализованное основание и совместимость native adapter.
- Синтаксис публичного DSL остаётся открытым. Текущие аннотации и строковое описание
  полей — временный формат эксперимента.

## Следующий шаг

Сохранить этот backend как проверенный вариант и выделить нормализованную IR и
editor service из commandlet. Затем расширить PIE-проверки: несколько клиентов,
Actor lifecycle callbacks, подписки и задачи при reload; отдельно — metadata-only changes,
undo/redo и обновление открытых Blueprint-графов. Решение о production backend
принять после этих проверок и проверки cook parity.

## Обновление после выделения сервиса

Генерация перенесена в FDasBlueprintService, схема представлена FDasClassSchema.
Добавлен слабый self и scalar-доступ к свойствам. Теперь reload/reopen и PIE также
проверяют запись из daScript; см. Docs/Interop.md. Указанные выше ограничения
отсутствия self относятся к исходной версии эксперимента.
