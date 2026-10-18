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
        'Классификация export refusals ведётся отдельно: [UMGExportBlockers](UMGExportBlockers.md).',
        '', '| Группа | Уникальных функций |', '| --- | ---: |']
    lines += [f'| {g} | {n} |' for g,n in r['partition'].items()]
    i=r['input_scope'];lines += ['',('33 input snapshot functions реализованы; непрозрачные input-event shapes больше не входят в этот backlog.' if r.get('runtime_transport_implemented') else f"Input events: {i['standalone_functions']} отдельных функций; {i['detached_snapshot_candidates']} кандидата на detached snapshot, {i['navigation_context_candidates']} требуют отдельной navigation policy."),
        (('Оставшиеся execution-contract отказы перечислены ниже.' if r.get('scoped_rejections') else 'Navigation и DetectDragIfPressed открыты через scoped live adapters; отдельных execution-contract отказов нет.') if r.get('runtime_transport_implemented') else 'Cross-group: '+', '.join(i['cross_group_functions'])+'.'),
        '', '## Непрозрачные типы', '', '| Native type | Затронутых функций |', '| --- | ---: |']
    lines += [f"| `{t['type_path']}` | {t['affected_functions']} |" for t in r['opaque_types']]
    lines += ['', 'Числа типов пересекаются; таблица групп выше — непересекающееся разбиение функций.',
        '', '## Поимённый backlog', '', '| Функция | Группа | Поля и направления | Следующий барьер |','| --- | --- | --- | --- |']
    for m in r['members']:
        fields=', '.join(f"`{f['path']}`: {f['type_path'].split('.')[-1]} ({'inout' if f['input'] and f['output'] else 'in' if f['input'] else 'out'})" for f in m['fields'])
        lines.append(f"| {m['class'].split('.')[-1]}.{m['function']} | {m['primary_group']} | {fields} | {m['planned_barrier']} |")
    if r.get('scoped_rejections'):
        lines += ['', '## Явные execution-contract отказы', '', '33 input snapshot functions реализованы. Оставшиеся функции требуют отдельного live navigation adapter:', '', '| Функция | Причина |', '| --- | --- |']
        for m in r['scoped_rejections']:
            lines.append('| '+m['class'].split('.')[-1]+'.'+m['function']+' | '+m['reason'].strip().replace('\n',' ').replace('|','\\|')+' |')
    lines += ['', '## Native source evidence', '']
    for name,e in r.get('native_evidence',{}).items():
        lines.append(f"- {name}: `{e['path']}` (SHA256 `{e['sha256']}`); строки "+', '.join(str(a['line']) for a in e['anchors'])+'.')
    if r.get('implemented_navigation_functions'): lines += ['', 'Три navigation queries открыты через [live input adapter](InputNavigation.md), current native event и weak Slate user. Analog query потребляет событие и обновляет repeat state не более одного раза на callback.']
    if r.get('implemented_drag_events'): lines += ['', 'DetectDragIfPressed и шесть native drag/drop callbacks реализованы через [managed DragDropOperation](DragDrop.md); source/participating target Destroy и committed reload отзывают операцию.']
    if r.get('implemented_paint_functions'): lines += ['', '5 PaintContext draw functions открыты через [синхронный OnPaint bridge](PaintContext.md); script-команды публикуются только после успешного callback.']
    if r.get('implemented_widget_animation_functions'): lines += ['', '4 WidgetAnimation producers открыты через [weak WidgetAnimation transport](WidgetAnimation.md); finish, NativeDestruct и смена VM отзывают доступ.']
    if r.get('implemented_event_reply_functions'): lines += ['', '12 EventReply functions открыты через [scoped callback transport](EventReply.md). Reply и call frames действуют только внутри текущего синхронного callback.']
    if r.get('implemented_geometry_functions'): lines += ['', '17 Geometry functions реализованы через [owned Geometry snapshots](Geometry.md); эта группа больше не входит в backlog.']
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
        result['implemented_snapshot_functions']=33
        result['scoped_rejection_count']=len(result['scoped_rejections'])
        root=a.contract.parent.parent
        files=['Plugins/DasForUE/Source/DasRuntime/Private/DasInputEvents.cpp','Plugins/DasForUE/Source/DasRuntime/Private/DasInputEventsRuntime.inl','Plugins/DasForUE/Source/DasRuntime/Private/DasRuntime.cpp','Plugins/DasForUE/Content/Das/Scripts/ue_input_events.das','Scripts/GenerateBindings.py']
        result['implementation_evidence']={file:digest(root/file) for file in files}
        import re
        native=(root/files[0]).read_text(encoding='utf-8-sig').split('static const TCHAR* Allowed[]={',1)[1].split('};',1)[0]
        names=re.findall(r'TEXT\("([^\"]+)"\)',native)
        if len(names)!=33 or set(names)!=set(read(a.contract)['snapshot_function_allowlist']): raise ValueError('Native allowlist differs from contract')
    geometry_contract=a.contract.parent/'Geometry.contract.json'
    if geometry_contract.exists() and read(geometry_contract).get('status')=='implemented_owned_snapshot':
        geometry=read(geometry_contract)
        if len(geometry['functions'])!=17 or not set(geometry['functions'])<=set(result['ready_functions']) or result['partition'].get('geometry',0): raise ValueError('Implemented Geometry coverage differs from contract')
        import re
        native=(a.contract.parent.parent/'Plugins/DasForUE/Source/DasRuntime/Private/DasGeometry.cpp').read_text(encoding='utf-8-sig').split('static const TCHAR* Allowed[]={',1)[1].split('};',1)[0]
        names=re.findall(r'TEXT\("([^\"]+)"\)',native)
        if len(names)!=17 or set(names)!=set(geometry['functions']): raise ValueError('Native Geometry allowlist differs from contract')
        result['implemented_geometry_functions']=17
        result['geometry_contract_sha256']=digest(geometry_contract)
    reply_contract=a.contract.parent/'EventReply.contract.json'
    if reply_contract.exists() and read(reply_contract).get('status')=='implemented_scoped_callback':
        reply=read(reply_contract)
        if len(reply['functions'])!=12 or not set(reply['functions'])<=set(result['ready_functions']) or result['partition'].get('event_reply',0): raise ValueError('Implemented EventReply coverage differs from contract')
        import re
        root=a.contract.parent.parent
        native=(root/'Plugins/DasForUE/Source/DasRuntime/Private/DasEventReplyTransport.cpp').read_text(encoding='utf-8-sig').split('static const TCHAR* Allowed[]={',1)[1].split('};',1)[0]
        names=['/Script/UMG.WidgetBlueprintLibrary.'+n for n in re.findall(r'TEXT\("([^\"]+)"\)',native)]
        if len(names)!=12 or set(names)!=set(reply['functions']): raise ValueError('Native EventReply allowlist differs from contract')
        result['implemented_event_reply_functions']=12
        result['event_reply_contract_sha256']=digest(reply_contract)
        result['event_reply_implementation_evidence']={file:digest(root/file) for file in ['Plugins/DasForUE/Source/DasRuntime/Private/DasEventReplyTransport.cpp','Plugins/DasForUE/Source/DasRuntime/Private/DasEventReplyCalls.inl','Plugins/DasForUE/Source/DasRuntime/Private/DasWidgetEventsRuntime.inl']}
    navigation_contract=a.contract.parent/'InputNavigation.contract.json'
    if navigation_contract.exists() and read(navigation_contract).get('status')=='implemented_live_navigation':
        navigation=read(navigation_contract)
        if len(navigation['functions'])!=3 or not set(navigation['functions'])<=set(result['ready_functions']): raise ValueError('Navigation contract coverage mismatch')
        if navigation['ordinary_snapshot_dispatch'] is not False: raise ValueError('Navigation cannot be ordinary detached snapshot dispatch')
        import re
        root=a.contract.parent.parent
        native=(root/'Plugins/DasForUE/Source/DasRuntime/Private/DasInputEvents.cpp').read_text(encoding='utf-8-sig').split('bool DasInputNavigationFunctionAllowed(',1)[1].split('static const TCHAR* Names[]={',1)[1].split('};',1)[0]
        names=['/Script/Engine.KismetInputLibrary.'+n for n in re.findall(r'TEXT\("([^\"]+)"\)',native)]
        if set(names)!=set(navigation['functions']): raise ValueError('Native navigation allowlist mismatch')
        result['implemented_navigation_functions']=3
        result['navigation_contract_sha256']=digest(navigation_contract)
        result['navigation_implementation_evidence']={file:digest(root/file) for file in ['Plugins/DasForUE/Source/DasRuntime/Private/DasInputNavigation.inl','Plugins/DasForUE/Source/DasRuntime/Private/DasRuntime.cpp','Plugins/DasForUE/Source/DasRuntime/Private/DasInputEvents.cpp']}
    drag_contract=a.contract.parent/'DragDrop.contract.json'
    if drag_contract.exists() and read(drag_contract).get('status')=='implemented_managed_drag':
        drag=read(drag_contract)
        if drag['generated_function'] not in result['ready_functions'] or len(drag['events'])!=6: raise ValueError('DragDrop contract coverage mismatch')
        result['implemented_drag_events']=6
        result['drag_contract_sha256']=digest(drag_contract)
        root=a.contract.parent.parent
        result['drag_implementation_evidence']={file:digest(root/file) for file in ['Plugins/DasForUE/Source/DasRuntime/Private/DasDragDropOperation.cpp','Plugins/DasForUE/Source/DasRuntime/Private/DasDragRuntime.inl','Plugins/DasForUE/Source/DasRuntime/Private/DasScriptUserWidget.cpp','Plugins/DasForUE/Content/Das/Scripts/ue_drag_drop.das']}
    animation_contract=a.contract.parent/'WidgetAnimation.contract.json'
    if animation_contract.exists() and read(animation_contract).get('status')=='implemented_weak_capability':
        animation=read(animation_contract)
        if len(animation['functions'])!=4 or not set(animation['functions'])<=set(result['ready_functions']) or result['partition'].get('widget_animation_handle',0): raise ValueError('Implemented WidgetAnimation coverage differs from contract')
        import re
        root=a.contract.parent.parent
        native=(root/'Plugins/DasForUE/Source/DasRuntime/Private/DasWidgetAnimationTransport.cpp').read_text(encoding='utf-8-sig').split('static const TCHAR* Allowed[]={',1)[1].split('};',1)[0]
        names=['/Script/UMG.UserWidget.'+n for n in re.findall(r'TEXT\("([^\"]+)"\)',native)]
        if len(names)!=4 or set(names)!=set(animation['functions']): raise ValueError('Native WidgetAnimation allowlist differs from contract')
        result['implemented_widget_animation_functions']=4
        result['widget_animation_contract_sha256']=digest(animation_contract)
        result['widget_animation_implementation_evidence']={file:digest(root/file) for file in ['Plugins/DasForUE/Source/DasRuntime/Private/DasWidgetAnimationTransport.cpp','Plugins/DasForUE/Source/DasRuntime/Private/DasWidgetAnimationRuntime.inl','Plugins/DasForUE/Content/Das/Scripts/ue_widget_animation.das']}
    paint_contract=a.contract.parent/'PaintContext.contract.json'
    if paint_contract.exists() and read(paint_contract).get('status')=='implemented_scoped_paint':
        paint=read(paint_contract)
        if len(paint['functions'])!=5 or not set(paint['functions'])<=set(result['ready_functions']) or result['partition'].get('paint_context',0): raise ValueError('Implemented PaintContext coverage differs from contract')
        import re
        root=a.contract.parent.parent
        native=(root/'Plugins/DasForUE/Source/DasRuntime/Private/DasPaintTransport.cpp').read_text(encoding='utf-8-sig').split('static const TCHAR* Allowed[]={',1)[1].split('};',1)[0]
        names=['/Script/UMG.WidgetBlueprintLibrary.'+n for n in re.findall(r'TEXT\("([^\"]+)"\)',native)]
        if len(names)!=5 or set(names)!=set(paint['functions']): raise ValueError('Native PaintContext allowlist differs from contract')
        result['implemented_paint_functions']=5
        result['paint_contract_sha256']=digest(paint_contract)
        result['paint_implementation_evidence']={file:digest(root/file) for file in ['Plugins/DasForUE/Source/DasRuntime/Private/DasPaintTransport.cpp','Plugins/DasForUE/Source/DasRuntime/Private/DasPaintRuntime.inl','Plugins/DasForUE/Source/DasRuntime/Private/DasScriptUserWidget.cpp','Plugins/DasForUE/Content/Das/Scripts/ue_paint.das']}
    result['native_evidence']=source_evidence(a.engine)
    result['evidence']={n:dict(path=str(getattr(a,n)),sha256=digest(getattr(a,n))) for n in ('profile','reflection','report','summary','contract')}
    a.output.write_text(json.dumps(result,ensure_ascii=False,indent=2)+'\n',encoding='utf-8')
    a.markdown.write_text(markdown(result),encoding='utf-8')
    print('DAS_UMG_INPUT_AUDIT=PASS '+json.dumps(dict(partition=result['partition'],input_scope=result['input_scope'])))
if __name__=='__main__':main()
