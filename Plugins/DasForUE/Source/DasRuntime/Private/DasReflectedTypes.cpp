#include "UObject/Class.h"
#include "UObject/UObjectGlobals.h"
#include "DasReflectedTypes.h"
#include "DasTypeAdapters.h"
#include "DasSoftReferences.h"
#include "Dom/JsonObject.h"
#include "Misc/SecureHash.h"
#include "Serialization/JsonSerializer.h"
bool IsDasMathStructPath(const FString& Path)
{
    return Path == TEXT("/Script/CoreUObject.Vector2D") ||
                        Path == TEXT("/Script/CoreUObject.Vector") ||
                                     Path == TEXT("/Script/CoreUObject.Rotator") ||
                                                  Path == TEXT("/Script/CoreUObject.Transform");
}
namespace
{
bool TypeIdentifier(const FString& S)
{
    if (S.IsEmpty() || S.Len() >= NAME_SIZE || FChar::IsDigit(S[0]) || S.Equals(TEXT("None"), ESearchCase::IgnoreCase))
        return false;
    for (TCHAR C : S)
        if (!FChar::IsAlnum(C) && C != '_')
            return false;
    return true;
}
bool TypeStable(const FString& S)
{
    if (S.IsEmpty() || S.Len() > 256)
        return false;
    for (TCHAR C : S)
        if (!FChar::IsAlnum(C) && C != '_' && C != '.' && C != '-')
            return false;
    return true;
}
bool TypeKeys(const TSharedPtr<FJsonObject>& J, std::initializer_list<const TCHAR*> Keys)
{
    for (const auto& P : J->Values)
    {
        bool Found = false;
        for (const auto* K : Keys)
            Found |= FString(P.Key) == K;
        if (!Found)
            return false;
    }
    return true;
}
bool TypeMeta(const TMap<FString, FString>& M)
{
    for (const auto& P : M)
        if ((P.Key != TEXT("DisplayName") && P.Key != TEXT("ToolTip") && P.Key != TEXT("Category")) ||
                                                 P.Value.Len() > 4096)
            return false;
    return true;
}
}
bool FDasReflectedTypes::Validate(FString& Error, bool AllowUnresolved) const
{
    Error.Reset();
    auto Fail = [&](const TCHAR* E)
    {
        Error = E;
        return false;
    };
    if (!TypeStable(Id) || Enums.Num() + Structs.Num() == 0 || Enums.Num() + Structs.Num() > 256)
        return Fail(TEXT("Expected module ID and 1..256 reflected type declarations"));
    TSet<FString> IDs, Names;
    IDs.Add(Id);
    for (const auto& E : Enums)
    {
        if (!TypeStable(E.Id) || IDs.Contains(E.Id) || !TypeIdentifier(E.Name) || Names.Contains(E.Name.ToLower()) ||
            !TypeMeta(E.Metadata) || E.Entries.IsEmpty() || E.Entries.Num() > 255)
            return Fail(TEXT("Invalid enum declaration identity/metadata/count"));
        IDs.Add(E.Id);
        Names.Add(E.Name.ToLower());
        TSet<FString> Members;
        TSet<int64> Values;
        for (const auto& V : E.Entries)
        {
            if (!TypeIdentifier(V.Name) || V.Name.EndsWith(TEXT("_MAX"), ESearchCase::IgnoreCase) ||
                                                           Members.Contains(V.Name.ToLower()) ||
                                                           Values.Contains(V.Value) || V.Value < 0 || V.Value > 254)
                return Fail(TEXT("Enums require unique explicit byte values 0..254 and nonreserved names"));
            const FString MemberId = V.Id.IsEmpty() ? E.Id + TEXT(".")+ V.Name : V.Id;
            if (!TypeStable(MemberId) || IDs.Contains(MemberId))
                return Fail(TEXT("Invalid or duplicate enum member stable ID"));
            IDs.Add(MemberId);
            Members.Add(V.Name.ToLower());
            Values.Add(V.Value);
        }
    }
    for (const auto& S : Structs)
    {
        if (!TypeStable(S.Id) || IDs.Contains(S.Id) || !TypeIdentifier(S.Name) || Names.Contains(S.Name.ToLower()) ||
            !TypeMeta(S.Metadata) || S.Properties.IsEmpty() || S.Properties.Num() > 64 || !S.Methods.IsEmpty())
            return Fail(TEXT("Invalid struct identity/metadata/field count"));
        IDs.Add(S.Id);
        Names.Add(S.Name.ToLower());
        FDasAuthoringModule Carrier;
        Carrier.Id = Id;
        auto C = S;
        C.NativeParent = TEXT("/Script/Engine.Actor");
        Carrier.Classes.Add(C);
        if (!Carrier.Validate(Error))
            return false;
        for (const auto& P : S.Properties)
        {
            if (IDs.Contains(P.Id))
                return Fail(TEXT("Duplicate type/member stable ID"));
            IDs.Add(P.Id);
            if (P.bReadOnly || !P.bEditable)
                return Fail(TEXT("Initial user structs require editable read-write fields"));
            if (P.Type.Kind !=
                TEXT("byte") &&
                    P.Type.Kind !=
                        TEXT("text") &&
                            P.Type.Kind !=
                                TEXT("int") &&
                                    P.Type.Kind !=
                                        TEXT("float") &&
                                            P.Type.Kind !=
                                                TEXT("double") &&
                                                    P.Type.Kind !=
                                                        TEXT("bool") &&
                                                            P.Type.Kind !=
                                                                TEXT("string") &&
                                                                    P.Type.Kind !=
                                                                        TEXT("name") &&
                                                                            P.Type.Kind !=
                                                                                TEXT("object") &&
                                                                                    P.Type.Kind !=
                                                                                        TEXT("enum") &&
                                                                                            P.Type.Kind !=
                                                                                                TEXT("struct") &&
                                                                                                    P.Type.Kind !=
                                                                                                        TEXT("array") &&
                                                                                                            P.Type.Kind !=
                                                                                                                TEXT("map") &&
                                                                                                                    P.Type.Kind !=
                                                                                                                        TEXT("set"))
                return Fail(TEXT("Initial reflected structs support scalar/name fields only"));
        }
    }
    TMap<FString, const FDasDeclaredClass*> Refs;
    for (const auto& S : Structs)
        Refs.Add(TEXT("/DasTypes/")+ FMD5::HashAnsiString(*S.Id) + TEXT(".Type"), &S);
    TMap<FString, const FDasEnumDeclaration*> EnumRefs;
    for (const auto& E : Enums)
        EnumRefs.Add(TEXT("/DasTypes/")+ FMD5::HashAnsiString(*E.Id) + TEXT(".Type"), &E);
    TSet<FString> Active;
    TFunction<bool(const FDasDeclaredClass&, int32, int32&)> Visit;
    Visit = [&](const FDasDeclaredClass& S, int32 Depth, int32& Leaves)
    {
        if (Depth > 16 || Active.Contains(S.Id))
        {
            Error = TEXT("Recursive authored struct or nesting depth exceeded");
            return false;
        }
        Active.Add(S.Id);
        for (const auto& P : S.Properties)
        {
            if (P.Type.Kind == TEXT("enum"))
            {
                const auto* E = EnumRefs.Find(P.Type.TypePath);
                if (!E && !AllowUnresolved)
                {
                    Error = TEXT("Unresolved authored enum member type");
                    return false;
                }
                if (P.Default.Kind !=
                    TEXT("string") || (E && !(**E).Entries.ContainsByPredicate([&](const auto& V)
                                                                               { return V.Name == P.Default.String; })))
                {
                    Error = TEXT("Invalid authored enum member default");
                    return false;
                }
                if (++Leaves > 1024)
                {
                    Error = TEXT("Authored struct leaf limit exceeded");
                    return false;
                }
                continue;
            }
            if (P.Type.Kind == TEXT("set"))
            {
                if (!P.Type.Element ||
                    (P.Type.Element->Kind !=
                     TEXT("byte") && P.Type.Element->Kind != TEXT("int") && P.Type.Element->Kind != TEXT("string")) ||
                                         !P.Default.Kind.IsEmpty())
                {
                    Error = TEXT("Authored sets require int/string elements and empty defaults");
                    return false;
                }
                if (++Leaves > 1024)
                {
                    Error = TEXT("Authored struct leaf limit exceeded");
                    return false;
                }
                continue;
            }
            if (P.Type.Kind == TEXT("map"))
            {
                if (!P.Type.Key || !P.Type.Value ||
                    (P.Type.Key->Kind !=
                     TEXT("byte") &&
                         P.Type.Key->Kind != TEXT("int") && P.Type.Key->Kind != TEXT("string")) ||
                             (P.Type.Value->Kind !=
                              TEXT("byte") &&
                                  P.Type.Value->Kind !=
                                      TEXT("text") &&
                                          P.Type.Value->Kind !=
                                              TEXT("int") &&
                                                  P.Type.Value
                                                          ->Kind !=
                                                      TEXT("string") &&
                                                          P.Type.Value
                                                                  ->Kind !=
                                                              TEXT("float") &&
                                                                  P.Type.Value
                                                                          ->Kind !=
                                                                      TEXT("double") &&
                                                                          P.Type.Value
                                                                                  ->Kind !=
                                                                              TEXT("bool") &&
                                                                                  P.Type.Value
                                                                                          ->Kind !=
                                                                                      TEXT("name") &&
                                                                                          P.Type.Value
                                                                                                  ->Kind !=
                                                                                              TEXT("struct") &&
                                                                                                  P.Type.Value
                                                                                                          ->Kind !=
                                                                                                      TEXT("enum") &&
                                                                                                          P.Type.Value
                                                                                                                  ->Kind !=
                                                                                                              TEXT("object")) ||
                                                                                                      !P.Default.Kind
                                                                                                           .IsEmpty())
                {
                    Error = TEXT("Authored maps require int/string keys, scalar/name/struct/enum/reference values and empty defaults");
                    return false;
                }
                if (P.Type.Value->Kind == TEXT("enum") && !EnumRefs.Contains(P.Type.Value->TypePath) &&
                                               !AllowUnresolved)
                {
                    Error = TEXT("Unresolved authored enum map value");
                    return false;
                }
                if (P.Type.Value->Kind == TEXT("object"))
                {
                    auto* C = FindObject<UClass>(nullptr, *P.Type.Value->TypePath);
                    if (!P.Type.Value->AuthoredId.IsEmpty() || !P.Type.Value->TypePath.StartsWith(TEXT("/Script/")) ||
                                                                                                  !C)
                    {
                        Error = TEXT("Struct reference maps require loaded native types");
                        return false;
                    }
                }
                if (P.Type.Value->Kind == TEXT("struct") && !DasSoftProperty(P.Type.Value->TypePath) &&
                                               !IsDasMathStructPath(P.Type.Value->TypePath) &&
                                               !DasLeafValueAdapter(P.Type.Value->TypePath))
                {
                    const auto* Child = Refs.Find(P.Type.Value->TypePath);
                    if (!Child)
                    {
                        if (!AllowUnresolved)
                        {
                            Error = TEXT("Unresolved authored map struct value");
                            return false;
                        }
                    }
                    else if (!Visit(**Child, Depth + 1, Leaves))
                        return false;
                }
                if (++Leaves > 1024)
                    return false;
                continue;
            }
            if (P.Type.Kind == TEXT("struct") || P.Type.Kind == TEXT("array"))
            {
                const auto* Ref = &P.Type;
                if (P.Type.Kind == TEXT("array"))
                {
                    if (!P.Type.Element || !P.Default.Kind.IsEmpty())
                    {
                        Error = TEXT("Authored arrays require an element and empty defaults");
                        return false;
                    }
                    const auto& K = P.Type.Element->Kind;
                    if (K == TEXT("object"))
                    {
                        auto* C = FindObject<UClass>(nullptr, *P.Type.Element->TypePath);
                        if (!P.Type.Element->AuthoredId.IsEmpty() ||
                            !P.Type.Element->TypePath.StartsWith(TEXT("/Script/")) || !C)
                        {
                            Error = TEXT("Reference arrays require loaded native object/interface types");
                            return false;
                        }
                        if (++Leaves > 1024)
                            return false;
                        continue;
                    }
                    if (K == TEXT("enum"))
                    {
                        if (!EnumRefs.Contains(P.Type.Element->TypePath) && !AllowUnresolved)
                        {
                            Error = TEXT("Unresolved authored enum array element");
                            return false;
                        }
                        if (++Leaves > 1024)
                            return false;
                        continue;
                    }
                    if (K != TEXT("struct"))
                    {
                        if (K != TEXT("byte") &&
                                      K != TEXT("text") &&
                                                K != TEXT("int") &&
                                                          K != TEXT("float") &&
                                                                    K != TEXT("double") &&
                                                                              K != TEXT("bool") &&
                                                                                        K != TEXT("string") &&
                                                                                                  K != TEXT("name"))
                        {
                            Error = TEXT("Unsupported authored array element");
                            return false;
                        }
                        if (++Leaves > 1024)
                        {
                            Error = TEXT("Authored struct leaf limit exceeded");
                            return false;
                        }
                        continue;
                    }
                    Ref = P.Type.Element.Get();
                    if (DasSoftProperty(Ref->TypePath) || IsDasMathStructPath(Ref->TypePath) ||
                        DasLeafValueAdapter(Ref->TypePath))
                    {
                        if (++Leaves > 1024)
                        {
                            Error = TEXT("Authored struct leaf limit exceeded");
                            return false;
                        }
                        continue;
                    }
                }
                if (P.Type.Kind == TEXT("struct") && DasSoftProperty(Ref->TypePath))
                {
                    if (!P.Default.Kind.IsEmpty())
                    {
                        Error = TEXT("Authored soft fields require implicit null defaults");
                        return false;
                    }
                    if (++Leaves > 1024)
                        return false;
                    continue;
                }
                if (const auto* Leaf =
                        P.Type.GetKind() == EDasTypeKind::Struct ? DasLeafValueAdapter(Ref->TypePath) : nullptr)
                {
                    if (!P.Default.Kind.IsEmpty() && Leaf->GetDefaultPolicy() == EDasAdapterDefault::ImplicitEmpty)
                    {
                        Error = Leaf->GetStruct()->GetName() + TEXT(" struct fields use implicit empty defaults");
                        return false;
                    }
                    if (++Leaves > 1024)
                        return false;
                    continue;
                }
                if (P.Type.Kind == TEXT("struct") && IsDasMathStructPath(Ref->TypePath))
                {
                    if (P.Default.Kind != TEXT("string"))
                    {
                        Error = TEXT("Math struct members require literal defaults");
                        return false;
                    }
                    if (++Leaves > 1024)
                    {
                        Error = TEXT("Authored struct leaf limit exceeded");
                        return false;
                    }
                    continue;
                }
                if (!P.Default.Kind.IsEmpty())
                {
                    Error = TEXT("Nested authored fields use implicit child defaults");
                    return false;
                }
                const auto* Child = Refs.Find(Ref->TypePath);
                if (!Child)
                {
                    if (!AllowUnresolved)
                    {
                        Error = TEXT("Unresolved authored struct reference");
                        return false;
                    }
                    continue;
                }
                if (!Visit(**Child, Depth + 1, Leaves))
                    return false;
            }
            else if (++Leaves > 1024)
            {
                Error = TEXT("Authored struct leaf limit exceeded");
                return false;
            }
        }
        Active.Remove(S.Id);
        return true;
    };
    for (const auto& S : Structs)
    {
        int32 Leaves = 0;
        if (!Visit(S, 0, Leaves))
            return false;
    }
    return true;
}
bool FDasReflectedTypes::ToJson(FString& Out, FString& Error, bool AllowUnresolved) const
{
    if (!Validate(Error, AllowUnresolved))
        return false;
    auto Root = MakeShared<FJsonObject>();
    Root->SetNumberField(TEXT("schema_version"), 2);
    Root->SetStringField(TEXT("module_id"), Id);
    TArray<TSharedPtr<FJsonValue>> ES, SS;
    for (const auto& E : Enums)
    {
        auto J = MakeShared<FJsonObject>();
        J->SetStringField(TEXT("id"), E.Id);
        J->SetStringField(TEXT("name"), E.Name);
        auto Meta = MakeShared<FJsonObject>();
        TArray<FString> MetaKeys;
        E.Metadata.GetKeys(MetaKeys);
        MetaKeys.Sort();
        for (const auto& K : MetaKeys)
            Meta->SetStringField(K, E.Metadata[K]);
        J->SetObjectField(TEXT("metadata"), Meta);
        TArray<TSharedPtr<FJsonValue>> Entries;
        for (const auto& V : E.Entries)
        {
            auto X = MakeShared<FJsonObject>();
            X->SetStringField(TEXT("name"), V.Name);
            X->SetStringField(TEXT("id"), V.Id.IsEmpty() ? E.Id + TEXT(".")+ V.Name : V.Id);
            X->SetNumberField(TEXT("value"), V.Value);
            Entries.Add(MakeShared<FJsonValueObject>(X));
        }
        J->SetArrayField(TEXT("entries"), Entries);
        ES.Add(MakeShared<FJsonValueObject>(J));
    }
    for (const auto& S : Structs)
    {
        FDasAuthoringModule Carrier;
        Carrier.Id = Id;
        auto C = S;
        C.NativeParent = TEXT("/Script/Engine.Actor");
        Carrier.Classes.Add(C);
        FString Wire;
        if (!Carrier.ToJson(Wire, Error))
            return false;
        TSharedPtr<FJsonObject> Parsed;
        FJsonSerializer::Deserialize(TJsonReaderFactory<>::Create(Wire), Parsed);
        auto J = Parsed->GetArrayField(TEXT("classes"))[0]->AsObject();
        J->RemoveField(TEXT("native_parent"));
        J->RemoveField(TEXT("methods"));
        SS.Add(MakeShared<FJsonValueObject>(J));
    }
    Root->SetArrayField(TEXT("enums"), ES);
    Root->SetArrayField(TEXT("structs"), SS);
    FString Result;
    FJsonSerializer::Serialize(Root, TJsonWriterFactory<>::Create(&Result));
    Out = MoveTemp(Result);
    return true;
}
bool FDasReflectedTypes::FromJson(const FString& Wire, FDasReflectedTypes& Out, FString& Error, bool AllowUnresolved)
{
    Error.Reset();
    TSharedPtr<FJsonObject> J;
    if (Wire.Len() > 1024 * 1024 || !FJsonSerializer::Deserialize(TJsonReaderFactory<>::Create(Wire), J) || !J ||
        !J->HasTypedField<EJson::Number>(
            TEXT("schema_version")) ||
            (J->GetNumberField(TEXT("schema_version")) != 1 && J->GetNumberField(TEXT("schema_version")) != 2))
    {
        Error = TEXT("Invalid reflected types JSON/version");
        return false;
    }
    const bool Legacy = J->GetNumberField(TEXT("schema_version")) == 1;
    FDasReflectedTypes C;
    if (!TypeKeys(J, {TEXT("schema_version"), TEXT("module_id"), TEXT("enums"), TEXT("structs")}) ||
                           !J->TryGetStringField(TEXT("module_id"), C.Id))
    {
        Error = TEXT("Missing reflected module ID");
        return false;
    }
    const TArray<TSharedPtr<FJsonValue>>*ES = nullptr, *SS = nullptr;
    if (!J->TryGetArrayField(TEXT("enums"), ES) || !J->TryGetArrayField(TEXT("structs"), SS))
    {
        Error = TEXT("Missing enum/struct arrays");
        return false;
    }
    for (const auto& V : *ES)
    {
        const TSharedPtr<FJsonObject>* E = nullptr;
        if (!V->TryGetObject(E) || !TypeKeys(*E, {TEXT("id"), TEXT("name"), TEXT("metadata"), TEXT("entries")}))
        {
            Error = TEXT("Expected enum object");
            return false;
        }
        FDasEnumDeclaration D;
        if (!(*E)->TryGetStringField(TEXT("id"), D.Id) || !(*E)->TryGetStringField(TEXT("name"), D.Name))
        {
            Error = TEXT("Missing enum identity");
            return false;
        }
        const TSharedPtr<FJsonObject>* M = nullptr;
        if (!(*E)->TryGetObjectField(TEXT("metadata"), M))
        {
            Error = TEXT("Missing enum metadata");
            return false;
        }
        for (const auto& P : (*M)->Values)
        {
            FString S;
            if (!P.Value->TryGetString(S))
            {
                Error = TEXT("Expected string metadata");
                return false;
            }
            D.Metadata.Add(FString(P.Key), S);
        }
        const TArray<TSharedPtr<FJsonValue>>* Entries = nullptr;
        if (!(*E)->TryGetArrayField(TEXT("entries"), Entries))
        {
            Error = TEXT("Missing enum entries");
            return false;
        }
        for (const auto& X : *Entries)
        {
            const TSharedPtr<FJsonObject>* Entry = nullptr;
            FDasEnumEntry N;
            double Number;
            if (!X->TryGetObject(Entry) ||
                !(Legacy            ? TypeKeys(*Entry, {TEXT("name"), TEXT("value")})
                                    : TypeKeys(*Entry, {TEXT("id"), TEXT("name"), TEXT("value")})) ||
                                               !(*Entry)->TryGetStringField(
                                                   TEXT("name"), N.Name) ||
                                                   !(*Entry)->TryGetNumberField(TEXT("value"), Number) ||
                                                                                !FMath::IsFinite(Number) ||
                                                                                Number < 0 || Number > 254 ||
                                                                                FMath::FloorToDouble(Number) != Number)
            {
                Error = TEXT("Invalid explicit enum value");
                return false;
            }
            N.Value = int64(Number);
            if (Legacy)
                N.Id = D.Id + TEXT(".")+ N.Name;
            else if (!(*Entry)->TryGetStringField(TEXT("id"), N.Id))
            {
                Error = TEXT("Missing enum member ID");
                return false;
            }
            D.Entries.Add(N);
        }
        C.Enums.Add(D);
    }
    for (const auto& V : *SS)
    {
        const TSharedPtr<FJsonObject>* S = nullptr;
        if (!V->TryGetObject(S))
        {
            Error = TEXT("Expected struct object");
            return false;
        }
        auto Copy = MakeShared<FJsonObject>();
        Copy->Values = (*S)->Values;
        Copy->SetStringField(TEXT("native_parent"), TEXT("/Script/Engine.Actor"));
        Copy->SetArrayField(TEXT("methods"), {});
        auto Root = MakeShared<FJsonObject>();
        Root->SetNumberField(TEXT("schema_version"), 1);
        Root->SetStringField(TEXT("module_id"), C.Id);
        Root->SetArrayField(TEXT("classes"), {MakeShared<FJsonValueObject>(Copy)});
        FString Temp;
        FJsonSerializer::Serialize(Root, TJsonWriterFactory<>::Create(&Temp));
        FDasAuthoringModule Carrier;
        if (!FDasAuthoringModule::FromJson(Temp, Carrier, Error))
            return false;
        auto D = Carrier.Classes[0];
        D.NativeParent.Empty();
        C.Structs.Add(D);
    }
    if (!C.Validate(Error, AllowUnresolved))
        return false;
    Out = MoveTemp(C);
    return true;
}
