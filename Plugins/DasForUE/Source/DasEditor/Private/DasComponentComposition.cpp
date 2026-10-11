#include "DasComponentComposition.h"
#include "DasComponentDefaults.h"
#include "DasDSLService.h"
#include "DasRuntime.h"
#include "DasScriptComponent.h"
#include "DasProbeActor.h"
#include "Engine/Blueprint.h"
#include "Engine/SimpleConstructionScript.h"
#include "Engine/SCS_Node.h"
#include "Engine/InheritableComponentHandler.h"
#include "Components/SceneComponent.h"
#include "Misc/PackageName.h"
#include "UObject/MetaData.h"
#include "Kismet2/BlueprintEditorUtils.h"
bool DasPrepareComponentComposition(const FDasDeclaredClass& D,const FDasDeclaredClass& Effective,const FString& Package,TMap<FString,UBlueprint*>& Components,FString& Error){
 auto& Runtime=FModuleManager::LoadModuleChecked<FDasRuntimeModule>(TEXT("DasRuntime"));struct FSource{FString File,Id,Package,ComponentId;};TArray<FSource> Sources;
 FDasDeclaredClass Preflight=D;
 for(const auto& O:D.ComponentOverrides){const auto* Target=Effective.Components.FindByPredicate([&](const auto& V){return V.Id==O.Id;});if(!Target){Error=TEXT("Missing inherited override target");return false;}auto Component=*Target;Component.Defaults=O.Defaults;Preflight.Components.Add(Component);}
 for(const auto& C:Preflight.Components){if(ADasProbeActor::StaticClass()->FindPropertyByName(*C.Name) || ADasProbeActor::StaticClass()->FindFunctionByName(*C.Name)){Error=TEXT("Component conflicts with inherited member: ")+C.Name;return false;}if(!C.NativeClass.IsEmpty()){auto* Native=FindObject<UClass>(nullptr,*C.NativeClass);if(!Native || !Native->IsChildOf(UActorComponent::StaticClass()) || Native->HasAnyClassFlags(CLASS_Abstract|CLASS_Deprecated|CLASS_NewerVersionExists)){Error=TEXT("Invalid native component class: ")+C.NativeClass;return false;}if(!DasComponentDefaults(Native,C,nullptr,Error))return false;continue;}FDasAuthoringModule Model;if(!Runtime.ReadAuthoringScript(Runtime.ScriptPath(C.ScriptFile),Model,Error))return false;const auto* Type=Model.Classes.FindByPredicate([&](const auto& X){return X.Name==C.ClassName;});FDasDeclaredClass EffectiveType;if(!Type || !Model.ResolveClass(Type->Id,EffectiveType,Error) || EffectiveType.NativeParent!=TEXT("/Script/Engine.ActorComponent") || !EffectiveType.Components.IsEmpty()){Error=TEXT("Component type must inherit authored ActorComponent: ")+C.ClassName;return false;}FDasAuthoringModule DefaultsModel=Model;
for(const auto& Pair:C.Defaults){FDasDeclaredProperty* Property=nullptr;FString DefaultsId=Type->Id;for(int32 Depth=0;Depth<32 && !DefaultsId.IsEmpty();++Depth){auto* DefaultsClass=DefaultsModel.Classes.FindByPredicate([&](const auto& X){return X.Id==DefaultsId;});if(!DefaultsClass)break;Property=DefaultsClass->Properties.FindByPredicate([&](const auto& X){return X.Name==Pair.Key;});if(Property)break;DefaultsId=DefaultsClass->ParentId;}if(!Property || !Property->bEditable || (Property->Type.Kind!=TEXT("int") && Property->Type.Kind!=TEXT("float") && Property->Type.Kind!=TEXT("double") && Property->Type.Kind!=TEXT("bool") && Property->Type.Kind!=TEXT("string") && Property->Type.Kind!=TEXT("name"))){Error=TEXT("Authored component defaults require editable scalar fields: ")+C.Name+TEXT(".")+Pair.Key;return false;}Property->Default=Pair.Value;}
if(!DefaultsModel.Validate(Error))return false;FDasClassSchema Schema;if(!Model.ToActorSchema(Type->Id,Schema,Error))return false;for(const auto& M:Schema.EffectiveMethods())if(!Runtime.ValidateFrameMethod(Runtime.ScriptPath(M.ScriptFile),Error,M.EntryPoint,M.Contract()))return false;const FString ChildPackage=FPackageName::GetLongPackagePath(Package)+TEXT("/BP_")+Type->Name;if(ChildPackage.Equals(Package,ESearchCase::IgnoreCase)){Error=TEXT("Component asset path conflicts with Actor package");return false;}const FString ChildPath=ChildPackage+TEXT(".")+FPackageName::GetLongPackageAssetName(ChildPackage);UObject* Existing=LoadObject<UObject>(nullptr,*ChildPath,nullptr,LOAD_NoWarn);if(Existing){auto* Child=Cast<UBlueprint>(Existing);if(!Child || Child->GetOutermost()->GetMetaData().GetValue(Child,TEXT("DasDSLClassId"))!=Type->Id || Child->GetOutermost()->GetMetaData().GetValue(Child,TEXT("DasDSLSource"))!=C.ScriptFile){Error=TEXT("Component asset path is occupied by a different asset: ")+ChildPath;return false;}}else if(FPackageName::DoesPackageExist(ChildPackage)){Error=TEXT("Existing component package cannot be loaded");return false;}for(const auto& S:Sources)if(S.Package.Equals(ChildPackage,ESearchCase::IgnoreCase) && (S.Id!=Type->Id || S.File!=C.ScriptFile)){Error=TEXT("Component asset name conflict: ")+ChildPackage;return false;}Sources.Add({C.ScriptFile,Type->Id,ChildPackage,C.Id});}
 for(const auto& C:D.Components){
  auto* Type=C.NativeClass.IsEmpty()?nullptr:FindObject<UClass>(nullptr,*C.NativeClass);
  if((C.bRoot || !C.Attach.IsEmpty() || !C.AttachSocket.IsEmpty()) && (!Type || !Type->IsChildOf(USceneComponent::StaticClass()))){Error=TEXT("Root/attachment requires native SceneComponent: ")+C.Name;return false;}
  if(!C.Attach.IsEmpty()){const auto* Parent=Effective.Components.FindByPredicate([&](const auto& V){return V.Name==C.Attach;});auto* ParentType=Parent?FindObject<UClass>(nullptr,*Parent->NativeClass):nullptr;if(!ParentType || !ParentType->IsChildOf(USceneComponent::StaticClass())){Error=TEXT("Attachment parent must be a scene component");return false;}}
 }
 for(int32 I=0;I<Sources.Num();++I){const auto& S=Sources[I];UBlueprint* BP=nullptr;if(!FDasDSLService::Apply(S.File,S.Package,S.Id,BP,Error))return false;Components.Add(S.ComponentId,BP);}return true;
}
bool DasApplyComponentComposition(UBlueprint* BP,const FDasDeclaredClass& D,const FDasDeclaredClass& Effective,const TMap<FString,UBlueprint*>& Components,bool Create,FString& Error){
 if(D.Components.IsEmpty() && D.ComponentOverrides.IsEmpty())return true;if(!BP->SimpleConstructionScript){Error=TEXT("Actor Blueprint has no construction script");return false;}
 for(const auto& C:D.Components){auto* Child=Components.FindRef(C.Id);UClass* ComponentClass=C.NativeClass.IsEmpty()?(Child?Child->GeneratedClass.Get():nullptr):FindObject<UClass>(nullptr,*C.NativeClass);if(!ComponentClass || !ComponentClass->IsChildOf(UActorComponent::StaticClass()) || ComponentClass->HasAnyClassFlags(CLASS_Abstract|CLASS_Deprecated|CLASS_NewerVersionExists)){Error=TEXT("Component Blueprint is unavailable");return false;}USCS_Node* Existing=nullptr;for(auto* Node:BP->SimpleConstructionScript->GetAllNodes())if(Node->GetVariableName()==FName(*C.Name))Existing=Node;
  if(!Create){if(!Existing || Existing->ComponentClass!=ComponentClass || (Existing->FindMetaDataEntryIndexForKey(TEXT("DasComponentId"))==INDEX_NONE || Existing->GetMetaData(TEXT("DasComponentId"))!=C.Id)){Error=TEXT("Component composition differs from imported Blueprint: ")+C.Name;return false;}continue;}
  if(Existing || BP->GeneratedClass->FindPropertyByName(*C.Name) || BP->GeneratedClass->FindFunctionByName(*C.Name)){Error=TEXT("Component name conflicts with Blueprint member: ")+C.Name;return false;}auto* Node=BP->SimpleConstructionScript->CreateNode(ComponentClass,FName(*C.Name));if(!DasComponentDefaults(ComponentClass,C,Node->ComponentTemplate,Error))return false;Node->SetMetaData(TEXT("DasComponentId"),C.Id);Node->SetMetaData(TEXT("DasComponentReadOnly"),C.bReadOnly?TEXT("true"):TEXT("false"));Node->SetMetaData(TEXT("DasComponentEditable"),C.bEditable?TEXT("true"):TEXT("false"));Node->SetMetaData(TEXT("DasComponentVisible"),C.bVisible?TEXT("true"):TEXT("false"));for(const auto& Pair:C.Metadata)Node->SetMetaData(FName(*Pair.Key),Pair.Value);if(const auto* Category=C.Metadata.Find(TEXT("Category")))Node->CategoryName=FText::FromString(*Category);BP->SimpleConstructionScript->AddNode(Node);
 }
 if(Create){
  const auto* Root=Effective.Components.FindByPredicate([](const auto& C){return C.bRoot;});
  for(const auto& C:D.Components){
   const FString ParentName=!C.Attach.IsEmpty()?C.Attach:Root && Root->Name!=C.Name?Root->Name:FString();
   if(ParentName.IsEmpty())continue;
   USCS_Node* Child=nullptr;USCS_Node* Parent=nullptr;
   for(auto* Node:BP->SimpleConstructionScript->GetAllNodes()){if(Node->GetVariableName()==FName(*C.Name))Child=Node;if(Node->GetVariableName()==FName(*ParentName))Parent=Node;}
   if(Child && !Parent && Child->ComponentClass->IsChildOf(USceneComponent::StaticClass())){
    for(auto* Ancestor=BP->ParentClass.Get();Ancestor && !Parent;Ancestor=Ancestor->GetSuperClass())if(auto* ParentBP=Cast<UBlueprint>(Ancestor->ClassGeneratedBy))if(ParentBP->SimpleConstructionScript)for(auto* Node:ParentBP->SimpleConstructionScript->GetAllNodes())if(Node->GetVariableName()==FName(*ParentName)){Parent=Node;break;}
    if(!Parent){Error=TEXT("Inherited attachment node is unavailable: ")+ParentName;return false;}
    // Cross-Blueprint parent references must retain the node in the child's SCS.
    Child->SetParent(Parent);Child->AttachToName=FName(*C.AttachSocket);continue;
   }
   if(Child && Parent && Child->ComponentClass->IsChildOf(USceneComponent::StaticClass())){BP->SimpleConstructionScript->RemoveNode(Child,false);Parent->AddChildNode(Child);Child->AttachToName=FName(*C.AttachSocket);}
  }
 }
 for(const auto& O:D.ComponentOverrides){
  USCS_Node* Original=nullptr;
  for(auto* Ancestor=BP->ParentClass.Get();Ancestor && !Original;Ancestor=Ancestor->GetSuperClass())if(auto* ParentBP=Cast<UBlueprint>(Ancestor->ClassGeneratedBy))if(ParentBP->SimpleConstructionScript)for(auto* Node:ParentBP->SimpleConstructionScript->GetAllNodes())if(Node->GetVariableName()==FName(*O.Name) && Node->FindMetaDataEntryIndexForKey(TEXT("DasComponentId"))!=INDEX_NONE && Node->GetMetaData(TEXT("DasComponentId"))==O.Id){Original=Node;break;}
  if(!Original){Error=TEXT("Inherited component node is unavailable: ")+O.Name;return false;}
  const FComponentKey Key(Original);auto* Handler=BP->GetInheritableComponentHandler(Create);
  auto* Template=Handler?Handler->GetOverridenComponentTemplate(Key):nullptr;
  if(Create && !Template)Template=Handler?Handler->CreateOverridenComponentTemplate(Key):nullptr;
  FDasDeclaredComponent Defaults;Defaults.Name=O.Name;Defaults.Defaults=O.Defaults;
  if(!Template || !Template->IsA(Original->ComponentClass) || !DasComponentDefaults(Original->ComponentClass,Defaults,Create?Template:nullptr,Error)){if(Error.IsEmpty())Error=TEXT("Inherited component override template is unavailable: ")+O.Name;return false;}
 }
 if(Create)FBlueprintEditorUtils::MarkBlueprintAsStructurallyModified(BP);
 return true;
}
