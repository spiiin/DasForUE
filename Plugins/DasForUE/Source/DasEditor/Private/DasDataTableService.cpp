#include "DasDataTableService.h"
#include "DasReflectedTypes.h"
#include "DasSchema.h"
#include "DasTypePins.h"
#include "Engine/DataTable.h"
#include "DataTableUtils.h"
#include "StructUtils/UserDefinedStruct.h"
#include "StructUtils/InstancedStruct.h"
#include "Kismet2/StructureEditorUtils.h"
#include "UserDefinedStructure/UserDefinedStructEditorData.h"
#include "UObject/Package.h"
#include "UObject/MetaData.h"
#include "UObject/StrongObjectPtr.h"
#include "UObject/UnrealType.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "Misc/PackageName.h"
#include "Misc/SecureHash.h"
#include "Dom/JsonObject.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"
#include "Editor.h"

namespace {
bool AuthoredRow(UScriptStruct* Type,FString& Error){
 auto* Row=Cast<UUserDefinedStruct>(Type);if(!IsValid(Row) || Row->Status!=UDSS_UpToDate){Error=TEXT("Expected a compiled authored DSL struct");return false;}
 auto& Meta=Row->GetOutermost()->GetMetaData();const FString Id=Meta.GetValue(Row,TEXT("DasTypeId"));FDasReflectedTypes Model;
 if(Id.IsEmpty() || !FDasReflectedTypes::FromJson(Meta.GetValue(Row,TEXT("DasTypesModel")),Model,Error)){Error=TEXT("Row struct has no valid authored DSL identity/model");return false;}
 const auto* Decl=Model.Structs.FindByPredicate([&](const auto& D){return D.Id==Id;});
 const auto& Vars=FStructureEditorUtils::GetVarDesc(Row);
 if(!Decl || Decl->Name!=Row->GetName() || Vars.Num()!=Decl->Properties.Num()){Error=TEXT("Row struct differs from its saved authored declaration");return false;}
 const FString Folder=FPackageName::GetLongPackagePath(Row->GetOutermost()->GetName());
 FDasPinTypeResolver Resolve=[&](EDasTypeKind Kind,const FString& Path)->UObject*{
  if(!Path.StartsWith(TEXT("/DasTypes/")))return FindObject<UObject>(nullptr,*Path);
  FString Name;if(Kind==EDasTypeKind::Enum){for(const auto& D:Model.Enums)if(Path==TEXT("/DasTypes/")+FMD5::HashAnsiString(*D.Id)+TEXT(".Type"))Name=D.Name;}
  if(Kind==EDasTypeKind::Struct){for(const auto& D:Model.Structs)if(Path==TEXT("/DasTypes/")+FMD5::HashAnsiString(*D.Id)+TEXT(".Type"))Name=D.Name;}
  return Name.IsEmpty()?nullptr:FindObject<UObject>(nullptr,*(Folder+TEXT("/")+Name+TEXT(".")+Name));
 };
 for(const auto& D:Decl->Properties){
  const auto* V=Vars.FindByPredicate([&](const auto& X){return X.FriendlyName==D.Name;});FEdGraphPinType Expected;
  if(!V || !FindDasStructField(Row,D.Name) || V->MetaData.FindRef(TEXT("DasMemberId"))!=D.Id || !DasTypeToPin(D.Type,Expected,Error,Resolve) || V->ToPinType()!=Expected){Error=TEXT("Authored row field drift requires explicit type migration: ")+D.Name;return false;}
 }
 return ValidateDasValueStruct(Row->GetPathName(),Error);
}
bool JsonRows(UScriptStruct* Type,const FString& Json,int32& Count,FString& Error){
 if(Json.Len()>16*1024*1024){Error=TEXT("Table JSON exceeds 16M characters");return false;}
 TArray<TSharedPtr<FJsonValue>> Rows;auto Reader=TJsonReaderFactory<>::Create(Json);
 if(!FJsonSerializer::Deserialize(Reader,Rows)){Error=TEXT("Expected a JSON array of row objects: ")+Reader->GetErrorMessage();return false;}
 if(Rows.Num()>4096){Error=TEXT("Table JSON exceeds 4096 rows");return false;}
 TSet<FName> Names;
 for(int32 Index=0;Index<Rows.Num();++Index){
  if(!Rows[Index] || Rows[Index]->Type!=EJson::Object){Error=FString::Printf(TEXT("Row %d must be an object"),Index);return false;}
  auto Obj=Rows[Index]->AsObject();FString Name;const auto NameValue=Obj->TryGetField(TEXT("Name"));
  if(!NameValue || NameValue->Type!=EJson::String || !Obj->TryGetStringField(TEXT("Name"),Name) || Name.IsEmpty() || Name.Len()>1024 || DataTableUtils::MakeValidName(Name).ToString()!=Name || FName(*Name).IsNone()){Error=FString::Printf(TEXT("Row %d requires a valid non-None string Name without UE name sanitization"),Index);return false;}
  if(Names.Contains(FName(*Name))){Error=TEXT("Duplicate row name: ")+Name;return false;}Names.Add(FName(*Name));
  for(TFieldIterator<FProperty> It(Type);It;++It){
   auto* P=*It;const auto Aliases=DataTableUtils::GetPropertyImportNames(P);TSharedPtr<FJsonValue> Value;int32 Matches=0;
   for(const auto& Alias:Aliases){if(Alias==TEXT("Name")){Error=TEXT("Row struct column conflicts with reserved JSON key Name");return false;}if(auto Found=Obj->TryGetField(Alias)){Value=Found;++Matches;}}
   if(Matches!=1){Error=TEXT("Row ")+Name+TEXT(" requires exactly one value for column ")+DataTableUtils::GetPropertyExportName(P);return false;}
   // UE's numeric importer can narrow storage; reject loss before that conversion.
   if(auto* Number=CastField<FNumericProperty>(P);Number && !Number->IsEnum()){
    double V=0;if(Value->Type!=EJson::Number || !Value->TryGetNumber(V) || !FMath::IsFinite(V)){Error=TEXT("Expected finite JSON number for ")+Name+TEXT(".")+P->GetName();return false;}
    if((CastField<FIntProperty>(P) && (V<double(MIN_int32) || V>double(MAX_int32) || V!=FMath::TruncToDouble(V))) || (CastField<FByteProperty>(P) && (V<0 || V>255 || V!=FMath::TruncToDouble(V))) || (CastField<FFloatProperty>(P) && FMath::Abs(V)>double(MAX_flt))){Error=TEXT("JSON number out of range or fractional integer at ")+Name+TEXT(".")+P->GetName();return false;}
   }
  }
 }
 Count=Rows.Num();return true;
}
}

