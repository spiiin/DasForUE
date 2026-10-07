#include "DasRuntime.h"
#include "DasReflectedTypes.h"
#include "DasAuthoring.h"
#include "Serialization/JsonSerializer.h"
#include "Dom/JsonObject.h"
#include "Misc/EngineVersion.h"
#include "DasProbeActor.h"
#include "DasIntDelegateProxy.h"
#include "UObject/StrongObjectPtr.h"
#include "UObject/StructOnScope.h"
#include "GameFramework/Actor.h"
#include "Engine/World.h"
#include "UObject/UObjectGlobals.h"
#include "UObject/UnrealType.h"
#include "Interfaces/IPluginManager.h"
#include "Misc/FileHelper.h"
#include "Misc/ScopeExit.h"
#include "Misc/Paths.h"
#include "HAL/FileManager.h"
#include "Modules/ModuleManager.h"

#pragma push_macro("verify")
#pragma push_macro("ensure")
#pragma push_macro("check")
#undef verify
#undef ensure
#undef check
THIRD_PARTY_INCLUDES_START
// arraytype.h resets packing: load simulation prerequisites first, then match
// the static daScript AST library's /Zp16 rather than Unreal's /Zp8.
#include "daScript/simulate/simulate.h"
#pragma pack(push, 16)
#include "daScript/daScript.h"
#include "daScript/ast/ast_handle.h"
#include "daScript/ast/ast_visitor.h"
#include "daScript/simulate/data_walker.h"
THIRD_PARTY_INCLUDES_END
#pragma pack(pop)
#pragma pop_macro("check")
#pragma pop_macro("ensure")
#pragma pop_macro("verify")

// No fields are exposed to scripts: this is a context-local capability, never a pointer.
struct FDasNameValue { uint64 Token=0; };
MAKE_TYPE_FACTORY(UENameValue,FDasNameValue);
namespace das {template<> struct cast<FDasNameValue>{static vec4f from(FDasNameValue V){return cast<uint64_t>::from(V.Token);}static FDasNameValue to(vec4f V){return {cast<uint64_t>::to(V)};}};}
struct FDasObjectHandle { uint64 Token = 0; };
MAKE_TYPE_FACTORY(UObjectHandle, FDasObjectHandle);
struct FDasStructValue {uint64 Token=0;};
MAKE_TYPE_FACTORY(UEStructValue,FDasStructValue);
namespace das{template<>struct cast<FDasStructValue>{static vec4f from(FDasStructValue V){return cast<uint64_t>::from(V.Token);}static FDasStructValue to(vec4f V){return {cast<uint64_t>::to(V)};}};}
MAKE_TYPE_FACTORY(UEVector,FVector);
MAKE_TYPE_FACTORY(UERotator,FRotator);
MAKE_TYPE_FACTORY(UETransform,FTransform);
struct FDasCallHandle { uint64 Token = 0; };
MAKE_TYPE_FACTORY(UCallHandle, FDasCallHandle);

DEFINE_LOG_CATEGORY_STATIC(LogDasRuntime, Log, All);

namespace
{
    struct FSourceSnapshot
    {
        bool bExists = false;
        TArray<uint8> Bytes;
        static FSourceSnapshot Read(const FString& Path)
        {
            FSourceSnapshot Result;
            Result.bExists = IFileManager::Get().FileExists(*Path) && FFileHelper::LoadFileToArray(Result.Bytes, *Path);
            return Result;
        }
        bool Matches(const FString& Path) const
        {
            const auto Current = Read(Path);
            return bExists == Current.bExists && Bytes == Current.Bytes;
        }
    };
    using FSourceSet = TMap<FString, FSourceSnapshot>;

    class FUnrealTextFile final : public das::FileInfo
    {
    public:
        TArray<uint8> Bytes;
        virtual void getSourceAndLength(const char*& Source, uint32_t& Length) override
        {
            Source = reinterpret_cast<const char*>(Bytes.GetData());
            Length = static_cast<uint32_t>(Bytes.Num() - 1);
        }
    };

    // Reads through UE's platform file chain, including staged Pak/UFS files.
    class FUnrealFileSystem final : public das::AnyFileSystem
    {
    public:
        FSourceSet* Sources = nullptr;
        explicit FUnrealFileSystem(FSourceSet* InSources = nullptr) : Sources(InSources) {}
        virtual das::FileInfo* tryOpenFile(const das::string& Name) override
        {
            const FString Path(UTF8_TO_TCHAR(Name.c_str()));
            auto Snapshot = FSourceSnapshot::Read(Path);
            if (Sources) Sources->Add(Path, Snapshot);
            if (!Snapshot.bExists) return nullptr;
            auto File = MakeUnique<FUnrealTextFile>();
            File->Bytes = MoveTemp(Snapshot.Bytes);
            File->Bytes.Add(0);
            File->name = Name;
            return File.Release();
        }
    };

    struct FSubscriptionDeclaration
    {
        TWeakObjectPtr<UObject> Source;
        FName Event;
        FString Handler;
    };

    struct FObjectHandleEntry
    {
        TWeakObjectPtr<UObject> Object;
        TWeakObjectPtr<UClass> ExpectedClass;
    };
    uint64 NextObjectHandle = 1;

    enum class EMarshallingKind { Int, Float, Double, Bool, String, Struct, Object, Name, Enum, Array, Map, Set };
    // Flattened scalar signature IR: names/types/directions, independent of property addresses.
    struct FMarshalledParameter { FString Path; EMarshallingKind Kind; bool bInput; bool bOutput; FString TypePath; };
    struct FHashValue
    {
        FProperty* Property;
        void* Memory;
        explicit FHashValue(FProperty* InProperty) : Property(InProperty)
        { Memory = FMemory::Malloc(Property->GetSize(), Property->GetMinAlignment()); Property->InitializeValue(Memory); }
        ~FHashValue() { Property->DestroyValue(Memory); FMemory::Free(Memory); }
        FHashValue(const FHashValue&) = delete;
        FHashValue& operator=(const FHashValue&) = delete;
    };
    struct FPendingHash
    {
        TArray<TUniquePtr<FHashValue>> Keys;
        TArray<TUniquePtr<FHashValue>> Values;
    };
    struct FMarshalledCall
    {
        TWeakObjectPtr<UObject> Target;
        TStrongObjectPtr<UFunction> Function;
        TUniquePtr<FStructOnScope> Frame;
        TSet<FString> Assigned;
        TMap<FString, TStrongObjectPtr<UObject>> ObjectPins;
        TArray<FMarshalledParameter> Signature;
        FString SignatureKey;
        TMap<FString, TSharedPtr<FPendingHash>> PendingHashes;
        TMap<FString, TArray<int32>> HashIndices;
        bool bExecuted = false;
        bool bFailed = false;
    };
    uint64 NextCallHandle = 1;

    class FUnrealDasContext final : public das::Context
    {
    public:
        explicit FUnrealDasContext(uint32 StackSize) : das::Context(StackSize) {}
        TArray<FDasTypeDeclaration> Types;
        TMap<uint64,FInstancedStruct> StructValues;uint64 StructCollections=0,StructReclaimed=0;int32 StructNextCollection=256;
        TMap<uint64,FName> DSLNames;TMap<FName,uint64> DSLNameTokens;
        TMap<FString, int32> PersistentInts;
        bool bEnding = false;
        bool bCollectingSubscriptions = false;
        TArray<FSubscriptionDeclaration> Subscriptions;
        TWeakObjectPtr<UObject> Self;
        TMap<uint64, FObjectHandleEntry> ObjectHandles;
        TMap<uint64, TSharedPtr<FMarshalledCall>> Calls;
    };

    thread_local FUnrealDasContext* ActiveContext = nullptr;

    struct FActiveContextScope
    {
        FUnrealDasContext* Previous;
        explicit FActiveContextScope(FUnrealDasContext& Context) : Previous(ActiveContext)
        {
            ActiveContext = &Context;
        }
        ~FActiveContextScope() { ActiveContext->Calls.Empty(); ActiveContext = Previous; }
    };

    int32 GetStateInt(const char* Name, int32 Default, das::Context* Context)
    {
        if (Context != ActiveContext || !Name || !*Name)
            Context->throw_error("state access requires an active context and a nonempty key");
        return ActiveContext->PersistentInts.FindOrAdd(UTF8_TO_TCHAR(Name), Default);
    }
    void SetStateInt(const char* Name, int32 Value, das::Context* Context)
    {
        if (Context != ActiveContext || !Name || !*Name)
            Context->throw_error("state access requires an active context and a nonempty key");
        ActiveContext->PersistentInts.Add(UTF8_TO_TCHAR(Name), Value);
    }

    int32 NativeAdd(int32 A, int32 B) { return A + B; }

    int32 EmitType(const char* Name, const char* Parent, const char* Fields, das::Context* Context)
    {
        // Copies strings while their owning context is alive. No UE objects or
        // compiled function pointers escape this execution in the foundation.
        if (Context != ActiveContext)
            Context->throw_error("UE schema emission is only allowed in the active runtime context");
        if (!Name || !Parent || !Fields)
            Context->throw_error("UE schema declaration strings must not be null");
        auto& UEContext = *ActiveContext;
        UEContext.Types.Add({ UTF8_TO_TCHAR(Name), UTF8_TO_TCHAR(Parent), UTF8_TO_TCHAR(Fields) });
        return UEContext.Types.Num();
    }

    UObject* ResolveSelf(das::Context* Context)
    {
        if (!IsInGameThread() || Context != ActiveContext || !ActiveContext)
        { Context->throw_error("self access requires the active game-thread runtime context"); }
        UObject* Self = ActiveContext->Self.Get();
        if (!IsValid(Self) || (Self->IsA<AActor>() && CastChecked<AActor>(Self)->IsActorBeingDestroyed() && !ActiveContext->bEnding))
        { Context->throw_error("self is missing or expired"); }
        return Self;
    }

    template<class P> P* ResolveProperty(UObject* Self, const char* Name, bool bWrite, das::Context* Context)
    {
        if (!Name || !*Name) { Context->throw_error("property name is empty"); }
        // Resolve again for every call; never retain FProperty pointers across reinstancing.
        const FString FieldName(UTF8_TO_TCHAR(Name));
        if (FieldName.Len() >= NAME_SIZE) { Context->throw_error("property name is too long"); }
        FProperty* Property = Self->GetClass()->FindPropertyByName(FName(*FieldName));
        if (!Property) { Context->throw_error("self property does not exist"); }
        P* Typed = CastField<P>(Property);
        if (!Typed || Property->ArrayDim != 1)
        { Context->throw_error("self property has a different scalar type"); }
        if (!Property->HasAnyPropertyFlags(CPF_BlueprintVisible) ||
            (bWrite && Property->HasAnyPropertyFlags(CPF_BlueprintReadOnly)))
        { Context->throw_error("self property access is not permitted (hidden or read-only)"); }
        return Typed;
    }

