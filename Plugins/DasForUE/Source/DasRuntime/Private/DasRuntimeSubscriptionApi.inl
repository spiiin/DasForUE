// Part of DasRuntime.cpp: Event subscription externs and forward declarations of managed gameplay services.
// Included in place; declaration order matters.
void SubscribeObjectValue(const FDasObjectHandle& Handle, const char* Event, const char* Handler, const char* Path,
                          das::Context* Context, EDasMethodSignature Kind)
{
    RequireObjectContext(Context);
    const FString Expected = Path ? UTF8_TO_TCHAR(Path) : TEXT("");
    FString Error;
    if (!ValidateDasCallbackTypePath(Kind == EDasMethodSignature::Class ? TEXT("class")
                                                                        : Kind == EDasMethodSignature::Enum
                                     ? TEXT("enum")
                                     : TEXT("struct"), Expected, Error))
        Context->throw_error(TCHAR_TO_UTF8(*Error));
    SubscribeObjectScalar(Handle, Event, Handler, Context, Kind);
    auto& Declaration = ActiveContext->Subscriptions.Last();
    auto* P = ScalarEventParameter(ScalarEventProperty(ResolveObject(Handle, Context), Declaration.Event, Kind));
    const FString Actual = FindDasCallbackFrame(Expected) ? Expected
                           : Kind == EDasMethodSignature::Class
                               ? CastFieldChecked<FClassProperty>(P)->MetaClass->GetPathName()
                           : Kind == EDasMethodSignature::Enum ? DasEnumPropertyType(P)->GetPathName()
                                                               : DasCallbackSnapshotForProperty(P)->GetPathName();
    if (Actual != Expected &&
        !(Kind == EDasMethodSignature::Class &&
          FindObject<UClass>(nullptr, *Actual)->IsChildOf(FindObject<UClass>(nullptr, *Expected))))
    {
        ActiveContext->Subscriptions.Pop();
        Context->throw_error("Event and callback require the same concrete enum/struct type");
    }
    TStrongObjectPtr<UDasIntDelegateProxy> ReceiverCheck(NewObject<UDasIntDelegateProxy>());
    auto* Signature =
        ScalarEventProperty(ResolveObject(Handle, Context), Declaration.Event, Kind)->SignatureFunction.Get();
    if (ReceiverCheck->ConfigureDynamic(Signature, Expected, Kind, Error).IsNone())
    {
        ActiveContext->Subscriptions.Pop();
        Context->throw_error(TCHAR_TO_UTF8(*Error));
    }
    Declaration.PayloadPath = Expected;
}
void SubscribeObjectClass(const FDasObjectHandle& H, const char* E, const char* F, const char* P, das::Context* C)
{
    SubscribeObjectValue(H, E, F, P, C, EDasMethodSignature::Class);
}
void SubscribeObjectEnum(const FDasObjectHandle& H, const char* E, const char* F, const char* P, das::Context* C)
{
    SubscribeObjectValue(H, E, F, P, C, EDasMethodSignature::Enum);
}
void SubscribeObjectStruct(const FDasObjectHandle& H, const char* E, const char* F, const char* P, das::Context* C)
{
    SubscribeObjectValue(H, E, F, P, C, EDasMethodSignature::Struct);
}
void SubscribeObjectByte(const FDasObjectHandle& H, const char* Event, const char* Handler, das::Context* C)
{
    SubscribeObjectScalar(H, Event, Handler, C, EDasMethodSignature::Byte);
}
void SubscribeObjectArrayFloat(const FDasObjectHandle& H, const char* Event, const char* Handler, das::Context* C)
{
    SubscribeObjectScalar(H, Event, Handler, C, EDasMethodSignature::Struct, FDasArrayFloatCarrier::StaticStruct());
}
void SubscribeObjectArrayBool(const FDasObjectHandle& H, const char* Event, const char* Handler, das::Context* C)
{
    SubscribeObjectScalar(H, Event, Handler, C, EDasMethodSignature::Struct, FDasArrayBoolCarrier::StaticStruct());
}
void SubscribeObjectArrayName(const FDasObjectHandle& H, const char* Event, const char* Handler, das::Context* C)
{
    SubscribeObjectScalar(H, Event, Handler, C, EDasMethodSignature::Struct, FDasArrayNameCarrier::StaticStruct());
}
void SubscribeObjectArrayByte(const FDasObjectHandle& H, const char* Event, const char* Handler, das::Context* C)
{
    SubscribeObjectScalar(H, Event, Handler, C, EDasMethodSignature::Struct, FDasArrayByteCarrier::StaticStruct());
}
void SubscribeObjectText(const FDasObjectHandle& H, const char* Event, const char* Handler, das::Context* C)
{
    SubscribeObjectScalar(H, Event, Handler, C, EDasMethodSignature::Text);
}
void SubscribeObjectArrayText(const FDasObjectHandle& H, const char* Event, const char* Handler, das::Context* C)
{
    SubscribeObjectScalar(H, Event, Handler, C, EDasMethodSignature::Struct, FDasArrayTextCarrier::StaticStruct());
}
void SubscribeObjectInt(const FDasObjectHandle& H, const char* Event, const char* Handler, das::Context* C)
{
    SubscribeObjectScalar(H, Event, Handler, C, EDasMethodSignature::Int);
}
void SubscribeObjectFloat(const FDasObjectHandle& H, const char* Event, const char* Handler, das::Context* C)
{
    SubscribeObjectScalar(H, Event, Handler, C, EDasMethodSignature::Float);
}
void SubscribeObjectBool(const FDasObjectHandle& H, const char* Event, const char* Handler, das::Context* C)
{
    SubscribeObjectScalar(H, Event, Handler, C, EDasMethodSignature::Bool);
}
void SubscribeObjectString(const FDasObjectHandle& H, const char* Event, const char* Handler, das::Context* C)
{
    SubscribeObjectScalar(H, Event, Handler, C, EDasMethodSignature::String);
}
void SubscribeObjectVoid(const FDasObjectHandle& H, const char* Event, const char* Handler, das::Context* C)
{
    SubscribeObjectScalar(H, Event, Handler, C, EDasMethodSignature::Void);
}

