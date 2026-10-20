#include "DasGASAuthoring.h"
#include "Dom/JsonObject.h"
#include "Serialization/JsonSerializer.h"
#include "Misc/Paths.h"
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
                                               !Object->TryGetArrayField(TEXT("fields"), Fields) || Fields->Num() > 64)
            return Fail(TEXT("Invalid/duplicate GAS declaration or field count"));
        Names.Add(Name.ToLower());
        Ids.Add(Id);
        if (Kind == TEXT("calculation"))
        {
            FDasGASCalculationDeclaration D;
            D.Name = Name;
            D.Id = Id;
            if (!Object->TryGetStringField(
                    TEXT("calculation_kind"), D.Kind) ||
                    !Object->TryGetStringField(
                        TEXT("attributes"), D.Attributes) ||
                        !Object->TryGetStringField(
                            TEXT("output"), D.Output) ||
                            !Object->TryGetStringField(
                                TEXT("operation"), D.Operation) ||
                                !Object->TryGetStringField(
                                    TEXT("script"), D.ScriptFile) ||
                                    !Object->TryGetStringField(
                                        TEXT("entry"), D.Entry) ||
                                        !Object->TryGetStringField(
                                            TEXT("contract"), D.ScriptContract) || Fields->Num() > 16 ||
                                            (D.Kind != TEXT("Magnitude")&& D.Kind != TEXT("Execution")) ||
                                             (D.Kind == TEXT("Magnitude")? !D.Output.IsEmpty() : !GASName(D.Output)) ||
                                              (D.Operation !=
                                               TEXT("Add")&& D.Operation != TEXT("Multiply")&&
                                                                                 D.Operation != TEXT("Override")) ||
                                                                 FPaths::GetCleanFilename(D.ScriptFile) !=
                                                                     D.ScriptFile ||
                                                                 FPaths::GetExtension(D.ScriptFile) !=
                                                                     TEXT("das")|| D.ScriptContract.Len() != 32 ||
                                                                          D.Entry != TEXT("__ue_gas_calculate_")+ Name)
                return Fail(TEXT("Invalid calculation descriptor"));
            TSet<FName> Seen;
            for (const auto& ItemField : *Fields)
            {
                auto F = ItemField->AsObject();
                FDasGASCaptureDeclaration C;
                FString Alias, Attribute;
                if (!F || !F->TryGetStringField(
                              TEXT("name"), Alias) || !GASName(Alias) || Seen.Contains(FName(*Alias)) ||
                              !F->TryGetStringField(
                                  TEXT("attribute"), Attribute) || !GASName(Attribute) ||
                                  !F->TryGetStringField(
                                      TEXT("attributes"), C.Attributes) ||
                                      !F->TryGetStringField(
                                          TEXT("source"), C.Source) ||
                                          !F->TryGetBoolField(TEXT("snapshot"), C.Snapshot) ||
                                                              (C.Source != TEXT("Source")&& C.Source != TEXT("Target")))
                    return Fail(TEXT("Invalid/duplicate attribute capture"));
                C.Name = FName(*Alias);
                C.Attribute = FName(*Attribute);
                Seen.Add(C.Name);
                D.Captures.Add(C);
            }
            Candidate.Calculations.Add(MoveTemp(D));
            continue;
        }
        if (Kind == TEXT("ability"))
        {
            FDasGASAbilityDeclaration Ability;
            Ability.Name = Name;
            Ability.Id = Id;
            FString Tag;
            if (!Fields->IsEmpty() ||
                !Object->TryGetStringField(
                    TEXT("effect"), Ability.Effect) ||
                    (Object->HasField(TEXT("cost")) && !Object->TryGetStringField(TEXT("cost"), Ability.Cost)) ||
                     (Object->HasField(TEXT("cooldown")) && !GASNumber(Object, TEXT("cooldown"), Ability.Cooldown)) ||
                      (Object->HasField(TEXT("auto_end")) &&
                                        !Object->TryGetBoolField(TEXT("auto_end"), Ability.AutoEnd)) ||
                       (Object->HasField(TEXT("cooldown_tag")) &&
                                         !Object->TryGetStringField(TEXT("cooldown_tag"), Tag)))
                return Fail(TEXT("Invalid GameplayAbility options; an effect name is required"));
            if (Ability.Cooldown < 0 || (Ability.Cooldown > 0 && Ability.Cooldown < 0.01f) ||
                (Ability.Cooldown > 0 ? !FGameplayTag::IsValidGameplayTagString(Tag) : !Tag.IsEmpty()))
                return Fail(TEXT("Positive ability cooldown requires a valid tag and at least 0.01 seconds"));
            auto ReadHook = [&](const TCHAR* Field, FString& Value)
            {
                return !Object->HasField(Field) || Object->TryGetStringField(Field, Value);
            };
            if (!ReadHook(TEXT("script"), Ability.ScriptFile) ||
                          !ReadHook(TEXT("contract"), Ability.ScriptContract) ||
                                    !ReadHook(TEXT("activate"), Ability.ActivateEntry) ||
                                              !ReadHook(TEXT("end"), Ability.EndEntry) ||
                                                        !ReadHook(TEXT("cancel"), Ability.CancelEntry) ||
                                                                  !ReadHook(TEXT("task"), Ability.TaskEntry))
                return Fail(TEXT("Invalid ability script metadata"));
            const FString Prefix = TEXT("__ue_ability_") + Name + TEXT("_");
            if (!Ability.ActivateEntry.IsEmpty())
            {
                if (FPaths::GetCleanFilename(Ability.ScriptFile) != Ability.ScriptFile ||
                    FPaths::GetExtension(Ability.ScriptFile) !=
                        TEXT("das") || Ability.ScriptContract.Len() != 32 ||
                             Ability.ActivateEntry != Prefix + TEXT("Activate") ||
                                                                    (!Ability.EndEntry.IsEmpty() &&
                                                                         Ability.EndEntry != Prefix + TEXT("End")) ||
                                                                     (!Ability.CancelEntry.IsEmpty() &&
                                                                      Ability.CancelEntry != Prefix + TEXT("Cancel")))
                    return Fail(TEXT("Invalid generated ability hook identity"));
                if (!Ability.TaskEntry.IsEmpty() && Ability.TaskEntry != Prefix + TEXT("Task"))
                    return Fail(TEXT("Invalid ability task entry"));
            }
            else if (Ability.Effect.IsEmpty() || !Ability.ScriptFile.IsEmpty() || !Ability.ScriptContract.IsEmpty() ||
                     !Ability.EndEntry.IsEmpty() || !Ability.CancelEntry.IsEmpty() || !Ability.TaskEntry.IsEmpty())
                return Fail(TEXT("Ability requires an effect or an Activate method"));
            Ability.CooldownTag = FName(*Tag);
            Candidate.Abilities.Add(MoveTemp(Ability));
            continue;
        }
        if (Fields->IsEmpty() && !(Kind == TEXT("effect")&& Object->HasField(TEXT("execution"))))
            return Fail(TEXT("AttributeSet/GameplayEffect declarations require fields"));
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
        if (Kind == TEXT("effect"))
        {
            if (Object->HasField(TEXT("execution")) &&
                                 (!Object->TryGetStringField(TEXT("execution"), Effect.Execution) ||
                                                             Effect.Execution.IsEmpty()))
                return Fail(TEXT("Execution requires a calculation name"));
            auto StringOption = [&](const TCHAR* Key, FString& Value)
            {
                return !Object->HasField(Key) || Object->TryGetStringField(Key, Value);
            };
            auto NumberOption = [&](const TCHAR* Key, float& Value)
            {
                return !Object->HasField(Key) || GASNumber(Object, Key, Value);
            };
            auto BoolOption = [&](const TCHAR* Key, bool& Value)
            {
                return !Object->HasField(Key) || Object->TryGetBoolField(Key, Value);
            };
            double Limit = 0;
            if (!StringOption(
                    TEXT("duration"), Effect.Duration) ||
                    !NumberOption(
                        TEXT("seconds"), Effect.Seconds) ||
                        !NumberOption(
                            TEXT("period"), Effect.Period) ||
                            !BoolOption(TEXT("execute_on_application"), Effect.ExecuteOnApplication) ||
                                        !BoolOption(TEXT("deny_overflow"), Effect.DenyOverflow) ||
                                                    !StringOption(
                                                        TEXT("stacking"), Effect.Stacking) ||
                                                        !StringOption(
                                                            TEXT("stack_duration"), Effect.StackDuration) ||
                                                            !StringOption(
                                                                TEXT("stack_period"), Effect.StackPeriod) ||
                                                                !StringOption(
                                                                    TEXT("stack_expiration"), Effect.StackExpiration) ||
                                                                    (Object->HasField(TEXT("stack_limit")) &&
                                                                                      !Object->TryGetNumberField(TEXT("stack_limit"), Limit)) ||
                                                                     !FMath::IsFinite(Limit) || Limit < 0 ||
                                                                     Limit > 128 ||
                                                                     Limit != FMath::FloorToDouble(Limit))
                return Fail(TEXT("Invalid GAS duration/stacking option types or stack limit (1..128)"));
            Effect.StackLimit = int32(Limit);
            const bool Timed = Effect.Duration == TEXT("HasDuration");
            const bool Instant = Effect.Duration == TEXT("Instant");
            const bool Stacking = Effect.Stacking != TEXT("None");
            if ((!Instant && !Timed && Effect.Duration != TEXT("Infinite")) ||
                 (Timed ? Effect.Seconds <= 0 : Object->HasField(TEXT("seconds"))) || Effect.Period < 0 ||
                      (Effect.Period > 0 && Effect.Period < 0.01f) || (Instant && (Effect.Period != 0 || Stacking)) ||
                      (Object->HasField(TEXT("execute_on_application")) && Effect.Period == 0) ||
                       (Effect.Stacking !=
                        TEXT("None") && Effect.Stacking != TEXT("AggregateBySource") &&
                                                                Effect.Stacking != TEXT("AggregateByTarget")) ||
                                            (Stacking ? Effect.StackLimit < 1 : Object->HasField(TEXT("stack_limit"))))
                return Fail(TEXT("Invalid GAS duration/period/stacking combination"));
            if ((Effect.StackDuration !=
                 TEXT("RefreshOnSuccessfulApplication") &&
                      Effect.StackDuration != TEXT("NeverRefresh") && Effect.StackDuration != TEXT("ExtendDuration")) ||
                          (Effect.StackPeriod !=
                           TEXT("ResetOnSuccessfulApplication") && Effect.StackPeriod != TEXT("NeverReset")) ||
                           (Effect.StackExpiration !=
                            TEXT("ClearEntireStack") &&
                                 Effect.StackExpiration != TEXT("RemoveSingleStackAndRefreshDuration") &&
                                                                Effect.StackExpiration != TEXT("RefreshDuration")) ||
                                     (!Stacking &&
                                      (Object->HasField(
                                          TEXT("stack_duration")) ||
                                          Object->HasField(TEXT("stack_period")) ||
                                                           Object->HasField(TEXT("stack_expiration")) ||
                                                                            Object->HasField(TEXT("deny_overflow")))) ||
                                          (Object->HasField(TEXT("stack_duration")) && !Timed) ||
                                           (Object->HasField(TEXT("stack_expiration")) && !Timed) ||
                                            (Object->HasField(TEXT("stack_period")) && Effect.Period == 0))
                return Fail(TEXT("Invalid or inapplicable GAS stacking policy"));
        }
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
                if (F->HasField(TEXT("calculation")) && (!F->TryGetStringField(TEXT("calculation"), Mod.Calculation) ||
                                                                               Mod.Calculation.IsEmpty()))
                    return Fail(TEXT("Calculation requires a name"));
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
                if (!Mod.Calculation.IsEmpty() && (!Mod.SetByCaller.IsNone() || Mod.Magnitude != 0))
                    return Fail(TEXT("Calculation conflicts with literal/SetByCaller magnitude"));
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
    for (auto& D : Candidate.Calculations)
    {
        auto ResolveSet = [&](const FString& Hint)
        {
            return Candidate.Sets.FindByPredicate([&](const auto& S) { return S.Name == Hint || S.Id == Hint; });
        };
        const auto* OutputSet = ResolveSet(D.Attributes);
        if (!OutputSet)
            return Fail(TEXT("Calculation attribute set is not in the explicit registry"));
        D.Attributes = OutputSet->Id;
        if (D.Kind == TEXT("Execution")&& !OutputSet->Attributes.ContainsByPredicate(
                                              [&](const auto& A) { return A.Name == FName(*D.Output); }))
            return Fail(TEXT("Unknown execution output attribute"));
        for (auto& C : D.Captures)
        {
            const auto* Set = ResolveSet(C.Attributes);
            if (!Set || !Set->Attributes.ContainsByPredicate([&](const auto& A) { return A.Name == C.Attribute; }))
                return Fail(TEXT("Unknown captured attribute"));
            C.Attributes = Set->Id;
        }
    }
    for (auto& Effect : Candidate.Effects)
    {
        const auto* Set = Candidate.Sets.FindByPredicate(
            [&](const auto& S) { return S.Name == Effect.Attributes || S.Id == Effect.Attributes; });
        if (!Set)
            return Fail(TEXT("Effect attribute-set name is not in this script's explicit registry"));
        Effect.Attributes = Set->Id;
        auto ResolveCalculation = [&](FString& Hint, const TCHAR* Kind)
        {
            const auto* D =
                Candidate.Calculations.FindByPredicate([&](const auto& C) { return C.Id == Hint || C.Name == Hint; });
            if (!D || D->Kind != Kind)
                return false;
            Hint = D->Id;
            return true;
        };
        if (!Effect.Execution.IsEmpty() &&
            (!ResolveCalculation(Effect.Execution, TEXT("Execution")) ||
                                 (Effect.Duration != TEXT("Instant")&& Effect.Period <= 0)))
            return Fail(TEXT("Execution calculation requires Instant or periodic effect"));
        for (auto& M : Effect.Modifiers)
            if (!M.Calculation.IsEmpty() && !ResolveCalculation(M.Calculation, TEXT("Magnitude")))
                return Fail(TEXT("Unknown magnitude calculation"));
        for (const auto& Mod : Effect.Modifiers)
            if (!Set->Attributes.ContainsByPredicate([&](const auto& A) { return A.Name == Mod.Attribute; }))
                return Fail(TEXT("Effect modifies an unknown attribute"));
    }
    for (auto& Ability : Candidate.Abilities)
    {
        auto Resolve = [&](const FString& Reference) -> const FDasGASEffectDeclaration*
        {
            return Candidate.Effects.FindByPredicate([&](const auto& E)
                                                     { return E.Name == Reference || E.Id == Reference; });
        };
        const auto* Effect = Ability.Effect.IsEmpty() ? nullptr : Resolve(Ability.Effect);
        const auto* Cost = Ability.Cost.IsEmpty() ? nullptr : Resolve(Ability.Cost);
        if ((!Ability.Effect.IsEmpty() && !Effect) || (!Ability.Cost.IsEmpty() && !Cost))
            return Fail(TEXT("Ability effect/cost is not in this script's explicit effect registry"));
        for (const auto* E : {Effect, Cost})
            if (E)
                for (const auto& Mod : E->Modifiers)
                    if (!Mod.SetByCaller.IsNone())
                        return Fail(TEXT("Declarative abilities require constant effect magnitudes"));
        if (Cost)
        {
            if (Cost->Duration != TEXT("Instant") || !Cost->Execution.IsEmpty())
                return Fail(TEXT("Ability cost must be an Instant GameplayEffect"));
            for (const auto& Mod : Cost->Modifiers)
                if (Mod.Operation != TEXT("Add") || Mod.Magnitude >= 0 || !Mod.Calculation.IsEmpty())
                    return Fail(TEXT("Ability costs require strictly negative Add modifiers"));
        }
        if (Effect)
            Ability.Effect = Effect->Id;
        if (Cost)
            Ability.Cost = Cost->Id;
    }
    Out = MoveTemp(Candidate);
    return true;
}
