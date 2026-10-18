#pragma once
#include "CoreMinimal.h"
#include "DasSchema.h"

enum class EDasTypeKind : uint8 { Invalid,Void,Byte,Text,Int,Float,Double,Bool,String,Name,Object,Class,Struct,Enum,Array,Set,Map,Delegate,Event };
enum class EDasEnumStorage : uint8 { Automatic,Byte,Enum8 };
DASRUNTIME_API EDasTypeKind DasTypeKind(const FString& Name);
DASRUNTIME_API FString DasTypeKindName(EDasTypeKind Kind);

// Syntax-neutral class declarations. JSON is a transport; future daScript macros produce the same model.
struct DASRUNTIME_API FDasDeclaredType
{
    FString Kind = TEXT("int");
    FString TypePath;
    FString DelegateWire; // Single-cast method value signature; not a UObject path.
    FString AuthoredId; // Optional authored object identity, separate from host storage.
    TSharedPtr<const FDasDeclaredType> Element, Key, Value;
    FString Identity() const;
    EDasTypeKind GetKind() const { return DasTypeKind(Kind); }
    // Physical reflection detail, not authored syntax/JSON. Describe preserves
    // it so native byte enums and enum-class fields roundtrip without guessing.
    EDasEnumStorage EnumStorage = EDasEnumStorage::Automatic;
    bool WeakReference = false; // Physical native storage; nominal DSL object type is unchanged.
    FString StorageIdentity() const;
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
    FDasDeclaredLiteral Default; // Optional literal for an input parameter.
    FDasDeclaredType Type;
};
struct FDasDeclaredMethod
{
    FString Id, Name;
    FDasDeclaredType ReturnType;
    TArray<FDasDeclaredParameter> Parameters;
    FString ScriptFile, EntryPoint = TEXT("main");
    FString WorldContext; // Optional input object parameter of a static library method.
    bool bCallable = true, bPure = false, bStatic = false, bConst = false;
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
    FString ParameterTypePath;
    bool operator==(const FDasDeclaredCallback& O)const{return Name==O.Name && Kind==O.Kind && Source==O.Source && EventName==O.EventName && (Kind!=TEXT("event") || (ParameterKind==O.ParameterKind && ParameterTypePath==O.ParameterTypePath));}
};
struct FDasPropertyDefaultOverride {
    FString Id,Name;
    FDasDeclaredLiteral Default;
    bool operator==(const FDasPropertyDefaultOverride& O)const{return Id==O.Id && Name==O.Name && Default==O.Default;}
};
struct FDasNativeDefaultGroup {
    FString Component; // Empty: native host property; otherwise a native subobject reference.
    TMap<FString,FDasDeclaredLiteral> Defaults;
    bool operator==(const FDasNativeDefaultGroup& O)const{return Component==O.Component && Defaults.OrderIndependentCompareEqual(O.Defaults);}
};
struct FDasDeclaredClass
{
    FString Id, Name, NativeParent, ParentId, SourceScript;
    FString LibraryTypeRegistry;
    TArray<FString> Interfaces;
    TArray<FDasDeclaredComponent> Components;
    TArray<FDasComponentOverride> ComponentOverrides;
    TArray<FDasPropertyDefaultOverride> PropertyOverrides;
    TArray<FDasNativeDefaultGroup> NativeDefaults;
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
DASRUNTIME_API FString DasDefaultSourceContract(const FDasDeclaredProperty& Property);
struct DASRUNTIME_API FDasAuthoringModule
{
    FString Id;
    TArray<FDasDeclaredClass> Classes;
    bool Validate(FString& Error) const;
    static bool DefaultsOnly(const FDasAuthoringModule& Before,const FDasAuthoringModule& After,FString& Error);
    // Effective view; source declarations retain only their own members.
    bool ResolveClass(const FString& ClassId,FDasDeclaredClass& Out,FString& Error) const;
    static bool FromJson(const FString& Json, FDasAuthoringModule& Out, FString& Error);
    bool ToJson(FString& Out, FString& Error) const;
    // Preflight for the existing Actor adapter: all unsupported capabilities are refused explicitly.
    bool ToActorSchema(const FString& ClassId, FDasClassSchema& Out, FString& Error,bool IncludeInherited=false) const;
    static FDasAuthoringDelta Compare(const FDasDeclaredClass& Old, const FDasDeclaredClass& Next);
};
