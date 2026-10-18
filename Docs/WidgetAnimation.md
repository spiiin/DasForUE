# WidgetAnimationHandle: weak transport

`require ue_api_umg_input` публично подключает `ue_widget_animation`.
Четыре функции `ue_user_widget_PlayAnimation`, `PlayAnimationForward`,
`PlayAnimationReverse`, `PlayAnimationTimeRange` возвращают `UEWidgetAnimationHandle`.

```dascript
require ue_api_umg_input
let handle=ue_user_widget_PlayAnimationForward(widget,animation,1.0,false)
if(ue_widget_animation_is_valid(handle)) {
    let playing=ue_widget_animation_is_playing(handle)
}
```

`widget` и `animation` — generated wrappers `C_UserWidget_…` и
`C_WidgetAnimation_…`. Для полного вызова PlayAnimation явно задаются время,
число циклов, play mode, скорость и restore-state; действует explicit-defaults
политика пакета. Пример: `Tests/WidgetAnimation/probe.template.das`.

## Владение и время жизни

- Token принадлежит одной VM; native FWidgetAnimationHandle содержит weak state.
  Native widget и анимация не удерживаются через handle, state или legacy player.
- Handle — capability, а не копия текущего состояния анимации. Глобальные
  переменные, массивы и clone удерживают только capability в общей rooted таблице.
- `is_valid` проверяет native state, widget/world и флаг отзыва; `is_playing`
  дополнительно проверяет Playing. Пауза сама по себе не отзывает handle.
  Default, finished, destroyed, torn-down и expired значения дают false.
- Finish и NativeDestruct навсегда отзывают выданную capability. Повторный запуск
  и реконструкция того же UObject не оживляют старые handles.
- Наблюдение за NativeDestruct начинается до native Play. Результат закрепляется
  сразу после dispatch: позднее чтение существующего call frame не создаёт новую
  lease. Между отдельными входами в VM обычные call frames и так освобождаются.
- Native callbacks захватывают только weak lease; VM, Context и DAS closures
  в них отсутствуют. GC/reload/release снимают эти callbacks. Reload не останавливает
  саму UE-анимацию, которой управляет виджет, но удаляет старый контекст handles.

## Контракт

Exact native UScriptStruct + allowlist четырёх instance producers, scalar
ReturnValue only. Native input, reflected ue_property/defaults, JSON и native
контейнеры не поддерживаются. Лимит общей snapshot-таблицы — 4096 значений;
root traversal учитывает typed handles и результаты ещё живых call frames.

Вызовы требуют constructed widget в игровом мире текущего контекста, live
animation asset, finite неотрицательное время, положительную скорость и
неотрицательное число циклов (0 — бесконечно). Сохраняются общие guards для
execution role, construction, сигнатуры, enum и направления параметров.
Native FWidgetAnimationHandle::IsValid недостаточен: он может возвращать true,
когда finished state ещё временно удерживается нативным кодом.

Машинный контракт: `Bindings/WidgetAnimation.contract.json`.

## Целевые проверки

`Scripts/TestWidgetAnimation.ps1 -SkipBuild -Publish` проверяет свежий reflection,
четыре ready функции, компиляцию всего generated модуля и native/DAS сценарий.
Используется настоящая короткая UWidgetAnimation с MovieScene и advancement
времени, а не синтетический finished-флаг. Проверяются естественное завершение,
replay, NativeDestruct/реконструкция, позднее чтение результата, globals/clone/
arrays/GC, отзыв и снятие native callbacks при reload/release. Отдельно проверяется,
что живой DAS-контекст не удерживает виджет и asset от native GC.
Проверяются три compile-time попытки подделать token и девять generator tests.
Source/DLL/BuildId должны оставаться стабильными. Отчёт:
`logs/widget-animation-validation.json`.

Полные Foundation/PIE/packaged, физический ввод и видимое окно не запускаются.
Тестовый класс с доступом к protected animation tick находится только в DasEditor.
