"""Check C++ formatting and line length in the plugin sources.

Usage:
    python Scripts/CheckFormat.py            # check all sources
    python Scripts/CheckFormat.py --fix      # reformat in place
    python Scripts/CheckFormat.py --staged   # check only files staged for commit

clang-format is looked up on PATH, then in the Visual Studio LLVM tools.
Generated files (*.generated.h, Private/Generated/*) are skipped.
"""
import argparse
import glob
import os
import shutil
import subprocess
import sys

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
SOURCE = os.path.join(ROOT, "Plugins", "DasForUE", "Source")
TEST_SOURCE = os.path.join(ROOT, "Plugins", "DasForUE", "Tests", "Runtime")
MAX_LINE = 400  # limit for new lines; string literals may exceed ColumnLimit


def find_clang_format():
    exe = shutil.which("clang-format")
    if exe:
        return exe
    for pattern in (
        r"C:\Program Files\Microsoft Visual Studio\2022\*\VC\Tools\Llvm\x64\bin\clang-format.exe",
        r"C:\BuildTools\2022\VC\Tools\Llvm\x64\bin\clang-format.exe",
    ):
        hits = glob.glob(pattern)
        if hits:
            return hits[0]
    return None


def is_generated(path):
    norm = path.replace("\\", "/")
    return norm.endswith(".generated.h") or "/Private/Generated/" in norm


def source_files(staged):
    if staged:
        out = subprocess.run(
            ["git", "diff", "--cached", "--name-only", "--diff-filter=ACM"],
            cwd=ROOT, capture_output=True, text=True, check=True).stdout.split()
        files = [os.path.join(ROOT, f) for f in out]
    else:
        files = [os.path.join(r, f) for base in (SOURCE, TEST_SOURCE) for r, _, fs in os.walk(base) for f in fs]
    return [f for f in files
            if f.endswith((".cpp", ".h", ".inl")) and not is_generated(f)
            and any(os.path.abspath(f).startswith(base + os.sep) for base in (SOURCE, TEST_SOURCE))]


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--fix", action="store_true")
    parser.add_argument("--staged", action="store_true")
    args = parser.parse_args()

    files = source_files(args.staged)
    failures = 0
    if args.staged:
        # New or changed lines must stay under MAX_LINE (existing long literals are tolerated).
        diff = subprocess.run(["git", "diff", "--cached", "-U0", "--", *files],
                              cwd=ROOT, capture_output=True, text=True, check=True).stdout
        current = None
        for line in diff.splitlines():
            if line.startswith("+++ "):
                current = line[6:]
            elif line.startswith("+") and len(line) - 1 > MAX_LINE:
                print(f"{current}: added line longer than {MAX_LINE} characters")
                failures += 1
    else:
        long_lines = 0
        for path in files:
            with open(path, encoding="utf-8", errors="replace") as f:
                long_lines += sum(1 for line in f if len(line.rstrip("\r\n")) > MAX_LINE)
        if long_lines:
            print(f"note: {long_lines} existing lines exceed {MAX_LINE} characters (mostly embedded script literals)")

    exe = find_clang_format()
    if exe is None:
        print("clang-format not found; only line length was checked")
    elif files:
        mode = ["-i"] if args.fix else ["--dry-run", "-Werror"]
        for start in range(0, len(files), 50):
            result = subprocess.run([exe, *mode, "--style=file"] + files[start:start + 50], cwd=ROOT)
            if result.returncode != 0:
                failures += 1

    print("DAS_FORMAT=" + ("FAIL" if failures else "PASS"))
    return 1 if failures else 0


if __name__ == "__main__":
    sys.exit(main())
