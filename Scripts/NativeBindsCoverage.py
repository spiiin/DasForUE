"""Coverage of the native binds against the AngelScript UE binds (informational).

Compares two lists with the native modules of logs/native-binds-manifest.json (written by the DasNativeBindsManifest
commandlet):
  1. the hand-written binds of UnrealEngine-Angelscript (Binds/Bind_*.cpp), by area, before (the step-1 baseline,
     optional) and now;
  2. the reflected functions our generated bindings expose for the classes ue_engine binds (DasNativeGameplay
     .generated.h): which of them are now direct.
Matching is by name (snake_case, K2_ dropped, a few explicit aliases) and by the owner's script type appearing in the
signature. Writes Docs/Dev/Architecture/NativeBindsCoverage.md.

Usage:
    python Scripts/NativeBindsCoverage.py --angelscript <UnrealEngine-Angelscript>/Engine/Plugins/Angelscript/Source/AngelscriptCode/Private/Binds
        [--manifest logs/native-binds-manifest.json] [--baseline step1.json] [--output Docs/Dev/Architecture/NativeBindsCoverage.md]
"""
import argparse
import collections
import json
import os
import re

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
GENERATED = os.path.join(ROOT, 'Plugins', 'DasForUE', 'Source', 'DasRuntime', 'Private', 'Generated',
                         'DasNativeGameplay.generated.h')
PLUGIN_MODULES = ('ue_engine', 'ue_math', 'ue_time')
# Owner in the AngelScript binds -> script type in the native modules (None: free functions).
OWNER_TYPES = {
    'AActor': 'AActor', 'APawn': 'APawn', 'UActorComponent': 'UActorComponent', 'USceneComponent': 'USceneComponent',
    'UWorld': 'UWorld', 'UGameInstance': 'UGameInstance', 'FVector': 'UEVector', 'FVector2D': 'UEVector2D',
    'FRotator': 'UERotator', 'FQuat': 'UEQuat', 'FTransform': 'UETransform', 'FLinearColor': 'UELinearColor',
    'FDateTime': 'UEDateTime', 'FTimespan': 'UETimespan', 'FMath': None,
}
# Names that differ beyond snake_case.
ALIASES = {
    ('FRotator', 'Clamp'): 'get_clamped', ('FMath', 'FRandRange'): 'rand_range', ('FMath', 'FRand'): 'frand',
    ('FDateTime', 'Parse'): 'parse_date_time', ('FDateTime', 'MinValue'): 'min_date_time',
    ('FDateTime', 'MaxValue'): 'max_date_time', ('FTimespan', 'Zero'): 'zero_timespan',
    ('FTimespan', 'MinValue'): 'min_timespan', ('FTimespan', 'MaxValue'): 'max_timespan',
    ('FVector2D', 'ZeroVector'): 'zero_vector2d',
}
# Binds the native modules leave out on purpose, with the reason (counted as "not applicable", not "missing").
NOT_APPLICABLE = {
    ('FMath', 'ComputeBoundingSphereForCone'): 'FSphere result (not a script type)',
    ('FMath', 'FloatSpringInterp'): 'spring state struct (not a script type)',
    ('FMath', 'VectorSpringInterp'): 'spring state struct (not a script type)',
    ('FMath', 'QuaternionSpringInterp'): 'spring state struct (not a script type)',
    ('FQuat', 'InitFromString'): "AngelScript text parsing; scripts build values with ue_foundation's constructors",
    ('FRotator', 'InitFromString'): "AngelScript text parsing; scripts build values with ue_foundation's constructors",
    ('FTransform', 'InitFromString'): "AngelScript text parsing; scripts build values with ue_foundation's constructors",
    ('FVector', 'InitFromString'): "AngelScript text parsing; scripts build values with ue_foundation's constructors",
    ('FVector2D', 'InitFromString'): "AngelScript text parsing; scripts build values with ue_foundation's constructors",
    ('FRotator', 'ToColorString'): 'debug text',
    ('AActor', 'SpawnActor'): 'spawning goes through the DSL (spawn policies, authored classes)',
    ('AActor', 'SpawnPersistentActor'): 'spawning goes through the DSL (spawn policies, authored classes)',
    ('AActor', 'FinishSpawningActor'): 'spawning goes through the DSL (spawn policies, authored classes)',
    ('AActor', 'CreateComponent'): 'components are created through the DSL (component operations)',
    ('AActor', 'GetOrCreateComponent'): 'components are created through the DSL (component operations)',
    ('AActor', '__Actor_CreateComponentByClass'): 'components are created through the DSL (component operations)',
    ('AActor', '__Actor_GetOrCreateComponentByClass'): 'components are created through the DSL (component operations)',
    ('AActor', '__Actor_GetComponentByClass'): 'AngelScript internal behind GetComponentByClass',
    ('AActor', '__Actor_GetAllComponentsByClass'): 'AngelScript internal behind GetComponentsByClass',
    ('UActorComponent', 'SetbIsEditorOnly'): 'editor only',
    ('UActorComponent', 'SetbTickInEditor'): 'editor only',
    ('UActorComponent', 'IsVisualizationComponent'): 'editor only',
    ('UActorComponent', 'SetIsVisualizationComponent'): 'editor only',
    ('UWorld', 'GetCurrentWorld'): "AngelScript global; scripts take the world from an object (get_world)",
    ('UWorld', 'SetGameInstance'): 'engine internal',
    ('UWorld', 'ServerTravel'): 'networking, out of scope',
    ('UGameInstance', 'FindLocalPlayerFromUniqueNetId'): 'FUniqueNetIdRepl (not a script type)',
}
OPERATORS = {'opAdd': '+', 'opSub': '-', 'opMul': '*', 'opDiv': '/', 'opMod': '%', 'opEquals': '==', 'opCmp': '<',
             'opNeg': '-'}
