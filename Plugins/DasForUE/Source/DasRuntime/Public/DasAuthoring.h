#pragma once
#include "CoreMinimal.h"
#include "DasSchema.h"

// Syntax-neutral class declarations. JSON is a transport; future daScript macros produce the same model.
struct DASRUNTIME_API FDasDeclaredType
{
    FString Kind = TEXT("int");
    FString TypePath;
    TSharedPtr<const FDasDeclaredType> Element, Key, Value;
    FString Identity() const;
};
struct FDasDeclaredLiteral
{
    FString Kind; // Empty means no explicit default; null/bool/number/string otherwise.
    double Number = 0;
    bool Boolean = false;
    FString String;
};
struct FDasDeclaredProperty
{
    FString Id, Name;
    FDasDeclaredType Type;
    FDasDeclaredLiteral Default;
    bool bEditable = true;
    bool bReadOnly = false;
    TMap<FString, FString> Metadata;
};
struct FDasDeclaredParameter
{
    FString Id, Name, Direction = TEXT("in");
    FDasDeclaredType Type;
};
struct FDasDeclaredMethod
{
    FString Id, Name;
    FDasDeclaredType ReturnType;
    TArray<FDasDeclaredParameter> Parameters;
    FString ScriptFile, EntryPoint = TEXT("main");
    bool bCallable = true, bPure = false, bStatic = false;
    TMap<FString, FString> Metadata;
};
struct FDasDeclaredClass
{
    FString Id, Name, NativeParent, ParentId;
    TArray<FDasDeclaredProperty> Properties;
    TArray<FDasDeclaredMethod> Methods;
    TMap<FString, FString> Metadata;
};
struct FDasAuthoringDelta
{
    bool bStructural = false, bDefaults = false, bMetadata = false, bImplementation = false;
    TArray<FString> Added, Removed;
};
struct DASRUNTIME_API FDasAuthoringModule
{
    FString Id;
    TArray<FDasDeclaredClass> Classes;
    bool Validate(FString& Error) const;
    static bool FromJson(const FString& Json, FDasAuthoringModule& Out, FString& Error);
    bool ToJson(FString& Out, FString& Error) const;
    // Preflight for the existing Actor adapter: all unsupported capabilities are refused explicitly.
    bool ToActorSchema(const FString& ClassId, FDasClassSchema& Out, FString& Error) const;
    static FDasAuthoringDelta Compare(const FDasDeclaredClass& Old, const FDasDeclaredClass& Next);
};
