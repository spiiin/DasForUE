"""Stage a declarative reflected API package; UE compilation is a separate gate."""
import argparse
import copy
import fnmatch
import hashlib
import json
import re
from pathlib import Path
from GenerateBindings import generate


def read(path):
    return json.loads(Path(path).read_text(encoding='utf-8-sig'))


def selection(profile):
    if profile.get('schema_version') != 1:
        raise ValueError('Unsupported package schema')
    if not re.fullmatch(r'ue_[a-z][a-z0-9_]*', profile.get('module', '')):
        raise ValueError('Invalid package module')
    if profile.get('default_arguments') != 'explicit':
        raise ValueError('Package must explicitly choose default_arguments=explicit')
    if profile.get('value_types') not in ('generated_transport', 'canonical_math'):
        raise ValueError('Unsupported package value representation')
    if not isinstance(profile.get('probe'), str) or not profile['probe'].strip():
        raise ValueError('Expected an exported main(int):int runtime probe returning 42')
    if type(profile.get('probe_world', False)) is not bool:
        raise ValueError('probe_world must be boolean')
    if 'world_probe' in profile and (not profile.get('probe_world') or not isinstance(profile['world_probe'],str) or not profile['world_probe'].strip()):
        raise ValueError('world_probe requires game world and nonempty source')
    rules = profile.get('libraries', []) + profile.get('receivers', [])
    if not rules or len({r['class'] for r in rules}) != len(rules):
        raise ValueError('Expected unique library/receiver classes')
    prefixes = set()
    for rule in rules:
        if not re.fullmatch(r'/Script/[A-Za-z0-9_]+\.[A-Za-z0-9_]+', rule['class']):
            raise ValueError('Expected native library class')
        if not re.fullmatch(r'ue_[A-Za-z0-9_]+_', rule['prefix']) or rule['prefix'] in prefixes:
            raise ValueError('Expected unique ue_ alias prefixes')
        prefixes.add(rule['prefix'])
        if not rule.get('include') or any(not isinstance(x, str) for x in rule['include']):
            raise ValueError('Expected include patterns')
        if rule.get('exclude'):
            raise ValueError('Use documented exclusions, not silent class-rule excludes')
    for exclusion in profile.get('exclusions', []):
        if not exclusion.get('reason') or exclusion.get('class') not in {r['class'] for r in rules} or not exclusion.get('function'):
            raise ValueError('Exclusions require selected class, function pattern and reason')
    modules = profile.get('ue_modules', [])
    if not modules or any(not re.fullmatch(r'[A-Za-z][A-Za-z0-9_]*', m) for m in modules):
        raise ValueError('Expected UE module names')
    for rule in rules:
        if type(rule.get('include_inherited', False)) is not bool:
            raise ValueError('include_inherited must be boolean')
    return {'class_rules': [dict(r, include_inherited=r.get('include_inherited', False)) for r in rules],
            'policy': {'allow_deprecated': False, 'allow_internal': False}}


def closure(ir, functions):
    """Discard unrelated and partially exported types, retaining ancestors and values."""
    structures = {s['type_path']: s for s in ir['structs']}
    classes = {c['type_path']: c for c in ir['classes']}
    used_structs, used_classes = set(), set()
    def cls(path):
        if not path or path in used_classes:
            return
        used_classes.add(path)
        cls(classes[path]['parent'])
    def visit(field):
        kind, path = field['kind'], field.get('type_path', '')
        if kind in ('object', 'class', 'soft_object'):
            cls(path)
        if kind == 'struct' and path not in used_structs:
            if path not in structures:
                raise ValueError('Missing reflected struct identity/definition: ' + repr(path))
            used_structs.add(path)
            for child in structures[path]['fields']:
                visit(child)
        for key in ('element', 'key', 'value'):
            if key in field:
                visit(field[key])
    for fn in functions:
        cls(fn['class'])
        for field in fn['parameters']:
            visit(field)
    return dict(ir, functions=functions, diagnostics=[],
                structs=[structures[p] for p in sorted(used_structs)],
                classes=[classes[p] for p in sorted(used_classes)])


