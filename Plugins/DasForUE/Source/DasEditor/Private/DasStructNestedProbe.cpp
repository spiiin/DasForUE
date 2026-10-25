#include "DasActor.h"
#include "DasTypeService.h"
#include "DasTypeRegistry.h"
#include "DasRuntime.h"
#include "StructUtils/InstancedStruct.h"
#include "Engine/Blueprint.h"
#include "StructUtils/UserDefinedStruct.h"
#include "UserDefinedStructure/UserDefinedStructEditorData.h"
#include "Kismet2/StructureEditorUtils.h"
#include "EdGraphSchema_K2.h"
#include "UObject/SavePackage.h"
#include "Misc/PackageName.h"
#include "Misc/CommandLine.h"
#include "Misc/Parse.h"
namespace
{
bool SaveNestedType(UObject* O)
{
    FSavePackageArgs A;
    A.TopLevelFlags = RF_Public | RF_Standalone;
    A.SaveFlags = SAVE_NoError;
    return UPackage::SavePackage(O->GetOutermost(), O,
                                 *FPackageName::LongPackageNameToFilename(O->GetOutermost()->GetName(),
                                                                          FPackageName::GetAssetPackageExtension()),
                                 A);
}
void* NestedInner(ADasActor* A, const TCHAR* Side, UScriptStruct*& Type)
{
    auto* Bundle = FindFProperty<FStructProperty>(A->GetClass(), TEXT("Bundle"));
    if (!Bundle)
        return nullptr;
    void* Memory = Bundle->ContainerPtrToValuePtr<void>(A);
    auto* Outer = CastFieldChecked<FStructProperty>(FindDasStructField(Bundle->Struct, Side));
    auto* Inner = CastFieldChecked<FStructProperty>(FindDasStructField(Outer->Struct, TEXT("Inner")));
    Type = Inner->Struct;
    return Inner->ContainerPtrToValuePtr<void>(Outer->ContainerPtrToValuePtr<void>(Memory));
}
}
bool BuildDasNestedWrapper(const FString& Base, UScriptStruct* Inner, FString& Source)
{
    auto Build = [&](const TCHAR* Name)
    {
        auto* S = FStructureEditorUtils::CreateUserDefinedStruct(CreatePackage(*(Base + TEXT("/")+ Name)), Name,
                                                                               RF_Public | RF_Standalone);
        FStructureEditorUtils::GetVarDesc(S).Empty();
        return S;
    };
    auto Add = [&](UUserDefinedStruct* S, const TCHAR* Name, const FEdGraphPinType& Pin, const TCHAR* Value)
    {
        if (!FStructureEditorUtils::AddVariable(S, Pin))
            return false;
        const auto Id = FStructureEditorUtils::GetVarDesc(S).Last().VarGuid;
        if (!FStructureEditorUtils::RenameVariable(S, Id, Name))
            return false;
        return !Value || FStructureEditorUtils::ChangeVariableDefaultValue(S, Id, Value);
    };
    auto* Envelope = Build(TEXT("Envelope"));
    FEdGraphPinType P;
    P.PinCategory = UEdGraphSchema_K2::PC_Struct;
    P.PinSubCategoryObject = Inner;
    if (!Add(Envelope, TEXT("Inner"), P, nullptr))
        return false;
    P.PinCategory = UEdGraphSchema_K2::PC_String;
    P.PinSubCategoryObject = nullptr;
    if (!Add(Envelope, TEXT("Label"), P, TEXT("Envelope")))
        return false;
    Envelope->SetMetaData(TEXT("BlueprintType"), TEXT("true"));
    auto* Bundle = Build(TEXT("Bundle"));
    P.PinCategory = UEdGraphSchema_K2::PC_Struct;
    P.PinSubCategoryObject = Envelope;
    if (!Add(Bundle, TEXT("Left"), P, nullptr) || !Add(Bundle, TEXT("Right"), P, nullptr))
        return false;
    if (FParse::Param(FCommandLine::Get(), TEXT("StructArray")))
    {
        P.ContainerType = EPinContainerType::Array;
        if (!Add(Bundle, TEXT("Items"), P, nullptr) || !Add(Bundle, TEXT("Empty"), P, nullptr))
            return false;
        P.ContainerType = EPinContainerType::None;
    }
    P.PinCategory = UEdGraphSchema_K2::PC_Real;
    P.PinSubCategory = UEdGraphSchema_K2::PC_Double;
    P.PinSubCategoryObject = nullptr;
    if (!Add(Bundle, TEXT("Ratio"), P, TEXT("1.25")))
        return false;
    Bundle->SetMetaData(TEXT("BlueprintType"), TEXT("true"));
    Source = Source.Replace(TEXT("class TypedActor {"),
        *(TEXT("class TypedActor {\n    @ue_property=(EditAnywhere, BlueprintReadWrite)\n    Bundle : UEStructValue = ue_struct_value(\"")+
            Bundle->GetPathName() + TEXT("\")")));
    return Envelope->Status == UDSS_UpToDate && Bundle->Status == UDSS_UpToDate && SaveNestedType(Envelope) &&
           SaveNestedType(Bundle);
}
void SeedDasNestedProbe(ADasActor* A)
{
    auto* B = FindFProperty<FStructProperty>(A->GetClass(), TEXT("Bundle"));
    void* Root = B->ContainerPtrToValuePtr<void>(A);
    for (const auto& Pair : TArray<TPair<FString, int32>>{{TEXT("Left"), 61}, {TEXT("Right"), 97}})
    {
        UScriptStruct* T = nullptr;
        void* Memory = NestedInner(A, *Pair.Key, T);
        CastFieldChecked<FIntProperty>(
            FindDasStructField(T, TEXT("Count")))->SetPropertyValue_InContainer(Memory, Pair.Value);
        CastFieldChecked<FStrProperty>(
            FindDasStructField(T, TEXT("Title")))->SetPropertyValue_InContainer(Memory, Pair.Key + TEXT(" 世界"));
        auto* Side = CastFieldChecked<FStructProperty>(FindDasStructField(B->Struct, Pair.Key));
        CastFieldChecked<FStrProperty>(FindDasStructField(Side->Struct, TEXT("Label")))
                                           ->SetPropertyValue_InContainer(Side->ContainerPtrToValuePtr<void>(Root),
                                                                          Pair.Key + TEXT(" label"));
    }
    CastFieldChecked<FDoubleProperty>(
        FindDasStructField(B->Struct, TEXT("Ratio")))->SetPropertyValue_InContainer(Root, 9.87654321098765);
    if (auto* Items = CastField<FArrayProperty>(FindDasStructField(B->Struct, TEXT("Items"))))
    {
        FScriptArrayHelper Array(Items, Items->ContainerPtrToValuePtr<void>(Root));
        Array.EmptyAndAddValues(3);
        auto* E = CastFieldChecked<FStructProperty>(Items->Inner);
        auto* Inner = CastFieldChecked<FStructProperty>(FindDasStructField(E->Struct, TEXT("Inner")));
        for (int32 I = 0; I < 3; ++I)
        {
            void* V = Array.GetRawPtr(I);
            void* M = Inner->ContainerPtrToValuePtr<void>(V);
            CastFieldChecked<FIntProperty>(
                FindDasStructField(Inner->Struct, TEXT("Count")))->SetPropertyValue_InContainer(M, 31 + I * 7);
            CastFieldChecked<FStrProperty>(FindDasStructField(Inner->Struct, TEXT("Title")))
                                               ->SetPropertyValue_InContainer(M,
                                                                              FString::Printf(TEXT("Item %d 世界"), I));
            CastFieldChecked<FStrProperty>(
                FindDasStructField(E->Struct, TEXT("Label")))
                    ->SetPropertyValue_InContainer(V, FString::Printf(TEXT("Array label %d"), I));
        }
    }
}
bool CheckDasNestedProbe(ADasActor* A, bool After)
{
    if (!FindFProperty<FStructProperty>(A->GetClass(), TEXT("Bundle")))
        return true;
    bool Pass = true;
    for (const auto& Pair : TArray<TPair<FString, int32>>{{TEXT("Left"), 61}, {TEXT("Right"), 97}})
    {
        UScriptStruct* Type = nullptr;
        void* Memory = NestedInner(A, *Pair.Key, Type);
        auto* P = CastField<FNumericProperty>(FindDasStructField(Type, After ? TEXT("Score"): TEXT("Count")));
        if (!P)
            return false;
        const void* V = P->ContainerPtrToValuePtr<void>(Memory);
        Pass &= (P->IsInteger() ? double(P->GetSignedIntPropertyValue(V)) : P->GetFloatingPointPropertyValue(V)) ==
                Pair.Value;
        Pass &= CastFieldChecked<FStrProperty>(
            FindDasStructField(Type, TEXT("Title")))->GetPropertyValue_InContainer(Memory) == Pair.Key + TEXT(" 世界");
        if (After)
        {
            auto* Enabled = CastField<FBoolProperty>(FindDasStructField(Type, TEXT("Enabled")));
            Pass &= !Enabled || Enabled->GetPropertyValue_InContainer(Memory);
        }
        UScriptStruct* CDOType = nullptr;
        void* CDO = NestedInner(CastChecked<ADasActor>(A->GetClass()->GetDefaultObject()), *Pair.Key, CDOType);
        const void* CV = P->ContainerPtrToValuePtr<void>(CDO);
        Pass &=
            (P->IsInteger() ? double(P->GetSignedIntPropertyValue(CV)) : P->GetFloatingPointPropertyValue(CV)) == 15;
    }
    auto* B = FindFProperty<FStructProperty>(A->GetClass(), TEXT("Bundle"));
    void* Root = B->ContainerPtrToValuePtr<void>(A);
    Pass &= CastFieldChecked<FDoubleProperty>(
        FindDasStructField(B->Struct, TEXT("Ratio")))->GetPropertyValue_InContainer(Root) == 9.87654321098765;
    for (const auto* Name : {TEXT("Left"), TEXT("Right")})
    {
        auto* Side = CastFieldChecked<FStructProperty>(FindDasStructField(B->Struct, Name));
        Pass &= CastFieldChecked<FStrProperty>(FindDasStructField(Side->Struct, TEXT("Label")))
                                                   ->GetPropertyValue_InContainer(Side->ContainerPtrToValuePtr<void>(
                                                       Root)) == FString(Name) + TEXT(" label");
    }
    if (auto* Items = CastField<FArrayProperty>(FindDasStructField(B->Struct, TEXT("Items"))))
    {
        FScriptArrayHelper Array(Items, Items->ContainerPtrToValuePtr<void>(Root));
        Pass &= Array.Num() == 3;
        auto* E = CastFieldChecked<FStructProperty>(Items->Inner);
        auto* Inner = CastFieldChecked<FStructProperty>(FindDasStructField(E->Struct, TEXT("Inner")));
        auto* Count =
            CastField<FNumericProperty>(FindDasStructField(Inner->Struct, After ? TEXT("Score"): TEXT("Count")));
        if (!Count)
            return false;
        for (int32 I = 0; I < Array.Num(); ++I)
        {
            void* V = Array.GetRawPtr(I);
            void* M = Inner->ContainerPtrToValuePtr<void>(V);
            const void* N = Count->ContainerPtrToValuePtr<void>(M);
            Pass &= (Count->IsInteger() ? double(Count->GetSignedIntPropertyValue(N))
                                        : Count->GetFloatingPointPropertyValue(N)) == 31 + I * 7;
            Pass &= CastFieldChecked<FStrProperty>(
                FindDasStructField(Inner->Struct, TEXT("Title")))->GetPropertyValue_InContainer(M) ==
                FString::Printf(TEXT("Item %d 世界"), I);
            Pass &= CastFieldChecked<FStrProperty>(
                FindDasStructField(E->Struct, TEXT("Label")))->GetPropertyValue_InContainer(V) ==
                FString::Printf(TEXT("Array label %d"), I);
            if (After)
                if (auto* Enabled = CastField<FBoolProperty>(FindDasStructField(Inner->Struct, TEXT("Enabled"))))
                    Pass &= Enabled->GetPropertyValue_InContainer(M);
        }
        auto* Empty = CastFieldChecked<FArrayProperty>(FindDasStructField(B->Struct, TEXT("Empty")));
        Pass &= FScriptArrayHelper(Empty, Empty->ContainerPtrToValuePtr<void>(Root)).Num() == 0;
    }
    auto* CDO = CastChecked<ADasActor>(A->GetClass()->GetDefaultObject());
    Pass &= B->Struct->CompareScriptStruct(B->ContainerPtrToValuePtr<void>(CDO),
                                           CastChecked<UUserDefinedStruct>(B->Struct)->GetDefaultInstance(), 0);
    UE_LOG(LogTemp, Display,
           TEXT("DAS_STRUCT_NESTED %s two_levels_repeated_inner_values_CDO_Unicode_siblings_precision"),
                Pass ? TEXT("PASS"): TEXT("FAIL"));
    return Pass;
}

