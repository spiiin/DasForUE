#pragma once
#include "DasSchema.h"
struct FDasDefaultRoots;
// Blueprint member literals use pin grammar (not carrier struct ImportText).
// Retain parsed defaults until the import/reinstance transaction finishes.
bool DasBlueprintFieldDefault(const FDasFieldSchema& Field, FString& Out, FDasDefaultRoots& Roots, FString& Error);
