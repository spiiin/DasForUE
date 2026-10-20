#include "DasDataAssets.h"
#include "DasCallbackTypes.h"
#include "DasScriptHosts.h"
#include "DasAuthoring.h"
#include "Components/ActorComponent.h"
#include "DasNativeDefaults.h"
#include "DasContainerTypes.h"
#include "Misc/SecureHash.h"
#include "StructUtils/InstancedStruct.h"
#include "Dom/JsonObject.h"
#include "Serialization/JsonSerializer.h"
#include "Policies/CondensedJsonPrintPolicy.h"
#include "Misc/Paths.h"

namespace
{
bool Fail(FString& Error, const FString& Message)
{
    Error = Message;
    return false;
}
bool Identifier(const FString& Text)
{
    if (Text.IsEmpty() || Text.Len() >= NAME_SIZE || FChar::IsDigit(Text[0]) ||
        Text.Equals(TEXT("None"), ESearchCase::IgnoreCase))
        return false;
    for (TCHAR Ch : Text)
        if (!FChar::IsAlnum(Ch) && Ch != TEXT('_'))
            return false;
    return true;
}
bool StableId(const FString& Text)
{
    if (Text.IsEmpty() || Text.Len() > 256)
        return false;
    for (TCHAR Ch : Text)
        if (!((Ch >= 'a' && Ch <= 'z') || (Ch >= 'A' && Ch <= 'Z') || (Ch >= '0' && Ch <= '9') || Ch == '_' ||
              Ch == '.' || Ch == '-'))
            return false;
    return true;
}
bool NativePath(const FString& Text)
{
    if (!Text.StartsWith(TEXT("/")) || !Text.Contains(TEXT(".")) || Text.Len() >= NAME_SIZE)
        return false;
    for (TCHAR Ch : Text)
        if (!FChar::IsAlnum(Ch) && Ch != '_' && Ch != '/' && Ch != '.')
            return false;
    return true;
}
bool MetadataValid(const TMap<FString, FString>& Metadata, FString& Error, bool Class = false, bool Property = false)
{
    for (const auto& Pair : Metadata)
        if ((Pair.Key != TEXT("Category") &&
                              Pair.Key != TEXT("DisplayName") &&
                                               Pair.Key !=
                                                   TEXT("ToolTip") &&
                                                        !(!Class && !Property &&
                                                          Pair.Key == TEXT("BlueprintInternalUseOnly") &&
                                                                           Pair.Value == TEXT("true")) &&
                                                              !(Class && Pair.Key == TEXT("PrimaryAssetType")) &&
                                                                !(Property && Pair.Key == TEXT("AssetBundles"))) ||
                                                              Pair.Value.Len() > 4096)
            return Fail(Error, TEXT("Unsupported metadata key/value: ") + Pair.Key);
    return true;
}
bool SameMetadata(const TMap<FString, FString>& A, const TMap<FString, FString>& B)
{
    if (A.Num() != B.Num())
        return false;
    for (const auto& Pair : A)
    {
        const auto* Value = B.Find(Pair.Key);
        if (!Value || *Value != Pair.Value)
            return false;
    }
    return true;
}
bool TypeValid(const FDasDeclaredType& Type, const TSet<FString>& Classes, bool bVoid, FString& Error, int32 Depth = 0)
{
    if (Depth > 16)
        return Fail(Error, TEXT("Declaration type nesting limit exceeded"));
    const FString& K = Type.Kind;
    if (!Type.AuthoredId.IsEmpty() && (K != TEXT("object") || !StableId(Type.AuthoredId)))
        return Fail(Error, TEXT("Invalid authored object identity"));
    if (K == TEXT("array") || K == TEXT("set"))
    {
        if (!Type.Element || Type.Key || Type.Value || !Type.TypePath.IsEmpty())
            return Fail(Error, TEXT("Invalid element type shape"));
        return TypeValid(*Type.Element, Classes, false, Error, Depth + 1);
    }
    if (K == TEXT("map"))
    {
        if (!Type.Key || !Type.Value || Type.Element || !Type.TypePath.IsEmpty())
            return Fail(Error, TEXT("Invalid map type shape"));
        if (Type.Key->Kind !=
            TEXT("byte") && Type.Key->Kind !=
                                TEXT("int") && Type.Key->Kind !=
                                                   TEXT("float") &&
                                                        Type.Key->Kind !=
                                                            TEXT("double") &&
                                                                 Type.Key->Kind !=
                                                                     TEXT("bool") &&
                                                                          Type.Key->Kind !=
                                                                              TEXT("string") &&
                                                                                   Type.Key->Kind !=
                                                                                       TEXT("name") &&
                                                                                            Type.Key->Kind !=
                                                                                                TEXT("enum") &&
                                                                                                     Type.Key->Kind !=
                                                                                                         TEXT("object"))
            return Fail(Error, TEXT("Map key requires a supported scalar identity"));
        return TypeValid(*Type.Key, Classes, false, Error, Depth + 1) &&
               TypeValid(*Type.Value, Classes, false, Error, Depth + 1);
    }
    if (!Type.DelegateWire.IsEmpty() &&
        (K != TEXT("delegate") || Type.DelegateWire.Len() > 131072 ||
                   Type.TypePath != TEXT("/DasDelegates/")+ FMD5::HashAnsiString(*Type.DelegateWire) + TEXT(".Type")))
        return Fail(Error, TEXT("Delegate type/signature identity mismatch"));
    if (Type.Element || Type.Key || Type.Value)
        return Fail(Error, TEXT("Scalar/reference type has container members"));
    if (K == TEXT("object") || K == TEXT("enum") || K == TEXT("struct") ||
                                                              (K == TEXT("event") || K == TEXT("delegate")) ||
                                                               K == TEXT("class"))
        return (NativePath(Type.TypePath) || (K == TEXT("object") && Classes.Contains(Type.TypePath))) ||
                Fail(Error, TEXT("Missing type identity: ") + Type.TypePath);
    if (!Type.TypePath.IsEmpty())
        return Fail(Error, TEXT("Primitive type has a type_path"));
    if (K == TEXT("byte") ||
                  K == TEXT("text") ||
                            K == TEXT("int") ||
                                      K == TEXT("float") ||
                                                K == TEXT("double") ||
                                                          K == TEXT("bool") ||
                                                                    K == TEXT("string") ||
                                                                              K == TEXT("name") ||
                                                                                        (bVoid && K == TEXT("void")))
        return true;
    return Fail(Error, TEXT("Unsupported declaration type: ") + K);
}
bool DefaultValid(const FDasDeclaredProperty& P, FString& Error)
{
    const auto& D = P.Default;
    if (auto* Carrier = DasContainerCarrier(P.Type))
    {
        if (D.Kind.IsEmpty())
            return true;
        FInstancedStruct Value;
        return (D.Kind == TEXT("string") && ParseDasStructDefault(Carrier->GetPathName(), D.String, Value, Error)) ||
                Fail(Error, TEXT("Invalid container default: ")+ Error);
    }
    if (D.Kind.IsEmpty())
        return true;
    const auto& K = P.Type.Kind;
    if (K == TEXT("text"))
    {
        FText Value;
        return D.Kind == TEXT("string") && ParseDasTextDefault(D.String, Value, Error);
    }
    if (K == TEXT("byte"))
        return D.Kind == TEXT("number") && FMath::IsFinite(D.Number) && D.Number >= 0 && D.Number <= 255 &&
                              FMath::FloorToDouble(D.Number) == D.Number;
    if (D.Kind == TEXT("number") && FMath::IsFinite(D.Number))
    {
        if (K == TEXT("int") && D.Number >= MIN_int32 && D.Number <= MAX_int32 &&
                      FMath::FloorToDouble(D.Number) == D.Number)
            return true;
        if (K == TEXT("double"))
            return true;
        if (K == TEXT("float") && FMath::IsFinite(static_cast<float>(D.Number)) &&
                      (D.Number == 0 || static_cast<float>(D.Number) != 0))
            return true;
    }
    if (K == TEXT("bool") && D.Kind == TEXT("bool"))
        return true;
    if ((K == TEXT("string") || K == TEXT("name") || K == TEXT("enum")) &&
                                          D.Kind == TEXT("string") &&
                                                         (K != TEXT("name") || D.String.Len() < NAME_SIZE) &&
                                                          (K != TEXT("enum") || Identifier(D.String)))
        return true;
    if (K == TEXT("struct") && D.Kind == TEXT("string"))
    {
        FInstancedStruct Value;
        return ParseDasStructDefault(P.Type.TypePath, D.String, Value, Error);
    }
    if ((K == TEXT("object") || K == TEXT("class")) && D.Kind == TEXT("null"))
        return true;
    if ((K == TEXT("object") || K == TEXT("class")) && D.Kind == TEXT("string") && !D.String.IsEmpty())
    {
        if (!P.Type.AuthoredId.IsEmpty())
            return Fail(Error, TEXT("Non-null authored object defaults require a separate identity contract"));
        UObject* Value = nullptr;
        return ResolveDasReferenceDefault(K == TEXT("class"), P.Type.TypePath, D.String, Value, Error);
    }
    return Fail(Error, TEXT("Invalid default for property ") + P.Name);
}
bool SameDefault(const FDasDeclaredLiteral& A, const FDasDeclaredLiteral& B)
{
    if (A.Kind != B.Kind)
        return false;
    if (A.Kind == TEXT("number"))
        return A.Number == B.Number;
    if (A.Kind == TEXT("bool"))
        return A.Boolean == B.Boolean;
    if (A.Kind == TEXT("string"))
        return A.String == B.String;
    return true;
}
bool Keys(const TSharedPtr<FJsonObject>& J, std::initializer_list<const TCHAR*> Allowed, FString& Error)
{
    for (const auto& Pair : J->Values)
    {
        bool Found = false;
        for (const auto* K : Allowed)
            if (Pair.Key == K)
                Found = true;
        if (!Found)
            return Fail(Error, TEXT("Unknown authoring key: ") + FString(Pair.Key));
    }
    return true;
}
bool String(const TSharedPtr<FJsonObject>& J, const TCHAR* Key, FString& Out, FString& Error)
{
    return (J->HasTypedField<EJson::String>(Key) && J->TryGetStringField(Key, Out)) ||
           Fail(Error, FString(Key) + TEXT(" must be a string"));
}
bool Boolean(const TSharedPtr<FJsonObject>& J, const TCHAR* Key, bool& Out, FString& Error)
{
    if (!J->HasField(Key))
        return true;
    return (J->HasTypedField<EJson::Boolean>(Key) && J->TryGetBoolField(Key, Out)) ||
           Fail(Error, FString(Key) + TEXT(" must be boolean"));
}
bool Meta(const TSharedPtr<FJsonObject>& J, TMap<FString, FString>& Out, FString& Error)
{
    if (!J->HasField(TEXT("metadata")))
        return true;
    const TSharedPtr<FJsonObject>* M;
    if (!J->TryGetObjectField(TEXT("metadata"), M))
        return Fail(Error, TEXT("metadata must be an object"));
    for (const auto& Pair : (*M)->Values)
    {
        FString Value;
        if (Pair.Value->Type != EJson::String || !Pair.Value->TryGetString(Value))
            return Fail(Error, TEXT("metadata values must be strings"));
        Out.Add(FString(Pair.Key), Value);
    }
    return true;
}
bool ReadType(const TSharedPtr<FJsonObject>& J, FDasDeclaredType& Out, FString& Error, int32 Depth = 0)
{
    if (Depth > 16)
        return Fail(Error, TEXT("Declaration type nesting limit exceeded"));
    if (!Keys(J, {TEXT("kind"), TEXT("type_path"), TEXT("authored_id"), TEXT("delegate_wire"),
                                                                             TEXT("element"),
                                                                                  TEXT("key"), TEXT("value")}, Error) ||
                                                                             !String(J, TEXT("kind"), Out.Kind, Error))
        return false;
    if (J->HasField(TEXT("type_path")) && !String(J, TEXT("type_path"), Out.TypePath, Error))
        return false;
    if (J->HasField(TEXT("authored_id")) && !String(J, TEXT("authored_id"), Out.AuthoredId, Error))
        return false;
    if (J->HasField(TEXT("delegate_wire")) && !String(J, TEXT("delegate_wire"), Out.DelegateWire, Error))
        return false;
    for (const auto* Name : {TEXT("element"), TEXT("key"), TEXT("value")})
        if (J->HasField(Name))
        {
            const TSharedPtr<FJsonObject>* Child;
            if (!J->TryGetObjectField(Name, Child))
                return Fail(Error, TEXT("Type child must be an object"));
            auto T = MakeShared<FDasDeclaredType>();
            if (!ReadType(*Child, *T, Error, Depth + 1))
                return false;
            if (FString(Name) == TEXT("element"))
                Out.Element = T;
            else if (FString(Name) == TEXT("key"))
                Out.Key = T;
            else
                Out.Value = T;
        }
    return true;
}
bool Type(const TSharedPtr<FJsonObject>& J, const TCHAR* Key, FDasDeclaredType& Out, FString& Error)
{
    const TSharedPtr<FJsonObject>* T;
    return J->TryGetObjectField(Key, T) ? ReadType(*T, Out, Error)
                                        : Fail(Error, FString(Key) + TEXT(" must be a type object"));
}
bool Objects(const TSharedPtr<FJsonObject>& J, const TCHAR* Key, TArray<TSharedPtr<FJsonObject>>& Out, FString& Error)
{
    const TArray<TSharedPtr<FJsonValue>>* Values;
    if (!J->TryGetArrayField(Key, Values))
        return Fail(Error, FString(Key) + TEXT(" must be an array"));
    if (Values->Num() > 1024)
        return Fail(Error, TEXT("Declaration count limit exceeded"));
    for (const auto& V : *Values)
    {
        const TSharedPtr<FJsonObject>* O;
        if (!V->TryGetObject(O))
            return Fail(Error, TEXT("Expected declaration object"));
        Out.Add(*O);
    }
    return true;
}
TSharedPtr<FJsonObject> TypeJson(const FDasDeclaredType& T, int32 Depth = 0)
{
    auto J = MakeShared<FJsonObject>();
    if (Depth > 16)
    {
        J->SetStringField(TEXT("kind"), TEXT("invalid_depth"));
        return J;
    }
    J->SetStringField(TEXT("kind"), T.Kind);
    if (!T.TypePath.IsEmpty())
        J->SetStringField(TEXT("type_path"), T.TypePath);
    if (!T.AuthoredId.IsEmpty())
        J->SetStringField(TEXT("authored_id"), T.AuthoredId);
    if (!T.DelegateWire.IsEmpty())
        J->SetStringField(TEXT("delegate_wire"), T.DelegateWire);
    if (T.Element)
        J->SetObjectField(TEXT("element"), TypeJson(*T.Element, Depth + 1));
    if (T.Key)
        J->SetObjectField(TEXT("key"), TypeJson(*T.Key, Depth + 1));
    if (T.Value)
        J->SetObjectField(TEXT("value"), TypeJson(*T.Value, Depth + 1));
    return J;
}
void MetadataJson(const TMap<FString, FString>& M, const TSharedPtr<FJsonObject>& J)
{
    auto O = MakeShared<FJsonObject>();
    TArray<FString> K;
    M.GetKeys(K);
    K.Sort();
    for (const auto& Key : K)
        O->SetStringField(Key, M[Key]);
    J->SetObjectField(TEXT("metadata"), O);
}
}

