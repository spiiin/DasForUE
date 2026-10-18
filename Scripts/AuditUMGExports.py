"""Classify export refusals without changing eligibility or published coverage."""
import argparse
import collections
import hashlib
import json
import re
from pathlib import Path
from GenerateApiPackage import stage

CATEGORIES = {'policy_exclusion', 'implemented_adapter', 'technical_blocker'}

def read(path):
    return json.loads(Path(path).read_text(encoding='utf-8-sig'))

def digest(path):
    return hashlib.sha256(Path(path).read_bytes()).hexdigest()

def key(row):
    return row.get('class_path', row.get('class')), row['function']

def unique(rows):
    result = {}
    for row in rows:
        k = key(row)
        if k in result:
            raise ValueError('Duplicate member: ' + str(k))
        result[k] = row
    return result

def compare_members(a, b):
    left, right = unique(a['members']), unique(b['members'])
    if set(left) != set(right):
        raise ValueError('Package member set changed')
    for k in left:
        for field in ('status', 'reason', 'alias', 'signature', 'surface', 'declaring_class'):
            if left[k].get(field) != right[k].get(field):
                raise ValueError('Package member evidence changed: ' + str(k) + ': ' + field)
    for field in ('package', 'engine_version', 'counts', 'candidate_count'):
        if a[field] != b[field]:
            raise ValueError('Package evidence changed: ' + field)

def partition(report, rules):
    if rules.get('schema_version') != 1 or rules.get('package') != report['package']:
        raise ValueError('Rules schema/package mismatch')
    all_members = unique(report['members'])
    blocked = {k: m for k, m in all_members.items() if m['status'] == 'export_blocked'}
    decisions = unique(rules['members'])
    if len(blocked) != rules['expected_export_blocked'] or len(blocked) != report['counts']['export_blocked']:
        raise ValueError('Export refusal count changed; review the rules')
    if set(blocked) != set(decisions):
        raise ValueError('Unclassified or extra export refusal: missing=' + str(sorted(set(blocked)-set(decisions))) + '; extra=' + str(sorted(set(decisions)-set(blocked))))
    rows = []
    for k in sorted(blocked):
        m, d = blocked[k], decisions[k]
        if m['reason'] != d['expected_reason']:
            raise ValueError('Export reason changed: ' + str(k))
        if d.get('category') not in CATEGORIES:
            raise ValueError('Unknown decision category: ' + str(k))
        if not all(d.get(f) for f in ('group', 'rationale', 'next_action', 'native_evidence')):
            raise ValueError('Decision needs rationale/action/native evidence: ' + str(k))
        if m['reason'] in ('deprecated_function', 'protected_function') or m['function'].endswith('__DelegateSignature'):
            if d['category'] != 'policy_exclusion':
                raise ValueError('Signature/deprecated/protected declaration is not an implemented callable: ' + str(k))
        if d['category'] == 'implemented_adapter':
            adapter = rules['adapters'].get(d.get('adapter'))
            if not adapter or not all(adapter.get(f) for f in ('api', 'scope', 'evidence')):
                raise ValueError('Missing adapter contract/evidence: ' + str(k))
        elif 'adapter' in d:
            raise ValueError('Non-adapter decision carries adapter claim: ' + str(k))
        rows.append(dict(d, raw_status=m['status'], raw_reason=m['reason'], surface=m['surface']))
    return rows

def source_anchor(root, spec):
    relative = Path(spec['path'])
    if relative.is_absolute() or '..' in relative.parts:
        raise ValueError('Evidence path must stay within root')
    path = Path(root) / relative
    lines = path.read_text(encoding='utf-8-sig').splitlines()
    matches = [i+1 for i, line in enumerate(lines) if spec['marker'] in line]
    if not matches:
        raise ValueError('Missing source anchor: ' + str(path) + ': ' + spec['marker'])
    return dict(spec, line=matches[0], sha256=digest(path))

