#pragma once
#include "DasDiagnostics.h"
#include "Logging/TokenizedMessage.h"
DECLARE_DELEGATE_RetVal_ThreeParams(bool, FDasOpenDiagnosticSource, const FString&, int32, int32);
namespace DasDiagnosticEditor
{
TSharedRef<FTokenizedMessage> Message(const FDasDiagnostic& Diagnostic, FDasOpenDiagnosticSource Open = {});
bool Navigate(const FDasDiagnostic& Diagnostic, const FDasOpenDiagnosticSource& Open = {});
void Report(const FString& Text, FName Channel = TEXT("BlueprintLog"));
}
