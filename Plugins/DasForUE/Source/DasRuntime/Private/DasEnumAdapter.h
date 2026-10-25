#pragma once
#include "DasTypeAdapters.h"
TSharedRef<const IDasTypeAdapter> MakeDasEnumAdapter();

// Automatic storage policy shared by property and container adapters inside the runtime module.
class UEnum;
EDasEnumStorage DasAutomaticEnumStorage(const UEnum* Enum);
