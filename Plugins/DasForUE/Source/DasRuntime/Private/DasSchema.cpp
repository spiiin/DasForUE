#include "DasDataAssets.h"
#include "DasTypeContracts.h"
#include "UObject/TextProperty.h"
#include "Internationalization/Text.h"
#include "DasScriptHosts.h"
#include "DasSchema.h"
#include "DasAuthoring.h"
#include "DasDelegateSignatures.h"
#include "UObject/StrongObjectPtr.h"
#include "UObject/Package.h"
#include "DasInterfaceContracts.h"
#include "DasContainerTypes.h"
#include "DasRuntime.h"
#include "Misc/Paths.h"
#include "Misc/PackageName.h"
#include "UObject/Class.h"
#include "StructUtils/InstancedStruct.h"
#include "UObject/SoftObjectPath.h"

namespace
{
bool Identifier(const FString& Value)
{
    if (Value.IsEmpty() || Value.Len() >= NAME_SIZE || FChar::IsDigit(Value[0]))
        return false;
    for (TCHAR Ch : Value)
        if (!FChar::IsAlnum(Ch) && Ch != TEXT('_'))
            return false;
    return FName(*Value) != NAME_None;
}
}

namespace
{
bool HasDasTextDefaultLeaf(const UScriptStruct* Type);
bool HasDasTextDefaultProperty(const FProperty* P)
{
    if (CastField<FTextProperty>(P))
        return true;
    if (auto* S = CastField<FStructProperty>(P))
        return HasDasTextDefaultLeaf(S->Struct);
    if (auto* A = CastField<FArrayProperty>(P))
        return HasDasTextDefaultProperty(A->Inner);
    if (auto* M = CastField<FMapProperty>(P))
        return HasDasTextDefaultProperty(M->ValueProp);
    return false;
}
bool HasDasTextDefaultLeaf(const UScriptStruct* Type)
{
    for (TFieldIterator<FProperty> It(Type); It; ++It)
        if (HasDasTextDefaultProperty(*It))
            return true;
    return false;
}
}
bool MatchesDasPropertyDefault(const FProperty* P, const void* A, const void* B)
{
    if (!HasDasTextDefaultProperty(P))
        return P->Identical(A, B);
    if (auto* T = CastField<FTextProperty>(P))
        return DasTextDefaultsEqual(T->GetPropertyValue(A), T->GetPropertyValue(B));
    if (auto* S = CastField<FStructProperty>(P))
        return MatchesDasStructDefault(S->Struct, A, B);
    if (auto* V = CastField<FArrayProperty>(P))
    {
        FScriptArrayHelper X(V, A), Y(V, B);
        if (X.Num() != Y.Num())
            return false;
        for (int32 I = 0; I < X.Num(); ++I)
            if (!MatchesDasPropertyDefault(V->Inner, X.GetRawPtr(I), Y.GetRawPtr(I)))
                return false;
        return true;
    }
    if (auto* V = CastField<FMapProperty>(P))
    {
        FScriptMapHelper X(V, A), Y(V, B);
        if (X.Num() != Y.Num())
            return false;
        for (int32 I = 0; I < X.GetMaxIndex(); ++I)
            if (X.IsValidIndex(I))
            {
                const int32 J = Y.FindMapIndexWithKey(X.GetKeyPtr(I));
                if (J == INDEX_NONE || !MatchesDasPropertyDefault(V->ValueProp, X.GetValuePtr(I), Y.GetValuePtr(J)))
                    return false;
            }
        return true;
    }
    return false;
}
bool MatchesDasStructDefault(UScriptStruct* Type, const void* Actual, const void* Expected)
{
    if (!HasDasTextDefaultLeaf(Type))
    {
        if (Type->CompareScriptStruct(Actual, Expected, 0))
            return true;
        if (Type != TBaseStructure<FTransform>::Get())
            return false;
        const auto& V = *static_cast<const FTransform*>(Expected);
        const FTransform Converted(V.Rotator(), V.GetTranslation(), V.GetScale3D());
        return Type->CompareScriptStruct(Actual, &Converted, 0);
    }
    for (TFieldIterator<FProperty> It(Type); It; ++It)
        if (!MatchesDasPropertyDefault(*It, It->ContainerPtrToValuePtr<void>(Actual),
                                       It->ContainerPtrToValuePtr<void>(Expected)))
            return false;
    return true;
}

