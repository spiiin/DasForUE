#include "DasTypeService.h"
#include "DasTypeAdapters.h"
#include "DasSoftReferences.h"
#include "DasTypePins.h"
#include "DasReflectedTypes.h"
#include "DasSchema.h"
#include "DasRuntime.h"
#include "DasDefinedEnum.h"
#include "DasTypeRegistry.h"
#include "StructUtils/UserDefinedStruct.h"
#include "UserDefinedStructure/UserDefinedStructEditorData.h"
#include "Kismet2/EnumEditorUtils.h"
#include "Kismet2/StructureEditorUtils.h"
#include "EdGraphSchema_K2.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "Modules/ModuleManager.h"
#include "UObject/MetaData.h"
#include "UObject/StrProperty.h"
#include "Misc/PackageName.h"
#include "Misc/Paths.h"
#include "Misc/SecureHash.h"
namespace
{

bool SameTypeWire(const FString& Stored, const FString& Expected)
{
    FDasReflectedTypes M;
    FString Normal, Error;
    return FDasReflectedTypes::FromJson(Stored, M, Error) && M.ToJson(Normal, Error) && Normal == Expected;
}
bool TypeFieldDefaultMatches(UUserDefinedStruct* U, const FDasDeclaredProperty& D)
{
    auto* P = FStructureEditorUtils::GetPropertyByFriendlyName(U, D.Name);
    const auto* M = U->GetDefaultInstance();
    if (auto* Soft = CastField<FSoftObjectProperty>(P))
        return D.Type.Kind == TEXT("struct") && DasSoftCarrierForProperty(P) &&
                                   DasSoftCarrierForProperty(P)->GetPathName() == D.Type.TypePath &&
                                   Soft->GetPropertyValue_InContainer(M).IsNull();
    if (D.Type.Kind == TEXT("enum"))
    {
        auto* E = DasEnumPropertyType(P);
        return E && DasEnumPropertyNumber(P, P->ContainerPtrToValuePtr<void>(M)) ==
                        E->GetValueByNameString(D.Default.String);
    }
    if (auto* Set = CastField<FSetProperty>(P))
        return D.Type.Kind == TEXT("set") && FScriptSetHelper(Set, Set->ContainerPtrToValuePtr<void>(M)).Num() == 0;
    if (auto* Map = CastField<FMapProperty>(P))
        return D.Type.Kind == TEXT("map") && FScriptMapHelper(Map, Map->ContainerPtrToValuePtr<void>(M)).Num() == 0;
    if (auto* Array = CastField<FArrayProperty>(P))
        return D.Type.Kind == TEXT("array") &&
                                   FScriptArrayHelper(Array, Array->ContainerPtrToValuePtr<void>(M)).Num() == 0;
    if (auto* V = CastField<FByteProperty>(P))
        return !V->Enum && V->GetPropertyValue_InContainer(M) == uint8(D.Default.Number);
    if (auto* V = CastField<FTextProperty>(P))
    {
        FText Expected;
        FString Error;
        return ParseDasTextDefault(D.Default.String, Expected, Error) &&
               DasTextDefaultsEqual(V->GetPropertyValue_InContainer(M), Expected);
    }
    if (auto* V = CastField<FIntProperty>(P))
        return V->GetPropertyValue_InContainer(M) == int32(D.Default.Number);
    if (auto* V = CastField<FFloatProperty>(P))
        return V->GetPropertyValue_InContainer(M) == float(D.Default.Number);
    if (auto* V = CastField<FDoubleProperty>(P))
        return V->GetPropertyValue_InContainer(M) == D.Default.Number;
    if (auto* V = CastField<FBoolProperty>(P))
        return V->GetPropertyValue_InContainer(M) == D.Default.Boolean;
    if (auto* V = CastField<FStrProperty>(P))
        return V->GetPropertyValue_InContainer(M) == D.Default.String;
    if (DasReferencePropertyClass(P))
        return !DasReferenceGet(P, P->ContainerPtrToValuePtr<void>(M));
    if (auto* V = CastField<FNameProperty>(P))
        return V->GetPropertyValue_InContainer(M) == FName(*D.Default.String);
    if (auto* V = CastField<FStructProperty>(P))
    {
        if (DasMathAdapter(D.Type.TypePath))
        {
            FInstancedStruct Expected;
            FString Error;
            return V->Struct->GetPathName() == D.Type.TypePath &&
                   ParseDasStructDefault(D.Type.TypePath, D.Default.String, Expected, Error) &&
                   V->Struct->CompareScriptStruct(V->ContainerPtrToValuePtr<void>(M), Expected.GetMemory(), 0);
        }
        auto* Child = Cast<UUserDefinedStruct>(V->Struct);
        return Child && MatchesDasStructDefault(Child, V->ContainerPtrToValuePtr<void>(M), Child->GetDefaultInstance());
    }
    return false;
}
FString TypeFieldDefault(const FDasDeclaredProperty& P)
{
    if (DasSoftProperty(P.Type.TypePath))
        return TEXT("None");
    if (P.Type.Kind == TEXT("enum"))
        return P.Default.String;
    if (P.Type.Kind == TEXT("struct") && DasMathAdapter(P.Type.TypePath))
    {
        FInstancedStruct Value;
        FString Error;
        if (!ParseDasStructDefault(P.Type.TypePath, P.Default.String, Value, Error))
            return {};
        if (P.Type.TypePath == TEXT("/Script/CoreUObject.Vector"))
        {
            const auto& V = Value.Get<FVector>();
            return FString::Printf(TEXT("%.17g,%.17g,%.17g"), V.X, V.Y, V.Z);
        }
        if (P.Type.TypePath == TEXT("/Script/CoreUObject.Rotator"))
        {
            const auto& V = Value.Get<FRotator>();
            return FString::Printf(TEXT("%.17g,%.17g,%.17g"), V.Pitch, V.Yaw, V.Roll);
        }
        // Vector2D uses its struct pin grammar; it used to fall through to the Transform branch.
        if (P.Type.TypePath == TEXT("/Script/CoreUObject.Vector2D"))
        {
            const auto& V = Value.Get<FVector2D>();
            return FString::Printf(TEXT("(X=%.17g,Y=%.17g)"), V.X, V.Y);
        }
        return Value.Get<FTransform>().ToString();
    }
    if (P.Type.Kind == TEXT("object"))
        return TEXT("None");
    if (P.Type.Kind == TEXT("text"))
        return P.Default.String;
    if (P.Type.Kind == TEXT("string") || P.Type.Kind == TEXT("name"))
        return P.Default.String;
    if (P.Type.Kind == TEXT("bool"))
        return P.Default.Boolean ? TEXT("true"): TEXT("false");
    return P.Type.Kind == TEXT("int")? FString::FromInt(int32(P.Default.Number))
                                     : FString::Printf(TEXT("%.17g"), P.Default.Number);
}
}
bool FDasTypeService::Apply(const FString& Script, const FString& Folder, TArray<UObject*>& Out, FString& Error)
{
    Out.Reset();
    Error.Reset();
    if (!IsInGameThread() || Script != FPaths::GetCleanFilename(Script) ||
        FPaths::GetExtension(Script) != TEXT("das") || !Folder.StartsWith(TEXT("/Game/")) ||
                                                                          !FPackageName::IsValidLongPackageName(Folder))
    {
        Error = TEXT("Expected local Script.das and /Game type folder");
        return false;
    }
    auto& R = FModuleManager::LoadModuleChecked<FDasRuntimeModule>(TEXT("DasRuntime"));
    FDasReflectedTypes Model;
    FString Wire;
    if (!R.ReadReflectedTypes(R.ScriptPath(Script), Model, Error))
        return false;
    TMap<FString, int32> StructRefs;
    for (int32 I = 0; I < Model.Structs.Num(); ++I)
        StructRefs.Add(TEXT("/DasTypes/")+ FMD5::HashAnsiString(*Model.Structs[I].Id) + TEXT(".Type"), I);
    TArray<FDasDeclaredClass> Ordered;
    TSet<int32> Added;
    TFunction<void(int32)> Order;
    Order = [&](int32 I)
    {
        if (Added.Contains(I))
            return;
        for (const auto& P : Model.Structs[I].Properties)
            if (P.Type.Kind == TEXT("struct") ||
                                    (P.Type.Kind == TEXT("array") && P.Type.Element->Kind == TEXT("struct")) ||
                                     (P.Type.Kind == TEXT("map") && P.Type.Value->Kind == TEXT("struct")))
            {
                const auto* Ref =
                    StructRefs.Find(P.Type.Kind == TEXT("map")? P.Type.Value->TypePath
                                                              : P.Type.Kind == TEXT("array")? P.Type.Element->TypePath
                                                                                            : P.Type.TypePath);
                if (Ref)
                    Order(*Ref);
            }
        Added.Add(I);
        Ordered.Add(Model.Structs[I]);
    };
    for (int32 I = 0; I < Model.Structs.Num(); ++I)
        Order(I);
    Model.Structs = MoveTemp(Ordered);
    if (!Model.ToJson(Wire, Error))
        return false;
    FDasPinTypeResolver Resolve = [&](EDasTypeKind Kind, const FString& Path) -> UObject*
    {
        if (!Path.StartsWith(TEXT("/DasTypes/")))
            return FindObject<UObject>(nullptr, *Path);
        FString Name;
        if (Kind == EDasTypeKind::Enum)
        {
            const auto* E = Model.Enums.FindByPredicate(
                [&](const auto& V) { return TEXT("/DasTypes/")+ FMD5::HashAnsiString(*V.Id) + TEXT(".Type")== Path; });
            if (E)
                Name = E->Name;
        }
        if (Kind == EDasTypeKind::Struct)
        {
            const auto* S = Model.Structs.FindByPredicate(
                [&](const auto& V) { return TEXT("/DasTypes/")+ FMD5::HashAnsiString(*V.Id) + TEXT(".Type")== Path; });
            if (S)
                Name = S->Name;
        }
        return Name.IsEmpty()
                   ? nullptr
                   : LoadObject<UObject>(nullptr, *(Folder + TEXT("/")+ Name + TEXT(".")+ Name), nullptr, LOAD_NoWarn);
    };
    auto PinFor = [&](const FDasDeclaredProperty& P)
    {
        return DasTypePinChecked(P.Type, Resolve);
    };
    struct FTarget
    {
        FString Package, Id;
        UObject* Existing = nullptr;
    };
    TArray<FTarget> Targets;
    auto Preflight = [&](const FString& Name, const FString& Id, UClass* Expected)
    {
        FTarget T;
        T.Package = Folder + TEXT("/")+ Name;
        T.Id = Id;
        T.Existing = LoadObject<UObject>(nullptr, *(T.Package + TEXT(".")+ Name), nullptr, LOAD_NoWarn);
        if (!T.Existing && FPackageName::DoesPackageExist(T.Package))
        {
            Error = TEXT("Existing type package could not load");
            return false;
        }
        if (T.Existing &&
            (!T.Existing->IsA(Expected) || T.Existing->GetOutermost()->GetMetaData().GetValue(
                                               T.Existing, TEXT("DasTypeId")) != Id ||
                                               !SameTypeWire(T.Existing->GetOutermost()->GetMetaData().GetValue(
                                                   T.Existing, TEXT("DasTypesModel")), Wire)))
        {
            Error = TEXT("Existing type conflict or changed definition requires explicit type migration: ")+ T.Package;
            return false;
        }
        Targets.Add(T);
        return true;
    };
    for (const auto& E : Model.Enums)
    {
        if (!Preflight(E.Name, E.Id, UDasDefinedEnum::StaticClass()))
            return false;
        if (auto* Existing = Cast<UDasDefinedEnum>(Targets.Last().Existing))
        {
            if (Existing->NumEnums() != E.Entries.Num() + 1)
            {
                Error = TEXT("Authored enum asset differs from stored declaration");
                return false;
            }
            for (const auto& V : E.Entries)
                if (Existing->GetValueByNameString(V.Name) != V.Value)
                {
                    Error = TEXT("Authored enum value drift requires migration");
                    return false;
                }
        }
    }
    for (const auto& S : Model.Structs)
    {
        if (!Preflight(S.Name, S.Id, UUserDefinedStruct::StaticClass()))
            return false;
        if (auto* Existing = Cast<UUserDefinedStruct>(Targets.Last().Existing))
        {
            const auto& Variables = FStructureEditorUtils::GetVarDesc(Existing);
            if (Existing->Status != UDSS_UpToDate || Variables.Num() != S.Properties.Num())
            {
                Error = TEXT("Authored struct layout drift requires migration");
                return false;
            }
            for (const auto& P : S.Properties)
            {
                const auto* V = Variables.FindByPredicate([&](const auto& X) { return X.FriendlyName == P.Name; });
                if (!V || V->ToPinType() != PinFor(P) || !TypeFieldDefaultMatches(Existing, P) ||
                    V->MetaData.FindRef(TEXT("DasMemberId")) != P.Id)
                {
                    Error = FString::Printf(
                        TEXT("Authored struct field drift: %s; default=%s expected=%s; id=%s expected=%s"), *P.Name,
                             V          ? *V->DefaultValue
                                        : TEXT("missing"), *TypeFieldDefault(P),
                                      V ? *V->MetaData.FindRef(TEXT("DasMemberId")) : TEXT("missing"), *P.Id);
                    return false;
                }
            }
        }
    }
    TArray<UObject*> Created;
    auto Fail = [&](const TCHAR* Issue)
    {
        for (auto* A : Created)
        {
            A->ClearFlags(RF_Public | RF_Standalone);
            A->SetFlags(RF_Transient);
            A->Rename(nullptr, GetTransientPackage(), REN_DontCreateRedirectors | REN_DoNotDirty);
        }
        Out.Reset();
        Error = Issue;
        return false;
    };
    int32 Index = 0;
    for (const auto& E : Model.Enums)
    {
        auto& T = Targets[Index++];
        if (T.Existing)
        {
            Out.Add(T.Existing);
            continue;
        }
        auto* U = NewObject<UDasDefinedEnum>(CreatePackage(*T.Package), *E.Name, RF_Public | RF_Standalone);
        if (!U)
            return Fail(TEXT("Could not create authored enum"));
        Created.Add(U);
        U->StableId = E.Id;
        TArray<TPair<FName, int64>> Entries;
        for (const auto& V : E.Entries)
        {
            Entries.Add({FName(*(E.Name + TEXT("::")+ V.Name)), V.Value});
            U->MemberIds.Add(V.Value, V.Id.IsEmpty() ? E.Id + TEXT(".")+ V.Name : V.Id);
        }
        if (!U->SetEnums(Entries, UEnum::ECppForm::Namespaced, UEnum::EUnderlyingType::uint8, EEnumFlags::None,
                         UEnum::EAddMaxKeyIfMissing::Yes))
            return Fail(TEXT("Could not assign authored enum values"));
        for (int32 I = 0; I < E.Entries.Num(); ++I)
            U->SetMetaData(TEXT("DisplayName"), *E.Entries[I].Name, I);
        U->SetMetaData(TEXT("Hidden"), TEXT("true"), U->NumEnums() - 1);
        for (const auto& M : E.Metadata)
            U->SetMetaData(*M.Key, *M.Value);
        U->SetMetaData(TEXT("BlueprintType"), TEXT("true"));
        Out.Add(U);
    }
    for (const auto& S : Model.Structs)
    {
        auto& T = Targets[Index++];
        if (T.Existing)
        {
            Out.Add(T.Existing);
            continue;
        }
        auto* U = FStructureEditorUtils::CreateUserDefinedStruct(CreatePackage(*T.Package), *S.Name,
                                                                 RF_Public | RF_Standalone);
        if (!U)
            return Fail(TEXT("Could not create authored struct"));
        Created.Add(U);
        auto& Variables = FStructureEditorUtils::GetVarDesc(U);
        Variables.Empty();
        for (const auto& P : S.Properties)
        {
            if (P.Type.Kind == TEXT("enum"))
            {
                const auto Pin = PinFor(P);
                if (!FStructureEditorUtils::CanHaveAMemberVariableOfType(U, Pin))
                    return Fail(TEXT("Unsupported authored enum member pin"));
                FStructVariableDescription Variable;
                Variable.VarGuid = FGuid::NewGuid();
                const auto UniqueId =
                    CastChecked<UUserDefinedStructEditorData>(U->EditorData)->GenerateUniqueNameIdForMemberVariable();
                Variable.VarName = *FString::Printf(TEXT("%s_%u_%s"), *P.Name, UniqueId,
                                                         *Variable.VarGuid.ToString(EGuidFormats::Digits));
                Variable.FriendlyName = P.Name;
                Variable.SetPinType(Pin);
                Variable.DefaultValue = P.Default.String;
                for (const auto& Meta : P.Metadata)
                    Variable.MetaData.Add(*Meta.Key, Meta.Value);
                Variable.MetaData.Add(TEXT("DasMemberId"), P.Id);
                FStructureEditorUtils::BroadcastPreChange(U);
                FStructureEditorUtils::GetVarDesc(U).Add(MoveTemp(Variable));
                FStructureEditorUtils::OnStructureChanged(
                    U, FStructureEditorUtils::EStructureEditorChangeInfo::AddedVariable);
                continue;
            }
            if (!FStructureEditorUtils::AddVariable(U, PinFor(P)))
                return Fail(TEXT("Could not add authored struct field"));
            const FGuid Guid = FStructureEditorUtils::GetVarDesc(U).Last().VarGuid;
            if (!FStructureEditorUtils::RenameVariable(U, Guid, P.Name) ||
                (P.Type.Kind !=
                 TEXT("object") &&
                      (P.Type.Kind != TEXT("struct") || DasMathAdapter(P.Type.TypePath)) &&
                       P.Type.Kind != TEXT("array") &&
                                           P.Type.Kind !=
                                               TEXT("map") &&
                                                    P.Type.Kind !=
                                                        TEXT("set") &&
                                                             FStructureEditorUtils::GetVarDesc(U).Last().DefaultValue !=
                                                                 TypeFieldDefault(P) &&
                                                             !FStructureEditorUtils::ChangeVariableDefaultValue(
                                                                 U, Guid, TypeFieldDefault(P))))
            {
                UE_LOG(LogTemp, Error,
                       TEXT("Authored struct field %s.%s default=%s"), *S.Name, *P.Name, *TypeFieldDefault(P));
                return Fail(TEXT("Could not set authored struct field name/default"));
            }
            FStructureEditorUtils::SetMetaData(U, Guid, TEXT("DasMemberId"), P.Id);
            for (const auto& M : P.Metadata)
                FStructureEditorUtils::SetMetaData(U, Guid, *M.Key, M.Value);
        }
        FStructureEditorUtils::CompileStructure(U);
        if (U->Status != UDSS_UpToDate)
            return Fail(TEXT("Authored struct compilation failed"));
        for (const auto& M : S.Metadata)
            U->SetMetaData(*M.Key, *M.Value);
        U->SetMetaData(TEXT("BlueprintType"), TEXT("true"));
        Out.Add(U);
    }
    for (int32 I = 0; I < Out.Num(); ++I)
    {
        auto* A = Out[I];
        if (Created.Contains(A))
        {
            auto& Meta = A->GetOutermost()->GetMetaData();
            Meta.SetValue(A, TEXT("DasTypeId"), *Targets[I].Id);
            Meta.SetValue(A, TEXT("DasTypesModel"), *Wire);
            Meta.SetValue(A, TEXT("DasTypesSource"), *Script);
            A->MarkPackageDirty();
            FAssetRegistryModule::AssetCreated(A);
        }
    }
    UE_LOG(LogTemp, Display, TEXT("DAS_TYPES_APPLY PASS %d reflected assets"), Out.Num());
    return true;
}

