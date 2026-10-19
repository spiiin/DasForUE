# Native annotation syntax for Unreal DSL

Дата: 2026-10-06. По запросу пользователя reader-macro вариант снят с направления
DSL. Предложена и реально проверена штатная grammar daScript 0.6.4:

```das
[ue_class(parent="/Script/Engine.Actor", Blueprintable)]
class Counter {
    @ue_property=(EditAnywhere, BlueprintReadWrite)
    @Category="Stats"
    Score : int = 10

    [ue_function(BlueprintCallable, Category="Logic")]
    def Evaluate(input : int) : int {
        return self.Score + input
    }
}
```

class/def являются обычными keywords daScript. ue_class и ue_function — наши
зарегистрированные structure/function annotations. Поля используют встроенные
field metadata через @name, @name=value и @name=(values). Парсер превращает tuple
в несколько аргументов с одинаковым именем; structure macro читает field.annotation.
Capability probe проверил, что два ue_property flags доступны в AST и что метод
работает. Результат CLI: exit 0, NATIVE_ANNOTATIONS_COMPACT_PASS.

Это syntax/AST capability proof с минимальными marker annotations; модель authoring
и Unreal backend из этой формы пока не вызываются. Counter в standalone-пробе —
обычный класс daScript. UE instance allocation, weak handles и доступ к native
properties должны обеспечиваться полноценным frontend/backend; Lua-like mirror
layout класса не должен становиться layout UObject.

parent в annotation задаёт Unreal-родителя по полному native path. Это не требует
создавать специальную daScript class Actor лишь ради разбора декларации. Когда
появится registry authored types, parent сможет поддерживать их stable IDs.

Блоки ue_properties/ue_functions из предыдущего эскиза заменяются annotations
на отдельных членах. Общие defaults групп можно добавить как class-level options
и overlays коллектора; отдельные новые block keywords не являются native grammar.
Синтаксис не требует reader, source-text preprocessor или изменений daScript parser.
На стадии frontend нужно извлечение declarations/default AST/source spans,
переписывание self/property/method access и связывание с validated authoring model.

Приложены native_annotations_compact.das и ue_annotation_probe.das. В probe
использована ручная heap allocation обычного daScript class и явное удаление
для проверки языка; это не предложенный способ SpawnActor/NewObject в Unreal DSL.


## Уточнение — 2026-10-06

Пользователь выбрал индивидуальные [ue_class(...)]/[ue_function(...)] annotations
и field metadata. ue_function_group не нужен и не входит в DSL.
Блоки ue_properties(flags) { ... } / ue_functions(flags) { ... } рассматривались
только как дополнение при поддержке штатными макросами.

Оба точных block-варианта проверены через compile-only daScript 0.6.4 внутри class.
Оба завершились exit 1: syntax error, unexpected '(', expecting newline/semicolon.
Это parse-stage отказ до AST macro expansion. Регистрация call/block annotation
сама по себе не добавит class-body grammar production. В текущем frontend такие
блоки не включаются; reader/preprocessor/parser modification не используются.

Рабочая основа:

~~~das
[ue_class(parent="/Script/Engine.Actor")]
class Counter {
    @ue_property=(EditAnywhere, BlueprintReadWrite)
    Score : int = 10

    [ue_function(BlueprintCallable)]
    def Evaluate(input:int):int {
        return self.Score + input
    }
}
~~~

Реализация первого frontend: см. Docs/DSLFrontend.md; module_id используется
вместо reserved keyword module.
