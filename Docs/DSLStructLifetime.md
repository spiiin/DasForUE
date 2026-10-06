# Время жизни snapshots UEStructValue

Добавлен mark/sweep для context-owned FInstancedStruct snapshots.
4096 — теперь предел одновременно retained values, а не всех values за
время жизни context. Identical snapshots по-прежнему deduplicate.

## Сборка мусора

- После persistent call, когда host result уже скопирован, runtime просматривает
  VM globals и зарегистрированные external GC roots, затем удаляет недостижимые
  snapshots. Это работает и без options gc для серии коротких вызовов.
- Внутри active interpreter call сборка запускается под давлением начиная с
  примерно 256 retained snapshots. Для неё нужны штатные GC liveness metadata:

~~~das
options gen2
options gc
require ue_dsl
~~~

- Mark traverses typed globals, arrays/tables/nested script values, active args
  и live locals. Allocating native helpers имеют needCallerStackFrame, чтобы
  вызывающие functions не исчезали в frameless fastcall chain.
- `ue_struct_collect()` позволяет запросить collection явно;
  `ue_struct_value_count()` — диагностика текущего retained count.
- Если active frame нельзя атрибутировать, runtime не освобождает snapshots
  на основании неполного набора roots. Explicit collection сообщает ошибку;
  automatic pressure collection пропускается. Это касается missing options gc,
  foreign/position-less frames и compiled AOT/JIT paths.
- Hard cap 4096 retained snapshots остаётся защитой от действительно большого
  live set. Context release освобождает все snapshots штатным destructor.

Нативные данные не перемещаются: mark/sweep удаляет map entries по tokens;
живые tokens и immutable value semantics остаются прежними. Host results
содержат самостоятельный FInstancedStruct и не зависят от VM table после return.
Во время native with-helper исходная native value copy уже получена до collection.

## Проверено

struct_snapshot_gc.das выполняет 20,000 новых values в одном call, проверяет
active local/argument roots во вложенной helper function и global array anchors.
Следующий call выполняет ещё 10,000 updates. После call остаются только четыре
global values; больше 10,000 transient snapshots освобождено.

struct_snapshot_boundary.das выполняется 5,000 раз без options gc: transient
values удаляются между calls, global anchor остаётся один. Release оставляет
нулевой count. Foundation stress tests входят в Editor и packaged runtime;
PIE/SaveReopen reflected struct paths проверяются отдельно как regression.

Прежняя документация bounded adapter обновлена: production interpreted loops
могут создавать новые transient values дольше лимита table. Универсальной
поддержки arbitrary foreign compiled frames, native opaque holders или AOT/JIT
root attribution этот этап не добавляет. Для supported scripts следует хранить
values в typed VM roots; native extensions должны регистрировать GC roots.
