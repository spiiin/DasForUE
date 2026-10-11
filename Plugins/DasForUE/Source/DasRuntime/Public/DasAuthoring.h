#pragma once
#include "CoreMinimal.h"
#include "DasSchema.h"

// Syntax-neutral class declarations. JSON is a transport; future daScript macros produce the same model.
struct DASRUNTIME_API FDasDeclaredType
{
    FString Kind = TEXT("int");
    FString TypePath;
    FString DelegateWire; // Single-cast method value signature; not a UObject path.
    FString AuthoredId; // Optional authored object identity, separate from host storage.
    TSharedPtr<const FDasDeclaredType> Element, Key, Value;
    FString Identity() const;
};
struct FDasDeclaredLiteral
{
    FString Kind; // Empty means no explicit default; null/bool/number/string otherwise.
    double Number = 0;
    bool Boolean = false;
    bool operator==(const FDasDeclaredLiteral& O)const{return Kind==O.Kind && Number==O.Number && Boolean==O.Boolean && String==O.String;}
    FString String;
};
struct FDasDeclaredProperty
{
    FString Id, Name;
    FDasDeclaredType Type;
    FDasDeclaredLiteral Default;
    FString DelegateWire; // Authoring-only signature contract for event fields.
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
    bool bBlueprintEvent = false;
    TMap<FString, FString> Metadata;
};
struct FDasDeclaredComponent
{
    FString Id,Name,ScriptFile,ClassName;
    TMap<FString,FDasDeclaredLiteral> Defaults;
    FString Attach, AttachSocket;
    bool bRoot=false;
    FString NativeClass; // Native component path; mutually exclusive with authored source.
    bool bReadOnly=false,bEditable=false,bVisible=false;
    TMap<FString,FString> Metadata;
    bool operator==(const FDasDeclaredComponent& Other)const{return Id==Other.Id && Name==Other.Name && ScriptFile==Other.ScriptFile && ClassName==Other.ClassName && NativeClass==Other.NativeClass && Attach==Other.Attach && AttachSocket==Other.AttachSocket && bRoot==Other.bRoot && Defaults.OrderIndependentCompareEqual(Other.Defaults) && bReadOnly==Other.bReadOnly && bEditable==Other.bEditable && bVisible==Other.bVisible && Metadata.OrderIndependentCompareEqual(Other.Metadata);}
};
struct FDasComponentOverride {
    FString Id,Name; // Stable identity of an inherited SCS node.
    TMap<FString,FDasDeclaredLiteral> Defaults;
    bool operator==(const FDasComponentOverride& O)const{return Id==O.Id && Name==O.Name && Defaults.OrderIndependentCompareEqual(O.Defaults);}
};
struct FDasDeclaredCallback {
    FString Name,Kind,Source,EventName;
    FString ParameterKind=TEXT("int");
    bool operator==(const FDasDeclaredCallback& O)const{return Name==O.Name && Kind==O.Kind && Source==O.Source && EventName==O.EventName && (Kind!=TEXT("event") || ParameterKind==O.ParameterKind);}
};
struct FDasDeclaredClass
{
    FString Id, Name, NativeParent, ParentId, SourceScript;
    TArray<FString> Interfaces;
    TArray<FDasDeclaredComponent> Components;
    TArray<FDasComponentOverride> ComponentOverrides;
    TArray<FDasDeclaredCallback> Callbacks;
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
    // Effective view; source declarations retain only their own members.
    bool ResolveClass(const FString& ClassId,FDasDeclaredClass& Out,FString& Error) const;
    static bool FromJson(const FString& Json, FDasAuthoringModule& Out, FString& Error);
    bool ToJson(FString& Out, FString& Error) const;
    // Preflight for the existing Actor adapter: all unsupported capabilities are refused explicitly.
    bool ToActorSchema(const FString& ClassId, FDasClassSchema& Out, FString& Error) const;
    static FDasAuthoringDelta Compare(const FDasDeclaredClass& Old, const FDasDeclaredClass& Next);
};