def check_widget_routes(rows, source):
    inputs = set(re.findall(r'DAS_WIDGET_INPUT\((On\w+),', source))
    drag = set(re.findall(r'UDasScriptUserWidget::Native(OnDrag\w+|OnDrop)\s*\(', source))
    for row in rows:
        adapter=row.get('adapter');f=row['function'];owner=row['class_path']
        if adapter:
            allowed=False
            if adapter=='input_reply':allowed=owner=='/Script/UMG.UserWidget' and f in inputs
            elif adapter=='drag':allowed=owner=='/Script/UMG.UserWidget' and f in drag
            elif adapter=='focus_lost':allowed=owner=='/Script/UMG.UserWidget' and f=='OnFocusLost'
            elif adapter=='paint':allowed=owner=='/Script/UMG.UserWidget' and f=='OnPaint'
            elif adapter=='widget_factory':allowed=owner=='/Script/UMG.WidgetBlueprintLibrary' and f=='Create'
            elif adapter=='drag_factory':allowed=owner=='/Script/UMG.WidgetBlueprintLibrary' and f=='CreateDragDropOperation'
            elif adapter.startswith('input_convert_'):allowed=owner=='/Script/EnhancedInput.EnhancedInputLibrary' and f=='Conv_InputActionValueTo'+adapter.removeprefix('input_convert_')
            if not allowed:raise ValueError('Adapter does not implement this declaration: '+str(key(row)))
        if row['class_path'] != '/Script/UMG.UserWidget':
            continue
        f = row['function']
        if row['category'] == 'technical_blocker' and (f in inputs or (f != 'Destruct' and re.search(r'UDasScriptUserWidget::(?:Native'+re.escape(f)+'|'+re.escape(f)+'_Implementation)'+r'\s*\(', source))):
            raise ValueError('Technical callback now has a native route; review classification: ' + f)

def markdown(result):
    labels = {'policy_exclusion':'Исключения политики', 'implemented_adapter':'Готовые адаптеры', 'technical_blocker':'Технические блокеры'}
    counts=result['counts'];adapted=result['partition'].get('implemented_adapter',0)
    lines = ['# UMG / Input: аудит 110 отказов экспорта', '',
        'Профиль: 20 receiver-классов и 5 библиотек; это не аудит всего UMG.',
        'Свежая reflection проверена настоящим пакетным генератором. Исходные статусы и причины сохранены.', '',
        f"**{counts['generation_ready']} generated ready, {counts['export_blocked']} export blocked, {counts['generation_blocked']} generation blocked.** Классификация не меняет eligibility и не публикует API.", '',
        '| Категория | Членов |', '| --- | ---: |']
    lines += ['| '+labels[c]+' | '+str(result['partition'].get(c, 0))+' |' for c in ('policy_exclusion','implemented_adapter','technical_blocker')]
    lines += ['', f"{adapted} адаптированных объявления не прибавляются к числу {counts['generation_ready']}: это отдельные callback/factory/value-helper возможности, часто с более узким контрактом.",
        '25 delegate signatures — определения типов, а не готовые подписки. Protected legacy input и editor helpers остаются закрытыми.',
        'Наличие общего accessor backend не является проверкой конкретного UMG свойства; альтернативы для трёх accessor entrypoints отмечены в строках.', '',
        '## Реальный backlog', '', '| Группа | Объявлений |', '| --- | ---: |']
    counts=collections.Counter(r['group'] for r in result['members'] if r['category']=='technical_blocker')
    lines += ['| '+k+' | '+str(v)+' |' for k,v in sorted(counts.items())]
    lines += ['', 'Два GetBoundActionValue — две точки входа одной возможности polling; payload Enhanced Input callback не заменяет опрос.',
        'Destruct cleanup и отзыв animation lease уже существуют, но не реализуют пользовательские Destruct/OnAnimationFinished callbacks.', '',
        'Следующий порядок: soft brush adapter; группа pointer/focus-path notifications; widget lifecycle и query overrides; animation events, motion и guarded input polling.',
        'После этого расширять профиль на новые классы, сохраняя такой же поимённый отчёт. Порядок — рекомендация аудита, а не обещание открыть internal API без guard-контрактов.', '',
        '## Поимённые решения', '']
    for c in ('technical_blocker','implemented_adapter','policy_exclusion'):
        lines += ['### '+labels[c], '', '| Объявление | Исходная причина | Решение / следующий шаг | Native evidence |', '| --- | --- | --- | --- |']
        for r in result['members']:
            if r['category'] != c:
                continue
            evidence=r['native_evidence']; note=r['rationale']
            if 'adapter' in r:
                a=result['adapters'][r['adapter']];note+=' API: `'+a['api']+'`. Граница: '+a['scope']
            lines.append('| `'+r['class_path']+'.'+r['function']+'` | `'+r['raw_reason']+'` | '+note.replace('|','\\|')+' | `'+evidence['path']+':'+str(evidence['line'])+'` |')
        lines += ['']
    lines += ['## Проверка и ограничения', '',
        'Runner: `Scripts/AuditUMGExports.ps1`; правила: `Bindings/UMGExportAudit.rules.json`; машинный отчёт: `Bindings/Packages/ue_api_umg_input.export-audit.json`.',
        'Проверяются точное разбиение без пропусков/дубликатов, изменившиеся причины, свежие package statuses/signatures, native declaration anchors, adapter source anchors и input routes.',
        'JSON содержит SHA256 исходников и evidence. Это статический аудит существующего кода, не новый runtime-тест адаптеров.',
        'Не изменяются runtime, профиль, installed generated module или coverage summary. Build, полные Foundation/PIE/packaged и physical input не запускаются. Коммит не создаётся.',
        'Не охвачены произвольные native/Blueprint DragDropOperation subclasses, автоматические authored widget overrides, все delegate subscriptions, классы вне профиля и неотражённый Slate API.', '']
    return '\n'.join(lines)

