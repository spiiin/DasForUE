options gen2
options gc
require ue_text_byte_container_types
require ue_object_keys
[ue_class(type_folder="/Game/DasTextByteContainers/Types",Blueprintable)]
class TextByteContainers : Actor {
    properties=ue_properties(EditAnywhere,BlueprintReadWrite) {
        var ProbeCalls:int=0
        var Weights:array<uint8> = [uint8(0),uint8(255)]
        var Messages:array<FText> = [ue_text("Containers","First","First"),ue_text_invariant("Second")]
        var ByteTags:table<uint8> = {uint8(0),uint8(255)}
        var IntBytes:table<int;uint8>
        var StringBytes:table<string;uint8> = {"boundary"=>uint8(255)}
        var NameBytes:table<UENameValue;uint8>
        var StateBytes:table<ContainerState;uint8>
        var ObjectBytes:table<ActorKey;uint8>
        var IntTexts:table<int;FText>
        var StringTexts:table<string;FText>
        var NameTexts:table<UENameValue;FText>
        var StateTexts:table<ContainerState;FText>
        var ObjectTexts:table<ActorKey;FText>
        var ByteTexts:table<uint8;FText> = {uint8(255)=>ue_text("Containers","DefaultLabel","Default label")}
        var ByteInts:table<uint8;int>
        var ByteFloats:table<uint8;float>
        var ByteDoubles:table<uint8;double>
        var ByteBools:table<uint8;bool>
        var ByteStrings:table<uint8;string>
        var ByteNames:table<uint8;UENameValue>
        var ByteBytes:table<uint8;uint8>
        var ByteEnums:table<uint8;ContainerState>
        var ByteObjects:table<uint8;ActorRef>
        var ByteStructs:table<uint8;TextByteBag>
        var Bag:TextByteBag = TextByteBag()
    }
    [ue_function(BlueprintCallable)]
    def EchoBytes(value:array<uint8>):array<uint8> { return <- clone(value) }
    [ue_function(BlueprintCallable)]
    def EchoTexts(value:table<string;FText>):table<string;FText> { return <- clone(value) }
    [ue_function(BlueprintCallable)]
    def Adjust(var bytes:array<uint8>&; @ue_out var texts:array<FText>&):void {
        push(bytes,uint8(255))
        push(texts,ue_text("Containers","Output","Output"))
    }
    [ue_function(BlueprintCallable)]
    def Reverse(var texts:array<FText>&; @ue_out var bytes:table<string;uint8>&):table<uint8> {
        push(texts,ue_text_invariant("Appended"))
        bytes["boundary"]=uint8(255)
        var result:table<uint8>
        insert(result,uint8(0))
        insert(result,uint8(255))
        return <- result
    }
}
