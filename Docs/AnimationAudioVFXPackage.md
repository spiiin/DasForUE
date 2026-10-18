# Пакет Animation / Audio / Niagara

Профиль: `Bindings/Packages/AnimationAudioVFX.profile.json`.
Модуль: `ue_api_animation_audio_vfx`; UE modules: Engine, AnimGraphRuntime,
AudioMixer, Niagara. Это следующий пакет после Pawn / Character / AI / Navigation
в [ApiExpansionPlan](ApiExpansionPlan.md).

Выбраны четыре Function Libraries и девять receiver-классов. Каждый include=*
означает весь локальный reflected API выбранного класса; inherited копии не
размножаются. Возвращаемые assets остаются типизированными ссылками, даже если
у соответствующего asset-класса нет собственных BlueprintCallable методов.
AnimSequenceBase и NiagaraSystem не добавлены как receivers: у них нет локальных
UFUNCTION в данном снимке. SoundBase содержит одно служебное объявление без
BlueprintCallable, оно явно отражено в backlog, а не посчитано готовой функцией.

## Результат — UE 5.8.3, 2026-10-18

Из **580** кандидатов: **514** generation_ready, **57** export_blocked,
**9** generation_blocked, 0 policy_excluded. Тридцать wrappers сохраняют
construction guard. Весь staged модуль успешно скомпилирован в UE.

| Класс / library | Ready | Export blocked | Generation blocked |
| --- | ---: | ---: | ---: |
| KismetAnimationLibrary | 10 | 0 | 1 |
| AudioMixerBlueprintLibrary | 33 | 3 | 4 |
| AnimInstance | 77 | 36 | 0 |
| AnimMontage | 13 | 0 | 0 |
| AnimSequence | 3 | 1 | 0 |
| AudioComponent | 38 | 4 | 4 |
| SkeletalMeshComponent | 116 | 8 | 0 |
| SkinnedMeshComponent | 51 | 3 | 0 |
| SoundBase | 0 | 1 | 0 |
| NiagaraComponent | 92 | 0 | 0 |
| NiagaraDataInterfaceArrayFunctionLibrary | 44 | 0 | 0 |
| NiagaraFunctionLibrary | 21 | 1 | 0 |
| NiagaraParameterCollectionInstance | 16 | 0 | 0 |

Типы Vector/Rotator/Transform используют canonical_math; defaults=explicit.
Существующие call frames, signature/world/context guards и UObject handles
используются без нового C++ dispatcher и без ручных wrappers на каждую функцию.
Managed montage/playback/effect callbacks остаются в существующих `ue_animation`
и `ue_effects`; генерация reflected API не добавляет lifecycle гарантии для
любых новых async операций.

## Проверка в принятом сокращённом режиме

```powershell
./Scripts/TestAnimationAudioVFXPackage.ps1
# Когда Editor уже собран:
./Scripts/TestAnimationAudioVFXPackage.ps1 -SkipBuild
```

Runner собирает Editor, экспортирует reflection, генерирует пакет и запускает
**один** короткий runtime probe. В probe `KismetAnimationLibrary.CalculateDirection`
получает velocity=(0,100,0), rotation=(0,0,0) и возвращает 90 градусов.
Этот вызов проверяет новую library-обёртку и передачу UEVector/UERotator/float.
Полная компиляция модуля не означает выполнение остальных 513 функций.

Сборка и probe — PASS; Error/ensure отсутствуют, hashes DLL/profile/generators
не изменялись во время проверки. Отчёт: `logs/animation-audio-vfx-validation.json`;
машинный backlog: `Bindings/Packages/ue_api_animation_audio_vfx.summary.json`.
Полные Foundation/PIE/packaged, animation playback, аудиовывод и Niagara simulation
не запускались. Дополнительные runtime commandlets и регрессионные suites не
добавлялись.

## Открытые формы сигнатур

| Функция | Причина отказа генератора |
| --- | --- |
| KismetAnimationLibrary.K2_CalculateVelocityFromSockets | Opaque/empty structs need an adapter |
| AudioMixerBlueprintLibrary.GetAvailableAudioOutputDevices | Unsupported or colliding field: onobtaindevicesevent |
| AudioMixerBlueprintLibrary.GetCurrentAudioOutputDeviceName | Unsupported or colliding field: onobtaincurrentdeviceevent |
| AudioMixerBlueprintLibrary.PrimeSoundForPlayback | Unsupported or colliding field: onloadcompletion |
| AudioMixerBlueprintLibrary.SwapAudioOutputDevice | Unsupported or colliding field: oncompleteddeviceswap |
| AudioComponent.AddModulationRouting | Unsupported hash container member |
| AudioComponent.GetModulators | Unsupported hash container member |
| AudioComponent.RemoveModulationRouting | Unsupported hash container member |
| AudioComponent.SetModulationRouting | Unsupported hash container member |

Async device/prime callbacks требуют явного managed-контракта; object sets в
modulation routing — расширения общего hash-container ABI; spring/socket state
не нужно объявлять поддержанным через пустой struct. Export blockers отдельно
сохраняют internal/deprecated policy, soft/native-reference структуры,
construction/constraint contract и instance WorldContext ограничения. Override
события AnimInstance не являются обычными callable-функциями, поэтому их отказы
не означают, что весь AnimInstance отсутствует.

## Использование и публикация

Пакет пока staged в `Build/ApiPackages/ue_api_animation_audio_vfx/<run>/`.
Проверенный hash-suffixed модуль не подменяет установленную копию. Для публикации
существующий общий конвейер поддерживает:

```powershell
./Scripts/GenerateApiPackage.ps1 -Profile ./Bindings/Packages/AnimationAudioVFX.profile.json -SkipBuild -Publish
```

После публикации пример использует обычный require:

```das
options gen2
require ue_api_animation_audio_vfx
[export]
def main(input:int):int {
    let angle=ue_anim_math_CalculateDirection(ue_vector(0.0lf,100.0lf,0.0lf),ue_rotator(0.0lf,0.0lf,0.0lf))
    return int(angle)
}
```

Следующий основной пакет по плану — UMG / Input. Отдельные blockers этого пакета
можно исправлять под конкретную игровую потребность без полного прогона после
каждого подключения класса.
