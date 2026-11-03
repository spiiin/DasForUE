"""Run ue_try contracts in the engine's actual embedded daScript compiler."""
from pathlib import Path
import json
import uuid
import unreal

root = Path(__file__).resolve().parents[2]
scripts = root / 'Plugins/DasForUE/Content/Das/Scripts'
cases = [
    ('non_result_scope', '[export]\ndef main(input:int):int { let x=ok(1,type<string>) |> ue_try; return x }', 'enclosing Result-returning'),
    ('nested_expression', 'def bad():Result<bool;string> { if (ok(true,type<string>) |> ue_try) {}; return ok(true,type<string>) }', 'standalone statement'),
    ('defer', 'require daslib/defer\ndef bad():Result<bool;string> { defer() { ok(1,type<string>) |> ue_try }; return ok(true,type<string>) }', 'enclosing Result-returning'),
    ('option_input', 'def bad():Result<bool;string> { let x=some(1) |> ue_try; return ok(true,type<string>) }', 'ue_try'),
    ('field_assignment', 'struct S { value:int }\ndef bad():Result<bool;string> { var s:S; s.value=ok(1,type<string>) |> ue_try; return ok(true,type<string>) }', 'standalone statement'),
    ('address', 'def bad():Result<bool;string> { let ptr=@@ue_try; return ok(true,type<string>) }', 'ue_try'),
    ('wrong_error_type', 'def bad():Result<bool;int> { err("text",type<int>) |> ue_try; return ok(true,type<int>) }', 'return'),
]
checks = []
created = []
actors = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)
if not unreal.get_editor_subsystem(unreal.UnrealEditorSubsystem).get_editor_world():
    unreal.EditorLoadingAndSavingUtils.new_blank_map(False)

def run(name, source):
    path = scripts / f'result_try_{name}_{uuid.uuid4().hex}.das'
    created.append(path)
    path.write_text(source, encoding='utf-8')
    actor = actors.spawn_actor_from_class(unreal.DasActor, unreal.Vector())
    assert actor
    try:
        value = actor.call_method('ExecuteDasNamed', args=(path.name, 'main', 5))
        return value, str(actor.get_editor_property('LastDasError'))
    finally:
        actors.destroy_actor(actor)

try:
    value, error = run('positive', (root/'Tests/ResultTry/probe.das').read_text())
    assert value == 78 and not error, (value, error)
    checks.append('values_moves_once_only_cleanup_nearest_scope_and_typed_errors')
    for name, source, expected in cases:
        if '[export]' not in source:
            source += '\n[export]\ndef main(input:int):int { return input }\n'
        value, error = run(name, 'options gen2\nrequire ue_result\n' + source)
        assert value == -2147483648 and expected.lower() in error.lower(), (name, value, error)
        checks.append(name)
    result = root/'logs/result-try-validation.json'
    result.write_text(json.dumps({'passed': True, 'checks': checks}, indent=2))
    unreal.log(f'DAS_RESULT_TRY=PASS checks={len(checks)}')
finally:
    for path in created:
        path.unlink(missing_ok=True)
