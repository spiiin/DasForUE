// Project configuration is sampled when ue_foundation is initialized. IDs never depend on list order.
TMap<int64, FName> CollisionProfileIds;
TMap<FString, FName> CollisionProfileSymbols;
FString CollisionProfileLabel(const FString& Name)
{
    FString Label;
    for (TCHAR C : Name)
        Label.AppendChar(
            (C >= 'a' && C <= 'z') || (C >= 'A' && C <= 'Z') || (C >= '0' && C <= '9') || C == '_' ? C : TCHAR('_'));
    const auto Utf8 = DSLUtf8(Label);
    if (Label.IsEmpty() || FChar::IsDigit(Label[0]) || das::isDasKeyword(Utf8.c_str()))
        Label = TEXT("Profile_")+ Label;
    return Label;
}
void AddCollisionProfileEnum(das::Module& M)
{
    TArray<FName> Names;
    auto* Profiles = UCollisionProfile::Get();
    for (int32 I = 0; I < Profiles->GetNumOfProfiles(); ++I)
        if (const auto* P = Profiles->GetProfileByIndex(I))
            Names.AddUnique(P->Name);
    Names.Sort([](FName A, FName B) { return A.LexicalLess(B); });
    auto* Enum = new das::Enumeration("CollisionProfile");
    Enum->baseType = das::Type::tInt64;
    TMap<FString, int32> Labels;
    for (FName Name : Names)
        Labels.FindOrAdd(CollisionProfileLabel(Name.ToString()))++;
    TSet<FString> RawLabels;
    for (FName Name : Names)
        if (CollisionProfileLabel(Name.ToString()) == Name.ToString())
            RawLabels.Add(Name.ToString());
    CollisionProfileIds.Reset();
    CollisionProfileSymbols.Reset();
    for (FName Name : Names)
    {
        const auto Utf8 = DSLUtf8(Name.ToString().ToLower());
        const FString Hash = FMD5::HashBytes(reinterpret_cast<const uint8*>(Utf8.data()), Utf8.size());
        const int64 Id = int64(FCString::Strtoui64(*Hash.Left(16), nullptr, 16));
        if (CollisionProfileIds.Contains(Id))
            DSLFail(TEXT("Collision profile identity hash collision"));
        FString Label = CollisionProfileLabel(Name.ToString());
        if (Labels[Label] > 1 || Label != Name.ToString())
        {
            Label += TEXT("_")+ Hash;
            while (RawLabels.Contains(Label) || CollisionProfileSymbols.Contains(Label))
                Label += TEXT("_");
        }
        if (!Enum->addI(DSLUtf8(Label), Id, das::LineInfo()))
            DSLFail(TEXT("Duplicate collision profile label"));
        CollisionProfileIds.Add(Id, Name);
        CollisionProfileSymbols.Add(Label, Name);
    }
    M.addEnumeration(Enum);
}
FDasNameValue CollisionProfileName(int64 Id, das::Context* C)
{
    const auto* Name = CollisionProfileIds.Find(Id);
    FCollisionResponseTemplate Profile;
    if (!Name || !UCollisionProfile::Get()->GetProfileTemplate(*Name, Profile))
        C->throw_error("Collision profile is unknown or was removed from project configuration");
    return MakeDSLName(*Name, C);
}
