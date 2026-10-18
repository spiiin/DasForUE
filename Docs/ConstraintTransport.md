# Безопасный transport GetConstraint

`UPhysicsConstraintComponent.GetConstraint` возвращает в daScript непрозрачный
`UEConstraintHandle`. Generic generator передаёт его в ref-параметры
`ConstraintInstanceBlueprintLibrary`; отдельные ручные wrappers для её методов
не нужны. В CollisionPhysics.profile.json добавлены все 56 локальных методов
этой библиотеки с префиксом `ue_joint_`.

```das
let handle=ue_constraint_GetConstraint(component)
if (ue_constraint_valid(handle)) {
    let updated=ue_joint_SetDisableCollision(handle,true)
    let state=ue_joint_GetDisableCollsion(updated)
    verify(state.out_returnvalue)
}
```

`GetDisableCollsion` сохраняет написание имени в Unreal, включая опечатку.
`component` — typed view PhysicsConstraintComponent из generated-модуля.
Ref/out-параметр возвращается как обновлённый checked handle; при нескольких
выходах он находится в `out_accessor` вместе с другими результатами.

## Lifetime и происхождение

В daScript хранится только токен. Запись в общей таблице snapshot-значений
содержит weak owner/body references, имена костей, broken state и метку
поколения Chaos proxy. Shared timestamp не удерживает UObject, solver или сам
constraint. Native accessor заново получается от компонента перед вызовом.
Его приватные Owner/Index и editor OnRelease callback не сериализуются и не
копируются в долгоживущий snapshot.

Проверяются активный VM-контекст, game thread, зарегистрированный компонент,
живой Actor, совпадающий Game world и допустимый lifecycle. Destroy, смена
constrained bodies/bones, изменение broken state и пересоздание физического
constraint делают handle устаревшим. После этого нужен новый GetConstraint.
Для компонента без physical handle допускается logical profile; появление
physical handle инвалидирует прежний logical handle. Если физическое поколение
нельзя надёжно определить, вызов отклоняется.

Pending вызов повторно проверяется непосредственно перед native dispatch.
Поколение результата GetConstraint фиксируется при dispatch, поэтому позднее
чтение старого call frame не приобретает новый constraint незаметно для скрипта.
Ошибки не выполняют откат уже завершённых native операций: если библиотечный
метод сам изменил поколение, получение его ref/out может быть отклонено.

Записи используют существующие snapshot GC, лимит 4096 и очистку VM-контекста.
UEConstraintHandle участвует в обходе корней; удерживаемые call frames хранят
проверяемую запись независимо от snapshot GC. Owner остаётся слабым и там.

## Границы

Поддержан scalar component-owned GetConstraint и ref/input-accessor параметры
из известной native ConstraintInstanceBlueprintLibrary. Skeletal/PhysicsAsset
accessors, произвольные producers, массивы/вложенные accessors и доступ к
приватному индексу пока отклоняются. Обычные ue_property/ue_function с этим
native accessor не включены. Construction, bind_events и EndPlay не являются
разрешёнными фазами работы с handle. После смены VM/hot reload старый токен
не переносится в новый контекст.

## Целевые проверки

`Scripts/TestConstraintTransport.ps1 -SkipBuild` — после сборки smoke Editor.

112 Python-тестов, компиляция всего staged Collision-модуля и две runtime-пробы:
getter/library ref-out; реальное создание двух Chaos bodies и constraint;
пересоздание constraint; устаревшие pending input и delayed output; отсутствие
побочного эффекта stale setter; null, wrong type/direction, unassigned input,
unexecuted result; snapshot GC; Destroy и реальный UObject GC при удерживаемом
snapshot/frame; отказ токена предыдущего VM-контекста при живом общем мире.

Отчёт: `logs/constraint-transport-validation.json`. Collision: 233/241 ready,
7 export blockers + 1 generation blocker. Из прироста 57 обёрток одна —
GetConstraint, остальные 56 — методы библиотеки. Generated означает компиляцию
всех методов, а не runtime-проверку каждого setter/getter. Силы constraint,
устойчивость симуляции и физические режимы этим тестом не измеряются.

Пакет остаётся staged. Полные Foundation, PIE и packaged не запускаются;
коммит не создаётся. Оставшиеся блокеры: CustomThunk
SetCollisionProfileNameProperty, opaque async physics tick handle и шесть
deprecated/служебных объявлений. Свежий список — CollisionPhysicsBlockers.md.

Реальная симуляция двух тел и режимов constraint теперь проверена отдельно:
[ConstraintSimulation](ConstraintSimulation.md), Scripts/TestConstraintSimulation.ps1.
12 этапов и 516 Chaos steps — PASS; включая изменение лимитов, break/recreate
и отказ stale snapshot/pending setter без побочного эффекта. Без полного PIE/packaged.
