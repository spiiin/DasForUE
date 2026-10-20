"""Type adapter gate (Docs/Dev/Design/TypeAdapters.md).

A ported value type is identified only inside its adapter files: no struct path literal or old predicate
may reappear in other C++ sources.
"""
import os
import re
import unittest

ROOT = os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', 'Plugins', 'DasForUE', 'Source')

# Ported type -> (patterns, files allowed to contain them).
PORTED = {
    'GameplayTag': (
        [r'/Script/GameplayTags\.', r'\bDSLGameplay', r'\bIsDasGameplayTagPath\b', r'\bDasIsGameplayTagType\b'],
        {'DasGameplayTags.cpp', 'DasGameplayTagsDSL.inl', 'DasGameplayTagsRuntime.inl', 'DasGameplayTags.h'},
    ),
}


def sources():
    for folder, _, files in os.walk(ROOT):
        for name in files:
            if name.endswith(('.cpp', '.h', '.inl')):
                yield name, os.path.join(folder, name)


class TypeAdapterGateTest(unittest.TestCase):
    def test_ported_types_stay_inside_their_adapters(self):
        offenders = []
        for kind, (patterns, allowed) in PORTED.items():
            regex = re.compile('|'.join(patterns))
            for name, path in sources():
                if name in allowed:
                    continue
                with open(path, encoding='utf-8', errors='replace') as f:
                    for number, line in enumerate(f, 1):
                        if regex.search(line):
                            offenders.append(f'{kind}: {os.path.relpath(path, ROOT)}:{number}: {line.strip()}')
        self.assertEqual(offenders, [], '\n'.join(offenders))


if __name__ == '__main__':
    unittest.main()
