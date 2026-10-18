"""Explain package blockers using a fresh reflection inventory and prior package reports.

Inventory type issues are evidence from the native policy, not an exhaustive
diagnosis of the generic ABI. Fixing a reported issue does not promise export.
"""
import argparse
import collections
import copy
import hashlib
import json
from pathlib import Path


def read(path):
    return json.loads(Path(path).read_text(encoding='utf-8-sig'))


def identity(member):
    return member.get('declaring_class') or member['class'], member['function']


def explain(report, inventory, baselines=()):
    if report['engine_version'] != inventory['engine_version']:
        raise ValueError('Inventory engine version differs from package')
    members = copy.deepcopy(report['members'])
    keys = [(m['class'], m['function']) for m in members]
    if len(set(keys)) != len(keys):
        raise ValueError('Duplicate package member')
    counts = collections.Counter(m['status'] for m in members)
    if len(members) != report['candidate_count'] or any(counts[k] != v for k, v in report['counts'].items()) or set(counts) - report['counts'].keys():
        raise ValueError('Package counts do not partition members')
    definitions = {(m['declaring_class'], m['name']): m for m in inventory['members'] if m['domain'] == 'method'}
    baseline_keys = set()
    sources = []
    for baseline in baselines:
        if baseline['engine_version'] != report['engine_version']:
            raise ValueError('Baseline engine version differs from package')
        if baseline['state'] not in ('published', 'ue_preflight_passed'):
            raise ValueError('Baseline has not passed UE preflight')
        baseline_keys.update(identity(m) for m in baseline['members'] if m['status'] == 'generation_ready')
        sources.append(dict(package=baseline['package'], source_sha256=baseline['source_sha256']))
    groups = collections.defaultdict(set)
    kinds = collections.defaultdict(set)
    class_counts = collections.defaultdict(collections.Counter)
    ready = set()
    for m in members:
        key = identity(m)
        class_counts[m['class']][m['status']] += 1
        if m['status'] == 'generation_ready':
            ready.add(key)
            continue
        if not m.get('reason', '').strip():
            raise ValueError('Blocked member has no reason')
        evidence = definitions.get(key)
        if evidence is None:
            raise ValueError('Blocked member missing from inventory: ' + str(key))
        m['native_policy_evidence'] = dict(reasons=evidence['reasons'], type_issues=evidence['type_issues'], marshalling_error=evidence.get('marshalling_error', ''))
        reason = m['reason'].strip()
        if m['status'] == 'generation_blocked':
            group = 'generation_shape'
        elif reason == 'deprecated_function' or 'not_blueprint_callable' in evidence['reasons']:
            group = 'visibility_or_deprecation_policy'
        elif reason == 'requires_special_call_adapter':
            group = 'special_call_adapter'
        elif reason.startswith('unsupported UFunction or struct field type') or reason.startswith('Actor instance handles require'):
            group = 'type_or_abi'
        else:
            group = 'other'
        m['blocker_group'] = group
        groups[group].add(key)
        for issue in evidence['type_issues']:
            kinds[(issue['property_kind'], issue.get('cpp_type', ''))].add(key)
    output = {k: copy.deepcopy(v) for k, v in report.items() if k != 'members'}
    output.update(members=members, blockers=[m for m in members if m['status'] != 'generation_ready'],
        by_class={k: dict(v) for k, v in sorted(class_counts.items())},
        blocker_groups={k: len(v) for k, v in sorted(groups.items())},
        type_issue_counts=[dict(property_kind=k[0], cpp_type=k[1], unique_declarations=len(v)) for k, v in sorted(kinds.items())],
        baseline_packages=sources, generated_unique_declarations=len(ready),
        shared_generated_declarations=len(ready & baseline_keys), additional_generated_declarations=len(ready - baseline_keys),
        evidence_scope='Fresh native-policy type hints; not exhaustive generic-ABI diagnosis. Counts per declaration, not per field. Baseline overlap measures generated packages only, not all existing adapters/native API.')
    return output


def markdown(report):
    lines = [f"# {report['package']}: отчёт генерации", '',
        f"Движок: `{report['engine_version']}`. Состояние: `{report['state']}`.", '',
        '| Класс | Готовы | Export blockers | Generation blockers | Policy exclusions |',
        '| --- | ---: | ---: | ---: | ---: |']
    for name, counts in report['by_class'].items():
        lines.append('| '+name.split('.')[-1]+' | '+' | '.join(str(counts.get(k, 0)) for k in ('generation_ready','export_blocked','generation_blocked','policy_excluded'))+' |')
    lines += ['', f"Всего {report['candidate_count']} кандидатов, {report['unique_declaration_count']} уникальных объявлений. Construction guarded: {report['construction_guarded_count']}.",
        f"Из {report['generated_unique_declarations']} готовых объявлений {report['shared_generated_declarations']} уже сгенерированы в baseline-пакетах; {report['additional_generated_declarations']} отсутствуют в них. Это не число новых возможностей всей привязки.",
        'Baseline: '+', '.join(b['package'] for b in report['baseline_packages'])+'.', '',
        '## Группы блокеров', '', '| Группа | Уникальных объявлений |', '| --- | ---: |']
    lines += [f'| {k} | {v} |' for k, v in report['blocker_groups'].items()]
    lines += ['', '## Подсказки отражённого native-аудита', '',
        'Это отдельная policy-оценка тех же объявлений. Она уточняет пути проблемных полей, но не перечисляет все будущие generic ABI blockers. Устранение одного типа не гарантирует прохождения функции.', '',
        '| Тип | Уникальных объявлений с проблемой |', '| --- | ---: |']
    lines += [f"| `{m['property_kind']}: {m['cpp_type']}` | {m['unique_declarations']} |" for m in report['type_issue_counts']]
    lines += ['', 'Числа типов пересекаются: несколько weak-полей одного HitResult считаются одним объявлением для каждого типа.', '',
        '## Поимённый backlog', '', '| Функция | Стадия / причина | Пути проблемных полей (native policy) |', '| --- | --- | --- |']
    for m in report['blockers']:
        paths = ', '.join('`'+t['path']+'`' for t in m['native_policy_evidence']['type_issues'])
        paths = paths or m['native_policy_evidence']['marshalling_error'].strip().replace('|', '\\|').replace('\n', ' ') or '—'
        reason = m['reason'].strip().replace('|', '\\|').replace('\n', ' ')
        lines.append(f"| {m['class'].split('.')[-1]}.{m['function']} | {m['status']}: {reason} | {paths} |")
    return '\n'.join(lines)+'\n'


def main():
    p = argparse.ArgumentParser(description=__doc__)
    for key in ('report', 'inventory', 'output', 'markdown'):
        p.add_argument('--'+key, required=True, type=Path)
    p.add_argument('--baseline', action='append', default=[], type=Path)
    a = p.parse_args()
    report = explain(read(a.report), read(a.inventory), [read(b) for b in a.baseline])
    report['evidence_sha256'] = {key: hashlib.sha256(path.read_bytes()).hexdigest() for key, path in [('report', a.report), ('inventory', a.inventory)]}
    a.output.write_text(json.dumps(report, indent=2, ensure_ascii=False)+'\n', encoding='utf-8')
    a.markdown.write_text(markdown(report), encoding='utf-8')
    print(json.dumps(dict(counts=report['counts'], groups=report['blocker_groups'], shared=report['shared_generated_declarations'], additional=report['additional_generated_declarations'])))


if __name__ == '__main__':
    main()