def stage(profile, ir):
    selection(profile)
    rules = {r['class']: r for r in profile.get('libraries', []) + profile.get('receivers', [])}
    receiver_classes = {r['class'] for r in profile.get('receivers', [])}
    canonical_math = profile['value_types'] == 'canonical_math'
    records, functions = [], []
    def included(item):
        rule = rules.get(item['class'])
        return rule and any(fnmatch.fnmatchcase(item['function'], p) for p in rule['include'])
    def excluded(item):
        return next((e for e in profile.get('exclusions', [])
                     if e['class'] == item['class'] and fnmatch.fnmatchcase(item['function'], e['function'])), None)
    seen = set()
    for item, exported in [(f, True) for f in ir['functions']] + [(d, False) for d in ir.get('diagnostics', [])]:
        if not included(item):
            continue
        identity = (item['class'], item['function'])
        if identity in seen:
            raise ValueError('Duplicate function identity: ' + str(identity))
        seen.add(identity)
        record = dict(zip(('class', 'function'), identity))
        record['surface'] = 'receiver' if item['class'] in receiver_classes else 'library'
        record['construction_unsafe'] = exported and 'UnsafeDuringActorConstruction' in item.get('metadata', {})
        record['declaring_class'] = item.get('declaring_class') or (None if rules[item['class']].get('include_inherited', False) else item['class'])
        reason = excluded(item)
        if reason:
            record.update(status='policy_excluded', reason=reason['reason'])
            if reason.get('alternative'): record['alternative'] = reason['alternative']
            if not exported: record['export_reason'] = item['reason']
        elif exported and not item.get('static', False) and item['class'] not in receiver_classes:
            record.update(status='policy_excluded', reason='function static/instance mode does not match profile surface')
        elif not exported:
            record.update(status='export_blocked', reason=item['reason'])
        else:
            fn = copy.deepcopy(item)
            fn['alias'] = rules[item['class']]['prefix'] + item['function']
            record['alias'] = fn['alias']
            record['explicit_defaults'] = []
            for param in fn['parameters']:
                if 'default_text' in param:
                    record['explicit_defaults'].append({'parameter': param['path'], 'value': param.pop('default_text')})
            try:
                generate(closure(ir, [fn]), profile['module'], canonical_math=canonical_math)
            except ValueError as error:
                record.update(status='generation_blocked', reason=str(error))
            else:
                record['status'] = 'generation_ready'
                functions.append(fn)
        records.append(record)
    for library in rules:
        if not any(record['class'] == library for record in records):
            raise ValueError('No candidates found for selected library/receiver: ' + library)
    if not functions:
        raise ValueError('No generation-ready functions')
    package_ir = closure(ir, sorted(functions, key=lambda f: f['alias']))
    # This is deliberately a whole-package gate: cross-function collisions must fail.
    public = generate(package_ir, profile['module'], canonical_math=canonical_math)
    digest = hashlib.sha256(public.encode()).hexdigest()[:16]
    staged_module = profile['module'] + '_preflight_' + digest
    staged = generate(package_ir, staged_module, canonical_math=canonical_math)
    counts = {state: sum(r['status'] == state for r in records) for state in
              ('generation_ready', 'export_blocked', 'generation_blocked', 'policy_excluded')}
    report = {'schema_version': 1, 'package': profile['module'], 'engine_version': ir.get('engine_version'),
              'state': 'awaiting_ue_preflight', 'value_types': profile['value_types'],
              'default_arguments': profile['default_arguments'], 'counts': counts,
              'candidate_count': len(records),
              'construction_guarded_count': sum(m['status']=='generation_ready' and m['construction_unsafe'] for m in records),
              'unique_declaration_count': len({(r['declaring_class'], r['function']) for r in records if r['declaring_class']}),
              'unknown_declaration_occurrences': sum(r['declaring_class'] is None for r in records),
              'surfaces': {surface: {state: sum(r['surface']==surface and r['status']==state for r in records) for state in counts} for surface in ('receiver','library')}, 'source_sha256': hashlib.sha256(public.encode()).hexdigest(),
              'dependencies': dict(das_modules=sorted(set(re.findall(r'^require ([A-Za-z_][A-Za-z0-9_]*)', public, re.MULTILINE))), ue_modules=profile['ue_modules']),
              'staged_module': staged_module, 'members': sorted(records, key=lambda r: (r['class'], r['function']))}
    return package_ir, public, staged, report


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--profile', required=True, type=Path)
    parser.add_argument('--selection', type=Path)
    parser.add_argument('--ir', type=Path)
    parser.add_argument('--stage', type=Path)
    args = parser.parse_args()
    profile = read(args.profile)
    request = selection(profile)
    if args.selection:
        args.selection.parent.mkdir(parents=True, exist_ok=True)
        args.selection.write_text(json.dumps(request, indent=2) + '\n', encoding='utf-8')
    if args.ir:
        if not args.stage:
            parser.error('--ir requires --stage')
        package_ir, public, staged, report = stage(profile, read(args.ir))
        args.stage.mkdir(parents=True, exist_ok=True)
        for name, value in [('package.ir.json', package_ir), ('report.json', report)]:
            (args.stage / name).write_text(json.dumps(value, indent=2, ensure_ascii=False) + '\n', encoding='utf-8')
        (args.stage / (profile['module'] + '.das')).write_text(public, encoding='utf-8', newline='\n')
        (args.stage / (report['staged_module'] + '.das')).write_text(staged, encoding='utf-8', newline='\n')
        probe = 'options gen2\nrequire ' + report['staged_module'] + '\n' + profile['probe'] + '\n'
        (args.stage / 'probe.das').write_text(probe, encoding='utf-8', newline='\n')
        if profile.get('world_probe'):
            (args.stage / 'world-probe.das').write_text('options gen2\nrequire '+report['staged_module']+'\n'+profile['world_probe']+'\n', encoding='utf-8', newline='\n')
        print(json.dumps(report['counts']))


if __name__ == '__main__':
    main()
