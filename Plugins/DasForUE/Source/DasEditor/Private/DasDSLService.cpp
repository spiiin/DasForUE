#include "DasDSLService.h"
#include "DasCharacter.h"
#include "DasPawn.h"
#include "GameFramework/Controller.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerState.h"
#include "UObject/UObjectIterator.h"
#include "DasDataAssets.h"
#include "DasSaveGame.h"
#include "DasTypePins.h"
#include "DasBlueprintDefaults.h"
#include "DasDefaultRoots.h"
#include "DasContainerTypes.h"
#include "DasScriptHosts.h"
#include "DasLibraryService.h"
#include "DasComponentComposition.h"
#include "DasDefaultsMigration.h"
#include "DasAuthoring.h"
#include "DasWidgetLayouts.h"
#include "Blueprint/WidgetBlueprintGeneratedClass.h"
#include "Blueprint/WidgetTree.h"
#include "DasBlueprintService.h"
#include "DasRuntime.h"
#include "DasAsyncPhysicsVM.h"
#include "DasProbeActor.h"
#include "DasScriptComponent.h"
#include "DasPropertyValues.h"
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
#include "UObject/SavePackage.h"
#include "HAL/FileManager.h"
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
#include "DasBlueprintRollback.h"
#include "Misc/SecureHash.h"
#if WITH_DEV_AUTOMATION_TESTS
static bool GDasFailWidenAfterCompile = false;
static int32 GDasFailStructuralStage = 0, GDasFailRenameStage = 0;
void DasFailStructuralForTesting(int32 Stage)
{
    GDasFailStructuralStage = Stage;
}
void DasFailRenameForTesting(int32 Stage)
{
    GDasFailRenameStage = Stage;
}
void DasFailWidenAfterCompileForTesting(bool Enabled)
{
    GDasFailWidenAfterCompile = Enabled;
}
#endif

#include "DasWidgetLayoutsPreflight.inl"

