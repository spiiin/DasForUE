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
#include "UObject/UObjectHash.h"
#include "UObject/UnrealType.h"
#include "UObject/StrProperty.h"
#include "UObject/StrongObjectPtr.h"
#include "Misc/ScopeExit.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "BlueprintCompilationManager.h"
#include "DasCallFrame.h"
#include "Engine/World.h"
#include "Engine/Level.h"
#include "AssetRegistry/AssetData.h"
#include "EdGraphSchema_K2.h"
#include "K2Node.h"
#include "DasNumericGraphMigration.h"
#if WITH_DEV_AUTOMATION_TESTS
static bool GDasFailWidenAfterCompile=false;
void DasFailWidenAfterCompileForTesting(bool Enabled){GDasFailWidenAfterCompile=Enabled;}
#endif

bool FDasDSLService::Apply(const FString& Script,const FString& Package,const FString& Id,UBlueprint*& Out,FString& Error,EDasDefaultMigrationPolicy DefaultPolicy)
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
    if(DefaultPolicy!=EDasDefaultMigrationPolicy::Refuse){
        if(DefaultPolicy!=EDasDefaultMigrationPolicy::PreserveInstances || Created){Error=TEXT("Default migration requires an existing imported Blueprint and a known policy");return false;}
        FDasAuthoringModule Prior;
        if(!FDasAuthoringModule::FromJson(BP->GetOutermost()->GetMetaData().GetValue(BP,TEXT("DasDSLModel")),Prior,Error))return false;
        const auto* Old=Prior.Classes.FindByPredicate([&](const auto& C){return C.Id==Id;});const auto* Next=Model.Classes.FindByPredicate([&](const auto& C){return C.Id==Id;});
        if(!Old || !Next || Prior.Id!=Model.Id){Error=TEXT("Default migration requires matching stored source identity");return false;}
        const auto Delta=FDasAuthoringModule::Compare(*Old,*Next);
        if(Delta.bStructural || Delta.bMetadata || Delta.bImplementation){Error=TEXT("Default migration accepts only default changes; apply other declaration changes separately");return false;}
        FDasClassSchema Previous;if(!Prior.ToActorSchema(Id,Previous,Error) || !FDasBlueprintService::Apply(BP,Previous,Error,true))return false;
    }
    if(Created)
    {
        UPackage* P=CreatePackage(*Package);
        BP=FKismetEditorUtilities::CreateBlueprint(ADasProbeActor::StaticClass(),P,*FPackageName::GetLongPackageAssetName(Package),BPTYPE_Normal,UBlueprint::StaticClass(),UBlueprintGeneratedClass::StaticClass(),TEXT("DasDSL"));
    }
    if(!BP){Error=TEXT("Cannot create Blueprint");return false;}
    auto& Metadata=BP->GetOutermost()->GetMetaData();
    const bool BodyOnly=!Created && Metadata.GetValue(BP,TEXT("DasDSLModel"))==Json;
    // Validate the current Blueprint even when unchanged declarations avoid reinstancing.
    if(!FDasBlueprintService::Apply(BP,Schema,Error,BodyOnly,DefaultPolicy)) return false;
    BP->Modify();Metadata.SetValue(BP,TEXT("DasDSLSource"),*Script);Metadata.SetValue(BP,TEXT("DasDSLClassId"),*Id);Metadata.SetValue(BP,TEXT("DasDSLModel"),*Json);
    BP->MarkPackageDirty();if(Created) FAssetRegistryModule::AssetCreated(BP);Out=BP;
    UE_LOG(LogTemp,Display,TEXT("DAS_DSL_APPLY PASS %s (%s)"),*ObjectPath,BodyOnly?TEXT("body-only; no Blueprint recompile"):TEXT("declaration applied"));return true;
}
bool FDasDSLService::Reimport(UBlueprint* BP,FString& Error,EDasDefaultMigrationPolicy DefaultPolicy)
{
    if(!IsInGameThread() || !IsValid(BP)){Error=TEXT("Expected valid editor Blueprint");return false;}
    auto& Metadata=BP->GetOutermost()->GetMetaData();
    const FString Source=Metadata.GetValue(BP,TEXT("DasDSLSource")),Id=Metadata.GetValue(BP,TEXT("DasDSLClassId"));
    if(Source.IsEmpty() || Id.IsEmpty()){Error=TEXT("Blueprint has no DSL import source");return false;}
    UBlueprint* Result=nullptr;return Apply(Source,BP->GetOutermost()->GetName(),Id,Result,Error,DefaultPolicy);
}

