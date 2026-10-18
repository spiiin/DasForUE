"""Report reflected API coverage without enabling or generating new runtime API."""
import argparse,collections,csv,hashlib,json
from pathlib import Path
from DiscoverNativeGameplay import tokens

ROOT=Path(__file__).resolve().parent.parent
POLICY={'not_blueprint_visible','deprecated','editor_only','not_blueprint_callable','not_public','not_native','delegate_function','metadata:BlueprintProtected','metadata:DeprecatedFunction','metadata:BlueprintInternalUseOnly'}
TYPE={'unsupported_property_type','unsupported_parameter_type'}
ADAPTER={'native_accessor','static','net','metadata:Latent','metadata:CustomThunk','metadata:WorldContext'}

def canonical_hash(value):
    return hashlib.sha256(json.dumps(value,ensure_ascii=False,sort_keys=True,separators=(',',':')).encode()).hexdigest()

def library_wrappers(source):
    """Read literal target + signature pairs from generated wrappers, skipping comments."""
    ts=tokens(source);found={};pending=None
    for i,t in enumerate(ts):
        if t=='def':pending=None
        if t=='ue_begin_static_call' and ts[i+1:i+2]==['(']:
            args=ts[i+2:i+7]
            if len(args)!=5 or args[1]!=',' or args[3]!=',':raise ValueError('Noncanonical generated static call')
            pending=(json.loads(args[0]),json.loads(args[2]))
        if pending and t=='ue_expect_signature':
            if ts[i+1:i+4]!=['(','call',',']:raise ValueError('Noncanonical generated signature')
            signature=json.loads(ts[i+4])
            if pending in found and found[pending]!=signature:raise ValueError('Conflicting generated static signatures')
            found[pending]=signature;pending=None
    return found

def block_category(reasons):
    r=set(reasons)
    if r&POLICY:return 'policy_excluded'
    if r&ADAPTER:return 'requires_adapter'
    if 'metadata:UnsafeDuringActorConstruction' in r:return 'policy_review'
    if r&TYPE:return 'type_blocked'
    if 'unsupported_marshalling_shape' in r:return 'marshalling_blocked'
    return 'unclassified_blocker'

