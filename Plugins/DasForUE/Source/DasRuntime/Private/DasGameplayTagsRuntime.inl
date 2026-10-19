FDasStructValue GameplayCheck(const FDasStructValue& H,bool Container,das::Context* C){
 RequireObjectContext(C);auto* Type=Container?FGameplayTagContainer::StaticStruct():FGameplayTag::StaticStruct();
 if(!H.Token){FInstancedStruct V;V.InitializeAs(Type);return StoreStructValue(MoveTemp(V),C);}
 const auto& V=ResolveStructValue(H,C);FString Error;if(V.GetScriptStruct()!=Type || !ValidateDasGameplayTagValue(Type,V.GetMemory(),Error))C->throw_error("GameplayTags type/context/value mismatch");return H;
}
const FInstancedStruct& GameplayValue(const FDasStructValue& H,bool Container,das::Context* C){return ResolveStructValue(GameplayCheck(H,Container,C),C);}
FDasStructValue GameplayRequest(const char* Name,bool Required,das::Context* C){
 RequireObjectContext(C);const FString Text=Name?UTF8_TO_TCHAR(Name):TEXT("");
 if(Text.Len()>1024 || (!Text.IsEmpty() && !FGameplayTag::IsValidGameplayTagString(Text)))C->throw_error("invalid GameplayTag name");
 const auto Tag=Text.IsEmpty()?FGameplayTag():FGameplayTag::RequestGameplayTag(FName(*Text),false);
 if(Required && !Text.IsEmpty() && !Tag.IsValid())C->throw_error("GameplayTag is not registered; add it to the project's dictionary");
 FInstancedStruct V;V.InitializeAs<FGameplayTag>(Tag);return StoreStructValue(MoveTemp(V),C);
}
bool GameplayValid(const FDasStructValue& H,das::Context* C){return GameplayValue(H,false,C).Get<FGameplayTag>().IsValid();}
char* GameplayName(const FDasStructValue& H,das::Context* C){return CopyDSLString(GameplayValue(H,false,C).Get<FGameplayTag>().ToString(),C);}
bool GameplayMatches(const FDasStructValue& H,const FDasStructValue& Parent,bool Exact,das::Context* C){const auto Tag=GameplayValue(H,false,C).Get<FGameplayTag>();const auto Query=GameplayValue(Parent,false,C).Get<FGameplayTag>();return Exact?Tag.MatchesTagExact(Query):Tag.MatchesTag(Query);}
FDasStructValue GameplayWith(const FDasStructValue& H,const FDasStructValue& Tag,bool Add,das::Context* C,das::LineInfoArg* At){
 auto Tags=GameplayValue(H,true,C).Get<FGameplayTagContainer>();const auto Value=GameplayValue(Tag,false,C).Get<FGameplayTag>();if(!Value.IsValid())C->throw_error("cannot add/remove an empty GameplayTag");
 if(Add)Tags.AddTag(Value);else Tags.RemoveTag(Value);FInstancedStruct V;V.InitializeAs<FGameplayTagContainer>(Tags);return StoreStructValue(MoveTemp(V),C,At);
}
int32 GameplayCount(const FDasStructValue& H,das::Context* C){return GameplayValue(H,true,C).Get<FGameplayTagContainer>().Num();}
bool GameplayHas(const FDasStructValue& H,const FDasStructValue& Tag,bool Exact,das::Context* C){const auto& Tags=GameplayValue(H,true,C).Get<FGameplayTagContainer>();const auto Value=GameplayValue(Tag,false,C).Get<FGameplayTag>();return Exact?Tags.HasTagExact(Value):Tags.HasTag(Value);}
bool GameplayHasSet(const FDasStructValue& H,const FDasStructValue& Query,bool All,bool Exact,das::Context* C){const auto& Tags=GameplayValue(H,true,C).Get<FGameplayTagContainer>();const auto& Values=GameplayValue(Query,true,C).Get<FGameplayTagContainer>();return All?(Exact?Tags.HasAllExact(Values):Tags.HasAll(Values)):(Exact?Tags.HasAnyExact(Values):Tags.HasAny(Values));}
FDasStructValue GameplayNames(const FDasStructValue& H,das::Context* C,das::LineInfoArg* At){
 const auto& Tags=GameplayValue(H,true,C).Get<FGameplayTagContainer>();FInstancedStruct V;V.InitializeAs<FDasArrayStringCarrier>();auto& Names=V.GetMutable<FDasArrayStringCarrier>().Values;for(const auto& Tag:Tags.GetGameplayTagArray())Names.Add(Tag.ToString());Names.Sort();return StoreStructValue(MoveTemp(V),C,At);
}
