options gen2
require ue_text_byte_container_types
require ue_object_keys
[ue_object_ref(script="dsl_text_byte_containers.das",type="TextByteContainers")]
struct ContainersRef { handle:UObjectHandle }
[export]
def main():int {
    let actor=ue_ref(type<ContainersRef>,ue_self_object())
    if (actor.ProbeCalls==0) {
        if (length(actor.ByteTags)!=2 || !key_exists(actor.ByteTags,uint8(0)) || !key_exists(actor.ByteTags,uint8(255)) || actor.StringBytes["boundary"]!=uint8(255)) { return 47 }
        let defaultText=actor.ByteTexts[uint8(255)]
        if (ue_text_namespace(defaultText)!="Containers" || ue_text_key(defaultText)!="DefaultLabel" || ue_text_to_string(defaultText)!="Default label") { return 48 }
    }
    actor.ProbeCalls += 1
    var values:array<uint8> = [uint8(0),uint8(255)]
    actor.Weights:=values
    var echoed <- actor.EchoBytes(actor.Weights)
    if (length(echoed)!=2 || echoed[0]!=uint8(0) || echoed[1]!=uint8(255)) { return 1 }
    let text=ue_text("Containers","Roundtrip","Localized value")
    var texts:array<FText> = [text,ue_text_invariant("Invariant")]
    actor.Messages:=texts
    var stored <- actor.Messages
    if (ue_text_key(stored[0])!="Roundtrip" || !ue_text_is_invariant(stored[1])) { return 2 }
    let owner=ue_ref(type<ActorRef>,ue_self_object())
    let objectKey=ue_key(owner)
    {
        let key=5
        var data:table<int;uint8>
        data[key]=uint8(255)
        actor.IntBytes:=data
        var read <- actor.IntBytes
        if (read[key]!=uint8(255)) { return 10 }
    }
    {
        let key=5
        var data:table<int;FText>
        data[key]=text
        actor.IntTexts:=data
        var read <- actor.IntTexts
        if (ue_text_key(read[key])!="Roundtrip") { return 11 }
    }
    {
        let key="label"
        var data:table<string;uint8>
        data[key]=uint8(255)
        actor.StringBytes:=data
        var read <- actor.StringBytes
        if (read[key]!=uint8(255)) { return 12 }
    }
    {
        let key="label"
        var data:table<string;FText>
        data[key]=text
        actor.StringTexts:=data
        var read <- actor.StringTexts
        if (ue_text_key(read[key])!="Roundtrip") { return 13 }
    }
    {
        let key=ue_name_value("Label")
        var data:table<UENameValue;uint8>
        data[key]=uint8(255)
        actor.NameBytes:=data
        var read <- actor.NameBytes
        if (read[key]!=uint8(255)) { return 14 }
    }
    {
        let key=ue_name_value("Label")
        var data:table<UENameValue;FText>
        data[key]=text
        actor.NameTexts:=data
        var read <- actor.NameTexts
        if (ue_text_key(read[key])!="Roundtrip") { return 15 }
    }
    {
        let key=ContainerState.Active
        var data:table<ContainerState;uint8>
        data[key]=uint8(255)
        actor.StateBytes:=data
        var read <- actor.StateBytes
        if (read[key]!=uint8(255)) { return 16 }
    }
    {
        let key=ContainerState.Active
        var data:table<ContainerState;FText>
        data[key]=text
        actor.StateTexts:=data
        var read <- actor.StateTexts
        if (ue_text_key(read[key])!="Roundtrip") { return 17 }
    }
    {
        let key=objectKey
        var data:table<ActorKey;uint8>
        data[key]=uint8(255)
        actor.ObjectBytes:=data
        var read <- actor.ObjectBytes
        if (read[key]!=uint8(255)) { return 18 }
    }
    {
        let key=objectKey
        var data:table<ActorKey;FText>
        data[key]=text
        actor.ObjectTexts:=data
        var read <- actor.ObjectTexts
        if (ue_text_key(read[key])!="Roundtrip") { return 19 }
    }
    {
        var data:table<uint8;int>
        data[uint8(255)]=-7
        actor.ByteInts:=data
        var read <- actor.ByteInts
        if (read[uint8(255)]!=-7) { return 20 }
    }
    {
        var data:table<uint8;float>
        data[uint8(255)]=1.25f
        actor.ByteFloats:=data
        var read <- actor.ByteFloats
        if (read[uint8(255)]!=1.25f) { return 21 }
    }
    {
        var data:table<uint8;double>
        data[uint8(255)]=2.5lf
        actor.ByteDoubles:=data
        var read <- actor.ByteDoubles
        if (read[uint8(255)]!=2.5lf) { return 22 }
    }
    {
        var data:table<uint8;bool>
        data[uint8(255)]=true
        actor.ByteBools:=data
        var read <- actor.ByteBools
        if (!read[uint8(255)]) { return 23 }
    }
    {
        var data:table<uint8;string>
        data[uint8(255)]="Value"
        actor.ByteStrings:=data
        var read <- actor.ByteStrings
        if (read[uint8(255)]!="Value") { return 24 }
    }
    {
        var data:table<uint8;UENameValue>
        data[uint8(255)]=ue_name_value("Name")
        actor.ByteNames:=data
        var read <- actor.ByteNames
        if (ue_name_text(read[uint8(255)])!="Name") { return 25 }
    }
    {
        var data:table<uint8;uint8>
        data[uint8(255)]=uint8(255)
        actor.ByteBytes:=data
        var read <- actor.ByteBytes
        if (read[uint8(255)]!=uint8(255)) { return 26 }
    }
    {
        var data:table<uint8;FText>
        data[uint8(255)]=text
        actor.ByteTexts:=data
        var read <- actor.ByteTexts
        if (ue_text_key(read[uint8(255)])!="Roundtrip") { return 27 }
    }
    {
        var data:table<uint8;ContainerState>
        data[uint8(255)]=ContainerState.Active
        actor.ByteEnums:=data
        var read <- actor.ByteEnums
        if (read[uint8(255)]!=ContainerState.Active) { return 28 }
    }
    {
        var data:table<uint8;ActorRef>
        data[uint8(255)]=owner
        actor.ByteObjects:=data
        var read <- actor.ByteObjects
        if (!ue_object_valid(read[uint8(255)].handle)) { return 29 }
    }
    var bag:TextByteBag
    push(bag.Weights,uint8(255))
    push(bag.Messages,text)
    insert(bag.Tags,uint8(0))
    insert(bag.Tags,uint8(255))
    bag.Labels[uint8(255)]=text
    bag.Values["value"]=uint8(255)
    actor.Bag:=bag
    var saved <- actor.Bag
    if (saved.Weights[0]!=uint8(255) || ue_text_key(saved.Messages[0])!="Roundtrip" || length(saved.Tags)!=2 || ue_text_key(saved.Labels[uint8(255)])!="Roundtrip" || saved.Values["value"]!=uint8(255)) { return 40 }
    var bags:table<uint8;TextByteBag>
    insert_clone(bags,uint8(255),bag)
    actor.ByteStructs:=bags
    var savedBags <- actor.ByteStructs
    if (ue_text_key(savedBags[uint8(255)].Messages[0])!="Roundtrip") { return 41 }
    var output:array<FText>
    actor.Adjust(values,output)
    if (length(values)!=3 || values[2]!=uint8(255) || length(output)!=1 || ue_text_key(output[0])!="Output") { return 42 }
    var byteMap:table<string;uint8>
    var tags <- actor.Reverse(output,byteMap)
    if (length(output)!=2 || !ue_text_is_invariant(output[1]) || byteMap["boundary"]!=uint8(255) || length(tags)!=2) { return 43 }
    actor.ByteTags:=tags
    var copiedTags <- actor.ByteTags
    if (length(copiedTags)!=2) { return 44 }
    var labels:table<string;FText>
    labels["title"]=text
    var copied <- actor.EchoTexts(labels)
    if (ue_text_key(copied["title"])!="Roundtrip") { return 45 }
    actor.Weights[0]=uint8(9)
    push(actor.Weights,uint8(7))
    actor.Messages[0]=ue_text("Containers","Mutation","Changed")
    actor.StringTexts["label"]=ue_text_invariant("Changed")
    insert(actor.ByteTags,uint8(42))
    let removed=erase(actor.ByteTags,uint8(0))
    if (actor.Weights[0]!=uint8(9) || length(actor.Weights)!=3 || ue_text_key(actor.Messages[0])!="Mutation" || !ue_text_is_invariant(actor.StringTexts["label"]) || !removed || length(actor.ByteTags)!=2) { return 46 }
    return 0
}
