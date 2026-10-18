# DSL регистрации Async Physics Tick

Native-код для регистрации и разрешения тела больше не нужен. На `ue_class`
добавляется отдельная аннотация:

```dascript
[ue_class(module_id="gameplay",Blueprintable),
 ue_async_physics(script="thruster_physics.das",body_0="Mesh")]
class Thruster {
    // Mesh — объявленный ue_component либо native component property.
    // Обычные Initialize/BeginPlay/Tick остаются в gameplay VM.
}
```

`script` — имя отдельного `.das` файла в том же каталоге, без пути.
`body_0`…`body_7` — имена component properties, а не UObject paths или C++
выражения. Можно объявить несколько слотов или не объявлять тел для чистого
вычисления. Для класса ActorComponent доступен selector
`body_0="owner.RootComponent"` (или другое component property владельца).
Actor не может использовать prefix owner. Повторные параметры, неизвестные
слоты, одинаковые имена targets и отсутствующие объявленные/native properties
отклоняются. Проверки типа фактического тела и его поколения остаются native.

Physics-файл имеет прежний ограниченный entry point:

```dascript
options gen2
require ue_foundation
[export]
def physics_tick(dt:float; time:float) {
    let body=ue_physics_body(0)
    ue_physics_add_force(body,ue_physics_input(0),0.0lf,0.0lf,false)
    ue_physics_output(0,double(time))
}
```

Это отдельный Program/Context. В нём нет gameplay self или доступа к обычным
UE externs. Body tokens остаются scoped, а силы commit в том же preSim только
после успешного выполнения. DSL не ослабляет ограничения из
[ScopedPhysicsBody](ScopedPhysicsBody.md).

## Gameplay mailbox

Из Initialize, BeginPlay, Tick и обычных ue_function:

```dascript
ue_async_physics_set_input(0,1000.0lf)
if(ue_async_physics_has_output()) {
    let seconds=ue_async_physics_output(0)
}
let running=ue_async_physics_running()
let failure=ue_async_physics_error()
```

Input можно заполнить в Initialize/BeginPlay до активации VM; значения будут
переданы при запуске. После активации setter публикует весь фиксированный
packet с изменённым слотом. Размер — 16 finite double в каждую сторону.
Output до первого успешного шага недоступен: проверяйте has_output.
ConstructionScript, EndPlay, physics callback и отсутствие live self/DSL recipe
не дают доступа к этому gameplay mailbox. Никаких UObject/token в mailbox нет.

## Lifecycle, наследование и reload

При подготовке gameplay candidate отдельно компилируется physics Program,
проверяется physics_tick и исполняются globals без разрешения UE вызовов.
Initialize и пользовательский BeginPlay выполняются до регистрации callback.
Тела должны быть готовы к этому моменту. Если BeginPlay Actor ещё не завершён
во время BeginPlay компонента, активация откладывается до его первого обычного
Tick после готовности Actor. Ошибка пользовательского BeginPlay блокирует
регистрацию. До успешной активации силы не применяются.

Recipe наследуется от authored parent. Аннотация дочернего класса целиком
заменяет inherited recipe. Один canonical mailbox принадлежит экземпляру
дочернего класса; inherited callbacks и вызовы parent-методов используют его.
Дополнительная physics VM из parent lifecycle не создаётся.

Файл физики и прочитанные им зависимости добавляются к source dependencies
gameplay session. Изменение реализации пересобирает candidate на обычном GT.
При ошибке syntax/signature/init старая рабочая gameplay/physics версия
остаётся активной. После успешной подготовки новой VM и проверки body bindings
старые callbacks отзываются, новая версия заменяет их. Initialize/BeginPlay не
повторяются; input packet сохраняется, physics globals/output начинают новую
версию. Повреждённый неизменный файл не компилируется на каждом Tick.

Изменение самого recipe (script filename/body slot/selector) — изменение
declaration: требуется обычный Apply/Reimport Blueprint, как для других DSL
объявлений. Editor preflight отдельно компилирует physics script **до**
модификации Blueprint. Destroy, unregister, EndPlay, world cleanup и runtime
shutdown используют прежние guards/deferred cleanup. Runtime error physics_tick
останавливает эту VM; автоматического бесконечного restart после ошибки нет.
Подробности ошибки доступны через ue_async_physics_error.

## IR

У FDasDeclaredClass есть отдельное optional поле AsyncPhysics. JSON transport:

```json
"async_physics": {
  "script": "thruster_physics.das",
  "bodies": [{"slot": 0,"component": "Mesh"}]
}
```

Это описание recipe, а не сериализация token, объекта, proxy или состояния VM.
Блоки проходят JSON roundtrip, effective inheritance и structural comparison.

## Пример и целевые проверки

- `Content/Das/Scripts/async_physics_thruster.das` — Actor, native Box component,
  настройка симуляции в DAS Initialize и gameplay mailbox.
- `Content/Das/Scripts/async_physics_thruster_tick.das` — isolated physics VM.
- `Scripts/TestAsyncPhysicsDSL.ps1` — полный целевой прогон в sync/fixed async
  плюс только связанные scoped body/VM/role regression checks.
- Для короткой итерации: `-SkipBuild -SkipRegression -SyncOnly`.

Runner создаёт только GUID-файлы скриптов и тестовый GUID-каталог assets в smoke
проекте и удаляет именно их. Отчёт: `logs/async-dsl-validation.json`.
Foundation/полный PIE/packaged не запускаются; коммит не создаётся.

Итоговый целевой прогон: **298 PASS** — 50 DSL, 80 scoped body, 66 VM и 102
execution-role/lifecycle checks, в sync и fixed async (1/60). Runtime/Editor DLL
SHA256 стабильны, engine errors/ensure отсутствуют. Проверены наследование,
ActorComponent owner binding, отказ до модификации Blueprint, сохранение старой
VM при ошибке reload, отсутствие повторного BeginPlay, ошибка BeginPlay и GC.
Foundation, полный PIE и packaged не запускались. Коммит не создавался.