namespace {
FDasScalarValue CaptureRenamedField(UObject* Object,FName Name,EDasScalarKind Kind){
    FDasScalarValue V;V.Kind=static_cast<int32>(Kind);auto* P=Object->GetClass()->FindPropertyByName(Name);
    switch(Kind){
    case EDasScalarKind::Int32:V.IntValue=CastFieldChecked<FIntProperty>(P)->GetPropertyValue_InContainer(Object);break;
    case EDasScalarKind::Float32:V.FloatValue=CastFieldChecked<FFloatProperty>(P)->GetPropertyValue_InContainer(Object);break;
    case EDasScalarKind::Bool:V.BoolValue=CastFieldChecked<FBoolProperty>(P)->GetPropertyValue_InContainer(Object);break;
    case EDasScalarKind::String:V.StringValue=CastFieldChecked<FStrProperty>(P)->GetPropertyValue_InContainer(Object);break;
    case EDasScalarKind::Double:V.DoubleValue=CastFieldChecked<FDoubleProperty>(P)->GetPropertyValue_InContainer(Object);break;
    case EDasScalarKind::Name:V.NameValue=CastFieldChecked<FNameProperty>(P)->GetPropertyValue_InContainer(Object);break;
    case EDasScalarKind::Struct:V.StructValue.InitializeAs(CastFieldChecked<FStructProperty>(P)->Struct,P->ContainerPtrToValuePtr<uint8>(Object));break;
    }return V;
}
bool RestoreRenamedField(UObject* Object,FName Name,const FDasScalarValue& V){
    auto* P=Object?Object->GetClass()->FindPropertyByName(Name):nullptr;if(!P)return false;
    switch(static_cast<EDasScalarKind>(V.Kind)){
    case EDasScalarKind::Int32:if(auto* F=CastField<FIntProperty>(P)){F->SetPropertyValue_InContainer(Object,V.IntValue);return true;}break;
    case EDasScalarKind::Float32:if(auto* F=CastField<FFloatProperty>(P)){F->SetPropertyValue_InContainer(Object,V.FloatValue);return true;}break;
    case EDasScalarKind::Bool:if(auto* F=CastField<FBoolProperty>(P)){F->SetPropertyValue_InContainer(Object,V.BoolValue);return true;}break;
    case EDasScalarKind::String:if(auto* F=CastField<FStrProperty>(P)){F->SetPropertyValue_InContainer(Object,V.StringValue);return true;}break;
    case EDasScalarKind::Double:if(auto* F=CastField<FDoubleProperty>(P)){F->SetPropertyValue_InContainer(Object,V.DoubleValue);return true;}break;
    case EDasScalarKind::Name:if(auto* F=CastField<FNameProperty>(P)){F->SetPropertyValue_InContainer(Object,V.NameValue);return true;}break;
    case EDasScalarKind::Struct:if(auto* F=CastField<FStructProperty>(P))if(F->Struct==V.StructValue.GetScriptStruct()){F->Struct->CopyScriptStruct(P->ContainerPtrToValuePtr<void>(Object),V.StructValue.GetMemory());return true;}break;
    }return false;
}
}
bool FDasDSLService::ReimportRenamedFields(UBlueprint* BP,FString& Error,bool IncludeDependencies){
    Error.Reset();if(!IsInGameThread() || !IsValid(BP) || !BP->GeneratedClass){Error=TEXT("Expected valid imported Blueprint on game thread");return false;}
    auto& Runtime=FModuleManager::LoadModuleChecked<FDasRuntimeModule>(TEXT("DasRuntime"));auto& Meta=BP->GetOutermost()->GetMetaData();
    const FString Script=Meta.GetValue(BP,TEXT("DasDSLSource")),Id=Meta.GetValue(BP,TEXT("DasDSLClassId"));
    if(Script.IsEmpty() || FPaths::GetCleanFilename(Script)!=Script){Error=TEXT("Blueprint has no local DSL source");return false;}
    FDasAuthoringModule Prior,Next;FDasClassSchema Previous,Candidate;FString Json;
    if(!FDasAuthoringModule::FromJson(Meta.GetValue(BP,TEXT("DasDSLModel")),Prior,Error) || !Runtime.ReadAuthoringScript(Runtime.ScriptPath(Script),Next,Error) || !Prior.ToActorSchema(Id,Previous,Error) || !Next.ToActorSchema(Id,Candidate,Error) || !Next.ToJson(Json,Error))return false;
    for(const auto& M:Candidate.EffectiveMethods())if(!Runtime.ValidateFrameMethod(Runtime.ScriptPath(M.ScriptFile),Error,M.EntryPoint,M.Contract()))return false;
    if(!FDasBlueprintService::Apply(BP,Previous,Error,true))return false;
    const auto* Old=Prior.Classes.FindByPredicate([&](const auto& C){return C.Id==Id;});auto* New=Next.Classes.FindByPredicate([&](const auto& C){return C.Id==Id;});
    if(!Old || !New || Prior.Id!=Next.Id){Error=TEXT("Rename migration requires matching stored class/module identity");return false;}
    struct FRename {FName OldName,NewName;EDasScalarKind Kind;};TArray<FRename> Renames;
    auto Normalized=*New;
    for(auto& Property:Normalized.Properties){
        const auto* Before=Old->Properties.FindByPredicate([&](const auto& P){return P.Id==Property.Id;});
        if(!Before){Error=TEXT("Rename requires unchanged stable IDs; use ue_id equal to the prior property ID");return false;}
        if(Before->Name!=Property.Name){
            FName From(*Before->Name),To(*Property.Name);
            if(From==To || BP->GeneratedClass->FindPropertyByName(To) || BP->GeneratedClass->FindFunctionByName(To)){Error=TEXT("Rename destination already exists or differs only by case: ")+Property.Name;return false;}
            const auto* Field=Previous.Fields.FindByPredicate([&](const auto& F){return F.StableId==Property.Id;});
            const auto* Variable=BP->NewVariables.FindByPredicate([&](const auto& V){return V.VarName==From;});
            if(!Field || !Variable || !Variable->RepNotifyFunc.IsNone() || (Variable->PropertyFlags & CPF_Net)){Error=TEXT("Rename supports authored non-replicated variables only");return false;}
            Renames.Add({From,To,Field->Kind});Property.Name=Before->Name;
        }
    }
    const auto Delta=FDasAuthoringModule::Compare(*Old,Normalized);
    if(Delta.bStructural || Delta.bDefaults || Delta.bMetadata || Delta.bImplementation){Error=TEXT("Rename migration accepts only field names; type/default/flags/method changes must be separate");return false;}
    if(Renames.IsEmpty()){UBlueprint* Result=nullptr;return Apply(Script,BP->GetOutermost()->GetName(),Id,Result,Error);}
    TArray<UClass*> Children;GetDerivedClasses(BP->GeneratedClass,Children,true);TArray<UBlueprint*> Dependents;FBlueprintEditorUtils::FindDependentBlueprints(BP,Dependents);
    auto& Registry=FModuleManager::LoadModuleChecked<FAssetRegistryModule>(TEXT("AssetRegistry")).Get();
    if(!Registry.IsSearchAllAssets())Registry.SearchAllAssets(true);else Registry.WaitForCompletion();
    if(Registry.IsLoadingAssets()){Error=TEXT("Asset Registry scan must finish before rename migration");return false;}
    TArray<FName> Referencers;Registry.GetReferencers(BP->GetOutermost()->GetFName(),Referencers);
    if(!IncludeDependencies && (!Children.IsEmpty() || !Dependents.IsEmpty() || Referencers.ContainsByPredicate([&](FName P){return P!=BP->GetOutermost()->GetFName();}))){Error=TEXT("Rename of dependent Blueprints or saved referencing assets requires fields=rename_with_dependencies");return false;}
    TArray<TStrongObjectPtr<UObject>> ScopeAssets;TArray<UBlueprint*> ScopeBlueprints;ScopeBlueprints.Add(BP);TSet<FName> ScopePackages;ScopePackages.Add(BP->GetOutermost()->GetFName());
    if(IncludeDependencies){
        TArray<FName> Queue;Queue.Add(BP->GetOutermost()->GetFName());
        // Include unsaved/modified loaded dependencies before walking saved
        // referencers, so maps referring to those packages are included too.
        for(auto* Class:Children)if(auto* B=Cast<UBlueprint>(Class->ClassGeneratedBy))Dependents.AddUnique(B);
        for(auto* B:Dependents){ScopeBlueprints.AddUnique(B);ScopeAssets.Emplace(B);const FName Package=B->GetOutermost()->GetFName();if(!ScopePackages.Contains(Package)){if(Queue.Num()>=256 || (!Package.ToString().StartsWith(TEXT("/Game/")) && FPackageName::DoesPackageExist(Package.ToString()))){Error=TEXT("Loaded dependent package exceeds supported rename scope: ")+Package.ToString();return false;}ScopePackages.Add(Package);Queue.Add(Package);}}

        for(int32 Index=0;Index<Queue.Num();++Index){TArray<FName> References;Registry.GetReferencers(Queue[Index],References);
            for(FName Package:References)if(!ScopePackages.Contains(Package)){
                if(!Package.ToString().StartsWith(TEXT("/Game/")) || Queue.Num()>=256){Error=TEXT("Dependent rename requires a bounded /Game package scope: ")+Package.ToString();return false;}
                ScopePackages.Add(Package);Queue.Add(Package);TArray<FAssetData> Assets;Registry.GetAssetsByPackageName(Package,Assets);
                if(Assets.Num()!=1 || (Assets[0].AssetClassPath!=UBlueprint::StaticClass()->GetClassPathName() && Assets[0].AssetClassPath!=UWorld::StaticClass()->GetClassPathName())){Error=TEXT("Unsupported dependent package; only Blueprint and ordinary map assets supported: ")+Package.ToString();return false;}
                UObject* Asset=Assets[0].GetAsset();if(!Asset){Error=TEXT("Could not load dependent asset: ")+Package.ToString();return false;}ScopeAssets.Emplace(Asset);
                if(auto* B=Cast<UBlueprint>(Asset)){if(!B->GeneratedClass || B->Status==BS_Error || !B->GetOutermost()->GetMetaData().GetValue(B,TEXT("DasDSLSource")).IsEmpty()){Error=TEXT("Dependent Blueprint must be valid and have no separate DSL source: ")+Package.ToString();return false;}ScopeBlueprints.AddUnique(B);}
                else if(auto* W=Cast<UWorld>(Asset)){if(W->IsPartitionedWorld() || !W->GetStreamingLevels().IsEmpty()){Error=TEXT("World Partition/streaming map migration requires a separate scope: ")+Package.ToString();return false;}}
            }
        }
        Dependents.Reset();FBlueprintEditorUtils::FindDependentBlueprints(BP,Dependents);for(auto* B:Dependents){ScopeBlueprints.AddUnique(B);ScopePackages.Add(B->GetOutermost()->GetFName());ScopeAssets.Emplace(B);}
        Children.Reset();GetDerivedClasses(BP->GeneratedClass,Children,true);
        for(auto* Class:Children){auto* Child=Cast<UBlueprint>(Class->ClassGeneratedBy);if(!Child || !ScopeBlueprints.Contains(Child)){Error=TEXT("Unmanaged derived class in rename scope");return false;}}
        for(auto* B:ScopeBlueprints)if(B!=BP){if(!B->GeneratedClass || B->Status==BS_Error){Error=TEXT("Dependent Blueprint is not valid for migration: ")+B->GetPathName();return false;}if(!B->GetOutermost()->GetMetaData().GetValue(B,TEXT("DasDSLSource")).IsEmpty()){Error=TEXT("Dependent DSL source requires an explicit source migration");return false;}for(const auto& R:Renames)if(B->GeneratedClass && (B->GeneratedClass->FindPropertyByName(R.NewName) || B->GeneratedClass->FindFunctionByName(R.NewName))){Error=TEXT("Rename destination conflicts in dependent Blueprint: ")+B->GetPathName();return false;}}
        ScopeBlueprints.Sort([&](const UBlueprint& A,const UBlueprint& B){if(&A==BP)return &B!=BP;if(&B==BP)return false;auto Depth=[](const UClass* C){int32 N=0;while(C){++N;C=C->GetSuperClass();}return N;};return Depth(A.GeneratedClass)<Depth(B.GeneratedClass);});
        for(FName Package:ScopePackages)UE_LOG(LogTemp,Display,TEXT("DAS_RENAME_SCOPE %s"),*Package.ToString());
    }
    struct FSnapshot {TStrongObjectPtr<UObject> Object;TArray<FDasScalarValue> Values;};TArray<FSnapshot> Snapshots;
    TArray<UObject*> Objects;GetObjectsOfClass(BP->GeneratedClass,Objects,IncludeDependencies,RF_NoFlags);
    if(IncludeDependencies)for(auto* Object:Objects)if(auto* Actor=Cast<AActor>(Object))if(auto* World=Actor->GetWorld())if(World->IsPartitionedWorld() || !World->GetStreamingLevels().IsEmpty()){Error=TEXT("Loaded World Partition/streaming instances require a separate migration scope");return false;}else if(!ScopePackages.Contains(World->GetOutermost()->GetFName())){ScopePackages.Add(World->GetOutermost()->GetFName());UE_LOG(LogTemp,Display,TEXT("DAS_RENAME_SCOPE live map %s"),*World->GetOutermost()->GetName());}
    for(auto* Object:Objects)if(IsValid(Object) && !Object->GetClass()->HasAnyClassFlags(CLASS_NewerVersionExists) && !Object->GetClass()->GetName().StartsWith(TEXT("SKEL_")) && !Object->GetClass()->GetName().StartsWith(TEXT("REINST_"))){FSnapshot S;S.Object.Reset(Object);for(const auto& R:Renames)S.Values.Add(CaptureRenamedField(Object,R.OldName,R.Kind));Snapshots.Add(MoveTemp(S));}
    const auto Handle=FCoreUObjectDelegates::OnObjectsReplaced.AddLambda([&](const FCoreUObjectDelegates::FReplacementObjectMap& Map){for(auto& S:Snapshots)if(auto* Replacement=Map.Find(S.Object.Get()))S.Object.Reset(*Replacement);});
    ON_SCOPE_EXIT {FCoreUObjectDelegates::OnObjectsReplaced.Remove(Handle);};
    for(auto* B:ScopeBlueprints)B->Modify();for(auto& S:Snapshots)S.Object->Modify();
    BP->Modify();for(const auto& R:Renames)FBlueprintEditorUtils::RenameMemberVariable(BP,R.OldName,R.NewName);
    for(auto* B:ScopeBlueprints){FKismetEditorUtilities::CompileBlueprint(B);FBlueprintCompilationManager::FlushCompilationQueueAndReinstance();if(B->Status==BS_Error || !B->GeneratedClass){Error=TEXT("Dependent rename compiler failed after mutation: ")+B->GetPathName();return false;}}
    if(BP->Status==BS_Error || !BP->GeneratedClass){Error=TEXT("Rename compiler failed after mutation; inspect Blueprint diagnostics");return false;}
    for(auto& S:Snapshots)for(int32 I=0;I<Renames.Num();++I)if(!RestoreRenamedField(S.Object.Get(),Renames[I].NewName,S.Values[I])){Error=TEXT("Rename failed to restore instance value after mutation");return false;}
    for(auto* B:ScopeBlueprints)B->MarkPackageDirty();for(auto& S:Snapshots)S.Object->MarkPackageDirty();for(auto& Asset:ScopeAssets)Asset->MarkPackageDirty();
    if(!FDasBlueprintService::Apply(BP,Candidate,Error,true))return false;
    BP->GetOutermost()->GetMetaData().SetValue(BP,TEXT("DasDSLModel"),*Json);BP->MarkPackageDirty();
    UE_LOG(LogTemp,Display,TEXT("DAS_FIELD_RENAME PASS %d fields; instance values preserved"),Renames.Num());return true;
}

