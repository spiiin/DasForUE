"""Require every HitResult declaration in the collision profile to pass generation.

This checks a freshly regenerated package report, not runtime trace behaviour.
It deliberately does not equate a partially generated package with full coverage.
"""
import argparse
import itertools
import json
from pathlib import Path

def required_declarations():
    result = {("/Script/Engine.KismetSystemLibrary", shape+"Trace"+mode+suffix)
              for shape, mode, suffix in itertools.product(
                  ("Line", "Sphere", "Box", "Capsule"),
                  ("Single", "Multi"), ("", "ByProfile", "ForObjects"))}
    result.update(("/Script/Engine.GameplayStatics", name) for name in (
        "Blueprint_PredictProjectilePath_Advanced",
        "Blueprint_PredictProjectilePath_ByObjectType",
        "Blueprint_PredictProjectilePath_ByTraceChannel",
        "BreakHitResult", "FindCollisionUV", "GetSurfaceType", "MakeHitResult"))
    result.update(("/Script/Engine.PrimitiveComponent", name) for name in (
        "K2_BoxOverlapComponent", "K2_SphereOverlapComponent"))
    return result

def check(report, traces_only=False):
    if report.get("package") != "ue_api_collision":
        raise ValueError("Expected collision package report")
    if report.get("state") not in ("ue_preflight_passed", "published"):
        raise ValueError("Whole generated module has not passed UE preflight")
    members = {}
    for member in report["members"]:
        key = (member["class"], member["function"])
        if key in members:
            raise ValueError("Duplicate declaration: " + str(key))
        members[key] = member
    required = required_declarations()
    if traces_only:
        required = {key for key in required if key[0].endswith(".KismetSystemLibrary")}
    failures = []
    for key in sorted(required):
        member = members.get(key)
        if not member or member.get("status") != "generation_ready":
            failures.append(dict(class_path=key[0], function=key[1],
                status=member.get("status") if member else "missing",
                reason=member.get("reason", "").strip() if member else "Missing declaration"))
    return dict(state="FAIL" if failures else "PASS",
                required=len(required), ready=len(required)-len(failures), failures=failures,
                scope="generation_and_whole_module_preflight",
                runtime_trace_sweep="not_assessed")

def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("report", type=Path)
    parser.add_argument("--output", type=Path)
    parser.add_argument("--traces-only", action="store_true")
    args = parser.parse_args()
    result = check(json.loads(args.report.read_text(encoding="utf-8-sig")), traces_only=args.traces_only)
    text = json.dumps(result, ensure_ascii=False, indent=2)+"\n"
    if args.output:
        args.output.write_text(text, encoding="utf-8")
    print(text, end="")
    return int(result["state"] != "PASS")

if __name__ == "__main__":
    raise SystemExit(main())
