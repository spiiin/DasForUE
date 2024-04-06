#include "ReflectionHelpers.h"

#include "UObject/ObjectMacros.h"

UPackage* AnyPackage() { return ANY_PACKAGE; }

const char* FStringToCStr(const FString& str) {
	return TCHAR_TO_UTF8(*str);
}