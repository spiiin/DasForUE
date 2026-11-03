// Read declaration headers without importing or compiling classes that themselves use the catalog.
bool FDasSourceCatalog::DeclarationInventory(const TMap<FString, FString>& Sources, FDasSourceCatalog& Out,
                                             FString& Error)
{
    FDasSourceCatalog Candidate;
    struct FToken
    {
        FString Text;
        bool String = false;
    };
    for (const auto& Source : Sources)
    {
        if (!LocalFile(Source.Key, TEXT("das")))
        {
            Error = TEXT("Invalid catalog source filename: ") + Source.Key;
            return false;
        }
        TArray<FToken> Tokens;
        const auto& S = Source.Value;
        for (int32 I = 0; I < S.Len();)
        {
            if (FChar::IsWhitespace(S[I]))
            {
                ++I;
                continue;
            }
            if (S.Mid(I, 2) == TEXT("//"))
            {
                while (I < S.Len() && S[I] != '\n')
                    ++I;
                continue;
            }
            if (S.Mid(I, 2) == TEXT("/*"))
            {
                I += 2;
                int32 Nest = 1;
                while (I < S.Len() && Nest)
                {
                    if (S.Mid(I, 2) == TEXT("/*"))
                    {
                        ++Nest;
                        I += 2;
                    }
                    else if (S.Mid(I, 2) == TEXT("*/"))
                    {
                        --Nest;
                        I += 2;
                    }
                    else
                        ++I;
                }
                if (Nest)
                {
                    Error = TEXT("Unclosed catalog source comment: ") + Source.Key;
                    return false;
                }
                continue;
            }
            if (S[I] == '"' || S[I] == '\'')
            {
                const TCHAR Quote = S[I++];
                FString Value;
                bool Closed = false;
                while (I < S.Len())
                {
                    TCHAR C = S[I++];
                    if (C == Quote)
                    {
                        Closed = true;
                        break;
                    }
                    if (C == '\\' && I < S.Len())
                        C = S[I++];
                    Value.AppendChar(C);
                }
                if (!Closed)
                {
                    Error = TEXT("Unclosed catalog source string: ") + Source.Key;
                    return false;
                }
                Tokens.Add({Value, true});
                continue;
            }
            const int32 Start = I++;
            if (FChar::IsAlpha(S[Start]) || S[Start] == '_')
                while (I < S.Len() && (FChar::IsAlnum(S[I]) || S[I] == '_'))
                    ++I;
            Tokens.Add({S.Mid(Start, I - Start), false});
        }
        int32 Depth = 0;
        FString Pending;
        FString Module, Id;
        FString SourceModule = FPaths::GetBaseFilename(Source.Key);
        for (int32 I = 0; I < Tokens.Num(); ++I)
        {
            const auto& T = Tokens[I];
            if (T.String)
                continue;
            if (T.Text == TEXT("{"))
            {
                ++Depth;
                continue;
            }
            if (T.Text == TEXT("}"))
            {
                --Depth;
                continue;
            }
            if (Depth != 0)
                continue;
            if (T.Text == TEXT("module") && I + 1 < Tokens.Num() && !Tokens[I + 1].String)
                SourceModule = Tokens[I + 1].Text;
            if (T.Text == TEXT("["))
            {
                int32 End = I + 1, Brackets = 1;
                for (; End < Tokens.Num() && Brackets; ++End)
                    if (!Tokens[End].String)
                    {
                        if (Tokens[End].Text == TEXT("["))
                            ++Brackets;
                        else if (Tokens[End].Text == TEXT("]"))
                            --Brackets;
                    }
                if (Brackets)
                {
                    Error = TEXT("Unclosed catalog annotation: ") + Source.Key;
                    return false;
                }
                for (int32 A = I + 1; A + 1 < End; ++A)
                {
                    if (Tokens[A].String)
                        continue;
                    const auto& Marker = Tokens[A].Text;
                    if (Marker != TEXT("ue_class") && Marker != TEXT("ue_interface") &&
                                                                     Marker != TEXT("ue_struct") &&
                                                                                    Marker != TEXT("ue_enum"))
                        continue;
                    if (!Pending.IsEmpty())
                    {
                        Error = TEXT("Conflicting catalog type annotations: ") + Source.Key;
                        return false;
                    }
                    Pending = Marker == TEXT("ue_class")
                              ? TEXT("class")
                              : Marker == TEXT("ue_interface")
                                    ? TEXT("interface")
                                    : Marker == TEXT("ue_struct") ? TEXT("struct") : TEXT("enum");
                    Module = FPaths::GetBaseFilename(Source.Key);
                    Id.Reset();
                    if (Tokens[A + 1].Text != TEXT("("))
                        break;
                    TSet<FString> IdentityKeys;
                    int32 Parens = 1;
                    for (int32 K = A + 2; K + 1 < End && Parens; ++K)
                    {
                        if (Tokens[K].String)
                            continue;
                        if (Tokens[K].Text == TEXT("("))
                            ++Parens;
                        if (Tokens[K].Text == TEXT(")"))
                            --Parens;
                        if (Parens != 1 || (Tokens[K].Text != TEXT("module_id") && Tokens[K].Text != TEXT("id")))
                            continue;
                        if (K + 2 >= End || Tokens[K + 1].Text != TEXT("=") || !Tokens[K + 2].String)
                        {
                            Error = TEXT("Catalog type identity requires string literals: ") + Source.Key;
                            return false;
                        }
                        if (Tokens[K + 2].Text.IsEmpty() || IdentityKeys.Contains(Tokens[K].Text))
                        {
                            Error = TEXT("Empty/repeated catalog identity: ") + Source.Key;
                            return false;
                        }
                        IdentityKeys.Add(Tokens[K].Text);
                        (Tokens[K].Text == TEXT("id") ? Id : Module) = Tokens[K + 2].Text;
                    }
                    break;
                }
                I = End - 1;
                continue;
            }
            if (!Pending.IsEmpty() && I + 1 < Tokens.Num() && !Tokens[I + 1].String &&
                T.Text == ((Pending == TEXT("class") || Pending == TEXT("interface")) ? TEXT("class") : Pending))
            {
                const FString Name = Tokens[++I].Text;
                const FString StableId = Id.IsEmpty() ? Module + TEXT(".") + Name : Id;
                if (Pending == TEXT("class"))
                    Candidate.Entries.Add({StableId, Name, Source.Key, Name + TEXT("Ref")});
                else
                {
                    auto* Values =
                        Candidate.ValueModules.FindByPredicate([&](const auto& M) { return M.Script == Source.Key; });
                    if (!Values)
                    {
                        Candidate.ValueModules.Add({SourceModule, Source.Key, {}});
                        Values = &Candidate.ValueModules.Last();
                    }
                    Values->Types.Add({Pending, StableId, Name});
                }
                Pending.Reset();
            }
        }
    }
    if (Candidate.Entries.IsEmpty() && Candidate.ValueModules.IsEmpty())
    {
        Error = TEXT("No UE type declarations found for automatic source catalog");
        return false;
    }
    TMap<FString, int32> Counts;
    for (const auto& Entry : Candidate.Entries)
        ++Counts.FindOrAdd(Entry.Reference.ToLower());
    for (auto& Entry : Candidate.Entries)
        if (Counts[Entry.Reference.ToLower()] > 1)
            Entry.Reference += TEXT("_") + FMD5::HashAnsiString(*Entry.Id).Left(8);
    Candidate.Entries.Sort([](const auto& A, const auto& B) { return A.Id < B.Id; });
    Candidate.ValueModules.Sort([](const auto& A, const auto& B) { return A.Module < B.Module; });
    if (!Candidate.Validate(Error))
        return false;
    Out = MoveTemp(Candidate);
    return true;
}

