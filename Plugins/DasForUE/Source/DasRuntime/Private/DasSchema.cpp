#include "DasSchema.h"
#include "DasRuntime.h"
#include "Misc/Paths.h"
#include "UObject/Class.h"
#include "StructUtils/InstancedStruct.h"

namespace
{
    bool Identifier(const FString& Value)
    {
        if (Value.IsEmpty() || Value.Len() >= NAME_SIZE || FChar::IsDigit(Value[0])) return false;
        for (TCHAR Ch : Value) if (!FChar::IsAlnum(Ch) && Ch != TEXT('_')) return false;
        return FName(*Value) != NAME_None;
    }
}

TArray<FDasMethodSchema> FDasClassSchema::EffectiveMethods() const
{ return Methods.IsEmpty()?TArray<FDasMethodSchema>{Method}:Methods; }

bool FDasClassSchema::Validate(FString& Error) const
{
    Error.Reset();
    if (!Identifier(Name.ToString()) || Name.IsNone() || Parent != TEXT("Actor"))
    { Error = TEXT("Expected a named Actor-derived schema"); return false; }
    auto ValidMeta=[&](const TMap<FString,FString>& Values)
    {
        for (const auto& Pair:Values)
            if ((Pair.Key!=TEXT("Category") && Pair.Key!=TEXT("DisplayName") && Pair.Key!=TEXT("ToolTip")) || Pair.Value.Len()>4096)
            {Error=TEXT("Unsupported adapter metadata");return false;}
        return true;
    };
    if (!ValidMeta(Metadata)) return false;
    TSet<FName> Names;
    const auto AllMethods=EffectiveMethods();
    if (AllMethods.Num()>64) {Error=TEXT("Adapter method count exceeds 64");return false;}
    for (const auto& M:AllMethods)
    {
        if (!Identifier(M.Name.ToString()) || M.Name.IsNone() || Names.Contains(M.Name) || !Identifier(M.EntryPoint) ||
            M.ScriptFile.IsEmpty() || FPaths::GetCleanFilename(M.ScriptFile)!=M.ScriptFile || M.ScriptFile.Contains(TEXT(":")) || !M.ScriptFile.EndsWith(TEXT(".das")))
        {Error=TEXT("Invalid method name, entry point or local script filename");return false;}
        if (M.Signature!=EDasMethodSignature::Int && M.Signature!=EDasMethodSignature::Float && M.Signature!=EDasMethodSignature::Bool && M.Signature!=EDasMethodSignature::Void && M.Signature!=EDasMethodSignature::Frame) {Error=TEXT("Unsupported method signature");return false;}
        const auto Contract=M.Contract();if(Contract.Inputs.Num()>16 || (static_cast<uint8>(Contract.ReturnKind)>12 || Contract.ReturnKind==EDasMethodSignature::Frame)){Error=TEXT("Invalid scalar frame contract");return false;}
        TSet<FName> ParameterNames;for(const auto& P:M.EffectiveParameters()){if(!Identifier(P.Name.ToString()) || P.Name==TEXT("ReturnValue") || ParameterNames.Contains(P.Name) || (static_cast<uint8>(P.Kind)>12 || P.Kind==EDasMethodSignature::Void || P.Kind==EDasMethodSignature::Frame)){Error=TEXT("Invalid scalar frame parameter");return false;}ParameterNames.Add(P.Name);}
        for(const auto& P:M.EffectiveParameters())if(P.Kind==EDasMethodSignature::Object && (!P.ClassPath.StartsWith(TEXT("/Script/")) || !FindObject<UClass>(nullptr,*P.ClassPath))){Error=TEXT("Object parameter requires loaded native class path");return false;}
        if(Contract.ReturnKind==EDasMethodSignature::Object && (!M.ReturnClass.StartsWith(TEXT("/Script/")) || !FindObject<UClass>(nullptr,*M.ReturnClass))){Error=TEXT("Object result requires loaded native class path");return false;}
        for(const auto& P:M.EffectiveParameters())if(P.Kind==EDasMethodSignature::Struct && !ValidateDasValueStruct(P.ClassPath,Error))return false;
        if(Contract.ReturnKind==EDasMethodSignature::Struct && !ValidateDasValueStruct(M.ReturnClass,Error))return false;
        if (!ValidMeta(M.Metadata)) return false;
        Names.Add(M.Name);
    }
    for (const auto& Field : Fields)
    {
        if (Field.Name.IsNone() || !Identifier(Field.Name.ToString()) || Names.Contains(Field.Name))
        { Error = FString::Printf(TEXT("Invalid or duplicate field: %s"), *Field.Name.ToString()); return false; }
        if ((Field.Kind != EDasScalarKind::Int32 && Field.Kind != EDasScalarKind::Float32 && Field.Kind != EDasScalarKind::Bool && Field.Kind!=EDasScalarKind::String && Field.Kind!=EDasScalarKind::Double && Field.Kind!=EDasScalarKind::Name && Field.Kind!=EDasScalarKind::Struct) ||
            ((Field.Kind == EDasScalarKind::Float32 && !FMath::IsFinite(Field.FloatDefault)) || (Field.Kind==EDasScalarKind::Double && !FMath::IsFinite(Field.DoubleDefault))))
        { Error = TEXT("Unsupported scalar kind or non-finite default"); return false; }
        if(Field.Kind==EDasScalarKind::Struct){FInstancedStruct Value;if(!ParseDasStructDefault(Field.StructPath,Field.StructDefault,Value,Error))return false;}
        if (!ValidMeta(Field.Metadata)) return false;
        Names.Add(Field.Name);
    }
    return true;
}

bool FDasClassSchema::FromDeclaration(const FDasTypeDeclaration& Declaration, const FString& Script,
                                    FDasClassSchema& Out, FString& Error)
{
    Error.Reset();
    if (!Identifier(Declaration.Name) || !Identifier(Declaration.Parent))
    { Error = TEXT("Invalid declared type name"); return false; }
    FDasClassSchema Candidate;
    Candidate.Name = *Declaration.Name;
    Candidate.Parent = *Declaration.Parent;
    Candidate.Method.ScriptFile = Script;
    TArray<FString> Items;
    Declaration.Fields.ParseIntoArray(Items, TEXT(";"));
    for (const auto& Item : Items)
    {
        FString Name, Kind;
        if (!Item.Split(TEXT(":"), &Name, &Kind) || !Identifier(Name))
        { Error = FString::Printf(TEXT("Invalid field declaration: %s"), *Item); return false; }
        FDasFieldSchema Field;
        Field.Name = *Name;
        if (Kind == TEXT("int")) Field.Kind = EDasScalarKind::Int32;
        else if (Kind == TEXT("float")) Field.Kind = EDasScalarKind::Float32;
        else { Error = FString::Printf(TEXT("Unsupported field type: %s"), *Kind); return false; }
        Candidate.Fields.Add(Field);
    }
    if (!Candidate.Validate(Error)) return false;
    Out = MoveTemp(Candidate);
    return true;
}