bool FDasTypeService::BuildRegistry(const TArray<UObject*>& Types, const FString& Package, UDasTypeRegistry*& Out,
                                    FString& Error)
{
    Out = nullptr;
    Error.Reset();
    if (!IsInGameThread() || !Package.StartsWith(TEXT("/Game/")) || !FPackageName::IsValidLongPackageName(Package) ||
                                                 Types.IsEmpty() || Types.Num() > 1024)
    {
        Error = TEXT("Expected authored types and /Game registry package");
        return false;
    }
    TArray<FDasTypeBinding> Bindings;
    TSet<FString> Ids;
    TSet<UObject*> Seen;
    TArray<UDasDefinedEnum*> Upgrade;
    for (auto* Type : Types)
    {
        if (!IsValid(Type) || !Type->GetPathName().StartsWith(TEXT("/Game/")) || Seen.Contains(Type))
        {
            Error = TEXT("Invalid or duplicate registry source type");
            return false;
        }
        Seen.Add(Type);
        FDasTypeBinding B;
        B.Id = Type->GetOutermost()->GetMetaData().GetValue(Type, TEXT("DasTypeId"));
        B.Type = Type;
        if (B.Id.IsEmpty() || Ids.Contains(B.Id))
        {
            Error = TEXT("Missing or duplicate stable ID in registry source");
            return false;
        }
        Ids.Add(B.Id);
        if (auto* E = Cast<UDasDefinedEnum>(Type))
        {
            B.bEnum = true;
            if (E->StableId.IsEmpty())
                Upgrade.Add(E);
            else if (E->StableId != B.Id)
            {
                Error = TEXT("Enum stable ID differs from authoring identity");
                return false;
            }
        }
        else if (auto* S = Cast<UUserDefinedStruct>(Type))
        {
            B.StructGuid = S->GetCustomGuid();
            if (!B.StructGuid.IsValid())
            {
                Error = TEXT("Missing authored struct GUID");
                return false;
            }
        }
        else
        {
            Error = TEXT("Registry requires authored enum/struct assets");
            return false;
        }
        Bindings.Add(B);
    }
    const FString Path = Package + TEXT(".")+ FPackageName::GetLongPackageAssetName(Package);
    UObject* Existing = LoadObject<UObject>(nullptr, *Path, nullptr, LOAD_NoWarn);
    auto* Registry = Cast<UDasTypeRegistry>(Existing);
    if ((Existing && !Registry) || (!Existing && FPackageName::DoesPackageExist(Package)))
    {
        Error = TEXT("Existing registry package conflict");
        return false;
    }
    if (Registry)
    {
        if (Registry->Bindings.Num() != Bindings.Num())
        {
            Error = TEXT("Existing registry identity changes require explicit migration");
            return false;
        }
        for (const auto& B : Bindings)
        {
            const auto* Old = Registry->Bindings.FindByPredicate([&](const auto& V) { return V.Id == B.Id; });
            if (!Old || Old->Type != B.Type || Old->bEnum != B.bEnum || Old->StructGuid != B.StructGuid)
            {
                Error = TEXT("Existing registry rebinding requires explicit migration");
                return false;
            }
        }
    }
    for (auto* E : Upgrade)
    {
        E->StableId = E->GetOutermost()->GetMetaData().GetValue(E, TEXT("DasTypeId"));
        E->MarkPackageDirty();
    }
    if (!Registry)
    {
        Registry = NewObject<UDasTypeRegistry>(CreatePackage(*Package), *FPackageName::GetLongPackageAssetName(Package),
                                               RF_Public | RF_Standalone);
        Registry->Bindings = MoveTemp(Bindings);
        Registry->MarkPackageDirty();
        FAssetRegistryModule::AssetCreated(Registry);
    }
    if (!Registry->Validate(Error))
        return false;
    Out = Registry;
    return true;
}