bool FDasDataTableService::CreateFromJson(UScriptStruct* RowType,const FString& Json,const FString& Package,UDataTable*& Out,FString& Error){
 Out=nullptr;Error.Reset();if(!IsInGameThread() || (GEditor && GEditor->PlayWorld)){Error=TEXT("Create/import requires the editor game thread outside PIE");return false;}
 if(!Package.StartsWith(TEXT("/Game/")) || !FPackageName::IsValidLongPackageName(Package)){Error=TEXT("Expected a new /Game/Folder/TableName package");return false;}
 if(FPackageName::DoesPackageExist(Package) || FindPackage(nullptr,*Package)){Error=TEXT("Table package already exists; refusing to overwrite");return false;}
 if(!AuthoredRow(RowType,Error))return false;
 int32 ExpectedRows=0;if(!JsonRows(RowType,Json,ExpectedRows,Error))return false;
 TStrongObjectPtr<UDataTable> Draft(NewObject<UDataTable>(GetTransientPackage(),NAME_None,RF_Transient));Draft->RowStruct=RowType;Draft->ImportKeyField=TEXT("Name");Draft->bIgnoreExtraFields=false;Draft->bIgnoreMissingFields=false;Draft->bPreserveExistingValues=false;
 if(ExpectedRows){auto Problems=Draft->CreateTableFromJSONString(Json);if(!Problems.IsEmpty()){Error=FString::Join(Problems,TEXT("\n"));return false;}}
 if(Draft->GetRowMap().Num()!=ExpectedRows){Error=TEXT("Imported row count differs from validated JSON");return false;}
 for(const auto& Pair:Draft->GetRowMap()){FInstancedStruct Snapshot;Snapshot.InitializeAs(RowType,Pair.Value);if(!ValidateDasStructSnapshot(Snapshot,Error)){Error=Pair.Key.ToString()+TEXT(": ")+Error;return false;}}
 // A valid draft has the right row struct. UE copies rows using UScriptStruct lifecycle APIs.
 auto* P=CreatePackage(*Package);auto* Table=NewObject<UDataTable>(P,*FPackageName::GetLongPackageAssetName(Package),RF_Public|RF_Standalone|RF_Transactional);
 Table->ImportKeyField=TEXT("Name");Table->CreateTableFromOtherTable(Draft.Get());FAssetRegistryModule::AssetCreated(Table);Table->MarkPackageDirty();Out=Table;return true;
}