bool ResolveDasReferenceDefault(bool IsClass, const FString& BasePath, const FString& Path, UObject*& Out,
                                FString& Error)
{
    Out = nullptr;
    if (Path.IsEmpty())
        return true;
    if (Path.Len() > 4096 || !FPackageName::IsValidObjectPath(Path))
    {
        Error = TEXT("Default requires a valid object path");
        return false;
    }
    auto* Base = FindObject<UClass>(nullptr, *BasePath);
    FSoftObjectPath ObjectPath(Path);
    if (!Base || !ObjectPath.IsValid() || !ObjectPath.GetSubPathString().IsEmpty() || Path.Len() > 4096)
    {
        Error = TEXT("Default requires a valid top-level object path and loaded base class");
        return false;
    }
    Out = LoadObject<UObject>(nullptr, *Path, nullptr, LOAD_NoWarn);
    bool Good = IsValid(Out);
    if (IsClass)
    {
        auto* Class = Cast<UClass>(Out);
        Good &=
            Class && Class->IsChildOf(Base) && !Class->HasAnyClassFlags(CLASS_Deprecated | CLASS_NewerVersionExists);
    }
    else
        Good &=
            Out && Out->IsAsset() && !Out->IsA<UClass>() &&
            (Base->HasAnyClassFlags(CLASS_Interface) ? DasImplementsInterface(Out->GetClass(), Base) : Out->IsA(Base));
    if (!Good)
    {
        Out = nullptr;
        Error = TEXT("Default asset/class is missing, obsolete or incompatible: ")+ Path;
        return false;
    }
    return true;
}

TArray<FDasMethodSchema> FDasClassSchema::EffectiveMethods() const
{
    return Methods.IsEmpty()
               ? (Method.ScriptFile.IsEmpty() ? TArray<FDasMethodSchema>{} : TArray<FDasMethodSchema>{Method})
               : Methods;
}

