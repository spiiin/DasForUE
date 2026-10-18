# Аудит покрытия API генератора

Движок: `5.8.3-0+UE5`. Область: AIModule, Engine, EnhancedInput, NavigationSystem, Niagara, UMG.
Учитываются загруженные отражённые native UClass после явной загрузки модулей. Это не инвентаризация всего C++ Unreal и не процент готовности привязки.

Проверено классов: **1938**, выбранных receiver-классов в этой области: **19**. Отдельно исключено интерфейсов/устаревших классов: 96.
Уникальных объявлений членов: **16401**. С учётом наследования на всех receiver-классах: **110125** вхождений. Эти два счётчика нельзя смешивать.
Из уникальных объявлений native-фильтр принимает 4546: 1231 уже зарегистрированы хотя бы для одного receiver, 3315 пока не зарегистрированы ни для одного.

## Регистрация и пропуски

| Состояние | Вхождения на receiver-классах |
| --- | ---: |
| Отклонено существующим library exporter | 455 |
| Прошло существующий library exporter, обёртка не создана | 1762 |
| Есть статическая DAS-обёртка, сигнатура совпадает | 3 |
| Проходит reflection-фильтр, но receiver/член не зарегистрирован | 39195 |
| Есть в native registry, текущая сигнатура совпадает | 2998 |
| Исключено политикой видимости/назначения | 55836 |
| Требует пересмотра политики construction | 3251 |
| Нужен специальный путь/адаптер | 2097 |
| Ограничение типов без других исключений | 4528 |

Регистрация среди проходящих native reflection-фильтр: 2998/42193 (7.1%). Это доля регистраций на receiver-классах, не доля всего API UE и не runtime-валидация.

`eligible` означает результат reflection/export policy. Конфликты имён типов, состав зависимостей пакета, вывод типов DAS, runtime и packaged здесь не доказываются. Library-функции проверены настоящим ExportBindings; статические вызовы не записываются автоматически в неподдержанный API.

## Подсистемы

| Подсистема | Классов | Native registry | Eligible, не выбран | Library: обёртки / eligible без обёрток | Type / ABI | Adapter | Policy |
| --- | ---: | ---: | ---: | ---: | ---: | ---: | ---: |
| ai_navigation | 212 | 0 | 3637 | 0 / 49 | 337 / 0 | 115 | 5570 |
| animation | 21 | 462 | 1634 | 0 / 0 | 149 / 0 | 30 | 1537 |
| assets_and_engine_other | 977 | 32 | 2152 | 0 / 0 | 93 / 0 | 71 | 16249 |
| audio | 18 | 13 | 267 | 0 / 0 | 55 / 0 | 39 | 391 |
| function_libraries | 56 | 0 | 0 | 3 / 1532 | 0 / 0 | 0 | 76 |
| input | 43 | 21 | 181 | 0 / 9 | 60 / 0 | 0 | 227 |
| niagara | 221 | 7 | 1545 | 0 / 73 | 143 / 0 | 43 | 3287 |
| pawn_controller_movement | 17 | 741 | 1412 | 0 / 0 | 165 / 0 | 76 | 2129 |
| scene_collision_physics | 71 | 1515 | 10872 | 0 / 0 | 1329 / 0 | 146 | 7665 |
| ui | 141 | 97 | 4169 | 0 / 99 | 449 / 0 | 996 | 3510 |
| world_actor_components | 161 | 110 | 13326 | 0 / 0 | 1748 / 0 | 581 | 18446 |

Подсистемы задаются упорядоченными правилами в `Bindings/ApiCoverage.profile.json`: первое совпадение по модулю или базовому классу. Оставшиеся Engine-классы вынесены в assets_and_engine_other. Это техническая группировка, а не официальная классификация Epic.

## Причины отказов

Ниже — уникальные объявления. Причины пересекаются: один метод может одновременно быть latent и иметь неподдержанный параметр. Их суммы не являются числом пропущенных методов.

