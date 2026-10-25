#include "DasDefaultsMigration.h"
#include "DasScriptHosts.h"
#include "DasAuthoring.h"
#include "DasBlueprintService.h"
#include "DasBlueprintRollback.h"
#include "DasComponentDefaults.h"
#include "DasContainerTypes.h"
#include "DasActor.h"
#include "DasScriptComponent.h"
#include "DasRuntime.h"
#include "Engine/Blueprint.h"
#include "Engine/BlueprintGeneratedClass.h"
#include "Engine/SimpleConstructionScript.h"
#include "Engine/SCS_Node.h"
#include "Engine/InheritableComponentHandler.h"
#include "Engine/World.h"
#include "Engine/StaticMesh.h"
#include "Components/StaticMeshComponent.h"
#include "UObject/GCObject.h"
#include "UObject/StructOnScope.h"
#include "UObject/UObjectHash.h"
#include "UObject/MetaData.h"
#include "UObject/Package.h"
#include "Modules/ModuleManager.h"
#include "Misc/ScopeExit.h"

#if WITH_DEV_AUTOMATION_TESTS && DAS_WITH_EDITOR_TESTS
static int32 GDasDefaultsFailure = 0;
void DasFailDefaultsMergeForTesting(int32 Stage)
{
    GDasDefaultsFailure = Stage;
}
#endif
namespace
{
struct FImage
{
    TStrongObjectPtr<UObject> Object;
    TUniquePtr<FStructOnScope> Before, After;
    explicit FImage(UObject* O)
        : Object(O), Before(MakeUnique<FStructOnScope>(O->GetClass())), After(MakeUnique<FStructOnScope>(O->GetClass()))
    {
        for (TFieldIterator<FProperty> It(O->GetClass()); It; ++It)
        {
            auto* P = *It;
            P->CopyCompleteValue(P->ContainerPtrToValuePtr<void>(Before->GetStructMemory()),
                                 P->ContainerPtrToValuePtr<void>(O));
            P->CopyCompleteValue(P->ContainerPtrToValuePtr<void>(After->GetStructMemory()),
                                 P->ContainerPtrToValuePtr<void>(O));
        }
    }
};
struct FPlan : FGCObject
{
    TArray<TUniquePtr<FImage>> Images;
    TArray<TUniquePtr<FStructOnScope>> Values;
    struct FChange
    {
        FImage* Image;
        FProperty* Property;
    };
    TArray<FChange> Changes;
    TMap<UPackage*, bool> Dirty;
    FImage& Image(UObject* O)
    {
        for (auto& I : Images)
            if (I->Object.Get() == O)
                return *I;
        Dirty.FindOrAdd(O->GetOutermost(), O->GetOutermost()->IsDirty());
        Images.Add(MakeUnique<FImage>(O));
        return *Images.Last();
    }
    FStructOnScope& Value(UClass* Class)
    {
        Values.Add(MakeUnique<FStructOnScope>(Class));
        return *Values.Last();
    }
    virtual void AddReferencedObjects(FReferenceCollector& C) override
    {
        auto Keep = [&](FStructOnScope& S)
        {
            C.AddPropertyReferencesWithStructARO(CastChecked<UClass>(S.GetStruct()), S.GetStructMemory());
        };
        for (auto& I : Images)
        {
            Keep(*I->Before);
            Keep(*I->After);
        }
        for (auto& V : Values)
            Keep(*V);
    }
    virtual FString GetReferencerName() const override { return TEXT("DasDefaultsMergePlan"); }
};
bool Same(FProperty* P, const void* A, const void* B)
{
    if (auto* S = CastField<FStructProperty>(P))
        return MatchesDasStructDefault(S->Struct, A, B);
    return P->Identical(A, B, PPF_None);
}
void Merge(FProperty* P, const void* Old, const void* Actual, const void* Next, void* Out)
{
    P->CopyCompleteValue(Out, Actual);
    if (Same(P, Actual, Old))
    {
        P->CopyCompleteValue(Out, Next);
        return;
    }
    // Merge named struct members independently, but never mix quaternion coordinates.
    if (auto* S = CastField<FStructProperty>(P); S && S->Struct != TBaseStructure<FQuat>::Get())
        for (TFieldIterator<FProperty> It(S->Struct); It; ++It)
        {
            auto* F = *It;
            Merge(F, F->ContainerPtrToValuePtr<void>(Old), F->ContainerPtrToValuePtr<void>(Actual),
                  F->ContainerPtrToValuePtr<void>(Next), F->ContainerPtrToValuePtr<void>(Out));
        }
}
bool DefaultFieldMatches(FProperty* P, const FDasFieldSchema& F)
{
    if (!P || P->ArrayDim != 1)
        return false;
    switch (F.Kind)
    {
    case EDasScalarKind::Int32:
        return CastField<FIntProperty>(P) != nullptr;
    case EDasScalarKind::Float32:
        return CastField<FFloatProperty>(P) != nullptr;
    case EDasScalarKind::Double:
        return CastField<FDoubleProperty>(P) != nullptr;
    case EDasScalarKind::Bool:
        return CastField<FBoolProperty>(P) != nullptr;
    case EDasScalarKind::String:
        return CastField<FStrProperty>(P) != nullptr;
    case EDasScalarKind::Byte:
        return CastField<FByteProperty>(P) != nullptr;
    case EDasScalarKind::Text:
        return CastField<FTextProperty>(P) != nullptr;
    case EDasScalarKind::Name:
        return CastField<FNameProperty>(P) != nullptr;
    case EDasScalarKind::Enum:
    {
        auto* E = DasEnumPropertyType(P);
        return E && E->GetPathName() == F.EnumPath;
    }
    case EDasScalarKind::Class:
    {
        auto* C = CastField<FClassProperty>(P);
        return C && C->MetaClass && C->MetaClass->GetPathName() == F.ObjectClass;
    }
    case EDasScalarKind::Object:
    {
        auto* C = DasReferencePropertyClass(P);
        return !CastField<FClassProperty>(P) && C && C->GetPathName() == F.ObjectClass;
    }
    case EDasScalarKind::Struct:
    {
        if (auto* C = DasContainerProperty(F.StructPath))
            return P->SameType(C);
        auto* S = CastField<FStructProperty>(P);
        return S && S->Struct->GetPathName() == F.StructPath;
    }
    default:
        return false;
    }
}
bool OwnValue(FProperty* P, const FDasFieldSchema& F, void* Data, FString& Error)
{
    switch (F.Kind)
    {
    case EDasScalarKind::Int32:
        CastFieldChecked<FIntProperty>(P)->SetPropertyValue(Data, F.IntDefault);
        break;
    case EDasScalarKind::Float32:
        CastFieldChecked<FFloatProperty>(P)->SetPropertyValue(Data, F.FloatDefault);
        break;
    case EDasScalarKind::Double:
        CastFieldChecked<FDoubleProperty>(P)->SetPropertyValue(Data, F.DoubleDefault);
        break;
    case EDasScalarKind::Bool:
        CastFieldChecked<FBoolProperty>(P)->SetPropertyValue(Data, F.BoolDefault);
        break;
    case EDasScalarKind::String:
        CastFieldChecked<FStrProperty>(P)->SetPropertyValue(Data, F.StringDefault);
        break;
    case EDasScalarKind::Name:
        CastFieldChecked<FNameProperty>(P)->SetPropertyValue(Data, F.NameDefault);
        break;
    case EDasScalarKind::Byte:
        CastFieldChecked<FByteProperty>(P)->SetPropertyValue(Data, uint8(F.IntDefault));
        break;
    case EDasScalarKind::Text:
        CastFieldChecked<FTextProperty>(P)->SetPropertyValue(Data, F.TextDefault);
        break;
    case EDasScalarKind::Enum:
    {
        auto* E = ResolveDasEnum(F.EnumPath, Error);
        if (!E)
            return false;
        SetDasEnumPropertyNumber(P, Data, E->GetValueByNameString(F.EnumDefault));
        break;
    }
    case EDasScalarKind::Class:
    case EDasScalarKind::Object:
    {
        UObject* V = nullptr;
        if (!ResolveDasReferenceDefault(F.Kind == EDasScalarKind::Class, F.ObjectClass, F.StringDefault, V, Error))
            return false;
        if (F.Kind == EDasScalarKind::Class)
            CastFieldChecked<FClassProperty>(P)->SetObjectPropertyValue(Data, V);
        else
            DasReferenceSet(P, Data, V);
        break;
    }
    case EDasScalarKind::Struct:
    {
        if (auto* C = DasContainerProperty(F.StructPath))
        {
            FInstancedStruct V;
            if (!ParseDasStructDefault(F.StructPath, F.StructDefault, V, Error))
                return false;
            P->CopyCompleteValue(Data, C->ContainerPtrToValuePtr<void>(V.GetMemory()));
            break;
        }
        FInstancedStruct V;
        if (!ParseDasStructDefault(F.StructPath, F.StructDefault, V, Error))
            return false;
        if (V.GetScriptStruct() == TBaseStructure<FTransform>::Get())
        {
            const auto& T = V.Get<FTransform>();
            const FTransform Canonical(T.Rotator(), T.GetTranslation(), T.GetScale3D());
            CastFieldChecked<FStructProperty>(P)->Struct->CopyScriptStruct(Data, &Canonical);
        }
        else
            CastFieldChecked<FStructProperty>(P)->Struct->CopyScriptStruct(Data, V.GetMemory());
        break;
    }
    case EDasScalarKind::Event:
    case EDasScalarKind::Delegate:
        P->ClearValue(Data);
        break;
    default:
        Error = TEXT("Unsupported default property");
        return false;
    }
    return true;
}
FString StructText(const UScriptStruct* S, const void* Memory)
{
    FString Text = TEXT("(");
    bool First = true;
    for (TFieldIterator<FProperty> It(S); It; ++It)
    {
        auto* P = *It;
        auto* Data = P->ContainerPtrToValuePtr<void>(Memory);
        FString Part;
        if (auto* Nested = CastField<FStructProperty>(P))
            Part = StructText(Nested->Struct, Data);
        else if (auto* N = CastField<FNumericProperty>(P); N && N->IsFloatingPoint())
            Part = FString::Printf(TEXT("%.17g"), N->GetFloatingPointPropertyValue(Data));
        else
            P->ExportTextItem_Direct(Part, Data, nullptr, nullptr, PPF_Delimited);
        if (!First)
            Text += TEXT(",");
        First = false;
        Text += P->GetName() + TEXT("=")+ Part;
    }
    return Text + TEXT(")");
}
FString VariableText(FProperty* P, UObject* O)
{
    auto* Data = P->ContainerPtrToValuePtr<void>(O);
    if (P->IsA<FArrayProperty>() || P->IsA<FSetProperty>() || P->IsA<FMapProperty>())
        return DasPropertyDefaultText(P, Data);
    if (auto* S = CastField<FStructProperty>(P))
    {
        if (S->Struct != TBaseStructure<FTransform>::Get())
            return StructText(S->Struct, Data);
        const auto& V = *static_cast<const FTransform*>(Data);
        const auto T = V.GetTranslation(), Scale = V.GetScale3D();
        const auto R = V.Rotator();
        return FString::Printf(TEXT("%.17g,%.17g,%.17g|%.17g,%.17g,%.17g|%.17g,%.17g,%.17g"), T.X, T.Y, T.Z, R.Pitch,
                                    R.Yaw, R.Roll, Scale.X, Scale.Y, Scale.Z);
    }
    if (auto* S = CastField<FStrProperty>(P))
        return S->GetPropertyValue(Data);
    if (auto* N = CastField<FNumericProperty>(P); N && N->IsFloatingPoint())
        return FString::Printf(TEXT("%.17g"), N->GetFloatingPointPropertyValue(Data));
    if (auto* C = CastField<FClassProperty>(P))
    {
        auto* V = C->GetObjectPropertyValue(Data);
        return V ? V->GetPathName() : TEXT("None");
    }
    if (DasReferencePropertyClass(P))
    {
        auto* V = DasReferenceGet(P, Data);
        return V ? V->GetPathName() : TEXT("None");
    }
    FString Text;
    P->ExportTextItem_Direct(Text, Data, nullptr, O, PPF_None);
    return Text;
}
int32 Distance(UObject* O, UObject* Root)
{
    TSet<UObject*> Seen;
    int32 Depth = 0;
    while (O && O != Root && Depth < 128 && !Seen.Contains(O))
    {
        Seen.Add(O);
        O = O->GetArchetype();
        ++Depth;
    }
    return O == Root ? Depth : MAX_int32;
}
bool PlanRoot(FPlan& Plan, UObject* Root, FProperty* P, const void* Old, const void* Next, FString& Error)
{
    auto& Image = Plan.Image(Root);
    auto* Before = P->ContainerPtrToValuePtr<void>(Image.Before->GetStructMemory());
    auto* After = P->ContainerPtrToValuePtr<void>(Image.After->GetStructMemory());
    Merge(P, Old, Before, Next, After);
    TArray<UObject*> Objects;
    Root->GetArchetypeInstances(Objects);
    Objects.RemoveAll(
        [&](UObject* O)
        {
            if (auto* A = Cast<AActor>(O); A && A->IsActorBeingDestroyed())
                return true;
            if (auto* C = Cast<UActorComponent>(O);
                C && (C->IsBeingDestroyed() || (C->GetOwner() && C->GetOwner()->IsActorBeingDestroyed())))
                return true;
            // Objects of a running game (PIE) are merged like editor instances: a value still equal to the old
            // default follows the new one, a value the game changed is kept.
            return !IsValid(O) || O == Root || O->GetClass()->HasAnyClassFlags(CLASS_NewerVersionExists) ||
                   O->GetClass()->GetName().StartsWith(TEXT("SKEL_")) || O->GetClass()->GetName().StartsWith(
                                                                             TEXT("REINST_")) ||
                                                                             Distance(O, Root) == MAX_int32;
        });
    Objects.Sort([&](const UObject& A, const UObject& B)
                 { return Distance(const_cast<UObject*>(&A), Root) < Distance(const_cast<UObject*>(&B), Root); });
    if (Objects.Num() + Plan.Images.Num() > 8192)
    {
        Error = TEXT("Default migration exceeds 8192 objects");
        return false;
    }
    for (auto* O : Objects)
    {
        if (auto* W = O->GetWorld(); W && (W->IsPartitionedWorld() || !W->GetStreamingLevels().IsEmpty()))
        {
            Error = TEXT("Partitioned/streaming world defaults require a separate migration scope");
            return false;
        }
        // An unopened editor map can retain pre-construction component data. Do not
        // run arbitrary construction scripts from a defaults preflight or merge it.
        if (O->IsA<UActorComponent>() && !O->IsTemplate())
            if (auto* W = O->GetWorld(); W && !W->IsInitialized())
            {
                Error = TEXT("Open/initialize the referencing editor map before updating component defaults: ")+
                             W->GetPathName();
                return false;
            }
        UObject* NativeOwner = O->HasAnyFlags(RF_ClassDefaultObject) ? O : O->GetOuter();
        if (NativeOwner && NativeOwner->HasAnyFlags(RF_ClassDefaultObject))
            if (auto* BP = Cast<UBlueprint>(NativeOwner->GetClass()->ClassGeneratedBy))
            {
                const FString Wire = BP->GetOutermost()->GetMetaData().GetValue(
                                  BP, TEXT("DasDSLModel")),
                                  Id = BP->GetOutermost()->GetMetaData().GetValue(BP, TEXT("DasDSLClassId"));
                if (!Wire.IsEmpty())
                {
                    FDasAuthoringModule M;
                    FDasDeclaredClass E;
                    if (!FDasAuthoringModule::FromJson(Wire, M, Error) || !M.ResolveClass(Id, E, Error))
                        return false;
                    const auto* D = M.Classes.FindByPredicate([&](const auto& C) { return C.Id == Id; });
                    bool Explicit = false;
                    if (D)
                        for (const auto& G : D->NativeDefaults)
                            if (G.Defaults.Contains(P->GetName()) &&
                                DasNativeDefaultTarget(E.NativeParent, NativeOwner, G.Component, Error) == O)
                                Explicit = true;
                    if (Explicit)
                        continue;
                }
            }
        auto* Archetype = O->GetArchetype();
        if (Distance(Archetype, Root) == MAX_int32)
            continue;
        auto& Parent = Plan.Image(Archetype);
        auto& Child = Plan.Image(O);
        if (O->HasAnyFlags(RF_ClassDefaultObject))
            if (auto* BP = Cast<UBlueprint>(O->GetClass()->ClassGeneratedBy))
            {
                const FString Wire = BP->GetOutermost()->GetMetaData().GetValue(BP, TEXT("DasDSLModel"));
                const FString Id = BP->GetOutermost()->GetMetaData().GetValue(BP, TEXT("DasDSLClassId"));
                if (!Wire.IsEmpty())
                {
                    FDasAuthoringModule Model;
                    if (!FDasAuthoringModule::FromJson(Wire, Model, Error))
                        return false;
                    const auto* Decl = Model.Classes.FindByPredicate([&](const auto& C) { return C.Id == Id; });
                    if (!Decl)
                    {
                        Error = TEXT("Missing dependent default declaration");
                        return false;
                    }
                    if (Decl->PropertyOverrides.ContainsByPredicate([&](const auto& V)
                                                                    { return V.Name == P->GetName(); }))
                        continue;
                }
            }
        Merge(P, P->ContainerPtrToValuePtr<void>(Parent.Before->GetStructMemory()),
              P->ContainerPtrToValuePtr<void>(Child.Before->GetStructMemory()),
              P->ContainerPtrToValuePtr<void>(Parent.After->GetStructMemory()),
              P->ContainerPtrToValuePtr<void>(Child.After->GetStructMemory()));
    }
    return true;
}
void NotifyTemplate(UObject* O, FProperty* P)
{
    if (O->IsTemplate() && O->IsA<UActorComponent>())
    {
        FPropertyChangedEvent Event(P, EPropertyChangeType::ValueSet);
        O->PostEditChangeProperty(Event);
    }
}
void NativeValue(UObject* O, FProperty* P, const void* Data)
{
    if (!O->IsTemplate())
        if (auto* Mesh = Cast<UStaticMeshComponent>(O); Mesh && P->GetFName() == TEXT("StaticMesh"))
            Mesh->SetStaticMesh(Cast<UStaticMesh>(CastFieldChecked<FObjectProperty>(P)->GetObjectPropertyValue(Data)));
}
void RefreshInstance(UObject* O, FProperty*)
{
    if (!O->IsTemplate())
        if (auto* Scene = Cast<USceneComponent>(O))
            Scene->UpdateComponentToWorld();
}

}
bool DasInitializeInheritedDefaults(UClass* Class, const FDasAuthoringModule& Model, const FString& Id, FString& Error,
                                    bool ValidateOnly)
{
    const auto* Decl = Model.Classes.FindByPredicate([&](const auto& C) { return C.Id == Id; });
    if (!Class || !Decl)
    {
        Error = TEXT("Missing inherited default class");
        return false;
    }
    if (Decl->PropertyOverrides.IsEmpty())
        return true;
    FDasClassSchema Schema;
    FDasDeclaredClass Effective;
    if (!Model.ToActorSchema(Id, Schema, Error, true) || !Model.ResolveClass(Id, Effective, Error))
        return false;
    FStructOnScope Values(Class);
    TArray<FProperty*> Properties;
    for (const auto& O : Decl->PropertyOverrides)
    {
        const auto* F = Schema.Fields.FindByPredicate([&](const auto& V)
                                                      { return V.StableId == O.Id && V.Name == FName(*O.Name); });
        auto* P = Class->FindPropertyByName(*O.Name);
        if (!P || !F || !DefaultFieldMatches(P, *F) || !P->HasAnyPropertyFlags(CPF_Edit) ||
            P->HasAnyPropertyFlags(CPF_EditConst | CPF_DisableEditOnTemplate))
        {
            Error = TEXT("Inherited default requires an editable template property: ")+ O.Name;
            return false;
        }
        // The source parent is imported/validated before this helper is called.
        if (!OwnValue(P, *F, P->ContainerPtrToValuePtr<void>(Values.GetStructMemory()), Error))
            return false;
        Properties.Add(P);
    }
    if (ValidateOnly)
        return true;
    auto* CDO = Class->GetDefaultObject();
    auto* Contracts = DasHostContracts(CDO);
    if (!Contracts)
    {
        Error = TEXT("Unsupported inherited default host");
        return false;
    }
    CDO->Modify();
    for (auto* P : Properties)
        P->CopyCompleteValue(P->ContainerPtrToValuePtr<void>(CDO),
                             P->ContainerPtrToValuePtr<void>(Values.GetStructMemory()));
    for (const auto& O : Decl->PropertyOverrides)
        if (const auto* P = Effective.Properties.FindByPredicate([&](const auto& V) { return V.Id == O.Id; }))
            Contracts->Add(P->Id, DasDefaultSourceContract(*P));
    CastChecked<UBlueprintGeneratedClass>(Class)->UpdateCustomPropertyListForPostConstruction();
    CDO->MarkPackageDirty();
    return true;
}
bool DasInitializeNativeDefaults(UClass* Class, const FDasAuthoringModule& Model, const FString& Id, FString& Error,
                                 bool ValidateOnly)
{
    const auto* Decl = Model.Classes.FindByPredicate([&](const auto& C) { return C.Id == Id; });
    FDasDeclaredClass Effective;
    if (!Class || !Decl || !Model.ResolveClass(Id, Effective, Error))
        return false;
    if (Decl->NativeDefaults.IsEmpty())
        return true;
    auto* CDO = Class->GetDefaultObject();
    FPlan Plan;
    for (const auto& G : Decl->NativeDefaults)
    {
        auto* Target = DasNativeDefaultTarget(Effective.NativeParent, CDO, G.Component, Error);
        if (!Target)
            return false;
        auto& Image = Plan.Image(Target);
        for (const auto& V : G.Defaults)
        {
            auto* P = Target->GetClass()->FindPropertyByName(*V.Key);
            if (!DasParseEditableDefault(
                    P, V.Value, P ? P->ContainerPtrToValuePtr<void>(Image.After->GetStructMemory()) : nullptr, Error))
                return false;
            Plan.Changes.Add({&Image, P});
        }
    }
    if (ValidateOnly)
        return true;
    auto* Contracts = DasHostContracts(CDO);
    if (!Contracts)
    {
        Error = TEXT("Unsupported native defaults host");
        return false;
    }
    for (const auto& Change : Plan.Changes)
    {
        auto* O = Change.Image->Object.Get();
        auto* P = Change.Property;
        O->Modify();
        if (O->IsA<UActorComponent>())
            O->PreEditChange(P);
        P->CopyCompleteValue(P->ContainerPtrToValuePtr<void>(O),
                             P->ContainerPtrToValuePtr<void>(Change.Image->After->GetStructMemory()));
        NotifyTemplate(O, P);
        O->MarkPackageDirty();
    }
    for (const auto& G : Decl->NativeDefaults)
        for (const auto& V : G.Defaults)
            Contracts->Add(DasNativeDefaultKey(Id, G.Component, V.Key), DasNativeDefaultContract(V.Value));
    CastChecked<UBlueprintGeneratedClass>(Class)->UpdateCustomPropertyListForPostConstruction();
    CDO->MarkPackageDirty();
    return true;
}
bool DasMergeDefaults(UBlueprint* BP, const FDasAuthoringModule& Next, const FString& Id, FString& Error)
{
    FDasAuthoringModule Before;
    const FString PriorWire = BP->GetOutermost()->GetMetaData().GetValue(BP, TEXT("DasDSLModel"));
    if (!BP->GeneratedClass || !FDasAuthoringModule::FromJson(PriorWire, Before, Error) ||
        !FDasAuthoringModule::DefaultsOnly(Before, Next, Error))
        return false;
    FDasClassSchema OldSchema, NewSchema;
    if (!Before.ToActorSchema(Id, OldSchema, Error) || !Next.ToActorSchema(Id, NewSchema, Error) ||
        !FDasBlueprintService::Apply(BP, OldSchema, Error, true, EDasDefaultMigrationPolicy::PreserveOverrides))
        return false;
    const auto* Old = Before.Classes.FindByPredicate([&](const auto& C) { return C.Id == Id; });
    const auto* New = Next.Classes.FindByPredicate([&](const auto& C) { return C.Id == Id; });
    if (!Old || !New)
    {
        Error = TEXT("Missing default source identity");
        return false;
    }
    // A child must not accept a new source snapshot whose owning parent has not
    // been imported yet. Compare owned contracts, not stale ancestor snapshots.
    FString ParentId = New->ParentId;
    auto* ParentClass = BP->ParentClass.Get();
    while (!ParentId.IsEmpty())
    {
        auto* ParentBP = ParentClass ? Cast<UBlueprint>(ParentClass->ClassGeneratedBy) : nullptr;
        const auto* Required = Next.Classes.FindByPredicate([&](const auto& C) { return C.Id == ParentId; });
        if (!ParentBP || !Required || (ParentBP->Status != BS_UpToDate && ParentBP->Status != BS_UpToDateWithWarnings))
        {
            Error = TEXT("Compile/import the authored parent before updating child defaults: ")+ ParentId;
            return false;
        }
        FDasAuthoringModule Stored;
        const FString Wire = ParentBP->GetOutermost()->GetMetaData().GetValue(ParentBP, TEXT("DasDSLModel"));
        if (!FDasAuthoringModule::FromJson(Wire, Stored, Error))
            return false;
        const auto* Actual = Stored.Classes.FindByPredicate([&](const auto& C) { return C.Id == ParentId; });
        if (!Actual)
        {
            Error = TEXT("Missing imported parent identity: ")+ ParentId;
            return false;
        }
        const auto Delta = FDasAuthoringModule::Compare(*Actual, *Required);
        if (Delta.bDefaults || Delta.bStructural || Delta.bMetadata || Delta.bImplementation ||
            Actual->SourceScript != Required->SourceScript)
        {
            Error = TEXT("Reimport authored parent before child defaults: ")+ ParentId;
            return false;
        }
        FDasClassSchema ParentSchema;
        if (!Next.ToActorSchema(ParentId, ParentSchema, Error) ||
            !FDasBlueprintService::Apply(ParentBP, ParentSchema, Error, true,
                                         EDasDefaultMigrationPolicy::PreserveOverrides))
            return false;
        ParentId = Required->ParentId;
        ParentClass = ParentClass->GetSuperClass();
    }
    TArray<UBlueprint*> Scope;
    TArray<TStrongObjectPtr<UObject>> Assets;
    if (!FDasBlueprintRollback::GatherScope(BP, Scope, Assets, Error, true))
        return false;
    FPlan Plan;
    auto* Class = BP->GeneratedClass.Get();
    auto* CDO = Class->GetDefaultObject();
    Plan.Image(CDO);
    auto& OldValues = Plan.Value(Class);
    auto& NewValues = Plan.Value(Class);
    FDasClassSchema AllOld, AllNew;
    if (!Before.ToActorSchema(Id, AllOld, Error, true) || !Next.ToActorSchema(Id, AllNew, Error, true))
        return false;
    TSet<FString> Inherited;
    for (const auto& O : Old->PropertyOverrides)
        Inherited.Add(O.Id);
    for (const auto& O : New->PropertyOverrides)
        Inherited.Add(O.Id);
    for (const auto& F : AllOld.Fields)
    {
        const bool IsOwn =
            OldSchema.Fields.ContainsByPredicate([&](const auto& V) { return V.StableId == F.StableId; });
        if (!IsOwn && !Inherited.Contains(F.StableId))
            continue;
        const auto* NF = AllNew.Fields.FindByPredicate([&](const auto& V) { return V.StableId == F.StableId; });
        auto* P = Class->FindPropertyByName(F.Name);
        if (!P || !NF)
            return false;
        if (!IsOwn && (!DefaultFieldMatches(P, F) || !P->HasAnyPropertyFlags(CPF_Edit) ||
                       P->HasAnyPropertyFlags(CPF_EditConst | CPF_DisableEditOnTemplate)))
        {
            Error = TEXT("Inherited property layout/access differs from source: ")+ F.Name.ToString();
            return false;
        }
        auto* OldData = P->ContainerPtrToValuePtr<void>(OldValues.GetStructMemory());
        auto* NewData = P->ContainerPtrToValuePtr<void>(NewValues.GetStructMemory());
        const auto* OldOverride =
            Old->PropertyOverrides.FindByPredicate([&](const auto& O) { return O.Id == F.StableId; });
        const auto* NewOverride =
            New->PropertyOverrides.FindByPredicate([&](const auto& O) { return O.Id == F.StableId; });
        if (!IsOwn && !OldOverride)
            P->CopyCompleteValue(OldData, P->ContainerPtrToValuePtr<void>(BP->ParentClass->GetDefaultObject()));
        else if (!OwnValue(P, F, OldData, Error))
            return false;
        if (!IsOwn && !NewOverride)
            P->CopyCompleteValue(NewData, P->ContainerPtrToValuePtr<void>(BP->ParentClass->GetDefaultObject()));
        else if (!OwnValue(P, *NF, NewData, Error))
            return false;
        if (!PlanRoot(Plan, CDO, P, P->ContainerPtrToValuePtr<void>(OldValues.GetStructMemory()),
                      P->ContainerPtrToValuePtr<void>(NewValues.GetStructMemory()), Error))
            return false;
    }
    // Native host fields and fixed native default subobjects. Absent source keys
    // return to the actual immediate parent CDO, retaining manual overrides.
    FDasDeclaredClass NativeEffective;
    if (!Next.ResolveClass(Id, NativeEffective, Error))
        return false;
    TSet<FString> NativeGroups;
    for (const auto& G : Old->NativeDefaults)
        NativeGroups.Add(G.Component);
    for (const auto& G : New->NativeDefaults)
        NativeGroups.Add(G.Component);
    for (const auto& Component : NativeGroups)
    {
        auto* Target = DasNativeDefaultTarget(NativeEffective.NativeParent, CDO, Component, Error);
        auto* Baseline =
            DasNativeDefaultTarget(NativeEffective.NativeParent, BP->ParentClass->GetDefaultObject(), Component, Error);
        if (!Target || !Baseline)
            return false;
        const auto* OG = Old->NativeDefaults.FindByPredicate([&](const auto& G) { return G.Component == Component; });
        const auto* NG = New->NativeDefaults.FindByPredicate([&](const auto& G) { return G.Component == Component; });
        TSet<FString> Names;
        if (OG)
            for (const auto& V : OG->Defaults)
                Names.Add(V.Key);
        if (NG)
            for (const auto& V : NG->Defaults)
                Names.Add(V.Key);
        auto& OV = Plan.Value(Target->GetClass());
        auto& NV = Plan.Value(Target->GetClass());
        for (const auto& Name : Names)
        {
            auto* P = Target->GetClass()->FindPropertyByName(*Name);
            if (!P)
            {
                Error = TEXT("Native default field is missing");
                return false;
            }
            auto* A = P->ContainerPtrToValuePtr<void>(OV.GetStructMemory());
            auto* B = P->ContainerPtrToValuePtr<void>(NV.GetStructMemory());
            auto* Base = P->ContainerPtrToValuePtr<void>(Baseline);
            P->CopyCompleteValue(A, Base);
            P->CopyCompleteValue(B, Base);
            if (OG)
                if (const auto* V = OG->Defaults.Find(Name))
                    if (!DasParseEditableDefault(P, *V, A, Error))
                        return false;
            if (NG)
                if (const auto* V = NG->Defaults.Find(Name))
                    if (!DasParseEditableDefault(P, *V, B, Error))
                        return false;
            if (!PlanRoot(Plan, Target, P, A, B, Error))
                return false;
        }
    }
    // Root SCS templates: compare source baselines, not another template's manual edits.
    if (!Old->Components.IsEmpty() && !BP->SimpleConstructionScript)
    {
        Error = TEXT("Missing component composition");
        return false;
    }
    for (const auto& C : Old->Components)
    {
        const auto* NC = New->Components.FindByPredicate([&](const auto& V) { return V.Id == C.Id; });
        USCS_Node* Node = nullptr;
        for (auto* N : BP->SimpleConstructionScript->GetAllNodes())
            if (N->GetVariableName() == FName(*C.Name))
                Node = N;
        if (!NC || !Node || !Node->ComponentTemplate || Node->GetMetaData(TEXT("DasComponentId")) != C.Id)
        {
            Error = TEXT("Component identity differs from import");
            return false;
        }
        auto* Template = Node->ComponentTemplate.Get();
        auto& O = Plan.Value(Template->GetClass());
        auto& N = Plan.Value(Template->GetClass());
        TSet<FString> Names;
        for (const auto& Pair : C.Defaults)
            Names.Add(Pair.Key);
        for (const auto& Pair : NC->Defaults)
            Names.Add(Pair.Key);
        for (const auto& Name : Names)
        {
            auto* P = Template->GetClass()->FindPropertyByName(*Name);
            if (!P)
            {
                Error = TEXT("Missing component default property: ")+ Name;
                return false;
            }
            auto* OP = P->ContainerPtrToValuePtr<void>(O.GetStructMemory());
            auto* NP = P->ContainerPtrToValuePtr<void>(N.GetStructMemory());
            auto* Base = P->ContainerPtrToValuePtr<void>(Template->GetClass()->GetDefaultObject());
            P->CopyCompleteValue(OP, Base);
            P->CopyCompleteValue(NP, Base);
            if (const auto* V = C.Defaults.Find(Name))
                if (!DasComponentDefaultValue(P, *V, OP, Error))
                    return false;
            if (const auto* V = NC->Defaults.Find(Name))
                if (!DasComponentDefaultValue(P, *V, NP, Error))
                    return false;
            if (!PlanRoot(Plan, Template, P, OP, NP, Error))
                return false;
        }
    }
    // Existing inherited component override templates have stable identities.
    for (const auto& O : Old->ComponentOverrides)
    {
        const auto* N = New->ComponentOverrides.FindByPredicate([&](const auto& V) { return V.Id == O.Id; });
        USCS_Node* Source = nullptr;
        for (auto* Parent = BP->ParentClass.Get(); Parent && !Source; Parent = Parent->GetSuperClass())
            if (auto* B = Cast<UBlueprint>(Parent->ClassGeneratedBy))
                if (B->SimpleConstructionScript)
                    for (auto* Node : B->SimpleConstructionScript->GetAllNodes())
                        if (Node->GetVariableName() == FName(*O.Name))
                            Source = Node;
        auto* Handler = BP->GetInheritableComponentHandler(false);
        auto* Template = Source && Handler ? Handler->GetOverridenComponentTemplate(FComponentKey(Source)) : nullptr;
        if (!N || !Template)
        {
            Error = TEXT("Inherited component override identity differs");
            return false;
        }
        auto& OV = Plan.Value(Template->GetClass());
        auto& NV = Plan.Value(Template->GetClass());
        TSet<FString> Names;
        for (const auto& P : O.Defaults)
            Names.Add(P.Key);
        for (const auto& P : N->Defaults)
            Names.Add(P.Key);
        for (const auto& Name : Names)
        {
            auto* P = Template->GetClass()->FindPropertyByName(*Name);
            if (!P)
                return false;
            auto* A = P->ContainerPtrToValuePtr<void>(OV.GetStructMemory());
            auto* B = P->ContainerPtrToValuePtr<void>(NV.GetStructMemory());
            auto* Base = P->ContainerPtrToValuePtr<void>(Source->ComponentTemplate);
            P->CopyCompleteValue(A, Base);
            P->CopyCompleteValue(B, Base);
            if (const auto* V = O.Defaults.Find(Name))
                if (!DasComponentDefaultValue(P, *V, A, Error))
                    return false;
            if (const auto* V = N->Defaults.Find(Name))
                if (!DasComponentDefaultValue(P, *V, B, Error))
                    return false;
            if (!PlanRoot(Plan, Template, P, A, B, Error))
                return false;
        }
    }
    for (auto& I : Plan.Images)
        for (TFieldIterator<FProperty> It(I->Object->GetClass()); It; ++It)
        {
            auto* P = *It;
            if (!P->Identical(P->ContainerPtrToValuePtr<void>(I->Before->GetStructMemory()),
                              P->ContainerPtrToValuePtr<void>(I->After->GetStructMemory())))
                Plan.Changes.Add({I.Get(), P});
        }
    struct FBPImage
    {
        UBlueprint* BP;
        TArray<FBPVariableDescription> Variables;
        EBlueprintStatus Status;
    };
    TArray<FBPImage> Blueprints;
    for (auto* B : Scope)
    {
        Blueprints.Add({B, B->NewVariables, B->Status});
        Plan.Dirty.FindOrAdd(B->GetOutermost(), B->GetOutermost()->IsDirty());
    }
    const auto* Meta = FMetaData::GetMapForObject(BP);
    const TMap<FName, FString> Metadata = Meta ? *Meta : TMap<FName, FString>();
    auto& Runtime = FModuleManager::LoadModuleChecked<FDasRuntimeModule>(TEXT("DasRuntime"));
    if (!Runtime.BeginDefaultUpdate())
    {
        Error = TEXT("Defaults update requires an idle runtime with no executing calls or pending events");
        return false;
    }
    bool Applied = false;
    ON_SCOPE_EXIT
    {
        Runtime.EndDefaultUpdate(Class, Applied);
    };
    auto Restore = [&](const FString& Why)
    {
        for (const auto& Change : Plan.Changes)
            NativeValue(Change.Image->Object.Get(), Change.Property,
                        Change.Property->ContainerPtrToValuePtr<void>(Change.Image->Before->GetStructMemory()));
        for (auto& I : Plan.Images)
            for (TFieldIterator<FProperty> It(I->Object->GetClass()); It; ++It)
            {
                auto* P = *It;
                P->CopyCompleteValue(P->ContainerPtrToValuePtr<void>(I->Object.Get()),
                                     P->ContainerPtrToValuePtr<void>(I->Before->GetStructMemory()));
            }
        for (const auto& Change : Plan.Changes)
        {
            NotifyTemplate(Change.Image->Object.Get(), Change.Property);
            RefreshInstance(Change.Image->Object.Get(), Change.Property);
        }
        for (auto& B : Blueprints)
        {
            B.BP->NewVariables = B.Variables;
            B.BP->Status = B.Status;
            CastChecked<UBlueprintGeneratedClass>(B.BP->GeneratedClass)->UpdateCustomPropertyListForPostConstruction();
        }
        BP->GetOutermost()->GetMetaData().SetObjectValues(BP, Metadata);
        for (const auto& D : Plan.Dirty)
            D.Key->SetDirtyFlag(D.Value);
        Error = TEXT("Defaults update rolled back: ")+ Why;
        UE_LOG(LogTemp, Display, TEXT("DAS_DEFAULTS_MERGE_ROLLBACK=PASS"));
        return false;
    };
    for (int32 Index = 0; Index < Plan.Changes.Num(); ++Index)
    {
        const auto& Change = Plan.Changes[Index];
        auto* O = Change.Image->Object.Get();
        auto* P = Change.Property;
        O->Modify();
        if (O->IsTemplate() && O->IsA<UActorComponent>())
            O->PreEditChange(P);
        NativeValue(O, P, P->ContainerPtrToValuePtr<void>(Change.Image->After->GetStructMemory()));
        P->CopyCompleteValue(P->ContainerPtrToValuePtr<void>(O),
                             P->ContainerPtrToValuePtr<void>(Change.Image->After->GetStructMemory()));
        NotifyTemplate(O, P);
        RefreshInstance(O, P);
        if (!P->Identical(P->ContainerPtrToValuePtr<void>(O),
                          P->ContainerPtrToValuePtr<void>(Change.Image->After->GetStructMemory())))
            return Restore(TEXT("Native property handling changed a staged value"));
#if WITH_DEV_AUTOMATION_TESTS && DAS_WITH_EDITOR_TESTS
        if (GDasDefaultsFailure == 1)
            return Restore(TEXT("Injected value-stage failure"));
#endif
    }
    // Persist actual CDO defaults for future Blueprint recompilation, including manual values.
    for (auto& B : Blueprints)
    {
        if (B.BP == BP)
            for (auto& V : B.BP->NewVariables)
                if (New->Properties.ContainsByPredicate(
                        [&](const auto& F)
                        {
                            return F.Name == V.VarName.ToString() &&
                                   F.Type.Kind != TEXT("event") && F.Type.Kind != TEXT("delegate");
                        }))
                    if (auto* P = B.BP->GeneratedClass->FindPropertyByName(V.VarName))
                        V.DefaultValue = VariableText(P, B.BP->GeneratedClass->GetDefaultObject());
        CastChecked<UBlueprintGeneratedClass>(B.BP->GeneratedClass)->UpdateCustomPropertyListForPostConstruction();
    }
    auto* Contracts = DasHostContracts(CDO);
    if (!Contracts)
        return Restore(TEXT("Unsupported default owner"));
    CDO->Modify();
    for (const auto& P : New->Properties)
        Contracts->Add(P.Id, DasDefaultSourceContract(P));
    for (const auto& O : Old->PropertyOverrides)
        Contracts->Remove(O.Id);
    FDasDeclaredClass Effective;
    if (!Next.ResolveClass(Id, Effective, Error))
        return Restore(Error);
    for (const auto& O : New->PropertyOverrides)
        if (const auto* P = Effective.Properties.FindByPredicate([&](const auto& V) { return V.Id == O.Id; }))
            Contracts->Add(P->Id, DasDefaultSourceContract(*P));
    const FString NativePrefix = TEXT("native:")+ Id + TEXT(":");
    for (auto It = Contracts->CreateIterator(); It; ++It)
        if (It.Key().StartsWith(NativePrefix))
            It.RemoveCurrent();
    for (const auto& G : New->NativeDefaults)
        for (const auto& V : G.Defaults)
            Contracts->Add(DasNativeDefaultKey(Id, G.Component, V.Key), DasNativeDefaultContract(V.Value));
    FString Wire;
    if (!Next.ToJson(Wire, Error))
        return Restore(Error);
    BP->Modify();
    BP->GetOutermost()->GetMetaData().SetValue(BP, TEXT("DasDSLModel"), *Wire);
#if WITH_DEV_AUTOMATION_TESTS && DAS_WITH_EDITOR_TESTS
    if (GDasDefaultsFailure == 2)
        return Restore(TEXT("Injected metadata-stage failure"));
#endif
    BP->MarkPackageDirty();
    CDO->MarkPackageDirty();
    for (const auto& Change : Plan.Changes)
        Change.Image->Object->MarkPackageDirty();
    // Template edits notify the editor, which marks the Blueprint as needing a compile. The merge changes
    // values only (variable defaults included), so a compiled Blueprint stays compiled; otherwise the next
    // structural reimport would be refused until someone compiles it.
    for (auto& B : Blueprints)
        if (B.Status == BS_UpToDate || B.Status == BS_UpToDateWithWarnings)
            B.BP->Status = B.Status;
    Applied = true;
    UE_LOG(LogTemp, Display,
           TEXT("DAS_DEFAULTS_MERGE=PASS changed=%d objects=%d"), Plan.Changes.Num(), Plan.Images.Num());
    return true;
}
