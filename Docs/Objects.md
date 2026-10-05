# UObject handles и вызовы UFunction

Репозиторий: `C:\src\DasForUE2`.

В daScript зарегистрирован отдельный непрозрачный тип `UObjectHandle`. Это копируемое
значение со слабой ссылкой через реестр текущего контекста, а не сырой UObject pointer
и не обычное целое число. Скрипту не нужен `unsafe`. Проверка конкретного UE-класса
пока выполняется в runtime; отдельных сгенерированных daScript-типов для каждого
UCLASS ещё нет.

## Ссылки на объекты

| API | Назначение |
| --- | --- |
| `ue_null_object()` | Пустой handle, в том числе для инициализации глобальной переменной |
| `ue_self_object()` | Handle текущего владельца скрипта |
| `ue_get_object(handle, field)` | Чтение Blueprint-visible сильной объектной UPROPERTY |
| `ue_object_valid(handle)` | Проверка существования объекта и ожидаемого класса |
| `ue_object_is_a(handle, class_path)` | Проверка наследования |
| `ue_cast_object(handle, class_path)` | Проверяемое приведение с сохранением ожидаемого класса |
| `ue_object_get_int(handle, field)` | Чтение Blueprint-visible int32-свойства |
| `ue_object_set_int(handle, field, value)` | Запись int32-свойства с проверкой BlueprintReadOnly |

Классы задаются полным путём, например `/Script/Engine.Actor`. Класс должен быть уже
загружен; эти API не загружают assets автоматически. Чтение ссылок можно продолжать
цепочкой UPROPERTY, поэтому источник больше не ограничен полями self/peer.

Handle можно сохранить между вызовами:

```das
var target = ue_null_object()

[export]
def begin_play() {
    target = ue_cast_object(
        ue_get_object(ue_self_object(), "Peer"),
        "/Script/Engine.Actor")
}
```

Ссылка не удерживает объект от GC. Actor считается недоступным сразу после начала
Destroy, даже до GC. Обычный доступ через просроченный, чужой или пустой handle
возвращает ошибку daScript; `ue_object_valid` возвращает `false`.
Пустой handle разрешён как аргумент объектного UFunction-параметра. Просроченная
непустая ссылка не превращается молча в `nullptr`.

При logic reload реестр handles создаётся заново; глобальные handles нужно получить
повторно, например в `bind_events()` или при следующем вызове. Через существующий
int32-state handles не переносятся. Замена UObject при structural reinstancing также
не перенаправляет старый handle на новый экземпляр.

## UFunction

Вызовы идут через `ProcessEvent`, поэтому работают как native UFunction, так и
обычные Blueprint-функции. Сейчас реализованы пять адаптеров:

| API | Поддерживаемая сигнатура UE |
| --- | --- |
| `ue_call_int(object, name, value)` | `int32 → int32` |
| `ue_call_float(object, name, value)` | `float32 → float32` |
| `ue_call_void_int(object, name, value)` | `int32 → void` |
| `ue_call_object(object, name)` | `() → UObject*` или производный класс |
| `ue_call_void_object(object, name, argument)` | `UObject*` или производный класс `→ void` |

Перед вызовом проверяются наличие функции, `BlueprintCallable`, число параметров,
точные scalar-типы, возвращаемое значение и UE-класс объектного аргумента.
Несовместимые сигнатуры отклоняются до выполнения функции. Буфер параметров
создаётся и освобождается через `FStructOnScope`; на время `ProcessEvent` target
и объектный аргумент удерживаются сильными ссылками. Между вызовами владение слабое.
Указатели на UFunction/FProperty между вызовами не кешируются.

Пример для Actor:

```das
let self = ue_self_object()
let actor = ue_cast_object(ue_get_object(self, "Peer"), "/Script/Engine.Actor")
ue_call_void_object(self, "SetOwner", actor)
let owner = ue_call_object(self, "GetOwner")
verify(ue_object_is_a(owner, "/Script/Engine.Actor"))
ue_call_void_int(actor, "EmitDasSignal", 5)
```

