# Модель authoring DSL — первый этап

FDasAuthoringModule описывает авторские Unreal-классы независимо от будущего
синтаксиса daScript. JSON schema_version=1 — транспорт этой модели и формат
тестовых деклараций. Это отдельная схема от reflection bindings IR v5.
Фронтенд синтаксических макросов сможет создавать те же C++ descriptors.
Синтаксис пользователь выбирает отдельно.

## Содержимое

- Module ID и список классов с явными стабильными class/member/parameter IDs.
- Native parent path либо parent_id класса того же модуля.
- Свойства: имя, тип, scalar default, editable/read_only и metadata.
- Методы: имя, return_type, упорядоченные параметры с in/out/inout, callable/pure/static.
- Implementation binding: script_file и entry_point.
- Типы: int/float/double/bool/string/name/void (return), enum/struct по native path,
  object по native path или authored class ID, array/set/map descriptors.
- Metadata allowlist: Category, DisplayName, ToolTip.

Model.Validate проверяет форму типов, ID и имена, defaults, параметры и граф
наследования. IDs должны быть уникальны во всём модуле; имена членов проверяются
без учёта регистра, как UE names. Циклы наследования и конфликты с унаследованными
authored членами запрещены. Ссылка object на собственный class ID допустима.
Произвольные native paths проверяются структурно; наличие UEnum/UScriptStruct,
enum symbols и конкретные native capabilities должен проверять emitting backend.
Authoring declarations UStruct/UEnum/interfaces ещё не добавлены.

FromJson принимает только известные ключи и точные JSON типы, ограничивает размер
и вложенность, валидирует candidate целиком и только затем заменяет Out.
ToJson проверяет модель и выдаёт стабильный порядок классов, свойств, методов и
metadata. Порядок параметров сохраняется. Это детерминированность модели;
Blueprint asset bytes с generated GUIDs не обязаны совпадать.

## Пример

Полные версии: Authoring/Counter.v1.json и Counter.v2.json.

```json
{
  "schema_version": 1,
  "module_id": "example",
  "classes": [{
    "id": "example.counter",
    "name": "AuthoredCounter",
    "native_parent": "/Script/Engine.Actor",
    "properties": [{
      "id": "example.counter.score",
      "name": "Score",
      "type": {"kind": "int"},
      "default": 10
    }],
    "methods": [{
      "id": "example.counter.evaluate",
      "name": "Evaluate",
      "return_type": {"kind": "int"},
      "parameters": [{
        "id": "example.counter.evaluate.input",
        "name": "Input",
        "direction": "in",
        "type": {"kind": "int"}
      }],
      "implementation": {"script_file": "authoring_eval_v1.das", "entry_point": "main"}
    }]
  }]
}
```

Тело authoring_eval_v1.das:

```das
options gen2
require ue_foundation
[export]
def main(input:int):int { return ue_get_self_int("Score") + input }
```

ID задаётся явно и не вычисляется из изменяемого имени. Переименование Score с
сохранением ID считается structural change; новый ID — удаление и добавление.
Compare отдельно отмечает structure, defaults, metadata и implementation binding.
Изменение содержимого файла при прежнем script_file относится к существующей
системе отслеживания исходников, а не к diff декларации. Diff не выполняет миграцию.

## Связь с Blueprint backend

ToActorSchema и FDasBlueprintService.ApplyAuthoring подключают модель к уже
проверенному Actor adapter. Пока этот backend поддерживает direct native Actor
parent, int/float поля и ровно один callable instance int(Input:int):int метод
с entry main. Реальный Blueprint parent — ADasProbeActor, наследник Actor.
Логическое имя класса хранится отдельно от UE asset/generated class name.

Metadata, read_only, pure/static, иные сигнатуры/типы и authored-parent emission
явно отвергаются до изменения Blueprint. Тот факт, что тип представим в модели,
не означает готовую поддержку его генерации. Adapter сохраняет стабильные IDs
в Blueprint package/variable metadata и отвергает их случайную замену.
Rename/removal/type/default/editability changes требуют отдельной migration policy.
Сейчас проверено добавление нового scalar field и замена implementation binding.

Перед Apply компилируется script candidate и проверяется один exported main с
int argument/result по значению. Runtime global initializers и тело main не
исполняются при preflight. Compile-time макросы по обычным правилам daScript
могут участвовать в компиляции. Проверка не доказывает отсутствие runtime ошибок
и не проверяет строковые имена self-properties внутри тела.
Preflight failures сохраняют asset. Полного rollback после ошибки самого
Blueprint compiler/reinstancing по-прежнему нет.

## Интеграционный сценарий

Scripts/TestAuthoring.ps1 запускает DasAuthoring commandlet в двух процессах:
Create и Reopen. Он ограничен изолированным DasForUESmoke проектом и создаёт
уникальный package внутри /Game/DasAuthoringProbe/Run_<guid>/.
Повторный прогон не перезаписывает старый package; generated assets игнорируются Git.

Create превращает v1-модель в Blueprint, вызывает Evaluate через ProcessEvent,
задаёт instance Score=77, применяет v2 (Bonus=5 и другое тело), находит reinstanced
Actor и проверяет результат 187. CDO Score=10 остаётся прежним. Затем проверяются
отказы static/invalid source/wrong signature/changed ID без мутации и asset сохраняется.
Reopen загружает asset в новом процессе, проверяет defaults/result=120, стабильные
class/method/parameter IDs и повторное применение модели. В runtime/packaged foundation suite
проверяется модель/валидация/diff; emitting backend здесь остаётся editor-only.

Следующий этап: macro frontend, который собирает descriptors, и расширение backend
на методы/metadata с согласованным синтаксисом. Затем hot reload по матрице diff,
структурная миграция и generated C++/UHT/AOT backend для production packaging.


## Верификация

268 foundation-проверок прошли в Editor и 268 — в packaged Win64 Development,
оба процесса завершились с кодом 0. Проверены model normalization/roundtrip,
duplicate IDs/names, defaults, inheritance cycles/conflicts, metadata/diff,
capability refusal и сохранность предыдущей модели/adapter schema при ошибках.
Compile-only preflight проверен на body с panic: main не исполняется.
21 контрольная точка TestAuthoring.ps1 прошла в двух процессах Create/Reopen:
реальные Blueprint generation, ProcessEvent, reinstance и override preservation,
проверки отказов без мутации, сохранение class/property/method/parameter IDs,
reopen и повторное применение. Регрессионный Objects PIE прошёл
(1 success, 0 failed, 0 succeededWithWarnings). git diff --check прошёл.
Runtime suite в packaged проверяет модель; это не proof нового cook/C++ backend
для произвольного authoring DSL. Журналы: logs/authoring-*.log,
logs/smoke-editor.log и logs/smoke-packaged.log.

Обновление backend: несколько методов/entrypoints, metadata и BlueprintReadOnly реализованы следующим этапом — AuthoringMethods.md. Ограничения первого этапа выше приведены исторически.

Текущее scalar/void расширение adapter: Docs/DSLScalarSignatures.md.
