"""Integration tests against the built editor exporter; no engine-wide API scan."""
import argparse
import json
import subprocess
from pathlib import Path
from GenerateBindings import generate

ROOT = Path(__file__).resolve().parents[1]
FIXTURE = '/Script/DasRuntime.DasObjectFixture'

def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--engine', type=Path, default=Path('C:/src/unreal/UnrealEngine'))
    args = parser.parse_args()
    stage = ROOT / 'Build/Bindings/PolicyTests'
    stage.mkdir(parents=True, exist_ok=True)
    def export(name, selection, error=None):
        source, output = stage / (name + '.selection.json'), stage / (name + '.json')
        source.write_text(json.dumps(selection), encoding='utf-8')
        output.write_text('previous validated output', encoding='utf-8')
        with (stage / (name + '.log')).open('w', encoding='utf-8') as log:
            result = subprocess.run([str(args.engine / 'Engine/Binaries/Win64/UnrealEditor-Cmd.exe'),
                str(ROOT / 'Tests/DasForUESmoke/DasForUESmoke.uproject'), '-run=DasExport',
                '-Selection=' + str(source), '-Output=' + str(output), '-unattended', '-NullRHI', '-nosplash', '-stdout',
                '-abslog=' + str(stage / (name + '.engine.log'))], stdout=log, stderr=subprocess.STDOUT, timeout=180)
        if error:
            assert result.returncode == 1, (name, result.returncode)
            assert output.read_text() == 'previous validated output', name
            assert error in (stage / (name + '.engine.log')).read_text(encoding='utf-8-sig'), name
            print('PASS', name, flush=True)
            return
        assert result.returncode == 0, (name, result.returncode)
        ir = json.loads(output.read_text(encoding='utf-8-sig'))
        generate(ir)  # Every successful export must also be consumable by the generator.
        print('PASS', name, flush=True)
        return ir

    rule = dict(class_rules=[{'class': FIXTURE, 'prefix': 'fixture_', 'include': ['Policy*', 'Transform*'], 'exclude': ['Transform*']}],
        functions=[{'class': '/Script/Engine.Actor', 'function': 'K2_GetComponentsByClass', 'alias': 'actor_Unsupported', 'required': False}])
    ir = export('filtered', rule)
    assert [f['function'] for f in ir['functions']] == ['PolicyDefault']
    reasons = {d['function']: d['reason'] for d in ir['diagnostics']}
    assert reasons['TransformPayload'] == 'excluded_by_rule'
    assert reasons['PolicyDeprecated'] == 'deprecated_function'
    assert reasons['PolicyInternal'] == 'internal_function'
    assert 'unsupported UFunction or struct field type' in reasons['K2_GetComponentsByClass']
    fn = ir['functions'][0]
    assert fn['pure'] and fn['const'] and fn['declaring_class'] == FIXTURE
    assert fn['metadata']['Category'] == 'Policy' and fn['metadata']['DisplayName'] == 'Policy default'
    assert next(p for p in fn['parameters'] if p['path'] == 'value')['default_text'] == '7'
    assert ir == export('deterministic', rule)
    rule['policy'] = dict(allow_deprecated=True, allow_internal=True)
    allowed = export('opt_in', rule)
    assert {f['function'] for f in allowed['functions']} == {'PolicyDefault', 'PolicyDeprecated', 'PolicyInternal'}
    inherited = {'class_rules': [{'class': '/Script/DasRuntime.DasProbeActor', 'prefix': 'ue_', 'include': ['GetOwner'], 'include_inherited': True}]}
    inherited_ir = export('inherited', inherited)
    assert inherited_ir['functions'][0]['declaring_class'] == '/Script/Engine.Actor'
    assert inherited_ir['functions'][0]['class'] == '/Script/DasRuntime.DasProbeActor'
    inherited['class_rules'][0]['include_inherited'] = False
    export('own_only_no_match', inherited, 'matched no functions')
    export('strict_required', {'functions': [{'class': FIXTURE, 'function': 'PolicyDeprecated', 'alias': 'fixture_Deprecated'}]}, 'deprecated_function')
    export('unknown_policy', {'policy': {'allow_deprected': True}}, 'Unknown selection key')
    export('invalid_bool', {'policy': {'allow_internal': 'yes'}}, 'must be boolean')
    export('duplicate_alias', {'functions': [{'class': FIXTURE, 'function': 'AddHealth', 'alias': 'fixture_PolicyDefault'}],
        'class_rules': [{'class': FIXTURE, 'prefix': 'fixture_', 'include': ['PolicyDefault']}]}, 'Duplicate or empty alias')
    print('DAS_POLICY_RESULT=PASS exports=9', flush=True)

if __name__ == '__main__': main()
