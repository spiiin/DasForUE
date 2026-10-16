# String, double и FName в authoring DSL

Новые типы поддержаны для полей, входных параметров и результатов методов.
Общий frame сохраняет предел 0–16 inputs, metadata/stable IDs и last-good reload.

| daScript | Unreal | Frame kind |
|---|---|---|
| string | FString / FStrProperty | 5 |
| double | double / FDoubleProperty | 6 |
| UENameValue | FName / FNameProperty | 7 |

UENameValue — новый nominal value type, зарегистрированный в UEFoundation.
Это не string alias и не FName pointer. UENameValue предотвращает случайную
передачу string вместо FName. В authoring JSON ему соответствует kind `name`.
Существующий generated `ue_generated::UEName` остаётся отдельным struct wrapper;
он не заменён. Преобразование возможно через `ue_name_text` и `.value`/`ue_name`.

~~~das
options gen2
require ue_dsl
[ue_class(Blueprintable)]
class TextCounter {
    @ue_property=(EditAnywhere,BlueprintReadWrite)
    Label : string = "Привет"
    @ue_property=(EditAnywhere,BlueprintReadWrite)
    Precision : double = 1.23456789012345lf
    @ue_property=(EditAnywhere,BlueprintReadWrite)
    Key : UENameValue = ue_name_value("Initial_7")

    [ue_function(BlueprintCallable)]
    def Echo(value:string; suffix:string):string {
        self.Label = value + suffix
        return self.Label
    }
    [ue_function(BlueprintCallable)]
    def Rename(value:UENameValue):UENameValue {
        self.Key = value
        return self.Key
    }
}
~~~

Полный пример без explicit IDs: dsl_text_methods.das. Импорт:

~~~text
Das.DSL.Apply dsl_text_methods.das /Game/DasExamples/BP_Text text.TextCounter
~~~

## Ownership и semantics

- На входе FString копируется в string heap текущей VM; исходный C++ buffer
  может измениться после возврата, а строка, сохранённая в globals, остаётся
  собственностью этого context. На выходе результат копируется в FString до
  завершения scope. FString в Actor properties также владеет своей памятью.
- Double переносится в 64-битном VM slot, без промежуточного float. Используйте
  `lf` suffix для double literals; finite defaults и round-trip precision
  проверяются при authoring. В Blueprint используется PC_Real/PC_Double.
- UENameValue содержит закрытый 64-битный token. Context хранит соответствующие
  FName values, максимум 65,536 distinct names. Tokens не содержат pointer и
  не принимаются другим context. На выходе FName копируется в host-owned value.
  None — token 0. UE case-insensitive equality и number suffix semantics
  используются через настоящий FName, без сравнения только текстовых строк.
- `ue_name_value(text)` создаёт name, `ue_name_text(value)` возвращает VM-owned
  string, `ue_names_equal(a,b)` сравнивает UE names. Методы/helpers требуют
  active runtime context. Bare comparison operators для UENameValue не добавлены.
- VM globals проходят обычную инициализацию при valid context swap; произвольные
  string/name globals пока не мигрируются. Host results и Actor fields живут
  независимо от уничтоженного context. Broken reload продолжает старый context.

## Defaults и проверки

String defaults — literals, double — numeric literals, name —
`ue_name_value("literal")`. Preflight извлекает их без выполнения constructor/
global initializer. Blueprint CDO defaults сверяются с декларацией; смена уже
созданного default по-прежнему требует migration. Self reads/writes проходят
общие hidden/readonly/type checks. Embedded NUL не поддерживается строковым
контрактом; host inputs с NUL отклоняются до eval.

TestDSLTextPIE.ps1 проверяет Unicode ProcessEvent, FName input/result, double
precision, CDO defaults, сохранённые VM inputs, изоляцию миров, reimport и UE GC.
TestDSLTextAsset.ps1 сохраняет графы и проверяет text ProcessEvent, double/name
defaults и source binding после открытия в новом процессе. Foundation tests
проверяют host buffer change, results после reload/context release, broken
reload fallback, FName case/number/None, nominal mismatch и embedded NUL.

Следующий этап — object references и reflected value structs с ownership/GC
правилами; контейнеры, out/ref, parameter defaults/annotations, pure/static/RPC
и production AOT authoring ещё требуют отдельных capabilities.
