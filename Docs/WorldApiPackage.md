# Пакет World / Actor / Components

Профиль `Bindings/Packages/WorldActorComponents.profile.json` выбирает
GameplayStatics и KismetSystemLibrary, а также семь receiver-классов:
World, Actor, ActorComponent, SceneComponent, PrimitiveComponent, Controller,
MovementComponent. Модуль: `ue_api_world`.

Это систематическая генерация отражённых методов и статических функций.
Она использует существующие generic call frames и ProcessEvent. Поля,
native getter/setter, новые managed callbacks и неотражённый C++ в этот шаг
не входят. Project discovery и существующий native registry остаются прежними;
описанные здесь вызовы доступны через пакетные функции, а не через расширение
реестра `self` новыми классами.

## Покрытие UE 5.8.3

| Поверхность | Готовы обёртки | Отказ экспортёра | Ограничение генератора |
| --- | ---: | ---: | ---: |
| Методы семи receiver-классов | 364 | 99 | 1 |
| Две Function Libraries | 288 | 147 | 5 |
| Всего | 652 | 246 | 6 |

Профиль по умолчанию выбирает объявления каждого receiver без унаследованных
копий. Это 904 кандидата. Часть методов уже доступна через native gameplay
registry; 652 обёртки не означают 652 ранее недоступные возможности.

В отчёте сохранены конкретные причины отказов: неподдержанные сигнатуры,
internal/protected/deprecated, latent/custom thunk и soft references. Construction restrictions больше
не считаются безусловным отказом экспорта. Ограничения Python-генератора — непрозрачный AsyncPhysicsTickHandle у
PrimitiveComponent и пять Dialogue-функций с неподдержанной struct array shape.

88 сгенерированных функций используют [construction guard](NativeAccessAndConstruction.md):
они доступны в обычном gameplay и отклоняются в ConstructionScript до dispatch.
Ограничение сохранено в generated policy и не требует editor metadata.
SoftObject/SoftClass references поддержаны в других слоях привязки, но generic
wrapper ABI для них пока отсутствует. Экспортёр теперь сообщает явную причину,
а не создаёт фиктивную struct-ссылку с пустым type_path.

## Вызовы

```das
options gen2
require ue_api_world

[export]
def main(input:int):int {
    let actor = C_Actor_adf65d5b_from(ue_self_object())
    ue_actor_SetActorScale3D(actor,ue_vector(2.0lf,3.0lf,4.0lf))
    let scale : UEVector = ue_actor_GetActorScale3D(actor)
    let root = ue_actor_K2_GetRootComponent(actor)
    ue_scene_SetVisibility(root,true,true)
    let actors <- ue_gameplay_GetAllActorsWithTag(ue_name("Pickup"))
    return int(scale.z) + length(actors)
}
```

Для WorldContext-функций контекст берётся из текущего владельца скрипта и
скрыт из аргументов обёртки. Вызов без live Game world отклоняется; автоматический
параметр нельзя перезаписать вручную. Пример требует запуска с Actor-владельцем.
Канонические UEVector2D/UEVector/UERotator/UETransform общие с Math и native API.

Object views пока используют существующее представление generic-генератора:
хеш в имени обеспечивает устойчивую identity. Короткие Actor/World/Component
имена — typedef этих views; `_from`/`_raw` обеспечивают проверенный переход через
UObjectHandle. Это не создание второго UObject. При совместном импорте других
пакетов квалифицируйте имена, например `ue_api_world::ue_name` и
`ue_api_world::C_Actor_adf65d5b_from`. Канонизация object/enum/UEName views между
пакетами остаётся отдельной задачей; полной взаимозаменяемости этих оболочек
пока нет.

## Воспроизведение и проверки

```powershell
.\Scripts\TestWorldApiPackage.ps1 -SkipBuild
.\Scripts\TestWorldApiPackage.ps1 -SkipBuild -Publish
```

Без SkipBuild сначала собирается commandlet. Генерация без тестового runner:
`GenerateApiPackage.ps1 -Profile Bindings/Packages/WorldActorComponents.profile.json`.
Публикация выполняется только с Publish и после успешных основных probes.

Целевые проверки: компиляция всех 652 функций, настоящие Actor mutation/read,
корневой компонент, Component owner, рекурсивная visibility, дочерние компоненты,
поиск по tag/классу компонента, WorldContext, имя объекта, World origin, вызов
WorldSettings через World receiver. Используются отдельные transient Game worlds,
инициализированные InitializeActorsForPlay; ассеты не сохраняются, открытый
редактор не используется. Миры и контексты удаляются после каждого запуска.

Отрицательные сценарии: отсутствующий WorldContext, попытка его перезаписи,
null, несовместимый тип и уничтоженный receiver. Python-проверки охватывают
receiver-only профили, static/instance policy, inheritance counts, коллизии
surface и отсутствующие struct identities. Прошли 120 Python-тестов и совместный импорт установленных World/Math/native gameplay
с проверенным переходом object views. Полные Foundation/PIE/packaged не запускались.

Снимок: `Bindings/Packages/ue_api_world.summary.json`.
Полный последний успешный отчёт: `Build/ApiPackages/ue_api_world/latest-report.json`.
Целевая валидация: `logs/world-api-package-validation.json`.

## Профили receivers

`receivers` имеет те же class/prefix/include/include_inherited правила, что и
`libraries`, но требует instance methods. Несовпадение static/instance записывается
как policy_excluded. Класс не может одновременно быть в двух surfaces.
Исключения требуют причины. Полное замыкание зависимостей и общая компиляция
проверяются до публикации.

Для include_inherited=true отчёт разделяет receiver occurrences и уникальные
объявления. Если старый exporter diagnostic не содержит declaring_class,
такое вхождение учитывается в unknown_declaration_occurrences и не выдаётся
за ещё одно уникальное объявление. В текущем профиле наследованные копии
не запрашиваются.

Общий native accessor/context слой [реализован](NativeAccessAndConstruction.md).
Далее — Collision/Physics и устранение измеренных type blockers.
