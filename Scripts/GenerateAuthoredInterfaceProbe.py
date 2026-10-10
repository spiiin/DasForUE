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
    {"kind":"struct","path":settings_def["type_path"],"name":"InterfaceSettings","id":"interfacevalues.Settings","members":{field["path"]:("Count" if field["kind"]=="int" else "Title" if field["kind"]=="string" else "Details" if field["kind"]=="struct" else "Items") for field in settings_def["fields"]}}
]}
details_def = next(s for s in ir["structs"] if s["type_path"].endswith("InterfaceDetails.InterfaceDetails"))
manifest["types"].append({"kind":"struct","path":details_def["type_path"],"name":"InterfaceDetails","id":"interfacevalues.Details","members":{field["path"]:("Weights" if field["kind"]=="map" and field["key"]["kind"]=="string" else "Labels" if field["kind"]=="map" else "Tags" if field["kind"]=="set" and field["element"]["kind"]=="string" else "Ids" if field["kind"]=="set" else "Amount" if field["kind"]=="int" else "Text" if field["kind"]=="string" else "Key" if field["kind"]=="name" else "Events" if field["type_path"]=="/Script/DasRuntime.DasEventInterface" else "Target" if field["type_path"]=="/Script/Engine.Actor" else "Object") for field in details_def["fields"]}})
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
    let events = ue_interface(type<{module}::DasEventInterface>, ue_self_object())
    let echoed_events = api.EchoEvents(events)
    verify(ue_implements_interface(echoed_events.handle,"/Script/DasRuntime.DasEventInterface"))
    let empty_events = ue_interface(type<{module}::DasEventInterface>, ue_null_object())
    let null_events = api.EchoEvents(empty_events)
    verify(!ue_object_valid(null_events.handle))
    let state = api.EchoState(InterfaceState.Active)
    verify(state == InterfaceState.Active)
    var settings = InterfaceSettings()
    settings.Count = 33
    settings.Title = "Generated 世界"
    settings.Details.Weights["alpha"] = 1.25lf
    settings.Details.Weights["Unicode 世界"] = 2.5lf
    settings.Details.Labels[7] = ue_name_value("MapName_9")
    insert(settings.Details.Tags, "Tag 世界")
    insert(settings.Details.Ids, 17)
    settings.Details.Events.handle = ue_self_object()
    settings.Details.Key = ue_name_value("NestedKey_7")
    settings.Details.Target.handle = ue_self_object()
    settings.Details.Object.handle = ue_self_object()
    settings.Details.Amount = 21
    settings.Details.Text = "Nested 世界"
    var first = InterfaceDetails()
    first.Weights["item"] = 3.75lf
    first.Labels[-5] = ue_name_value("ItemName_3")
    insert(first.Tags, "ItemTag")
    insert(first.Ids, 23)
    first.Events.handle = ue_self_object()
    first.Key = ue_name_value("ItemKey_9")
    first.Target.handle = ue_self_object()
    first.Object.handle = ue_self_object()
    first.Amount = 31
    first.Text = "Item 世界"
    push_clone(settings.Items, first)
    first.Amount = 47
    push_clone(settings.Items, first)
    var details <- api.EchoDetails(settings.Items)
    verify(length(details)==2 && details[0].Amount==31 && details[1].Amount==47)
    verify(details[0].Weights["item"]==3.75lf && key_exists(details[1].Tags,"ItemTag"))
    verify(ue_name_text(details[0].Labels[-5])=="ItemName_3" && ue_object_valid(details[0].Target.handle))
    details[0].Amount=999
    details[0].Weights["item"]=99.0lf
    verify(settings.Items[0].Amount==31 && settings.Items[0].Weights["item"]==3.75lf)
    var none : array<InterfaceDetails>
    var none_copy <- api.EchoDetails(none)
    verify(length(none_copy)==0)
    var ref_state = InterfaceState.Active
    var previous_state = InterfaceState.Idle
    api.RefState(ref_state, previous_state)
    verify(ref_state==InterfaceState.Active && previous_state==InterfaceState.Active)
    var ref_settings <- clone(settings)
    var previous_settings = InterfaceSettings()
    api.RefSettings(ref_settings, previous_settings)
    verify(previous_settings.Count==33 && length(previous_settings.Items)==2 && previous_settings.Items[0].Weights["item"]==3.75lf)
    previous_settings.Items[0].Weights["item"]=999.0lf
    verify(ref_settings.Items[0].Weights["item"]==3.75lf && settings.Items[0].Weights["item"]==3.75lf)
    var copied <- api.EchoSettings(settings)
    verify(copied.Count == 33 && copied.Title == "Generated 世界")
    verify(copied.Details.Amount == 21 && copied.Details.Text == "Nested 世界")
    verify(length(copied.Items) == 2 && copied.Items[0].Amount == 31 && copied.Items[1].Amount == 47)
    verify(ue_implements_interface(copied.Details.Events.handle,"/Script/DasRuntime.DasEventInterface") && ue_implements_interface(copied.Items[0].Events.handle,"/Script/DasRuntime.DasEventInterface"))
    verify(ue_names_equal(copied.Details.Key, settings.Details.Key))
    verify(ue_name_text(copied.Items[0].Key) == "ItemKey_9")
    verify(ue_object_valid(copied.Details.Target.handle) && ue_object_valid(copied.Items[0].Object.handle))
    copied.Items[0].Target = default<ActorRef>
    copied.Details.Key = ue_name_value("ChangedKey")
    verify(ue_object_valid(settings.Items[0].Target.handle) && ue_name_text(settings.Details.Key) == "NestedKey_7")
    verify(copied.Details.Weights["alpha"] == 1.25lf && copied.Details.Weights["Unicode 世界"] == 2.5lf)
    verify(ue_name_text(copied.Details.Labels[7]) == "MapName_9")
    verify(key_exists(copied.Details.Tags, "Tag 世界") && key_exists(copied.Details.Ids, 17))
    verify(copied.Items[1].Weights["item"] == 3.75lf && ue_name_text(copied.Items[1].Labels[-5]) == "ItemName_3")
    verify(key_exists(copied.Items[0].Tags, "ItemTag") && key_exists(copied.Items[0].Ids, 23))
    copied.Details.Weights["alpha"] = 99.0lf
    copied.Items[0].Labels[-5] = ue_name_value("ChangedMap")
    erase(copied.Items[0].Tags, "ItemTag")
    verify(settings.Details.Weights["alpha"] == 1.25lf && ue_name_text(settings.Items[0].Labels[-5]) == "ItemName_3")
    verify(key_exists(settings.Items[0].Tags, "ItemTag"))
    copied.Items[0].Amount = 99
    copied.Details.Text = "Changed"
    verify(settings.Items[0].Amount == 31 && settings.Details.Text == "Nested 世界")
    var again <- api.EchoSettings(settings)
    verify(again.Items[0].Amount == 31 && again.Details.Text == "Nested 世界")
    var empty = InterfaceSettings()
    var empty_copy <- api.EchoSettings(empty)
    verify(length(empty_copy.Items) == 0 && empty_copy.Details.Amount == 2 && empty_copy.Details.Text == "Detail")
    verify(ue_name_text(empty_copy.Details.Key) == "None" && !ue_object_valid(empty_copy.Details.Target.handle) && !ue_object_valid(empty_copy.Details.Object.handle))
    verify(length(empty_copy.Details.Weights) == 0 && length(empty_copy.Details.Tags) == 0 && length(empty_copy.Details.Labels) == 0 && length(empty_copy.Details.Ids) == 0)
    return 1
}}
'''
(scripts / ("authored_wrapper_check_" + probe_id + ".das")).write_text(source, encoding="utf-8")
holder = f'''options gen2
require ue_dsl
require {module}
[ue_class(module_id="referenceholder_{probe_id}", Blueprintable)]
class ReferenceHolder {{
    @ue_property=(EditAnywhere, BlueprintReadWrite)
    Target : {module}::CounterContract
    [ue_function(BlueprintCallable)]
    def Echo(value:{module}::CounterContract):{module}::CounterContract {{ return value }}
    [ue_function(BlueprintCallable)]
    def Store(value:{module}::CounterContract):void {{ self.Target = value }}
    [ue_function(BlueprintCallable)]
    def Read():{module}::CounterContract {{ return self.Target }}
}}
'''
(scripts / ("interface_holder_" + probe_id + ".das")).write_text(holder,encoding="utf-8")
print("DAS_AUTHORED_WRAPPER_GENERATION=PASS")
