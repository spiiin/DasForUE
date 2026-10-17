#include "DasDefaultValueProbe.h"
#include "DasDefaultRoots.h"
#include "DasSchema.h"
#include "DasPropertyTypes.h"
#include "Dom/JsonValue.h"
#include "UObject/UnrealType.h"
#include "UObject/GarbageCollection.h"
bool CheckDasRecursiveDefaults(FString& Error){
 if(!CheckDasPropertyTypes(Error))return false;
 const FString Path=FDasDefaultBundleProbe::StaticStruct()->GetPathName();
 const FString Row=TEXT("{\"Positions\":[{\"X\":1.5,\"Y\":2,\"Z\":3}],\"Assets\":[{\"asset\":\"/Game/DasAssetDefaults/ArrayAsset.ArrayAsset\"}],\"Lookup\":[[\"Main\",{\"asset\":\"/Game/DasAssetDefaults/MapAsset.MapAsset\"}]],\"Tags\":[\"Tag_3\"]}");
 const FString Json=TEXT("{\"Nested\":")+Row+TEXT(",\"Rows\":[")+Row+TEXT("],\"Catalog\":[[\"one\",")+Row+TEXT("]]}");
 FDasDefaultRoots Roots;auto& Snapshot=Roots.Values.AddDefaulted_GetRef();if(!ParseDasStructDefault(Path,Json,Snapshot,Error))return false;
 CollectGarbage(RF_NoFlags,true);const auto& V=Snapshot.Get<FDasDefaultBundleProbe>();
 auto Good=[](const FDasDefaultNestedProbe& R){return R.Positions.Num()==1 && R.Positions[0]==FVector(1.5,2,3) && R.Assets.Num()==1 && IsValid(R.Assets[0]) && R.Assets[0]->GetName()==TEXT("ArrayAsset") && R.Lookup.Num()==1 && IsValid(R.Lookup.FindRef(TEXT("MAIN"))) && R.Lookup.FindRef(TEXT("MAIN"))->GetName()==TEXT("MapAsset") && R.Tags.Contains(TEXT("TAG_3"));};
 if(!Good(V.Nested) || V.Rows.Num()!=1 || !Good(V.Rows[0]) || V.Catalog.Num()!=1 || !Good(V.Catalog.FindChecked(TEXT("one")))){Error=TEXT("Recursive defaults did not survive materialization/GC");return false;}
 auto* Property=FDasDefaultBundleProbe::StaticStruct()->FindPropertyByName(TEXT("Nested"));
 auto JsonValue=ExportDasPropertyDefault(Property,&V.Nested);FDasDefaultNestedProbe Applied;
 if(!ParseDasPropertyDefault(Property,JsonValue,&Applied,Error) || !Good(Applied)){Error=TEXT("Generic property roundtrip failed: ")+Error;return false;}
 auto* Caption=FDasDefaultNestedProbe::StaticStruct()->FindPropertyByName(TEXT("Caption"));
 FText Text;if(!ParseDasTextDefault(DasTextLiteral(TEXT("defaults"),TEXT("caption"),TEXT("Ready"),false),Text,Error))return false;
 if(!ParseDasPropertyDefault(Caption,ExportDasPropertyDefault(Caption,&Text),&Applied.Caption,Error) || !DasTextDefaultsEqual(Text,Applied.Caption)){Error=TEXT("Text localization lost in generic defaults");return false;}
 auto* Priority=FDasDefaultNestedProbe::StaticStruct()->FindPropertyByName(TEXT("Priority"));
 if(!ParseDasPropertyDefault(Priority,MakeShared<FJsonValueNumber>(255),&Applied.Priority,Error) || Applied.Priority!=255)return false;
 if(ParseDasPropertyDefault(Priority,MakeShared<FJsonValueNumber>(256),&Applied.Priority,Error) || Applied.Priority!=255){Error=TEXT("Atomic byte range validation failed");return false;}Error.Reset();
 TSharedPtr<FJsonValue> InvalidJson;if(!ParseDasDefaultJson(TEXT("{\"Positions\":[{\"X\":99}],\"Assets\":[{\"asset\":\"/Game/DasAssetDefaults/Missing.Missing\"}]}"),InvalidJson,Error))return false;
 if(ParseDasPropertyDefault(Property,InvalidJson,&Applied,Error) || !Good(Applied)){Error=TEXT("Failed default modified destination");return false;}Error.Reset();
 const TArray<FString> Bad={
  TEXT("{\"Nested\":{\"Tags\":[1]}}"),
  TEXT("{\"Nested\":{\"Caption\":{\"text\":123}}}"),
  TEXT("{\"Nested\":{\"Missing\":1}}"),
  TEXT("{\"Nested\":{\"Tags\":[\"same\",\"SAME\"]}}"),
  TEXT("{\"Nested\":{\"Lookup\":[[\"a\",{\"asset\":\"\"}],[\"A\",{\"asset\":\"\"}]]}}"),
  TEXT("{\"Nested\":{\"Positions\":[{\"X\":true}]}}"),
  TEXT("{\"Nested\":{\"Positions\":[{\"X\":1,\"X\":2}]}}"),
  TEXT("{\"Nested\":{},\"nested\":{}}"),
  TEXT("{\"Nested\":\"(Tags=())\"}")
 };
 for(const auto& Input:Bad){FInstancedStruct Invalid;FString Issue;if(ParseDasStructDefault(Path,Input,Invalid,Issue) || Invalid.IsValid()){Error=TEXT("Invalid recursive default accepted: ")+Input;return false;}}
 FString Huge=TEXT("{\"Rows\":[");for(int32 I=0;I<4097;++I){if(I)Huge+=TEXT(",");Huge+=TEXT("{}");}Huge+=TEXT("]}");FInstancedStruct Invalid;FString Issue;if(ParseDasStructDefault(Path,Huge,Invalid,Issue)){Error=TEXT("Container element budget not enforced");return false;}
 return true;
}