# Script classes of the generated reflected bindings -> native script types.
REFLECTED_CLASSES = {'/Script/Engine.Actor': 'AActor', '/Script/Engine.Pawn': 'APawn',
                     '/Script/Engine.ActorComponent': 'UActorComponent',
                     '/Script/Engine.SceneComponent': 'USceneComponent', '/Script/Engine.World': 'UWorld',
                     '/Script/Engine.GameInstance': 'UGameInstance'}


def norm(name):
    return re.sub(r'[^a-z0-9]', '', name.lower())


def angelscript_members(binds):
    """(file, owner, kind, name) of every bind, as counted in step 1."""
    members = []
    for f in sorted(os.listdir(binds)):
        if not f.startswith('Bind_') or not f.endswith('.cpp'):
            continue
        owner = f[5:-4]
        text = open(os.path.join(binds, f), encoding='utf-8', errors='replace').read()
        pattern = (r'ExistingClass\("([^"]+)"\)|ValueClass<[^>]*>\(\s*"([^"]+)"|FNamespace\s+\w+\(\s*"([^"]+)"|'
                   r'\.(Method|Property|Constructor)\(\s*"([^"]+)"|BindGlobalFunction\(\s*"([^"]+)"|'
                   r'BindGlobalVariable\(\s*"([^"]+)"')
        for m in re.finditer(pattern, text):
            if m.group(1) or m.group(2) or m.group(3):
                owner = m.group(1) or m.group(2) or m.group(3)
                continue
            kind = m.group(4) or ('Global' if m.group(6) else 'Variable')
            decl = m.group(5) or m.group(6) or m.group(7)
            if kind == 'Constructor':
                name = '<ctor>'
            elif kind in ('Property', 'Variable'):
                name = decl.split()[-1]
            else:
                n = re.search(r'([A-Za-z_][A-Za-z0-9_]*)\s*\(', decl)
                name = n.group(1) if n else decl
            members.append((f, owner, kind, name))
    return members


