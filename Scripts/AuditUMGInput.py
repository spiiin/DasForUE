"""Audit existing UMG generation failures; this does not enable a transport."""
import argparse
import collections
import hashlib
import json
from pathlib import Path
from GenerateApiPackage import stage
from GenerateBindings import OPAQUE_TRANSPORTS

INPUT_TYPES = {"/Script/SlateCore."+n for n in (
    'InputEvent','KeyEvent','AnalogInputEvent','CharacterEvent','PointerEvent','NavigationEvent')}
GROUPS = dict.fromkeys(INPUT_TYPES, 'input_events') | {
    '/Script/SlateCore.Geometry':'geometry', '/Script/UMG.EventReply':'event_reply',
    '/Script/UMG.PaintContext':'paint_context', '/Script/UMG.WidgetAnimationHandle':'widget_animation_handle'}
NAVIGATION = {'/Script/Engine.KismetInputLibrary.'+n for n in ('Key_GetNavigationActionFromKey','Key_GetNavigationDirectionFromKey','Key_GetNavigationDirectionFromAnalog')}
PRIORITY = ('paint_context','widget_animation_handle','event_reply','geometry','input_events')
EVIDENCE = {
 'input_storage': ('Engine/Source/Runtime/SlateCore/Public/Input/Events.h', [
     'struct FInputEvent', 'const FWidgetPath* EventPath;', 'uint64 EventTimestamp;',
     'TSet<FKey> PressedButtons;', 'const TSet<FKey>& GetPressedButtons() const',
     'struct FKeyEvent : public FInputEvent','struct FAnalogInputEvent',
     'struct FCharacterEvent','struct FPointerEvent','struct FNavigationEvent']),
 'path_dereference': ('Engine/Source/Runtime/SlateCore/Private/Input/Events.cpp', [
     'return EventPath->FindArrangedWidget','return EventPath->GetWindow();']),
 'event_projections': ('Engine/Source/Runtime/UMG/Private/WidgetBlueprintLibrary.cpp', [
     'FInputEvent UWidgetBlueprintLibrary::GetInputEventFromKeyEvent',
     'FKeyEvent UWidgetBlueprintLibrary::GetKeyEventFromAnalogInputEvent',
     'FInputEvent UWidgetBlueprintLibrary::GetInputEventFromCharacterEvent',
     'FInputEvent UWidgetBlueprintLibrary::GetInputEventFromPointerEvent',
     'FInputEvent UWidgetBlueprintLibrary::GetInputEventFromNavigationEvent']),
 'navigation_policy': ('Engine/Source/Runtime/Slate/Private/Framework/Application/NavigationConfig.cpp', [
     'EUINavigation FNavigationConfig::GetNavigationDirectionFromAnalog(',
     'UserNavigationState.FindOrAdd', 'AnalogState.LastNavigationTime = CurrentTime;', 'AnalogState.Repeats++;']),
 'navigation_dispatch': ('Engine/Source/Runtime/Engine/Private/KismetInputLibrary.cpp', [
     'EUINavigationAction UKismetInputLibrary::Key_GetNavigationActionFromKey',
     'EUINavigation UKismetInputLibrary::Key_GetNavigationDirectionFromKey',
     'EUINavigation UKismetInputLibrary::Key_GetNavigationDirectionFromAnalog',
     'FSlateApplication::IsInitialized()']),
 'paint_and_callbacks': ('Engine/Source/Runtime/UMG/Public/Blueprint/UserWidget.h', [
     'struct FPaintContext','FSlateWindowElementList& OutDrawElements;',
     'const FWidgetStyle& WidgetStyle;', 'FEventReply OnKeyDown(', 'FEventReply OnMouseMove(']),
 'reply': ('Engine/Source/Runtime/UMG/Public/Components/SlateWrapperTypes.h', [
     'struct FEventReply', 'FReply NativeReply;']),
 'geometry': ('Engine/Source/Runtime/SlateCore/Public/Layout/Geometry.h', [
     'struct FGeometry', 'FSlateRenderTransform AccumulatedRenderTransform;']),
 'animation_handle': ('Engine/Source/Runtime/UMG/Public/Animation/WidgetAnimationHandle.h', [
     'struct FWidgetAnimationHandle', 'TWeakPtr<FWidgetAnimationState> WeakState;',
     'A valid handle may still return a null animation state'])}

