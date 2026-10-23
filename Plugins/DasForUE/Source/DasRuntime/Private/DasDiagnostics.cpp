#include "DasDiagnostics.h"
#include "Internationalization/Regex.h"
#include "Misc/Paths.h"

namespace
{
thread_local FDasDiagnosticCapture* Capture = nullptr;
FString Canonical(const FString& Path)
{
    FString Result = FPaths::ConvertRelativePathToFull(Path);
    FPaths::NormalizeFilename(Result);
    return Result;
}
bool Token(const FString& Text, const FString& Name)
{
    if (Name.IsEmpty())
        return false;
    int32 At = Text.Find(Name, ESearchCase::CaseSensitive);
    auto Part = [](TCHAR C)
    {
        return FChar::IsAlnum(C) || C == TEXT('_');
    };
    while (At != INDEX_NONE)
    {
        const int32 End = At + Name.Len();
        if ((At == 0 || !Part(Text[At - 1])) && (End == Text.Len() || !Part(Text[End])))
            return true;
        At = Text.Find(Name, ESearchCase::CaseSensitive, ESearchDir::FromStart, At + 1);
    }
    return false;
}
}
FString FDasDiagnostic::Format() const
{
    FString Summary = Message.Replace(TEXT("\r"), TEXT(" ")).Replace(TEXT("\n"), TEXT(" "));
    return FString::Printf(TEXT("%s(%d,%d): [DAS:%s] %s | %s"),
                                File.IsEmpty() ? TEXT("<daScript>")    : *File, Line, Column, *Phase,
                                                      Entity.IsEmpty() ? TEXT("script"): *Entity, *Summary) +
                                                      (Details.IsEmpty() ? FString() : TEXT("\nDetails:\n")+ Details);
}
TArray<FDasDiagnostic> FDasDiagnostic::Parse(const FString& Text)
{
    const FRegexPattern Pattern(
        TEXT("([^\r\n]+)\\(([0-9]+),([0-9]+)\\): \\[DAS:([A-Za-z]+)\\] ([^\r\n]*?) \\| ([^\r\n]*)"));
    FRegexMatcher Match(Pattern, Text);
    TArray<FDasDiagnostic> Result;
    while (Match.FindNext())
    {
        FDasDiagnostic D;
        D.File = Match.GetCaptureGroup(1);
        D.Line = FCString::Atoi(*Match.GetCaptureGroup(2));
        D.Column = FCString::Atoi(*Match.GetCaptureGroup(3));
        D.Phase = Match.GetCaptureGroup(4);
        D.Entity = Match.GetCaptureGroup(5);
        D.Message = Match.GetCaptureGroup(6);
        // Callers may prefix their log message; absolute paths always begin at the drive/root.
        const int32 Drive = D.File.Find(TEXT(":/"));
        const int32 BackDrive = D.File.Find(TEXT(":\\"));
        const int32 Start = Drive != INDEX_NONE ? Drive : BackDrive;
        if (Start > 0)
            D.File = D.File.Mid(Start - 1);
        Result.Add(MoveTemp(D));
    }
    return Result;
}
FDasSourceLocation DasDiagnosticSource(const TArray<FDasSourceLocation>& Sources, const FString& File, int32 Line,
                                       const FString& EntityHint)
{
    FDasSourceLocation Result;
    const FString Path = Canonical(File);
    int32 Width = MAX_int32;
    for (const auto& S : Sources)
    {
        if (Canonical(S.File) != Path)
            continue;
        const int32 Size = FMath::Max(S.EndLine, S.Line) - S.Line;
        const bool Hint = !EntityHint.IsEmpty() && (S.Entity == EntityHint || EntityHint.EndsWith(TEXT(".")+ S.Entity));
        const bool Inside = Line > 0 && Line >= S.Line && Line <= FMath::Max(S.Line, S.EndLine);
        if ((Inside && Size < Width) || (Line == 0 && Hint))
        {
            Result = S;
            Width = Size;
        }
    }
    // Explicit stable IDs need not contain the public class name. A single top-level
    // declaration is still an unambiguous import fallback; multiple declarations are not.
    if (Line == 0 && Result.Entity.IsEmpty())
    {
        TMap<FString, FDasSourceLocation> Roots;
        for (const auto& S : Sources)
            if (Canonical(S.File) == Path && !S.Entity.IsEmpty() && !S.Entity.Contains(TEXT(".")))
                Roots.Add(S.Entity, S);
        if (Roots.Num() == 1)
            for (const auto& Root : Roots)
                Result = Root.Value;
    }
    return Result;
}
FDasDiagnosticCapture::FDasDiagnosticCapture(FString& E, const FString& F, const FString& Hint)
    : Error(E), File(Canonical(F)), Entity(Hint), Previous(Capture)
{
    Capture = this;
}
FDasDiagnosticCapture::~FDasDiagnosticCapture()
{
    Capture = Previous;
    if (Error.IsEmpty() || !FDasDiagnostic::Parse(Error).IsEmpty())
        return;
    auto Location = DasDiagnosticSource(Sources, File, 0, Entity);
    // Match only a unique explicit member name, never a substring or a nearby declaration.
    TArray<FDasSourceLocation> Members;
    const FString OwnerHint = Location.Entity.IsEmpty() ? Entity : Location.Entity;
    for (const auto& S : Sources)
    {
        FString Owner, Member;
        if (Canonical(S.File) == File &&
            S.Entity.Split(TEXT("."), &Owner, &Member, ESearchCase::CaseSensitive, ESearchDir::FromEnd) &&
                           (OwnerHint.IsEmpty() || OwnerHint == Owner || OwnerHint.EndsWith(TEXT(".")+ Owner)) &&
                            Token(Error, Member))
            Members.Add(S);
    }
    if (Members.Num() == 1)
        Location = Members[0];
    FDasDiagnostic D;
    static_cast<FDasSourceLocation&>(D) = Location;
    if (D.File.IsEmpty())
        D.File = File;
    if (D.Entity.IsEmpty())
        D.Entity = Entity;
    D.Phase = TEXT("import");
    D.Message = Error;
    D.Details = Error;
    Error = D.Format();
}
void DasCaptureDiagnosticSources(const TArray<FDasSourceLocation>& Sources)
{
    for (auto* C = Capture; C; C = C->Previous)
        for (const auto& S : Sources)
            if (!C->Sources.ContainsByPredicate(
                    [&](const auto& Old)
                    { return Old.File == S.File && Old.Line == S.Line && Old.Entity == S.Entity; }))
                C->Sources.Add(S);
}
FDasDiagnosticEvent& DasDiagnosticEvents()
{
    static FDasDiagnosticEvent Event;
    return Event;
}