namespace
{
// The declaration the runtime may reload into: the class and its authored ancestors, as the script's model
// lists them (a child's model is invalid without its parent).
FDasAuthoringModule DasApprovalModel(const FDasAuthoringModule& Model, const FString& Id)
{
    TSet<FString> Keep;
    for (FString Cur = Id; !Cur.IsEmpty() && !Keep.Contains(Cur);)
    {
        Keep.Add(Cur);
        const auto* C = Model.Classes.FindByPredicate([&](const auto& X) { return X.Id == Cur; });
        Cur = C ? C->ParentId : FString();
    }
    FDasAuthoringModule Out;
    Out.Id = Model.Id;
    for (const auto& C : Model.Classes)
        if (Keep.Contains(C.Id))
            Out.Classes.Add(C);
    return Out;
}
// Reinstancing replaces a possessed Pawn without moving its controller to the replacement; keep the player
// (or AI) in control of the new instance, also when a failed migration rolls the class back.
struct FKeepPossession
{
    struct FEntry
    {
        TWeakObjectPtr<AController> Controller;
        UObject* Pawn;
    };
    TArray<FEntry> Entries;
    FDelegateHandle Handle;
    explicit FKeepPossession(UClass* Class)
    {
        for (TObjectIterator<APawn> It; It; ++It)
            if (It->IsA(Class) && !It->IsTemplate() && It->GetController())
                Entries.Add({It->GetController(), *It});
        Handle = FCoreUObjectDelegates::OnObjectsReplaced.AddLambda(
            [this](const FCoreUObjectDelegates::FReplacementObjectMap& Map)
            {
                for (auto& E : Entries)
                    if (auto* New = Map.Find(E.Pawn))
                        E.Pawn = *New;
            });
    }
    ~FKeepPossession()
    {
        FCoreUObjectDelegates::OnObjectsReplaced.Remove(Handle);
        for (const auto& E : Entries)
            if (auto* Pawn = Cast<APawn>(E.Pawn);
                Pawn && IsValid(Pawn) && E.Controller.IsValid() && E.Controller->GetPawn() != Pawn)
            {
                // The replacement inherits the old pawn's bindings, including the player state's destroy
                // callback that possession adds again.
                if (APlayerState* State = E.Controller->PlayerState)
                    Pawn->OnDestroyed.RemoveAll(State);
                Pawn->Controller = nullptr;
                E.Controller->Possess(Pawn);
            }
    }
};

// Apply with the declaration read from Script, or with Override (the declaration of Script with some
// values replaced; see SplitDefaultChanges).
bool ApplyDeclaration(const FString& Script, const FString& Package, const FString& Id, UBlueprint*& Out,
                      FString& Error, EDasDefaultMigrationPolicy DefaultPolicy, const FDasAuthoringModule* Override)
{
    Out = nullptr;
    Error.Reset();
    if (!IsInGameThread() || FPaths::GetCleanFilename(Script) != Script ||
        FPaths::GetExtension(Script) !=
            TEXT("das") || !Package.StartsWith(TEXT("/Game/")) || !FPackageName::IsValidLongPackageName(Package))
    {
        Error = TEXT("Expected ScriptName.das in plugin Scripts and /Game/Folder/BP_Name on game thread");
        return false;
    }
    auto& Runtime = FModuleManager::LoadModuleChecked<FDasRuntimeModule>(TEXT("DasRuntime"));
    FDasAuthoringModule Model;
    FDasClassSchema Schema;
    FString Json;
    if (Override)
        Model = *Override;
    if ((!Override && !Runtime.ReadAuthoringScript(Runtime.ScriptPath(Script), Model, Error)) ||
        !Model.ToActorSchema(Id, Schema, Error) || !Model.ToJson(Json, Error))
        return false;
    FDasDeclaredClass PhysicsDeclaration;
    if (!Model.ResolveClass(Id, PhysicsDeclaration, Error))
        return false;
    if (PhysicsDeclaration.AsyncPhysics)
    {
        auto Prepared =
            PrepareDasAsyncPhysicsVM(Runtime.ScriptPath(PhysicsDeclaration.AsyncPhysics->ScriptFile), nullptr, Error);
        if (!Prepared)
        {
            Error = TEXT("Async Physics preflight: ")+ Error;
            return false;
        }
    }
    for (const auto& Method : Schema.EffectiveMethods())
        if (!Runtime.ValidateFrameMethod(Runtime.ScriptPath(Method.ScriptFile), Error, Method.EntryPoint,
                                         Method.Contract()))
            return false;
    if (Schema.Parent == TEXT("FunctionLibrary"))
        return ApplyDasLibrary(Model, Package, Id, Out, Error);
    const FString ObjectPath = Package + TEXT(".")+ FPackageName::GetLongPackageAssetName(Package);
    // A non-Blueprint asset at the requested path must never be replaced.
    UObject* Existing = LoadObject<UObject>(nullptr, *ObjectPath, nullptr, LOAD_NoWarn);
    if (!Existing && FPackageName::DoesPackageExist(Package))
    {
        Error = TEXT("Existing package could not be loaded; refusing to create over it");
        return false;
    }
    UBlueprint* BP = Cast<UBlueprint>(Existing);
    if (Existing && !BP)
    {
        Error = TEXT("Asset exists and is not a Blueprint");
        return false;
    }
    if (!PreflightDasWidgetLayouts(PhysicsDeclaration, BP, Error))
        return false;
    const bool Created = !BP;
    if (!Created && BP->GeneratedClass)
    {
        const UObject* CDO = BP->GeneratedClass->GetDefaultObject();
        const FString ExistingIdentity = DasHostIdentity(CDO);
        if (!ExistingIdentity.IsEmpty() && ExistingIdentity != Id)
        {
            Error = TEXT("Cooked authored identity differs from import declaration");
            return false;
        }
    }
    const auto& Declaration = *Model.Classes.FindByPredicate([&](const auto& C) { return C.Id == Id; });
    if (DefaultPolicy == EDasDefaultMigrationPolicy::Automatic)
    {
        DefaultPolicy = EDasDefaultMigrationPolicy::Refuse;
        if (!Created)
        {
            FDasAuthoringModule Prior;
            FString Reason;
            const FString Wire = BP->GetOutermost()->GetMetaData().GetValue(BP, TEXT("DasDSLModel"));
            if (!Wire.IsEmpty() && FDasAuthoringModule::FromJson(Wire, Prior, Reason) &&
                FDasAuthoringModule::DefaultsOnly(Prior, Model, Reason))
            {
                // Keep the existing body-only route (including PIE) when the CDO
                // already matches the source. A real default delta or edited CDO
                // needs the staged merge and its stronger world/idle checks.
                if (Wire != Json || !FDasBlueprintService::Apply(BP, Schema, Reason, true))
                    DefaultPolicy = EDasDefaultMigrationPolicy::PreserveOverrides;
            }
        }
    }
    if (DefaultPolicy == EDasDefaultMigrationPolicy::PreserveOverrides)
    {
        if (Created)
        {
            Error = TEXT("Safe default update requires an existing imported Blueprint");
            return false;
        }
        if (!DasMergeDefaults(BP, Model, Id, Error))
            return false;
        Out = BP;
        return true;
    }
    if (!Created)
    {
        const FString Prior = BP->GetOutermost()->GetMetaData().GetValue(BP, TEXT("DasDSLModel"));
        FDasAuthoringModule Previous;
        if (!Prior.IsEmpty() && FDasAuthoringModule::FromJson(Prior, Previous, Error))
        {
            const auto* Old = Previous.Classes.FindByPredicate([&](const auto& C) { return C.Id == Id; });
            if (Old && (Old->NativeDefaults.Num() != Declaration.NativeDefaults.Num() ||
                        Old->NativeDefaults.ContainsByPredicate([&](const auto& G)
                                                                { return !Declaration.NativeDefaults.Contains(G); }) ||
                        Old->PropertyOverrides.Num() != Declaration.PropertyOverrides.Num() ||
                        Old->PropertyOverrides.ContainsByPredicate(
                            [&](const auto& O) { return !Declaration.PropertyOverrides.Contains(O); })))
            {
                Error = TEXT("Inherited defaults require PreserveOverrides policy");
                return false;
            }
        }
    }
    if (!Created)
    {
        const FString PriorWire = BP->GetOutermost()->GetMetaData().GetValue(BP, TEXT("DasDSLModel"));
        if (!PriorWire.IsEmpty())
        {
            FDasAuthoringModule Prior;
            if (!FDasAuthoringModule::FromJson(PriorWire, Prior, Error))
                return false;
            const auto* Old = Prior.Classes.FindByPredicate([&](const auto& C) { return C.Id == Id; });
            if (!Old ||
                (Old->ParentId == Declaration.ParentId && Old->ComponentOverrides != Declaration.ComponentOverrides))
            {
                Error = TEXT("Component composition migration is deferred");
                return false;
            }
            // Components may be added, removed, renamed (same id), re-attached or get new template defaults; a
            // changed component class or root stays a new Blueprint.
            for (const auto& C : Declaration.Components)
            {
                const auto* Was = Old->Components.FindByPredicate([&](const auto& X) { return X.Id == C.Id; });
                if ((Was && (Was->NativeClass != C.NativeClass || Was->ScriptFile != C.ScriptFile ||
                             Was->ClassName != C.ClassName || Was->bRoot != C.bRoot)) ||
                    (!Was && C.bRoot))
                {
                    Error = TEXT("Changing a component class or the root component is deferred; create a new "
                                 "Blueprint");
                    return false;
                }
            }
            for (const auto& C : Old->Components)
                if (C.bRoot && !Declaration.Components.ContainsByPredicate([&](const auto& X) { return X.Id == C.Id; }))
                {
                    Error = TEXT("Changing a component class or the root component is deferred; create a new "
                                 "Blueprint");
                    return false;
                }
            for (const auto& Property : Declaration.Properties)
            {
                const auto* Before =
                    Old->Properties.FindByPredicate([&](const auto& P) { return P.Id == Property.Id; });
                if (Before && Before->Type.AuthoredId != Property.Type.AuthoredId)
                {
                    Error = TEXT("Authored reference field identity migration is deferred");
                    return false;
                }
            }
        }
        else if (!Declaration.Components.IsEmpty())
        {
            Error = TEXT("Adding components to an existing unimported Blueprint is unsupported");
            return false;
        }
    }
    if (!Created)
    {
        const FString PriorWire = BP->GetOutermost()->GetMetaData().GetValue(BP, TEXT("DasDSLModel"));
        FDasAuthoringModule Prior;
        if (!PriorWire.IsEmpty() && !FDasAuthoringModule::FromJson(PriorWire, Prior, Error))
            return false;
        const auto* Old = Prior.Classes.FindByPredicate([&](const auto& C) { return C.Id == Id; });
        auto HasContainers = [](const FDasDeclaredClass& C)
        {
            return C.Properties.ContainsByPredicate(
                [](const auto& P)
                { return P.Type.Kind == TEXT("array") || P.Type.Kind == TEXT("map") || P.Type.Kind == TEXT("set"); });
        };
        // Classes with container fields migrate like other classes (rollback keeps container values as property
        // text); the explicit per-instance default migration does not cover containers yet.
        if ((HasContainers(Declaration) || (Old && HasContainers(*Old))) &&
            DefaultPolicy == EDasDefaultMigrationPolicy::PreserveInstances)
        {
            Error = TEXT("Default migration of classes with container fields is deferred; other declaration changes "
                         "are supported");
            return false;
        }
    }
    // Composed actors migrate like other actors while their composition is unchanged (checked above), so the
    // SCS and its templates stay as they are. Default migration of placed instances does not cover them yet.
    if (!Created && !Declaration.Components.IsEmpty() && DefaultPolicy == EDasDefaultMigrationPolicy::PreserveInstances)
    {
        Error =
            TEXT("Default migration of actors with components is deferred; other declaration changes are supported");
        return false;
    }
    if (!Created)
    {
        const FString Wire = BP->GetOutermost()->GetMetaData().GetValue(BP, TEXT("DasDSLModel"));
        FDasAuthoringModule Prior;
        if (!Wire.IsEmpty())
        {
            if (!FDasAuthoringModule::FromJson(Wire, Prior, Error))
                return false;
            if (const auto* Old = Prior.Classes.FindByPredicate([&](const auto& C) { return C.Id == Id; }))
                for (const auto& M : Declaration.Methods)
                {
                    const auto* Before = Old->Methods.FindByPredicate([&](const auto& P) { return P.Id == M.Id; });
                    if (Before && Before->bBlueprintEvent != M.bBlueprintEvent)
                    {
                        Error = TEXT("Changing callable/BlueprintEvent role requires a later dependency migration; create a new Blueprint for this declaration");
                        return false;
                    }
                }
        }
    }
    if (!Created)
    {
        const FString Wire = BP->GetOutermost()->GetMetaData().GetValue(BP, TEXT("DasDSLModel"));
        FDasAuthoringModule Prior;
        if (!Wire.IsEmpty())
        {
            if (!FDasAuthoringModule::FromJson(Wire, Prior, Error))
                return false;
            const auto* Old = Prior.Classes.FindByPredicate([&](const auto& C) { return C.Id == Id; });
            auto HasEvents = [](const FDasDeclaredClass& C)
            {
                return C.Properties.ContainsByPredicate(
                    [](const auto& P) { return P.Type.Kind == TEXT("event") || P.Type.Kind == TEXT("delegate"); });
            };
            // Classes with events and delegates migrate like others; an event or delegate keeps its signature
            // (bindings in Blueprints and live sessions depend on it), and their defaults are not migrated per
            // instance.
            if ((HasEvents(Declaration) || (Old && HasEvents(*Old))) &&
                DefaultPolicy == EDasDefaultMigrationPolicy::PreserveInstances)
            {
                Error = TEXT("Default migration of classes with delegates/events is deferred");
                return false;
            }
            if (Old)
                for (const auto& P : Declaration.Properties)
                    if (const auto* Was = Old->Properties.FindByPredicate([&](const auto& X) { return X.Id == P.Id; });
                        Was && (P.Type.Kind ==
                                TEXT("event") || P.Type.Kind == TEXT("delegate") ||
                                                                     Was->Type.Kind ==
                                                                         TEXT("event") ||
                                                                              Was->Type.Kind == TEXT("delegate")) &&
                                                                              Was->Type.Identity() != P.Type.Identity())
                    {
                        Error =
                            TEXT("Changing an event or delegate signature is deferred; create a new field: ")+ P.Name;
                        return false;
                    }
        }
    }
    FDasDeclaredClass EffectiveDeclaration;
    if (!Model.ResolveClass(Id, EffectiveDeclaration, Error))
        return false;
    TMap<FString, UBlueprint*> ComponentBlueprints;
    if (!DasPrepareComponentComposition(Declaration, EffectiveDeclaration, Package, ComponentBlueprints, Error))
        return false;
    if (!Created && Schema.Parent == TEXT("ActorComponent") &&
                                          (DefaultPolicy != EDasDefaultMigrationPolicy::Refuse ||
                                           BP->GetOutermost()->GetMetaData().GetValue(BP, TEXT("DasDSLModel")) != Json))
    {
        Error = TEXT("Component declaration migration is deferred; body-only reimport is supported");
        return false;
    }
    if (DefaultPolicy != EDasDefaultMigrationPolicy::Refuse)
    {
        if (DefaultPolicy != EDasDefaultMigrationPolicy::PreserveInstances || Created)
        {
            Error = TEXT("Default migration requires an existing imported Blueprint and a known policy");
            return false;
        }
        FDasAuthoringModule Prior;
        if (!FDasAuthoringModule::FromJson(BP->GetOutermost()->GetMetaData().GetValue(BP, TEXT("DasDSLModel")), Prior,
                                                                                      Error))
            return false;
        const auto* Old = Prior.Classes.FindByPredicate([&](const auto& C) { return C.Id == Id; });
        const auto* Next = Model.Classes.FindByPredicate([&](const auto& C) { return C.Id == Id; });
        if (!Old || !Next || Prior.Id != Model.Id)
        {
            Error = TEXT("Default migration requires matching stored source identity");
            return false;
        }
        const auto Delta = FDasAuthoringModule::Compare(*Old, *Next);
        if (Delta.bStructural || Delta.bMetadata || Delta.bImplementation)
        {
            Error = TEXT("Default migration accepts only default changes; apply other declaration changes separately");
            return false;
        }
        FDasClassSchema Previous;
        if (!Prior.ToActorSchema(Id, Previous, Error) || !FDasBlueprintService::Apply(BP, Previous, Error, true))
            return false;
    }
    UClass* DeclaredParent = DasNativeScriptHost(Schema.Parent.ToString());
    if (!Schema.ParentStableId.IsEmpty())
    {
        const auto* Parent =
            Model.Classes.FindByPredicate([&](const auto& C) { return C.Id == Schema.ParentStableId; });
        if (!Parent || Parent->SourceScript.IsEmpty())
        {
            Error = TEXT("Authored parent source is unavailable");
            return false;
        }
        const FString ParentPackage = FPackageName::GetLongPackagePath(Package) + TEXT("/BP_")+ Parent->Name;
        if (ParentPackage == Package)
        {
            Error = TEXT("Authored parent package conflicts with child");
            return false;
        }
        const bool SaveParent = !FPackageName::DoesPackageExist(ParentPackage);
        if (!SaveParent)
        {
            auto* ExistingParent = LoadObject<UBlueprint>(
                nullptr, *(ParentPackage + TEXT(".")+ FPackageName::GetLongPackageAssetName(ParentPackage)));
            if (!ExistingParent || FString(ExistingParent->GetOutermost()->GetMetaData().GetValue(
                                               ExistingParent, TEXT("DasDSLClassId"))) != Parent->Id)
            {
                Error = TEXT("Authored parent path conflicts with an unrelated asset");
                return false;
            }
        }
        UBlueprint* ParentBP = nullptr;
        if (!FDasDSLService::Apply(Parent->SourceScript, ParentPackage, Parent->Id, ParentBP, Error,
                                   EDasDefaultMigrationPolicy::Automatic))
            return false;
        DeclaredParent = ParentBP->GeneratedClass;
        if (SaveParent)
        {
            const FString File =
                FPackageName::LongPackageNameToFilename(ParentPackage, FPackageName::GetAssetPackageExtension());
            IFileManager::Get().MakeDirectory(*FPaths::GetPath(File), true);
            FSavePackageArgs Save;
            Save.TopLevelFlags = RF_Public | RF_Standalone;
            if (!UPackage::SavePackage(ParentBP->GetOutermost(), ParentBP, *File, Save))
            {
                Error = TEXT("Cannot save newly generated authored parent");
                return false;
            }
        }
    }
    if (Created && (!DasInitializeInheritedDefaults(DeclaredParent, Model, Id, Error, true) ||
                    !DasInitializeNativeDefaults(DeclaredParent, Model, Id, Error, true)))
        return false;
    // A new authored parent with the same native host is applied after the rollback snapshot below.
    UClass* Reparent = nullptr;
    if (!Created && BP->ParentClass != DeclaredParent)
    {
        auto NativeRoot = [](UClass* C)
        {
            while (C && !C->HasAnyClassFlags(CLASS_Native))
                C = C->GetSuperClass();
            return C;
        };
        if (!DeclaredParent || NativeRoot(BP->ParentClass) != NativeRoot(DeclaredParent) ||
            DefaultPolicy != EDasDefaultMigrationPolicy::Refuse)
        {
            Error = TEXT("Changing the native parent of a Blueprint is deferred; create a new Blueprint");
            return false;
        }
        Reparent = DeclaredParent;
    }
    if (Created)
    {
        UPackage* P = CreatePackage(*Package);
        BP = FKismetEditorUtilities::CreateBlueprint(DeclaredParent, P, *FPackageName::GetLongPackageAssetName(Package),
                                                     BPTYPE_Normal, UBlueprint::StaticClass(),
                                                     UBlueprintGeneratedClass::StaticClass(), TEXT("DasDSL"));
    }
    if (!BP)
    {
        Error = TEXT("Cannot create Blueprint");
        return false;
    }
    // A new Blueprint gets its components now; an existing one after the rollback snapshot below.
    if (Created &&
        !DasApplyComponentComposition(BP, Declaration, EffectiveDeclaration, ComponentBlueprints, true, Error))
        return false;
    FDasAuthoringModule PriorModel;
    const FDasDeclaredClass* PriorClass = nullptr;
    if (FString PriorError; !Created && FDasAuthoringModule::FromJson(BP->GetOutermost()->GetMetaData().GetValue(
                                            BP, TEXT("DasDSLModel")), PriorModel, PriorError))
        PriorClass = PriorModel.Classes.FindByPredicate([&](const auto& C) { return C.Id == Id; });
    auto& Metadata = BP->GetOutermost()->GetMetaData();
    const bool BodyOnly = !Created && Metadata.GetValue(BP, TEXT("DasDSLModel")) == Json;
    // Pawn and Character declarations migrate like other actors: the rollback snapshot covers every host.
    const bool Preserve = !Created && !BodyOnly && DefaultPolicy == EDasDefaultMigrationPolicy::Refuse;
    TOptional<FKeepPossession> Possession;
    if (Preserve)
        Possession.Emplace(BP->GeneratedClass);
    FString ApprovalWire;
    bool Applied = false;
    TUniquePtr<FDasBlueprintRollback> Rollback;
    if (Preserve)
    {
        // A reparent is checked by the real apply below (the current parent does not match the schema yet).
        if (!Reparent && !FDasBlueprintService::Apply(BP, Schema, Error, true))
            return false;
        TArray<UBlueprint*> Scope;
        TArray<TStrongObjectPtr<UObject>> Assets;
        if (!FDasBlueprintRollback::GatherScope(BP, Scope, Assets, Error))
            return false;
        Rollback = MakeUnique<FDasBlueprintRollback>();
        if (!Rollback->Prepare(BP, Schema, Scope, Assets, Error))
            return false;
        const FDasAuthoringModule Approval = DasApprovalModel(Model, Id);
        if (!Approval.ToJson(ApprovalWire, Error))
            return false;
        if (!Runtime.BeginPreserveContextsForReinstance(BP->GeneratedClass))
        {
            Error = TEXT("Declaration reimport requires idle runtime with no active call frames");
            return false;
        }
    }
    ON_SCOPE_EXIT
    {
        if (Preserve)
            Runtime.EndPreserveContextsForReinstance(Applied ? ApprovalWire : FString());
    };
    auto Restore = [&](const FString& Reason)
    {
        FString RestoreError;
        const bool OK = Rollback && Rollback->Restore(RestoreError);
        Error = (OK ? TEXT("Structural migration rolled back: "): TEXT("Structural rollback incomplete: ")) + Reason;
        if (!RestoreError.IsEmpty())
            Error += TEXT("; ")+ RestoreError;
        UE_LOG(LogTemp, Display, TEXT("DAS_STRUCTURAL_ROLLBACK=%s"), OK ? TEXT("PASS"): TEXT("FAIL"));
        return false;
    };
    if (Reparent)
    {
        if (!Rollback)
        {
            Error = TEXT("Changing the parent requires a structural reimport");
            return false;
        }
        BP->Modify();
        BP->ParentClass = Reparent;
        FBlueprintEditorUtils::RefreshAllNodes(BP);
        FBlueprintEditorUtils::MarkBlueprintAsStructurallyModified(BP);
    }
    if (!Created && !DasApplyComponentComposition(BP, Declaration, EffectiveDeclaration, ComponentBlueprints, false,
                                                  Error, PriorClass))
    {
        if (Rollback)
        {
            const FString Reason = Error;
            return Restore(Reason);
        }
        return false;
    }
    // Validate the current Blueprint even when unchanged declarations avoid reinstancing.
    if (!FDasBlueprintService::Apply(BP, Schema, Error, BodyOnly, DefaultPolicy))
    {
        if (Rollback)
        {
            const FString Reason = Error;
            return Restore(Reason);
        }
        return false;
    }
#if WITH_DEV_AUTOMATION_TESTS
    if (Rollback && GDasFailStructuralStage == 1)
        return Restore(TEXT("Injected post-compile development failure"));
#endif
    if (Created)
        if (DasActorHostClass(BP->GeneratedClass))
        {
            auto* Host = BP->GeneratedClass->GetDefaultObject();
            const auto* Root = EffectiveDeclaration.Components.FindByPredicate([](const auto& C) { return C.bRoot; });
            FindFProperty<FBoolProperty>(Host->GetClass(),
                                         TEXT("bDasConstructionScript"))
                                             ->SetPropertyValue_InContainer(
                                                 Host, EffectiveDeclaration.Callbacks.ContainsByPredicate(
                                                           [](const auto& C)
                                                           { return C.Kind == TEXT("ConstructionScript"); }));
            FindFProperty<FNameProperty>(Host->GetClass(), TEXT("DasSceneRootProperty"))
                                                               ->SetPropertyValue_InContainer(
                                                                   Host, Root ? FName(*Root->Name) : NAME_None);
            CastChecked<UBlueprintGeneratedClass>(BP->GeneratedClass)->UpdateCustomPropertyListForPostConstruction();
        }
    if (Created && (!DasInitializeInheritedDefaults(BP->GeneratedClass, Model, Id, Error) ||
                    !DasInitializeNativeDefaults(BP->GeneratedClass, Model, Id, Error)))
        return false;
    BP->Modify();
    Metadata.SetValue(BP, TEXT("DasDSLSource"), *Script);
    Metadata.SetValue(BP, TEXT("DasDSLClassId"), *Id);
    Metadata.SetValue(BP, TEXT("DasDSLModel"), *Json);
    BP->MarkPackageDirty();
    if (Created)
        FAssetRegistryModule::AssetCreated(BP);
#if WITH_DEV_AUTOMATION_TESTS
    if (Rollback && GDasFailStructuralStage == 2)
        return Restore(TEXT("Injected post-metadata development failure"));
#endif
    if (Rollback && !Rollback->Finish(Error))
    {
        const FString Reason = Error;
        return Restore(Reason);
    }
    if (DasActorNativeParent(EffectiveDeclaration.NativeParent))
    {
        auto* Lifecycle = DasHostString(BP->GeneratedClass->GetDefaultObject(), TEXT("LifecycleScript"));
        const bool Needed = !EffectiveDeclaration.Callbacks.IsEmpty() || EffectiveDeclaration.AsyncPhysics.IsSet();
        if (Lifecycle && (Needed || *Lifecycle == Script))
        {
            *Lifecycle = Needed ? Script : FString();
            CastChecked<UBlueprintGeneratedClass>(BP->GeneratedClass)->UpdateCustomPropertyListForPostConstruction();
        }
    }
    if (Created && Schema.Parent == TEXT("ActorComponent"))
    {
        auto* Component = CastChecked<UDasScriptComponent>(BP->GeneratedClass->GetDefaultObject());
        Component->LifecycleScript = Script;
        Component->bDasConstructionScript = EffectiveDeclaration.Callbacks.ContainsByPredicate(
            [](const auto& C) { return C.Kind == TEXT("ConstructionScript"); });
        CastChecked<UBlueprintGeneratedClass>(BP->GeneratedClass)->UpdateCustomPropertyListForPostConstruction();
    }
    {
        auto* CDO = BP->GeneratedClass->GetDefaultObject();
        auto* Identity = DasHostString(CDO, TEXT("AuthoredTypeId"));
        if (Identity && (Created || Identity->IsEmpty()))
        {
            CDO->Modify();
            *Identity = Id;
            CastChecked<UBlueprintGeneratedClass>(BP->GeneratedClass)->UpdateCustomPropertyListForPostConstruction();
        }
    }
    if (auto* Data = Cast<UDasPrimaryDataAsset>(BP->GeneratedClass->GetDefaultObject()))
    {
        const FString Type = Schema.Metadata.Contains(TEXT("PrimaryAssetType"))
                             ? Schema.Metadata.FindRef(TEXT("PrimaryAssetType")) : Schema.Name.ToString();
        if (Data->DasPrimaryAssetType != FName(*Type))
        {
            Data->Modify();
            Data->DasPrimaryAssetType = FName(*Type);
            CastChecked<UBlueprintGeneratedClass>(BP->GeneratedClass)->UpdateCustomPropertyListForPostConstruction();
        }
    }
    Out = BP;
    Applied = true;
    UE_LOG(LogTemp, Display,
           TEXT("DAS_DSL_APPLY PASS %s (%s)"), *ObjectPath,
                BodyOnly ? TEXT("body-only; no Blueprint recompile"): TEXT("declaration applied"));
    return true;
}
// A change that adds members and also edits default values is applied in two steps: first the new
// declaration with the defaults the Blueprint already has (Out), then only the default values. Returns
// false when the change needs no split.
bool SplitDefaultChanges(UBlueprint* BP, const FString& Source, FDasAuthoringModule& Out)
{
    auto& Runtime = FModuleManager::LoadModuleChecked<FDasRuntimeModule>(TEXT("DasRuntime"));
    FDasAuthoringModule Prior, Next;
    FString Error, PriorJson, NextJson, SplitJson;
    if (!FDasAuthoringModule::FromJson(BP->GetOutermost()->GetMetaData().GetValue(BP, TEXT("DasDSLModel")), Prior,
                                                                                  Error) ||
                                       !Runtime.ReadAuthoringScript(Runtime.ScriptPath(Source), Next, Error) ||
                                       FDasAuthoringModule::DefaultsOnly(Prior, Next, Error))
        return false;
    Out = Next;
    for (auto& C : Out.Classes)
    {
        const auto* Old = Prior.Classes.FindByPredicate([&](const auto& X) { return X.Id == C.Id; });
        if (!Old)
            continue;
        C.NativeDefaults = Old->NativeDefaults;
        C.PropertyOverrides = Old->PropertyOverrides;
        for (auto& P : C.Properties)
            if (const auto* Was = Old->Properties.FindByPredicate([&](const auto& X) { return X.Id == P.Id; }))
                P.Default = Was->Default;
        for (auto& Component : C.Components)
            if (const auto* Was = Old->Components.FindByPredicate([&](const auto& X) { return X.Id == Component.Id; }))
                Component.Defaults = Was->Defaults;
        for (auto& O : C.ComponentOverrides)
            if (const auto* Was = Old->ComponentOverrides.FindByPredicate([&](const auto& X) { return X.Id == O.Id; }))
                O.Defaults = Was->Defaults;
    }
    // Split only when the second step is a pure default update and the first step differs from the source.
    return Out.ToJson(SplitJson, Error) && Next.ToJson(NextJson, Error) && SplitJson != NextJson &&
           FDasAuthoringModule::DefaultsOnly(Out, Next, Error);
}
}