def main():
    p=argparse.ArgumentParser()
    for name in ('repo','engine','profile','reflection','report','summary','rules','output','markdown'):
        p.add_argument('--'+name, type=Path, required=True)
    a=p.parse_args(); rules=read(a.rules); report=read(a.report); summary=read(a.summary)
    if report.get('state') not in ('published','ue_preflight_passed') or summary.get('state')!='published':
        raise ValueError('Expected existing accepted/published package evidence')
    compare_members(report,summary)
    _,_,_,fresh=stage(read(a.profile),read(a.reflection))
    compare_members(report,fresh)
    rows=partition(fresh,rules)
    check_widget_routes(rows,(a.repo/'Plugins/DasForUE/Source/DasRuntime/Private/DasScriptUserWidget.cpp').read_text(encoding='utf-8-sig'))
    for row in rows:
        row['native_evidence']=source_anchor(a.engine,row['native_evidence'])
    adapters={}
    for name,spec in rules['adapters'].items():
        adapters[name]=dict(spec,evidence=[source_anchor(a.repo,s) for s in spec['evidence']])
    result=dict(schema_version=1,package=fresh['package'],engine_version=fresh['engine_version'],counts=fresh['counts'],candidate_count=fresh['candidate_count'],
        partition=dict(collections.Counter(r['category'] for r in rows)),groups=dict(collections.Counter(r['group'] for r in rows)),
        scope=rules['scope'],coverage_changed=False,fresh_reflection=True,real_generator_recheck=True,runtime_adapter_execution='not_run_static_audit_only',
        evidence={n:dict(path=str(getattr(a,n)),sha256=digest(getattr(a,n))) for n in ('profile','reflection','report','summary','rules')},
        regenerated_source_sha256=fresh['source_sha256'],adapters=adapters,members=rows)
    a.output.write_text(json.dumps(result,ensure_ascii=False,indent=2)+'\n',encoding='utf-8')
    a.markdown.write_text(markdown(result),encoding='utf-8')
    print('DAS_UMG_EXPORT_AUDIT=PASS '+json.dumps(result['partition']))

if __name__=='__main__':
    main()
