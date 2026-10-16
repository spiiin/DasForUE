#pragma once
#include "DasTypeService.h"
#include "DasTypeRegistry.h"
#include "Misc/PackageName.h"
#include "UObject/SavePackage.h"
inline bool PrepareDasLibraryTestTypes(FString& Error){
 TArray<UObject*> Types;UDasTypeRegistry* Registry=nullptr;if(!FDasTypeService::Apply(TEXT("ue_library_values_types.das"),TEXT("/Game/DasLibraryValuesTypes"),Types,Error) || !FDasTypeService::BuildRegistry(Types,TEXT("/Game/DasLibraryValuesTypes/Registry"),Registry,Error))return false;Types.Add(Registry);
 for(auto* Object:Types){FSavePackageArgs Args;Args.TopLevelFlags=RF_Public|RF_Standalone;if(!UPackage::SavePackage(Object->GetOutermost(),Object,*FPackageName::LongPackageNameToFilename(Object->GetOutermost()->GetName(),FPackageName::GetAssetPackageExtension()),Args))return false;}return true;
}
