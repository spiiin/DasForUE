#include "DasPropertyAccess.h"
#include "DasSchema.h"
#include "UObject/UnrealType.h"
#include "UObject/StructOnScope.h"
#include "UObject/Script.h"
#include "UObject/StrongObjectPtr.h"

namespace {
struct FPropertyStorage {
    FProperty* Property; void* Memory;
    explicit FPropertyStorage(FProperty* P):Property(P),Memory(P->AllocateAndInitializeValue()){}
    ~FPropertyStorage(){Property->DestroyAndFreeValue(Memory);}
    FPropertyStorage(const FPropertyStorage&)=delete;
};
bool Accessor(FProperty* P,UClass* Class,FName Name,bool Write,UFunction*& Function,FProperty*& Value,FString& Error){
    Function=Class?Class->FindFunctionByName(Name):nullptr;Value=nullptr;
    if(!Function || !Function->HasAnyFunctionFlags(FUNC_Native) || Function->HasAnyFunctionFlags(FUNC_Static|FUNC_Net|FUNC_Delegate|FUNC_MulticastDelegate|FUNC_EditorOnly)){
        Error=TEXT("Property accessor requires a local native instance UFunction");return false;
    }
#if WITH_EDITOR
    if(Function->HasMetaData(TEXT("Latent")) || Function->HasMetaData(TEXT("CustomThunk"))){Error=TEXT("Property accessor needs a special adapter");return false;}
#endif
    int32 Count=0;
    for(TFieldIterator<FProperty> It(Function);It;++It)if(It->HasAnyPropertyFlags(CPF_Parm)){
        ++Count;const bool Return=It->HasAnyPropertyFlags(CPF_ReturnParm);
        const bool Out=It->HasAnyPropertyFlags(CPF_OutParm) && !It->HasAnyPropertyFlags(CPF_ConstParm);
        if((Write?(Return || Out):!Return) || It->ArrayDim!=1 || !P->SameType(*It)){
            Error=TEXT("Property accessor value type/direction changed");return false;
        }
        Value=*It;
    }
    if(Count!=1){Error=TEXT("Property accessor must have exactly one input or return value");return false;}
    return true;
}
bool OwnerCheck(FProperty* P,UObject* O,const FDasPropertyAccess& Access,FString& Error){
    if(!IsInGameThread() || !P || !IsValid(O) || !P->GetOwnerClass() || !O->IsA(P->GetOwnerClass()) || P->ArrayDim!=1){Error=TEXT("Invalid owner-property access");return false;}
    return ValidateDasPropertyAccess(P,Access,Error);
}
bool ContextCheck(bool Unsafe,bool Construction,UFunction* Function,FString& Error){
#if WITH_EDITOR
    Unsafe |= Function && Function->HasMetaData(TEXT("UnsafeDuringActorConstruction"));
#endif
    if(Unsafe && Construction){Error=TEXT("UFunction is unsafe during ConstructionScript");return false;}
    return true;
}
void Invoke(UObject* Owner,UFunction* Function,void* Frame){
#if WITH_EDITOR
    // An explicit accessor is authorized by its checked field contract, including editor construction.
    FEditorScriptExecutionGuard Guard;
#endif
    Owner->ProcessEvent(Function,Frame);
}
}

