#include "DasContainerRegistry.h"
#include "DasPropertyTypes.h"
#include "DasDefinedEnum.h"
#include "UObject/Package.h"
#include "UObject/UnrealType.h"

namespace
{
// Capture referenced definitions as well as container shape. A path alone does
// not detect edits to a live UUserDefinedStruct or authored enum.
class FLayoutFingerprint
{
public:
    FString Text;
    bool Type(const FDasDeclaredType& T, int32 Depth = 0)
    {
        if (!Step(Depth)) return false;
        Token(T.Kind); Token(T.TypePath);
        Text += FString::Printf(TEXT(":%d;"), int32(T.EnumStorage));
        if (T.Element && !Type(*T.Element, Depth + 1)) return false;
        if (T.Key && !Type(*T.Key, Depth + 1)) return false;
        if (T.Value && !Type(*T.Value, Depth + 1)) return false;
        if (T.GetKind() == EDasTypeKind::Struct)
            return Struct(FindObject<UScriptStruct>(nullptr, *T.TypePath), Depth + 1);
        if (T.GetKind() == EDasTypeKind::Enum)
            return Enum(FindObject<UEnum>(nullptr, *T.TypePath));
        if (T.GetKind() == EDasTypeKind::Object || T.GetKind() == EDasTypeKind::Class)
            return Reference(FindObject<UClass>(nullptr, *T.TypePath));
        return Text.Len() <= MaxCharacters;
    }
private:
    static constexpr int32 MaxCharacters = 1048576;
    int32 Nodes = 0;
    TSet<const UScriptStruct*> Active;
    bool Step(int32 Depth) { return Depth <= 64 && ++Nodes <= 16384 && Text.Len() <= MaxCharacters; }
    void Token(const FString& S) { Text += FString::Printf(TEXT("%d:"), S.Len()) + S; }
    bool Reference(const UClass* C)
    {
        if (!IsValid(C)) return false;
        Token(C->GetPathName());
        Text += FString::Printf(TEXT("@%p:%u;"), C, uint32(C->GetClassFlags() & (CLASS_Interface | CLASS_Deprecated | CLASS_NewerVersionExists)));
        return Text.Len() <= MaxCharacters;
    }
    bool Enum(const UEnum* E)
    {
        if (!IsValid(E)) return false;
        Token(E->GetPathName());
        Text += FString::Printf(TEXT("@%p:%d:%d{"), E, int32(E->GetCppForm()), int32(E->GetUnderlyingType()));
        const auto* Authored = Cast<UDasDefinedEnum>(E);
        for (int32 I = 0; I < E->NumEnums(); ++I)
        {
            if (!Step(0)) return false;
            Token(E->GetNameByIndex(I).ToString());
            Text += FString::Printf(TEXT("=%lld;"), E->GetValueByIndex(I));
            if (Authored) Token(Authored->MemberIds.FindRef(E->GetValueByIndex(I)));
        }
        Text += TEXT("}");
        return Text.Len() <= MaxCharacters;
    }
    bool Struct(const UScriptStruct* S, int32 Depth)
    {
        if (!IsValid(S) || !Step(Depth) || Active.Contains(S)) return false;
        Active.Add(S); Token(S->GetPathName());
        Text += FString::Printf(TEXT("@%p:%d:%d{"), S, S->GetStructureSize(), S->GetMinAlignment());
        for (TFieldIterator<FProperty> It(S); It; ++It)
            if (!Property(*It, Depth + 1)) return false;
        Active.Remove(S); Text += TEXT("}");
        return Text.Len() <= MaxCharacters;
    }
    bool Property(const FProperty* P, int32 Depth)
    {
        if (!P || !Step(Depth)) return false;
        Token(P->GetName()); Token(P->GetClass()->GetName());
        Text += FString::Printf(TEXT(":%d:%d:%d:%llu;"), P->GetOffset_ForInternal(), P->GetElementSize(), P->ArrayDim, uint64(P->GetPropertyFlags()));
        if (auto* S = CastField<FStructProperty>(P)) return Struct(S->Struct, Depth + 1);
        if (auto* A = CastField<FArrayProperty>(P)) return Property(A->Inner, Depth + 1);
        if (auto* M = CastField<FMapProperty>(P)) return Property(M->KeyProp, Depth + 1) && Property(M->ValueProp, Depth + 1);
        if (auto* Set = CastField<FSetProperty>(P)) return Property(Set->ElementProp, Depth + 1);
        if (auto* C = CastField<FClassProperty>(P)) return Reference(C->MetaClass);
        if (auto* C = DasReferencePropertyClass(P)) return Reference(C);
        if (auto* E = DasEnumPropertyType(const_cast<FProperty*>(P))) return Enum(E);
        if (auto* B = CastField<FBoolProperty>(P))
            Text += FString::Printf(TEXT("bool:%d:%d:%d;"), B->GetByteOffset(), B->GetByteMask(), B->GetFieldMask());
        return Text.Len() <= MaxCharacters;
    }
};
}