bool CheckDasNestedDefaultGuard(const FString& Script, UDasTypeRegistry* Registry, UBlueprint* BP)
{
    auto* Bundle = FindFProperty<FStructProperty>(BP->GeneratedClass, TEXT("Bundle"));
    if (!Bundle)
        return true;
    void* Memory = Bundle->ContainerPtrToValuePtr<void>(BP->GeneratedClass->GetDefaultObject());
    auto* Ratio = CastFieldChecked<FDoubleProperty>(FindDasStructField(Bundle->Struct, TEXT("Ratio")));
    const double Old = Ratio->GetPropertyValue_InContainer(Memory);
    Ratio->SetPropertyValue_InContainer(Memory, 2.5);
    FString Error;
    bool Pass = !FDasTypeService::MigrateStructFields(Script, Registry->GetPathName(), Error) &&
                Error.Contains(TEXT("native defaults differ"));
    Ratio->SetPropertyValue_InContainer(Memory, Old);
    void* Native = const_cast<uint8*>(CastChecked<UUserDefinedStruct>(Bundle->Struct)->GetDefaultInstance());
    const double NativeOld = Ratio->GetPropertyValue_InContainer(Native);
    Ratio->SetPropertyValue_InContainer(Native, 1.23456789012345);
    Ratio->SetPropertyValue_InContainer(Memory, 1.23456789012345);
    Pass &= !FDasTypeService::MigrateStructFields(Script, Registry->GetPathName(), Error) &&
            Error.Contains(TEXT("round-trip"));
    Ratio->SetPropertyValue_InContainer(Native, NativeOld);
    Ratio->SetPropertyValue_InContainer(Memory, Old);
    UE_LOG(LogTemp, Display,
           TEXT("DAS_STRUCT_NESTED %s mismatched_root_CDO_default_refused_before_mutation"),
                Pass ? TEXT("PASS"): TEXT("FAIL"));
    return Pass;
}

