// Physical descriptors, not authored syntax. Paths carry the recipe so another
// process can reconstruct a transport without an out-of-band registry manifest.
const FString GenericContainerPrefix = TEXT("/Script/DasRuntime.DasContainerV1_");

TSharedPtr<FJsonValue> ContainerRecipe(const FDasDeclaredType& Type)
{
    TArray<TSharedPtr<FJsonValue>> Parts = {MakeShared<FJsonValueString>(Type.Kind),
                                            MakeShared<FJsonValueString>(Type.TypePath),
                                            MakeShared<FJsonValueNumber>(int32(DasResolvedEnumStorage(Type)))};
    if (const auto* Adapter = FDasTypeAdapters::Get().Find(Type))
        for (const auto& Slot : Adapter->GetContainerSlots())
            Parts.Add(ContainerRecipe(*(Type.*Slot.Descriptor)));
    return MakeShared<FJsonValueArray>(Parts);
}

FString GenericContainerPath(const FDasDeclaredType& Type)
{
    FString Json;
    FJsonSerializer::Serialize(ContainerRecipe(Type)->AsArray(),
                               TJsonWriterFactory<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>::Create(&Json));
    // Uppercase hex is intentional: FName comparison is case insensitive.
    return GenericContainerPrefix + ObjectHashEncode(Json);
}

UScriptStruct* GenericContainerCarrier(const FDasDeclaredType& Type)
{
    const auto* Adapter = FDasTypeAdapters::Get().Find(Type);
    if (!Adapter || Adapter->GetContainerSlots().IsEmpty())
        return nullptr;
    const FString Path = GenericContainerPath(Type);
    return Path.Len() < NAME_SIZE ? RegisteredCarrier(Path, Type) : nullptr;
}

bool ReadContainerRecipe(const TSharedPtr<FJsonValue>& Value, FDasDeclaredType& Out, int32 Depth = 0)
{
    const TArray<TSharedPtr<FJsonValue>>* Parts;
    double Storage;
    if (Depth > 16 || !Value || !Value->TryGetArray(Parts) || Parts->Num() < 3 ||
        !(*Parts)[0]->TryGetString(Out.Kind) || !(*Parts)[1]->TryGetString(Out.TypePath) ||
        !(*Parts)[2]->TryGetNumber(Storage) || Storage < 0 || Storage > int32(EDasEnumStorage::Enum8) ||
        Storage != int32(Storage))
        return false;
    Out.EnumStorage = EDasEnumStorage(int32(Storage));
    const auto* Adapter = FDasTypeAdapters::Get().FindKind(Out.GetKind());
    const auto Slots = Adapter ? Adapter->GetContainerSlots() : TConstArrayView<FDasContainerSlot>();
    if (Parts->Num() != 3 + Slots.Num())
        return false;
    int32 I = 3;
    for (const auto& Slot : Slots)
    {
        auto Child = MakeShared<FDasDeclaredType>();
        if (!ReadContainerRecipe((*Parts)[I++], *Child, Depth + 1))
            return false;
        Out.*Slot.Descriptor = Child;
    }
    const auto Kind = Out.GetKind();
    if (Kind == EDasTypeKind::Class || Kind == EDasTypeKind::Object || Kind == EDasTypeKind::Enum ||
        Kind == EDasTypeKind::Struct)
    {
        // Recipes may refer to native, project or plugin assets, never transient
        // definitions. Loading is limited to the expected reflected definition type.
        if (!FPackageName::IsValidObjectPath(Out.TypePath) ||
            Out.TypePath.StartsWith(TEXT("/Temp/")) || Out.TypePath.StartsWith(TEXT("/Engine/Transient.")))
            return false;
        if (Kind == EDasTypeKind::Class || Kind == EDasTypeKind::Object)
        {
            if (!FindObject<UClass>(nullptr, *Out.TypePath) &&
                !LoadObject<UClass>(nullptr, *Out.TypePath, nullptr, LOAD_NoWarn))
                return false;
        }
        else if (Kind == EDasTypeKind::Enum)
        {
            if (!FindObject<UEnum>(nullptr, *Out.TypePath) &&
                !LoadObject<UEnum>(nullptr, *Out.TypePath, nullptr, LOAD_NoWarn))
                return false;
        }
        else if (!FindObject<UScriptStruct>(nullptr, *Out.TypePath) &&
                 !LoadObject<UScriptStruct>(nullptr, *Out.TypePath, nullptr, LOAD_NoWarn))
            return false;
    }
    return true;
}

UScriptStruct* ResolveGenericContainerCarrier(const FString& Path)
{
    if (!IsInGameThread() || !Path.StartsWith(GenericContainerPrefix) || Path.Len() >= NAME_SIZE)
        return nullptr;
    FString Json, Error;
    if (!ObjectHashDecode(Path.RightChop(GenericContainerPrefix.Len()), Json))
        return nullptr;
    TArray<TSharedPtr<FJsonValue>> Parts;
    if (!FJsonSerializer::Deserialize(TJsonReaderFactory<>::Create(Json), Parts))
        return nullptr;
    FDasDeclaredType Type;
    if (!ReadContainerRecipe(MakeShared<FJsonValueArray>(Parts), Type) ||
        !ValidateDasPropertyType(Type, EDasPropertyUsage::Container, Error) || GenericContainerPath(Type) != Path)
        return nullptr;
    return GenericContainerCarrier(Type);
}
