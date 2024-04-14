#pragma once

#include "Logging/LogMacros.h"


void das_to_stdout(const char* format, ...);
void das_to_stderr(const char* format, ...);
void DAS_FATAL_LOG(const char* format, ...);