def read(path): return json.loads(Path(path).read_text(encoding='utf-8-sig'))
def digest(path): return hashlib.sha256(Path(path).read_bytes()).hexdigest()
def key(m): return m['class'], m['function']

def source_evidence(engine):
    result={}
    for name,(file,markers) in EVIDENCE.items():
        path=Path(engine)/file
        text=path.read_text(encoding='utf-8-sig'); lines=text.splitlines()
        anchors=[]
        for marker in markers:
            matches=[i+1 for i,line in enumerate(lines) if marker in line]
            if not matches: raise ValueError('Missing native source anchor: '+file+': '+marker)
            anchors.append(dict(marker=marker,line=matches[0]))
        result[name]=dict(path=file,sha256=digest(path),anchors=anchors)
    return result

def audit(profile, reflection, report, summary):
    # Re-run the real generator, not a hand-maintained list of expected failures.
    _,_,_,fresh=stage(profile,reflection)
    for name in ('package','engine_version','counts','candidate_count','source_sha256'):
        if fresh[name]!=report[name] or report[name]!=summary[name]:
            raise ValueError('Stale or mismatched package evidence: '+name)
    if report['state'] not in ('published','ue_preflight_passed'):
        raise ValueError('Package report has no accepted preflight')
    if summary.get('state')!=report['state']: raise ValueError('Summary publication state differs')
    old={key(m):m for m in report['members']}
    if len(old)!=len(report['members']): raise ValueError('Duplicate report member')
    now={key(m):m for m in fresh['members']}
    if set(old)!=set(now) or any((old[k]['status'],old[k].get('reason','').strip())!=(now[k]['status'],now[k].get('reason','').strip()) for k in now):
        raise ValueError('Member status/reason differs from regenerated evidence')
    listed=summary.get('members',summary.get('blockers',[]))
    if {key(m) for m in listed if m['status']=='generation_blocked'}!={key(m) for m in now.values() if m['status']=='generation_blocked'}:
        raise ValueError('Summary missing or extra generation failure')
    functions={key(f):f for f in reflection['functions']}
    if len(functions)!=len(reflection['functions']): raise ValueError('Duplicate reflected function')
    structs={s['type_path']:s for s in reflection['structs']}
    rows=[]; types=collections.defaultdict(set)
    for m in fresh['members']:
        if m['status']!='generation_blocked': continue
        if m['reason']!='Opaque/empty structs need an adapter':
            raise ValueError('Unclassified generation failure: '+str(key(m)))
        if key(m) not in functions: raise ValueError('Blocked function missing from reflection')
        fields=[]
        def visit(f,path,seen):
            t=f.get('type_path','')
            if f['kind']=='struct':
                if t not in structs: raise ValueError('Missing struct definition: '+t)
                if not structs[t]['fields'] and t not in GROUPS and t not in OPAQUE_TRANSPORTS:
                    raise ValueError('Unclassified opaque type: '+t)
                if t in GROUPS:
                    if structs[t]['fields']: raise ValueError('Expected opaque reflected type: '+t)
                    fields.append(dict(path=path,type_path=t,input=f.get('input',False),output=f.get('output',False),group=GROUPS[t]))
                if t not in seen:
                    for child in structs[t]['fields']: visit(child,path+'.'+child['path'],seen|{t})
            for n in ('element','key','value'):
                if n in f: visit(f[n],path+'.'+n,seen)
        for f in functions[key(m)]['parameters']: visit(f,f['path'],set())
        if not fields: raise ValueError('Opaque failure has no classified type: '+str(key(m)))
        groups={f['group'] for f in fields}
        primary=next(g for g in PRIORITY if g in groups)
        barrier='separate_'+primary+'_contract'
        if primary=='input_events': barrier='live_navigation_policy' if m['class']+'.'+m['function'] in NAVIGATION else 'detached_input_snapshot'
        rows.append(dict(**m,primary_group=primary,required_groups=sorted(groups),fields=fields,planned_barrier=barrier))
        for t in {f['type_path'] for f in fields}: types[t].add(key(m))
    counts=dict(sorted(collections.Counter(m['primary_group'] for m in rows).items()))
    nav=[m for m in rows if m['planned_barrier']=='live_navigation_policy']
    detached=[m for m in rows if m['planned_barrier']=='detached_input_snapshot']
    return dict(schema_version=1,state='audit_passed_contract_only',package=fresh['package'],engine_version=fresh['engine_version'],
        counts=fresh['counts'],generation_failures=len(rows),partition=counts,
        ready_functions=[m['class']+'.'+m['function'] for m in fresh['members'] if m['status']=='generation_ready'],
        scoped_rejections=[m for m in fresh['members'] if m.get('reason','').startswith('input events require exact scalar snapshot function allowlist')],
        opaque_types=[dict(type_path=t,affected_functions=len(v)) for t,v in sorted(types.items())],
        input_scope=dict(standalone_functions=len(detached)+len(nav),detached_snapshot_candidates=len(detached),
            navigation_context_candidates=len(nav),cross_group_functions=[m['class']+'.'+m['function'] for m in rows if 'input_events' in m['required_groups'] and m['primary_group']!='input_events']),
        source_sha256=fresh['source_sha256'],members=rows,
        runtime_transport_implemented=False,coverage_changed=False)