FString FDasDeclaredType::Identity() const
{
    FString S;
    FJsonSerializer::Serialize(TypeJson(*this),
                               TJsonWriterFactory<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>::Create(&S));
    return S;
}
#include "DasComponentDefaultsWire.inl"

bool FDasAuthoringModule::Validate(FString& Error) const
{
    Error.Reset();
    if (!StableId(Id) || Classes.IsEmpty() || Classes.Num() > 1024)
        return Fail(Error, TEXT("Expected module ID and 1..1024 classes"));
    TSet<FString> Ids, ClassIds, ClassNames;
    Ids.Add(Id);
    auto AddId = [&](const FString& Value)
    {
        if (!StableId(Value) || Ids.Contains(Value))
            return Fail(Error, TEXT("Invalid or duplicate stable ID: ")+ Value);
        Ids.Add(Value);
        return true;
    };
    for (const auto& C : Classes)
    {
        if (!AddId(C.Id) || !Identifier(C.Name) || ClassNames.Contains(C.Name.ToLower()))
            return Fail(Error, TEXT("Invalid/duplicate class identity or name: ")+ C.Name);
        ClassIds.Add(C.Id);
        ClassNames.Add(C.Name.ToLower());
    }
    for (const auto& C : Classes)
    {
        if (C.NativeParent.IsEmpty() == C.ParentId.IsEmpty() ||
            (!C.NativeParent.IsEmpty() && !NativePath(C.NativeParent)) ||
            (!C.ParentId.IsEmpty() && !ClassIds.Contains(C.ParentId)))
            return Fail(Error, TEXT("Expected one valid parent for ")+ C.Id);
        if (C.NativeParent == TEXT("/Script/Engine.BlueprintFunctionLibrary"))
        {
            if (C.AsyncPhysics.IsSet() || !C.Properties.IsEmpty() || !C.Components.IsEmpty() ||
                !C.Callbacks.IsEmpty() || !C.Interfaces.IsEmpty() || !C.PropertyOverrides.IsEmpty() ||
                !C.ComponentOverrides.IsEmpty() || C.Methods.IsEmpty())
                return Fail(Error, TEXT("Function libraries require static methods only"));
            for (const auto& M : C.Methods)
                if (!M.bStatic || M.bConst || M.bBlueprintEvent)
                    return Fail(Error, TEXT("Library methods must be static, non-event and not const"));
        }
        if (!C.LibraryTypeRegistry.IsEmpty() &&
            (C.NativeParent != TEXT("/Script/Engine.BlueprintFunctionLibrary") || !NativePath(C.LibraryTypeRegistry)))
            return Fail(Error, TEXT("Invalid library type registry"));
        if (!C.SourceScript.IsEmpty() && (FPaths::GetCleanFilename(C.SourceScript) != C.SourceScript ||
                                          !C.SourceScript.EndsWith(TEXT(".das")) || C.SourceScript.Contains(TEXT(":"))))
            return Fail(Error, TEXT("Invalid authored source filename"));
        if (C.Interfaces.Num() > 16)
            return Fail(Error, TEXT("Interface count limit exceeded"));
        TSet<FString> SeenInterfaces;
        for (const auto& I : C.Interfaces)
        {
            if ((!I.StartsWith(TEXT("/Script/")) && !I.StartsWith(TEXT("/Game/"))) || !NativePath(I) ||
                  SeenInterfaces.Contains(I))
                return Fail(Error, TEXT("Invalid/duplicate native interface path"));
            SeenInterfaces.Add(I);
        }
        if (!MetadataValid(C.Metadata, Error, true))
            return false;
        if (C.Properties.Num() > 1024 || C.Methods.Num() > 1024)
            return Fail(Error, TEXT("Declaration count limit exceeded"));
        TSet<FString> Names;
        if (C.Components.Num() > 32 ||
            (!C.Components.IsEmpty() && !C.NativeParent.IsEmpty() && !DasActorNativeParent(C.NativeParent)))
            return Fail(Error, TEXT("Components require native Actor parent and at most 32 declarations"));
        for (const auto& Component : C.Components)
        {
            if (!AddId(Component.Id) || !Identifier(Component.Name) || Names.Contains(Component.Name.ToLower()) ||
                (Component.NativeClass.IsEmpty()
                 ? (!Identifier(Component.ClassName) || Component.ScriptFile.IsEmpty() ||
                    FPaths::GetCleanFilename(Component.ScriptFile) != Component.ScriptFile ||
                    Component.ScriptFile.Contains(TEXT(":")) || !Component.ScriptFile.EndsWith(TEXT(".das")))
                    : (!Component.NativeClass.StartsWith(TEXT("/Script/")) || !NativePath(Component.NativeClass) ||
                                                         !Component.ScriptFile.IsEmpty() ||
                                                         !Component.ClassName.IsEmpty())))
                return Fail(Error, TEXT("Invalid/duplicate component declaration: ")+ Component.Name);
            Names.Add(Component.Name.ToLower());
            if ((Component.bEditable && Component.bVisible) || !MetadataValid(Component.Metadata, Error))
                return Fail(Error, TEXT("Invalid component metadata/visibility"));
        }
        for (const auto& Component : C.Components)
            if (!ValidateComponentDefaults(Component, Error))
                return false;
        for (const auto& P : C.Properties)
        {
            if (!AddId(P.Id) || !Identifier(P.Name) || Names.Contains(P.Name.ToLower()))
                return Fail(Error, TEXT("Invalid or duplicate property: ")+ P.Name);
            if (P.Type.Kind == TEXT("event") || P.Type.Kind == TEXT("delegate"))
            {
                if (P.DelegateWire.IsEmpty() || P.DelegateWire.Len() > 131072 ||
                    P.Type.TypePath != TEXT("/DasDelegates/")+ FMD5::HashAnsiString(*P.DelegateWire) +
                                            TEXT(".Type") || !P.Default.Kind.IsEmpty() ||
                                                 (P.Type.Kind == TEXT("event") && (P.bEditable || P.bReadOnly)))
                    return Fail(Error, TEXT("Invalid authored delegate signature/default/flags"));
            }
            else if (!P.DelegateWire.IsEmpty())
                return Fail(Error, TEXT("Delegate signature is only valid on event fields"));
            Names.Add(P.Name.ToLower());
            if (!TypeValid(P.Type, ClassIds, false, Error) || !DefaultValid(P, Error) ||
                !MetadataValid(P.Metadata, Error, false, true))
                return false;
        }
        if (C.AsyncPhysics)
        {
            const auto& A = *C.AsyncPhysics;
            if (A.ScriptFile.IsEmpty() || FPaths::GetCleanFilename(A.ScriptFile) != A.ScriptFile ||
                !A.ScriptFile.EndsWith(TEXT(".das")) || A.ScriptFile.Contains(TEXT(":")) || A.Bodies.Num() > 8)
                return Fail(Error, TEXT("Invalid async physics script or binding count"));
            TSet<FString> BodyNames;
            for (const auto& B : A.Bodies)
            {
                FString Name = B.Value;
                if (Name.StartsWith(TEXT("owner.")))
                    Name.RightChopInline(6);
                if (B.Key < 0 || B.Key > 7 || !Identifier(Name) || BodyNames.Contains(B.Value.ToLower()))
                    return Fail(Error, TEXT("Invalid/duplicate async physics body binding"));
                BodyNames.Add(B.Value.ToLower());
            }
        }
        if (C.Callbacks.Num() > 64)
            return Fail(Error, TEXT("Callback limit exceeded"));
        TSet<FString> LifecycleKinds;
        for (const auto& Callback : C.Callbacks)
        {
            if (!Identifier(Callback.Name) || Names.Contains(Callback.Name.ToLower()))
                return Fail(Error, TEXT("Invalid/duplicate callback name"));
            Names.Add(Callback.Name.ToLower());
            if (Callback.Kind == TEXT("event"))
            {
                if (!FindDasCallbackType(Callback.ParameterKind))
                    return Fail(Error, TEXT("Unsupported event callback parameter kind"));
                if (Callback.ParameterKind ==
                    TEXT("class") || Callback.ParameterKind ==
                                         TEXT("enum") ||
                                              Callback.ParameterKind ==
                                                  TEXT("struct") ||
                                                       (Callback.ParameterKind ==
                                                        TEXT("object") && !Callback.ParameterTypePath.IsEmpty()))
                {
                    if (!ValidateDasCallbackTypePath(Callback.ParameterKind, Callback.ParameterTypePath, Error))
                        return false;
                }
                else if (!Callback.ParameterTypePath.IsEmpty())
                    return Fail(Error, TEXT("Unexpected callback type identity"));
                if ((Callback.Source !=
                     TEXT("self") && Callback.Source != TEXT("owner") &&
                                                             !(Identifier(Callback.Source) &&
                                                               (C.Components.ContainsByPredicate(
                                                                    [&](const auto& Component)
                                                                    { return Component.Name == Callback.Source; }) ||
                                                                !C.ParentId.IsEmpty()))) ||
                     !Identifier(Callback.EventName) ||
                     (Callback.Source == TEXT("owner") && !C.NativeParent.IsEmpty() &&
                                              C.NativeParent != TEXT("/Script/Engine.ActorComponent")))
                    return Fail(Error, TEXT("Invalid event callback source/name"));
            }
            else
            {
                if ((Callback.Kind !=
                     TEXT("ConstructionScript") &&
                          Callback.Kind !=
                              TEXT("Initialize") &&
                                   Callback.Kind !=
                                       TEXT("BeginPlay") &&
                                            Callback.Kind != TEXT("Tick") && Callback.Kind != TEXT("EndPlay")) ||
                                                !Callback.Source.IsEmpty() || !Callback.EventName.IsEmpty() ||
                                                LifecycleKinds.Contains(Callback.Kind))
                    return Fail(Error, TEXT("Invalid/duplicate lifecycle callback"));
                LifecycleKinds.Add(Callback.Kind);
            }
        }
        for (const auto& M : C.Methods)
        {
            if (!AddId(M.Id) || !Identifier(M.Name) || Names.Contains(M.Name.ToLower()))
                return Fail(Error, TEXT("Invalid or duplicate method: ")+ M.Name);
            Names.Add(M.Name.ToLower());
            if (M.bPure &&
                (!M.bCallable ||
                 (M.ReturnType.Kind ==
                  TEXT("void") &&
                       !M.Parameters.ContainsByPredicate(
                           [](const auto& P) { return P.Direction == TEXT("out") || P.Direction == TEXT("inout"); }))))
                return Fail(Error, TEXT("BlueprintPure requires callable function with a result or output parameter: ")+
                                        M.Name);
            if (!TypeValid(M.ReturnType, ClassIds, true, Error) || !MetadataValid(M.Metadata, Error))
                return false;
            if (!Identifier(M.EntryPoint) || M.ScriptFile.IsEmpty() ||
                FPaths::GetCleanFilename(M.ScriptFile) != M.ScriptFile ||
                M.ScriptFile.Contains(TEXT(":")) || !M.ScriptFile.EndsWith(TEXT(".das")))
                return Fail(Error, TEXT("Expected local script filename and entry point: ")+ M.Id);
            if (!M.WorldContext.IsEmpty() &&
                (!M.bStatic || C.NativeParent != TEXT("/Script/Engine.BlueprintFunctionLibrary") ||
                                                      !M.Parameters.ContainsByPredicate(
                                                          [&](const auto& P)
                                                          {
                                                              return P.Name == M.WorldContext &&
                                                                     P.Direction == TEXT("in") &&
                                                                                         P.Type.Kind == TEXT("object");
                                                          })))
                return Fail(Error, TEXT("WorldContext must name an input object of a static library"));
            if (M.Parameters.Num() > 64)
                return Fail(Error, TEXT("Parameter count limit exceeded"));
            TSet<FString> Parameters;
            bool SawParameterDefault = false;
            for (const auto& P : M.Parameters)
            {
                if (!AddId(P.Id) || !Identifier(P.Name) || Parameters.Contains(P.Name.ToLower()) ||
                    (P.Direction != TEXT("in") && P.Direction != TEXT("out") && P.Direction != TEXT("inout")))
                    return Fail(Error, TEXT("Invalid parameter: ")+ P.Name);
                Parameters.Add(P.Name.ToLower());
                if (!TypeValid(P.Type, ClassIds, false, Error))
                    return false;
                if (!P.Default.Kind.IsEmpty())
                {
                    if (P.Direction !=
                        TEXT("in") || P.Name == M.WorldContext ||
                            (P.Type.Kind !=
                             TEXT("byte") &&
                                  P.Type.Kind != TEXT("text") &&
                                                      P.Type.Kind !=
                                                          TEXT("int") && P.Type.Kind !=
                                                                             TEXT("float") &&
                                                                                  P.Type.Kind !=
                                                                                      TEXT("double") &&
                                                                                           P.Type.Kind !=
                                                                                               TEXT("bool") &&
                                                                                                    P.Type.Kind !=
                                                                                                        TEXT("string")))
                        return Fail(Error, TEXT("Parameter default requires a scalar input: ")+ P.Name);
                    FDasDeclaredProperty Value;
                    Value.Name = P.Name;
                    Value.Type = P.Type;
                    Value.Default = P.Default;
                    if (P.Default.Kind == TEXT("string"))
                    {
                        if (P.Default.String.Len() > 4096)
                            return Fail(Error, TEXT("Parameter string default exceeds supported limits"));
                        for (int32 I = 0; I < P.Default.String.Len(); ++I)
                            if (P.Default.String[I] == 0)
                                return Fail(Error, TEXT("Parameter string default contains NUL"));
                    }
                    if (!DefaultValid(Value, Error))
                        return false;
                    SawParameterDefault = true;
                }
                else if (SawParameterDefault)
                    return Fail(Error, TEXT("Required parameter follows a default parameter: ")+ P.Name);
            }
        }
    }
    // Inheritance dependencies are acyclic; object references may legitimately point to self.
    for (const auto& C : Classes)
    {
        FDasDeclaredClass Effective;
        if (!ResolveClass(C.Id, Effective, Error))
            return false;
    }
    return true;
}
bool FDasAuthoringModule::FromJson(const FString& Json, FDasAuthoringModule& Out, FString& Error)
{
    Error.Reset();
    if (FTCHARToUTF8(*Json).Length() > 1024 * 1024)
        return Fail(Error, TEXT("Authoring JSON exceeds 1 MiB"));
    TSharedPtr<FJsonObject> Root;
    if (!FJsonSerializer::Deserialize(TJsonReaderFactory<>::Create(Json), Root) || !Root)
        return Fail(Error, TEXT("Invalid authoring JSON"));
    if (!Keys(Root, {TEXT("schema_version"), TEXT("module_id"), TEXT("classes")}, Error))
        return false;
    if (!Root->HasTypedField<EJson::Number>(TEXT("schema_version")) ||
                                            Root->GetNumberField(TEXT("schema_version")) != 1)
        return Fail(Error, TEXT("Expected authoring schema_version 1"));
    FDasAuthoringModule Candidate;
    if (!String(Root, TEXT("module_id"), Candidate.Id, Error))
        return false;
    TArray<TSharedPtr<FJsonObject>> ClassesJson;
    if (!Objects(Root, TEXT("classes"), ClassesJson, Error))
        return false;
    for (const auto& J : ClassesJson)
    {
        FDasDeclaredClass C;
        if (!Keys(J, {TEXT("id"),
                           TEXT("name"),
                                TEXT("native_parent"),
                                     TEXT("parent_id"),
                                          TEXT("source_script"),
                                               TEXT("library_type_registry"),
                                                    TEXT("widget_layouts"),
                                                         TEXT("async_physics"),
                                                              TEXT("interfaces"),
                                                                   TEXT("components"),
                                                                        TEXT("component_overrides"),
                                                                             TEXT("property_overrides"),
                                                                                  TEXT("native_defaults"),
                                                                                       TEXT("callbacks"),
                                                                                            TEXT("properties"),
                                                                                                 TEXT("methods"),
                                                                                                      TEXT("metadata")},
                                                                                                     Error) ||
                                                                                            !String(
                                                                                                J, TEXT("id"), C.Id,
                                                                                                        Error) ||
                                                                                                !String(
                                                                                                    J, TEXT("name"),
                                                                                                            C.Name,
                                                                                                            Error) ||
                                                                                                    !Meta(J, C.Metadata,
                                                                                                          Error))
            return false;
        if (J->HasField(TEXT("widget_layouts")))
        {
            const TArray<TSharedPtr<FJsonValue>>* Layouts = nullptr;
            if (!J->TryGetArrayField(TEXT("widget_layouts"), Layouts) || Layouts->Num() > 64)
                return Fail(Error, TEXT("Expected at most 64 widget layouts"));
            for (const auto& V : *Layouts)
            {
                if (V->Type != EJson::Object)
                    return Fail(Error, TEXT("Expected widget layout object"));
                FString Spec;
                FJsonSerializer::Serialize(V->AsObject().ToSharedRef(), TJsonWriterFactory<>::Create(&Spec));
                FDasWidgetLayoutUse L;
                if (!ParseDasWidgetLayout(Spec, L, Error))
                    return false;
                C.WidgetLayouts.Add(MoveTemp(L));
            }
        }
        if (J->HasField(TEXT("native_parent")) && !String(J, TEXT("native_parent"), C.NativeParent, Error))
            return false;
        if (J->HasField(TEXT("parent_id")) && !String(J, TEXT("parent_id"), C.ParentId, Error))
            return false;
        if (J->HasField(TEXT("library_type_registry")) &&
                        !String(J, TEXT("library_type_registry"), C.LibraryTypeRegistry, Error))
            return false;
        if (J->HasField(TEXT("source_script")) && !String(J, TEXT("source_script"), C.SourceScript, Error))
            return false;
        if (J->HasField(TEXT("async_physics")))
        {
            const TSharedPtr<FJsonObject>* A = nullptr;
            if (!J->TryGetObjectField(TEXT("async_physics"), A) || !Keys(*A, {TEXT("script"), TEXT("bodies")}, Error))
                return Fail(Error, TEXT("Expected async_physics object"));
            FDasDeclaredAsyncPhysics Spec;
            if (!String(*A, TEXT("script"), Spec.ScriptFile, Error))
                return false;
            TArray<TSharedPtr<FJsonObject>> Items;
            if (!Objects(*A, TEXT("bodies"), Items, Error))
                return false;
            for (const auto& B : Items)
            {
                double Slot;
                FString Name;
                if (!Keys(B, {TEXT("slot"), TEXT("component")}, Error) ||
                          !B->TryGetNumberField(TEXT("slot"), Slot) || !FMath::IsFinite(Slot) || Slot < 0 || Slot > 7 ||
                                                Slot != FMath::FloorToDouble(Slot) ||
                                                !String(B, TEXT("component"), Name, Error) ||
                                                        Spec.Bodies.Contains(int32(Slot)))
                    return Fail(Error, TEXT("Invalid/duplicate async physics body slot"));
                Spec.Bodies.Add(int32(Slot), Name);
            }
            C.AsyncPhysics = MoveTemp(Spec);
        }
        if (J->HasField(TEXT("interfaces")))
        {
            const TArray<TSharedPtr<FJsonValue>>* Items = nullptr;
            if (!J->TryGetArrayField(TEXT("interfaces"), Items))
                return Fail(Error, TEXT("Expected interfaces array"));
            for (const auto& V : *Items)
            {
                FString I;
                if (!V->TryGetString(I))
                    return Fail(Error, TEXT("Expected interface path string"));
                C.Interfaces.Add(I);
            }
        }
        if (J->HasField(TEXT("components")))
        {
            TArray<TSharedPtr<FJsonObject>> Items;
            if (!Objects(J, TEXT("components"), Items, Error))
                return false;
            for (const auto& Item : Items)
            {
                FDasDeclaredComponent Component;
                if (!Keys(
                        Item,
                        {TEXT("id"),
                              TEXT("name"),
                                   TEXT("script_file"),
                                        TEXT("class_name"),
                                             TEXT("native_class"),
                                                  TEXT("defaults"),
                                                       TEXT("attach"),
                                                            TEXT("attach_socket"),
                                                                 TEXT("root"),
                                                                      TEXT("read_only"),
                                                                           TEXT("editable"),
                                                                                TEXT("visible"), TEXT("metadata")},
                                                                                    Error) ||
                                                                           !String(Item,
                                                                                   TEXT("id"), Component.Id, Error) ||
                                                                                   !String(
                                                                                       Item,
                                                                                       TEXT("name"), Component.Name,
                                                                                            Error) ||
                                                                                       !String(
                                                                                           Item,
                                                                                           TEXT("script_file"),
                                                                                                Component.ScriptFile,
                                                                                                Error) ||
                                                                                           !String(
                                                                                               Item,
                                                                                               TEXT("class_name"),
                                                                                                    Component.ClassName,
                                                                                                    Error))
                    return false;
                if (!ReadComponentDefaults(Item, Component.Defaults, Error))
                    return false;
                if (Item->HasField(TEXT("attach")) && !String(Item, TEXT("attach"), Component.Attach, Error))
                    return false;
                if (Item->HasField(TEXT("attach_socket")) &&
                                   !String(Item, TEXT("attach_socket"), Component.AttachSocket, Error))
                    return false;
                if (Item->HasField(TEXT("root")) && !Boolean(Item, TEXT("root"), Component.bRoot, Error))
                    return false;
                if (Item->HasField(TEXT("native_class")) &&
                                   !String(Item, TEXT("native_class"), Component.NativeClass, Error))
                    return false;
                if (Item->HasField(TEXT("read_only")) && !Boolean(Item, TEXT("read_only"), Component.bReadOnly, Error))
                    return false;
                if (Item->HasField(TEXT("editable")) && !Boolean(Item, TEXT("editable"), Component.bEditable, Error))
                    return false;
                if (Item->HasField(TEXT("visible")) && !Boolean(Item, TEXT("visible"), Component.bVisible, Error))
                    return false;
                if (!Meta(Item, Component.Metadata, Error))
                    return false;
                C.Components.Add(Component);
            }
        }
        if (J->HasField(TEXT("property_overrides")))
        {
            TArray<TSharedPtr<FJsonObject>> Items;
            if (!Objects(J, TEXT("property_overrides"), Items, Error))
                return false;
            for (const auto& Item : Items)
            {
                FDasPropertyDefaultOverride O;
                if (!Keys(Item, {TEXT("id"), TEXT("name"), TEXT("default")}, Error) ||
                                 !String(Item, TEXT("id"), O.Id, Error) || !String(Item, TEXT("name"), O.Name, Error))
                    return false;
                const auto* Value = Item->Values.Find(TEXT("default"));
                if (!Value)
                    return Fail(Error, TEXT("Inherited override needs explicit default"));
                switch ((*Value)->Type)
                {
                case EJson::Null:
                    O.Default.Kind = TEXT("null");
                    break;
                case EJson::Boolean:
                    O.Default.Kind = TEXT("bool");
                    O.Default.Boolean = (*Value)->AsBool();
                    break;
                case EJson::Number:
                    O.Default.Kind = TEXT("number");
                    O.Default.Number = (*Value)->AsNumber();
                    break;
                case EJson::String:
                    O.Default.Kind = TEXT("string");
                    O.Default.String = (*Value)->AsString();
                    break;
                default:
                    return Fail(Error, TEXT("Inherited default must be a literal"));
                }
                C.PropertyOverrides.Add(O);
            }
        }
        if (J->HasField(TEXT("component_overrides")))
        {
            TArray<TSharedPtr<FJsonObject>> Items;
            if (!Objects(J, TEXT("component_overrides"), Items, Error))
                return false;
            for (const auto& Item : Items)
            {
                FDasComponentOverride O;
                if (!Keys(Item, {TEXT("id"), TEXT("name"), TEXT("defaults")}, Error) ||
                                 !String(Item, TEXT("id"), O.Id, Error) ||
                                         !String(Item, TEXT("name"), O.Name, Error) ||
                                                 !ReadComponentDefaults(Item, O.Defaults, Error))
                    return false;
                C.ComponentOverrides.Add(O);
            }
        }
        if (J->HasField(TEXT("native_defaults")))
        {
            TArray<TSharedPtr<FJsonObject>> Items;
            if (!Objects(J, TEXT("native_defaults"), Items, Error))
                return false;
            for (const auto& Item : Items)
            {
                FDasNativeDefaultGroup G;
                if (!Keys(Item, {TEXT("component"), TEXT("defaults")}, Error) ||
                          !String(Item, TEXT("component"), G.Component, Error) ||
                                  !ReadComponentDefaults(Item, G.Defaults, Error))
                    return false;
                C.NativeDefaults.Add(MoveTemp(G));
            }
        }
        if (J->HasField(TEXT("callbacks")))
        {
            TArray<TSharedPtr<FJsonObject>> Items;
            if (!Objects(J, TEXT("callbacks"), Items, Error))
                return false;
            for (const auto& Item : Items)
            {
                FDasDeclaredCallback Callback;
                if (!Keys(Item, {TEXT("name"),
                                      TEXT("kind"),
                                           TEXT("source"), TEXT("event_name"), TEXT("parameter_kind"),
                                                                                    TEXT("parameter_type_path")},
                                                                                   Error) ||
                                                !String(Item, TEXT("name"), Callback.Name, Error) ||
                                                        !String(Item, TEXT("kind"), Callback.Kind, Error) ||
                                                                !String(Item, TEXT("source"), Callback.Source, Error) ||
                                                                        !String(Item, TEXT("event_name"),
                                                                                           Callback.EventName, Error))
                    return false;
                if (Item->HasField(
                        TEXT("parameter_kind")) &&
                        (Callback.Kind != TEXT("event") ||
                                               !String(Item, TEXT("parameter_kind"), Callback.ParameterKind, Error)))
                    return false;
                if (Item->HasField(TEXT("parameter_type_path")) &&
                                   (Callback.Kind !=
                                    TEXT("event") ||
                                         !String(Item, TEXT("parameter_type_path"), Callback.ParameterTypePath, Error)))
                    return false;
                C.Callbacks.Add(Callback);
            }
        }
        TArray<TSharedPtr<FJsonObject>> Ps, Ms;
        if (!Objects(J, TEXT("properties"), Ps, Error) || !Objects(J, TEXT("methods"), Ms, Error))
            return false;
        for (const auto& V : Ps)
        {
            FDasDeclaredProperty P;
            if (!Keys(V, {TEXT("id"),
                               TEXT("name"),
                                    TEXT("type"),
                                         TEXT("default"),
                                              TEXT("editable"), TEXT("read_only"),
                                                                     TEXT("metadata"), TEXT("delegate_wire")}, Error) ||
                                                   !String(V, TEXT("id"), P.Id, Error) ||
                                                           !String(V, TEXT("name"), P.Name, Error) ||
                                                                   !Type(V, TEXT("type"), P.Type, Error) ||
                                                                         !Meta(V, P.Metadata, Error) ||
                                                                         !Boolean(V, TEXT("editable"), P.bEditable,
                                                                                          Error) ||
                                                                                  !Boolean(V, TEXT("read_only"),
                                                                                                   P.bReadOnly, Error))
                return false;
            if (V->HasField(TEXT("delegate_wire")) && !String(V, TEXT("delegate_wire"), P.DelegateWire, Error))
                return false;
            if (const auto* D = V->Values.Find(TEXT("default")))
            {
                if ((*D)->Type == EJson::Null)
                    P.Default.Kind = TEXT("null");
                else if ((*D)->Type == EJson::Number)
                {
                    P.Default.Kind = TEXT("number");
                    P.Default.Number = (*D)->AsNumber();
                }
                else if ((*D)->Type == EJson::Boolean)
                {
                    P.Default.Kind = TEXT("bool");
                    P.Default.Boolean = (*D)->AsBool();
                }
                else if ((*D)->Type == EJson::String)
                {
                    P.Default.Kind = TEXT("string");
                    P.Default.String = (*D)->AsString();
                }
                else
                    return Fail(Error, TEXT("Property default must be a scalar literal"));
            }
            C.Properties.Add(MoveTemp(P));
        }
        for (const auto& V : Ms)
        {
            FDasDeclaredMethod M;
            if (!Keys(
                    V,
                    {TEXT("id"),
                        TEXT("name"),
                             TEXT("return_type"),
                                  TEXT("parameters"),
                                       TEXT("implementation"),
                                            TEXT("callable"),
                                                 TEXT("pure"),
                                                      TEXT("static"),
                                                           TEXT("const"),
                                                                TEXT("blueprint_event"),
                                                                     TEXT("world_context"),
                                                                          TEXT("metadata")},
                                                                         Error) ||
                                                                !String(
                                                                    V,
                                                                    TEXT("id"), M.Id, Error) ||
                                                                    !String(
                                                                        V,
                                                                        TEXT("name"), M.Name, Error) ||
                                                                        !Type(
                                                                            V,
                                                                            TEXT("return_type"), M.ReturnType, Error) ||
                                                                            !Meta(V, M.Metadata, Error) ||
                                                                            !Boolean(
                                                                                V,
                                                                                TEXT("callable"), M.bCallable, Error) ||
                                                                                !Boolean(
                                                                                    V, TEXT("pure"), M.bPure, Error) ||
                                                                                    !Boolean(
                                                                                        V,
                                                                                        TEXT("static"),
                                                                                             M.bStatic,
                                                                                             Error) ||
                                                                                        !Boolean(
                                                                                            V,
                                                                                            TEXT("const"),
                                                                                                 M.bConst,
                                                                                                 Error) ||
                                                                                            !Boolean(
                                                                                                V,
                                                                                                TEXT("blueprint_event"),
                                                                                                     M.bBlueprintEvent,
                                                                                                     Error))
                return false;
            if (V->HasField(TEXT("world_context")) && !String(V, TEXT("world_context"), M.WorldContext, Error))
                return false;
            const TSharedPtr<FJsonObject>* Impl;
            if (!V->TryGetObjectField(TEXT("implementation"), Impl) ||
                                      !Keys(*Impl, {TEXT("script_file"), TEXT("entry_point")}, Error) ||
                                            !String(*Impl, TEXT("script_file"), M.ScriptFile, Error) ||
                                                    !String(*Impl, TEXT("entry_point"), M.EntryPoint, Error))
                return Fail(Error, Error.IsEmpty() ? TEXT("Expected method implementation"): Error);
            TArray<TSharedPtr<FJsonObject>> Params;
            if (!Objects(V, TEXT("parameters"), Params, Error))
                return false;
            for (const auto& Q : Params)
            {
                FDasDeclaredParameter P;
                if (!Keys(Q, {TEXT("id"), TEXT("name"), TEXT("type"), TEXT("direction"), TEXT("default")}, Error) ||
                                               !String(Q, TEXT("id"), P.Id, Error) ||
                                                       !String(Q, TEXT("name"), P.Name, Error) ||
                                                               !String(Q, TEXT("direction"), P.Direction, Error) ||
                                                                       !Type(Q, TEXT("type"), P.Type, Error))
                    return false;
                if (Q->HasField(TEXT("default")))
                {
                    const auto Literal = Q->Values[TEXT("default")];
                    if (Literal->Type == EJson::Number)
                    {
                        P.Default.Kind = TEXT("number");
                        P.Default.Number = Literal->AsNumber();
                    }
                    else if (Literal->Type == EJson::Boolean)
                    {
                        P.Default.Kind = TEXT("bool");
                        P.Default.Boolean = Literal->AsBool();
                    }
                    else if (Literal->Type == EJson::String)
                    {
                        P.Default.Kind = TEXT("string");
                        P.Default.String = Literal->AsString();
                    }
                    else
                        return Fail(Error, TEXT("Expected scalar parameter default literal"));
                }
                M.Parameters.Add(MoveTemp(P));
            }
            C.Methods.Add(MoveTemp(M));
        }
        Candidate.Classes.Add(MoveTemp(C));
    }
    if (!Candidate.Validate(Error))
        return false;
    Out = MoveTemp(Candidate);
    return true;
}
bool FDasAuthoringModule::ToJson(FString& Out, FString& Error) const
{
    if (!Validate(Error))
        return false;
    auto Root = MakeShared<FJsonObject>();
    Root->SetNumberField(TEXT("schema_version"), 1);
    Root->SetStringField(TEXT("module_id"), Id);
    TArray<const FDasDeclaredClass*> Cs;
    for (const auto& C : Classes)
        Cs.Add(&C);
    Cs.Sort([](const auto& A, const auto& B) { return A.Id < B.Id; });
    TArray<TSharedPtr<FJsonValue>> Cj;
    for (const auto* C : Cs)
    {
        auto J = MakeShared<FJsonObject>();
        J->SetStringField(TEXT("id"), C->Id);
        J->SetStringField(TEXT("name"), C->Name);
        if (!C->PropertyOverrides.IsEmpty())
        {
            auto Sorted = C->PropertyOverrides;
            Sorted.Sort([](const auto& A, const auto& B) { return A.Id < B.Id; });
            TArray<TSharedPtr<FJsonValue>> Items;
            for (const auto& O : Sorted)
            {
                auto Item = MakeShared<FJsonObject>();
                Item->SetStringField(TEXT("id"), O.Id);
                Item->SetStringField(TEXT("name"), O.Name);
                const auto& D = O.Default;
                if (D.Kind == TEXT("number"))
                    Item->SetNumberField(TEXT("default"), D.Number);
                else if (D.Kind == TEXT("bool"))
                    Item->SetBoolField(TEXT("default"), D.Boolean);
                else if (D.Kind == TEXT("string"))
                    Item->SetStringField(TEXT("default"), D.String);
                else
                    Item->SetField(TEXT("default"), MakeShared<FJsonValueNull>());
                Items.Add(MakeShared<FJsonValueObject>(Item));
            }
            J->SetArrayField(TEXT("property_overrides"), Items);
        }
        if (!C->ComponentOverrides.IsEmpty())
        {
            TArray<FDasComponentOverride> Sorted = C->ComponentOverrides;
            Sorted.Sort([](const auto& A, const auto& B) { return A.Id < B.Id; });
            TArray<TSharedPtr<FJsonValue>> Items;
            for (const auto& O : Sorted)
            {
                auto Item = MakeShared<FJsonObject>();
                Item->SetStringField(TEXT("id"), O.Id);
                Item->SetStringField(TEXT("name"), O.Name);
                WriteComponentDefaults(O.Defaults, Item);
                Items.Add(MakeShared<FJsonValueObject>(Item));
            }
            J->SetArrayField(TEXT("component_overrides"), Items);
        }
        if (!C->NativeDefaults.IsEmpty())
        {
            auto Sorted = C->NativeDefaults;
            Sorted.Sort([](const auto& A, const auto& B) { return A.Component < B.Component; });
            TArray<TSharedPtr<FJsonValue>> Items;
            for (const auto& G : Sorted)
            {
                auto Item = MakeShared<FJsonObject>();
                Item->SetStringField(TEXT("component"), G.Component);
                WriteComponentDefaults(G.Defaults, Item);
                Items.Add(MakeShared<FJsonValueObject>(Item));
            }
            J->SetArrayField(TEXT("native_defaults"), Items);
        }
        if (!C->Callbacks.IsEmpty())
        {
            TArray<TSharedPtr<FJsonValue>> Items;
            for (const auto& Callback : C->Callbacks)
            {
                auto Item = MakeShared<FJsonObject>();
                Item->SetStringField(TEXT("name"), Callback.Name);
                Item->SetStringField(TEXT("kind"), Callback.Kind);
                Item->SetStringField(TEXT("source"), Callback.Source);
                Item->SetStringField(TEXT("event_name"), Callback.EventName);
                if (Callback.Kind == TEXT("event") && Callback.ParameterKind != TEXT("int"))
                    Item->SetStringField(TEXT("parameter_kind"), Callback.ParameterKind);
                if (!Callback.ParameterTypePath.IsEmpty())
                    Item->SetStringField(TEXT("parameter_type_path"), Callback.ParameterTypePath);
                Items.Add(MakeShared<FJsonValueObject>(Item));
            }
            J->SetArrayField(TEXT("callbacks"), Items);
        }
        if (!C->Components.IsEmpty())
        {
            TArray<FDasDeclaredComponent> Components = C->Components;
            Components.Sort([](const auto& A, const auto& B) { return A.Id < B.Id; });
            TArray<TSharedPtr<FJsonValue>> Items;
            for (const auto& Component : Components)
            {
                auto Item = MakeShared<FJsonObject>();
                Item->SetStringField(TEXT("id"), Component.Id);
                Item->SetStringField(TEXT("name"), Component.Name);
                Item->SetStringField(TEXT("script_file"), Component.ScriptFile);
                Item->SetStringField(TEXT("class_name"), Component.ClassName);
                WriteComponentDefaults(Component.Defaults, Item);
                if (!Component.Attach.IsEmpty())
                    Item->SetStringField(TEXT("attach"), Component.Attach);
                if (!Component.AttachSocket.IsEmpty())
                    Item->SetStringField(TEXT("attach_socket"), Component.AttachSocket);
                if (Component.bRoot)
                    Item->SetBoolField(TEXT("root"), true);
                if (!Component.NativeClass.IsEmpty())
                    Item->SetStringField(TEXT("native_class"), Component.NativeClass);
                if (Component.bReadOnly || Component.bEditable || Component.bVisible || !Component.Metadata.IsEmpty())
                {
                    Item->SetBoolField(TEXT("read_only"), Component.bReadOnly);
                    Item->SetBoolField(TEXT("editable"), Component.bEditable);
                    Item->SetBoolField(TEXT("visible"), Component.bVisible);
                    MetadataJson(Component.Metadata, Item);
                }
                Items.Add(MakeShared<FJsonValueObject>(Item));
            }
            J->SetArrayField(TEXT("components"), Items);
        }
        if (C->AsyncPhysics)
        {
            auto A = MakeShared<FJsonObject>();
            A->SetStringField(TEXT("script"), C->AsyncPhysics->ScriptFile);
            TArray<TSharedPtr<FJsonValue>> Bodies;
            for (int32 Slot = 0; Slot < 8; ++Slot)
                if (const auto* Name = C->AsyncPhysics->Bodies.Find(Slot))
                {
                    auto B = MakeShared<FJsonObject>();
                    B->SetNumberField(TEXT("slot"), Slot);
                    B->SetStringField(TEXT("component"), *Name);
                    Bodies.Add(MakeShared<FJsonValueObject>(B));
                }
            A->SetArrayField(TEXT("bodies"), Bodies);
            J->SetObjectField(TEXT("async_physics"), A);
        }
        J->SetStringField(C->ParentId.IsEmpty()
                          ? TEXT("native_parent")
                          : TEXT("parent_id"), C->ParentId.IsEmpty() ? C->NativeParent : C->ParentId);
        if (!C->WidgetLayouts.IsEmpty())
        {
            TArray<TSharedPtr<FJsonValue>> Layouts;
            for (const auto& L : C->WidgetLayouts)
            {
                TSharedPtr<FJsonObject> Item;
                FJsonSerializer::Deserialize(TJsonReaderFactory<>::Create(DasWidgetLayoutSpec(L)), Item);
                Layouts.Add(MakeShared<FJsonValueObject>(Item));
            }
            J->SetArrayField(TEXT("widget_layouts"), Layouts);
        }
        MetadataJson(C->Metadata, J);
        if (!C->LibraryTypeRegistry.IsEmpty())
            J->SetStringField(TEXT("library_type_registry"), C->LibraryTypeRegistry);
        if (!C->SourceScript.IsEmpty())
            J->SetStringField(TEXT("source_script"), C->SourceScript);
        TArray<const FDasDeclaredProperty*> Ps;
        for (const auto& P : C->Properties)
            Ps.Add(&P);
        Ps.Sort([](const auto& A, const auto& B) { return A.Id < B.Id; });
        TArray<TSharedPtr<FJsonValue>> Pj;
        for (const auto* P : Ps)
        {
            auto V = MakeShared<FJsonObject>();
            V->SetStringField(TEXT("id"), P->Id);
            V->SetStringField(TEXT("name"), P->Name);
            V->SetObjectField(TEXT("type"), TypeJson(P->Type));
            V->SetBoolField(TEXT("editable"), P->bEditable);
            V->SetBoolField(TEXT("read_only"), P->bReadOnly);
            MetadataJson(P->Metadata, V);
            if (!P->DelegateWire.IsEmpty())
                V->SetStringField(TEXT("delegate_wire"), P->DelegateWire);
            if (P->Default.Kind == TEXT("number"))
                V->SetNumberField(TEXT("default"), P->Default.Number);
            else if (P->Default.Kind == TEXT("bool"))
                V->SetBoolField(TEXT("default"), P->Default.Boolean);
            else if (P->Default.Kind == TEXT("string"))
                V->SetStringField(TEXT("default"), P->Default.String);
            else if (P->Default.Kind == TEXT("null"))
                V->SetField(TEXT("default"), MakeShared<FJsonValueNull>());
            Pj.Add(MakeShared<FJsonValueObject>(V));
        }
        if (!C->Interfaces.IsEmpty())
        {
            TArray<TSharedPtr<FJsonValue>> Interfaces;
            for (const auto& I : C->Interfaces)
                Interfaces.Add(MakeShared<FJsonValueString>(I));
            J->SetArrayField(TEXT("interfaces"), Interfaces);
        }
        J->SetArrayField(TEXT("properties"), Pj);
        TArray<const FDasDeclaredMethod*> Ms;
        for (const auto& M : C->Methods)
            Ms.Add(&M);
        Ms.Sort([](const auto& A, const auto& B) { return A.Id < B.Id; });
        TArray<TSharedPtr<FJsonValue>> Mj;
        for (const auto* M : Ms)
        {
            auto V = MakeShared<FJsonObject>();
            V->SetStringField(TEXT("id"), M->Id);
            V->SetStringField(TEXT("name"), M->Name);
            V->SetObjectField(TEXT("return_type"), TypeJson(M->ReturnType));
            MetadataJson(M->Metadata, V);
            if (!M->WorldContext.IsEmpty())
                V->SetStringField(TEXT("world_context"), M->WorldContext);
            V->SetBoolField(TEXT("callable"), M->bCallable);
            V->SetBoolField(TEXT("pure"), M->bPure);
            V->SetBoolField(TEXT("static"), M->bStatic);
            if (M->bConst)
                V->SetBoolField(TEXT("const"), true);
            if (M->bBlueprintEvent)
                V->SetBoolField(TEXT("blueprint_event"), true);
            auto Impl = MakeShared<FJsonObject>();
            Impl->SetStringField(TEXT("script_file"), M->ScriptFile);
            Impl->SetStringField(TEXT("entry_point"), M->EntryPoint);
            V->SetObjectField(TEXT("implementation"), Impl);
            TArray<TSharedPtr<FJsonValue>> Qj;
            for (const auto& P : M->Parameters)
            {
                auto Q = MakeShared<FJsonObject>();
                Q->SetStringField(TEXT("id"), P.Id);
                Q->SetStringField(TEXT("name"), P.Name);
                Q->SetStringField(TEXT("direction"), P.Direction);
                Q->SetObjectField(TEXT("type"), TypeJson(P.Type));
                if (P.Default.Kind == TEXT("number"))
                    Q->SetNumberField(TEXT("default"), P.Default.Number);
                else if (P.Default.Kind == TEXT("bool"))
                    Q->SetBoolField(TEXT("default"), P.Default.Boolean);
                else if (P.Default.Kind == TEXT("string"))
                    Q->SetStringField(TEXT("default"), P.Default.String);
                Qj.Add(MakeShared<FJsonValueObject>(Q));
            }
            V->SetArrayField(TEXT("parameters"), Qj);
            Mj.Add(MakeShared<FJsonValueObject>(V));
        }
        J->SetArrayField(TEXT("methods"), Mj);
        Cj.Add(MakeShared<FJsonValueObject>(J));
    }
    Root->SetArrayField(TEXT("classes"), Cj);
    FString Result;
    FJsonSerializer::Serialize(Root, TJsonWriterFactory<>::Create(&Result));
    Out = MoveTemp(Result);
    return true;
}
bool FDasAuthoringModule::ResolveClass(const FString& ClassId, FDasDeclaredClass& Out, FString& Error) const
{
    Error.Reset();
    const auto* Leaf = Classes.FindByPredicate([&](const auto& C) { return C.Id == ClassId; });
    if (!Leaf)
        return Fail(Error, TEXT("Unknown authored class: ")+ ClassId);
    TArray<const FDasDeclaredClass*> Chain;
    TSet<FString> Seen;
    for (const auto* C = Leaf; C;)
    {
        if (Seen.Contains(C->Id) || Chain.Num() >= 32)
            return Fail(Error, TEXT("Inheritance cycle or depth limit: ")+ ClassId);
        Seen.Add(C->Id);
        Chain.Add(C);
        if (C->ParentId.IsEmpty())
            break;
        const FString Parent = C->ParentId;
        C = Classes.FindByPredicate([&](const auto& P) { return P.Id == Parent; });
        if (!C)
            return Fail(Error, TEXT("Missing authored parent: ")+ Parent);
    }
    FDasDeclaredClass Effective;
    Effective.Id = Leaf->Id;
    Effective.Name = Leaf->Name;
    Effective.NativeParent = Chain.Last()->NativeParent;
    Effective.Metadata = Leaf->Metadata;
    Effective.SourceScript = Leaf->SourceScript;
    Effective.LibraryTypeRegistry = Leaf->LibraryTypeRegistry;
    auto SameSignature = [](const FDasDeclaredMethod& A, const FDasDeclaredMethod& B)
    {
        if (A.ReturnType.Identity() != B.ReturnType.Identity() || A.Parameters.Num() != B.Parameters.Num() ||
            A.bPure != B.bPure || A.bConst != B.bConst || A.bStatic != B.bStatic || A.WorldContext != B.WorldContext)
            return false;
        for (int32 I = 0; I < A.Parameters.Num(); ++I)
            if (A.Parameters[I].Name != B.Parameters[I].Name ||
                A.Parameters[I].Type.Identity() != B.Parameters[I].Type.Identity() ||
                A.Parameters[I].Direction != B.Parameters[I].Direction)
                return false;
        return true;
    };
    for (int32 I = Chain.Num() - 1; I >= 0; --I)
    {
        const auto& C = *Chain[I];
        if (C.AsyncPhysics)
            Effective.AsyncPhysics = C.AsyncPhysics;
        for (const auto& L : C.WidgetLayouts)
            Effective.WidgetLayouts.AddUnique(L);
        if (const auto* Type = C.Metadata.Find(TEXT("PrimaryAssetType")))
            Effective.Metadata.Add(TEXT("PrimaryAssetType"), *Type);
        for (const auto& Interface : C.Interfaces)
            Effective.Interfaces.AddUnique(Interface);
        if (C.NativeDefaults.Num() > 32)
            return Fail(Error, TEXT("At most 32 native default targets"));
        TSet<FString> NativeTargets;
        for (const auto& G : C.NativeDefaults)
        {
            if ((!G.Component.IsEmpty() && !Identifier(G.Component)) || NativeTargets.Contains(G.Component.ToLower()) ||
                G.Defaults.IsEmpty())
                return Fail(Error, TEXT("Invalid/duplicate native defaults target"));
            NativeTargets.Add(G.Component.ToLower());
            FDasDeclaredComponent Check;
            Check.Defaults = G.Defaults;
            if (!ValidateComponentDefaults(Check, Error))
                return false;
            auto* Target =
                Effective.NativeDefaults.FindByPredicate([&](const auto& V) { return V.Component == G.Component; });
            if (!Target)
            {
                Effective.NativeDefaults.Add(G);
                continue;
            }
            for (const auto& V : G.Defaults)
                Target->Defaults.Add(V.Key, V.Value);
        }
        if (C.PropertyOverrides.Num() > 128)
            return Fail(Error, TEXT("At most 128 inherited property defaults"));
        TSet<FString> PropertyDefaults;
        for (const auto& O : C.PropertyOverrides)
        {
            auto* P =
                Effective.Properties.FindByPredicate([&](const auto& V) { return V.Id == O.Id && V.Name == O.Name; });
            if (C.ParentId.IsEmpty() || !P || !P->bEditable || PropertyDefaults.Contains(O.Id) ||
                O.Default.Kind.IsEmpty() ||
                P->Type.Kind == TEXT("event") || P->Type.Kind == TEXT("delegate") || DasContainerCarrier(P->Type))
                return Fail(Error, TEXT("Invalid, noneditable or duplicate inherited property default: ")+ O.Name);
            PropertyDefaults.Add(O.Id);
            auto Value = *P;
            Value.Default = O.Default;
            if (!DefaultValid(Value, Error))
                return false;
            P->Default = O.Default;
        }
        for (const auto& P : C.Properties)
        {
            if (Effective.Properties.ContainsByPredicate(
                    [&](const auto& Old) { return Old.Name.Equals(P.Name, ESearchCase::IgnoreCase); }) ||
                Effective.Methods.ContainsByPredicate([&](const auto& M)
                                                      { return M.Name.Equals(P.Name, ESearchCase::IgnoreCase); }))
                return Fail(Error, TEXT("Inherited member cannot be shadowed: ")+ C.Id + TEXT(".")+ P.Name);
            Effective.Properties.Add(P);
        }
        for (const auto& M : C.Methods)
        {
            if (Effective.Properties.ContainsByPredicate([&](const auto& P)
                                                         { return P.Name.Equals(M.Name, ESearchCase::IgnoreCase); }))
                return Fail(Error, TEXT("Method shadows inherited property: ")+ M.Name);
            auto* Parent = Effective.Methods.FindByPredicate(
                [&](const auto& P) { return P.Name.Equals(M.Name, ESearchCase::IgnoreCase); });
            if (Parent)
            {
                if (!Parent->bBlueprintEvent || !M.bBlueprintEvent || !SameSignature(*Parent, M))
                    return Fail(Error, TEXT("Override requires a BlueprintEvent with unchanged signature: ")+ C.Id +
                                            TEXT(".")+ M.Name);
                *Parent = M;
            }
            else
                Effective.Methods.Add(M);
        }
        TSet<FString> Overridden;
        for (const auto& O : C.ComponentOverrides)
        {
            auto* Target =
                Effective.Components.FindByPredicate([&](const auto& V) { return V.Id == O.Id && V.Name == O.Name; });
            if (C.ParentId.IsEmpty() || !Target || Overridden.Contains(O.Id) || O.Defaults.IsEmpty())
                return Fail(Error, TEXT("Invalid/duplicate inherited component override: ")+ O.Name);
            Overridden.Add(O.Id);
            FDasDeclaredComponent Check = *Target;
            Check.Defaults = O.Defaults;
            if (!ValidateComponentDefaults(Check, Error))
                return false;
            for (const auto& Pair : O.Defaults)
                Target->Defaults.Add(Pair.Key, Pair.Value);
            if (!ValidateComponentDefaults(*Target, Error))
                return false;
        }
        for (const auto& Component : C.Components)
        {
            if (Effective.Components.ContainsByPredicate(
                    [&](const auto& P) { return P.Name.Equals(Component.Name, ESearchCase::IgnoreCase); }))
                return Fail(Error, TEXT("Inherited component cannot be shadowed: ")+ Component.Name);
            Effective.Components.Add(Component);
        }
        for (const auto& Callback : C.Callbacks)
        {
            auto* Parent = Effective.Callbacks.FindByPredicate(
                [&](const auto& P)
                { return P.Kind == Callback.Kind && (Callback.Kind != TEXT("event") || P.Name == Callback.Name); });
            if (Parent)
            {
                if (Callback.Kind == TEXT("event"))
                    return Fail(Error,
                                TEXT("Inherited managed event callback override is unsupported: ")+ Callback.Name);
                *Parent = Callback;
            }
            else
                Effective.Callbacks.Add(Callback);
        }
    }
    if (!DasValidateNativeDefaults(Effective, Error))
        return false;
    if (Effective.NativeParent == TEXT("/Script/Engine.Character") &&
                                       Effective.Components.ContainsByPredicate([](const auto& C) { return C.bRoot; }))
        return Fail(Error, TEXT("Character must retain its native capsule root"));
    if (Effective.Components.Num() > 32 ||
        (!Effective.Components.IsEmpty() && !DasActorNativeParent(Effective.NativeParent)))
        return Fail(Error, TEXT("Components require Actor root and at most 32 total declarations"));
    for (const auto& Component : Effective.Components)
        if (Effective.Properties.ContainsByPredicate(
                [&](const auto& P) { return P.Name.Equals(Component.Name, ESearchCase::IgnoreCase); }) ||
            Effective.Methods.ContainsByPredicate([&](const auto& M)
                                                  { return M.Name.Equals(Component.Name, ESearchCase::IgnoreCase); }))
            return Fail(Error, TEXT("Component conflicts with inherited member: ")+ Component.Name);
    int32 Roots = 0;
    for (const auto& Component : Effective.Components)
    {
        if (Component.bRoot && (++Roots > 1 || !Component.Attach.IsEmpty() || !Component.AttachSocket.IsEmpty()))
            return Fail(Error, TEXT("Expected one unattached root component"));
        if (!Component.Attach.IsEmpty() && !Identifier(Component.Attach))
            return Fail(Error, TEXT("Invalid component attachment name"));
        if (Component.AttachSocket.Len() >= NAME_SIZE)
            return Fail(Error, TEXT("Component socket name too long"));
        TSet<FString> Visited;
        const FDasDeclaredComponent* Current = &Component;
        while (Current)
        {
            if (Visited.Contains(Current->Name))
                return Fail(Error, TEXT("Component attachment cycle"));
            Visited.Add(Current->Name);
            if (Current->Attach.IsEmpty())
                break;
            const FString Parent = Current->Attach;
            Current = Effective.Components.FindByPredicate([&](const auto& V) { return V.Name == Parent; });
            if (!Current)
                return Fail(Error, TEXT("Unknown component attachment parent: ")+ Parent);
        }
    }
    if (Effective.AsyncPhysics)
        for (const auto& B : Effective.AsyncPhysics->Bodies)
        {
            const bool Owner = B.Value.StartsWith(TEXT("owner."));
            const FString Name = Owner ? B.Value.Mid(6) : B.Value;
            if (Owner && Effective.NativeParent != TEXT("/Script/Engine.ActorComponent"))
                return Fail(Error, TEXT("owner body selector requires ActorComponent"));
            if (Owner)
                continue; // owner concrete class is known only at runtime.
            if (Effective.Components.ContainsByPredicate([&](const auto& C) { return C.Name == Name; }))
                continue;
            auto* Native = FindObject<UClass>(nullptr, *Effective.NativeParent);
            auto* Field = Native ? FindFProperty<FObjectPropertyBase>(Native, *Name) : nullptr;
            if (!Field || !Field->PropertyClass->IsChildOf<UActorComponent>())
                return Fail(Error, TEXT("Async physics component property is missing: ")+ Name);
        }
    for (const auto& Hook : Effective.Callbacks)
        if (Hook.Kind == TEXT("event") &&
                              Hook.Source != TEXT("self") &&
                                                  Hook.Source != TEXT("owner") &&
                                                                      !Effective.Components.ContainsByPredicate(
                                                                          [&](const auto& Component)
                                                                          { return Component.Name == Hook.Source; }))
            return Fail(Error, TEXT("Event source component is missing"));
    for (const auto& Hook : Effective.Callbacks)
        if (Hook.Source == TEXT("owner") && Effective.NativeParent != TEXT("/Script/Engine.ActorComponent"))
            return Fail(Error, TEXT("Owner event source requires ActorComponent root"));
    Out = MoveTemp(Effective);
    return true;
}