void SubscribeObjectDouble(const FDasObjectHandle& H, const char* Event, const char* Handler, das::Context* C)
{
    SubscribeObjectScalar(H, Event, Handler, C, EDasMethodSignature::Double);
}
void SubscribeObjectObject(const FDasObjectHandle& H, const char* Event, const char* Handler, das::Context* C)
{
    SubscribeObjectScalar(H, Event, Handler, C, EDasMethodSignature::Object);
    ActiveContext->Subscriptions.Last().PayloadPath = TEXT("/Script/CoreUObject.Object");
}
void SubscribeObjectTyped(const FDasObjectHandle& H, const char* Event, const char* Handler, const char* Path,
                          das::Context* C)
{
    FString Error;
    const FString Expected = Path ? UTF8_TO_TCHAR(Path) : TEXT("");
    if (!ValidateDasCallbackTypePath(TEXT("object"), Expected, Error))
        C->throw_error(TCHAR_TO_UTF8(*Error));
    SubscribeObjectScalar(H, Event, Handler, C, EDasMethodSignature::Object);
    auto& D = ActiveContext->Subscriptions.Last();
    auto* Property = ScalarEventProperty(ResolveObject(H, C), D.Event, D.Kind);
    auto* Actual = DasReferencePropertyClass(ScalarEventParameter(Property));
    TStrongObjectPtr<UDasIntDelegateProxy> Probe(NewObject<UDasIntDelegateProxy>());
    if (!Actual || Probe->ConfigureDynamic(Property->SignatureFunction, Actual->GetPathName(), D.Kind, Error).IsNone())
    {
        ActiveContext->Subscriptions.Pop();
        C->throw_error(TCHAR_TO_UTF8(*Error));
    }
    D.PayloadPath = Expected;
}
void SubscribeObjectVector(const FDasObjectHandle& H, const char* Event, const char* Handler, das::Context* C)
{
    SubscribeObjectScalar(H, Event, Handler, C, EDasMethodSignature::Vector);
}

void SubscribeObjectRotator(const FDasObjectHandle& H, const char* Event, const char* Handler, das::Context* C)
{
    SubscribeObjectScalar(H, Event, Handler, C, EDasMethodSignature::Rotator);
}
void SubscribeObjectTransform(const FDasObjectHandle& H, const char* Event, const char* Handler, das::Context* C)
{
    SubscribeObjectScalar(H, Event, Handler, C, EDasMethodSignature::Transform);
}

