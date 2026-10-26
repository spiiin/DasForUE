#include "DasComponentComposition.h"
#include "DasTypeAdapters.h"
#include "DasScriptHosts.h"
#include "DasComponentDefaults.h"
#include "DasDSLService.h"
#include "DasRuntime.h"
#include "DasScriptComponent.h"
#include "DasActor.h"
#include "Engine/Blueprint.h"
#include "Engine/SimpleConstructionScript.h"
#include "Engine/SCS_Node.h"
#include "Engine/InheritableComponentHandler.h"
#include "Components/SceneComponent.h"
#include "Misc/PackageName.h"
#include "UObject/MetaData.h"
#include "Kismet2/BlueprintEditorUtils.h"
bool DasPrepareComponentComposition(const FDasDeclaredClass& D, const FDasDeclaredClass& Effective,
                                    const FString& Package, TMap<FString, UBlueprint*>& Components, FString& Error)
{
    auto& Runtime = FModuleManager::LoadModuleChecked<FDasRuntimeModule>(TEXT("DasRuntime"));
    struct FSource
    {
        FString File, Id, Package, ComponentId;
    };
    TArray<FSource> Sources;
    FDasDeclaredClass Preflight = D;
    for (const auto& O : D.ComponentOverrides)
    {
        const auto* Target = Effective.Components.FindByPredicate([&](const auto& V) { return V.Id == O.Id; });
        if (!Target)
        {
            Error = TEXT("Missing inherited override target");
            return false;
        }
        auto Component = *Target;
        Component.Defaults = O.Defaults;
        Preflight.Components.Add(Component);
    }
    for (const auto& C : Preflight.Components)
    {
        if (DasNativeScriptHost(Effective.NativeParent)->FindPropertyByName(*C.Name) ||
            DasNativeScriptHost(Effective.NativeParent)->FindFunctionByName(*C.Name))
        {
            Error = TEXT("Component conflicts with inherited member: ")+ C.Name;
            return false;
        }
        if (!C.NativeClass.IsEmpty())
        {
            auto* Native = FindObject<UClass>(nullptr, *C.NativeClass);
            if (!Native || !Native->IsChildOf(UActorComponent::StaticClass()) ||
                Native->HasAnyClassFlags(CLASS_Abstract | CLASS_Deprecated | CLASS_NewerVersionExists))
            {
                Error = TEXT("Invalid native component class: ")+ C.NativeClass;
                return false;
            }
            if (!DasComponentDefaults(Native, C, nullptr, Error))
                return false;
            continue;
        }
        FDasAuthoringModule Model;
        if (!Runtime.ReadAuthoringScript(Runtime.ScriptPath(C.ScriptFile), Model, Error))
            return false;
        const auto* Type = Model.Classes.FindByPredicate([&](const auto& X) { return X.Name == C.ClassName; });
        FDasDeclaredClass EffectiveType;
        if (!Type || !Model.ResolveClass(Type->Id, EffectiveType, Error) ||
            EffectiveType.NativeParent != TEXT("/Script/Engine.ActorComponent") || !EffectiveType.Components.IsEmpty())
        {
            Error = TEXT("Component type must inherit authored ActorComponent: ")+ C.ClassName;
            return false;
        }
        FDasAuthoringModule DefaultsModel = Model;
        for (const auto& Pair : C.Defaults)
        {
            FDasDeclaredProperty* Property = nullptr;
            FString DefaultsId = Type->Id;
            for (int32 Depth = 0; Depth < 32 && !DefaultsId.IsEmpty(); ++Depth)
            {
                auto* DefaultsClass =
                    DefaultsModel.Classes.FindByPredicate([&](const auto& X) { return X.Id == DefaultsId; });
                if (!DefaultsClass)
                    break;
                Property = DefaultsClass->Properties.FindByPredicate([&](const auto& X) { return X.Name == Pair.Key; });
                if (Property)
                    break;
                DefaultsId = DefaultsClass->ParentId;
            }
            if (!Property || !Property->bEditable ||
                (Property->Type.Kind !=
                 TEXT("int") && Property->Type.Kind !=
                                    TEXT("float") &&
                                         Property->Type.Kind !=
                                             TEXT("double") &&
                                                  Property->Type.Kind !=
                                                      TEXT("bool") &&
                                                           Property->Type.Kind !=
                                                               TEXT("string") &&
                                                                    Property->Type.Kind !=
                                                                        TEXT("name") &&
                                                                             Property->Type.Kind !=
                                                                                 TEXT("struct") &&
                                                                                      Property->Type.Kind !=
                                                                                          TEXT("object") &&
                                                                                               Property->Type.Kind !=
                                                                                                   TEXT("class")))
            {
                Error =
                    TEXT("Authored component defaults require editable scalar fields: ")+ C.Name + TEXT(".")+ Pair.Key;
                return false;
            }
            Property->Default = Pair.Value;
            if (Property->Type.Kind == TEXT("struct"))
            {
                const auto* Math = DasMathAdapter(Property->Type.TypePath);
                const FString Expected = Math ? Math->GetDefaultKind() : TEXT("");
                if (Pair.Value.Kind != TEXT("struct") && (Expected.IsEmpty() || Pair.Value.Kind != Expected))
                {
                    Error = TEXT("Authored component math default type mismatch");
                    return false;
                }
                Property->Default.Kind = TEXT("string");
            }
            else if (Property->Type.Kind == TEXT("object") || Property->Type.Kind == TEXT("class"))
            {
                if (Pair.Value.Kind != (Property->Type.Kind == TEXT("class")? TEXT("class"): TEXT("asset")))
                {
                    Error = TEXT("Authored component reference default type mismatch");
                    return false;
                }
                Property->Default.Kind = TEXT("string");
            }
        }
        if (!DefaultsModel.Validate(Error))
            return false;
        FDasClassSchema Schema;
        if (!Model.ToActorSchema(Type->Id, Schema, Error))
            return false;
        for (const auto& M : Schema.EffectiveMethods())
            if (!Runtime.ValidateFrameMethod(Runtime.ScriptPath(M.ScriptFile), Error, M.EntryPoint, M.Contract()))
                return false;
        const FString ChildPackage = FPackageName::GetLongPackagePath(Package) + TEXT("/BP_")+ Type->Name;
        if (ChildPackage.Equals(Package, ESearchCase::IgnoreCase))
        {
            Error = TEXT("Component asset path conflicts with Actor package");
            return false;
        }
        const FString ChildPath = ChildPackage + TEXT(".")+ FPackageName::GetLongPackageAssetName(ChildPackage);
        UObject* Existing = LoadObject<UObject>(nullptr, *ChildPath, nullptr, LOAD_NoWarn);
        if (Existing)
        {
            auto* Child = Cast<UBlueprint>(Existing);
            if (!Child ||
                Child->GetOutermost()->GetMetaData().GetValue(Child, TEXT("DasDSLClassId")) != Type->Id ||
                                                              Child->GetOutermost()->GetMetaData().GetValue(
                                                                  Child, TEXT("DasDSLSource")) != C.ScriptFile)
            {
                Error = TEXT("Component asset path is occupied by a different asset: ")+ ChildPath;
                return false;
            }
        }
        else if (FPackageName::DoesPackageExist(ChildPackage))
        {
            Error = TEXT("Existing component package cannot be loaded");
            return false;
        }
        for (const auto& S : Sources)
            if (S.Package.Equals(ChildPackage, ESearchCase::IgnoreCase) && (S.Id != Type->Id || S.File != C.ScriptFile))
            {
                Error = TEXT("Component asset name conflict: ")+ ChildPackage;
                return false;
            }
        Sources.Add({C.ScriptFile, Type->Id, ChildPackage, C.Id});
    }
    for (const auto& C : D.Components)
    {
        auto* Type = C.NativeClass.IsEmpty() ? nullptr : FindObject<UClass>(nullptr, *C.NativeClass);
        if ((C.bRoot || !C.Attach.IsEmpty() || !C.AttachSocket.IsEmpty()) &&
            (!Type || !Type->IsChildOf(USceneComponent::StaticClass())))
        {
            Error = TEXT("Root/attachment requires native SceneComponent: ")+ C.Name;
            return false;
        }
        if (!C.Attach.IsEmpty())
        {
            const auto* Parent =
                Effective.Components.FindByPredicate([&](const auto& V) { return V.Name == C.Attach; });
            auto* ParentType = Parent ? FindObject<UClass>(nullptr, *Parent->NativeClass) : nullptr;
            if (!ParentType || !ParentType->IsChildOf(USceneComponent::StaticClass()))
            {
                Error = TEXT("Attachment parent must be a scene component");
                return false;
            }
        }
    }
    for (int32 I = 0; I < Sources.Num(); ++I)
    {
        const auto& S = Sources[I];
        UBlueprint* BP = nullptr;
        // An existing component class follows its script like any other class (the automatic policy); field
        // migrations that change other assets are applied when the component script itself is saved.
        if (auto* Existing = LoadObject<UBlueprint>(
                nullptr, *(S.Package + TEXT(".") + FPackageName::GetLongPackageAssetName(S.Package)), nullptr,
                           LOAD_NoWarn))
        {
            if (!FDasDSLService::Reimport(Existing, Error))
                return false;
            BP = Existing;
        }
        else if (!FDasDSLService::Apply(S.File, S.Package, S.Id, BP, Error))
            return false;
        Components.Add(S.ComponentId, BP);
    }
    return true;
}
bool DasApplyComponentComposition(UBlueprint* BP, const FDasDeclaredClass& D, const FDasDeclaredClass& Effective,
                                  const TMap<FString, UBlueprint*>& Components, bool Create, FString& Error,
                                  const FDasDeclaredClass* Old)
{
    if (D.Components.IsEmpty() && D.ComponentOverrides.IsEmpty() && (!Old || Old->Components.IsEmpty()))
        return true;
    if (!BP->SimpleConstructionScript)
    {
        Error = TEXT("Actor Blueprint has no construction script");
        return false;
    }
    USimpleConstructionScript* SCS = BP->SimpleConstructionScript;
    auto FindById = [&](const FString& Id) -> USCS_Node*
    {
        for (auto* Node : SCS->GetAllNodes())
            if (Node->FindMetaDataEntryIndexForKey(TEXT("DasComponentId")) != INDEX_NONE &&
                                                   Node->GetMetaData(TEXT("DasComponentId")) == Id)
                return Node;
        return nullptr;
    };
    auto FindByName = [&](const FString& Name) -> USCS_Node*
    {
        for (auto* Node : SCS->GetAllNodes())
            if (Node->GetVariableName() == FName(*Name))
                return Node;
        return nullptr;
    };
    auto Previous = [&](const FDasDeclaredComponent& C) -> const FDasDeclaredComponent*
    {
        return Old ? Old->Components.FindByPredicate([&](const auto& X) { return X.Id == C.Id; }) : nullptr;
    };
    auto SetNodeMetadata = [](USCS_Node* Node, const FDasDeclaredComponent& C)
    {
        Node->SetMetaData(TEXT("DasComponentId"), C.Id);
        Node->SetMetaData(TEXT("DasComponentReadOnly"), C.bReadOnly ? TEXT("true") : TEXT("false"));
        Node->SetMetaData(TEXT("DasComponentEditable"), C.bEditable ? TEXT("true") : TEXT("false"));
        Node->SetMetaData(TEXT("DasComponentVisible"), C.bVisible ? TEXT("true") : TEXT("false"));
        for (const auto& Pair : C.Metadata)
            Node->SetMetaData(FName(*Pair.Key), Pair.Value);
        if (const auto* Category = C.Metadata.Find(TEXT("Category")))
            Node->CategoryName = FText::FromString(*Category);
    };
    bool Changed = Create;
    // Components that left the declaration; their children move up to the removed node's parent.
    if (!Create)
        for (auto* Node : TArray<USCS_Node*>(SCS->GetAllNodes()))
            if (Node->FindMetaDataEntryIndexForKey(
                    TEXT("DasComponentId")) != INDEX_NONE &&
                    !D.Components.ContainsByPredicate([&](const auto& C)
                                                      { return C.Id == Node->GetMetaData(TEXT("DasComponentId")); }))
            {
                SCS->RemoveNodeAndPromoteChildren(Node);
                Changed = true;
            }
    for (const auto& C : D.Components)
    {
        auto* Child = Components.FindRef(C.Id);
        UClass* ComponentClass = C.NativeClass.IsEmpty() ? (Child ? Child->GeneratedClass.Get() : nullptr)
                                                         : FindObject<UClass>(nullptr, *C.NativeClass);
        if (!ComponentClass || !ComponentClass->IsChildOf(UActorComponent::StaticClass()) ||
            ComponentClass->HasAnyClassFlags(CLASS_Abstract | CLASS_Deprecated | CLASS_NewerVersionExists))
        {
            Error = TEXT("Component Blueprint is unavailable");
            return false;
        }
        const auto* Was = Previous(C);
        if (USCS_Node* Existing = Create ? nullptr : FindById(C.Id))
        {
            if (Existing->ComponentClass != ComponentClass)
            {
                Error = TEXT("Component composition differs from imported Blueprint: ")+ C.Name;
                return false;
            }
            if (Existing->GetVariableName() != FName(*C.Name))
            {
                if (FindByName(C.Name) || BP->GeneratedClass->FindFunctionByName(*C.Name))
                {
                    Error = TEXT("Component name conflicts with Blueprint member: ")+ C.Name;
                    return false;
                }
                FBlueprintEditorUtils::RenameComponentMemberVariable(BP, Existing, FName(*C.Name));
                Changed = true;
            }
            // Only declared values that changed are written: template edits made in the editor stay.
            if (Was && !Was->Defaults.OrderIndependentCompareEqual(C.Defaults))
            {
                if (!DasComponentDefaults(ComponentClass, C, Existing->ComponentTemplate, Error))
                    return false;
                Changed = true;
            }
            if (Was && (Was->bReadOnly != C.bReadOnly || Was->bEditable != C.bEditable || Was->bVisible != C.bVisible ||
                        !Was->Metadata.OrderIndependentCompareEqual(C.Metadata)))
            {
                SetNodeMetadata(Existing, C);
                Changed = true;
            }
            continue;
        }
        if (!Create && Was)
        {
            Error = TEXT("Component composition differs from imported Blueprint: ")+ C.Name;
            return false;
        }
        if (FindByName(C.Name) || BP->GeneratedClass->FindPropertyByName(*C.Name) ||
            BP->GeneratedClass->FindFunctionByName(*C.Name))
        {
            Error = TEXT("Component name conflicts with Blueprint member: ")+ C.Name;
            return false;
        }
        auto* Node = SCS->CreateNode(ComponentClass, FName(*C.Name));
        if (!DasComponentDefaults(ComponentClass, C, Node->ComponentTemplate, Error))
            return false;
        SetNodeMetadata(Node, C);
        SCS->AddNode(Node);
        Changed = true;
    }
    // Attach new components and the ones whose attachment changed.
    const auto* Root = Effective.Components.FindByPredicate([](const auto& C) { return C.bRoot; });
    for (const auto& C : D.Components)
    {
        const auto* Was = Previous(C);
        if (!Create && Was && Was->Attach == C.Attach && Was->AttachSocket == C.AttachSocket)
            continue;
        const FString ParentName = !C.Attach.IsEmpty()            ? C.Attach
                                   : Root && Root->Name != C.Name ? Root->Name
                                                                  : FString();
        if (ParentName.IsEmpty())
            continue;
        USCS_Node* ChildNode = FindByName(C.Name);
        USCS_Node* Parent = FindByName(ParentName);
        if (!ChildNode || !ChildNode->ComponentClass->IsChildOf(USceneComponent::StaticClass()))
            continue;
        if (!Parent)
        {
            for (auto* Ancestor = BP->ParentClass.Get(); Ancestor && !Parent; Ancestor = Ancestor->GetSuperClass())
                if (auto* ParentBP = Cast<UBlueprint>(Ancestor->ClassGeneratedBy))
                    if (ParentBP->SimpleConstructionScript)
                        for (auto* Node : ParentBP->SimpleConstructionScript->GetAllNodes())
                            if (Node->GetVariableName() == FName(*ParentName))
                            {
                                Parent = Node;
                                break;
                            }
            if (!Parent)
            {
                Error = TEXT("Inherited attachment node is unavailable: ")+ ParentName;
                return false;
            }
            // Cross-Blueprint parent references must retain the node in the child's SCS.
            ChildNode->SetParent(Parent);
            ChildNode->AttachToName = FName(*C.AttachSocket);
            Changed = true;
            continue;
        }
        if (USCS_Node* Current = SCS->FindParentNode(ChildNode))
            Current->RemoveChildNode(ChildNode);
        else
            SCS->RemoveNode(ChildNode, false);
        Parent->AddChildNode(ChildNode);
        ChildNode->AttachToName = FName(*C.AttachSocket);
        Changed = true;
    }
    for (const auto& O : D.ComponentOverrides)
    {
        USCS_Node* Original = nullptr;
        for (auto* Ancestor = BP->ParentClass.Get(); Ancestor && !Original; Ancestor = Ancestor->GetSuperClass())
            if (auto* ParentBP = Cast<UBlueprint>(Ancestor->ClassGeneratedBy))
                if (ParentBP->SimpleConstructionScript)
                    for (auto* Node : ParentBP->SimpleConstructionScript->GetAllNodes())
                        if (Node->GetVariableName() == FName(*O.Name) &&
                            Node->FindMetaDataEntryIndexForKey(TEXT("DasComponentId")) != INDEX_NONE &&
                                                               Node->GetMetaData(TEXT("DasComponentId")) == O.Id)
                        {
                            Original = Node;
                            break;
                        }
        if (!Original)
        {
            Error = TEXT("Inherited component node is unavailable: ")+ O.Name;
            return false;
        }
        const FComponentKey Key(Original);
        auto* Handler = BP->GetInheritableComponentHandler(Create);
        auto* Template = Handler ? Handler->GetOverridenComponentTemplate(Key) : nullptr;
        if (Create && !Template)
            Template = Handler ? Handler->CreateOverridenComponentTemplate(Key) : nullptr;
        FDasDeclaredComponent Defaults;
        Defaults.Name = O.Name;
        Defaults.Defaults = O.Defaults;
        if (!Template || !Template->IsA(Original->ComponentClass) ||
            !DasComponentDefaults(Original->ComponentClass, Defaults, Create ? Template : nullptr, Error))
        {
            if (Error.IsEmpty())
                Error = TEXT("Inherited component override template is unavailable: ")+ O.Name;
            return false;
        }
    }
    if (Changed)
        FBlueprintEditorUtils::MarkBlueprintAsStructurallyModified(BP);
    return true;
}
