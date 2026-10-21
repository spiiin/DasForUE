import contextlib
import io
import json
import os
from pathlib import Path
import tempfile
import unittest

import RenderValidation as renderer


class TestRenderValidation(unittest.TestCase):
    def setUp(self):
        self.temp = tempfile.TemporaryDirectory()
        self.addCleanup(self.temp.cleanup)
        self.root = Path(self.temp.name)
        self.logs = self.root / "logs"
        self.logs.mkdir()
        self.output = self.root / "Docs/Dev/Validation/index.md"

    def receipt(self, name, data, *, bom=False):
        path = self.logs / name
        path.write_text(json.dumps(data), encoding="utf-8-sig" if bom else "utf-8")
        os.utime(path, (0, 0))
        return path

    def cli(self, *arguments):
        with contextlib.redirect_stdout(io.StringIO()), contextlib.redirect_stderr(io.StringIO()):
            return renderer.main(["--logs", str(self.logs), "--output", str(self.output), *arguments])

    def test_mixed_receipts_bom_sorted_counts_and_dates(self):
        self.receipt("z-validation.json", {"result": "PASS", "checks": ["a", "b"],
                     "runtime_logs": ["logs/z.log", "logs/z2.log"]}, bom=True)
        self.receipt("a-validation.json", {"state": "FAIL", "runtime_checks": 7,
                     "timestamp": "2026-10-20T13:00:00+02:00", "runtime_log": "logs/a.log"})
        text, count = renderer.render(self.logs, self.output)
        self.assertEqual(count, 2)
        self.assertLess(text.index("[a]"), text.index("[z]"))
        self.assertIn("| FAIL | 7 | 2026-10-20T11:00:00+00:00 |", text)
        self.assertIn("| PASS | 2 | 1970-01-01T00:00:00+00:00 |", text)
        self.assertIn("[log 2](../../../logs/z2.log)", text)
        self.assertEqual(text, renderer.render(self.logs, self.output)[0])

    def test_no_invented_count_or_log_and_pending_is_not_pass(self):
        self.receipt("pending-validation.json", {"state": "pending", "checks": True})
        text, _ = renderer.render(self.logs, self.output)
        self.assertIn("| pending | — |", text)
        self.assertIn("| — |\n", text)
        self.assertNotIn("| PASS |", text)

    def test_other_top_level_receipt_and_unrelated_json(self):
        self.receipt("benchmark.json", {"result": "PASS", "checks": 33})
        self.receipt("example.json", {"map": "Entry"})
        self.receipt("array.json", [1, 2])
        (self.logs / "unrelated.json").write_text("incomplete probe")
        self.assertEqual(renderer.render(self.logs, self.output)[1], 1)

    def test_check_missing_current_changed_added_removed_and_no_write(self):
        first = self.receipt("first-validation.json", {"state": "PASS", "checks": 2})
        self.assertEqual(self.cli("--check"), 1)
        self.assertFalse(self.output.exists())
        self.assertEqual(self.cli(), 0)
        self.assertEqual(self.cli("--check"), 0)
        original = self.output.read_bytes()
        self.receipt(first.name, {"state": "PASS", "checks": 2, "evidence": "changed"})
        self.assertEqual(self.cli("--check"), 1)
        self.assertEqual(self.output.read_bytes(), original)
        self.assertEqual(self.cli(), 0)
        second = self.receipt("second-validation.json", {"state": "FAIL"})
        self.assertEqual(self.cli("--check"), 1)
        self.assertEqual(self.cli(), 0)
        second.unlink()
        self.assertEqual(self.cli("--check"), 1)

    def test_mtime_fallback_is_part_of_freshness(self):
        path = self.receipt("probe-validation.json", {"state": "PASS"})
        self.assertEqual(self.cli(), 0)
        os.utime(path, (86400, 86400))
        self.assertEqual(self.cli("--check"), 1)

    def test_manual_reports_are_preserved(self):
        self.output.parent.mkdir(parents=True)
        manual = self.output.parent / "Manual.md"
        manual.write_text("manual evidence")
        self.assertEqual(self.cli(), 0)
        self.assertEqual(manual.read_text(), "manual evidence")

    def test_escape_cells_and_links(self):
        self.receipt("a b-validation.json", {"feature": "a|b\n[x]<script>", "state": "FAIL",
                     "runtime_log": "logs/a b).log"})
        text, _ = renderer.render(self.logs, self.output)
        self.assertIn("a&#124;b &#91;x&#93;&lt;script&gt;", text)
        self.assertIn("a%20b%29.log", text)
        self.assertNotIn("<script>", text)

    def test_invalid_receipts_fail_without_replacing_index(self):
        self.cli()
        original = self.output.read_bytes()
        bad = self.logs / "bad-validation.json"
        for value in ["{", "[]", '{"state":false}', '{"state":"PASS","date":"yesterday"}']:
            bad.write_text(value)
            self.assertEqual(self.cli(), 2)
            self.assertEqual(self.output.read_bytes(), original)

    def test_ubt_json_lines_are_logs_even_with_validation_suffix(self):
        log = self.logs / "build-game-validation.json"
        event = {"time": "2026-10-10T19:28:21", "level": "Debug", "message": "building"}
        log.write_text(json.dumps(event) + "\n" + json.dumps(event) + "\n")
        self.assertEqual(renderer.render(self.logs, self.output)[1], 0)
        log.write_text(json.dumps(event))
        self.assertEqual(renderer.render(self.logs, self.output)[1], 0)

    def test_legacy_utc_dates_and_unknown_aggregate_state(self):
        self.receipt("aggregate-validation.json", {"checked_at_utc": "2026-10-17T13:57:57.886915Z"})
        text, count = renderer.render(self.logs, self.output)
        self.assertEqual(count, 1)
        self.assertIn("| UNKNOWN | — | 2026-10-17T13:57:57+00:00 |", text)

    def test_empty_logs_and_timestamp_without_zone(self):
        self.assertEqual(self.cli(), 0)
        self.assertIn("No local validation receipts", self.output.read_text(encoding="utf-8"))
        self.receipt("one-validation.json", {"state": "PASS", "date": "2026-10-20"})
        self.assertIn("2026-10-20T00:00:00+00:00", renderer.render(self.logs, self.output)[0])


if __name__ == "__main__":
    unittest.main()
