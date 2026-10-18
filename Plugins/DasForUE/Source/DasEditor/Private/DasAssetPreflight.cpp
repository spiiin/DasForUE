#include "DasAssetPreflight.h"
#include "DasDataAssets.h"
#include "DasAuthoring.h"
#include "DasRuntime.h"
#include "DasScriptHosts.h"
#include "DasBlueprintService.h"
#include "DasSoftReferences.h"
#include "Engine/Blueprint.h"
#include "Engine/AssetManager.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "Interfaces/IPluginManager.h"
#include "Misc/Paths.h"
#include "Misc/PackageName.h"
#include "HAL/FileManager.h"
#include "UObject/Package.h"
#include "UObject/MetaData.h"
#include "UObject/UnrealType.h"
#include "Dom/JsonObject.h"
#include "Serialization/JsonSerializer.h"
#include "Serialization/JsonWriter.h"

bool FDasAssetPreflightReport::HasErrors()const{return Issues.ContainsByPredicate([](const auto& I){return I.Severity==TEXT("error");});}
bool FDasAssetPreflightReport::HasIssue(const FString& Code)const{return Issues.ContainsByPredicate([&](const auto& I){return I.Code==Code;});}
FString FDasAssetPreflightReport::ToJson()const{
 auto J=MakeShared<FJsonObject>();J->SetStringField(TEXT("asset"),Asset);J->SetStringField(TEXT("primary_id"),PrimaryId);J->SetBoolField(TEXT("passed_preflight"),!HasErrors());J->SetBoolField(TEXT("packaged_verified"),false);J->SetStringField(TEXT("staging_scope"),TEXT("saved_class_sources_and_method_entries_in_standard_UFS_roots"));
 auto Strings=[&](const TCHAR* Key,const TArray<FString>& A){TArray<TSharedPtr<FJsonValue>> Values;for(const auto& S:A)Values.Add(MakeShared<FJsonValueString>(S));J->SetArrayField(Key,Values);};Strings(TEXT("packages"),Packages);Strings(TEXT("scripts"),Scripts);
 TArray<TSharedPtr<FJsonValue>> List;for(const auto& I:Issues){auto V=MakeShared<FJsonObject>();V->SetStringField(TEXT("severity"),I.Severity);V->SetStringField(TEXT("code"),I.Code);V->SetStringField(TEXT("resource"),I.Resource);V->SetStringField(TEXT("message"),I.Message);List.Add(MakeShared<FJsonValueObject>(V));}J->SetArrayField(TEXT("issues"),List);FString Text;auto W=TJsonWriterFactory<>::Create(&Text);FJsonSerializer::Serialize(J,W);return Text;
}
FDasAssetPreflightReport FDasAssetPreflight::Inspect(UDataAsset* Asset){
 FDasAssetPreflightReport Out;
 auto Issue=[&](const TCHAR* Severity,const TCHAR* Code,const FString& Resource,const FString& Message){Out.Issues.Add({Severity,Code,Resource,Message});};
 if(!IsInGameThread() || !IsValid(Asset) || Asset->IsTemplate() || (!Asset->IsA<UDasDataAsset>() && !Asset->IsA<UDasPrimaryDataAsset>())){Issue(TEXT("error"),TEXT("invalid_asset"),TEXT(""),TEXT("Expected an authored DataAsset instance on the game thread"));return Out;}
 Out.Asset=Asset->GetPathName();auto& Runtime=FModuleManager::LoadModuleChecked<FDasRuntimeModule>(TEXT("DasRuntime"));
 auto Package=[&](UPackage* P,const TCHAR* Missing,const TCHAR* Dirty){const FString Name=P->GetName();Out.Packages.AddUnique(Name);if(!FPackageName::DoesPackageExist(Name))Issue(TEXT("error"),Missing,Name,TEXT("Save this package before cooking or restarting the editor"));else if(P->IsDirty())Issue(TEXT("error"),Dirty,Name,TEXT("Disk package does not contain the current edits; save it"));};
 Package(Asset->GetOutermost(),TEXT("asset_not_saved"),TEXT("asset_unsaved_changes"));
 const auto Plugin=IPluginManager::Get().FindPlugin(TEXT("DasForUE"));const FString PluginRoot=Plugin.IsValid()?FPaths::ConvertRelativePathToFull(Plugin->GetContentDir()/TEXT("Das/Scripts")):FString();
 const FString ProjectRoot=FPaths::ConvertRelativePathToFull(FPaths::ProjectContentDir()/TEXT("Das/Scripts"));
 auto Source=[&](const FString& Name){
  if(FPaths::GetCleanFilename(Name)!=Name || FPaths::GetExtension(Name)!=TEXT("das")){Issue(TEXT("error"),TEXT("invalid_script_name"),Name,TEXT("Expected a flat .das source name"));return false;}
  const FString Path=FPaths::ConvertRelativePathToFull(Runtime.ScriptPath(Name));Out.Scripts.AddUnique(Path);
  if(!IFileManager::Get().FileExists(*Path)){Issue(TEXT("error"),TEXT("script_missing"),Path,TEXT("Restore the DAS file used by the saved class"));return false;}
  if(!FPaths::IsUnderDirectory(Path,ProjectRoot) && (PluginRoot.IsEmpty() || !FPaths::IsUnderDirectory(Path,PluginRoot))){Issue(TEXT("error"),TEXT("script_outside_staging_roots"),Path,TEXT("Move the source into project or plugin Content/Das/Scripts"));return false;}
  return true;
 };
 for(UClass* Class=Asset->GetClass();Class && !Class->HasAnyClassFlags(CLASS_Native);Class=Class->GetSuperClass()){
  auto* BP=Cast<UBlueprint>(Class->ClassGeneratedBy);if(!BP){Issue(TEXT("error"),TEXT("class_blueprint_missing"),Class->GetPathName(),TEXT("Generated class has no editable Blueprint source"));continue;}
  if(Class->HasAnyClassFlags(CLASS_Deprecated|CLASS_NewerVersionExists) || BP->GeneratedClass!=Class){Issue(TEXT("error"),TEXT("class_stale"),Class->GetPathName(),TEXT("Asset refers to an obsolete generated class; resolve/reinstance before cooking"));continue;}
  Package(BP->GetOutermost(),TEXT("class_not_saved"),TEXT("class_unsaved_changes"));
  auto& Meta=BP->GetOutermost()->GetMetaData();const FString File=Meta.GetValue(BP,TEXT("DasDSLSource")),Id=Meta.GetValue(BP,TEXT("DasDSLClassId")),Saved=Meta.GetValue(BP,TEXT("DasDSLModel"));
  if(Id.IsEmpty() || Saved.IsEmpty() || DasHostIdentity(Class->GetDefaultObject())!=Id){Issue(TEXT("error"),TEXT("authored_identity_mismatch"),Class->GetPathName(),TEXT("Reimport the authored class; saved identity/model is missing or different"));continue;}
  if(!Source(File))continue;
  FDasAuthoringModule Current;FString Error,Wire;
  if(!Runtime.ReadAuthoringScript(Runtime.ScriptPath(File),Current,Error) || !Current.ToJson(Wire,Error)){Issue(TEXT("error"),TEXT("script_compile_failed"),File,Error);continue;}
  if(Wire!=Saved){Issue(TEXT("error"),TEXT("class_declaration_stale"),BP->GetPathName(),TEXT("DAS declaration differs from the saved Blueprint; resolve/reimport before cooking"));continue;}
  FDasClassSchema Schema;
  if(!Current.ToActorSchema(Id,Schema,Error) || !FDasBlueprintService::Apply(BP,Schema,Error,true))Issue(TEXT("error"),TEXT("class_schema_mismatch"),BP->GetPathName(),Error);
  if(auto* CDO=Cast<UDasPrimaryDataAsset>(Class->GetDefaultObject())){const FString Expected=Schema.Metadata.Contains(TEXT("PrimaryAssetType"))?Schema.Metadata.FindRef(TEXT("PrimaryAssetType")):Schema.Name.ToString();if(CDO->DasPrimaryAssetType!=FName(*Expected))Issue(TEXT("error"),TEXT("primary_type_mismatch"),Class->GetPathName(),TEXT("Serialized primary type differs from the source declaration"));}
  for(const auto& Field:Schema.Fields)if(auto* Property=Class->FindPropertyByName(Field.Name))if(Property->GetMetaData(TEXT("AssetBundles"))!=Field.Metadata.FindRef(TEXT("AssetBundles")))Issue(TEXT("error"),TEXT("class_bundle_metadata_mismatch"),Class->GetPathName()+TEXT(".")+Field.Name.ToString(),TEXT("Reflected AssetBundles metadata differs from the DAS declaration"));
  for(const auto& C:Current.Classes){if(!C.SourceScript.IsEmpty())Source(C.SourceScript);for(const auto& Method:C.Methods)Source(Method.ScriptFile);}
 }
 if(auto* Primary=Cast<UDasPrimaryDataAsset>(Asset)){
  if(Primary->DasPrimaryAssetType!=CastChecked<UDasPrimaryDataAsset>(Primary->GetClass()->GetDefaultObject())->DasPrimaryAssetType)Issue(TEXT("error"),TEXT("primary_instance_type_mismatch"),Out.Asset,TEXT("Instance primary type differs from its class default"));
  auto* Manager=UAssetManager::GetIfInitialized();const auto Id=Primary->GetPrimaryAssetId();Out.PrimaryId=Id.ToString();
  if(!Id.IsValid()){Issue(TEXT("error"),TEXT("primary_id_invalid"),Out.Asset,TEXT("Import a PrimaryDataAsset class with a valid primary_asset_type"));return Out;}
  if(!Manager){Issue(TEXT("error"),TEXT("assetmanager_unavailable"),Out.Asset,TEXT("AssetManager is not initialized"));return Out;}
  FPrimaryAssetTypeInfo Info;
  const bool Registered=Manager->GetPrimaryAssetTypeInfo(Id.PrimaryAssetType,Info) && Manager->GetPrimaryAssetPath(Id)==FSoftObjectPath(Asset);
  if(!Registered)Issue(TEXT("error"),TEXT("primary_not_registered"),Id.ToString(),TEXT("Configure AssetManager type/base class/directories for data instances, save and rescan"));
  else if(Info.bHasBlueprintClasses || Info.bIsEditorOnly)Issue(TEXT("error"),TEXT("primary_type_wrong_scan_mode"),Id.ToString(),TEXT("Runtime data instances require HasBlueprintClasses=false and IsEditorOnly=false"));
  const auto Rules=Manager->GetPrimaryAssetRules(Id);
  if(Rules.CookRule==EPrimaryAssetCookRule::NeverCook)Issue(TEXT("error"),TEXT("asset_never_cook"),Id.ToString(),TEXT("Effective Primary Asset cook rule is NeverCook"));
  else if(Rules.CookRule!=EPrimaryAssetCookRule::AlwaysCook)Issue(TEXT("warning"),TEXT("cook_inclusion_unproven"),Id.ToString(),TEXT("No AlwaysCook guarantee; verify inclusion via your map, label, manager or target cook rules"));
  bool ValidPaths=true;for(TPropertyValueIterator<const FProperty> I(Asset->GetClass(),Asset);I;++I)if(auto* P=CastField<const FSoftObjectProperty>(I.Key())){const auto Path=P->GetPropertyValue(I.Value()).ToSoftObjectPath();FString Error;if(!ValidateDasSoftPath(Path.ToString(),Error)){Issue(TEXT("error"),TEXT("bundle_soft_path_invalid"),P->GetName(),Error);ValidPaths=false;}}
  if(ValidPaths){
   FAssetBundleData Expected;Manager->InitializeAssetBundlesFromMetadata(Asset,Expected);TArray<FAssetBundleEntry> Saved;Manager->GetAssetBundleEntries(Id,Saved);
   bool Same=Expected.Bundles.Num()==Saved.Num();
   for(const auto& Entry:Expected.Bundles){const auto* Old=Saved.FindByPredicate([&](const auto& E){return E.BundleName==Entry.BundleName;});if(!Old || Old->AssetPaths.Num()!=Entry.AssetPaths.Num())Same=false;else for(const auto& Path:Entry.AssetPaths)if(!Old->AssetPaths.Contains(Path))Same=false;
    for(const auto& Path:Entry.AssetPaths){const FString Name=Path.GetPackageName().ToString();Out.Packages.AddUnique(Name);if(!FPackageName::DoesPackageExist(Name))Issue(TEXT("error"),TEXT("bundle_package_missing"),Path.ToString(),TEXT("Bundle dependency has no saved package"));}
   }
   if(Registered && !Same)Issue(TEXT("error"),TEXT("bundle_registry_stale"),Out.Asset,TEXT("Live bundle references differ from AssetManager registry data; save the asset and refresh registry data"));
  }
 }
 if(!Asset->IsA<UDasPrimaryDataAsset>())Issue(TEXT("warning"),TEXT("cook_inclusion_unproven"),Out.Asset,TEXT("Ordinary DataAsset cook inclusion depends on map/label references or project cook settings"));
 Out.Packages.Sort();Out.Scripts.Sort();return Out;
}
