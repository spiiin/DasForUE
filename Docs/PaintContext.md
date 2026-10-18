# PaintContext и синхронный OnPaint

В Widget Blueprint с родителем `UDasScriptUserWidget` задаются `ScriptFile`
и `PaintHandler`. Handler имеет точную сигнатуру:

```dascript
options gen2
require ue_api_umg_input
[export]
def paint(context:UEPaintContext):void {
    let tint=S_LinearColor_f4abb14b(f_r=1.0,f_g=1.0,f_b=1.0,f_a=1.0)
    let updated=ue_ui_DrawLine(context,
        ue_vector2d(5.0lf,5.0lf),ue_vector2d(60.0lf,30.0lf),tint,true,1.0)
}
```

`require ue_api_umg_input` публично подключает `ue_paint`. Открыты DrawBox,
DrawLine, DrawLines, DrawSpline, DrawTextFormatted. Generated inout обёртки
возвращают `UEPaintContext` того же текущего callback; можно использовать
исходное значение или результат для следующей команды.

## Время жизни

UEPaintContext — непрозрачный epoch token текущего VM/NativePaint. Geometry,
culling rect, widget style и draw list остаются ссылками только на native stack.
Снимки, reflected storage/defaults, JSON и native контейнеры запрещены.
Сохранение токена в global/array/clone не продлевает его жизнь. Default, другой
callback, другой контекст, старые call frames и вызовы вне OnPaint отклоняются.
Reload/NativeDestruct освобождают контекст; paint не ставится в очередь ticker.
Рекурсивный вход в VM из paint запрещён; используется native parent fallback.

## Публикация draw-команд

Во время DAS handler копируются только owned параметры команд: векторы,
цвета, точки, текст и ссылки на ресурсы. Brush/font pin-ятся до конца callback.
Реальный FPaintContext никогда не копируется в persistent call frames или DAS.
Native draw-функции выполняются последовательно только после успешного
возврата handler и повторной проверки виджета. Ошибка, panic, invalid token,
лимит или NativeDestruct отменяют всю script-порцию команд; родительская
отрисовка остаётся. Код не очищает и не подменяет родительский draw list.

Сначала выполняется `Super::NativePaint`, затем script-порция поверх его
итогового слоя. Возвращается фактический MaxLayer native функций. Native no-op
поведение (пустая/вырожденная DrawLines, null Brush/Font) сохраняется.
Последняя ошибка доступна в `LastPaintError`.

## Ограничения

- Gameplay game thread, живой game-world widget, idle VM, exact scalar allowlist.
- Не более 1024 команд, 4096 точек в вызове, 65536 точек на callback.
- Текст: 32768 UTF-16 code units в вызове, суммарно 131072 на callback.
- Координаты/scalars/colors конечны и ограничены по модулю 1e7; размеры box
  неотрицательны. Проверяется переполнение Slate float transform/layer.
- Thickness: 0..10000; FontSize: больше 0 и не больше 4096.
- Общие guards для аргументов, enum, signature, construction и execution role
  сохраняются. Paint не разрешает EventReply-команды вне input callback.

Машинный контракт: `Bindings/PaintContext.contract.json`.

## Целевые проверки

`Scripts/TestPaint.ps1 -SkipBuild -Publish`: свежий export/generator/preflight,
пять ready функций и настоящий `SObjectWidget.Paint` с null Slate renderer.
Проверяется содержимое FSlateWindowElementList и слои после всех пяти draw calls,
отказы stale/default tokens и frames, ошибка после команды без частичной
публикации, limits/invalid input, NativeDestruct, reload, reentrant guard и GC
ресурса между постановкой команды и commit. Отдельно — три compile-time
попытки подделать token и восемь generator tests. Sources/DLL/BuildId сверяются.
Отчёт: `logs/paint-validation.json`; пример: `Tests/Paint/probe.template.das`.
Полные Foundation/PIE/packaged, GPU rendering и видимое окно не запускаются.
