"""Render registry contracts exported by DasExport -TypeSupport=...; never infer API support from a kind name."""
import argparse
import json
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
BASELINE = ROOT / 'Bindings/TypeSupport.json'
DOCUMENT = ROOT / 'Docs/Reference/TypeSupport.md'
BEGIN = '<!-- BEGIN GENERATED TYPE SUPPORT -->'
END = '<!-- END GENERATED TYPE SUPPORT -->'
CAPS = ('value_property', 'snapshot_member', 'delegate_parameter', 'array_element', 'map_value', 'map_key', 'set_element')

def unique_object(pairs):
    result = {}
    for key, value in pairs:
        if key in result:
            raise ValueError('Duplicate JSON key: ' + key)
        result[key] = value
    return result

def load(path):
    data = json.loads(Path(path).read_text(encoding='utf-8-sig'), object_pairs_hook=unique_object)
    validate(data)
    return data

def fields(value, names, label):
    if set(value) != set(names.split()):
        raise ValueError('Unexpected or missing fields in ' + label)

def validate_descriptor(t):
    optional = {'element', 'key', 'value'}
    if set(t) - optional != {'kind', 'type_path', 'weak', 'enum_storage', 'authored_id', 'delegate_wire'}:
        raise ValueError('Incomplete descriptor identity')
    if type(t['weak']) is not bool or type(t['enum_storage']) is not int:
        raise ValueError('Invalid descriptor storage')
    for key in optional & set(t):
        validate_descriptor(t[key])

def validate(data):
    fields(data, 'schema_version scope evidence aot_scope not_inferred adapters', 'root')
    if type(data.get('schema_version')) is not int or data['schema_version'] != 2:
        raise ValueError('Unsupported type support schema')
    if not data.get('scope') or not data.get('evidence') or not data.get('not_inferred') or not data.get('aot_scope'):
        raise ValueError('Missing scope/evidence boundaries')
    adapters = data['adapters']
    ids = [a['id'] for a in adapters]
    if not ids or ids != sorted(set(ids)):
        raise ValueError('Adapter identities must be unique and sorted')
    for a in adapters:
        fields(a, 'id kind struct_path family leaf reference_value container_value_policy default_policy default_kind container_slots script samples native_policy', 'adapter')
        for flag in ('family', 'leaf', 'reference_value', 'container_value_policy'):
            if type(a[flag]) is not bool:
                raise ValueError('Invalid adapter policy: ' + flag)
        if a['default_policy'] not in ('explicit', 'implicit_empty'):
            raise ValueError('Unknown default policy')
        for slot in a['container_slots']:
            fields(slot, 'name hash_key', 'container slot')
            if slot['name'] not in ('element', 'key', 'value') or type(slot['hash_key']) is not bool:
                raise ValueError('Invalid container slot')
        script = a['script']
        fields(script, 'registered module handle native_property self_property authored_reference frame_bridge method_bridge', 'script facet')
        for flag in ('registered', 'native_property', 'self_property', 'authored_reference'):
            if type(script[flag]) is not bool:
                raise ValueError('Invalid script query: ' + flag)
        native = a['native_policy']
        if native is not None:
            fields(native, 'roles transport handle lifetime storage_error call_constraint call_gate capture_snapshot validate_snapshot', 'native policy')
            for flag in ('call_gate', 'capture_snapshot', 'validate_snapshot'):
                if type(native[flag]) is not bool:
                    raise ValueError('Invalid native policy flag')
            if not native['lifetime'] or (native['capture_snapshot'] and not native['validate_snapshot']):
                raise ValueError('Incomplete native lifetime/snapshot contract')
        sample_ids = [s['id'] for s in a['samples']]
        if sample_ids != sorted(set(sample_ids)):
            raise ValueError('Sample identities must be unique and sorted')
        for sample in a['samples']:
            fields(sample, 'id type capabilities', 'sample')
            validate_descriptor(sample['type'])
            if sample['type']['kind'] != a['kind']:
                raise ValueError('Sample belongs to a different kind')
            if set(sample['capabilities']) != set(CAPS):
                raise ValueError('Incomplete property capability contexts')
            for cell in sample['capabilities'].values():
                fields(cell, 'state reason', 'capability')
                if cell['state'] not in ('supported', 'unsupported'):
                    raise ValueError('Unknown property capability state')
                if type(cell['reason']) is not str or ((cell['state'] == 'unsupported') != bool(cell['reason'])):
                    raise ValueError('Support state and validator reason disagree')

def escape(text):
    return str(text).replace('&', '&amp;').replace('|', '&#124;').replace('<', '&lt;').replace('>', '&gt;').replace('`', '&#96;').replace('\r', '').replace('\n', '<br>')

def code(text):
    return '<code>' + escape(text) + '</code>' if text else '—'

def descriptor(t):
    name = t['kind']
    if t.get('type_path'):
        name += ':' + t['type_path']
    if t.get('weak'):
        name = 'weak<' + name + '>'
    children = [descriptor(t[k]) for k in ('element', 'key', 'value') if k in t]
    return name + ('<' + ', '.join(children) + '>' if children else '')