bool FDasClassSchema::Validate(FString& Error) const
{
    Error.Reset();
    if (!Identifier(Name.ToString()) || Name.IsNone() ||
        (Parent !=
         TEXT("Actor") &&
#if DAS_WITH_TEST_PROBES
              Parent != TEXT("DasProbeActor") &&
#endif
                             Parent != TEXT("Pawn") &&
                                            Parent !=
                                                TEXT("Character") &&
                                                     Parent !=
                                                         TEXT("ActorComponent") &&
                                                              Parent !=
                                                                  TEXT("SaveGame") &&
                                                                       Parent !=
                                                                           TEXT("DataAsset") &&
                                                                                Parent !=
                                                                                    TEXT("PrimaryDataAsset") &&
                                                                                         Parent !=
                                                                                             TEXT("FunctionLibrary")))
    {
        Error = TEXT("Expected a named Actor, Pawn, Character, ActorComponent or function-library schema");
        return false;
    }
    auto ValidMeta = [&](const TMap<FString, FString>& Values, bool Method = false, bool Field = false)
    {
        for (const auto& Pair : Values)
            if ((Pair.Key !=
                 TEXT("Category") &&
                      Pair.Key !=
                          TEXT("DisplayName") &&
                               Pair.Key !=
                                   TEXT("ToolTip") &&
                                        !(Method &&
                                          Pair.Key == TEXT("BlueprintInternalUseOnly") && Pair.Value == TEXT("true")) &&
                                              !(Field &&
                                                Pair.Key == TEXT("AssetBundles") &&
                                                                 Parent == TEXT("PrimaryDataAsset")) &&
                                                    !(!Field && !Method &&
                                                      Pair.Key == TEXT("PrimaryAssetType") &&
                                                                       Parent == TEXT("PrimaryDataAsset")) &&
                                                          !(Method &&
                                                            Pair.Key == TEXT("DasFunctionKind") &&
                                                                             (Pair.Value ==
                                                                              TEXT("Event") ||
                                                                                   Pair.Value == TEXT("Callable")))) ||
                                                            Pair.Value.Len() > 4096)
            {
                Error = TEXT("Unsupported adapter metadata");
                return false;
            }
        return true;
    };
    if (!ValidMeta(Metadata))
        return false;
    TSet<FName> Names;
    const auto AllMethods = EffectiveMethods();
    if ((Parent ==
         TEXT("DataAsset") ||
              Parent == TEXT("PrimaryDataAsset") ||
                             Parent == TEXT("SaveGame") ||
                                            Parent == TEXT("Pawn") || Parent == TEXT("Character")) &&
                                                           AllMethods.ContainsByPredicate(
                                                               [](const auto& M)
                                                               { return M.Signature != EDasMethodSignature::Frame; }))
    {
        Error = TEXT("Pawn/Character hosts require the frame method ABI");
        return false;
    }
    if (Parent == TEXT("FunctionLibrary") && (!Fields.IsEmpty() || !ParentStableId.IsEmpty()))
    {
        Error = TEXT("Function library cannot have fields or authored parent");
        return false;
    }
    if (AllMethods.Num() > 64)
    {
        Error = TEXT("Adapter method count exceeds 64");
        return false;
    }
    for (const auto& M : AllMethods)
    {
        if (M.bStatic != (Parent == TEXT("FunctionLibrary")) ||
                          (M.bStatic && (M.bConst || M.bBlueprintEvent || M.LibraryContract.IsEmpty())))
        {
            Error = TEXT("Static methods require a function library contract");
            return false;
        }
        if (!Identifier(M.Name.ToString()) || M.Name.IsNone() || Names.Contains(M.Name) || !Identifier(M.EntryPoint) ||
            M.ScriptFile.IsEmpty() || FPaths::GetCleanFilename(M.ScriptFile) != M.ScriptFile ||
            M.ScriptFile.Contains(TEXT(":")) || !M.ScriptFile.EndsWith(TEXT(".das")))
        {
            Error = TEXT("Invalid method name, entry point or local script filename");
            return false;
        }
        if (M.Signature != EDasMethodSignature::Int && M.Signature != EDasMethodSignature::Float &&
            M.Signature != EDasMethodSignature::Bool && M.Signature != EDasMethodSignature::Void &&
            M.Signature != EDasMethodSignature::Frame)
        {
            Error = TEXT("Unsupported method signature");
            return false;
        }
        const auto Contract = M.Contract();
        if (Contract.Inputs.Num() > 16 ||
            ((static_cast<uint8>(Contract.ReturnKind) > 13 && Contract.ReturnKind != EDasMethodSignature::Class &&
              Contract.ReturnKind != EDasMethodSignature::Delegate &&
              Contract.ReturnKind != EDasMethodSignature::Byte && Contract.ReturnKind != EDasMethodSignature::Text) ||
             Contract.ReturnKind == EDasMethodSignature::Frame))
        {
            Error = TEXT("Invalid scalar frame contract");
            return false;
        }
        if (M.bPure && Contract.ReturnKind == EDasMethodSignature::Void &&
            !M.EffectiveParameters().ContainsByPredicate([](const auto& P) { return P.Direction != TEXT("in"); }))
        {
            Error = TEXT("BlueprintPure requires a result or output parameter");
            return false;
        }
        TSet<FName> ParameterNames;
        for (const auto& P : M.EffectiveParameters())
        {
            if ((P.Direction !=
                 TEXT("in") &&
                      P.Direction != TEXT("out") && P.Direction != TEXT("inout")) || !Identifier(P.Name.ToString()) ||
                          P.Name ==
                              TEXT("ReturnValue") || ParameterNames.Contains(P.Name) ||
                                   ((static_cast<uint8>(P.Kind) > 13 && P.Kind != EDasMethodSignature::Class &&
                                     P.Kind != EDasMethodSignature::Delegate && P.Kind != EDasMethodSignature::Byte &&
                                     P.Kind != EDasMethodSignature::Text) ||
                                    P.Kind == EDasMethodSignature::Void || P.Kind == EDasMethodSignature::Frame))
            {
                Error = TEXT("Invalid scalar frame parameter");
                return false;
            }
            ParameterNames.Add(P.Name);
        }
        if (!ValidateDasLoweredSignatureType(Contract.ReturnKind, M.ReturnClass, EDasTypeUse::AuthoredReturn, Error))
            return false;
        for (const auto& P : M.EffectiveParameters())
            if (!ValidateDasLoweredSignatureType(P.Kind, P.ClassPath,
                                                 P.Direction == TEXT("in") ? EDasTypeUse::AuthoredInput
                                                                           : P.Direction ==
                                                                                 TEXT("out") ? EDasTypeUse::AuthoredOut
                                                                                             : EDasTypeUse::AuthoredRef,
                                                                                      Error))
                return false;
        if (!ValidMeta(M.Metadata, true))
            return false;
        Names.Add(M.Name);
    }
    for (const auto& Field : Fields)
    {
        if (Field.Name.IsNone() || !Identifier(Field.Name.ToString()) || Names.Contains(Field.Name))
        {
            Error = FString::Printf(TEXT("Invalid or duplicate field: %s"), *Field.Name.ToString());
            return false;
        }
        if ((Field.Kind != EDasScalarKind::Byte && Field.Kind != EDasScalarKind::Text &&
             Field.Kind != EDasScalarKind::Int32 && Field.Kind != EDasScalarKind::Float32 &&
             Field.Kind != EDasScalarKind::Bool && Field.Kind != EDasScalarKind::String &&
             Field.Kind != EDasScalarKind::Double && Field.Kind != EDasScalarKind::Name &&
             Field.Kind != EDasScalarKind::Struct && Field.Kind != EDasScalarKind::Object &&
             Field.Kind != EDasScalarKind::Enum && Field.Kind != EDasScalarKind::Event &&
             Field.Kind != EDasScalarKind::Class && Field.Kind != EDasScalarKind::Delegate) ||
            ((Field.Kind == EDasScalarKind::Float32 && !FMath::IsFinite(Field.FloatDefault)) ||
             (Field.Kind == EDasScalarKind::Double && !FMath::IsFinite(Field.DoubleDefault))))
        {
            Error = TEXT("Unsupported scalar kind or non-finite default");
            return false;
        }
        if (Field.Kind == EDasScalarKind::Byte && (Field.IntDefault < 0 || Field.IntDefault > 255))
        {
            Error = TEXT("Byte default must be 0..255");
            return false;
        }
        if (Field.Kind == EDasScalarKind::Event || Field.Kind == EDasScalarKind::Delegate)
        {
            FDasAuthoringModule Definition;
            if (!FDasAuthoringModule::FromJson(Field.DelegateWire, Definition, Error) ||
                Definition.Classes.Num() != 1 || Definition.Classes[0].Methods.Num() != 1)
                return false;
            TStrongObjectPtr<UFunction> Signature(MakeDasDelegateSignature(Definition.Classes[0].Methods[0],
                                                                           Field.Kind == EDasScalarKind::Event,
                                                                           GetTransientPackage(), NAME_None, Error));
            if (!Signature)
                return false;
            if (Field.bInstanceEditable || (Field.Kind == EDasScalarKind::Event && Field.bReadOnly))
            {
                Error = TEXT("Delegate fields cannot be instance editable; events cannot be readonly");
                return false;
            }
        }
        if (Field.Kind == EDasScalarKind::Enum)
        {
            auto* E = ResolveDasEnum(Field.EnumPath, Error);
            if (!E || !ValidateDasEnumNumber(Field.EnumPath, E->GetValueByNameString(Field.EnumDefault), Error))
                return false;
        }
        if (Field.Kind == EDasScalarKind::Object &&
            (!FindObject<UClass>(nullptr, *Field.ObjectClass) ||
             (!Field.ObjectClass.StartsWith(
                 TEXT("/Script/")) &&
                 !FindObject<UClass>(nullptr, *Field.ObjectClass)->HasAnyClassFlags(CLASS_Interface))))
        {
            Error = TEXT("Object field requires a loaded native class path");
            return false;
        }
        if (Field.Kind == EDasScalarKind::Class)
        {
            auto* Base = FindObject<UClass>(nullptr, *Field.ObjectClass);
            if (!Field.ObjectClass.StartsWith(TEXT("/Script/")) || !Base || Base->HasAnyClassFlags(CLASS_Interface))
            {
                Error = TEXT("Class field requires loaded native base class");
                return false;
            }
        }
        if ((Field.Kind == EDasScalarKind::Class || Field.Kind == EDasScalarKind::Object) &&
            !Field.StringDefault.IsEmpty())
        {
            UObject* Value = nullptr;
            if (!ResolveDasReferenceDefault(Field.Kind == EDasScalarKind::Class, Field.ObjectClass, Field.StringDefault,
                                            Value, Error))
                return false;
        }
        if (Field.Kind == EDasScalarKind::Struct)
        {
            FInstancedStruct Value;
            if (!ParseDasStructDefault(Field.StructPath, Field.StructDefault, Value, Error))
                return false;
        }
        if (!ValidMeta(Field.Metadata, false, true))
            return false;
        Names.Add(Field.Name);
    }
    return ValidateDasInterfaceContracts(*this, Error);
}

