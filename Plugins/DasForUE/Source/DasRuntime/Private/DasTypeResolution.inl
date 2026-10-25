// All source identity spellings normalize to canonical Unreal paths before IR publication.
enum class EDSLTypeKind
{
    Class,
    Interface,
    Enum,
    Struct
};
using FDSLTypeScope = FDasScriptTypeScope;
FString DSLTypeKindLabel(EDSLTypeKind Kind)
{
    return Kind == EDSLTypeKind::Class ? TEXT("class")
                                       : Kind == EDSLTypeKind::Interface
        ? TEXT("interface")
        : Kind == EDSLTypeKind::Enum
        ? TEXT("enum")
        : TEXT("struct");
}
void DSLCheckReference(const FString& Name)
{
    if (Name.IsEmpty() || Name != Name.TrimStartAndEnd() || Name.Contains(TEXT(",")))
        DSLFail(TEXT("Expected one nonempty type identity without surrounding whitespace"));
}
FString DSLCheckTypePath(EDSLTypeKind Kind, const FString& Path)
{
    if (!FPackageName::IsValidObjectPath(Path))
        DSLFail(TEXT("Invalid type object path: ")+ Path);
    if (Kind == EDSLTypeKind::Class || Kind == EDSLTypeKind::Interface)
    {
        auto* C = LoadObject<UClass>(nullptr, *Path, nullptr, LOAD_NoWarn);
        if (!C || C->HasAnyClassFlags(CLASS_Interface) != (Kind == EDSLTypeKind::Interface) ||
            (Kind == EDSLTypeKind::Class && !Path.StartsWith(TEXT("/Script/"))))
            DSLFail(TEXT("Expected ")+ DSLTypeKindLabel(Kind) + TEXT(" type: ")+ Path);
        return C->GetPathName();
    }
    if (Kind == EDSLTypeKind::Enum)
    {
        LoadObject<UEnum>(nullptr, *Path, nullptr, LOAD_NoWarn);
        FString Error;
        auto* E = ResolveDasEnum(Path, Error);
        if (!E)
            DSLFail(Error);
        return E->GetPathName();
    }
    LoadObject<UScriptStruct>(nullptr, *Path, nullptr, LOAD_NoWarn);
    FString Error;
    if (!ValidateDasValueStruct(Path, Error))
        DSLFail(Error);
    return FindObject<UScriptStruct>(nullptr, *Path)->GetPathName();
}
struct FDSLDeclaredReference
{
    FString Id, Name, NativePath;
};
FDSLDeclaredReference DSLFindDeclaredReference(const FString& Name, EDSLTypeKind Kind, const FDSLTypeScope& Scope)
{
    FDSLDeclaredReference Result;
    if (!Scope.Group || !Scope.Module)
        return Result;
    TMap<FString, FDSLDeclaredReference> Matches;
    auto Add = [&](const das::AnnotationList& Annotations, const das::string& TypeName, const das::LineInfo& At,
                   const char* AnnotationName)
    {
        const auto* A = DSLAnnotation(Annotations, AnnotationName);
        if (!A)
            return;
        FDSLDeclaredReference D;
        D.Name = DSLText(TypeName);
        if ((AnnotationName == das::string("ue_native_base") || AnnotationName == das::string("ue_native_object") ||
             AnnotationName == das::string("ue_native_interface") || AnnotationName == das::string("ue_interface_ref")))
            D.NativePath = DSLString(A->arguments, "path");
        else
        {
            const FString Module =
                DSLString(A->arguments, "module_id", FPaths::GetBaseFilename(DSLText(At.fileInfo->name)));
            D.Id = DSLString(A->arguments, "id", Module + TEXT(".")+ D.Name);
        }
        Matches.Add(D.NativePath.IsEmpty() ? D.Id : D.NativePath, D);
    };
    if (Kind == EDSLTypeKind::Enum)
    {
        if (auto E = Scope.Module->findEnum(DSLUtf8(Name)))
            Add(E->annotations, E->name, E->at, "ue_enum");
        for (const auto& E : Scope.Group->findEnum(DSLUtf8(Name), Scope.Module))
            Add(E->annotations, E->name, E->at, "ue_enum");
        auto EnumAliases = Scope.Group->findAlias(DSLUtf8(Name), Scope.Module);
        if (auto Alias = Scope.Module->findAlias(DSLUtf8(Name)))
            EnumAliases.push_back(Alias);
        for (const auto& Alias : EnumAliases)
            if (Alias->baseType == das::Type::tEnumeration && Alias->enumType)
                Add(Alias->enumType->annotations, Alias->enumType->name, Alias->enumType->at, "ue_enum");
    }
    else
    {
        auto Structures = Scope.Group->findStructure(DSLUtf8(Name), Scope.Module);
        if (auto Structure = Scope.Module->findStructure(DSLUtf8(Name)))
            Structures.push_back(Structure);
        auto Aliases = Scope.Group->findAlias(DSLUtf8(Name), Scope.Module);
        if (auto Alias = Scope.Module->findAlias(DSLUtf8(Name)))
            Aliases.push_back(Alias);
        for (const auto& Alias : Aliases)
            if (Alias->baseType == das::Type::tStructure && Alias->structType)
                Structures.push_back(Alias->structType);
        for (const auto& S : Structures)
        {
            if (Kind == EDSLTypeKind::Interface)
            {
                Add(S->annotations, S->name, S->at, "ue_native_interface");
                Add(S->annotations, S->name, S->at, "ue_interface_ref");
            }
            if (Kind == EDSLTypeKind::Class)
            {
                Add(S->annotations, S->name, S->at, "ue_native_object");
                Add(S->annotations, S->name, S->at, "ue_native_base");
            }
            else
                Add(S->annotations, S->name, S->at, Kind == EDSLTypeKind::Interface ? "ue_interface" : "ue_struct");
        }
    }
    if (Matches.Num() > 1)
        DSLFail(TEXT("Ambiguous declared ")+ DSLTypeKindLabel(Kind) + TEXT(" identity: ")+ Name);
    if (Matches.Num() == 1)
        Result = Matches.CreateConstIterator().Value();
    return Result;
}
FString DSLFindNativeReference(const FString& Name, EDSLTypeKind Kind)
{
    TSet<FString> Paths;
    auto Matches = [&](UObject* Object)
    {
        const FString Path = Object->GetPathName();
        return Path.StartsWith(TEXT("/Script/")) && (Object->GetName() == Name || Path == TEXT("/Script/")+ Name);
    };
    if (Kind == EDSLTypeKind::Class || Kind == EDSLTypeKind::Interface)
    {
        for (TObjectIterator<UClass> It; It; ++It)
            if (It->HasAnyClassFlags(CLASS_Interface) == (Kind == EDSLTypeKind::Interface) && Matches(*It))
                Paths.Add(It->GetPathName());
    }
    else if (Kind == EDSLTypeKind::Enum)
    {
        for (TObjectIterator<UEnum> It; It; ++It)
            if (Matches(*It))
                Paths.Add(It->GetPathName());
    }
    else
    {
        for (TObjectIterator<UScriptStruct> It; It; ++It)
            if (Matches(*It))
                Paths.Add(It->GetPathName());
    }
    if (Paths.Num() > 1)
        DSLFail(TEXT("Ambiguous native ")+ DSLTypeKindLabel(Kind) + TEXT(" identity; use a full path: ")+ Name);
    return Paths.Num() == 1 ? Paths.Array()[0] : FString();
}
FString DSLFindAuthoredInterface(const FString& Identity)
{
    TSet<FString> Candidates;
    for (TObjectIterator<UDasAuthoredInterfaceClass> It; It; ++It)
    {
        auto* C = *It;
        if (C->StableId == Identity && C->HasAnyClassFlags(CLASS_Interface) &&
            !C->HasAnyClassFlags(CLASS_NewerVersionExists) && !C->HasAnyFlags(RF_Transient) &&
            C->GetOutermost() != GetTransientPackage() &&
            !C->GetName().StartsWith(TEXT("SKEL_")) && !C->GetName().StartsWith(TEXT("REINST_")))
            Candidates.Add(C->GetPathName());
    }
#if WITH_EDITOR
    auto& Assets = FModuleManager::LoadModuleChecked<FAssetRegistryModule>(TEXT("AssetRegistry")).Get();
    Assets.WaitForCompletion();
    TArray<FAssetData> Entries;
    Assets.GetAssetsByClass(UDasAuthoredInterfaceBlueprint::StaticClass()->GetClassPathName(), Entries, true);
    for (const auto& Entry : Entries)
    {
        FString Id;
        if (Entry.GetTagValue(TEXT("StableId"), Id) && Id == Identity)
            Candidates.Add(Entry.PackageName.ToString() + TEXT(".")+ Entry.AssetName.ToString() + TEXT("_C"));
    }
#endif
    if (Candidates.Num() > 1)
        DSLFail(TEXT("Ambiguous authored interface; use a full Unreal path: ")+ Identity);
    if (Candidates.IsEmpty())
        return {};
    const FString Path = Candidates.Array()[0];
    auto* C = LoadObject<UDasAuthoredInterfaceClass>(nullptr, *Path, nullptr, LOAD_NoWarn);
    if (!C || C->StableId != Identity)
        DSLFail(TEXT("Authored interface identity mismatch: ")+ Identity);
    return DSLCheckTypePath(EDSLTypeKind::Interface, Path);
}
FString DSLResolveType(EDSLTypeKind Kind, const FString& Name, const FDSLTypeScope& Scope)
{
    DSLCheckReference(Name);
    if (Name.StartsWith(TEXT("/")))
        return DSLCheckTypePath(Kind, Name);
    const auto Declared = DSLFindDeclaredReference(Name, Kind, Scope);
    if (!Declared.NativePath.IsEmpty())
        return DSLCheckTypePath(Kind, Declared.NativePath);
    const bool Qualified = Name.Contains(TEXT("."));
    const FString Identity = !Declared.Id.IsEmpty() ? Declared.Id : Qualified ? Name : Scope.ModuleId + TEXT(".")+ Name;
    if ((Kind == EDSLTypeKind::Enum || Kind == EDSLTypeKind::Struct) && !Scope.Registry.IsEmpty())
    {
        auto* Registry = LoadObject<UDasTypeRegistry>(nullptr, *Scope.Registry, nullptr, LOAD_NoWarn);
        FString Error;
        if (!Registry || !Registry->Validate(Error))
            DSLFail(Error.IsEmpty() ? TEXT("Type registry could not load: ")+ Scope.Registry : Error);
        TSet<FString> IDs;
        bool Exact = false;
        for (const auto& B : Registry->Bindings)
            if (B.Id == Identity)
            {
                IDs.Add(B.Id);
                Exact = true;
            }
        if (!Exact && Declared.Id.IsEmpty() && !Qualified)
            for (const auto& B : Registry->Bindings)
            {
                FString Leaf;
                B.Id.Split(TEXT("."), nullptr, &Leaf, ESearchCase::CaseSensitive, ESearchDir::FromEnd);
                if (Leaf == Name || B.Type->GetName() == Name)
                    IDs.Add(B.Id);
            }
        if (IDs.IsEmpty() && Declared.Id.IsEmpty())
        {
            const FString Native = DSLFindNativeReference(Name, Kind);
            if (!Native.IsEmpty())
                return DSLCheckTypePath(Kind, Native);
        }
        if (IDs.Num() != 1)
            DSLFail((IDs.IsEmpty() ? TEXT("Stable type ID is not registered: ")
                                   : TEXT("Ambiguous registered type identity: ")) +
                     Identity);
        auto* Type = Registry->Resolve(IDs.Array()[0], Kind == EDSLTypeKind::Enum, Error);
        if (!Type)
            DSLFail(Error);
        return DSLCheckTypePath(Kind, Type->GetPathName());
    }
    if ((Kind == EDSLTypeKind::Enum || Kind == EDSLTypeKind::Struct) &&
        (!Declared.Id.IsEmpty() || !Scope.Folder.IsEmpty()))
    {
        if (Declared.Id.IsEmpty())
        {
            // Preserve an explicitly selected folder's existing asset identity/kind.
            if (Scope.Folder.StartsWith(TEXT("/Game/")) && !Name.Contains(TEXT(".")))
            {
                const FString Candidate = Scope.Folder + TEXT("/")+ Name + TEXT(".")+ Name;
                if (LoadObject<UObject>(nullptr, *Candidate, nullptr, LOAD_NoWarn))
                    return DSLCheckTypePath(Kind, Candidate);
            }
            const FString Native = DSLFindNativeReference(Name, Kind);
            if (!Native.IsEmpty())
                return DSLCheckTypePath(Kind, Native);
        }
        if (!Scope.Folder.StartsWith(TEXT("/Game/")))
            DSLFail(TEXT("Direct authored types require type_registry or type_folder"));
        const FString Leaf = Declared.Name.IsEmpty() ? Name : Declared.Name;
        if (Leaf.Contains(TEXT(".")))
            DSLFail(TEXT("Stable type ID requires an explicit type registry: ")+ Identity);
        return DSLCheckTypePath(Kind, Scope.Folder + TEXT("/")+ Leaf + TEXT(".")+ Leaf);
    }
    if (Kind == EDSLTypeKind::Interface)
    {
        if (!Scope.Folder.IsEmpty())
        {
            const FString Leaf = Declared.Name.IsEmpty() ? Name : Declared.Name;
            if (!Leaf.Contains(TEXT(".")))
            {
                const FString Path = Scope.Folder + TEXT("/")+ Leaf + TEXT(".")+ Leaf + TEXT("_C");
                if (auto* C = LoadObject<UDasAuthoredInterfaceClass>(nullptr, *Path, nullptr, LOAD_NoWarn))
                {
                    if (C->StableId != Identity)
                        DSLFail(TEXT("Authored interface identity mismatch: ")+ Identity);
                    return DSLCheckTypePath(Kind, Path);
                }
            }
            if (Declared.Id.IsEmpty())
            {
                const FString Native = DSLFindNativeReference(Name, Kind);
                if (!Native.IsEmpty())
                    return DSLCheckTypePath(Kind, Native);
            }
            DSLFail(TEXT("Unknown authored interface in type_folder: ")+ Identity);
        }
        const FString Authored = DSLFindAuthoredInterface(Identity);
        if (!Authored.IsEmpty())
            return Authored;
        if (!Declared.Id.IsEmpty())
            DSLFail(TEXT("Unknown authored interface: ")+ Identity);
    }
    const FString Native = DSLFindNativeReference(Name, Kind);
    if (!Native.IsEmpty())
        return DSLCheckTypePath(Kind, Native);
    DSLFail((Kind == EDSLTypeKind::Interface ? TEXT("Unknown authored interface: ")
                                             : TEXT("Unknown ")+ DSLTypeKindLabel(Kind) + TEXT(" type identity: ")) +
             Identity);
}
FString DSLConstraint(TMap<FString, FString>& Options, const FString& Key, EDSLTypeKind Kind, const FString& Inferred,
                      const FDSLTypeScope& Scope)
{
    FString Explicit;
    if (const auto* Value = Options.Find(Key))
        Explicit = DSLResolveType(Kind, *Value, Scope);
    Options.Remove(Key);
    if (!Inferred.IsEmpty() && !Explicit.IsEmpty() && Inferred != Explicit)
        DSLFail(TEXT("Conflicting type constraint: ")+ Key);
    return Inferred.IsEmpty() ? Explicit : Inferred;
}
