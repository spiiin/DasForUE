"""Generate deterministic daScript wrappers from DasExport schema v5 (stdlib only)."""
import argparse
import hashlib
import json
import math
import struct
from decimal import Decimal
import re
from pathlib import Path

from DasKinds import property_classes


ROOT = Path(__file__).resolve().parents[1]
# Native modules of the plugin (DasNativeBinds.h); projects add their own with --native-binds-source.
NATIVE_BINDS_SOURCE = ROOT / 'Plugins/DasForUE/Source'
# Script modules of the plugin: ue_dsl declares the DSL base classes ([ue_native_base] class Actor {}).
PLUGIN_SCRIPTS = ROOT / 'Plugins/DasForUE/Content/Das/Scripts'
_native_type_names_cache = {}
_dsl_base_names_cache = {}


def native_type_names(dirs):
    """Script type names that native modules declare: Class<T>(TEXT("Name")) in files with DAS_NATIVE_BINDS.

    A generated short alias with such a name would make the name ambiguous in a script that requires both
    modules, so generate() emits none (the full C_<Leaf>_<hash> name stays). Comments are ignored.
    """
    key = tuple(sorted(str(Path(d).resolve()) for d in dirs))
    if key not in _native_type_names_cache:
        names = set()
        for root in key:
            for path in sorted(Path(root).rglob('*')):
                if path.suffix not in ('.cpp', '.h', '.inl') or not path.is_file():
                    continue
                text = re.sub(r'//[^\n]*', '', path.read_text(encoding='utf-8', errors='replace'))
                if 'DAS_NATIVE_BINDS(' not in text:
                    continue
                names.update(re.findall(r'\bClass<[^;()]*?>\(\s*TEXT\("([A-Za-z_][A-Za-z0-9_]*)"\)', text))
        _native_type_names_cache[key] = frozenset(names)
    return _native_type_names_cache[key]


def dsl_base_names(folder):
    """DSL base classes of the script modules in folder: name -> module (`[ue_native_base(...)]` + `class Name`).

    A generated short alias with such a name would be ambiguous next to the base class in a script that requires
    both modules (`var a : Actor`), so generate() emits none. Only modules count, not test scripts.
    """
    key = str(Path(folder).resolve())
    if key not in _dsl_base_names_cache:
        names = {}
        for path in sorted(Path(key).glob('*.das')):
            text = path.read_text(encoding='utf-8', errors='replace')
            module = re.search(r'^module ([A-Za-z_][A-Za-z0-9_]*)', text, re.M)
            if not module:
                continue
            for name in re.findall(r'^\[ue_native_base\(path="[^"]*"\)\]\s*\nclass ([A-Za-z_][A-Za-z0-9_]*)', text, re.M):
                names.setdefault(name, module.group(1))
        _dsl_base_names_cache[key] = names
    return _dsl_base_names_cache[key]


def skipped_aliases(text):
    """Short aliases generate() left out for reserved names, from its comments."""
    return re.findall(r'^// ([A-Za-z_][A-Za-z0-9_]*): no short alias,', text, re.M)


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
    if kind == 'class' and text in ('None', 'nullptr', 'NULL'): return 'ue_class_null()'
    if kind == 'object' and text in ('None', 'nullptr', 'NULL'):
        return class_name(field['type_path']) + '_null()'
    raise ValueError('unsupported default kind or representation: ' + kind)


# Identity mappings are explicit: lookalike structs must never become native math.
HIT_RESULT_PATH = '/Script/Engine.HitResult'
HIT_RESULT_TRANSPORT = 'hit_result_v1'

def is_hit_result(value):
    return value.get('kind') == 'struct' and value.get('type_path') == HIT_RESULT_PATH

CONSTRAINT_PATH = '/Script/Engine.ConstraintInstanceAccessor'
INPUT_ACTION_VALUE_PATH = '/Script/EnhancedInput.InputActionValue'
OPAQUE_TRANSPORTS = {
    '/Script/UMG.PaintContext': ('scoped_paint_context_v1','UEPaintContext','__ue_arg_paint_context','__ue_result_paint_context'),
    '/Script/UMG.WidgetAnimationHandle': ('weak_widget_animation_v1','UEWidgetAnimationHandle','__ue_arg_widget_animation','__ue_result_widget_animation'),
    '/Script/UMG.EventReply': ('scoped_event_reply_v1','UEEventReply','__ue_arg_event_reply','__ue_result_event_reply'),
    '/Script/SlateCore.Geometry': ('slate_geometry_snapshot_v1','UEGeometry','__ue_arg_geometry','__ue_result_geometry'),
    INPUT_ACTION_VALUE_PATH: ('input_action_value_v1','InputActionValue','__ue_arg_input_value','__ue_result_input_value'),
    HIT_RESULT_PATH: (HIT_RESULT_TRANSPORT,'UEHitResult','__ue_arg_hit_result','__ue_result_hit_result'),
    '/Script/SlateCore.InputEvent': ('slate_input_event_snapshot_v1','UEInputEvent','__ue_arg_inputevent','__ue_result_inputevent'),
    '/Script/SlateCore.KeyEvent': ('slate_input_event_snapshot_v1','UEKeyEvent','__ue_arg_keyevent','__ue_result_keyevent'),
    '/Script/SlateCore.AnalogInputEvent': ('slate_input_event_snapshot_v1','UEAnalogInputEvent','__ue_arg_analoginputevent','__ue_result_analoginputevent'),
    '/Script/SlateCore.CharacterEvent': ('slate_input_event_snapshot_v1','UECharacterEvent','__ue_arg_characterevent','__ue_result_characterevent'),
    '/Script/SlateCore.PointerEvent': ('slate_input_event_snapshot_v1','UEPointerEvent','__ue_arg_pointerevent','__ue_result_pointerevent'),
    '/Script/SlateCore.NavigationEvent': ('slate_input_event_snapshot_v1','UENavigationEvent','__ue_arg_navigationevent','__ue_result_navigationevent'),
    CONSTRAINT_PATH: ('constraint_component_v1','UEConstraintHandle','__ue_arg_constraint','__ue_result_constraint'),
}
def soft_brush_adapter(fn):
    expected={'SetBrushFromSoftTexture':('softtexture','/Script/Engine.Texture2D'), 'SetBrushFromSoftMaterial':('softmaterial','/Script/Engine.MaterialInterface')}.get(fn.get('function'))
    if not expected or fn.get('class')!='/Script/UMG.Image' or fn.get('static',False) or fn.get('call_adapter')!='managed_soft_brush_v1': return False
    values=fn.get('parameters',[]); fields=fn.get('fields',[])
    def valid(items):
        if len(items)!=(2 if expected[0]=='softtexture' else 1):return False
        soft=[x for x in items if x.get('kind')=='soft_object']
        return len(soft)==1 and soft[0].get('path')==expected[0] and soft[0].get('type_path')==expected[1] and soft[0].get('transport')=='managed_soft_brush_v1' and all(x.get('input') and not x.get('output') and (x in soft or (x.get('path')=='bmatchsize' and x.get('kind')=='bool' and x.get('type_path','')=='')) for x in items)
    return valid(values) and valid(fields)

