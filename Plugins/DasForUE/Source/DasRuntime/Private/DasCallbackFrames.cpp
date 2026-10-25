#include "DasInterfaceContracts.h"
#include "DasCallbackFrames.h"
#include "DasPerceptionTypes.h"
#include "DasCallbackTypes.h"
#include "DasDelegateSignatures.h"
#include "UObject/StrongObjectPtr.h"
#include "DasContainerRegistry.h"
#include "DasPropertyTypes.h"
#include "DasPropertyValues.h"
#include "UObject/Package.h"
#include "UObject/UnrealType.h"
#include "Misc/SecureHash.h"

namespace
{
struct FFrameEntry
{
    TWeakObjectPtr<UScriptStruct> Type;
    TWeakObjectPtr<UFunction> Builder;
    FString Fingerprint;
};
TMap<FString, FFrameEntry> Frames;
TArray<FProperty*> Parameters(UFunction* F)
{
    TArray<FProperty*> R;
    if (F)
        for (TFieldIterator<FProperty> I(F); I; ++I)
            if (I->HasAnyPropertyFlags(CPF_Parm))
                R.Add(*I);
    return R;
}
bool Compatible(FProperty* Source, FProperty* Target)
{
    if (auto* Expected = CastField<FClassProperty>(Target))
    {
        auto* Actual = CastField<FClassProperty>(Source);
        return Actual && Actual->MetaClass && Expected->MetaClass && Actual->MetaClass->IsChildOf(Expected->MetaClass);
    }
    if (auto* Expected = DasReferencePropertyClass(Target))
    {
        auto* Actual = DasReferencePropertyClass(Source);
        return Actual && (Actual == Expected ||
                          (Expected->HasAnyClassFlags(CLASS_Interface) ? DasImplementsInterface(Actual, Expected)
                                                                       : Actual->IsChildOf(Expected)));
    }
    if (auto* Expected = DasEnumPropertyType(Target))
        return DasEnumPropertyType(Source) == Expected;
    if (auto* Expected = CastField<FStructProperty>(Target))
        return DasCallbackSnapshotForProperty(Source) == Expected->Struct ||
               CanProjectDasAIStimulus(Source, Expected->Struct);
    return DasPropertyKind(Source) == DasPropertyKind(Target) && Source->SameType(Target);
}
}
UScriptStruct* DasCallbackFrame(const TArray<FDasDeclaredType>& Fields, FString& Error)
{
    check(IsInGameThread());
    Error.Reset();
    if (Fields.Num() < 2 || Fields.Num() > 16)
    {
        Error = TEXT("Callback frames require 2..16 input arguments");
        return nullptr;
    }
    FString Identity = TEXT("callback-frame-v1;"), Fingerprint;
    for (const auto& T : Fields)
    {
        FString Part;
        if (!ValidateDasPropertyType(T, EDasPropertyUsage::Value, Error) || !DasPropertyLayoutFingerprint(T, Part))
            return nullptr;
        Identity += T.StorageIdentity() + TEXT(";");
        Fingerprint += Part + TEXT(";");
    }
    const FString Name = TEXT("DasCallbackFrame_")+ FMD5::HashAnsiString(*Identity),
                              Path = TEXT("/Script/DasRuntime.")+ Name;
    if (const auto* Entry = Frames.Find(Path))
    {
        if (Entry->Type.IsValid() && Entry->Fingerprint == Fingerprint)
            return Entry->Type.Get();
        Error = TEXT("Callback frame layout changed; live frame cannot be reused");
        return nullptr;
    }
    if (Frames.Num() >= 1024 || FindObject<UObject>(nullptr, *Path))
    {
        Error = TEXT("Callback frame registry limit or occupied path");
        return nullptr;
    }
    auto* Type = NewObject<UScriptStruct>(CreatePackage(TEXT("/Script/DasRuntime")), *Name, RF_Transient);
    auto* Builder = NewObject<UFunction>(GetTransientPackage(), NAME_None, RF_Transient);
    Builder->FunctionFlags = FUNC_Public;
    for (int32 I = Fields.Num() - 1; I >= 0; --I)
    {
        const FName Field(*FString::Printf(TEXT("arg_%d"), I));
        auto* P = CreateDasProperty(Type, Field, Fields[I], EDasPropertyUsage::Value, Error);
        auto* B = CreateDasProperty(Builder, Field, Fields[I], EDasPropertyUsage::Value, Error);
        if (!P || !B)
        {
            delete P;
            delete B;
            Type->Rename(nullptr, GetTransientPackage(), REN_DontCreateRedirectors | REN_NonTransactional);
            Type->MarkAsGarbage();
            return nullptr;
        }
        Type->AddCppProperty(P);
        B->SetPropertyFlags(CPF_Parm);
        Builder->AddCppProperty(B);
    }
    Type->Bind();
    Type->StaticLink(true);
    Builder->Bind();
    Builder->StaticLink(true);
    Builder->InitializeDerivedMembers();
    if (!ValidateDasCallbackTypePath(TEXT("struct"), Path, Error))
    {
        Type->Rename(nullptr, GetTransientPackage(), REN_DontCreateRedirectors | REN_NonTransactional);
        Type->MarkAsGarbage();
        return nullptr;
    }
    Type->AddToRoot();
    Builder->AddToRoot();
    Frames.Add(Path, {Type, Builder, MoveTemp(Fingerprint)});
    return Type;
}
UScriptStruct* FindDasCallbackFrame(const FString& Path)
{
    auto* E = Frames.Find(Path);
    return E ? E->Type.Get() : nullptr;
}
UFunction* DasCallbackFrameBuilder(UScriptStruct* Frame)
{
    auto* E = Frame ? Frames.Find(Frame->GetPathName()) : nullptr;
    return E ? E->Builder.Get() : nullptr;
}
bool ValidateDasCallbackFrameSource(UFunction* Signature, UScriptStruct* Frame, FString& Error)
{
    Error.Reset();
    const auto Source = Parameters(Signature);
    if (!Frame || FindDasCallbackFrame(Frame->GetPathName()) != Frame || !Signature || Signature->GetReturnProperty() ||
        Source.Num() < 2 || Source.Num() > 16)
    {
        Error = TEXT("Invalid callback frame signature");
        return false;
    }
    int32 I = 0;
    for (TFieldIterator<FProperty> Target(Frame); Target; ++Target, ++I)
    {
        if (!Source.IsValidIndex(I) || Source[I]->HasAnyPropertyFlags(CPF_ReturnParm) ||
            (Source[I]->HasAnyPropertyFlags(CPF_OutParm | CPF_ReferenceParm) &&
             !Source[I]->HasAnyPropertyFlags(CPF_ConstParm)) ||
            !DasCallbackTypeForProperty(Source[I]) || !Compatible(Source[I], *Target))
        {
            Error = FString::Printf(TEXT("Callback argument %d type/direction differs from source"), I);
            return false;
        }
    }
    if (I != Source.Num())
    {
        Error = TEXT("Callback argument count differs from source");
        return false;
    }
    return true;
}
bool CaptureDasCallbackFrame(UFunction* Signature, const void* Memory, UScriptStruct* Frame, FDasScalarValue& Out,
                             FString& Error)
{
    if (!Memory || !ValidateDasCallbackFrameSource(Signature, Frame, Error))
        return false;
    const auto Source = Parameters(Signature);
    FDasScalarValue Value;
    Value.Kind = 12;
    Value.ObjectClass = Frame->GetPathName();
    Value.StructValue.InitializeAs(Frame);
    int32 I = 0;
    for (TFieldIterator<FProperty> Target(Frame); Target; ++Target, ++I)
    {
        FDasScalarValue Input;
        const auto* Expected = CastField<FStructProperty>(*Target);
        if (Expected && CanProjectDasAIStimulus(Source[I], Expected->Struct))
        {
            if (!ReadDasAIStimulusCallback(Source[I], Source[I]->ContainerPtrToValuePtr<void>(Memory), Expected->Struct,
                                           Input, Error))
                return false;
        }
        else if (!ReadDasPropertyValue(Source[I], Source[I]->ContainerPtrToValuePtr<void>(Memory), Input, Error))
            return false;
        if (auto* Class = CastField<FClassProperty>(*Target))
            Input.ObjectClass = Class->MetaClass->GetPathName();
        else if (auto* ObjectClass = DasReferencePropertyClass(*Target))
            Input.ObjectClass = ObjectClass->GetPathName();
        if (!WriteDasPropertyValue(*Target, Target->ContainerPtrToValuePtr<void>(Value.StructValue.GetMutableMemory()),
                                   Input, Error))
            return false;
    }
    if (!ValidateDasCallbackPayload(Value, EDasCallbackUsage::Event, Error))
        return false;
    Out = MoveTemp(Value);
    return true;
}
void ResetDasCallbackFrames()
{
    check(IsInGameThread());
    for (auto& E : Frames)
    {
        if (auto* B = E.Value.Builder.Get())
            B->RemoveFromRoot();
        if (auto* T = E.Value.Type.Get())
            T->RemoveFromRoot();
    }
    Frames.Empty();
}