def markdown(r):
    lines=[f"# UMG: аудит {r['generation_failures']} generation failures",'',f"UE `{r['engine_version']}`. Снимок: **{r['counts']['generation_ready']} ready, {r['counts']['export_blocked']} export blocked, {r['generation_failures']} generation blocked**.",
        '', 'Этот отчёт не меняет coverage; состояние реализации указано отдельно в контракте.',
        '', '| Группа | Уникальных функций |', '| --- | ---: |']
    lines += [f'| {g} | {n} |' for g,n in r['partition'].items()]
    i=r['input_scope'];lines += ['',f"Input events: {i['standalone_functions']} отдельных функций; {i['detached_snapshot_candidates']} кандидата на detached snapshot, {i['navigation_context_candidates']} требуют отдельной navigation policy.",
        'Cross-group: '+', '.join(i['cross_group_functions'])+'.',
        '', '## Непрозрачные типы', '', '| Native type | Затронутых функций |', '| --- | ---: |']
    lines += [f"| `{t['type_path']}` | {t['affected_functions']} |" for t in r['opaque_types']]
    lines += ['', 'Числа типов пересекаются; таблица групп выше — непересекающееся разбиение функций.',
        '', '## Поимённый backlog', '', '| Функция | Группа | Поля и направления | Следующий барьер |','| --- | --- | --- | --- |']
    for m in r['members']:
        fields=', '.join(f"`{f['path']}`: {f['type_path'].split('.')[-1]} ({'inout' if f['input'] and f['output'] else 'in' if f['input'] else 'out'})" for f in m['fields'])
        lines.append(f"| {m['class'].split('.')[-1]}.{m['function']} | {m['primary_group']} | {fields} | {m['planned_barrier']} |")
    if r.get('scoped_rejections'):
        lines += ['', '## Явные execution-contract отказы', '', '33 input snapshot functions реализованы. Эти четыре функции остаются закрытыми:', '', '| Функция | Причина |', '| --- | --- |']
        for m in r['scoped_rejections']:
            lines.append('| '+m['class'].split('.')[-1]+'.'+m['function']+' | '+m['reason'].strip().replace('\n',' ').replace('|','\\|')+' |')
    lines += ['', '## Native source evidence', '']
    for name,e in r.get('native_evidence',{}).items():
        lines.append(f"- {name}: `{e['path']}` (SHA256 `{e['sha256']}`); строки "+', '.join(str(a['line']) for a in e['anchors'])+'.')
    lines += ['', 'Контракт и порядок реализации: [InputEventTransportContract](InputEventTransportContract.md).',
        'Input snapshots не реализуют UserWidget overrides, EventReply или paint callbacks автоматически.',
        'Проверки: реальный повтор генерации на свежей reflection + unit tests аудита; без PIE/packaged/physical input.']
    return '\n'.join(lines)+'\n'