def audit(inventory,native,libraries,wrappers):
    if inventory.get('schema_version')!=1 or native.get('schema_version')!=5 or libraries.get('schema_version')!=5:raise ValueError('Unsupported audit input schema')
    definitions={m['id']:m for m in inventory['members']}
    if len(definitions)!=len(inventory['members']):raise ValueError('Duplicate reflected member IDs')
    classes=sorted(inventory['classes'],key=lambda c:c['path'])
    if len({c['path'] for c in classes})!=len(classes):raise ValueError('Duplicate reflected classes')
    registered={}
    for cls in native['classes']:
        for domain,key in [('field','fields'),('method','methods')]:
            for m in cls[key]:
                k=(cls['path'],domain,m['name'])
                if k in registered:raise ValueError('Duplicate native registration')
                registered[k]=m['signature']
    lib_ok={(f['class'],f['function']):f for f in libraries['functions']}
    lib_bad={(f['class'],f['function']):f['reason'] for f in libraries['diagnostics']}
    if lib_ok.keys()&lib_bad.keys():raise ValueError('Library has both accepted and rejected members')
    summary=collections.Counter();groups={};class_rows=[];member_stats={};surface=[];library_rows=[];observed=set()
    reasons=collections.defaultdict(set);types=collections.defaultdict(set);single_type=collections.defaultdict(set)
    for c in classes:
        counts=collections.Counter();g=c['subsystem'];gc=groups.setdefault(g,collections.Counter());gc['classes']+=1
        for domain,key in [('field','fields'),('method','methods')]:
            if len(c[key])!=len(set(c[key])):raise ValueError('Duplicate receiver member ID')
            for ident in sorted(c[key]):
                if ident not in definitions:raise ValueError('Missing member definition '+ident)
                m=definitions[ident]
                if m['domain']!=domain:raise ValueError('Member domain mismatch')
                stat=member_stats.setdefault(ident,{'id':ident,'domain':domain,'name':m['name'],'declaring_class':m['declaring_class'],'eligible':m['eligible'],'primary_reason':m['primary_reason'],'reasons':sorted(m['reasons']),'type_issues':m['type_issues'],'marshalling_error':m.get('marshalling_error',''),'receiver_count':0,'states':collections.Counter(),'subsystems':set()})
                lib_key=(c['path'],m['name']);reg_key=(c['path'],domain,m['name'])
                if c['function_library'] and domain=='method' and (lib_key in lib_ok or lib_key in lib_bad):
                    if lib_key in lib_ok:
                        f=lib_ok[lib_key]
                        state='library_generated' if wrappers.get(lib_key)==f['signature'] else 'library_wrapper_stale' if lib_key in wrappers else 'library_export_eligible'
                    else:state='library_export_blocked'
                    library_rows.append({'class':c['path'],'function':m['name'],'state':state,'reason':lib_bad.get(lib_key,''),'subsystem':g})
                elif reg_key in registered:
                    observed.add(reg_key)
                    state='native_generated' if m['eligible'] and m['signature']==registered[reg_key] else 'native_registration_stale'
                elif m['eligible']:state='native_eligible_unselected'
                else:state=block_category(m['reasons'])
                counts[state]+=1;gc[state]+=1;summary[state]+=1;stat['receiver_count']+=1;stat['states'][state]+=1;stat['subsystems'].add(g)
                # Reasons overlap; unique declarations, not repeated inherited surfaces.
                if not state.startswith('library_'):
                    for reason in m['reasons']:reasons[reason].add(ident)
                shape_keys={(t['property_kind'],t.get('cpp_type',''),t.get('array_dim',1)) for t in m['type_issues']}
                if m['reasons'] and set(m['reasons'])<=TYPE:
                    for t in shape_keys:types[t].add(ident)
                    if len(shape_keys)==1:single_type[next(iter(shape_keys))].add(ident)
                surface.append({'subsystem':g,'receiver':c['path'],'declaring_class':m['declaring_class'],'domain':domain,'name':m['name'],'state':state,'primary_reason':m['primary_reason'],'reasons':';'.join(sorted(m['reasons'])),'type_issues':json.dumps(m['type_issues'],ensure_ascii=False,sort_keys=True),'marshalling_error':m.get('marshalling_error','')})
        class_rows.append({'path':c['path'],'subsystem':g,'function_library':c['function_library'],'selected_receiver':any(cls['path']==c['path'] for cls in native['classes']),'counts':dict(sorted(counts.items()))})
    scoped={c['path'] for c in classes}
    missing=[{'class':k[0],'domain':k[1],'name':k[2]} for k in sorted(registered.keys()-observed) if k[0] in scoped]
    if missing:raise ValueError('Registered members missing from inventory: '+str(missing[:3]))
    members=[]
    for ident in sorted(member_stats):
        m=member_stats[ident];m['states']=dict(sorted(m['states'].items()));m['subsystems']=sorted(m['subsystems']);members.append(m)
    declared=collections.Counter()
    for m in members:
        if m['eligible']:declared['native_eligible']+=1
        if 'native_generated' in m['states']:declared['native_generated_somewhere']+=1
        if not m['eligible']:declared['native_ineligible']+=1
    reflection={**inventory,'classes':classes,'members':sorted(inventory['members'],key=lambda m:m['id']),'excluded_classes':sorted(inventory['excluded_classes'],key=lambda c:c['path'])}
    result={'schema_version':1,'engine_version':inventory['engine_version'],'scope':inventory['scope'],'modules':sorted(inventory['profile']['modules']),
      'summary':{'classes':len(classes),'selected_receivers_in_scope':sum(c['selected_receiver'] for c in class_rows),'selected_receivers_outside_scope':sorted({c['path'] for c in native['classes']}-scoped),'excluded_classes':len(inventory['excluded_classes']),'unique_declarations':len(members),'receiver_member_occurrences':sum(summary.values()),'states':dict(sorted(summary.items())),'declaration_counts':dict(sorted(declared.items())),'eligible_declarations_never_registered':sum(m['eligible'] and 'native_generated' not in m['states'] for m in members)},
      'subsystems':[{'id':g,**dict(sorted(v.items()))} for g,v in sorted(groups.items())],
      'blockers':[{'reason':k,'unique_declarations':len(v)} for k,v in sorted(reasons.items(),key=lambda kv:(-len(kv[1]),kv[0]))],
      'type_only_opportunities':[{'property_kind':k[0],'cpp_type':k[1],'array_dim':k[2],'affected_unique_declarations':len(v),'single_shape_candidates':len(single_type[k]),'examples':sorted(v)[:6]} for k,v in sorted(types.items(),key=lambda kv:(-len(kv[1]),kv[0]))],
      'single_policy_opportunities':[{'reason':reason,'unique_declarations':sum(set(m['reasons'])=={reason} for m in members),'examples':[m['id'] for m in members if set(m['reasons'])=={reason}][:8]} for reason in ['native_accessor','metadata:UnsafeDuringActorConstruction']],
      'classes':class_rows,'library_members':sorted(library_rows,key=lambda m:(m['class'],m['function'])),'members':members,
      'library_blockers':[{'reason':reason,'receiver_functions':count} for reason,count in sorted(collections.Counter(lib_bad.values()).items(),key=lambda kv:(-kv[1],kv[0]))],
      'marshalling_blockers':[{'reason':reason,'unique_declarations':count} for reason,count in sorted(collections.Counter(m['marshalling_error'] for m in members if m['marshalling_error']).items(),key=lambda kv:(-kv[1],kv[0]))],
      'sources':{'reflection_sha256':canonical_hash(reflection),'native_manifest_sha256':canonical_hash(native),'profile_sha256':canonical_hash(inventory['profile'])}}
    if summary['unclassified_blocker']:raise ValueError('Unclassified reflection blockers')
    return result,surface

