options gen2
require ue_text_byte_delegates
[ue_class(Blueprintable), ue_implements(interface="/Game/DasTextByteEvents/Types/TextByteContract.TextByteContract_C")]
class TextByteEvents {
    @ue_property=(BlueprintReadWrite)
    TextCallback:TextQuery
    @ue_property=(BlueprintReadWrite)
    ByteCallback:ByteQuery
    @ue_property=(BlueprintAssignable)
    Changed:TextByteChanged
    @ue_property=(BlueprintAssignable)
    RefChanged:TextByteRefChanged
    @ue_property=(BlueprintReadWrite)
    LastByte:uint8=0
    @ue_property=(BlueprintReadWrite)
    LastText:FText=ue_text_invariant("")
    @ue_property=(BlueprintReadWrite)
    Seen:int=0
    [ue_function(BlueprintCallable)]
    def EchoByte(value:uint8):uint8 { return value }
    [ue_function(BlueprintCallable)]
    def EchoText(value:FText):FText { return value }
    [ue_function(BlueprintCallable)]
    def Change(var number:uint8&; var title:FText&;
               @ue_out var previous:uint8&; @ue_out var oldTitle:FText&):FText {
        previous=number
        oldTitle=title
        number=uint8(255)
        title=ue_text("TextByteEvents","Changed","Changed text")
        return oldTitle
    }
    [ue_function(BlueprintCallable)]
    def ChangeArrays(var numbers:array<uint8>&; @ue_out var messages:array<FText>&):void {
        push(numbers,uint8(255))
        messages <- [{ue_text("TextByteEvents","Array","Array text"); ue_text_invariant("Invariant")}]
    }
    [ue_function(BlueprintCallable)]
    def RunText(callback:TextQuery; value:FText):FText { return callback.Execute(value) }
    [ue_function(BlueprintCallable)]
    def RunByte(callback:ByteQuery; value:uint8):uint8 { return callback.Execute(value) }
    [ue_function(BlueprintCallable)]
    def OnChanged(number:uint8; title:FText):void {
        self.LastByte=number; self.LastText=title; self.Seen+=1
    }
    [ue_function(BlueprintCallable)]
    def OnRefChanged(var number:uint8&; var title:FText&):void {
        number=uint8(255); title=ue_text_invariant("ref changed")
    }
    [ue_event(name="OnDasByteSignal")]
    def NativeByte(value:uint8):void { self.LastByte=value; self.Seen+=1 }
    [ue_event(name="OnDasTextSignal")]
    def NativeText(value:FText):void { self.LastText=value; self.Seen+=1 }
    [ue_event(name="OnDasTextRefSignal")]
    def NativeTextRef(value:FText):void { self.LastText=value; self.Seen+=1 }
    [ue_event(name="OnDasArrayByteSignal")]
    def NativeBytes(value:array<uint8>):void {
        if(length(value)!=2 || value[0]!=uint8(0) || value[1]!=uint8(255)) { panic("byte array event") }
        self.Seen+=1
    }
    [ue_event(name="OnDasArrayTextSignal")]
    def NativeTexts(value:array<FText>):void {
        if(length(value)!=2 || ue_text_key(value[0])!="Source" || !ue_text_is_invariant(value[1])) { panic("text array event") }
        self.Seen+=1
    }
    [ue_function(BlueprintCallable)]
    def Probe():bool {
        let original=ue_text("TextByteEvents","Source","Original text")
        self.TextCallback=ue_bind(type<TextQuery>,ue_self_object(),"EchoText")
        self.ByteCallback=ue_bind(type<ByteQuery>,ue_self_object(),"EchoByte")
        let copy=self.TextCallback
        if(ue_text_key(copy.Execute(original))!="Source" || self.ByteCallback.Execute(uint8(255))!=uint8(255) || self.ByteCallback.Execute(uint8(0))!=uint8(0)) { return false }
        let mutation=ue_bind(type<TextByteMutation>,ue_self_object(),"Change")
        var number:uint8=0
        var title=original
        var previous:uint8=99
        var oldTitle=ue_text_invariant("old")
        let returned=mutation.Execute(number,title,previous,oldTitle)
        if(number!=uint8(255) || previous!=uint8(0) || ue_text_key(title)!="Changed" || ue_text_key(oldTitle)!="Source" || ue_text_key(returned)!="Source") { return false }
        var rejected=false
        try { let wrong=ue_bind(type<ByteQuery>,ue_self_object(),"EchoText") }
        recover { rejected=true }
        if(!rejected) { return false }
        // Multicast mutable references are shared across listeners.
        self.RefChanged.AddUFunction(ue_self_object(),"OnRefChanged")
        self.RefChanged.Broadcast(number,title)
        self.RefChanged.Clear()
        if(number!=uint8(255) || !ue_text_is_invariant(title)) { return false }
        self.Changed.AddUFunction(ue_self_object(),"OnChanged")
        self.Changed.Broadcast(uint8(255),original)
        self.Changed.RemoveUFunction(ue_self_object(),"OnChanged")
        if(self.LastByte!=uint8(255) || ue_text_key(self.LastText)!="Source") { return false }
        let arrays=ue_bind(type<TextByteArrays>,ue_self_object(),"ChangeArrays")
        var numbers:array<uint8>
        var messages:array<FText>
        arrays.Execute(numbers,messages)
        return length(numbers)==1 && numbers[0]==uint8(255) && length(messages)==2 && ue_text_key(messages[0])=="Array" && ue_text_is_invariant(messages[1])
    }
}
