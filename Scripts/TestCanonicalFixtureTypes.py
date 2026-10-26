"""Transport fixtures follow the canonical built-in color API used by generated packages."""
from pathlib import Path
import re
import unittest

ROOT = Path(__file__).resolve().parents[1]

class CanonicalFixtureTypesTest(unittest.TestCase):
    def test_transport_colors_use_canonical_handles(self):
        for relative in ('Tests/Paint/probe.template.das', 'Tests/HitResultTransport/probe.template.das'):
            with self.subTest(path=relative):
                source = (ROOT / relative).read_text(encoding='utf-8-sig')
                self.assertNotRegex(source, r'\bS_LinearColor_[0-9a-f]+\b')
                self.assertIn('ue_linear_color(', source)

if __name__ == '__main__':
    unittest.main()
