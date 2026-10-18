# Точечная проверка AI Perception

UE 5.8.3, 2026-10-18. Runner: `Scripts/TestPerception.ps1`;
native fixture: `DasPerceptionCommandlet.cpp` и `UDasPerceptionObserver`;
DAS handler: `Tests/Perception/probe.template.das`.

```powershell
./Scripts/TestPerception.ps1
./Scripts/TestPerception.ps1 -SkipBuild
./Scripts/TestPerception.ps1 -SkipBuild -Report <successful-pawn-ai-run/report.json>
```

Без Report экспортируется свежий Pawn/AI пакет и компилируется весь staged
модуль. Затем запускается только perception commandlet в transient Game world.
Никаких Blueprint/assets/maps не создаётся и не сохраняется.

## Настоящие стимулы и обработчики

Создаются реальный AISystem/PerceptionSystem, AIController с Pawn и
AIPerceptionComponent. Sight: радиус 1000 см, loss radius 1200 см;
hearing: радиус 3000 см; MaxAge обоих sense — 0.5 сек. Все affiliation-флаги
включены, поэтому neutral TargetPoint не исключается фильтром команд.
Tick мира идёт по 0.05 сек с увеличением GFrameCounter: обычный UE tick
сам обрабатывает senses, доставку и aging. Ожидания ограничены 120 шагами.

- TargetPoint в (300,0,64) регистрируется как sight source и обнаруживается.
- После переноса в (2000,0,64) sight теряется. Current query пуст,
  known query ещё содержит цель до MaxAge.
- `AISense_Hearing.ReportNoiseEvent` от реального source в этой позиции
  создаёт noise с loudness=2 и Tag=DasNoise. Hearing обнаруживает его,
  хотя цель уже за пределами sight. Проверены Tag, Strength и location snapshot.
- Через 3 сек hearing больше не считается current, повторной success-доставки нет.
- Шум в (10000,0,64) с MaxRange=1000 не доставляется.
- После ReleaseObject подписка удаляется, но native listener продолжает
  получать sight при возврате source. Новая DAS session не содержит старых
  callback counters. Перед уничтожением listener session явно освобождается.

Стимулы обрабатывает UE: fixture не вызывает RegisterStimulus и не делает
ручной Broadcast делегата. Native observer независимо проверяет Actor identity,
sense ID и hearing payload. Результат: sight_on=2, sight_off=1, hearing_on=1.
Дополнительно проверены Invalid для native default stimulus, Other для зарегистрированного
AISense_Damage, отказ при несовместимом struct и совместимость исходного raw tuple.

Использован общий multi-argument callback backend с узкой проекцией:
`OnTargetPerceptionUpdated(Actor, FAIStimulus)` → owned `AIStimulus` → DAS.
Компилятор обычного `[ue_event]` создаёт tuple и `bind_events` автоматически.
В примере больше нет `@FRAME@`, ручного binder, `arg_0`/`arg_1` и строкового
чтения полей стимула. Сгенерированные `ue_perception_*` запросы сохранены.

## Типизированный AIStimulus

`require ue_perception` импортирует `AIStimulus`, `AISense` и обычный `ActorRef`.
Это owned snapshot, не указатель на память AIPerceptionComponent. Поля:

| Поле | Тип / значение |
| --- | --- |
| Sense | AISense.Invalid, Sight, Hearing или Other |
| SuccessfullySensed | native WasSuccessfullySensed() |
| Expired | native IsExpired() |
| Valid | native IsValid(); это не синоним активного восприятия |
| Age | native GetAge() на момент callback |
| ExpirationAge | отражённый UE ExpirationAge |
| Strength | float |
| StimulusLocation, ReceiverLocation | UEVector |
| Tag | UENameValue |

Возраст snapshot не увеличивается сам по себе. Изменение локального значения
не меняет native perception. FAISenseID не UPROPERTY и имеет process-local ID:
проектор определяет Sight/Hearing по native sense ID, не по Tag, имени актора
или фиксированному числу. Невалидный ID → Invalid, другой валидный sense → Other.

```das
require ue_perception

[ue_class]
class PerceptionProbe : Actor {
    @ue_component
    Perception:PerceptionComponentRef

    [ue_event(source="Perception",name="OnTargetPerceptionUpdated")]
    def OnTarget(actor:ActorRef; stimulus:AIStimulus):void {
        if(stimulus.Sense==AISense.Sight && stimulus.SuccessfullySensed) {
            ue_state_set_int("seen",ue_state_get_int("seen",0)+1)
        }
    }
}
```

В fixture DSL descriptor проверяется через ReadAuthoringScript, затем сгенерированный
binder исполняется на ADasPerceptionHost (native AIController с BlueprintReadOnly
полем Perception); Blueprint не создаётся. Права доступа к полям UE не меняются.
Прямое наследование DSL от AIPerceptionComponent не расширялось: пример использует
поддерживаемую композицию Actor + native component.
Проекция разрешена только для точной пары native FAIStimulus → FDasAIStimulus
в multi-argument callback frame. Совпадение полей другого struct не разрешает
подмену типа. Обычный raw FAIStimulus snapshot остаётся допустимым, а generic
property/call ABI и запись обратно в FAIStimulus не менялись. Новых UE externs
и отдельного perception dispatcher нет.

## Результат и границы

Editor Development build, компиляция всего импортированного модуля и 15 targeted checks — PASS.
Engine Error/ensure нет; SHA256 DLL, fixture/template и staged module не менялись
во время проверки. Отчёт: `logs/perception-validation.json`; подробный runtime.log
сохраняется в указанном в нём staged run. Коммит автоматически не создаётся.

Не покрыты sight occlusion, team-policy combinations, hearing LOS, forgotten/info
delegates, уничтожение source во время queued delivery, perception reload,
BehaviorTree/EQS, полный PIE, packaged и сеть. Типизированная проекция на этом
этапе проверяется для OnTargetPerceptionUpdated; single-parameter и другие
формы native perception events отдельно не расширялись.
