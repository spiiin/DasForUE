options gen2
require ue_native_gameplay


def hash_probe():NativeHashProbeRef {
 return ue_component_get(type<NativeHashProbeRef>,ue_ref(type<ActorRef>,ue_self_object()),"NativeHashProbe")
}
[export]
def main():bool {
 let api=hash_probe()
 var counts:table<string;int>
 counts["one"]=11
 api.Counts:=counts
 var saved <- api.Counts
 saved["one"]=99
 verify(api.Counts["one"]==11,"native map field is an owned copy")
 var echoed <- api.EchoCounts(counts)
 verify(echoed["one"]==11 && counts["one"]==11,"native map input and return")
 api.UpdateCounts(counts)
 verify(counts["one"]==11 && counts["native"]==7,"native map inout preserves input")
 var output:table<string;int>
 output["old"]=1
 api.ReadCounts(output)
 verify(length(output)==1 && output["one"]==11,"native map out replaces old data")
 var positions:table<UENameValue;UEVector>
 let point=ue_name_value("point")
 positions[point]=ue_vector(1.0lf,2.0lf,3.0lf)
 api.Positions:=positions
 var positioned <- api.EchoPositions(positions)
 verify(positioned[point].z==3.0lf && api.Positions[point].x==1.0lf,"native name-key math map")
 var weights:table<EDasNativeHashKey;double>
 weights[EDasNativeHashKey.First]=2.5lf
 weights[EDasNativeHashKey.Second]=7.0lf
 api.Weights:=weights
 var weighted <- api.EchoWeights(weights)
 verify(weighted[EDasNativeHashKey.First]==2.5lf && length(api.Weights)==2,"native enum-key map")
 let actor=ue_ref(type<ActorRef>,ue_self_object())
 let key=ue_key(actor)
 let nil=default<ActorKey>
 var labels:table<ActorKey;string>
 labels[key]="owner"
 labels[nil]="null"
 api.ActorLabels:=labels
 var labeled <- api.EchoActorLabels(labels)
 verify(labeled[key]=="owner" && labeled[nil]=="null" && api.ActorLabels[key]=="owner","native UObject-key map")
 var numbers:table<int>
 insert(numbers,-1)
 insert(numbers,3)
 api.Numbers:=numbers
 api.UpdateNumbers(numbers)
 verify(key_exists(numbers,42) && key_exists(numbers,3) && !key_exists(numbers,-1),"native set inout")
 var tags:table<UENameValue>
 insert(tags,ue_name_value("tag"))
 insert(tags,ue_name_value("tag"))
 api.Tags:=tags
 verify(length(api.Tags)==1,"native name set deduplicates")
 var modes:table<EDasNativeHashKey>
 insert(modes,EDasNativeHashKey.First)
 insert(modes,EDasNativeHashKey.Second)
 api.Modes:=modes
 var mode_output:table<EDasNativeHashKey>
 api.ReadModes(mode_output)
 verify(length(mode_output)==2 && key_exists(mode_output,EDasNativeHashKey.Second),"native enum set out")
 var actors:table<ActorKey>
 insert(actors,key)
 insert(actors,ue_key(actor))
 insert(actors,nil)
 api.Actors:=actors
 var actor_output <- api.EchoActors(actors)
 verify(length(actor_output)==2 && key_exists(actor_output,key) && key_exists(actor_output,nil),"native object set identity and null")
 verify(length(api.ReadOnlyNames)==1 && key_exists(api.ReadOnlyNames,"locked"),"native readonly set read")
 var nested=DasNativeHashValue()
 nested.Labels[7]="nested"
 insert(nested.Names,"value")
 api.Nested:=nested
 var nested_output=api.EchoNested(nested)
 verify(nested_output.Labels[7]=="nested" && key_exists(api.Nested.Names,"value"),"native struct containing map/set")
 return true
}
[export]
def stale():bool {
 let api=hash_probe()
 var bad_set=false
 var bad_map=false
 try { var value <- api.Actors; length(value) } recover { bad_set=true }
 try { var value <- api.ActorLabels; length(value) } recover { bad_map=true }
 var empty_set:table<ActorKey>
 var empty_map:table<ActorKey;string>
 api.Actors:=empty_set
 api.ActorLabels:=empty_map
 return bad_set && bad_map && length(api.Actors)==0 && length(api.ActorLabels)==0
}
