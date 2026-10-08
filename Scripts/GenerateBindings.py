"""Generate deterministic daScript wrappers from DasExport schema v5 (stdlib only)."""
import argparse
import hashlib
import json
import math
import struct
from decimal import Decimal
import re
from pathlib import Path


def ident(value):
    result = re.sub(r'[^A-Za-z0-9_]', '_', value)
    return result if result and not result[0].isdigit() else '_' + result


def enum_name(path):
    return 'E_' + ident(path.rsplit('.', 1)[-1]) + '_' + hashlib.sha256(path.encode()).hexdigest()[:8]


def struct_name(path):
    return 'S_' + ident(path.rsplit('.', 1)[-1]) + '_' + hashlib.sha256(path.encode()).hexdigest()[:8]


def class_name(path):
    return 'C_' + ident(path.rsplit('.', 1)[-1]) + '_' + hashlib.sha256(path.encode()).hexdigest()[:8]


def hash_container_identity(field):
    types = [field['key'], field['value']] if field['kind'] == 'map' else [field['element']]
    identity = json.dumps([field['kind']] + [[t['kind'], t['type_path']] for t in types], separators=(',', ':'))
    return identity


def hash_container_name(field):
    types = [field['key'], field['value']] if field['kind'] == 'map' else [field['element']]
    identity = hash_container_identity(field)
    return 'H_' + field['kind'].capitalize() + '_' + '_'.join(t['kind'] for t in types) + '_' + hashlib.sha256(identity.encode()).hexdigest()[:8]


def quote(value):
    # Escape the interpolation syntax as well as string delimiters in generated daScript.
    return json.dumps(value, ensure_ascii=False).replace('{', r'\{').replace('}', r'\}')


def default_expr(field, enums):
    text = field['default_text']
    if not isinstance(text, str): raise ValueError('default_text must be a string')
    if not field['input'] or field['output']: raise ValueError('defaults require input-only parameters')
    kind = field['kind']
    if kind == 'int':
        if not re.fullmatch(r'[+-]?[0-9]+', text): raise ValueError('expected decimal int32')
        value = int(text)
        if not -(2**31) <= value < 2**31: raise ValueError('int32 default out of range')
        return str(value) if value != -(2**31) else '(-2147483647 - 1)'
    if kind in ('float', 'double'):
        if not re.fullmatch(r'[+-]?(?:[0-9]+(?:\.[0-9]*)?|\.[0-9]+)(?:[eE][+-]?[0-9]+)?', text):
            raise ValueError('expected finite decimal floating-point value')
        value = float(text)
        if not math.isfinite(value) or (value == 0 and Decimal(text) != 0): raise ValueError('floating default out of range')
        if kind == 'float':
            try: rounded = struct.unpack('f', struct.pack('f', value))[0]
            except OverflowError: raise ValueError('float32 default out of range') from None
            if not math.isfinite(rounded) or (value != 0 and rounded == 0): raise ValueError('float32 default out of range')
        return repr(value) + ('lf' if kind == 'double' else '')
    if kind == 'bool':
        if text not in ('true', 'false'): raise ValueError('expected true or false')
        return text
    if kind in ('string', 'name'):
        if '\0' in text: raise ValueError('embedded NUL is unsupported')
        if kind == 'name' and len(text.encode('utf-16-le')) // 2 >= 1024: raise ValueError('FName default too long')
        return quote(text) if kind == 'string' else 'ue_name(' + quote(text) + ')'
    if kind == 'enum':
        members = enums.get(field['type_path'], [])
        short_type = field['type_path'].rsplit('.', 1)[-1]
        for member in members:
            if text in (member['name'], short_type + '::' + member['name']):
                return enum_name(field['type_path']) + '.v_' + ident(member['name'])
        raise ValueError('unknown enum default symbol')
    if kind == 'object' and text in ('None', 'nullptr', 'NULL'):
        return class_name(field['type_path']) + '_null()'
    raise ValueError('unsupported default kind or representation: ' + kind)


