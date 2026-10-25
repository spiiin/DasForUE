#include "Modules/ModuleManager.h"
#include "DasRuntime.h"
#if !defined(DAS_WITH_TEST_PROBES) || DAS_WITH_TEST_PROBES
#error Ordinary game targets must not include DAS test probes.
#endif
IMPLEMENT_PRIMARY_GAME_MODULE(FDefaultGameModuleImpl, DasRuntimeIsolation, "DasRuntimeIsolation");