bool DescribeDasPropertyAccess(FProperty* P,FDasPropertyAccess& Out,FString& Error){
    Error.Reset();
    if(!P || !P->GetOwnerClass()){Error=TEXT("Expected an object property");return false;}
    FDasPropertyAccess Access;Access.NativeGetter=P->HasGetter();Access.NativeSetter=P->HasSetter();
#if WITH_EDITOR
    Access.Getter=FName(*P->GetMetaData(TEXT("BlueprintGetter")));Access.Setter=FName(*P->GetMetaData(TEXT("BlueprintSetter")));
    for(bool Write:{false,true}){
        const FName Name=Write?Access.Setter:Access.Getter;if(Name.IsNone())continue;
        UFunction* Function=nullptr;FProperty* Value=nullptr;
        if(!Accessor(P,P->GetOwnerClass(),Name,Write,Function,Value,Error))return false;
        (Write?Access.SetterUnsafe:Access.GetterUnsafe)=Function->HasMetaData(TEXT("UnsafeDuringActorConstruction"));
    }
#endif
    Out=Access;return true;
}
bool ValidateDasPropertyAccess(FProperty* P,const FDasPropertyAccess& Access,FString& Error){
    Error.Reset();
    if(!P || P->HasGetter()!=Access.NativeGetter || P->HasSetter()!=Access.NativeSetter){Error=TEXT("Native property accessor contract changed; regenerate bindings");return false;}
#if WITH_EDITOR
    if(FName(*P->GetMetaData(TEXT("BlueprintGetter")))!=Access.Getter || FName(*P->GetMetaData(TEXT("BlueprintSetter")))!=Access.Setter){Error=TEXT("Blueprint property accessor contract changed; regenerate bindings");return false;}
#endif
    return true;
}
bool ReadDasObjectProperty(FProperty* P,UObject* O,const FDasPropertyAccess& Access,bool Construction,FDasScalarValue& Out,FString& Error){
    Error.Reset();
    if(!OwnerCheck(P,O,Access,Error))return false;
    TStrongObjectPtr<UObject> KeepOwner(O);
    if(!Access.Getter.IsNone()){
        UFunction* F=nullptr;FProperty* Value=nullptr;
        if(!Accessor(P,O->GetClass(),Access.Getter,false,F,Value,Error) || !ContextCheck(Access.GetterUnsafe,Construction,F,Error))return false;
        FStructOnScope Frame(F);Invoke(O,F,Frame.GetStructMemory());
        return ReadDasPropertyValue(Value,Value->ContainerPtrToValuePtr<void>(Frame.GetStructMemory()),Out,Error);
    }
    if(!Access.NativeGetter)return ReadDasPropertyValue(P,P->ContainerPtrToValuePtr<void>(O),Out,Error);
    FPropertyStorage Storage(P);
    // UHT bool accessors exchange a native bool, whereas reflected bitfields use a mask.
    if(auto* Bool=CastField<FBoolProperty>(P)){bool Value=false;P->CallGetter(O,&Value);Bool->SetPropertyValue(Storage.Memory,Value);}
    else P->GetValue_InContainer(O,Storage.Memory);
    return ReadDasPropertyValue(P,Storage.Memory,Out,Error);
}
bool WriteDasObjectProperty(FProperty* P,UObject* O,const FDasPropertyAccess& Access,bool Construction,const FDasScalarValue& Value,FString& Error){
    Error.Reset();
    if(!OwnerCheck(P,O,Access,Error))return false;
    TStrongObjectPtr<UObject> KeepOwner(O);
    if((Value.Kind==int32(EDasMethodSignature::Float) && !FMath::IsFinite(Value.FloatValue)) || (Value.Kind==int32(EDasMethodSignature::Double) && !FMath::IsFinite(Value.DoubleValue))){Error=TEXT("Native numeric property must be finite");return false;}
    if(P->HasAnyPropertyFlags(CPF_BlueprintReadOnly)){Error=TEXT("Native property is readonly");return false;}
    if(!Access.Setter.IsNone()){
        UFunction* F=nullptr;FProperty* Input=nullptr;
        if(!Accessor(P,O->GetClass(),Access.Setter,true,F,Input,Error) || !ContextCheck(Access.SetterUnsafe,Construction,F,Error))return false;
        FStructOnScope Frame(F);
        if(!WriteDasPropertyValue(Input,Input->ContainerPtrToValuePtr<void>(Frame.GetStructMemory()),Value,Error))return false;
        FDasPropertyValueRoots Roots;Roots.Keep(Value);Invoke(O,F,Frame.GetStructMemory());return true;
    }
    if(!Access.NativeSetter)return WriteDasPropertyValue(P,P->ContainerPtrToValuePtr<void>(O),Value,Error);
    FPropertyStorage Storage(P);
    if(!WriteDasPropertyValue(P,Storage.Memory,Value,Error))return false;
    FDasPropertyValueRoots Roots;Roots.Keep(Value);
    if(auto* Bool=CastField<FBoolProperty>(P)){const bool Native=Bool->GetPropertyValue(Storage.Memory);P->CallSetter(O,&Native);}
    else P->SetValue_InContainer(O,Storage.Memory);return true;
}
