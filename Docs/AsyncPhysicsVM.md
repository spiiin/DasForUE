# Отдельная VM для Async Physics Tick

`CreateDasAsyncPhysicsVM(Path, Owner, Error)` — native API в
`Public/DasAsyncPhysicsVM.h`. Он компилирует отдельный Program, создаёт отдельный
interpreter Context, выполняет globals initialization и проверяет сигнатуру
`[export] physics_tick(dt:float; time:float)` **до** включения engine callback.
Обычный gameplay Context не используется; globals принадлежат новой VM.
В callback нет повторного чтения исходника, компиляции или поиска entry point.
Заранее скомпилированная VM здесь означает подготовленный interpreter Program,
а не AOT/JIT DLL.

Это нужно, чтобы physics callback не входил повторно в занятую gameplay VM и
не компилировал код, пока solver ждёт завершения callback. Изменяемая память
скрипта, UObject и snapshot handles между двумя VM не передаются.

## Обмен

Один пакет содержит 16 `double`, только конечные значения. Входной пакет
публикуется целиком через `IDasAsyncPhysicsVM::Publish` на обычном game thread.
Последний вход заменяет предыдущий: это mailbox, а не очередь команд/событий.
Пакет копируется на входе physics callback. Номер принятого входа и номер
успешного шага возвращаются вместе с выходным пакетом через `Read`.

В скрипте доступны `ue_physics_input(slot:int):double` и
`ue_physics_output(slot:int,value:double)`. Разрешены слоты 0..15, в обе стороны
по 128 байт значений. Выход начинается с нулей на каждом шаге, а публикуется
только после успешного возврата. Неверный слот/NaN/Inf отравляет транзакцию,
в том числе если скрипт перехватил исключение. Ошибка отключает binding.
После отключения `Read` возвращает false и не меняет переданный Out.
Нет строк, объектов, контейнеров, указателей или ссылок на память VM. Для
больших int64 такой transport непригоден: double не сохраняет все их значения.

```dascript
options gen2
require ue_foundation
var steps:int=0
[export]
def physics_tick(dt:float; time:float) {
    steps+=1
    let desired=ue_physics_input(0)
    ue_physics_output(0,desired*double(dt))
    ue_physics_output(1,double(steps))
}
```

```cpp
// На обычном game thread после BeginPlay владельца:
auto VM = CreateDasAsyncPhysicsVM(Path, Actor, Error);
if (VM) {
    FDasPhysicsPacket Input;
    Input.Values[0] = 100.0;
    VM->Publish(Input, Error);
    // Позднее, на обычном game thread:
    FDasPhysicsOutput Output;
    if (VM->Read(Output)) { /* использовать копию Output.Packet.Values */ }
}
```

Функции IO имеют отдельный проверяемый FFI gate: только текущий bridge-issued
step, только соответствующий isolated Context. Обычные UE externs запрещены
и при инициализации, и во время physics_tick. Вызов physics IO из gameplay,
из globals initializer или из чужой VM отклоняется. Publish/Read также
отклоняются во время frozen physics phase, даже из другого native callback.
Мьютексов и ожидания другой VM в обмене нет: bridge разрешает только
engine-owned frozen-game-thread preSim, не произвольный physics worker.

## Владение и обновление

Bridge владеет VM, наружный native handle хранит только mailbox и weak bridge.
Оставленный наружный handle не сохраняет Actor или Context после остановки.
При отмене во время шага VM уничтожается после выхода из callback через
существующую deferred cleanup. Destroy, EndPlay, unregister, ReleaseObject,
world cleanup, runtime shutdown и принятый gameplay reload используют уже
имеющиеся lifecycle hooks. После остановки автоматического rebind нет.

Для замены сначала создать новую VM и проверить успешный результат, затем
отключить старую через `Revoke`. Ошибка компиляции/сигнатуры/инициализации
кандидата старую VM не отключает. Автоматического file watcher для isolated
physics script пока нет; успешная замена начинает globals и mailbox с нуля.
Удаление native handle само по себе binding не снимает: используйте Revoke
либо lifecycle владельца.

## Границы

Unsafe запрещён compiler policy; global heap запрещён. Stack ограничен
64 KiB, static globals — 64 KiB; runtime DAS heap и string heap ограничены
по 1 MiB, host повторно устанавливает эти heap caps после simulate.
Это не гарантия отсутствия script allocations в callback и не жёсткий deadline:
бесконечный цикл всё ещё может задержать solver. Сторонние daScript modules,
native extensions и compile-time macro code не превращаются этим в security
sandbox. Предполагается доверенный код; обещается изоляция Context и
проверенный UE/physics-data FFI boundary, а не sandbox произвольных externs.

Native bind API, DAS IO и [декларативная регистрация](AsyncPhysicsDSL.md) реализованы;
network rewind/resim и worker-thread VM ещё не реализованы.
Scoped body token/AddForce теперь реализованы: [ScopedPhysicsBody](ScopedPhysicsBody.md). Raw BodyInstanceAsyncPhysicsTickHandle по-прежнему отклоняется
общим reflected transport.

## Целевая проверка

`Scripts/TestAsyncPhysicsVM.ps1` собирает smoke Editor и выполняет native
commandlet в sync и fixed async 1/60; `-SkipBuild` использует готовые DLL.
Проверяет отдельные globals, mailbox, finite values, запрет UE/чужого phase,
отсутствие чтения исходника во время tick, отказ кандидата, signature/init,
ошибки без partial output, revoke, Destroy/GC, reload и world cleanup.
Runner сверяет SHA256 Runtime/Editor DLL до/после, отклоняет engine errors/ensure.
Результат: `logs/physics-vm-validation.json`.
Foundation/полный PIE/packaged здесь не запускаются. Коммит не создаётся.


Финальный прогон: **66 VM checks + 102 execution-role/lifecycle regression
checks = 168 PASS**, по обоим режимам. Проверены также запрет shared/oversized
stack source options, обход no_global_heap, host heap cap при source unlimited,
physics IO в globals initializer и poisoned transaction после try/recover.
Runtime и Editor DLL SHA256 не изменились за весь прогон; engine errors/ensure
нет. Логи: `Build/AsyncPhysicsVM/280d4d0b0c98491aabe15e0ece7a98f7`.
