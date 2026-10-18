# SetCollisionProfileNameProperty

Узкий адаптер `collision_profile_property_v1` открывает generated-вызов
`ue_collision_SetCollisionProfileNameProperty` из KismetSystemLibrary.
Сохраняются обычные typed arguments, signature check и call frames;
специальным является только native dispatch конкретной функции.

```das
var value:S_CollisionProfileName_76e8ab1d
value.f_name=UEName(value="BlockAll")
let object=C_Object_55c3753a_from(ue_self_object())
ue_collision_SetCollisionProfileNameProperty(object,UEName(value="Profile"),value)
```

`object` — generated typed view UObject. Полный компилируемый пример с именами
сгенерированных типов находится в staging-каталоге из отчёта проверки.
Шаблон: Tests/CollisionProfileAdapter/collision_profile_probe.template.das.

Функция **присваивает значение struct-полю FCollisionProfileName объекта**.
Она не применяет collision profile к BodyInstance/PrimitiveComponent.
Для изменения физического поведения компонента остаётся
`ue_body_SetCollisionProfileName`. Имя внутри Value может быть NAME_None или
пользовательским именем; адаптер не требует наличия профиля в CollisionProfile
manager, поскольку это операция записи значения, а не применения профиля.

## Проверки перед записью

- Только точный native owner KismetSystemLibrary и имя этой функции.
  Повторно проверяются три scalar input-параметра UObject, FName и
  FCollisionProfileName. Остальные CustomThunk/Latent не получают разрешение.
- Активный game-thread VM context, живой script WorldContext и совпадающий
  Game world. Запрещены ConstructionScript, bind_events, EndPlay, teardown,
  editor worlds, null/template/CDO и уничтожаемые Actor/ActorComponent.
- Поле каждый раз ищется заново по FName в текущем классе объекта. Допускается
  унаследованное scalar FCollisionProfileName; другие структуры, числа,
  missing fields, fixed/dynamic arrays отклоняются. Пустое имя недопустимо.
- Нужны BlueprintVisible и разрешение записи: BlueprintReadOnly/EditConst
  или hidden поля не записываются.
- Источник берётся из проверенного typed call frame. Все обязательные inputs
  должны быть заполнены; объект и функция повторно проверяются при invocation.
  Setter вызывается через общий DescribeDasPropertyAccess/WriteDasObjectProperty,
  поэтому не обходятся доступные общему слою native/Blueprint property setters.

Адаптер не вызывает C++ stub Unreal, содержащий check(0), и не строит
искусственный Blueprint VM stack. Это намеренное ужесточение исходного thunk:
ошибочная цель/тип приводит к ошибке daScript вместо тихого пропуска записи.
Construction guard и identity dispatch встроены в runtime и сохраняются без
editor-only CustomThunk metadata. Export публикует явный `call_adapter` marker
и construction-unsafe contract; исключение BlueprintInternalUseOnly ограничено
этой функцией. Общая policy для внутренних функций не ослабляется.

## Проверка

`Scripts/TestCollisionProfileAdapter.ps1 -SkipBuild` после сборки smoke Editor.
113 Python-тестов и 22 целевые native/runtime проверки: generated inherited
write, semantic setter, type/name/visibility/array errors, null/CDO/foreign
world, destroyed pending argument, incomplete/wrong-source frames, unrelated
CustomThunk rejection и lifecycle/context guards. Thunk metadata удаляется
на время runtime-пробы; исходное значение восстанавливается при выходе.

Отчёт: logs/collision-profile-adapter-validation.json. Проверки запускаются
в transient мирах Editor NullRHI, пользовательские проекты не изменяются.
Collision package остаётся staged. Foundation/PIE/packaged не входят в этот
целевой прогон; коммит не создаётся.
