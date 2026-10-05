#pragma once
#include "CoreMinimal.h"

struct FDasTypeDeclaration;
enum class EDasScalarKind : uint8 { Int32, Float32 };

struct FDasFieldSchema
{
    FName Name;
    EDasScalarKind Kind = EDasScalarKind::Int32;
    int32 IntDefault = 0;
    float FloatDefault = 0;
    bool bInstanceEditable = true;
};

// Only the int32 -> int32 method signature is supported by this first IR version.
struct FDasMethodSchema
{
    FName Name = TEXT("Evaluate");
    FString ScriptFile;
};

struct DASRUNTIME_API FDasClassSchema
{
    FName Name;
    FName Parent = TEXT("Actor");
    TArray<FDasFieldSchema> Fields;
    FDasMethodSchema Method;

    bool Validate(FString& Error) const;
    // Adapter for the temporary macro wire format. The editor backend never parses it.
    static bool FromDeclaration(const FDasTypeDeclaration& Declaration, const FString& Script,
                                FDasClassSchema& Out, FString& Error);
};