    bool GetSelfBool(const char* Name,das::Context* Context)
    {return ResolveProperty<FBoolProperty>(ResolveSelf(Context),Name,false,Context)->GetPropertyValue_InContainer(ResolveSelf(Context));}
    void SetSelfBool(const char* Name,bool Value,das::Context* Context)
    {if(ActiveContext && ActiveContext->bCollectingSubscriptions) Context->throw_error("self writes are forbidden in bind_events");UObject* Self=ResolveSelf(Context);ResolveProperty<FBoolProperty>(Self,Name,true,Context)->SetPropertyValue_InContainer(Self,Value);}
    FDasNameValue MakeDSLName(FName Name,das::Context* C)
    {
        if(C!=ActiveContext || !ActiveContext || !IsInGameThread()) C->throw_error("name requires active context");
        if(Name.IsNone())return {};if(auto* T=ActiveContext->DSLNameTokens.Find(Name))return {*T};
        if(ActiveContext->DSLNames.Num()>=65536)C->throw_error("name table limit exceeded");
        static uint64 NextToken=1;const uint64 Token=NextToken++;ActiveContext->DSLNames.Add(Token,Name);ActiveContext->DSLNameTokens.Add(Name,Token);return {Token};
    }
    FName ResolveDSLName(FDasNameValue V,das::Context* C)
    {if(C!=ActiveContext || !ActiveContext)C->throw_error("name requires active context");if(!V.Token)return NAME_None;auto* N=ActiveContext->DSLNames.Find(V.Token);if(!N)C->throw_error("name belongs to another context");return *N;}
    FDasNameValue DSLNameFromString(const char* S,das::Context* C)
    {const FString T=S?UTF8_TO_TCHAR(S):TEXT("");if(T.Len()>=NAME_SIZE)C->throw_error("FName value is too long");return MakeDSLName(FName(*T),C);}
    char* CopyDSLString(const FString& S,das::Context* C){for(int32 I=0;I<S.Len();++I)if(S[I]==0)C->throw_error("embedded NUL is not supported in strings");const FTCHARToUTF8 U(*S);return C->allocateString(U.Get(),U.Length(),nullptr);}
    char* DSLNameToString(FDasNameValue V,das::Context* C){return CopyDSLString(ResolveDSLName(V,C).ToString(),C);}
    bool DSLNamesEqual(FDasNameValue A,FDasNameValue B,das::Context* C){return ResolveDSLName(A,C)==ResolveDSLName(B,C);}
    double GetSelfDouble(const char* N,das::Context* C){auto* S=ResolveSelf(C);return ResolveProperty<FDoubleProperty>(S,N,false,C)->GetPropertyValue_InContainer(S);}
    void SetSelfDouble(const char* N,double V,das::Context* C){auto* S=ResolveSelf(C);if(ActiveContext->bCollectingSubscriptions)C->throw_error("self writes are forbidden in bind_events");ResolveProperty<FDoubleProperty>(S,N,true,C)->SetPropertyValue_InContainer(S,V);}
    char* GetSelfString(const char* N,das::Context* C){auto* S=ResolveSelf(C);return CopyDSLString(ResolveProperty<FStrProperty>(S,N,false,C)->GetPropertyValue_InContainer(S),C);}
    void SetSelfString(const char* N,const char* V,das::Context* C){auto* S=ResolveSelf(C);if(ActiveContext->bCollectingSubscriptions)C->throw_error("self writes are forbidden in bind_events");ResolveProperty<FStrProperty>(S,N,true,C)->SetPropertyValue_InContainer(S,V?UTF8_TO_TCHAR(V):TEXT(""));}
    FDasNameValue GetSelfName(const char* N,das::Context* C){auto* S=ResolveSelf(C);return MakeDSLName(ResolveProperty<FNameProperty>(S,N,false,C)->GetPropertyValue_InContainer(S),C);}
    void SetSelfName(const char* N,FDasNameValue V,das::Context* C){auto* S=ResolveSelf(C);if(ActiveContext->bCollectingSubscriptions)C->throw_error("self writes are forbidden in bind_events");ResolveProperty<FNameProperty>(S,N,true,C)->SetPropertyValue_InContainer(S,ResolveDSLName(V,C));}
    FVector DSLVector(double X,double Y,double Z,das::Context* C){if(!FMath::IsFinite(X) || !FMath::IsFinite(Y) || !FMath::IsFinite(Z))C->throw_error("vector components must be finite");return FVector(X,Y,Z);}
    FRotator DSLRotator(double Pitch,double Yaw,double Roll,das::Context* C){if(!FMath::IsFinite(Pitch) || !FMath::IsFinite(Yaw) || !FMath::IsFinite(Roll))C->throw_error("rotator components must be finite");return FRotator(Pitch,Yaw,Roll);}
    FTransform DSLTransform(const FVector& Translation,const FRotator& Rotation,const FVector& Scale,das::Context* C){if(Translation.ContainsNaN() || Rotation.ContainsNaN() || Scale.ContainsNaN())C->throw_error("transform components must be finite");return FTransform(Rotation,Translation,Scale);}
    FTransform DSLIdentityTransform(){return FTransform::Identity;}
    FVector DSLTransformPosition(const FTransform& T,const FVector& V,das::Context* C){if(T.ContainsNaN() || !T.IsRotationNormalized() || V.ContainsNaN())C->throw_error("invalid transform or position");const FVector Result=T.TransformPosition(V);if(Result.ContainsNaN())C->throw_error("non-finite transformed position");return Result;}
    FVector DSLTranslation(const FTransform& T){return T.GetTranslation();}
    FVector DSLScale(const FTransform& T){return T.GetScale3D();}
    FRotator DSLRotation(const FTransform& T){return T.Rotator();}
    UEnum* RequireCallEnum(FProperty*,const char*,das::Context*);
    FNumericProperty* CallEnumStorage(FProperty*);
    bool IsCallEnumMember(UEnum*,int32);
    struct FStructRootWalker:das::DataWalker {
        TSet<uint64> Tokens;bool Complete=true;
        virtual bool canVisitHandle(char* Data,das::TypeInfo* Info) override {auto* A=Info->getAnnotation();if(A && A->name=="UEStructValue")Tokens.Add(reinterpret_cast<FDasStructValue*>(Data)->Token);return false;}
    };
    bool CollectStructValues(FUnrealDasContext& C,const das::LineInfo* At,bool Boundary=false)
    {
        if(C.StructValues.IsEmpty())return true;
        // Active compiled/position-less frames cannot prove reachability.
        if(!Boundary && !C.gcEnabled)return false;
        FStructRootWalker W;W.context=&C;
        C.foreach_gc_root([&](void* P,das::TypeInfo* T){if(T)W.walk(static_cast<char*>(P),T);else {W.Complete=false;}});
        for(int I=0;I<C.getTotalVariables();++I){const auto V=C.getGlobalVariable(I);if(!V.debugInfo){W.Complete=false;break;}char* Base=V.shared?C.shared:C.globals;if(Base)W.walk(Base+V.offset,V.debugInfo);}
        if(!Boundary){char* SP=C.stack.ap();const das::LineInfo* Handoff=At;while(SP<C.stack.top()){
            auto* P=reinterpret_cast<das::Prologue*>(SP);das::FuncInfo* Info=nullptr;char* Locals=SP;
            if(P->info){intptr_t B=reinterpret_cast<intptr_t>(P->block);if(B&1){auto* Block=reinterpret_cast<das::Block*>(B&~intptr_t(1));Info=Block->info;Locals=C.stack.bottom()+Block->stackOffset;}else Info=P->info;}
            if(Info){const uint32 Pos=das::lineFramePos(Handoff,Info->spaceId);if(Info->localCount && !Pos)return false;
                for(uint32 I=0;I<Info->count;++I)W.walk(P->arguments[I],Info->fields[I]);
                for(uint32 I=0;I<Info->localCount;++I){auto* L=Info->locals[I];if(das::isLiveAt(L,Pos)){char* Addr=L->cmres?static_cast<char*>(P->cmres):Locals+L->stackTop;if(Addr)W.walk(Addr,L);}}
            }
            const uint32 Step=Info?Info->stackSize:P->stackSize;if(!Step)return false;Handoff=Info?P->line:nullptr;SP+=Step;
        }}
        if(!W.Complete)return false;
        const int32 Before=C.StructValues.Num();for(auto It=C.StructValues.CreateIterator();It;++It)if(!W.Tokens.Contains(It.Key()))It.RemoveCurrent();
        ++C.StructCollections;C.StructReclaimed+=Before-C.StructValues.Num();C.StructNextCollection=FMath::Min(4096,FMath::Max(256,C.StructValues.Num()+256));return true;
    }
    void ExplicitStructCollect(das::Context* C,das::LineInfoArg* At){if(C!=ActiveContext || !ActiveContext || !CollectStructValues(*ActiveContext,At))C->throw_error("struct collection requires options gc and attributable interpreter frames");}
    int32 StructValueCount(das::Context* C){if(C!=ActiveContext || !ActiveContext)C->throw_error("struct stats require active context");return ActiveContext->StructValues.Num();}
    struct FStructCollectOnExit {FUnrealDasContext& C;~FStructCollectOnExit(){try{CollectStructValues(C,nullptr,true);}catch(...){/* Failed root traversal must not turn a result into an exception. */}}};
    FDasStructValue StoreStructValue(const FInstancedStruct& V,das::Context* C,das::LineInfoArg* At=nullptr){if(C!=ActiveContext || !ActiveContext)C->throw_error("struct requires active context");FString Error;if(!ValidateDasStructSnapshot(V,Error))C->throw_error(TCHAR_TO_UTF8(*Error));for(const auto& P:ActiveContext->StructValues)if(P.Value.GetScriptStruct()==V.GetScriptStruct() && V.GetScriptStruct()->CompareScriptStruct(P.Value.GetMemory(),V.GetMemory(),0))return {P.Key};if(At && ActiveContext->StructValues.Num()>=ActiveContext->StructNextCollection)CollectStructValues(*ActiveContext,At);if(ActiveContext->StructValues.Num()>=4096)C->throw_error("struct value table limit exceeded");static uint64 Next=1;uint64 Token=Next++;ActiveContext->StructValues.Add(Token,V);return {Token};}
    const FInstancedStruct& ResolveStructValue(FDasStructValue V,das::Context* C){if(C!=ActiveContext || !ActiveContext)C->throw_error("struct requires active context");const auto* S=ActiveContext->StructValues.Find(V.Token);if(!S)C->throw_error("struct is null or belongs to another context");return *S;}
    FDasStructValue NewStructDefault(const char* Path,const char* Text,das::Context* C,das::LineInfoArg* At){FInstancedStruct Value;FString Error;if(!ParseDasStructDefault(Path?UTF8_TO_TCHAR(Path):TEXT(""),Text?UTF8_TO_TCHAR(Text):TEXT(""),Value,Error))C->throw_error(TCHAR_TO_UTF8(*Error));return StoreStructValue(Value,C,At);}
    FDasStructValue GetSelfStruct(const char* N,das::Context* C,das::LineInfoArg* At){auto* Self=ResolveSelf(C);auto* P=ResolveProperty<FStructProperty>(Self,N,false,C);FInstancedStruct Value;Value.InitializeAs(P->Struct,P->ContainerPtrToValuePtr<uint8>(Self));return StoreStructValue(Value,C,At);}
    void SetSelfStruct(const char* N,const FDasStructValue& V,das::Context* C){auto* Self=ResolveSelf(C);if(ActiveContext->bCollectingSubscriptions)C->throw_error("self writes are forbidden in bind_events");auto* P=ResolveProperty<FStructProperty>(Self,N,true,C);const auto& Value=ResolveStructValue(V,C);FString Error;if(Value.GetScriptStruct()!=P->Struct)C->throw_error("Struct self assignment type mismatch");if(!ValidateDasStructSnapshot(Value,Error))C->throw_error(TCHAR_TO_UTF8(*Error));P->Struct->CopyScriptStruct(P->ContainerPtrToValuePtr<void>(Self),Value.GetMemory());}
    FDasStructValue NewStructValue(const char* Path,das::Context* C,das::LineInfoArg* At){FString P=Path?UTF8_TO_TCHAR(Path):TEXT("");FString E;if(!ValidateDasValueStruct(P,E))C->throw_error(TCHAR_TO_UTF8(*E));FInstancedStruct S;S.InitializeAs(FindObject<UScriptStruct>(nullptr,*P));return StoreStructValue(S,C,At);}
    struct FStructLeaf{FProperty* Property;uint8* Memory;};
    FStructLeaf StructLeaf(FInstancedStruct& V,const char* Path,das::Context* C){TArray<FString> Parts;FString(Path?UTF8_TO_TCHAR(Path):TEXT("")).ParseIntoArray(Parts,TEXT("."),false);if(Parts.IsEmpty())C->throw_error("empty struct field path");UScriptStruct* Type=const_cast<UScriptStruct*>(V.GetScriptStruct());uint8* Memory=V.GetMutableMemory();for(int32 I=0;I<Parts.Num();++I){FProperty* P=Type->FindPropertyByName(*Parts[I]);if(!P)C->throw_error("struct field does not exist");Memory=P->ContainerPtrToValuePtr<uint8>(Memory);if(I==Parts.Num()-1)return {P,Memory};auto* S=CastField<FStructProperty>(P);if(!S)C->throw_error("path crosses non-struct field");Type=S->Struct;}C->throw_error("invalid field path");return {};}
    int32 StructGetInt(const FDasStructValue& H,const char* P,das::Context* C){FInstancedStruct V=ResolveStructValue(H,C);auto F=StructLeaf(V,P,C);auto* T=CastField<FIntProperty>(F.Property);if(!T)C->throw_error("struct field scalar type mismatch");return T->GetPropertyValue(F.Memory);}
    FDasStructValue StructWithInt(const FDasStructValue& H,const char* P,int32 X,das::Context* C,das::LineInfoArg* At){FInstancedStruct V=ResolveStructValue(H,C);auto F=StructLeaf(V,P,C);auto* T=CastField<FIntProperty>(F.Property);if(!T)C->throw_error("struct field scalar type mismatch");T->SetPropertyValue(F.Memory,X);return StoreStructValue(V,C,At);}
    float StructGetFloat(const FDasStructValue& H,const char* P,das::Context* C){FInstancedStruct V=ResolveStructValue(H,C);auto F=StructLeaf(V,P,C);auto* T=CastField<FFloatProperty>(F.Property);if(!T)C->throw_error("struct field scalar type mismatch");return T->GetPropertyValue(F.Memory);}
    FDasStructValue StructWithFloat(const FDasStructValue& H,const char* P,float X,das::Context* C,das::LineInfoArg* At){FInstancedStruct V=ResolveStructValue(H,C);auto F=StructLeaf(V,P,C);auto* T=CastField<FFloatProperty>(F.Property);if(!T)C->throw_error("struct field scalar type mismatch");T->SetPropertyValue(F.Memory,X);return StoreStructValue(V,C,At);}
    double StructGetDouble(const FDasStructValue& H,const char* P,das::Context* C){FInstancedStruct V=ResolveStructValue(H,C);auto F=StructLeaf(V,P,C);auto* T=CastField<FDoubleProperty>(F.Property);if(!T)C->throw_error("struct field scalar type mismatch");return T->GetPropertyValue(F.Memory);}
    FDasStructValue StructWithDouble(const FDasStructValue& H,const char* P,double X,das::Context* C,das::LineInfoArg* At){FInstancedStruct V=ResolveStructValue(H,C);auto F=StructLeaf(V,P,C);auto* T=CastField<FDoubleProperty>(F.Property);if(!T)C->throw_error("struct field scalar type mismatch");T->SetPropertyValue(F.Memory,X);return StoreStructValue(V,C,At);}
    bool StructGetBool(const FDasStructValue& H,const char* P,das::Context* C){FInstancedStruct V=ResolveStructValue(H,C);auto F=StructLeaf(V,P,C);auto* T=CastField<FBoolProperty>(F.Property);if(!T)C->throw_error("struct field scalar type mismatch");return T->GetPropertyValue(F.Memory);}
    FDasStructValue StructWithBool(const FDasStructValue& H,const char* P,bool X,das::Context* C,das::LineInfoArg* At){FInstancedStruct V=ResolveStructValue(H,C);auto F=StructLeaf(V,P,C);auto* T=CastField<FBoolProperty>(F.Property);if(!T)C->throw_error("struct field scalar type mismatch");T->SetPropertyValue(F.Memory,X);return StoreStructValue(V,C,At);}
    char* StructGetString(const FDasStructValue& H,const char* P,das::Context* C){FInstancedStruct V=ResolveStructValue(H,C);auto F=StructLeaf(V,P,C);auto* T=CastField<FStrProperty>(F.Property);if(!T)C->throw_error("struct field scalar type mismatch");return CopyDSLString(T->GetPropertyValue(F.Memory),C);}
    FDasStructValue StructWithString(const FDasStructValue& H,const char* P,const char* X,das::Context* C,das::LineInfoArg* At){FInstancedStruct V=ResolveStructValue(H,C);auto F=StructLeaf(V,P,C);auto* T=CastField<FStrProperty>(F.Property);if(!T)C->throw_error("struct field scalar type mismatch");T->SetPropertyValue(F.Memory,X?UTF8_TO_TCHAR(X):TEXT(""));return StoreStructValue(V,C,At);}
    FDasNameValue StructGetName(const FDasStructValue& H,const char* P,das::Context* C){FInstancedStruct V=ResolveStructValue(H,C);auto F=StructLeaf(V,P,C);auto* T=CastField<FNameProperty>(F.Property);if(!T)C->throw_error("struct field scalar type mismatch");return MakeDSLName(T->GetPropertyValue(F.Memory),C);}
    FDasStructValue StructWithName(const FDasStructValue& H,const char* P,FDasNameValue X,das::Context* C,das::LineInfoArg* At){FInstancedStruct V=ResolveStructValue(H,C);auto F=StructLeaf(V,P,C);auto* T=CastField<FNameProperty>(F.Property);if(!T)C->throw_error("struct field scalar type mismatch");T->SetPropertyValue(F.Memory,ResolveDSLName(X,C));return StoreStructValue(V,C,At);}
    char* StructGetEnum(const FDasStructValue& H,const char* Path,const char* EnumPath,das::Context* C){FInstancedStruct V=ResolveStructValue(H,C);auto F=StructLeaf(V,Path,C);UEnum* E=RequireCallEnum(F.Property,EnumPath,C);int32 Index=E->GetIndexByValue(CallEnumStorage(F.Property)->GetSignedIntPropertyValue(F.Memory));if(!IsCallEnumMember(E,Index))C->throw_error("unknown or reserved enum value in struct");return CopyDSLString(E->GetNameStringByIndex(Index),C);}
    FDasStructValue StructWithEnum(const FDasStructValue& H,const char* Path,const char* EnumPath,const char* Member,das::Context* C,das::LineInfoArg* At){FInstancedStruct V=ResolveStructValue(H,C);auto F=StructLeaf(V,Path,C);UEnum* E=RequireCallEnum(F.Property,EnumPath,C);const FString Symbol=Member?UTF8_TO_TCHAR(Member):TEXT("");if(Symbol.Len()>=NAME_SIZE)C->throw_error("enum member is too long");int32 Index=INDEX_NONE;for(int32 I=0;I<E->NumEnums();++I)if(Symbol.Equals(E->GetNameStringByIndex(I),ESearchCase::IgnoreCase) || Symbol.Equals(E->GetNameByIndex(I).ToString(),ESearchCase::IgnoreCase)){Index=I;break;}if(!IsCallEnumMember(E,Index))C->throw_error("unknown or reserved enum member");CallEnumStorage(F.Property)->SetIntPropertyValue(F.Memory,E->GetValueByIndex(Index));return StoreStructValue(V,C,At);}
    bool VerifyContract(const das::ProgramPtr& Program,const FString& Entry,const FDasCallContract& C)
    {
        if(C.Inputs.Num()>16 || (static_cast<uint8>(C.ReturnKind)>12 || C.ReturnKind==EDasMethodSignature::Frame)) return false;
        const FTCHARToUTF8 N(*Entry);auto* F=Program->getThisModule()->findUniqueFunction(N.Get());
        auto Matches=[](const das::TypeDeclPtr& T,EDasMethodSignature K){if(!T || T->ref || (static_cast<uint8>(K)>12 || K==EDasMethodSignature::Frame))return false;if(K==EDasMethodSignature::Struct)return T->baseType==das::Type::tHandle && T->annotation && T->annotation->name=="UEStructValue";if(K==EDasMethodSignature::Vector || K==EDasMethodSignature::Rotator || K==EDasMethodSignature::Transform)return T->baseType==das::Type::tHandle && T->annotation && T->annotation->name==(K==EDasMethodSignature::Vector?"UEVector":K==EDasMethodSignature::Rotator?"UERotator":"UETransform");if(K==EDasMethodSignature::Object)return T->baseType==das::Type::tHandle && T->annotation && T->annotation->name=="UObjectHandle";if(K==EDasMethodSignature::Name)return T->baseType==das::Type::tHandle && T->annotation && T->annotation->name=="UENameValue";const auto D=K==EDasMethodSignature::String?das::Type::tString:K==EDasMethodSignature::Double?das::Type::tDouble:K==EDasMethodSignature::Int?das::Type::tInt:K==EDasMethodSignature::Float?das::Type::tFloat:K==EDasMethodSignature::Bool?das::Type::tBool:das::Type::tVoid;return K==EDasMethodSignature::Void?T->baseType==D:T->isSimpleType(D);};
        if(!F || !F->exports || F->arguments.size()!=static_cast<size_t>(C.Inputs.Num()) || !Matches(F->result,C.ReturnKind)) return false;
        for(int32 I=0;I<C.Inputs.Num();++I) if(C.Inputs[I]==EDasMethodSignature::Void || !Matches(F->arguments[I]->type,C.Inputs[I])) return false;
        bool Objects=C.ReturnKind==EDasMethodSignature::Object || C.ReturnKind==EDasMethodSignature::Struct;for(auto K:C.Inputs)Objects |= K==EDasMethodSignature::Object || K==EDasMethodSignature::Struct;
        if(Objects){bool Found=false,Valid=true;Program->getThisModule()->structures.foreach([&](const das::StructurePtr& St){for(auto* A:St->annotations)if(A->annotation->name=="ue_class"){auto* Wire=A->arguments.find("_ue_model_json",das::Type::tString);FDasAuthoringModule M;FString Error;if(!Wire || !FDasAuthoringModule::FromJson(UTF8_TO_TCHAR(Wire->sValue.c_str()),M,Error)){Valid=false;continue;}for(const auto& Cl:M.Classes)for(const auto& Method:Cl.Methods)if(Method.EntryPoint==Entry){FDasClassSchema Schema;if(!M.ToActorSchema(Cl.Id,Schema,Error)){Valid=false;continue;}for(const auto& S:Schema.Methods)if(S.EntryPoint==Entry){Found=true;Valid &= S.Contract()==C;}}}});if(!Valid)return false;if(!Found){if(C.ReturnKind==EDasMethodSignature::Struct)return false;for(auto K:C.Inputs)if(K==EDasMethodSignature::Struct)return false;if(C.ReturnKind==EDasMethodSignature::Object && C.ReturnClass!=TEXT("/Script/CoreUObject.Object"))return false;for(int32 I=0;I<C.Inputs.Num();++I)if(C.Inputs[I]==EDasMethodSignature::Object && (!C.InputClasses.IsValidIndex(I) || C.InputClasses[I]!=TEXT("/Script/CoreUObject.Object")))return false;}}
        return true;
    }
    int32 GetSelfInt(const char* Name, das::Context* Context)
    {
        UObject* Self = ResolveSelf(Context);
        return ResolveProperty<FIntProperty>(Self, Name, false, Context)->GetPropertyValue_InContainer(Self);
    }
    float GetSelfFloat(const char* Name, das::Context* Context)
    {
        UObject* Self = ResolveSelf(Context);
        return ResolveProperty<FFloatProperty>(Self, Name, false, Context)->GetPropertyValue_InContainer(Self);
    }
    void SetSelfInt(const char* Name, int32 Value, das::Context* Context)
    {
        if (ActiveContext && ActiveContext->bCollectingSubscriptions) Context->throw_error("self writes are forbidden in bind_events");
        UObject* Self = ResolveSelf(Context);
        ResolveProperty<FIntProperty>(Self, Name, true, Context)->SetPropertyValue_InContainer(Self, Value);
    }
    void SetSelfFloat(const char* Name, float Value, das::Context* Context)
    {
        if (ActiveContext && ActiveContext->bCollectingSubscriptions) Context->throw_error("self writes are forbidden in bind_events");
        UObject* Self = ResolveSelf(Context);
        ResolveProperty<FFloatProperty>(Self, Name, true, Context)->SetPropertyValue_InContainer(Self, Value);
    }