class Native:
    """The plugin's native functions, indexed by normalized name."""

    def __init__(self, manifest):
        self.by_name = collections.defaultdict(list)
        self.modules = []
        self.routed = set()  # UFUNCTION names that gameplay scripts call through a bind (Reflects)
        for module in json.load(open(manifest, encoding='utf-8'))['modules']:
            if module['name'] not in PLUGIN_MODULES:
                continue
            self.modules.append(module)
            for f in module['functions']:
                if f['origin'] != 'helper':
                    self.by_name[norm(f['name'])].append(f)
                if f.get('routed'):
                    self.routed.add(f['reflects'].rsplit(':', 1)[-1])

    def find(self, owner, name):
        script_type = OWNER_TYPES.get(owner, '')
        if script_type == '':
            return None
        if name in OPERATORS:
            candidates = [OPERATORS[name]]
        else:
            alias = ALIASES.get((owner, name))
            base = re.sub(r'^K2_', '', name)
            short = norm(re.sub(r'^[AUF](?=[A-Z])', '', owner))
            candidates = [alias] if alias else [base, short + base]
        for candidate in candidates:
            for f in self.by_name.get(norm(candidate), []):
                signature = [f['receiver'], f['result']] + f['arguments']
                if script_type is None or script_type in signature:
                    return f
        return None


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('--angelscript', required=True)
    parser.add_argument('--manifest', default=os.path.join(ROOT, 'logs', 'native-binds-manifest.json'))
    parser.add_argument('--baseline', help='step-1 coverage JSON: members with a status')
    parser.add_argument('--output', default=os.path.join(ROOT, 'Docs', 'Dev', 'Architecture', 'NativeBindsCoverage.md'))
    args = parser.parse_args()
    native = Native(args.manifest)
    baseline = {}
    if args.baseline:
        for m in json.load(open(args.baseline, encoding='utf-8'))['members']:
            baseline[(m['file'], m['owner'], m['kind'], m['name'])] = m['status']

    # 1. AngelScript binds of the areas the native modules cover.
    rows = collections.OrderedDict()
    missing = collections.defaultdict(list)
    for f, owner, kind, name in angelscript_members(args.angelscript):
        if owner not in OWNER_TYPES or kind == 'Constructor' or kind in ('Property', 'Variable'):
            continue
        before = baseline.get((f, owner, kind, name), '?')
        now = 'native' if native.find(owner, name) else before
        if now != 'native' and (owner, name) in NOT_APPLICABLE:
            now = 'not applicable'
        row = rows.setdefault(owner, collections.Counter())
        row['total'] += 1
        row['before_direct'] += before == 'direct'
        row['before_missing'] += before == 'missing'
        row['native'] += now == 'native'
        row['now_missing'] += now == 'missing'
        row['not_applicable'] += now == 'not applicable'
        if now == 'missing':
            missing[owner].append(name)

    # 2. Reflected functions of the bound classes in the generated bindings.
    reflected = collections.OrderedDict()
    if os.path.exists(GENERATED):
        text = open(GENERATED, encoding='utf-8', errors='replace').read()
        for path, function in re.findall(r'\{TEXT\("(/Script/Engine\.[A-Za-z]+)"\),TEXT\("([A-Za-z0-9_]+)"\)', text):
            script_type = REFLECTED_CLASSES.get(path)
            if not script_type:
                continue
            row = reflected.setdefault(script_type, [0, 0, [], 0])
            if function in row[2]:
                continue
            row[2].append(function)
            row[0] += 1
            name = norm(re.sub(r'^K2_', '', function))
            row[1] += any(script_type == f['receiver'] for f in native.by_name.get(name, []))
            row[3] += function in native.routed

    functions = sum(len(m['functions']) for m in native.modules)
    out = ['# Native binds coverage', '',
           'Generated by `Scripts/NativeBindsCoverage.py` from `logs/native-binds-manifest.json` (the',
           '`DasNativeBindsManifest` commandlet). Informational: matching is by name and by the script type in the',
           'signature, so a few binds may be counted on the wrong side.', '',
           'Native modules: ' + ', '.join('`%s` (%d functions)' % (m['name'], len(m['functions']))
                                         for m in native.modules) + '; %d in total, helpers and inherited methods'
           % functions + ' included.', '',
           '## AngelScript hand-written binds', '',
           'Methods and functions of `Bind_*.cpp` for the owners the native modules cover (no constructors, fields,',
           'float variants). "Before" is the step-1 summary (direct = an extern or a script helper, missing = no',
           'binding at all); "native" is a native bind now; "not applicable" are left out on purpose (the list',
           'below says why).', '',
           '| Owner | Binds | Before: direct | Before: missing | Native now | Not applicable | Still missing |',
           '| --- | ---: | ---: | ---: | ---: | ---: | ---: |']
    for owner, row in rows.items():
        out.append('| %s | %d | %d | %d | %d | %d | %d |' % (
            owner, row['total'], row['before_direct'], row['before_missing'], row['native'], row['not_applicable'],
            row['now_missing']))
    out += ['', '## Reflected functions of the bound classes', '',
            'The functions our generated bindings call through reflection (tens of µs a call, see Performance.md)',
            'for the classes `ue_engine` declares, and how many of them are now direct methods. AngelScript binds',
            'these through reflection too, so they are not in the table above. "Routed" counts the UFUNCTIONs',
            'that gameplay scripts (`ref.K2_GetActorLocation()`) now call through the bind that reflects them',
            '(`Reflects`), matched by name.',
            '',
            '| Script type | Reflected functions | Direct now | Routed |', '| --- | ---: | ---: | ---: |']
    for script_type, (total, direct, _, routed) in reflected.items():
        out.append('| %s | %d | %d | %d |' % (script_type, total, direct, routed))
    out += ['', '## Still missing in these areas', '']
    for owner, names in missing.items():
        out.append('- **%s**: %s' % (owner, ', '.join('`%s`' % n for n in sorted(set(names)))))
    out += ['', '## Not applicable', '']
    for (owner, name), reason in sorted(NOT_APPLICABLE.items()):
        out.append('- %s `%s`: %s' % (owner, name, reason))
    with open(args.output, 'w', encoding='utf-8', newline='\n') as w:
        w.write('\n'.join(out) + '\n')
    print('NATIVE_BINDS_COVERAGE=PASS output=%s' % args.output)


if __name__ == '__main__':
    main()
