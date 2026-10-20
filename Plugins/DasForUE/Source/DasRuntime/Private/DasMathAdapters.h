#pragma once
#include "DasTypeAdapters.h"

// Math value structs (Vector, Vector2D, Rotator, Transform) and InputActionValue, registered with the
// built-in type adapters (DasTypeAdaptersBuiltin.inl).
TSharedRef<const IDasTypeAdapter> MakeDasVectorAdapter();
TSharedRef<const IDasTypeAdapter> MakeDasVector2DAdapter();
TSharedRef<const IDasTypeAdapter> MakeDasRotatorAdapter();
TSharedRef<const IDasTypeAdapter> MakeDasTransformAdapter();
TSharedRef<const IDasTypeAdapter> MakeDasInputActionValueAdapter();
// Finite math values; a Transform also needs a normalized rotation.
bool DasMathValueValid(const UScriptStruct* Type, const void* Data);
