# ue_api_pawn_ai: отчёт генерации

Движок: `5.8.3-0+UE5`. Состояние: `ue_preflight_passed`.

| Класс | Готовы | Export blockers | Generation blockers | Policy exclusions |
| --- | ---: | ---: | ---: | ---: |
| AIBlueprintHelperLibrary | 15 | 0 | 0 | 1 |
| AIController | 19 | 2 | 0 | 0 |
| AIPerceptionComponent | 9 | 1 | 0 | 0 |
| PathFollowingComponent | 0 | 4 | 0 | 0 |
| Character | 18 | 31 | 0 | 0 |
| CharacterMovementComponent | 39 | 5 | 0 | 0 |
| Controller | 21 | 7 | 0 | 0 |
| NavMovementComponent | 6 | 0 | 0 | 0 |
| Pawn | 26 | 8 | 0 | 0 |
| PawnMovementComponent | 6 | 2 | 0 | 0 |
| PlayerController | 66 | 96 | 0 | 0 |
| NavigationPath | 9 | 0 | 0 | 0 |
| NavigationSystemV1 | 7 | 12 | 0 | 0 |

Всего 410 кандидатов, 410 уникальных объявлений. Construction guarded: 6.
Из 241 готовых объявлений 21 уже сгенерированы в baseline-пакетах; 220 отсутствуют в них. Это не число новых возможностей всей привязки.
Baseline: ue_api_world, ue_api_math, ue_api_collision.

## Группы блокеров

| Группа | Уникальных объявлений |
| --- | ---: |
| other | 20 |
| special_call_adapter | 1 |
| visibility_or_deprecation_policy | 148 |

## Подсказки отражённого native-аудита

Это отдельная policy-оценка тех же объявлений. Она уточняет пути проблемных полей, но не перечисляет все будущие generic ABI blockers. Устранение одного типа не гарантирует прохождения функции.

| Тип | Уникальных объявлений с проблемой |
| --- | ---: |
| `ArrayProperty: TArray<uint8>` | 7 |
| `StructProperty: FCharacterMoveResponsePackedBits` | 1 |
| `StructProperty: FCharacterServerMovePackedBits` | 1 |
| `StructProperty: FColor` | 1 |
| `StructProperty: FGameplayResourceSet` | 1 |
| `StructProperty: FRootMotionSourceSettings` | 1 |
| `UInt32Property: uint32` | 9 |
| `WeakObjectProperty: TWeakObjectPtr<UObject>` | 5 |
| `WeakObjectProperty: TWeakObjectPtr<UPhysicalMaterial>` | 5 |
| `WeakObjectProperty: TWeakObjectPtr<UPrimitiveComponent>` | 5 |

Числа типов пересекаются: несколько weak-полей одного HitResult считаются одним объявлением для каждого типа.

## Поимённый backlog

