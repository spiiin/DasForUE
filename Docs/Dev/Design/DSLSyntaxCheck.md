# Проверка синтаксиса DSL на daScript 0.6.4

Проверены исходники закреплённой daScript, ds2_parser.ypp, AstReaderMacro API,
reader_macro tests и реальные компиляции через bin/Release/daslang_static.exe.
Executable сообщил версию 0.6.4.

## Результат

- Bare `ue_class Counter : Actor { ... }` не принимается штатной грамматикой:
  syntax error, unexpected name, expecting struct or class (exit 1).
- `%ue~ ue_class ... %%` работает через reader macro: exit 0, READER_PROBE_PASS.
- `%ue_class~ Counter : Actor { ... }` работает с reader, завершающимся на
  соответствующей внешней закрывающей скобке: exit 0, BALANCED_READER_PROBE_PASS.

Следовательно, пользовательский DSL возможен без изменения парсера, с native
reader-macro prefix. Предлагаемый практический вид:

```das
require ue_dsl

%ue_class~ Counter : Actor {
    ue_properties(EditAnywhere, BlueprintReadWrite) {
        Score : int = 10
    }
    ue_functions(BlueprintCallable) {
        def Evaluate(input:int):int { return self.Score + input }
    }
}
```

`ue_dsl` в этом примере — имя будущего frontend module, пока не реализация.
Reader получает raw characters, наш frontend разбирает declarations/scopes,
формирует модель authoring и выдаёт native daScript declarations/bodies через
AST либо suffix, который парсер перечитывает. Обычные AST/structure/function
annotations работают на существующей грамматике, а не добавляют произвольный
bare top-level keyword. Macro module должен быть отдельным require.

Эксперименты являются capability probes, не полноценным Unreal DSL. Они проверяют
получение вложенного блока и выдачу фиксированного тестового daScript-кода.
Для production нужны tokenizer для строк/комментариев/interpolation, нормальная
диагностика незакрытых blocks, source mapping, парсинг annotations/types/methods
и интеграция с model/backend. Простого подсчёта всех `{`/`}` недостаточно.
Unreal-классы в этих языковых экспериментах не создавались.

Исходные доказательства в dependency:
skills/daslang/references/macros.md; tests/reader_macro/inline_reader.das;
src/parser/ds2_parser.ypp (top_level_reader_macro и reader_character_sequence).
Fixtures и журналы эксперимента приложены рядом с этим документом.
