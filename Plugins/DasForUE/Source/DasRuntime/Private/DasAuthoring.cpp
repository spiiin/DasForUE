#include "DasAuthoring.h"
#include "Dom/JsonObject.h"
#include "Serialization/JsonSerializer.h"
#include "Policies/CondensedJsonPrintPolicy.h"
#include "Misc/Paths.h"

namespace
{
    bool Fail(FString& Error, const FString& Message) { Error = Message; return false; }
    bool Identifier(const FString& Text)
    {
        if (Text.IsEmpty() || Text.Len() >= NAME_SIZE || FChar::IsDigit(Text[0]) || Text.Equals(TEXT("None"), ESearchCase::IgnoreCase)) return false;
        for (TCHAR Ch : Text) if (!FChar::IsAlnum(Ch) && Ch != TEXT('_')) return false;
        return true;
    }
    bool StableId(const FString& Text)
    {
        if (Text.IsEmpty() || Text.Len() > 256) return false;
        for (TCHAR Ch : Text) if (!((Ch >= 'a' && Ch <= 'z') || (Ch >= 'A' && Ch <= 'Z') || (Ch >= '0' && Ch <= '9') || Ch == '_' || Ch == '.' || Ch == '-')) return false;
        return true;
    }
    bool NativePath(const FString& Text)
    {
        if (!Text.StartsWith(TEXT("/")) || !Text.Contains(TEXT(".")) || Text.Len() >= NAME_SIZE) return false;
        for (TCHAR Ch : Text) if (!FChar::IsAlnum(Ch) && Ch != '_' && Ch != '/' && Ch != '.') return false;
        return true;
    }
    bool MetadataValid(const TMap<FString,FString>& Metadata, FString& Error)
    {
        for (const auto& Pair : Metadata)
            if ((Pair.Key != TEXT("Category") && Pair.Key != TEXT("DisplayName") && Pair.Key != TEXT("ToolTip")) || Pair.Value.Len() > 4096)
                return Fail(Error, TEXT("Unsupported metadata key/value: ") + Pair.Key);
        return true;
    }
    bool SameMetadata(const TMap<FString,FString>& A, const TMap<FString,FString>& B)
    {
        if (A.Num() != B.Num()) return false;
        for (const auto& Pair : A) { const auto* Value = B.Find(Pair.Key); if (!Value || *Value != Pair.Value) return false; }
        return true;
    }
    bool TypeValid(const FDasDeclaredType& Type, const TSet<FString>& Classes, bool bVoid, FString& Error, int32 Depth = 0)
    {
        if (Depth > 16) return Fail(Error, TEXT("Declaration type nesting limit exceeded"));
        const FString& K = Type.Kind;
        if (K == TEXT("array") || K == TEXT("set"))
        {
            if (!Type.Element || Type.Key || Type.Value || !Type.TypePath.IsEmpty()) return Fail(Error, TEXT("Invalid element type shape"));
            return TypeValid(*Type.Element, Classes, false, Error, Depth + 1);
        }
        if (K == TEXT("map"))
        {
            if (!Type.Key || !Type.Value || Type.Element || !Type.TypePath.IsEmpty()) return Fail(Error, TEXT("Invalid map type shape"));
            if (Type.Key->Kind != TEXT("int") && Type.Key->Kind != TEXT("float") && Type.Key->Kind != TEXT("double") && Type.Key->Kind != TEXT("bool") && Type.Key->Kind != TEXT("string") && Type.Key->Kind != TEXT("name") && Type.Key->Kind != TEXT("enum")) return Fail(Error, TEXT("Map key requires a supported scalar identity"));
            return TypeValid(*Type.Key, Classes, false, Error, Depth + 1) && TypeValid(*Type.Value, Classes, false, Error, Depth + 1);
        }
        if (Type.Element || Type.Key || Type.Value) return Fail(Error, TEXT("Scalar/reference type has container members"));
        if (K == TEXT("object") || K == TEXT("enum") || K == TEXT("struct"))
            return (NativePath(Type.TypePath) || (K == TEXT("object") && Classes.Contains(Type.TypePath))) || Fail(Error, TEXT("Missing type identity: ") + Type.TypePath);
        if (!Type.TypePath.IsEmpty()) return Fail(Error, TEXT("Primitive type has a type_path"));
        if (K == TEXT("int") || K == TEXT("float") || K == TEXT("double") || K == TEXT("bool") || K == TEXT("string") || K == TEXT("name") || (bVoid && K == TEXT("void"))) return true;
        return Fail(Error, TEXT("Unsupported declaration type: ") + K);
    }
    bool DefaultValid(const FDasDeclaredProperty& P, FString& Error)
    {
        const auto& D = P.Default; if (D.Kind.IsEmpty()) return true;
        const auto& K = P.Type.Kind;
        if (D.Kind == TEXT("number") && FMath::IsFinite(D.Number))
        {
            if (K == TEXT("int") && D.Number >= MIN_int32 && D.Number <= MAX_int32 && FMath::FloorToDouble(D.Number) == D.Number) return true;
            if (K == TEXT("double")) return true;
            if (K == TEXT("float") && FMath::IsFinite(static_cast<float>(D.Number)) && (D.Number == 0 || static_cast<float>(D.Number) != 0)) return true;
        }
        if (K == TEXT("bool") && D.Kind == TEXT("bool")) return true;
        if ((K == TEXT("string") || K == TEXT("name") || K == TEXT("enum")) && D.Kind == TEXT("string") && (K != TEXT("name") || D.String.Len() < NAME_SIZE) && (K != TEXT("enum") || Identifier(D.String))) return true;
        if (K == TEXT("object") && D.Kind == TEXT("null")) return true;
        return Fail(Error, TEXT("Invalid default for property ") + P.Name);
    }
    bool SameDefault(const FDasDeclaredLiteral& A, const FDasDeclaredLiteral& B)
    {
        if (A.Kind!=B.Kind) return false;
        if (A.Kind==TEXT("number")) return A.Number==B.Number;
        if (A.Kind==TEXT("bool")) return A.Boolean==B.Boolean;
        if (A.Kind==TEXT("string")) return A.String==B.String;
        return true;
    }
    bool Keys(const TSharedPtr<FJsonObject>& J, std::initializer_list<const TCHAR*> Allowed, FString& Error)
    {
        for (const auto& Pair : J->Values)
        { bool Found = false; for (const auto* K : Allowed) if (Pair.Key == K) Found = true; if (!Found) return Fail(Error, TEXT("Unknown authoring key: ") + FString(Pair.Key)); }
        return true;
    }
    bool String(const TSharedPtr<FJsonObject>& J, const TCHAR* Key, FString& Out, FString& Error)
    { return (J->HasTypedField<EJson::String>(Key) && J->TryGetStringField(Key, Out)) || Fail(Error, FString(Key) + TEXT(" must be a string")); }
    bool Boolean(const TSharedPtr<FJsonObject>& J, const TCHAR* Key, bool& Out, FString& Error)
    { if (!J->HasField(Key)) return true; return (J->HasTypedField<EJson::Boolean>(Key) && J->TryGetBoolField(Key, Out)) || Fail(Error, FString(Key) + TEXT(" must be boolean")); }
    bool Meta(const TSharedPtr<FJsonObject>& J, TMap<FString,FString>& Out, FString& Error)
    {
        if (!J->HasField(TEXT("metadata"))) return true;
        const TSharedPtr<FJsonObject>* M; if (!J->TryGetObjectField(TEXT("metadata"), M)) return Fail(Error, TEXT("metadata must be an object"));
        for (const auto& Pair : (*M)->Values)
        { FString Value; if (Pair.Value->Type != EJson::String || !Pair.Value->TryGetString(Value)) return Fail(Error, TEXT("metadata values must be strings")); Out.Add(FString(Pair.Key), Value); }
        return true;
    }
    bool ReadType(const TSharedPtr<FJsonObject>& J, FDasDeclaredType& Out, FString& Error, int32 Depth = 0)
    {
        if (Depth > 16) return Fail(Error, TEXT("Declaration type nesting limit exceeded"));
        if (!Keys(J, {TEXT("kind"), TEXT("type_path"), TEXT("element"), TEXT("key"), TEXT("value")}, Error) || !String(J, TEXT("kind"), Out.Kind, Error)) return false;
        if (J->HasField(TEXT("type_path")) && !String(J, TEXT("type_path"), Out.TypePath, Error)) return false;
        for (const auto* Name : {TEXT("element"), TEXT("key"), TEXT("value")})
            if (J->HasField(Name))
            {
                const TSharedPtr<FJsonObject>* Child; if (!J->TryGetObjectField(Name, Child)) return Fail(Error, TEXT("Type child must be an object"));
                auto T = MakeShared<FDasDeclaredType>(); if (!ReadType(*Child, *T, Error, Depth + 1)) return false;
                if (FString(Name) == TEXT("element")) Out.Element = T; else if (FString(Name) == TEXT("key")) Out.Key = T; else Out.Value = T;
            }
        return true;
    }
    bool Type(const TSharedPtr<FJsonObject>& J, const TCHAR* Key, FDasDeclaredType& Out, FString& Error)
    { const TSharedPtr<FJsonObject>* T; return J->TryGetObjectField(Key,T) ? ReadType(*T, Out, Error) : Fail(Error, FString(Key) + TEXT(" must be a type object")); }
    bool Objects(const TSharedPtr<FJsonObject>& J, const TCHAR* Key, TArray<TSharedPtr<FJsonObject>>& Out, FString& Error)
    {
        const TArray<TSharedPtr<FJsonValue>>* Values; if (!J->TryGetArrayField(Key, Values)) return Fail(Error, FString(Key) + TEXT(" must be an array"));
        if (Values->Num() > 1024) return Fail(Error, TEXT("Declaration count limit exceeded"));
        for (const auto& V : *Values) { const TSharedPtr<FJsonObject>* O; if (!V->TryGetObject(O)) return Fail(Error, TEXT("Expected declaration object")); Out.Add(*O); }
        return true;
    }
    TSharedPtr<FJsonObject> TypeJson(const FDasDeclaredType& T, int32 Depth = 0)
    {
        auto J = MakeShared<FJsonObject>(); if (Depth > 16) { J->SetStringField(TEXT("kind"),TEXT("invalid_depth")); return J; } J->SetStringField(TEXT("kind"),T.Kind);
        if (!T.TypePath.IsEmpty()) J->SetStringField(TEXT("type_path"),T.TypePath);
        if (T.Element) J->SetObjectField(TEXT("element"),TypeJson(*T.Element, Depth + 1));
        if (T.Key) J->SetObjectField(TEXT("key"),TypeJson(*T.Key, Depth + 1));
        if (T.Value) J->SetObjectField(TEXT("value"),TypeJson(*T.Value, Depth + 1));
        return J;
    }
    void MetadataJson(const TMap<FString,FString>& M, const TSharedPtr<FJsonObject>& J)
    { auto O = MakeShared<FJsonObject>(); TArray<FString> K; M.GetKeys(K); K.Sort(); for (const auto& Key : K) O->SetStringField(Key,M[Key]); J->SetObjectField(TEXT("metadata"), O); }
}

