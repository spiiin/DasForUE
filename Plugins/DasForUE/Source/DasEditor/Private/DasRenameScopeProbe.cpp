#include "DasDSLService.h"
#include "DasRuntime.h"
#include "DasProbeActor.h"
#include "Modules/ModuleManager.h"
#include "Engine/Blueprint.h"
#include "Engine/BlueprintGeneratedClass.h"
#include "Engine/World.h"
#include "Engine/Level.h"
#include "Editor.h"
#include "Kismet2/KismetEditorUtilities.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "K2Node_VariableGet.h"
#include "EdGraphSchema_K2.h"
#include "UObject/UnrealType.h"
#include "UObject/SavePackage.h"
#include "UObject/StructOnScope.h"
#include "Misc/Parse.h"
#include "Misc/Paths.h"
#include "Misc/PackageName.h"
#include "Misc/FileHelper.h"
#include "HAL/FileManager.h"
namespace {
int32 ScopeScore(UObject* Object,FName Name=TEXT("Total")){auto* P=Object?FindFProperty<FIntProperty>(Object->GetClass(),Name):nullptr;return P?P->GetPropertyValue_InContainer(Object):-1;}
int32 ScopeCount(UObject* Object,FName Name=TEXT("Data")){auto* P=Object?FindFProperty<FStructProperty>(Object->GetClass(),Name):nullptr;auto* C=P?FindFProperty<FIntProperty>(P->Struct,TEXT("Count")):nullptr;return C?C->GetPropertyValue_InContainer(P->ContainerPtrToValuePtr<void>(Object)):-1;}
void SetScopeValues(UObject* Object,int32 Score,int32 Count){FindFProperty<FIntProperty>(Object->GetClass(),TEXT("Score"))->SetPropertyValue_InContainer(Object,Score);auto* P=FindFProperty<FStructProperty>(Object->GetClass(),TEXT("Payload"));FindFProperty<FIntProperty>(P->Struct,TEXT("Count"))->SetPropertyValue_InContainer(P->ContainerPtrToValuePtr<void>(Object),Count);}
bool SaveScopeAsset(UObject* Asset){if(!Asset)return false;auto* P=Asset->GetOutermost();const FString File=FPackageName::LongPackageNameToFilename(P->GetName(),Asset->IsA<UWorld>()?FPackageName::GetMapPackageExtension():FPackageName::GetAssetPackageExtension());IFileManager::Get().MakeDirectory(*FPaths::GetPath(File),true);Asset->SetFlags(RF_Public|RF_Standalone);FSavePackageArgs Args;Args.TopLevelFlags=RF_Public|RF_Standalone;Args.SaveFlags=SAVE_NoError;return UPackage::SavePackage(P,Asset,*File,Args);}
UBlueprint* ScopeBP(const FString& Base,const TCHAR* Name){return LoadObject<UBlueprint>(nullptr,*(Base+Name+TEXT(".")+Name));}
UBlueprint* NewScopeBP(const FString& Base,const TCHAR* Name,UClass* Parent){auto* BP=FKismetEditorUtilities::CreateBlueprint(Parent,CreatePackage(*(Base+Name)),Name,BPTYPE_Normal,UBlueprint::StaticClass(),UBlueprintGeneratedClass::StaticClass(),TEXT("DasRenameScope"));FAssetRegistryModule::AssetCreated(BP);return BP;}
ADasProbeActor* ScopeActor(UWorld* World,FName Name){if(World && World->PersistentLevel)for(auto& Entry:World->PersistentLevel->Actors)if(auto* A=Cast<ADasProbeActor>(Entry.Get()))if(A->GetFName()==Name)return A;return nullptr;}
}
int32 RunDasRenameScopeProbe(const FString& Params){
    if(FApp::GetProjectName()!=FString(TEXT("DasForUESmoke")))return 1;FString Id,Phase,Error;if(!FParse::Value(*Params,TEXT("ProbeId="),Id) || Id.Len()!=32 || !FParse::Value(*Params,TEXT("Phase="),Phase))return 1;for(TCHAR C:Id)if(!FChar::IsHexDigit(C))return 1;
    const FString Base=TEXT("/Game/DasFrameAssetProbe/Scope_")+Id+TEXT("/"),Script=TEXT("dsl_renamescope_")+Id+TEXT(".das");auto& Runtime=FModuleManager::LoadModuleChecked<FDasRuntimeModule>(TEXT("DasRuntime"));bool Passed=true;
    auto Check=[&](bool Value,const TCHAR* Name){Passed &= Value;UE_LOG(LogTemp,Display,TEXT("DAS_RENAME_SCOPE_TEST %s %s"),Value?TEXT("PASS"):TEXT("FAIL"),Name);};
    UBlueprint* Parent=nullptr;UBlueprint* Child=nullptr;UBlueprint* Grand=nullptr;UBlueprint* External=nullptr;UWorld* World=nullptr;
    if(Phase==TEXT("Create")){
        FString Source;Check(FFileHelper::LoadFileToString(Source,*Runtime.ScriptPath(TEXT("dsl_rename_fields.das"))) && FFileHelper::SaveStringToFile(Source,*Runtime.ScriptPath(Script),FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM),TEXT("create_scope_source"));
        Check(FDasDSLService::Apply(Script,Base+TEXT("BP_Parent"),TEXT("rename.RenameFields"),Parent,Error),TEXT("create_scope_parent"));if(!Parent)return 1;
        Child=NewScopeBP(Base,TEXT("BP_Child"),Parent->GeneratedClass);SetScopeValues(Child->GeneratedClass->GetDefaultObject(),23,34);
        Grand=NewScopeBP(Base,TEXT("BP_Grand"),Child->GeneratedClass);SetScopeValues(Grand->GeneratedClass->GetDefaultObject(),45,56);
        External=NewScopeBP(Base,TEXT("BP_External"),ADasProbeActor::StaticClass());FEdGraphPinType Type;Type.PinCategory=UEdGraphSchema_K2::PC_Object;Type.PinSubCategoryObject=Parent->GeneratedClass;Check(FBlueprintEditorUtils::AddMemberVariable(External,TEXT("Target"),Type),TEXT("create_external_parent_reference"));
        auto* Graph=External->UbergraphPages[0].Get();auto* Target=NewObject<UK2Node_VariableGet>(Graph);Target->VariableReference.SetSelfMember(TEXT("Target"));Graph->AddNode(Target);Target->CreateNewGuid();Target->AllocateDefaultPins();auto* Value=NewObject<UK2Node_VariableGet>(Graph);Value->VariableReference.SetExternalMember(TEXT("Score"),Parent->GeneratedClass);Graph->AddNode(Value);Value->CreateNewGuid();Value->AllocateDefaultPins();Check(Graph->GetSchema()->TryCreateConnection(Target->GetValuePin(),Value->FindPinChecked(UEdGraphSchema_K2::PN_Self)),TEXT("connect_external_field_getter"));FKismetEditorUtilities::CompileBlueprint(External);
        World=UWorld::CreateWorld(EWorldType::Editor,false,TEXT("Map"),CreatePackage(*(Base+TEXT("Map"))));World->SetFlags(RF_Public|RF_Standalone);World->GetOutermost()->SetPackageFlags(PKG_ContainsMap);FAssetRegistryModule::AssetCreated(World);
        FActorSpawnParameters Spawn;Spawn.Name=TEXT("ParentActor");SetScopeValues(World->SpawnActor<ADasProbeActor>(Parent->GeneratedClass,FTransform::Identity,Spawn),77,88);Spawn.Name=TEXT("ChildActor");SetScopeValues(World->SpawnActor<ADasProbeActor>(Child->GeneratedClass,FTransform::Identity,Spawn),99,100);Spawn.Name=TEXT("GrandActor");SetScopeValues(World->SpawnActor<ADasProbeActor>(Grand->GeneratedClass,FTransform::Identity,Spawn),111,112);
        Check(SaveScopeAsset(Parent) && SaveScopeAsset(Child) && SaveScopeAsset(Grand) && SaveScopeAsset(External) && SaveScopeAsset(World),TEXT("save_original_scope_assets"));
    }else if(Phase==TEXT("Migrate") || Phase==TEXT("Reopen")){
        Parent=ScopeBP(Base,TEXT("BP_Parent"));Check(Parent!=nullptr,TEXT("load_scope_parent"));if(!Parent)return 1;
        if(Phase==TEXT("Migrate")){
            Check(!FindObject<UBlueprint>(nullptr,*(Base+TEXT("BP_Child.BP_Child"))) && !FindObject<UWorld>(nullptr,*(Base+TEXT("Map.Map"))),TEXT("dependencies_initially_unloaded"));
            FString Source;FFileHelper::LoadFileToString(Source,*Runtime.ScriptPath(Script));Source=Source.Replace(TEXT("Score :"),TEXT("Total :")).Replace(TEXT("self.Score"),TEXT("self.Total")).Replace(TEXT("Payload :"),TEXT("Data :")).Replace(TEXT("self.Payload"),TEXT("self.Data"));Check(FFileHelper::SaveStringToFile(Source,*Runtime.ScriptPath(Script),FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM),TEXT("write_scope_rename"));
            UClass* Before=Parent->GeneratedClass;Check(!FDasDSLService::ReimportRenamedFields(Parent,Error) && Parent->GeneratedClass==Before,TEXT("isolated_policy_refuses_saved_dependencies"));
            auto* CollisionChild=ScopeBP(Base,TEXT("BP_Child"));FEdGraphPinType CollisionType;CollisionType.PinCategory=UEdGraphSchema_K2::PC_Int;Check(FBlueprintEditorUtils::AddMemberVariable(CollisionChild,TEXT("Total"),CollisionType),TEXT("create_child_rename_collision"));FKismetEditorUtilities::CompileBlueprint(CollisionChild);
            Before=Parent->GeneratedClass;Check(!FDasDSLService::ReimportRenamedFields(Parent,Error,true) && Parent->GeneratedClass==Before && Parent->GeneratedClass->FindPropertyByName(TEXT("Score")) && !Parent->GeneratedClass->FindPropertyByName(TEXT("Total")),TEXT("child_destination_collision_refused_before_rename"));
            FBlueprintEditorUtils::RemoveMemberVariable(CollisionChild,TEXT("Total"));FKismetEditorUtilities::CompileBlueprint(CollisionChild);
            Check(FDasDSLService::ReimportRenamedFields(Parent,Error,true),TEXT("migrate_complete_dependency_scope"));if(!Error.IsEmpty())UE_LOG(LogTemp,Error,TEXT("Scope: %s"),*Error);
        }
        Child=ScopeBP(Base,TEXT("BP_Child"));Grand=ScopeBP(Base,TEXT("BP_Grand"));External=ScopeBP(Base,TEXT("BP_External"));World=LoadObject<UWorld>(nullptr,*(Base+TEXT("Map.Map")));
        Check(Child && Grand && External && World,TEXT("load_migrated_dependency_assets"));
        Check(ScopeScore(Parent->GeneratedClass->GetDefaultObject())==5 && ScopeCount(Parent->GeneratedClass->GetDefaultObject())==10 && Child && ScopeScore(Child->GeneratedClass->GetDefaultObject())==23 && ScopeCount(Child->GeneratedClass->GetDefaultObject())==34 && Grand && ScopeScore(Grand->GeneratedClass->GetDefaultObject())==45 && ScopeCount(Grand->GeneratedClass->GetDefaultObject())==56,TEXT("parent_child_grandchild_CDO_overrides_preserved"));
        auto* A=ScopeActor(World,TEXT("ParentActor"));auto* B=ScopeActor(World,TEXT("ChildActor"));auto* C=ScopeActor(World,TEXT("GrandActor"));Check(ScopeScore(A)==77 && ScopeCount(A)==88 && ScopeScore(B)==99 && ScopeCount(B)==100 && ScopeScore(C)==111 && ScopeCount(C)==112,TEXT("saved_map_actor_overrides_preserved"));
        if(External){TArray<UK2Node_VariableGet*> Nodes;FBlueprintEditorUtils::GetAllNodesOfClass(External,Nodes);Check(Nodes.ContainsByPredicate([](const auto* N){return N->VariableReference.GetMemberName()==TEXT("Total");}) && !Nodes.ContainsByPredicate([](const auto* N){return N->VariableReference.GetMemberName()==TEXT("Score");}),TEXT("external_Blueprint_field_reference_updated"));}
        if(C){auto* F=C->FindFunction(TEXT("Add"));FStructOnScope Frame(F);FindFProperty<FIntProperty>(F,TEXT("Input"))->SetPropertyValue_InContainer(Frame.GetStructMemory(),0);FEditorScriptExecutionGuard Guard;C->ProcessEvent(F,Frame.GetStructMemory());Check(CastField<FIntProperty>(F->GetReturnProperty())->GetPropertyValue_InContainer(Frame.GetStructMemory())==223,TEXT("inherited_DSL_method_uses_declaring_class_defaults"));}
        if(Phase==TEXT("Migrate")){Check(Parent->GetOutermost()->IsDirty() && Child && Child->GetOutermost()->IsDirty() && External && External->GetOutermost()->IsDirty() && World && World->GetOutermost()->IsDirty(),TEXT("migration_marks_scope_dirty_without_autosave"));Check(SaveScopeAsset(Parent) && SaveScopeAsset(Child) && SaveScopeAsset(Grand) && SaveScopeAsset(External) && SaveScopeAsset(World),TEXT("save_migrated_scope"));}
        UClass* Before=Parent->GeneratedClass;Check(FDasDSLService::ReimportRenamedFields(Parent,Error,true) && Parent->GeneratedClass==Before,TEXT("dependent_scope_repeat_idempotent"));
    }else return 1;
    if(World && World->IsInitialized())World->DestroyWorld(false);
    UE_LOG(LogTemp,Display,TEXT("DAS_RENAME_SCOPE_RESULT=%s"),Passed?TEXT("PASS"):TEXT("FAIL"));return Passed?0:1;
}