namespace
{
// A change of existing fields that one of the explicit migrations handles on its own.
enum class EFieldMigration
{
    None,
    Rename,
    Remove,
    Numeric,
};
// True when every saved package referencing BP is a DataAsset instance of its class (no children, no
// dependent Blueprints, no maps).
bool OnlyInstanceReferencers(UBlueprint* BP)
{
    TArray<UClass*> Children;
    GetDerivedClasses(BP->GeneratedClass, Children, true);
    TArray<UBlueprint*> Dependents;
    FBlueprintEditorUtils::FindDependentBlueprints(BP, Dependents);
    auto& Registry = FModuleManager::LoadModuleChecked<FAssetRegistryModule>(TEXT("AssetRegistry")).Get();
    if (!Children.IsEmpty() || !Dependents.IsEmpty() || Registry.IsLoadingAssets())
        return false;
    TArray<FName> Referencers;
    Registry.GetReferencers(BP->GetOutermost()->GetFName(), Referencers);
    Referencers.Remove(BP->GetOutermost()->GetFName());
    for (FName P : Referencers)
    {
        TArray<FAssetData> Data;
        Registry.GetAssetsByPackageName(P, Data);
        if (Data.Num() != 1 || !FDasBlueprintRollback::InstanceAsset(Data[0], BP->GeneratedClass))
            return false;
    }
    return !Referencers.IsEmpty();
}
EFieldMigration ClassifyFieldMigration(UBlueprint* BP, const FString& Source, const FString& Id)
{
    auto& Runtime = FModuleManager::LoadModuleChecked<FDasRuntimeModule>(TEXT("DasRuntime"));
    FDasAuthoringModule Prior, Next;
    FString Error;
    if (!FDasAuthoringModule::FromJson(BP->GetOutermost()->GetMetaData().GetValue(BP, TEXT("DasDSLModel")), Prior,
                                                                                  Error) ||
                                       !Runtime.ReadAuthoringScript(Runtime.ScriptPath(Source), Next, Error))
        return EFieldMigration::None;
    const auto* Old = Prior.Classes.FindByPredicate([&](const auto& C) { return C.Id == Id; });
    const auto* New = Next.Classes.FindByPredicate([&](const auto& C) { return C.Id == Id; });
    if (!Old || !New)
        return EFieldMigration::None;
    auto Numeric = [](const FDasDeclaredType& T)
    {
        const auto K = T.GetKind();
        return K == EDasTypeKind::Int || K == EDasTypeKind::Float || K == EDasTypeKind::Double;
    };
    int32 Added = 0, Removed = 0, Renamed = 0, Retyped = 0, Other = 0;
    for (const auto& P : New->Properties)
    {
        const auto* Was = Old->Properties.FindByPredicate([&](const auto& X) { return X.Id == P.Id; });
        if (!Was)
            ++Added;
        else if (P.Type.Identity() != Was->Type.Identity())
            (Numeric(P.Type) && Numeric(Was->Type) && P.Name == Was->Name ? Retyped : Other) += 1;
        else if (P.Name != Was->Name)
            ++Renamed;
    }
    for (const auto& P : Old->Properties)
        if (!New->Properties.ContainsByPredicate([&](const auto& X) { return X.Id == P.Id; }))
        {
            // Events and delegates hold bindings, not values: they are removed by the plain path.
            if (P.Type.Kind == TEXT("event") || P.Type.Kind == TEXT("delegate"))
                return EFieldMigration::None;
            ++Removed;
        }
    // Each explicit migration also checks that its change is the only difference (methods, defaults, ...).
    if (Other || Added)
        return EFieldMigration::None;
    if (Renamed && !Removed && !Retyped)
        return EFieldMigration::Rename;
    if (Removed && !Renamed && !Retyped)
        return EFieldMigration::Remove;
    if (Retyped && !Renamed && !Removed)
        return EFieldMigration::Numeric;
    return EFieldMigration::None;
}
}

bool FDasDSLService::Apply(const FString& Script, const FString& Package, const FString& Id, UBlueprint*& Out,
                           FString& Error, EDasDefaultMigrationPolicy DefaultPolicy)
{
    return ApplyDeclaration(Script, Package, Id, Out, Error, DefaultPolicy, nullptr);
}
namespace
{
// The previous declaration with the new default values of the fields that keep their type. A field migration
// accepts no other difference, so these defaults are applied first.
bool DefaultsBeforeMigration(UBlueprint* BP, const FString& Source, FDasAuthoringModule& Out)
{
    auto& Runtime = FModuleManager::LoadModuleChecked<FDasRuntimeModule>(TEXT("DasRuntime"));
    FDasAuthoringModule Prior, Next;
    FString Error, Before, After;
    if (!FDasAuthoringModule::FromJson(BP->GetOutermost()->GetMetaData().GetValue(BP, TEXT("DasDSLModel")), Prior,
                                                                                  Error) ||
                                       !Runtime.ReadAuthoringScript(Runtime.ScriptPath(Source), Next, Error))
        return false;
    Out = Prior;
    for (auto& C : Out.Classes)
    {
        const auto* N = Next.Classes.FindByPredicate([&](const auto& X) { return X.Id == C.Id; });
        if (!N)
            continue;
        for (auto& P : C.Properties)
            if (const auto* X = N->Properties.FindByPredicate([&](const auto& Y) { return Y.Id == P.Id; });
                X && X->Type.Identity() == P.Type.Identity())
                P.Default = X->Default;
        C.PropertyOverrides = N->PropertyOverrides;
        C.NativeDefaults = N->NativeDefaults;
        for (auto& Component : C.Components)
            if (const auto* X = N->Components.FindByPredicate([&](const auto& Y) { return Y.Id == Component.Id; }))
                Component.Defaults = X->Defaults;
        for (auto& O : C.ComponentOverrides)
            if (const auto* X = N->ComponentOverrides.FindByPredicate([&](const auto& Y) { return Y.Id == O.Id; }))
                O.Defaults = X->Defaults;
    }
    return Out.ToJson(After, Error) && Prior.ToJson(Before, Error) && After != Before &&
           FDasAuthoringModule::DefaultsOnly(Prior, Out, Error);
}
FString PlanLiteral(const FDasDeclaredLiteral& L)
{
    if (L.Kind == TEXT("number"))
        return FString::SanitizeFloat(L.Number, 0);
    if (L.Kind == TEXT("bool"))
        return L.Boolean ? TEXT("true") : TEXT("false");
    if (L.Kind == TEXT("string"))
        return TEXT("\"") + L.String + TEXT("\"");
    return L.Kind.IsEmpty() ? TEXT("(none)") : L.Kind;
}
FString PlanCount(int32 N, const TCHAR* One, const TCHAR* Many)
{
    return FString::Printf(TEXT("%d %s"), N, N == 1 ? One : Many);
}
}

FString FDasReimportPlan::Summary() const
{
    TArray<FString> Parts;
    auto Add = [&](int32 N, const TCHAR* Text)
    {
        if (N)
            Parts.Add(FString::Printf(TEXT("%s %d"), Text, N));
    };
    Add(Removed.Num(), TEXT("removed"));
    Add(Renamed.Num(), TEXT("renamed"));
    Add(Retyped.Num(), TEXT("retyped"));
    Add(Added.Num(), TEXT("added"));
    Add(Defaults.Num(), TEXT("new defaults"));
    if (OtherDefaults)
        Parts.Add(TEXT("other default values"));
    FString Text = FString::Join(Parts, TEXT(", "));
    if (!Text.IsEmpty())
        Text[0] = FChar::ToUpper(Text[0]);
    return Text;
}
FString FDasReimportPlan::Short(int32 MaxFields, int32 MaxAssets) const
{
    TArray<FString> Lines{Summary()};
    int32 Shown = 0, Total = 0;
    auto Group = [&](const TArray<FString>& Items, const TCHAR* Mark, const TCHAR* Suffix)
    {
        for (const FString& Item : Items)
        {
            ++Total;
            if (Shown < MaxFields)
            {
                Lines.Add(FString::Printf(TEXT("%s %s%s"), Mark, *Item, Suffix));
                ++Shown;
            }
        }
    };
    // Most significant first: removals lose values (archived, restorable), defaults change nothing placed.
    Group(Removed, TEXT("-"), TEXT(": removed (values archived)"));
    Group(Renamed, TEXT("~"), TEXT(""));
    Group(Retyped, TEXT("~"), TEXT(""));
    Group(Added, TEXT("+"), TEXT(""));
    Group(Defaults, TEXT("="), TEXT(""));
    if (Total > Shown)
        Lines.Add(FString::Printf(TEXT("...and %d more"), Total - Shown));
    TArray<FString> Assets = Maps;
    Assets.Append(Blueprints);
    if (!Assets.IsEmpty())
    {
        if (Assets.Num() <= MaxAssets)
        {
            TArray<FString> Names;
            for (const FString& A : Assets)
                Names.Add(FPackageName::GetShortName(A));
            Lines.Add(TEXT("Will modify: ") + FString::Join(Names, TEXT(", ")));
        }
        else
            Lines.Add(
                FString::Printf(TEXT("Will modify %d assets: %s, %s"), Assets.Num(),
                                     *PlanCount(Maps.Num(), TEXT("map"), TEXT("maps")),
                                                *PlanCount(Blueprints.Num(), TEXT("Blueprint"), TEXT("Blueprints"))));
    }
    return FString::Join(Lines, TEXT("\n"));
}
FString FDasReimportPlan::Details(int32 MaxPerGroup) const
{
    FString Text = Summary() + TEXT("\n");
    auto Group = [&](const TCHAR* Title, const TArray<FString>& Items)
    {
        if (Items.IsEmpty())
            return;
        Text += FString::Printf(TEXT("\n%s (%d):\n"), Title, Items.Num());
        for (int32 I = 0; I < Items.Num() && I < MaxPerGroup; ++I)
            Text += TEXT("    ") + Items[I] + TEXT("\n");
        if (Items.Num() > MaxPerGroup)
            Text += FString::Printf(TEXT("    ...and %d more (the full list is in the Output Log)\n"),
                                         Items.Num() - MaxPerGroup);
    };
    Group(TEXT("Removed fields; their values are archived on each instance and can be restored"), Removed);
    Group(TEXT("Renamed fields"), Renamed);
    Group(TEXT("Retyped fields"), Retyped);
    Group(TEXT("Added fields"), Added);
    Group(TEXT("New default values"), Defaults);
    if (OtherDefaults)
        Text += TEXT("\nOther default values (components, inherited or native properties) change.\n");
    Group(TEXT("Maps that will be modified"), Maps);
    Group(TEXT("Blueprints that will be modified"), Blueprints);
    if (!Maps.IsEmpty() || !Blueprints.IsEmpty())
        Text += TEXT("\nModified assets stay unsaved until you save them. A failure rolls everything back.\n");
    return Text;
}

