#include "DasInheritedDefaultsCommandlet.h"
#include "DasDSLService.h"
#include "DasDefaultsMigration.h"
#include "DasRuntime.h"
#include "DasAuthoring.h"
#include "DasProbeActor.h"
#include "DasScriptComponent.h"
#include "Engine/Blueprint.h"
#include "Engine/BlueprintGeneratedClass.h"
#include "Engine/World.h"
#include "Engine/Level.h"
#include "Kismet2/KismetEditorUtilities.h"
#include "UObject/UnrealType.h"
#include "UObject/StructOnScope.h"
#include "UObject/SavePackage.h"
#include "UObject/Script.h"
#include "Misc/FileHelper.h"
#include "Misc/ScopeExit.h"
#include "Misc/PackageName.h"
#include "HAL/FileManager.h"
namespace {
int32 InheritedInt(UObject* O,const TCHAR* Name){return FindFProperty<FIntProperty>(O->GetClass(),Name)->GetPropertyValue_InContainer(O);}
void InheritedSet(UObject* O,const TCHAR* Name,int32 V){FindFProperty<FIntProperty>(O->GetClass(),Name)->SetPropertyValue_InContainer(O,V);}
FVector& InheritedVector(UObject* O){return *FindFProperty<FStructProperty>(O->GetClass(),TEXT("Position"))->ContainerPtrToValuePtr<FVector>(O);}
int32 InheritedCall(UObject* A,const TCHAR* Name){auto* F=A->FindFunction(Name);if(!F)return MIN_int32;FStructOnScope Args(F);FindFProperty<FIntProperty>(F,TEXT("input"))->SetPropertyValue_InContainer(Args.GetStructMemory(),0);A->ProcessEvent(F,Args.GetStructMemory());return CastFieldChecked<FIntProperty>(F->GetReturnProperty())->GetPropertyValue_InContainer(Args.GetStructMemory());}
bool InheritedSave(UObject* O){const FString File=FPackageName::LongPackageNameToFilename(O->GetOutermost()->GetName(),O->IsA<UWorld>()?FPackageName::GetMapPackageExtension():FPackageName::GetAssetPackageExtension());IFileManager::Get().MakeDirectory(*FPaths::GetPath(File),true);FSavePackageArgs Args;Args.TopLevelFlags=RF_Public|RF_Standalone;return UPackage::SavePackage(O->GetOutermost(),O,*File,Args);}
}
UDasInheritedDefaultsCommandlet::UDasInheritedDefaultsCommandlet(){IsEditor=true;IsClient=false;IsServer=false;LogToConsole=true;}
int32 UDasInheritedDefaultsCommandlet::Main(const FString& Params){
 FString Id,Phase;FParse::Value(*Params,TEXT("ProbeId="),Id);FParse::Value(*Params,TEXT("Phase="),Phase);FGuid Guid;if(!FGuid::ParseExact(Id,EGuidFormats::Digits,Guid))return 1;
 FEditorScriptExecutionGuard Guard;auto& R=FModuleManager::LoadModuleChecked<FDasRuntimeModule>(TEXT("DasRuntime"));
 const FString Root=TEXT("/Game/DasInheritedDefaults/")+Id,BasePath=R.ScriptPath(TEXT("ue_inherited_defaults_base.das")),ChildPath=R.ScriptPath(TEXT("dsl_inherited_defaults.das"));FString BaseSource,ChildSource,Error;
 if(!FFileHelper::LoadFileToString(BaseSource,*BasePath) || !FFileHelper::LoadFileToString(ChildSource,*ChildPath))return 1;
 auto Write=[](const FString& Path,const FString& Text){return FFileHelper::SaveStringToFile(Text,*Path,FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM);};
 ON_SCOPE_EXIT{Write(BasePath,BaseSource);Write(ChildPath,ChildSource);};
 auto Check=[&](bool OK,const TCHAR* Name){UE_LOG(LogTemp,Display,TEXT("DAS_INHERITED_DEFAULTS %s %s %s"),OK?TEXT("PASS"):TEXT("FAIL"),Name,*Error);return OK;};
 UBlueprint* Base=nullptr;UBlueprint* Child=nullptr;UBlueprint* Leaf=nullptr;UBlueprint* Ordinary=nullptr;UWorld* W=nullptr;ADasProbeActor* A=nullptr;ADasProbeActor* M=nullptr;
 ON_SCOPE_EXIT{if(W && W->IsInitialized())W->DestroyWorld(false);};
 auto LoadBP=[&](const TCHAR* Name){return LoadObject<UBlueprint>(nullptr,*(Root+TEXT("/")+Name+TEXT(".")+Name));};
 if(Phase==TEXT("Create")){
  const FString OldBase=BaseSource.Replace(TEXT("int=12"),TEXT("int=10")),OldChild=ChildSource.Replace(TEXT("self.Score=25"),TEXT("self.Score=20")).Replace(TEXT("self.Manual=25"),TEXT("self.Manual=20"));
  if(!Write(BasePath,OldBase) || !Write(ChildPath,OldChild) || !FDasDSLService::Apply(TEXT("ue_inherited_defaults_base.das"),Root+TEXT("/BP_DefaultsBase"),TEXT("inheriteddefaults.DefaultsBase"),Base,Error))return 1;
  FKismetEditorUtilities::CompileBlueprint(Base);InheritedSet(Base->GeneratedClass->GetDefaultObject(),TEXT("Keep"),150);
  if(!Check(FDasDSLService::Apply(TEXT("dsl_inherited_defaults_leaf.das"),Root+TEXT("/BP_DefaultsLeaf"),TEXT("inheriteddefaults.DefaultsLeaf"),Leaf,Error),TEXT("initial_import")))return 1;
  Base=LoadBP(TEXT("BP_DefaultsBase"));Child=LoadBP(TEXT("BP_DefaultsChild"));if(!Base || !Child)return 1;auto* BCDO=Base->GeneratedClass->GetDefaultObject();auto* CDO=Child->GeneratedClass->GetDefaultObject();
  if(!Check(Child->NewVariables.IsEmpty() && Leaf->NewVariables.IsEmpty() && Child->GeneratedClass->FindPropertyByName(TEXT("Score"))->GetOwnerClass()==Base->GeneratedClass && InheritedInt(BCDO,TEXT("Score"))==10 && InheritedInt(BCDO,TEXT("Keep"))==150 && InheritedInt(CDO,TEXT("Keep"))==150 && InheritedInt(CDO,TEXT("Score"))==20 && InheritedInt(Leaf->GeneratedClass->GetDefaultObject(),TEXT("Score"))==30,TEXT("override_without_duplicate_property_or_parent_mutation")))return 1;
  Ordinary=FKismetEditorUtilities::CreateBlueprint(Child->GeneratedClass,CreatePackage(*(Root+TEXT("/BP_Ordinary"))),TEXT("BP_Ordinary"),BPTYPE_Normal,UBlueprint::StaticClass(),UBlueprintGeneratedClass::StaticClass());FKismetEditorUtilities::CompileBlueprint(Ordinary);InheritedSet(Ordinary->GeneratedClass->GetDefaultObject(),TEXT("Manual"),55);
  InheritedSet(CDO,TEXT("Manual"),77);InheritedVector(CDO).X=99;
  W=UWorld::CreateWorld(EWorldType::Editor,false,TEXT("Map"),CreatePackage(*(Root+TEXT("/Map"))));if(!W)return 1;W->SetFlags(RF_Public|RF_Standalone);W->GetOutermost()->SetPackageFlags(PKG_ContainsMap);
  A=W->SpawnActor<ADasProbeActor>(Child->GeneratedClass);M=W->SpawnActor<ADasProbeActor>(Child->GeneratedClass);if(!A || !M)return 1;A->Tags.Add(TEXT("InheritedDefault"));M->Tags.Add(TEXT("InheritedManual"));InheritedSet(M,TEXT("Score"),111);InheritedVector(M).Y=222;
  if(!Check(InheritedCall(A,TEXT("Read"))==97 && A->LastDasError.IsEmpty(),TEXT("initial_inherited_runtime_validation")))return 1;
  // 20 + manually edited 77, then migrate source defaults to 25.
  const auto* Class=Child->GeneratedClass.Get();const auto* BaseClass=Base->GeneratedClass.Get();const int32 Contexts=R.PersistentContextCount();
  if(!Write(ChildPath,ChildSource))return 1;
#if WITH_DEV_AUTOMATION_TESTS
  for(int32 Stage:{1,2}){DasFailDefaultsMergeForTesting(Stage);const bool Changed=FDasDSLService::Reimport(Child,Error);DasFailDefaultsMergeForTesting(0);if(!Check(!Changed && Error.Contains(TEXT("rolled back")) && InheritedInt(CDO,TEXT("Score"))==20 && InheritedInt(A,TEXT("Score"))==20 && InheritedInt(CDO,TEXT("Manual"))==77 && Child->GeneratedClass.Get()==Class,TEXT("inherited_default_rollback")))return 1;}
#endif
  if(!Check(FDasDSLService::Reimport(Child,Error) && Child->GeneratedClass.Get()==Class && R.PersistentContextCount()==Contexts,TEXT("child_defaults_merge_without_reinstance")))return 1;
  if(!Check(InheritedCall(A,TEXT("Read"))==102 && A->LastDasError.IsEmpty(),TEXT("existing_VM_accepts_inherited_default_update")))return 1;
  // Removing an override returns unchanged values to the current actual parent CDO.
  const FString Removed=ChildSource.Replace(TEXT("self.Score=25"),TEXT(""));if(!Write(ChildPath,Removed) || !Check(FDasDSLService::Reimport(Child,Error) && InheritedInt(CDO,TEXT("Score"))==10 && InheritedInt(M,TEXT("Score"))==111 && InheritedInt(Leaf->GeneratedClass->GetDefaultObject(),TEXT("Score"))==30,TEXT("remove_override_uses_parent_without_erasing_manual_values")))return 1;
  if(!Write(ChildPath,ChildSource) || !Check(FDasDSLService::Reimport(Child,Error) && InheritedInt(CDO,TEXT("Score"))==25,TEXT("add_inherited_override_again")))return 1;
  if(!Write(BasePath,BaseSource))return 1;const FString ChildWire=Child->GetOutermost()->GetMetaData().GetValue(Child,TEXT("DasDSLModel"));
  if(!Check(!FDasDSLService::Reimport(Child,Error) && Error.Contains(TEXT("Reimport authored parent")) && ChildWire==Child->GetOutermost()->GetMetaData().GetValue(Child,TEXT("DasDSLModel")) && InheritedInt(CDO,TEXT("Score"))==25 && InheritedInt(BCDO,TEXT("Score"))==10,TEXT("unapplied_parent_defaults_refused_before_child_mutation")))return 1;
  if(!Write(BasePath,BaseSource) || !Check(FDasDSLService::Reimport(Base,Error) && Base->GeneratedClass.Get()==BaseClass && InheritedInt(BCDO,TEXT("Score"))==12 && InheritedInt(CDO,TEXT("Score"))==25 && InheritedInt(CDO,TEXT("Frozen"))==10 && InheritedInt(Leaf->GeneratedClass->GetDefaultObject(),TEXT("Frozen"))==10,TEXT("parent_merge_respects_explicit_child_defaults_even_equal_old_parent")))return 1;
  if(!Check(FDasDSLService::Reimport(Child,Error),TEXT("refresh_child_parent_source_snapshot")))return 1;
  if(!Check(FDasDSLService::Reimport(Child,Error),TEXT("unchanged_source_reimport_is_order_independent")))return 1;
  FDasAuthoringModule Model,Copy;FString Wire;if(!R.ReadAuthoringScript(ChildPath,Model,Error) || !Model.ToJson(Wire,Error) || !FDasAuthoringModule::FromJson(Wire,Copy,Error))return 1;
  const auto* Decl=Copy.Classes.FindByPredicate([](const auto& C){return C.Name==TEXT("DefaultsChild");});if(!Check(Decl && Decl->Properties.IsEmpty() && Decl->PropertyOverrides.Num()==6,TEXT("JSON_roundtrip_preserves_owned_override_records")))return 1;
  for(const TCHAR* Case:{TEXT("unknown"),TEXT("duplicate"),TEXT("type"),TEXT("expression")}){
   FString BadSource=ChildSource.Replace(TEXT("DefaultsChild"),TEXT("BadDefaults"));
   if(FString(Case)==TEXT("unknown"))BadSource.ReplaceInline(TEXT("self.Score=25"),TEXT("self.Missing=25"));
   if(FString(Case)==TEXT("duplicate"))BadSource.ReplaceInline(TEXT("self.Manual=25"),TEXT("self.Score=25"));
   if(FString(Case)==TEXT("type"))BadSource.ReplaceInline(TEXT("self.Score=25"),TEXT("self.Score=true"));
   if(FString(Case)==TEXT("expression"))BadSource.ReplaceInline(TEXT("self.Score=25"),TEXT("self.Score=ue_get_self_int(\"Score\")"));
   const FString Script=TEXT("dsl_inherited_bad_")+Id+TEXT("_")+Case+TEXT(".das"),File=R.ScriptPath(Script),Package=Root+TEXT("/BP_Rejected_")+Case;
   if(!Write(File,BadSource))return 1;ON_SCOPE_EXIT{IFileManager::Get().Delete(*File);};UBlueprint* Rejected=nullptr;
   if(!Check(!FDasDSLService::Apply(Script,Package,TEXT("inheriteddefaults.BadDefaults"),Rejected,Error) && !FindPackage(nullptr,*Package),*(TEXT("negative_")+FString(Case)+TEXT("_before_asset_creation"))))return 1;
  }
  auto Bad=Copy;auto* BadDecl=Bad.Classes.FindByPredicate([](const auto& C){return C.Name==TEXT("DefaultsChild");});const auto Duplicate=BadDecl->PropertyOverrides[0];BadDecl->PropertyOverrides.Add(Duplicate);if(!Check(!Bad.Validate(Error),TEXT("duplicate_override_IR_rejected")))return 1;
  Bad=Copy;BadDecl=Bad.Classes.FindByPredicate([](const auto& C){return C.Name==TEXT("DefaultsBase");});BadDecl->Properties.FindByPredicate([](const auto& P){return P.Name==TEXT("Score");})->bEditable=false;if(!Check(!Bad.Validate(Error),TEXT("noneditable_parent_override_IR_rejected")))return 1;
 }else{
  Base=LoadBP(TEXT("BP_DefaultsBase"));Child=LoadBP(TEXT("BP_DefaultsChild"));Leaf=LoadBP(TEXT("BP_DefaultsLeaf"));Ordinary=LoadBP(TEXT("BP_Ordinary"));W=LoadObject<UWorld>(nullptr,*(Root+TEXT("/Map.Map")));if(!Base || !Child || !Leaf || !Ordinary || !W)return 1;W->WorldType=EWorldType::Editor;if(!W->IsInitialized())W->InitWorld();W->UpdateWorldComponents(true,false);
  for(const auto& Actor:W->PersistentLevel->Actors)if(auto* P=Cast<ADasProbeActor>(Actor)){if(P->Tags.Contains(TEXT("InheritedDefault")))A=P;if(P->Tags.Contains(TEXT("InheritedManual")))M=P;}
 }
 if(!A || !M)return 1;auto* CDO=Child->GeneratedClass->GetDefaultObject();auto* BCDO=Base->GeneratedClass->GetDefaultObject();
 if(!Check(InheritedInt(CDO,TEXT("Score"))==25 && InheritedInt(CDO,TEXT("Manual"))==77 && InheritedInt(BCDO,TEXT("Score"))==12 && InheritedInt(BCDO,TEXT("Keep"))==150 && InheritedInt(CDO,TEXT("Keep"))==150 && InheritedInt(BCDO,TEXT("Frozen"))==12 && InheritedInt(CDO,TEXT("Frozen"))==10,TEXT("parent_child_CDO_isolation")) ||
 !Check(InheritedInt(A,TEXT("Score"))==25 && InheritedInt(M,TEXT("Score"))==111 && InheritedInt(Ordinary->GeneratedClass->GetDefaultObject(),TEXT("Manual"))==55 && InheritedInt(Leaf->GeneratedClass->GetDefaultObject(),TEXT("Score"))==30,TEXT("placed_and_descendant_overrides")) ||
 !Check(InheritedVector(CDO)==FVector(99,5,6) && InheritedVector(M)==FVector(99,222,6),TEXT("inherited_struct_manual_members")) ||
 !Check(InheritedCall(A,TEXT("Read"))==102 && InheritedCall(A,TEXT("BaseRead"))==25 && A->LastDasError.IsEmpty(),TEXT("own_and_parent_entry_runtime_validation")))return 1;
 if(Phase==TEXT("Recompile")){R.ReleaseObject(A);R.ReleaseObject(M);FKismetEditorUtilities::CompileBlueprint(Base);FKismetEditorUtilities::CompileBlueprint(Child);FKismetEditorUtilities::CompileBlueprint(Leaf);auto* Fresh=W->SpawnActor<ADasProbeActor>(Child->GeneratedClass);if(!Check(Fresh && InheritedInt(Fresh,TEXT("Score"))==25 && InheritedInt(Fresh,TEXT("Manual"))==77 && InheritedInt(Fresh,TEXT("Frozen"))==10 && InheritedCall(Fresh,TEXT("Read"))==102 && Fresh->LastDasError.IsEmpty(),TEXT("inherited_defaults_survive_Blueprint_recompile")))return 1;R.ReleaseObject(Fresh);Fresh->Destroy();}
 if(Phase==TEXT("Create")){
  const FString Path=R.ScriptPath(TEXT("dsl_inherited_component_child.das"));FString Final;if(!FFileHelper::LoadFileToString(Final,*Path))return 1;ON_SCOPE_EXIT{Write(Path,Final);};
  if(!Write(Path,Final.Replace(TEXT("self.Bonus=10"),TEXT("self.Bonus=9"))))return 1;UBlueprint* ComponentBP=nullptr;
  if(!Check(FDasDSLService::Apply(TEXT("dsl_inherited_component_child.das"),Root+TEXT("/BP_InheritedComponentChild"),TEXT("inheritedcomponents.InheritedComponentChild"),ComponentBP,Error),TEXT("component_initial_override_import")))return 1;
  FKismetEditorUtilities::CompileBlueprint(ComponentBP);auto* Parent=CastChecked<UBlueprint>(ComponentBP->ParentClass->ClassGeneratedBy);auto* CDOComponent=ComponentBP->GeneratedClass->GetDefaultObject();InheritedSet(CDOComponent,TEXT("Bonus"),19);
  auto* Component=NewObject<UDasScriptComponent>(A,ComponentBP->GeneratedClass);A->AddInstanceComponent(Component);Component->RegisterComponent();InheritedSet(Component,TEXT("Bonus"),55);Component->ComponentTags.Add(TEXT("InheritedComponentFixture"));
  if(!Check(InheritedCall(Component,TEXT("Read"))==55 && Component->LastDasError.IsEmpty(),TEXT("component_runtime_inherited_owner")))return 1;
  if(!Write(Path,Final) || !Check(FDasDSLService::Reimport(ComponentBP,Error) && InheritedInt(CDOComponent,TEXT("Bonus"))==19 && InheritedInt(Component,TEXT("Bonus"))==55 && InheritedInt(Parent->GeneratedClass->GetDefaultObject(),TEXT("Bonus"))==7 && InheritedCall(Component,TEXT("Read"))==55 && Component->LastDasError.IsEmpty(),TEXT("component_reimport_preserves_manual_CDO_and_instance")))return 1;
  auto* Fresh=NewObject<UDasScriptComponent>(A,ComponentBP->GeneratedClass);A->AddInstanceComponent(Fresh);Fresh->RegisterComponent();if(!Check(InheritedInt(Fresh,TEXT("Bonus"))==19 && InheritedCall(Fresh,TEXT("Read"))==19 && Fresh->LastDasError.IsEmpty(),TEXT("component_fresh_instance")))return 1;R.ReleaseObject(Fresh);Fresh->DestroyComponent();R.ReleaseObject(Component);
  if(!InheritedSave(Parent) || !InheritedSave(ComponentBP))return 1;
 }
 if(Phase==TEXT("Create")){auto* PlacedLeaf=W->SpawnActor<ADasProbeActor>(Leaf->GeneratedClass);if(!PlacedLeaf)return 1;PlacedLeaf->Tags.Add(TEXT("InheritedLeaf"));if(!InheritedSave(Base) || !InheritedSave(Child) || !InheritedSave(Leaf) || !InheritedSave(Ordinary) || !InheritedSave(W))return 1;}
 if(Phase!=TEXT("Recompile")){R.ReleaseObject(A);R.ReleaseObject(M);}W->DestroyWorld(false);UE_LOG(LogTemp,Display,TEXT("DAS_INHERITED_DEFAULTS_RESULT=PASS phase=%s"),*Phase);return 0;
}
