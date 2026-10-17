# Montage и события анимации

Публичный модуль — `ue_animation.das`, пример — `dsl_montage_actor.das`.

```dascript
require ue_animation

[ue_class(Blueprintable)]
class CombatActor : Actor {
    @ue_property=(EditAnywhere,BlueprintReadWrite)
    Attack:AnimMontageRef
    @ue_component
    @RootComponent
    Mesh:SkeletalMeshComponentRef

    [ue_function(BlueprintCallable)]
    def PlayAttack():float {
        return ue_montage_play("attack",self.Mesh,self.Attack,"OnAttack")
    }
    [ue_function(BlueprintCallable)]
    def OnAttack(event:MontageEvent):void {
        if(event.Kind==MontageEventKind.NotifyBegin) {
            // event.NotifyName — FName из Montage Notify / Montage Notify Window.
        }
        if(event.Kind==MontageEventKind.Completed) { /* нормальное завершение */ }
        if(event.Kind==MontageEventKind.Interrupted) { /* прервано */ }
    }
}
```

На mesh нужно назначить SkeletalMesh и подходящий AnimInstance/AnimBlueprint.
Для видимой анимации в AnimGraph должен быть Slot, соответствующий Montage.
Пример показывает логику привязки; готовый анимированный персонаж не поставляется.

`ue_montage_play(name,mesh,montage,handler,rate=1,position=0,section=NAME_None)`
возвращает длительность из Unreal или 0, если движок отказался начать
воспроизведение. До вызова проверяются ссылки, игровой мир, совместимость skeleton,
положительная конечная скорость, позиция, секция и контракт обработчика.
`SoftAnimMontage` можно использовать с существующим async asset loading.

Обработчик — `[ue_function] def Handler(event:MontageEvent):void` в DSL-классе.
Имя метода передаётся строковым литералом; макрос переводит его в экспортируемую
точку входа. Сигнатура проверяется по authoring-контракту.

`MontageEvent.Kind`:

| Значение | Когда приходит |
| --- | --- |
| Completed | Завершено без прерывания. |
| Interrupted | Прерванное воспроизведение окончательно закончилось. |
| BlendOut | Началось обычное, непрерванное затухание. |
| NotifyBegin | Montage Notify либо начало Montage Notify Window. |
| NotifyEnd | Конец Montage Notify Window. |

`NotifyName` заполнен у notify-событий, у остальных — NAME_None. В этом API
используются нативные `OnPlayMontageNotifyBegin/End`; произвольные классы
AnimNotify и функции AnimBlueprint вида `AnimNotify_*` автоматически не подключаются.

## Время жизни

- `ue_montage_stop(name,blend_out=0.1)` останавливает конкретный принадлежащий
  подписке экземпляр и оставляет доставку Interrupted. Возвращает false, если
  активного экземпляра уже нет.
- `ue_montage_cancel(name)` отзывает подписку и останавливает её экземпляр без
  пользовательских callbacks. Уже поставленные события отбрасываются.
- `ue_montage_playing(name)` проверяет активный экземпляр. Во время завершения
  blend-out может уже возвращать false до доставки терминального события.
- Повторное имя заменяет предыдущую подписку этого контекста. Экземпляры
  различаются native instance ID, даже если проигрывается тот же ассет.
- Завершение/прерывание освобождает подписку перед вызовом обработчика: из него
  можно безопасно запустить следующую анимацию с тем же именем.

События доставляются на game thread через managed subscription ID после текущего
прохода анимации. Нативные делегаты не содержат указателей на VM/функции daScript.
Destroy/EndPlay, отмена и успешная замена VM при hot reload отзывают ID,
отключают свои делегаты и останавливают только свои экземпляры. Отклонённый reload
сохраняет действующую VM, воспроизведение и обработчики. Автоматического повторного
запуска Montage после успешного reload нет.

Адаптер использует per-instance end/blend delegates и собственные подписки на
notify multicast. Если end/blend делегаты нового экземпляра уже заняты сторонним
кодом, адаптер отказывается их перезаписывать. При очистке снимаются только
делегаты, всё ещё принадлежащие адаптеру. Новый запуск подчиняется обычным правилам
Unreal о прерывании Montage в одной группе; очистка использует instance ID.

## Целевая проверка

```powershell
.\Scripts\TestMontage.ps1
# После сборки:
.\Scripts\TestMontage.ps1 -SkipBuild
```

Команда создаёт только Blueprint примера и отдельную карту в свежей папке.
Game-процесс использует Engine SkeletalCube, transient Montage и настоящий
Montage Notify Window. Время Montage продвигается через публичный `UAnimInstance::TickMontageOnly`
без рендера. Синтетические notify используются только для проверки неправильного
instance ID, отмены уже поставленных событий и границы hot reload/Destroy.
Для reload создаётся уникальная временная копия probe; оригинальные исходники
не меняются, временная копия удаляется при завершении проверки.

Отчёт — `logs/montage-validation.json`. Полный Foundation/PIE/packaged прогон,
видимые позы, root motion и сетевое воспроизведение не входят в эту проверку.

Текущий результат: десять групп целевых сценариев и отказ для неверной
сигнатуры callback при authoring — PASS. Также проверены
нулевая скорость, отсутствующие handler/asset/section и сохранение текущего
воспроизведения после отказа. Коммит не создавался.

## Повторный запуск из Completed

В `dsl_montage_actor.das` есть переключатель `RepeatOnComplete`. Обработчик
сначала сбрасывает его, затем вызывает `self.Play()` с тем же именем `action`.
Это даёт один дополнительный цикл, без бесконечного повтора. Старая подписка
уже отозвана перед callback и не должна отменить новую.

Точечный runner дополнительно проверяет этот сценарий, а также сохранность
нативного montage в другой группе после play/stop/replace/cancel нашей подписки.
В UE 5.8 используемый `Montage_Play(...,true)` останавливает проигрывания той же
группы через `StopAllMontagesByGroupName`; флаг не означает остановку всех групп.
Правило одного root-motion montage остаётся правилом движка и здесь не проверяется.

`montage_bad_callback.das` — отрицательный authoring-пример. Его обработчик
принимает `int` вместо `MontageEvent`; экспортёр должен отказать именно с
диагностикой несовпадения montage callback, до создания тестового Blueprint.

Проверено 2026-10-18 в основном DasForUESmoke: Editor Development собран,
`TestMontage.ps1 -SkipBuild` прошёл на карте
`/Game/DasMontage/e1a8db3234444c52a62abe99d47568a5/MontageMap`.
Новый тест сторонней группы использует montage со стартовой секцией Default;
без секции тестовый экземпляр самостоятельно завершался при продвижении времени.
Изменять правила остановки групп в адаптере не потребовалось.
Полные Foundation/PIE/packaged и рендер анимации не проверялись. Без коммита.
