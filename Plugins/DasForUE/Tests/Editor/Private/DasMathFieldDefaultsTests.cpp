#include "DasTypeService.h"
#include "DasRuntime.h"
#include "StructUtils/UserDefinedStruct.h"
#include "UObject/UnrealType.h"
#include "UObject/StrongObjectPtr.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "Misc/FileHelper.h"
#include "Misc/ScopeExit.h"
#include "HAL/FileManager.h"
#include "Modules/ModuleManager.h"
#include "Misc/AutomationTest.h"

#if WITH_DEV_AUTOMATION_TESTS
// Authored struct fields of every math type keep their DSL defaults. A Vector2D default used to be
// formatted as a Transform and failed the struct type check.
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FDasMathFieldDefaultsTest, "DasForUE.Types.MathFieldDefaults",
                                 EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FDasMathFieldDefaultsTest::RunTest(const FString&)
{
    auto& Runtime = FModuleManager::LoadModuleChecked<FDasRuntimeModule>(TEXT("DasRuntime"));
    const FString Id = FGuid::NewGuid().ToString(EGuidFormats::Digits);
    const FString Script = TEXT("dsl_math_field_defaults_")+ Id + TEXT(".das"), Path = Runtime.ScriptPath(Script);
    const FString Source = TEXT("options gen2\n"
                                "require ue_dsl\n"
                                "[ue_struct(module_id=\"mathdefaults\", BlueprintType)]\n"
                                "struct MathDefaults {\n"
                                "    @ue_property=(EditAnywhere, BlueprintReadWrite)\n"
                                "    Position:UEVector=ue_vector(1.0lf,2.0lf,3.0lf)\n"
                                "    @ue_property=(EditAnywhere, BlueprintReadWrite)\n"
                                "    Offset:UEVector2D=ue_vector2d(4.0lf,5.0lf)\n"
                                "    @ue_property=(EditAnywhere, BlueprintReadWrite)\n"
                                "    Facing:UERotator=ue_rotator(10.0lf,20.0lf,30.0lf)\n"
                                "    @ue_property=(EditAnywhere, BlueprintReadWrite)\n"
                                "    Pose:UETransform\n"
                                "}\n");
    if (!TestTrue(TEXT("Write fixture"), FFileHelper::SaveStringToFile(
                                             Source, *Path, FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM)))
        return false;
    ON_SCOPE_EXIT
    {
        IFileManager::Get().Delete(*Path);
    };
    TArray<UObject*> Types;
    FString Error;
    const bool Applied =
        FDasTypeService::Apply(Script, TEXT("/Game/Tests/DasMathFieldDefaults/")+ Id, Types, Error) && Types.Num() == 1;
    ON_SCOPE_EXIT
    {
        for (auto* Type : Types)
        {
            FAssetRegistryModule::AssetDeleted(Type);
            Type->GetOutermost()->SetDirtyFlag(false);
            Type->ClearFlags(RF_Public | RF_Standalone);
        }
    };
    if (!TestTrue(TEXT("Apply math default struct"), Applied))
    {
        AddInfo(Error);
        return false;
    }
    auto* Struct = Cast<UUserDefinedStruct>(Types[0]);
    if (!TestNotNull(TEXT("User defined struct"), Struct))
        return false;
    const uint8* Defaults = Struct->GetDefaultInstance();
    auto Field = [&](const UScriptStruct* Type) -> const void*
    {
        for (TFieldIterator<FStructProperty> It(Struct); It; ++It)
            if (It->Struct == Type)
                return It->ContainerPtrToValuePtr<void>(Defaults);
        return nullptr;
    };
    const auto* Position = static_cast<const FVector*>(Field(TBaseStructure<FVector>::Get()));
    const auto* Offset = static_cast<const FVector2D*>(Field(TBaseStructure<FVector2D>::Get()));
    const auto* Facing = static_cast<const FRotator*>(Field(TBaseStructure<FRotator>::Get()));
    const auto* Pose = static_cast<const FTransform*>(Field(TBaseStructure<FTransform>::Get()));
    TestTrue(TEXT("Vector default"), Position && Position->Equals(FVector(1, 2, 3)));
    TestTrue(TEXT("Vector2D default"), Offset && Offset->Equals(FVector2D(4, 5)));
    TestTrue(TEXT("Rotator default"), Facing && Facing->Equals(FRotator(10, 20, 30)));
    TestTrue(TEXT("Transform default"), Pose && Pose->Equals(FTransform::Identity));
    return true;
}
#endif
