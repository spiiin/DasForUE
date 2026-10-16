#include "DasSchema.h"
#include "DasAuthoring.h"
#include "DasDelegateSignatures.h"
#include "UObject/StrongObjectPtr.h"
#include "UObject/Package.h"
#include "DasInterfaceContracts.h"
#include "DasContainerTypes.h"
#include "DasRuntime.h"
#include "Misc/Paths.h"
#include "Misc/PackageName.h"
#include "UObject/Class.h"
#include "StructUtils/InstancedStruct.h"
#include "UObject/SoftObjectPath.h"

namespace
{
    bool Identifier(const FString& Value)
    {
        if (Value.IsEmpty() || Value.Len() >= NAME_SIZE || FChar::IsDigit(Value[0])) return false;
        for (TCHAR Ch : Value) if (!FChar::IsAlnum(Ch) && Ch != TEXT('_')) return false;
        return FName(*Value) != NAME_None;
    }
}

bool MatchesDasStructDefault(UScriptStruct* Type,const void* Actual,const void* Expected){
 if(Type->CompareScriptStruct(Actual,Expected,0))return true;
 if(Type!=TBaseStructure<FTransform>::Get())return false;
 const auto& Value=*static_cast<const FTransform*>(Expected);
 // Blueprint variable defaults serialize rotation as Euler angles. Reproduce that
 // exact conversion rather than accepting an arbitrary numerical tolerance.
 const FTransform BlueprintValue(Value.Rotator(),Value.GetTranslation(),Value.GetScale3D());
 return Type->CompareScriptStruct(Actual,&BlueprintValue,0);
}

bool ResolveDasReferenceDefault(bool IsClass,const FString& BasePath,const FString& Path,UObject*& Out,FString& Error){
 Out=nullptr;if(Path.IsEmpty())return true;
 if(Path.Len()>4096 || !FPackageName::IsValidObjectPath(Path)){Error=TEXT("Default requires a valid object path");return false;}
 auto* Base=FindObject<UClass>(nullptr,*BasePath);FSoftObjectPath ObjectPath(Path);
 if(!Base || !ObjectPath.IsValid() || !ObjectPath.GetSubPathString().IsEmpty() || Path.Len()>4096){Error=TEXT("Default requires a valid top-level object path and loaded base class");return false;}
 Out=LoadObject<UObject>(nullptr,*Path,nullptr,LOAD_NoWarn);
 bool Good=IsValid(Out);
 if(IsClass){auto* Class=Cast<UClass>(Out);Good &= Class && Class->IsChildOf(Base) && !Class->HasAnyClassFlags(CLASS_Deprecated|CLASS_NewerVersionExists);}
 else Good &= Out && Out->IsAsset() && !Out->IsA<UClass>() && (Base->HasAnyClassFlags(CLASS_Interface)?Out->GetClass()->ImplementsInterface(Base):Out->IsA(Base));
 if(!Good){Out=nullptr;Error=TEXT("Default asset/class is missing, obsolete or incompatible: ")+Path;return false;}return true;
}

TArray<FDasMethodSchema> FDasClassSchema::EffectiveMethods() const
{ return Methods.IsEmpty()?(Method.ScriptFile.IsEmpty()?TArray<FDasMethodSchema>{}:TArray<FDasMethodSchema>{Method}):Methods; }