bool FDasSourceCatalog::UpdateDeclarationInventory(const FString& ScriptsFolder, FString& Error)
{
    TArray<FString> Files;
    IFileManager::Get().FindFilesRecursive(Files, *ScriptsFolder, TEXT("*.das"), true, false);
    const FString Base = FPaths::ConvertRelativePathToFull(ScriptsFolder) + TEXT("/");
    TMap<FString, FString> Sources;
    for (const auto& Absolute : Files)
    {
        FString File = FPaths::ConvertRelativePathToFull(Absolute);
        FPaths::MakePathRelativeTo(File, *Base);
        FString Source;
        if (!FFileHelper::LoadFileToString(Source, *(ScriptsFolder / File)))
        {
            Error = TEXT("Cannot read catalog source: ") + File;
            return false;
        }
        Sources.Add(File, MoveTemp(Source));
    }
    FDasSourceCatalog Catalog;
    FString Json;
    if (!DeclarationInventory(Sources, Catalog, Error) || !Catalog.ToJson(Json, Error))
        return false;
    const FString Path = ScriptsFolder / TEXT("ue_type_catalog.json");
    FString Existing;
    if (FFileHelper::LoadFileToString(Existing, *Path) && Existing == Json)
        return true;
    const FString Temporary = Path + TEXT(".tmp");
    if (!FFileHelper::SaveStringToFile(Json, *Temporary, FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM) ||
        !IFileManager::Get().Move(*Path, *Temporary, true, true))
    {
        Error = TEXT("Cannot update automatic source catalog: ") + Path;
        return false;
    }
    UE_LOG(LogTemp, Display, TEXT("DAS_SOURCE_INVENTORY classes=%d path=%s"), Catalog.Entries.Num(), *Path);
    return true;
}
