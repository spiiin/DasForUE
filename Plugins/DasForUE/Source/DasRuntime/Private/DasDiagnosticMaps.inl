// Compiler-owned maps live in ModuleGroup alongside the program, including retained reload versions.
FDasSourceLocation DSLDiagnosticLocation(const das::LineInfo& At, const FString& Entity = FString())
{
    FDasSourceLocation R;
    R.Entity = Entity;
    if (At.fileInfo)
        R.File = UTF8_TO_TCHAR(At.fileInfo->name.c_str());
    R.Line = int32(At.line);
    R.Column = At.line ? int32(At.column) + 1 : 0;
    R.EndLine = FMath::Max(int32(At.line), int32(At.last_line));
    return R;
}
struct FDSLDiagnosticMap : das::ModuleGroupUserData
{
    FDSLDiagnosticMap() : ModuleGroupUserData("das.ue.source.diagnostics") {}
    TArray<FDasSourceLocation> Sources;
    TMap<FString, FDasSourceLocation> Symbols;
    void Remember(const FDasSourceLocation& S)
    {
        if (S.File.IsEmpty() || S.Line <= 0)
            return;
        auto* Old = Sources.FindByPredicate([&](const auto& V)
                                            { return V.File == S.File && V.Entity == S.Entity && V.Line == S.Line; });
        if (Old)
            Old->EndLine = FMath::Max(Old->EndLine, S.EndLine);
        else
            Sources.Add(S);
    }
    FString Friendly(FString Text, const FString& File) const
    {
        TArray<FString> Names;
        Symbols.GetKeys(Names);
        Names.Sort([](const FString& A, const FString& B) { return A.Len() > B.Len(); });
        for (const auto& Name : Names)
        {
            const auto& Origin = Symbols[Name];
            if (!File.IsEmpty() && Origin.File != File)
                continue;
            int32 At = Text.Find(Name, ESearchCase::CaseSensitive);
            while (At != INDEX_NONE)
            {
                auto Part = [](TCHAR C)
                {
                    return FChar::IsAlnum(C) || C == TEXT('_');
                };
                const int32 End = At + Name.Len();
                if ((At == 0 || !Part(Text[At - 1])) && (End == Text.Len() || !Part(Text[End])))
                {
                    Text = Text.Left(At) + Origin.Entity + Text.Mid(End);
                    At += Origin.Entity.Len();
                }
                else
                    ++At;
                At = Text.Find(Name, ESearchCase::CaseSensitive, ESearchDir::FromStart, At);
            }
        }
        return Text;
    }
};
FDSLDiagnosticMap& DSLDiagnosticMap(das::ModuleGroup& G)
{
    auto* Map = static_cast<FDSLDiagnosticMap*>(G.getUserData("das.ue.source.diagnostics"));
    if (!Map)
    {
        Map = new FDSLDiagnosticMap();
        G.setUserData(Map);
    }
    return *Map;
}
const FDSLDiagnosticMap* DSLDiagnosticMap(const das::ProgramPtr& P)
{
    return P && P->thisModuleGroup
               ? static_cast<FDSLDiagnosticMap*>(P->thisModuleGroup->getUserData("das.ue.source.diagnostics"))
               : nullptr;
}
thread_local FDSLDiagnosticMap* DSLDiagnosticActiveMap = nullptr;
thread_local FDasSourceLocation DSLDiagnosticCurrent;
struct FDSLDiagnosticCompileScope
{
    TGuardValue<FDSLDiagnosticMap*> Active;
    das::ModuleGroup& Group;
    explicit FDSLDiagnosticCompileScope(das::ModuleGroup& G)
        : Active(DSLDiagnosticActiveMap, &DSLDiagnosticMap(G)), Group(G)
    {
    }
    ~FDSLDiagnosticCompileScope() { DasCaptureDiagnosticSources(DSLDiagnosticMap(Group).Sources); }
    void RebuiltGroup() { DSLDiagnosticActiveMap = &DSLDiagnosticMap(Group); }
};
struct FDSLDiagnosticLocationScope
{
    TGuardValue<FDasSourceLocation> Current;
    FDSLDiagnosticLocationScope(const das::LineInfo& At, const FString& Entity)
        : Current(DSLDiagnosticCurrent, DSLDiagnosticLocation(At, Entity))
    {
        if (DSLDiagnosticActiveMap)
            DSLDiagnosticActiveMap->Remember(DSLDiagnosticCurrent);
    }
};
FString DSLDiagnosticFunctionEntity(const das::FunctionPtr& F)
{
    const auto Name = F->name.substr(F->name.find_last_of("`") + 1);
    return (F->classParent ? FString(UTF8_TO_TCHAR(F->classParent->name.c_str())) + TEXT("."): FString()) +
            UTF8_TO_TCHAR(Name.c_str());
}
struct FDSLDiagnosticEntityScope : FDSLDiagnosticLocationScope
{
    FDSLDiagnosticEntityScope(const das::StructurePtr& S, das::ModuleGroup& G)
        : FDSLDiagnosticLocationScope(S->at, UTF8_TO_TCHAR(S->name.c_str()))
    {
        auto& Map = DSLDiagnosticMap(G);
        Map.Remember(DSLDiagnosticCurrent);
        for (const auto& Field : S->fields)
            if (!Field.generated && Field.name != "__rtti")
                Map.Remember(DSLDiagnosticLocation(Field.at, DSLDiagnosticCurrent.Entity +
                                                                 TEXT(".")+ UTF8_TO_TCHAR(Field.name.c_str())));
        S->module->functions.foreach (
            [&](const das::FunctionPtr& F)
            {
                if (F->classParent == S)
                {
                    FString Name = UTF8_TO_TCHAR(F->name.c_str());
                    // A class method's source field supplies its public name; native entry names are separate.
                    const int32 Separator = Name.Find(TEXT("`"), ESearchCase::CaseSensitive, ESearchDir::FromEnd);
                    if (Separator != INDEX_NONE)
                        Name = Name.Mid(Separator + 1);
                    auto Origin = DSLDiagnosticLocation(F->at, DSLDiagnosticCurrent.Entity + TEXT(".")+ Name);
                    if (F->body)
                        Origin.EndLine = FMath::Max(Origin.EndLine, int32(F->body->at.last_line));
                    Map.Remember(Origin);
                }
            });
    }
};
FString DSLDiagnosticFailure(const FString& Message)
{
    if (DSLDiagnosticCurrent.File.IsEmpty() || !FDasDiagnostic::Parse(Message).IsEmpty())
        return Message;
    FDasDiagnostic D;
    static_cast<FDasSourceLocation&>(D) = DSLDiagnosticCurrent;
    D.Phase = TEXT("DSL");
    D.Message = Message;
    D.Details = Message;
    return D.Format();
}
void DSLDiagnosticGenerated(const das::ProgramPtr& P)
{
    if (!P || !P->thisModuleGroup)
        return;
    auto& Map = DSLDiagnosticMap(*P->thisModuleGroup);
    P->library.foreach (
        [&](das::Module* M)
        {
            M->functions.foreach (
                [&](const das::FunctionPtr& F)
                {
                    if (!F->generated)
                        return;
                    const auto At = DSLDiagnosticLocation(F->at);
                    auto Origin = DasDiagnosticSource(Map.Sources, At.File, At.Line);
                    if (Origin.Entity.IsEmpty())
                        return;
                    const FString Name = UTF8_TO_TCHAR(F->name.c_str());
                    if (!Map.Symbols.Contains(Name))
                        Map.Symbols.Add(Name, Origin);
                    Map.Symbols.Add(FString(UTF8_TO_TCHAR(M->name.c_str())) + TEXT("::")+ Name, Origin);
                });
            return true;
        },
        "*");
}
FString DSLDiagnosticCompilerError(const das::ProgramPtr& P, const das::Error& E)
{
    const FString Raw = UTF8_TO_TCHAR(das::reportError(E.at, E.what, E.extra, E.fixme, E.cerr).c_str());
    const FString What = UTF8_TO_TCHAR(E.what.c_str());
    if (!FDasDiagnostic::Parse(What).IsEmpty())
        return What + TEXT("\nDetails (compiler):\n")+ Raw;
    FDasDiagnostic D;
    static_cast<FDasSourceLocation&>(D) = DSLDiagnosticLocation(E.at);
    if (const auto* Map = DSLDiagnosticMap(P))
    {
        D.Entity = DasDiagnosticSource(Map->Sources, D.File, D.Line).Entity;
        D.Message = Map->Friendly(What, D.File);
    }
    else
        D.Message = What;
    D.Phase = TEXT("compile");
    D.Details = Raw;
    return D.Format();
}
FString DSLDiagnosticRuntime(FUnrealDasContext& C, const das::ProgramPtr& P = nullptr, const FString& Entry = FString())
{
    const FString Raw = C.getException() ? UTF8_TO_TCHAR(C.getException()) : TEXT("Unknown script exception");
    if (!FDasDiagnostic::Parse(Raw).IsEmpty())
        return Raw;
    FDasDiagnostic D;
    const bool Captured = C.DiagnosticException == Raw;
    static_cast<FDasSourceLocation&>(D) = Captured ? C.DiagnosticFault : DSLDiagnosticLocation(C.exceptionAt);
    if (const auto* Map = DSLDiagnosticMap(P))
    {
        auto Origin = DasDiagnosticSource(Map->Sources, D.File, D.Line);
        if (!Origin.Entity.Contains(TEXT(".")))
            if (const auto* Named = Map->Symbols.Find(Entry))
            {
                // A caller entry is only a fallback for missing/generated locations.
                // A real fault in an imported free function must keep that file and line.
                if (D.File.IsEmpty() || D.Line <= 0)
                {
                    Origin = *Named;
                    static_cast<FDasSourceLocation&>(D) = Origin;
                }
                else if (FPaths::IsSamePath(D.File, Named->File) &&
                         (Origin.Entity.IsEmpty() || !Origin.Entity.Contains(TEXT("."))))
                {
                    Origin.Entity = Named->Entity;
                }
            }
        D.Entity = Origin.Entity.IsEmpty() ? Map->Friendly(Entry, FString()) : Origin.Entity;
        D.Message = Map->Friendly(Raw.TrimStartAndEnd(), D.File);
    }
    else
        D.Message = Raw.TrimStartAndEnd();
    if (D.Entity.IsEmpty())
        D.Entity = Entry;
    D.Phase = TEXT("runtime");
    D.Details = Raw + TEXT("\n")+ (Captured ? C.DiagnosticStack : FString());
    return D.Format();
}
