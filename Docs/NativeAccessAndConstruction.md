# Native accessors и construction context

Обновлено 2026-10-18. Общий слой объектных свойств находится в
`DasPropertyAccess.h/.cpp`. Он использует существующие descriptors, value codec,
контейнерные carriers и GC roots. Отдельных C++ адаптеров для каждого свойства нет.

## Зачем это нужно

Раньше native-отбор исключал свойства с getter/setter, даже если тип значения
поддерживался. Простое снятие запрета и запись по смещению обходили бы логику UE:
нормализацию значения, обновление зависимого состояния, render/physics/network
уведомления. Теперь обычный доступ к полю сохраняет его Unreal-семантику.

Функции с `UnsafeDuringActorConstruction` раньше исключались целиком, включая
обычный gameplay. Теперь генераторы сохраняют ограничение в контракте вызова,
а runtime проверяет текущую фазу. Это расширяет gameplay API и сохраняет запрет
в ConstructionScript. Полагаться только на editor metadata нельзя: в cooked
сборке её может не быть.

## Пример настоящего Engine API

```das
options gen2
require ue_native_gameplay

[export]
def main(input:int):int {
    let actor=ue_ref(type<ActorRef>,ue_self_object())
    actor.NetUpdateFrequency=37.5
    let actual=actor.NetUpdateFrequency
    let root=actor.RootComponent
    return int(actual)
}
```

NetUpdateFrequency использует SetNetUpdateFrequency/GetNetUpdateFrequency,
RootComponent — K2_GetRootComponent. RootComponent остаётся readonly.
Полный запускаемый пример: `native_accessors_example.das`; нужен Actor owner.
Native properties на typed references и через `self` используют один слой.

## Правила доступа

- Native C++ Getter/Setter вызываются через FProperty. Если задан
  BlueprintGetter/BlueprintSetter, используется указанная native UFunction.
- Getter обязан возвращать ровно одно значение нужного типа; setter — принимать
  ровно одно входное значение без return/out. Static, network, editor-only,
  latent/custom thunk accessors не принимаются. Blueprint graph implementations
  вместо native accessor-функции в этот контракт не входят.
- BlueprintVisible/BlueprintReadOnly, тип, declaring class, shape и accessor
  identity проверяются. Наличие C++ setter не снимает readonly.
- Запись сначала валидируется в отдельном хранилище. Ошибочный тип, неконечный
  scalar/math input и readonly-запись не вызывают setter. Валидный setter может
  нормализовать значение; следующий getter возвращает его фактический результат.
- Объект и ссылки внутри входного значения удерживаются на время setter,
  включая случай, когда setter запускает GC. UObject/FProperty/UFunction адреса
  в DAS не передаются; актуальные отражённые объекты разрешаются при доступе.
- Bool bitfields переводятся между native bool ABI accessor'а и отражённой
  битовой маской. Проверена маска второго бита, а не только первый бит.
- Для обычных snapshot-свойств без setter сохранён прежний borrowed write path;
  временное хранилище добавляется там, где его требует accessor.

Значения и контейнеры поддерживаются в пределах существующего native type
descriptor. Эта работа не добавляет произвольные C++ типы и не делает все
свойства движка доступными автоматически.

## Guard вызовов

Native manifest содержит construction_unsafe для методов и отдельные ограничения
для Blueprint getter/setter. Generic wrappers всегда передают явную политику
через ue_require_call_context. Unsafe-флаг монотонен: последующая отметка false
его не снимает. Guard проверяется при подготовке и перед dispatch.

В runtime предусмотрен отказ неизвестного низкоуровневого вызова без generated
policy в ConstructionScript, если политика не установлена и metadata недоступна.
Проверенные authored-вызовы сохраняют свой отдельный контракт. Это правило
выполнения bridge, не security sandbox для намеренно подделанного low-level API.

Разрешённые native методы и accessors работают в обычном gameplay и в настоящем
ConstructionScript, включая Editor world. Существующие WorldContext, world
identity, bind_events и EndPlay ограничения сохраняются. Guard не является
транзакцией всего ConstructionScript: ранее выполненные разрешённые операции
не откатываются. Проверяется отсутствие эффектов именно запрещённого вызова.

## Проверки и покрытие

```powershell
.\Scripts\TestAccessContext.ps1 -SkipBuild
```

160 Python-тестов, точная проверка native artifacts и целевой UE commandlet:
scalar getter/setter с отличием от backing field, bool bitfield, array, Vector,
BlueprintGetter/Setter, readonly, неверный тип, infinity, stale accessor contract,
GC внутри setter и настоящий Engine-пример. ConstructionScript проверен в Game
и Editor worlds; разрешённый вызов выполняется, запрещённые native/instance/
static/accessor вызовы не меняют свои счётчики.

После генерации у тестовых функций удаляется UnsafeDuringActorConstruction
metadata; проверки повторяются с сохранёнными контрактами. Это проверка
отсутствия metadata в Editor binary, а не packaged/Shipping-проверка.

В текущем выбранном native registry — 41 уникальное accessor-свойство (73
receiver occurrences) и 81 уникальный guarded-метод (362 occurrences), включая
фикстуры. Это не аудит всех классов движка.

Math: 619 функций. World: 652 функции, из них 88 с construction guard.
World blockers: 246 export + 6 generation. Поимённые причины сохранены в
Bindings/Packages/ue_api_world.summary.json; opaque Dialogue/physics types
и soft-reference generic ABI остаются отдельными задачами.

Логи: `logs/access-context-validation.json`, `logs/access-context-runtime.log`,
`logs/access-context-engine.log`. Пакеты повторно проверены и опубликованы.
Полные Foundation, PIE, packaged/Shipping не запускались. Коммит не создавался.

После обновления генератора project-specific C++ header сначала обновляется
из сохранённого IR, затем экспортируется и собирается свежий контракт.
Обновление checked-in/project-generated artifacts остаётся частью сборочного
процесса; stale compiled contracts нельзя переносить в другую версию engine/API.