void SubscribeObjectName(const FDasObjectHandle& H, const char* Event, const char* Handler, das::Context* C)
{
    SubscribeObjectScalar(H, Event, Handler, C, EDasMethodSignature::Name);
}
void SubscribeObjectArrayInt(const FDasObjectHandle& H, const char* Event, const char* Handler, das::Context* C)
{
    SubscribeObjectScalar(H, Event, Handler, C, EDasMethodSignature::Struct, FDasArrayIntCarrier::StaticStruct());
}
void SubscribeObjectArrayDouble(const FDasObjectHandle& H, const char* Event, const char* Handler, das::Context* C)
{
    SubscribeObjectScalar(H, Event, Handler, C, EDasMethodSignature::Struct, FDasArrayDoubleCarrier::StaticStruct());
}
void SubscribeObjectArrayString(const FDasObjectHandle& H, const char* Event, const char* Handler, das::Context* C)
{
    SubscribeObjectScalar(H, Event, Handler, C, EDasMethodSignature::Struct, FDasArrayStringCarrier::StaticStruct());
}

void SubscribeObjectVector2d(const FDasObjectHandle& H, const char* E, const char* F, das::Context* C)
{
    SubscribeObjectScalar(H, E, F, C, EDasMethodSignature::Struct, TBaseStructure<FVector2D>::Get());
}
FName EventProxyFunction(EDasMethodSignature K, FMulticastDelegateProperty* Property)
{
    auto* Arg = ScalarEventParameter(Property);
    const auto* Type =
        K == EDasMethodSignature::Void ? FindDasCallbackType(TEXT("void")) : DasCallbackTypeForProperty(Arg);
    if (!Type || Type->Signature != K)
        return NAME_None;
    return Arg && Arg->HasAnyPropertyFlags(CPF_OutParm | CPF_ReferenceParm) ? Type->ConstRefProxy : Type->ValueProxy;
}
bool VerifyEventHandler(das::SimFunction* F, das::ModuleGroup& G, EDasMethodSignature K)
{
    if (!F)
        return false;
    switch (K)
    {
    case EDasMethodSignature::Class:
        return das::verifyCall<void, FDasClassHandle>(F->debugInfo, G);
    case EDasMethodSignature::Enum:
        return das::verifyCall<void, FDasEnumValue>(F->debugInfo, G);
    case EDasMethodSignature::Byte:
        return das::verifyCall<void, uint8_t>(F->debugInfo, G);
    case EDasMethodSignature::Text:
        return das::verifyCall<void, FDasTextValue>(F->debugInfo, G);
    case EDasMethodSignature::Name:
        return das::verifyCall<void, FDasNameValue>(F->debugInfo, G);
    case EDasMethodSignature::Struct:
        return das::verifyCall<void, FDasStructValue>(F->debugInfo, G);
    case EDasMethodSignature::Double:
        return das::verifyCall<void, double>(F->debugInfo, G);
    case EDasMethodSignature::Object:
        return das::verifyCall<void, FDasObjectHandle>(F->debugInfo, G);
    case EDasMethodSignature::Rotator:
        return das::verifyCall<void, FRotator>(F->debugInfo, G);
    case EDasMethodSignature::Transform:
        return das::verifyCall<void, FTransform>(F->debugInfo, G);
    case EDasMethodSignature::Vector:
        return das::verifyCall<void, FVector>(F->debugInfo, G);
    case EDasMethodSignature::Float:
        return das::verifyCall<void, float>(F->debugInfo, G);
    case EDasMethodSignature::Bool:
        return das::verifyCall<void, bool>(F->debugInfo, G);
    case EDasMethodSignature::String:
        return das::verifyCall<void, char*>(F->debugInfo, G);
    case EDasMethodSignature::Void:
        return das::verifyCall<void>(F->debugInfo, G);
    default:
        return das::verifyCall<void, int32>(F->debugInfo, G);
    }
}

void SubscribeInt(const char* SourceKind, const char* Event, const char* Handler, das::Context* Context)
{
    RequireObjectContext(Context);
    if (!SourceKind)
        Context->throw_error("subscription source is required");
    UObject* Source = ResolveSelf(Context);
    const FString Kind(UTF8_TO_TCHAR(SourceKind));
    if (Kind == TEXT("peer"))
    {
        auto* Actor = Cast<ADasProbeActor>(Source);
        Source = Actor ? Actor->Peer.Get() : nullptr;
    }
    else if (Kind != TEXT("self"))
        Context->throw_error("subscription source must be self or peer");
    SubscribeObjectInt(MakeObjectHandle(Source, UObject::StaticClass(), Context), Event, Handler, Context);
}