def markdown(report):
    s=report['summary'];states=s['states'];get=lambda k:states.get(k,0)
    lines=['# Аудит покрытия API генератора','',f"Движок: `{report['engine_version']}`. Область: {', '.join(report['modules'])}.",
      'Учитываются загруженные отражённые native UClass после явной загрузки модулей. Это не инвентаризация всего C++ Unreal и не процент готовности привязки.',
      '',f"Проверено классов: **{s['classes']}**, выбранных receiver-классов в этой области: **{s['selected_receivers_in_scope']}**. Отдельно исключено интерфейсов/устаревших классов: {s['excluded_classes']}.",
      f"Уникальных объявлений членов: **{s['unique_declarations']}**. С учётом наследования на всех receiver-классах: **{s['receiver_member_occurrences']}** вхождений. Эти два счётчика нельзя смешивать.",
      f"Из уникальных объявлений native-фильтр принимает {s['declaration_counts'].get('native_eligible',0)}: {s['declaration_counts'].get('native_generated_somewhere',0)} уже зарегистрированы хотя бы для одного receiver, {s['eligible_declarations_never_registered']} пока не зарегистрированы ни для одного.",
      '', '## Регистрация и пропуски', '', '| Состояние | Вхождения на receiver-классах |','| --- | ---: |']
    labels={'native_generated':'Есть в native registry, текущая сигнатура совпадает','native_eligible_unselected':'Проходит reflection-фильтр, но receiver/член не зарегистрирован','native_registration_stale':'Регистрация не соответствует reflection','policy_excluded':'Исключено политикой видимости/назначения','policy_review':'Требует пересмотра политики construction','requires_adapter':'Нужен специальный путь/адаптер','type_blocked':'Ограничение типов без других исключений','marshalling_blocked':'Ограничение call-frame ABI без других исключений','library_generated':'Есть статическая DAS-обёртка, сигнатура совпадает','library_export_eligible':'Прошло существующий library exporter, обёртка не создана','library_wrapper_stale':'Статическая обёртка с устаревшей сигнатурой','library_export_blocked':'Отклонено существующим library exporter'}
    for k in sorted(states):lines.append(f'| {labels.get(k,k)} | {states[k]} |')
    n=get('native_generated')+get('native_eligible_unselected')
    if n:lines+=['',f"Регистрация среди проходящих native reflection-фильтр: {get('native_generated')}/{n} ({get('native_generated')/n:.1%}). Это доля регистраций на receiver-классах, не доля всего API UE и не runtime-валидация."]
    lines+=['','`eligible` означает результат reflection/export policy. Конфликты имён типов, состав зависимостей пакета, вывод типов DAS, runtime и packaged здесь не доказываются. Library-функции проверены настоящим ExportBindings; статические вызовы не записываются автоматически в неподдержанный API.',
      '', '## Подсистемы', '', '| Подсистема | Классов | Native registry | Eligible, не выбран | Library: обёртки / eligible без обёрток | Type / ABI | Adapter | Policy |','| --- | ---: | ---: | ---: | ---: | ---: | ---: | ---: |']
    for g in report['subsystems']:
        lines.append(f"| {g['id']} | {g['classes']} | {g.get('native_generated',0)} | {g.get('native_eligible_unselected',0)} | {g.get('library_generated',0)} / {g.get('library_export_eligible',0)} | {g.get('type_blocked',0)} / {g.get('marshalling_blocked',0)} | {g.get('requires_adapter',0)} | {g.get('policy_excluded',0)+g.get('policy_review',0)} |")
    lines+=['','Подсистемы задаются упорядоченными правилами в `Bindings/ApiCoverage.profile.json`: первое совпадение по модулю или базовому классу. Оставшиеся Engine-классы вынесены в assets_and_engine_other. Это техническая группировка, а не официальная классификация Epic.',
      '', '## Причины отказов', '', 'Ниже — уникальные объявления. Причины пересекаются: один метод может одновременно быть latent и иметь неподдержанный параметр. Их суммы не являются числом пропущенных методов.', '', '| Причина | Уникальных объявлений |','| --- | ---: |']
    for b in report['blockers']:lines.append(f"| `{b['reason']}` | {b['unique_declarations']} |")
    lines+=['','### Отдельный путь Function Libraries','','Проверка выполняется существующим ExportBindings с optional-запросом для каждой public native static BlueprintCallable функции. Неудачи не прерывают остальные запросы. Причины ниже относятся к этому экспортёру; static/WorldContext сами по себе не считаются его ошибками.','','| Причина | Функций на library-классах |','| --- | ---: |']
    for b in report['library_blockers']:lines.append(f"| `{b['reason'].replace('|','/').replace(chr(10),' ')}` | {b['receiver_functions']} |")
    lines+=['','## Общие ограничения типов с измеримым эффектом','', 'Только члены, у которых native-фильтр не нашёл иных причин отказа. Число кандидатов с одной формой означает отсутствие других форм в текущей диагностике, а не обещание полной поддержки после одного изменения. Диагностика показывает неподдержанные листья либо ограничение их внешнего контейнера/структуры.', '', '| FProperty | C++ тип | Затронуто уникальных | Кандидаты с одной формой | Пример |','| --- | --- | ---: | ---: | --- |']
    for t in report['type_only_opportunities'][:25]:
        typ=t['cpp_type'].replace('|','\\|')+(f" [fixed dim {t['array_dim']}]" if t['array_dim']!=1 else '');example=t['examples'][0] if t['examples'] else ''
        lines.append(f"| {t['property_kind']} | `{typ}` | {t['affected_unique_declarations']} | {t['single_shape_candidates']} | `{example}` |")
    lines+=['','### Call-frame ABI','','| Диагностика | Уникальных объявлений |','| --- | ---: |']
    for b in report['marshalling_blockers']:lines.append(f"| `{b['reason'].replace('|','/').replace(chr(10),' ')}` | {b['unique_declarations']} |")
    lines+=['','## Возможности общей политики','','У этих объявлений есть только указанная причина отказа. Это кандидаты на общий механизм, а не разрешение снять защитные проверки.','','| Причина | Уникальных объявлений |','| --- | ---: |']
    for b in report['single_policy_opportunities']:lines.append(f"| `{b['reason']}` | {b['unique_declarations']} |")
    lines+=['','## Как воспроизвести','', '```powershell', '.\\Scripts\\AuditApiCoverage.ps1', '# Если экспортёр уже собран:', '.\\Scripts\\AuditApiCoverage.ps1 -SkipBuild', '```', '',
      '- `Bindings/ApiCoverage.summary.json` — компактный снимок результатов и provenance SHA-256.',
      '- `Build/ApiCoverage/report.json` — уникальные члены, все причины, неподдержанные типы и состояния.',
      '- `Build/ApiCoverage/members.csv` — каждое вхождение на receiver-классе, удобно искать конкретный API.',
      '- `Build/ApiCoverage/Reflection.json` — reflection inventory; соседний `.libraries.json` — фактический результат library exporter.',
      '- `Scripts/TestApiCoverage.py` — целевые проверки подсчёта, наследования, нескольких причин и stale-сигнатур.',
      '', 'Запуск не расширяет runtime registry, не генерирует новые игровые привязки и не меняет plugin dependencies. Внешние native/C++ методы, неотражённые поля, произвольные virtual override и отключённые плагины находятся вне этого отчёта.',
      '', 'Managed helpers (MoveTo, Montage, effects, async и др.) не засчитываются как автоматически сгенерированные члены. Для них нужен отдельный контракт подсистемы; наличие адаптера не означает покрытия всех одноимённых методов reflection.',
      '', 'Состояния native относятся к native receiver registry. Наличие альтернативных явных instance-обёрток ExportBindings в ue_generated/ue_library_generated этим счётчиком не измеряется. Поэтому type_blocked или policy_review не означают, что вызов вообще невозможен через другой backend. Проверка статических обёрток читает только явно перечисленные generated-модули и сопоставляет literal target + signature, без выполнения функций.',
      '', 'Диагностика неподдержанных типов ограничена 64 записями на член и глубиной 16; она помогает найти причины, но не является полным рекурсивным описанием каждого отвергнутого типа. AudioMixer, GameplayAbilities, StateTree и другие модули вне профиля здесь не оценивались.',
      '', 'Текущие проверки аудита: необходимая сборка, reflection export, проверка неизменности native registry и целевые Python-тесты. Полные Foundation/PIE/packaged не выполняются.']
    return '\n'.join(lines)+'\n'