    void RequireObjectContext(das::Context* Context)
    {
        if (!IsInGameThread() || !ActiveContext || Context != ActiveContext)
            Context->throw_error("object handles require the active game-thread context");
    }

    FDasObjectHandle MakeObjectHandle(UObject* Object, UClass* Expected, das::Context* Context)
    {
        RequireObjectContext(Context);
        if (!IsValid(Object) || (Object->IsA<AActor>() && CastChecked<AActor>(Object)->IsActorBeingDestroyed())) return {};
        for (auto It = ActiveContext->ObjectHandles.CreateIterator(); It; ++It)
        {
            if (!It.Value().Object.IsValid() || !It.Value().ExpectedClass.IsValid()) { It.RemoveCurrent(); continue; }
            if (It.Value().Object.Get() == Object && It.Value().ExpectedClass.Get() == Expected) return {It.Key()};
        }
        const uint64 Token = NextObjectHandle++;
        ActiveContext->ObjectHandles.Add(Token, {Object, Expected});
        return {Token};
    }

    UObject* TryObject(const FDasObjectHandle& Handle, das::Context* Context)
    {
        RequireObjectContext(Context);
        const auto* Entry = ActiveContext->ObjectHandles.Find(Handle.Token);
        if (!Entry) return nullptr;
        UObject* Object = Entry->Object.Get();
        UClass* Expected = Entry->ExpectedClass.Get();
        if (!IsValid(Object) || !Expected || !Object->IsA(Expected)) return nullptr;
        if (auto* Actor = Cast<AActor>(Object); Actor && Actor->IsActorBeingDestroyed()) return nullptr;
        return Object;
    }

    UObject* ResolveObject(const FDasObjectHandle& Handle, das::Context* Context)
    {
        UObject* Object = TryObject(Handle, Context);
        if (!Object) Context->throw_error("object handle is null, expired, or belongs to another context");
        return Object;
    }

    FDasObjectHandle NullObject() { return {}; }
    FDasObjectHandle ObjectSelf(das::Context* Context) { return MakeObjectHandle(ResolveSelf(Context), UObject::StaticClass(), Context); }
    bool ObjectValid(const FDasObjectHandle& Handle, das::Context* Context) { return TryObject(Handle, Context) != nullptr; }
    UClass* ResolveClass(const char* Path, das::Context* Context)
    {
        if (!Path || !*Path) Context->throw_error("a loaded full class path is required");
        const FString Name(UTF8_TO_TCHAR(Path));
        if (!Name.StartsWith(TEXT("/")) || !Name.Contains(TEXT("."))) Context->throw_error("use a full loaded class path");
        UClass* Class = FindObject<UClass>(nullptr, *Name);
        if (!Class) Context->throw_error("class is not loaded");
        return Class;
    }
    FDasObjectHandle ObjectValue(const char* Path,das::Context* C){const FString P=Path?UTF8_TO_TCHAR(Path):TEXT("");if(!P.StartsWith(TEXT("/Script/")))C->throw_error("Object field requires a native class path");ResolveClass(Path,C);return {};}
    FObjectProperty* SelfObjectProperty(const char* N,bool Write,das::Context* C){auto* P=ResolveProperty<FObjectProperty>(ResolveSelf(C),N,Write,C);if(CastField<FClassProperty>(P))C->throw_error("class properties are not object fields");return P;}
    FDasObjectHandle GetSelfObject(const char* N,das::Context* C){auto* P=SelfObjectProperty(N,false,C);return MakeObjectHandle(P->GetObjectPropertyValue_InContainer(ResolveSelf(C)),P->PropertyClass,C);}
    void SetSelfObject(const char* N,const FDasObjectHandle& V,das::Context* C){ResolveSelf(C);if(ActiveContext->bCollectingSubscriptions)C->throw_error("self writes are forbidden in bind_events");auto* P=SelfObjectProperty(N,true,C);UObject* O=V.Token?ResolveObject(V,C):nullptr;if(O && !O->IsA(P->PropertyClass))C->throw_error("Object assignment does not match field class");P->SetObjectPropertyValue_InContainer(ResolveSelf(C),O);}
    bool ObjectIsA(const FDasObjectHandle& Handle, const char* ClassPath, das::Context* Context)
    {
        UClass* Class = ResolveClass(ClassPath, Context);
        UObject* Object = TryObject(Handle, Context);
        return Object && Object->IsA(Class);
    }
    FDasObjectHandle ObjectCast(const FDasObjectHandle& Handle, const char* ClassPath, das::Context* Context)
    {
        UObject* Object = ResolveObject(Handle, Context);
        UClass* Class = ResolveClass(ClassPath, Context);
        if (!Object->IsA(Class)) Context->throw_error("object does not match requested class");
        return MakeObjectHandle(Object, Class, Context);
    }
    FDasObjectHandle ObjectCastNullable(const FDasObjectHandle& Handle, const char* ClassPath, das::Context* Context)
    {
        RequireObjectContext(Context);
        UClass* Class = ResolveClass(ClassPath, Context);
        if (!Handle.Token) return {};
        UObject* Object = ResolveObject(Handle, Context);
        if (!Object->IsA(Class)) Context->throw_error("object does not match requested class");
        return MakeObjectHandle(Object, Class, Context);
    }

    FDasObjectHandle ObjectProperty(const FDasObjectHandle& Handle, const char* Name, das::Context* Context)
    {
        UObject* Object = ResolveObject(Handle, Context);
        auto* Property = ResolveProperty<FObjectProperty>(Object, Name, false, Context);
        return MakeObjectHandle(Property->GetObjectPropertyValue_InContainer(Object), Property->PropertyClass, Context);
    }
    int32 ObjectGetInt(const FDasObjectHandle& Handle, const char* Name, das::Context* Context)
    {
        UObject* Object = ResolveObject(Handle, Context);
        return ResolveProperty<FIntProperty>(Object, Name, false, Context)->GetPropertyValue_InContainer(Object);
    }
    void ObjectSetInt(const FDasObjectHandle& Handle, const char* Name, int32 Value, das::Context* Context)
    {
        UObject* Object = ResolveObject(Handle, Context);
        if (ActiveContext->bCollectingSubscriptions) Context->throw_error("object writes are forbidden in bind_events");
        ResolveProperty<FIntProperty>(Object, Name, true, Context)->SetPropertyValue_InContainer(Object, Value);
    }