void EmitSignal(int32 Value, das::Context* Context)
{
    if (ConstructingObject || (ActiveContext && ActiveContext->bCollectingSubscriptions))
        Context->throw_error("event emission is forbidden in bind_events/ConstructionScript");
    auto* Actor = Cast<ADasProbeActor>(ResolveSelf(Context));
    if (!Actor)
        Context->throw_error("signal emission requires a DasProbeActor");
    Actor->EmitDasSignal(Value);
}

void ScriptEnhancedAddContext(const FDasObjectHandle&, const FDasObjectHandle&, int32, das::Context*);
bool ScriptEnhancedRemoveContext(const FDasObjectHandle&, const FDasObjectHandle&, das::Context*);
bool ScriptEnhancedHasContext(const FDasObjectHandle&, const FDasObjectHandle&, das::Context*);
int32 ScriptEnhancedClearContexts(das::Context*);
void ScriptEnhancedBind(const FDasObjectHandle&, const char*, const char*, das::Context*);
bool ScriptEnhancedUnbind(const FDasObjectHandle&, const char*, das::Context*);
bool ScriptEnhancedAvailable(das::Context*);
void ScriptInputAction(const char*, const char*, const char*, bool, bool, das::Context*);
void ScriptInputAxis(const char*, const char*, bool, bool, das::Context*);
bool ScriptInputAvailable(das::Context*);
bool ScriptInputUnbindAction(const char*, const char*, das::Context*);
bool ScriptInputUnbindAxis(const char*, das::Context*);
int32 ScriptInputUnbindAll(das::Context*);
FDasStructValue GameplayCheck(const FDasStructValue&, bool, das::Context*);
FDasStructValue GameplayRequest(const char*, bool, das::Context*);
bool GameplayValid(const FDasStructValue&, das::Context*);
char* GameplayName(const FDasStructValue&, das::Context*);
bool GameplayMatches(const FDasStructValue&, const FDasStructValue&, bool, das::Context*);
FDasStructValue GameplayWith(const FDasStructValue&, const FDasStructValue&, bool, das::Context*, das::LineInfoArg*);
int32 GameplayCount(const FDasStructValue&, das::Context*);
bool GameplayHas(const FDasStructValue&, const FDasStructValue&, bool, das::Context*);
bool GameplayHasSet(const FDasStructValue&, const FDasStructValue&, bool, bool, das::Context*);
FDasStructValue GameplayNames(const FDasStructValue&, das::Context*, das::LineInfoArg*);
bool ScriptCurveTableHas(const FDasObjectHandle&, const char*, das::Context*);
FDasStructValue ScriptCurveTableNames(const FDasObjectHandle&, das::Context*, das::LineInfoArg*);
float ScriptCurveTableEval(const FDasObjectHandle&, const char*, float, float, das::Context*);
bool ScriptDataTableHas(const FDasObjectHandle&, const char*, das::Context*);
FDasStructValue ScriptDataTableNames(const FDasObjectHandle&, das::Context*, das::LineInfoArg*);
FDasStructValue ScriptDataTableRow(const FDasObjectHandle&, const char*, const char*, das::Context*, das::LineInfoArg*);
uint64 ScriptPrimaryFind(const char*, das::Context*);
int32 ScriptPrimaryQueryCount(uint64, das::Context*);
const char* ScriptPrimaryQueryName(uint64, int32, das::Context*);
void ScriptPrimaryQueryRelease(uint64, das::Context*);
bool ScriptPrimaryExists(const char*, const char*, das::Context*);
const char* ScriptPrimaryPath(const char*, const char*, das::Context*);
FDasObjectHandle ScriptPrimaryObject(const char*, const char*, das::Context*);
void ScriptPrimaryLoad(const char*, const char*, const char*, const das::TArray<char*>&, const char*, das::Context*);
bool ScriptPrimaryPending(const char*, das::Context*);
bool ScriptPrimaryCancel(const char*, das::Context*);
bool ScriptPrimaryUnload(const char*, const char*, das::Context*);
FDasObjectHandle ScriptSaveGameCreate(const FDasClassHandle&, das::Context*);
FDasObjectHandle ScriptSaveGameCreatePath(const char*, das::Context*);
bool ScriptSaveGameSave(const FDasObjectHandle&, const char*, int32, das::Context*);
FDasObjectHandle ScriptSaveGameLoad(const char*, int32, das::Context*);
bool ScriptSaveGameExists(const char*, int32, das::Context*);
bool ScriptSaveGameDelete(const char*, int32, das::Context*);
bool ScriptSaveGameRelease(const FDasObjectHandle&, das::Context*);
bool ScriptSaveGameValid(const FDasObjectHandle&, das::Context*);
void ScriptSaveGameSaveAsync(const char*, const FDasObjectHandle&, const char*, int32, const char*, das::Context*);
void ScriptSaveGameLoadAsync(const char*, const char*, int32, const char*, das::Context*);
bool ScriptSaveGameCancel(const char*, das::Context*);
bool ScriptSaveGamePending(const char*, das::Context*);
void ScriptAssetLoad(const char*, const char*, const char*, const char*, das::Context*);
void ScriptAssetLoadGeneric(const char*, const char*, const char*, das::Context*);
bool ScriptAssetCancel(const char*, das::Context*);
int32 ScriptAssetCancelAll(das::Context*);
bool ScriptAssetPending(const char*, das::Context*);
void ScriptLatentDelay(const char*, float, const char*, das::Context*);
FDasObjectHandle ScriptAISelf(das::Context*);
bool ScriptAIMove(const char*, const FDasObjectHandle&, const FDasObjectHandle&, const FVector&, bool, const char*,
                  float, bool, bool, das::Context*);