def is_opaque_native(value):
    return value.get('kind')=='struct' and value.get('type_path') in OPAQUE_TRANSPORTS

CANONICAL_MATH = {
    '/Script/CoreUObject.Quat': ('UEQuat', 'quat', {'x':'double','y':'double','z':'double','w':'double'}),
    '/Script/CoreUObject.LinearColor': ('UELinearColor', 'linear_color', {'r':'float','g':'float','b':'float','a':'float'}),
    '/Script/CoreUObject.Color': ('UEColor', 'color', {'r':'byte','g':'byte','b':'byte','a':'byte'}),
    '/Script/CoreUObject.Vector2D': ('UEVector2D', 'vector2d', {'x':'double', 'y':'double'}),
    '/Script/CoreUObject.Vector': ('UEVector', 'vector', {'x':'double', 'y':'double', 'z':'double'}),
    '/Script/CoreUObject.Rotator': ('UERotator', 'rotator', {'pitch':'double', 'yaw':'double', 'roll':'double'}),
    '/Script/CoreUObject.Transform': ('UETransform', 'transform', {'rotation':'/Script/CoreUObject.Quat', 'translation':'/Script/CoreUObject.Vector', 'scale3d':'/Script/CoreUObject.Vector'}),
}


def generate(ir, module="ue_generated", source_types=None, canonical_math=False, native_names=None,
             dsl_bases=None):
    if not re.fullmatch(r"[A-Za-z_][A-Za-z0-9_]*", module): raise ValueError("Invalid module name")
    if ir.get('schema_version') != 5:
        raise ValueError('Unsupported IR schema version')
    def validate_weak_marker(value, depth=0):
        if depth>16: raise ValueError('Weak type descriptor nesting limit')
        if value.get('kind')=='soft_object':
            if depth or value.get('transport')!='managed_soft_brush_v1' or not value.get('input') or value.get('output'):raise ValueError('Soft brush requires attested scalar input')
            return
        if value.get('transport')=='weak_widget_animation_v1' and (depth or value.get('input') or not value.get('output') or value.get('path','').lower()!='returnvalue'): raise ValueError('WidgetAnimation transport requires scalar producer result')
        if value.get('transport')=='scoped_paint_context_v1' and (depth or value.get('path','').lower()!='context'): raise ValueError('PaintContext transport requires scalar context parameter')
        if depth and value.get('transport')=='scoped_event_reply_v1': raise ValueError('EventReply transport requires scalar parameters')
        if depth and value.get('transport')=='slate_geometry_snapshot_v1': raise ValueError('Geometry transport requires scalar parameters')
        if depth and value.get('transport')=='slate_input_event_snapshot_v1': raise ValueError('Input event transport requires scalar parameters')
        transport=value.get('transport')
        expected=OPAQUE_TRANSPORTS.get(value.get('type_path')) if value.get('kind')=='struct' else None
        if (transport is not None and (not expected or transport!=expected[0])) or (expected and transport!=expected[0]):
            raise ValueError('Invalid or missing native HitResult transport')
        if 'weak_reference' in value and (type(value['weak_reference']) is not bool or value.get('kind')!='object'):
            raise ValueError('Invalid weak reference marker')
        for key in ('element','key','value'):
            if key in value: validate_weak_marker(value[key],depth+1)
    def navigation_adapter(fn):
        events={'Key_GetNavigationActionFromKey':('inkeyevent','/Script/SlateCore.KeyEvent'), 'Key_GetNavigationDirectionFromKey':('inkeyevent','/Script/SlateCore.KeyEvent'), 'Key_GetNavigationDirectionFromAnalog':('inanalogevent','/Script/SlateCore.AnalogInputEvent')}
        expected=events.get(fn.get('function'))
        return fn.get('call_adapter')=='live_input_navigation_v1' and fn.get('class')=='/Script/Engine.KismetInputLibrary' and expected and all(v.get('path','').lower()==expected[0] and v.get('type_path')==expected[1] and v.get('input') and not v.get('output') for v in [*fn['parameters'],*fn['fields']] if v.get('transport')=='slate_input_event_snapshot_v1')
    for fn in ir['functions']:
        if (fn.get('call_adapter')=='managed_soft_brush_v1' or any(v.get('kind')=='soft_object' for v in [*fn['parameters'],*fn['fields']])) and not soft_brush_adapter(fn):raise ValueError('Soft brush requires exact attested Image function')
        if any(v.get('transport')=='scoped_paint_context_v1' for v in [*fn['parameters'],*fn['fields']]) and fn.get('call_adapter')!='scoped_paint_context_v1': raise ValueError('PaintContext requires attested synchronous function adapter')
        if any(v.get('transport')=='weak_widget_animation_v1' for v in [*fn['parameters'],*fn['fields']]) and fn.get('call_adapter')!='weak_widget_animation_v1': raise ValueError('WidgetAnimation requires attested producer function adapter')
        if any(v.get('transport')=='scoped_event_reply_v1' for v in [*fn['parameters'],*fn['fields']]) and fn.get('call_adapter')!='scoped_event_reply_v1': raise ValueError('EventReply requires attested synchronous function adapter')
        if any(v.get('transport')=='slate_geometry_snapshot_v1' for v in [*fn['parameters'],*fn['fields']]) and fn.get('call_adapter')!='slate_geometry_snapshot_v1': raise ValueError('Geometry requires attested snapshot function adapter')
        if any(v.get('transport')=='slate_input_event_snapshot_v1' for v in [*fn['parameters'],*fn['fields']]) and fn.get('call_adapter')!='slate_input_event_snapshot_v1' and not navigation_adapter(fn) and not (fn.get('call_adapter')=='scoped_event_reply_v1' and fn.get('class')=='/Script/UMG.WidgetBlueprintLibrary' and fn.get('function')=='DetectDragIfPressed' and all(v.get('type_path')=='/Script/SlateCore.PointerEvent' and v.get('path','').lower()=='pointerevent' and v.get('input') and not v.get('output') for v in [*fn['parameters'],*fn['fields']] if v.get('transport')=='slate_input_event_snapshot_v1')): raise ValueError('Input event requires attested snapshot function adapter')
        for value in [*fn['parameters'],*fn['fields']]: validate_weak_marker(value)
    for definition in ir['structs']:
        for value in definition['fields']: validate_weak_marker(value)
    for fn in ir['functions']:
        for leaf in fn['fields']:
            if leaf.get('type_path')!=CONSTRAINT_PATH:continue
            getter=fn['class']=='/Script/Engine.PhysicsConstraintComponent' and fn['function']=='GetConstraint' and leaf['path']=='returnvalue' and leaf['output'] and not leaf['input'] and not fn.get('static',False)
            consumer=fn['class']=='/Script/Engine.ConstraintInstanceBlueprintLibrary' and fn.get('static',False) and leaf['input'] and '.' not in leaf['path']
            if not (getter or consumer):raise ValueError('Constraint transport requires scalar component getter/library contract')
    classes = {}
    interfaces = set()
    interface_ids = {}
    for item in ir['classes']:
        path = item['type_path']
        if not path.startswith('/') or path in classes:
            raise ValueError('Invalid or duplicate class identity')
        classes[path] = item['parent']
        if type(item.get('interface', False)) is not bool: raise ValueError('Invalid interface class marker')
        if item.get('interface', False):
            stable = item.get('interface_id', '')
            if path.startswith('/Game/'):
                if not isinstance(stable, str) or not re.fullmatch(r'[A-Za-z_][A-Za-z0-9_.-]*', stable): raise ValueError('Generated authored interface requires stable identity')
                interface_ids[path] = stable
            elif not path.startswith('/Script/') or stable: raise ValueError('Invalid generated interface identity')
            interfaces.add(path)
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
    kinds = {'soft_object', 'delegate', 'byte', 'text', 'int', 'float', 'double', 'bool', 'string', 'object', 'class', 'name', 'enum', 'array', 'map', 'set', 'struct'}
    for fn in ir['functions']:
        alias = fn['alias']
        if not re.fullmatch(r'[A-Za-z_][A-Za-z0-9_]*', alias) or alias in aliases or not alias.startswith(('actor_', 'fixture_', 'ue_')):
            raise ValueError('Invalid, reserved or duplicate alias: ' + alias)
        aliases.add(alias)
        for key in ('class', 'function', 'signature'):
            if not isinstance(fn[key], str) or not fn[key]:
                raise ValueError('Missing ' + key)
        if type(fn.get('static', False)) is not bool: raise ValueError('static marker must be boolean')
        world_context = fn.get('world_context', '')
        if not isinstance(world_context, str) or (world_context and not re.fullmatch(r'[A-Za-z_][A-Za-z0-9_]*', world_context)): raise ValueError('Invalid WorldContext name')
        if world_context and not fn.get('static', False): raise ValueError('WorldContext requires static function')
        if fn.get('static', False):
            if not fn['class'].startswith('/Script/'): raise ValueError('Static call requires native /Script/ class')
        if world_context:
            contexts = [p for p in fn['parameters'] if p['path'] == world_context]
            if len(contexts) != 1 or contexts[0]['kind'] != 'object' or not contexts[0]['input'] or contexts[0]['output']: raise ValueError('WorldContext must name one input object parameter')
        names = set()
        for field in fn['fields']:
            name = ident(field['path'])
            if name in names or field['kind'] not in kinds:
                raise ValueError('Unsupported or colliding field: ' + field['path'])
            names.add(name)
            if field['kind']=='delegate' and (not field['input'] or field['output'] or '.' in field['path'] or not field['type_path'].startswith('/Script/')): raise ValueError('Native delegates require scalar input-only signatures')
            if field['kind']=='struct' and not is_opaque_native(field): raise ValueError('Unsupported opaque call struct')
            if field['kind'] == 'class' and field['type_path'] not in classes: raise ValueError('Missing class MetaClass identity')
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

    hash_containers = {}
    scalar_properties = property_classes()
    def register_hash(field):
        if (field.get('key',{}) if field['kind']=='map' else field.get('element',{})).get('kind') == 'text': raise ValueError('FText hash keys/elements are unsupported')
        children = [field.get('key',{}),field.get('value',{})] if field['kind']=='map' else [field.get('element',{})]
        markers=field['type_path'].split(';')
        if len(markers)!=len(children): raise ValueError('Invalid hash container type markers')
        for child,marker in zip(children,markers):
            kind,path=child.get('kind'),child.get('type_path')
            if kind not in scalar_properties or (marker not in ('EnumProperty','ByteProperty') if kind=='enum' else marker!=scalar_properties[kind]): raise ValueError('Unsupported hash container member')
            if kind=='enum' and path not in enums: raise ValueError('Missing hash enum definition')
            if kind!='enum' and path!='': raise ValueError('Invalid hash member identity')
        name=hash_container_name(field)
        if name in hash_containers and hash_container_identity(hash_containers[name])!=hash_container_identity(field): raise ValueError('Hash container generated symbol collision')
        hash_containers[name]=field
    def validate_array_storage(field, message):
        element = field.get('element', {})
        storage = dict(property_classes('class'), object='ObjectProperty', struct='StructProperty')
        kind = element.get('kind')
        expected = ('EnumProperty', 'ByteProperty') if kind == 'enum' else (storage.get(kind),)
        if kind not in storage or field.get('type_path') not in expected or element.get('weak_reference', False):
            raise ValueError(message)
        return element

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
        definition=structures[path]
        if path in OPAQUE_TRANSPORTS:
            if definition.get('transport')!=OPAQUE_TRANSPORTS[path][0] or definition['fields']: raise ValueError('HitResult must use opaque native transport')
            visited.add(path);order.append(path);return
        if 'transport' in definition: raise ValueError('Unknown native struct transport')
        visiting.add(path)
        seen = set()
        if not structures[path]['fields']: raise ValueError('Opaque/empty structs need an adapter')
        for field in structures[path]['fields']:
            name = field['path']
            if not name or '.' in name or ident(name) in seen:
                raise ValueError('Invalid or colliding struct field')
            seen.add(ident(name))
            def validate_member(member):
                if member['kind'] == 'struct': visit(member['type_path'])
                elif member['kind'] == 'array':
                    element = validate_array_storage(member, 'Unsupported struct array member')
                    validate_member(element)
                elif member['kind'] in ('map','set'): register_hash(member)
                elif member['kind'] in ('object', 'class'):
                    if member['type_path'] not in classes: raise ValueError('Missing or incompatible struct object class')
                elif member['kind'] not in kinds: raise ValueError('Unsupported struct leaf')
                if member['kind'] == 'enum' and member['type_path'] not in enums: raise ValueError('Missing struct enum definition')
            validate_member(field)
        visiting.remove(path)
        visited.add(path)
        order.append(path)
    for path in sorted(structures): visit(path)

    def leaves(field, path):
        if is_opaque_native(field):
            if field['type_path'] not in structures: raise ValueError('Missing HitResult definition')
            yield dict(path=path,kind='struct',type_path=field['type_path'])
            return
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
    for fn in ir['functions']:
        flattened, names = [], set()
        for param in fn['parameters']:
            if param['kind'] in ('map', 'set'): register_hash(param)
            if param['kind'] == 'array':
                element = validate_array_storage(param, 'Unsupported or inconsistent array element')
                kind = element.get('kind')
                path = element.get('type_path')
                if kind in ('object', 'class') and path not in classes: raise ValueError('Missing object class for array element')
                if kind == 'struct' and path not in structures: raise ValueError('Missing struct definition for array element')
                if kind == 'enum' and path not in enums: raise ValueError('Missing enum definition for array element')
                if kind not in ('object', 'class', 'struct', 'enum') and path != '': raise ValueError('Unexpected type identity for array element')
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

    math_types = CANONICAL_MATH if canonical_math else {}
    for path, (_, _, layout) in math_types.items():
        if path not in structures: continue
        fields = structures[path]['fields']
        actual = {f['path']: f['type_path'] if f['kind']=='struct' else f['kind'] for f in fields}
        if actual != layout or any(f.get('type_path', '') for f in fields if f['kind']!='struct'):
            raise ValueError('Canonical math layout changed: ' + path)

    source_mappings = {}
    source_module = None
    if source_types is not None:
        if not isinstance(source_types, dict) or set(source_types) != {'module', 'registry', 'types'}: raise ValueError('Invalid source type manifest')
        source_module = source_types['module']
        if not isinstance(source_module, str) or not re.fullmatch(r'[A-Za-z_][A-Za-z0-9_]*', source_module) or source_module == module: raise ValueError('Invalid source type module')
        if not isinstance(source_types['registry'], str) or not source_types['registry'].startswith('/Game/'): raise ValueError('Explicit source registry required')
        if not isinstance(source_types['types'], list): raise ValueError('Invalid source type list')
        for mapping in source_types['types']:
            if not isinstance(mapping, dict) or set(mapping) != {'path', 'kind', 'name', 'id', 'members'}: raise ValueError('Invalid source type mapping')
            path, kind = mapping['path'], mapping['kind']
            if not isinstance(path, str) or path in source_mappings or not path.startswith('/Game/'): raise ValueError('Duplicate or nonauthored source identity')
            if not isinstance(mapping['name'], str) or not isinstance(mapping['id'], str) or not isinstance(mapping['members'], dict) or not re.fullmatch(r'[A-Za-z_][A-Za-z0-9_]*', mapping['name']) or not re.fullmatch(r'[A-Za-z_][A-Za-z0-9_.-]*', mapping['id']): raise ValueError('Invalid source declaration identity')
            if kind == 'enum':
                expected = {member['name']: int(member['value']) for member in enums.get(path, [])}
                if not expected or mapping['members'] != expected: raise ValueError('Source enum values differ from exported identity')
            elif kind == 'struct':
                definition = structures.get(path)
                if not definition or set(mapping['members']) != {field['path'] for field in definition['fields']}: raise ValueError('Source struct members differ from exported layout')

                members = list(mapping['members'].values())
                if len(set(members)) != len(members) or any(not re.fullmatch(r'[A-Za-z_][A-Za-z0-9_]*', member) for member in members): raise ValueError('Invalid source struct member names')
            else: raise ValueError('Invalid source value kind')
            source_mappings[path] = mapping
    def check_source_field(field, depth=0):
        if depth > 16: raise ValueError('Source mapping nesting limit exceeded')
        kind = field['kind']
        if kind in ('byte', 'text', 'int', 'float', 'double', 'bool', 'string', 'name', 'object', 'class'): return
        if is_opaque_native(field) or (kind == 'struct' and field['type_path'] in math_types): return
        if kind in ('struct', 'enum'):
            mapped = source_mappings.get(field['type_path'])
            if not mapped or mapped['kind'] != kind: raise ValueError('Nested source identity requires an explicit matching mapping')
            if kind == 'struct':
                for child in structures[field['type_path']]['fields']: check_source_field(child, depth + 1)
            return
        if kind in ('map','set'):
            key=field['key'] if kind=='map' else field['element']
            if key['kind'] not in ('byte','int','string'): raise ValueError('Source hash keys/elements require int/string')
            if kind=='map' and field['value']['kind'] not in ('byte','text','int','float','double','bool','string','name'): raise ValueError('Unsupported source hash value')
            return
        if kind == 'array': check_source_field(field['element'], depth + 1); return
        raise ValueError('Unsupported source mapping field kind: ' + kind)
    for path, mapping in source_mappings.items():
        if mapping['kind'] == 'struct':
            for field in structures[path]['fields']: check_source_field(field)
    def owns_container(field):
        if field['kind'] in ('array', 'map', 'set'): return True
        if is_opaque_native(field): return False
        if field['kind'] == 'struct': return any(owns_container(child) for child in structures[field['type_path']]['fields'])
        return False
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

    symbols = {'UEName', 'ue_name', 'UEHitResult', 'UEConstraintHandle', 'InputActionValue', 'InputValueType', 'UEFunctionDelegate', 'ue_function_delegate', 'ue_function_delegate_null'} | {t[0] for t in math_types.values()}
    def register(name):
        if name in symbols:
            raise ValueError('Generated symbol collision: ' + name)
        symbols.add(name)
    for path in classes:
        name = class_name(path)
        for suffix in ('', '_from', '_raw', '_null', '_valid'):
            register(name + suffix)
        if path not in interfaces:
            for parent in ancestors(path): register(name + '_as_' + class_name(parent))
    for path in enums:
        name = enum_name(path)
        for symbol in (name, name + '_to_symbol', name + '_from_symbol'):
            register(symbol)
    for path in order:
        if path not in math_types and path not in OPAQUE_TRANSPORTS: register(struct_name(path))
    for name, field in hash_containers.items():
        register(name)
        if field['kind'] == 'map': register(name + '_Entry')
    for fn in ir['functions']:
        register(fn['alias'])
        if sum(p['output'] for p in fn['parameters']) > 1:
            register(fn['alias'] + '_Result')

    def typ(field):
        if field['kind']=='soft_object':return '$UESoftObject<'+class_name(field['type_path'])+'>'
        if is_opaque_native(field): return OPAQUE_TRANSPORTS[field['type_path']][1]
        if field['kind'] == 'struct' and field['type_path'] in math_types: return math_types[field['type_path']][0]
        if field['kind'] in ('map', 'set'): return hash_container_name(field)
        if field['kind'] == 'array': return 'array<' + typ(field['element']) + '>'
        if field['kind'] == 'delegate': return 'UEFunctionDelegate'
        return {'byte':'uint8', 'text':'UETextValue', 'class': 'UEClassHandle', 'object': class_name(field['type_path']), 'name': 'UEName', 'enum': enum_name(field['type_path']), 'struct': struct_name(field['type_path'])}.get(field['kind'], field['kind'])

    lines = ['options gen2', 'module ' + module + ' public', 'require ue_foundation' + (' public' if canonical_math else ''), '',
             '// Generated by Scripts/GenerateBindings.py. Do not edit.',
             'struct UEName { value : string }',
             'def ue_name(value : string) : UEName { return UEName(value = value) }', '']
    if '/Script/UMG.WidgetAnimationHandle' in structures: lines += ['require ue_widget_animation public', '']
    if '/Script/UMG.PaintContext' in structures: lines += ['require ue_paint public', '']
    if '/Script/UMG.EventReply' in structures: lines += ['require ue_widget_events public', '']
    if '/Script/SlateCore.Geometry' in structures: lines += ['require ue_geometry public', '']
    if any(p.startswith('/Script/SlateCore.') and p!='/Script/SlateCore.Geometry' and p in OPAQUE_TRANSPORTS for p in structures): lines += ['require ue_input_events public', '']
    if INPUT_ACTION_VALUE_PATH in structures: lines += ['require ue_input_value public', '']
    if any(f['kind']=='soft_object' for fn in ir['functions'] for f in fn['fields']): lines += ['require ue_image_soft_brush public', '']
    if any(f['kind']=='delegate' for fn in ir['functions'] for f in fn['fields']): lines += ['require ue_native_delegate public', '']
    if source_module: lines += ['require ' + source_module + ' public', '']
    for path in sorted(classes):
        annotation = 'ue_native_interface' if path in interfaces else 'ue_native_object'
        args = 'path=' + quote(path)
        if path in interface_ids: annotation = 'ue_interface_ref'; args += ', id=' + quote(interface_ids[path])
        lines += [f'[{annotation}({args})]', f'struct {class_name(path)} {{ handle : UObjectHandle }}', '']
    # Short names are aliases of existing wrappers, never a second object representation. None for names of native
    # bind types (native_type_names) and DSL base classes (dsl_base_names: name -> module): such a name would be
    # ambiguous in a script that requires both.
    if native_names is None:
        native_names = native_type_names([NATIVE_BINDS_SOURCE])
    if dsl_bases is None:
        dsl_bases = dsl_base_names(PLUGIN_SCRIPTS)
    names = {}
    short_aliases = set()
    for path in sorted(classes):
        leaf = path.rsplit('.', 1)[-1]
        name = ident(leaf[:-2] if path in interface_ids and leaf.endswith('_C') else leaf)
        names.setdefault(name, []).append(path)
    for name, paths in sorted(names.items()):
        if len(paths) == 1 and name not in symbols:
            if name in native_names:
                lines += [f'// {name}: no short alias, the name belongs to the native bind type {name} '
                          f'(DasNativeBinds.h); use {class_name(paths[0])}', '']
                continue
            if name in dsl_bases:
                lines += [f'// {name}: no short alias, the name belongs to the DSL base class {dsl_bases[name]}::{name}; '
                          f'use {class_name(paths[0])}', '']
                continue
            register(name)
            short_aliases.add(name)
            lines += [f'typedef {name} = {class_name(paths[0])}', '']
    for path in sorted(classes):
        name = class_name(path)
        if path in interfaces:
            lines += [
                f'def {name}_from(value : UObjectHandle) : {name} {{ return {name}(handle = ue_cast_interface(value, {quote(path)})) }}',
                f'def {name}_raw(value : {name}) : UObjectHandle {{ return ue_cast_interface(value.handle, {quote(path)}) }}',
                f'def {name}_null() : {name} {{ return {name}(handle = ue_null_object()) }}',
                f'def {name}_valid(value : {name}) : bool {{ return ue_implements_interface(value.handle, {quote(path)}) }}', '']
            continue
        lines += [
            f'def {name}_from(value : UObjectHandle) : {name} {{ return {name}(handle = ue_cast_nullable_object(value, {quote(path)})) }}',
            f'def {name}_raw(value : {name}) : UObjectHandle {{ return ue_cast_nullable_object(value.handle, {quote(path)}) }}',
            f'def {name}_null() : {name} {{ return {name}(handle = ue_null_object()) }}',
            f'def {name}_valid(value : {name}) : bool {{ return ue_object_is_a(value.handle, {quote(path)}) }}', '']
        for parent in ancestors(path):
            base = class_name(parent)
            lines += [f'def {name}_as_{base}(value : {name}) : {base} {{ return {base}_from({name}_raw(value)) }}', '']
    for path in sorted(interfaces):
        name = class_name(path)
        lines += [f'def ue_interface(requested : type<{name}>; value : UObjectHandle) : {name} {{ return {name}_from(value) }}', '']
    # Friendly interface helpers are emitted only as one collision-free group.
    for short, paths in sorted(names.items()):
        if len(paths) != 1 or paths[0] not in interfaces or short not in short_aliases: continue
        helper_names = [short + suffix for suffix in ('_from', '_raw', '_null', '_valid')]
        if any(helper in symbols for helper in helper_names): continue
        for helper in helper_names: register(helper)
        name = class_name(paths[0])
        lines += [f'def {short}_from(value : UObjectHandle) : {short} {{ return {name}_from(value) }}',
                  f'def {short}_raw(value : {short}) : UObjectHandle {{ return {name}_raw(value) }}',
                  f'def {short}_null() : {short} {{ return {name}_null() }}',
                  f'def {short}_valid(value : {short}) : bool {{ return {name}_valid(value) }}', '']
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

    nested_hashes={hash_container_name(f) for s in structures.values() for f in s['fields'] if f['kind'] in ('map','set')}
    for name in sorted(nested_hashes):
        field=hash_containers[name]
        if field['kind']=='map': lines += [f'struct {name}_Entry {{ key : {typ(field["key"])}; value : {typ(field["value"])} }}',f'struct {name} {{ entries : array<{name}_Entry> }}','']
        else: lines += [f'struct {name} {{ values : array<{typ(field["element"])}> }}','']
    for path in order:
        if path in math_types or path in OPAQUE_TRANSPORTS: continue
        lines += [f'struct {struct_name(path)} {{'] + [f'    f_{ident(p["path"])} : {typ(p)}' for p in structures[path]['fields']] + ['}', '']

    for name, field in sorted(hash_containers.items()):
        if name in nested_hashes: continue
        if field['kind'] == 'map':
            lines += [f'struct {name}_Entry {{ key : {typ(field["key"])}; value : {typ(field["value"])} }}',
                      f'struct {name} {{ entries : array<{name}_Entry> }}', '']
        else: lines += [f'struct {name} {{ values : array<{typ(field["element"])}> }}', '']

    def path_expr(path):
        # Replace the validated array wildcard after quoting, inserting only our loop variable.
        text = quote(path)
        for depth in range(16,0,-1): text = text.replace('.*i' + str(depth), '.{input_index_' + str(depth) + '}')
        for depth in range(16,-1,-1): text = text.replace('.*' + str(depth), '.{output_index_' + str(depth) + '}')
        return text.replace('.*', '.{index}')

    # Friendly authored value names stay aliases of the generated representation.
    authored_values = {}
    for path in [*enums, *order]:
        if path.startswith('/Game/') and path not in source_mappings: authored_values.setdefault(ident(path.rsplit('.', 1)[-1]), []).append(path)
    for short, paths in sorted(authored_values.items()):
        if len(paths) == 1 and short not in symbols:
            register(short)
            full = enum_name(paths[0]) if paths[0] in enums else struct_name(paths[0])
            lines += [f'typedef {short} = {full}', '']
            if paths[0] in enums and all(short + suffix not in symbols for suffix in ('_from_symbol', '_to_symbol')):
                register(short + '_from_symbol'); register(short + '_to_symbol')
                lines += [f'def {short}_from_symbol(value : string) : {short} {{ return {full}_from_symbol(value) }}', f'def {short}_to_symbol(value : {short}) : string {{ return {full}_to_symbol(value) }}', '']
    def source_name(field):
        mapping = source_mappings.get(field.get('type_path', ''))
        if field['kind'] == 'array': return 'array<' + source_name(field['element']) + '>'
        return source_module + '::' + mapping['name'] if mapping else typ(field)
    def uses_source(field):
        return uses_source(field['element']) if field['kind']=='array' else field.get('type_path') in source_mappings
    def source_convert_expr(field, value, to_generated):
        if field.get('type_path') in source_mappings:
            full = enum_name(field['type_path']) if field['kind'] == 'enum' else struct_name(field['type_path'])
            return ('ue_source_to_' if to_generated else 'ue_source_from_') + full + '(' + value + ')'
        return value
    def convert_field(field, value, target, to_generated, depth=0):
        if field['kind'] in ('map','set'):
            kind=field['kind']; index='source_index_'+str(depth); key='source_key_'+str(depth)
            child=field['key'] if kind=='map' else field['element']
            if to_generated:
                items=target+('.entries' if kind=='map' else '.values')
                mapped='source_value_'+str(depth)
                iteration=f'{key}, {mapped}, {index} in keys({value}), values({value}), range(length({value}))' if kind=='map' else f'{key}, {index} in keys({value}), range(length({value}))'
                lines.extend([f'    resize({items}, length({value}))',f'    for ({iteration}) {{'])
                lines.append(f'        {items}[{index}]'+('.key' if kind=='map' else '')+' = '+key)
                if kind=='map':
                    before=len(lines);convert_field(field['value'],mapped,items+'['+index+'].value',True,depth+1)
                    for i in range(before,len(lines)): lines[i]='    '+lines[i]
                lines.append('    }')
            else:
                items=value+('.entries' if kind=='map' else '.values')
                key_value=items+'['+index+']'+('.key' if kind=='map' else '')
                lines.extend([f'    for ({index} in range(length({items}))) {{',f'        if (key_exists({target}, {key_value})) {{ panic("duplicate source hash key/element") }}'])
                if kind=='map':
                    before=len(lines);convert_field(field['value'],items+'['+index+'].value',target+'['+key_value+']',False,depth+1)
                    for i in range(before,len(lines)): lines[i]='    '+lines[i]
                else: lines.append(f'        insert({target}, {key_value})')
                lines.append('    }')
        elif field['kind'] == 'array':
            index = 'source_index_'  + str(depth)
            lines.extend([f'    resize({target}, length({value}))', f'    for ({index} in range(length({value}))) {{'])
            before = len(lines)
            convert_field(field['element'], value + '[' + index + ']', target + '[' + index + ']', to_generated, depth + 1)
            for i in range(before, len(lines)): lines[i] = '    ' + lines[i]
            lines.append('    }')
        elif field['kind'] == 'name':
            expression = 'ue_name(ue_name_text(' + value + '))' if to_generated else 'ue_name_value(' + value + '.value)'
            lines.append('    ' + target + ' = ' + expression)
        elif field['kind'] == 'object':
            wrapper = class_name(field['type_path'])
            if to_generated: lines.append('    ' + target + ' = ' + wrapper + '_from(' + value + '.handle)')
            else: lines.append('    ' + target + '.handle = ' + wrapper + '_raw(' + value + ')')
        else:
            operation = '<-' if owns_container(field) else '='
            lines.append('    ' + target + ' ' + operation + ' ' + source_convert_expr(field, value, to_generated))
    for path, mapping in sorted(source_mappings.items()):
        full = enum_name(path) if mapping['kind'] == 'enum' else struct_name(path)
        source = source_module + '::' + mapping['name']
        to_native, from_native = 'ue_source_to_' + full, 'ue_source_from_' + full
        args = ', '.join(key + '=' + quote(value) for key, value in [('kind', mapping['kind']), ('id', mapping['id']), ('path', path), ('registry', source_types['registry'])])
        lines += ['[ue_source_value_binding(' + args + ')]', f'def {to_native}(value : {source}) : {full} {{']
        if mapping['kind'] == 'enum':
            for member in enums[path]: lines += [f'    if (value == {source}.{member["name"]}) {{ return {full}.v_{ident(member["name"])} }}']
            lines += ['    panic("invalid source enum binding value")', f'    return {full}.v_{ident(enums[path][0]["name"])}']
        else:
            lines += [f'    var result : {full}']
            for field in structures[path]['fields']:
                convert_field(field, 'value.' + mapping['members'][field['path']], 'result.f_' + ident(field['path']), True)
            lines += ['    return <- result' if owns_container(dict(kind='struct',type_path=path)) else '    return result']
        lines += ['}', '', '[ue_source_value_binding(' + args + ')]', f'def {from_native}(value : {full}) : {source} {{']
        if mapping['kind'] == 'enum':
            for member in enums[path]: lines += [f'    if (value == {full}.v_{ident(member["name"])}) {{ return {source}.{member["name"]} }}']
            lines += ['    panic("invalid generated enum binding value")', f'    return {source}.{enums[path][0]["name"]}']
        else:
            lines += [f'    var result = {source}()']
            for field in structures[path]['fields']:
                convert_field(field, 'value.f_' + ident(field['path']), 'result.' + mapping['members'][field['path']], False)
            lines += ['    return <- result' if owns_container(dict(kind='struct',type_path=path)) else '    return result']
        lines += ['}', '']
    def write_input(field, path, val, indent='    ', depth=0):
        if field['kind']=='soft_object':
            lines.append(f'{indent}__ue_method_arg_struct(call, {path_expr(path)}, ue_soft_value({val}))');return
        if field['kind']=='delegate':
            lines.append(f"{indent}__ue_arg_native_delegate(call, {path_expr(path)}, {quote(field['type_path'])}, {val}.target, {val}.method)")
            return
        if is_opaque_native(field):
            lines.append(f"{indent}{OPAQUE_TRANSPORTS[field['type_path']][2]}(call, {path_expr(path)}, {val})")
            return
        if field['kind'] == 'struct' and field['type_path'] in math_types:
            bridge = math_types[field['type_path']][1]
            lines.append(f'{indent}__ue_method_arg_{bridge}(call, {path_expr(path)}, {val})')
            return
        if field['kind'] in ('map', 'set'):
            items = val + ('.entries' if field['kind'] == 'map' else '.values')
            index='index' if depth==0 else 'input_index_'+str(depth)
            marker='.*' if depth==0 else '.*i'+str(depth)
            lines.extend([f'{indent}ue_arg_hash(call, {path_expr(path)}, int(length({items})))', f'{indent}for ({index} in range(length({items}))) {{'])
            if field['kind'] == 'map':
                write_input(field['key'], path + marker + '.key', items + '['+index+'].key', indent + '    ', depth+1)
                write_input(field['value'], path + marker + '.value', items + '['+index+'].value', indent + '    ', depth+1)
            else: write_input(field['element'], path + marker, items + '['+index+']', indent + '    ', depth+1)
            lines.append(indent + '}')
            return
        if field['kind'] == 'array':
            index = 'index' if depth == 0 else 'input_index_' + str(depth)
            lines.extend([f'{indent}ue_arg_array(call, {path_expr(path)}, int(length({val})))',
                          f'{indent}for ({index} in range(length({val}))) {{'])
            marker = '.*' if depth == 0 else '.*i' + str(depth)
            write_input(field['element'], path + marker, val + '[' + index + ']', indent + '    ', depth + 1)
            lines.append(indent + '}')
            return
        if field['kind'] == 'struct':
            for child in structures[field['type_path']]['fields']:
                write_input(child, path + '.' + child['path'], val + '.f_' + ident(child['path']), indent, depth)
            return
        if field['kind'] == 'object': val = class_name(field['type_path']) + '_raw(' + val + ')'
        if field['kind'] == 'name': val += '.value'
        if field['kind'] == 'enum': val = quote(field['type_path']) + ', ' + enum_name(field['type_path']) + '_to_symbol(' + val + ')'
        if field['kind'] == 'byte': val = 'int(' + val + ')'
        bridge = '__ue_method_arg_text' if field['kind'] == 'text' else 'ue_arg_' + field['kind']
        lines.append(f'{indent}{bridge}(call, {path_expr(path)}, {val})')

    def read_output(field, path):
        if is_opaque_native(field): return f"{OPAQUE_TRANSPORTS[field['type_path']][3]}(call, {path_expr(path)})"
        if field['kind'] == 'struct' and field['type_path'] in math_types:
            bridge = math_types[field['type_path']][1]
            return f'__ue_method_result_{bridge}(call, {path_expr(path)})'
        if field['kind'] == 'struct':
            return struct_name(field['type_path']) + '(' + ', '.join('f_' + ident(p['path']) + ' = ' + read_output(p, path + '.' + p['path']) for p in structures[field['type_path']]['fields']) + ')'
        bridge = '__ue_method_result_text' if field['kind'] == 'text' else 'ue_result_' + field['kind']
        expr = f'{bridge}(call, {path_expr(path)}' + (', ' + quote(field['type_path']) if field['kind'] == 'enum' else '') + ')'
        if field['kind'] == 'byte': expr = 'uint8(' + expr + ')'
        if field['kind'] == 'object': expr = class_name(field['type_path']) + '_from(' + expr + ')'
        if field['kind'] == 'name': expr = 'UEName(value = ' + expr + ')'
        if field['kind'] == 'enum': expr = enum_name(field['type_path']) + '_from_symbol(' + expr + ')'
        return expr

    def read_owned_output(field, path, target, depth=0, indent='    '):
        if field['kind'] == 'struct' and field['type_path'] not in math_types and not is_opaque_native(field):
            for child in structures[field['type_path']]['fields']: read_owned_output(child, path + '.' + child['path'], target + '.f_' + ident(child['path']), depth, indent)
        elif field['kind'] in ('map','set'):
            index='output_index_'+str(depth)
            items=target+('.entries' if field['kind']=='map' else '.values')
            lines.extend([f'{indent}resize({items}, ue_result_hash_size(call, {path_expr(path)}))',f'{indent}for ({index} in range(length({items}))) {{'])
            if field['kind']=='map':
                read_owned_output(field['key'],path+'.*'+str(depth)+'.key',items+'['+index+'].key',depth+1,indent+'    ')
                read_owned_output(field['value'],path+'.*'+str(depth)+'.value',items+'['+index+'].value',depth+1,indent+'    ')
            else: read_owned_output(field['element'],path+'.*'+str(depth),items+'['+index+']',depth+1,indent+'    ')
            lines.append(indent+'}')
        elif field['kind'] == 'array':
            index = 'output_index_'  + str(depth)
            def indexed_quote(value):
                text = quote(value)
                for i in range(16,-1,-1): text = text.replace('.*' + str(i), '.{output_index_' + str(i) + '}')
                return text
            lines.extend([f'{indent}resize({target}, ue_result_array_size(call, {indexed_quote(path)}))', f'{indent}for ({index} in range(length({target}))) {{'])
            read_owned_output(field['element'], path + '.*' + str(depth), target + '[' + index + ']', depth + 1, indent + '    ')
            lines.append(indent + '}')
        else:
            value = read_output(field, path)
            # read_output preserves the generic wildcard syntax; replace typed indices.
            for i in range(16,-1,-1): value = value.replace('.*' + str(i), '.{output_index_' + str(i) + '}')
            lines.append(indent + target + ' = ' + value)
    for fn in sorted(ir['functions'], key=lambda item: item['alias']):
        alias, params_ir = fn['alias'], fn['parameters']
        inputs = [p for p in params_ir if p['input'] and p['path'] != fn.get('world_context', '')]
        outputs = [p for p in params_ir if p['output']]
        result_type = alias + '_Result' if len(outputs) > 1 else typ(outputs[0]) if outputs else ''
        if len(outputs) > 1:
            lines += [f'struct {result_type} {{'] + [f'    out_{ident(p["path"])} : {typ(p)}' for p in outputs] + ['}', '']
        params = ([] if fn.get('static', False) else ['target : ' + class_name(fn['class'])]) + [f'arg_{ident(p["path"])} : {typ(p)}' + (' = ' + defaults[(alias, p['path'])] if (alias, p['path']) in defaults else '') for p in inputs]
        if fn.get('static', False):
            lines += [f'[ue_reflected_call(owner={quote(fn["class"])}, method={quote(fn["function"])}, signature={quote(fn["signature"])})]']
        lines += [f'def {alias}(' + '; '.join(params) + ')' + (f' : {result_type}' if result_type else '') + ' {']
        if fn.get('static', False):
            lines += [f'    let call = ue_begin_static_call({quote(fn["class"])}, {quote(fn["function"])}, {quote(fn.get("world_context", ""))})',
                      f'    ue_expect_signature(call, {quote(fn["signature"])})']
        elif fn['class'] in interfaces:
            lines += [f'    let receiver = {class_name(fn["class"])}_raw(target)',
                      f'    let call = ue_begin_interface_call(receiver, {quote(fn["class"])}, {quote(fn["function"])})',
                      f'    ue_expect_interface_signature(call, {quote(fn["signature"])})']
        else:
            lines += [f'    let receiver = ue_cast_object({class_name(fn["class"])}_raw(target), {quote(fn["class"])})',
                      f'    let call = ue_begin_call(receiver, {quote(fn["function"])})',
                      f'    ue_expect_signature(call, {quote(fn["signature"])})']
        lines += ['    ue_require_call_context(call, ' + ('true' if 'UnsafeDuringActorConstruction' in fn.get('metadata', {}) else 'false') + ')']
        for p in inputs: write_input(p, p['path'], 'arg_' + ident(p['path']))
        lines += ['    ue_invoke_call(call)']
        has_arrays = any(owns_container(p) for p in outputs)
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
                if (p['kind'] == 'struct' and owns_container(p)) or (p['kind'] == 'array' and owns_container(p['element'])):
                    read_owned_output(p, p['path'], target)
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
    # daScript resolves value.Method(args) to Method(value, args) for free functions.
    # Keep the checked low-level wrappers as the single native call implementation.
    emitted_methods = set()
    for fn in sorted(ir['functions'], key=lambda f: f['alias']):
        if fn['class'] not in interfaces: continue
        method = fn['function']
        if not re.fullmatch(r'[A-Za-z_][A-Za-z0-9_]*', method) or method == 'handle': continue
        key = (fn['class'], method)
        if key in emitted_methods: continue
        emitted_methods.add(key)
        parameters = fn['parameters']
        returns = [p for p in parameters if p['path'].lower() == 'returnvalue']
        if len(returns) > 1: raise ValueError('Duplicate interface return parameter')
        returned = returns[0] if returns else None
        arguments = [p for p in parameters if p is not returned]
        outputs = [p for p in parameters if p['output']]
        signature = ['target : ' + class_name(fn['class'])]
        for param in arguments:
            name = 'arg_' + ident(param['path'])
            mutable = param['output']
            declaration = ('var ' if mutable else '') + name + ' : ' + source_name(param) + ('&' if mutable else '')
            if not mutable and (fn['alias'], param['path']) in defaults:
                default_value = defaults[(fn['alias'], param['path'])]
                if param.get('type_path') in source_mappings:
                    if param['kind'] != 'enum': raise ValueError('Source-mapped default requires enum')
                    member = default_value.split('.v_', 1)[1]
                    default_value = source_name(param) + '.' + member
                declaration += ' = ' + default_value
            signature.append(declaration)
        lines += [f'def {method}(' + '; '.join(signature) + ')' + (' : ' + source_name(returned) if returned else ' : void') + ' {']
        input_values=['target']
        for p in parameters:
            if not p['input']: continue
            value='arg_'+ident(p['path'])
            if p['kind']=='array' and uses_source(p):
                local='native_'+value;lines.append('    var '+local+' : '+typ(p));convert_field(p,value,local,True);value=local
            elif p.get('type_path') in source_mappings: value=source_convert_expr(p,value,True)
            input_values.append(value)
        call = fn['alias'] + '(' + ', '.join(input_values) + ')'
        container_outputs = any(owns_container(p) for p in outputs)
        if outputs: lines += [('    var result <- ' if container_outputs else '    let result = ') + call]
        else: lines += ['    ' + call]
        source_outputs={}
        for p in outputs:
            if uses_source(p):
                local='source_out_'+ident(p['path']);value='result.out_'+ident(p['path']) if len(outputs)>1 else 'result'
                if p['kind']=='array':
                    lines.append('    var '+local+' : '+source_name(p));convert_field(p,value,local,False)
                else: lines.append('    var '+local+(' <- ' if owns_container(p) else ' = ')+source_convert_expr(p,value,False))
                source_outputs[p['path']]=local
        def selected_output(param):
            if param['path'] in source_outputs:return source_outputs[param['path']]
            value = 'result.out_' + ident(param['path']) if len(outputs) > 1 else 'result'
            if param.get('type_path') in source_mappings: value = 'ue_source_from_' + (enum_name(param['type_path']) if param['kind'] == 'enum' else struct_name(param['type_path'])) + '(' + value + ')'
            return value
        # Native validation/result extraction completes before modifying caller variables.
        for param in arguments:
            if param['output']:
                operation = '<-' if owns_container(param) else '='
                lines += ['    arg_' + ident(param['path']) + ' ' + operation + ' ' + selected_output(param)]
        if returned:
            operation = '<-' if owns_container(returned) else ''
            lines += ['    return ' + (operation + ' ' if operation else '') + selected_output(returned)]
        lines += ['}', '']
    text = '\n'.join(lines)
    if source_module:
        # Source declarations may transitively import another binding module.
        # Qualify our own references without rewriting declarations, fields,
        # comments, quoted metadata or already-qualified names.
        owned_functions = set(re.findall(r'^def ([A-Za-z_][A-Za-z0-9_]*)', text, re.M))
        owned_types = set(re.findall(r'^(?:struct|enum|typedef) ([A-Za-z_][A-Za-z0-9_]*)', text, re.M))
        owned_enums = set(re.findall(r'^enum ([A-Za-z_][A-Za-z0-9_]*)', text, re.M))
        tokens = re.findall(r'//[^\n]*|"(?:\\.|[^"\\])*"|[A-Za-z_][A-Za-z0-9_]*|::|\s+|.', text)
        significant = [i for i, token in enumerate(tokens) if not token.isspace() and not token.startswith('//')]
        for position, i in enumerate(significant):
            token = tokens[i]
            before = tokens[significant[position - 1]] if position else ''
            after = tokens[significant[position + 1]] if position + 1 < len(significant) else ''
            alias_rhs = before == '=' and position >= 3 and tokens[significant[position - 3]] == 'typedef'
            reference = (token in owned_functions and after == '(') or (token in owned_types and (before in (':','<') or after == '(' or alias_rhs)) or (token in owned_enums and after == '.')
            if reference and before not in ('def','struct','enum','typedef','module','require','.','::') and after != '::':
                tokens[i] = module + '::' + token
        text = ''.join(tokens)
    return text


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('ir', type=Path)
    parser.add_argument('output', type=Path)
    parser.add_argument('--module', default='ue_generated')
    parser.add_argument('--source-types', type=Path)
    parser.add_argument('--canonical-math', action='store_true', help='Reuse foundation UEVector2D/UEVector/UERotator/UETransform')
    parser.add_argument('--check', action='store_true', help='Fail if the checked-in module differs')
    parser.add_argument('--native-binds-source', type=Path, action='append', default=[],
                        help='Also scan this source folder for native bind types (besides the plugin)')
    args = parser.parse_args()
    source_types = json.loads(args.source_types.read_text(encoding='utf-8-sig')) if args.source_types else None
    native_names = native_type_names([NATIVE_BINDS_SOURCE] + args.native_binds_source)
    text = generate(json.loads(args.ir.read_text(encoding='utf-8-sig')), args.module, source_types,
                    canonical_math=args.canonical_math, native_names=native_names)
    print('GENERATE_BINDINGS_SKIPPED_ALIASES=' + ','.join(skipped_aliases(text)))
    if args.check:
        if not args.output.exists() or args.output.read_text(encoding='utf-8') != text:
            parser.exit(1, 'Generated bindings are stale\n')
    else:
        args.output.write_text(text, encoding='utf-8', newline='\n')


if __name__ == '__main__':
    main()
