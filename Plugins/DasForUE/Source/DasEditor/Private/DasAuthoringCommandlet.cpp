#include "DasAuthoringCommandlet.h"
#include "DasAuthoring.h"
#include "DasRuntime.h"
#include "Modules/ModuleManager.h"
#include "DasBlueprintService.h"
#include "DasDSLService.h"
#include "DasProbeActor.h"
#include "Editor.h"
#include "Engine/Blueprint.h"
#include "Engine/BlueprintGeneratedClass.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "Kismet2/KismetEditorUtilities.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "Misc/Parse.h"
#include "Misc/PackageName.h"
#include "HAL/FileManager.h"
#include "UObject/MetaData.h"
#include "UObject/SavePackage.h"
#include "UObject/StructOnScope.h"
#include "UObject/StrongObjectPtr.h"
#include "UObject/UnrealType.h"

namespace
{
    constexpr const TCHAR* AuthoringProbeClassId = TEXT("example.counter");
    bool LoadModel(const FString& File, FDasAuthoringModule& Model, FString& Error)
    { if(FPaths::GetExtension(File)==TEXT("das")) return FModuleManager::LoadModuleChecked<FDasRuntimeModule>(TEXT("DasRuntime")).ReadAuthoringScript(File,Model,Error); FString Json; return FFileHelper::LoadFileToString(Json,*File) ? FDasAuthoringModule::FromJson(Json,Model,Error) : (Error=TEXT("Cannot read authoring model"),false); }
    int32 Execute(UObject* Object, FName Name=TEXT("Evaluate"))
    {
        UFunction* Method=Object->FindFunction(Name); if (!Method) return MIN_int32;
        FStructOnScope Frame(Method);auto* Input=FindFProperty<FIntProperty>(Method,TEXT("Input"));auto* Result=CastField<FIntProperty>(Method->GetReturnProperty());
        if (!Input || !Result) return MIN_int32;
        Input->SetPropertyValue_InContainer(Frame.GetStructMemory(),5);
        FEditorScriptExecutionGuard Guard;Object->ProcessEvent(Method,Frame.GetStructMemory());
        return Result->GetPropertyValue_InContainer(Frame.GetStructMemory());
    }
    ADasProbeActor* Find(UWorld* World)
    { for (TActorIterator<ADasProbeActor> It(World);It;++It) if (It->GetFName()==TEXT("AuthoringCounter")) return *It;return nullptr; }
}
UDasAuthoringCommandlet::UDasAuthoringCommandlet() { IsClient=false;IsServer=false;IsEditor=true;LogToConsole=true; }
int32 UDasAuthoringCommandlet::Main(const FString& Params)
{
    if (FApp::GetProjectName()!=FString(TEXT("DasForUESmoke"))) {UE_LOG(LogTemp,Error,TEXT("Authoring probe requires isolated DasForUESmoke project"));return 1;}
    FString Phase,FirstFile,SecondFile,Error,ProbeId;
    if (!FParse::Value(*Params,TEXT("ProbeId="),ProbeId) || ProbeId.Len()!=32) return 1;
    for (TCHAR Ch : ProbeId) if (!FChar::IsHexDigit(Ch)) return 1;
    const FString AuthoringAssetPath=TEXT("/Game/DasAuthoringProbe/Run_")+ProbeId+TEXT("/BP_AuthoringCounter");
    if (!FParse::Value(*Params,TEXT("Phase="),Phase) || !FParse::Value(*Params,TEXT("First="),FirstFile) || !FParse::Value(*Params,TEXT("Second="),SecondFile)) return 1;
    FDasAuthoringModule First,Second;
    if (!LoadModel(FirstFile,First,Error) || !LoadModel(SecondFile,Second,Error)) {UE_LOG(LogTemp,Error,TEXT("%s"),*Error);return 1;}
    FDasClassSchema Preflight;
    if (!First.ToActorSchema(AuthoringProbeClassId,Preflight,Error) || !Second.ToActorSchema(AuthoringProbeClassId,Preflight,Error)) {UE_LOG(LogTemp,Error,TEXT("%s"),*Error);return 1;}
    bool Passed=true;
    auto Check=[&](bool OK,const TCHAR* Name){Passed &= OK;UE_LOG(LogTemp,Display,TEXT("DAS_AUTHORING %s %s"),OK?TEXT("PASS"):TEXT("FAIL"),Name);};
    UBlueprint* BP=nullptr;
    if (Phase==TEXT("Create"))
    {
        bool Applied=false;
        if(FPaths::GetExtension(FirstFile)==TEXT("das")) Applied=FDasDSLService::Apply(FPaths::GetCleanFilename(FirstFile),AuthoringAssetPath,AuthoringProbeClassId,BP,Error);
        else
        {
            UPackage* Package=CreatePackage(*AuthoringAssetPath);
            BP=FKismetEditorUtilities::CreateBlueprint(ADasProbeActor::StaticClass(),Package,TEXT("BP_AuthoringCounter"),BPTYPE_Normal,UBlueprint::StaticClass(),UBlueprintGeneratedClass::StaticClass(),TEXT("DasAuthoringProbe"));
            Applied=BP && FDasBlueprintService::ApplyAuthoring(BP,First,AuthoringProbeClassId,Error);
        }
        Check(Applied,TEXT("model_to_blueprint"));
    }
    else if (Phase==TEXT("Reopen"))
    {
        BP=LoadObject<UBlueprint>(nullptr,*(AuthoringAssetPath+TEXT(".BP_AuthoringCounter")));
        Check(BP && BP->GeneratedClass,TEXT("reopen_saved_blueprint"));
    }
    else return 1;
    if (!Passed || !BP || !BP->GeneratedClass) {UE_LOG(LogTemp,Error,TEXT("%s"),*Error);return 1;}
    TStrongObjectPtr<UWorld> World(UWorld::CreateWorld(EWorldType::Editor,false));
    FActorSpawnParameters Spawn;Spawn.Name=TEXT("AuthoringCounter");Spawn.SpawnCollisionHandlingOverride=ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
    auto* Actor=World->SpawnActor<ADasProbeActor>(BP->GeneratedClass,FTransform::Identity,Spawn);
    Check(Actor!=nullptr,TEXT("spawn_authored_actor")); if (!Actor) return 1;
    if (Phase==TEXT("Create"))
    {
        Check(Execute(Actor)==15,TEXT("declared_method_dispatches_dascript"));
        auto* ScoreProperty=FindFProperty<FIntProperty>(Actor->GetClass(),TEXT("Score"));
        Check(ScoreProperty!=nullptr,TEXT("declared_property_reflected"));
        if (!ScoreProperty) { World->DestroyWorld(false); return 1; }
        ScoreProperty->SetPropertyValue_InContainer(Actor,77);
        const auto Delta=FDasAuthoringModule::Compare(First.Classes[0],Second.Classes[0]);
        Check(Delta.bStructural && Delta.bImplementation && Delta.Added.Num()==4,TEXT("stable_id_diff_add_and_implementation"));
        const bool AppliedSecond=FPaths::GetExtension(SecondFile)==TEXT("das")
            ? FDasDSLService::Apply(FPaths::GetCleanFilename(SecondFile),AuthoringAssetPath,AuthoringProbeClassId,BP,Error)
            : FDasBlueprintService::ApplyAuthoring(BP,Second,AuthoringProbeClassId,Error);
        Check(AppliedSecond,TEXT("apply_second_declaration"));
        Actor=Find(World.Get());
        Check(Actor && Execute(Actor)==187,TEXT("reinstance_preserves_override_and_adds_default"));
        const FString StablePropertyId=TEXT("example.counter.score");FString StoredId;
        Check(FBlueprintEditorUtils::GetBlueprintVariableMetaData(BP,TEXT("Score"),nullptr,TEXT("DasMemberId"),StoredId) && StoredId==StablePropertyId,TEXT("property_stable_id_persisted"));
        const int32 VariableCount=BP->NewVariables.Num();UClass* Before=BP->GeneratedClass;
        auto Unsupported=Second;Unsupported.Classes[0].Methods[0].bStatic=true;
        Check(!FDasBlueprintService::ApplyAuthoring(BP,Unsupported,AuthoringProbeClassId,Error) && BP->GeneratedClass==Before && BP->NewVariables.Num()==VariableCount,TEXT("unsupported_backend_no_mutation"));
        auto InvalidBody=Second;InvalidBody.Classes[0].Methods[0].ScriptFile=TEXT("invalid.das");
        Check(!FDasBlueprintService::ApplyAuthoring(BP,InvalidBody,AuthoringProbeClassId,Error) && BP->GeneratedClass==Before && BP->NewVariables.Num()==VariableCount,TEXT("invalid_method_compile_no_mutation"));
        auto BadSignature=Second;BadSignature.Classes[0].Methods[0].ScriptFile=TEXT("wrong_signature.das");
        Check(!FDasBlueprintService::ApplyAuthoring(BP,BadSignature,AuthoringProbeClassId,Error) && BP->GeneratedClass==Before,TEXT("invalid_method_signature_no_mutation"));
        auto MissingEntry=Second;MissingEntry.Classes[0].Methods.Last().EntryPoint=TEXT("missing_entry");
        Check(!FDasBlueprintService::ApplyAuthoring(BP,MissingEntry,AuthoringProbeClassId,Error) && BP->GeneratedClass==Before && BP->NewVariables.Num()==VariableCount,TEXT("later_method_preflight_no_mutation"));
        auto WrongParameter=Second;WrongParameter.Classes[0].Methods[0].Parameters[0].Id=TEXT("other-input-id");
        Check(!FDasBlueprintService::ApplyAuthoring(BP,WrongParameter,AuthoringProbeClassId,Error) && BP->GeneratedClass==Before,TEXT("parameter_id_change_no_mutation"));
        auto WrongId=Second;WrongId.Classes[0].Properties[0].Id=TEXT("other-score-id");
        Check(!FDasBlueprintService::ApplyAuthoring(BP,WrongId,AuthoringProbeClassId,Error) && BP->GeneratedClass==Before,TEXT("property_id_change_no_mutation"));
        UPackage* Package=BP->GetOutermost();BP->SetFlags(RF_Public|RF_Standalone);
        const FString Filename=FPackageName::LongPackageNameToFilename(AuthoringAssetPath,FPackageName::GetAssetPackageExtension());
        IFileManager::Get().MakeDirectory(*FPaths::GetPath(Filename),true);
        FSavePackageArgs Args;Args.TopLevelFlags=RF_Public|RF_Standalone;Args.SaveFlags=SAVE_NoError;
        Check(UPackage::SavePackage(Package,BP,*Filename,Args),TEXT("save_authored_blueprint"));
    }
    else
    {
        Check(Execute(Actor)==120,TEXT("saved_defaults_and_method_reopen"));
        Check(BP->GetOutermost()->GetMetaData().GetValue(BP,TEXT("DasClassId"))==AuthoringProbeClassId,TEXT("class_stable_id_reopen"));
        Check(BP->GetOutermost()->GetMetaData().GetValue(BP,TEXT("DasMethodId"))==TEXT("example.counter.evaluate"),TEXT("method_stable_id_reopen"));
        Check(BP->GetOutermost()->GetMetaData().GetValue(BP,TEXT("DasInputId"))==TEXT("example.counter.evaluate.input"),TEXT("parameter_stable_id_reopen"));
        UFunction* LoadedInspect=BP->GeneratedClass->FindFunctionByName(TEXT("Inspect"));
        FProperty* LoadedScore=FindFProperty<FProperty>(BP->GeneratedClass,TEXT("Score"));
        Check(LoadedInspect && LoadedInspect->GetMetaData(TEXT("DisplayName"))==TEXT("Inspect state") && LoadedScore && LoadedScore->GetMetaData(TEXT("ToolTip"))==TEXT("Score tooltip") && BP->GeneratedClass->GetMetaData(TEXT("DisplayName"))==TEXT("Counter DSL"),TEXT("metadata_survives_reopen_before_apply"));
        if(FPaths::GetExtension(SecondFile)==TEXT("das"))
        {
            auto& Metadata=BP->GetOutermost()->GetMetaData();
            Check(Metadata.GetValue(BP,TEXT("DasDSLSource"))==FPaths::GetCleanFilename(SecondFile) && Metadata.GetValue(BP,TEXT("DasDSLClassId"))==AuthoringProbeClassId,TEXT("DSL_import_source_survives_reopen"));
            UClass* BeforeClass=BP->GeneratedClass;
            Check(FDasDSLService::Reimport(BP,Error) && BP->GeneratedClass==BeforeClass,TEXT("reopened_DSL_reimport_without_reinstance"));
        }
        Check(FDasBlueprintService::ApplyAuthoring(BP,Second,AuthoringProbeClassId,Error),TEXT("reopened_model_idempotent_apply"));
    }
    Actor=Find(World.Get());
    Check(Actor && Execute(Actor,TEXT("Inspect"))==18 && Execute(Actor,TEXT("InspectAgain"))==1007 && Execute(Actor,TEXT("Inspect"))==20,TEXT("named_methods_share_context_state"));
    auto* Locked=FindFProperty<FIntProperty>(BP->GeneratedClass,TEXT("Locked"));
    auto* Score=FindFProperty<FIntProperty>(BP->GeneratedClass,TEXT("Score"));
    auto* Inspect=BP->GeneratedClass->FindFunctionByName(TEXT("Inspect"));
    Check(Locked && Locked->HasAnyPropertyFlags(CPF_BlueprintReadOnly) && Locked->GetMetaData(TEXT("Category"))==TEXT("Stats"),TEXT("readonly_property_and_category_metadata"));
    Check(Score && Score->GetMetaData(TEXT("ToolTip"))==TEXT("Score tooltip") && Inspect && Inspect->GetMetaData(TEXT("DisplayName"))==TEXT("Inspect state"),TEXT("property_and_method_metadata"));
    Check(BP->GeneratedClass->GetMetaData(TEXT("DisplayName"))==TEXT("Counter DSL"),TEXT("class_metadata"));
    auto& Runtime=FModuleManager::LoadModuleChecked<FDasRuntimeModule>(TEXT("DasRuntime"));
    const auto Write=Runtime.RunNamedPersistent(Runtime.ScriptPath(TEXT("authoring_methods.das")),TEXT("readonly_write"),0,Actor);
    Check(!Write.bSucceeded && Write.Error.Contains(TEXT("read-only")) && Locked && Locked->GetPropertyValue_InContainer(Actor)==12,TEXT("readonly_write_rejected"));
    World->DestroyWorld(false);
    UE_LOG(LogTemp,Display,TEXT("DAS_AUTHORING_RESULT=%s"),Passed?TEXT("PASS"):TEXT("FAIL"));return Passed?0:1;
}
