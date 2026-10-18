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

Использован существующий общий multi-argument callback backend:
`OnTargetPerceptionUpdated(Actor, FAIStimulus)` → owned callback tuple → DAS.
Тест регистрирует внутренний tuple через DasCallbackFrame и подставляет его
identity в `ue_subscribe_object_struct`. Обработчик принимает UEStructValue,
читает arg_0/arg_1 и обновляет persistent counters. Запросы current/known actors
выполняются сгенерированными `ue_perception_*` функциями из staged пакета.
Новых UE externs и отдельного perception dispatcher не добавлялось.

## Результат и границы

Editor Development build, whole-module preflight и 11 targeted checks — PASS.
Engine Error/ensure нет; SHA256 DLL, fixture/template и staged module не менялись
во время проверки. Отчёт: `logs/perception-validation.json`; подробный runtime.log
сохраняется в указанном в нём staged run. Коммит автоматически не создаётся.

Здесь проверен snapshot-handler, а не новый пользовательский typed perception
DSL. `FAIStimulus.Type` не является UPROPERTY; sense identity в native baseline
проверяется непосредственно, DAS отличает используемые stimulus по Tag.
Отдельный удобный typed stimulus/sense view этим шагом не добавлялся.
Не покрыты sight occlusion, team-policy combinations, hearing LOS, forgotten/info
delegates, уничтожение source во время queued delivery, perception reload,
BehaviorTree/EQS, полный PIE, packaged и сеть. Это следующие независимые задачи,
а не заявление о завершении всей AI-подсистемы.