| Причина | Уникальных объявлений |
| --- | ---: |
| `not_blueprint_visible` | 7781 |
| `unsupported_property_type` | 1533 |
| `editor_only` | 1343 |
| `not_blueprint_callable` | 665 |
| `native_accessor` | 421 |
| `deprecated` | 402 |
| `unsupported_parameter_type` | 269 |
| `not_native` | 243 |
| `not_public` | 188 |
| `static` | 128 |
| `metadata:DeprecatedFunction` | 126 |
| `net` | 117 |
| `metadata:UnsafeDuringActorConstruction` | 97 |
| `metadata:BlueprintInternalUseOnly` | 95 |
| `delegate_function` | 76 |
| `metadata:WorldContext` | 60 |
| `metadata:CustomThunk` | 21 |
| `metadata:BlueprintProtected` | 13 |
| `metadata:Latent` | 1 |

### Отдельный путь Function Libraries

Проверка выполняется существующим ExportBindings с optional-запросом для каждой public native static BlueprintCallable функции. Неудачи не прерывают остальные запросы. Причины ниже относятся к этому экспортёру; static/WorldContext сами по себе не считаются его ошибками.

| Причина | Функций на library-классах |
| --- | ---: |
| `unsupported UFunction or struct field type ` | 254 |
| `requires_special_call_adapter` | 84 |
| `internal_function` | 69 |
| `deprecated_function` | 33 |
| `structs with native object references are unsupported ` | 14 |
| `WorldContext must name an input object parameter ` | 1 |

## Общие ограничения типов с измеримым эффектом

Только члены, у которых native-фильтр не нашёл иных причин отказа. Число кандидатов с одной формой означает отсутствие других форм в текущей диагностике, а не обещание полной поддержки после одного изменения. Диагностика показывает неподдержанные листья либо ограничение их внешнего контейнера/структуры.

| FProperty | C++ тип | Затронуто уникальных | Кандидаты с одной формой | Пример |
| --- | --- | ---: | ---: | --- |
| WeakObjectProperty | `TWeakObjectPtr<UObject>` | 47 | 0 | `field:/Script/Engine.CharacterMovementComponent.CurrentFloor` |
| WeakObjectProperty | `TWeakObjectPtr<UPhysicalMaterial>` | 43 | 0 | `field:/Script/Engine.CharacterMovementComponent.CurrentFloor` |
| WeakObjectProperty | `TWeakObjectPtr<UPrimitiveComponent>` | 43 | 0 | `field:/Script/Engine.CharacterMovementComponent.CurrentFloor` |
| StructProperty | `FColor` | 28 | 28 | `field:/Script/AIModule.AISenseConfig.DebugColor` |
| StructProperty | `FPrimitiveInstanceId` | 18 | 18 | `method:/Script/Engine.InstancedSkinnedMeshComponent.AddInstance` |
| StructProperty | `FKeyHandleMap` | 17 | 5 | `field:/Script/Engine.AnimDataModel.AnimatedBoneAttributes` |
| OptionalProperty | `TOptional<float>` | 11 | 0 | `field:/Script/Engine.AudioVolume.SubmixSendSettings` |
| UInt32Property | `uint32` | 9 | 5 | `method:/Script/Engine.InputDeviceSubsystem.ActivateDevicePropertyOfClass` |
| DelegateProperty | `FCustomWidgetNavigationDelegate` | 8 | 2 | `field:/Script/UMG.WidgetNavigation.Down` |
| EnumProperty | `ETranslucencyType` | 6 | 0 | `field:/Script/Engine.CameraComponent.PostProcessSettings` |
| InterfaceProperty | `TScriptInterface<IBlendableInterface>` | 6 | 6 | `method:/Script/Engine.CameraComponent.AddOrUpdateBlendable` |
| StructProperty | `FLinearColor [fixed dim 8]` | 6 | 0 | `field:/Script/Engine.CameraComponent.PostProcessSettings` |
| StructProperty | `FMatteBoxFlag [fixed dim 3]` | 6 | 0 | `field:/Script/Engine.CameraComponent.PostProcessSettings` |
| StructProperty | `FScriptTypedElementHandle` | 6 | 3 | `method:/Script/Engine.TypedElementCommonActions.DuplicateNormalizedElements` |
| StructProperty | `FScriptTypedElementListProxy` | 6 | 3 | `method:/Script/Engine.TypedElementCommonActions.CopyNormalizedElements` |
| WeakObjectProperty | `TWeakObjectPtr<UWidget>` | 6 | 0 | `field:/Script/UMG.WidgetNavigation.Down` |
| DelegateProperty | `FWidgetAnimationDynamicEvent` | 5 | 5 | `method:/Script/UMG.UserWidget.BindToAnimationEvent` |
| StructProperty | `FPlayerKeyMapping` | 5 | 5 | `field:/Script/EnhancedInput.EnhancedPlayerMappableKeyProfile.PlayerMappedKeys` |
| StructProperty | `FSoundAttenuationPluginSettings` | 5 | 0 | `field:/Script/Engine.SoundAttenuation.Attenuation` |
| ByteProperty | `TEnumAsByte<EBlendMode>` | 4 | 4 | `field:/Script/Engine.Material.BlendMode` |
| FloatProperty | `float [fixed dim 4]` | 4 | 4 | `method:/Script/Engine.SkeletalMesh.GetMaterials` |
| Int64Property | `int64` | 4 | 4 | `method:/Script/Engine.SoundWave.GetCuePoints` |
| StructProperty | `FInputActionValue` | 4 | 4 | `field:/Script/EnhancedInput.InputTrigger.LastValue` |
| StructProperty | `FWidgetAnimationHandle` | 4 | 4 | `method:/Script/UMG.UserWidget.PlayAnimation` |
| WeakObjectProperty | `TWeakObjectPtr<USceneComponent>` | 4 | 4 | `field:/Script/Engine.AudioComponent.AutoAttachParent` |