    enum class ECallKind { None, Int, Float, Object };
    struct FCheckedCall { UFunction* Function; FProperty* Input = nullptr; FProperty* Output = nullptr; };
    bool MatchesCallProperty(FProperty* Property, ECallKind Kind)
    {
        if (!Property || Property->ArrayDim != 1) return false;
        if (Kind == ECallKind::Int) return CastField<FIntProperty>(Property) != nullptr;
        if (Kind == ECallKind::Float) return CastField<FFloatProperty>(Property) != nullptr;
        if (Kind == ECallKind::Object) return CastField<FObjectProperty>(Property) != nullptr && !CastField<FClassProperty>(Property);
        return false;
    }
    FCheckedCall CheckCall(UObject* Object, const char* Name, ECallKind Input, ECallKind Output, das::Context* Context)
    {
        if (ActiveContext->bCollectingSubscriptions) Context->throw_error("UFunction calls are forbidden in bind_events");
        if (!Name || !*Name || FCStringAnsi::Strlen(Name) >= NAME_SIZE) Context->throw_error("invalid function name");
        UFunction* Function = Object->FindFunction(FName(UTF8_TO_TCHAR(Name)));
        if (!Function || !Function->HasAnyFunctionFlags(FUNC_BlueprintCallable) ||
            Function->HasAnyFunctionFlags(FUNC_Static | FUNC_Net | FUNC_Delegate | FUNC_MulticastDelegate))
            Context->throw_error("expected a local BlueprintCallable instance UFunction");
        FCheckedCall Call{Function};
        int32 Inputs = 0;
        for (TFieldIterator<FProperty> It(Function); It; ++It)
        {
            FProperty* Property = *It;
            if (!Property->HasAnyPropertyFlags(CPF_Parm)) continue;
            if (Property->HasAnyPropertyFlags(CPF_ReturnParm)) Call.Output = Property;
            else
            {
                ++Inputs; Call.Input = Property;
                if (Property->HasAnyPropertyFlags(CPF_OutParm | CPF_ReferenceParm)) Context->throw_error("out/ref parameters are not supported");
            }
        }
        if (Inputs != (Input == ECallKind::None ? 0 : 1) ||
            (Input != ECallKind::None && !MatchesCallProperty(Call.Input, Input)) ||
            (Output == ECallKind::None ? Call.Output != nullptr : !MatchesCallProperty(Call.Output, Output)))
            Context->throw_error("UFunction signature does not match the requested call adapter");
        return Call;
    }
    int32 CallInt(const FDasObjectHandle& Handle, const char* Name, int32 Value, das::Context* Context)
    {
        UObject* Object = ResolveObject(Handle, Context);
        const auto Call = CheckCall(Object, Name, ECallKind::Int, ECallKind::Int, Context);
        FStructOnScope Frame(Call.Function);
        CastFieldChecked<FIntProperty>(Call.Input)->SetPropertyValue_InContainer(Frame.GetStructMemory(), Value);
        TStrongObjectPtr<UObject> KeepAlive(Object);
        Object->ProcessEvent(Call.Function, Frame.GetStructMemory());
        return CastFieldChecked<FIntProperty>(Call.Output)->GetPropertyValue_InContainer(Frame.GetStructMemory());
    }
    float CallFloat(const FDasObjectHandle& Handle, const char* Name, float Value, das::Context* Context)
    {
        UObject* Object = ResolveObject(Handle, Context);
        const auto Call = CheckCall(Object, Name, ECallKind::Float, ECallKind::Float, Context);
        FStructOnScope Frame(Call.Function);
        CastFieldChecked<FFloatProperty>(Call.Input)->SetPropertyValue_InContainer(Frame.GetStructMemory(), Value);
        TStrongObjectPtr<UObject> KeepAlive(Object);
        Object->ProcessEvent(Call.Function, Frame.GetStructMemory());
        return CastFieldChecked<FFloatProperty>(Call.Output)->GetPropertyValue_InContainer(Frame.GetStructMemory());
    }
    void CallVoidInt(const FDasObjectHandle& Handle, const char* Name, int32 Value, das::Context* Context)
    {
        UObject* Object = ResolveObject(Handle, Context);
        const auto Call = CheckCall(Object, Name, ECallKind::Int, ECallKind::None, Context);
        FStructOnScope Frame(Call.Function);
        CastFieldChecked<FIntProperty>(Call.Input)->SetPropertyValue_InContainer(Frame.GetStructMemory(), Value);
        TStrongObjectPtr<UObject> KeepAlive(Object);
        Object->ProcessEvent(Call.Function, Frame.GetStructMemory());
    }
    FDasObjectHandle CallObject(const FDasObjectHandle& Handle, const char* Name, das::Context* Context)
    {
        UObject* Object = ResolveObject(Handle, Context);
        const auto Call = CheckCall(Object, Name, ECallKind::None, ECallKind::Object, Context);
        FStructOnScope Frame(Call.Function);
        TStrongObjectPtr<UObject> KeepAlive(Object);
        Object->ProcessEvent(Call.Function, Frame.GetStructMemory());
        auto* Return = CastFieldChecked<FObjectProperty>(Call.Output);
        return MakeObjectHandle(Return->GetObjectPropertyValue_InContainer(Frame.GetStructMemory()), Return->PropertyClass, Context);
    }
    void CallVoidObject(const FDasObjectHandle& Handle, const char* Name, const FDasObjectHandle& Value, das::Context* Context)
    {
        UObject* Object = ResolveObject(Handle, Context);
        const auto Call = CheckCall(Object, Name, ECallKind::Object, ECallKind::None, Context);
        UObject* Argument = Value.Token == 0 ? nullptr : ResolveObject(Value, Context);
        auto* Property = CastFieldChecked<FObjectProperty>(Call.Input);
        if (Argument && !Argument->IsA(Property->PropertyClass)) Context->throw_error("object argument has an incompatible class");
        FStructOnScope Frame(Call.Function);
        Property->SetObjectPropertyValue_InContainer(Frame.GetStructMemory(), Argument);
        TStrongObjectPtr<UObject> KeepAlive(Object);
        TStrongObjectPtr<UObject> KeepArgumentAlive(Argument);
        Object->ProcessEvent(Call.Function, Frame.GetStructMemory());
    }

    #include "DasMarshalling.inl"

    FMulticastDelegateProperty* IntEventProperty(UObject* Source, FName Name)
    {
        auto* Property = FindFProperty<FMulticastDelegateProperty>(Source->GetClass(), Name);
        if (!Property || !Property->HasAnyPropertyFlags(CPF_BlueprintAssignable) || Property->ArrayDim != 1) return nullptr;
        UFunction* Signature = Property->SignatureFunction;
        if (!Signature || Signature->NumParms != 1) return nullptr;
        for (TFieldIterator<FProperty> It(Signature); It; ++It)
        {
            if (It->HasAnyPropertyFlags(CPF_Parm))
                return CastField<FIntProperty>(*It) && It->ArrayDim == 1 &&
                    !It->HasAnyPropertyFlags(CPF_OutParm | CPF_ReturnParm | CPF_ReferenceParm) ? Property : nullptr;
        }
        return nullptr;
    }

    void SubscribeObjectInt(const FDasObjectHandle& Handle, const char* Event, const char* Handler, das::Context* Context)
    {
        RequireObjectContext(Context);
        if (!ActiveContext->bCollectingSubscriptions) Context->throw_error("subscriptions may only be declared in bind_events");
        if (!Event || !*Event || !Handler || !*Handler) Context->throw_error("event and handler are required");
        UObject* Source = ResolveObject(Handle, Context);
        if (!IsValid(Source) || (Source->IsA<AActor>() && CastChecked<AActor>(Source)->IsActorBeingDestroyed()))
            Context->throw_error("subscription source is missing or expired");
        const FString EventName(UTF8_TO_TCHAR(Event));
        if (EventName.Len() >= NAME_SIZE) Context->throw_error("event name is too long");
        const FName Name(*EventName);
        if (!IntEventProperty(Source, Name)) Context->throw_error("expected BlueprintAssignable multicast event with one int32 value");
        for (const auto& Existing : ActiveContext->Subscriptions)
            if (Existing.Source.Get() == Source && Existing.Event == Name && Existing.Handler == UTF8_TO_TCHAR(Handler))
                Context->throw_error("duplicate subscription declaration");
        ActiveContext->Subscriptions.Add({Source, Name, UTF8_TO_TCHAR(Handler)});
    }

    void SubscribeInt(const char* SourceKind, const char* Event, const char* Handler, das::Context* Context)
    {
        RequireObjectContext(Context);
        if (!SourceKind) Context->throw_error("subscription source is required");
        UObject* Source = ResolveSelf(Context);
        const FString Kind(UTF8_TO_TCHAR(SourceKind));
        if (Kind == TEXT("peer"))
        {
            auto* Actor = Cast<ADasProbeActor>(Source);
            Source = Actor ? Actor->Peer.Get() : nullptr;
        }
        else if (Kind != TEXT("self")) Context->throw_error("subscription source must be self or peer");
        SubscribeObjectInt(MakeObjectHandle(Source, UObject::StaticClass(), Context), Event, Handler, Context);
    }

    void EmitSignal(int32 Value, das::Context* Context)
    {
        if (ActiveContext && ActiveContext->bCollectingSubscriptions) Context->throw_error("event emission is forbidden in bind_events");
        auto* Actor = Cast<ADasProbeActor>(ResolveSelf(Context));
        if (!Actor) Context->throw_error("signal emission requires a DasProbeActor");
        Actor->EmitDasSignal(Value);
    }

    struct FObjectHandleAnnotation : das::ManagedStructureAnnotation<FDasObjectHandle, false>
    {
        explicit FObjectHandleAnnotation(das::ModuleLibrary& Library)
            : ManagedStructureAnnotation("UObjectHandle", Library, "FDasObjectHandle") {}
        virtual bool isLocal() const override { return true; }
        virtual bool canCopy() const override { return true; }
        virtual bool canMove() const override { return true; }
        virtual bool hasNonTrivialCtor() const override { return false; }
        virtual bool isPod() const override { return true; }
        virtual bool canBePlacedInContainer() const override { return true; }
    };

    struct FDSLNameAnnotation:das::ManagedStructureAnnotation<FDasNameValue,false>{
        FDSLNameAnnotation(das::ModuleLibrary& L):ManagedStructureAnnotation("UENameValue",L,"FDasNameValue"){}
        virtual bool isRefType() const override{return false;}virtual bool isLocal() const override{return true;}
        virtual bool canCopy() const override{return true;}virtual bool canMove() const override{return true;}
        virtual bool hasNonTrivialCtor() const override{return false;}virtual bool isPod() const override{return true;}
    };
    template<class T> struct FMathValueAnnotation:das::ManagedStructureAnnotation<T,false>{
        FMathValueAnnotation(const char* Name,das::ModuleLibrary& L,const char* Cpp):das::ManagedStructureAnnotation<T,false>(Name,L,Cpp){}
        virtual bool isLocal() const override{return true;}virtual bool canCopy() const override{return true;}virtual bool canMove() const override{return true;}virtual bool hasNonTrivialCtor() const override{return false;}
    };
    struct FDSLVectorAnnotation:FMathValueAnnotation<FVector>{FDSLVectorAnnotation(das::ModuleLibrary& L):FMathValueAnnotation("UEVector",L,"FVector"){addField<DAS_BIND_MANAGED_FIELD(X)>("x");addField<DAS_BIND_MANAGED_FIELD(Y)>("y");addField<DAS_BIND_MANAGED_FIELD(Z)>("z");}};
    struct FDSLRotatorAnnotation:FMathValueAnnotation<FRotator>{FDSLRotatorAnnotation(das::ModuleLibrary& L):FMathValueAnnotation("UERotator",L,"FRotator"){addField<DAS_BIND_MANAGED_FIELD(Pitch)>("pitch");addField<DAS_BIND_MANAGED_FIELD(Yaw)>("yaw");addField<DAS_BIND_MANAGED_FIELD(Roll)>("roll");}};
    struct FReflectedValueAnnotation:FMathValueAnnotation<FDasStructValue>{FReflectedValueAnnotation(das::ModuleLibrary& L):FMathValueAnnotation("UEStructValue",L,"FDasStructValue"){}virtual bool isPod()const override{return true;}};
    #include "DasDSL.inl"

