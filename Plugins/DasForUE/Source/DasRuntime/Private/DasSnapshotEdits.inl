// Context-owned drafts survive daScript error recovery, which can bypass C++ destructors.
// Published snapshots remain immutable; existing codecs share this mutation boundary.
struct FStructMutation {
    FInstancedStruct Owned;
    FUnrealDasContext::FStructEdit* Draft;
    FDasStructValue Handle;
    FInstancedStruct& Value;
    static FUnrealDasContext::FStructEdit* Begin(const FDasStructValue& H,das::Context* C){
        if(C!=ActiveContext || !ActiveContext)C->throw_error("struct requires active context");
        auto* Found=ActiveContext->StructEdits.Find(H.Token);
        if(!Found)return nullptr;
        auto* Draft=Found->Get();
        if(Draft->Poisoned)C->throw_error("snapshot edit was invalidated by a failed write");
        // A codec may insert a map key before it rejects the value. Never publish
        // that partial write, even if the script catches the error inside the block.
        Draft->Poisoned=true;
        return Draft;
    }
    FStructMutation(const FDasStructValue& H,das::Context* C)
        :Draft(Begin(H,C)),Handle(H),Value(Draft?Draft->Value:Owned){
        if(!Draft)Owned=ResolveStructValue(H,C);
    }
    FDasStructValue Finish(das::Context* C,das::LineInfoArg* At){
        if(Draft){Draft->Poisoned=false;return Handle;}
        return StoreStructValue(MoveTemp(Owned),C,At);
    }
};

FDasStructValue EditStructValue(const FDasStructValue& Source,const das::TBlock<void,FDasStructValue>& Block,das::Context* C,das::LineInfoArg* At){
    if(C!=ActiveContext || !ActiveContext || !IsInGameThread())C->throw_error("snapshot edit requires active game-thread context");
    if(ActiveContext->StructEdits.Num()>=8)C->throw_error("snapshot edit nesting limit exceeded");
    if(ActiveContext->StructEdits.Contains(Source.Token))C->throw_error("cannot start another edit of an active snapshot draft");
    const auto& Original=ResolveStructValue(Source,C);
    auto State=MakeUnique<FUnrealDasContext::FStructEdit>();
    State->Value=Original; // The single deep copy for this block.
    const uint64 Token=NextStructValueToken++;
    ActiveContext->StructEdits.Add(Token,MoveTemp(State));
    FDasStructValue Argument{Token},Result{};
    // Managed UEStructValue block arguments use the same by-reference ABI as function frames.
    vec4f Args[]={das::cast<void*>::from(&Argument)};
    const bool Good=C->runWithCatch([&](){
        C->invoke(Block,Args,nullptr,At);
        auto& Draft=*ActiveContext->StructEdits.FindChecked(Token);
        if(Draft.Poisoned)C->throw_error("snapshot edit was invalidated by a failed write");
        Result=StoreStructValue(MoveTemp(Draft.Value),C,At);
    });
    // Explicit cleanup before rethrow, including errors from validation/publication.
    ActiveContext->StructEdits.Remove(Token);
    if(!Good)C->rethrow();
    return Result;
}