bool FDasDSLService::ReimportWidenedNumericFields(UBlueprint* BP,FString& Error,bool IncludeDependencies){FDasNumericMigrationOptions O;O.IncludeDependencies=IncludeDependencies;O.AdaptGraphs=false;O.WidenOnly=true;return ReimportConvertedNumericFields(BP,O,Error);}
bool FDasDSLService::ReimportConvertedNumericFields(UBlueprint* BP,const FDasNumericMigrationOptions& Options,FString& Error){
    const bool IncludeDependencies=Options.IncludeDependencies;if(uint8(Options.Integers)>4 || uint8(Options.Floats)>1){Error=TEXT("Unknown numeric rounding/precision policy");return false;}
    Error.Reset();if(!IsInGameThread() || !IsValid(BP) || !BP->GeneratedClass){Error=TEXT("Expected valid imported Blueprint on game thread");return false;}
    auto& Runtime=FModuleManager::LoadModuleChecked<FDasRuntimeModule>(TEXT("DasRuntime"));auto& Meta=BP->GetOutermost()->GetMetaData();
    const FString Script=Meta.GetValue(BP,TEXT("DasDSLSource")),Id=Meta.GetValue(BP,TEXT("DasDSLClassId"));
    if(Script.IsEmpty() || FPaths::GetCleanFilename(Script)!=Script){Error=TEXT("Blueprint has no local DSL source");return false;}
    FDasAuthoringModule Prior,Next;FDasClassSchema Previous,Candidate;FString Json;
    if(!FDasAuthoringModule::FromJson(Meta.GetValue(BP,TEXT("DasDSLModel")),Prior,Error) || !Runtime.ReadAuthoringScript(Runtime.ScriptPath(Script),Next,Error) || !Prior.ToActorSchema(Id,Previous,Error) || !Next.ToActorSchema(Id,Candidate,Error) || !Next.ToJson(Json,Error))return false;
    for(const auto& M:Candidate.EffectiveMethods())if(!Runtime.ValidateFrameMethod(Runtime.ScriptPath(M.ScriptFile),Error,M.EntryPoint,M.Contract()))return false;
    if(!FDasBlueprintService::Apply(BP,Previous,Error,true))return false;
    const auto* Old=Prior.Classes.FindByPredicate([&](const auto& C){return C.Id==Id;});auto* New=Next.Classes.FindByPredicate([&](const auto& C){return C.Id==Id;});
    if(!Old || !New || Prior.Id!=Next.Id){Error=TEXT("Rename migration requires matching stored class/module identity");return false;}
    struct FConversion {FName OldName,NewName;EDasScalarKind Kind,Target;};TArray<FConversion> Renames;
    auto Normalized=*New;
    for(auto& Property:Normalized.Properties){
        const auto* Before=Old->Properties.FindByPredicate([&](const auto& P){return P.Id==Property.Id;});
        if(!Before || Property.Name!=Before->Name){Error=TEXT("Numeric widening requires unchanged property names and stable IDs");return false;}
        if(Property.Type.Identity()!=Before->Type.Identity()){
            auto Numeric=[](const FString& K){return K==TEXT("int") || K==TEXT("float") || K==TEXT("double");};
            if(!Numeric(Property.Type.Kind) || !Numeric(Before->Type.Kind) || (Options.WidenOnly && (Property.Type.Kind!=TEXT("double") || (Before->Type.Kind!=TEXT("int") && Before->Type.Kind!=TEXT("float"))))){Error=TEXT("Unsupported numeric field conversion");return false;}
            const auto* Field=Previous.Fields.FindByPredicate([&](const auto& F){return F.StableId==Property.Id;});const auto* Variable=BP->NewVariables.FindByPredicate([&](const auto& V){return V.VarName==FName(*Property.Name);});
            if(!Field || !Variable || !Variable->RepNotifyFunc.IsNone() || (Variable->PropertyFlags & CPF_Net)){Error=TEXT("Numeric widening requires an authored non-replicated field");return false;}
            const auto* Target=Candidate.Fields.FindByPredicate([&](const auto& F){return F.StableId==Property.Id;});auto Default=CaptureRenamedField(BP->GeneratedClass->GetDefaultObject(),Field->Name,Field->Kind);FDasScalarValue Converted;
            if(!Target || !ConvertDasNumber(Default,int32(Target->Kind),Options.Integers,Options.Floats,Converted,Error))return false;
            const double Expected=Converted.Kind==0?double(Converted.IntValue):Converted.Kind==1?double(Converted.FloatValue):Converted.DoubleValue;
            const double Declared=Target->Kind==EDasScalarKind::Int32?double(Target->IntDefault):Target->Kind==EDasScalarKind::Float32?double(Target->FloatDefault):Target->DoubleDefault;
            if(Expected!=Declared){Error=FString::Printf(TEXT("New default must equal policy-converted prior default %.17g: %s"),Expected,*Property.Name);return false;}
            Renames.Add({FName(*Property.Name),FName(*Property.Name),Field->Kind,Target->Kind});Property.Type=Before->Type;Property.Default=Before->Default;
        }
    }
    const auto Delta=FDasAuthoringModule::Compare(*Old,Normalized);
    if(Delta.bStructural || Delta.bDefaults || Delta.bMetadata || Delta.bImplementation){Error=TEXT("Widening accepts only field types and exact promoted defaults; all other declaration changes must be separate");return false;}
    if(Renames.IsEmpty()){UBlueprint* Result=nullptr;return Apply(Script,BP->GetOutermost()->GetName(),Id,Result,Error);}
    TArray<UClass*> Children;GetDerivedClasses(BP->GeneratedClass,Children,true);TArray<UBlueprint*> Dependents;FBlueprintEditorUtils::FindDependentBlueprints(BP,Dependents);
    auto& Registry=FModuleManager::LoadModuleChecked<FAssetRegistryModule>(TEXT("AssetRegistry")).Get();
    if(!Registry.IsSearchAllAssets())Registry.SearchAllAssets(true);else Registry.WaitForCompletion();
    if(Registry.IsLoadingAssets()){Error=TEXT("Asset Registry scan must finish before numeric widening migration");return false;}
    TArray<FName> Referencers;Registry.GetReferencers(BP->GetOutermost()->GetFName(),Referencers);
    if(!IncludeDependencies && (!Children.IsEmpty() || !Dependents.IsEmpty() || Referencers.ContainsByPredicate([&](FName P){return P!=BP->GetOutermost()->GetFName();}))){Error=TEXT("Rename of dependent Blueprints or saved referencing assets requires fields=widen_numeric_with_dependencies");return false;}
    TArray<TStrongObjectPtr<UObject>> ScopeAssets;TArray<UBlueprint*> ScopeBlueprints;ScopeBlueprints.Add(BP);TSet<FName> ScopePackages;ScopePackages.Add(BP->GetOutermost()->GetFName());
    if(IncludeDependencies){
        TArray<FName> Queue;Queue.Add(BP->GetOutermost()->GetFName());
        // Include unsaved/modified loaded dependencies before walking saved
        // referencers, so maps referring to those packages are included too.
        for(auto* Class:Children)if(auto* B=Cast<UBlueprint>(Class->ClassGeneratedBy))Dependents.AddUnique(B);
        for(auto* B:Dependents){ScopeBlueprints.AddUnique(B);ScopeAssets.Emplace(B);const FName Package=B->GetOutermost()->GetFName();if(!ScopePackages.Contains(Package)){if(Queue.Num()>=256 || (!Package.ToString().StartsWith(TEXT("/Game/")) && FPackageName::DoesPackageExist(Package.ToString()))){Error=TEXT("Loaded dependent package exceeds supported numeric widening scope: ")+Package.ToString();return false;}ScopePackages.Add(Package);Queue.Add(Package);}}

        for(int32 Index=0;Index<Queue.Num();++Index){TArray<FName> References;Registry.GetReferencers(Queue[Index],References);
            for(FName Package:References)if(!ScopePackages.Contains(Package)){
                if(!Package.ToString().StartsWith(TEXT("/Game/")) || Queue.Num()>=256){Error=TEXT("Dependent widening requires a bounded /Game package scope: ")+Package.ToString();return false;}
                ScopePackages.Add(Package);Queue.Add(Package);TArray<FAssetData> Assets;Registry.GetAssetsByPackageName(Package,Assets);
                if(Assets.Num()!=1 || (Assets[0].AssetClassPath!=UBlueprint::StaticClass()->GetClassPathName() && Assets[0].AssetClassPath!=UWorld::StaticClass()->GetClassPathName())){Error=TEXT("Unsupported dependent package; only Blueprint and ordinary map assets supported: ")+Package.ToString();return false;}
                UObject* Asset=Assets[0].GetAsset();if(!Asset){Error=TEXT("Could not load dependent asset: ")+Package.ToString();return false;}ScopeAssets.Emplace(Asset);
                if(auto* B=Cast<UBlueprint>(Asset)){if(!B->GeneratedClass || B->Status==BS_Error || !B->GetOutermost()->GetMetaData().GetValue(B,TEXT("DasDSLSource")).IsEmpty()){Error=TEXT("Dependent Blueprint must be valid and have no separate DSL source: ")+Package.ToString();return false;}ScopeBlueprints.AddUnique(B);}
                else if(auto* W=Cast<UWorld>(Asset)){if(W->IsPartitionedWorld() || !W->GetStreamingLevels().IsEmpty()){Error=TEXT("World Partition/streaming map migration requires a separate scope: ")+Package.ToString();return false;}}
            }
        }
        Dependents.Reset();FBlueprintEditorUtils::FindDependentBlueprints(BP,Dependents);for(auto* B:Dependents){ScopeBlueprints.AddUnique(B);ScopePackages.Add(B->GetOutermost()->GetFName());ScopeAssets.Emplace(B);}
        Children.Reset();GetDerivedClasses(BP->GeneratedClass,Children,true);
        for(auto* Class:Children){auto* Child=Cast<UBlueprint>(Class->ClassGeneratedBy);if(!Child || !ScopeBlueprints.Contains(Child)){Error=TEXT("Unmanaged derived class in numeric widening scope");return false;}}
        for(auto* B:ScopeBlueprints)if(B!=BP){if(!B->GeneratedClass || B->Status==BS_Error){Error=TEXT("Dependent Blueprint is not valid for migration: ")+B->GetPathName();return false;}if(!B->GetOutermost()->GetMetaData().GetValue(B,TEXT("DasDSLSource")).IsEmpty()){Error=TEXT("Dependent DSL source requires an explicit source migration");return false;}}
        ScopeBlueprints.Sort([&](const UBlueprint& A,const UBlueprint& B){if(&A==BP)return &B!=BP;if(&B==BP)return false;auto Depth=[](const UClass* C){int32 N=0;while(C){++N;C=C->GetSuperClass();}return N;};return Depth(A.GeneratedClass)<Depth(B.GeneratedClass);});
        for(FName Package:ScopePackages)UE_LOG(LogTemp,Display,TEXT("DAS_WIDEN_SCOPE %s"),*Package.ToString());
    }
    // A raw Blueprint property node may need a conversion or narrowing adapter.
    // Refuse it before mutation; DSL method call graphs retain their contracts.
    if(!Options.AdaptGraphs)for(auto* B:ScopeBlueprints){TArray<UK2Node*> Nodes;FBlueprintEditorUtils::GetAllNodesOfClass(B,Nodes);for(const auto& R:Renames)if(Nodes.ContainsByPredicate([&](const auto* Node){return Node->ReferencesVariable(R.OldName,nullptr);})){Error=TEXT("Use convert_numeric policy to adapt Blueprint property links");return false;}}
    auto NumericPin=[](EDasScalarKind K){FEdGraphPinType T;T.PinCategory=K==EDasScalarKind::Int32?UEdGraphSchema_K2::PC_Int:UEdGraphSchema_K2::PC_Real;if(K!=EDasScalarKind::Int32)T.PinSubCategory=K==EDasScalarKind::Float32?UEdGraphSchema_K2::PC_Float:UEdGraphSchema_K2::PC_Double;return T;};
    FDasNumericGraphMigration GraphMigration;TMap<FName,FEdGraphPinType> ChangedTypes;for(const auto& R:Renames)ChangedTypes.Add(R.OldName,NumericPin(R.Target));
    if(Options.AdaptGraphs && !GraphMigration.Prepare(ScopeBlueprints,BP->GeneratedClass,ChangedTypes,Options.Integers,Options.Floats,Error))return false;
    struct FSnapshot {TStrongObjectPtr<UObject> Object;TArray<FDasScalarValue> Values,Converted;};TArray<FSnapshot> Snapshots;
    TArray<UObject*> Objects;GetObjectsOfClass(BP->GeneratedClass,Objects,IncludeDependencies,RF_NoFlags);
    if(IncludeDependencies)for(auto* Object:Objects)if(auto* Actor=Cast<AActor>(Object))if(auto* World=Actor->GetWorld())if(World->IsPartitionedWorld() || !World->GetStreamingLevels().IsEmpty()){Error=TEXT("Loaded World Partition/streaming instances require a separate migration scope");return false;}else if(!ScopePackages.Contains(World->GetOutermost()->GetFName())){ScopePackages.Add(World->GetOutermost()->GetFName());UE_LOG(LogTemp,Display,TEXT("DAS_WIDEN_SCOPE live map %s"),*World->GetOutermost()->GetName());}
    for(auto* Object:Objects)if(IsValid(Object) && !Object->GetClass()->HasAnyClassFlags(CLASS_NewerVersionExists) && !Object->GetClass()->GetName().StartsWith(TEXT("SKEL_")) && !Object->GetClass()->GetName().StartsWith(TEXT("REINST_"))){FSnapshot S;S.Object.Reset(Object);for(const auto& R:Renames){auto V=CaptureRenamedField(Object,R.OldName,R.Kind);FDasScalarValue Converted;if(!ConvertDasNumber(V,int32(R.Target),Options.Integers,Options.Floats,Converted,Error)){Error+=TEXT(": ")+Object->GetPathName()+TEXT(".")+R.OldName.ToString();return false;}S.Values.Add(V);S.Converted.Add(MoveTemp(Converted));}Snapshots.Add(MoveTemp(S));}
    const auto Handle=FCoreUObjectDelegates::OnObjectsReplaced.AddLambda([&](const FCoreUObjectDelegates::FReplacementObjectMap& Map){for(auto& S:Snapshots)if(auto* Replacement=Map.Find(S.Object.Get()))S.Object.Reset(*Replacement);});
    ON_SCOPE_EXIT {FCoreUObjectDelegates::OnObjectsReplaced.Remove(Handle);};
    for(auto* B:ScopeBlueprints)if(B->Status!=BS_UpToDate && B->Status!=BS_UpToDateWithWarnings){Error=TEXT("Compile scoped Blueprints before numeric widening: ")+B->GetPathName();return false;}
    if(!Runtime.BeginPreserveMethodContextsForReinstance(BP->GeneratedClass)){Error=TEXT("Numeric widening requires idle runtime reinstance scope");return false;}
    FDasAuthoringModule Approval;Approval.Id=Next.Id;Approval.Classes.Add(*New);FString ApprovedWire;if(!Approval.ToJson(ApprovedWire,Error)){Runtime.EndPreserveMethodContextsForReinstance();return false;}bool MigrationSucceeded=false;
    ON_SCOPE_EXIT {Runtime.EndPreserveMethodContextsForReinstance(MigrationSucceeded?ApprovedWire:FString());};
    const auto OriginalVariables=BP->NewVariables;TMap<UPackage*,bool> OriginalDirty;
    auto RememberDirty=[&](UObject* Object){auto* Package=Object->GetOutermost();if(!OriginalDirty.Contains(Package))OriginalDirty.Add(Package,Package->IsDirty());};
    for(auto* B:ScopeBlueprints)RememberDirty(B);for(auto& S:Snapshots)RememberDirty(S.Object.Get());for(auto& Asset:ScopeAssets)RememberDirty(Asset.Get());
    auto CompileScope=[&](){for(auto* B:ScopeBlueprints){FKismetEditorUtilities::CompileBlueprint(B);FBlueprintCompilationManager::FlushCompilationQueueAndReinstance();if(B->Status==BS_Error || !B->GeneratedClass)return false;}return true;};
    auto Rollback=[&](const FString& Failure){
        BP->NewVariables=OriginalVariables;FString GraphError;bool Restored=!Options.AdaptGraphs || GraphMigration.Restore(GraphError);FBlueprintEditorUtils::MarkBlueprintAsStructurallyModified(BP);Restored &= CompileScope();if(Options.AdaptGraphs)Restored &= GraphMigration.Restore(GraphError);
        for(auto& S:Snapshots)for(int32 I=0;I<Renames.Num();++I)Restored &= RestoreRenamedField(S.Object.Get(),Renames[I].OldName,S.Values[I]);
        FString Validation;Restored &= FDasBlueprintService::Apply(BP,Previous,Validation,true);
        if(Restored)for(const auto& Dirty:OriginalDirty)Dirty.Key->SetDirtyFlag(Dirty.Value);
        Error=(Restored?TEXT("Numeric migration rolled back: "):TEXT("Numeric rollback incomplete: "))+Failure;
        UE_LOG(LogTemp,Display,TEXT("DAS_NUMERIC_ROLLBACK=%s"),Restored?TEXT("PASS"):TEXT("FAIL"));return false;
    };
    for(auto* B:ScopeBlueprints)B->Modify();for(auto& S:Snapshots)S.Object->Modify();
    for(const auto& R:Renames){auto* Variable=BP->NewVariables.FindByPredicate([&](const auto& V){return V.VarName==R.OldName;});Variable->VarType=NumericPin(R.Target);const auto* Field=Candidate.Fields.FindByPredicate([&](const auto& F){return F.Name==R.OldName;});Variable->DefaultValue=R.Target==EDasScalarKind::Int32?FString::FromInt(Field->IntDefault):FString::Printf(TEXT("%.17g"),R.Target==EDasScalarKind::Float32?double(Field->FloatDefault):Field->DoubleDefault);}
    FBlueprintEditorUtils::MarkBlueprintAsStructurallyModified(BP);
    if(Options.AdaptGraphs && !GraphMigration.Apply(Error)){const FString Failure=Error;return Rollback(Failure);}
    if(!CompileScope())return Rollback(TEXT("Blueprint compilation failed"));
#if WITH_DEV_AUTOMATION_TESTS
    if(GDasFailWidenAfterCompile)return Rollback(TEXT("Injected development test failure"));
#endif
    for(auto& S:Snapshots)for(int32 I=0;I<Renames.Num();++I)if(!RestoreRenamedField(S.Object.Get(),Renames[I].NewName,S.Converted[I]))return Rollback(TEXT("Converted value restore failed"));
    if(!FDasBlueprintService::Apply(BP,Candidate,Error,true)){const FString Failure=Error;return Rollback(Failure);}
    for(auto* B:ScopeBlueprints)B->MarkPackageDirty();for(auto& S:Snapshots)S.Object->MarkPackageDirty();for(auto& Asset:ScopeAssets)Asset->MarkPackageDirty();
    BP->GetOutermost()->GetMetaData().SetValue(BP,TEXT("DasDSLModel"),*Json);BP->MarkPackageDirty();
    MigrationSucceeded=true;UE_LOG(LogTemp,Display,TEXT("DAS_NUMERIC_WIDEN PASS %d fields; exact finite values preserved"),Renames.Num());return true;
}
