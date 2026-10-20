"""Value kinds shared by the generators.

Bindings/Kinds.json is exported from the C++ kind table (DasScalarAccessors.inl) and checked by the
foundation tests, so generators take property classes and daScript types from it instead of keeping
their own copies.
"""
import json
import os

_PATH = os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', 'Bindings', 'Kinds.json')
_KINDS = None


def kinds():
    """Kind rows by name: property, self (daScript self accessor type), reference, struct_leaf."""
    global _KINDS
    if _KINDS is None:
        with open(_PATH, encoding='utf-8') as f:
            _KINDS = {row['name']: row for row in json.load(f)['kinds']}
    return _KINDS


def property_classes(*extra):
    """{kind: property class} for the scalar kinds plus enum, and the named extra kinds."""
    rows = kinds()
    return {name: row['property'] for name, row in rows.items() if row['struct_leaf'] or name == 'enum' or name in extra}


def reference_type(kind):
    """daScript value type of a kind on authored/native references, or None for non-table kinds."""
    row = kinds().get(kind)
    return row['reference'] if row else None
