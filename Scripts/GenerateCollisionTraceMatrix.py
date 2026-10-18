"""Generate a focused runtime probe for all 24 reflected trace/sweep variants."""
import argparse
import json
from pathlib import Path
from GenerateBindings import class_name, enum_name, struct_name
from CheckCollisionHitResultCoverage import check

def generate(report, ir):
    coverage=check(report, traces_only=True)
    if coverage['failures']:
        raise ValueError('HitResult declarations are still blocked; run coverage checker first')
    functions={f['function']:f for f in ir['functions'] if f['class']=='/Script/Engine.KismetSystemLibrary'}
    records={m['function']:m for m in report['members'] if m['class']=='/Script/Engine.KismetSystemLibrary'}
    names=sorted(n for n in functions if n.startswith(('LineTrace','SphereTrace','BoxTrace','CapsuleTrace')))
    if len(names)!=24:raise ValueError('Expected exactly 24 selected trace variants')
    actor=class_name('/Script/Engine.Actor');body=class_name('/Script/Engine.PrimitiveComponent')
    enabled=enum_name('/Script/Engine.ECollisionEnabled');channel=enum_name('/Script/Engine.ECollisionChannel')
    response=enum_name('/Script/Engine.ECollisionResponse')
    lines=['options gen2','require '+report['staged_module'],'[export]','def main(input:int):int {',
      f'let actor={actor}_from(ue_self_object())',
      'let call=ue_begin_call(ue_self_object(),"K2_GetRootComponent")',
      'ue_require_call_context(call,false)','ue_invoke_call(call)',
      f'let body={body}_from(ue_result_object(call,"returnvalue"))','ue_release_call(call)',
      f'ue_body_SetCollisionEnabled(body,{enabled}.v_QueryOnly)',
      f'ue_body_SetCollisionObjectType(body,{channel}.v_ECC_WorldStatic)',
      f'ue_body_SetCollisionResponseToAllChannels(body,{response}.v_ECR_Block)',
      f'var ignored:array<{actor}>',
      'let start=ue_vector(-300.0lf,0.0lf,0.0lf)','let finish=ue_vector(300.0lf,0.0lf,0.0lf)',
      'let miss_start=ue_vector(-300.0lf,0.0lf,1000.0lf)','let miss_end=ue_vector(300.0lf,0.0lf,1000.0lf)']
    for name in names:
        fn=functions[name];alias=records[name]['alias'];multi='Multi' in name
        lines.append('{')
        def arguments(s='start',e='finish',ignore=False):
            result=[]
            for field in fn['parameters']:
                if not field['input'] or field['path']==fn.get('world_context',''):continue
                path=field['path']
                if path=='start':value=s
                elif path=='end':value=e
                elif path in ('radius','halfheight'):value='10.0' if path=='radius' else '20.0'
                elif path=='halfsize':value='ue_vector(10.0lf,10.0lf,10.0lf)'
                elif path=='orientation':value='ue_rotator(0.0lf,0.0lf,0.0lf)'
                elif path=='tracechannel':value=enum_name(field['type_path'])+'.v_TraceTypeQuery1'
                elif path=='objecttypes':
                    value='types'
                elif path=='profilename':value='ue_name("BlockAll")'
                elif path=='btracecomplex':value='false'
                elif path=='actorstoignore':value='ignored'
                elif path=='drawdebugtype':value=enum_name(field['type_path'])+'.v_None'
                elif path=='bignoreself':value='true' if ignore else 'false'
                elif path in ('tracecolor','tracehitcolor'):value=struct_name(field['type_path'])+'()'
                elif path=='drawtime':value='0.0'
                else:raise ValueError('Unmapped trace input: '+name+'.'+path)
                result.append(value)
            return ','.join(result)
        obj=next((f for f in fn['parameters'] if f['path']=='objecttypes'),None)
        if obj:
            enum=enum_name(obj['element']['type_path'])
            lines += [f'var types:array<{enum}>',f'push(types,{enum}.v_ObjectTypeQuery1)']
        op='<-' if multi else '='
        lines += [f'let hit {op} {alias}('+arguments()+')',f'verify(hit.out_returnvalue,"{name}: hit")']
        if multi:
            lines += [f'verify(length(hit.out_outhits)==1,"{name}: native hit array")','let native=hit.out_outhits[0]']
        else:lines += ['let native=hit.out_outhit']
        lines += ['let view=ue_hit_result_view(native)',
            f'verify(view.blocking_hit && view.distance>200.0lf && view.distance<300.0lf,"{name}: native fields")',
            f'verify(__ue_same_object(__ue_trace_actor(view),ue_self_object()),"{name}: actor identity")',
            f'verify(__ue_same_object(__ue_trace_component(view),{body}_raw(body)),"{name}: component identity")',
            'let broken=ue_collision_gameplay_BreakHitResult(native)',
            f'verify(broken.out_bblockinghit,"{name}: roundtrip flag")',
            f'verify(__ue_same_object({actor}_raw(broken.out_hitactor),ue_self_object()),"{name}: roundtrip actor")',
            f'let miss {op} {alias}('+arguments('miss_start','miss_end')+')',
            f'verify(!miss.out_returnvalue,"{name}: miss")']
        if multi:lines += [f'verify(length(miss.out_outhits)==0,"{name}: empty miss array")']
        lines += ['push(ignored,actor)',f'let filtered {op} {alias}('+arguments()+')',
            f'verify(!filtered.out_returnvalue,"{name}: ignored actor")','clear(ignored)',
            f'let self_filtered {op} {alias}('+arguments(ignore=True)+')',
            f'verify(!self_filtered.out_returnvalue,"{name}: ignore self")',
            f'print("DAS_TRACE_MATRIX_PASS {name}\\n")','}']
    lines += ['return 42','}']
    return '\n'.join(lines)+'\n'

def main():
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument('report',type=Path)
    parser.add_argument('--output',type=Path)
    args=parser.parse_args()
    report=json.loads(args.report.read_text(encoding='utf-8-sig'))
    ir=json.loads((args.report.parent/'reflection.json').read_text(encoding='utf-8-sig'))
    output=args.output or args.report.parent/'trace-matrix.das'
    output.write_text(generate(report,ir),encoding='utf-8')
    print(output)

if __name__=='__main__':main()
