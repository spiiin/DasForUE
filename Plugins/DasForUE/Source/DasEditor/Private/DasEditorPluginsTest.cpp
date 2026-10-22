// The test project disables engine plugins by default. Opening any Blueprint editor primes the action menu, which
// compiles a template AnimBlueprint; without the PropertyAccessEditor plugin that compile dereferences a missing
// modular feature and crashes the editor.
#include "Animation/AnimBlueprint.h"
#include "Animation/AnimBlueprintGeneratedClass.h"
#include "Animation/AnimInstance.h"
#include "Features/IModularFeatures.h"
#include "Kismet2/KismetEditorUtilities.h"
#if WITH_DEV_AUTOMATION_TESTS
#include "Misc/AutomationTest.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FDasEditorPluginsTest, "DasForUE.Types.EditorPlugins",
                                 EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FDasEditorPluginsTest::RunTest(const FString&)
{
    if (!TestTrue(TEXT("PropertyAccessEditor feature is registered"),
                       IModularFeatures::Get().IsModularFeatureAvailable(TEXT("PropertyAccessEditor"))))
        return false;
    auto* BP = FKismetEditorUtilities::CreateBlueprint(
        UAnimInstance::StaticClass(), GetTransientPackage(),
        MakeUniqueObjectName(GetTransientPackage(), UAnimBlueprint::StaticClass()), BPTYPE_Normal,
        UAnimBlueprint::StaticClass(), UAnimBlueprintGeneratedClass::StaticClass());
    TestTrue(TEXT("template AnimBlueprint compiles"), BP && BP->GeneratedClass);
    return true;
}
#endif