def validate_contract(c,r):
    if c.get('status')=='implemented_detached_snapshots':
        if c.get('schema_version')!=1 or set(c['native_types'])!=INPUT_TYPES: raise ValueError('Implemented input event identities differ')
        ready=set(r['ready_functions']);closed={m['class']+'.'+m['function'] for m in r['scoped_rejections']}
        if not set(c['snapshot_function_allowlist'])<=ready or not set(c['deferred_navigation_functions'])<=closed: raise ValueError('Implemented input function coverage differs from contract')
        if c['ownership']['event_path']!='always_null_in_detached_native_copy' or c['ownership']['memcpy_native_layout'] is not False: raise ValueError('Unsafe implemented snapshot ownership')
        if c['navigation']['ordinary_snapshot_dispatch'] is not False or c['ui_callbacks']['queue_reply_callbacks'] is not False: raise ValueError('Unsafe implemented navigation/reply contract')
        return
    if c.get('schema_version')!=1 or c.get('status')!='design_only_not_implemented':
        raise ValueError('Contract must describe a planned transport')
    if set(c['native_types'])!=INPUT_TYPES: raise ValueError('Contract native type identities differ')
    allowed={m['class']+'.'+m['function'] for m in r['members'] if m['planned_barrier']=='detached_input_snapshot'}
    gated={m['class']+'.'+m['function'] for m in r['members'] if m['planned_barrier']=='live_navigation_policy'}
    if set(c['snapshot_function_allowlist'])!=allowed or set(c['deferred_navigation_functions'])!=gated:
        raise ValueError('Contract function allowlist differs from audited barriers')
    if c['ownership']['event_path']!='always_null_in_detached_native_copy' or c['ownership']['memcpy_native_layout'] is not False:
        raise ValueError('Unsafe input event ownership contract')
    if c['navigation']['ordinary_snapshot_dispatch'] is not False or c['ui_callbacks']['queue_reply_callbacks'] is not False:
        raise ValueError('Unsafe navigation or reply callback contract')
    if c['implementation_changes_coverage'] is not False: raise ValueError('Audit cannot claim implementation coverage')

def main():
    p=argparse.ArgumentParser(description=__doc__)
    for n in ('profile','reflection','report','summary','engine','contract','output','markdown'):p.add_argument('--'+n,required=True,type=Path)
    a=p.parse_args();report=read(a.report)
    expected=report.get('preflight',{}).get('profile_sha256')
    if expected and digest(a.profile)!=expected: raise ValueError('Profile bytes changed since preflight')
    result=audit(read(a.profile),read(a.reflection),report,read(a.summary))
    validate_contract(read(a.contract),result)
    if read(a.contract)['status']=='implemented_detached_snapshots':
        result['runtime_transport_implemented']=True
        root=a.contract.parent.parent
        files=['Plugins/DasForUE/Source/DasRuntime/Private/DasInputEvents.cpp','Plugins/DasForUE/Source/DasRuntime/Private/DasInputEventsRuntime.inl','Plugins/DasForUE/Source/DasRuntime/Private/DasRuntime.cpp','Plugins/DasForUE/Content/Das/Scripts/ue_input_events.das','Scripts/GenerateBindings.py']
        result['implementation_evidence']={file:digest(root/file) for file in files}
        import re
        native=(root/files[0]).read_text(encoding='utf-8-sig').split('static const TCHAR* Allowed[]={',1)[1].split('};',1)[0]
        names=re.findall(r'TEXT\("([^\"]+)"\)',native)
        if len(names)!=33 or set(names)!=set(read(a.contract)['snapshot_function_allowlist']): raise ValueError('Native allowlist differs from contract')
    result['native_evidence']=source_evidence(a.engine)
    result['evidence']={n:dict(path=str(getattr(a,n)),sha256=digest(getattr(a,n))) for n in ('profile','reflection','report','summary','contract')}
    a.output.write_text(json.dumps(result,ensure_ascii=False,indent=2)+'\n',encoding='utf-8')
    a.markdown.write_text(markdown(result),encoding='utf-8')
    print('DAS_UMG_INPUT_AUDIT=PASS '+json.dumps(dict(partition=result['partition'],input_scope=result['input_scope'])))
if __name__=='__main__':main()