bool ScriptAICancel(const char*, das::Context*);
bool ScriptAIActive(const char*, das::Context*);
bool ScriptEffectPlay(const char*, const FDasObjectHandle&, const FDasObjectHandle&, const FTransform&, const char*,
                      float, float, das::Context*);
bool ScriptEffectStop(const char*, das::Context*);
int32 ScriptEffectStopAll(das::Context*);
bool ScriptEffectActive(const char*, das::Context*);
float ScriptMontagePlay(const char*, const FDasObjectHandle&, const FDasObjectHandle&, const char*, float, float,
                        FDasNameValue, das::Context*);
bool ScriptMontageStop(const char*, float, das::Context*);
bool ScriptMontageCancel(const char*, das::Context*);
bool ScriptMontagePlaying(const char*, das::Context*);
bool ScriptLatentCancel(const char*, das::Context*);
int32 ScriptLatentCancelAll(das::Context*);
bool ScriptLatentPending(const char*, das::Context*);
void ScriptTimerStart(const char*, float, const char*, bool, das::Context*);
void ScriptTimerStartVector2d(const char*, float, const char*, bool, const FVector2D&, das::Context*);
void ScriptTimerNextTickVector2d(const char*, const char*, const FVector2D&, das::Context*);
void ScriptTimerStartArray(const char*, float, const char*, bool, const FDasStructValue&, das::Context*);
void ScriptTimerStartClass(const char*, float, const char*, bool, FDasClassHandle, const char*, das::Context*);
void ScriptTimerNextTickClass(const char*, const char*, FDasClassHandle, const char*, das::Context*);
void ScriptTimerStartObject(const char*, float, const char*, bool, const FDasObjectHandle&, const char*, das::Context*);
void ScriptTimerNextTickObject(const char*, const char*, const FDasObjectHandle&, const char*, das::Context*);
void ScriptTimerStartEnum(const char*, float, const char*, bool, FDasEnumValue, das::Context*);
void ScriptTimerNextTickEnum(const char*, const char*, FDasEnumValue, das::Context*);
void ScriptTimerStartVector(const char*, float, const char*, bool, const FVector&, das::Context*);
void ScriptTimerStartRotator(const char*, float, const char*, bool, const FRotator&, das::Context*);
void ScriptTimerStartTransform(const char*, float, const char*, bool, const FTransform&, das::Context*);
void ScriptTimerNextTick(const char*, const char*, das::Context*);
void ScriptTimerNextTickArray(const char*, const char*, const FDasStructValue&, das::Context*);
bool ScriptTimerPaused(const char*, das::Context*);
float ScriptTimerElapsed(const char*, das::Context*);
int32 ScriptTimerCancelAll(das::Context*);
void ScriptTimerNextTickVector(const char*, const char*, const FVector&, das::Context*);
void ScriptTimerNextTickRotator(const char*, const char*, const FRotator&, das::Context*);
void ScriptTimerNextTickTransform(const char*, const char*, const FTransform&, das::Context*);
bool ScriptTimerCancel(const char*, das::Context*);
bool ScriptTimerPause(const char*, das::Context*);
bool ScriptTimerResume(const char*, das::Context*);
bool ScriptTimerActive(const char*, das::Context*);
float ScriptTimerRemaining(const char*, das::Context*);
