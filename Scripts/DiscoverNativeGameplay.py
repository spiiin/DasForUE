"""Discover native receiver declarations before DAS inference (no generated feedback)."""
import argparse,json,re
from pathlib import Path

NATIVE_PATH=re.compile(r'/Script/[A-Za-z_]\w*\.[A-Za-z_]\w*\Z')
IDENT=re.compile(r'[A-Za-z_]\w*\Z')
GENERATED='ue_native_gameplay'
BOOTSTRAP='ue_native_gameplay_sources'
BUILTINS={'ue_foundation'}

def tokens(source):
    """Small declaration lexer: strings/comments cannot create phantom declarations."""
    out=[];i=0
    while i<len(source):
        c=source[i]
        if c.isspace():i+=1;continue
        if source.startswith('//',i):
            end=source.find('\n',i);i=len(source) if end<0 else end+1;continue
        if source.startswith('/*',i):
            depth=1;i+=2
            while i<len(source) and depth:
                if source.startswith('/*',i):depth+=1;i+=2
                elif source.startswith('*/',i):depth-=1;i+=2
                else:i+=1
            if depth:raise ValueError('Unterminated DAS comment')
            continue
        if c in ('"',"'"):
            start=i;quote=c;i+=1
            while i<len(source):
                if source[i]=='\\':i+=2;continue
                if source[i]==quote:i+=1;break
                i+=1
            else:raise ValueError('Unterminated DAS string')
            out.append(source[start:i]);continue
        match=re.match(r'[A-Za-z_]\w*',source[i:])
        if match:out.append(match.group());i+=len(match.group());continue
        out.append(c);i+=1
    return out

def declarations(source):
    ts=tokens(source);requires=[];objects=[];enums=[];module=None;i=0
    while i<len(ts):
        if ts[i] in ('require','module') and i+1<len(ts) and IDENT.fullmatch(ts[i+1]):
            if ts[i]=='require':requires.append(ts[i+1])
            elif module is None:module=ts[i+1]
        if ts[i]!='[':i+=1;continue
        start=i;i+=1;depth=1
        while i<len(ts) and depth:
            if ts[i]=='[':depth+=1
            elif ts[i]==']':depth-=1
            i+=1
        block=ts[start+1:i-1]
        for annotation,kind,items in [('ue_native_object','struct',objects),('ue_native_enum','enum',enums)]:
            if annotation not in block:continue
            if block.count(annotation)!=1:raise ValueError('Repeated '+annotation)
            n=block.index(annotation)
            if n+1>=len(block) or block[n+1]!='(':raise ValueError('Missing native annotation arguments')
            end=n+2;parens=1
            while end<len(block) and parens:
                if block[end]=='(':parens+=1
                elif block[end]==')':parens-=1
                end+=1
            args=block[n+2:end-1]
            if len(args)!=3 or args[:2]!=['path','='] or not args[2].startswith('"'):
                raise ValueError('Native annotations require one literal path')
            try:path=json.loads(args[2])
            except json.JSONDecodeError as exc:raise ValueError('Invalid native path literal') from exc
            if not NATIVE_PATH.fullmatch(path):raise ValueError('Invalid native path '+path)
            if i+1>=len(ts) or ts[i]!=kind or not IDENT.fullmatch(ts[i+1]):
                raise ValueError(annotation+' must annotate a '+kind)
            items.append((path,ts[i+1]))
    return module,requires,objects,enums

def discover(entries,script_dirs,external=()):
    dirs=[Path(p).resolve() for p in script_dirs];seen={};known={};known_enums={};files={};visiting=set()
    external=BUILTINS|set(external)
    def resolve(name):
        matches=list(dict.fromkeys((d/(name+'.das')).resolve() for d in dirs if (d/(name+'.das')).is_file()))
        if len(matches)>1:raise ValueError('Ambiguous DAS module '+name)
        if not matches:
            if name in external:return None
            raise ValueError('Missing DAS module '+name)
        return matches[0]
    def visit(name):
        # The generated module is an output, not a source of receiver declarations.
        if name==GENERATED:name=BOOTSTRAP
        if name in seen:return
        path=resolve(name)
        if path is None:return
        seen[name]=True;visiting.add(name)
        declared,deps,objects,enums=declarations(path.read_text(encoding='utf-8-sig'))
        if declared and declared!=name:raise ValueError('DAS module/file mismatch '+name)
        files[name]=(deps,objects,enums)
        for dep in deps:visit(dep)
        for rows,target in ((objects,known),(enums,known_enums)):
            for native,view in rows:
                if native in target:raise ValueError('Ambiguous native declaration '+native)
                target[native]={'path':native,'module':name,'name':view}
        visiting.remove(name)
    for entry in entries:visit(entry)
    if not known:raise ValueError('No native object declarations discovered')
    # Canonical wrappers must compile independently of the generated methods.
    def depends_generated(name,active):
        if name==GENERATED:return True
        if name in active:return False
        return any(depends_generated(dep,active|{name}) for dep in files.get(name,([],[],[]))[0])
    owners=sorted({v['module'] for v in [*known.values(),*known_enums.values()]})
    for name in owners:
        if depends_generated(name,set()):raise ValueError('Native declaration module depends on generated bindings: '+name)
    return {'schema_version':1,'entries':sorted(set(entries)),'modules':owners,
            'classes':[known[p] for p in sorted(known)],'enums':[known_enums[p] for p in sorted(known_enums)]}

def main():
    p=argparse.ArgumentParser(description=__doc__)
    p.add_argument('--config',type=Path);p.add_argument('--entry',action='append',default=[])
    p.add_argument('--scripts',type=Path,action='append',default=[])
    p.add_argument('--external',action='append',default=[]);p.add_argument('--output',type=Path,required=True)
    a=p.parse_args();entries=a.entry;dirs=a.scripts;external=a.external
    if a.config:
        config=json.loads(a.config.read_text(encoding='utf-8-sig'));base=a.config.resolve().parent
        entries=config['entries']+entries
        dirs=[base/d for d in config['script_dirs']]+dirs
        external=config.get('external_modules',[])+external
    result=discover(entries,dirs,external)
    a.output.parent.mkdir(parents=True,exist_ok=True)
    a.output.write_text(json.dumps(result,ensure_ascii=False,indent=2)+'\n',encoding='utf-8')
    print('DAS_NATIVE_DISCOVERY=PASS classes='+str(len(result['classes'])))
if __name__=='__main__':main()