bool CheckDasArrayAccess(const FString& Script, ADasActor* A, FDasRuntimeModule& R)
{
    auto* Bundle = FindFProperty<FStructProperty>(A->GetClass(), TEXT("Bundle"));
    if (!Bundle || !FindDasStructField(Bundle->Struct, TEXT("Items")))
        return true;
    R.ReleaseObject(A);
    auto Read = R.RunNamedFramePersistent(R.ScriptPath(Script), TEXT("ue_TypedActor_ArrayRead"), {},
                                                                     EDasMethodSignature::Int, A);
    bool Pass = Read.bSucceeded && Read.Value == 31;
    FDasScalarValue Value;
    Value.Kind = 0;
    Value.IntValue = 44;
    auto Write = R.RunNamedFramePersistent(R.ScriptPath(Script), TEXT("ue_TypedActor_ArrayWrite"), {Value},
                                                                      EDasMethodSignature::Void, A);
    Pass &= Write.bSucceeded;
    Read = R.RunNamedFramePersistent(R.ScriptPath(Script), TEXT("ue_TypedActor_ArrayRead"), {},
                                                                EDasMethodSignature::Int, A);
    Pass &= Read.bSucceeded && Read.Value == 44;
    auto Bad = R.RunNamedFramePersistent(R.ScriptPath(Script), TEXT("ue_TypedActor_ArrayBad"), {},
                                                                    EDasMethodSignature::Int, A);
    Pass &= !Bad.bSucceeded && Bad.Error.Contains(TEXT("out of range"));
    Value.IntValue = 31;
    Pass &= R.RunNamedFramePersistent(
        R.ScriptPath(Script), TEXT("ue_TypedActor_ArrayWrite"), {Value}, EDasMethodSignature::Void, A).bSucceeded;
    R.ReleaseObject(A);
    FInstancedStruct Oversized;
    Oversized.InitializeAs(Bundle->Struct);
    auto* Items = CastFieldChecked<FArrayProperty>(FindDasStructField(Bundle->Struct, TEXT("Items")));
    FScriptArrayHelper Elements(Items, Items->ContainerPtrToValuePtr<void>(Oversized.GetMutableMemory()));
    Elements.EmptyAndAddValues(4097);
    FString Error;
    Pass &= !ValidateDasStructSnapshot(Oversized, Error) && Error.Contains(TEXT("limit"));
    UE_LOG(LogTemp, Display,
           TEXT("DAS_STRUCT_ARRAY %s indexed_owned_read_write_bounds_and_snapshot_limit"),
                Pass ? TEXT("PASS"): TEXT("FAIL"));
    return Pass;
}