bool DasPropertyLayoutFingerprint(const FDasDeclaredType& Type,FString& Out){FLayoutFingerprint F;if(!F.Type(Type))return false;Out=MoveTemp(F.Text);return true;}

FDasContainerRegistry::~FDasContainerRegistry() { Reset(); }

UScriptStruct* FDasContainerRegistry::GetOrCreate(const FString& Path, const FDasDeclaredType& Type, FString& Error)
{
    Error.Reset();
    auto Fail = [&](const TCHAR* Why) -> UScriptStruct* { Error = FString(Why) + TEXT(": ") + Path; return nullptr; };
    if (!IsInGameThread()) return Fail(TEXT("Carrier registry requires the game thread"));
    const FString Prefix = TEXT("/Script/DasRuntime.");
    if (!Path.StartsWith(Prefix) || Path.Len() >= NAME_SIZE || Path.Len() == Prefix.Len() ||
        Path.RightChop(Prefix.Len()).Contains(TEXT("/")) || Path.RightChop(Prefix.Len()).Contains(TEXT(".")))
        return Fail(TEXT("Invalid carrier path"));
    if (Type.GetKind() != EDasTypeKind::Array && Type.GetKind() != EDasTypeKind::Set && Type.GetKind() != EDasTypeKind::Map)
        return Fail(TEXT("Expected container descriptor"));
    if (!ValidateDasPropertyType(Type, EDasPropertyUsage::Container, Error)) return nullptr;
    FLayoutFingerprint Layout;
    if (!Layout.Type(Type)) return Fail(TEXT("Unsupported or excessive carrier layout"));
    if (const auto* Entry = Entries.Find(Path))
    {
        auto* S = Entry->Carrier.Get();
        return S && S->IsRooted() && Entry->Fingerprint == Layout.Text ? S : Fail(TEXT("Carrier definition changed; live carrier cannot be reused"));
    }
    if (Entries.Num() >= Limit) return Fail(TEXT("Carrier registry limit reached"));
    if (FindObject<UObject>(nullptr, *Path)) return Fail(TEXT("Carrier path is already occupied"));
    auto* S = NewObject<UScriptStruct>(CreatePackage(TEXT("/Script/DasRuntime")), *Path.RightChop(Prefix.Len()), RF_Transient);
    auto* Values = CreateDasProperty(S, TEXT("Values"), Type, EDasPropertyUsage::Container, Error);
    if (!Values)
    {
        S->Rename(nullptr, GetTransientPackage(), REN_DontCreateRedirectors | REN_NonTransactional);
        S->MarkAsGarbage(); return nullptr;
    }
    S->AddCppProperty(Values); S->Bind(); S->StaticLink(true); S->AddToRoot();
    Entries.Add(Path, {S, MoveTemp(Layout.Text)});
    return S;
}

void FDasContainerRegistry::Reset()
{
    if (Entries.IsEmpty()) return;
    check(IsInGameThread());
    for (auto& Entry : Entries)
        if (auto* S = Entry.Value.Carrier.Get()) S->RemoveFromRoot();
    Entries.Empty();
}
