"""Docs of the native binds stay equal to the code they quote, and the reference generator renders a manifest.

A guide quotes a block of a source file with a marker line before its fenced code block:

    <!-- snippet: Tests/DasForUESmoke/Source/DasForUESmoke/NativeBindsExample.cpp#module -->

The source marks the block with `// [doc:module]` ... `// [/doc]`; the quote is the dedented block.
"""
import re
import textwrap
import unittest
from pathlib import Path

from NativeModulesReference import render

ROOT = Path(__file__).resolve().parents[1]
SNIPPET = re.compile(r'<!-- snippet: ([^#\s]+)#([A-Za-z0-9_-]+) -->\n```[a-z]*\n(.*?)\n```', re.S)


def source_block(path, name):
    text = (ROOT / path).read_text(encoding='utf-8')
    match = re.search(r'^[ \t]*// \[doc:' + re.escape(name) + r'\]\n(.*?)^[ \t]*// \[/doc\]', text, re.S | re.M)
    return textwrap.dedent(match.group(1)).rstrip('\n') if match else None


class NativeBindsDocsTest(unittest.TestCase):
    def test_snippets_match_sources(self):
        found = 0
        for guide in sorted((ROOT / 'Docs').rglob('*.md')):
            text = guide.read_text(encoding='utf-8')
            for path, name, quoted in SNIPPET.findall(text):
                found += 1
                block = source_block(path, name)
                where = f'{guide.relative_to(ROOT)}: {path}#{name}'
                self.assertIsNotNone(block, where + ': no [doc:' + name + '] block in the source')
                self.assertEqual(quoted, block, where + ': the quote differs from the source')
        self.assertGreaterEqual(found, 2)

    def test_reference_render(self):
        def fn(name, receiver, arguments, result, **extra):
            item = dict(name=name, receiver=receiver, arguments=arguments, result=result, origin='declared',
                        pure=False, non_null_args=False, reflects='', routed=False, route_note='')
            item.update(extra)
            return item
        manifest = {'modules': [
            {'name': 'ue_engine', 'classes': ['AActor'], 'functions': [
                fn('get_actor_location', 'AActor', ['AActor'], 'UEVector',
                   reflects='/Script/Engine.Actor:K2_GetActorLocation', routed=True),
                fn('set_replicates', 'AActor', ['AActor', 'bool'], 'void',
                   reflects='/Script/Engine.Actor:SetReplicates', route_note='requires authority'),
                fn('get_distance_to', 'AActor', ['AActor', 'AActor'], 'float', non_null_args=True),
                fn('valid', '', ['AActor'], 'bool', origin='helper'),
                fn('world_of', '', ['UObjectHandle'], 'UWorld')]},
            {'name': 'ue_math', 'classes': [], 'functions': [
                fn('clamp', '', ['int', 'int', 'int'], 'int', pure=True)]},
            {'name': 'game_module', 'classes': ['APickup'], 'functions': [fn('grant', 'APickup', ['APickup'], 'void')]},
        ]}
        text = render(manifest)
        self.assertIn('| `get_actor_location() : UEVector` | reflects `K2_GetActorLocation` |', text)
        self.assertIn('| `set_replicates(bool)` | reflects `SetReplicates` (kept: requires authority) |', text)
        self.assertIn('| `get_distance_to(AActor) : float` | non-null arguments |', text)
        self.assertIn('| `world_of(UObjectHandle) : UWorld` |  |', text)
        self.assertIn('| `clamp(int, int, int) : int` | pure |', text)
        self.assertNotIn('valid(', text.split('### AActor')[1])  # helpers are described once, not listed
        self.assertNotIn('game_module', text)  # project modules are not part of the plugin reference
        self.assertEqual(render(manifest), text)


if __name__ == '__main__':
    unittest.main()