    class FUnrealFoundationModule final : public das::Module
    {
    public:
        FUnrealFoundationModule() : das::Module("ue_foundation")
        {
            addAnnotation(new FUEClassAnnotation());addAnnotation(new FUEEnumAnnotation());addAnnotation(new FUEStructAnnotation());
            addAnnotation(new FUEFunctionAnnotation());
            das::ModuleLibrary Library(this);
            Library.addBuiltInModule();
            addAnnotation(new FDSLNameAnnotation(Library));
            addAnnotation(new FReflectedValueAnnotation(Library));
            addAnnotation(new FDSLVectorAnnotation(Library));addAnnotation(new FDSLRotatorAnnotation(Library));addAnnotation(new FMathValueAnnotation<FTransform>("UETransform",Library,"FTransform"));
            addAnnotation(new FObjectHandleAnnotation(Library));
            das::addExtern<DAS_BIND_FUN(ObjectCastNullable), das::SimNode_ExtFuncCallAndCopyOrMove>(*this, Library, "ue_cast_nullable_object", das::SideEffects::modifyExternal, "ObjectCastNullable");
            addAnnotation(new FCallHandleAnnotation(Library));
            das::addExtern<DAS_BIND_FUN(ExpectCallSignature)>(*this, Library, "ue_expect_signature", das::SideEffects::modifyExternal, "ExpectCallSignature");
            das::addExtern<DAS_BIND_FUN(GetCallEnum)>(*this, Library, "ue_result_enum", das::SideEffects::modifyExternal, "GetCallEnum");
            das::addExtern<DAS_BIND_FUN(SetCallEnum)>(*this, Library, "ue_arg_enum", das::SideEffects::modifyExternal, "SetCallEnum");
            das::addExtern<DAS_BIND_FUN(GetCallName)>(*this, Library, "ue_result_name", das::SideEffects::modifyExternal, "GetCallName");
            das::addExtern<DAS_BIND_FUN(SetCallName)>(*this, Library, "ue_arg_name", das::SideEffects::modifyExternal, "SetCallName");
            das::addExtern<DAS_BIND_FUN(SetCallObject)>(*this, Library, "ue_arg_object", das::SideEffects::modifyExternal, "SetCallObject");
            das::addExtern<DAS_BIND_FUN(GetCallObject), das::SimNode_ExtFuncCallAndCopyOrMove>(*this, Library, "ue_result_object", das::SideEffects::modifyExternal, "GetCallObject");
            das::addExtern<DAS_BIND_FUN(BeginCall), das::SimNode_ExtFuncCallAndCopyOrMove>(*this, Library, "ue_begin_call", das::SideEffects::modifyExternal, "BeginCall");
            das::addExtern<DAS_BIND_FUN(InvokeCall)>(*this, Library, "ue_invoke_call", das::SideEffects::modifyExternal, "InvokeCall");
            das::addExtern<DAS_BIND_FUN(ReleaseCall)>(*this, Library, "ue_release_call", das::SideEffects::modifyExternal, "ReleaseCall");
            das::addExtern<DAS_BIND_FUN(PrepareCallHash)>(*this, Library, "ue_arg_hash", das::SideEffects::modifyExternal, "PrepareCallHash");
            das::addExtern<DAS_BIND_FUN(GetCallHashSize)>(*this, Library, "ue_result_hash_size", das::SideEffects::modifyExternal, "GetCallHashSize");
            das::addExtern<DAS_BIND_FUN(PrepareCallArray)>(*this, Library, "ue_arg_array", das::SideEffects::modifyExternal, "PrepareCallArray");
            das::addExtern<DAS_BIND_FUN(GetCallArraySize)>(*this, Library, "ue_result_array_size", das::SideEffects::modifyExternal, "GetCallArraySize");
            das::addExtern<DAS_BIND_FUN(SetCallInt)>(*this, Library, "ue_arg_int", das::SideEffects::modifyExternal, "SetCallInt");
            das::addExtern<DAS_BIND_FUN(GetCallInt)>(*this, Library, "ue_result_int", das::SideEffects::modifyExternal, "GetCallInt");
            das::addExtern<DAS_BIND_FUN(SetCallFloat)>(*this, Library, "ue_arg_float", das::SideEffects::modifyExternal, "SetCallFloat");
            das::addExtern<DAS_BIND_FUN(GetCallFloat)>(*this, Library, "ue_result_float", das::SideEffects::modifyExternal, "GetCallFloat");
            das::addExtern<DAS_BIND_FUN(SetCallDouble)>(*this, Library, "ue_arg_double", das::SideEffects::modifyExternal, "SetCallDouble");
            das::addExtern<DAS_BIND_FUN(GetCallDouble)>(*this, Library, "ue_result_double", das::SideEffects::modifyExternal, "GetCallDouble");
            das::addExtern<DAS_BIND_FUN(SetCallBool)>(*this, Library, "ue_arg_bool", das::SideEffects::modifyExternal, "SetCallBool");
            das::addExtern<DAS_BIND_FUN(GetCallBool)>(*this, Library, "ue_result_bool", das::SideEffects::modifyExternal, "GetCallBool");
            das::addExtern<DAS_BIND_FUN(SetCallString)>(*this, Library, "ue_arg_string", das::SideEffects::modifyExternal, "SetCallString");
            das::addExtern<DAS_BIND_FUN(GetCallString)>(*this, Library, "ue_result_string", das::SideEffects::modifyExternal, "GetCallString");

            das::addExtern<DAS_BIND_FUN(ObjectValue),das::SimNode_ExtFuncCallAndCopyOrMove>(*this,Library,"ue_object_value",das::SideEffects::modifyExternal,"ObjectValue");
            das::addExtern<DAS_BIND_FUN(GetSelfObject),das::SimNode_ExtFuncCallAndCopyOrMove>(*this,Library,"ue_get_self_object",das::SideEffects::modifyExternal,"GetSelfObject");
            das::addExtern<DAS_BIND_FUN(SetSelfObject)>(*this,Library,"ue_set_self_object",das::SideEffects::modifyExternal,"SetSelfObject");
            das::addExtern<DAS_BIND_FUN(NullObject), das::SimNode_ExtFuncCallAndCopyOrMove>(*this, Library, "ue_null_object", das::SideEffects::none, "NullObject");
            das::addExtern<DAS_BIND_FUN(ObjectSelf), das::SimNode_ExtFuncCallAndCopyOrMove>(*this, Library, "ue_self_object", das::SideEffects::modifyExternal, "ObjectSelf");
            das::addExtern<DAS_BIND_FUN(ObjectProperty), das::SimNode_ExtFuncCallAndCopyOrMove>(*this, Library, "ue_get_object", das::SideEffects::modifyExternal, "ObjectProperty");
            das::addExtern<DAS_BIND_FUN(ObjectCast), das::SimNode_ExtFuncCallAndCopyOrMove>(*this, Library, "ue_cast_object", das::SideEffects::modifyExternal, "ObjectCast");
            das::addExtern<DAS_BIND_FUN(ObjectValid)>(*this, Library, "ue_object_valid", das::SideEffects::accessExternal, "ObjectValid");
            das::addExtern<DAS_BIND_FUN(ObjectIsA)>(*this, Library, "ue_object_is_a", das::SideEffects::accessExternal, "ObjectIsA");
            das::addExtern<DAS_BIND_FUN(ObjectGetInt)>(*this, Library, "ue_object_get_int", das::SideEffects::accessExternal, "ObjectGetInt");
            das::addExtern<DAS_BIND_FUN(ObjectSetInt)>(*this, Library, "ue_object_set_int", das::SideEffects::modifyExternal, "ObjectSetInt");
            das::addExtern<DAS_BIND_FUN(CallInt)>(*this, Library, "ue_call_int", das::SideEffects::modifyExternal, "CallInt");
            das::addExtern<DAS_BIND_FUN(CallFloat)>(*this, Library, "ue_call_float", das::SideEffects::modifyExternal, "CallFloat");
            das::addExtern<DAS_BIND_FUN(CallVoidInt)>(*this, Library, "ue_call_void_int", das::SideEffects::modifyExternal, "CallVoidInt");
            das::addExtern<DAS_BIND_FUN(CallObject), das::SimNode_ExtFuncCallAndCopyOrMove>(*this, Library, "ue_call_object", das::SideEffects::modifyExternal, "CallObject");
            das::addExtern<DAS_BIND_FUN(CallVoidObject)>(*this, Library, "ue_call_void_object", das::SideEffects::modifyExternal, "CallVoidObject");
            das::addExtern<DAS_BIND_FUN(SubscribeObjectInt)>(*this, Library, "ue_subscribe_object_int", das::SideEffects::modifyExternal, "SubscribeObjectInt");

            das::addExtern<DAS_BIND_FUN(SubscribeInt)>(*this, Library, "ue_subscribe_int", das::SideEffects::modifyExternal, "SubscribeInt");
            das::addExtern<DAS_BIND_FUN(EmitSignal)>(*this, Library, "ue_emit_signal", das::SideEffects::modifyExternal, "EmitSignal");
            das::addExtern<DAS_BIND_FUN(GetStateInt)>(*this, Library, "ue_state_get_int", das::SideEffects::modifyExternal, "GetStateInt");
            das::addExtern<DAS_BIND_FUN(SetStateInt)>(*this, Library, "ue_state_set_int", das::SideEffects::modifyExternal, "SetStateInt");
            das::addExtern<DAS_BIND_FUN(NativeAdd)>(*this, Library, "ue_add", das::SideEffects::none, "NativeAdd");
            das::addExtern<DAS_BIND_FUN(DSLVector),das::SimNode_ExtFuncCallAndCopyOrMove>(*this,Library,"ue_vector",das::SideEffects::none,"DSLVector");
            das::addExtern<DAS_BIND_FUN(DSLRotator),das::SimNode_ExtFuncCallAndCopyOrMove>(*this,Library,"ue_rotator",das::SideEffects::none,"DSLRotator");
            das::addExtern<DAS_BIND_FUN(DSLTransform),das::SimNode_ExtFuncCallAndCopyOrMove>(*this,Library,"ue_transform",das::SideEffects::none,"DSLTransform");
            das::addExtern<DAS_BIND_FUN(DSLIdentityTransform),das::SimNode_ExtFuncCallAndCopyOrMove>(*this,Library,"ue_transform_identity",das::SideEffects::none,"DSLIdentityTransform");
            das::addExtern<DAS_BIND_FUN(DSLTransformPosition),das::SimNode_ExtFuncCallAndCopyOrMove>(*this,Library,"ue_transform_position",das::SideEffects::none,"DSLTransformPosition");
            das::addExtern<DAS_BIND_FUN(DSLTranslation),das::SimNode_ExtFuncCallAndCopyOrMove>(*this,Library,"ue_transform_translation",das::SideEffects::none,"DSLTranslation");
            das::addExtern<DAS_BIND_FUN(DSLScale),das::SimNode_ExtFuncCallAndCopyOrMove>(*this,Library,"ue_transform_scale",das::SideEffects::none,"DSLScale");
            das::addExtern<DAS_BIND_FUN(DSLRotation),das::SimNode_ExtFuncCallAndCopyOrMove>(*this,Library,"ue_transform_rotation",das::SideEffects::none,"DSLRotation");
            das::addExtern<DAS_BIND_FUN(ExplicitStructCollect)>(*this,Library,"ue_struct_collect",das::SideEffects::modifyExternal,"ExplicitStructCollect")->needCallerStackFrame=true;
            das::addExtern<DAS_BIND_FUN(StructValueCount)>(*this,Library,"ue_struct_value_count",das::SideEffects::accessExternal,"StructValueCount");
            das::addExtern<DAS_BIND_FUN(NewStructDefault),das::SimNode_ExtFuncCallAndCopyOrMove>(*this,Library,"ue_struct_default",das::SideEffects::modifyExternal,"NewStructDefault")->needCallerStackFrame=true;
            das::addExtern<DAS_BIND_FUN(GetSelfStruct),das::SimNode_ExtFuncCallAndCopyOrMove>(*this,Library,"ue_get_self_struct",das::SideEffects::modifyExternal,"GetSelfStruct")->needCallerStackFrame=true;
            das::addExtern<DAS_BIND_FUN(SetSelfStruct)>(*this,Library,"ue_set_self_struct",das::SideEffects::modifyExternal,"SetSelfStruct");
            das::addExtern<DAS_BIND_FUN(NewStructValue),das::SimNode_ExtFuncCallAndCopyOrMove>(*this,Library,"ue_struct_value",das::SideEffects::modifyExternal,"NewStructValue")->needCallerStackFrame=true;
            das::addExtern<DAS_BIND_FUN(StructGetInt)>(*this,Library,"ue_struct_get_int",das::SideEffects::modifyExternal,"StructGetInt");
            das::addExtern<DAS_BIND_FUN(StructWithInt),das::SimNode_ExtFuncCallAndCopyOrMove>(*this,Library,"ue_struct_with_int",das::SideEffects::modifyExternal,"StructWithInt")->needCallerStackFrame=true;
            das::addExtern<DAS_BIND_FUN(StructGetFloat)>(*this,Library,"ue_struct_get_float",das::SideEffects::modifyExternal,"StructGetFloat");
            das::addExtern<DAS_BIND_FUN(StructWithFloat),das::SimNode_ExtFuncCallAndCopyOrMove>(*this,Library,"ue_struct_with_float",das::SideEffects::modifyExternal,"StructWithFloat")->needCallerStackFrame=true;
            das::addExtern<DAS_BIND_FUN(StructGetDouble)>(*this,Library,"ue_struct_get_double",das::SideEffects::modifyExternal,"StructGetDouble");
            das::addExtern<DAS_BIND_FUN(StructWithDouble),das::SimNode_ExtFuncCallAndCopyOrMove>(*this,Library,"ue_struct_with_double",das::SideEffects::modifyExternal,"StructWithDouble")->needCallerStackFrame=true;
            das::addExtern<DAS_BIND_FUN(StructGetBool)>(*this,Library,"ue_struct_get_bool",das::SideEffects::modifyExternal,"StructGetBool");
            das::addExtern<DAS_BIND_FUN(StructWithBool),das::SimNode_ExtFuncCallAndCopyOrMove>(*this,Library,"ue_struct_with_bool",das::SideEffects::modifyExternal,"StructWithBool")->needCallerStackFrame=true;
            das::addExtern<DAS_BIND_FUN(StructGetString)>(*this,Library,"ue_struct_get_string",das::SideEffects::modifyExternal,"StructGetString");
            das::addExtern<DAS_BIND_FUN(StructWithString),das::SimNode_ExtFuncCallAndCopyOrMove>(*this,Library,"ue_struct_with_string",das::SideEffects::modifyExternal,"StructWithString")->needCallerStackFrame=true;
            das::addExtern<DAS_BIND_FUN(StructGetEnum)>(*this,Library,"ue_struct_get_enum",das::SideEffects::modifyExternal,"StructGetEnum");
            das::addExtern<DAS_BIND_FUN(StructWithEnum),das::SimNode_ExtFuncCallAndCopyOrMove>(*this,Library,"ue_struct_with_enum",das::SideEffects::modifyExternal,"StructWithEnum")->needCallerStackFrame=true;
            das::addExtern<DAS_BIND_FUN(StructGetName)>(*this,Library,"ue_struct_get_name",das::SideEffects::modifyExternal,"StructGetName");
            das::addExtern<DAS_BIND_FUN(StructWithName),das::SimNode_ExtFuncCallAndCopyOrMove>(*this,Library,"ue_struct_with_name",das::SideEffects::modifyExternal,"StructWithName")->needCallerStackFrame=true;
            das::addExtern<DAS_BIND_FUN(DSLNameFromString)>(*this,Library,"ue_name_value",das::SideEffects::modifyExternal,"DSLNameFromString");
            das::addExtern<DAS_BIND_FUN(DSLNameToString)>(*this,Library,"ue_name_text",das::SideEffects::modifyExternal,"DSLNameToString");
            das::addExtern<DAS_BIND_FUN(DSLNamesEqual)>(*this,Library,"ue_names_equal",das::SideEffects::accessExternal,"DSLNamesEqual");
            das::addExtern<DAS_BIND_FUN(GetSelfDouble)>(*this,Library,"ue_get_self_double",das::SideEffects::accessExternal,"GetSelfDouble");
            das::addExtern<DAS_BIND_FUN(SetSelfDouble)>(*this,Library,"ue_set_self_double",das::SideEffects::modifyExternal,"SetSelfDouble");
            das::addExtern<DAS_BIND_FUN(GetSelfString)>(*this,Library,"ue_get_self_string",das::SideEffects::modifyExternal,"GetSelfString");
            das::addExtern<DAS_BIND_FUN(SetSelfString)>(*this,Library,"ue_set_self_string",das::SideEffects::modifyExternal,"SetSelfString");
            das::addExtern<DAS_BIND_FUN(GetSelfName)>(*this,Library,"ue_get_self_name",das::SideEffects::modifyExternal,"GetSelfName");
            das::addExtern<DAS_BIND_FUN(SetSelfName)>(*this,Library,"ue_set_self_name",das::SideEffects::modifyExternal,"SetSelfName");
            das::addExtern<DAS_BIND_FUN(GetSelfBool)>(*this, Library, "ue_get_self_bool", das::SideEffects::accessExternal, "GetSelfBool");
            das::addExtern<DAS_BIND_FUN(SetSelfBool)>(*this, Library, "ue_set_self_bool", das::SideEffects::modifyExternal, "SetSelfBool");
            das::addExtern<DAS_BIND_FUN(GetSelfInt)>(*this, Library, "ue_get_self_int", das::SideEffects::accessExternal, "GetSelfInt");
            das::addExtern<DAS_BIND_FUN(GetSelfFloat)>(*this, Library, "ue_get_self_float", das::SideEffects::accessExternal, "GetSelfFloat");
            das::addExtern<DAS_BIND_FUN(SetSelfInt)>(*this, Library, "ue_set_self_int", das::SideEffects::modifyExternal, "SetSelfInt");
            das::addExtern<DAS_BIND_FUN(SetSelfFloat)>(*this, Library, "ue_set_self_float", das::SideEffects::modifyExternal, "SetSelfFloat");
            das::addExtern<DAS_BIND_FUN(EmitType)>(*this, Library, "ue_emit_type", das::SideEffects::modifyExternal, "EmitType");
        }
    };

    FString Errors(const das::ProgramPtr& Program, das::TextWriter& Output)
    {
        if (Program)
            for (const auto& Error : Program->errors)
                Output << das::reportError(Error.at, Error.what, Error.extra, Error.fixme, Error.cerr);
        const auto Text = Output.str();
        return UTF8_TO_TCHAR(Text.c_str());
    }

    // Declaration order keeps compiler owners alive until the context is destroyed.
    struct FCompiledContext
    {
        FString DSLDeclaration;
        das::ModuleGroup Modules;
        das::FileAccessPtr Access;
        das::ProgramPtr Program;
        TUniquePtr<FUnrealDasContext> Context;
        das::SimFunction* Function = nullptr;
        das::SimFunction* BeginPlay = nullptr;
        das::SimFunction* Tick = nullptr;
        das::SimFunction* EndPlay = nullptr;
    };
    struct FManagedSubscription
    {
        uint64 Id = 0;
        TWeakObjectPtr<UObject> Source;
        FName Event;
        FString Handler;
        TStrongObjectPtr<UDasIntDelegateProxy> Proxy;
        ~FManagedSubscription()
        {
            if (UObject* Object = Source.Get())
                if (auto* Property = IntEventProperty(Object, Event))
                {
                    FScriptDelegate Delegate;
                    Delegate.BindUFunction(Proxy.Get(), GET_FUNCTION_NAME_CHECKED(UDasIntDelegateProxy, Receive));
                    Property->RemoveDelegate(Delegate, Object, Property->ContainerPtrToValuePtr<void>(Object));
                }
        }
    };

