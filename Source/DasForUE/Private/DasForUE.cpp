// Copyright Epic Games, Inc. All Rights Reserved.

#include "DasForUE.h"

#include "Interfaces/IPluginManager.h"

#define LOCTEXT_NAMESPACE "FDasForUEModule"
#define PLUGIN_NAME "DasForUE"

#include "das_config.h"
THIRD_PARTY_INCLUDES_START
#include "daScript/daScript.h"
THIRD_PARTY_INCLUDES_END
#include "das_compile.h"

#include "codegen/dasUnreal.h"
#include "using_das.inc"

void FDasForUEModule::StartupModule()
{
    //TODO: setup for non-editor also
    FString ContentDir = IPluginManager::Get().FindPlugin(TEXT("DasForUE"))->GetContentDir();
    FTCHARToUTF8 ConvertToUtf8(*ContentDir);
    das::setDasRoot(ConvertToUtf8.Get());
    //
    NEED_ALL_DEFAULT_MODULES;
    NEED_MODULE(Module_dasUnreal);
    Module::Initialize();
}

void FDasForUEModule::ShutdownModule()
{
    clean_scripts();
    Module::Shutdown();
}

#undef LOCTEXT_NAMESPACE
    
IMPLEMENT_MODULE(FDasForUEModule, DasForUE)
