#pragma once

#include "das_config.h"

#include <stdarg.h>

#define BUF_SIZE 4096

void das_to_stdout(const char* format, ...) {
    va_list args;
    va_start(args, format);
    char buffer[BUF_SIZE];
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);
    UE_LOG(LogTemp, Log, TEXT("%s"), ANSI_TO_TCHAR(buffer));
}

void das_to_stderr(const char* format, ...)
{
    va_list args;
    va_start(args, format);
    char buffer[BUF_SIZE];
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);
    UE_LOG(LogTemp, Warning, TEXT("%s"), ANSI_TO_TCHAR(buffer));
}

void DAS_FATAL_LOG(const char* format, ...)
{
    va_list args;
    va_start(args, format);
    char buffer[BUF_SIZE];
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);
    UE_LOG(LogTemp, Warning, TEXT("%s"), ANSI_TO_TCHAR(buffer));
}

#undef BUF_SIZE
