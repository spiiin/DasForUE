#pragma once
#include "DasRuntime.h"
#include "DasProbeActor.h"
#include "Engine/World.h"
#include "Engine/EngineBaseTypes.h"
#include "UObject/StrongObjectPtr.h"
#include "UObject/UnrealType.h"
#include "DasSchema.h"
inline bool RunDasMethodValuesProbe(UClass* DriverClass,UClass* TargetClass,FString& Report){
 TStrongObjectPtr<UWorld> World(UWorld::CreateWorld(EWorldType::Game,false));World->InitializeActorsForPlay(FURL());auto* Driver=World->SpawnActor<ADasProbeActor>(DriverClass);auto* Target=World->SpawnActor<ADasProbeActor>(TargetClass);if(!Driver || !Target)return false;
 struct FArguments{UObject* Target;int32 Result=0;};FArguments Args{Target};auto* Function=Driver->FindFunction(TEXT("Exercise"));if(Function)Driver->ProcessEvent(Function,&Args);bool Pass=Function && Args.Result==1 && Driver->LastDasError.IsEmpty();
 if(Pass){auto* Partner=FindFProperty<FObjectPropertyBase>(TargetClass,TEXT("Partner"));auto* Reject=Driver->FindFunction(TEXT("RejectNativeAuthoredField"));if(!Partner || !Reject)Pass=false;else {Partner->SetObjectPropertyValue_InContainer(Target,Driver);Args.Result=0;Driver->ProcessEvent(Reject,&Args);Pass=Args.Result==1 && Driver->LastDasError.IsEmpty() && !Partner->GetObjectPropertyValue_InContainer(Target);}Report+=FString::Printf(TEXT("%s native_assigned_wrong_authored_field_rejected_and_cleared\n"),Pass?TEXT("PASS"):TEXT("FAIL"));}
 if(Pass){auto* Ephemeral=World->SpawnActor<AActor>();auto* Property=FindFProperty<FObjectPropertyBase>(TargetClass,TEXT("NativeActor"));auto* Expired=Driver->FindFunction(TEXT("ExpiredNativeFields"));if(!Ephemeral || !Property || !Expired)Pass=false;else {Property->SetObjectPropertyValue_InContainer(Target,Ephemeral);auto* Settings=FindFProperty<FStructProperty>(TargetClass,TEXT("Settings"));auto* Point=Settings?CastField<FStructProperty>(FindDasStructField(Settings->Struct,TEXT("Point"))):nullptr;auto* Owner=Point?CastField<FObjectPropertyBase>(FindDasStructField(Point->Struct,TEXT("Owner"))):nullptr;if(!Owner)return false;Owner->SetObjectPropertyValue_InContainer(Point->ContainerPtrToValuePtr<void>(Settings->ContainerPtrToValuePtr<void>(Target)),Ephemeral);Ephemeral->Destroy();Args.Result=0;Driver->ProcessEvent(Expired,&Args);Pass=Args.Result==1 && Driver->LastDasError.IsEmpty();}Report+=FString::Printf(TEXT("%s destroyed_native_reference_property_is_null\n"),Pass?TEXT("PASS"):TEXT("FAIL"));}Report+=FString::Printf(TEXT("%s natural_enum_nested_struct_result_ref_out_free_function_and_recovery\n"),Pass?TEXT("PASS"):TEXT("FAIL"));if(!Pass)Report+=FString::Printf(TEXT("result=%d error=%s\n"),Args.Result,*Driver->LastDasError);
 World->DestroyWorld(false);Report+=FString::Printf(TEXT("DAS_METHOD_VALUES_RESULT=%s\n"),Pass?TEXT("PASS"):TEXT("FAIL"));return Pass;
}