bool FDasDSLService::PlanReimport(UBlueprint* BP, FDasReimportPlan& Out, FString& Error)
{
    Out = FDasReimportPlan();
    auto& Runtime = FModuleManager::LoadModuleChecked<FDasRuntimeModule>(TEXT("DasRuntime"));
    auto& Metadata = BP->GetOutermost()->GetMetaData();
    const FString Source = Metadata.GetValue(BP, TEXT("DasDSLSource")),
                                             Id = Metadata.GetValue(BP, TEXT("DasDSLClassId"));
    FDasAuthoringModule Prior, Next;
    if (Source.IsEmpty() ||
        !FDasAuthoringModule::FromJson(Metadata.GetValue(BP, TEXT("DasDSLModel")), Prior, Error) ||
                                       !Runtime.ReadAuthoringScript(Runtime.ScriptPath(Source), Next, Error))
        return false;
    const auto* Old = Prior.Classes.FindByPredicate([&](const auto& C) { return C.Id == Id; });
    const auto* New = Next.Classes.FindByPredicate([&](const auto& C) { return C.Id == Id; });
    if (!Old || !New)
    {
        Error = TEXT("Class is missing from the script");
        return false;
    }
    for (const auto& P : New->Properties)
    {
        const auto* Was = Old->Properties.FindByPredicate([&](const auto& X) { return X.Id == P.Id; });
        if (!Was)
            Out.Added.Add(P.Name);
        else if (P.Type.Identity() != Was->Type.Identity())
            Out.Retyped.Add(FString::Printf(TEXT("%s: %s -> %s"), *P.Name, *Was->Type.Identity(), *P.Type.Identity()));
        else
        {
            if (P.Name != Was->Name)
                Out.Renamed.Add(Was->Name + TEXT(" -> ") + P.Name);
            if (!(P.Default == Was->Default))
                Out.Defaults.Add(FString::Printf(TEXT("%s: %s -> %s"), *P.Name, *PlanLiteral(Was->Default),
                                                      *PlanLiteral(P.Default)));
        }
    }
    for (const auto& P : Old->Properties)
        if (!New->Properties.ContainsByPredicate([&](const auto& X) { return X.Id == P.Id; }))
            Out.Removed.Add(P.Name);
    FDasAuthoringModule Defaults;
    if (DefaultsBeforeMigration(BP, Source, Defaults))
    {
        // Defaults beyond the literal field values: components, inherited and native properties.
        FDasAuthoringModule Literal = Prior;
        for (auto& C : Literal.Classes)
            if (const auto* N = Defaults.Classes.FindByPredicate([&](const auto& X) { return X.Id == C.Id; }))
                C.Properties = N->Properties;
        FString A, B;
        Out.OtherDefaults = Literal.ToJson(A, Error) && Defaults.ToJson(B, Error) && A != B;
        Error.Reset();
    }
    // Saved assets outside the class and its own instances.
    TArray<UClass*> Children;
    GetDerivedClasses(BP->GeneratedClass, Children, true);
    for (auto* C : Children)
        if (auto* B = Cast<UBlueprint>(C->ClassGeneratedBy))
            Out.Blueprints.AddUnique(B->GetOutermost()->GetName());
    TArray<UBlueprint*> Dependents;
    FBlueprintEditorUtils::FindDependentBlueprints(BP, Dependents);
    for (auto* B : Dependents)
        Out.Blueprints.AddUnique(B->GetOutermost()->GetName());
    auto& Registry = FModuleManager::LoadModuleChecked<FAssetRegistryModule>(TEXT("AssetRegistry")).Get();
    TArray<FName> Referencers;
    Registry.GetReferencers(BP->GetOutermost()->GetFName(), Referencers);
    for (FName P : Referencers)
    {
        if (P == BP->GetOutermost()->GetFName())
            continue;
        TArray<FAssetData> Data;
        Registry.GetAssetsByPackageName(P, Data);
        if (Data.Num() == 1 && FDasBlueprintRollback::InstanceAsset(Data[0], BP->GeneratedClass))
            continue;
        if (Data.Num() == 1 && Data[0].AssetClassPath == UWorld::StaticClass()->GetClassPathName())
            Out.Maps.AddUnique(P.ToString());
        else
            Out.Blueprints.AddUnique(P.ToString());
    }
    Out.NeedsConfirmation = ClassifyFieldMigration(BP, Source, Id) != EFieldMigration::None &&
                            (!Out.Maps.IsEmpty() || !Out.Blueprints.IsEmpty());
    return true;
}

bool FDasDSLService::Reimport(UBlueprint* BP, FString& Error, EDasDefaultMigrationPolicy DefaultPolicy,
                              bool IncludeDependencies)
{
    if (!IsInGameThread() || !IsValid(BP))
    {
        Error = TEXT("Expected valid editor Blueprint");
        return false;
    }
    auto& Metadata = BP->GetOutermost()->GetMetaData();
    const FString Source = Metadata.GetValue(BP, TEXT("DasDSLSource")),
                                             Id = Metadata.GetValue(BP, TEXT("DasDSLClassId"));
    if (Source.IsEmpty() || Id.IsEmpty())
    {
        Error = TEXT("Blueprint has no DSL import source");
        return false;
    }
    UBlueprint* Result = nullptr;
    const FString Package = BP->GetOutermost()->GetName();
    // Renaming, removing or numerically retyping existing fields: the matching explicit migration keeps the
    // values (renamed, archived for a later restore, exactly converted) instead of a refusal. New default values
    // in the same save are applied first: the migration accepts no other difference.
    if (DefaultPolicy == EDasDefaultMigrationPolicy::Automatic)
    {
        const EFieldMigration Migration = ClassifyFieldMigration(BP, Source, Id);
        if (Migration != EFieldMigration::None)
        {
            FKeepPossession Possession(BP->GeneratedClass);
            FDasAuthoringModule Defaults;
            if (DefaultsBeforeMigration(BP, Source, Defaults) &&
                !ApplyDeclaration(Source, Package, Id, Result, Error, DefaultPolicy, &Defaults))
                return false;
            // Saved DataAsset instances of the class are migrated with it; other saved referencers only when the
            // caller allows it (auto-reimport asks first).
            const bool Dependencies = IncludeDependencies || OnlyInstanceReferencers(BP);
            switch (Migration)
            {
            case EFieldMigration::Rename:
                return ReimportRenamedFields(BP, Error, Dependencies);
            case EFieldMigration::Remove:
                return ReimportArchivedFields(BP, false, Dependencies, Error);
            case EFieldMigration::Numeric:
            {
                // Exact rules: a value that the new type cannot hold exactly refuses the change.
                FDasNumericMigrationOptions Options;
                Options.IncludeDependencies = Dependencies;
                return ReimportConvertedNumericFields(BP, Options, Error);
            }
            case EFieldMigration::None:
                break;
            }
        }
    }
    FDasAuthoringModule Declaration;
    if (DefaultPolicy == EDasDefaultMigrationPolicy::Automatic && SplitDefaultChanges(BP, Source, Declaration))
    {
        if (!ApplyDeclaration(Source, Package, Id, Result, Error, DefaultPolicy, &Declaration))
            return false;
        if (!ApplyDeclaration(Source, Package, Id, Result, Error, DefaultPolicy, nullptr))
        {
            Error = TEXT("New members applied; default values not yet: ")+ Error;
            return false;
        }
        return true;
    }
    return Apply(Source, Package, Id, Result, Error, DefaultPolicy);
}

