#pragma once
#include "CoreMinimal.h"
class UPrimitiveComponent;
// Fixed value-only mailbox. No pointer, UObject, string, container or VM memory
// crosses this interface. Latest input wins; this is not a reliable event queue.
struct DASRUNTIME_API FDasPhysicsPacket {
 static constexpr int32 Capacity=16;
 double Values[Capacity]={};
};
struct DASRUNTIME_API FDasPhysicsOutput {
 FDasPhysicsPacket Packet;
 uint64 InputSequence=0,StepSequence=0;
};
class DASRUNTIME_API IDasAsyncPhysicsVM {
public:
 virtual ~IDasAsyncPhysicsVM()=default;
 // Ordinary game-thread only. A failed publication leaves the mailbox intact.
 virtual bool Publish(const FDasPhysicsPacket& Input,FString& Error)=0;
 // False before first success, after revoke, or in any physics callback.
 // Out is unchanged on failure. Native handles do not retain the owner or VM.
 virtual bool Read(FDasPhysicsOutput& Out)const=0;
 virtual bool IsLive()const=0;
 virtual FString LastError()const=0;
 virtual void Revoke()=0;
 virtual uint64 BindingRevision()const=0;
 // Explicit native authorization; only same-owner single unwelded components.
 // Failed binding preserves the previous slot. No UObject reference enters DAS.
 virtual bool BindBody(int32 Slot,UPrimitiveComponent* Body,FString& Error)=0;
 virtual bool ClearBody(int32 Slot,FString& Error)=0;
};
// Compiles, initializes and resolves [export] physics_tick(dt:float,time:float)
// before enabling the engine callback. Creates a fresh, isolated interpreter.
// Prepare replacement first; revoke the old handle only after successful create.
DASRUNTIME_API TSharedPtr<IDasAsyncPhysicsVM> CreateDasAsyncPhysicsVM(
 const FString& Path,UObject* Owner,FString& Error);

// Prepared interpreter owns no registered engine callback. Start is one-shot;
// a prepared VM may be compiled before owner BeginPlay.
class DASRUNTIME_API IDasPreparedAsyncPhysicsVM {
public:
 virtual ~IDasPreparedAsyncPhysicsVM()=default;
 virtual TSharedPtr<IDasAsyncPhysicsVM> Start(FString& Error)=0;
};
DASRUNTIME_API TSharedPtr<IDasPreparedAsyncPhysicsVM> PrepareDasAsyncPhysicsVM(const FString& Path,UObject* Owner,FString& Error);
