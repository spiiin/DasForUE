#include "DasGASAuthoring.h"
#include "Dom/JsonObject.h"
#include "Serialization/JsonSerializer.h"
namespace
{
bool GASName(const FString& Name)
{
    if (Name.IsEmpty() || Name.Len() > 128 || Name.Equals(TEXT("None"), ESearchCase::IgnoreCase))
        return false;
    for (int32 I = 0; I < Name.Len(); ++I)
        if (!(Name[I] == '_' || (Name[I] >= 'a' && Name[I] <= 'z') || (Name[I] >= 'A' && Name[I] <= 'Z') ||
              (I > 0 && Name[I] >= '0' && Name[I] <= '9')))
            return false;
    return true;
}
bool GASNumber(const TSharedPtr<FJsonObject>& Object, const TCHAR* Field, float& Out)
{
    double Value = 0;
    if (!Object->TryGetNumberField(Field, Value) || !FMath::IsFinite(Value) || FMath::Abs(Value) > MAX_flt)
        return false;
    Out = float(Value);
    return true;
}
}
bool ParseDasGASDeclarations(const FString& Json, FDasGASDeclarations& Out, FString& Error)
{
    Out = {};
    Error.Reset();
    auto Fail = [&](const TCHAR* Message)
    {
        Error = Message;
        return false;
    };
    TSharedPtr<FJsonObject> Root;
    const TArray<TSharedPtr<FJsonValue>>* Items = nullptr;
    if (Json.Len() > 262144 || !FJsonSerializer::Deserialize(TJsonReaderFactory<>::Create(Json), Root) || !Root ||
        !Root->TryGetArrayField(TEXT("declarations"), Items) || Items->IsEmpty() || Items->Num() > 32)
        return Fail(TEXT("Expected 1..32 GAS declarations"));
    TSet<FString> Names, Ids;
    FDasGASDeclarations Candidate;
    for (const auto& Item : *Items)
    {
        const auto Object = Item->AsObject();
        FString Kind, Name, Id;
        const TArray<TSharedPtr<FJsonValue>>* Fields = nullptr;
        if (!Object ||
            !Object->TryGetStringField(
                TEXT("kind"), Kind) ||
                !Object->TryGetStringField(
                    TEXT("name"), Name) || !GASName(Name) || Names.Contains(Name.ToLower()) ||
                    !Object->TryGetStringField(TEXT("id"), Id) || Id.IsEmpty() || Id.Len() > 256 || Ids.Contains(Id) ||
                                               !Object->TryGetArrayField(TEXT("fields"), Fields) || Fields->IsEmpty() ||
                                                                         Fields->Num() > 64)
            return Fail(TEXT("Invalid/duplicate GAS declaration or field count"));
        Names.Add(Name.ToLower());
        Ids.Add(Id);
        TSet<FName> FieldNames;
        FDasGASSetDeclaration Set;
        Set.Name = Name;
        Set.Id = Id;
        FDasGASEffectDeclaration Effect;
        Effect.Name = Name;
        Effect.Id = Id;
        if (Kind == TEXT("effect") && (!Object->TryGetStringField(TEXT("attributes"), Effect.Attributes) ||
                                                                  Effect.Attributes.IsEmpty() || Fields->Num() > 32))
            return Fail(TEXT("Effect requires an explicit attribute-set name and 1..32 modifiers"));
        if (Kind != TEXT("attributes") && Kind != TEXT("effect"))
            return Fail(TEXT("Unknown GAS declaration kind"));
        for (const auto& Field : *Fields)
        {
            const auto F = Field->AsObject();
            FString N;
            float Initial = 0;
            if (!F || !F->TryGetStringField(TEXT("name"), N) || !GASName(N) || FieldNames.Contains(FName(*N)) ||
                                            !GASNumber(F, TEXT("value"), Initial))
                return Fail(TEXT("GAS fields require unique names and finite float values"));
            FieldNames.Add(FName(*N));
            if (Kind == TEXT("attributes"))
            {
                FDasGASAttributeRule Rule;
                Rule.Name = FName(*N);
                Rule.Initial = Initial;
                Rule.HasMin = F->HasField(TEXT("min"));
                Rule.HasMax = F->HasField(TEXT("max"));
                if ((Rule.HasMin && !GASNumber(F, TEXT("min"), Rule.Minimum)) ||
                     (Rule.HasMax && !GASNumber(F, TEXT("max"), Rule.Maximum)))
                    return Fail(TEXT("Attribute limits must be finite floats"));
                FString Linked;
                if (F->TryGetStringField(TEXT("max_attribute"), Linked))
                {
                    if (!GASName(Linked) || Rule.HasMax)
                        return Fail(TEXT("Conflicting maximum rules"));
                    Rule.MaximumAttribute = FName(*Linked);
                }
                if ((Rule.HasMin && Initial < Rule.Minimum) || (Rule.HasMax && Initial > Rule.Maximum) ||
                    (Rule.HasMin && Rule.HasMax && Rule.Minimum > Rule.Maximum))
                    return Fail(TEXT("Attribute initial value is outside its limits"));
                Set.Attributes.Add(Rule);
            }
            else
            {
                FDasGASModifierDeclaration Mod;
                Mod.Attribute = FName(*N);
                Mod.Magnitude = Initial;
                F->TryGetStringField(TEXT("operation"), Mod.Operation);
                if (Mod.Operation != TEXT("Add") && Mod.Operation != TEXT("Multiply") &&
                                                                          Mod.Operation != TEXT("Override"))
                    return Fail(TEXT("Supported modifier operations: Add, Multiply, Override"));
                FString Tag;
                if (F->TryGetStringField(TEXT("set_by_caller"), Tag))
                {
                    if (Tag.IsEmpty() || !FGameplayTag::IsValidGameplayTagString(Tag))
                        return Fail(TEXT("Invalid SetByCaller tag"));
                    Mod.SetByCaller = FName(*Tag);
                }
                Effect.Modifiers.Add(Mod);
            }
        }
        if (Kind == TEXT("attributes"))
            Candidate.Sets.Add(MoveTemp(Set));
        else
            Candidate.Effects.Add(MoveTemp(Effect));
    }
    for (const auto& Set : Candidate.Sets)
        for (const auto& Rule : Set.Attributes)
        {
            TSet<FName> Visited;
            const auto* Current = &Rule;
            while (!Current->MaximumAttribute.IsNone())
            {
                if (Visited.Contains(Current->Name))
                    return Fail(TEXT("Cyclic linked attribute limits"));
                Visited.Add(Current->Name);
                const auto* Maximum =
                    Set.Attributes.FindByPredicate([&](const auto& A) { return A.Name == Current->MaximumAttribute; });
                if (!Maximum || Current->Initial > Maximum->Initial ||
                    (Current->HasMin && (!Maximum->HasMin || Maximum->Minimum < Current->Minimum)))
                    return Fail(TEXT("Missing/incompatible linked maximum attribute"));
                Current = Maximum;
            }
        }
    for (auto& Effect : Candidate.Effects)
    {
        const auto* Set = Candidate.Sets.FindByPredicate(
            [&](const auto& S) { return S.Name == Effect.Attributes || S.Id == Effect.Attributes; });
        if (!Set)
            return Fail(TEXT("Effect attribute-set name is not in this script's explicit registry"));
        Effect.Attributes = Set->Id;
        for (const auto& Mod : Effect.Modifiers)
            if (!Set->Attributes.ContainsByPredicate([&](const auto& A) { return A.Name == Mod.Attribute; }))
                return Fail(TEXT("Effect modifies an unknown attribute"));
    }
    Out = MoveTemp(Candidate);
    return true;
}
