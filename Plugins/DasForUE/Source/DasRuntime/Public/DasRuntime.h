#pragma once

#include "CoreMinimal.h"
#include "DasSchema.h"
#include "DasCallFrame.h"
#include "Modules/ModuleInterface.h"
#include "UObject/WeakObjectPtr.h"
#include "UObject/WeakObjectPtrTemplates.h"

struct FDasAuthoringModule;
struct FDasSourceCatalog;

struct FDasTypeDeclaration
{
    FString Name;
    FString Parent;
    FString Fields;
};

struct FDasExecutionResult
{
    bool bSucceeded = false;
    bool bUsedAot = false;
    int32 Value = 0;
    float FloatValue = 0;
    bool BoolValue = false;
    FString StringValue;double DoubleValue=0;FName NameValue;TWeakObjectPtr<UObject> ObjectValue;
    FInstancedStruct StructValue;
    FScriptDelegate DelegateValue;
    TArray<FDasScalarValue> OutputValues;
    FVector VectorValue=FVector::ZeroVector;FRotator RotatorValue=FRotator::ZeroRotator;FTransform TransformValue=FTransform::Identity;
    FString Error;
    FString ReloadError; // Candidate rejected; a successful result may still use the previous version.
    uint32 Revision = 0;
    TArray<FDasTypeDeclaration> Types;
};

enum class EDasCallback : uint8 { Main, BeginPlay, Tick, EndPlay, Initialize, Construction };

struct FDasReflectedTypes;
class DASRUNTIME_API FDasRuntimeModule final : public IModuleInterface
{
public:
#if WITH_EDITOR
    bool BeginDefaultUpdate();
    void EndDefaultUpdate(UClass* RootClass,bool Applied);
    bool BeginPreserveContextsForReinstance(UClass* RootClass);
    void EndPreserveContextsForReinstance(const FString& ApprovedDeclaration=FString());
#endif

    virtual void StartupModule() override;
    virtual void ShutdownModule() override;
    virtual bool SupportsDynamicReloading() override { return false; }
    // Compile-only annotation collection; no global/body execution, Out unchanged on failure.
    bool ReadAuthoredInterfaces(const FString& Path,FDasAuthoringModule& Out,FString& Error);
    bool ReadReflectedTypes(const FString& Path,FDasReflectedTypes& Out,FString& Error,FString* SourceModule=nullptr);
    bool ReadAuthoringScript(const FString& Path,FDasAuthoringModule& Out,FString& Error,FDasSourceCatalog* ImportedValues=nullptr);
    bool ReadSourceCatalogImports(const FString& Path,FDasSourceCatalog& Out,FString& Error);
    bool ValidateFrameMethod(const FString& Path,FString& Error,const FString& EntryPoint,const FDasCallContract& Contract);
    FDasExecutionResult RunLibraryFrame(const FString& Contract,const FString& Script,const FString& Entry,const TArray<FDasScalarValue>& Arguments,EDasMethodSignature ReturnKind,const FString& ReturnClass);
    FDasExecutionResult RunNamedFramePersistent(const FString& Path,const FString& EntryPoint,const TArray<FDasScalarValue>& Arguments,EDasMethodSignature ReturnKind,TWeakObjectPtr<UObject> Self,const FString& ReturnClass = FString());
    bool ValidateScalarMethod(const FString& Path,FString& Error,const FString& EntryPoint,EDasMethodSignature Signature);
    bool ValidateIntMethod(const FString& Path, FString& Error, const FString& EntryPoint = TEXT("main"));
    FDasExecutionResult RunFile(const FString& Path, int32 Input, bool bUseAot = false, TWeakObjectPtr<UObject> Self = {});
    FDasExecutionResult RunNamedFloatPersistent(const FString& Path,const FString& EntryPoint,float Input,TWeakObjectPtr<UObject> Self);
    FDasExecutionResult RunNamedBoolPersistent(const FString& Path,const FString& EntryPoint,bool Input,TWeakObjectPtr<UObject> Self);
    FDasExecutionResult RunNamedVoidPersistent(const FString& Path,const FString& EntryPoint,TWeakObjectPtr<UObject> Self);
    FDasExecutionResult RunNamedPersistent(const FString& Path, const FString& EntryPoint, int32 Input, TWeakObjectPtr<UObject> Self);
    FDasExecutionResult RunPersistent(const FString& Path, int32 Input, TWeakObjectPtr<UObject> Self);
    FDasExecutionResult RunLifecycle(const FString& Path, EDasCallback Event, TWeakObjectPtr<UObject> Self, float DeltaSeconds = 0.f, int32 EndReason = 0);
    FDasExecutionResult RunConstruction(const FString& Path, TWeakObjectPtr<UObject> Self);
    // Internal reflected delegate bridge: IDs never contain a VM/function pointer.
    void DispatchIntEvent(uint64 SubscriptionId, int32 Value);
    void DispatchScalarEvent(uint64 SubscriptionId,const FDasScalarValue& Value);
    int32 SubscriptionCount(const UObject* Owner = nullptr) const;
    void ReleaseObject(UObject* Object);
    int32 StructSnapshotCount(const UObject* Owner=nullptr) const;
    uint64 StructSnapshotsReclaimed(const UObject* Owner=nullptr) const;
    // Nested authored calls report failures to the caller VM, which may recover them.
    bool IsAuthoredMethodDispatchActive()const;
    bool CanMigrateAuthoredTypes()const;
    int32 PersistentContextCount(const UWorld* World = nullptr) const;
    bool ExportBindings(const FString& SelectionPath, const FString& OutputPath, FString& Error);
    FString ScriptPath(const FString& Name) const;
private:
    FDasExecutionResult RunCallback(const FString& Path, EDasCallback Event, int32 Input, float DeltaSeconds, TWeakObjectPtr<UObject> Self, const FString& EntryPoint = TEXT("main"), EDasMethodSignature Signature = EDasMethodSignature::Int,const TArray<FDasScalarValue>* Frame = nullptr,const FString& ReturnClass = FString());
    FDelegateHandle InputRestoreTickHandle;
    FDelegateHandle WorldCleanupHandle;
    FDelegateHandle PostGCHandle;
    FDelegateHandle ObjectsReplacedHandle;
    FString Root;
    bool bInitialized = false;
};

// Shared by the editor commandlet and the packaged smoke game.
DASRUNTIME_API bool RunDasFoundationTests(FString& Report);
