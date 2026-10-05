#include "DasSchema.h"
#include "DasRuntime.h"
#include "Misc/Paths.h"

namespace
{
    bool Identifier(const FString& Value)
    {
        if (Value.IsEmpty() || Value.Len() >= NAME_SIZE || FChar::IsDigit(Value[0])) return false;
        for (TCHAR Ch : Value) if (!FChar::IsAlnum(Ch) && Ch != TEXT('_')) return false;
        return FName(*Value) != NAME_None;
    }
}

bool FDasClassSchema::Validate(FString& Error) const
{
    Error.Reset();
    if (!Identifier(Name.ToString()) || Name.IsNone() || Parent != TEXT("Actor"))
    { Error = TEXT("Expected a named Actor-derived schema"); return false; }
    if (!Identifier(Method.Name.ToString()) || Method.Name.IsNone() ||
        Method.ScriptFile.IsEmpty() || FPaths::GetCleanFilename(Method.ScriptFile) != Method.ScriptFile ||
        Method.ScriptFile.Contains(TEXT(":")) || !Method.ScriptFile.EndsWith(TEXT(".das")))
    { Error = TEXT("Expected a method name and a .das filename without directories"); return false; }
    TSet<FName> Names;
    for (const auto& Field : Fields)
    {
        if (Field.Name.IsNone() || !Identifier(Field.Name.ToString()) || Names.Contains(Field.Name) || Field.Name == Method.Name)
        { Error = FString::Printf(TEXT("Invalid or duplicate field: %s"), *Field.Name.ToString()); return false; }
        if ((Field.Kind != EDasScalarKind::Int32 && Field.Kind != EDasScalarKind::Float32) ||
            (Field.Kind == EDasScalarKind::Float32 && !FMath::IsFinite(Field.FloatDefault)))
        { Error = TEXT("Unsupported scalar kind or non-finite default"); return false; }
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