    struct FPersistentSession
    {
        TWeakObjectPtr<UObject> Owner;
        TWeakObjectPtr<UWorld> World;
        FString Path;
        FSourceSet AttemptedSources;
        TMap<FString,FDasCallContract> RequiredEntries;
        FString ApprovedDSLDeclaration;FString AttemptedEntry;FDasCallContract AttemptedContract;
        TUniquePtr<FCompiledContext> Live;
        FString ReloadError;
        uint32 Revision = 0;
        bool bBeginDelivered = false;
        bool bEndDelivered = false;
        bool bReleaseRequested = false;
        TArray<TUniquePtr<FManagedSubscription>> Bindings;
#if WITH_EDITOR
        bool bReinstanceOwnerInScope=false,bReinstanceOwnerChanged=false,bReinstanceTickEnabled=false;
        FString ReinstanceLifecyclePath;
#endif
    };
    TArray<TUniquePtr<FPersistentSession>> Sessions;
#if WITH_EDITOR
    TArray<TUniquePtr<FPersistentSession>> PreservedSessions;
    bool bPreservingSessions=false;
#endif
    uint64 NextSubscriptionId = 1;
    struct FPendingIntEvent { uint64 Id; int32 Value; };
    TArray<FPendingIntEvent> PendingEvents;
    bool bDrainingEvents = false;
    void RevokeSession(FPersistentSession& Session)
    {
        Session.bReleaseRequested = true;
        Session.Bindings.Empty();
    }
    void FlushReleasedSessions()
    {
        if (!ActiveContext)
            Sessions.RemoveAll([](const auto& Session) { return Session->bReleaseRequested; });
    }
    void DrainEvents();
    struct FDrainEventsOnExit { ~FDrainEventsOnExit() { DrainEvents(); } };

    void InstallSubscriptions(FPersistentSession& Session)
    {
        for (const auto& Declaration : Session.Live->Context->Subscriptions)
        {
            UObject* Source = Declaration.Source.Get();
            if (!IsValid(Source)) continue;
            auto* Property = IntEventProperty(Source, Declaration.Event);
            if (!Property) continue;
            auto Binding = MakeUnique<FManagedSubscription>();
            Binding->Id = NextSubscriptionId++;
            Binding->Source = Source;
            Binding->Event = Declaration.Event;
            Binding->Handler = Declaration.Handler;
            Binding->Proxy.Reset(NewObject<UDasIntDelegateProxy>());
            Binding->Proxy->SubscriptionId = Binding->Id;
            FScriptDelegate Delegate;
            Delegate.BindUFunction(Binding->Proxy.Get(), GET_FUNCTION_NAME_CHECKED(UDasIntDelegateProxy, Receive));
            Property->AddDelegate(Delegate, Source, Property->ContainerPtrToValuePtr<void>(Source));
            Session.Bindings.Add(MoveTemp(Binding));
        }
    }

    void DrainEvents()
    {
        if (ActiveContext || bDrainingEvents || !IsInGameThread()) return;
#if WITH_EDITOR
        if(bPreservingSessions)return;
#endif
        TGuardValue<bool> Draining(bDrainingEvents, true);
        FlushReleasedSessions();
        int32 Index = 0;
        while (Index < PendingEvents.Num() && Index < 1024)
        {
            FlushReleasedSessions();
            const auto Event = PendingEvents[Index++];
            FPersistentSession* Session = nullptr;
            FString Handler;
            for (const auto& Candidate : Sessions)
            {
                for (const auto& Binding : Candidate->Bindings)
                    if (Binding->Id == Event.Id && Binding->Source.IsValid())
                    { Session = Candidate.Get(); Handler = Binding->Handler; break; }
                if (Session) break;
            }
            if (!Session || !Session->Live || !Session->Owner.IsValid() || Session->bEndDelivered || Session->bReleaseRequested) continue;
            if (auto* Actor = Cast<AActor>(Session->Owner.Get()); Actor && Actor->IsActorBeingDestroyed()) continue;
            auto& Context = *Session->Live->Context;
            Context.restart();
            FActiveContextScope Scope(Context);
            FString Error;
            try
            {
                const FTCHARToUTF8 Name(*Handler);
                auto* Function = Context.findFunction(Name.Get());
                if (!Function) continue; // Candidate validation makes this unreachable for a current binding.
                vec4f Args[] = {das::cast<int32>::from(Event.Value)};
                Context.evalWithCatch(Function, Args);
                if (const char* Exception = Context.getException()) Error = UTF8_TO_TCHAR(Exception);
            }
            catch (const std::exception& Exception) { Error = UTF8_TO_TCHAR(Exception.what()); }
            if (!Error.IsEmpty())
            {
                UE_LOG(LogDasRuntime, Display, TEXT("Event handler %s failed: %s"), *Handler, *Error);
                if (auto* Actor = Cast<ADasProbeActor>(Session->Owner.Get())) Actor->LastDasError = Error;
            }
        }
        if (Index < PendingEvents.Num()) UE_LOG(LogDasRuntime, Display, TEXT("Event dispatch budget exceeded; remaining recursive events dropped."));
        PendingEvents.Reset();
        FlushReleasedSessions();
    }

    void PruneSessions()
    {
        for (const auto& Session : Sessions)
        {
            if (!Session->Owner.IsValid()) RevokeSession(*Session);
            else Session->Bindings.RemoveAll([](const auto& Binding) { return !Binding->Source.IsValid(); });
        }
        FlushReleasedSessions();
    }
}

void FDasRuntimeModule::StartupModule()
{
    Root = FPaths::ConvertRelativePathToFull(IPluginManager::Get().FindPlugin(TEXT("DasForUE"))->GetContentDir() / TEXT("Das"));
    const FTCHARToUTF8 Utf8Root(*Root);
    das::setDasRoot(das::string(Utf8Root.Get(), Utf8Root.Length()));
    NEED_ALL_DEFAULT_MODULES;
    das::Module::Initialize();
    new FUnrealFoundationModule();
    WorldCleanupHandle = FWorldDelegates::OnWorldCleanup.AddLambda([](UWorld* World, bool, bool)
    {
        for (const auto& Session : Sessions)
            if (Session->World.Get() == World) RevokeSession(*Session);
#if WITH_EDITOR
        for (const auto& Session : PreservedSessions)
            if (Session->World.Get() == World) RevokeSession(*Session);
#endif
        FlushReleasedSessions();
    });
    PostGCHandle = FCoreUObjectDelegates::GetPostGarbageCollect().AddStatic(&PruneSessions);
#if WITH_EDITOR
    ObjectsReplacedHandle=FCoreUObjectDelegates::OnObjectsReplaced.AddLambda([](const FCoreUObjectDelegates::FReplacementObjectMap& Map){
        if(!IsInGameThread())return;
        for(auto& Session:PreservedSessions){
            if(UObject* const* New=Map.Find(Session->Owner.GetEvenIfUnreachable())){Session->Owner=*New;Session->World=*New?(*New)->GetWorld():nullptr;Session->bReinstanceOwnerChanged=true;}
            if(Session->Live){auto& C=*Session->Live->Context;if(UObject* const* New=Map.Find(C.Self.GetEvenIfUnreachable()))C.Self=*New;for(auto& Declaration:C.Subscriptions)if(UObject* const* New=Map.Find(Declaration.Source.GetEvenIfUnreachable()))Declaration.Source=*New;for(auto& Pair:C.ObjectHandles){auto& Entry=Pair.Value;if(UObject* const* New=Map.Find(Entry.ExpectedClass.GetEvenIfUnreachable()))Entry.ExpectedClass=Cast<UClass>(*New);if(UObject* const* New=Map.Find(Entry.Object.GetEvenIfUnreachable()))Entry.Object=*New;if(Entry.Object.IsValid() && (!Entry.ExpectedClass.IsValid() || !Entry.Object->IsA(Entry.ExpectedClass.Get())))Entry.Object.Reset();}}
        }
        for(const auto& Session:Sessions)if(Session->Live)for(auto& Pair:Session->Live->Context->ObjectHandles){auto& Entry=Pair.Value;
            if(UObject* const* New=Map.Find(Entry.ExpectedClass.GetEvenIfUnreachable()))Entry.ExpectedClass=Cast<UClass>(*New);
            if(UObject* const* New=Map.Find(Entry.Object.GetEvenIfUnreachable()))Entry.Object=*New;
            if(Entry.Object.IsValid() && (!Entry.ExpectedClass.IsValid() || !Entry.Object->IsA(Entry.ExpectedClass.Get())))Entry.Object.Reset();
        }
    });
#endif
    bInitialized = true;
    UE_LOG(LogDasRuntime, Display, TEXT("daScript initialized; content: %s"), *Root);
}

void FDasRuntimeModule::ShutdownModule()
{
    if (bInitialized)
    {
        FWorldDelegates::OnWorldCleanup.Remove(WorldCleanupHandle);
        FCoreUObjectDelegates::GetPostGarbageCollect().Remove(PostGCHandle);
#if WITH_EDITOR
        FCoreUObjectDelegates::OnObjectsReplaced.Remove(ObjectsReplacedHandle);
#endif
#if WITH_EDITOR
        PreservedSessions.Empty();bPreservingSessions=false;
#endif
        Sessions.Empty();
        PendingEvents.Empty();
        das::Module::Shutdown();
        bInitialized = false;
    }
}

int32 FDasRuntimeModule::StructSnapshotCount(const UObject* Owner) const{int32 Count=0;for(const auto& S:Sessions)if(S->Live && (!Owner || S->Owner.Get()==Owner))Count+=S->Live->Context->StructValues.Num();return Count;}
uint64 FDasRuntimeModule::StructSnapshotsReclaimed(const UObject* Owner) const{uint64 Count=0;for(const auto& S:Sessions)if(S->Live && (!Owner || S->Owner.Get()==Owner))Count+=S->Live->Context->StructReclaimed;return Count;}

FString FDasRuntimeModule::ScriptPath(const FString& Name) const
{
    return Root / TEXT("Scripts") / Name;
}

bool FDasRuntimeModule::ReadReflectedTypes(const FString& Path,FDasReflectedTypes& Out,FString& Error){
 Error.Reset();if(!IsInGameThread() || !bInitialized){Error=TEXT("Type declaration compilation requires initialized game-thread runtime");return false;}try{das::TextWriter Text;das::ModuleGroup Group;auto Access=das::make_smart<das::FsFileAccess>();Access->addFileSystem(new FUnrealFileSystem(),true,false);const FTCHARToUTF8 File(*Path);auto Program=das::compileDaScript(File.Get(),Access,Text,Group);if(!Program || Program->failed()){Error=Errors(Program,Text);return false;}FDasReflectedTypes Model;bool OK=true;auto Append=[&](const das::AnnotationList& List,const char* Name){const auto* A=DSLAnnotation(List,Name);if(!A || !OK)return;const auto* J=A->arguments.find(DSLTypesKey,das::Type::tString);FDasReflectedTypes Part;if(!J || !FDasReflectedTypes::FromJson(DSLText(J->sValue),Part,Error)){OK=false;return;}if(!Model.Id.IsEmpty() && Model.Id!=Part.Id){Error=TEXT("Reflected declarations must share module ID");OK=false;return;}Model.Id=Part.Id;Model.Enums.Append(Part.Enums);Model.Structs.Append(Part.Structs);};Program->getThisModule()->enumerations.foreach([&](const das::EnumerationPtr& E){Append(E->annotations,"ue_enum");});Program->getThisModule()->structures.foreach([&](const das::StructurePtr& S){Append(S->annotations,"ue_struct");});if(!OK || !Model.Validate(Error))return false;Out=MoveTemp(Model);return true;}catch(const std::exception& X){Error=UTF8_TO_TCHAR(X.what());return false;}
}

bool FDasRuntimeModule::ReadAuthoringScript(const FString& Path,FDasAuthoringModule& Out,FString& Error)
{
    Error.Reset();if(!IsInGameThread() || !bInitialized){Error=TEXT("DSL compilation requires initialized runtime on game thread");return false;}
    try
    {
        das::TextWriter Output;das::ModuleGroup Group;auto Access=das::make_smart<das::FsFileAccess>();Access->addFileSystem(new FUnrealFileSystem(),true,false);
        const FTCHARToUTF8 File(*Path);auto Program=das::compileDaScript(File.Get(),Access,Output,Group);
        if(!Program || Program->failed()){Error=Errors(Program,Output);return false;}
        FDasAuthoringModule Candidate;bool OK=true;
        Program->getThisModule()->structures.foreach([&](const das::StructurePtr& St)
        {
            const auto* A=DSLAnnotation(St->annotations,"ue_class");if(!A || !OK) return;
            const auto* Wire=A->arguments.find(DSLModelKey,das::Type::tString);FDasAuthoringModule Part;
            if(!Wire || !FDasAuthoringModule::FromJson(DSLText(Wire->sValue),Part,Error)){OK=false;return;}
            if(!Candidate.Id.IsEmpty() && Candidate.Id!=Part.Id){Error=TEXT("Declarations in a file must share module ID");OK=false;return;}
            Candidate.Id=Part.Id;Candidate.Classes.Append(Part.Classes);
        });
        if(!OK || !Candidate.Validate(Error)) return false;Out=MoveTemp(Candidate);return true;
    }
    catch(const std::exception& E){Error=UTF8_TO_TCHAR(E.what());return false;}
}

bool FDasRuntimeModule::ValidateIntMethod(const FString& Path,FString& Error,const FString& EntryPoint)
{return ValidateScalarMethod(Path,Error,EntryPoint,EDasMethodSignature::Int);}
bool FDasRuntimeModule::ValidateScalarMethod(const FString& Path,FString& Error,const FString& EntryPoint,EDasMethodSignature Signature)
{return ValidateFrameMethod(Path,Error,EntryPoint,FDasCallContract::Scalar(Signature));}
bool FDasRuntimeModule::ValidateFrameMethod(const FString& Path,FString& Error,const FString& EntryPoint,const FDasCallContract& Contract)
{
    Error.Reset();
    if (!IsInGameThread() || !bInitialized) { Error=TEXT("Method preflight requires initialized runtime on game thread");return false; }
    try
    {
        das::TextWriter Output;das::ModuleGroup Modules;
        auto Access=das::make_smart<das::FsFileAccess>();Access->addFileSystem(new FUnrealFileSystem(),true,false);
        const FTCHARToUTF8 Utf8Path(*Path);das::CodeOfPolicies Policies;
        auto Program=das::compileDaScript(Utf8Path.Get(),Access,Output,Modules,Policies);
        if (!Program || Program->failed()) { Error=Errors(Program,Output);return false; }
        if(!VerifyContract(Program,EntryPoint,Contract)){Error=TEXT("Expected exported scalar frame signature by value: ")+EntryPoint;return false;}
        // No simulate/eval: runtime global initializers and the method body are not executed during preflight.
        return true;
    }
    catch (const std::exception& Exception) { Error=UTF8_TO_TCHAR(Exception.what());return false; }
}

