#include "DasPropertyValues.h"
#include "Internationalization/Text.h"
#include "UObject/TextProperty.h"
#include "UObject/UnrealType.h"

bool IdenticalDasStructSnapshot(const FInstancedStruct& A,const FInstancedStruct& B){
 const auto* Type=A.GetScriptStruct();
 if(!Type || Type!=B.GetScriptStruct() || !Type->CompareScriptStruct(A.GetMemory(),B.GetMemory(),0))return false;
 // Editor FTextProperty equality compares display strings. Snapshot reuse must
 // also retain localization history. Walk all leaves (including map keys) in
 // native storage order: different container order conservatively skips reuse.
 FPropertyValueIterator Left(FProperty::StaticClass(),Type,A.GetMemory()),Right(FProperty::StaticClass(),Type,B.GetMemory());
 for(;Left && Right;++Left,++Right){
  if(Left->Key!=Right->Key)return false;
  if(auto* Text=CastField<FTextProperty>(Left->Key)){
   if(!FTextSnapshot(Text->GetPropertyValue(Left->Value)).IdenticalTo(Text->GetPropertyValue(Right->Value)))return false;
  }else if(!Left->Key->Identical(Left->Value,Right->Value,0))return false;
 }
 return !Left && !Right;
}
