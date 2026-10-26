"""Generate probe-free variants from the same reflection IR as the test bindings."""
import argparse
import copy
import json
import re
import tempfile
from pathlib import Path
from GenerateBindings import generate as generate_api
from GenerateNativeGameplay import generate as generate_native
from DiscoverNativeGameplay import declarations

ROOT = Path(__file__).resolve().parent.parent

def references(value, excluded):
    if isinstance(value, str):
        return any(path in excluded for path in re.findall(r'/Script/\w+\.\w+', value))
    if isinstance(value, dict):
        return any(references(v, excluded) for k, v in value.items() if k != 'metadata')
    if isinstance(value, list):
        return any(references(v, excluded) for v in value)
    return False

def generate(root=ROOT):
    excluded = set(json.loads((root/'Plugins/DasForUE/Tests/Runtime/ReflectedTypes.json').read_text()))
    api = json.loads((root/'Bindings/Api.json').read_text(encoding='utf-8-sig'))
    native = json.loads((root/'Bindings/NativeGameplay.json').read_text(encoding='utf-8-sig'))
    # Close over types containing fixture references, including inherited types.
    changed = True
    while changed:
        before = len(excluded)
        for item in api['classes'] + api['structs'] + native['classes'] + native['structs'] + native['enums']:
            identity = item.get('type_path', item.get('path'))
            if references(item, excluded) and identity:
                # A native class may have both real and test-typed methods. Drop those members below.
                if 'methods' not in item or item.get('parent') in excluded:
                    excluded.add(identity)
        changed = len(excluded) != before
    api = copy.deepcopy(api)
    for key in ('classes','structs','functions'):
        api[key] = [v for v in api[key] if not references(v, excluded)]
    api_text = generate_api(api, 'ue_generated')
    native = copy.deepcopy(native)
    native['classes'] = [v for v in native['classes'] if v['path'] not in excluded]
    for cls in native['classes']:
        for key in ('fields','methods'):
            cls[key] = [v for v in cls[key] if not references(v, excluded)]
    for key in ('structs','enums'):
        native[key] = [v for v in native[key] if not references(v, excluded)]
    discovery = native['discovery']
    for key in ('classes','enums'):
        discovery[key] = [v for v in discovery[key] if not references(v, excluded)]
    content = root/'Plugins/DasForUE/Content/Das'
    with tempfile.TemporaryDirectory(prefix='das-runtime-bindings-') as temp:
        scripts = Path(temp)
        # Use only production modules, so a fixture module cannot re-enter through discovery.
        names = []
        for entry in (content/'RuntimeScripts.txt').read_text().splitlines():
            entry = entry.strip()
            if not entry or entry.startswith('#'): continue
            name = Path(entry).name
            if name in ('ue_generated.das', 'ue_native_gameplay.das'): continue
            text = (content/entry).read_text(encoding='utf-8-sig')
            if references(text, excluded):
                raise ValueError('Runtime module still references a fixture: '+entry)
            (scripts/name).write_text(text, encoding='utf-8')
            names.append(name[:-4])
        (scripts/'ue_generated.das').write_text(api_text, encoding='utf-8')
        names.append('ue_generated')
        discovery['modules'] = [m for m in discovery['modules'] if m in names]
        header, module = generate_native(native, scripts_root=scripts)
    outputs = {
        content/'Runtime/ue_generated.das': api_text,
        content/'Runtime/ue_native_gameplay.das': module,
        root/'Plugins/DasForUE/Source/DasRuntime/Private/Generated/DasNativeGameplayRuntime.generated.h': header,
    }
    for file, text in outputs.items():
        if references(text, excluded): raise ValueError('Fixture leaked into '+str(file))
    return outputs

def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--check', action='store_true')
    args = parser.parse_args()
    for file, text in generate().items():
        if args.check:
            if not file.exists() or file.read_text(encoding='utf-8') != text:
                raise SystemExit('Stale runtime binding: '+str(file))
        else:
            file.parent.mkdir(parents=True, exist_ok=True)
            file.write_text(text, encoding='utf-8', newline='\n')
    print('DAS_RUNTIME_BINDINGS=PASS')

if __name__ == '__main__': main()