bool FDasAuthoringModule::ToActorSchema(const FString& ClassId, FDasClassSchema& Out, FString& Error,
                                        bool IncludeInherited) const
{
    if (!Validate(Error))
        return false;
    const auto* C = Classes.FindByPredicate([&](const auto& X) { return X.Id == ClassId; });
    if (!C)
        return Fail(Error, TEXT("Unknown authored class ID"));
    FDasDeclaredClass Effective;
    if (!ResolveClass(ClassId, Effective, Error))
        return false;
    const auto* Declared = C;
    C = &Effective;
    const bool Library = C->NativeParent == TEXT("/Script/Engine.BlueprintFunctionLibrary");
    const bool SaveGame = C->NativeParent == TEXT("/Script/Engine.SaveGame");
    const bool DataAsset = C->NativeParent == TEXT("/Script/Engine.DataAsset") ||
                                                   C->NativeParent == TEXT("/Script/Engine.PrimaryDataAsset");
    if (C->Metadata.Contains(
            TEXT("PrimaryAssetType")) &&
            (C->NativeParent != TEXT("/Script/Engine.PrimaryDataAsset") ||
                                     !Identifier(C->Metadata.FindRef(TEXT("PrimaryAssetType"))) ||
                                                 C->Metadata.FindRef(TEXT("PrimaryAssetType")).Len() > 128))
        return Fail(Error, TEXT("primary_asset_type requires a PrimaryDataAsset root and a valid type name"));
    for (const auto& P : C->Properties)
        if (const auto* Bundles = P.Metadata.Find(TEXT("AssetBundles")))
        {
            TArray<FName> Names;
            if (!DataAsset || C->NativeParent != TEXT("/Script/Engine.PrimaryDataAsset"))
                return Fail(Error, TEXT("AssetBundles requires a PrimaryDataAsset property"));
            if (!ParseDasAssetBundleNames(*Bundles, Names, Error))
                return false;
        }
    if ((SaveGame || DataAsset) && (!C->Callbacks.IsEmpty() || C->AsyncPhysics || !C->Components.IsEmpty()))
        return Fail(Error, TEXT("SaveGame/DataAsset supports data and methods, without world lifecycle/components"));
    if (!Library && !SaveGame && !DataAsset &&
        (!DasActorNativeParent(C->NativeParent) && C->NativeParent != TEXT("/Script/Engine.ActorComponent")))
        return Fail(Error, TEXT("Class adapter requires an Actor, Pawn, Character, ActorComponent or SaveGame root"));
    if ((!SaveGame && !DataAsset && C->Methods.IsEmpty() && C->Callbacks.IsEmpty() && !C->AsyncPhysics) ||
        C->Methods.Num() > 64)
        return Fail(Error, TEXT("Actor adapter requires 1..64 methods"));
    FDasClassSchema Result;
    Result.ParentStableId = Declared->ParentId;
    Result.StableId = C->Id;
    Result.Name = *C->Name;
    Result.Parent = Library ? TEXT("FunctionLibrary")
                            : DataAsset
        ? (C->NativeParent == TEXT("/Script/Engine.DataAsset")? TEXT("DataAsset"): TEXT("PrimaryDataAsset"))
           : SaveGame
           ? TEXT("SaveGame")
           : C->NativeParent == TEXT("/Script/Engine.ActorComponent")
                 ? TEXT("ActorComponent")
                 : C->NativeParent == TEXT("/Script/Engine.Pawn")
                       ? TEXT("Pawn")
                       : C->NativeParent == TEXT("/Script/Engine.Character")? TEXT("Character"): TEXT("Actor");
    Result.Interfaces = C->Interfaces;
    Result.Metadata = C->Metadata;
    TArray<const FDasDeclaredMethod*> Methods;
    for (const auto& M : Declared->Methods)
        Methods.Add(&M);
    Methods.Sort([](const auto& A, const auto& B) { return A.Id < B.Id; });
    for (const auto* Ptr : Methods)
    {
        const auto& M = *Ptr;
        const FString K = M.ReturnType.Kind;
        const bool Action = K == TEXT("void") && M.Parameters.IsEmpty();
        const bool Scalar = (K == TEXT("int") || K == TEXT("float") || K == TEXT("bool")) && M.Parameters.Num() == 1 &&
                                                           M.Parameters[0].Type.Kind == K &&
                                                           M.Parameters[0].Direction ==
                                                               TEXT("in") && M.Parameters[0].Name == TEXT("Input");
        auto Kind = [](const FString& T)
        {
            return T ==
                   TEXT("byte")
                       ? EDasMethodSignature::Byte
                       : T ==
                             TEXT("text")
                                 ? EDasMethodSignature::Text
                                 : T ==
                                       TEXT("int")
                                           ? EDasMethodSignature::Int
                                           : T ==
                                                 TEXT("float")
                                                     ? EDasMethodSignature::Float
                                                     : T ==
                                                           TEXT("bool")
                                                               ? EDasMethodSignature::Bool
                                                               : T ==
                                                                     TEXT("void")
                                                                         ? EDasMethodSignature::
                                                                               Void
                                                                         : T ==
                                                                               TEXT("string")
                                                                                   ? EDasMethodSignature::String
                                                                                   : T ==
                                                                                         TEXT("double")
                                                                                             ? EDasMethodSignature::
                                                                                                   Double
                                                                                             : T ==
                                                                                                   TEXT("name")
                                                                                                       ? EDasMethodSignature::
                                                                                                             Name
                                                                                                       : T ==
                                                                                                             TEXT("class")
                                                                                                                 ? EDasMethodSignature::
                                                                                                                       Class
                                                                                                                 : T ==
                                                                                                                       TEXT("object")
                                                                                                                           ? EDasMethodSignature::
                                                                                                                                 Object
                                                                                                                           : T ==
                                                                                                                                 TEXT("delegate")
                                                                                                                                     ? EDasMethodSignature::
                                                                                                                                           Delegate
                                                                                                                                     : T ==
                                                                                                                                           TEXT("enum")
                                                                                                                                               ? EDasMethodSignature::
                                                                                                                                                     Enum
                                                                                                                                               : EDasMethodSignature::
                                                                                                                                                     Frame;
        };
        auto ValueKind = [&](const FDasDeclaredType& T)
        {
            if (T.Kind == TEXT("array") || T.Kind == TEXT("map") || T.Kind == TEXT("set"))
                return DasContainerCarrier(T) ? EDasMethodSignature::Struct : EDasMethodSignature::Frame;
            if (T.Kind == TEXT("struct"))
            {
                if (T.TypePath == TEXT("/Script/CoreUObject.Vector"))
                    return EDasMethodSignature::Vector;
                if (T.TypePath == TEXT("/Script/CoreUObject.Rotator"))
                    return EDasMethodSignature::Rotator;
                if (T.TypePath == TEXT("/Script/CoreUObject.Transform"))
                    return EDasMethodSignature::Transform;
                return EDasMethodSignature::Struct;
            }
            return Kind(T.Kind);
        };
        if (ValueKind(M.ReturnType) == EDasMethodSignature::Frame || M.Parameters.Num() > 16 || !M.bCallable ||
            M.bStatic != Library)
            return Fail(
                Error,
                TEXT("Actor adapter requires callable scalar/void methods with up to 16 by-value inputs, without static: ")+
                    M.Id);
        FDasMethodSchema Method;
        Method.Name = *M.Name;
        Method.ScriptFile = M.ScriptFile;
        Method.EntryPoint = M.EntryPoint;
        Method.StableId = M.Id;
        Method.InputStableId = M.Parameters.Num() == 1 ? M.Parameters[0].Id : FString();
        Method.Metadata = M.Metadata;
        Method.bBlueprintEvent = M.bBlueprintEvent;
        Method.bPure = M.bPure;
        Method.bConst = M.bConst;
        Method.bStatic = M.bStatic;
        Method.WorldContext = M.WorldContext;
        if (Library && !ToJson(Method.LibraryContract, Error))
            return false;
        Method.Metadata.Add(TEXT("DasFunctionKind"),
                                 (M.bBlueprintEvent || C->Interfaces.ContainsByPredicate(
                                                           [&](const FString& Path)
                                                           {
                                                               auto* I = FindObject<UClass>(nullptr, *Path);
                                                               return I && I->FindFunctionByName(*M.Name);
                                                           }))
                                 ? TEXT("Event")
                                 : TEXT("Callable"));
        Method.ReturnKind = ValueKind(M.ReturnType);
        Method.ReturnClass =
            K ==
            TEXT("delegate")? M.ReturnType.DelegateWire
             : DasContainerCarrier(M.ReturnType)
                 ? DasContainerCarrier(M.ReturnType)->GetPathName()
                 : (K == TEXT("class") ||
                              K == TEXT("enum") ||
                                        K == TEXT("object") || (K == TEXT("struct") && Method.ReturnKind ==
                                                                                           EDasMethodSignature::Struct))
                                            ? M.ReturnType.TypePath
                                            : FString();
        for (const auto& P : M.Parameters)
        {
            if ((ValueKind(P.Type) == EDasMethodSignature::Frame || ValueKind(P.Type) == EDasMethodSignature::Void) ||
                (P.Direction != TEXT("in") && P.Direction != TEXT("out") && P.Direction != TEXT("inout")))
                return Fail(Error, TEXT("Actor adapter parameter capability missing: ")+ P.Id);
            Method.Parameters.Add(
                {*P.Name, P.Id, ValueKind(P.Type),
                 P.Type.Kind ==
                     TEXT("delegate")? P.Type.DelegateWire
                      : DasContainerCarrier(P.Type)
                          ? DasContainerCarrier(P.Type)->GetPathName()
                          : (P.Type.Kind ==
                             TEXT("class") ||
                                  P.Type.Kind == TEXT("enum") ||
                                                      P.Type.Kind ==
                                                          TEXT("object") ||
                                                                   (P.Type.Kind ==
                                                                    TEXT("struct") && ValueKind(P.Type) ==
                                                                                          EDasMethodSignature::Struct))
                                                               ? P.Type.TypePath
                                                               : FString(),
                                                          P.Direction});
            auto& Parameter = Method.Parameters.Last();
            Parameter.bHasDefault = !P.Default.Kind.IsEmpty();
            Parameter.DefaultValue =
                P.Default.Kind == TEXT("string")? P.Default.String
                                                : P.Default.Kind == TEXT("bool")
                                                      ? (P.Default.Boolean ? TEXT("true"): TEXT("false"))
                                                      : P.Type.Kind == TEXT("int")
                                                            ? FString::Printf(TEXT("%.0f"), P.Default.Number)
                                                            : (P.Type.Kind == TEXT("float")
                                                               ? FString::Printf(TEXT("%.9g"), P.Default.Number)
                                                               : FString::Printf(TEXT("%.17g"), P.Default.Number));
        }
        Method.Signature = (Result.Parent == TEXT("Pawn") || Result.Parent == TEXT("Character")) || Library ||
                            SaveGame || DataAsset || M.bConst ||
                            Result.Parent == TEXT("ActorComponent") || !C->Interfaces.IsEmpty()
                                                  ? EDasMethodSignature::Frame
                                              : Action ? EDasMethodSignature::Void
                                              : Scalar ? Kind(K)
                                                       : EDasMethodSignature::Frame;
        Result.Methods.Add(MoveTemp(Method));
    }
    if (!Result.Methods.IsEmpty())
        Result.Method = Result.Methods[0];
    TArray<const FDasDeclaredProperty*> Properties;
    for (const auto& P : (IncludeInherited ? Effective.Properties : Declared->Properties))
        Properties.Add(&P);
    Properties.Sort([](const auto& A, const auto& B) { return A.Id < B.Id; });
    for (const auto* Ptr : Properties)
    {
        const auto& P = *Ptr;
        if ((P.Type.Kind !=
             TEXT("byte") &&
                 P.Type.Kind !=
                     TEXT("text") &&
                         P.Type.Kind !=
                             TEXT("int") &&
                                 P.Type.Kind !=
                                     TEXT("float") &&
                                         P.Type.Kind !=
                                             TEXT("bool") &&
                                                 P.Type.Kind !=
                                                     TEXT("string") &&
                                                         P.Type.Kind !=
                                                             TEXT("double") &&
                                                                 P.Type.Kind !=
                                                                     TEXT("name") &&
                                                                         P.Type.Kind !=
                                                                             TEXT("struct") &&
                                                                                 P.Type.Kind !=
                                                                                     TEXT("object") &&
                                                                                         P.Type.Kind !=
                                                                                             TEXT("enum") &&
                                                                                                 P.Type.Kind !=
                                                                                                     TEXT("event") &&
                                                                                                         P.Type.Kind !=
                                                                                                             TEXT("delegate") &&
                                                                                                                 P.Type.Kind !=
                                                                                                                     TEXT("class") &&
                                                                                                                         !DasContainerCarrier(
                                                                                                                             P.Type)))
            return Fail(Error, TEXT("Actor adapter property capability missing: ")+ P.Name);
        FDasFieldSchema F;
        F.StableId = P.Id;
        F.Name = *P.Name;
        F.DelegateWire = P.DelegateWire;
        F.Kind =
            P.Type.Kind ==
            TEXT("byte")
                ? EDasScalarKind::Byte
                : P.Type.Kind ==
                      TEXT("text")
                          ? EDasScalarKind::Text
                          : P.Type.Kind ==
                                TEXT("delegate")
                                    ? EDasScalarKind::Delegate
                                    : P.Type.Kind ==
                                          TEXT("class")
                                              ? EDasScalarKind::Class
                                              : P.Type.Kind ==
                                                    TEXT("event")
                                                        ? EDasScalarKind::Event
                                                        : P.Type.Kind ==
                                                              TEXT("int")
                                                                  ? EDasScalarKind::Int32
                                                                  : P.Type.Kind ==
                                                                        TEXT("float")
                                                                            ? EDasScalarKind::Float32
                                                                            : P.Type.Kind ==
                                                                                  TEXT("bool")
                                                                                      ? EDasScalarKind::Bool
                                                                                      : P.Type.Kind ==
                                                                                            TEXT("string")
                                                                                                ? EDasScalarKind::String
                                                                                                : P.Type.Kind ==
                                                                                                      TEXT("double")
                                                                                                          ? EDasScalarKind::
                                                                                                                Double
                                                                                                          : (P.Type.Kind ==
                                                                                                                     TEXT("struct") ||
                                                                                                                         DasContainerCarrier(
                                                                                                                             P.Type))
                                                                                                             ? EDasScalarKind::
                                                                                                                   Struct
                                                                                                             : P.Type.Kind ==
                                                                                                                   TEXT("object")
                                                                                                                       ? EDasScalarKind::
                                                                                                                             Object
                                                                                                                       : P.Type.Kind ==
                                                                                                                             TEXT("enum")
                                                                                                                                 ? EDasScalarKind::
                                                                                                                                       Enum
                                                                                                                                 : EDasScalarKind::
                                                                                                                                       Name;
        F.bInstanceEditable = P.bEditable;
        F.bReadOnly = P.bReadOnly;
        F.Metadata = P.Metadata;
        F.IntDefault = (P.Type.Kind == TEXT("int") || P.Type.Kind == TEXT("byte"))
                        ? static_cast<int32>(P.Default.Kind == TEXT("number")? P.Default.Number : 0)
                        : 0;
        F.FloatDefault = P.Type.Kind ==
                         TEXT("float")? static_cast<float>(P.Default.Kind == TEXT("number")? P.Default.Number : 0) : 0;
        F.BoolDefault = P.Default.Boolean;
        F.DoubleDefault = P.Default.Number;
        F.StringDefault = P.Default.String;
        F.NameDefault = P.Type.Kind == TEXT("name")? FName(*P.Default.String) : NAME_None;
        F.StructPath = DasContainerCarrier(P.Type) ? DasContainerCarrier(P.Type)->GetPathName()
                                                   : P.Type.Kind == TEXT("struct")? P.Type.TypePath : FString();
        F.StructDefault = P.Default.String;
        F.ObjectClass = (P.Type.Kind == TEXT("object") || P.Type.Kind == TEXT("class")) ? P.Type.TypePath : FString();
        F.EnumPath = P.Type.Kind == TEXT("enum")? P.Type.TypePath : FString();
        F.EnumDefault = P.Type.Kind == TEXT("enum")? P.Default.String : FString();
        if (P.Type.Kind == TEXT("text") && !ParseDasTextDefault(P.Default.String, F.TextDefault, Error))
            return false;
        Result.Fields.Add(F);
    }
    if (!Result.Validate(Error))
        return false;
    Out = MoveTemp(Result);
    return true;
}
FDasAuthoringDelta FDasAuthoringModule::Compare(const FDasDeclaredClass& Old, const FDasDeclaredClass& Next)
{
    FDasAuthoringDelta D;
    D.bImplementation = Old.WidgetLayouts != Next.WidgetLayouts;
    D.bStructural = Old.Id != Next.Id || Old.Name != Next.Name || Old.NativeParent != Next.NativeParent ||
                    Old.ParentId != Next.ParentId || Old.Interfaces != Next.Interfaces ||
                    Old.Components != Next.Components || Old.ComponentOverrides != Next.ComponentOverrides ||
                    Old.Callbacks != Next.Callbacks;
    D.bStructural |= Old.AsyncPhysics != Next.AsyncPhysics;
    D.bStructural |= Old.LibraryTypeRegistry != Next.LibraryTypeRegistry;
    D.bMetadata = !SameMetadata(Old.Metadata, Next.Metadata);
    D.bDefaults =
        Old.NativeDefaults.Num() != Next.NativeDefaults.Num() ||
        Old.NativeDefaults.ContainsByPredicate([&](const auto& G) { return !Next.NativeDefaults.Contains(G); }) ||
        Old.PropertyOverrides.Num() != Next.PropertyOverrides.Num() ||
        Old.PropertyOverrides.ContainsByPredicate([&](const auto& O) { return !Next.PropertyOverrides.Contains(O); });
    for (const auto& P : Next.Properties)
    {
        const auto* Prior = Old.Properties.FindByPredicate([&](const auto& X) { return X.Id == P.Id; });
        if (!Prior)
        {
            D.Added.Add(P.Id);
            D.bStructural = true;
            continue;
        }
        D.bStructural |= P.Name != Prior->Name || P.Type.Identity() != Prior->Type.Identity() ||
                         P.bEditable != Prior->bEditable || P.bReadOnly != Prior->bReadOnly;
        D.bDefaults |= !SameDefault(P.Default, Prior->Default);
        D.bMetadata |= !SameMetadata(P.Metadata, Prior->Metadata);
    }
    for (const auto& P : Old.Properties)
        if (!Next.Properties.ContainsByPredicate([&](const auto& X) { return X.Id == P.Id; }))
        {
            D.Removed.Add(P.Id);
            D.bStructural = true;
        }
    for (const auto& M : Next.Methods)
    {
        const auto* Prior = Old.Methods.FindByPredicate([&](const auto& X) { return X.Id == M.Id; });
        if (!Prior)
        {
            D.Added.Add(M.Id);
            D.bStructural = true;
            continue;
        }
        D.bStructural |= M.Name != Prior->Name || M.ReturnType.Identity() != Prior->ReturnType.Identity() ||
                         M.Parameters.Num() != Prior->Parameters.Num() || M.bCallable != Prior->bCallable ||
                         M.bPure != Prior->bPure || M.bConst != Prior->bConst || M.bStatic != Prior->bStatic ||
                         M.WorldContext != Prior->WorldContext || M.bBlueprintEvent != Prior->bBlueprintEvent;
        for (int32 Index = 0; Index < FMath::Min(M.Parameters.Num(), Prior->Parameters.Num()); ++Index)
        {
            const auto& A = M.Parameters[Index];
            const auto& B = Prior->Parameters[Index];
            D.bStructural |= A.Id != B.Id || A.Name != B.Name || A.Direction != B.Direction ||
                             A.Type.Identity() != B.Type.Identity();
        }
        for (int32 I = 0; I < FMath::Min(M.Parameters.Num(), Prior->Parameters.Num()); ++I)
            D.bMetadata |= !SameDefault(M.Parameters[I].Default, Prior->Parameters[I].Default);
        D.bMetadata |= !SameMetadata(M.Metadata, Prior->Metadata);
        D.bImplementation |= M.ScriptFile != Prior->ScriptFile || M.EntryPoint != Prior->EntryPoint;
    }
    for (const auto& M : Old.Methods)
        if (!Next.Methods.ContainsByPredicate([&](const auto& X) { return X.Id == M.Id; }))
        {
            D.Removed.Add(M.Id);
            D.bStructural = true;
        }
    D.Added.Sort();
    D.Removed.Sort();
    return D;
}

