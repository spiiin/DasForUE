#include "DasAuthoredInterfaceClass.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "AssetRegistry/AssetData.h"
#include "UObject/UObjectIterator.h"
#include "DasDefinedEnum.h"
#include "DasTypeRegistry.h"
#include "DasRuntime.h"
#include "DasContainerTypes.h"
#include "DasFrameLibrary.h"
#include <type_traits>
#include "DasReflectedTypes.h"
#include "DasAuthoring.h"
#include "Serialization/JsonSerializer.h"
#include "Dom/JsonObject.h"
#include "Misc/EngineVersion.h"
#include "DasProbeActor.h"
#include "DasScriptComponent.h"
#include "DasIntDelegateProxy.h"
#include "UObject/StrongObjectPtr.h"
#include "UObject/GCObject.h"
#include "UObject/StructOnScope.h"
#include "GameFramework/Actor.h"
#include "Components/ActorComponent.h"
#include "Engine/World.h"
#include "UObject/UObjectGlobals.h"
#include "UObject/UnrealType.h"
#include "Interfaces/IPluginManager.h"
#include "Misc/FileHelper.h"
#include "Misc/ScopeExit.h"
#include "Misc/SecureHash.h"
#include "Misc/Paths.h"
#include "Misc/PackageName.h"
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
struct FDasEnumValue {uint64 Token=0;};
MAKE_TYPE_FACTORY(UEEnumValue,FDasEnumValue);
namespace das {template<> struct cast<FDasEnumValue>{static vec4f from(FDasEnumValue V){return cast<uint64_t>::from(V.Token);}static FDasEnumValue to(vec4f V){return {cast<uint64_t>::to(V)};}};}
struct FDasNameValue { uint64 Token=0; };
MAKE_TYPE_FACTORY(UENameValue,FDasNameValue);
namespace das {template<> struct cast<FDasNameValue>{static vec4f from(FDasNameValue V){return cast<uint64_t>::from(V.Token);}static FDasNameValue to(vec4f V){return {cast<uint64_t>::to(V)};}};}
// Keep native returns and the annotation's uint64 value representation in the same ABI slot.
namespace das {
template<> struct ext_ret_carrier<FDasEnumValue> {using type=uint64_t;};
template<> struct ext_ret_to_carrier<FDasEnumValue,uint64_t> {static uint64_t from(FDasEnumValue V){return V.Token;}};
template<> struct ext_ret_carrier<FDasNameValue> {using type=uint64_t;};
template<> struct ext_ret_to_carrier<FDasNameValue,uint64_t> {static uint64_t from(FDasNameValue V){return V.Token;}};
}
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
    FSourceSet* AuthoredReferenceSources=nullptr;

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
        explicit FUnrealFileSystem(FSourceSet* InSources = nullptr) : Sources(InSources?InSources:AuthoredReferenceSources) {}
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
        TSet<FString> WholeInputs;
        TMap<FString, TStrongObjectPtr<UObject>> ObjectPins;
        TArray<FMarshalledParameter> Signature;
        FString SignatureKey;
        TWeakObjectPtr<UClass> RequiredInterface;
        FString InterfaceSignatureKey;
        TMap<FString, TSharedPtr<FPendingHash>> PendingHashes;
        TMap<FString, TArray<int32>> HashIndices;
        bool bAuthored = false;
        bool bExecuted = false;
        bool bFailed = false;
    };
    uint64 NextCallHandle = 1;

    class FUnrealDasContext final : public das::Context, public FGCObject
    {
    public:
        explicit FUnrealDasContext(uint32 StackSize) : das::Context(StackSize) {}
        virtual void AddReferencedObjects(FReferenceCollector& Collector) override {
            for(auto& Value:StructValues)Value.Value.AddStructReferencedObjects(Collector);
        }
        virtual FString GetReferencerName() const override {return TEXT("FUnrealDasContext snapshots");}
        TArray<FDasTypeDeclaration> Types;
        TMap<uint64,FInstancedStruct> StructValues;uint64 StructCollections=0,StructReclaimed=0;int32 StructNextCollection=256;
        struct FEnumValue {TWeakObjectPtr<UEnum> Type;int32 Value;};TMap<uint64,FEnumValue> EnumValues;
        TMap<uint64,FName> DSLNames;TMap<FName,uint64> DSLNameTokens;
        TMap<FString, int32> PersistentInts;
        int32 ExecutionDepth = 0;
        bool bEnding = false;
        bool bCollectingSubscriptions = false;
        TArray<FSubscriptionDeclaration> Subscriptions;
        TWeakObjectPtr<UObject> Self;
        TMap<uint64, FObjectHandleEntry> ObjectHandles;
        TMap<uint64, TSharedPtr<FMarshalledCall>> Calls;
    };

    struct FStructFrameRoots final : FGCObject {
        TArray<FDasScalarValue>& Values;
        explicit FStructFrameRoots(TArray<FDasScalarValue>& InValues):Values(InValues){}
        virtual void AddReferencedObjects(FReferenceCollector& Collector) override {
            for(auto& Value:Values)if(Value.Kind==12)Value.StructValue.AddStructReferencedObjects(Collector);
        }
        virtual FString GetReferencerName() const override {return TEXT("daScript native input frame snapshots");}
    };
    thread_local FUnrealDasContext* ActiveContext = nullptr;
    struct FOutputFrame:FGCObject {const TArray<FDasScalarValue>* Arguments=nullptr;TMap<int32,FDasScalarValue> Values;TArray<TStrongObjectPtr<UObject>> ObjectPins;
        virtual void AddReferencedObjects(FReferenceCollector& Collector)override{for(auto& V:Values)if(V.Value.Kind==12)V.Value.StructValue.AddStructReferencedObjects(Collector);}
        virtual FString GetReferencerName()const override{return TEXT("daScript pending frame outputs");}
    };
    thread_local FOutputFrame* ActiveOutputFrame=nullptr;


    thread_local int32 AuthoredDispatchDepth=0;
    thread_local FString* AuthoredDispatchFailure=nullptr;
    struct FActiveContextScope
    {
        FUnrealDasContext* Previous;
        TSet<uint64> ExistingCalls;
        explicit FActiveContextScope(FUnrealDasContext& Context) : Previous(ActiveContext)
        {
            for(const auto& Call:Context.Calls)ExistingCalls.Add(Call.Key);
            ActiveContext = &Context;
        }
        ~FActiveContextScope() { for(auto It=ActiveContext->Calls.CreateIterator();It;++It)if(!ExistingCalls.Contains(It.Key()))It.RemoveCurrent(); ActiveContext = Previous; }
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
    struct FStructCollectOnExit {FUnrealDasContext& C;~FStructCollectOnExit(){if(C.ExecutionDepth>1)return;try{CollectStructValues(C,nullptr,true);}catch(...){/* Failed root traversal must not turn a result into an exception. */}}};
    // The context reports every stored snapshot through FInstancedStruct's GC collector.
    // This roots its reflected objects and lets UE clear destroyed references in native memory.
    bool MathSnapshotValid(const FInstancedStruct& Snapshot){const auto* Type=Snapshot.GetScriptStruct();if(Type==TBaseStructure<FVector>::Get())return !Snapshot.Get<FVector>().ContainsNaN();if(Type==TBaseStructure<FRotator>::Get())return !Snapshot.Get<FRotator>().ContainsNaN();if(Type==TBaseStructure<FTransform>::Get())return !Snapshot.Get<FTransform>().ContainsNaN() && Snapshot.Get<FTransform>().IsRotationNormalized();return true;}
    FDasStructValue StoreStructValue(const FInstancedStruct& V,das::Context* C,das::LineInfoArg* At=nullptr){if(C!=ActiveContext || !ActiveContext)C->throw_error("struct requires active context");FString Error;if(!ValidateDasStructSnapshot(V,Error))C->throw_error(TCHAR_TO_UTF8(*Error));if(!MathSnapshotValid(V))C->throw_error("invalid math snapshot value");for(const auto& P:ActiveContext->StructValues)if(P.Value.GetScriptStruct()==V.GetScriptStruct() && V.GetScriptStruct()->CompareScriptStruct(P.Value.GetMemory(),V.GetMemory(),0))return {P.Key};if(At && ActiveContext->StructValues.Num()>=ActiveContext->StructNextCollection)CollectStructValues(*ActiveContext,At);if(ActiveContext->StructValues.Num()>=4096)C->throw_error("struct value table limit exceeded");static uint64 Next=1;uint64 Token=Next++;ActiveContext->StructValues.Add(Token,V);return {Token};}
    const FInstancedStruct& ResolveStructValue(FDasStructValue V,das::Context* C){if(C!=ActiveContext || !ActiveContext)C->throw_error("struct requires active context");const auto* S=ActiveContext->StructValues.Find(V.Token);if(!S)C->throw_error("struct is null or belongs to another context");return *S;}
    FDasStructValue NewStructDefault(const char* Path,const char* Text,das::Context* C,das::LineInfoArg* At){FInstancedStruct Value;FString Error;if(!ParseDasStructDefault(Path?UTF8_TO_TCHAR(Path):TEXT(""),Text?UTF8_TO_TCHAR(Text):TEXT(""),Value,Error))C->throw_error(TCHAR_TO_UTF8(*Error));return StoreStructValue(Value,C,At);}
    FDasStructValue ReadPropertySnapshot(FProperty* P,void* Memory,das::Context* C,das::LineInfoArg* At){
        FInstancedStruct V;
        if(auto* S=CastField<FStructProperty>(P))V.InitializeAs(S->Struct,static_cast<const uint8*>(Memory));
        else {auto* Carrier=DasContainerCarrierForProperty(P);auto* Values=Carrier?Carrier->FindPropertyByName(TEXT("Values")):nullptr;if(!Values || !Values->SameType(P))C->throw_error("Unsupported container property");V.InitializeAs(Carrier);Values->CopyCompleteValue(Values->ContainerPtrToValuePtr<void>(V.GetMutableMemory()),Memory);}
        return StoreStructValue(V,C,At);
    }
    void WritePropertySnapshot(FProperty* P,void* Memory,const FDasStructValue& Handle,das::Context* C){
        const auto& V=ResolveStructValue(Handle,C);FString Error;
        if(auto* S=CastField<FStructProperty>(P)){if(V.GetScriptStruct()!=S->Struct)C->throw_error("Authored struct assignment type mismatch");}
        else {auto* Carrier=DasContainerCarrierForProperty(P);auto* Values=Carrier?Carrier->FindPropertyByName(TEXT("Values")):nullptr;if(!Values || V.GetScriptStruct()!=Carrier || !Values->SameType(P))C->throw_error("Container property identity mismatch");}
        if(!ValidateDasStructSnapshot(V,Error))C->throw_error(TCHAR_TO_UTF8(*Error));if(!MathSnapshotValid(V))C->throw_error("Invalid math property value");
        if(auto* S=CastField<FStructProperty>(P))S->Struct->CopyScriptStruct(Memory,V.GetMemory());else {auto* Values=V.GetScriptStruct()->FindPropertyByName(TEXT("Values"));P->CopyCompleteValue(Memory,Values->ContainerPtrToValuePtr<void>(V.GetMemory()));}
    }
    FDasStructValue GetSelfStruct(const char* N,das::Context* C,das::LineInfoArg* At){auto* Self=ResolveSelf(C);auto* P=ResolveProperty<FProperty>(Self,N,false,C);return ReadPropertySnapshot(P,P->ContainerPtrToValuePtr<void>(Self),C,At);}
    void SetSelfStruct(const char* N,const FDasStructValue& V,das::Context* C){auto* Self=ResolveSelf(C);if(ActiveContext->bCollectingSubscriptions)C->throw_error("self writes are forbidden in bind_events");auto* P=ResolveProperty<FProperty>(Self,N,true,C);WritePropertySnapshot(P,P->ContainerPtrToValuePtr<void>(Self),V,C);}

    FDasStructValue NewStructValue(const char* Path,das::Context* C,das::LineInfoArg* At){FString P=Path?UTF8_TO_TCHAR(Path):TEXT("");FString E;if(!ValidateDasValueStruct(P,E))C->throw_error(TCHAR_TO_UTF8(*E));FInstancedStruct S;S.InitializeAs(FindObject<UScriptStruct>(nullptr,*P));return StoreStructValue(S,C,At);}
    struct FStructLeaf{FProperty* Property;uint8* Memory;};
    FStructLeaf StructLeaf(FInstancedStruct& V,const char* Path,das::Context* C){TArray<FString> Parts;FString(Path?UTF8_TO_TCHAR(Path):TEXT("")).ParseIntoArray(Parts,TEXT("."),false);if(Parts.IsEmpty())C->throw_error("empty struct field path");UScriptStruct* Type=const_cast<UScriptStruct*>(V.GetScriptStruct());uint8* Memory=V.GetMutableMemory();for(int32 I=0;I<Parts.Num();++I){FProperty* P=FindDasStructField(Type,Parts[I]);if(!P)C->throw_error("struct field does not exist");Memory=P->ContainerPtrToValuePtr<uint8>(Memory);if(I==Parts.Num()-1)return {P,Memory};if(auto* Array=CastField<FArrayProperty>(P)){if(++I>=Parts.Num())C->throw_error("array path requires index");const FString& Index=Parts[I];if(Index.IsEmpty() || Index.Len()>4 || (Index.Len()>1 && Index[0]==TEXT('0')))C->throw_error("invalid struct array index");for(TCHAR Digit:Index)if(Digit<TEXT('0') || Digit>TEXT('9'))C->throw_error("invalid struct array index");const int32 N=FCString::Atoi(*Index);FScriptArrayHelper Elements(Array,Memory);if(!Elements.IsValidIndex(N))C->throw_error("struct array index out of range");Memory=Elements.GetRawPtr(N);if(I==Parts.Num()-1)return {Array->Inner,Memory};auto* Element=CastField<FStructProperty>(Array->Inner);if(!Element)C->throw_error("array element is not struct");Type=Element->Struct;continue;}auto* S=CastField<FStructProperty>(P);if(!S)C->throw_error("path crosses non-struct field");Type=S->Struct;}C->throw_error("invalid field path");return {};}
    FMapProperty* StructIntMap(FStructLeaf F,das::Context* C){auto* P=CastField<FMapProperty>(F.Property);if(!P || !CastField<FIntProperty>(P->KeyProp) || !CastField<FIntProperty>(P->ValueProp))C->throw_error("expected int-int snapshot map");return P;}
    FSetProperty* StructTypedSet(const FStructLeaf& F,bool StringElement,das::Context* C){
        auto* P=CastField<FSetProperty>(F.Property);
        if(!P || (StringElement?!P->ElementProp->IsA<FStrProperty>():!P->ElementProp->IsA<FIntProperty>()))C->throw_error("snapshot set element type mismatch");
        return P;
    }
    int32 StructSetSize(const FDasStructValue& H,const char* Path,das::Context* C){
        FInstancedStruct V=ResolveStructValue(H,C);auto F=StructLeaf(V,Path,C);auto* P=CastField<FSetProperty>(F.Property);
        if(!P)C->throw_error("expected snapshot set");return FScriptSetHelper(P,F.Memory).Num();
    }
    int32 StructSetElementInt(const FDasStructValue& H,const char* Path,int32 Index,das::Context* C){
        FInstancedStruct V=ResolveStructValue(H,C);auto F=StructLeaf(V,Path,C);auto* P=StructTypedSet(F,false,C);FScriptSetHelper Values(P,F.Memory);
        if(Index<0 || Index>=Values.Num())C->throw_error("set element index out of range");
        TArray<int32> Elements;for(int32 I=0;I<Values.GetMaxIndex();++I)if(Values.IsValidIndex(I))Elements.Add(CastFieldChecked<FIntProperty>(P->ElementProp)->GetPropertyValue(Values.GetElementPtr(I)));
        Elements.Sort();return Elements[Index];
    }
    char* StructSetElementString(const FDasStructValue& H,const char* Path,int32 Index,das::Context* C){
        FInstancedStruct V=ResolveStructValue(H,C);auto F=StructLeaf(V,Path,C);auto* P=StructTypedSet(F,true,C);FScriptSetHelper Values(P,F.Memory);
        if(Index<0 || Index>=Values.Num())C->throw_error("set element index out of range");
        TArray<FString> Elements;for(int32 I=0;I<Values.GetMaxIndex();++I)if(Values.IsValidIndex(I))Elements.Add(CastFieldChecked<FStrProperty>(P->ElementProp)->GetPropertyValue(Values.GetElementPtr(I)));
        Elements.Sort();return CopyDSLString(Elements[Index],C);
    }
    bool StructSetContainsInt(const FDasStructValue& H,const char* Path,int32 Element,das::Context* C){
        FInstancedStruct V=ResolveStructValue(H,C);auto F=StructLeaf(V,Path,C);auto* P=StructTypedSet(F,false,C);return FScriptSetHelper(P,F.Memory).FindElementIndex(&Element)!=INDEX_NONE;
    }
    bool StructSetContainsString(const FDasStructValue& H,const char* Path,const char* Element,das::Context* C){
        FInstancedStruct V=ResolveStructValue(H,C);auto F=StructLeaf(V,Path,C);auto* P=StructTypedSet(F,true,C);FString Native=Element?UTF8_TO_TCHAR(Element):TEXT("");return FScriptSetHelper(P,F.Memory).FindElementIndex(&Native)!=INDEX_NONE;
    }
    FDasStructValue StructSetWithInt(const FDasStructValue& H,const char* Path,int32 Element,bool Present,das::Context* C,das::LineInfoArg* At){
        FInstancedStruct V=ResolveStructValue(H,C);auto F=StructLeaf(V,Path,C);auto* P=StructTypedSet(F,false,C);FScriptSetHelper Values(P,F.Memory);const int32 Existing=Values.FindElementIndex(&Element);
        if(Present){if(Existing==INDEX_NONE && Values.Num()>=4096)C->throw_error("snapshot set size limit");Values.AddElement(&Element);}else if(Existing!=INDEX_NONE)Values.RemoveAt(Existing);
        return StoreStructValue(V,C,At);
    }
    FDasStructValue StructSetWithString(const FDasStructValue& H,const char* Path,const char* Element,bool Present,das::Context* C,das::LineInfoArg* At){
        FInstancedStruct V=ResolveStructValue(H,C);auto F=StructLeaf(V,Path,C);auto* P=StructTypedSet(F,true,C);FString Native=Element?UTF8_TO_TCHAR(Element):TEXT("");FScriptSetHelper Values(P,F.Memory);const int32 Existing=Values.FindElementIndex(&Native);
        if(Present){if(Existing==INDEX_NONE && Values.Num()>=4096)C->throw_error("snapshot set size limit");if(Existing!=INDEX_NONE && !CastFieldChecked<FStrProperty>(P->ElementProp)->GetPropertyValue(Values.GetElementPtr(Existing)).Equals(Native,ESearchCase::CaseSensitive))C->throw_error("string set key collision under Unreal equality");Values.AddElement(&Native);}else if(Existing!=INDEX_NONE)Values.RemoveAt(Existing);
        return StoreStructValue(V,C,At);
    }
    int32 StructMapSize(const FDasStructValue& H,const char* Path,das::Context* C){FInstancedStruct V=ResolveStructValue(H,C);auto F=StructLeaf(V,Path,C);auto* P=CastField<FMapProperty>(F.Property);if(!P)C->throw_error("expected snapshot map");return FScriptMapHelper(P,F.Memory).Num();}
    int32 StructMapKey(const FDasStructValue& H,const char* Path,int32 Index,das::Context* C){FInstancedStruct V=ResolveStructValue(H,C);auto F=StructLeaf(V,Path,C);auto* P=CastField<FMapProperty>(F.Property);if(!P || !P->KeyProp->IsA<FIntProperty>())C->throw_error("expected int-key map");FScriptMapHelper Values(P,F.Memory);if(Index<0 || Index>=Values.Num())C->throw_error("map key index out of range");TArray<int32> Keys;for(int32 I=0;I<Values.GetMaxIndex();++I)if(Values.IsValidIndex(I))Keys.Add(CastFieldChecked<FIntProperty>(P->KeyProp)->GetPropertyValue(Values.GetKeyPtr(I)));Keys.Sort();return Keys[Index];}
    int32 StructMapGetInt(const FDasStructValue& H,const char* Path,int32 Key,das::Context* C){FInstancedStruct V=ResolveStructValue(H,C);auto F=StructLeaf(V,Path,C);auto* P=StructIntMap(F,C);FScriptMapHelper Values(P,F.Memory);const int32 I=Values.FindMapIndexWithKey(&Key);if(I==INDEX_NONE)C->throw_error("snapshot map key not found");return CastFieldChecked<FIntProperty>(P->ValueProp)->GetPropertyValue(Values.GetValuePtr(I));}
    FDasStructValue StructMapWithInt(const FDasStructValue& H,const char* Path,int32 Key,int32 Value,das::Context* C,das::LineInfoArg* At){FInstancedStruct V=ResolveStructValue(H,C);auto F=StructLeaf(V,Path,C);auto* P=StructIntMap(F,C);FScriptMapHelper Values(P,F.Memory);if(Values.Num()>=4096 && Values.FindMapIndexWithKey(&Key)==INDEX_NONE)C->throw_error("snapshot map size limit");Values.AddPair(&Key,&Value);return StoreStructValue(V,C,At);}
    FMapProperty* StructTypedMap(FStructLeaf F,bool StringKey,bool StringValue,das::Context* C){auto* P=CastField<FMapProperty>(F.Property);if(!P || (StringKey?!P->KeyProp->IsA<FStrProperty>():!P->KeyProp->IsA<FIntProperty>()) || (StringValue?!P->ValueProp->IsA<FStrProperty>():!P->ValueProp->IsA<FIntProperty>()))C->throw_error("snapshot map key/value type mismatch");return P;}
    char* StructMapStringKey(const FDasStructValue& H,const char* Path,int32 Index,das::Context* C){FInstancedStruct V=ResolveStructValue(H,C);auto F=StructLeaf(V,Path,C);auto* P=CastField<FMapProperty>(F.Property);if(!P || !P->KeyProp->IsA<FStrProperty>())C->throw_error("expected string-key map");FScriptMapHelper Values(P,F.Memory);if(Index<0 || Index>=Values.Num())C->throw_error("map key index out of range");TArray<FString> Keys;for(int32 I=0;I<Values.GetMaxIndex();++I)if(Values.IsValidIndex(I))Keys.Add(CastFieldChecked<FStrProperty>(P->KeyProp)->GetPropertyValue(Values.GetKeyPtr(I)));Keys.Sort();return CopyDSLString(Keys[Index],C);}
    char* StructMapGetIntString(const FDasStructValue& H,const char* Path,int32 Key,das::Context* C){FInstancedStruct V=ResolveStructValue(H,C);auto F=StructLeaf(V,Path,C);auto* P=StructTypedMap(F,false,true,C);FScriptMapHelper Values(P,F.Memory);const int32 I=Values.FindMapIndexWithKey(&Key);if(I==INDEX_NONE)C->throw_error("snapshot map key not found");return CopyDSLString(CastFieldChecked<FStrProperty>(P->ValueProp)->GetPropertyValue(Values.GetValuePtr(I)),C);}
    FDasStructValue StructMapWithIntString(const FDasStructValue& H,const char* Path,int32 Key,const char* Value,das::Context* C,das::LineInfoArg* At){FInstancedStruct Snapshot=ResolveStructValue(H,C);auto F=StructLeaf(Snapshot,Path,C);auto* P=StructTypedMap(F,false,true,C);FString NativeValue=Value?UTF8_TO_TCHAR(Value):TEXT("");FScriptMapHelper Values(P,F.Memory);if(Values.Num()>=4096 && Values.FindMapIndexWithKey(&Key)==INDEX_NONE)C->throw_error("snapshot map size limit");Values.AddPair(&Key,&NativeValue);return StoreStructValue(Snapshot,C,At);}
    int32 StructMapGetStringInt(const FDasStructValue& H,const char* Path,const char* Key,das::Context* C){FInstancedStruct V=ResolveStructValue(H,C);auto F=StructLeaf(V,Path,C);auto* P=StructTypedMap(F,true,false,C);FString NativeKey=Key?UTF8_TO_TCHAR(Key):TEXT("");FScriptMapHelper Values(P,F.Memory);const int32 I=Values.FindMapIndexWithKey(&NativeKey);if(I==INDEX_NONE)C->throw_error("snapshot map key not found");return CastFieldChecked<FIntProperty>(P->ValueProp)->GetPropertyValue(Values.GetValuePtr(I));}
    FDasStructValue StructMapWithStringInt(const FDasStructValue& H,const char* Path,const char* Key,int32 Value,das::Context* C,das::LineInfoArg* At){FInstancedStruct Snapshot=ResolveStructValue(H,C);auto F=StructLeaf(Snapshot,Path,C);auto* P=StructTypedMap(F,true,false,C);FString NativeKey=Key?UTF8_TO_TCHAR(Key):TEXT("");FScriptMapHelper Values(P,F.Memory);if(Values.Num()>=4096 && Values.FindMapIndexWithKey(&NativeKey)==INDEX_NONE)C->throw_error("snapshot map size limit");const int32 Existing=Values.FindMapIndexWithKey(&NativeKey);if(Existing!=INDEX_NONE && !CastFieldChecked<FStrProperty>(P->KeyProp)->GetPropertyValue(Values.GetKeyPtr(Existing)).Equals(NativeKey,ESearchCase::CaseSensitive))C->throw_error("string map key collision under Unreal equality");Values.AddPair(&NativeKey,&Value);return StoreStructValue(Snapshot,C,At);}
    char* StructMapGetStringString(const FDasStructValue& H,const char* Path,const char* Key,das::Context* C){FInstancedStruct V=ResolveStructValue(H,C);auto F=StructLeaf(V,Path,C);auto* P=StructTypedMap(F,true,true,C);FString NativeKey=Key?UTF8_TO_TCHAR(Key):TEXT("");FScriptMapHelper Values(P,F.Memory);const int32 I=Values.FindMapIndexWithKey(&NativeKey);if(I==INDEX_NONE)C->throw_error("snapshot map key not found");return CopyDSLString(CastFieldChecked<FStrProperty>(P->ValueProp)->GetPropertyValue(Values.GetValuePtr(I)),C);}
    FDasStructValue StructMapWithStringString(const FDasStructValue& H,const char* Path,const char* Key,const char* Value,das::Context* C,das::LineInfoArg* At){FInstancedStruct Snapshot=ResolveStructValue(H,C);auto F=StructLeaf(Snapshot,Path,C);auto* P=StructTypedMap(F,true,true,C);FString NativeKey=Key?UTF8_TO_TCHAR(Key):TEXT("");FString NativeValue=Value?UTF8_TO_TCHAR(Value):TEXT("");FScriptMapHelper Values(P,F.Memory);if(Values.Num()>=4096 && Values.FindMapIndexWithKey(&NativeKey)==INDEX_NONE)C->throw_error("snapshot map size limit");const int32 Existing=Values.FindMapIndexWithKey(&NativeKey);if(Existing!=INDEX_NONE && !CastFieldChecked<FStrProperty>(P->KeyProp)->GetPropertyValue(Values.GetKeyPtr(Existing)).Equals(NativeKey,ESearchCase::CaseSensitive))C->throw_error("string map key collision under Unreal equality");Values.AddPair(&NativeKey,&NativeValue);return StoreStructValue(Snapshot,C,At);}
    int32 StructArraySize(const FDasStructValue& H,const char* Path,das::Context* C){FInstancedStruct V=ResolveStructValue(H,C);auto F=StructLeaf(V,Path,C);auto* P=CastField<FArrayProperty>(F.Property);if(!P)C->throw_error("expected snapshot array");return FScriptArrayHelper(P,F.Memory).Num();}
    FDasStructValue StructArrayResize(const FDasStructValue& H,const char* Path,int32 N,das::Context* C,das::LineInfoArg* At){if(N<0 || N>4096)C->throw_error("struct array size out of range");FInstancedStruct V=ResolveStructValue(H,C);auto F=StructLeaf(V,Path,C);auto* P=CastField<FArrayProperty>(F.Property);if(!P || int64(N)*P->Inner->GetElementSize()>16*1024*1024)C->throw_error("invalid struct array size/type");FScriptArrayHelper Items(P,F.Memory);const int32 OldNum=Items.Num();Items.Resize(N);if(auto* E=DasEnumPropertyType(P->Inner)){int32 First=INDEX_NONE;for(int32 I=0;I<E->NumEnums();++I)if(IsCallEnumMember(E,I)){First=I;break;}if(First==INDEX_NONE)C->throw_error("Enum array has no usable member");for(int32 I=OldNum;I<N;++I)if(!SetDasEnumPropertyNumber(P->Inner,Items.GetRawPtr(I),int32(E->GetValueByIndex(First))))C->throw_error("Enum array default is invalid");}return StoreStructValue(V,C,At);}
    template<typename ValueProperty> FMapProperty* StructScalarValueMap(FStructLeaf F,bool StringKey,das::Context* C){
        auto* P=CastField<FMapProperty>(F.Property);
        if(!P || (StringKey?!P->KeyProp->IsA<FStrProperty>():!P->KeyProp->IsA<FIntProperty>()) || !P->ValueProp->IsA<ValueProperty>())C->throw_error("snapshot map key/value type mismatch");
        return P;
    }
    template<typename ValueProperty,typename KeyType> auto StructScalarMapGet(const FDasStructValue& H,const char* Path,const KeyType& Key,bool StringKey,das::Context* C){
        FInstancedStruct V=ResolveStructValue(H,C);auto F=StructLeaf(V,Path,C);auto* P=StructScalarValueMap<ValueProperty>(F,StringKey,C);FScriptMapHelper Values(P,F.Memory);
        const int32 I=Values.FindMapIndexWithKey(&Key);if(I==INDEX_NONE)C->throw_error("snapshot map key not found");
        return CastFieldChecked<ValueProperty>(P->ValueProp)->GetPropertyValue(Values.GetValuePtr(I));
    }
    template<typename ValueProperty,typename KeyType,typename ValueType> FDasStructValue StructScalarMapWith(const FDasStructValue& H,const char* Path,const KeyType& Key,const ValueType& Value,bool StringKey,das::Context* C,das::LineInfoArg* At){
        FInstancedStruct V=ResolveStructValue(H,C);auto F=StructLeaf(V,Path,C);auto* P=StructScalarValueMap<ValueProperty>(F,StringKey,C);FScriptMapHelper Values(P,F.Memory);const int32 Existing=Values.FindMapIndexWithKey(&Key);
        if(Existing==INDEX_NONE && Values.Num()>=4096)C->throw_error("snapshot map size limit");
        if constexpr(std::is_same_v<KeyType,FString>){if(Existing!=INDEX_NONE && !CastFieldChecked<FStrProperty>(P->KeyProp)->GetPropertyValue(Values.GetKeyPtr(Existing)).Equals(Key,ESearchCase::CaseSensitive))C->throw_error("string map key collision under Unreal equality");}
        Values.AddPair(&Key,&Value);return StoreStructValue(V,C,At);
    }
    float StructMapGetIntFloat(const FDasStructValue& H,const char* Path,int32 Key,das::Context* C){return StructScalarMapGet<FFloatProperty>(H,Path,Key,false,C);}
    FDasStructValue StructMapWithIntFloat(const FDasStructValue& H,const char* Path,int32 Key,float Value,das::Context* C,das::LineInfoArg* At){return StructScalarMapWith<FFloatProperty>(H,Path,Key,Value,false,C,At);}
    double StructMapGetIntDouble(const FDasStructValue& H,const char* Path,int32 Key,das::Context* C){return StructScalarMapGet<FDoubleProperty>(H,Path,Key,false,C);}
    FDasStructValue StructMapWithIntDouble(const FDasStructValue& H,const char* Path,int32 Key,double Value,das::Context* C,das::LineInfoArg* At){return StructScalarMapWith<FDoubleProperty>(H,Path,Key,Value,false,C,At);}
    bool StructMapGetIntBool(const FDasStructValue& H,const char* Path,int32 Key,das::Context* C){return StructScalarMapGet<FBoolProperty>(H,Path,Key,false,C);}
    FDasStructValue StructMapWithIntBool(const FDasStructValue& H,const char* Path,int32 Key,bool Value,das::Context* C,das::LineInfoArg* At){return StructScalarMapWith<FBoolProperty>(H,Path,Key,Value,false,C,At);}
    FDasNameValue StructMapGetIntName(const FDasStructValue& H,const char* Path,int32 Key,das::Context* C){return MakeDSLName(StructScalarMapGet<FNameProperty>(H,Path,Key,false,C),C);}
    FDasStructValue StructMapWithIntName(const FDasStructValue& H,const char* Path,int32 Key,FDasNameValue Value,das::Context* C,das::LineInfoArg* At){return StructScalarMapWith<FNameProperty>(H,Path,Key,ResolveDSLName(Value,C),false,C,At);}
    float StructMapGetStringFloat(const FDasStructValue& H,const char* Path,const char* Key,das::Context* C){return StructScalarMapGet<FFloatProperty>(H,Path,FString(Key?UTF8_TO_TCHAR(Key):TEXT("")),true,C);}
    FDasStructValue StructMapWithStringFloat(const FDasStructValue& H,const char* Path,const char* Key,float Value,das::Context* C,das::LineInfoArg* At){return StructScalarMapWith<FFloatProperty>(H,Path,FString(Key?UTF8_TO_TCHAR(Key):TEXT("")),Value,true,C,At);}
    double StructMapGetStringDouble(const FDasStructValue& H,const char* Path,const char* Key,das::Context* C){return StructScalarMapGet<FDoubleProperty>(H,Path,FString(Key?UTF8_TO_TCHAR(Key):TEXT("")),true,C);}
    FDasStructValue StructMapWithStringDouble(const FDasStructValue& H,const char* Path,const char* Key,double Value,das::Context* C,das::LineInfoArg* At){return StructScalarMapWith<FDoubleProperty>(H,Path,FString(Key?UTF8_TO_TCHAR(Key):TEXT("")),Value,true,C,At);}
    bool StructMapGetStringBool(const FDasStructValue& H,const char* Path,const char* Key,das::Context* C){return StructScalarMapGet<FBoolProperty>(H,Path,FString(Key?UTF8_TO_TCHAR(Key):TEXT("")),true,C);}
    FDasStructValue StructMapWithStringBool(const FDasStructValue& H,const char* Path,const char* Key,bool Value,das::Context* C,das::LineInfoArg* At){return StructScalarMapWith<FBoolProperty>(H,Path,FString(Key?UTF8_TO_TCHAR(Key):TEXT("")),Value,true,C,At);}
    FDasNameValue StructMapGetStringName(const FDasStructValue& H,const char* Path,const char* Key,das::Context* C){return MakeDSLName(StructScalarMapGet<FNameProperty>(H,Path,FString(Key?UTF8_TO_TCHAR(Key):TEXT("")),true,C),C);}
    FDasStructValue StructMapWithStringName(const FDasStructValue& H,const char* Path,const char* Key,FDasNameValue Value,das::Context* C,das::LineInfoArg* At){return StructScalarMapWith<FNameProperty>(H,Path,FString(Key?UTF8_TO_TCHAR(Key):TEXT("")),ResolveDSLName(Value,C),true,C,At);}
    FDasStructValue StructNestedGet(const FDasStructValue& H,const char* Path,int32 Index,das::Context* C,das::LineInfoArg* At){FInstancedStruct V=ResolveStructValue(H,C);auto F=StructLeaf(V,Path,C);FStructProperty* P=nullptr;void* Memory=F.Memory;if(Index==-1)P=CastField<FStructProperty>(F.Property);else{auto* Array=CastField<FArrayProperty>(F.Property);if(!Array)C->throw_error("expected struct array");FScriptArrayHelper Elements(Array,F.Memory);if(!Elements.IsValidIndex(Index))C->throw_error("struct array index out of range");P=CastField<FStructProperty>(Array->Inner);Memory=Elements.GetRawPtr(Index);}if(Index==-1 && !P){auto* Carrier=DasContainerCarrierForProperty(F.Property);auto* Values=Carrier?Carrier->FindPropertyByName(TEXT("Values")):nullptr;if(!Values || !Values->SameType(F.Property))C->throw_error("expected supported nested container");FInstancedStruct Out;Out.InitializeAs(Carrier);Values->CopyCompleteValue(Values->ContainerPtrToValuePtr<void>(Out.GetMutableMemory()),F.Memory);return StoreStructValue(Out,C,At);}if(!P)C->throw_error("expected nested struct");FInstancedStruct Out;Out.InitializeAs(P->Struct);P->Struct->CopyScriptStruct(Out.GetMutableMemory(),Memory);return StoreStructValue(Out,C,At);}
    FDasStructValue StructNestedWith(const FDasStructValue& H,const char* Path,int32 Index,const FDasStructValue& Value,das::Context* C,das::LineInfoArg* At){FInstancedStruct V=ResolveStructValue(H,C);auto F=StructLeaf(V,Path,C);FStructProperty* P=nullptr;void* Memory=F.Memory;if(Index==-1)P=CastField<FStructProperty>(F.Property);else{auto* Array=CastField<FArrayProperty>(F.Property);if(!Array)C->throw_error("expected struct array");FScriptArrayHelper Elements(Array,F.Memory);if(!Elements.IsValidIndex(Index))C->throw_error("struct array index out of range");P=CastField<FStructProperty>(Array->Inner);Memory=Elements.GetRawPtr(Index);}const auto& Input=ResolveStructValue(Value,C);if(Index==-1 && !P){auto* Carrier=DasContainerCarrierForProperty(F.Property);auto* Values=Carrier?Carrier->FindPropertyByName(TEXT("Values")):nullptr;if(!Carrier || Input.GetScriptStruct()!=Carrier || !Values || !Values->SameType(F.Property))C->throw_error("nested container identity mismatch");F.Property->CopyCompleteValue(F.Memory,Values->ContainerPtrToValuePtr<void>(Input.GetMemory()));return StoreStructValue(V,C,At);}if(!P || P->Struct!=Input.GetScriptStruct())C->throw_error("nested struct identity mismatch");P->Struct->CopyScriptStruct(Memory,Input.GetMemory());return StoreStructValue(V,C,At);}
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
    FDasObjectHandle MakeObjectHandle(UObject*,UClass*,das::Context*);
    UObject* ResolveObject(const FDasObjectHandle&,das::Context*);
    FDasObjectHandle StructGetObject(const FDasStructValue& H,const char* P,das::Context* C){FInstancedStruct V=ResolveStructValue(H,C);auto F=StructLeaf(V,P,C);auto* T=DasReferencePropertyClass(F.Property);if(!T)C->throw_error("struct field object type mismatch");return MakeObjectHandle(DasReferenceGet(F.Property,F.Memory),T,C);}
    FDasStructValue StructWithObject(const FDasStructValue& H,const char* P,const FDasObjectHandle& X,das::Context* C,das::LineInfoArg* At){FInstancedStruct V=ResolveStructValue(H,C);auto F=StructLeaf(V,P,C);auto* T=DasReferencePropertyClass(F.Property);if(!T)C->throw_error("struct field object type mismatch");UObject* Object=X.Token?ResolveObject(X,C):nullptr;if(Object && !DasReferenceMatches(Object,T))C->throw_error("struct object field has an incompatible class");DasReferenceSet(F.Property,F.Memory,Object);return StoreStructValue(V,C,At);}
    FDasNameValue StructGetName(const FDasStructValue& H,const char* P,das::Context* C){FInstancedStruct V=ResolveStructValue(H,C);auto F=StructLeaf(V,P,C);auto* T=CastField<FNameProperty>(F.Property);if(!T)C->throw_error("struct field scalar type mismatch");return MakeDSLName(T->GetPropertyValue(F.Memory),C);}
    FDasStructValue StructWithName(const FDasStructValue& H,const char* P,FDasNameValue X,das::Context* C,das::LineInfoArg* At){FInstancedStruct V=ResolveStructValue(H,C);auto F=StructLeaf(V,P,C);auto* T=CastField<FNameProperty>(F.Property);if(!T)C->throw_error("struct field scalar type mismatch");T->SetPropertyValue(F.Memory,ResolveDSLName(X,C));return StoreStructValue(V,C,At);}
    char* StructGetEnum(const FDasStructValue& H,const char* Path,const char* EnumPath,das::Context* C){FInstancedStruct V=ResolveStructValue(H,C);auto F=StructLeaf(V,Path,C);UEnum* E=RequireCallEnum(F.Property,EnumPath,C);int32 Index=E->GetIndexByValue(CallEnumStorage(F.Property)->GetSignedIntPropertyValue(F.Memory));if(!IsCallEnumMember(E,Index))C->throw_error("unknown or reserved enum value in struct");return CopyDSLString(E->GetNameStringByIndex(Index),C);}
    FDasStructValue StructWithEnum(const FDasStructValue& H,const char* Path,const char* EnumPath,const char* Member,das::Context* C,das::LineInfoArg* At){FInstancedStruct V=ResolveStructValue(H,C);auto F=StructLeaf(V,Path,C);UEnum* E=RequireCallEnum(F.Property,EnumPath,C);const FString Symbol=Member?UTF8_TO_TCHAR(Member):TEXT("");if(Symbol.Len()>=NAME_SIZE)C->throw_error("enum member is too long");int32 Index=INDEX_NONE;for(int32 I=0;I<E->NumEnums();++I)if(Symbol.Equals(E->GetNameStringByIndex(I),ESearchCase::IgnoreCase) || Symbol.Equals(E->GetNameByIndex(I).ToString(),ESearchCase::IgnoreCase)){Index=I;break;}if(!IsCallEnumMember(E,Index))C->throw_error("unknown or reserved enum member");CallEnumStorage(F.Property)->SetIntPropertyValue(F.Memory,E->GetValueByIndex(Index));return StoreStructValue(V,C,At);}
    FDasEnumValue StructEnumGetValue(const FDasStructValue& H,const char* Path,const char* EnumPath,das::Context* C);
    FDasStructValue StructEnumWithValue(const FDasStructValue& H,const char* Path,FDasEnumValue Value,das::Context* C,das::LineInfoArg* At);
    FDasEnumValue MakeEnumValue(const FString& Path,int32 V,das::Context* C){if(!ActiveContext || ActiveContext!=C)C->throw_error("Enum values require active context");FString Error;auto* E=ResolveDasEnum(Path,Error);if(!E || !ValidateDasEnumNumber(Path,V,Error))C->throw_error(TCHAR_TO_UTF8(*Error));for(const auto& P:ActiveContext->EnumValues)if(P.Value.Type.Get()==E && P.Value.Value==V)return {P.Key};if(ActiveContext->EnumValues.Num()>=4096)C->throw_error("Enum value limit reached");static uint64 Next=1;const uint64 Token=Next++;ActiveContext->EnumValues.Add(Token,{E,V});return {Token};}
    const FUnrealDasContext::FEnumValue& ResolveEnumValue(FDasEnumValue V,das::Context* C){if(!ActiveContext || ActiveContext!=C)C->throw_error("Enum values require active context");auto* Value=ActiveContext->EnumValues.Find(V.Token);FString Error;if(!Value || !Value->Type.IsValid() || !ValidateDasEnumNumber(Value->Type->GetPathName(),Value->Value,Error))C->throw_error("Invalid or foreign enum value");return *Value;}
    FDasEnumValue EnumValue(const char* Path,const char* Member,das::Context* C){FString Error;auto* E=ResolveDasEnum(Path?UTF8_TO_TCHAR(Path):TEXT(""),Error);if(!E)C->throw_error(TCHAR_TO_UTF8(*Error));return MakeEnumValue(E->GetPathName(),int32(E->GetValueByNameString(Member?UTF8_TO_TCHAR(Member):TEXT(""))),C);}
    FDasEnumValue EnumFromNumber(const char* Path,int32 V,das::Context* C){return MakeEnumValue(Path?UTF8_TO_TCHAR(Path):TEXT(""),V,C);}
    FDasEnumValue StructEnumGetValue(const FDasStructValue& H,const char* Path,const char* EnumPath,das::Context* C){FInstancedStruct V=ResolveStructValue(H,C);auto F=StructLeaf(V,Path,C);auto* E=RequireCallEnum(F.Property,EnumPath,C);return MakeEnumValue(E->GetPathName(),int32(DasEnumPropertyNumber(F.Property,F.Memory)),C);}
    FDasStructValue StructEnumWithValue(const FDasStructValue& H,const char* Path,FDasEnumValue Value,das::Context* C,das::LineInfoArg* At){FInstancedStruct V=ResolveStructValue(H,C);auto F=StructLeaf(V,Path,C);const auto& E=ResolveEnumValue(Value,C);if(DasEnumPropertyType(F.Property)!=E.Type.Get() || !SetDasEnumPropertyNumber(F.Property,F.Memory,E.Value))C->throw_error("Enum struct member identity/value mismatch");return StoreStructValue(V,C,At);}
    int32 EnumNumber(FDasEnumValue V,das::Context* C){return ResolveEnumValue(V,C).Value;}
    int32 CheckedEnumNumber(FDasEnumValue V,const char* Path,das::Context* C){const auto& Value=ResolveEnumValue(V,C);if(!Path || Value.Type->GetPathName()!=UTF8_TO_TCHAR(Path))C->throw_error("Source enum snapshot identity mismatch");return Value.Value;}
    void RequireStructType(const FDasStructValue& V,const char* Path,das::Context* C){const auto& Value=ResolveStructValue(V,C);if(!Path || Value.GetScriptStruct()->GetPathName()!=UTF8_TO_TCHAR(Path))C->throw_error("Source struct snapshot identity mismatch");}
    FDasEnumValue GetSelfEnum(const char* Name,das::Context* C){auto* Self=ResolveSelf(C);auto* P=ResolveProperty<FProperty>(Self,Name,false,C);auto* Enum=DasEnumPropertyType(P);if(!Enum)C->throw_error("Expected enum property");return MakeEnumValue(Enum->GetPathName(),int32(DasEnumPropertyNumber(P,P->ContainerPtrToValuePtr<void>(Self))),C);}
    void SetSelfEnum(const char* Name,FDasEnumValue V,das::Context* C){auto* Self=ResolveSelf(C);if(ActiveContext->bCollectingSubscriptions)C->throw_error("self writes are forbidden in bind_events");auto* P=ResolveProperty<FProperty>(Self,Name,true,C);const auto& Value=ResolveEnumValue(V,C);if(DasEnumPropertyType(P)!=Value.Type.Get() || !SetDasEnumPropertyNumber(P,P->ContainerPtrToValuePtr<void>(Self),Value.Value))C->throw_error("Enum assignment type mismatch");}
    void PublishFrameValue(int32 Index,FDasScalarValue Value,das::Context* C){
        if(C!=ActiveContext || !ActiveOutputFrame || !ActiveOutputFrame->Arguments || !ActiveOutputFrame->Arguments->IsValidIndex(Index))C->throw_error("output requires active declared frame slot");
        const auto& Expected=(*ActiveOutputFrame->Arguments)[Index];
        if(Expected.Direction==0 || Value.Kind!=Expected.Kind || ActiveOutputFrame->Values.Contains(Index))C->throw_error("output slot direction/type/duplicate mismatch");
        if(Value.Kind==13){FString Error;if(Value.ObjectClass!=Expected.ObjectClass || !ValidateDasEnumNumber(Expected.ObjectClass,Value.IntValue,Error))C->throw_error("output enum identity/value mismatch");}
        if(Value.Kind==8){auto* Object=Value.ObjectValue.Get();auto* Class=FindObject<UClass>(nullptr,*Expected.ObjectClass);if(!Class || !DasReferenceMatches(Object,Class) || (Object && (!IsValid(Object) || (Cast<AActor>(Object) && CastChecked<AActor>(Object)->IsActorBeingDestroyed()))))C->throw_error("output object/interface identity mismatch");if(Object)ActiveOutputFrame->ObjectPins.Emplace(Object);}
        if((Value.Kind==9 && Value.VectorValue.ContainsNaN()) || (Value.Kind==10 && Value.RotatorValue.ContainsNaN()) || (Value.Kind==11 && (Value.TransformValue.ContainsNaN() || !Value.TransformValue.IsRotationNormalized())))C->throw_error("invalid math frame output");
        Value.Direction=Expected.Direction;Value.ObjectClass=Expected.ObjectClass;
        if(Value.Kind==12){FString Error;if(!Value.StructValue.IsValid() || Value.StructValue.GetScriptStruct()->GetPathName()!=Expected.ObjectClass || !ValidateDasStructSnapshot(Value.StructValue,Error))C->throw_error("output snapshot type/limit mismatch");}
        if((Value.Kind==1 && !FMath::IsFinite(Value.FloatValue)) || (Value.Kind==6 && !FMath::IsFinite(Value.DoubleValue)))C->throw_error("non-finite frame output");
        ActiveOutputFrame->Values.Add(Index,MoveTemp(Value));
    }
    void PublishFrameInt(int32 I,int32 X,das::Context* C){FDasScalarValue V;V.Kind=0;V.IntValue=X;PublishFrameValue(I,MoveTemp(V),C);}
    void PublishFrameFloat(int32 I,float X,das::Context* C){FDasScalarValue V;V.Kind=1;V.FloatValue=X;PublishFrameValue(I,MoveTemp(V),C);}
    void PublishFrameBool(int32 I,bool X,das::Context* C){FDasScalarValue V;V.Kind=2;V.BoolValue=X;PublishFrameValue(I,MoveTemp(V),C);}
    void PublishFrameString(int32 I,const char* X,das::Context* C){FDasScalarValue V;V.Kind=5;V.StringValue=X?UTF8_TO_TCHAR(X):TEXT("");PublishFrameValue(I,MoveTemp(V),C);}
    void PublishFrameDouble(int32 I,double X,das::Context* C){FDasScalarValue V;V.Kind=6;V.DoubleValue=X;PublishFrameValue(I,MoveTemp(V),C);}
    void PublishFrameName(int32 I,FDasNameValue X,das::Context* C){FDasScalarValue V;V.Kind=7;V.NameValue=ResolveDSLName(X,C);PublishFrameValue(I,MoveTemp(V),C);}
    void PublishFrameStruct(int32 I,const FDasStructValue& X,das::Context* C){FDasScalarValue V;V.Kind=12;V.StructValue=ResolveStructValue(X,C);PublishFrameValue(I,MoveTemp(V),C);}
    void PublishFrameObject(int32 I,const FDasObjectHandle& X,das::Context* C){FDasScalarValue V;V.Kind=8;V.ObjectValue=X.Token?ResolveObject(X,C):nullptr;PublishFrameValue(I,MoveTemp(V),C);}
    void PublishFrameEnum(int32 I,FDasEnumValue X,das::Context* C){const auto& Value=ResolveEnumValue(X,C);FDasScalarValue V;V.Kind=13;V.ObjectClass=Value.Type->GetPathName();V.IntValue=Value.Value;PublishFrameValue(I,MoveTemp(V),C);}
    void PublishFrameVector(int32 I,const FVector& X,das::Context* C){FDasScalarValue V;V.Kind=9;V.VectorValue=X;PublishFrameValue(I,MoveTemp(V),C);}
    void PublishFrameRotator(int32 I,const FRotator& X,das::Context* C){FDasScalarValue V;V.Kind=10;V.RotatorValue=X;PublishFrameValue(I,MoveTemp(V),C);}
    void PublishFrameTransform(int32 I,const FTransform& X,das::Context* C){FDasScalarValue V;V.Kind=11;V.TransformValue=X;PublishFrameValue(I,MoveTemp(V),C);}
    bool VerifyContract(const das::ProgramPtr& Program,const FString& Entry,const FDasCallContract& C)
    {
        if(C.Inputs.Num()>16 || (static_cast<uint8>(C.ReturnKind)>13 || C.ReturnKind==EDasMethodSignature::Frame)) return false;
        const FTCHARToUTF8 N(*Entry);auto* F=Program->getThisModule()->findUniqueFunction(N.Get());
        auto Matches=[](const das::TypeDeclPtr& T,EDasMethodSignature K){if(!T || T->ref || (static_cast<uint8>(K)>13 || K==EDasMethodSignature::Frame))return false;if(K==EDasMethodSignature::Enum)return T->baseType==das::Type::tHandle && T->annotation && T->annotation->name=="UEEnumValue";if(K==EDasMethodSignature::Struct)return T->baseType==das::Type::tHandle && T->annotation && T->annotation->name=="UEStructValue";if(K==EDasMethodSignature::Vector || K==EDasMethodSignature::Rotator || K==EDasMethodSignature::Transform)return T->baseType==das::Type::tHandle && T->annotation && T->annotation->name==(K==EDasMethodSignature::Vector?"UEVector":K==EDasMethodSignature::Rotator?"UERotator":"UETransform");if(K==EDasMethodSignature::Object)return T->baseType==das::Type::tHandle && T->annotation && T->annotation->name=="UObjectHandle";if(K==EDasMethodSignature::Name)return T->baseType==das::Type::tHandle && T->annotation && T->annotation->name=="UENameValue";const auto D=K==EDasMethodSignature::String?das::Type::tString:K==EDasMethodSignature::Double?das::Type::tDouble:K==EDasMethodSignature::Int?das::Type::tInt:K==EDasMethodSignature::Float?das::Type::tFloat:K==EDasMethodSignature::Bool?das::Type::tBool:das::Type::tVoid;return K==EDasMethodSignature::Void?T->baseType==D:T->isSimpleType(D);};
        if(!F || !F->exports || F->arguments.size()!=static_cast<size_t>(C.Inputs.Num()) || !Matches(F->result,C.ReturnKind)) return false;
        for(int32 I=0;I<C.Inputs.Num();++I) if(C.Inputs[I]==EDasMethodSignature::Void || !Matches(F->arguments[I]->type,C.Inputs[I])) return false;
        bool Objects=C.ReturnKind==EDasMethodSignature::Object || C.ReturnKind==EDasMethodSignature::Struct || C.ReturnKind==EDasMethodSignature::Enum;for(auto K:C.Inputs)Objects |= K==EDasMethodSignature::Object || K==EDasMethodSignature::Struct || K==EDasMethodSignature::Enum;
        {bool Found=false,Valid=true;Program->getThisModule()->structures.foreach([&](const das::StructurePtr& St){for(auto* A:St->annotations)if(A->annotation->name=="ue_class"){auto* Wire=A->arguments.find("_ue_model_json",das::Type::tString);FDasAuthoringModule M;FString Error;if(!Wire || !FDasAuthoringModule::FromJson(UTF8_TO_TCHAR(Wire->sValue.c_str()),M,Error)){Valid=false;continue;}for(const auto& Cl:M.Classes)for(const auto& Method:Cl.Methods)if(Method.EntryPoint==Entry){FDasClassSchema Schema;if(!M.ToActorSchema(Cl.Id,Schema,Error)){Valid=false;continue;}for(const auto& S:Schema.Methods)if(S.EntryPoint==Entry){Found=true;Valid &= S.Contract()==C;}}}});if(!Valid)return false;if(!Found){if(!C.InputDirections.IsEmpty())return false;if(C.ReturnKind==EDasMethodSignature::Struct || C.ReturnKind==EDasMethodSignature::Enum)return false;for(auto K:C.Inputs)if(K==EDasMethodSignature::Struct || K==EDasMethodSignature::Enum)return false;if(C.ReturnKind==EDasMethodSignature::Object && C.ReturnClass!=TEXT("/Script/CoreUObject.Object"))return false;for(int32 I=0;I<C.Inputs.Num();++I)if(C.Inputs[I]==EDasMethodSignature::Object && (!C.InputClasses.IsValidIndex(I) || C.InputClasses[I]!=TEXT("/Script/CoreUObject.Object")))return false;}}
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
        if(Expected && Expected->HasAnyClassFlags(CLASS_Interface))Expected=UObject::StaticClass();
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
    FDasObjectHandle ComponentOwner(const FDasObjectHandle& H,das::Context* C){auto* Component=Cast<UActorComponent>(ResolveObject(H,C));if(!Component)C->throw_error("expected ActorComponent handle");return MakeObjectHandle(Component->GetOwner(),AActor::StaticClass(),C);}
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
    FProperty* SelfObjectProperty(const char* N,bool Write,das::Context* C){auto* S=ResolveSelf(C);auto* P=S->GetClass()->FindPropertyByName(FName(UTF8_TO_TCHAR(N)));if(!P || P->ArrayDim!=1 || !DasReferencePropertyClass(P))C->throw_error("self reference property type mismatch");if(!P->HasAnyPropertyFlags(CPF_BlueprintVisible) || (Write && P->HasAnyPropertyFlags(CPF_BlueprintReadOnly)))C->throw_error("self property access is not permitted");return P;}
    FDasObjectHandle GetSelfObject(const char* N,das::Context* C){auto* P=SelfObjectProperty(N,false,C);return MakeObjectHandle(DasReferenceGet(P,P->ContainerPtrToValuePtr<void>(ResolveSelf(C))),DasReferencePropertyClass(P),C);}
    void SetSelfObject(const char* N,const FDasObjectHandle& V,das::Context* C){ResolveSelf(C);if(ActiveContext->bCollectingSubscriptions)C->throw_error("self writes are forbidden in bind_events");auto* P=SelfObjectProperty(N,true,C);UObject* O=V.Token?ResolveObject(V,C):nullptr;if(!DasReferenceSet(P,P->ContainerPtrToValuePtr<void>(ResolveSelf(C)),O))C->throw_error("Object/interface assignment does not match field class or interface");}
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
        if (!DasReferenceMatches(Object,Class)) Context->throw_error("object does not match requested class");
        return MakeObjectHandle(Object, Class, Context);
    }
    FDasObjectHandle ObjectCastNullable(const FDasObjectHandle& Handle, const char* ClassPath, das::Context* Context)
    {
        RequireObjectContext(Context);
        UClass* Class = ResolveClass(ClassPath, Context);
        if (!Handle.Token) return {};
        UObject* Object = ResolveObject(Handle, Context);
        if (!DasReferenceMatches(Object,Class)) Context->throw_error("object does not match requested class");
        return MakeObjectHandle(Object, Class, Context);
    }

    FDasObjectHandle ObjectProperty(const FDasObjectHandle& Handle, const char* Name, das::Context* Context)
    {
        UObject* Object = ResolveObject(Handle, Context);
        auto* Property = ResolveProperty<FObjectProperty>(Object, Name, false, Context);
        return MakeObjectHandle(Property->GetObjectPropertyValue_InContainer(Object), Property->PropertyClass, Context);
    }
UObject* ResolveAuthoredObject(const FDasObjectHandle& Handle,const char* Id,das::Context* C){auto* Object=ResolveObject(Handle,C);const UObject* CDO=Object->GetClass()->GetDefaultObject();const FString Actual=Cast<ADasProbeActor>(CDO)?CastChecked<ADasProbeActor>(CDO)->AuthoredTypeId:Cast<UDasScriptComponent>(CDO)?CastChecked<UDasScriptComponent>(CDO)->AuthoredTypeId:FString();if(!Id || Actual.IsEmpty() || Actual!=UTF8_TO_TCHAR(Id))C->throw_error("authored object identity mismatch");return Object;}
FDasObjectHandle AuthoredObjectCheck(const FDasObjectHandle& H,const char* Id,das::Context* C){ResolveAuthoredObject(H,Id,C);return H;}
FDasObjectHandle AuthoredNullableCheck(const FDasObjectHandle& Handle,const char* Id,das::Context* Context){RequireObjectContext(Context);if(!Handle.Token)return {};ResolveAuthoredObject(Handle,Id,Context);return Handle;}
int32 AuthoredGetint(const FDasObjectHandle& H,const char* Id,const char* Name,das::Context* C){auto* Object=ResolveAuthoredObject(H,Id,C);return ResolveProperty<FIntProperty>(Object,Name,false,C)->GetPropertyValue_InContainer(Object);}
void AuthoredSetint(const FDasObjectHandle& H,const char* Id,const char* Name,int32 Value,das::Context* C){auto* Object=ResolveAuthoredObject(H,Id,C);if(ActiveContext->bCollectingSubscriptions)C->throw_error("object writes are forbidden in bind_events");ResolveProperty<FIntProperty>(Object,Name,true,C)->SetPropertyValue_InContainer(Object,Value);}
float AuthoredGetfloat(const FDasObjectHandle& H,const char* Id,const char* Name,das::Context* C){auto* Object=ResolveAuthoredObject(H,Id,C);return ResolveProperty<FFloatProperty>(Object,Name,false,C)->GetPropertyValue_InContainer(Object);}
void AuthoredSetfloat(const FDasObjectHandle& H,const char* Id,const char* Name,float Value,das::Context* C){auto* Object=ResolveAuthoredObject(H,Id,C);if(ActiveContext->bCollectingSubscriptions)C->throw_error("object writes are forbidden in bind_events");ResolveProperty<FFloatProperty>(Object,Name,true,C)->SetPropertyValue_InContainer(Object,Value);}
double AuthoredGetdouble(const FDasObjectHandle& H,const char* Id,const char* Name,das::Context* C){auto* Object=ResolveAuthoredObject(H,Id,C);return ResolveProperty<FDoubleProperty>(Object,Name,false,C)->GetPropertyValue_InContainer(Object);}
void AuthoredSetdouble(const FDasObjectHandle& H,const char* Id,const char* Name,double Value,das::Context* C){auto* Object=ResolveAuthoredObject(H,Id,C);if(ActiveContext->bCollectingSubscriptions)C->throw_error("object writes are forbidden in bind_events");ResolveProperty<FDoubleProperty>(Object,Name,true,C)->SetPropertyValue_InContainer(Object,Value);}
bool AuthoredGetbool(const FDasObjectHandle& H,const char* Id,const char* Name,das::Context* C){auto* Object=ResolveAuthoredObject(H,Id,C);return ResolveProperty<FBoolProperty>(Object,Name,false,C)->GetPropertyValue_InContainer(Object);}
void AuthoredSetbool(const FDasObjectHandle& H,const char* Id,const char* Name,bool Value,das::Context* C){auto* Object=ResolveAuthoredObject(H,Id,C);if(ActiveContext->bCollectingSubscriptions)C->throw_error("object writes are forbidden in bind_events");ResolveProperty<FBoolProperty>(Object,Name,true,C)->SetPropertyValue_InContainer(Object,Value);}
char* AuthoredGetstring(const FDasObjectHandle& H,const char* Id,const char* Name,das::Context* C){auto* O=ResolveAuthoredObject(H,Id,C);return CopyDSLString(ResolveProperty<FStrProperty>(O,Name,false,C)->GetPropertyValue_InContainer(O),C);}
void AuthoredSetstring(const FDasObjectHandle& H,const char* Id,const char* Name,const char* Value,das::Context* C){auto* O=ResolveAuthoredObject(H,Id,C);if(ActiveContext->bCollectingSubscriptions)C->throw_error("object writes are forbidden in bind_events");ResolveProperty<FStrProperty>(O,Name,true,C)->SetPropertyValue_InContainer(O,Value?UTF8_TO_TCHAR(Value):TEXT(""));}
FDasNameValue AuthoredGetname(const FDasObjectHandle& H,const char* Id,const char* Name,das::Context* C){auto* O=ResolveAuthoredObject(H,Id,C);return MakeDSLName(ResolveProperty<FNameProperty>(O,Name,false,C)->GetPropertyValue_InContainer(O),C);}
void AuthoredSetname(const FDasObjectHandle& H,const char* Id,const char* Name,FDasNameValue Value,das::Context* C){auto* O=ResolveAuthoredObject(H,Id,C);if(ActiveContext->bCollectingSubscriptions)C->throw_error("object writes are forbidden in bind_events");ResolveProperty<FNameProperty>(O,Name,true,C)->SetPropertyValue_InContainer(O,ResolveDSLName(Value,C));}


FDasStructValue AuthoredGetstruct(const FDasObjectHandle& H,const char* Id,const char* Name,das::Context* C,das::LineInfoArg* At){auto* O=ResolveAuthoredObject(H,Id,C);auto* P=ResolveProperty<FProperty>(O,Name,false,C);return ReadPropertySnapshot(P,P->ContainerPtrToValuePtr<void>(O),C,At);}
void AuthoredSetstruct(const FDasObjectHandle& H,const char* Id,const char* Name,const FDasStructValue& Value,das::Context* C){auto* O=ResolveAuthoredObject(H,Id,C);if(ActiveContext->bCollectingSubscriptions)C->throw_error("object writes are forbidden in bind_events");auto* P=ResolveProperty<FProperty>(O,Name,true,C);WritePropertySnapshot(P,P->ContainerPtrToValuePtr<void>(O),Value,C);}
FDasEnumValue AuthoredGetenum(const FDasObjectHandle& H,const char* Id,const char* Name,das::Context* C){auto* O=ResolveAuthoredObject(H,Id,C);auto* P=ResolveProperty<FProperty>(O,Name,false,C);auto* E=DasEnumPropertyType(P);if(!E)C->throw_error("Expected authored enum property");return MakeEnumValue(E->GetPathName(),int32(DasEnumPropertyNumber(P,P->ContainerPtrToValuePtr<void>(O))),C);}
void AuthoredSetenum(const FDasObjectHandle& H,const char* Id,const char* Name,FDasEnumValue Value,das::Context* C){auto* O=ResolveAuthoredObject(H,Id,C);if(ActiveContext->bCollectingSubscriptions)C->throw_error("object writes are forbidden in bind_events");auto* P=ResolveProperty<FProperty>(O,Name,true,C);const auto& V=ResolveEnumValue(Value,C);if(DasEnumPropertyType(P)!=V.Type.Get() || !SetDasEnumPropertyNumber(P,P->ContainerPtrToValuePtr<void>(O),V.Value))C->throw_error("Authored enum assignment type mismatch");}
FDasObjectHandle AuthoredGetobject(const FDasObjectHandle& H,const char* Id,const char* Name,const char* Expected,das::Context* C){auto* O=ResolveAuthoredObject(H,Id,C);auto* P=ResolveProperty<FProperty>(O,Name,false,C);auto* Type=DasReferencePropertyClass(P);if(!Type)C->throw_error("Expected authored object/interface property");if(Expected && *Expected && Type->GetPathName()!=UTF8_TO_TCHAR(Expected))C->throw_error("Authored object property class identity mismatch");return MakeObjectHandle(DasReferenceGet(P,P->ContainerPtrToValuePtr<void>(O)),Type,C);}
void AuthoredSetobject(const FDasObjectHandle& H,const char* Id,const char* Name,const char* Expected,const FDasObjectHandle& Value,das::Context* C){auto* O=ResolveAuthoredObject(H,Id,C);if(ActiveContext->bCollectingSubscriptions)C->throw_error("object writes are forbidden in bind_events");auto* P=ResolveProperty<FProperty>(O,Name,true,C);auto* Type=DasReferencePropertyClass(P);if(!Type || (Expected && *Expected && Type->GetPathName()!=UTF8_TO_TCHAR(Expected)))C->throw_error("Authored object property class identity mismatch");if(!DasReferenceSet(P,P->ContainerPtrToValuePtr<void>(O),Value.Token?ResolveObject(Value,C):nullptr))C->throw_error("Authored object/interface assignment type mismatch");}

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
    UClass* CheckedInterface(const char* Path,das::Context* C){auto* I=ResolveClass(Path,C);if((!FString(Path).StartsWith(TEXT("/Script/")) && !FString(Path).StartsWith(TEXT("/Game/"))) || !I->HasAnyClassFlags(CLASS_Interface))C->throw_error("expected loaded native UInterface path");return I;}
    FDasObjectHandle CastInterface(const FDasObjectHandle& Handle,const char* Path,das::Context* C){auto* Interface=CheckedInterface(Path,C);if(!Handle.Token)return {};auto* Object=ResolveObject(Handle,C);if(!Object->GetClass()->ImplementsInterface(Interface))C->throw_error("object does not implement requested interface");return Handle;}
    bool ObjectImplementsInterface(const FDasObjectHandle& H,const char* Path,das::Context* C){auto* I=CheckedInterface(Path,C);auto* O=TryObject(H,C);return O && O->GetClass()->ImplementsInterface(I);}
    UFunction* CheckedInterfaceFunction(UObject* O,const char* Path,const char* Name,das::Context* C){auto* I=CheckedInterface(Path,C);if(!O->GetClass()->ImplementsInterface(I))C->throw_error("object does not implement requested interface");if(!Name || !*Name)C->throw_error("expected interface method name");auto* Declared=I->FindFunctionByName(FName(UTF8_TO_TCHAR(Name)));auto* Actual=O->FindFunction(FName(UTF8_TO_TCHAR(Name)));if((O->IsA<ADasProbeActor>() || O->IsA<UDasScriptComponent>()) && ActiveContext)C->throw_error("interface dispatch into daScript requires a later reentrant backend");if(!Declared || !Declared->HasAnyFunctionFlags(FUNC_BlueprintEvent) || !Actual)C->throw_error("reflected interface implementation is unavailable");if(!Actual->IsSignatureCompatibleWith(Declared))C->throw_error("interface dispatch signature mismatch");return Actual;}
    int32 InterfaceCallInt(const FDasObjectHandle& H,const char* Path,const char* Name,int32 Input,das::Context* C){auto* O=ResolveObject(H,C);CheckedInterfaceFunction(O,Path,Name,C);return CallInt(H,Name,Input,C);}
    char* InterfaceCallString(const FDasObjectHandle& H,const char* Path,const char* Name,das::Context* C){auto* O=ResolveObject(H,C);auto* F=CheckedInterfaceFunction(O,Path,Name,C);if(ActiveContext->bCollectingSubscriptions || F->HasAnyFunctionFlags(FUNC_Static|FUNC_Net|FUNC_Delegate|FUNC_MulticastDelegate) || F->NumParms!=1 || !CastField<FStrProperty>(F->GetReturnProperty()))C->throw_error("interface string getter signature mismatch");FStructOnScope Args(F);TStrongObjectPtr<UObject> Keep(O);O->ProcessEvent(F,Args.GetStructMemory());return CopyDSLString(CastFieldChecked<FStrProperty>(F->GetReturnProperty())->GetPropertyValue_InContainer(Args.GetStructMemory()),C);}
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

// Cleanup is idempotent: a rejected BeginCall may not have produced a handle yet.
void ReleaseAuthoredCall(const FDasCallHandle& H,das::Context* C){if(C==ActiveContext && ActiveContext)ActiveContext->Calls.Remove(H.Token);}
FDasCallHandle BeginAuthoredCall(const FDasObjectHandle& Target,const char* Id,const char* Name,const char* Wire,das::Context* Context){
 auto* Object=ResolveAuthoredObject(Target,Id,Context);FDasAuthoringModule Model;FString Error;if(!Wire || !FDasAuthoringModule::FromJson(UTF8_TO_TCHAR(Wire),Model,Error) || Model.Classes.Num()!=1 || Model.Classes[0].Id!=UTF8_TO_TCHAR(Id))Context->throw_error("invalid authored method contract");
 const auto* Method=Model.Classes[0].Methods.FindByPredicate([&](const auto& Item){return Name && Item.Name==UTF8_TO_TCHAR(Name);});if(!Method || !Method->bCallable || Method->bStatic)Context->throw_error("unknown callable authored method");
 auto Handle=BeginCall(Target,Name,Context);auto& Call=ResolveCall(Handle,Context);

 auto Matches=[&](FProperty* Property,const FDasDeclaredType& Type){if(!Property || FString(CallKindName(FieldKind(Property,Context)))!=Type.Kind)return false;if(Type.Kind==TEXT("array") || Type.Kind==TEXT("map") || Type.Kind==TEXT("set")){auto* Carrier=DasContainerCarrier(Type);auto* Values=Carrier?Carrier->FindPropertyByName(TEXT("Values")):nullptr;return Values && Values->SameType(Property);}if(auto* Enum=CallEnumType(Property))return Enum->GetPathName()==Type.TypePath;if(auto* Struct=CastField<FStructProperty>(Property))return Struct->Struct->GetPathName()==Type.TypePath;if(auto* Class=DasReferencePropertyClass(Property))return Class->GetPathName()==Type.TypePath;return true;};
 if(Method->ReturnType.Kind==TEXT("void")?Call.Function->GetReturnProperty()!=nullptr:!Matches(Call.Function->GetReturnProperty(),Method->ReturnType))Context->throw_error("authored method return contract changed");
 int32 Index=0;for(TFieldIterator<FProperty> It(Call.Function.Get());It;++It){if(!It->HasAnyPropertyFlags(CPF_Parm) || It->HasAnyPropertyFlags(CPF_ReturnParm))continue;if(!Method->Parameters.IsValidIndex(Index))Context->throw_error("authored method parameter count changed");const auto& Param=Method->Parameters[Index++];const bool Out=It->HasAnyPropertyFlags(CPF_OutParm) && !It->HasAnyPropertyFlags(CPF_ConstParm);const FString Direction=Out?(It->HasAnyPropertyFlags(CPF_ReferenceParm)?TEXT("inout"):TEXT("out")):TEXT("in");const bool ContainerInput=Param.Direction==TEXT("in") && Direction==TEXT("inout") && (Param.Type.Kind==TEXT("array") || Param.Type.Kind==TEXT("map") || Param.Type.Kind==TEXT("set"));if(It->GetName()!=Param.Name || (Direction!=Param.Direction && !ContainerInput) || !Matches(*It,Param.Type))Context->throw_error(TCHAR_TO_UTF8(*FString::Printf(TEXT("authored method parameter contract changed: %s.%s direction=%s/%s kind=%s/%s matches=%d"),*Method->Name,*It->GetName(),*Direction,*Param.Direction,CallKindName(FieldKind(*It,Context)),*Param.Type.Kind,Matches(*It,Param.Type))));}
 if(Index!=Method->Parameters.Num())Context->throw_error("authored method parameter count changed");Call.bAuthored=true;return Handle;
}
void MethodArgname(const FDasCallHandle& H,const char* Path,FDasNameValue Value,das::Context* C){const auto Name=ResolveDSLName(Value,C);SetCallName(H,Path,TCHAR_TO_UTF8(*Name.ToString()),C);}
FDasNameValue MethodResultname(const FDasCallHandle& H,const char* Path,das::Context* C){return MakeDSLName(GetCallScalar<FNameProperty>(H,Path,C),C);}
void MethodArgenum(const FDasCallHandle& H,const char* Path,FDasEnumValue Value,das::Context* C){const auto& V=ResolveEnumValue(Value,C);auto& Call=ResolveCall(H,C);const auto Field=ResolveCallField(Call,Path,false,C);RequireCallEnum(Field.Property,TCHAR_TO_UTF8(*V.Type->GetPathName()),C);CallEnumStorage(Field.Property)->SetIntPropertyValue(Field.Memory,int64(V.Value));Call.Assigned.Add(Field.Path);}
FDasEnumValue MethodResultenum(const FDasCallHandle& H,const char* Path,das::Context* C){auto& Call=ResolveCall(H,C);const auto Field=ResolveCallField(Call,Path,true,C);auto* Enum=CallEnumType(Field.Property);if(!Enum)C->throw_error("expected enum result");return MakeEnumValue(Enum->GetPathName(),int32(CallEnumStorage(Field.Property)->GetSignedIntPropertyValue(Field.Memory)),C);}
// Whole struct snapshots avoid exposing UE addresses or retaining stale property pointers.
FCallField MethodStructField(FMarshalledCall& Call,const char* Path,bool Output,das::Context* C){
 if(Call.bFailed || Call.bExecuted!=Output || !Path)C->throw_error("invalid method snapshot access direction");auto* Prop=FindFProperty<FProperty>(Call.Function.Get(),FName(UTF8_TO_TCHAR(Path)));if(!Prop || !Prop->HasAnyPropertyFlags(CPF_Parm) || (!CastField<FStructProperty>(Prop) && !DasContainerCarrierForProperty(Prop)))C->throw_error("expected struct/container method parameter");const bool MutableOut=Prop->HasAnyPropertyFlags(CPF_OutParm) && !Prop->HasAnyPropertyFlags(CPF_ConstParm);const bool Return=Prop->HasAnyPropertyFlags(CPF_ReturnParm);if(Output?!(Return || MutableOut):(Return || (MutableOut && !Prop->HasAnyPropertyFlags(CPF_ReferenceParm))))C->throw_error("wrong snapshot parameter direction");return {Prop,Prop->ContainerPtrToValuePtr<void>(Call.Frame->GetStructMemory()),Prop->GetName().ToLower()};
}
void PinMethodSnapshot(FProperty* Property,void* Memory,FMarshalledCall& Call,const FString& Path,das::Context* C,int32& Budget,int32 Depth=0){
 if(Depth>16 || --Budget<0)C->throw_error("method snapshot traversal limit exceeded");
 if(DasReferencePropertyClass(Property)){auto* Object=DasReferenceGet(Property,Memory);if(Object && (!IsValid(Object) || (Cast<AActor>(Object) && CastChecked<AActor>(Object)->IsActorBeingDestroyed())))C->throw_error("method snapshot object is expired");Call.ObjectPins.Add(Path,TStrongObjectPtr<UObject>(Object));}
 else if(auto* Struct=CastField<FStructProperty>(Property)){for(TFieldIterator<FProperty> It(Struct->Struct);It;++It)PinMethodSnapshot(*It,It->ContainerPtrToValuePtr<void>(Memory),Call,Path+TEXT(".")+It->GetName().ToLower(),C,Budget,Depth+1);}
 else if(auto* Array=CastField<FArrayProperty>(Property)){FScriptArrayHelper Values(Array,Memory);if(Values.Num()>MaxCallArrayElements)C->throw_error("method array size limit exceeded");for(int32 Index=0;Index<Values.Num();++Index)PinMethodSnapshot(Array->Inner,Values.GetRawPtr(Index),Call,Path+TEXT(".")+FString::FromInt(Index),C,Budget,Depth+1);}
 else if(auto* Map=CastField<FMapProperty>(Property)){FScriptMapHelper Values(Map,Memory);if(Values.Num()>MaxCallArrayElements)C->throw_error("method map size limit exceeded");for(int32 Index=0;Index<Values.GetMaxIndex();++Index)if(Values.IsValidIndex(Index)){PinMethodSnapshot(Map->KeyProp,Values.GetKeyPtr(Index),Call,Path+TEXT(".key")+FString::FromInt(Index),C,Budget,Depth+1);PinMethodSnapshot(Map->ValueProp,Values.GetValuePtr(Index),Call,Path+TEXT(".value")+FString::FromInt(Index),C,Budget,Depth+1);}}
 else if(auto* Set=CastField<FSetProperty>(Property)){FScriptSetHelper Values(Set,Memory);if(Values.Num()>MaxCallArrayElements)C->throw_error("method set size limit exceeded");for(int32 Index=0;Index<Values.GetMaxIndex();++Index)if(Values.IsValidIndex(Index))PinMethodSnapshot(Set->ElementProp,Values.GetElementPtr(Index),Call,Path+TEXT(".")+FString::FromInt(Index),C,Budget,Depth+1);}
}
void MethodArgstruct(const FDasCallHandle& H,const char* Path,const FDasStructValue& Value,das::Context* C){
 auto& Call=ResolveCall(H,C);const auto Field=MethodStructField(Call,Path,false,C);const auto& Snapshot=ResolveStructValue(Value,C);FString Error;if(!ValidateDasStructSnapshot(Snapshot,Error))C->throw_error(TCHAR_TO_UTF8(*Error));
 if(auto* Prop=CastField<FStructProperty>(Field.Property)){if(Snapshot.GetScriptStruct()!=Prop->Struct)C->throw_error("method struct identity mismatch");Prop->Struct->CopyScriptStruct(Field.Memory,Snapshot.GetMemory());}
 else {auto* Carrier=DasContainerCarrierForProperty(Field.Property);auto* Values=Carrier?Carrier->FindPropertyByName(TEXT("Values")):nullptr;if(Snapshot.GetScriptStruct()!=Carrier || !Values || !Values->SameType(Field.Property))C->throw_error("method container identity mismatch");Field.Property->CopyCompleteValue(Field.Memory,Values->ContainerPtrToValuePtr<void>(Snapshot.GetMemory()));}
 int32 Budget=1048576;PinMethodSnapshot(Field.Property,Field.Memory,Call,Field.Path,C,Budget);Call.WholeInputs.Add(Field.Path);
}
FDasStructValue MethodResultstruct(const FDasCallHandle& H,const char* Path,das::Context* C,das::LineInfoArg* At){
 auto& Call=ResolveCall(H,C);const auto Field=MethodStructField(Call,Path,true,C);FInstancedStruct Snapshot;
 if(auto* Prop=CastField<FStructProperty>(Field.Property))Snapshot.InitializeAs(Prop->Struct,static_cast<const uint8*>(Field.Memory));
 else {auto* Carrier=DasContainerCarrierForProperty(Field.Property);Snapshot.InitializeAs(Carrier);auto* Values=Carrier->FindPropertyByName(TEXT("Values"));Values->CopyCompleteValue(Values->ContainerPtrToValuePtr<void>(Snapshot.GetMutableMemory()),Field.Memory);}
 return StoreStructValue(Snapshot,C,At);
}


// Math values use native copied storage in the same reflected call frame.
template<class T> FDasStructValue MathPack(const T& Value,das::Context* C,das::LineInfoArg* At){FInstancedStruct Snapshot;Snapshot.InitializeAs(TBaseStructure<T>::Get(),reinterpret_cast<const uint8*>(&Value));return StoreStructValue(Snapshot,C,At);}
template<class T> T MathUnpack(const FDasStructValue& Value,das::Context* C){const auto& Snapshot=ResolveStructValue(Value,C);if(Snapshot.GetScriptStruct()!=TBaseStructure<T>::Get() || !MathSnapshotValid(Snapshot))C->throw_error("math property snapshot type/value mismatch");return Snapshot.Get<T>();}
FDasStructValue MathPackvector(const FVector& V,das::Context* C,das::LineInfoArg* At){return MathPack(V,C,At);}
FDasStructValue MathPackrotator(const FRotator& V,das::Context* C,das::LineInfoArg* At){return MathPack(V,C,At);}
FDasStructValue MathPacktransform(const FTransform& V,das::Context* C,das::LineInfoArg* At){return MathPack(V,C,At);}
FVector MathUnpackvector(const FDasStructValue& V,das::Context* C){return MathUnpack<FVector>(V,C);}
FRotator MathUnpackrotator(const FDasStructValue& V,das::Context* C){return MathUnpack<FRotator>(V,C);}
FTransform MathUnpacktransform(const FDasStructValue& V,das::Context* C){return MathUnpack<FTransform>(V,C);}
bool MethodMathValid(const FVector& Value){return !Value.ContainsNaN();}
bool MethodMathValid(const FRotator& Value){return !Value.ContainsNaN();}
bool MethodMathValid(const FTransform& Value){return !Value.ContainsNaN() && Value.IsRotationNormalized();}
template<class T> void MethodArgMath(const FDasCallHandle& H,const char* Path,const T& Value,das::Context* C){auto& Call=ResolveCall(H,C);const auto Field=MethodStructField(Call,Path,false,C);auto* Prop=CastField<FStructProperty>(Field.Property);if(!Prop || Prop->Struct!=TBaseStructure<T>::Get() || !MethodMathValid(Value))C->throw_error("method math input type/value mismatch");Prop->Struct->CopyScriptStruct(Field.Memory,&Value);Call.WholeInputs.Add(Field.Path);}
template<class T> T MethodResultMath(const FDasCallHandle& H,const char* Path,das::Context* C){auto& Call=ResolveCall(H,C);const auto Field=MethodStructField(Call,Path,true,C);auto* Prop=CastField<FStructProperty>(Field.Property);if(!Prop || Prop->Struct!=TBaseStructure<T>::Get())C->throw_error("method math result type mismatch");T Value=*static_cast<const T*>(Field.Memory);if(!MethodMathValid(Value))C->throw_error("method math result value mismatch");return Value;}
void MethodArgvector(const FDasCallHandle& H,const char* P,const FVector& V,das::Context* C){MethodArgMath(H,P,V,C);}
void MethodArgrotator(const FDasCallHandle& H,const char* P,const FRotator& V,das::Context* C){MethodArgMath(H,P,V,C);}
void MethodArgtransform(const FDasCallHandle& H,const char* P,const FTransform& V,das::Context* C){MethodArgMath(H,P,V,C);}
FVector MethodResultvector(const FDasCallHandle& H,const char* P,das::Context* C){return MethodResultMath<FVector>(H,P,C);}
FRotator MethodResultrotator(const FDasCallHandle& H,const char* P,das::Context* C){return MethodResultMath<FRotator>(H,P,C);}
FTransform MethodResulttransform(const FDasCallHandle& H,const char* P,das::Context* C){return MethodResultMath<FTransform>(H,P,C);}


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

    struct FDSLEnumAnnotation:das::ManagedStructureAnnotation<FDasEnumValue,false>{FDSLEnumAnnotation(das::ModuleLibrary& L):ManagedStructureAnnotation("UEEnumValue",L,"FDasEnumValue"){}virtual das::TypeDeclPtr makeValueType()const override{static_assert(sizeof(FDasEnumValue)==sizeof(uint64));return new das::TypeDecl(das::Type::tUInt64,das::LineInfo());}virtual das::SimNode* simulateClone(das::Context& C,const das::LineInfo& At,das::SimNode* Left,das::SimNode* Right)const override{return C.code->makeNode<das::SimNode_Set<FDasEnumValue>>(At,Left,Right);}virtual bool isRefType()const override{return false;}virtual bool isLocal()const override{return true;}virtual bool canCopy()const override{return true;}virtual bool canMove()const override{return true;}virtual bool hasNonTrivialCtor()const override{return false;}virtual bool isPod()const override{return true;}};
    struct FDSLNameAnnotation:das::ManagedStructureAnnotation<FDasNameValue,false>{
        FDSLNameAnnotation(das::ModuleLibrary& L):ManagedStructureAnnotation("UENameValue",L,"FDasNameValue"){}
        // Reference-to-value conversion during table cloning/calls copies the owned token.
        virtual das::TypeDeclPtr makeValueType() const override {static_assert(sizeof(FDasNameValue)==sizeof(uint64));return new das::TypeDecl(das::Type::tUInt64,das::LineInfo());}
        virtual das::SimNode* simulateClone(das::Context& C,const das::LineInfo& At,das::SimNode* Left,das::SimNode* Right) const override {return C.code->makeNode<das::SimNode_Set<FDasNameValue>>(At,Left,Right);}
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
            addAnnotation(new FUENativeInterfaceAnnotation());addAnnotation(new FUENativeInterfaceAnnotation("ue_interface_ref"));addAnnotation(new FUESourceValueBinding());addAnnotation(new FUEObjectTypeAnnotation());addAnnotation(new FUEAuthoredRefAnnotation());addTypeMacro(new FUEObjectTypeMacro());
            addAnnotation(new FUEFunctionAnnotation());addAnnotation(new FUECallbackAnnotation(false));addAnnotation(new FUECallbackAnnotation(true));addAnnotation(new FUEInterfaceAnnotation());addAnnotation(new FUEImplementsAnnotation());addAnnotation(new FUEComponentAnnotation());
            inferMacros.push_back(das::unique_ptr<das::PassMacro>(new FUEPropertyPass()));for(bool Owner:{false,true}){auto* Macro=new FUEReferenceFactory(Owner);ownedCallMacros.push_back(das::unique_ptr<das::CallMacro>(Macro));Macro->module=this;addCallMacro(Macro->name,[Macro](const das::LineInfo& At)->das::ExprLooksLikeCall*{auto* E=new das::ExprCallMacro(At,Macro->name);E->macro=Macro;return E;});}
            das::ModuleLibrary Library(this);
            Library.addBuiltInModule();
            addAnnotation(new FDSLNameAnnotation(Library));addAnnotation(new FDSLEnumAnnotation(Library));
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
            das::addExtern<DAS_BIND_FUN(ExpectInterfaceSignature)>(*this,Library,"ue_expect_interface_signature",das::SideEffects::modifyExternal,"ExpectInterfaceSignature");
            das::addExtern<DAS_BIND_FUN(BeginInterfaceCall),das::SimNode_ExtFuncCallAndCopyOrMove>(*this,Library,"ue_begin_interface_call",das::SideEffects::modifyExternal,"BeginInterfaceCall");

            das::addExtern<DAS_BIND_FUN(MathPackvector),das::SimNode_ExtFuncCallAndCopyOrMove>(*this,Library,"__ue_math_pack_vector",das::SideEffects::modifyExternal,"MathPackvector")->needCallerStackFrame=true;
            das::addExtern<DAS_BIND_FUN(MathUnpackvector),das::SimNode_ExtFuncCallAndCopyOrMove>(*this,Library,"__ue_math_unpack_vector",das::SideEffects::modifyExternal,"MathUnpackvector");
            das::addExtern<DAS_BIND_FUN(MathPackrotator),das::SimNode_ExtFuncCallAndCopyOrMove>(*this,Library,"__ue_math_pack_rotator",das::SideEffects::modifyExternal,"MathPackrotator")->needCallerStackFrame=true;
            das::addExtern<DAS_BIND_FUN(MathUnpackrotator),das::SimNode_ExtFuncCallAndCopyOrMove>(*this,Library,"__ue_math_unpack_rotator",das::SideEffects::modifyExternal,"MathUnpackrotator");
            das::addExtern<DAS_BIND_FUN(MathPacktransform),das::SimNode_ExtFuncCallAndCopyOrMove>(*this,Library,"__ue_math_pack_transform",das::SideEffects::modifyExternal,"MathPacktransform")->needCallerStackFrame=true;
            das::addExtern<DAS_BIND_FUN(MathUnpacktransform),das::SimNode_ExtFuncCallAndCopyOrMove>(*this,Library,"__ue_math_unpack_transform",das::SideEffects::modifyExternal,"MathUnpacktransform");
            das::addExtern<DAS_BIND_FUN(MethodArgvector)>(*this,Library,"__ue_method_arg_vector",das::SideEffects::modifyExternal,"MethodArgvector");
            das::addExtern<DAS_BIND_FUN(MethodResultvector),das::SimNode_ExtFuncCallAndCopyOrMove>(*this,Library,"__ue_method_result_vector",das::SideEffects::modifyExternal,"MethodResultvector");
            das::addExtern<DAS_BIND_FUN(MethodArgrotator)>(*this,Library,"__ue_method_arg_rotator",das::SideEffects::modifyExternal,"MethodArgrotator");
            das::addExtern<DAS_BIND_FUN(MethodResultrotator),das::SimNode_ExtFuncCallAndCopyOrMove>(*this,Library,"__ue_method_result_rotator",das::SideEffects::modifyExternal,"MethodResultrotator");
            das::addExtern<DAS_BIND_FUN(MethodArgtransform)>(*this,Library,"__ue_method_arg_transform",das::SideEffects::modifyExternal,"MethodArgtransform");
            das::addExtern<DAS_BIND_FUN(MethodResulttransform),das::SimNode_ExtFuncCallAndCopyOrMove>(*this,Library,"__ue_method_result_transform",das::SideEffects::modifyExternal,"MethodResulttransform");
            das::addExtern<DAS_BIND_FUN(ReleaseAuthoredCall)>(*this,Library,"__ue_release_authored_call",das::SideEffects::modifyExternal,"ReleaseAuthoredCall");
            das::addExtern<DAS_BIND_FUN(BeginAuthoredCall),das::SimNode_ExtFuncCallAndCopyOrMove>(*this,Library,"__ue_begin_authored_call",das::SideEffects::modifyExternal,"BeginAuthoredCall");
            das::addExtern<DAS_BIND_FUN(MethodArgname)>(*this,Library,"__ue_method_arg_name",das::SideEffects::modifyExternal,"MethodArgname");
            das::addExtern<DAS_BIND_FUN(MethodResultname)>(*this,Library,"__ue_method_result_name",das::SideEffects::modifyExternal,"MethodResultname");
            das::addExtern<DAS_BIND_FUN(MethodArgenum)>(*this,Library,"__ue_method_arg_enum",das::SideEffects::modifyExternal,"MethodArgenum");
            das::addExtern<DAS_BIND_FUN(MethodResultenum)>(*this,Library,"__ue_method_result_enum",das::SideEffects::modifyExternal,"MethodResultenum");
            das::addExtern<DAS_BIND_FUN(MethodArgstruct)>(*this,Library,"__ue_method_arg_struct",das::SideEffects::modifyExternal,"MethodArgstruct");
            das::addExtern<DAS_BIND_FUN(MethodResultstruct),das::SimNode_ExtFuncCallAndCopyOrMove>(*this,Library,"__ue_method_result_struct",das::SideEffects::modifyExternal,"MethodResultstruct")->needCallerStackFrame=true;
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
            das::addExtern<DAS_BIND_FUN(EnumValue)>(*this,Library,"ue_enum_value",das::SideEffects::modifyExternal,"EnumValue");
            das::addExtern<DAS_BIND_FUN(EnumFromNumber)>(*this,Library,"ue_enum_from_number",das::SideEffects::modifyExternal,"EnumFromNumber");
            das::addExtern<DAS_BIND_FUN(StructEnumGetValue)>(*this,Library,"__ue_struct_get_enum_value",das::SideEffects::modifyExternal,"StructEnumGetValue");
            das::addExtern<DAS_BIND_FUN(StructEnumWithValue),das::SimNode_ExtFuncCallAndCopyOrMove>(*this,Library,"__ue_struct_with_enum_value",das::SideEffects::modifyExternal,"StructEnumWithValue")->needCallerStackFrame=true;
            das::addExtern<DAS_BIND_FUN(EnumNumber)>(*this,Library,"ue_enum_number",das::SideEffects::accessExternal,"EnumNumber");
            das::addExtern<DAS_BIND_FUN(CheckedEnumNumber)>(*this,Library,"__ue_checked_enum_number",das::SideEffects::accessExternal,"CheckedEnumNumber");
            das::addExtern<DAS_BIND_FUN(RequireStructType)>(*this,Library,"__ue_require_struct_type",das::SideEffects::accessExternal,"RequireStructType");
            das::addExtern<DAS_BIND_FUN(GetSelfEnum)>(*this,Library,"ue_get_self_enum",das::SideEffects::modifyExternal,"GetSelfEnum");
            das::addExtern<DAS_BIND_FUN(SetSelfEnum)>(*this,Library,"ue_set_self_enum",das::SideEffects::modifyExternal,"SetSelfEnum");
            das::addExtern<DAS_BIND_FUN(NullObject), das::SimNode_ExtFuncCallAndCopyOrMove>(*this, Library, "ue_null_object", das::SideEffects::none, "NullObject");
            das::addExtern<DAS_BIND_FUN(ObjectSelf), das::SimNode_ExtFuncCallAndCopyOrMove>(*this, Library, "ue_self_object", das::SideEffects::modifyExternal, "ObjectSelf");
            das::addExtern<DAS_BIND_FUN(ComponentOwner),das::SimNode_ExtFuncCallAndCopyOrMove>(*this,Library,"ue_component_owner",das::SideEffects::accessExternal,"ComponentOwner");
            das::addExtern<DAS_BIND_FUN(ObjectProperty), das::SimNode_ExtFuncCallAndCopyOrMove>(*this, Library, "ue_get_object", das::SideEffects::modifyExternal, "ObjectProperty");
            das::addExtern<DAS_BIND_FUN(ObjectCast), das::SimNode_ExtFuncCallAndCopyOrMove>(*this, Library, "ue_cast_object", das::SideEffects::modifyExternal, "ObjectCast");
            das::addExtern<DAS_BIND_FUN(ObjectValid)>(*this, Library, "ue_object_valid", das::SideEffects::accessExternal, "ObjectValid");
            das::addExtern<DAS_BIND_FUN(ObjectIsA)>(*this, Library, "ue_object_is_a", das::SideEffects::accessExternal, "ObjectIsA");
            das::addExtern<DAS_BIND_FUN(AuthoredNullableCheck),das::SimNode_ExtFuncCallAndCopyOrMove>(*this,Library,"__ue_authored_nullable_check",das::SideEffects::accessExternal,"AuthoredNullableCheck");
            das::addExtern<DAS_BIND_FUN(AuthoredObjectCheck),das::SimNode_ExtFuncCallAndCopyOrMove>(*this,Library,"__ue_authored_object_check",das::SideEffects::accessExternal,"AuthoredObjectCheck");
das::addExtern<DAS_BIND_FUN(AuthoredGetint)>(*this,Library,"__ue_authored_get_int",das::SideEffects::accessExternal,"AuthoredGetint");
das::addExtern<DAS_BIND_FUN(AuthoredSetint)>(*this,Library,"__ue_authored_set_int",das::SideEffects::modifyExternal,"AuthoredSetint");
das::addExtern<DAS_BIND_FUN(AuthoredGetfloat)>(*this,Library,"__ue_authored_get_float",das::SideEffects::accessExternal,"AuthoredGetfloat");
das::addExtern<DAS_BIND_FUN(AuthoredSetfloat)>(*this,Library,"__ue_authored_set_float",das::SideEffects::modifyExternal,"AuthoredSetfloat");
das::addExtern<DAS_BIND_FUN(AuthoredGetdouble)>(*this,Library,"__ue_authored_get_double",das::SideEffects::accessExternal,"AuthoredGetdouble");
das::addExtern<DAS_BIND_FUN(AuthoredSetdouble)>(*this,Library,"__ue_authored_set_double",das::SideEffects::modifyExternal,"AuthoredSetdouble");
das::addExtern<DAS_BIND_FUN(AuthoredGetbool)>(*this,Library,"__ue_authored_get_bool",das::SideEffects::accessExternal,"AuthoredGetbool");
das::addExtern<DAS_BIND_FUN(AuthoredSetbool)>(*this,Library,"__ue_authored_set_bool",das::SideEffects::modifyExternal,"AuthoredSetbool");
das::addExtern<DAS_BIND_FUN(AuthoredGetstring)>(*this,Library,"__ue_authored_get_string",das::SideEffects::modifyExternal,"AuthoredGetstring");
das::addExtern<DAS_BIND_FUN(AuthoredSetstring)>(*this,Library,"__ue_authored_set_string",das::SideEffects::modifyExternal,"AuthoredSetstring");
das::addExtern<DAS_BIND_FUN(AuthoredGetname)>(*this,Library,"__ue_authored_get_name",das::SideEffects::modifyExternal,"AuthoredGetname");
das::addExtern<DAS_BIND_FUN(AuthoredSetname)>(*this,Library,"__ue_authored_set_name",das::SideEffects::modifyExternal,"AuthoredSetname");
            das::addExtern<DAS_BIND_FUN(AuthoredGetenum)>(*this,Library,"__ue_authored_get_enum",das::SideEffects::modifyExternal,"AuthoredGetenum");
das::addExtern<DAS_BIND_FUN(AuthoredSetenum)>(*this,Library,"__ue_authored_set_enum",das::SideEffects::modifyExternal,"AuthoredSetenum");
das::addExtern<DAS_BIND_FUN(AuthoredGetstruct),das::SimNode_ExtFuncCallAndCopyOrMove>(*this,Library,"__ue_authored_get_struct",das::SideEffects::modifyExternal,"AuthoredGetstruct")->needCallerStackFrame=true;
das::addExtern<DAS_BIND_FUN(AuthoredSetstruct)>(*this,Library,"__ue_authored_set_struct",das::SideEffects::modifyExternal,"AuthoredSetstruct");
das::addExtern<DAS_BIND_FUN(AuthoredGetobject),das::SimNode_ExtFuncCallAndCopyOrMove>(*this,Library,"__ue_authored_get_object",das::SideEffects::modifyExternal,"AuthoredGetobject");
das::addExtern<DAS_BIND_FUN(AuthoredSetobject)>(*this,Library,"__ue_authored_set_object",das::SideEffects::modifyExternal,"AuthoredSetobject");
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
            das::addExtern<DAS_BIND_FUN(StructSetSize)>(*this,Library,"ue_struct_set_size",das::SideEffects::modifyExternal,"StructSetSize");
            das::addExtern<DAS_BIND_FUN(StructSetElementInt)>(*this,Library,"ue_struct_set_element_int",das::SideEffects::modifyExternal,"StructSetElementInt");
            das::addExtern<DAS_BIND_FUN(StructSetElementString)>(*this,Library,"ue_struct_set_element_string",das::SideEffects::modifyExternal,"StructSetElementString");
            das::addExtern<DAS_BIND_FUN(StructSetContainsInt)>(*this,Library,"ue_struct_set_contains_int",das::SideEffects::modifyExternal,"StructSetContainsInt");
            das::addExtern<DAS_BIND_FUN(StructSetContainsString)>(*this,Library,"ue_struct_set_contains_string",das::SideEffects::modifyExternal,"StructSetContainsString");
            das::addExtern<DAS_BIND_FUN(StructSetWithInt),das::SimNode_ExtFuncCallAndCopyOrMove>(*this,Library,"ue_struct_set_with_int",das::SideEffects::modifyExternal,"StructSetWithInt")->needCallerStackFrame=true;
            das::addExtern<DAS_BIND_FUN(StructSetWithString),das::SimNode_ExtFuncCallAndCopyOrMove>(*this,Library,"ue_struct_set_with_string",das::SideEffects::modifyExternal,"StructSetWithString")->needCallerStackFrame=true;
            das::addExtern<DAS_BIND_FUN(StructMapGetIntFloat)>(*this,Library,"ue_struct_map_get_int_float",das::SideEffects::modifyExternal,"StructMapGetIntFloat");
            das::addExtern<DAS_BIND_FUN(StructMapWithIntFloat),das::SimNode_ExtFuncCallAndCopyOrMove>(*this,Library,"ue_struct_map_with_int_float",das::SideEffects::modifyExternal,"StructMapWithIntFloat")->needCallerStackFrame=true;
            das::addExtern<DAS_BIND_FUN(StructMapGetIntDouble)>(*this,Library,"ue_struct_map_get_int_double",das::SideEffects::modifyExternal,"StructMapGetIntDouble");
            das::addExtern<DAS_BIND_FUN(StructMapWithIntDouble),das::SimNode_ExtFuncCallAndCopyOrMove>(*this,Library,"ue_struct_map_with_int_double",das::SideEffects::modifyExternal,"StructMapWithIntDouble")->needCallerStackFrame=true;
            das::addExtern<DAS_BIND_FUN(StructMapGetIntBool)>(*this,Library,"ue_struct_map_get_int_bool",das::SideEffects::modifyExternal,"StructMapGetIntBool");
            das::addExtern<DAS_BIND_FUN(StructMapWithIntBool),das::SimNode_ExtFuncCallAndCopyOrMove>(*this,Library,"ue_struct_map_with_int_bool",das::SideEffects::modifyExternal,"StructMapWithIntBool")->needCallerStackFrame=true;
            das::addExtern<DAS_BIND_FUN(StructMapGetIntName)>(*this,Library,"ue_struct_map_get_int_name",das::SideEffects::modifyExternal,"StructMapGetIntName");
            das::addExtern<DAS_BIND_FUN(StructMapWithIntName),das::SimNode_ExtFuncCallAndCopyOrMove>(*this,Library,"ue_struct_map_with_int_name",das::SideEffects::modifyExternal,"StructMapWithIntName")->needCallerStackFrame=true;
            das::addExtern<DAS_BIND_FUN(StructMapGetStringFloat)>(*this,Library,"ue_struct_map_get_string_float",das::SideEffects::modifyExternal,"StructMapGetStringFloat");
            das::addExtern<DAS_BIND_FUN(StructMapWithStringFloat),das::SimNode_ExtFuncCallAndCopyOrMove>(*this,Library,"ue_struct_map_with_string_float",das::SideEffects::modifyExternal,"StructMapWithStringFloat")->needCallerStackFrame=true;
            das::addExtern<DAS_BIND_FUN(StructMapGetStringDouble)>(*this,Library,"ue_struct_map_get_string_double",das::SideEffects::modifyExternal,"StructMapGetStringDouble");
            das::addExtern<DAS_BIND_FUN(StructMapWithStringDouble),das::SimNode_ExtFuncCallAndCopyOrMove>(*this,Library,"ue_struct_map_with_string_double",das::SideEffects::modifyExternal,"StructMapWithStringDouble")->needCallerStackFrame=true;
            das::addExtern<DAS_BIND_FUN(StructMapGetStringBool)>(*this,Library,"ue_struct_map_get_string_bool",das::SideEffects::modifyExternal,"StructMapGetStringBool");
            das::addExtern<DAS_BIND_FUN(StructMapWithStringBool),das::SimNode_ExtFuncCallAndCopyOrMove>(*this,Library,"ue_struct_map_with_string_bool",das::SideEffects::modifyExternal,"StructMapWithStringBool")->needCallerStackFrame=true;
            das::addExtern<DAS_BIND_FUN(StructMapGetStringName)>(*this,Library,"ue_struct_map_get_string_name",das::SideEffects::modifyExternal,"StructMapGetStringName");
            das::addExtern<DAS_BIND_FUN(StructMapWithStringName),das::SimNode_ExtFuncCallAndCopyOrMove>(*this,Library,"ue_struct_map_with_string_name",das::SideEffects::modifyExternal,"StructMapWithStringName")->needCallerStackFrame=true;
            das::addExtern<DAS_BIND_FUN(PublishFrameInt)>(*this,Library,"ue_frame_out_int",das::SideEffects::modifyExternal,"PublishFrameInt");
            das::addExtern<DAS_BIND_FUN(PublishFrameFloat)>(*this,Library,"ue_frame_out_float",das::SideEffects::modifyExternal,"PublishFrameFloat");
            das::addExtern<DAS_BIND_FUN(PublishFrameBool)>(*this,Library,"ue_frame_out_bool",das::SideEffects::modifyExternal,"PublishFrameBool");
            das::addExtern<DAS_BIND_FUN(PublishFrameString)>(*this,Library,"ue_frame_out_string",das::SideEffects::modifyExternal,"PublishFrameString");
            das::addExtern<DAS_BIND_FUN(PublishFrameDouble)>(*this,Library,"ue_frame_out_double",das::SideEffects::modifyExternal,"PublishFrameDouble");
            das::addExtern<DAS_BIND_FUN(PublishFrameName)>(*this,Library,"ue_frame_out_name",das::SideEffects::modifyExternal,"PublishFrameName");
            das::addExtern<DAS_BIND_FUN(PublishFrameObject)>(*this,Library,"ue_frame_out_object",das::SideEffects::modifyExternal,"PublishFrameObject");
            das::addExtern<DAS_BIND_FUN(PublishFrameEnum)>(*this,Library,"ue_frame_out_enum",das::SideEffects::modifyExternal,"PublishFrameEnum");
            das::addExtern<DAS_BIND_FUN(PublishFrameVector)>(*this,Library,"ue_frame_out_vector",das::SideEffects::modifyExternal,"PublishFrameVector");
            das::addExtern<DAS_BIND_FUN(PublishFrameRotator)>(*this,Library,"ue_frame_out_rotator",das::SideEffects::modifyExternal,"PublishFrameRotator");
            das::addExtern<DAS_BIND_FUN(PublishFrameTransform)>(*this,Library,"ue_frame_out_transform",das::SideEffects::modifyExternal,"PublishFrameTransform");
            das::addExtern<DAS_BIND_FUN(PublishFrameStruct)>(*this,Library,"ue_frame_out_struct",das::SideEffects::modifyExternal,"PublishFrameStruct");
            das::addExtern<DAS_BIND_FUN(CastInterface),das::SimNode_ExtFuncCallAndCopyOrMove>(*this,Library,"ue_cast_interface",das::SideEffects::modifyExternal,"CastInterface");
            das::addExtern<DAS_BIND_FUN(ObjectImplementsInterface)>(*this,Library,"ue_implements_interface",das::SideEffects::modifyExternal,"ObjectImplementsInterface");
            das::addExtern<DAS_BIND_FUN(InterfaceCallInt)>(*this,Library,"ue_interface_call_int",das::SideEffects::modifyExternal,"InterfaceCallInt");
            das::addExtern<DAS_BIND_FUN(InterfaceCallString)>(*this,Library,"ue_interface_call_string",das::SideEffects::modifyExternal,"InterfaceCallString");
            das::addExtern<DAS_BIND_FUN(StructMapSize)>(*this,Library,"ue_struct_map_size",das::SideEffects::modifyExternal,"StructMapSize");
            das::addExtern<DAS_BIND_FUN(StructMapKey)>(*this,Library,"ue_struct_map_key",das::SideEffects::modifyExternal,"StructMapKey");
            das::addExtern<DAS_BIND_FUN(StructMapGetInt)>(*this,Library,"ue_struct_map_get_int",das::SideEffects::modifyExternal,"StructMapGetInt");
            das::addExtern<DAS_BIND_FUN(StructMapWithInt),das::SimNode_ExtFuncCallAndCopyOrMove>(*this,Library,"ue_struct_map_with_int",das::SideEffects::modifyExternal,"StructMapWithInt")->needCallerStackFrame=true;
            das::addExtern<DAS_BIND_FUN(StructMapStringKey)>(*this,Library,"ue_struct_map_key_string",das::SideEffects::modifyExternal,"StructMapStringKey");
            das::addExtern<DAS_BIND_FUN(StructMapGetIntString)>(*this,Library,"ue_struct_map_get_int_string",das::SideEffects::modifyExternal,"StructMapGetIntString");
            das::addExtern<DAS_BIND_FUN(StructMapWithIntString),das::SimNode_ExtFuncCallAndCopyOrMove>(*this,Library,"ue_struct_map_with_int_string",das::SideEffects::modifyExternal,"StructMapWithIntString")->needCallerStackFrame=true;
            das::addExtern<DAS_BIND_FUN(StructMapGetStringInt)>(*this,Library,"ue_struct_map_get_string_int",das::SideEffects::modifyExternal,"StructMapGetStringInt");
            das::addExtern<DAS_BIND_FUN(StructMapWithStringInt),das::SimNode_ExtFuncCallAndCopyOrMove>(*this,Library,"ue_struct_map_with_string_int",das::SideEffects::modifyExternal,"StructMapWithStringInt")->needCallerStackFrame=true;
            das::addExtern<DAS_BIND_FUN(StructMapGetStringString)>(*this,Library,"ue_struct_map_get_string_string",das::SideEffects::modifyExternal,"StructMapGetStringString");
            das::addExtern<DAS_BIND_FUN(StructMapWithStringString),das::SimNode_ExtFuncCallAndCopyOrMove>(*this,Library,"ue_struct_map_with_string_string",das::SideEffects::modifyExternal,"StructMapWithStringString")->needCallerStackFrame=true;
            das::addExtern<DAS_BIND_FUN(StructArraySize)>(*this,Library,"ue_struct_array_size",das::SideEffects::modifyExternal,"StructArraySize");
            das::addExtern<DAS_BIND_FUN(StructArrayResize),das::SimNode_ExtFuncCallAndCopyOrMove>(*this,Library,"ue_struct_array_resize",das::SideEffects::modifyExternal,"StructArrayResize")->needCallerStackFrame=true;
            das::addExtern<DAS_BIND_FUN(StructNestedGet),das::SimNode_ExtFuncCallAndCopyOrMove>(*this,Library,"ue_struct_nested_get",das::SideEffects::modifyExternal,"StructNestedGet")->needCallerStackFrame=true;
            das::addExtern<DAS_BIND_FUN(StructNestedWith),das::SimNode_ExtFuncCallAndCopyOrMove>(*this,Library,"ue_struct_nested_with",das::SideEffects::modifyExternal,"StructNestedWith")->needCallerStackFrame=true;
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
            das::addExtern<DAS_BIND_FUN(StructGetObject),das::SimNode_ExtFuncCallAndCopyOrMove>(*this,Library,"ue_struct_get_object",das::SideEffects::modifyExternal,"StructGetObject");
            das::addExtern<DAS_BIND_FUN(StructWithObject),das::SimNode_ExtFuncCallAndCopyOrMove>(*this,Library,"ue_struct_with_object",das::SideEffects::modifyExternal,"StructWithObject")->needCallerStackFrame=true;
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
                if (auto* Component = Cast<UDasScriptComponent>(Session->Owner.Get())) Component->LastDasError = Error;
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
        ResetDasStructArrayCarriers();
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

bool FDasRuntimeModule::ReadAuthoredInterfaces(const FString& Path,FDasAuthoringModule& Out,FString& Error){
 Error.Reset();if(!IsInGameThread() || !bInitialized){Error=TEXT("Interface declaration compilation requires game-thread runtime");return false;}try{TGuardValue<bool> CollectOnly(CollectTypesOnly,true);das::TextWriter Text;das::ModuleGroup Group;auto Access=das::make_smart<das::FsFileAccess>();Access->addFileSystem(new FUnrealFileSystem(),true,false);const FTCHARToUTF8 File(*Path);auto Program=DSLCompile(File.Get(),Access,Text,Group);if(!Program || Program->failed()){Error=Errors(Program,Text);return false;}FDasAuthoringModule Model;bool OK=true;Program->getThisModule()->structures.foreach([&](const das::StructurePtr& St){auto* A=DSLAnnotation(St->annotations,"ue_interface");if(!A || !OK)return;auto* J=A->arguments.find(DSLInterfaceKey,das::Type::tString);FDasAuthoringModule Part;if(!J || !FDasAuthoringModule::FromJson(DSLText(J->sValue),Part,Error)){OK=false;return;}if(!Model.Id.IsEmpty() && Model.Id!=Part.Id){Error=TEXT("Interface declarations must share module ID");OK=false;return;}Model.Id=Part.Id;Model.Classes.Append(Part.Classes);});if(!OK || !Model.Validate(Error))return false;Out=MoveTemp(Model);return true;}catch(const std::exception& X){Error=UTF8_TO_TCHAR(X.what());return false;}
}
bool FDasRuntimeModule::ReadReflectedTypes(const FString& Path,FDasReflectedTypes& Out,FString& Error){
 Error.Reset();if(!IsInGameThread() || !bInitialized){Error=TEXT("Type declaration compilation requires initialized game-thread runtime");return false;}try{TGuardValue<bool> CollectOnly(CollectTypesOnly,true);das::TextWriter Text;das::ModuleGroup Group;auto Access=das::make_smart<das::FsFileAccess>();Access->addFileSystem(new FUnrealFileSystem(),true,false);const FTCHARToUTF8 File(*Path);auto Program=DSLCompile(File.Get(),Access,Text,Group);if(!Program || Program->failed()){Error=Errors(Program,Text);return false;}FDasReflectedTypes Model;bool OK=true;auto Append=[&](const das::AnnotationList& List,const char* Name){const auto* A=DSLAnnotation(List,Name);if(!A || !OK)return;const auto* J=A->arguments.find(DSLTypesKey,das::Type::tString);FDasReflectedTypes Part;if(!J || !FDasReflectedTypes::FromJson(DSLText(J->sValue),Part,Error,true)){OK=false;return;}if(!Model.Id.IsEmpty() && Model.Id!=Part.Id){Error=TEXT("Reflected declarations must share module ID");OK=false;return;}Model.Id=Part.Id;Model.Enums.Append(Part.Enums);Model.Structs.Append(Part.Structs);};Program->getThisModule()->enumerations.foreach([&](const das::EnumerationPtr& E){Append(E->annotations,"ue_enum");});Program->getThisModule()->structures.foreach([&](const das::StructurePtr& S){Append(S->annotations,"ue_struct");});if(!OK || !Model.Validate(Error))return false;Out=MoveTemp(Model);return true;}catch(const std::exception& X){Error=UTF8_TO_TCHAR(X.what());return false;}
}

bool FDasRuntimeModule::ReadAuthoringScript(const FString& Path,FDasAuthoringModule& Out,FString& Error)
{
    Error.Reset();if(!IsInGameThread() || !bInitialized){Error=TEXT("DSL compilation requires initialized runtime on game thread");return false;}
    try
    {
        das::TextWriter Output;das::ModuleGroup Group;auto Access=das::make_smart<das::FsFileAccess>();Access->addFileSystem(new FUnrealFileSystem(),true,false);
        const FTCHARToUTF8 File(*Path);auto Program=DSLCompile(File.Get(),Access,Output,Group);
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
        auto Program=DSLCompile(Utf8Path.Get(),Access,Output,Modules,Policies);
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
        auto Program = DSLCompile(Utf8Path.Get(), Access, Output, Modules, Policies);
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

bool FDasRuntimeModule::IsAuthoredMethodDispatchActive()const{return IsInGameThread() && AuthoredDispatchDepth>0;}

bool FDasRuntimeModule::CanMigrateAuthoredTypes()const {
#if WITH_EDITOR
 return IsInGameThread() && bInitialized && !ActiveContext && !bDrainingEvents && !bPreservingSessions && PendingEvents.IsEmpty() && PersistentContextCount()==0 && SubscriptionCount()==0;
#else
 return false;
#endif
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
    ON_SCOPE_EXIT{if(AuthoredDispatchFailure && !Result.bSucceeded)*AuthoredDispatchFailure=Result.Error;};
    if(Event==EDasCallback::Main)
    {
        bool Valid=!EntryPoint.IsEmpty() && EntryPoint.Len()<NAME_SIZE && !FChar::IsDigit(EntryPoint[0]);
        for(TCHAR Ch:EntryPoint) Valid &= FChar::IsAlnum(Ch) || Ch==TEXT('_');
        if(!Valid){Result.Error=TEXT("Invalid entry point name");return Result;}
    }
    if (!IsInGameThread() || !bInitialized || (ActiveContext && (AuthoredDispatchDepth==0 || Event!=EDasCallback::Main)))
    {
        Result.Error = TEXT("Persistent calls require an initialized runtime and authorized method dispatch on the game thread.");
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
    TArray<FDasScalarValue> NormalizedFrame,PendingOutputs;
    if(Frame){NormalizedFrame=*Frame;for(auto& V:NormalizedFrame){if(V.Direction<0 || V.Direction>2){Result.Error=TEXT("Invalid frame direction");return Result;}if(V.Direction==1)V=UDasFrameLibrary::DefaultValue(V.Kind,V.ObjectClass,1);}Frame=&NormalizedFrame;}
    FStructFrameRoots FrameRoots(NormalizedFrame);
    FDasCallContract Expected=FDasCallContract::Scalar(Signature);
    if(Frame){Expected.Inputs.Reset();if(Frame->Num()>16 || (static_cast<uint8>(Signature)>13 || Signature==EDasMethodSignature::Frame)){Result.Error=TEXT("Invalid scalar frame");return Result;}for(const auto& V:*Frame){if(V.Kind<0 || V.Kind>13 || V.Kind==3 || V.Kind==4){Result.Error=TEXT("Invalid scalar argument kind");return Result;}if(V.Kind==5)for(int32 I=0;I<V.StringValue.Len();++I)if(V.StringValue[I]==0){Result.Error=TEXT("embedded NUL is not supported in strings");return Result;}if((V.Kind==9 && V.VectorValue.ContainsNaN()) || (V.Kind==10 && V.RotatorValue.ContainsNaN()) || (V.Kind==11 && (V.TransformValue.ContainsNaN() || !V.TransformValue.IsRotationNormalized()))){Result.Error=TEXT("Invalid/non-finite math struct input");return Result;}Expected.Inputs.Add(static_cast<EDasMethodSignature>(V.Kind));}}
    TArray<TStrongObjectPtr<UObject>> ObjectPins;
    if(Frame){bool Directed=false;for(const auto& V:*Frame)Directed |= V.Direction!=0;if(Directed)for(const auto& V:*Frame)Expected.InputDirections.Add(V.Direction);}
    if(Frame){bool Objects=false;for(const auto& V:*Frame)Objects |= V.Kind==8 || V.Kind==12 || V.Kind==13;
        if(Objects)for(const auto& V:*Frame){const FString PathClass=(V.Kind==12 || V.Kind==13)?V.ObjectClass:V.Kind==8?(V.ObjectClass.IsEmpty()?TEXT("/Script/CoreUObject.Object"):V.ObjectClass):FString();Expected.InputClasses.Add(PathClass);if(V.Kind==8){UClass* Class=FindObject<UClass>(nullptr,*PathClass);UObject* Object=V.ObjectValue.Get();if(!Class || (Object && (!IsValid(Object) || !DasReferenceMatches(Object,Class) || (Object->IsA<AActor>() && CastChecked<AActor>(Object)->IsActorBeingDestroyed())))){Result.Error=TEXT("Object argument is expired or has wrong class");return Result;}if(Object)ObjectPins.Emplace(Object);}}
    }
    if(Frame)for(const auto& V:*Frame)if(V.Kind==12){FString E;if(!V.StructValue.IsValid() || V.StructValue.GetScriptStruct()->GetPathName()!=V.ObjectClass || !ValidateDasStructSnapshot(V.StructValue,E)){Result.Error=TEXT("Invalid reflected struct input: ")+E;return Result;}}
    if(Signature==EDasMethodSignature::Enum){Expected.ReturnClass=ReturnClass;FString E;if(!ResolveDasEnum(ReturnClass,E)){Result.Error=E;return Result;}}
    if(Frame)for(const auto& V:*Frame)if(V.Kind==13){FString E;if(!ValidateDasEnumNumber(V.ObjectClass,V.IntValue,E)){Result.Error=E;return Result;}}
    if(Signature==EDasMethodSignature::Struct){DasContainerProperty(ReturnClass);Expected.ReturnClass=ReturnClass;FString E;if(!ValidateDasValueStruct(ReturnClass,E)){Result.Error=E;return Result;}}
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
    if(Session->bReleaseRequested){Result.Error=TEXT("Script session was released");return Result;}
    const bool Reentering=Session->Live && Session->Live->Context->ExecutionDepth>0;
    if(Reentering && bChanged){Result.Error=TEXT("Cannot reload an executing script during reentrant dispatch");return Result;}
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
            {TGuardValue<FSourceSet*> ReferenceDependencies(AuthoredReferenceSources,&Session->AttemptedSources);Candidate->Program = DSLCompile(Utf8Path.Get(), Access, Output, Candidate->Modules);}
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
    if(!Reentering){Context.restart();Context.Types.Reset();}
    TGuardValue<int32> ExecutionScope(Context.ExecutionDepth,Context.ExecutionDepth+1);
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
                else if(V.Kind==5)Packed.push_back(das::cast<char*>::from(CopyDSLString(V.StringValue,&Context)));else if(V.Kind==6)Packed.push_back(das::cast<double>::from(V.DoubleValue));else if(V.Kind==13)Packed.push_back(das::cast<FDasEnumValue>::from(MakeEnumValue(V.ObjectClass,V.IntValue,&Context)));else if(V.Kind==7)Packed.push_back(das::cast<FDasNameValue>::from(MakeDSLName(V.NameValue,&Context)));else if(V.Kind==9){VectorArgs.Add(V.VectorValue);Packed.push_back(das::cast<void*>::from(&VectorArgs.Last()));}else if(V.Kind==10){RotatorArgs.Add(V.RotatorValue);Packed.push_back(das::cast<void*>::from(&RotatorArgs.Last()));}else if(V.Kind==12){StructArgs.Add(StoreStructValue(V.StructValue,&Context));Packed.push_back(das::cast<void*>::from(&StructArgs.Last()));}else if(V.Kind==11){TransformArgs.Add(V.TransformValue);Packed.push_back(das::cast<void*>::from(&TransformArgs.Last()));}else{const FString ClassPath=V.ObjectClass.IsEmpty()?TEXT("/Script/CoreUObject.Object"):V.ObjectClass;ObjectArgs.Add(MakeObjectHandle(V.ObjectValue.Get(),FindObject<UClass>(nullptr,*ClassPath),&Context));Packed.push_back(das::cast<void*>::from(&ObjectArgs.Last()));}
            }
            FOutputFrame OutFrame;OutFrame.Arguments=Frame;TGuardValue<FOutputFrame*> OutScope(ActiveOutputFrame,&OutFrame);
            const auto Value = Context.evalWithCatch(Function,Frame?(Packed.empty()?nullptr:Packed.data()):Event==EDasCallback::Main && Signature==EDasMethodSignature::Void?nullptr:Arguments,Signature==EDasMethodSignature::Struct?static_cast<void*>(&ReturnedStructToken):Signature==EDasMethodSignature::Object?static_cast<void*>(&ReturnedObject):Signature==EDasMethodSignature::Vector?static_cast<void*>(&ReturnedVector):Signature==EDasMethodSignature::Rotator?static_cast<void*>(&ReturnedRotator):Signature==EDasMethodSignature::Transform?static_cast<void*>(&ReturnedTransform):nullptr);
            if(Event==EDasCallback::Main){
                if(Signature==EDasMethodSignature::Enum && !Context.getException()){const auto& E=ResolveEnumValue(das::cast<FDasEnumValue>::to(Value),&Context);if(E.Type->GetPathName()!=Expected.ReturnClass){Result.Error=TEXT("Enum result type mismatch");return Result;}Result.Value=E.Value;}else if(Signature==EDasMethodSignature::Struct && !Context.getException()){const auto& S=ResolveStructValue(ReturnedStructToken,&Context);if(S.GetScriptStruct()->GetPathName()!=Expected.ReturnClass){Result.Error=TEXT("Reflected struct result type mismatch");return Result;}Result.StructValue=S;}else if(Signature==EDasMethodSignature::Vector)Result.VectorValue=ReturnedVector;else if(Signature==EDasMethodSignature::Rotator)Result.RotatorValue=ReturnedRotator;else if(Signature==EDasMethodSignature::Transform)Result.TransformValue=ReturnedTransform;else if(Signature==EDasMethodSignature::Int) Result.Value=das::cast<int32>::to(Value);
                else if(Signature==EDasMethodSignature::Float) Result.FloatValue=das::cast<float>::to(Value);
                else if(Signature==EDasMethodSignature::Bool) Result.BoolValue=das::cast<bool>::to(Value);
                else if(!Context.getException() && Signature==EDasMethodSignature::String){const auto* Text=das::cast<char*>::to(Value);Result.StringValue=Text?UTF8_TO_TCHAR(Text):TEXT("");}
                else if(Signature==EDasMethodSignature::Double)Result.DoubleValue=das::cast<double>::to(Value);
                else if(!Context.getException() && Signature==EDasMethodSignature::Name)Result.NameValue=ResolveDSLName(das::cast<FDasNameValue>::to(Value),&Context);
                else if(!Context.getException() && Signature==EDasMethodSignature::Object){UObject* Object=ReturnedObject.Token?TryObject(ReturnedObject,&Context):nullptr;if(ReturnedObject.Token && !Object){Result.Error=TEXT("Object result is expired or belongs to another context");return Result;}UClass* Class=FindObject<UClass>(nullptr,*Expected.ReturnClass);if(Object && !DasReferenceMatches(Object,Class)){Result.Error=TEXT("Object result has wrong class");return Result;}Result.ObjectValue=Object;}
            }
            if(Frame && !Context.getException()){TArray<FDasScalarValue> Published=*Frame;for(int32 I=0;I<Frame->Num();++I)if((*Frame)[I].Direction){auto* Out=OutFrame.Values.Find(I);if(!Out){Result.Error=TEXT("Declared output slot not published");return Result;}Published[I]=*Out;}if(!Expected.InputDirections.IsEmpty())PendingOutputs=MoveTemp(Published);}
        }
        if((Signature==EDasMethodSignature::Vector && Result.VectorValue.ContainsNaN()) || (Signature==EDasMethodSignature::Rotator && Result.RotatorValue.ContainsNaN()) || (Signature==EDasMethodSignature::Transform && (Result.TransformValue.ContainsNaN() || !Result.TransformValue.IsRotationNormalized()))){Result.Error=TEXT("Invalid/non-finite math struct result");return Result;}
        if (const char* Exception = Context.getException()) Result.Error = UTF8_TO_TCHAR(Exception);
        else { Result.Types = Context.Types; Result.bSucceeded = true;Result.OutputValues=MoveTemp(PendingOutputs); }
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