namespace
{
bool CaptureRenamedField(UObject* Object, FName Name, EDasScalarKind Kind, FDasScalarValue& Out, FString& Error)
{
    auto* P = IsValid(Object) ? Object->GetClass()->FindPropertyByName(Name) : nullptr;
    if (!P)
    {
        Error = TEXT("Missing migration field: ")+ Name.ToString();
        return false;
    }
    FDasScalarValue Value;
    if (!ReadDasPropertyValue(P, P->ContainerPtrToValuePtr<void>(Object), Value, Error))
        return false;
    if (Value.Kind != int32(Kind))
    {
        Error = TEXT("Migration field kind differs from declaration: ")+ Name.ToString();
        return false;
    }
    Out = MoveTemp(Value);
    return true;
}
bool RestoreRenamedField(UObject* Object, FName Name, const FDasScalarValue& Value)
{
    auto* P = IsValid(Object) ? Object->GetClass()->FindPropertyByName(Name) : nullptr;
    FString Error;
    return P && WriteDasPropertyValue(P, P->ContainerPtrToValuePtr<void>(Object), Value, Error);
}
}
bool FDasDSLService::ReimportRenamedFields(UBlueprint* BP, FString& Error, bool IncludeDependencies)
{
    Error.Reset();
    if (!IsInGameThread() || !IsValid(BP) || !BP->GeneratedClass)
    {
        Error = TEXT("Expected valid imported Blueprint on game thread");
        return false;
    }
    auto& Runtime = FModuleManager::LoadModuleChecked<FDasRuntimeModule>(TEXT("DasRuntime"));
    auto& Meta = BP->GetOutermost()->GetMetaData();
    const FString Script = Meta.GetValue(BP, TEXT("DasDSLSource")), Id = Meta.GetValue(BP, TEXT("DasDSLClassId"));
    if (Script.IsEmpty() || FPaths::GetCleanFilename(Script) != Script)
    {
        Error = TEXT("Blueprint has no local DSL source");
        return false;
    }
    FDasAuthoringModule Prior, Next;
    FDasClassSchema Previous, Candidate;
    FString Json;
    if (!FDasAuthoringModule::FromJson(Meta.GetValue(BP, TEXT("DasDSLModel")), Prior, Error) ||
                                       !Runtime.ReadAuthoringScript(Runtime.ScriptPath(Script), Next, Error) ||
                                       !Prior.ToActorSchema(Id, Previous, Error) ||
                                       !Next.ToActorSchema(Id, Candidate, Error) || !Next.ToJson(Json, Error))
        return false;
    if (Previous.ParentStableId != Candidate.ParentStableId)
    {
        Error = TEXT("Changing the parent together with a field migration is deferred; save them separately");
        return false;
    }
    for (const auto& M : Candidate.EffectiveMethods())
        if (!Runtime.ValidateFrameMethod(Runtime.ScriptPath(M.ScriptFile), Error, M.EntryPoint, M.Contract()))
            return false;
    if (!FDasBlueprintService::Apply(BP, Previous, Error, true))
        return false;
    const auto* Old = Prior.Classes.FindByPredicate([&](const auto& C) { return C.Id == Id; });
    auto* New = Next.Classes.FindByPredicate([&](const auto& C) { return C.Id == Id; });
    if (!Old || !New || Prior.Id != Next.Id)
    {
        Error = TEXT("Rename migration requires matching stored class/module identity");
        return false;
    }
    struct FRename
    {
        FName OldName, NewName;
        EDasScalarKind Kind;
    };
    TArray<FRename> Renames;
    auto Normalized = *New;
    for (auto& Property : Normalized.Properties)
    {
        const auto* Before = Old->Properties.FindByPredicate([&](const auto& P) { return P.Id == Property.Id; });
        if (!Before)
        {
            Error = TEXT("Rename requires unchanged stable IDs; use ue_id equal to the prior property ID");
            return false;
        }
        if (Before->Name != Property.Name)
        {
            FName From(*Before->Name), To(*Property.Name);
            if (From == To || BP->GeneratedClass->FindPropertyByName(To) || BP->GeneratedClass->FindFunctionByName(To))
            {
                Error = TEXT("Rename destination already exists or differs only by case: ")+ Property.Name;
                return false;
            }
            const auto* Field =
                Previous.Fields.FindByPredicate([&](const auto& F) { return F.StableId == Property.Id; });
            const auto* Variable = BP->NewVariables.FindByPredicate([&](const auto& V) { return V.VarName == From; });
            if (!Field || !Variable || !Variable->RepNotifyFunc.IsNone() || (Variable->PropertyFlags & CPF_Net))
            {
                Error = TEXT("Rename supports authored non-replicated variables only");
                return false;
            }
            Renames.Add({From, To, Field->Kind});
            Property.Name = Before->Name;
        }
    }
    const auto Delta = FDasAuthoringModule::Compare(*Old, Normalized);
    if (Delta.bStructural || Delta.bDefaults || Delta.bMetadata || Delta.bImplementation)
    {
        Error = TEXT("Rename migration accepts only field names; type/default/flags/method changes must be separate");
        return false;
    }
    if (Renames.IsEmpty())
    {
        UBlueprint* Result = nullptr;
        return Apply(Script, BP->GetOutermost()->GetName(), Id, Result, Error);
    }
    TArray<UClass*> Children;
    GetDerivedClasses(BP->GeneratedClass, Children, true);
    TArray<UBlueprint*> Dependents;
    FBlueprintEditorUtils::FindDependentBlueprints(BP, Dependents);
    auto& Registry = FModuleManager::LoadModuleChecked<FAssetRegistryModule>(TEXT("AssetRegistry")).Get();
    if (!Registry.IsSearchAllAssets())
        Registry.SearchAllAssets(true);
    else
        Registry.WaitForCompletion();
    if (Registry.IsLoadingAssets())
    {
        Error = TEXT("Asset Registry scan must finish before rename migration");
        return false;
    }
    TArray<FName> Referencers;
    Registry.GetReferencers(BP->GetOutermost()->GetFName(), Referencers);
    if (!IncludeDependencies &&
        (!Children.IsEmpty() || !Dependents.IsEmpty() ||
         Referencers.ContainsByPredicate([&](FName P) { return P != BP->GetOutermost()->GetFName(); })))
    {
        Error =
            TEXT("Rename of dependent Blueprints or saved referencing assets requires fields=rename_with_dependencies");
        return false;
    }
    TArray<TStrongObjectPtr<UObject>> ScopeAssets;
    TArray<UBlueprint*> ScopeBlueprints;
    ScopeBlueprints.Add(BP);
    TSet<FName> ScopePackages;
    ScopePackages.Add(BP->GetOutermost()->GetFName());
    if (IncludeDependencies)
    {
        TArray<FName> Queue;
        Queue.Add(BP->GetOutermost()->GetFName());
        // Include unsaved/modified loaded dependencies before walking saved
        // referencers, so maps referring to those packages are included too.
        for (auto* Class : Children)
            if (auto* B = Cast<UBlueprint>(Class->ClassGeneratedBy))
                Dependents.AddUnique(B);
        for (auto* B : Dependents)
        {
            ScopeBlueprints.AddUnique(B);
            ScopeAssets.Emplace(B);
            const FName Package = B->GetOutermost()->GetFName();
            if (!ScopePackages.Contains(Package))
            {
                if (Queue.Num() >= 256 || (!Package.ToString().StartsWith(
                                              TEXT("/Game/")) && FPackageName::DoesPackageExist(Package.ToString())))
                {
                    Error = TEXT("Loaded dependent package exceeds supported rename scope: ")+ Package.ToString();
                    return false;
                }
                ScopePackages.Add(Package);
                Queue.Add(Package);
            }
        }

        for (int32 Index = 0; Index < Queue.Num(); ++Index)
        {
            TArray<FName> References;
            Registry.GetReferencers(Queue[Index], References);
            for (FName Package : References)
                if (!ScopePackages.Contains(Package))
                {
                    if (!Package.ToString().StartsWith(TEXT("/Game/")) || Queue.Num() >= 256)
                    {
                        Error = TEXT("Dependent rename requires a bounded /Game package scope: ")+ Package.ToString();
                        return false;
                    }
                    ScopePackages.Add(Package);
                    Queue.Add(Package);
                    TArray<FAssetData> Assets;
                    Registry.GetAssetsByPackageName(Package, Assets);
                    if (Assets.Num() != 1 ||
                        (Assets[0].AssetClassPath != UBlueprint::StaticClass()->GetClassPathName() &&
                         Assets[0].AssetClassPath != UWorld::StaticClass()->GetClassPathName() &&
                         !FDasBlueprintRollback::InstanceAsset(Assets[0], BP->GeneratedClass)))
                    {
                        Error = TEXT("Unsupported dependent package; only Blueprint, map and DataAsset instance assets supported: ")+
                            Package.ToString();
                        return false;
                    }
                    UObject* Asset = Assets[0].GetAsset();
                    if (!Asset)
                    {
                        Error = TEXT("Could not load dependent asset: ")+ Package.ToString();
                        return false;
                    }
                    ScopeAssets.Emplace(Asset);
                    if (auto* B = Cast<UBlueprint>(Asset))
                    {
                        if (!B->GeneratedClass || B->Status == BS_Error ||
                            !B->GetOutermost()->GetMetaData().GetValue(B, TEXT("DasDSLSource")).IsEmpty())
                        {
                            Error = TEXT("Dependent Blueprint must be valid and have no separate DSL source: ")+
                                         Package.ToString();
                            return false;
                        }
                        ScopeBlueprints.AddUnique(B);
                    }
                    else if (auto* W = Cast<UWorld>(Asset))
                    {
                        if (W->IsPartitionedWorld() || !W->GetStreamingLevels().IsEmpty())
                        {
                            Error = TEXT("World Partition/streaming map migration requires a separate scope: ")+
                                         Package.ToString();
                            return false;
                        }
                    }
                }
        }
        Dependents.Reset();
        FBlueprintEditorUtils::FindDependentBlueprints(BP, Dependents);
        for (auto* B : Dependents)
        {
            ScopeBlueprints.AddUnique(B);
            ScopePackages.Add(B->GetOutermost()->GetFName());
            ScopeAssets.Emplace(B);
        }
        Children.Reset();
        GetDerivedClasses(BP->GeneratedClass, Children, true);
        for (auto* Class : Children)
        {
            auto* Child = Cast<UBlueprint>(Class->ClassGeneratedBy);
            if (!Child || !ScopeBlueprints.Contains(Child))
            {
                Error = TEXT("Unmanaged derived class in rename scope");
                return false;
            }
        }
        for (auto* B : ScopeBlueprints)
            if (B != BP)
            {
                if (!B->GeneratedClass || B->Status == BS_Error)
                {
                    Error = TEXT("Dependent Blueprint is not valid for migration: ")+ B->GetPathName();
                    return false;
                }
                if (!B->GetOutermost()->GetMetaData().GetValue(B, TEXT("DasDSLSource")).IsEmpty())
                {
                    Error = TEXT("Dependent DSL source requires an explicit source migration");
                    return false;
                }
                for (const auto& R : Renames)
                    if (B->GeneratedClass && (B->GeneratedClass->FindPropertyByName(R.NewName) ||
                                              B->GeneratedClass->FindFunctionByName(R.NewName)))
                    {
                        Error = TEXT("Rename destination conflicts in dependent Blueprint: ")+ B->GetPathName();
                        return false;
                    }
            }
        ScopeBlueprints.Sort(
            [&](const UBlueprint& A, const UBlueprint& B)
            {
                if (&A == BP)
                    return &B != BP;
                if (&B == BP)
                    return false;
                auto Depth = [](const UClass* C)
                {
                    int32 N = 0;
                    while (C)
                    {
                        ++N;
                        C = C->GetSuperClass();
                    }
                    return N;
                };
                return Depth(A.GeneratedClass) < Depth(B.GeneratedClass);
            });
        for (FName Package : ScopePackages)
            UE_LOG(LogTemp, Display, TEXT("DAS_RENAME_SCOPE %s"), *Package.ToString());
    }
    struct FSnapshot
    {
        TStrongObjectPtr<UObject> Object;
        TArray<FDasScalarValue> Values;
    };
    TArray<FSnapshot> Snapshots;
    TArray<UObject*> Objects;
    GetObjectsOfClass(BP->GeneratedClass, Objects, IncludeDependencies, RF_NoFlags);
    if (IncludeDependencies)
        for (auto* Object : Objects)
            if (auto* Actor = Cast<AActor>(Object))
                if (auto* World = Actor->GetWorld())
                    if (World->IsPartitionedWorld() || !World->GetStreamingLevels().IsEmpty())
                    {
                        Error = TEXT("Loaded World Partition/streaming instances require a separate migration scope");
                        return false;
                    }
                    else if (!ScopePackages.Contains(World->GetOutermost()->GetFName()))
                    {
                        ScopePackages.Add(World->GetOutermost()->GetFName());
                        UE_LOG(LogTemp, Display,
                               TEXT("DAS_RENAME_SCOPE live map %s"), *World->GetOutermost()->GetName());
                    }
    for (auto* Object : Objects)
        if (IsValid(Object) && !Object->GetClass()->HasAnyClassFlags(CLASS_NewerVersionExists) &&
            !Object->GetClass()->GetName().StartsWith(TEXT("SKEL_")) &&
                                                      !Object->GetClass()->GetName().StartsWith(TEXT("REINST_")))
        {
            FSnapshot S;
            S.Object.Reset(Object);
            for (const auto& R : Renames)
            {
                FDasScalarValue Value;
                if (!CaptureRenamedField(Object, R.OldName, R.Kind, Value, Error))
                    return false;
                S.Values.Add(MoveTemp(Value));
            }
            Snapshots.Add(MoveTemp(S));
        }
    FDasPropertyValueRoots ObjectPins;
    for (const auto& S : Snapshots)
        for (const auto& V : S.Values)
            ObjectPins.Keep(V);
    const auto Handle = FCoreUObjectDelegates::OnObjectsReplaced.AddLambda(
        [&](const FCoreUObjectDelegates::FReplacementObjectMap& Map)
        {
            for (auto& S : Snapshots)
            {
                if (auto* Replacement = Map.Find(S.Object.Get()))
                    S.Object.Reset(*Replacement);
                for (auto& V : S.Values)
                    FDasPropertyValueRoots::Remap(V, Map);
            }
            ObjectPins.RemapPins(Map);
        });
    ON_SCOPE_EXIT
    {
        FCoreUObjectDelegates::OnObjectsReplaced.Remove(Handle);
    };
    FDasBlueprintRollback Rollback;
    if (!Rollback.Prepare(BP, Previous, ScopeBlueprints, ScopeAssets, Error))
        return false;
    auto Restore = [&](const FString& Reason)
    {
        FString E;
        const bool OK = Rollback.Restore(E);
        Error = (OK ? TEXT("Rename migration rolled back: "): TEXT("Rename rollback incomplete: ")) + Reason;
        if (!E.IsEmpty())
            Error += TEXT("; ")+ E;
        UE_LOG(LogTemp, Display, TEXT("DAS_RENAME_ROLLBACK=%s"), OK ? TEXT("PASS"): TEXT("FAIL"));
        return false;
    };
    const FDasAuthoringModule Approval = DasApprovalModel(Next, New->Id);
    FString ApprovalWire;
    if (!Approval.ToJson(ApprovalWire, Error))
        return false;
    if (!Runtime.BeginPreserveContextsForReinstance(BP->GeneratedClass))
    {
        Error = TEXT("Field rename requires idle runtime with no active call frames");
        return false;
    }
    bool Renamed = false;
    ON_SCOPE_EXIT
    {
        Runtime.EndPreserveContextsForReinstance(Renamed ? ApprovalWire : FString());
    };
    for (auto* B : ScopeBlueprints)
        B->Modify();
    for (auto& S : Snapshots)
        S.Object->Modify();
    BP->Modify();
    for (const auto& R : Renames)
        FBlueprintEditorUtils::RenameMemberVariable(BP, R.OldName, R.NewName);
#if WITH_DEV_AUTOMATION_TESTS
    if (GDasFailRenameStage == 1)
        return Restore(TEXT("Injected post-rename development failure"));
#endif
    for (auto* B : ScopeBlueprints)
    {
        FKismetEditorUtilities::CompileBlueprint(B);
        FBlueprintCompilationManager::FlushCompilationQueueAndReinstance();
        if (B->Status == BS_Error || !B->GeneratedClass)
        {
            return Restore(TEXT("Dependent rename compilation failed: ")+ B->GetPathName());
        }
    }
    if (BP->Status == BS_Error || !BP->GeneratedClass)
    {
        return Restore(TEXT("Rename compilation failed"));
    }
#if WITH_DEV_AUTOMATION_TESTS
    if (GDasFailRenameStage == 2)
        return Restore(TEXT("Injected post-compile development failure"));
#endif
    for (auto& S : Snapshots)
        for (int32 I = 0; I < Renames.Num(); ++I)
            if (!RestoreRenamedField(S.Object.Get(), Renames[I].NewName, S.Values[I]))
            {
                return Restore(TEXT("Rename instance value restoration failed"));
            }
    for (auto* B : ScopeBlueprints)
        B->MarkPackageDirty();
    for (auto& S : Snapshots)
        S.Object->MarkPackageDirty();
    for (auto& Asset : ScopeAssets)
        Asset->MarkPackageDirty();
    if (!FDasBlueprintService::Apply(BP, Candidate, Error, true))
    {
        const FString Reason = Error;
        return Restore(Reason);
    }
    BP->GetOutermost()->GetMetaData().SetValue(BP, TEXT("DasDSLModel"), *Json);
    BP->MarkPackageDirty();
#if WITH_DEV_AUTOMATION_TESTS
    if (GDasFailRenameStage == 3)
        return Restore(TEXT("Injected post-metadata development failure"));
#endif
    Renamed = true;
    UE_LOG(LogTemp, Display, TEXT("DAS_FIELD_RENAME PASS %d fields; instance values preserved"), Renames.Num());
    return true;
}

