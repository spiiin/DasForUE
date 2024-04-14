#pragma once

#include "das_config.h"
THIRD_PARTY_INCLUDES_START
#include "daScript/daScript.h"
THIRD_PARTY_INCLUDES_END

#include "TextPrinterUE.h"

#include "using_das.inc"

struct FCompiledScript {
    das::ProgramPtr program;
    das::ContextPtr context;
    das::SimFunction* fnMain;
    das::SimFunction* fnAfterCompile;
};

FCompiledScript compile_script(const char* fname);
void run_script();
void compile_and_run_script(const char* fname);
void clean_scripts();