"""Render recorded validation receipts, with a deterministic freshness gate.

    python Scripts/RenderValidation.py
    python Scripts/RenderValidation.py --check

Only top-level logs/*.json receipts are considered; unrelated probe/report JSON is
ignored. Existing manual reports are never edited or deleted. A PASS row records
what its receipt says, not a certification of the current checkout's source hashes.
"""
from __future__ import annotations

import argparse
from datetime import datetime, timezone
import hashlib
import json
import os
from pathlib import Path, PureWindowsPath
from urllib.parse import quote
import sys

ROOT = Path(__file__).resolve().parents[1]


def cell(value: object) -> str:
    return (str(value).replace("&", "&amp;").replace("<", "&lt;").replace(">", "&gt;")
            .replace("|", "&#124;").replace("[", "&#91;").replace("]", "&#93;")
            .replace("\r", " ").replace("\n", " "))


def link(label: str, target: Path | str, output: Path) -> str:
    text = str(target)
    if PureWindowsPath(text).is_absolute() and os.name != "nt":
        href = PureWindowsPath(text).as_posix()
    else:
        path = Path(text).resolve()
        try:
            href = os.path.relpath(path, output.parent.resolve()).replace("\\", "/")
        except ValueError:  # different Windows drives
            href = path.as_posix()
    return f"[{cell(label)}]({quote(href, safe='/:.~_-')})"


def check_count(data: dict) -> str:
    for key in ("checks", "runtime_checks", "check_count", "measurements", "cases"):
        value = data.get(key)
        if isinstance(value, int) and not isinstance(value, bool) and value >= 0:
            return str(value)
        if isinstance(value, list):
            return str(len(value))
    return "—"


def receipt_date(data: dict, source: Path) -> str:
    for key in ("completed_at", "verified_at_utc", "checked_at_utc", "timestamp", "generated_at", "date"):
        value = data.get(key)
        if isinstance(value, str) and value:
            try:
                stamp = datetime.fromisoformat(value.replace("Z", "+00:00"))
            except ValueError as exc:
                raise ValueError(f"{source.name}: invalid {key}: {value!r}") from exc
            if stamp.tzinfo is None:
                stamp = stamp.replace(tzinfo=timezone.utc)
            return stamp.astimezone(timezone.utc).isoformat(timespec="seconds")
    return datetime.fromtimestamp(source.stat().st_mtime, timezone.utc).isoformat(timespec="seconds")


def log_links(data: dict, source: Path, output: Path) -> str:
    paths = []
    for key in ("runtime_log", "runtime_logs", "log", "logs"):
        value = data.get(key)
        for item in value if isinstance(value, list) else [value]:
            if isinstance(item, str) and item and item not in paths:
                paths.append(item)
    links = []
    for index, value in enumerate(paths):
        if not Path(value).is_absolute() and not PureWindowsPath(value).is_absolute():
            # Relative receipt paths are relative to the checkout, not the Markdown directory.
            value = source.parent.parent / value
        links.append(link("log" if len(paths) == 1 else f"log {index + 1}", value, output))
    return ", ".join(links) or "—"


def is_build_log(raw: bytes) -> bool:
    # UBT also writes *-validation.json, but as JSON Lines diagnostic events, not receipts.
    try:
        records = [json.loads(line) for line in raw.decode("utf-8-sig").splitlines() if line.strip()]
    except (UnicodeError, json.JSONDecodeError):
        return False
    return bool(records) and all(isinstance(record, dict) and
                                 {"time", "level", "message"}.issubset(record) for record in records)


def render(logs: Path, output: Path) -> tuple[str, int]:
    rows, fingerprints = [], []
    for source in sorted(logs.glob("*.json"), key=lambda p: p.name.casefold()):
        raw = source.read_bytes()
        candidate = source.name.endswith("-validation.json")
        try:
            data = json.loads(raw.decode("utf-8-sig"))
        except (UnicodeError, json.JSONDecodeError) as exc:
            if candidate and not is_build_log(raw):
                raise ValueError(f"{source.name}: malformed validation JSON: {exc}") from exc
            continue
        if isinstance(data, dict) and {"time", "level", "message"}.issubset(data):
            continue
        if not isinstance(data, dict):
            if candidate:
                raise ValueError(f"{source.name}: receipt must be a JSON object")
            continue
        if not candidate and not any(key in data for key in ("state", "result")):
            continue
        state = data.get("state", data.get("result", "UNKNOWN"))
        if not isinstance(state, str) and not candidate:
            continue
        if not isinstance(state, str):
            raise ValueError(f"{source.name}: state/result must be a string")
        feature = data.get("feature") or source.stem.removesuffix("-validation")
        if not isinstance(feature, str):
            raise ValueError(f"{source.name}: feature must be a string")
        rows.append("| " + " | ".join((link(feature, source, output), cell(state),
                    check_count(data), receipt_date(data, source), log_links(data, source, output))) + " |")
        relative = os.path.relpath(source.resolve(), output.parent.resolve()).replace("\\", "/")
        fingerprints.append(f"<!-- source: {cell(relative)} sha256={hashlib.sha256(raw).hexdigest()} -->")
    lines = ["# Validation receipts", "", "Generated by `Scripts/RenderValidation.py` from local `logs/*.json`.",
             "Feature links open the source JSON. Dates are UTC; when a receipt has no explicit timestamp,",
             "its file modification time is used. Missing counts and log paths are shown as —.",
             "Recorded PASS results do not certify the current source revision. Manual reports remain available.",
             "", "Regenerate: `python Scripts/RenderValidation.py`; check freshness: add `--check`.", "",
             "| Feature | State | Checks | Date (UTC) | Log |", "| --- | --- | ---: | --- | --- |", *rows]
    if not rows:
        lines += ["", "No local validation receipts are available."]
    lines += ["", *fingerprints, ""]
    return "\n".join(lines), len(rows)


def main(argv: list[str] | None = None) -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--logs", type=Path, default=ROOT / "logs")
    parser.add_argument("--output", type=Path, default=ROOT / "Docs/Dev/Validation/index.md")
    parser.add_argument("--check", action="store_true", help="fail without writing when the index differs")
    args = parser.parse_args(argv)
    try:
        text, count = render(args.logs, args.output)
        if args.check:
            actual = args.output.read_text(encoding="utf-8") if args.output.exists() else None
            if actual != text:
                print(f"DAS_VALIDATION_INDEX=STALE {args.output}; run RenderValidation.py", file=sys.stderr)
                return 1
        else:
            args.output.parent.mkdir(parents=True, exist_ok=True)
            temporary = args.output.with_name(args.output.name + ".tmp")
            temporary.write_text(text, encoding="utf-8", newline="\n")
            temporary.replace(args.output)
        print(f"DAS_VALIDATION_INDEX=PASS receipts={count}")
        return 0
    except (OSError, ValueError) as exc:
        print(f"DAS_VALIDATION_INDEX=ERROR {exc}", file=sys.stderr)
        return 2


if __name__ == "__main__":
    raise SystemExit(main())
