#include "DasAccessContextCommandlet.h"
#include "DasRuntime.h"
#include "DasDSLService.h"
#include "DasNativeHashProbeComponent.h"
#include "DasFunctionLibraryFixture.h"
#include "DasPropertyAccess.h"
#include "Engine/Blueprint.h"
#include "Engine/World.h"
#include "UObject/StrongObjectPtr.h"
#include "UObject/UnrealType.h"
#include "UObject/GarbageCollection.h"
#include "Misc/ScopeExit.h"
#include <limits>

UDasAccessContextCommandlet::UDasAccessContextCommandlet(){IsClient=false;IsServer=false;IsEditor=true;LogToConsole=true;}
int32 UDasAccessContextCommandlet::Main(const FString& Params){
    int32 Failures=0;
    auto Check=[&](bool Good,const TCHAR* Name){UE_LOG(LogTemp,Display,TEXT("DAS_ACCESS_CONTEXT_%s %s"),Good?TEXT("PASS"):TEXT("FAIL"),Name);if(!Good)++Failures;return Good;};
    auto& Runtime=FModuleManager::LoadModuleChecked<FDasRuntimeModule>(TEXT("DasRuntime"));
    FString Error;
    UBlueprint* BP=nullptr;
    const FString Package=TEXT("/Game/DasAccessContext/")+FGuid::NewGuid().ToString(EGuidFormats::Digits)+TEXT("/BP_Probe");
    if(!Check(FDasDSLService::Apply(TEXT("dsl_access_context.das"),Package,TEXT("accesscontext.AccessContext"),BP,Error),TEXT("transient_import"))){UE_LOG(LogTemp,Error,TEXT("%s"),*Error);return 1;}
    for(auto WorldType:{EWorldType::Game,EWorldType::Editor}){
        TStrongObjectPtr<UWorld> World(UWorld::CreateWorld(WorldType,false));
        if(WorldType==EWorldType::Game)World->InitializeActorsForPlay(FURL());
        auto* Actor=World->SpawnActor<AActor>(BP->GeneratedClass);
        auto* Probe=NewObject<UDasNativeHashProbeComponent>(Actor);
        Actor->AddInstanceComponent(Probe);Probe->RegisterComponent();
        ON_SCOPE_EXIT{Runtime.ReleaseObject(Probe);Runtime.ReleaseObject(Actor);World->DestroyWorld(false);};
        FindFProperty<FObjectProperty>(Actor->GetClass(),TEXT("Probe"))->SetObjectPropertyValue_InContainer(Actor,Probe);
        if(WorldType==EWorldType::Game){
            const auto Run=Runtime.RunFile(Runtime.ScriptPath(TEXT("dsl_access_context.das")),0,false,Probe);
            if(!Run.bSucceeded)UE_LOG(LogTemp,Error,TEXT("Gameplay accessor probe: %s"),*Run.Error);
            Check(Run.bSucceeded && Run.Value==42 && Probe->SemanticScore==16 && Probe->UnsafeCalls==1,TEXT("DSL_getter_setter_scalar_bool_array_math_and_gameplay_unsafe"));
            auto* P=FindFProperty<FIntProperty>(Probe->GetClass(),TEXT("SemanticScore"));FDasPropertyAccess Access;
            Check(DescribeDasPropertyAccess(P,Access,Error) && Access.NativeGetter && Access.NativeSetter,TEXT("native_accessor_contract"));
            const int32 Before=Probe->SetterCalls;FDasScalarValue Bad;Bad.Kind=int32(EDasMethodSignature::String);Bad.StringValue=TEXT("wrong");
            Check(!WriteDasObjectProperty(P,Probe,Access,false,Bad,Error) && Probe->SetterCalls==Before && Probe->SemanticScore==16,TEXT("invalid_input_before_setter"));
            auto Stale=Access;Stale.NativeGetter=false;FDasScalarValue Out;
            Check(!ReadDasObjectProperty(P,Probe,Stale,false,Out,Error),TEXT("stale_accessor_refused"));
            auto* Readonly=FindFProperty<FIntProperty>(Probe->GetClass(),TEXT("ReadonlyScore"));DescribeDasPropertyAccess(Readonly,Access,Error);FDasScalarValue Number;Number.Kind=int32(EDasMethodSignature::Int);Number.IntValue=20;
            Check(!WriteDasObjectProperty(Readonly,Probe,Access,false,Number,Error) && Probe->ReadonlyScore==7 && Probe->SetterCalls==Before,TEXT("readonly_even_with_native_setter"));
            auto* Level=FindFProperty<FFloatProperty>(Probe->GetClass(),TEXT("BlueprintLevel"));DescribeDasPropertyAccess(Level,Access,Error);FDasScalarValue Infinite;Infinite.Kind=int32(EDasMethodSignature::Float);Infinite.FloatValue=std::numeric_limits<float>::infinity();const int32 LevelBefore=Probe->BlueprintSetterCalls;
            Check(!WriteDasObjectProperty(Level,Probe,Access,false,Infinite,Error) && Probe->BlueprintSetterCalls==LevelBefore,TEXT("nonfinite_input_before_setter"));
            auto* Reference=FindFProperty<FObjectProperty>(Probe->GetClass(),TEXT("SemanticObject"));DescribeDasPropertyAccess(Reference,Access,Error);
            auto* Target=NewObject<UDasNativeHashProbeComponent>();TWeakObjectPtr<UObject> Weak=Target;FDasScalarValue Object;Object.Kind=int32(EDasMethodSignature::Object);Object.ObjectClass=TEXT("/Script/CoreUObject.Object");Object.ObjectValue=Target;Probe->CollectOnSet=true;
            Check(WriteDasObjectProperty(Reference,Probe,Access,false,Object,Error) && Weak.IsValid() && Probe->SemanticObject==Weak.Get(),TEXT("object_pinned_across_setter_GC"));Probe->CollectOnSet=false;
        }
        auto* Mode=FindFProperty<FIntProperty>(Actor->GetClass(),TEXT("Mode"));Mode->SetPropertyValue_InContainer(Actor,0);
        const int32 SafeBefore=Probe->SafeCalls;
        auto Safe=Runtime.RunConstruction(Runtime.ScriptPath(TEXT("dsl_access_context.das")),Actor);
        if(!Safe.bSucceeded)UE_LOG(LogTemp,Error,TEXT("Construction accessor probe: %s"),*Safe.Error);
        Check(Safe.bSucceeded && Probe->SafeCalls==SafeBefore+1 && Probe->SemanticScore==10 && FindFProperty<FIntProperty>(Actor->GetClass(),TEXT("Result"))->GetPropertyValue_InContainer(Actor)==11,TEXT("construction_safe_method_and_native_accessor"));
        // Remove metadata after generating the contracts: guards must also work when cooked metadata is absent.
        UFunction* Unsafe=Probe->FindFunction(TEXT("UnsafeContextBump"));
        UFunction* Setter=Probe->FindFunction(TEXT("WriteBlueprintLevel"));
        UFunction* Static=UDasFunctionLibraryFixture::StaticClass()->FindFunctionByName(TEXT("UnsafeContextProbe"));
        const FName Key(TEXT("UnsafeDuringActorConstruction"));
        for(auto* Function:{Unsafe,Setter,Static})Function->RemoveMetaData(Key);
        ON_SCOPE_EXIT{for(auto* Function:{Unsafe,Setter,Static})Function->SetMetaData(Key,TEXT("true"));};
        for(int32 TestMode=1;TestMode<=4;++TestMode){
            Mode->SetPropertyValue_InContainer(Actor,TestMode);
            const int32 UnsafeBefore=Probe->UnsafeCalls,SetterBefore=Probe->BlueprintSetterCalls,StaticBefore=UDasFunctionLibraryFixture::ContextMutations;
            const auto Result=Runtime.RunConstruction(Runtime.ScriptPath(TEXT("dsl_access_context.das")),Actor);
            Check(!Result.bSucceeded && Result.Error.Contains(TEXT("unsafe during ConstructionScript")) && Probe->UnsafeCalls==UnsafeBefore && Probe->BlueprintSetterCalls==SetterBefore && UDasFunctionLibraryFixture::ContextMutations==StaticBefore,TEXT("cooked_contract_guard_before_mutation"));
            if(Result.bSucceeded || !Result.Error.Contains(TEXT("unsafe during ConstructionScript")))UE_LOG(LogTemp,Error,TEXT("Unexpected mode %d: %s"),TestMode,*Result.Error);
        }
    }
    UE_LOG(LogTemp,Display,TEXT("DAS_ACCESS_CONTEXT_RESULT=%s"),Failures?TEXT("FAIL"):TEXT("PASS"));return Failures?1:0;
}
