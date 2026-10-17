# Трассировки, UMG и здоровье

Исходники публичного API: `ue_gameplay_services.das`. Игровой пример использует
`dsl_playable_character.das`, `dsl_playable_pickup.das`, `dsl_health_component.das`
и общий Blueprint-интерфейс из `ue_interaction_contract.das`.

## Трассировки

```dascript
let hit=ue_line_trace(start,finish)
if(hit.blocking_hit) {
    let actor=ue_hit_actor(hit)
    let component=ue_hit_component(hit)
    // actor/component — проверяемые слабые ссылки, не удерживающие цель от GC.
}
let sweep=ue_sphere_trace(start,finish,40.0lf)
var ignored:array<ActorRef>
push(ignored,actor)
let next=ue_line_trace(start,finish,ignored,TraceChannel.Visibility)
```

`UETraceHit` содержит `blocking_hit`, `start_penetrating`, `distance`, `time`,
`location`, `impact_point`, `normal`, `impact_normal`, `face_index`. Это обычное
значение внутри daScript с отдельными проверяемыми ссылками, а не snapshot всей
внутренней структуры `FHitResult`. Его можно сохранять в переменной; удалённые
цели возвращаются как пустые ссылки. Ссылки проверяются в контексте, создавшем
результат; неинициализированный результат отклоняется. Отражённый transport
`UETraceHit` через `ue_property` / `ue_function` пока не добавлен.

По умолчанию запрос игнорирует собственного Actor (или владельца Component).
Доступны channel, complex и ignore_self; есть вариант с массивом игнорируемых
Actor. Проверяются радиус, конечность координат, канал и принадлежность миру.
Это single-hit LineTrace/SphereTrace; multi/object/profile queries и физический
материал не входят в текущий API.

## UMG

```dascript
// HUDClass: $UEClass<UserWidgetRef>; может ссылаться на Widget Blueprint.
self.HUD=ue_widget_create(self.HUDClass,player)
verify(ue_widget_show(self.HUD),"HUD cannot be shown")
ue_widget_set_text(self.HUD,"Score",ue_text_invariant("Score: {self.Score}"))
ue_widget_bind_click(self.HUD,"Damage","HurtFromHUD")
ue_widget_input(self.HUD,true)
// Позже:
ue_widget_remove(self.HUD)
```

Имя текста должно находить `TextBlock`, кнопки — `Button`. Обработчик —
`BlueprintCallable` метод владельца контекста без параметров и результата.
Текст сохраняет семантику FText. Пример создаёт настоящий `WBP_HUD`, наследующий
нативный `UDasGameplayHUD` с готовым деревом виджетов; API работает и с собственным
деревом Widget Blueprint при соответствующих именах элементов.

Контекст удерживает созданные виджеты от GC. Повторная привязка той же кнопки
заменяет его предыдущий обработчик. Click доставляется на следующем проходе
CoreTicker с повторной проверкой владельца, виджета и метода. Удаление виджета,
уничтожение владельца или замена контекста отключают обработчики и отменяют ещё
не доставленные click. Завершение контекста снимает HUD с экрана.

`ue_widget_input` включает GameAndUI и курсор; выключение/очистка возвращает
GameOnly и прежнюю видимость курсора. Это управление вводом для одного владельца;
произвольный внешний UI input-mode stack не восстанавливается. Создание требует
живого игрового мира и локального PlayerController; выделенный сервер отклоняется.
UMG binding здесь ограничен созданием/показом, именованным текстом и click,
а не полным доступом ко всем Slate/UMG API.

## Damage и HealthComponent

`ue_apply_damage(target,amount,source,instigator)` вызывает нативный
`UGameplayStatics::ApplyDamage`. Проверяются конечная неотрицательная величина,
живые объекты одного мира и authority. Автоматическая репликация не добавлена.

HealthComponent — полностью авторский daScript ActorComponent. Он подписывается
на `owner.OnTakeAnyDamage`, ограничивает здоровье снизу нулём, предоставляет Heal
с ограничением сверху Maximum и отправляет `Changed(current,maximum)`.
Персонаж слушает Changed и обновляет HUD; урон поступает по H и от кнопки HUD.

## Запуск и проверка

`Scripts/CreatePlayableCharacter.ps1` создаёт свежую папку ассетов и записывает
путь карты в `logs/playable-example.json`. WASD — движение, мышь — обзор,
Space — прыжок, E — взаимодействие по направлению камеры, H — 25 урона,
Tab — включение/выключение курсора. Подбор при пересечении также работает.

`Scripts/TestGameplayFeatures.ps1 [-SkipBuild]` выполняет только проверку этого
среза в отдельном `-game -NullRHI` процессе. Отчёт:
`logs/gameplay-features-validation.json`. Рендеринг, физические устройства ввода,
Foundation, полный PIE-набор и packaged build этой командой не проверяются.