def generate(ir):
    if ir.get('schema_version') != 5:
        raise ValueError('Unsupported IR schema version')
    classes = {}
    for item in ir['classes']:
        path = item['type_path']
        if not path.startswith('/') or path in classes:
            raise ValueError('Invalid or duplicate class identity')
        classes[path] = item['parent']
    def ancestors(path):
        seen = {path}
        parent = classes[path]
        while parent:
            if parent not in classes: raise ValueError('Missing parent class')
            if parent in seen: raise ValueError('Cyclic class hierarchy')
            seen.add(parent)
            yield parent
            parent = classes[parent]
    for path in classes: list(ancestors(path))
    for fn in ir['functions']:
        if fn['class'] not in classes: raise ValueError('Missing receiver class')
        for field in fn['fields']:
            if field['kind'] == 'object' and field['type_path'] not in classes:
                raise ValueError('Missing object class')
    aliases = set()
    enums = {}
    kinds = {'int', 'float', 'double', 'bool', 'string', 'object', 'name', 'enum', 'array', 'map', 'set'}
    for fn in ir['functions']:
        alias = fn['alias']
        if not re.fullmatch(r'[A-Za-z_][A-Za-z0-9_]*', alias) or alias in aliases or not alias.startswith(('actor_', 'fixture_', 'ue_')):
            raise ValueError('Invalid, reserved or duplicate alias: ' + alias)
        aliases.add(alias)
        for key in ('class', 'function', 'signature'):
            if not isinstance(fn[key], str) or not fn[key]:
                raise ValueError('Missing ' + key)
        names = set()
        for field in fn['fields']:
            name = ident(field['path'])
            if name in names or field['kind'] not in kinds:
                raise ValueError('Unsupported or colliding field: ' + field['path'])
            names.add(name)
            if type(field['input']) is not bool or type(field['output']) is not bool or not (field['input'] or field['output']):
                raise ValueError('Invalid field direction')
            if field['kind'] == 'enum':
                path, members = field['type_path'], field['members']
                if not path.startswith('/') or not members:
                    raise ValueError('Enum needs type identity and members')
                symbols = [ident(m['name']) for m in members]
                if len(set(symbols)) != len(symbols):
                    raise ValueError('Colliding enum symbols')
                for member in members:
                    int(member['value'])  # Values are decimal strings to avoid JSON floating point loss.
                if path in enums and enums[path] != members:
                    raise ValueError('Inconsistent enum definitions')
                enums[path] = members

    structures = {}
    for item in ir['structs']:
        path = item['type_path']
        if not path.startswith('/') or path in structures:
            raise ValueError('Invalid or duplicate struct identity')
        structures[path] = item
    order, visiting, visited = [], set(), set()
    def visit(path):
        if path not in structures:
            raise ValueError('Missing struct definition: ' + path)
        if path in visiting:
            raise ValueError('Recursive value struct: ' + path)
        if path in visited: return
        if len(visiting) >= 16: raise ValueError('Struct nesting limit exceeded')
        visiting.add(path)
        seen = set()
        if not structures[path]['fields']: raise ValueError('Opaque/empty structs need an adapter')
        for field in structures[path]['fields']:
            name = field['path']
            if not name or '.' in name or ident(name) in seen:
                raise ValueError('Invalid or colliding struct field')
            seen.add(ident(name))
            if field['kind'] == 'struct': visit(field['type_path'])
            elif field['kind'] not in kinds or field['kind'] in ('object', 'array', 'map', 'set'):
                raise ValueError('Unsupported struct leaf')
            if field['kind'] == 'enum' and field['type_path'] not in enums:
                raise ValueError('Missing struct enum definition')
        visiting.remove(path)
        visited.add(path)
        order.append(path)
    for path in sorted(structures): visit(path)

    def leaves(field, path):
        if field['kind'] in ('map', 'set'):
            yield dict(path=path, kind=field['kind'], type_path=field['type_path'])
            if field['kind'] == 'map':
                yield from leaves(field['key'], path + '.*.key')
                yield from leaves(field['value'], path + '.*.value')
            else: yield from leaves(field['element'], path + '.*')
        elif field['kind'] == 'array':
            yield dict(path=path, kind='array', type_path=field['type_path'])
            yield from leaves(field['element'], path + '.*')
        elif field['kind'] == 'struct':
            if field['type_path'] not in structures:
                raise ValueError('Missing struct definition: ' + field['type_path'])
            for child in structures[field['type_path']]['fields']:
                yield from leaves(child, path + '.' + child['path'])
        else:
            yield dict(path=path, kind=field['kind'], type_path=field['type_path'])
    hash_containers = {}
    scalar_properties = {'int':'IntProperty', 'float':'FloatProperty', 'double':'DoubleProperty', 'bool':'BoolProperty', 'string':'StrProperty', 'name':'NameProperty', 'enum':'EnumProperty'}
    for fn in ir['functions']:
        flattened, names = [], set()
        for param in fn['parameters']:
            if param['kind'] in ('map', 'set'):
                children = [param.get('key', {}), param.get('value', {})] if param['kind'] == 'map' else [param.get('element', {})]
                markers = param['type_path'].split(';')
                if len(markers) != len(children): raise ValueError('Invalid hash container type markers')
                for child, marker in zip(children, markers):
                    kind, path = child.get('kind'), child.get('type_path')
                    if kind not in scalar_properties or (marker not in ('EnumProperty', 'ByteProperty') if kind == 'enum' else marker != scalar_properties[kind]):
                        raise ValueError('Unsupported hash container member')
                    if kind == 'enum' and path not in enums: raise ValueError('Missing hash enum definition')
                    if kind != 'enum' and path != '': raise ValueError('Invalid hash member identity')
                container_name = hash_container_name(param)
                if container_name in hash_containers and hash_container_identity(hash_containers[container_name]) != hash_container_identity(param):
                    raise ValueError('Hash container generated symbol collision')
                hash_containers[container_name] = param
            if param['kind'] == 'array':
                element = param.get('element', {})
                property_types = {'int':'IntProperty', 'float':'FloatProperty', 'double':'DoubleProperty', 'bool':'BoolProperty', 'string':'StrProperty', 'name':'NameProperty', 'object':'ObjectProperty', 'struct':'StructProperty', 'enum':'EnumProperty'}
                kind = element.get('kind')
                if kind not in property_types or (param['type_path'] not in ('EnumProperty', 'ByteProperty') if kind == 'enum' else param['type_path'] != property_types[kind]):
                    raise ValueError('Unsupported or inconsistent array element')
                path = element.get('type_path')
                if kind == 'object' and path not in classes: raise ValueError('Missing object class for array element')
                if kind == 'struct' and path not in structures: raise ValueError('Missing struct definition for array element')
                if kind == 'enum' and path not in enums: raise ValueError('Missing enum definition for array element')
                if kind not in ('object', 'struct', 'enum') and path != '': raise ValueError('Unexpected type identity for array element')
            if type(param['input']) is not bool or type(param['output']) is not bool or not (param['input'] or param['output']):
                raise ValueError('Invalid parameter direction')
            name = param['path']
            if not name or '.' in name or ident(name) in names:
                raise ValueError('Invalid or colliding parameter')
            names.add(ident(name))
            for leaf in leaves(param, name):
                leaf.update(input=param['input'], output=param['output'])
                flattened.append(leaf)
        expected = [{key: p[key] for key in ('path', 'kind', 'type_path', 'input', 'output')} for p in fn['fields']]
        if flattened != expected:
            raise ValueError('Struct tree does not match validated leaf signature')

    defaults = {}
    for fn in ir['functions']:
        seen_default = False
        for param in fn['parameters']:
            if 'default_text' in param:
                try: defaults[(fn['alias'], param['path'])] = default_expr(param, enums)
                except ValueError as error:
                    raise ValueError(f"Unsupported default for {fn['alias']}.{param['path']}: {error}") from None
                seen_default = True
            elif param['input'] and seen_default:
                raise ValueError(f"Required input after default in {fn['alias']}: {param['path']}")

    symbols = {'UEName', 'ue_name'}
    def register(name):
        if name in symbols:
            raise ValueError('Generated symbol collision: ' + name)
        symbols.add(name)
    for path in classes:
        name = class_name(path)
        for suffix in ('', '_from', '_raw', '_null', '_valid'):
            register(name + suffix)
        for parent in ancestors(path): register(name + '_as_' + class_name(parent))
    for path in enums:
        name = enum_name(path)
        for symbol in (name, name + '_to_symbol', name + '_from_symbol'):
            register(symbol)
    for path in order: register(struct_name(path))
    for name, field in hash_containers.items():
        register(name)
        if field['kind'] == 'map': register(name + '_Entry')
    for fn in ir['functions']:
        register(fn['alias'])
        if sum(p['output'] for p in fn['parameters']) > 1:
            register(fn['alias'] + '_Result')

    def typ(field):
        if field['kind'] in ('map', 'set'): return hash_container_name(field)
        if field['kind'] == 'array': return 'array<' + typ(field['element']) + '>'
        return {'object': class_name(field['type_path']), 'name': 'UEName', 'enum': enum_name(field['type_path']), 'struct': struct_name(field['type_path'])}.get(field['kind'], field['kind'])

    lines = ['options gen2', 'module ue_generated public', 'require ue_foundation', '',
             '// Generated by Scripts/GenerateBindings.py. Do not edit.',
             'struct UEName { value : string }',
             'def ue_name(value : string) : UEName { return UEName(value = value) }', '']
    for path in sorted(classes):
        lines += [f'[ue_native_object(path={quote(path)})]', f'struct {class_name(path)} {{ handle : UObjectHandle }}', '']
    # Short names are aliases of existing wrappers, never a second object representation.
    names = {}
    for path in sorted(classes):
        name = ident(path.rsplit('.', 1)[-1])
        names.setdefault(name, []).append(path)
    for name, paths in sorted(names.items()):
        if len(paths) == 1 and name not in symbols:
            register(name)
            lines += [f'typedef {name} = {class_name(paths[0])}', '']
    for path in sorted(classes):
        name = class_name(path)
        lines += [
            f'def {name}_from(value : UObjectHandle) : {name} {{ return {name}(handle = ue_cast_nullable_object(value, {quote(path)})) }}',
            f'def {name}_raw(value : {name}) : UObjectHandle {{ return ue_cast_nullable_object(value.handle, {quote(path)}) }}',
            f'def {name}_null() : {name} {{ return {name}(handle = ue_null_object()) }}',
            f'def {name}_valid(value : {name}) : bool {{ return ue_object_is_a(value.handle, {quote(path)}) }}', '']
        for parent in ancestors(path):
            base = class_name(parent)
            lines += [f'def {name}_as_{base}(value : {name}) : {base} {{ return {base}_from({name}_raw(value)) }}', '']
    for path, members in sorted(enums.items()):
        name = enum_name(path)
        # daScript enum ordinals are private to the binding; conversion uses UE symbols, not numeric casts.
        lines += [f'enum {name} {{'] + ['    v_' + ident(m['name']) for m in members] + ['}', '']
        lines += [f'def {name}_to_symbol(value : {name}) : string {{']
        for m in members:
            lines += [f'    if (value == {name}.v_{ident(m["name"])}) {{ return {quote(m["name"])} }}']
        lines += ['    panic("invalid generated enum value")', '    return ""', '}', '']
        lines += [f'def {name}_from_symbol(value : string) : {name} {{']
        for m in members:
            lines += [f'    if (value == {quote(m["name"])}) {{ return {name}.v_{ident(m["name"])} }}']
        lines += ['    panic("unknown enum result; regenerate bindings")', f'    return {name}.v_{ident(members[0]["name"])}', '}', '']

    for path in order:
        lines += [f'struct {struct_name(path)} {{'] + [f'    f_{ident(p["path"])} : {typ(p)}' for p in structures[path]['fields']] + ['}', '']

    for name, field in sorted(hash_containers.items()):
        if field['kind'] == 'map':
            lines += [f'struct {name}_Entry {{ key : {typ(field["key"])}; value : {typ(field["value"])} }}',
                      f'struct {name} {{ entries : array<{name}_Entry> }}', '']
        else: lines += [f'struct {name} {{ values : array<{typ(field["element"])}> }}', '']

    def path_expr(path):
        # Replace the validated array wildcard after quoting, inserting only our loop variable.
        return quote(path).replace('.*', '.{index}')

    def write_input(field, path, val, indent='    '):
        if field['kind'] in ('map', 'set'):
            items = val + ('.entries' if field['kind'] == 'map' else '.values')
            lines.extend([f'{indent}ue_arg_hash(call, {quote(path)}, int(length({items})))', f'{indent}for (index in range(length({items}))) {{'])
            if field['kind'] == 'map':
                write_input(field['key'], path + '.*.key', items + '[index].key', indent + '    ')
                write_input(field['value'], path + '.*.value', items + '[index].value', indent + '    ')
            else: write_input(field['element'], path + '.*', items + '[index]', indent + '    ')
            lines.append(indent + '}')
            return
        if field['kind'] == 'array':
            lines.extend([f'{indent}ue_arg_array(call, {path_expr(path)}, int(length({val})))',
                          f'{indent}for (index in range(length({val}))) {{'])
            write_input(field['element'], path + '.*', val + '[index]', indent + '    ')
            lines.append(indent + '}')
            return
        if field['kind'] == 'struct':
            for child in structures[field['type_path']]['fields']:
                write_input(child, path + '.' + child['path'], val + '.f_' + ident(child['path']), indent)
            return
        if field['kind'] == 'object': val = class_name(field['type_path']) + '_raw(' + val + ')'
        if field['kind'] == 'name': val += '.value'
        if field['kind'] == 'enum': val = quote(field['type_path']) + ', ' + enum_name(field['type_path']) + '_to_symbol(' + val + ')'
        lines.append(f'{indent}ue_arg_{field["kind"]}(call, {path_expr(path)}, {val})')

    def read_output(field, path):
        if field['kind'] == 'struct':
            return struct_name(field['type_path']) + '(' + ', '.join('f_' + ident(p['path']) + ' = ' + read_output(p, path + '.' + p['path']) for p in structures[field['type_path']]['fields']) + ')'
        expr = f'ue_result_{field["kind"]}(call, {path_expr(path)}' + (', ' + quote(field['type_path']) if field['kind'] == 'enum' else '') + ')'
        if field['kind'] == 'object': expr = class_name(field['type_path']) + '_from(' + expr + ')'
        if field['kind'] == 'name': expr = 'UEName(value = ' + expr + ')'
        if field['kind'] == 'enum': expr = enum_name(field['type_path']) + '_from_symbol(' + expr + ')'
        return expr

    for fn in sorted(ir['functions'], key=lambda item: item['alias']):
        alias, params_ir = fn['alias'], fn['parameters']
        inputs = [p for p in params_ir if p['input']]
        outputs = [p for p in params_ir if p['output']]
        result_type = alias + '_Result' if len(outputs) > 1 else typ(outputs[0]) if outputs else ''
        if len(outputs) > 1:
            lines += [f'struct {result_type} {{'] + [f'    out_{ident(p["path"])} : {typ(p)}' for p in outputs] + ['}', '']
        params = ['target : ' + class_name(fn['class'])] + [f'arg_{ident(p["path"])} : {typ(p)}' + (' = ' + defaults[(alias, p['path'])] if (alias, p['path']) in defaults else '') for p in inputs]
        lines += [f'def {alias}(' + '; '.join(params) + ')' + (f' : {result_type}' if result_type else '') + ' {',
                  f'    let receiver = ue_cast_object({class_name(fn["class"])}_raw(target), {quote(fn["class"])})',
                  f'    let call = ue_begin_call(receiver, {quote(fn["function"])})',
                  f'    ue_expect_signature(call, {quote(fn["signature"])})']
        for p in inputs: write_input(p, p['path'], 'arg_' + ident(p['path']))
        lines += ['    ue_invoke_call(call)']
        has_arrays = any(p['kind'] in ('array', 'map', 'set') for p in outputs)
        values = [read_output(p, p['path']) for p in outputs] if not has_arrays else []
        if has_arrays:
            lines += [f'    var result : {result_type}']
            for p in outputs:
                target = 'result' + ('.out_' + ident(p['path']) if len(outputs) > 1 else '')
                if p['kind'] in ('map', 'set'):
                    items = target + ('.entries' if p['kind'] == 'map' else '.values')
                    if p['kind'] == 'map':
                        expr = typ(p) + '_Entry(key = ' + read_output(p['key'], p['path'] + '.*.key') + ', value = ' + read_output(p['value'], p['path'] + '.*.value') + ')'
                    else: expr = read_output(p['element'], p['path'] + '.*')
                    lines += [f'    resize({items}, ue_result_hash_size(call, {quote(p["path"])}))',
                              f'    for (index in range(length({items}))) {{', f'        {items}[index] = {expr}', '    }']
                    continue
                if p['kind'] != 'array':
                    lines.append(f"    {target} = {read_output(p, p['path'])}")
                    continue
                expr = read_output(p['element'], p['path'] + '.*')
                lines += [f'    resize({target}, ue_result_array_size(call, {quote(p["path"])}))',
                          f'    for (index in range(length({target}))) {{', f'        {target}[index] = {expr}', '    }']
        elif len(outputs) > 1:
            lines += ['    let result = ' + result_type + '(' + ', '.join('out_' + ident(p['path']) + ' = ' + v for p, v in zip(outputs, values)) + ')']
        elif outputs:
            lines += ['    let result = ' + values[0]]
        lines += ['    ue_release_call(call)']
        if outputs: lines += ['    return <- result' if has_arrays else '    return result']
        lines += ['}', '']
    return '\n'.join(lines)


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('ir', type=Path)
    parser.add_argument('output', type=Path)
    parser.add_argument('--check', action='store_true', help='Fail if the checked-in module differs')
    args = parser.parse_args()
    text = generate(json.loads(args.ir.read_text(encoding='utf-8-sig')))
    if args.check:
        if not args.output.exists() or args.output.read_text(encoding='utf-8') != text:
            parser.exit(1, 'Generated bindings are stale\n')
    else:
        args.output.write_text(text, encoding='utf-8', newline='\n')


if __name__ == '__main__':
    main()