Для проверок внешнего состояния используйте `verify` либо сначала вычислите
значение в локальную переменную: daScript запрещает побочные эффекты внутри `assert`.

Native-образец с тестовым UObject-классом:
`C:\src\DasForUE2\Plugins\DasForUE\Content\Das\Scripts\object_calls.das`.
Он рассчитан на `UDasObjectFixture` и используется foundation-тестом; это не готовый
пользовательский Actor. Реальный Actor/Blueprint сценарий находится в
`DasReloadCommandlet.cpp`, тест `DasForUE.Objects.PIE`.

## Подписка на объектный источник

```das
[export]
def bind_events() {
    let source = ue_get_object(ue_self_object(), "Peer")
    ue_subscribe_object_int(source, "OnDasSignal", "on_signal")
}
```

Источник может быть любым доступным через handle UObject с подходящим
BlueprintAssignable multicast-событием. Сохраняются проверки обработчика,
атомарная замена набора подписок, отзыв старых ID и очередь вложенных событий.
UFunction-вызовы и запись свойств из `bind_events()` запрещены: отклонённый кандидат
не должен менять действующие UE-объекты через эти API.

## Проверки

Editor foundation: **103 PASS**. Новые случаи включают native-вызовы с int/float,
возврат UObject, проверку класса аргумента, null, неверные сигнатуры и cast,
не-BlueprintCallable функции, out/ref, объектный источник подписки, запрет вызова
из кандидата и сохранённую слабую ссылку, истекающую после GC.

Настоящий PIE: **8 контрольных точек**; 1 success, 0 failed,
0 succeededWithWarnings. Создаётся Blueprint с обычной функцией Echo, вызываемой
из daScript. Дополнительно проверены native EmitDasSignal, engine SetOwner/GetOwner,
очередь события при ProcessEvent, немедленный отказ после Destroy, GC источника
и очистка контекстов после EndPIE.

Команды: `Scripts\TestEditor.ps1` и `Scripts\TestObjectsPIE.ps1`.
Packaged Win64 Development: build/cook/package и запуск завершились с кодом 0;
все **103 foundation-проверки** прошли. Повторно прошли `TestDelegatesPIE.ps1`,
`TestLifecyclePIE.ps1`, `TestReloadPIE.ps1` и `TestReload.ps1` с открытием assets
в новом процессе. Журналы: `C:\src\DasForUE2\logs`.

Уточнён packaging workflow: `TestPackaged.ps1` сначала вызывает Build.bat напрямую
с верхнеуровневым `-NoUBTMakefiles`, проверяет, что каждый `.das` из каталога примеров
есть в target receipt, и только затем выполняет cook/stage/archive. Передача этого
переключателя внутри UAT `-UbtArgs` не обеспечивала актуальный список файлов.

## Границы этого этапа

Перечисленные выше ue_call_* adapters ограничены пятью сигнатурами.
Общий API нескольких аргументов, bool, строк и структур добавлен отдельно —
см. [Marshalling.md](Marshalling.md). Out/inout и объектные параметры общего API описаны в [InOut.md](InOut.md).
Пока нет контейнеров, class/TSubclassOf-параметров,
RPC, static-функций и latent/async-вызовов. Вызовы выполняются на game thread.
Слабые/soft/interface UPROPERTY пока не входят в объектный getter.

Повторный вход в daScript из вызываемой UFunction пока запрещён; обычные
Blueprint-функции и native-функции работают. Новые adapters проверены в VM backend,
их AOT code generation остаётся отдельной задачей. Handles уничтожаемого Actor
не используются даже из EndPlay; ранее реализованный ограниченный self-access
для EndPlay остаётся отдельным механизмом.

Общий marshalling теперь использует UCallHandle и внутренний IR сигнатуры;
см. Marshalling.md. Синтаксис пользовательского DSL пока не фиксируется.

Уточнение следующего этапа: в UE 5.8.3 -NoUBTMakefiles не отключает синхронную
загрузку Makefile.bin. Packaging workflow дополнен точечным сбросом кэша только
игрового тестового target; подробности в Marshalling.md.
