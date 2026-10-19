# Native annotation DSL frontend — первая рабочая версия

Реализован frontend на штатных C++ AST annotations daScript 0.6.4.
Reader macro, текстовый препроцессор и изменения parser не используются.
`require ue_dsl` подключает annotations из runtime-модуля UEFoundation.

~~~das
options gen2
require ue_dsl
[ue_class(module_id="example", id="example.counter",
          parent="/Script/Engine.Actor", Blueprintable)]
class Counter {
    @ue_property=(EditAnywhere, BlueprintReadWrite)
    @ue_id="example.counter.score"
    @Category="Stats"
    Score : int = 10

    [ue_function(id="example.counter.evaluate", BlueprintCallable)]
    def Evaluate(input:int):int {
        return self.Score + input
    }
}
~~~

`module` — keyword grammar, поэтому идентификатор authoring-модуля задаётся
через `module_id`. При отсутствии явных IDs используются basename файла,
имя класса и имя члена. Для долговечных assets задавайте IDs явно: rename
без явного ID означает другую декларацию. ID параметра — `<method-id>.input`.

## Путь компиляции и выполнения

1. Structure annotation собирает поля и аннотированные методы из native AST.
2. Создаётся authoring-модель v1 и проверяется capability существующего Actor
   adapter. Binding export IR v5 остаётся отдельным форматом.
3. Для каждого метода создаётся exported function `ue_<Class>_<Method>`.
   `self.Score` в копии AST преобразуется в вызов runtime getter настоящего
   UObject. Присваивания преобразуются в setter; исходный текст не меняется.
4. `ReadAuthoringScript` компилирует и собирает модель без simulate/global init
   и без исполнения методов. Только после успешной проверки заменяется Out.
5. Editor service применяет модель через существующий Blueprint backend;
   методы вызывают generated entry points с owner/file persistent context.

Сериализованная модель временно хранится в служебном аргументе annotation
`_ue_model_json` внутри AST компилятора, затем читается как C++ authoring-модель.
Пользователь не может задавать этот аргумент. JSON не записывается в generated
`.das` и не является состоянием экземпляров Actor.

Это декларативный frontend для Unreal-классов. Обычный `new Counter()` в
daScript не создаёт Unreal Actor. Создание UObject/Actor идёт через Unreal
backend/API; классы DSL нельзя трактовать как нативное представление UObject
в памяти daScript.

## Границы первого этапа

- Parent: `/Script/Engine.Actor`, фактический bridge parent — ADasProbeActor.
- Поля: явно типизированные `int`/`float`/`bool`, literal defaults; все пользовательские
  поля должны иметь `@ue_property`. Script-private fields пока отклоняются.
- Флаги: EditAnywhere, BlueprintReadWrite, BlueprintReadOnly. Одновременные
  ReadOnly/ReadWrite отклоняются. Metadata: Category, DisplayName, ToolTip.
- Методы: 1–64 instance BlueprintCallable, 0–16 ordered int/float/bool inputs и int/float/bool/void result.
  Legacy single same-type pin остаётся Input; mixed frame pins сохраняют имена из .das.
- Тело: явные `self.<property>` reads и assignment; для int/float также `+=`, `-=`, `*=`, `/=`. Стандартный
  daScript внутри тела и вызовы обычных свободных helper functions доступны.
  Вызовы других методов через `self`, implicit field names, передача самого
  `self`, private instance state и general signatures ещё не реализованы.
- Constructor/component hooks, structs/enums/interfaces/delegates authoring,
  BlueprintPure, static/RPC/events остаются следующими расширениями.
- Обновление структуры применяется явно через backend. Ручной editor Apply/Reimport реализован (Docs/DSLReimport.md); автоматического
  file watcher пока нет. Существующий body reload
  проверяет signatures и сохраняет последнюю рабочую версию context.
- Native-class inference всё ещё проверяет исходные methods; transformation
  выполняется для generated entry points, а не заменяет layout native class.

## Код и воспроизводимая проверка

- `Plugins/DasForUE/Source/DasRuntime/Private/DasDSL.inl`: annotations и AST rewrite.
- `DasRuntime.cpp`: регистрации и ReadAuthoringScript.
- `Scripts/TestDSL.ps1`: DSL -> Blueprint, live reinstance/override, metadata,
  save/reopen в новом процессе. Использует только изолированный smoke project.
- `dsl_counter_v1.das` и `dsl_counter_v2.das`: пользовательские примеры.
- `dsl_fixture.das`: int/float assignment/compound runtime probe.
- `dsl_bad_*.das`: negative compilation probes, сохранение предыдущей модели.

AST constructors потребовали выравнивания сторонних headers как в static
библиотеке daScript: UE задаёт /Zp8, библиотека — default /Zp16. В DasRuntime.cpp
simulate prerequisites загружаются до pack(push,16), затем включаются AST headers
и восстанавливается упаковка. Это важно из-за pack-reset внутри arraytype.h.

Ручной editor import/reimport и body/structural reload проверены в настоящем
Editor/PIE, см. Docs/DSLReimport.md. Следующий этап — general signatures/types;
автоматический watcher/UI можно добавить поверх import service. Расширение signatures/types должно проходить capability gates.

Canonical JSON authoring declaration теперь также хранится в compiled context
для защиты reload от неприменённых изменений схемы; это не состояние Actor.

Расширение typed signatures и bool properties: Docs/DSLScalarSignatures.md.

Текущее mixed scalar расширение: Docs/DSLMixedMethods.md.

String/name/double fields и method signatures реализованы: Docs/DSLTextTypes.md.