def render(data):
    validate(data)
    lines = [BEGIN, '## Registry-derived contracts', '',
             'Generated from the live registry export in [Bindings/TypeSupport.json](../../Bindings/TypeSupport.json). '
             'Update with `GenerateTypeSupport.py --manifest logs/type-support-live.json`; verify with `--check`.', '',
             '**Scope:** representative property descriptors checked by `ValidateDasPropertyType`. '
             '`Value`, `Snapshot member`, and `Delegate parameter` mean the three **property ABI usages**, '
             'not acceptance by every authored DSL parser, signature validator or callback. '
             'Container columns validate the complete composed descriptor with this sample as the child '
             '(the other map slot is `int`). Native class, interface and weak-reference samples remain separate.', '',
             '`Yes` applies to the shown descriptor only. `No` has the exact validator reason in the JSON. '
             '`Unreported` means an adapter has no representative sample; it is never rendered as supported.', '',
             '| Representative descriptor | Value | Snapshot member | Delegate parameter | Array element | Map value | Map key | Set element |',
             '| --- | --- | --- | --- | --- | --- | --- | --- |']
    count = 0
    for a in data['adapters']:
        if not a['samples']:
            lines.append('| ' + code(a['id']) + ' | ' + ' | '.join(['Unreported'] * len(CAPS)) + ' |')
        for s in a['samples']:
            count += 1
            lines.append('| ' + code(descriptor(s['type'])) + ' | ' + ' | '.join(
                'Yes' if s['capabilities'][key]['state'] == 'supported' else 'No' for key in CAPS) + ' |')
    lines += ['', '### Registered script facets and default policies', '',
              'These are adapter/facet declarations, not an independent support table. '
              '`—` means no dedicated hook is declared; generic lowering may still apply. '
              'View flags describe specialized view uses, not complete field/signature support. '
              '`explicit`/`implicit_empty` is the adapter default policy, not a claim that a particular '
              'property, parameter or computed-default expression is accepted.', '',
              '| Adapter | View module | Specialized view uses | Frame / method bridges | Default policy | Default kind |',
              '| --- | --- | --- | --- | --- | --- |']
    for a in data['adapters']:
        s = a['script']
        uses = ', '.join(key for key in ('native_property', 'self_property', 'authored_reference') if s[key])
        bridges = ' / '.join(code(s[key]) for key in ('frame_bridge', 'method_bridge'))
        lines.append('| ' + ' | '.join((code(a['id']), code(s['module']), escape(uses) or '—', bridges,
                                       code(a['default_policy']), code(a['default_kind']))) + ' |')
    lines += ['', '### Native transports and lifetime policies', '',
              'Transport identifiers and native-call constraints come from the owning adapter. '
              'A property ABI `Yes` above does not bypass this call gate or grant a scoped handle reflected storage. '
              'Opaque handles keep their dedicated entry points; registration does not add generic DSL views.', '',
              '| Adapter | Transport / handle | Lifetime | Native call constraint | Reflected storage constraint |',
              '| --- | --- | --- | --- | --- |']
    for a in data['adapters']:
        n = a['native_policy']
        if n is not None:
            lines.append('| ' + ' | '.join((code(a['id']), code(n['transport']) + ' / ' + code(n['handle']),
                                           code(n['lifetime']), escape(n['call_constraint']) or 'Generic',
                                           escape(n['storage_error']) or 'Generic validation')) + ' |')
    lines += ['', f'Coverage: **{len(data["adapters"])} registered adapters, {count} representative descriptors**.', '',
              '**AOT:** ' + escape(data['aot_scope']) + '. '
              'A scalar-compatible binding can be AOT eligible while an opaque overload of the same operation '
              'is closed. A type-level `Yes` would be misleading.', '',
              '**Not inferred by this registry export:** ' + '; '.join(escape(x) for x in data['not_inferred']) + '.', '', END]
    return '\n'.join(lines)

def replace_section(document, generated):
    if document.count(BEGIN) != 1 or document.count(END) != 1:
        raise ValueError('Expected exactly one generated section')
    start = document.index(BEGIN)
    end = document.index(END, start) + len(END)
    return document[:start] + generated + document[end:]

def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--manifest', type=Path, default=BASELINE)
    parser.add_argument('--check', action='store_true')
    args = parser.parse_args()
    data = load(args.manifest)
    before = DOCUMENT.read_text(encoding='utf-8')
    after = replace_section(before, render(data))
    if args.check:
        if data != load(BASELINE):
            raise ValueError('Live registry differs from Bindings/TypeSupport.json; refresh deliberately')
        if before != after:
            raise ValueError('TypeSupport.md generated section is stale')
    else:
        if args.manifest.resolve() != BASELINE.resolve():
            BASELINE.write_text(args.manifest.read_text(encoding='utf-8-sig'), encoding='utf-8', newline='\n')
        DOCUMENT.write_text(after, encoding='utf-8', newline='\n')
    print('DAS_TYPE_SUPPORT=' + ('CHECK_PASS' if args.check else 'GENERATED'))

if __name__ == '__main__':
    main()
