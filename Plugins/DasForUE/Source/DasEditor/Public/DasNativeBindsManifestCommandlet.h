#pragma once
#include "Commandlets/Commandlet.h"
#include "DasNativeBindsManifestCommandlet.generated.h"

// Writes the registered native modules (DasNativeBinds.h) as JSON: modules, declared script types and every bound
// function with its script signature. -Output=path (default logs/native-binds-manifest.json in the repository).
// Fails when a module has a registration error. Used by Scripts/NativeBindsCoverage.py.
UCLASS()
class UDasNativeBindsManifestCommandlet : public UCommandlet
{
    GENERATED_BODY()
public:
    UDasNativeBindsManifestCommandlet();
    virtual int32 Main(const FString& Params) override;
};
