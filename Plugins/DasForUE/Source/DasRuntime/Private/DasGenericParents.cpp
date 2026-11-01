#include "DasGenericParents.h"
#include "DasScriptHosts.h"
#include "UObject/Class.h"
bool DasGenericScriptParent(const FString& Parent)
{
    if (!Parent.StartsWith(TEXT("/")) || Parent == TEXT("/Script/Engine.BlueprintFunctionLibrary"))
        return false;
#if DAS_WITH_TEST_PROBES
    if (Parent == TEXT("/Script/DasRuntime.DasProbeActor"))
        return false; // Keep the existing probe's native lifecycle and migration adapter.
#endif
    auto* Host = DasNativeScriptHost(Parent);
    // Existing native roots intentionally map to a different bridge class.
    return Host && Host->GetPathName() == Parent;
}