bool ValidateDasCallbackSignature(UFunction* Signature, const TArray<FDasDeclaredType>& Arguments, FString& Error)
{
    const auto Source = Parameters(Signature);
    if (!Signature || Signature->GetReturnProperty() || Source.Num() != Arguments.Num() || Source.Num() > 16)
    {
        Error = TEXT("Callback argument count/return differs from source");
        return false;
    }
    FDasDeclaredMethod M;
    M.ReturnType.Kind = TEXT("void");
    for (int32 I = 0; I < Arguments.Num(); ++I)
    {
        FDasDeclaredParameter P;
        P.Name = FString::Printf(TEXT("arg_%d"), I);
        P.Type = Arguments[I];
        M.Parameters.Add(P);
    }
    TStrongObjectPtr<UFunction> Expected(MakeDasDelegateSignature(M, false, GetTransientPackage(), NAME_None, Error));
    if (!Expected.IsValid())
        return false;
    int32 I = 0;
    for (TFieldIterator<FProperty> Target(Expected.Get()); Target; ++Target, ++I)
    {
        const auto* Capability = Source.IsValidIndex(I) ? DasCallbackTypeForProperty(Source[I]) : nullptr;
        const bool Ref = Source.IsValidIndex(I) && Source[I]->HasAnyPropertyFlags(CPF_OutParm | CPF_ReferenceParm);
        if (Capability && !Capability->bDynamic &&
            (Ref ? Capability->ConstRefProxy.IsNone() : Capability->ValueProxy.IsNone()))
        {
            Error = FString::Printf(TEXT("Callback argument %d has no queued receiver"), I);
            return false;
        }
        if (!Source.IsValidIndex(I) ||
            (Source[I]->HasAnyPropertyFlags(CPF_OutParm | CPF_ReferenceParm) &&
             !Source[I]->HasAnyPropertyFlags(CPF_ConstParm)) ||
            !DasCallbackTypeForProperty(Source[I]) || !Compatible(Source[I], *Target))
        {
            Error = FString::Printf(TEXT("Callback argument %d (%s) type/direction differs from source"), I,
                                         Source.IsValidIndex(I) ? *Source[I]->GetName() : TEXT("?"));
            return false;
        }
    }
    return true;
}