FDasExecutionResult FDasRuntimeModule::RunFile(const FString& Path, int32 Input, bool bUseAot, TWeakObjectPtr<UObject> Self)
{
    FDasExecutionResult Result;
    if (!IsInGameThread() || !bInitialized)
    {
        Result.Error = TEXT("daScript calls require an initialized runtime on the game thread.");
        return Result;
    }
#if WITH_EDITOR
    if(bPreservingSessions){Result.Error=TEXT("Runtime callbacks are suspended during class migration");return Result;}
#endif
    FDrainEventsOnExit DrainOnExit;
    try
    {
        das::TextWriter Output;
        das::ModuleGroup Modules;
        auto Access = das::make_smart<das::FsFileAccess>();
        Access->addFileSystem(new FUnrealFileSystem(), true, false);
        const FTCHARToUTF8 Utf8Path(*Path);
        das::CodeOfPolicies Policies;
        Policies.aot = bUseAot;
        auto Program = das::compileDaScript(Utf8Path.Get(), Access, Output, Modules, Policies);
        if (!Program || Program->failed())
        {
            Result.Error = Errors(Program, Output);
            return Result;
        }
        FUnrealDasContext Context(Program->getContextStackSize());
        Context.Self = Self;
        FActiveContextScope ActiveScope(Context);
        if (!Program->simulate(Context, Output))
        {
            Result.Error = Errors(Program, Output);
            return Result;
        }
        auto Function = Context.findFunction("main");
        if (!Function || !das::verifyCall<int32, int32>(Function->debugInfo, Modules))
        {
            Result.Error = TEXT("Expected exported main(input: int): int.");
            return Result;
        }
        vec4f Arguments[] = { das::cast<int32>::from(Input) };
        const vec4f Value = Context.evalWithCatch(Function,Arguments);
        if (const char* Exception = Context.getException())
        {
            Result.Error = UTF8_TO_TCHAR(Exception);
            return Result;
        }
        Result.bUsedAot = Function->aot;
        Result.Value = das::cast<int32>::to(Value);
        Result.Types = MoveTemp(Context.Types);
        Result.bSucceeded = true;
        return Result;
    }
    catch (const std::exception& Exception)
    {
        Result.Error = UTF8_TO_TCHAR(Exception.what());
        return Result;
    }
}

void FDasRuntimeModule::DispatchIntEvent(uint64 SubscriptionId, int32 Value)
{
    if (!IsInGameThread() || !bInitialized) return;
#if WITH_EDITOR
    if(bPreservingSessions)return; // Old proxy IDs are detached and must never enter a VM during migration.
#endif
    if (PendingEvents.Num() < 1024) PendingEvents.Add({SubscriptionId, Value});
    DrainEvents();
}

int32 FDasRuntimeModule::SubscriptionCount(const UObject* Owner) const
{
    check(IsInGameThread());
    int32 Count = 0;
    for (const auto& Session : Sessions)
        if (!Owner || Session->Owner.Get() == Owner) Count += Session->Bindings.Num();
    return Count;
}

void FDasRuntimeModule::ReleaseObject(UObject* Object)
{
    check(IsInGameThread());
    for (const auto& Session : Sessions)
        if (Session->Owner.Get(true) == Object) RevokeSession(*Session);
    FlushReleasedSessions();
}

int32 FDasRuntimeModule::PersistentContextCount(const UWorld* World) const
{
    check(IsInGameThread());
    int32 Count = 0;
    for (const auto& Session : Sessions)
        if (Session->Live && Session->Owner.IsValid() && (!World || Session->World.Get() == World)) ++Count;
    return Count;
}

FDasExecutionResult FDasRuntimeModule::RunNamedFramePersistent(const FString& Path,const FString& Entry,const TArray<FDasScalarValue>& Args,EDasMethodSignature ResultKind,TWeakObjectPtr<UObject> Self,const FString& ReturnClass)
{return RunCallback(Path,EDasCallback::Main,0,0.f,Self,Entry,ResultKind,&Args,ReturnClass);}
FDasExecutionResult FDasRuntimeModule::RunNamedFloatPersistent(const FString& Path,const FString& Entry,float Input,TWeakObjectPtr<UObject> Self)
{return RunCallback(Path,EDasCallback::Main,0,Input,Self,Entry,EDasMethodSignature::Float);}
FDasExecutionResult FDasRuntimeModule::RunNamedBoolPersistent(const FString& Path,const FString& Entry,bool Input,TWeakObjectPtr<UObject> Self)
{return RunCallback(Path,EDasCallback::Main,Input?1:0,0.f,Self,Entry,EDasMethodSignature::Bool);}
FDasExecutionResult FDasRuntimeModule::RunNamedVoidPersistent(const FString& Path,const FString& Entry,TWeakObjectPtr<UObject> Self)
{return RunCallback(Path,EDasCallback::Main,0,0.f,Self,Entry,EDasMethodSignature::Void);}

FDasExecutionResult FDasRuntimeModule::RunNamedPersistent(const FString& Path, const FString& EntryPoint, int32 Input, TWeakObjectPtr<UObject> Self)
{
    if (EntryPoint.IsEmpty() || EntryPoint.Len()>=NAME_SIZE || FChar::IsDigit(EntryPoint[0]))
    { FDasExecutionResult Result;Result.Error=TEXT("Invalid entry point name");return Result; }
    for (TCHAR Ch:EntryPoint) if (!FChar::IsAlnum(Ch) && Ch!='_')
    { FDasExecutionResult Result;Result.Error=TEXT("Invalid entry point name");return Result; }
    return RunCallback(Path,EDasCallback::Main,Input,0.f,Self,EntryPoint);
}

FDasExecutionResult FDasRuntimeModule::RunPersistent(const FString& Path, int32 Input, TWeakObjectPtr<UObject> Self)
{
    return RunCallback(Path, EDasCallback::Main, Input, 0.f, Self);
}

FDasExecutionResult FDasRuntimeModule::RunLifecycle(const FString& Path, EDasCallback Event, TWeakObjectPtr<UObject> Self, float DeltaSeconds, int32 EndReason)
{
    if (Event == EDasCallback::Main)
    {
        FDasExecutionResult Result;
        Result.Error = TEXT("Use RunPersistent for main.");
        return Result;
    }
    return RunCallback(Path, Event, EndReason, DeltaSeconds, Self);
}

