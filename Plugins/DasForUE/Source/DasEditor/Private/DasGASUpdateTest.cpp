// GAS assets updated in place: a changed effect magnitude and a new attribute keep the same Blueprints, a removed
// attribute is refused, a failure restores every updated asset, and saving the script updates the folder through
// auto-reimport.
#include "DasAutoReimport.h"
#include "DasGASAbility.h"
#include "DasGASAssetService.h"
#include "DasGASAuthoring.h"
#include "DasRuntime.h"
#include "Engine/Blueprint.h"
#include "HAL/FileManager.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "Misc/ScopeExit.h"
#if WITH_DEV_AUTOMATION_TESTS
#include "Misc/AutomationTest.h"
void DasFailGASUpdateForTesting(bool);
namespace DasGASUpdate
{
FString GASScript(const FString& Module, const TCHAR* HealHealth, bool Stamina)
{
    return FString::Printf(TEXT("options gen2\nmodule %s public\nrequire ue_dsl\n\n"
                                "[ue_attribute_set]\nclass Stats {\n    @min=0\n    Health:float=100.0f\n%s}\n"
                                "[ue_gameplay_effect(attributes=\"Stats\")]\nclass Heal {\n    Health:float=%s\n%s}\n"
                                "[ue_gameplay_ability(effect=\"Heal\")]\nclass UseHeal {}\n"),
                                *Module,
                                Stamina ? TEXT("    Stamina:float=10.0f\n")
                                        : TEXT(""), HealHealth, Stamina ? TEXT("    Stamina:float=1.0f\n") : TEXT(""));
}
float Magnitude(UBlueprint* Effect, int32 Index)
{
    const auto* CDO = Effect ? Cast<UDasAuthoredGameplayEffect>(Effect->GeneratedClass->GetDefaultObject()) : nullptr;
    float Value = -1;
    if (CDO && CDO->Modifiers.IsValidIndex(Index))
        CDO->Modifiers[Index].ModifierMagnitude.GetStaticMagnitudeIfPossible(1.f, Value);
    return Value;
}
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FDasGASUpdateTest, "DasForUE.Reload.GASInPlace",
                                 EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FDasGASUpdateTest::RunTest(const FString&)
{
    using namespace DasGASUpdate;
    auto& R = FModuleManager::LoadModuleChecked<FDasRuntimeModule>(TEXT("DasRuntime"));
    const FString Tag = FGuid::NewGuid().ToString(EGuidFormats::Digits).Left(12).ToLower();
    const FString Module = TEXT("gas_update_") + Tag, Script = Module + TEXT(".das"),
                                                                             Folder = TEXT("/Game/DasGASUpdate/") + Tag,
                                                                                           Path = R.ScriptPath(Script);
    ON_SCOPE_EXIT
    {
        IFileManager::Get().Delete(*Path);
        DasFailGASUpdateForTesting(false);
    };
    FString Error;
    TMap<FString, UBlueprint*> V1, Out;
    auto Import = [&](const FString& Text, int32* Updated = nullptr)
    {
        Out.Reset();
        return FFileHelper::SaveStringToFile(Text, *Path) &&
               FDasGASAssetService::Apply(Script, Folder, Out, Error, false, Updated);
    };
    if (!TestTrue(TEXT("import v1"), Import(GASScript(Module, TEXT("5.0f"), false))))
    {
        AddInfo(Error);
        return false;
    }
    V1 = Out;
    UBlueprint *Stats = V1.FindRef(TEXT("Stats")), *Heal = V1.FindRef(TEXT("Heal")),
                                                                      *UseHeal = V1.FindRef(TEXT("UseHeal"));
    if (!TestTrue(TEXT("v1 assets"), Stats && Heal && UseHeal))
        return false;
    TArray<TStrongObjectPtr<UBlueprint>> Keep{TStrongObjectPtr<UBlueprint>(Stats), TStrongObjectPtr<UBlueprint>(Heal),
                                              TStrongObjectPtr<UBlueprint>(UseHeal)};
    TestEqual(TEXT("v1 magnitude"), Magnitude(Heal, 0), 5.f);

    // A changed magnitude rewrites the effect (and the ability that refers to it) in place.
    int32 Updated = 0;
    TestTrue(TEXT("magnitude change applies"), Import(GASScript(Module, TEXT("7.0f"), false), &Updated));
    AddInfo(Error);
    TestTrue(TEXT("same Blueprints"),
                  Out.FindRef(TEXT("Heal")) == Heal && Out.FindRef(TEXT("Stats")) == Stats &&
                                                                   Out.FindRef(TEXT("UseHeal")) == UseHeal);
    TestEqual(TEXT("updated magnitude"), Magnitude(Heal, 0), 7.f);
    TestEqual(TEXT("effect and ability updated"), Updated, 2);
    const auto* Ability = Cast<UDasAuthoredGameplayAbility>(UseHeal->GeneratedClass->GetDefaultObject());
    TestTrue(TEXT("ability keeps its effect"), Ability && Ability->Effect.Get() == Heal->GeneratedClass);

    // A new attribute is added to the same AttributeSet; effects bind to it.
    TestTrue(TEXT("new attribute applies"), Import(GASScript(Module, TEXT("7.0f"), true)));
    AddInfo(Error);
    FProperty* Stamina = Stats->GeneratedClass->FindPropertyByName(TEXT("Stamina"));
    const auto* HealCDO = Cast<UDasAuthoredGameplayEffect>(Heal->GeneratedClass->GetDefaultObject());
    TestTrue(TEXT("attribute added in place"), Stamina != nullptr && Out.FindRef(TEXT("Stats")) == Stats);
    TestTrue(TEXT("effect modifies the new attribute"), HealCDO && HealCDO->Modifiers.Num() == 2 &&
                                                            HealCDO->Modifiers[1].Attribute.GetUProperty() == Stamina);

    // Removing an attribute is a contract change: refused, nothing changes.
    TestFalse(TEXT("removed attribute refused"), Import(GASScript(Module, TEXT("7.0f"), false)));
    TestTrue(TEXT("refusal names the contract"), Error.Contains(TEXT("contract change")));
    TestTrue(TEXT("attribute kept"), Stats->GeneratedClass->FindPropertyByName(TEXT("Stamina")) != nullptr);
    TestEqual(TEXT("modifiers kept"),
                   Cast<UDasAuthoredGameplayEffect>(Heal->GeneratedClass->GetDefaultObject())->Modifiers.Num(), 2);

    // A failure after the update restores the updated assets.
    DasFailGASUpdateForTesting(true);
    TestFalse(TEXT("injected failure refused"), Import(GASScript(Module, TEXT("9.0f"), true)));
    DasFailGASUpdateForTesting(false);
    TestTrue(TEXT("rollback reported"), Error.Contains(TEXT("rolled back")));
    TestEqual(TEXT("magnitude restored"), Magnitude(Heal, 0), 7.f);

    // Saving the script updates the folder it was imported into.
    TestTrue(TEXT("write v5"), FFileHelper::SaveStringToFile(GASScript(Module, TEXT("11.0f"), true), *Path));
    FString Full = FPaths::ConvertRelativePathToFull(Path);
    FPaths::NormalizeFilename(Full);
    FDasAutoReimport Auto;
    Auto.ReimportFiles({Full});
    TestEqual(TEXT("auto-reimport updates the effect"), Magnitude(Heal, 0), 11.f);
    return true;
}
#endif
