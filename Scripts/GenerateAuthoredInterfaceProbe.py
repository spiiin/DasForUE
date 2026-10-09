"""Generate an isolated test module from actual authored-interface reflection IR."""
import json, re, sys
from pathlib import Path
from GenerateBindings import generate
ir_path, scripts, probe_id = Path(sys.argv[1]), Path(sys.argv[2]), sys.argv[3]
if not re.fullmatch(r"[0-9a-f]{32}", probe_id): raise ValueError("Invalid probe ID")
ir = json.loads(ir_path.read_text(encoding="utf-8-sig"))
module = "ue_authored_" + probe_id
state_path = next(p["type_path"] for fn in ir["functions"] if fn["alias"] == "ue_contract_EchoState" for p in fn["parameters"] if p["kind"] == "enum")
settings_def = next(s for s in ir["structs"] if s["type_path"].endswith("InterfaceSettings.InterfaceSettings"))
manifest = {"module":"ue_interface_values", "registry":"/Game/DasFrameAssetProbe/Registered_" + probe_id + "/TypesRegistry.TypesRegistry", "types":[
    {"kind":"enum","path":state_path,"name":"InterfaceState","id":"interfacevalues.State","members":{"Idle":7,"Active":42}},
    {"kind":"struct","path":settings_def["type_path"],"name":"InterfaceSettings","id":"interfacevalues.Settings","members":{field["path"]:("Count" if field["kind"]=="int" else "Title") for field in settings_def["fields"]}}
]}
(ir_path.parent / ("SourceTypes_" + probe_id + ".json")).write_text(json.dumps(manifest,indent=2),encoding="utf-8")
(scripts / (module + ".das")).write_text(generate(ir, module, manifest), encoding="utf-8")
settings = next(s for s in ir["structs"] if s["type_path"].endswith("InterfaceSettings.InterfaceSettings"))
count = next(f["path"] for f in settings["fields"] if f["kind"] == "int")
title = next(f["path"] for f in settings["fields"] if f["kind"] == "string")
source = f'''options gen2
require ue_foundation
require {module}
[export]
def main(mode:int):int {{
    let api = ue_interface(type<CounterContract>, ue_self_object())
    verify(CounterContract_valid(api))
    verify(api.Query(5) == 123)
    verify(api.Label() == "Wrapper 世界")
    var counter = 10
    var total = -1
    api.Stats(counter, total)
    verify(counter == 10 && total == 20)
    let state = api.EchoState(InterfaceState.Active)
    verify(state == InterfaceState.Active)
    var settings = InterfaceSettings()
    settings.Count = 33
    settings.Title = "Generated 世界"
    let copied = api.EchoSettings(settings)
    verify(copied.Count == 33 && copied.Title == "Generated 世界")
    return 1
}}
'''
(scripts / ("authored_wrapper_check_" + probe_id + ".das")).write_text(source, encoding="utf-8")
print("DAS_AUTHORED_WRAPPER_GENERATION=PASS")
