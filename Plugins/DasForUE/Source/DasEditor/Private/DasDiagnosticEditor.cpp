#include "DasDiagnosticEditor.h"
#include "DasRuntime.h"
#include "ISourceCodeAccessModule.h"
#include "ISourceCodeAccessor.h"
#include "Logging/MessageLog.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "Modules/ModuleManager.h"

bool DasDiagnosticEditor::Navigate(const FDasDiagnostic& D, const FDasOpenDiagnosticSource& Open)
{
    if (!IsInGameThread() || FPaths::GetExtension(D.File) != TEXT("das")|| D.Line < 0 || D.Column < 0)
        return false;
    FString File = FPaths::ConvertRelativePathToFull(D.File);
    FPaths::NormalizeFilename(File);
    TArray<FString> Lines;
    if (!FFileHelper::LoadFileToStringArray(Lines, *File))
        return false;
    const int32 Line = FMath::Max(1, D.Line), Column = FMath::Max(1, D.Column);
    // Reload/edit may remove a reported line. Refuse stale/out-of-range coordinates, don't jump elsewhere.
    if (Line > Lines.Num() || (D.Column > 0 && Column > Lines[Line - 1].Len() + 1))
        return false;
    if (Open.IsBound())
        return Open.Execute(File, Line, Column);
    auto& Access = FModuleManager::LoadModuleChecked<ISourceCodeAccessModule>(TEXT("SourceCodeAccess"));
    return Access.GetAccessor().OpenFileAtLine(File, Line, Column);
}
TSharedRef<FTokenizedMessage> DasDiagnosticEditor::Message(const FDasDiagnostic& D, FDasOpenDiagnosticSource Open)
{
    auto Result = FTokenizedMessage::Create(
        EMessageSeverity::Error,
        FText::FromString(FString::Printf(TEXT("daScript %s: %s — %s"), *D.Phase, *D.Entity, *D.Message)));
    if (!D.File.IsEmpty() && FPaths::GetExtension(D.File) == TEXT("das"))
        Result->AddToken(FActionToken::Create(FText::FromString(
            FString::Printf(TEXT("%s:%d"), *FPaths::GetCleanFilename(D.File), D.Line)),
            FText::FromString(TEXT("Open the original DAS source")),
                              FOnActionTokenExecuted::CreateLambda([D, Open]() { Navigate(D, Open); }), false));
    return Result;
}
void DasDiagnosticEditor::Report(const FString& Text, FName Channel)
{
    const auto Diagnostics = FDasDiagnostic::Parse(Text);
    if (Diagnostics.IsEmpty())
    {
        FMessageLog(Channel).Error(FText::FromString(Text));
        return;
    }
    TSet<FString> Seen;
    for (const auto& D : Diagnostics)
    {
        const FString Key = D.Format();
        if (Seen.Contains(Key))
            continue;
        Seen.Add(Key);
        FMessageLog(Channel).AddMessage(Message(D));
    }
}
