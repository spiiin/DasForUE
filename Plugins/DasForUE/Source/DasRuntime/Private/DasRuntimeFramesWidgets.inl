// Part of DasRuntime.cpp: Call-frame publication of math/name values and widget paint/reply/drag scopes.
// Included in place; declaration order matters.
void PublishFrameName(int32 I, FDasNameValue X, das::Context* C)
{
    FDasScalarValue V;
    V.Kind = 7;
    V.NameValue = ResolveDSLName(X, C);
    PublishFrameValue(I, MoveTemp(V), C);
}
void PublishFrameStruct(int32 I, const FDasStructValue& X, das::Context* C)
{
    FDasScalarValue V;
    V.Kind = 12;
    V.StructValue = ResolveStructValue(X, C);
    PublishFrameValue(I, MoveTemp(V), C);
}
void PublishFrameObject(int32 I, const FDasObjectHandle& X, das::Context* C)
{
    FDasScalarValue V;
    V.Kind = 8;
    V.ObjectValue = X.Token ? ResolveObject(X, C) : nullptr;
    PublishFrameValue(I, MoveTemp(V), C);
}
void PublishFrameClass(int32 I, FDasClassHandle X, das::Context* C)
{
    FDasScalarValue V;
    V.Kind = 15;
    if (X.Token)
    {
        auto* O = ResolveObject(FDasObjectHandle{X.Token}, C);
        V.ClassValue = Cast<UClass>(O);
        if (!V.ClassValue)
            C->throw_error("expected class capability");
    }
    PublishFrameValue(I, MoveTemp(V), C);
}
void PublishFrameEnum(int32 I, FDasEnumValue X, das::Context* C)
{
    const auto& Value = ResolveEnumValue(X, C);
    FDasScalarValue V;
    V.Kind = 13;
    V.ObjectClass = Value.Type->GetPathName();
    V.IntValue = Value.Value;
    PublishFrameValue(I, MoveTemp(V), C);
}
void PublishFrameVector(int32 I, const FVector& X, das::Context* C)
{
    FDasScalarValue V;
    V.Kind = 9;
    V.VectorValue = X;
    PublishFrameValue(I, MoveTemp(V), C);
}
void PublishFrameRotator(int32 I, const FRotator& X, das::Context* C)
{
    FDasScalarValue V;
    V.Kind = 10;
    V.RotatorValue = X;
    PublishFrameValue(I, MoveTemp(V), C);
}
void PublishFrameTransform(int32 I, const FTransform& X, das::Context* C)
{
    FDasScalarValue V;
    V.Kind = 11;
    V.TransformValue = X;
    PublishFrameValue(I, MoveTemp(V), C);
}
FDelegateHandle RuntimePreExitHandle;
thread_local bool InputSnapshotDispatch = false;
thread_local const FString* GASCalculationContract = nullptr;
thread_local bool GeometrySnapshotDispatch = false;
uint64 NextPaintEpoch = 1;
struct FPaintCommand
{
    TStrongObjectPtr<UFunction> Function;
    TUniquePtr<FStructOnScope> Arguments;
    TArray<TStrongObjectPtr<UObject>> Pins;
};
struct FWidgetPaintScope
{
    uint64 Epoch = NextPaintEpoch++;
    das::Context* Context = nullptr;
    TWeakObjectPtr<UDasScriptUserWidget> Widget;
    FPaintContext* Native = nullptr;
    TArray<FPaintCommand> Commands;
    int32 Points = 0, TextChars = 0;
    bool Succeeded = false;
};
thread_local FWidgetPaintScope* ActiveWidgetPaint = nullptr;
thread_local bool WidgetPaintContract = false;
uint64 NextWidgetReplyEpoch = 1;
struct FWidgetReplyScope
{
    uint64 Epoch = NextWidgetReplyEpoch++;
    das::Context* Context = nullptr;
    TWeakObjectPtr<UDasScriptUserWidget> Widget;
    TMap<uint64, FReply> Replies;
    TArray<TWeakObjectPtr<UWidget>> Targets;
    FReply Result = FReply::Unhandled();
    bool Produced = false;
    uint64 PointerToken = 0;
    uint64 NavigationInputToken = 0;
    const UScriptStruct* NavigationInputType = nullptr;
    uint32 NavigationUserIndex = MAX_uint32;
    TWeakPtr<FSlateUser> NavigationUser;
    bool NavigationRevoked = false, NavigationConsumedAnalog = false;
    TMap<FName, int64> NavigationResults;
};
thread_local FWidgetReplyScope* ActiveWidgetReply = nullptr;
thread_local bool WidgetReplyContract = false;
thread_local bool FocusNotificationContract = false;
uint64 NextWidgetReplyToken = 1;
struct FWidgetDragScope
{
    FName Event;
    uint32 UserIndex = 0, PointerIndex = 0;
    das::Context* Context = nullptr;
    TWeakObjectPtr<UDasScriptUserWidget> Widget;
    TWeakObjectPtr<UDasDragDropOperation> Operation;
    TStrongObjectPtr<UDasDragDropOperation> Created;
    bool Succeeded = false;
    ~FWidgetDragScope()
    {
        if (Created && !Succeeded)
            Created->Revoke();
    }
};
thread_local FWidgetDragScope* ActiveWidgetDrag = nullptr;
thread_local bool WidgetDragContract = false;
const char* WidgetEventHandleName(const FString& Path)
{
    if (Path == TEXT("/Script/SlateCore.FocusEvent"))
        return "UEFocusEvent";
    if (Path == TEXT("/Script/SlateCore.KeyEvent"))
        return "UEKeyEvent";
    if (Path == TEXT("/Script/SlateCore.AnalogInputEvent"))
        return "UEAnalogInputEvent";
    if (Path == TEXT("/Script/SlateCore.CharacterEvent"))
        return "UECharacterEvent";
    if (Path == TEXT("/Script/SlateCore.PointerEvent"))
        return "UEPointerEvent";
    return nullptr;
}
// The math structs of typed calls from C++ (DasScript.h); their script types belong to the struct adapters.
const UScriptStruct* DasTypedCallStruct(EDasMethodSignature Kind)
{
    switch (Kind)
    {
    case EDasMethodSignature::Vector2D:
        return TBaseStructure<FVector2D>::Get();
    case EDasMethodSignature::Quat:
        return TBaseStructure<FQuat>::Get();
    case EDasMethodSignature::LinearColor:
        return TBaseStructure<FLinearColor>::Get();
    case EDasMethodSignature::Color:
        return TBaseStructure<FColor>::Get();
    default:
        return nullptr;
    }
}
// The script facet of a typed call's math struct, or nullptr.
const IDasScriptTypeFacet* DasTypedCallFacet(EDasMethodSignature Kind)
{
    if (const UScriptStruct* Struct = DasTypedCallStruct(Kind))
        if (const auto* Adapter = FDasTypeAdapters::Get().FindStruct(Struct))
            for (const auto& Entry : FDasTypeAdapters::Get().Entries())
                if (&*Entry.Adapter == Adapter)
                    return Entry.Script;
    return nullptr;
}
bool VerifyContract(const das::ProgramPtr& Program, const FString& Entry, const FDasCallContract& C,
                    bool AllowNativeInputValue = false)
{
    if (C.Inputs.Num() > 16 ||
        ((static_cast<uint8>(C.ReturnKind) > 13 && C.ReturnKind != EDasMethodSignature::Class &&
          C.ReturnKind != EDasMethodSignature::Delegate && C.ReturnKind != EDasMethodSignature::Byte &&
          C.ReturnKind != EDasMethodSignature::Text && !DasIsTypedCallKind(C.ReturnKind)) ||
         C.ReturnKind == EDasMethodSignature::Frame))
        return false;
    const FTCHARToUTF8 N(*Entry);
    auto* F = Program->getThisModule()->findUniqueFunction(N.Get());
    if (WidgetPaintContract && C.ReturnKind == EDasMethodSignature::Void && C.Inputs.Num() == 1 &&
        C.Inputs[0] == EDasMethodSignature::Struct && C.InputClasses.Num() == 1 &&
        C.InputClasses[0] == TEXT("/Script/UMG.PaintContext"))
    {
        if (!F || !F->exports || !C.InputDirections.IsEmpty() || F->arguments.size() != 1 || !F->result ||
            F->result->ref || F->result->baseType != das::Type::tVoid)
            return false;
        const auto& T = F->arguments[0]->type;
        return T && !T->ref && T->baseType == das::Type::tHandle && T->annotation &&
               T->annotation->name == "UEPaintContext";
    }
    if (WidgetDragContract && C.InputClasses.Contains(TEXT("/Script/SlateCore.PointerEvent")) &&
                                                      (C.ReturnKind == EDasMethodSignature::Object ||
                                                       C.ReturnKind == EDasMethodSignature::Bool ||
                                                       C.ReturnKind == EDasMethodSignature::Void))
    {
        auto Handle = [](const das::TypeDeclPtr& T, const char* Name)
        {
            return T && !T->ref && T->baseType == das::Type::tHandle && T->annotation && T->annotation->name == Name;
        };
        if (!F || !F->exports || !C.InputDirections.IsEmpty() || C.InputClasses.Num() != C.Inputs.Num() ||
            F->arguments.size() != size_t(C.Inputs.Num()))
            return false;
        const int32 Count = C.Inputs.Num();
        const bool HasGeometry = Count && C.InputClasses[0] == TEXT("/Script/SlateCore.Geometry");
        if ((Count != 2 && Count != 3) || (!HasGeometry && Count != 2))
            return false;
        int32 I = 0;
        if (HasGeometry)
        {
            if (C.Inputs[0] != EDasMethodSignature::Struct || !Handle(F->arguments[0]->type, "UEGeometry"))
                return false;
            ++I;
        }
        if (C.Inputs[I] != EDasMethodSignature::Struct ||
            C.InputClasses[I] != TEXT("/Script/SlateCore.PointerEvent") ||
                                      !Handle(F->arguments[I]->type, "UEPointerEvent"))
            return false;
        ++I;
        if (I < Count)
        {
            if (C.Inputs[I] != EDasMethodSignature::Object ||
                C.InputClasses[I] != TEXT("/Script/CoreUObject.Object") ||
                                          !Handle(F->arguments[I]->type, "UObjectHandle"))
                return false;
            return F->result && !F->result->ref &&
                   ((C.ReturnKind == EDasMethodSignature::Bool && F->result->isSimpleType(das::Type::tBool)) ||
                    (C.ReturnKind == EDasMethodSignature::Void && F->result->baseType == das::Type::tVoid));
        }
        return HasGeometry && Count == 2 && C.ReturnKind == EDasMethodSignature::Object &&
               C.ReturnClass == TEXT("/Script/CoreUObject.Object") && Handle(F->result, "UObjectHandle");
    }
    if (FocusNotificationContract && C.ReturnKind == EDasMethodSignature::Void && C.Inputs.Num() == 1 &&
        C.Inputs[0] == EDasMethodSignature::Struct && C.InputClasses.Num() == 1 &&
        C.InputClasses[0] == TEXT("/Script/SlateCore.FocusEvent"))
    {
        const auto T = F && F->arguments.size() == 1 ? F->arguments[0]->type : das::TypeDeclPtr();
        return F && F->exports && C.InputDirections.IsEmpty() && F->result && !F->result->ref &&
               F->result->baseType == das::Type::tVoid && T && !T->ref && T->baseType == das::Type::tHandle &&
               T->annotation && T->annotation->name == "UEFocusEvent";
    }
    if (WidgetReplyContract && C.ReturnKind == EDasMethodSignature::Struct &&
        C.ReturnClass == TEXT("/Script/UMG.EventReply"))
    {
        auto Handle = [](const das::TypeDeclPtr& T, const char* Name)
        {
            return Name && T && !T->ref && T->baseType == das::Type::tHandle && T->annotation &&
                   T->annotation->name == Name;
        };
        return F && F->exports && C.InputDirections.IsEmpty() && C.Inputs.Num() == 2 &&
               C.Inputs[0] == EDasMethodSignature::Struct && C.Inputs[1] == EDasMethodSignature::Struct &&
               C.InputClasses.Num() == 2 &&
               C.InputClasses[0] == TEXT("/Script/SlateCore.Geometry") && F->arguments.size() == 2 &&
                                         Handle(F->result, "UEEventReply") &&
                                         Handle(F->arguments[0]->type, "UEGeometry") &&
                                         Handle(F->arguments[1]->type, WidgetEventHandleName(C.InputClasses[1]));
    }
    if (AllowNativeInputValue && F && F->exports && C.ReturnKind == EDasMethodSignature::Void && C.Inputs.Num() == 1 &&
        C.Inputs[0] == EDasMethodSignature::Struct && C.InputClasses.Num() == 1 &&
        C.InputClasses[0] == TEXT("/Script/EnhancedInput.InputActionValue") && C.InputDirections.IsEmpty() &&
                                  F->arguments.size() == 1 && F->result && !F->result->ref &&
                                  F->result->baseType == das::Type::tVoid)
    {
        const auto& T = F->arguments[0]->type;
        if (T && !T->ref && T->baseType == das::Type::tHandle && T->annotation &&
            T->annotation->name == "InputActionValue")
            return true;
    }
    auto Matches = [](const das::TypeDeclPtr& T, EDasMethodSignature K)
    {
        if (!T || T->ref ||
            ((static_cast<uint8>(K) > 13 && K != EDasMethodSignature::Class && K != EDasMethodSignature::Delegate &&
              K != EDasMethodSignature::Byte && K != EDasMethodSignature::Text && !DasIsTypedCallKind(K)) ||
             K == EDasMethodSignature::Frame))
            return false;
        if (K == EDasMethodSignature::Int64)
            return T->isSimpleType(das::Type::tInt64);
        if (K == EDasMethodSignature::DateTime || K == EDasMethodSignature::Timespan)
            return T->baseType == das::Type::tHandle && T->annotation &&
                   T->annotation->name == (K == EDasMethodSignature::DateTime ? "UEDateTime" : "UETimespan");
        if (DasIsTypedCallKind(K))
        {
            const auto* Facet = DasTypedCallFacet(K);
            return Facet && Facet->MatchesScriptType(T);
        }
        if (K == EDasMethodSignature::Byte)
            return T->isSimpleType(das::Type::tUInt8);
        if (K == EDasMethodSignature::Text)
            return T->baseType == das::Type::tHandle && T->annotation && T->annotation->name == "UETextValue";
        if (K == EDasMethodSignature::Class)
            return T->baseType == das::Type::tHandle && T->annotation && T->annotation->name == "UEClassHandle";
        if (K == EDasMethodSignature::Delegate)
            return T->isSimpleType(das::Type::tUInt64);
        if (K == EDasMethodSignature::Enum)
            return T->baseType == das::Type::tHandle && T->annotation && T->annotation->name == "UEEnumValue";
        if (K == EDasMethodSignature::Struct)
            return T->baseType == das::Type::tHandle && T->annotation && T->annotation->name == "UEStructValue";
        if (const auto* Adapter = FDasTypeAdapters::Get().FindSignature(K))
            for (const auto& Entry : FDasTypeAdapters::Get().Entries())
                if (&*Entry.Adapter == Adapter && Entry.Script)
                    return Entry.Script->MatchesScriptType(T);
        if (K == EDasMethodSignature::Object)
            return T->baseType == das::Type::tHandle && T->annotation && T->annotation->name == "UObjectHandle";
        if (K == EDasMethodSignature::Name)
            return T->baseType == das::Type::tHandle && T->annotation && T->annotation->name == "UENameValue";
        const auto D = K == EDasMethodSignature::String   ? das::Type::tString
                       : K == EDasMethodSignature::Double ? das::Type::tDouble
                       : K == EDasMethodSignature::Int    ? das::Type::tInt
                       : K == EDasMethodSignature::Float  ? das::Type::tFloat
                       : K == EDasMethodSignature::Bool   ? das::Type::tBool
                                                          : das::Type::tVoid;
        return K == EDasMethodSignature::Void ? T->baseType == D : T->isSimpleType(D);
    };
    if (!F || !F->exports || F->arguments.size() != static_cast<size_t>(C.Inputs.Num()) ||
        !Matches(F->result, C.ReturnKind))
        return false;
    for (int32 I = 0; I < C.Inputs.Num(); ++I)
        if (C.Inputs[I] == EDasMethodSignature::Void || !Matches(F->arguments[I]->type, C.Inputs[I]))
            return false;
    for (const auto* A : F->annotations)
        if (F->generated && A->annotation->name == "__ue_handler_contract")
        {
            const auto* Wire = A->arguments.find("wire", das::Type::tString);
            FDasAuthoringModule Model;
            FDasClassSchema Schema;
            FString Error;
            if (!Wire || !FDasAuthoringModule::FromJson(UTF8_TO_TCHAR(Wire->sValue.c_str()), Model, Error) ||
                Model.Classes.Num() != 1 || !Model.ToActorSchema(Model.Classes[0].Id, Schema, Error) ||
                Schema.Methods.Num() != 1)
                return false;
            auto Method = Schema.Methods[0];
            Method.Signature = EDasMethodSignature::Frame;
            return Method.Contract() == C;
        }
    for (const auto* A : F->annotations)
        if (F->generated && A->annotation->name == "__ue_callback_frame")
        {
            auto* P = A->arguments.find("path", das::Type::tString);
            return P && C.ReturnKind == EDasMethodSignature::Void && C.Inputs.Num() == 1 &&
                   C.Inputs[0] == EDasMethodSignature::Struct && C.InputClasses.Num() == 1 &&
                   C.InputClasses[0] == UTF8_TO_TCHAR(P->sValue.c_str()) && FindDasCallbackFrame(C.InputClasses[0]);
        }
    // Like authored UFunction entries, the compiler-owned ability descriptor is
    // an explicit registry for this otherwise non-public struct-frame ABI.
    if (F->generated && C.ReturnKind == EDasMethodSignature::Void && C.InputDirections.IsEmpty() &&
        C.Inputs.Num() == 1 && C.Inputs[0] == EDasMethodSignature::Struct && C.InputClasses.Num() == 1 &&
        C.InputClasses[0] == TEXT("/Script/DasRuntime.DasAuthoredAbilityContext"))
    {
        bool Declared = false;
        Program->getThisModule()->structures.foreach (
            [&](const das::StructurePtr& S)
            {
                for (auto* A : S->annotations)
                    if (A->annotation->name == "ue_gameplay_ability")
                    {
                        auto* Wire = A->arguments.find("_ue_gas_declaration", das::Type::tString);
                        TSharedPtr<FJsonObject> Object;
                        if (!Wire || !FJsonSerializer::Deserialize(
                                         TJsonReaderFactory<>::Create(UTF8_TO_TCHAR(Wire->sValue.c_str())), Object))
                            continue;
                        for (const auto* Key : {TEXT("activate"), TEXT("end"), TEXT("cancel"), TEXT("task")})
                        {
                            FString Value;
                            Declared |= Object->TryGetStringField(Key, Value) && Value == Entry;
                        }
                    }
            });
        if (Declared)
            return true;
    }
    if (F->generated && C.ReturnKind == EDasMethodSignature::Void && C.InputDirections.IsEmpty() &&
        C.Inputs.Num() == 1 && C.Inputs[0] == EDasMethodSignature::Struct && C.InputClasses.Num() == 1 &&
        C.InputClasses[0] == TEXT("/Script/DasRuntime.DasGASCueEvent"))
    {
        bool Declared = false;
        Program->getThisModule()->structures.foreach (
            [&](const das::StructurePtr& S)
            {
                for (auto* A : S->annotations)
                    if (A->annotation->name == "ue_gameplay_cue")
                    {
                        const auto* Wire = A->arguments.find("_ue_gas_declaration", das::Type::tString);
                        TSharedPtr<FJsonObject> O;
                        if (!Wire || !FJsonSerializer::Deserialize(
                                         TJsonReaderFactory<>::Create(UTF8_TO_TCHAR(Wire->sValue.c_str())), O))
                            continue;
                        for (const auto* Key :
                             {TEXT("execute"), TEXT("onactive"), TEXT("whileactive"), TEXT("removed")})
                        {
                            FString Value;
                            Declared |= O->TryGetStringField(Key, Value) && Value == Entry;
                        }
                    }
            });
        if (Declared)
            return true;
    }
    if (GASCalculationContract && F->generated &&
        (C.ReturnKind == EDasMethodSignature::Float ||
         (C.ReturnKind == EDasMethodSignature::Struct &&
          C.ReturnClass == TEXT("/Script/DasRuntime.DasGASExecutionResult"))) &&
         C.InputDirections.IsEmpty() && C.Inputs.Num() == 1 && C.Inputs[0] == EDasMethodSignature::Struct &&
         C.InputClasses.Num() == 1 && C.InputClasses[0] == TEXT("/Script/DasRuntime.DasGASCalculationContext"))
        return true;
    bool Objects = C.ReturnKind == EDasMethodSignature::Delegate || C.ReturnKind == EDasMethodSignature::Object ||
                   C.ReturnKind == EDasMethodSignature::Struct || C.ReturnKind == EDasMethodSignature::Enum;
    for (auto K : C.Inputs)
        Objects |= K == EDasMethodSignature::Delegate || K == EDasMethodSignature::Object ||
                   K == EDasMethodSignature::Struct || K == EDasMethodSignature::Enum;
    {
        bool Found = false, Valid = true;
        Program->getThisModule()->structures.foreach (
            [&](const das::StructurePtr& St)
            {
                for (auto* A : St->annotations)
                    if (A->annotation->name == "ue_class" || A->annotation->name == "ue_library")
                    {
                        auto* Wire = A->arguments.find("_ue_model_json", das::Type::tString);
                        FDasAuthoringModule M;
                        FString Error;
                        if (!Wire || !FDasAuthoringModule::FromJson(UTF8_TO_TCHAR(Wire->sValue.c_str()), M, Error))
                        {
                            Valid = false;
                            continue;
                        }
                        for (const auto& Cl : M.Classes)
                            for (const auto& Method : Cl.Methods)
                                if (Method.EntryPoint == Entry)
                                {
                                    FDasClassSchema Schema;
                                    if (!M.ToActorSchema(Cl.Id, Schema, Error))
                                    {
                                        Valid = false;
                                        continue;
                                    }
                                    for (const auto& S : Schema.Methods)
                                        if (S.EntryPoint == Entry)
                                        {
                                            Found = true;
                                            Valid &= S.Contract() == C;
                                        }
                                }
                    }
            });
        if (!Valid)
            return false;
        if (!Found)
        {
            if (InputSnapshotDispatch &&
                (C.ReturnKind == EDasMethodSignature::Int || C.ReturnKind == EDasMethodSignature::Void) &&
                C.InputDirections.IsEmpty() && !C.Inputs.IsEmpty() && C.Inputs.Num() == C.InputClasses.Num())
            {
                bool Safe = true;
                for (int32 I = 0; I < C.Inputs.Num(); ++I)
                    Safe &= C.Inputs[I] == EDasMethodSignature::Struct &&
                            DasIsInputEventType(FindObject<UScriptStruct>(nullptr, *C.InputClasses[I]));
                if (Safe)
                    return true;
            }
            if (GeometrySnapshotDispatch && C.ReturnKind == EDasMethodSignature::Int && C.InputDirections.IsEmpty() &&
                C.Inputs.Num() == 1 && C.Inputs[0] == EDasMethodSignature::Struct && C.InputClasses.Num() == 1 &&
                DasIsGeometryType(FindObject<UScriptStruct>(nullptr, *C.InputClasses[0])))
                return true;
            if (!C.InputDirections.IsEmpty())
                return false;
            if (C.ReturnKind == EDasMethodSignature::Class || C.ReturnKind == EDasMethodSignature::Delegate ||
                C.ReturnKind == EDasMethodSignature::Struct || C.ReturnKind == EDasMethodSignature::Enum)
                return false;
            for (auto K : C.Inputs)
                if (K == EDasMethodSignature::Class || K == EDasMethodSignature::Delegate ||
                    K == EDasMethodSignature::Struct || K == EDasMethodSignature::Enum)
                    return false;
            if (C.ReturnKind == EDasMethodSignature::Object && C.ReturnClass != TEXT("/Script/CoreUObject.Object"))
                return false;
            for (int32 I = 0; I < C.Inputs.Num(); ++I)
                if (C.Inputs[I] == EDasMethodSignature::Object &&
                    (!C.InputClasses.IsValidIndex(I) || C.InputClasses[I] != TEXT("/Script/CoreUObject.Object")))
                    return false;
        }
    }
    return true;
}