FDasExecutionResult FDasRuntimeModule::RunCallback(const FString& Path, EDasCallback Event, int32 Input, float DeltaSeconds, TWeakObjectPtr<UObject> Self, const FString& EntryPoint, EDasMethodSignature Signature,const TArray<FDasScalarValue>* Frame,const FString& ReturnClass)
{
    FDasExecutionResult Result;
    if(Event==EDasCallback::Main)
    {
        bool Valid=!EntryPoint.IsEmpty() && EntryPoint.Len()<NAME_SIZE && !FChar::IsDigit(EntryPoint[0]);
        for(TCHAR Ch:EntryPoint) Valid &= FChar::IsAlnum(Ch) || Ch==TEXT('_');
        if(!Valid){Result.Error=TEXT("Invalid entry point name");return Result;}
    }
    if (!IsInGameThread() || !bInitialized || ActiveContext)
    {
        Result.Error = TEXT("Persistent calls require an initialized, idle runtime on the game thread.");
        return Result;
    }
#if WITH_EDITOR
    if(bPreservingSessions){
        // Native teardown/reconstruction is not a gameplay lifecycle transition.
        Result.bSucceeded=Event!=EDasCallback::Main;
        if(!Result.bSucceeded)Result.Error=TEXT("Runtime callbacks are suspended during class migration");
        return Result;
    }
#endif
    FDasCallContract Expected=FDasCallContract::Scalar(Signature);
    if(Frame){Expected.Inputs.Reset();if(Frame->Num()>16 || (static_cast<uint8>(Signature)>12 || Signature==EDasMethodSignature::Frame)){Result.Error=TEXT("Invalid scalar frame");return Result;}for(const auto& V:*Frame){if(V.Kind<0 || V.Kind>12 || V.Kind==3 || V.Kind==4){Result.Error=TEXT("Invalid scalar argument kind");return Result;}if(V.Kind==5)for(int32 I=0;I<V.StringValue.Len();++I)if(V.StringValue[I]==0){Result.Error=TEXT("embedded NUL is not supported in strings");return Result;}if((V.Kind==9 && V.VectorValue.ContainsNaN()) || (V.Kind==10 && V.RotatorValue.ContainsNaN()) || (V.Kind==11 && (V.TransformValue.ContainsNaN() || !V.TransformValue.IsRotationNormalized()))){Result.Error=TEXT("Invalid/non-finite math struct input");return Result;}Expected.Inputs.Add(static_cast<EDasMethodSignature>(V.Kind));}}
    TArray<TStrongObjectPtr<UObject>> ObjectPins;
    if(Frame){bool Objects=false;for(const auto& V:*Frame)Objects |= V.Kind==8 || V.Kind==12;
        if(Objects)for(const auto& V:*Frame){const FString PathClass=V.Kind==12?V.ObjectClass:V.Kind==8?(V.ObjectClass.IsEmpty()?TEXT("/Script/CoreUObject.Object"):V.ObjectClass):FString();Expected.InputClasses.Add(PathClass);if(V.Kind==8){UClass* Class=FindObject<UClass>(nullptr,*PathClass);UObject* Object=V.ObjectValue.Get();if(!Class || (Object && (!IsValid(Object) || !Object->IsA(Class) || (Object->IsA<AActor>() && CastChecked<AActor>(Object)->IsActorBeingDestroyed())))){Result.Error=TEXT("Object argument is expired or has wrong class");return Result;}if(Object)ObjectPins.Emplace(Object);}}
    }
    if(Frame)for(const auto& V:*Frame)if(V.Kind==12){FString E;if(!V.StructValue.IsValid() || V.StructValue.GetScriptStruct()->GetPathName()!=V.ObjectClass || !ValidateDasStructSnapshot(V.StructValue,E)){Result.Error=TEXT("Invalid reflected struct input: ")+E;return Result;}}
    if(Signature==EDasMethodSignature::Struct){Expected.ReturnClass=ReturnClass;FString E;if(!ValidateDasValueStruct(ReturnClass,E)){Result.Error=E;return Result;}}
    if(Signature==EDasMethodSignature::Object){Expected.ReturnClass=ReturnClass.IsEmpty()?TEXT("/Script/CoreUObject.Object"):ReturnClass;if(!FindObject<UClass>(nullptr,*Expected.ReturnClass)){Result.Error=TEXT("Object result class is not loaded");return Result;}}
    FDrainEventsOnExit DrainOnExit;
    UObject* Owner = Self.Get();
    if (!IsValid(Owner) || (Owner->IsA<AActor>() && CastChecked<AActor>(Owner)->IsActorBeingDestroyed() && Event != EDasCallback::EndPlay))
    {
        Result.Error = TEXT("self is missing or expired");
        return Result;
    }
    PruneSessions();
    FString FullPath = FPaths::ConvertRelativePathToFull(Path);
    FPaths::NormalizeFilename(FullPath);
    FPaths::CollapseRelativeDirectories(FullPath);
    FPersistentSession* Session = nullptr;
    for (const auto& Existing : Sessions)
        if (Existing->Owner == Self && Existing->Path == FullPath) { Session = Existing.Get(); break; }
    if (!Session && Event == EDasCallback::EndPlay)
    {
        Result.bSucceeded = true; // Never start a script during teardown.
        return Result;
    }
    if (!Session)
    {
        auto NewSession = MakeUnique<FPersistentSession>();
        NewSession->Owner = Self;
        NewSession->World = Owner->GetWorld();
        NewSession->Path = FullPath;
        Session = NewSession.Get();
        Sessions.Add(MoveTemp(NewSession));
    }
    if (Session->bEndDelivered)
    {
        Result.bSucceeded = Event == EDasCallback::EndPlay;
        if (!Result.bSucceeded) Result.Error = TEXT("Script lifecycle has already ended.");
        Result.Revision = Session->Revision;
        return Result;
    }
    bool bChanged = Session->AttemptedSources.IsEmpty() || (!Session->Live && (Session->AttemptedEntry!=EntryPoint || Session->AttemptedContract!=Expected));
    for (const auto& Source : Session->AttemptedSources)
        if (!Source.Value.Matches(Source.Key)) { bChanged = true; break; }
    if (bChanged && Event != EDasCallback::EndPlay)
    {
        // Rejected sources are remembered too: no compile storm for an unchanged broken file.
        Session->AttemptedEntry=EntryPoint;Session->AttemptedContract=Expected;
        Session->AttemptedSources.Reset();
        Session->AttemptedSources.Add(FullPath, FSourceSnapshot::Read(FullPath));
        FString CandidateError;
        auto Candidate = MakeUnique<FCompiledContext>();
        try
        {
            das::TextWriter Output;
            auto Access = das::make_smart<das::FsFileAccess>();
            Access->addFileSystem(new FUnrealFileSystem(&Session->AttemptedSources), true, false);
            Candidate->Access = Access;
            const FTCHARToUTF8 Utf8Path(*FullPath);
            Candidate->Program = das::compileDaScript(Utf8Path.Get(), Access, Output, Candidate->Modules);
            if (!Candidate->Program || Candidate->Program->failed()) CandidateError = Errors(Candidate->Program, Output);
            else if(!DSLValidateOwner(Candidate->Program,EntryPoint,Self.Get(),Candidate->DSLDeclaration,CandidateError)) {}
            else if(Session->Live && Session->Live->DSLDeclaration!=Candidate->DSLDeclaration && (Session->ApprovedDSLDeclaration.IsEmpty() || Session->ApprovedDSLDeclaration!=Candidate->DSLDeclaration))
                CandidateError=TEXT("DSL declaration changed; apply/reimport the Blueprint before runtime reload");
            else
            {
                Candidate->Context = MakeUnique<FUnrealDasContext>(Candidate->Program->getContextStackSize());
                FActiveContextScope Scope(*Candidate->Context);
                // No self during initialization: rejected code cannot alter the live Actor.
                if (!Candidate->Program->simulate(*Candidate->Context, Output))
                {
                    CandidateError = Errors(Candidate->Program, Output);
                    if (CandidateError.IsEmpty()) CandidateError = TEXT("Candidate initialization failed.");
                }
                else
                {
                    Candidate->Function = Candidate->Context->findFunction("main");
                    Candidate->BeginPlay = Candidate->Context->findFunction("begin_play");
                    Candidate->Tick = Candidate->Context->findFunction("tick");
                    Candidate->EndPlay = Candidate->Context->findFunction("end_play");
                    const FTCHARToUTF8 EntryUtf8(*EntryPoint);
                    auto* Selected=Event==EDasCallback::Main?Candidate->Context->findFunction(EntryUtf8.Get()):Candidate->Function;
                    if ((Event==EDasCallback::Main && !Selected) || (Selected && !VerifyContract(Candidate->Program,Event==EDasCallback::Main?EntryPoint:TEXT("main"),Event==EDasCallback::Main?Expected:FDasCallContract::Scalar(EDasMethodSignature::Int))))
                        CandidateError=TEXT("Expected exported ")+EntryPoint+TEXT(" with requested scalar signature.");
                    else if (Candidate->BeginPlay && !das::verifyCall<void>(Candidate->BeginPlay->debugInfo, Candidate->Modules))
                        CandidateError = TEXT("Expected exported begin_play(): void.");
                    else if (Candidate->Tick && !das::verifyCall<void, float>(Candidate->Tick->debugInfo, Candidate->Modules))
                        CandidateError = TEXT("Expected exported tick(delta_time: float): void.");
                    else if (Candidate->EndPlay && !das::verifyCall<void, int32>(Candidate->EndPlay->debugInfo, Candidate->Modules))
                        CandidateError = TEXT("Expected exported end_play(reason: int): void.");
                    if (CandidateError.IsEmpty())
                        for (const auto& Required:Session->RequiredEntries)
                        {
                            const FTCHARToUTF8 RequiredUtf8(*Required.Key);
                            auto* RequiredFunction=Candidate->Context->findFunction(RequiredUtf8.Get());
                            if (!RequiredFunction || !VerifyContract(Candidate->Program,Required.Key,Required.Value))
                            {CandidateError=TEXT("Previously used entry point signature changed: ")+Required.Key;break;}
                        }
                }
                if (CandidateError.IsEmpty())
                {
                    if (Session->Live) Candidate->Context->PersistentInts = Session->Live->Context->PersistentInts;
                    Candidate->Context->Self = Self;
                    auto* BindEvents = Candidate->Context->findFunction("bind_events");
                    if (BindEvents)
                    {
                        if (!das::verifyCall<void>(BindEvents->debugInfo, Candidate->Modules))
                            CandidateError = TEXT("Expected exported bind_events(): void.");
                        else
                        {
                            TGuardValue<bool> Collect(Candidate->Context->bCollectingSubscriptions, true);
                            Candidate->Context->evalWithCatch(BindEvents, nullptr);
                        }
                    }
                    for (const auto& Declaration : Candidate->Context->Subscriptions)
                    {
                        const FTCHARToUTF8 Name(*Declaration.Handler);
                        auto* Handler = Candidate->Context->findFunction(Name.Get());
                        if (!Handler || !das::verifyCall<void, int32>(Handler->debugInfo, Candidate->Modules))
                            CandidateError = TEXT("Expected exported event handler(value: int): void: ") + Declaration.Handler;
                    }
                }
                if (const char* Exception = Candidate->Context->getException()) CandidateError = UTF8_TO_TCHAR(Exception);
            }
        }
        catch (const std::exception& Exception) { CandidateError = UTF8_TO_TCHAR(Exception.what()); }
        if (CandidateError.IsEmpty() && Candidate->Context && Candidate->Program && !Candidate->Program->failed())
        {
            Session->Bindings.Empty(); // Revoke old IDs before destroying any old VM state.
            Candidate->Context->Self = Self;
            Session->Live = MoveTemp(Candidate);
            Session->ApprovedDSLDeclaration.Empty();
            InstallSubscriptions(*Session);
            ++Session->Revision;
            Session->ReloadError.Empty();
        }
        else
        {
            Session->ReloadError = CandidateError.IsEmpty() ? TEXT("Candidate initialization failed.") : CandidateError;
            UE_LOG(LogDasRuntime, Display, TEXT("Reload rejected for %s; keeping revision %u: %s"), *FullPath, Session->Revision, *Session->ReloadError);
        }
    }
    Result.Revision = Session->Revision;
    Result.ReloadError = Session->ReloadError;
    if (Event == EDasCallback::EndPlay && (!Session->Live || !Session->bBeginDelivered || Session->bEndDelivered))
    {
        Session->Bindings.Empty();
        Session->bEndDelivered = true;
        Result.bSucceeded = true;
        return Result;
    }
    if (!Session->Live) { Result.Error = Session->ReloadError; return Result; }
    if (Session->bEndDelivered)
    {
        Result.Error = TEXT("Script lifecycle has already ended.");
        return Result;
    }
    auto& Context = *Session->Live->Context;
    Context.restart();
    Context.Types.Reset();
    FActiveContextScope Scope(Context);
    FStructCollectOnExit CollectStructs{Context};
    TGuardValue<bool> EndingScope(Context.bEnding, Event == EDasCallback::EndPlay);
    try
    {
        // Delivery flags belong to the session, so swapping code cannot replay BeginPlay.
        if ((Event == EDasCallback::BeginPlay || Event == EDasCallback::Tick) && !Session->bBeginDelivered)
        {
            Session->bBeginDelivered = true;
            if (Session->Live->BeginPlay) Context.evalWithCatch(Session->Live->BeginPlay, nullptr);
            if (const char* Exception = Context.getException())
            {
                Result.Error = UTF8_TO_TCHAR(Exception);
                return Result;
            }
        }
        das::SimFunction* Function = nullptr;
        vec4f Arguments[] = { Event==EDasCallback::Tick || (Event==EDasCallback::Main && Signature==EDasMethodSignature::Float)?das::cast<float>::from(DeltaSeconds):Event==EDasCallback::Main && Signature==EDasMethodSignature::Bool?das::cast<bool>::from(Input!=0):das::cast<int32>::from(Input) };
        if (Event == EDasCallback::Main)
        {
            const FTCHARToUTF8 EntryUtf8(*EntryPoint);
            Function=Context.findFunction(EntryUtf8.Get());
            if (!Function || !VerifyContract(Session->Live->Program,EntryPoint,Expected))
            { Result.Error=TEXT("Expected exported ")+EntryPoint+TEXT(" with requested scalar signature.");return Result; }
            Session->RequiredEntries.Add(EntryPoint,Expected);
        }
        else if (Event == EDasCallback::Tick) Function = Session->Live->Tick;
        else if (Event == EDasCallback::EndPlay)
        {
            Session->bEndDelivered = true;
            Session->Bindings.Empty();
            Function = Session->Live->EndPlay;
        }
        if (Function)
        {
            das::vector<vec4f> Packed;TArray<FDasObjectHandle> ObjectArgs;if(Frame)ObjectArgs.Reserve(Frame->Num());FDasStructValue ReturnedStructToken;TArray<FDasStructValue> StructArgs;if(Frame)StructArgs.Reserve(Frame->Num());FDasObjectHandle ReturnedObject;TArray<FVector> VectorArgs;TArray<FRotator> RotatorArgs;TArray<FTransform> TransformArgs;if(Frame){VectorArgs.Reserve(Frame->Num());RotatorArgs.Reserve(Frame->Num());TransformArgs.Reserve(Frame->Num());}FVector ReturnedVector=FVector::ZeroVector;FRotator ReturnedRotator=FRotator::ZeroRotator;FTransform ReturnedTransform=FTransform::Identity;
            if(Frame) for(const auto& V:*Frame){
                if(V.Kind==0)Packed.push_back(das::cast<int32>::from(V.IntValue));else if(V.Kind==1)Packed.push_back(das::cast<float>::from(V.FloatValue));else if(V.Kind==2)Packed.push_back(das::cast<bool>::from(V.BoolValue));
                else if(V.Kind==5)Packed.push_back(das::cast<char*>::from(CopyDSLString(V.StringValue,&Context)));else if(V.Kind==6)Packed.push_back(das::cast<double>::from(V.DoubleValue));else if(V.Kind==7)Packed.push_back(das::cast<FDasNameValue>::from(MakeDSLName(V.NameValue,&Context)));else if(V.Kind==9){VectorArgs.Add(V.VectorValue);Packed.push_back(das::cast<void*>::from(&VectorArgs.Last()));}else if(V.Kind==10){RotatorArgs.Add(V.RotatorValue);Packed.push_back(das::cast<void*>::from(&RotatorArgs.Last()));}else if(V.Kind==12){StructArgs.Add(StoreStructValue(V.StructValue,&Context));Packed.push_back(das::cast<void*>::from(&StructArgs.Last()));}else if(V.Kind==11){TransformArgs.Add(V.TransformValue);Packed.push_back(das::cast<void*>::from(&TransformArgs.Last()));}else{const FString ClassPath=V.ObjectClass.IsEmpty()?TEXT("/Script/CoreUObject.Object"):V.ObjectClass;ObjectArgs.Add(MakeObjectHandle(V.ObjectValue.Get(),FindObject<UClass>(nullptr,*ClassPath),&Context));Packed.push_back(das::cast<void*>::from(&ObjectArgs.Last()));}
            }
            const auto Value = Context.evalWithCatch(Function,Frame?(Packed.empty()?nullptr:Packed.data()):Event==EDasCallback::Main && Signature==EDasMethodSignature::Void?nullptr:Arguments,Signature==EDasMethodSignature::Struct?static_cast<void*>(&ReturnedStructToken):Signature==EDasMethodSignature::Object?static_cast<void*>(&ReturnedObject):Signature==EDasMethodSignature::Vector?static_cast<void*>(&ReturnedVector):Signature==EDasMethodSignature::Rotator?static_cast<void*>(&ReturnedRotator):Signature==EDasMethodSignature::Transform?static_cast<void*>(&ReturnedTransform):nullptr);
            if(Event==EDasCallback::Main){
                if(Signature==EDasMethodSignature::Struct && !Context.getException()){const auto& S=ResolveStructValue(ReturnedStructToken,&Context);if(S.GetScriptStruct()->GetPathName()!=Expected.ReturnClass){Result.Error=TEXT("Reflected struct result type mismatch");return Result;}Result.StructValue=S;}else if(Signature==EDasMethodSignature::Vector)Result.VectorValue=ReturnedVector;else if(Signature==EDasMethodSignature::Rotator)Result.RotatorValue=ReturnedRotator;else if(Signature==EDasMethodSignature::Transform)Result.TransformValue=ReturnedTransform;else if(Signature==EDasMethodSignature::Int) Result.Value=das::cast<int32>::to(Value);
                else if(Signature==EDasMethodSignature::Float) Result.FloatValue=das::cast<float>::to(Value);
                else if(Signature==EDasMethodSignature::Bool) Result.BoolValue=das::cast<bool>::to(Value);
                else if(!Context.getException() && Signature==EDasMethodSignature::String){const auto* Text=das::cast<char*>::to(Value);Result.StringValue=Text?UTF8_TO_TCHAR(Text):TEXT("");}
                else if(Signature==EDasMethodSignature::Double)Result.DoubleValue=das::cast<double>::to(Value);
                else if(!Context.getException() && Signature==EDasMethodSignature::Name)Result.NameValue=ResolveDSLName(das::cast<FDasNameValue>::to(Value),&Context);
                else if(!Context.getException() && Signature==EDasMethodSignature::Object){UObject* Object=ReturnedObject.Token?TryObject(ReturnedObject,&Context):nullptr;if(ReturnedObject.Token && !Object){Result.Error=TEXT("Object result is expired or belongs to another context");return Result;}UClass* Class=FindObject<UClass>(nullptr,*Expected.ReturnClass);if(Object && !Object->IsA(Class)){Result.Error=TEXT("Object result has wrong class");return Result;}Result.ObjectValue=Object;}
            }
        }
        if((Signature==EDasMethodSignature::Vector && Result.VectorValue.ContainsNaN()) || (Signature==EDasMethodSignature::Rotator && Result.RotatorValue.ContainsNaN()) || (Signature==EDasMethodSignature::Transform && (Result.TransformValue.ContainsNaN() || !Result.TransformValue.IsRotationNormalized()))){Result.Error=TEXT("Invalid/non-finite math struct result");return Result;}
        if (const char* Exception = Context.getException()) Result.Error = UTF8_TO_TCHAR(Exception);
        else { Result.Types = Context.Types; Result.bSucceeded = true; }
    }
    catch (const std::exception& Exception) { Result.Error = UTF8_TO_TCHAR(Exception.what()); }
    return Result;
}

IMPLEMENT_MODULE(FDasRuntimeModule, DasRuntime)

#include "DasBindingExport.inl"

#if WITH_EDITOR
bool FDasRuntimeModule::BeginPreserveContextsForReinstance(UClass* RootClass){
    if(!bInitialized || !IsInGameThread() || ActiveContext || bDrainingEvents || bPreservingSessions || !RootClass || !PendingEvents.IsEmpty())return false;
    PruneSessions();
    // Call frames are invocation-local and own reflected addresses. Require an
    // empty table at this idle boundary before any host layout can change.
    for(const auto& S:Sessions)if(S->Live && !S->Live->Context->Calls.IsEmpty())return false;
    bPreservingSessions=true;
    // Suspend all contexts: an unaffected owner can subscribe to a replaced peer.
    PreservedSessions=MoveTemp(Sessions);
    for(auto& S:PreservedSessions){
        S->bReinstanceOwnerInScope=S->Owner.IsValid() && S->Owner->IsA(RootClass);
        S->bReinstanceOwnerChanged=false;S->ReinstanceLifecyclePath.Empty();
        if(auto* A=Cast<ADasProbeActor>(S->Owner.Get())){
            if(A->bLifecycleActive)S->ReinstanceLifecyclePath=A->ActiveLifecyclePath;
            S->bReinstanceTickEnabled=A->IsActorTickEnabled();
        }
        S->Bindings.Empty(); // Remove old proxies before Unreal copies delegate properties.
    }
    return true;
}
void FDasRuntimeModule::EndPreserveContextsForReinstance(const FString& ApprovedDeclaration){
    if(!IsInGameThread() || !bPreservingSessions)return;
    for(auto& Saved:PreservedSessions)if(Saved->Owner.IsValid() && !Saved->bReleaseRequested){
        if(auto* Actor=Cast<ADasProbeActor>(Saved->Owner.Get())){
            if(Actor->IsActorBeingDestroyed())continue;
            if(Saved->bReinstanceOwnerChanged && !Saved->ReinstanceLifecyclePath.IsEmpty() && !Saved->bEndDelivered){
                Actor->ActiveLifecyclePath=Saved->ReinstanceLifecyclePath;Actor->bLifecycleActive=true;
                Actor->SetActorTickEnabled(Saved->bReinstanceTickEnabled);
            }
        }
        if(Saved->bReinstanceOwnerInScope)Saved->ApprovedDSLDeclaration=ApprovedDeclaration;
        if(Saved->bReinstanceOwnerInScope || Saved->bReinstanceOwnerChanged)Saved->AttemptedSources.Reset();
        if(Saved->Live && !Saved->bEndDelivered)InstallSubscriptions(*Saved);
        Saved->ReinstanceLifecyclePath.Empty();Sessions.Add(MoveTemp(Saved));
    }
    PreservedSessions.Empty();bPreservingSessions=false;
}
#endif
