#include "DasComponentComposition.h"
#include "DasDSLService.h"
#include "DasRuntime.h"
#include "DasScriptComponent.h"
#include "DasProbeActor.h"
#include "Engine/Blueprint.h"
#include "Engine/SimpleConstructionScript.h"
#include "Engine/SCS_Node.h"
#include "Misc/PackageName.h"
#include "UObject/MetaData.h"
#include "Kismet2/BlueprintEditorUtils.h"
bool DasPrepareComponentComposition(const FDasDeclaredClass& D,const FString& Package,TMap<FString,UBlueprint*>& Components,FString& Error){
 auto& Runtime=FModuleManager::LoadModuleChecked<FDasRuntimeModule>(TEXT("DasRuntime"));struct FSource{FString File,Id,Package;};TArray<FSource> Sources;
 for(const auto& C:D.Components){if(ADasProbeActor::StaticClass()->FindPropertyByName(*C.Name) || ADasProbeActor::StaticClass()->FindFunctionByName(*C.Name)){Error=TEXT("Component conflicts with inherited member: ")+C.Name;return false;}FDasAuthoringModule Model;if(!Runtime.ReadAuthoringScript(Runtime.ScriptPath(C.ScriptFile),Model,Error))return false;const auto* Type=Model.Classes.FindByPredicate([&](const auto& X){return X.Name==C.ClassName;});if(!Type || Type->NativeParent!=TEXT("/Script/Engine.ActorComponent") || !Type->Components.IsEmpty()){Error=TEXT("Component type must be a direct authored ActorComponent: ")+C.ClassName;return false;}FDasClassSchema Schema;if(!Model.ToActorSchema(Type->Id,Schema,Error))return false;for(const auto& M:Schema.EffectiveMethods())if(!Runtime.ValidateFrameMethod(Runtime.ScriptPath(M.ScriptFile),Error,M.EntryPoint,M.Contract()))return false;const FString ChildPackage=FPackageName::GetLongPackagePath(Package)+TEXT("/BP_")+Type->Name;if(ChildPackage.Equals(Package,ESearchCase::IgnoreCase)){Error=TEXT("Component asset path conflicts with Actor package");return false;}const FString ChildPath=ChildPackage+TEXT(".")+FPackageName::GetLongPackageAssetName(ChildPackage);UObject* Existing=LoadObject<UObject>(nullptr,*ChildPath,nullptr,LOAD_NoWarn);if(Existing){auto* Child=Cast<UBlueprint>(Existing);if(!Child || Child->GetOutermost()->GetMetaData().GetValue(Child,TEXT("DasDSLClassId"))!=Type->Id || Child->GetOutermost()->GetMetaData().GetValue(Child,TEXT("DasDSLSource"))!=C.ScriptFile){Error=TEXT("Component asset path is occupied by a different asset: ")+ChildPath;return false;}}else if(FPackageName::DoesPackageExist(ChildPackage)){Error=TEXT("Existing component package cannot be loaded");return false;}for(const auto& S:Sources)if(S.Package.Equals(ChildPackage,ESearchCase::IgnoreCase) && (S.Id!=Type->Id || S.File!=C.ScriptFile)){Error=TEXT("Component asset name conflict: ")+ChildPackage;return false;}Sources.Add({C.ScriptFile,Type->Id,ChildPackage});}
 for(int32 I=0;I<Sources.Num();++I){const auto& S=Sources[I];UBlueprint* BP=nullptr;if(!FDasDSLService::Apply(S.File,S.Package,S.Id,BP,Error))return false;Components.Add(D.Components[I].Id,BP);}return true;
}
bool DasApplyComponentComposition(UBlueprint* BP,const FDasDeclaredClass& D,const TMap<FString,UBlueprint*>& Components,bool Create,FString& Error){
 if(D.Components.IsEmpty())return true;if(!BP->SimpleConstructionScript){Error=TEXT("Actor Blueprint has no construction script");return false;}
 for(const auto& C:D.Components){auto* Child=Components.FindRef(C.Id);if(!Child || !Child->GeneratedClass || !Child->GeneratedClass->IsChildOf(UDasScriptComponent::StaticClass())){Error=TEXT("Component Blueprint is unavailable");return false;}USCS_Node* Existing=nullptr;for(auto* Node:BP->SimpleConstructionScript->GetAllNodes())if(Node->GetVariableName()==FName(*C.Name))Existing=Node;
  if(!Create){if(!Existing || Existing->ComponentClass!=Child->GeneratedClass || (Existing->FindMetaDataEntryIndexForKey(TEXT("DasComponentId"))==INDEX_NONE || Existing->GetMetaData(TEXT("DasComponentId"))!=C.Id)){Error=TEXT("Component composition differs from imported Blueprint: ")+C.Name;return false;}continue;}
  if(Existing || BP->GeneratedClass->FindPropertyByName(*C.Name) || BP->GeneratedClass->FindFunctionByName(*C.Name)){Error=TEXT("Component name conflicts with Blueprint member: ")+C.Name;return false;}auto* Node=BP->SimpleConstructionScript->CreateNode(Child->GeneratedClass,FName(*C.Name));Node->SetMetaData(TEXT("DasComponentId"),C.Id);Node->SetMetaData(TEXT("DasComponentReadOnly"),C.bReadOnly?TEXT("true"):TEXT("false"));Node->SetMetaData(TEXT("DasComponentEditable"),C.bEditable?TEXT("true"):TEXT("false"));Node->SetMetaData(TEXT("DasComponentVisible"),C.bVisible?TEXT("true"):TEXT("false"));for(const auto& Pair:C.Metadata)Node->SetMetaData(FName(*Pair.Key),Pair.Value);if(const auto* Category=C.Metadata.Find(TEXT("Category")))Node->CategoryName=FText::FromString(*Category);BP->SimpleConstructionScript->AddNode(Node);
 }
 if(Create)FBlueprintEditorUtils::MarkBlueprintAsStructurallyModified(BP);return true;
}