FString FDasDeclaredType::Identity() const
{
    FString S; FJsonSerializer::Serialize(TypeJson(*this), TJsonWriterFactory<TCHAR,TCondensedJsonPrintPolicy<TCHAR>>::Create(&S)); return S;
}
bool FDasAuthoringModule::Validate(FString& Error) const
{
    Error.Reset(); if (!StableId(Id) || Classes.IsEmpty() || Classes.Num() > 1024) return Fail(Error,TEXT("Expected module ID and 1..1024 classes"));
    TSet<FString> Ids, ClassIds, ClassNames; Ids.Add(Id);
    auto AddId = [&](const FString& Value) { if (!StableId(Value) || Ids.Contains(Value)) return Fail(Error,TEXT("Invalid or duplicate stable ID: ")+Value); Ids.Add(Value); return true; };
    for (const auto& C : Classes)
    {
        if (!AddId(C.Id) || !Identifier(C.Name) || ClassNames.Contains(C.Name.ToLower())) return Fail(Error,TEXT("Invalid/duplicate class identity or name: ")+C.Name);
        ClassIds.Add(C.Id); ClassNames.Add(C.Name.ToLower());
    }
    for (const auto& C : Classes)
    {
        if (C.NativeParent.IsEmpty() == C.ParentId.IsEmpty() || (!C.NativeParent.IsEmpty() && !NativePath(C.NativeParent)) || (!C.ParentId.IsEmpty() && !ClassIds.Contains(C.ParentId))) return Fail(Error,TEXT("Expected one valid parent for ")+C.Id);
        if (!MetadataValid(C.Metadata,Error)) return false;
        if (C.Properties.Num() > 1024 || C.Methods.Num() > 1024) return Fail(Error,TEXT("Declaration count limit exceeded"));
        TSet<FString> Names;
        for (const auto& P : C.Properties)
        {
            if (!AddId(P.Id) || !Identifier(P.Name) || Names.Contains(P.Name.ToLower())) return Fail(Error,TEXT("Invalid or duplicate property: ")+P.Name);
            Names.Add(P.Name.ToLower()); if (!TypeValid(P.Type,ClassIds,false,Error) || !DefaultValid(P,Error) || !MetadataValid(P.Metadata,Error)) return false;
        }
        for (const auto& M : C.Methods)
        {
            if (!AddId(M.Id) || !Identifier(M.Name) || Names.Contains(M.Name.ToLower())) return Fail(Error,TEXT("Invalid or duplicate method: ")+M.Name);
            Names.Add(M.Name.ToLower()); if (!TypeValid(M.ReturnType,ClassIds,true,Error) || !MetadataValid(M.Metadata,Error)) return false;
            if (!Identifier(M.EntryPoint) || M.ScriptFile.IsEmpty() || FPaths::GetCleanFilename(M.ScriptFile) != M.ScriptFile || M.ScriptFile.Contains(TEXT(":")) || !M.ScriptFile.EndsWith(TEXT(".das"))) return Fail(Error,TEXT("Expected local script filename and entry point: ")+M.Id);
            if (M.Parameters.Num() > 64) return Fail(Error,TEXT("Parameter count limit exceeded"));
            TSet<FString> Parameters;
            for (const auto& P : M.Parameters)
            {
                if (!AddId(P.Id) || !Identifier(P.Name) || Parameters.Contains(P.Name.ToLower()) || (P.Direction != TEXT("in") && P.Direction != TEXT("out") && P.Direction != TEXT("inout"))) return Fail(Error,TEXT("Invalid parameter: ")+P.Name);
                Parameters.Add(P.Name.ToLower()); if (!TypeValid(P.Type,ClassIds,false,Error)) return false;
            }
        }
    }
    // Inheritance dependencies are acyclic; object references may legitimately point to self.
    for (const auto& C : Classes)
    {
        TSet<FString> Chain, InheritedNames; const FDasDeclaredClass* Current = &C;
        while (Current)
        {
            if (Chain.Contains(Current->Id)) return Fail(Error,TEXT("Inheritance cycle: ")+C.Id);
            Chain.Add(Current->Id);
            for (const auto& P : Current->Properties) { const FString N=P.Name.ToLower(); if (InheritedNames.Contains(N)) return Fail(Error,TEXT("Inherited member name conflict: ")+P.Name); InheritedNames.Add(N); }
            for (const auto& M : Current->Methods) { const FString N=M.Name.ToLower(); if (InheritedNames.Contains(N)) return Fail(Error,TEXT("Inherited member name conflict: ")+M.Name); InheritedNames.Add(N); }
            Current = Current->ParentId.IsEmpty() ? nullptr : Classes.FindByPredicate([&](const auto& P){return P.Id == Current->ParentId;});
        }
    }
    return true;
}
bool FDasAuthoringModule::FromJson(const FString& Json, FDasAuthoringModule& Out, FString& Error)
{
    Error.Reset(); if (FTCHARToUTF8(*Json).Length() > 1024*1024) return Fail(Error,TEXT("Authoring JSON exceeds 1 MiB"));
    TSharedPtr<FJsonObject> Root; if (!FJsonSerializer::Deserialize(TJsonReaderFactory<>::Create(Json),Root) || !Root) return Fail(Error,TEXT("Invalid authoring JSON"));
    if (!Keys(Root,{TEXT("schema_version"),TEXT("module_id"),TEXT("classes")},Error)) return false;
    if (!Root->HasTypedField<EJson::Number>(TEXT("schema_version")) || Root->GetNumberField(TEXT("schema_version")) != 1) return Fail(Error,TEXT("Expected authoring schema_version 1"));
    FDasAuthoringModule Candidate; if (!String(Root,TEXT("module_id"),Candidate.Id,Error)) return false;
    TArray<TSharedPtr<FJsonObject>> ClassesJson; if (!Objects(Root,TEXT("classes"),ClassesJson,Error)) return false;
    for (const auto& J : ClassesJson)
    {
        FDasDeclaredClass C;
        if (!Keys(J,{TEXT("id"),TEXT("name"),TEXT("native_parent"),TEXT("parent_id"),TEXT("properties"),TEXT("methods"),TEXT("metadata")},Error) || !String(J,TEXT("id"),C.Id,Error) || !String(J,TEXT("name"),C.Name,Error) || !Meta(J,C.Metadata,Error)) return false;
        if (J->HasField(TEXT("native_parent")) && !String(J,TEXT("native_parent"),C.NativeParent,Error)) return false;
        if (J->HasField(TEXT("parent_id")) && !String(J,TEXT("parent_id"),C.ParentId,Error)) return false;
        TArray<TSharedPtr<FJsonObject>> Ps, Ms; if (!Objects(J,TEXT("properties"),Ps,Error) || !Objects(J,TEXT("methods"),Ms,Error)) return false;
        for (const auto& V : Ps)
        {
            FDasDeclaredProperty P;
            if (!Keys(V,{TEXT("id"),TEXT("name"),TEXT("type"),TEXT("default"),TEXT("editable"),TEXT("read_only"),TEXT("metadata")},Error) || !String(V,TEXT("id"),P.Id,Error) || !String(V,TEXT("name"),P.Name,Error) || !Type(V,TEXT("type"),P.Type,Error) || !Meta(V,P.Metadata,Error) || !Boolean(V,TEXT("editable"),P.bEditable,Error) || !Boolean(V,TEXT("read_only"),P.bReadOnly,Error)) return false;
            if (const auto* D = V->Values.Find(TEXT("default")))
            {
                if ((*D)->Type == EJson::Null) P.Default.Kind = TEXT("null");
                else if ((*D)->Type == EJson::Number) { P.Default.Kind = TEXT("number"); P.Default.Number = (*D)->AsNumber(); }
                else if ((*D)->Type == EJson::Boolean) { P.Default.Kind = TEXT("bool"); P.Default.Boolean = (*D)->AsBool(); }
                else if ((*D)->Type == EJson::String) { P.Default.Kind = TEXT("string"); P.Default.String = (*D)->AsString(); }
                else return Fail(Error,TEXT("Property default must be a scalar literal"));
            }
            C.Properties.Add(MoveTemp(P));
        }
        for (const auto& V : Ms)
        {
            FDasDeclaredMethod M;
            if (!Keys(V,{TEXT("id"),TEXT("name"),TEXT("return_type"),TEXT("parameters"),TEXT("implementation"),TEXT("callable"),TEXT("pure"),TEXT("static"),TEXT("metadata")},Error) || !String(V,TEXT("id"),M.Id,Error) || !String(V,TEXT("name"),M.Name,Error) || !Type(V,TEXT("return_type"),M.ReturnType,Error) || !Meta(V,M.Metadata,Error) || !Boolean(V,TEXT("callable"),M.bCallable,Error) || !Boolean(V,TEXT("pure"),M.bPure,Error) || !Boolean(V,TEXT("static"),M.bStatic,Error)) return false;
            const TSharedPtr<FJsonObject>* Impl;
            if (!V->TryGetObjectField(TEXT("implementation"),Impl) || !Keys(*Impl,{TEXT("script_file"),TEXT("entry_point")},Error) || !String(*Impl,TEXT("script_file"),M.ScriptFile,Error) || !String(*Impl,TEXT("entry_point"),M.EntryPoint,Error)) return Fail(Error,Error.IsEmpty()?TEXT("Expected method implementation"):Error);
            TArray<TSharedPtr<FJsonObject>> Params; if (!Objects(V,TEXT("parameters"),Params,Error)) return false;
            for (const auto& Q : Params)
            {
                FDasDeclaredParameter P;
                if (!Keys(Q,{TEXT("id"),TEXT("name"),TEXT("type"),TEXT("direction")},Error) || !String(Q,TEXT("id"),P.Id,Error) || !String(Q,TEXT("name"),P.Name,Error) || !String(Q,TEXT("direction"),P.Direction,Error) || !Type(Q,TEXT("type"),P.Type,Error)) return false;
                M.Parameters.Add(MoveTemp(P));
            }
            C.Methods.Add(MoveTemp(M));
        }
        Candidate.Classes.Add(MoveTemp(C));
    }
    if (!Candidate.Validate(Error)) return false; Out = MoveTemp(Candidate); return true;
}
bool FDasAuthoringModule::ToJson(FString& Out, FString& Error) const
{
    if (!Validate(Error)) return false;
    auto Root = MakeShared<FJsonObject>(); Root->SetNumberField(TEXT("schema_version"),1); Root->SetStringField(TEXT("module_id"),Id);
    TArray<const FDasDeclaredClass*> Cs; for (const auto& C : Classes) Cs.Add(&C); Cs.Sort([](const auto& A,const auto& B){return A.Id<B.Id;});
    TArray<TSharedPtr<FJsonValue>> Cj;
    for (const auto* C : Cs)
    {
        auto J = MakeShared<FJsonObject>(); J->SetStringField(TEXT("id"),C->Id); J->SetStringField(TEXT("name"),C->Name);
        J->SetStringField(C->ParentId.IsEmpty()?TEXT("native_parent"):TEXT("parent_id"),C->ParentId.IsEmpty()?C->NativeParent:C->ParentId); MetadataJson(C->Metadata,J);
        TArray<const FDasDeclaredProperty*> Ps; for (const auto& P : C->Properties) Ps.Add(&P); Ps.Sort([](const auto& A,const auto& B){return A.Id<B.Id;});
        TArray<TSharedPtr<FJsonValue>> Pj;
        for (const auto* P : Ps)
        {
            auto V = MakeShared<FJsonObject>(); V->SetStringField(TEXT("id"),P->Id); V->SetStringField(TEXT("name"),P->Name); V->SetObjectField(TEXT("type"),TypeJson(P->Type)); V->SetBoolField(TEXT("editable"),P->bEditable); V->SetBoolField(TEXT("read_only"),P->bReadOnly); MetadataJson(P->Metadata,V);
            if (P->Default.Kind == TEXT("number")) V->SetNumberField(TEXT("default"),P->Default.Number);
            else if (P->Default.Kind == TEXT("bool")) V->SetBoolField(TEXT("default"),P->Default.Boolean);
            else if (P->Default.Kind == TEXT("string")) V->SetStringField(TEXT("default"),P->Default.String);
            else if (P->Default.Kind == TEXT("null")) V->SetField(TEXT("default"),MakeShared<FJsonValueNull>());
            Pj.Add(MakeShared<FJsonValueObject>(V));
        }
        J->SetArrayField(TEXT("properties"),Pj);
        TArray<const FDasDeclaredMethod*> Ms; for (const auto& M : C->Methods) Ms.Add(&M); Ms.Sort([](const auto& A,const auto& B){return A.Id<B.Id;});
        TArray<TSharedPtr<FJsonValue>> Mj;
        for (const auto* M : Ms)
        {
            auto V = MakeShared<FJsonObject>(); V->SetStringField(TEXT("id"),M->Id); V->SetStringField(TEXT("name"),M->Name); V->SetObjectField(TEXT("return_type"),TypeJson(M->ReturnType)); MetadataJson(M->Metadata,V);
            V->SetBoolField(TEXT("callable"),M->bCallable); V->SetBoolField(TEXT("pure"),M->bPure); V->SetBoolField(TEXT("static"),M->bStatic);
            auto Impl = MakeShared<FJsonObject>(); Impl->SetStringField(TEXT("script_file"),M->ScriptFile); Impl->SetStringField(TEXT("entry_point"),M->EntryPoint); V->SetObjectField(TEXT("implementation"),Impl);
            TArray<TSharedPtr<FJsonValue>> Qj; for (const auto& P : M->Parameters)
            { auto Q = MakeShared<FJsonObject>(); Q->SetStringField(TEXT("id"),P.Id); Q->SetStringField(TEXT("name"),P.Name); Q->SetStringField(TEXT("direction"),P.Direction); Q->SetObjectField(TEXT("type"),TypeJson(P.Type)); Qj.Add(MakeShared<FJsonValueObject>(Q)); }
            V->SetArrayField(TEXT("parameters"),Qj); Mj.Add(MakeShared<FJsonValueObject>(V));
        }
        J->SetArrayField(TEXT("methods"),Mj); Cj.Add(MakeShared<FJsonValueObject>(J));
    }
    Root->SetArrayField(TEXT("classes"),Cj); FString Result; FJsonSerializer::Serialize(Root,TJsonWriterFactory<>::Create(&Result)); Out = MoveTemp(Result); return true;
}
bool FDasAuthoringModule::ToActorSchema(const FString& ClassId, FDasClassSchema& Out, FString& Error) const
{
    if (!Validate(Error)) return false;
    const auto* C = Classes.FindByPredicate([&](const auto& X){return X.Id == ClassId;}); if (!C) return Fail(Error,TEXT("Unknown authored class ID"));
    if (C->NativeParent != TEXT("/Script/Engine.Actor") || !C->ParentId.IsEmpty()) return Fail(Error,TEXT("Actor adapter requires direct native Actor parent"));
    if (C->Methods.IsEmpty() || C->Methods.Num()>64) return Fail(Error,TEXT("Actor adapter requires 1..64 methods"));
    FDasClassSchema Result;Result.StableId=C->Id;Result.Name=*C->Name;Result.Parent=TEXT("Actor");Result.Metadata=C->Metadata;
    TArray<const FDasDeclaredMethod*> Methods;for (const auto& M:C->Methods) Methods.Add(&M);
    Methods.Sort([](const auto& A,const auto& B){return A.Id<B.Id;});
    for (const auto* Ptr:Methods)
    {
        const auto& M=*Ptr;
        const FString K=M.ReturnType.Kind;
        const bool Action=K==TEXT("void") && M.Parameters.IsEmpty();
        const bool Scalar=(K==TEXT("int") || K==TEXT("float") || K==TEXT("bool")) && M.Parameters.Num()==1 && M.Parameters[0].Type.Kind==K && M.Parameters[0].Direction==TEXT("in") && M.Parameters[0].Name==TEXT("Input");
        auto Kind=[](const FString& T){return T==TEXT("int")?EDasMethodSignature::Int:T==TEXT("float")?EDasMethodSignature::Float:T==TEXT("bool")?EDasMethodSignature::Bool:T==TEXT("void")?EDasMethodSignature::Void:T==TEXT("string")?EDasMethodSignature::String:T==TEXT("double")?EDasMethodSignature::Double:T==TEXT("name")?EDasMethodSignature::Name:EDasMethodSignature::Frame;};
        if(Kind(K)==EDasMethodSignature::Frame || M.Parameters.Num()>16 || !M.bCallable || M.bPure || M.bStatic) return Fail(Error,TEXT("Actor adapter requires callable scalar/void methods with up to 16 by-value inputs, without pure/static: ")+M.Id);
        FDasMethodSchema Method;Method.Name=*M.Name;Method.ScriptFile=M.ScriptFile;Method.EntryPoint=M.EntryPoint;Method.StableId=M.Id;Method.InputStableId=M.Parameters.Num()==1?M.Parameters[0].Id:FString();Method.Metadata=M.Metadata;Method.ReturnKind=Kind(K);
        for(const auto& P:M.Parameters){if((Kind(P.Type.Kind)==EDasMethodSignature::Frame || Kind(P.Type.Kind)==EDasMethodSignature::Void) || P.Direction!=TEXT("in")) return Fail(Error,TEXT("Actor adapter parameter capability missing: ")+P.Id);Method.Parameters.Add({*P.Name,P.Id,Kind(P.Type.Kind)});}
        Method.Signature=Action?EDasMethodSignature::Void:Scalar?Kind(K):EDasMethodSignature::Frame;
        Result.Methods.Add(MoveTemp(Method));
    }
    Result.Method=Result.Methods[0];
    TArray<const FDasDeclaredProperty*> Properties; for (const auto& P : C->Properties) Properties.Add(&P);
    Properties.Sort([](const auto& A,const auto& B){return A.Id<B.Id;});
    for (const auto* Ptr : Properties)
    {
        const auto& P=*Ptr;
        if ((P.Type.Kind != TEXT("int") && P.Type.Kind != TEXT("float") && P.Type.Kind != TEXT("bool") && P.Type.Kind!=TEXT("string") && P.Type.Kind!=TEXT("double") && P.Type.Kind!=TEXT("name"))) return Fail(Error,TEXT("Actor adapter property capability missing: ")+P.Name);
        FDasFieldSchema F; F.StableId = P.Id; F.Name = *P.Name; F.Kind = P.Type.Kind == TEXT("int")?EDasScalarKind::Int32:P.Type.Kind==TEXT("float")?EDasScalarKind::Float32:P.Type.Kind==TEXT("bool")?EDasScalarKind::Bool:P.Type.Kind==TEXT("string")?EDasScalarKind::String:P.Type.Kind==TEXT("double")?EDasScalarKind::Double:EDasScalarKind::Name; F.bInstanceEditable=P.bEditable;F.bReadOnly=P.bReadOnly;F.Metadata=P.Metadata;
        F.IntDefault=P.Type.Kind==TEXT("int")?static_cast<int32>(P.Default.Kind==TEXT("number")?P.Default.Number:0):0; F.FloatDefault=P.Type.Kind==TEXT("float")?static_cast<float>(P.Default.Kind==TEXT("number")?P.Default.Number:0):0; F.BoolDefault=P.Default.Boolean;F.DoubleDefault=P.Default.Number;F.StringDefault=P.Default.String;F.NameDefault=P.Type.Kind==TEXT("name")?FName(*P.Default.String):NAME_None; Result.Fields.Add(F);
    }
    if (!Result.Validate(Error)) return false; Out = MoveTemp(Result); return true;
}
FDasAuthoringDelta FDasAuthoringModule::Compare(const FDasDeclaredClass& Old, const FDasDeclaredClass& Next)
{
    FDasAuthoringDelta D; D.bStructural = Old.Id!=Next.Id || Old.Name!=Next.Name || Old.NativeParent!=Next.NativeParent || Old.ParentId!=Next.ParentId;
    D.bMetadata = !SameMetadata(Old.Metadata,Next.Metadata);
    for (const auto& P : Next.Properties)
    {
        const auto* Prior=Old.Properties.FindByPredicate([&](const auto& X){return X.Id==P.Id;});
        if (!Prior) { D.Added.Add(P.Id); D.bStructural=true; continue; }
        D.bStructural |= P.Name!=Prior->Name || P.Type.Identity()!=Prior->Type.Identity() || P.bEditable!=Prior->bEditable || P.bReadOnly!=Prior->bReadOnly;
        D.bDefaults |= !SameDefault(P.Default,Prior->Default); D.bMetadata |= !SameMetadata(P.Metadata,Prior->Metadata);
    }
    for (const auto& P : Old.Properties) if (!Next.Properties.ContainsByPredicate([&](const auto& X){return X.Id==P.Id;})) {D.Removed.Add(P.Id);D.bStructural=true;}
    for (const auto& M : Next.Methods)
    {
        const auto* Prior=Old.Methods.FindByPredicate([&](const auto& X){return X.Id==M.Id;});
        if (!Prior) {D.Added.Add(M.Id);D.bStructural=true;continue;}
        D.bStructural |= M.Name!=Prior->Name || M.ReturnType.Identity()!=Prior->ReturnType.Identity() || M.Parameters.Num()!=Prior->Parameters.Num() || M.bCallable!=Prior->bCallable || M.bPure!=Prior->bPure || M.bStatic!=Prior->bStatic;
        for (int32 Index=0;Index<FMath::Min(M.Parameters.Num(),Prior->Parameters.Num());++Index)
        {const auto& A=M.Parameters[Index];const auto& B=Prior->Parameters[Index];D.bStructural |= A.Id!=B.Id || A.Name!=B.Name || A.Direction!=B.Direction || A.Type.Identity()!=B.Type.Identity();}
        D.bMetadata |= !SameMetadata(M.Metadata,Prior->Metadata); D.bImplementation |= M.ScriptFile!=Prior->ScriptFile || M.EntryPoint!=Prior->EntryPoint;
    }
    for (const auto& M:Old.Methods) if (!Next.Methods.ContainsByPredicate([&](const auto& X){return X.Id==M.Id;})){D.Removed.Add(M.Id);D.bStructural=true;}
    D.Added.Sort();D.Removed.Sort();return D;
}