bool FDasDSLService::ReimportWidenedNumericFields(UBlueprint* BP, FString& Error, bool IncludeDependencies)
{
    FDasNumericMigrationOptions O;
    O.IncludeDependencies = IncludeDependencies;
    O.AdaptGraphs = false;
    O.WidenOnly = true;
    return ReimportConvertedNumericFields(BP, O, Error);
}
bool FDasDSLService::ReimportConvertedNumericFields(UBlueprint* BP, const FDasNumericMigrationOptions& Options,
                                                    FString& Error)
{
    const bool IncludeDependencies = Options.IncludeDependencies;
    if (uint8(Options.Integers) > 4 || uint8(Options.Floats) > 1)
    {
        Error = TEXT("Unknown numeric rounding/precision policy");
        return false;
    }
    Error.Reset();
    if (!IsInGameThread() || !IsValid(BP) || !BP->GeneratedClass)
    {
        Error = TEXT("Expected valid imported Blueprint on game thread");
        return false;
    }
    auto& Runtime = FModuleManager::LoadModuleChecked<FDasRuntimeModule>(TEXT("DasRuntime"));
    auto& Meta = BP->GetOutermost()->GetMetaData();
    const FString Script = Meta.GetValue(BP, TEXT("DasDSLSource")), Id = Meta.GetValue(BP, TEXT("DasDSLClassId"));
    if (Script.IsEmpty() || FPaths::GetCleanFilename(Script) != Script)
    {
        Error = TEXT("Blueprint has no local DSL source");
        return false;
    }
    FDasAuthoringModule Prior, Next;
    FDasClassSchema Previous, Candidate;
    FString Json;
    if (!FDasAuthoringModule::FromJson(Meta.GetValue(BP, TEXT("DasDSLModel")), Prior, Error) ||
                                       !Runtime.ReadAuthoringScript(Runtime.ScriptPath(Script), Next, Error) ||
                                       !Prior.ToActorSchema(Id, Previous, Error) ||
                                       !Next.ToActorSchema(Id, Candidate, Error) || !Next.ToJson(Json, Error))
        return false;
    if (Previous.ParentStableId != Candidate.ParentStableId)
    {
        Error = TEXT("Changing the parent together with a field migration is deferred; save them separately");
        return false;
    }
    for (const auto& M : Candidate.EffectiveMethods())
        if (!Runtime.ValidateFrameMethod(Runtime.ScriptPath(M.ScriptFile), Error, M.EntryPoint, M.Contract()))
            return false;
    if (!FDasBlueprintService::Apply(BP, Previous, Error, true))
        return false;
    const auto* Old = Prior.Classes.FindByPredicate([&](const auto& C) { return C.Id == Id; });
    auto* New = Next.Classes.FindByPredicate([&](const auto& C) { return C.Id == Id; });
    if (!Old || !New || Prior.Id != Next.Id)
    {
        Error = TEXT("Rename migration requires matching stored class/module identity");
        return false;
    }
    struct FConversion
    {
        FName OldName, NewName;
        EDasScalarKind Kind, Target;
    };
    TArray<FConversion> Renames;
    auto Normalized = *New;
    for (auto& Property : Normalized.Properties)
    {
        const auto* Before = Old->Properties.FindByPredicate([&](const auto& P) { return P.Id == Property.Id; });
        if (!Before || Property.Name != Before->Name)
        {
            Error = TEXT("Numeric widening requires unchanged property names and stable IDs");
            return false;
        }
        if (Property.Type.Identity() != Before->Type.Identity())
        {
            auto Numeric = [](const FString& K)
            {
                return K == TEXT("int") || K == TEXT("float") || K == TEXT("double");
            };
            if (!Numeric(Property.Type.Kind) || !Numeric(Before->Type.Kind) ||
                (Options.WidenOnly &&
                 (Property.Type.Kind != TEXT("double") ||
                                             (Before->Type.Kind != TEXT("int") && Before->Type.Kind != TEXT("float")))))
            {
                Error = TEXT("Unsupported numeric field conversion");
                return false;
            }
            const auto* Field =
                Previous.Fields.FindByPredicate([&](const auto& F) { return F.StableId == Property.Id; });
            const auto* Variable =
                BP->NewVariables.FindByPredicate([&](const auto& V) { return V.VarName == FName(*Property.Name); });
            if (!Field || !Variable || !Variable->RepNotifyFunc.IsNone() || (Variable->PropertyFlags & CPF_Net))
            {
                Error = TEXT("Numeric widening requires an authored non-replicated field");
                return false;
            }
            const auto* Target =
                Candidate.Fields.FindByPredicate([&](const auto& F) { return F.StableId == Property.Id; });
            FDasScalarValue Default, Converted;
            if (!CaptureRenamedField(BP->GeneratedClass->GetDefaultObject(), Field->Name, Field->Kind, Default, Error))
                return false;
            if (!Target ||
                !ConvertDasNumber(Default, int32(Target->Kind), Options.Integers, Options.Floats, Converted, Error))
                return false;
            const double Expected = Converted.Kind == 0   ? double(Converted.IntValue)
                                    : Converted.Kind == 1 ? double(Converted.FloatValue)
                                                          : Converted.DoubleValue;
            const double Declared = Target->Kind == EDasScalarKind::Int32     ? double(Target->IntDefault)
                                    : Target->Kind == EDasScalarKind::Float32 ? double(Target->FloatDefault)
                                                                              : Target->DoubleDefault;
            if (Expected != Declared)
            {
                Error = FString::Printf(TEXT("New default must equal policy-converted prior default %.17g: %s"),
                                             Expected, *Property.Name);
                return false;
            }
            Renames.Add({FName(*Property.Name), FName(*Property.Name), Field->Kind, Target->Kind});
            Property.Type = Before->Type;
            Property.Default = Before->Default;
        }
    }
    const auto Delta = FDasAuthoringModule::Compare(*Old, Normalized);
    if (Delta.bStructural || Delta.bDefaults || Delta.bMetadata || Delta.bImplementation)
    {
        Error = TEXT("Widening accepts only field types and exact promoted defaults; all other declaration changes must be separate");
        return false;
    }
    if (Renames.IsEmpty())
    {
        UBlueprint* Result = nullptr;
        return Apply(Script, BP->GetOutermost()->GetName(), Id, Result, Error);
    }
    TArray<UClass*> Children;
    GetDerivedClasses(BP->GeneratedClass, Children, true);
    TArray<UBlueprint*> Dependents;
    FBlueprintEditorUtils::FindDependentBlueprints(BP, Dependents);
    auto& Registry = FModuleManager::LoadModuleChecked<FAssetRegistryModule>(TEXT("AssetRegistry")).Get();
    if (!Registry.IsSearchAllAssets())
        Registry.SearchAllAssets(true);
    else
        Registry.WaitForCompletion();
    if (Registry.IsLoadingAssets())
    {
        Error = TEXT("Asset Registry scan must finish before numeric widening migration");
        return false;
    }
    TArray<FName> Referencers;
    Registry.GetReferencers(BP->GetOutermost()->GetFName(), Referencers);
    if (!IncludeDependencies &&
        (!Children.IsEmpty() || !Dependents.IsEmpty() ||
         Referencers.ContainsByPredicate([&](FName P) { return P != BP->GetOutermost()->GetFName(); })))
    {
        Error = TEXT("Rename of dependent Blueprints or saved referencing assets requires fields=widen_numeric_with_dependencies");
        return false;
    }
    TArray<TStrongObjectPtr<UObject>> ScopeAssets;
    TArray<UBlueprint*> ScopeBlueprints;
    ScopeBlueprints.Add(BP);
    TSet<FName> ScopePackages;
    ScopePackages.Add(BP->GetOutermost()->GetFName());
    if (IncludeDependencies)
    {
        TArray<FName> Queue;
        Queue.Add(BP->GetOutermost()->GetFName());
        // Include unsaved/modified loaded dependencies before walking saved
        // referencers, so maps referring to those packages are included too.
        for (auto* Class : Children)
            if (auto* B = Cast<UBlueprint>(Class->ClassGeneratedBy))
                Dependents.AddUnique(B);
        for (auto* B : Dependents)
        {
            ScopeBlueprints.AddUnique(B);
            ScopeAssets.Emplace(B);
            const FName Package = B->GetOutermost()->GetFName();
            if (!ScopePackages.Contains(Package))
            {
                if (Queue.Num() >= 256 || (!Package.ToString().StartsWith(
                                              TEXT("/Game/")) && FPackageName::DoesPackageExist(Package.ToString())))
                {
                    Error =
                        TEXT("Loaded dependent package exceeds supported numeric widening scope: ")+ Package.ToString();
                    return false;
                }
                ScopePackages.Add(Package);
                Queue.Add(Package);
            }
        }

        for (int32 Index = 0; Index < Queue.Num(); ++Index)
        {
            TArray<FName> References;
            Registry.GetReferencers(Queue[Index], References);
            for (FName Package : References)
                if (!ScopePackages.Contains(Package))
                {
                    if (!Package.ToString().StartsWith(TEXT("/Game/")) || Queue.Num() >= 256)
                    {
                        Error = TEXT("Dependent widening requires a bounded /Game package scope: ")+ Package.ToString();
                        return false;
                    }
                    ScopePackages.Add(Package);
                    Queue.Add(Package);
                    TArray<FAssetData> Assets;
                    Registry.GetAssetsByPackageName(Package, Assets);
                    if (Assets.Num() != 1 ||
                        (Assets[0].AssetClassPath != UBlueprint::StaticClass()->GetClassPathName() &&
                         Assets[0].AssetClassPath != UWorld::StaticClass()->GetClassPathName() &&
                         !FDasBlueprintRollback::InstanceAsset(Assets[0], BP->GeneratedClass)))
                    {
                        Error = TEXT("Unsupported dependent package; only Blueprint, map and DataAsset instance assets supported: ")+
                            Package.ToString();
                        return false;
                    }
                    UObject* Asset = Assets[0].GetAsset();
                    if (!Asset)
                    {
                        Error = TEXT("Could not load dependent asset: ")+ Package.ToString();
                        return false;
                    }
                    ScopeAssets.Emplace(Asset);
                    if (auto* B = Cast<UBlueprint>(Asset))
                    {
                        if (!B->GeneratedClass || B->Status == BS_Error ||
                            !B->GetOutermost()->GetMetaData().GetValue(B, TEXT("DasDSLSource")).IsEmpty())
                        {
                            Error = TEXT("Dependent Blueprint must be valid and have no separate DSL source: ")+
                                         Package.ToString();
                            return false;
                        }
                        ScopeBlueprints.AddUnique(B);
                    }
                    else if (auto* W = Cast<UWorld>(Asset))
                    {
                        if (W->IsPartitionedWorld() || !W->GetStreamingLevels().IsEmpty())
                        {
                            Error = TEXT("World Partition/streaming map migration requires a separate scope: ")+
                                         Package.ToString();
                            return false;
                        }
                    }
                }
        }
        Dependents.Reset();
        FBlueprintEditorUtils::FindDependentBlueprints(BP, Dependents);
        for (auto* B : Dependents)
        {
            ScopeBlueprints.AddUnique(B);
            ScopePackages.Add(B->GetOutermost()->GetFName());
            ScopeAssets.Emplace(B);
        }
        Children.Reset();
        GetDerivedClasses(BP->GeneratedClass, Children, true);
        for (auto* Class : Children)
        {
            auto* Child = Cast<UBlueprint>(Class->ClassGeneratedBy);
            if (!Child || !ScopeBlueprints.Contains(Child))
            {
                Error = TEXT("Unmanaged derived class in numeric widening scope");
                return false;
            }
        }
        for (auto* B : ScopeBlueprints)
            if (B != BP)
            {
                if (!B->GeneratedClass || B->Status == BS_Error)
                {
                    Error = TEXT("Dependent Blueprint is not valid for migration: ")+ B->GetPathName();
                    return false;
                }
                if (!B->GetOutermost()->GetMetaData().GetValue(B, TEXT("DasDSLSource")).IsEmpty())
                {
                    Error = TEXT("Dependent DSL source requires an explicit source migration");
                    return false;
                }
            }
        ScopeBlueprints.Sort(
            [&](const UBlueprint& A, const UBlueprint& B)
            {
                if (&A == BP)
                    return &B != BP;
                if (&B == BP)
                    return false;
                auto Depth = [](const UClass* C)
                {
                    int32 N = 0;
                    while (C)
                    {
                        ++N;
                        C = C->GetSuperClass();
                    }
                    return N;
                };
                return Depth(A.GeneratedClass) < Depth(B.GeneratedClass);
            });
        for (FName Package : ScopePackages)
            UE_LOG(LogTemp, Display, TEXT("DAS_WIDEN_SCOPE %s"), *Package.ToString());
    }
    // A raw Blueprint property node may need a conversion or narrowing adapter.
    // Refuse it before mutation; DSL method call graphs retain their contracts.
    if (!Options.AdaptGraphs)
        for (auto* B : ScopeBlueprints)
        {
            TArray<UK2Node*> Nodes;
            FBlueprintEditorUtils::GetAllNodesOfClass(B, Nodes);
            for (const auto& R : Renames)
                if (Nodes.ContainsByPredicate([&](const auto* Node)
                                              { return Node->ReferencesVariable(R.OldName, nullptr); }))
                {
                    Error = TEXT("Use convert_numeric policy to adapt Blueprint property links");
                    return false;
                }
        }
    auto NumericPin = [](EDasScalarKind K)
    {
        return DasSchemaPinChecked(static_cast<EDasMethodSignature>(K));
    };
    FDasNumericGraphMigration GraphMigration;
    TMap<FName, FEdGraphPinType> ChangedTypes;
    for (const auto& R : Renames)
        ChangedTypes.Add(R.OldName, NumericPin(R.Target));
    if (Options.AdaptGraphs && !GraphMigration.Prepare(ScopeBlueprints, BP->GeneratedClass, ChangedTypes,
                                                       Options.Integers, Options.Floats, Error))
        return false;
    struct FSnapshot
    {
        TStrongObjectPtr<UObject> Object;
        TArray<FDasScalarValue> Values, Converted;
    };
    TArray<FSnapshot> Snapshots;
    TArray<UObject*> Objects;
    GetObjectsOfClass(BP->GeneratedClass, Objects, IncludeDependencies, RF_NoFlags);
    if (IncludeDependencies)
        for (auto* Object : Objects)
            if (auto* Actor = Cast<AActor>(Object))
                if (auto* World = Actor->GetWorld())
                    if (World->IsPartitionedWorld() || !World->GetStreamingLevels().IsEmpty())
                    {
                        Error = TEXT("Loaded World Partition/streaming instances require a separate migration scope");
                        return false;
                    }
                    else if (!ScopePackages.Contains(World->GetOutermost()->GetFName()))
                    {
                        ScopePackages.Add(World->GetOutermost()->GetFName());
                        UE_LOG(LogTemp, Display,
                               TEXT("DAS_WIDEN_SCOPE live map %s"), *World->GetOutermost()->GetName());
                    }
    for (auto* Object : Objects)
        if (IsValid(Object) && !Object->GetClass()->HasAnyClassFlags(CLASS_NewerVersionExists) &&
            !Object->GetClass()->GetName().StartsWith(TEXT("SKEL_")) &&
                                                      !Object->GetClass()->GetName().StartsWith(TEXT("REINST_")))
        {
            FSnapshot S;
            S.Object.Reset(Object);
            for (const auto& R : Renames)
            {
                FDasScalarValue V, Converted;
                if (!CaptureRenamedField(Object, R.OldName, R.Kind, V, Error))
                    return false;
                if (!ConvertDasNumber(V, int32(R.Target), Options.Integers, Options.Floats, Converted, Error))
                {
                    Error += TEXT(": ")+ Object->GetPathName() + TEXT(".")+ R.OldName.ToString();
                    return false;
                }
                S.Values.Add(V);
                S.Converted.Add(MoveTemp(Converted));
            }
            Snapshots.Add(MoveTemp(S));
        }
    const auto Handle = FCoreUObjectDelegates::OnObjectsReplaced.AddLambda(
        [&](const FCoreUObjectDelegates::FReplacementObjectMap& Map)
        {
            for (auto& S : Snapshots)
                if (auto* Replacement = Map.Find(S.Object.Get()))
                    S.Object.Reset(*Replacement);
        });
    ON_SCOPE_EXIT
    {
        FCoreUObjectDelegates::OnObjectsReplaced.Remove(Handle);
    };
    for (auto* B : ScopeBlueprints)
        if (B->Status != BS_UpToDate && B->Status != BS_UpToDateWithWarnings)
        {
            Error = TEXT("Compile scoped Blueprints before numeric widening: ")+ B->GetPathName();
            return false;
        }
    if (!Runtime.BeginPreserveContextsForReinstance(BP->GeneratedClass))
    {
        Error = TEXT("Numeric widening requires idle runtime reinstance scope");
        return false;
    }
    const FDasAuthoringModule Approval = DasApprovalModel(Next, New->Id);
    FString ApprovedWire;
    if (!Approval.ToJson(ApprovedWire, Error))
    {
        Runtime.EndPreserveContextsForReinstance();
        return false;
    }
    bool MigrationSucceeded = false;
    ON_SCOPE_EXIT
    {
        Runtime.EndPreserveContextsForReinstance(MigrationSucceeded ? ApprovedWire : FString());
    };
    const auto OriginalVariables = BP->NewVariables;
    TMap<UPackage*, bool> OriginalDirty;
    auto RememberDirty = [&](UObject* Object)
    {
        auto* Package = Object->GetOutermost();
        if (!OriginalDirty.Contains(Package))
            OriginalDirty.Add(Package, Package->IsDirty());
    };
    for (auto* B : ScopeBlueprints)
        RememberDirty(B);
    for (auto& S : Snapshots)
        RememberDirty(S.Object.Get());
    for (auto& Asset : ScopeAssets)
        RememberDirty(Asset.Get());
    auto CompileScope = [&]()
    {
        for (auto* B : ScopeBlueprints)
        {
            FKismetEditorUtilities::CompileBlueprint(B);
            FBlueprintCompilationManager::FlushCompilationQueueAndReinstance();
            if (B->Status == BS_Error || !B->GeneratedClass)
                return false;
        }
        return true;
    };
    auto Rollback = [&](const FString& Failure)
    {
        BP->NewVariables = OriginalVariables;
        FString GraphError;
        bool Restored = !Options.AdaptGraphs || GraphMigration.Restore(GraphError);
        FBlueprintEditorUtils::MarkBlueprintAsStructurallyModified(BP);
        Restored &= CompileScope();
        if (Options.AdaptGraphs)
            Restored &= GraphMigration.Restore(GraphError);
        for (auto& S : Snapshots)
            for (int32 I = 0; I < Renames.Num(); ++I)
                Restored &= RestoreRenamedField(S.Object.Get(), Renames[I].OldName, S.Values[I]);
        FString Validation;
        Restored &= FDasBlueprintService::Apply(BP, Previous, Validation, true);
        if (Restored)
            for (const auto& Dirty : OriginalDirty)
                Dirty.Key->SetDirtyFlag(Dirty.Value);
        Error = (Restored ? TEXT("Numeric migration rolled back: "): TEXT("Numeric rollback incomplete: ")) + Failure;
        UE_LOG(LogTemp, Display, TEXT("DAS_NUMERIC_ROLLBACK=%s"), Restored ? TEXT("PASS"): TEXT("FAIL"));
        return false;
    };
    for (auto* B : ScopeBlueprints)
        B->Modify();
    for (auto& S : Snapshots)
        S.Object->Modify();
    for (const auto& R : Renames)
    {
        auto* Variable = BP->NewVariables.FindByPredicate([&](const auto& V) { return V.VarName == R.OldName; });
        Variable->VarType = NumericPin(R.Target);
        const auto* Field = Candidate.Fields.FindByPredicate([&](const auto& F) { return F.Name == R.OldName; });
        Variable->DefaultValue =
            R.Target == EDasScalarKind::Int32
                ? FString::FromInt(Field->IntDefault)
                : FString::Printf(TEXT("%.17g"), R.Target == EDasScalarKind::Float32 ? double(Field->FloatDefault)
                                                                                     : Field->DoubleDefault);
    }
    FBlueprintEditorUtils::MarkBlueprintAsStructurallyModified(BP);
    if (Options.AdaptGraphs && !GraphMigration.Apply(Error))
    {
        const FString Failure = Error;
        return Rollback(Failure);
    }
    if (!CompileScope())
        return Rollback(TEXT("Blueprint compilation failed"));
#if WITH_DEV_AUTOMATION_TESTS
    if (GDasFailWidenAfterCompile)
        return Rollback(TEXT("Injected development test failure"));
#endif
    for (auto& S : Snapshots)
        for (int32 I = 0; I < Renames.Num(); ++I)
            if (!RestoreRenamedField(S.Object.Get(), Renames[I].NewName, S.Converted[I]))
                return Rollback(TEXT("Converted value restore failed"));
    if (!FDasBlueprintService::Apply(BP, Candidate, Error, true))
    {
        const FString Failure = Error;
        return Rollback(Failure);
    }
    for (auto* B : ScopeBlueprints)
        B->MarkPackageDirty();
    for (auto& S : Snapshots)
        S.Object->MarkPackageDirty();
    for (auto& Asset : ScopeAssets)
        Asset->MarkPackageDirty();
    BP->GetOutermost()->GetMetaData().SetValue(BP, TEXT("DasDSLModel"), *Json);
    BP->MarkPackageDirty();
    MigrationSucceeded = true;
    UE_LOG(LogTemp, Display, TEXT("DAS_NUMERIC_WIDEN PASS %d fields; exact finite values preserved"), Renames.Num());
    return true;
}

