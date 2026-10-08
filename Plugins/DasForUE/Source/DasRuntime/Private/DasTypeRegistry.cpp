#include "DasTypeRegistry.h"
#include "DasDefinedEnum.h"
#include "StructUtils/UserDefinedStruct.h"
#include "Misc/PackageName.h"
bool UDasTypeRegistry::Validate(FString& Error)const {
 if(Bindings.IsEmpty() || Bindings.Num()>1024){Error=TEXT("Type registry requires 1..1024 bindings");return false;}
 TSet<FString> Ids;TSet<UObject*> Types;
 for(const auto& B:Bindings){if(B.Id.IsEmpty() || Ids.Contains(B.Id) || !IsValid(B.Type) || Types.Contains(B.Type.Get()) || !B.Type->GetPathName().StartsWith(TEXT("/Game/"))){Error=TEXT("Missing or duplicate type registry identity");return false;}Ids.Add(B.Id);Types.Add(B.Type.Get());
  if(B.bEnum){const auto* E=Cast<UDasDefinedEnum>(B.Type);if(!E || E->StableId!=B.Id){Error=TEXT("Registered enum stable ID mismatch");return false;}}
  else{const auto* S=Cast<UUserDefinedStruct>(B.Type);if(!S || !B.StructGuid.IsValid() || S->GetCustomGuid()!=B.StructGuid){Error=TEXT("Registered struct GUID mismatch");return false;}}
 }return true;
}
UObject* UDasTypeRegistry::Resolve(const FString& Id,bool bEnum,FString& Error)const {
 Error.Reset();if(!Validate(Error))return nullptr;for(const auto& B:Bindings)if(B.Id==Id){if(B.bEnum!=bEnum){Error=TEXT("Registered type kind mismatch: ")+Id;return nullptr;}return B.Type.Get();}Error=TEXT("Stable type ID is not registered: ")+Id;return nullptr;
}
UObject* ResolveDasRegisteredType(const FString& Path,const FString& Id,bool bEnum,FString& Error){
 if(!IsInGameThread() || !Path.StartsWith(TEXT("/Game/")) || !FPackageName::IsValidObjectPath(Path)){Error=TEXT("Expected game-thread /Game registry object path");return nullptr;}
 auto* R=LoadObject<UDasTypeRegistry>(nullptr,*Path,nullptr,LOAD_NoWarn);if(!R){Error=TEXT("Type registry could not load: ")+Path;return nullptr;}return R->Resolve(Id,bEnum,Error);
}
