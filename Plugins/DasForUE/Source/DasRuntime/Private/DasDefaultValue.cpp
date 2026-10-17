#include "DasNativeDefaults.h"
#include "DasSchema.h"
#include "DasAuthoring.h"
#include "DasDefaultRoots.h"
#include "DasSoftReferences.h"
#include "Dom/JsonValue.h"
#include "Dom/JsonObject.h"
#include "UObject/UnrealType.h"
bool DasParseEditableDefault(FProperty* P,const FDasDeclaredLiteral& V,void* Data,FString& Error){
 auto Fail=[&](){Error=TEXT("Unsupported or non-editable default property: ")+(P?P->GetName():TEXT("<missing>"));return false;};
 if(!P || P->ArrayDim!=1 || !P->HasAnyPropertyFlags(CPF_Edit) || P->HasAnyPropertyFlags(CPF_EditConst|CPF_DisableEditOnTemplate))return Fail();
 TSharedPtr<FJsonValue> Json;
 if(V.Kind==TEXT("bool"))Json=MakeShared<FJsonValueBoolean>(V.Boolean);
 else if(V.Kind==TEXT("number"))Json=MakeShared<FJsonValueNumber>(V.Number);
 else if(V.Kind==TEXT("string"))Json=MakeShared<FJsonValueString>(V.String);
 else if(V.Kind==TEXT("asset") || V.Kind==TEXT("class")){auto Object=MakeShared<FJsonObject>();Object->SetStringField(V.Kind,V.String);Json=MakeShared<FJsonValueObject>(Object);}
 else if(V.Kind==TEXT("struct") && (CastField<FArrayProperty>(P) || CastField<FSetProperty>(P) || CastField<FMapProperty>(P))){if(!ParseDasDefaultJson(V.String,Json,Error))return false;}
 else if(auto* Struct=CastField<FStructProperty>(P)){
  const FString Path=Struct->Struct->GetPathName();const FString Expected=Path==TEXT("/Script/CoreUObject.Vector")?TEXT("vector"):Path==TEXT("/Script/CoreUObject.Rotator")?TEXT("rotator"):Path==TEXT("/Script/CoreUObject.Vector2D")?TEXT("vector2d"):Path==TEXT("/Script/CoreUObject.Transform")?TEXT("transform"):FString();
  if(V.Kind!=TEXT("struct") && (Expected.IsEmpty() || V.Kind!=Expected))return Fail();
  // Adapt existing source constructors at the boundary. The materializer only
  // consumes structured values; no legacy representation enters its tree.
  const FString& Text=V.String;
  FDasDefaultRoots Roots;auto& Value=Roots.Values.AddDefaulted_GetRef();if(!ParseDasStructDefault(Path,Text,Value,Error))return false;Json=ExportDasPropertyDefault(P,Value.GetMemory());
 }else return Fail();
 return ParseDasPropertyDefault(P,Json,Data,Error);
}
