#include "DasDSLService.h"
#include "DasAuthoring.h"
#include "DasBlueprintService.h"
#include "DasRuntime.h"
#include "DasProbeActor.h"
#include "Engine/Blueprint.h"
#include "Engine/BlueprintGeneratedClass.h"
#include "Modules/ModuleManager.h"
#include "Misc/Paths.h"
#include "Misc/PackageName.h"
#include "Kismet2/KismetEditorUtilities.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "UObject/MetaData.h"

bool FDasDSLService::Apply(const FString& Script,const FString& Package,const FString& Id,UBlueprint*& Out,FString& Error)
{
    Out=nullptr;Error.Reset();
    if(!IsInGameThread() || FPaths::GetCleanFilename(Script)!=Script || FPaths::GetExtension(Script)!=TEXT("das") || !Package.StartsWith(TEXT("/Game/")) || !FPackageName::IsValidLongPackageName(Package))
    {Error=TEXT("Expected ScriptName.das in plugin Scripts and /Game/Folder/BP_Name on game thread");return false;}
    auto& Runtime=FModuleManager::LoadModuleChecked<FDasRuntimeModule>(TEXT("DasRuntime"));
    FDasAuthoringModule Model;FDasClassSchema Schema;FString Json;
    if(!Runtime.ReadAuthoringScript(Runtime.ScriptPath(Script),Model,Error) || !Model.ToActorSchema(Id,Schema,Error) || !Model.ToJson(Json,Error)) return false;
    for(const auto& Method:Schema.EffectiveMethods()) if(!Runtime.ValidateFrameMethod(Runtime.ScriptPath(Method.ScriptFile),Error,Method.EntryPoint,Method.Contract())) return false;
    const FString ObjectPath=Package+TEXT(".")+FPackageName::GetLongPackageAssetName(Package);
    // A non-Blueprint asset at the requested path must never be replaced.
    UObject* Existing=LoadObject<UObject>(nullptr,*ObjectPath,nullptr,LOAD_NoWarn);
    if(!Existing && FPackageName::DoesPackageExist(Package)){Error=TEXT("Existing package could not be loaded; refusing to create over it");return false;}
    UBlueprint* BP=Cast<UBlueprint>(Existing);
    if(Existing && !BP){Error=TEXT("Asset exists and is not a Blueprint");return false;}
    const bool Created=!BP;
    if(Created)
    {
        UPackage* P=CreatePackage(*Package);
        BP=FKismetEditorUtilities::CreateBlueprint(ADasProbeActor::StaticClass(),P,*FPackageName::GetLongPackageAssetName(Package),BPTYPE_Normal,UBlueprint::StaticClass(),UBlueprintGeneratedClass::StaticClass(),TEXT("DasDSL"));
    }
    if(!BP){Error=TEXT("Cannot create Blueprint");return false;}
    auto& Metadata=BP->GetOutermost()->GetMetaData();
    const bool BodyOnly=!Created && Metadata.GetValue(BP,TEXT("DasDSLModel"))==Json;
    // Validate the current Blueprint even when unchanged declarations avoid reinstancing.
    if(!FDasBlueprintService::Apply(BP,Schema,Error,BodyOnly)) return false;
    BP->Modify();Metadata.SetValue(BP,TEXT("DasDSLSource"),*Script);Metadata.SetValue(BP,TEXT("DasDSLClassId"),*Id);Metadata.SetValue(BP,TEXT("DasDSLModel"),*Json);
    BP->MarkPackageDirty();if(Created) FAssetRegistryModule::AssetCreated(BP);Out=BP;
    UE_LOG(LogTemp,Display,TEXT("DAS_DSL_APPLY PASS %s (%s)"),*ObjectPath,BodyOnly?TEXT("body-only; no Blueprint recompile"):TEXT("declaration applied"));return true;
}
bool FDasDSLService::Reimport(UBlueprint* BP,FString& Error)
{
    if(!IsInGameThread() || !IsValid(BP)){Error=TEXT("Expected valid editor Blueprint");return false;}
    auto& Metadata=BP->GetOutermost()->GetMetaData();
    const FString Source=Metadata.GetValue(BP,TEXT("DasDSLSource")),Id=Metadata.GetValue(BP,TEXT("DasDSLClassId"));
    if(Source.IsEmpty() || Id.IsEmpty()){Error=TEXT("Blueprint has no DSL import source");return false;}
    UBlueprint* Result=nullptr;return Apply(Source,BP->GetOutermost()->GetName(),Id,Result,Error);
}
