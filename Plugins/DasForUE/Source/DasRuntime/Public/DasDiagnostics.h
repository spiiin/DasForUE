#pragma once
#include "CoreMinimal.h"

// Copied source coordinates, never compiler AST or debug-info pointers.
struct DASRUNTIME_API FDasSourceLocation
{
    FString File, Entity;
    int32 Line = 0, Column = 0, EndLine = 0;
};
struct DASRUNTIME_API FDasDiagnostic : FDasSourceLocation
{
    FString Phase, Message, Details;
    FString Format() const;
    static TArray<FDasDiagnostic> Parse(const FString& Text);
};
DASRUNTIME_API FDasSourceLocation DasDiagnosticSource(const TArray<FDasSourceLocation>& Sources, const FString& File,
                                                      int32 Line, const FString& EntityHint = FString());

DASRUNTIME_API void DasCaptureDiagnosticSources(const TArray<FDasSourceLocation>& Sources);

// Import transactions collect the maps of their compilations. Schema/import errors without
// expression coordinates point to a declaration; they never invent an expression line.
class DASRUNTIME_API FDasDiagnosticCapture
{
public:
    FDasDiagnosticCapture(FString& Error, const FString& File, const FString& EntityHint = FString());
    ~FDasDiagnosticCapture();
    FDasDiagnosticCapture(const FDasDiagnosticCapture&) = delete;
    FDasDiagnosticCapture& operator=(const FDasDiagnosticCapture&) = delete;

private:
    friend DASRUNTIME_API void DasCaptureDiagnosticSources(const TArray<FDasSourceLocation>&);
    FString& Error;
    FString File, Entity;
    TArray<FDasSourceLocation> Sources;
    FDasDiagnosticCapture* Previous = nullptr;
};
DECLARE_MULTICAST_DELEGATE_OneParam(FDasDiagnosticEvent, const FDasDiagnostic&);
DASRUNTIME_API FDasDiagnosticEvent& DasDiagnosticEvents();