| Функция | Стадия / причина | Пути проблемных полей (native policy) |
| --- | --- | --- |
| AIBlueprintHelperLibrary.CreateMoveToProxyObject | policy_excluded: Use the lifecycle-managed ue_ai MoveTo adapter; raw async proxy callbacks and ownership are outside the generic call contract. | — |
| AIController.OnGameplayTaskResourcesClaimed | export_blocked: Expected loaded class and local BlueprintCallable instance function: /Script/AIModule.AIController.OnGameplayTaskResourcesClaimed | `NewlyClaimed`, `FreshlyReleased` |
| AIController.OnUsingBlackBoard | export_blocked: Expected loaded class and local BlueprintCallable instance function: /Script/AIModule.AIController.OnUsingBlackBoard | — |
| AIPerceptionComponent.OnOwnerEndPlay | export_blocked: Expected loaded class and local BlueprintCallable instance function: /Script/AIModule.AIPerceptionComponent.OnOwnerEndPlay | — |
| PathFollowingComponent.GetPathActionType | export_blocked: deprecated_function | — |
| PathFollowingComponent.GetPathDestination | export_blocked: deprecated_function | — |
| PathFollowingComponent.OnActorBump | export_blocked: Expected loaded class and local BlueprintCallable instance function: /Script/AIModule.PathFollowingComponent.OnActorBump | `Hit.PhysMaterial`, `Hit.HitObjectHandle.ReferenceObject`, `Hit.Component` |
| PathFollowingComponent.OnNavDataRegistered | export_blocked: Expected loaded class and local BlueprintCallable instance function: /Script/AIModule.PathFollowingComponent.OnNavDataRegistered | — |
| Character.CanJumpInternal | export_blocked: Expected loaded class and local BlueprintCallable instance function: /Script/Engine.Character.CanJumpInternal | — |
| Character.ClientAckGoodMove | export_blocked: Expected loaded class and local BlueprintCallable instance function: /Script/Engine.Character.ClientAckGoodMove | — |
| Character.ClientAdjustPosition | export_blocked: Expected loaded class and local BlueprintCallable instance function: /Script/Engine.Character.ClientAdjustPosition | — |
| Character.ClientAdjustRootMotionPosition | export_blocked: Expected loaded class and local BlueprintCallable instance function: /Script/Engine.Character.ClientAdjustRootMotionPosition | — |
| Character.ClientAdjustRootMotionSourcePosition | export_blocked: Expected loaded class and local BlueprintCallable instance function: /Script/Engine.Character.ClientAdjustRootMotionSourcePosition | `ServerRootMotion.LastAccumulatedSettings` |
| Character.ClientCheatFly | export_blocked: Expected loaded class and local BlueprintCallable instance function: /Script/Engine.Character.ClientCheatFly | — |
| Character.ClientCheatGhost | export_blocked: Expected loaded class and local BlueprintCallable instance function: /Script/Engine.Character.ClientCheatGhost | — |
| Character.ClientCheatWalk | export_blocked: Expected loaded class and local BlueprintCallable instance function: /Script/Engine.Character.ClientCheatWalk | — |
| Character.ClientMoveResponsePacked | export_blocked: Expected loaded class and local BlueprintCallable instance function: /Script/Engine.Character.ClientMoveResponsePacked | `PackedBits` |
| Character.ClientVeryShortAdjustPosition | export_blocked: Expected loaded class and local BlueprintCallable instance function: /Script/Engine.Character.ClientVeryShortAdjustPosition | — |
| Character.K2_OnEndCrouch | export_blocked: Expected loaded class and local BlueprintCallable instance function: /Script/Engine.Character.K2_OnEndCrouch | — |
| Character.K2_OnMovementModeChanged | export_blocked: Expected loaded class and local BlueprintCallable instance function: /Script/Engine.Character.K2_OnMovementModeChanged | — |
| Character.K2_OnStartCrouch | export_blocked: Expected loaded class and local BlueprintCallable instance function: /Script/Engine.Character.K2_OnStartCrouch | — |
| Character.K2_UpdateCustomMovement | export_blocked: Expected loaded class and local BlueprintCallable instance function: /Script/Engine.Character.K2_UpdateCustomMovement | — |
| Character.OnJumped | export_blocked: Expected loaded class and local BlueprintCallable instance function: /Script/Engine.Character.OnJumped | — |
| Character.OnLanded | export_blocked: Expected loaded class and local BlueprintCallable instance function: /Script/Engine.Character.OnLanded | `Hit.PhysMaterial`, `Hit.HitObjectHandle.ReferenceObject`, `Hit.Component` |
| Character.OnLaunched | export_blocked: Expected loaded class and local BlueprintCallable instance function: /Script/Engine.Character.OnLaunched | — |
| Character.OnRep_IsCharacterMovementActive | export_blocked: Expected loaded class and local BlueprintCallable instance function: /Script/Engine.Character.OnRep_IsCharacterMovementActive | — |
| Character.OnRep_IsCrouched | export_blocked: Expected loaded class and local BlueprintCallable instance function: /Script/Engine.Character.OnRep_IsCrouched | — |
| Character.OnRep_ReplayLastTransformUpdateTimeStamp | export_blocked: Expected loaded class and local BlueprintCallable instance function: /Script/Engine.Character.OnRep_ReplayLastTransformUpdateTimeStamp | — |
| Character.OnRep_ReplicatedBasedMovement | export_blocked: Expected loaded class and local BlueprintCallable instance function: /Script/Engine.Character.OnRep_ReplicatedBasedMovement | — |
| Character.OnRep_RootMotion | export_blocked: Expected loaded class and local BlueprintCallable instance function: /Script/Engine.Character.OnRep_RootMotion | — |
| Character.OnWalkingOffLedge | export_blocked: Expected loaded class and local BlueprintCallable instance function: /Script/Engine.Character.OnWalkingOffLedge | — |
| Character.RootMotionDebugClientPrintOnScreen | export_blocked: Expected loaded class and local BlueprintCallable instance function: /Script/Engine.Character.RootMotionDebugClientPrintOnScreen | — |
| Character.ServerMove | export_blocked: Expected loaded class and local BlueprintCallable instance function: /Script/Engine.Character.ServerMove | `View` |
| Character.ServerMoveDual | export_blocked: Expected loaded class and local BlueprintCallable instance function: /Script/Engine.Character.ServerMoveDual | `View0`, `View` |
| Character.ServerMoveDualHybridRootMotion | export_blocked: Expected loaded class and local BlueprintCallable instance function: /Script/Engine.Character.ServerMoveDualHybridRootMotion | `View0`, `View` |
| Character.ServerMoveDualNoBase | export_blocked: Expected loaded class and local BlueprintCallable instance function: /Script/Engine.Character.ServerMoveDualNoBase | `View0`, `View` |
| Character.ServerMoveNoBase | export_blocked: Expected loaded class and local BlueprintCallable instance function: /Script/Engine.Character.ServerMoveNoBase | `View` |
| Character.ServerMoveOld | export_blocked: Expected loaded class and local BlueprintCallable instance function: /Script/Engine.Character.ServerMoveOld | — |
| Character.ServerMovePacked | export_blocked: Expected loaded class and local BlueprintCallable instance function: /Script/Engine.Character.ServerMovePacked | `PackedBits` |
| CharacterMovementComponent.CapsuleTouched | export_blocked: Expected loaded class and local BlueprintCallable instance function: /Script/Engine.CharacterMovementComponent.CapsuleTouched | `SweepResult.PhysMaterial`, `SweepResult.HitObjectHandle.ReferenceObject`, `SweepResult.Component` |
| CharacterMovementComponent.GetMovementBase | export_blocked: deprecated_function | — |
| CharacterMovementComponent.SetAvoidanceGroup | export_blocked: deprecated_function | — |
| CharacterMovementComponent.SetGroupsToAvoid | export_blocked: deprecated_function | — |
| CharacterMovementComponent.SetGroupsToIgnore | export_blocked: deprecated_function | — |
| Controller.ClientSetLocation | export_blocked: Expected loaded class and local BlueprintCallable instance function: /Script/Engine.Controller.ClientSetLocation | — |
| Controller.ClientSetRotation | export_blocked: Expected loaded class and local BlueprintCallable instance function: /Script/Engine.Controller.ClientSetRotation | — |
| Controller.OnRep_Pawn | export_blocked: Expected loaded class and local BlueprintCallable instance function: /Script/Engine.Controller.OnRep_Pawn | — |
| Controller.OnRep_PlayerState | export_blocked: Expected loaded class and local BlueprintCallable instance function: /Script/Engine.Controller.OnRep_PlayerState | — |
| Controller.ReceiveInstigatedAnyDamage | export_blocked: Expected loaded class and local BlueprintCallable instance function: /Script/Engine.Controller.ReceiveInstigatedAnyDamage | — |
| Controller.ReceivePossess | export_blocked: Expected loaded class and local BlueprintCallable instance function: /Script/Engine.Controller.ReceivePossess | — |
| Controller.ReceiveUnPossess | export_blocked: Expected loaded class and local BlueprintCallable instance function: /Script/Engine.Controller.ReceiveUnPossess | — |
| Pawn.GetMovementBaseActor | export_blocked: requires_native_function_library | — |
| Pawn.GetMovementBaseOwner | export_blocked: requires_native_function_library | — |
| Pawn.OnRep_Controller | export_blocked: Expected loaded class and local BlueprintCallable instance function: /Script/Engine.Pawn.OnRep_Controller | — |
| Pawn.OnRep_PlayerState | export_blocked: Expected loaded class and local BlueprintCallable instance function: /Script/Engine.Pawn.OnRep_PlayerState | — |
| Pawn.ReceiveControllerChanged | export_blocked: Expected loaded class and local BlueprintCallable instance function: /Script/Engine.Pawn.ReceiveControllerChanged | — |
| Pawn.ReceivePossessed | export_blocked: Expected loaded class and local BlueprintCallable instance function: /Script/Engine.Pawn.ReceivePossessed | — |
| Pawn.ReceiveRestarted | export_blocked: Expected loaded class and local BlueprintCallable instance function: /Script/Engine.Pawn.ReceiveRestarted | — |
| Pawn.ReceiveUnpossessed | export_blocked: Expected loaded class and local BlueprintCallable instance function: /Script/Engine.Pawn.ReceiveUnpossessed | — |
| PawnMovementComponent.MulticastAsyncPhysicsStateAction | export_blocked: Expected loaded class and local BlueprintCallable instance function: /Script/Engine.PawnMovementComponent.MulticastAsyncPhysicsStateAction | — |
| PawnMovementComponent.ServerAsyncPhysicsStateAction | export_blocked: Expected loaded class and local BlueprintCallable instance function: /Script/Engine.PawnMovementComponent.ServerAsyncPhysicsStateAction | — |
| PlayerController.Camera | export_blocked: Expected loaded class and local BlueprintCallable instance function: /Script/Engine.PlayerController.Camera | — |
| PlayerController.ClientAckTimeDilation | export_blocked: Expected loaded class and local BlueprintCallable instance function: /Script/Engine.PlayerController.ClientAckTimeDilation | — |
| PlayerController.ClientAckUpdateLevelVisibility | export_blocked: Expected loaded class and local BlueprintCallable instance function: /Script/Engine.PlayerController.ClientAckUpdateLevelVisibility | `TransactionId.Data` |
| PlayerController.ClientAddTextureStreamingLoc | export_blocked: Expected loaded class and local BlueprintCallable instance function: /Script/Engine.PlayerController.ClientAddTextureStreamingLoc | — |
| PlayerController.ClientCancelPendingMapChange | export_blocked: Expected loaded class and local BlueprintCallable instance function: /Script/Engine.PlayerController.ClientCancelPendingMapChange | — |
| PlayerController.ClientCapBandwidth | export_blocked: Expected loaded class and local BlueprintCallable instance function: /Script/Engine.PlayerController.ClientCapBandwidth | — |
| PlayerController.ClientClearCameraLensEffects | export_blocked: Expected loaded class and local BlueprintCallable instance function: /Script/Engine.PlayerController.ClientClearCameraLensEffects | — |
| PlayerController.ClientCommitMapChange | export_blocked: Expected loaded class and local BlueprintCallable instance function: /Script/Engine.PlayerController.ClientCommitMapChange | — |
| PlayerController.ClientEnableNetworkVoice | export_blocked: Expected loaded class and local BlueprintCallable instance function: /Script/Engine.PlayerController.ClientEnableNetworkVoice | — |
| PlayerController.ClientEndOnlineSession | export_blocked: Expected loaded class and local BlueprintCallable instance function: /Script/Engine.PlayerController.ClientEndOnlineSession | — |
| PlayerController.ClientFlushLevelStreaming | export_blocked: Expected loaded class and local BlueprintCallable instance function: /Script/Engine.PlayerController.ClientFlushLevelStreaming | — |
| PlayerController.ClientForceGarbageCollection | export_blocked: Expected loaded class and local BlueprintCallable instance function: /Script/Engine.PlayerController.ClientForceGarbageCollection | — |
| PlayerController.ClientGameEnded | export_blocked: Expected loaded class and local BlueprintCallable instance function: /Script/Engine.PlayerController.ClientGameEnded | — |
| PlayerController.ClientGotoState | export_blocked: Expected loaded class and local BlueprintCallable instance function: /Script/Engine.PlayerController.ClientGotoState | — |
| PlayerController.ClientIgnoreLookInput | export_blocked: Expected loaded class and local BlueprintCallable instance function: /Script/Engine.PlayerController.ClientIgnoreLookInput | — |
| PlayerController.ClientIgnoreMoveInput | export_blocked: Expected loaded class and local BlueprintCallable instance function: /Script/Engine.PlayerController.ClientIgnoreMoveInput | — |
| PlayerController.ClientMessage | export_blocked: Expected loaded class and local BlueprintCallable instance function: /Script/Engine.PlayerController.ClientMessage | — |
| PlayerController.ClientMutePlayer | export_blocked: Expected loaded class and local BlueprintCallable instance function: /Script/Engine.PlayerController.ClientMutePlayer | `PlayerId.ReplicationBytes` |
| PlayerController.ClientPlayForceFeedback_Internal | export_blocked: Expected loaded class and local BlueprintCallable instance function: /Script/Engine.PlayerController.ClientPlayForceFeedback_Internal | — |
| PlayerController.ClientPlaySound | export_blocked: Expected loaded class and local BlueprintCallable instance function: /Script/Engine.PlayerController.ClientPlaySound | — |
| PlayerController.ClientPlaySoundAtLocation | export_blocked: Expected loaded class and local BlueprintCallable instance function: /Script/Engine.PlayerController.ClientPlaySoundAtLocation | — |
| PlayerController.ClientPrepareMapChange | export_blocked: Expected loaded class and local BlueprintCallable instance function: /Script/Engine.PlayerController.ClientPrepareMapChange | — |
| PlayerController.ClientPrestreamTextures | export_blocked: Expected loaded class and local BlueprintCallable instance function: /Script/Engine.PlayerController.ClientPrestreamTextures | — |
| PlayerController.ClientReceiveLocalizedMessage | export_blocked: Expected loaded class and local BlueprintCallable instance function: /Script/Engine.PlayerController.ClientReceiveLocalizedMessage | — |
| PlayerController.ClientRepObjRef | export_blocked: Expected loaded class and local BlueprintCallable instance function: /Script/Engine.PlayerController.ClientRepObjRef | — |
| PlayerController.ClientReset | export_blocked: Expected loaded class and local BlueprintCallable instance function: /Script/Engine.PlayerController.ClientReset | — |
| PlayerController.ClientRestart | export_blocked: Expected loaded class and local BlueprintCallable instance function: /Script/Engine.PlayerController.ClientRestart | — |
| PlayerController.ClientRetryClientRestart | export_blocked: Expected loaded class and local BlueprintCallable instance function: /Script/Engine.PlayerController.ClientRetryClientRestart | — |
| PlayerController.ClientReturnToMainMenuWithTextReason | export_blocked: Expected loaded class and local BlueprintCallable instance function: /Script/Engine.PlayerController.ClientReturnToMainMenuWithTextReason | — |
| PlayerController.ClientSetBlockOnAsyncLoading | export_blocked: Expected loaded class and local BlueprintCallable instance function: /Script/Engine.PlayerController.ClientSetBlockOnAsyncLoading | — |
| PlayerController.ClientSetCameraFade | export_blocked: Expected loaded class and local BlueprintCallable instance function: /Script/Engine.PlayerController.ClientSetCameraFade | `FadeColor` |
| PlayerController.ClientSetCameraMode | export_blocked: Expected loaded class and local BlueprintCallable instance function: /Script/Engine.PlayerController.ClientSetCameraMode | — |
| PlayerController.ClientSetCinematicMode | export_blocked: Expected loaded class and local BlueprintCallable instance function: /Script/Engine.PlayerController.ClientSetCinematicMode | — |
| PlayerController.ClientSetForceMipLevelsToBeResident | export_blocked: Expected loaded class and local BlueprintCallable instance function: /Script/Engine.PlayerController.ClientSetForceMipLevelsToBeResident | — |
| PlayerController.ClientSetHUD | export_blocked: Expected loaded class and local BlueprintCallable instance function: /Script/Engine.PlayerController.ClientSetHUD | — |
| PlayerController.ClientSetSpectatorWaiting | export_blocked: Expected loaded class and local BlueprintCallable instance function: /Script/Engine.PlayerController.ClientSetSpectatorWaiting | — |
| PlayerController.ClientSetViewTarget | export_blocked: Expected loaded class and local BlueprintCallable instance function: /Script/Engine.PlayerController.ClientSetViewTarget | — |
| PlayerController.ClientSetupNetworkPhysicsTimestamp | export_blocked: Expected loaded class and local BlueprintCallable instance function: /Script/Engine.PlayerController.ClientSetupNetworkPhysicsTimestamp | — |
| PlayerController.ClientSpawnCameraLensEffect | export_blocked: Expected loaded class and local BlueprintCallable instance function: /Script/Engine.PlayerController.ClientSpawnCameraLensEffect | — |
| PlayerController.ClientSpawnGenericCameraLensEffect | export_blocked: Expected loaded class and local BlueprintCallable instance function: /Script/Engine.PlayerController.ClientSpawnGenericCameraLensEffect | — |
| PlayerController.ClientStartCameraShake | export_blocked: Expected loaded class and local BlueprintCallable instance function: /Script/Engine.PlayerController.ClientStartCameraShake | — |
| PlayerController.ClientStartOnlineSession | export_blocked: Expected loaded class and local BlueprintCallable instance function: /Script/Engine.PlayerController.ClientStartOnlineSession | — |
| PlayerController.ClientStopCameraShake | export_blocked: Expected loaded class and local BlueprintCallable instance function: /Script/Engine.PlayerController.ClientStopCameraShake | — |
| PlayerController.ClientStopForceFeedback | export_blocked: Expected loaded class and local BlueprintCallable instance function: /Script/Engine.PlayerController.ClientStopForceFeedback | — |
| PlayerController.ClientTeamMessage | export_blocked: Expected loaded class and local BlueprintCallable instance function: /Script/Engine.PlayerController.ClientTeamMessage | — |
| PlayerController.ClientTravel | export_blocked: Expected loaded class and local BlueprintCallable instance function: /Script/Engine.PlayerController.ClientTravel | — |
| PlayerController.ClientTravelInternal | export_blocked: Expected loaded class and local BlueprintCallable instance function: /Script/Engine.PlayerController.ClientTravelInternal | — |
| PlayerController.ClientUnmutePlayer | export_blocked: Expected loaded class and local BlueprintCallable instance function: /Script/Engine.PlayerController.ClientUnmutePlayer | `PlayerId.ReplicationBytes` |
| PlayerController.ClientUnmutePlayers | export_blocked: Expected loaded class and local BlueprintCallable instance function: /Script/Engine.PlayerController.ClientUnmutePlayers | `PlayerIds[].ReplicationBytes` |
| PlayerController.ClientUpdateLevelStreamingStatus | export_blocked: Expected loaded class and local BlueprintCallable instance function: /Script/Engine.PlayerController.ClientUpdateLevelStreamingStatus | `TransactionId.Data` |
| PlayerController.ClientUpdateMultipleLevelsStreamingStatus | export_blocked: Expected loaded class and local BlueprintCallable instance function: /Script/Engine.PlayerController.ClientUpdateMultipleLevelsStreamingStatus | — |
| PlayerController.ClientVoiceHandshakeComplete | export_blocked: Expected loaded class and local BlueprintCallable instance function: /Script/Engine.PlayerController.ClientVoiceHandshakeComplete | — |
| PlayerController.ClientWasKicked | export_blocked: Expected loaded class and local BlueprintCallable instance function: /Script/Engine.PlayerController.ClientWasKicked | — |
| PlayerController.ConsoleKey | export_blocked: Expected loaded class and local BlueprintCallable instance function: /Script/Engine.PlayerController.ConsoleKey | — |
| PlayerController.EnableCheats | export_blocked: Expected loaded class and local BlueprintCallable instance function: /Script/Engine.PlayerController.EnableCheats | — |
| PlayerController.FOV | export_blocked: Expected loaded class and local BlueprintCallable instance function: /Script/Engine.PlayerController.FOV | — |
| PlayerController.GetHitResultUnderCursor | export_blocked: deprecated_function | `HitResult.PhysMaterial`, `HitResult.HitObjectHandle.ReferenceObject`, `HitResult.Component` |
| PlayerController.GetHitResultUnderFinger | export_blocked: deprecated_function | `HitResult.PhysMaterial`, `HitResult.HitObjectHandle.ReferenceObject`, `HitResult.Component` |
| PlayerController.LocalTravel | export_blocked: Expected loaded class and local BlueprintCallable instance function: /Script/Engine.PlayerController.LocalTravel | — |
| PlayerController.OnServerStartedVisualLogger | export_blocked: Expected loaded class and local BlueprintCallable instance function: /Script/Engine.PlayerController.OnServerStartedVisualLogger | — |
| PlayerController.Pause | export_blocked: Expected loaded class and local BlueprintCallable instance function: /Script/Engine.PlayerController.Pause | — |
| PlayerController.PlayDynamicForceFeedback | export_blocked: requires_special_call_adapter | — |
| PlayerController.RestartLevel | export_blocked: Expected loaded class and local BlueprintCallable instance function: /Script/Engine.PlayerController.RestartLevel | — |
| PlayerController.SendToConsole | export_blocked: Expected loaded class and local BlueprintCallable instance function: /Script/Engine.PlayerController.SendToConsole | — |
| PlayerController.ServerAcknowledgePossession | export_blocked: Expected loaded class and local BlueprintCallable instance function: /Script/Engine.PlayerController.ServerAcknowledgePossession | — |
| PlayerController.ServerBlockPlayer | export_blocked: Expected loaded class and local BlueprintCallable instance function: /Script/Engine.PlayerController.ServerBlockPlayer | `PlayerId.ReplicationBytes` |
| PlayerController.ServerCamera | export_blocked: Expected loaded class and local BlueprintCallable instance function: /Script/Engine.PlayerController.ServerCamera | — |
| PlayerController.ServerChangeName | export_blocked: Expected loaded class and local BlueprintCallable instance function: /Script/Engine.PlayerController.ServerChangeName | — |
| PlayerController.ServerCheckClientPossession | export_blocked: Expected loaded class and local BlueprintCallable instance function: /Script/Engine.PlayerController.ServerCheckClientPossession | — |
| PlayerController.ServerCheckClientPossessionReliable | export_blocked: Expected loaded class and local BlueprintCallable instance function: /Script/Engine.PlayerController.ServerCheckClientPossessionReliable | — |
| PlayerController.ServerExec | export_blocked: Expected loaded class and local BlueprintCallable instance function: /Script/Engine.PlayerController.ServerExec | — |
| PlayerController.ServerExecRPC | export_blocked: Expected loaded class and local BlueprintCallable instance function: /Script/Engine.PlayerController.ServerExecRPC | — |
| PlayerController.ServerMutePlayer | export_blocked: Expected loaded class and local BlueprintCallable instance function: /Script/Engine.PlayerController.ServerMutePlayer | `PlayerId.ReplicationBytes` |
| PlayerController.ServerNotifyLoadedWorld | export_blocked: Expected loaded class and local BlueprintCallable instance function: /Script/Engine.PlayerController.ServerNotifyLoadedWorld | — |
| PlayerController.ServerPause | export_blocked: Expected loaded class and local BlueprintCallable instance function: /Script/Engine.PlayerController.ServerPause | — |
| PlayerController.ServerRestartPlayer | export_blocked: Expected loaded class and local BlueprintCallable instance function: /Script/Engine.PlayerController.ServerRestartPlayer | — |
| PlayerController.ServerSendLatestAsyncPhysicsTimestamp | export_blocked: Expected loaded class and local BlueprintCallable instance function: /Script/Engine.PlayerController.ServerSendLatestAsyncPhysicsTimestamp | — |
| PlayerController.ServerSetSpectatorLocation | export_blocked: Expected loaded class and local BlueprintCallable instance function: /Script/Engine.PlayerController.ServerSetSpectatorLocation | — |
| PlayerController.ServerSetSpectatorWaiting | export_blocked: Expected loaded class and local BlueprintCallable instance function: /Script/Engine.PlayerController.ServerSetSpectatorWaiting | — |
| PlayerController.ServerShortTimeout | export_blocked: Expected loaded class and local BlueprintCallable instance function: /Script/Engine.PlayerController.ServerShortTimeout | — |
| PlayerController.ServerToggleAILogging | export_blocked: Expected loaded class and local BlueprintCallable instance function: /Script/Engine.PlayerController.ServerToggleAILogging | — |
| PlayerController.ServerUnblockPlayer | export_blocked: Expected loaded class and local BlueprintCallable instance function: /Script/Engine.PlayerController.ServerUnblockPlayer | `PlayerId.ReplicationBytes` |
| PlayerController.ServerUnmutePlayer | export_blocked: Expected loaded class and local BlueprintCallable instance function: /Script/Engine.PlayerController.ServerUnmutePlayer | `PlayerId.ReplicationBytes` |
| PlayerController.ServerUpdateCamera | export_blocked: Expected loaded class and local BlueprintCallable instance function: /Script/Engine.PlayerController.ServerUpdateCamera | — |
| PlayerController.ServerUpdateContainerVisibility | export_blocked: Expected loaded class and local BlueprintCallable instance function: /Script/Engine.PlayerController.ServerUpdateContainerVisibility | — |
| PlayerController.ServerUpdateLevelVisibility | export_blocked: Expected loaded class and local BlueprintCallable instance function: /Script/Engine.PlayerController.ServerUpdateLevelVisibility | `LevelVisibility.VisibilityRequestId.Data` |
| PlayerController.ServerUpdateMultipleLevelsVisibility | export_blocked: Expected loaded class and local BlueprintCallable instance function: /Script/Engine.PlayerController.ServerUpdateMultipleLevelsVisibility | `LevelVisibilities[].VisibilityRequestId.Data` |
| PlayerController.ServerVerifyViewTarget | export_blocked: Expected loaded class and local BlueprintCallable instance function: /Script/Engine.PlayerController.ServerVerifyViewTarget | — |
| PlayerController.ServerViewNextPlayer | export_blocked: Expected loaded class and local BlueprintCallable instance function: /Script/Engine.PlayerController.ServerViewNextPlayer | — |
| PlayerController.ServerViewPrevPlayer | export_blocked: Expected loaded class and local BlueprintCallable instance function: /Script/Engine.PlayerController.ServerViewPrevPlayer | — |
| PlayerController.ServerViewSelf | export_blocked: Expected loaded class and local BlueprintCallable instance function: /Script/Engine.PlayerController.ServerViewSelf | — |
| PlayerController.SetName | export_blocked: Expected loaded class and local BlueprintCallable instance function: /Script/Engine.PlayerController.SetName | — |
| PlayerController.StartFire | export_blocked: Expected loaded class and local BlueprintCallable instance function: /Script/Engine.PlayerController.StartFire | — |
| PlayerController.SwitchLevel | export_blocked: Expected loaded class and local BlueprintCallable instance function: /Script/Engine.PlayerController.SwitchLevel | — |
| PlayerController.TestServerLevelVisibilityChange | export_blocked: Expected loaded class and local BlueprintCallable instance function: /Script/Engine.PlayerController.TestServerLevelVisibilityChange | — |
| PlayerController.ToggleSpeaking | export_blocked: Expected loaded class and local BlueprintCallable instance function: /Script/Engine.PlayerController.ToggleSpeaking | — |
| NavigationSystemV1.FindPathToActorSynchronously | export_blocked: requires_native_function_library | — |
| NavigationSystemV1.FindPathToLocationSynchronously | export_blocked: requires_native_function_library | — |
| NavigationSystemV1.GetNavigationSystem | export_blocked: requires_native_function_library | — |
| NavigationSystemV1.GetPathCost | export_blocked: requires_native_function_library | — |
| NavigationSystemV1.GetPathLength | export_blocked: requires_native_function_library | — |
| NavigationSystemV1.IsNavigationBeingBuilt | export_blocked: requires_native_function_library | — |
| NavigationSystemV1.IsNavigationBeingBuiltOrLocked | export_blocked: requires_native_function_library | — |
| NavigationSystemV1.K2_GetRandomLocationInNavigableRadius | export_blocked: requires_native_function_library | — |
| NavigationSystemV1.K2_GetRandomPointInNavigableRadius | export_blocked: deprecated_function | — |
| NavigationSystemV1.K2_GetRandomReachablePointInRadius | export_blocked: requires_native_function_library | — |
| NavigationSystemV1.K2_ProjectPointToNavigation | export_blocked: requires_native_function_library | — |
| NavigationSystemV1.NavigationRaycast | export_blocked: requires_native_function_library | — |
