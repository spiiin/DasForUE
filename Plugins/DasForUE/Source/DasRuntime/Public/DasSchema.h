#pragma once
#include "CoreMinimal.h"

struct FDasTypeDeclaration;
struct FInstancedStruct;
DASRUNTIME_API bool ValidateDasStructSnapshot(const FInstancedStruct& Value,FString& Error);
DASRUNTIME_API bool ParseDasStructDefault(const FString& Path,const FString& Text,FInstancedStruct& Out,FString& Error);
DASRUNTIME_API bool ValidateDasValueStruct(const FString& Path,FString& Error);
enum class EDasScalarKind : uint8 { Int32, Float32, Bool, String=5, Double=6, Name=7, Object=8, Struct=12 };

struct FDasFieldSchema
{
    FName Name;
    EDasScalarKind Kind = EDasScalarKind::Int32;
    int32 IntDefault = 0;
    float FloatDefault = 0;
    bool BoolDefault = false;
    double DoubleDefault=0;FString StringDefault;FName NameDefault;
    FString StructPath;FString StructDefault;FString ObjectClass;
    bool bInstanceEditable = true;
    FString StableId;
    bool bReadOnly = false;
    TMap<FString,FString> Metadata;
};

enum class EDasMethodSignature : uint8 { Int, Float, Bool, Void, Frame, String, Double, Name, Object, Vector, Rotator, Transform, Struct };
struct FDasCallContract {
    EDasMethodSignature ReturnKind=EDasMethodSignature::Int;
    TArray<EDasMethodSignature> Inputs;
    TArray<FString> InputClasses;FString ReturnClass;
    bool operator==(const FDasCallContract& Other) const {return ReturnKind==Other.ReturnKind && Inputs==Other.Inputs && InputClasses==Other.InputClasses && ReturnClass==Other.ReturnClass;}
    bool operator!=(const FDasCallContract& Other) const {return !(*this==Other);}
    static FDasCallContract Scalar(EDasMethodSignature Kind){FDasCallContract C;C.ReturnKind=Kind;if(Kind!=EDasMethodSignature::Void) C.Inputs.Add(Kind);return C;}
};
struct FDasParameterSchema {FName Name;FString StableId;EDasMethodSignature Kind=EDasMethodSignature::Int;FString ClassPath;};
// Legacy adapters and a bounded generic scalar call frame.
struct FDasMethodSchema
{
    EDasMethodSignature Signature = EDasMethodSignature::Int;
    EDasMethodSignature ReturnKind=EDasMethodSignature::Int;
    FString ReturnClass;
    TArray<FDasParameterSchema> Parameters;
    FDasCallContract Contract() const {if(Signature!=EDasMethodSignature::Frame) return FDasCallContract::Scalar(Signature);FDasCallContract C;C.ReturnKind=ReturnKind;C.ReturnClass=ReturnClass;bool Objects=false;for(const auto& P:Parameters){C.Inputs.Add(P.Kind);Objects |= P.Kind==EDasMethodSignature::Object || P.Kind==EDasMethodSignature::Struct;}if(Objects)for(const auto& P:Parameters) C.InputClasses.Add(P.ClassPath);return C;}
    TArray<FDasParameterSchema> EffectiveParameters() const {if(Signature==EDasMethodSignature::Frame) return Parameters;if(Signature==EDasMethodSignature::Void) return {};return {{TEXT("Input"),InputStableId,Signature}};}
    FName Name = TEXT("Evaluate");
    FString ScriptFile;
    FString StableId;
    FString InputStableId;
    FString EntryPoint = TEXT("main");
    TMap<FString,FString> Metadata;
};

struct DASRUNTIME_API FDasClassSchema
{
    FName Name;
    FName Parent = TEXT("Actor");
    TArray<FDasFieldSchema> Fields;
    FDasMethodSchema Method;
    FString StableId;
    TArray<FDasMethodSchema> Methods;
    TMap<FString,FString> Metadata;
    TArray<FDasMethodSchema> EffectiveMethods() const;

    bool Validate(FString& Error) const;
    // Adapter for the temporary macro wire format. The editor backend never parses it.
    static bool FromDeclaration(const FDasTypeDeclaration& Declaration, const FString& Script,
                                FDasClassSchema& Out, FString& Error);
};