FString DasDefaultSourceContract(const FDasDeclaredProperty& P)
{
    const auto& D = P.Default;
    const FString Text =
        P.Id + TEXT("|")+ P.Type.Identity() +
                    TEXT("|")+ D.Kind +
                         TEXT("|")+ FString::Printf(TEXT("%.17g|%d|"), D.Number, D.Boolean ? 1 : 0) + D.String;
    FTCHARToUTF8 UTF8(*Text);
    FMD5 Hash;
    Hash.Update(reinterpret_cast<const uint8*>(UTF8.Get()), UTF8.Length());
    uint8 Digest[16];
    Hash.Final(Digest);
    return BytesToHex(Digest, 16);
}
bool FDasAuthoringModule::DefaultsOnly(const FDasAuthoringModule& Before, const FDasAuthoringModule& After,
                                       FString& Error)
{
    auto Strip = [](FDasAuthoringModule M)
    {
        for (auto& C : M.Classes)
        {
            C.NativeDefaults.Reset();
            C.PropertyOverrides.Reset();
            for (auto& P : C.Properties)
                P.Default = {};
            for (auto& Cmp : C.Components)
                Cmp.Defaults.Reset();
            for (auto& O : C.ComponentOverrides)
            {
                O.Defaults.Reset();
                FDasDeclaredLiteral Marker;
                Marker.Kind = TEXT("bool");
                O.Defaults.Add(TEXT("__defaults"), Marker);
            }
        }
        return M;
    };
    // Validate the actual models before stripping defaults; stripped enum/reference
    // declarations intentionally lack defaults and are compared as deterministic JSON.
    if (!Before.Validate(Error) || !After.Validate(Error))
        return false;
    auto A = Strip(Before), B = Strip(After);
    FString AJ, BJ;
    if (!A.ToJson(AJ, Error) || !B.ToJson(BJ, Error))
        return false;
    if (AJ != BJ)
    {
        Error = TEXT("Safe default update accepts default changes only; layout, metadata, hierarchy and callback contracts must stay unchanged");
        return false;
    }
    return true;
}
