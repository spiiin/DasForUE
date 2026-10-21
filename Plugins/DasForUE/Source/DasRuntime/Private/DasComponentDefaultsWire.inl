
static bool ValidateComponentDefaults(const FDasDeclaredComponent& C, FString& Error)
{
    if (C.Defaults.Num() > 128)
        return Fail(Error, TEXT("At most 128 component defaults are supported"));
    TSet<FString> Names;
    for (const auto& Pair : C.Defaults)
    {
        const auto& V = Pair.Value;
        if (!Identifier(Pair.Key) || Names.Contains(Pair.Key.ToLower()))
            return Fail(Error, TEXT("Invalid/duplicate default property"));
        Names.Add(Pair.Key.ToLower());
        if (V.Kind == TEXT("number"))
        {
            if (!FMath::IsFinite(V.Number))
                return Fail(Error, TEXT("Nonfinite default"));
        }
        else if (V.Kind == TEXT("string"))
        {
            if (V.String.Len() > 4096)
                return Fail(Error, TEXT("Default string too long"));
        }
        else if (const auto* Math = FDasTypeAdapters::Get().FindDefaultKind(V.Kind))
        {
            FInstancedStruct Value;
            const FString Path = Math->GetStruct()->GetPathName();
            if (!ParseDasStructDefault(Path, V.String, Value, Error))
                return false;
        }
        else if (V.Kind == TEXT("asset") || V.Kind == TEXT("class"))
        {
            if (V.String.IsEmpty() || V.String.Len() > 4096 || !V.String.StartsWith(TEXT("/")))
                return Fail(Error, TEXT("Invalid reference default path"));
        }
        else if (V.Kind == TEXT("struct"))
        {
            if (V.String.Len() > 4096)
                return Fail(Error, TEXT("Struct default text too long"));
        }
        else if (V.Kind != TEXT("bool"))
            return Fail(Error, TEXT("Unsupported component default kind"));
    }
    return true;
}
static bool ReadComponentDefaults(const TSharedPtr<FJsonObject>& J, TMap<FString, FDasDeclaredLiteral>& Out,
                                  FString& Error)
{
    if (!J->HasField(TEXT("defaults")))
        return true;
    TArray<TSharedPtr<FJsonObject>> Items;
    if (!Objects(J, TEXT("defaults"), Items, Error))
        return false;
    for (const auto& Item : Items)
    {
        FString Name;
        FDasDeclaredLiteral V;
        if (!Keys(Item, {TEXT("name"), TEXT("kind"), TEXT("value")}, Error) ||
                         !String(Item, TEXT("name"), Name, Error) || !String(Item, TEXT("kind"), V.Kind, Error) ||
                                                                             Out.Contains(Name))
            return Fail(Error, TEXT("Invalid/duplicate component default"));
        if (V.Kind == TEXT("number"))
        {
            if (!Item->TryGetNumberField(TEXT("value"), V.Number))
                return Fail(Error, TEXT("Expected numeric default"));
        }
        else if (V.Kind == TEXT("bool"))
        {
            if (!Item->TryGetBoolField(TEXT("value"), V.Boolean))
                return Fail(Error, TEXT("Expected bool default"));
        }
        else if (!String(Item, TEXT("value"), V.String, Error))
            return false;
        Out.Add(Name, V);
    }
    return true;
}
static void WriteComponentDefaults(const TMap<FString, FDasDeclaredLiteral>& Values, const TSharedPtr<FJsonObject>& J)
{
    if (Values.IsEmpty())
        return;
    TArray<FString> Names;
    Values.GetKeys(Names);
    Names.Sort();
    TArray<TSharedPtr<FJsonValue>> Items;
    for (const auto& Name : Names)
    {
        const auto& V = Values[Name];
        auto Item = MakeShared<FJsonObject>();
        Item->SetStringField(TEXT("name"), Name);
        Item->SetStringField(TEXT("kind"), V.Kind);
        if (V.Kind == TEXT("number"))
            Item->SetNumberField(TEXT("value"), V.Number);
        else if (V.Kind == TEXT("bool"))
            Item->SetBoolField(TEXT("value"), V.Boolean);
        else
            Item->SetStringField(TEXT("value"), V.String);
        Items.Add(MakeShared<FJsonValueObject>(Item));
    }
    J->SetArrayField(TEXT("defaults"), Items);
}