#if WITH_DEV_AUTOMATION_TESTS
static bool GDasFailArchiveAfterCompile = false;
void DasFailArchiveAfterCompileForTesting(bool Enabled)
{
    GDasFailArchiveAfterCompile = Enabled;
}
#endif
namespace
{
// Retired values of any actor host (Actor, Pawn, Character); null for other objects.
TMap<FString, FDasScalarValue>* RetiredValues(UObject* O)
{
    if (auto* A = Cast<ADasProbeActor>(O))
        return &A->RetiredDasValues;
    if (auto* P = Cast<ADasPawn>(O))
        return &P->RetiredDasValues;
    if (auto* C = Cast<ADasCharacter>(O))
        return &C->RetiredDasValues;
    if (auto* D = Cast<UDasDataAsset>(O))
        return &D->RetiredDasValues;
    if (auto* D = Cast<UDasPrimaryDataAsset>(O))
        return &D->RetiredDasValues;
    if (auto* G = Cast<UDasSaveGame>(O))
        return &G->RetiredDasValues;
    return nullptr;
}
FString ArchiveKey(const FString& ClassId, const FString& FieldId)
{
    return ClassId + TEXT("::")+ FieldId;
}
FString ArchiveMetaKey(const FString& ClassId, const FString& FieldId)
{
    return TEXT("DasRetired.")+ FMD5::HashAnsiString(*ArchiveKey(ClassId, FieldId));
}

}
bool FDasDSLService::ReimportArchivedFields(UBlueprint* BP, bool Restore, bool IncludeDependencies, FString& Error)
{
    Error.Reset();
    if (!IsInGameThread() || !IsValid(BP) || !BP->GeneratedClass)
    {
        Error = TEXT("Expected imported Blueprint on game thread");
        return false;
    }
    auto& Runtime = FModuleManager::LoadModuleChecked<FDasRuntimeModule>(TEXT("DasRuntime"));
    auto& Meta = BP->GetOutermost()->GetMetaData();
    const FString Script = Meta.GetValue(BP, TEXT("DasDSLSource")),
                                         Id = Meta.GetValue(BP, TEXT("DasDSLClassId")),
                                                            OldWire = Meta.GetValue(BP, TEXT("DasDSLModel"));
    if (Script.IsEmpty() || FPaths::GetCleanFilename(Script) != Script)
    {
        Error = TEXT("Expected local imported source");
        return false;
    }
    FDasAuthoringModule Prior, Next;
    FDasClassSchema Previous, Candidate;
    FString Json;
    if (!FDasAuthoringModule::FromJson(OldWire, Prior, Error) ||
        !Runtime.ReadAuthoringScript(Runtime.ScriptPath(Script), Next, Error) ||
        !Prior.ToActorSchema(Id, Previous, Error) || !Next.ToActorSchema(Id, Candidate, Error) ||
        !Next.ToJson(Json, Error) || !FDasBlueprintService::Apply(BP, Previous, Error, true))
        return false;
    if (Previous.ParentStableId != Candidate.ParentStableId)
    {
        Error = TEXT("Changing the parent together with a field migration is deferred; save them separately");
        return false;
    }
    if (Previous.Fields.ContainsByPredicate([](const FDasFieldSchema& F) { return F.Kind == EDasScalarKind::Class; }) ||
        Candidate.Fields.ContainsByPredicate([](const FDasFieldSchema& F) { return F.Kind == EDasScalarKind::Class; }))
    {
        Error = TEXT("Retired-field migration of class references is deferred");
        return false;
    }
    for (const auto& M : Candidate.EffectiveMethods())
        if (!Runtime.ValidateFrameMethod(Runtime.ScriptPath(M.ScriptFile), Error, M.EntryPoint, M.Contract()))
            return false;
    const auto* Old = Prior.Classes.FindByPredicate([&](const auto& C) { return C.Id == Id; });
    const auto* New = Next.Classes.FindByPredicate([&](const auto& C) { return C.Id == Id; });
    if (!Old || !New || Prior.Id != Next.Id)
    {
        Error = TEXT("Archive operation requires matching class/module identity");
        return false;
    }
    struct FAction
    {
        FDasFieldSchema Field;
        FDasDeclaredProperty Declaration;
        FGuid Guid;
        FString MetaKey, StoredModel;
    };
    TArray<FAction> Actions;
    auto Expected = *Old;
    if (!Restore)
    {
        for (const auto& P : Old->Properties)
            if (!New->Properties.ContainsByPredicate([&](const auto& X) { return X.Id == P.Id; }))
            {
                const auto* F = Previous.Fields.FindByPredicate([&](const auto& X) { return X.StableId == P.Id; });
                const auto* V =
                    BP->NewVariables.FindByPredicate([&](const auto& X) { return X.VarName == FName(*P.Name); });
                if (!F || !V || !V->RepNotifyFunc.IsNone() || (V->PropertyFlags & CPF_Net))
                {
                    Error = TEXT("Archive supports authored non-replicated fields only");
                    return false;
                }
                const FString Key = ArchiveMetaKey(Id, P.Id);
                if (!Meta.GetValue(BP, *Key).IsEmpty())
                {
                    Error = TEXT("Existing retired declaration must be restored before reuse");
                    return false;
                }
                Actions.Add({*F, P, V->VarGuid, Key, OldWire});
                Expected.Properties.RemoveAll([&](const auto& X) { return X.Id == P.Id; });
            }
    }
    else
    {
        for (const auto& P : New->Properties)
            if (!Old->Properties.ContainsByPredicate([&](const auto& X) { return X.Id == P.Id; }))
            {
                const FString Key = ArchiveMetaKey(Id, P.Id), Stored = Meta.GetValue(BP, *Key);
                FDasAuthoringModule Retired;
                FDasClassSchema RetiredSchema;
                if (Stored.IsEmpty() || !FDasAuthoringModule::FromJson(Stored, Retired, Error) ||
                    !Retired.ToActorSchema(Id, RetiredSchema, Error))
                {
                    Error = TEXT("No valid retired declaration for stable ID: ")+ P.Id;
                    return false;
                }
                const auto* C = Retired.Classes.FindByPredicate([&](const auto& X) { return X.Id == Id; });
                const auto* D =
                    C ? C->Properties.FindByPredicate([&](const auto& X) { return X.Id == P.Id; }) : nullptr;
                const auto* F = RetiredSchema.Fields.FindByPredicate([&](const auto& X) { return X.StableId == P.Id; });
                FGuid Guid;
                if (!D || !F ||
                    !FGuid::Parse(Meta.GetValue(BP, *(Key + TEXT(".Guid"))), Guid) ||
                                  BP->GeneratedClass->FindPropertyByName(*P.Name) ||
                                  BP->GeneratedClass->FindFunctionByName(*P.Name))
                {
                    Error = TEXT("Restore declaration/GUID/destination conflict");
                    return false;
                }
                Expected.Properties.Add(*D);
                Actions.Add({*F, *D, Guid, Key, Stored});
            }
    }
    const auto Delta = FDasAuthoringModule::Compare(Expected, *New);
    if (Delta.bStructural || Delta.bDefaults || Delta.bMetadata || Delta.bImplementation)
    {
        Error = FString::Printf(TEXT("Archive policy accepts only removal, or exact restoration, of fields; other declarations must stay unchanged (structure=%d, defaults=%d, metadata=%d, implementation=%d)"),
            Delta.bStructural, Delta.bDefaults, Delta.bMetadata, Delta.bImplementation);
        return false;
    }
    if (Actions.IsEmpty())
    {
        UBlueprint* Result = nullptr;
        return Apply(Script, BP->GetOutermost()->GetName(), Id, Result, Error);
    }
    // Complete fallible conversion before touching variables, graphs or instances.
    FDasDefaultRoots DefaultRoots;
    TMap<FName, FEdGraphPinType> RestorePins;
    TMap<FName, FString> RestoreDefaults;
    if (Restore)
        for (const auto& A : Actions)
        {
            const auto& F = A.Field;
            FDasDeclaredType Type;
            FEdGraphPinType Pin;
            FString Default;
            const FString Path = F.Kind == EDasScalarKind::Enum                                          ? F.EnumPath
                                 : (F.Kind == EDasScalarKind::Object || F.Kind == EDasScalarKind::Class) ? F.ObjectClass
                                                                                                         : F.StructPath;
            if (!DasSchemaType(static_cast<EDasMethodSignature>(F.Kind), Path, Type, Error) ||
                !DasTypeToPin(Type, Pin, Error) || !DasBlueprintFieldDefault(F, Default, DefaultRoots, Error))
                return false;
            RestorePins.Add(F.Name, MoveTemp(Pin));
            RestoreDefaults.Add(F.Name, MoveTemp(Default));
        }
    TArray<UClass*> Children;
    GetDerivedClasses(BP->GeneratedClass, Children, true);
    TArray<UBlueprint*> Dependents;
    FBlueprintEditorUtils::FindDependentBlueprints(BP, Dependents);
    auto& Registry = FModuleManager::LoadModuleChecked<FAssetRegistryModule>(TEXT("AssetRegistry")).Get();
    if (!Registry.IsSearchAllAssets())
        Registry.SearchAllAssets(true);
    else
        Registry.WaitForCompletion();
    if (Registry.IsLoadingAssets())
    {
        Error = TEXT("Asset Registry scan must finish before archive field migration");
        return false;
    }
    TArray<FName> Referencers;
    Registry.GetReferencers(BP->GetOutermost()->GetFName(), Referencers);
    if (!IncludeDependencies &&
        (!Children.IsEmpty() || !Dependents.IsEmpty() ||
         Referencers.ContainsByPredicate([&](FName P) { return P != BP->GetOutermost()->GetFName(); })))
    {
        Error = TEXT("Dependent Blueprints or saved referencing assets require an archive policy with dependencies");
        return false;
    }
    TArray<TStrongObjectPtr<UObject>> ScopeAssets;
    TArray<UBlueprint*> ScopeBlueprints;
    ScopeBlueprints.Add(BP);
    TSet<FName> ScopePackages;
    ScopePackages.Add(BP->GetOutermost()->GetFName());
    if (IncludeDependencies)
    {
        TArray<FName> Queue;
        Queue.Add(BP->GetOutermost()->GetFName());
        // Include unsaved/modified loaded dependencies before walking saved
        // referencers, so maps referring to those packages are included too.
        for (auto* Class : Children)
            if (auto* B = Cast<UBlueprint>(Class->ClassGeneratedBy))
                Dependents.AddUnique(B);
        for (auto* B : Dependents)
        {
            ScopeBlueprints.AddUnique(B);
            ScopeAssets.Emplace(B);
            const FName Package = B->GetOutermost()->GetFName();
            if (!ScopePackages.Contains(Package))
            {
                if (Queue.Num() >= 256 || (!Package.ToString().StartsWith(
                                              TEXT("/Game/")) && FPackageName::DoesPackageExist(Package.ToString())))
                {
                    Error =
                        TEXT("Loaded dependent package exceeds supported archive field scope: ")+ Package.ToString();
                    return false;
                }
                ScopePackages.Add(Package);
                Queue.Add(Package);
            }
        }

        for (int32 Index = 0; Index < Queue.Num(); ++Index)
        {
            TArray<FName> References;
            Registry.GetReferencers(Queue[Index], References);
            for (FName Package : References)
                if (!ScopePackages.Contains(Package))
                {
                    if (!Package.ToString().StartsWith(TEXT("/Game/")) || Queue.Num() >= 256)
                    {
                        Error = TEXT("Field archive requires a bounded /Game package scope: ")+ Package.ToString();
                        return false;
                    }
                    ScopePackages.Add(Package);
                    Queue.Add(Package);
                    TArray<FAssetData> Assets;
                    Registry.GetAssetsByPackageName(Package, Assets);
                    if (Assets.Num() != 1 ||
                        (Assets[0].AssetClassPath != UBlueprint::StaticClass()->GetClassPathName() &&
                         Assets[0].AssetClassPath != UWorld::StaticClass()->GetClassPathName() &&
                         !FDasBlueprintRollback::InstanceAsset(Assets[0], BP->GeneratedClass)))
                    {
                        Error = TEXT("Unsupported dependent package; only Blueprint, map and DataAsset instance assets supported: ")+
                            Package.ToString();
                        return false;
                    }
                    UObject* Asset = Assets[0].GetAsset();
                    if (!Asset)
                    {
                        Error = TEXT("Could not load dependent asset: ")+ Package.ToString();
                        return false;
                    }
                    ScopeAssets.Emplace(Asset);
                    if (auto* B = Cast<UBlueprint>(Asset))
                    {
                        if (!B->GeneratedClass || B->Status == BS_Error ||
                            !B->GetOutermost()->GetMetaData().GetValue(B, TEXT("DasDSLSource")).IsEmpty())
                        {
                            Error = TEXT("Dependent Blueprint must be valid and have no separate DSL source: ")+
                                         Package.ToString();
                            return false;
                        }
                        ScopeBlueprints.AddUnique(B);
                    }
                    else if (auto* W = Cast<UWorld>(Asset))
                    {
                        if (W->IsPartitionedWorld() || !W->GetStreamingLevels().IsEmpty())
                        {
                            Error = TEXT("World Partition/streaming map migration requires a separate scope: ")+
                                         Package.ToString();
                            return false;
                        }
                    }
                }
        }
        Dependents.Reset();
        FBlueprintEditorUtils::FindDependentBlueprints(BP, Dependents);
        for (auto* B : Dependents)
        {
            ScopeBlueprints.AddUnique(B);
            ScopePackages.Add(B->GetOutermost()->GetFName());
            ScopeAssets.Emplace(B);
        }
        Children.Reset();
        GetDerivedClasses(BP->GeneratedClass, Children, true);
        for (auto* Class : Children)
        {
            auto* Child = Cast<UBlueprint>(Class->ClassGeneratedBy);
            if (!Child || !ScopeBlueprints.Contains(Child))
            {
                Error = TEXT("Unmanaged derived class in archive field scope");
                return false;
            }
        }
        for (auto* B : ScopeBlueprints)
            if (B != BP)
            {
                if (!B->GeneratedClass || B->Status == BS_Error)
                {
                    Error = TEXT("Dependent Blueprint is not valid for migration: ")+ B->GetPathName();
                    return false;
                }
                if (!B->GetOutermost()->GetMetaData().GetValue(B, TEXT("DasDSLSource")).IsEmpty())
                {
                    Error = TEXT("Dependent DSL source requires an explicit source migration");
                    return false;
                }
            }
        ScopeBlueprints.Sort(
            [&](const UBlueprint& A, const UBlueprint& B)
            {
                if (&A == BP)
                    return &B != BP;
                if (&B == BP)
                    return false;
                auto Depth = [](const UClass* C)
                {
                    int32 N = 0;
                    while (C)
                    {
                        ++N;
                        C = C->GetSuperClass();
                    }
                    return N;
                };
                return Depth(A.GeneratedClass) < Depth(B.GeneratedClass);
            });
        for (FName Package : ScopePackages)
            UE_LOG(LogTemp, Display, TEXT("DAS_ARCHIVE_SCOPE %s"), *Package.ToString());
    }
    for (auto* B : ScopeBlueprints)
    {
        if (B->Status != BS_UpToDate && B->Status != BS_UpToDateWithWarnings)
        {
            Error = TEXT("Compile scoped Blueprints before archive operation");
            return false;
        }
        TArray<UK2Node*> Nodes;
        FBlueprintEditorUtils::GetAllNodesOfClass(B, Nodes);
        for (const auto& A : Actions)
            if (!Restore &&
                Nodes.ContainsByPredicate([&](const auto* N) { return N->ReferencesVariable(A.Field.Name, nullptr); }))
            {
                Error = TEXT("Remove Blueprint graph references explicitly before retiring field: ")+
                             A.Field.Name.ToString();
                return false;
            }
        if (Restore && B != BP)
            for (const auto& A : Actions)
                if (B->GeneratedClass->FindPropertyByName(A.Field.Name) ||
                    B->GeneratedClass->FindFunctionByName(A.Field.Name))
                {
                    Error = TEXT("Restore conflicts in dependent Blueprint");
                    return false;
                }
    }
    FDasNumericGraphMigration Graphs;
    TMap<FName, FEdGraphPinType> NoChanges;
    if (!Graphs.Prepare(ScopeBlueprints, BP->GeneratedClass, NoChanges, EDasIntegerRule::Exact, EDasFloatRule::Exact,
                        Error))
        return false;
    struct FSnapshot
    {
        TStrongObjectPtr<UObject> Object;
        TArray<FDasScalarValue> Values;
        TMap<FString, FDasScalarValue> Archive;
    };
    TArray<FSnapshot> Snapshots;
    TArray<UObject*> Objects;
    GetObjectsOfClass(BP->GeneratedClass, Objects, IncludeDependencies, RF_NoFlags);
    for (auto* O : Objects)
        if (IsValid(O) && !O->GetClass()->HasAnyClassFlags(CLASS_NewerVersionExists) &&
            !O->GetClass()->GetName().StartsWith(TEXT("SKEL_")) &&
                                                 !O->GetClass()->GetName().StartsWith(TEXT("REINST_")))
        {
            auto* Retired = RetiredValues(O);
            if (!Retired)
            {
                Error = TEXT("Archive requires Actor, Pawn, Character, DataAsset or SaveGame instances");
                return false;
            }
            if (auto* W = O->GetWorld())
                if (W->IsPartitionedWorld() || !W->GetStreamingLevels().IsEmpty())
                {
                    Error = TEXT("Unsupported loaded partitioned/streaming archive scope");
                    return false;
                }
            FSnapshot S;
            S.Object.Reset(O);
            S.Archive = *Retired;
            for (const auto& A : Actions)
            {
                const FString Key = ArchiveKey(Id, A.Field.StableId);
                if (!Restore)
                {
                    if (S.Archive.Contains(Key))
                    {
                        Error = TEXT("Existing retired instance value cannot be overwritten");
                        return false;
                    }
                    FDasScalarValue Value;
                    if (!CaptureRenamedField(O, A.Field.Name, A.Field.Kind, Value, Error))
                        return false;
                    S.Values.Add(MoveTemp(Value));
                }
                else
                {
                    const auto* V = S.Archive.Find(Key);
                    if (!V || V->Kind != int32(A.Field.Kind) ||
                        (A.Field.Kind == EDasScalarKind::Enum &&
                         (V->ObjectClass != A.Field.EnumPath ||
                          !ValidateDasEnumNumber(V->ObjectClass, V->IntValue, Error))) ||
                        (A.Field.Kind == EDasScalarKind::Object &&
                         (V->ObjectClass != A.Field.ObjectClass ||
                          (IsValid(V->ObjectValue.Get()) &&
                           !DasReferenceMatches(V->ObjectValue.Get(),
                                                FindObject<UClass>(nullptr, *A.Field.ObjectClass))))) ||
                        (A.Field.Kind == EDasScalarKind::Struct &&
                         (!V->StructValue.IsValid() ||
                          V->StructValue.GetScriptStruct()->GetPathName() != A.Field.StructPath ||
                          !ValidateDasStructSnapshot(V->StructValue, Error))))
                    {
                        Error = TEXT("Missing or incompatible retained value: ")+ O->GetPathName() +
                                     TEXT(".")+ A.Field.StableId;
                        return false;
                    }
                    S.Values.Add(*V);
                }
            }
            Snapshots.Add(MoveTemp(S));
        }
    FDasPropertyValueRoots ObjectPins;
    for (const auto& S : Snapshots)
    {
        for (const auto& V : S.Values)
            ObjectPins.Keep(V);
        ObjectPins.Keep(S.Archive);
    }
    const auto Handle = FCoreUObjectDelegates::OnObjectsReplaced.AddLambda(
        [&](const FCoreUObjectDelegates::FReplacementObjectMap& Map)
        {
            for (auto& S : Snapshots)
            {
                if (auto* NewObject = Map.Find(S.Object.Get()))
                    S.Object.Reset(*NewObject);
                for (auto& V : S.Values)
                    FDasPropertyValueRoots::Remap(V, Map);
                FDasPropertyValueRoots::Remap(S.Archive, Map);
            }
            ObjectPins.RemapPins(Map);
        });
    ON_SCOPE_EXIT
    {
        FCoreUObjectDelegates::OnObjectsReplaced.Remove(Handle);
    };
    if (!Runtime.BeginPreserveContextsForReinstance(BP->GeneratedClass))
    {
        Error = TEXT("Archive operation requires idle runtime");
        return false;
    }
    const FDasAuthoringModule Approval = DasApprovalModel(Next, New->Id);
    FString ApprovalWire;
    if (!Approval.ToJson(ApprovalWire, Error))
    {
        Runtime.EndPreserveContextsForReinstance();
        return false;
    }
    bool Succeeded = false;
    ON_SCOPE_EXIT
    {
        Runtime.EndPreserveContextsForReinstance(Succeeded ? ApprovalWire : FString());
    };
    const auto OriginalVariables = BP->NewVariables;
    TMap<UPackage*, bool> Dirty;
    auto Remember = [&](UObject* O)
    {
        if (!Dirty.Contains(O->GetOutermost()))
            Dirty.Add(O->GetOutermost(), O->GetOutermost()->IsDirty());
    };
    for (auto* B : ScopeBlueprints)
        Remember(B);
    for (auto& S : Snapshots)
        Remember(S.Object.Get());
    for (auto& A : ScopeAssets)
        Remember(A.Get());
    auto Compile = [&]()
    {
        for (auto* B : ScopeBlueprints)
        {
            FKismetEditorUtilities::CompileBlueprint(B);
            FBlueprintCompilationManager::FlushCompilationQueueAndReinstance();
            if (B->Status == BS_Error || !B->GeneratedClass)
                return false;
        }
        return true;
    };
    auto Rollback = [&](const FString& Reason)
    {
        BP->NewVariables = OriginalVariables;
        FString GraphError;
        bool OK = Graphs.Restore(GraphError);
        FBlueprintEditorUtils::MarkBlueprintAsStructurallyModified(BP);
        OK &= Compile();
        OK &= Graphs.Restore(GraphError);
        for (auto& S : Snapshots)
        {
            if (!S.Object.IsValid())
            {
                OK = false;
                continue;
            }
            *RetiredValues(S.Object.Get()) = S.Archive;
            if (!Restore)
                for (int32 I = 0; I < Actions.Num(); ++I)
                    OK &= RestoreRenamedField(S.Object.Get(), Actions[I].Field.Name, S.Values[I]);
        }
        for (auto* B : ScopeBlueprints)
            if (auto* Generated = Cast<UBlueprintGeneratedClass>(B->GeneratedClass))
                Generated->UpdateCustomPropertyListForPostConstruction();
        FString E;
        OK &= FDasBlueprintService::Apply(BP, Previous, E, true);
        if (OK)
            for (const auto& D : Dirty)
                D.Key->SetDirtyFlag(D.Value);
        Error = (OK ? TEXT("Archive migration rolled back: "): TEXT("Archive rollback incomplete: ")) + Reason;
        UE_LOG(LogTemp, Display, TEXT("DAS_ARCHIVE_ROLLBACK=%s"), OK ? TEXT("PASS"): TEXT("FAIL"));
        return false;
    };
    for (auto* B : ScopeBlueprints)
        B->Modify();
    for (auto& S : Snapshots)
        S.Object->Modify();
    if (!Restore)
    {
        TArray<FName> Names;
        for (const auto& A : Actions)
            Names.Add(A.Field.Name);
        FBlueprintEditorUtils::BulkRemoveMemberVariables(BP, Names);
    }
    else
    {
        for (const auto& A : Actions)
        {
            const auto& F = A.Field;
            if (!FBlueprintEditorUtils::AddMemberVariable(BP, F.Name, RestorePins.FindChecked(F.Name),
                                                          RestoreDefaults.FindChecked(F.Name)))
                return Rollback(TEXT("Cannot recreate archived member"));
            auto* V = BP->NewVariables.FindByPredicate([&](const auto& X) { return X.VarName == F.Name; });
            V->VarGuid = A.Guid;
            FBlueprintEditorUtils::SetBlueprintOnlyEditableFlag(BP, F.Name, !F.bInstanceEditable);
            FBlueprintEditorUtils::SetBlueprintPropertyReadOnlyFlag(BP, F.Name, F.bReadOnly);
            FBlueprintEditorUtils::SetBlueprintVariableMetaData(BP, F.Name, nullptr, TEXT("DasMemberId"), F.StableId);
            for (const auto& M : F.Metadata)
                FBlueprintEditorUtils::SetBlueprintVariableMetaData(BP, F.Name, nullptr, *M.Key, M.Value);
        }
    }
    FBlueprintEditorUtils::MarkBlueprintAsStructurallyModified(BP);
    if (!Compile())
        return Rollback(TEXT("Compilation failed"));
#if WITH_DEV_AUTOMATION_TESTS
    if (GDasFailArchiveAfterCompile)
        return Rollback(TEXT("Injected development test failure"));
#endif
    for (auto& S : Snapshots)
    {
        if (!S.Object.IsValid())
            return Rollback(TEXT("Replacement actor missing"));
        for (int32 I = 0; I < Actions.Num(); ++I)
        {
            const auto& A = Actions[I];
            const FString Key = ArchiveKey(Id, A.Field.StableId);
            if (!Restore)
                RetiredValues(S.Object.Get())->Add(Key, S.Values[I]);
            else
            {
                if (!RestoreRenamedField(S.Object.Get(), A.Field.Name, S.Values[I]))
                    return Rollback(TEXT("Cannot restore retained value"));
                RetiredValues(S.Object.Get())->Remove(Key);
            }
        }
    }
    for (auto* B : ScopeBlueprints)
        if (auto* Generated = Cast<UBlueprintGeneratedClass>(B->GeneratedClass))
            Generated->UpdateCustomPropertyListForPostConstruction();
    if (!FDasBlueprintService::Apply(BP, Candidate, Error, true))
    {
        const FString Reason = Error;
        return Rollback(Reason);
    }
    for (const auto& A : Actions)
    {
        if (!Restore)
        {
            Meta.SetValue(BP, *A.MetaKey, *A.StoredModel);
            Meta.SetValue(BP, *(A.MetaKey + TEXT(".Guid")), *A.Guid.ToString(EGuidFormats::Digits));
        }
        else
        {
            Meta.RemoveValue(BP, *A.MetaKey);
            Meta.RemoveValue(BP, *(A.MetaKey + TEXT(".Guid")));
        }
    }
    Meta.SetValue(BP, TEXT("DasDSLModel"), *Json);
    for (auto* B : ScopeBlueprints)
        B->MarkPackageDirty();
    for (auto& S : Snapshots)
        S.Object->MarkPackageDirty();
    for (auto& A : ScopeAssets)
        A->MarkPackageDirty();
    Succeeded = true;
    UE_LOG(LogTemp, Display,
           TEXT("DAS_FIELD_ARCHIVE PASS %s %d fields"), Restore ? TEXT("restore"): TEXT("retire"), Actions.Num());
    return true;
}
