# Звук и Niagara

Модуль `ue_effects` предоставляет управляемые эффекты в позиции мира:
звук `SoundBaseRef`, Niagara `NiagaraSystemRef` или оба одновременно.
Доступны также типизированные soft-ссылки `SoftSound` и `SoftNiagaraSystem`.

```dascript
require ue_effects

// Внутри метода ue_class; PickupSound/PickupVFX — ue_property.
if (ue_effect_at("pickup", self.PickupSound, self.PickupVFX,
    self.Trigger.K2_GetComponentLocation(), "OnEffectFinished",
    0.35f, 1.4f, 0.4f)) {
    self.EffectsStarted += 1
}

[ue_function(BlueprintCallable)]
def OnEffectFinished() {
    self.EffectsFinished += 1
}
```

`ue_effect_play` принимает UETransform; `ue_effect_at` — позицию и необязательный
равномерный scale. Последние параметры: handler, volume, pitch; handler по
умолчанию пустой. `ue_sound_play` и `ue_niagara_spawn` запускают один вид эффекта.
`ue_effect_active(name)`, `ue_effect_stop(name)` и `ue_effect_stop_all()` работают
только с эффектами текущего экземпляра скрипта. Последний возвращает число
остановленных эффектов. В пределах экземпляра разрешено до 64 активных имён.

Повторный запуск с тем же именем заменяет предыдущий эффект. Компоненты создаются
как transient instance components владельца Actor; они удерживаются через GC.
После завершения обеих частей компоненты удаляются, затем один раз вызывается
необязательный `ue_function` без параметров и результата. Отсутствие handler не
отключает автоматическую очистку. Явная остановка не вызывает completion.

Destroy/EndPlay и успешный hot reload останавливают эффекты, снимают собственные
делегаты и отзывают уже поставленный в очередь callback до замены VM. Отклонённый
hot reload сохраняет работающую версию и её подписки. Чужие компоненты владельца
не затрагиваются. Постоянно работающий Niagara нужно остановить явно либо вместе
с владельцем; callback ожидает завершения всей пары.

Пустая пара ассетов, dedicated server или ещё не готовая NiagaraSystem дают
`false`, сохраняя предыдущий эффект с таким именем. Неверные типы, callback и
нечисловые параметры отклоняются; volume разрешён 0..16, pitch — (0,4].
Отложенный запуск до готовности системы пока не реализован.

## Пример и проверки

`dsl_playable_pickup.das`: звук и вспышка после подбора, счёт и восстановление
предмета. `dsl_playable_character.das`: отдельный эффект получения положительного
урона через OnTakeAnyDamage. В Blueprint редактируются PickupSound/PickupVFX и
HurtSound/HurtVFX. Экспорт demo создаёт собственные NS_Pickup/NS_Hurt из Niagara
RadialBurst; звук — Engine 1kSineTonePing с разными pitch и volume.

```powershell
.\Scripts\CreatePlayableCharacter.ps1 -SkipBuild
.\Scripts\TestEffects.ps1 -SkipBuild
```

Семь целевых групп — PASS (`logs/effects-validation.json`): подбор/урон,
естественное завершение audio + CPU Niagara, замена и остановка, отдельный
звук/Niagara, успешный и отклонённый reload, Destroy и поздние callbacks.
Искусственное завершение используется только для проверки границ отмены.
Тест запускается с настоящим RHI в offscreen-режиме: Niagara под NullRHI
отказывается запускаться. Качество изображения/звука, networking, packaged,
Foundation и полная PIE suite в этом этапе не проверялись.

Привязка к сокетам, fade, attenuation override и Niagara user parameters остаются
следующими расширениями; текущий API запускает эффект в позиции мира.
