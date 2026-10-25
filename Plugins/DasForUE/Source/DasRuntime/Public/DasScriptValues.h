#pragma once
#include "CoreMinimal.h"

// Script values with a C++ layout that generated AOT code and game modules share with the runtime: FDateTime and
// FTimespan as ticks (the ue_foundation types UEDateTime and UETimespan), an object reference as the token of its
// context's handle table (UObjectHandle and the class types of native binds), names and texts as tokens of the
// context's tables. Plain data, no daScript headers.
struct FDasDateTimeValue
{
    int64 Ticks = 0;
};
struct FDasTimespanValue
{
    int64 Ticks = 0;
};
struct FDasObjectHandle
{
    uint64 Token = 0;
};
// A class reference (UEClassHandle): the token of the class object in the context's handle table.
struct FDasClassHandle
{
    uint64 Token = 0;
};
// An FName (UENameValue) and an FText (UETextValue): tokens of the context's name and text tables, 0 for None and
// the empty text.
struct FDasNameValue
{
    uint64 Token = 0;
};
struct FDasTextValue
{
    uint64 Token = 0;
};
// An array as the script holds it (array<T>), in the layout of das::Array (DasRuntime checks it): native binds
// return arrays of objects in it, the AOT bridge hands generated code a das::TArray.
struct alignas(8) FDasArrayValue
{
    uint8 Bytes[40];
};