### Call-frame ABI

| Диагностика | Уникальных объявлений |
| --- | ---: |

## Возможности общей политики

У этих объявлений есть только указанная причина отказа. Это кандидаты на общий механизм, а не разрешение снять защитные проверки.

| Причина | Уникальных объявлений |
| --- | ---: |
| `native_accessor` | 401 |
| `metadata:UnsafeDuringActorConstruction` | 84 |

## Как воспроизвести

```powershell
.\Scripts\AuditApiCoverage.ps1
# Если экспортёр уже собран:
.\Scripts\AuditApiCoverage.ps1 -SkipBuild
```

- `Bindings/ApiCoverage.summary.json` — компактный снимок результатов и provenance SHA-256.
- `Build/ApiCoverage/report.json` — уникальные члены, все причины, неподдержанные типы и состояния.
- `Build/ApiCoverage/members.csv` — каждое вхождение на receiver-классе, удобно искать конкретный API.
- `Build/ApiCoverage/Reflection.json` — reflection inventory; соседний `.libraries.json` — фактический результат library exporter.
- `Scripts/TestApiCoverage.py` — целевые проверки подсчёта, наследования, нескольких причин и stale-сигнатур.

Запуск не расширяет runtime registry, не генерирует новые игровые привязки и не меняет plugin dependencies. Внешние native/C++ методы, неотражённые поля, произвольные virtual override и отключённые плагины находятся вне этого отчёта.

Managed helpers (MoveTo, Montage, effects, async и др.) не засчитываются как автоматически сгенерированные члены. Для них нужен отдельный контракт подсистемы; наличие адаптера не означает покрытия всех одноимённых методов reflection.

Состояния native относятся к native receiver registry. Наличие альтернативных явных instance-обёрток ExportBindings в ue_generated/ue_library_generated этим счётчиком не измеряется. Поэтому type_blocked или policy_review не означают, что вызов вообще невозможен через другой backend. Проверка статических обёрток читает только явно перечисленные generated-модули и сопоставляет literal target + signature, без выполнения функций.

Диагностика неподдержанных типов ограничена 64 записями на член и глубиной 16; она помогает найти причины, но не является полным рекурсивным описанием каждого отвергнутого типа. AudioMixer, GameplayAbilities, StateTree и другие модули вне профиля здесь не оценивались.

Текущие проверки аудита: необходимая сборка, reflection export, проверка неизменности native registry и целевые Python-тесты. Полные Foundation/PIE/packaged не выполняются.
