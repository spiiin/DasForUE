# Geometry: owned snapshot transport

Синхронные [UserWidget handlers](WidgetInputHandlers.md) получают native Geometry
первым параметром: `handler(geometry:UEGeometry; event:TYPE):UEEventReply`.
Снимок захватывается из аргумента NativeOn-вызова и может пережить callback;
новая раскладка и следующий вызов не изменяют сохранённое значение.

`require ue_api_umg_input` подключает `ue_geometry` публично. Тип `UEGeometry`
содержит непрозрачный token, а точная копия `FGeometry` хранится в ограниченной
таблице текущего VM-контекста. Native layout, адреса, widget/path/window/viewport
в скрипт не попадают. Пустой DAS handle недействителен; native `FGeometry()` с
нулевым размером и единичной трансформацией — допустимое значение до layout.

```dascript
require ue_api_umg_input
let widget = C_Widget_f2421369_from(ue_self_object())
let geometry = ue_widget_GetTickSpaceGeometry(widget)
let size = ue_ui_geometry_GetLocalSize(geometry)
let absolute = ue_ui_geometry_LocalToAbsolute(geometry, ue_vector2d(4.0lf, 6.0lf))
```

Доступны 17 функций: 12 преобразований/запросов SlateBlueprintLibrary,
три Widget geometry getter и два WidgetLayoutLibrary getter. Точный список
и машинный контракт: `Bindings/Geometry.contract.json`.

## Границы

- Это снимок определённой layout revision: изменение или уничтожение виджета
  не меняет сохранённое значение. Для актуальной раскладки вызовите getter снова.
- Native copy сохраняет render transform, layout transform, pivot, rotation и
  отражение; значение не восстанавливается по отражённым полям.
- Глобальные переменные, локальные значения, массивы DAS и clone удерживают
  snapshot через общую GC/root metadata. Reload/release удаляет прежний контекст.
- Точный UScriptStruct, scalar signature, allowlist, направление параметра и
  состояние call frame проверяются; numeric/token constructors не доступны.
- Не-конечные значения и переполнение float math отклоняются. Вырожденную
  конечную геометрию можно наблюдать; обратные преобразования отклоняются до
  native Inverse. Размер 0 сам по себе не делает identity transform вырожденным.
- Вызовы доступны на gameplay game thread, вне frozen physics. Общие guards
  для construction, receiver, signature и WorldContext сохраняются.
- Viewport conversion требует game world; отсутствие viewport/layer сохраняет
  native headless fallback. Для существующего viewport проверяются размеры,
  преобразования и возможность деления/инверсии. Null/expired player отклоняется.
- `ue_property`, defaults, сериализация и native контейнеры Geometry пока закрыты.

## Целевые проверки

`Scripts/TestGeometry.ps1 -SkipBuild -Publish` проверяет свежий reflection,
готовность всех 17 функций, компиляцию целого модуля, 25 native/runtime условий
и три compile-time запрета подделки handle. Дополнительно — 8 generator tests.
Проверены владение, transform roundtrip, finite/singular/overflow rejection,
globals/clone/arrays/GC, уничтожение widget и hot reload. Все 17 wrapper вызываются
в native/DAS probe; Widget и viewport producers проверяются до layout/headless.
Visible window layout, PIE и packaged не запускались. Результат:
`logs/geometry-validation.json`.