bool FDasClassSchema::FromDeclaration(const FDasTypeDeclaration& Declaration, const FString& Script,
                                      FDasClassSchema& Out, FString& Error)
{
    Error.Reset();
    if (!Identifier(Declaration.Name) || !Identifier(Declaration.Parent))
    {
        Error = TEXT("Invalid declared type name");
        return false;
    }
    FDasClassSchema Candidate;
    Candidate.Name = *Declaration.Name;
    Candidate.Parent = *Declaration.Parent;
    Candidate.Method.ScriptFile = Script;
    TArray<FString> Items;
    Declaration.Fields.ParseIntoArray(Items, TEXT(";"));
    for (const auto& Item : Items)
    {
        FString Name, Kind;
        if (!Item.Split(TEXT(":"), &Name, &Kind) || !Identifier(Name))
        {
            Error = FString::Printf(TEXT("Invalid field declaration: %s"), *Item);
            return false;
        }
        FDasFieldSchema Field;
        Field.Name = *Name;
        if (Kind == TEXT("int"))
            Field.Kind = EDasScalarKind::Int32;
        else if (Kind == TEXT("float"))
            Field.Kind = EDasScalarKind::Float32;
        else
        {
            Error = FString::Printf(TEXT("Unsupported field type: %s"), *Kind);
            return false;
        }
        Candidate.Fields.Add(Field);
    }
    if (!Candidate.Validate(Error))
        return false;
    Out = MoveTemp(Candidate);
    return true;
}