def main():
    p=argparse.ArgumentParser(description=__doc__);p.add_argument('--inventory',type=Path,required=True);p.add_argument('--native',type=Path,default=ROOT/'Bindings/NativeGameplay.json');p.add_argument('--libraries',type=Path,required=True);p.add_argument('--wrapper',type=Path,action='append',default=[]);p.add_argument('--output',type=Path,required=True);p.add_argument('--summary',type=Path,required=True);p.add_argument('--markdown',type=Path,required=True);p.add_argument('--csv',type=Path,required=True);a=p.parse_args()
    load=lambda file:json.loads(file.read_text(encoding='utf-8-sig'))
    wrappers={};wrapper_files=a.wrapper or [ROOT/'Plugins/DasForUE/Content/Das/Scripts/ue_library_generated.das',ROOT/'Plugins/DasForUE/Content/Das/Scripts/ue_generated.das']
    for f in wrapper_files:
        for k,v in library_wrappers(f.read_text(encoding='utf-8-sig')).items():
            if k in wrappers and wrappers[k]!=v:raise ValueError('Conflicting wrapper modules')
            wrappers[k]=v
    report,rows=audit(load(a.inventory),load(a.native),load(a.libraries),wrappers)
    report['sources']['wrapper_files']={str(f.relative_to(ROOT)).replace('\\','/'):hashlib.sha256(f.read_bytes()).hexdigest() for f in wrapper_files}
    sources=['Scripts/AuditApiCoverage.py','Scripts/GenerateNativeGameplay.py','Scripts/GenerateBindings.py','Plugins/DasForUE/Source/DasRuntime/Private/DasNativeGameplayCoverage.inl','Plugins/DasForUE/Source/DasRuntime/Private/DasNativeGameplayExport.inl','Plugins/DasForUE/Source/DasRuntime/Private/DasNativeGameplayPolicy.inl','Plugins/DasForUE/Source/DasRuntime/Private/DasNativeGameplayTypes.h','Plugins/DasForUE/Source/DasRuntime/Private/DasMarshalling.inl','Plugins/DasForUE/Source/DasRuntime/Private/DasBindingExport.inl']
    report['sources']['code_files']={f:hashlib.sha256((ROOT/f).read_bytes()).hexdigest() for f in sources}
    for f in [a.output,a.summary,a.markdown,a.csv]:f.parent.mkdir(parents=True,exist_ok=True)
    a.output.write_text(json.dumps(report,ensure_ascii=False,indent=2,sort_keys=True)+'\n',encoding='utf-8')
    compact={k:v for k,v in report.items() if k not in ('members','library_members','classes')}
    a.summary.write_text(json.dumps(compact,ensure_ascii=False,indent=2,sort_keys=True)+'\n',encoding='utf-8')
    a.markdown.write_text(markdown(report),encoding='utf-8')
    with a.csv.open('w',newline='',encoding='utf-8-sig') as f:
        writer=csv.DictWriter(f,fieldnames=['subsystem','receiver','declaring_class','domain','name','state','primary_reason','reasons','type_issues','marshalling_error']);writer.writeheader();writer.writerows(rows)
    print('DAS_API_COVERAGE_REPORT=PASS '+json.dumps(report['summary'],sort_keys=True))
if __name__=='__main__':main()