bool FDasClassSchema::Validate(FString& Error) const
{
    Error.Reset();
    if (!Identifier(Name.ToString()) || Name.IsNone() || (Parent != TEXT("Actor") && Parent != TEXT("ActorComponent")))
    { Error = TEXT("Expected a named Actor or ActorComponent schema"); return false; }
    auto ValidMeta=[&](const TMap<FString,FString>& Values,bool Method=false)
    {
        for (const auto& Pair:Values)
            if ((Pair.Key!=TEXT("Category") && Pair.Key!=TEXT("DisplayName") && Pair.Key!=TEXT("ToolTip") && !(Method && Pair.Key==TEXT("DasFunctionKind") && (Pair.Value==TEXT("Event") || Pair.Value==TEXT("Callable")))) || Pair.Value.Len()>4096)
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
        const auto Contract=M.Contract();if(Contract.Inputs.Num()>16 || ((static_cast<uint8>(Contract.ReturnKind)>13 && Contract.ReturnKind!=EDasMethodSignature::Class && Contract.ReturnKind!=EDasMethodSignature::Delegate) || Contract.ReturnKind==EDasMethodSignature::Frame)){Error=TEXT("Invalid scalar frame contract");return false;}
        if(M.bPure && Contract.ReturnKind==EDasMethodSignature::Void && !M.EffectiveParameters().ContainsByPredicate([](const auto& P){return P.Direction!=TEXT("in");})){Error=TEXT("BlueprintPure requires a result or output parameter");return false;}
        TSet<FName> ParameterNames;for(const auto& P:M.EffectiveParameters()){if((P.Direction!=TEXT("in") && P.Direction!=TEXT("out") && P.Direction!=TEXT("inout")) || !Identifier(P.Name.ToString()) || P.Name==TEXT("ReturnValue") || ParameterNames.Contains(P.Name) || ((static_cast<uint8>(P.Kind)>13 && P.Kind!=EDasMethodSignature::Class && P.Kind!=EDasMethodSignature::Delegate) || P.Kind==EDasMethodSignature::Void || P.Kind==EDasMethodSignature::Frame)){Error=TEXT("Invalid scalar frame parameter");return false;}ParameterNames.Add(P.Name);}
        for(const auto& P:M.EffectiveParameters())if(P.Kind==EDasMethodSignature::Object && (!FindObject<UClass>(nullptr,*P.ClassPath) || (!P.ClassPath.StartsWith(TEXT("/Script/")) && !FindObject<UClass>(nullptr,*P.ClassPath)->HasAnyClassFlags(CLASS_Interface)))){Error=TEXT("Object parameter requires loaded native class path");return false;}
        if(Contract.ReturnKind==EDasMethodSignature::Object && (!FindObject<UClass>(nullptr,*M.ReturnClass) || (!M.ReturnClass.StartsWith(TEXT("/Script/")) && !FindObject<UClass>(nullptr,*M.ReturnClass)->HasAnyClassFlags(CLASS_Interface)))){Error=TEXT("Object result requires loaded native class path");return false;}
        auto ClassBase=[](const FString& Path){auto* B=FindObject<UClass>(nullptr,*Path);return Path.StartsWith(TEXT("/Script/")) && IsValid(B) && !B->HasAnyClassFlags(CLASS_Interface|CLASS_Deprecated|CLASS_NewerVersionExists);};
        for(const auto& P:M.EffectiveParameters())if(P.Kind==EDasMethodSignature::Class && !ClassBase(P.ClassPath)){Error=TEXT("Class parameter requires a loaded native base class");return false;}
        if(Contract.ReturnKind==EDasMethodSignature::Class && !ClassBase(M.ReturnClass)){Error=TEXT("Class result requires a loaded native base class");return false;}
        for(const auto& P:M.EffectiveParameters())if(P.Kind==EDasMethodSignature::Struct && !ValidateDasValueStruct(P.ClassPath,Error))return false;
        if(Contract.ReturnKind==EDasMethodSignature::Struct && !ValidateDasValueStruct(M.ReturnClass,Error))return false;
        for(const auto& P:M.EffectiveParameters())if(P.Kind==EDasMethodSignature::Enum && !ResolveDasEnum(P.ClassPath,Error))return false;
        if(Contract.ReturnKind==EDasMethodSignature::Enum && !ResolveDasEnum(M.ReturnClass,Error))return false;
        if(Contract.ReturnKind==EDasMethodSignature::Delegate){TStrongObjectPtr<UFunction> Sig(MakeDasSingleDelegateSignature(M.ReturnClass,Error));if(!Sig)return false;}
        for(const auto& P:M.EffectiveParameters())if(P.Kind==EDasMethodSignature::Delegate){if(P.Direction!=TEXT("in")){Error=TEXT("Delegate-valued ref/out parameters are unsupported");return false;}TStrongObjectPtr<UFunction> Sig(MakeDasSingleDelegateSignature(P.ClassPath,Error));if(!Sig)return false;}
        if (!ValidMeta(M.Metadata,true)) return false;
        Names.Add(M.Name);
    }
    for (const auto& Field : Fields)
    {
        if (Field.Name.IsNone() || !Identifier(Field.Name.ToString()) || Names.Contains(Field.Name))
        { Error = FString::Printf(TEXT("Invalid or duplicate field: %s"), *Field.Name.ToString()); return false; }
        if ((Field.Kind != EDasScalarKind::Int32 && Field.Kind != EDasScalarKind::Float32 && Field.Kind != EDasScalarKind::Bool && Field.Kind!=EDasScalarKind::String && Field.Kind!=EDasScalarKind::Double && Field.Kind!=EDasScalarKind::Name && Field.Kind!=EDasScalarKind::Struct && Field.Kind!=EDasScalarKind::Object && Field.Kind!=EDasScalarKind::Enum && Field.Kind!=EDasScalarKind::Event && Field.Kind!=EDasScalarKind::Class && Field.Kind!=EDasScalarKind::Delegate) ||
            ((Field.Kind == EDasScalarKind::Float32 && !FMath::IsFinite(Field.FloatDefault)) || (Field.Kind==EDasScalarKind::Double && !FMath::IsFinite(Field.DoubleDefault))))
        { Error = TEXT("Unsupported scalar kind or non-finite default"); return false; }
        if(Field.Kind==EDasScalarKind::Event || Field.Kind==EDasScalarKind::Delegate){FDasAuthoringModule Definition;if(!FDasAuthoringModule::FromJson(Field.DelegateWire,Definition,Error) || Definition.Classes.Num()!=1 || Definition.Classes[0].Methods.Num()!=1)return false;TStrongObjectPtr<UFunction> Signature(MakeDasDelegateSignature(Definition.Classes[0].Methods[0],Field.Kind==EDasScalarKind::Event,GetTransientPackage(),NAME_None,Error));if(!Signature)return false;if(Field.bInstanceEditable || (Field.Kind==EDasScalarKind::Event && Field.bReadOnly)){Error=TEXT("Delegate fields cannot be instance editable; events cannot be readonly");return false;}}
        if(Field.Kind==EDasScalarKind::Enum){auto* E=ResolveDasEnum(Field.EnumPath,Error);if(!E || !ValidateDasEnumNumber(Field.EnumPath,E->GetValueByNameString(Field.EnumDefault),Error))return false;}
        if(Field.Kind==EDasScalarKind::Object && (!FindObject<UClass>(nullptr,*Field.ObjectClass) || (!Field.ObjectClass.StartsWith(TEXT("/Script/")) && !FindObject<UClass>(nullptr,*Field.ObjectClass)->HasAnyClassFlags(CLASS_Interface)))){Error=TEXT("Object field requires a loaded native class path");return false;}
        if(Field.Kind==EDasScalarKind::Class){auto* Base=FindObject<UClass>(nullptr,*Field.ObjectClass);if(!Field.ObjectClass.StartsWith(TEXT("/Script/")) || !Base || Base->HasAnyClassFlags(CLASS_Interface)){Error=TEXT("Class field requires loaded native base class");return false;}}
        if((Field.Kind==EDasScalarKind::Class || Field.Kind==EDasScalarKind::Object) && !Field.StringDefault.IsEmpty()){UObject* Value=nullptr;if(!ResolveDasReferenceDefault(Field.Kind==EDasScalarKind::Class,Field.ObjectClass,Field.StringDefault,Value,Error))return false;}
        if(Field.Kind==EDasScalarKind::Struct){FInstancedStruct Value;if(!ParseDasStructDefault(Field.StructPath,Field.StructDefault,Value,Error))return false;}
        if (!ValidMeta(Field.Metadata)) return false;
        Names.Add(Field.Name);
    }
    return ValidateDasInterfaceContracts(*this,Error);
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