FString DasPropertyDefaultText(const FProperty* P, const void* Data)
{
    if (auto* N = CastField<FNumericProperty>(P); N && N->IsFloatingPoint())
        return FString::Printf(TEXT("%.17g"), N->GetFloatingPointPropertyValue(Data));
    FString Text = TEXT("(");
    bool First = true;
    auto Add = [&](const FString& Part)
    {
        if (!First)
            Text += TEXT(",");
        First = false;
        Text += Part;
    };
    if (auto* A = CastField<FArrayProperty>(P))
    {
        FScriptArrayHelper H(A, Data);
        for (int32 I = 0; I < H.Num(); ++I)
            Add(DasPropertyDefaultText(A->Inner, H.GetRawPtr(I)));
    }
    else if (auto* S = CastField<FSetProperty>(P))
    {
        FScriptSetHelper H(S, Data);
        for (int32 I = 0; I < H.GetMaxIndex(); ++I)
            if (H.IsValidIndex(I))
                Add(DasPropertyDefaultText(S->ElementProp, H.GetElementPtr(I)));
    }
    else if (auto* M = CastField<FMapProperty>(P))
    {
        FScriptMapHelper H(M, Data);
        for (int32 I = 0; I < H.GetMaxIndex(); ++I)
            if (H.IsValidIndex(I))
                Add(TEXT("(")+ DasPropertyDefaultText(M->KeyProp, H.GetKeyPtr(I)) +
                         TEXT(",")+ DasPropertyDefaultText(M->ValueProp, H.GetValuePtr(I)) + TEXT(")"));
    }
    else if (auto* Struct = CastField<FStructProperty>(P))
    {
        for (TFieldIterator<FProperty> It(Struct->Struct); It; ++It)
            Add(It->GetName() + TEXT("=")+ DasPropertyDefaultText(*It, It->ContainerPtrToValuePtr<void>(Data)));
    }
    else
    {
        Text.Reset();
        P->ExportTextItem_Direct(Text, Data, nullptr, nullptr, PPF_Delimited);
        return Text;
    }
    return Text + TEXT(")");
}


