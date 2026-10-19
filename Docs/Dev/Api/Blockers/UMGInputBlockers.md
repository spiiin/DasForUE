# UMG: аудит 0 generation failures

UE `5.8.3-0+UE5`. Снимок: **407 ready, 110 export blocked, 0 generation blocked**.

Этот отчёт не меняет coverage; состояние реализации указано отдельно в контракте.
Классификация export refusals ведётся отдельно: [UMGExportBlockers](UMGExportBlockers.md).

| Группа | Уникальных функций |
| --- | ---: |

33 input snapshot functions реализованы; непрозрачные input-event shapes больше не входят в этот backlog.
Navigation и DetectDragIfPressed открыты через scoped live adapters; отдельных execution-contract отказов нет.

## Непрозрачные типы

| Native type | Затронутых функций |
| --- | ---: |

Числа типов пересекаются; таблица групп выше — непересекающееся разбиение функций.

## Поимённый backlog

| Функция | Группа | Поля и направления | Следующий барьер |
| --- | --- | --- | --- |

## Native source evidence

- input_storage: `Engine/Source/Runtime/SlateCore/Public/Input/Events.h` (SHA256 `7e534ed8373258f515d9f634741f8de9808b11bb7940ab662bcf6c41fe1147c1`); строки 154, 408, 413, 1135, 1114, 430, 526, 604, 693, 1254.
- path_dereference: `Engine/Source/Runtime/SlateCore/Private/Input/Events.cpp` (SHA256 `39955fa911629718f0d572d970be8f4d5b244dd55f18b35abef4150156aa6598`); строки 22, 27.
- event_projections: `Engine/Source/Runtime/UMG/Private/WidgetBlueprintLibrary.cpp` (SHA256 `6229bb54e725de53dcbec54c471c51af78a2951fac3dfae0c38b519f467dffe4`); строки 631, 636, 641, 646, 651.
- navigation_policy: `Engine/Source/Runtime/Slate/Private/Framework/Application/NavigationConfig.cpp` (SHA256 `2e5abc6be1b899c98ede483e5fca07a91959d4caa9d09dae0e4ea20dfa2a46c9`); строки 83, 90, 97, 98.
- navigation_dispatch: `Engine/Source/Runtime/Engine/Private/KismetInputLibrary.cpp` (SHA256 `ef39802ad963412e742a94e0f3af532b4f1ef1a3ab023d191d0148756f14b90c`); строки 114, 124, 134, 105.
- paint_and_callbacks: `Engine/Source/Runtime/UMG/Public/Blueprint/UserWidget.h` (SHA256 `4dc746ee00c41f01304b03a159cfd0693d3e30719c5697afb30f1a4ff819d636`); строки 177, 210, 212, 629, 693.
- reply: `Engine/Source/Runtime/UMG/Public/Components/SlateWrapperTypes.h` (SHA256 `60dc1f49b7444a113f2e4d3818cd335453fd0b98068542f67fb435a9982c7162`); строки 128, 141.
- geometry: `Engine/Source/Runtime/SlateCore/Public/Layout/Geometry.h` (SHA256 `b857272e3567da9fce115710ff9f2e09abcba1e6ef781c8647ab15df4e9fa347`); строки 39, 636.
- animation_handle: `Engine/Source/Runtime/UMG/Public/Animation/WidgetAnimationHandle.h` (SHA256 `fef4af705282d66d9ba8f3a46c997cc5d008128f8cf5e3073a780d256ac09c7f`); строки 21, 72, 46.

Три navigation queries открыты через [live input adapter](InputNavigation.md), current native event и weak Slate user. Analog query потребляет событие и обновляет repeat state не более одного раза на callback.

DetectDragIfPressed и шесть native drag/drop callbacks реализованы через [managed DragDropOperation](DragDrop.md); source/participating target Destroy и committed reload отзывают операцию.

5 PaintContext draw functions открыты через [синхронный OnPaint bridge](PaintContext.md); script-команды публикуются только после успешного callback.

4 WidgetAnimation producers открыты через [weak WidgetAnimation transport](WidgetAnimation.md); finish, NativeDestruct и смена VM отзывают доступ.

12 EventReply functions открыты через [scoped callback transport](EventReply.md). Reply и call frames действуют только внутри текущего синхронного callback.

17 Geometry functions реализованы через [owned Geometry snapshots](Geometry.md); эта группа больше не входит в backlog.

Контракт и порядок реализации: [InputEventTransportContract](InputEventTransportContract.md).
Input snapshots не реализуют UserWidget overrides, EventReply или paint callbacks автоматически.
Проверки: реальный повтор генерации на свежей reflection + unit tests аудита; без PIE/packaged/physical input.
