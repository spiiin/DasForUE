#pragma once
#include "CoreMinimal.h"

struct FDasTypeDeclaration;
struct FInstancedStruct;
class UScriptStruct;class FProperty;class UEnum;
DASRUNTIME_API UEnum* ResolveDasEnum(const FString& Path,FString& Error);
DASRUNTIME_API bool ValidateDasEnumNumber(const FString& Path,int64 Value,FString& Error);
DASRUNTIME_API FProperty* FindDasStructField(UScriptStruct* Type,const FString& Name);
DASRUNTIME_API bool ValidateDasStructSnapshot(const FInstancedStruct& Value,FString& Error);
DASRUNTIME_API bool ParseDasStructDefault(const FString& Path,const FString& Text,FInstancedStruct& Out,FString& Error);
DASRUNTIME_API bool ValidateDasValueStruct(const FString& Path,FString& Error);
enum class EDasScalarKind : uint8 { Int32, Float32, Bool, String=5, Double=6, Name=7, Object=8, Struct=12, Enum=13, Event=14, Class=15, Delegate=16 };

struct FDasFieldSchema
{
    FName Name;
    EDasScalarKind Kind = EDasScalarKind::Int32;
    int32 IntDefault = 0;
    float FloatDefault = 0;
    bool BoolDefault = false;
    double DoubleDefault=0;FString StringDefault;FName NameDefault;
    FString StructPath;FString StructDefault;FString ObjectClass;FString EnumPath,EnumDefault;
    bool bInstanceEditable = true;
    FString StableId;
    FString DelegateWire;
    bool bReadOnly = false;
    TMap<FString,FString> Metadata;
};

enum class EDasMethodSignature : uint8 { Int, Float, Bool, Void, Frame, String, Double, Name, Object, Vector, Rotator, Transform, Struct, Enum, Delegate=16 };
struct FDasCallContract {
    EDasMethodSignature ReturnKind=EDasMethodSignature::Int;
    TArray<EDasMethodSignature> Inputs;
    TArray<FString> InputClasses;FString ReturnClass;TArray<int32> InputDirections;
    bool operator==(const FDasCallContract& Other) const {return ReturnKind==Other.ReturnKind && Inputs==Other.Inputs && InputClasses==Other.InputClasses && ReturnClass==Other.ReturnClass && InputDirections==Other.InputDirections;}
    bool operator!=(const FDasCallContract& Other) const {return !(*this==Other);}
    static FDasCallContract Scalar(EDasMethodSignature Kind){FDasCallContract C;C.ReturnKind=Kind;if(Kind!=EDasMethodSignature::Void) C.Inputs.Add(Kind);return C;}
};
struct FDasParameterSchema {FName Name;FString StableId;EDasMethodSignature Kind=EDasMethodSignature::Int;FString ClassPath;FString Direction=TEXT("in");};
// Legacy adapters and a bounded generic scalar call frame.
struct FDasMethodSchema
{
    EDasMethodSignature Signature = EDasMethodSignature::Int;
    EDasMethodSignature ReturnKind=EDasMethodSignature::Int;
    FString ReturnClass;
    TArray<FDasParameterSchema> Parameters;
    FDasCallContract Contract() const {if(Signature!=EDasMethodSignature::Frame) return FDasCallContract::Scalar(Signature);FDasCallContract C;C.ReturnKind=ReturnKind;C.ReturnClass=ReturnClass;bool Objects=false;for(const auto& P:Parameters){C.Inputs.Add(P.Kind);Objects |= P.Kind==EDasMethodSignature::Object || P.Kind==EDasMethodSignature::Struct || P.Kind==EDasMethodSignature::Enum || P.Kind==EDasMethodSignature::Delegate;}if(Objects)for(const auto& P:Parameters) C.InputClasses.Add(P.ClassPath);bool Outputs=false;for(const auto& P:Parameters)Outputs |= P.Direction!=TEXT("in");if(Outputs)for(const auto& P:Parameters)C.InputDirections.Add(P.Direction==TEXT("out")?1:P.Direction==TEXT("inout")?2:0);return C;}
    TArray<FDasParameterSchema> EffectiveParameters() const {if(Signature==EDasMethodSignature::Frame) return Parameters;if(Signature==EDasMethodSignature::Void) return {};return {{TEXT("Input"),InputStableId,Signature}};}
    FName Name = TEXT("Evaluate");
    FString ScriptFile;
    FString StableId;
    FString InputStableId;
    FString EntryPoint = TEXT("main");
    bool bBlueprintEvent = false;
    TMap<FString,FString> Metadata;
};

struct DASRUNTIME_API FDasClassSchema
{
    FName Name;
    FName Parent = TEXT("Actor");
    FString ParentStableId; // Immediate authored parent; fields/methods here are owned only.
    TArray<FDasFieldSchema> Fields;
    FDasMethodSchema Method;
    FString StableId;
    TArray<FDasMethodSchema> Methods;
    TArray<FString> Interfaces;
    TMap<FString,FString> Metadata;
    TArray<FDasMethodSchema> EffectiveMethods() const;

    bool Validate(FString& Error) const;
    // Adapter for the temporary macro wire format. The editor backend never parses it.
    static bool FromDeclaration(const FDasTypeDeclaration& Declaration, const FString& Script,
                                FDasClassSchema& Out, FString& Error);
};

// Object and interface references share checked object capabilities in the VM.
DASRUNTIME_API UClass* DasReferencePropertyClass(const FProperty* Property);
DASRUNTIME_API bool DasReferenceMatches(const UObject* Object,const UClass* Class);
DASRUNTIME_API UObject* DasReferenceGet(const FProperty* Property,const void* Memory);
DASRUNTIME_API bool DasReferenceSet(const FProperty* Property,void* Memory,UObject* Object);
// Shared enum frame access: Blueprint byte enums and strongly typed native enums.
DASRUNTIME_API UEnum* DasEnumPropertyType(FProperty* Property);
DASRUNTIME_API int64 DasEnumPropertyNumber(FProperty* Property,const void* Memory);
DASRUNTIME_API bool SetDasEnumPropertyNumber(FProperty* Property,void* Memory,int64 Value);