bool ParseDasTextDefault(const FString& Wire, FText& Out, FString& Error)
{
    Out = FText::GetEmpty();
    if (Wire.IsEmpty())
        return true;
    if (Wire.Len() > 131072 || (!Wire.StartsWith(TEXT("NSLOCTEXT(")) && !Wire.StartsWith(TEXT("INVTEXT("))))
    {
        Error = TEXT("Text default requires bounded NSLOCTEXT or INVTEXT");
        return false;
    }
    const TCHAR* End = FTextStringHelper::ReadFromBuffer(*Wire, Out);
    if (!End || !FString(End).TrimStartAndEnd().IsEmpty())
    {
        Error = TEXT("Malformed text default");
        return false;
    }
    return true;
}
FString DasTextLiteral(const FString& Namespace, const FString& Key, const FString& Source, bool Invariant)
{
    auto Quote = [](const FString& V)
    {
        return TEXT("\"")+ V.ReplaceCharWithEscapedChar() + TEXT("\"");
    };
    return Invariant
        ? TEXT("INVTEXT(")+ Quote(Source) +
               TEXT(")")
               : TEXT("NSLOCTEXT(")+ Quote(Namespace) + TEXT(",")+ Quote(Key) + TEXT(",")+ Quote(Source) + TEXT(")");
}

bool DasTextDefaultsEqual(const FText& A, const FText& B)
{
    // Blueprint CDO delta serialization may omit an empty invariant value as
    // identical to the zero-initialized empty text. There is no localization
    // identity to preserve in that case; never apply this to named texts.
    auto UnnamedEmpty = [](const FText& V)
    {
        const auto Namespace = FTextInspector::GetNamespace(V), Key = FTextInspector::GetKey(V);
        return V.IsEmpty() && (!Namespace.IsSet() || Namespace.GetValue().IsEmpty()) &&
               (!Key.IsSet() || Key.GetValue().IsEmpty());
    };
    if (UnnamedEmpty(A) && UnnamedEmpty(B))
        return true;
    FString Left, Right;
    FTextStringHelper::WriteToBuffer(Left, A);
    FTextStringHelper::WriteToBuffer(Right, B);
    return Left == Right && A.IsCultureInvariant() == B.IsCultureInvariant();
}
