# Несколько методов, metadata и BlueprintReadOnly

Actor authoring backend теперь поддерживает 1..64 callable instance методов
int(Input:int):int. Каждый метод связывается с отдельным script_file/entry_point;
экспортированный main больше не обязателен для файла с именованными методами.
Pure/static, другие signatures и authored-parent emission пока отвергаются явно.
Синтаксис будущего DSL этим этапом не выбирается.

Authoring/Counter.v2.json содержит Evaluate, Inspect и InspectAgain. Первые два
named entry points inspect/inspect_again находятся в authoring_methods.das.

```das
options gen2
require ue_foundation
var calls = 0
[export]
def inspect(input:int):int {
    calls += 1
    return ue_get_self_int("Locked") + input + calls
}
[export]
def inspect_again(input:int):int {
    calls += 1
    return input + calls + 1000
}
```

## Контексты и reload

RunNamedPersistent выбирает entry point при каждом вызове. Один owner/file
использует общий persistent context для методов, globals, integer state и
подписок; разные owners/files независимы. Вызовы проходят signature verification.
Неправильный или отсутствующий entry point не разрушает рабочий live context.

Используется прежний source/dependency watcher с candidate compilation и
fallback к предыдущей версии. При reload проверяются сигнатуры ранее использованных
int entry points этого session, даже если текущий вызов относится к другому имени.
Ошибочный source/signature оставляет прежние implementations активными.
Неиспользованный ранее entry point должен пройти проверку при первом обращении;
это ещё не глобальный registry всех authoring contracts для runtime watcher.
Существующий main/lifecycle путь сохраняется; legacy Blueprint graphs остаются
на ExecuteDas, named graphs используют ExecuteDasNamed.

ApplyAuthoring заранее компилирует и проверяет все declared methods. Если
последний method неправильный, Blueprint не меняется. Existing method/parameter
IDs проверяются; удаление и rename требуют migration policy. Новые методы можно
добавлять. Старый single-method FDasClassSchema.Method остаётся compatibility adapter;
новый schema.Methods содержит нормализованный список.

## Metadata и доступ к свойствам

Поддержаны Category, DisplayName, ToolTip на классе, property и method. Function
metadata сохраняется в function-entry node, property metadata — в Blueprint
variable description, class metadata — в Blueprint display/description/category
source fields и package metadata. Generated reflection обновляется после compiler.
Это важно для save/reopen: проставления только на GeneratedClass недостаточно.
Метаданные не выполняются как код и проверяются по allowlist.

Для новых scalar properties применяется read_only: BlueprintReadOnly. Флаг
проверяется и обычным ue_set_self_int/float доступом: попытка записи отказана,
значение не меняется. Изменение read_only существующего поля пока считается
structural migration и явно отклоняется, как type/default/editability changes.
Editor instance editable и BlueprintReadOnly — разные настройки.

Текущий scalar property emitter остаётся int/float. Metadata-only Apply по-прежнему
может компилировать/reinstance Blueprint; оптимизация обновления metadata без
полной compilation — отдельная задача. Полного rollback после ошибки UE compiler нет.

## Проверка сценария

TestAuthoring.ps1 создаёт v1, обновляет до v2 и делает reopen в другом процессе.
Проверяются три метода, общие globals между inspect/inspect_again, Category/DisplayName/
ToolTip в reflection, readonly отказ, metadata persistence до повторного Apply,
и preflight последнего ошибочного метода без mutation. Compile/signature check
не запускает runtime globals или method bodies.
Foundation suite отдельно проверяет shared context без main, недопустимые signatures,
ошибки entry lookup, source reload/fallback/repair и cross-entry signature validation.

Следующий шаг: macro frontend, собирающий эту модель, и групповые annotations.
Выбор пользовательского синтаксиса остаётся отдельным решением; более общие signatures,
defaults/hooks/components, events/delegates и native adapters идут последующими этапами.


## Результат проверки

277 foundation-проверок прошли в Editor и 277 — в packaged Win64 Development,
оба процесса завершились с кодом 0. Authoring Create/Reopen: 33 контрольные точки,
включая metadata до повторного Apply после reopen, общие globals, readonly guard
и preflight ошибочного последнего метода. Регрессионные lifecycle и objects PIE
прошли; git diff --check прошёл. Эмиттер Blueprint проверен в Editor; packaged
suite проверяет runtime named entry points и прежний interop.
Журналы: logs/methods-*.log, logs/smoke-editor.log, logs/smoke-packaged.log.

Текущее scalar/void расширение adapter: Docs/DSLScalarSignatures.md.
