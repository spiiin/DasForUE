# AIController и управляемый MoveTo

Модуль `ue_ai` предоставляет типизированный `AIControllerRef` и управляемые
запросы движения к Actor или позиции. `ue_ai_self_controller()` получает уже
существующий AIController владельца Pawn; если скрипт принадлежит AIController,
возвращает его самого. Для ActorComponent используется его владелец. Сам helper
не создаёт контроллер: задайте AIControllerClass и AutoPossessAI в Blueprint.

```dascript
require ue_ai

// Внутри метода ue_class; Target — типизированный ActorRef.
let controller=ue_ai_self_controller()
ue_ai_move_to_actor("chase",controller,self.Target,"OnMoveFinished",40.0f)

[ue_function(BlueprintCallable)]
def OnMoveFinished(result:MoveResult):void {
    if(result.Code==MoveCode.Success) {
        // Прибыл к цели.
    }
}
```

`ue_ai_move_to_location` принимает UEVector вместо ActorRef. Последние аргументы
обоих helpers: необязательный handler, acceptance radius (по умолчанию 50),
pathfinding=true, partial=false. Радиус конечный и неотрицательный; UE включает
радиус агента в проверку достижения цели. Позиционная цель проецируется на NavMesh
при pathfinding=true. Для pathfinding=false проекция отключена; обход препятствий
в этом режиме не обеспечивается.

Результат `bool` означает, что UE принял запрос или уже находится у цели.
Отказ поиска пути возвращает false и ставит в очередь MoveResult.Invalid.
AlreadyAtGoal ставит Success. Ошибки типов, owner, authority, аргументов и callback
отклоняются до замены предыдущего запроса. Результат содержит Code и UE RequestId:
Success, Blocked, OffPath, Aborted, Invalid. Callback без handler не вызывается,
но subscription автоматически освобождается. Все callbacks отложены; они не
вызываются внутри стартового вызова daScript.

`ue_ai_move_active(name)` проверяет активный запрос текущего экземпляра скрипта;
`ue_ai_move_cancel(name)` снимает его callback и отменяет движение без callback.
На контроллере может выполняться одно движение. Новый управляемый запрос
заменяет предыдущий в текущем session, даже если имя отличается. Внешний MoveTo
может вытеснить запрос: его нормальный результат — Aborted. Очистка старого
subscription не отменяет новое внешнее движение и не снимает чужие listeners.

Managed proxy хранит UE request ID и weak-ссылки, без указателей на VM. При
Destroy/EndPlay и успешном hot reload снимается только его delegate и отменяется
именно его текущий запрос. Отложенный callback отзывается. Отклонённый reload
оставляет рабочий запрос и handler. Движение требует authority, живого Pawn в
текущем game world и соответствия контроллера владельцу скрипта; управлять чужим
контроллером этим API нельзя.

## Пример противника

`dsl_enemy_character.das`: настоящий Character с AIController, простым видимым
mesh и меткой Enemy. Раз в 0.25 сек он проверяет цель и запускает MoveToActor.
В радиусе атаки отменяет преследование и вызывает ApplyDamage с AI-инстигатором;
AttackDamage/AttackRange/AttackInterval/Target редактируются в Blueprint.
StopBrain/ResumeBrain управляют таймером и движением. После успешного logic reload
BeginPlay повторно не вызывается: для возобновления таймера вызовите ResumeBrain.

```powershell
.\Scripts\CreateAIEnemy.ps1 -SkipBuild
```

Команда создаёт отдельную `/Game/DasExamples/AI/<id>/PlayableMap`: существующий
игровой Character/HUD/pickup плюс противник, препятствие и сохранённый NavMesh.
Обычный CreatePlayableCharacter не добавляет противника. Путь карты хранится в
`logs/ai-example.json`. Откройте карту и нажмите Play, уходите от противника WASD.
При настройках по умолчанию противник медленнее игрока и атакует раз в секунду.

Это базовая навигация и ближняя атака. Perception, Behavior Tree, StateTree,
EQS, сетевой сценарий и полноценная боевая анимация в этот этап не входят.

## Целевые проверки

`Scripts/TestAI.ps1 -SkipBuild` использует отдельный game-процесс с NullRHI,
настоящим сохранённым NavMesh, path following и damage. Проверяется движение
вокруг препятствия и урон, MoveToLocation, AlreadyAtGoal/Invalid, замена и
отмена, отсутствие callback, принятый/отклонённый reload и Destroy. Для проверки
отзыва уже поставленного в очередь события используется искусственный результат.
Отчёт — `logs/ai-validation.json`. Рендер, звук, networking, полный Foundation,
полный PIE и packaged build этим тестом не проверяются.
