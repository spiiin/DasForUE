#include "DasTypePins.h"
#include "DasPropertyTypes.h"
#include "DasContainerTypes.h"
#include "DasProbeInterface.h"
#include "Engine/Blueprint.h"
#include "Engine/BlueprintGeneratedClass.h"
#include "Engine/EngineTypes.h"
#include "GameFramework/Actor.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "Kismet2/KismetEditorUtilities.h"
#include "EdGraphSchema_K2.h"
#include "UObject/StrongObjectPtr.h"
#include "UObject/Package.h"
#include "Misc/AutomationTest.h"

#if WITH_DEV_AUTOMATION_TESTS
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FDasTypePinsTest,"DasForUE.Types.PinAdapter",EAutomationTestFlags::EditorContext|EAutomationTestFlags::EngineFilter)
bool FDasTypePinsTest::RunTest(const FString&){
 auto Leaf=[](const TCHAR* Kind,const FString& Path=FString()){FDasDeclaredType T;T.Kind=Kind;T.TypePath=Path;return T;};
 auto Shape=[](const TCHAR* Kind,const FDasDeclaredType& A,const FDasDeclaredType& B=FDasDeclaredType()){FDasDeclaredType T;T.Kind=Kind;if(T.GetKind()==EDasTypeKind::Map){T.Key=MakeShared<FDasDeclaredType>(A);T.Value=MakeShared<FDasDeclaredType>(B);}else T.Element=MakeShared<FDasDeclaredType>(A);return T;};
 TArray<FDasDeclaredType> Values;
 for(const TCHAR* K:{TEXT("byte"),TEXT("text"),TEXT("int"),TEXT("float"),TEXT("double"),TEXT("bool"),TEXT("string"),TEXT("name")})Values.Add(Leaf(K));
 auto Enum=Leaf(TEXT("enum"),StaticEnum<ESpawnActorCollisionHandlingMethod>()->GetPathName());Enum.EnumStorage=EDasEnumStorage::Byte;
 auto Strong=Enum;Strong.EnumStorage=EDasEnumStorage::Enum8;
 const auto Object=Leaf(TEXT("object"),UObject::StaticClass()->GetPathName()),Interface=Leaf(TEXT("object"),UDasProbeInterface::StaticClass()->GetPathName());
 Values.Append({Enum,Strong,Object,Interface,Leaf(TEXT("struct"),TBaseStructure<FVector>::Get()->GetPathName()),Leaf(TEXT("struct"),TBaseStructure<FVector2D>::Get()->GetPathName())});
 TArray<FDasDeclaredType> Types=Values;Types.Add(Leaf(TEXT("class"),AActor::StaticClass()->GetPathName()));
 for(const auto& V:Values)Types.Add(Shape(TEXT("array"),V));
 const TArray<FDasDeclaredType> Keys={Leaf(TEXT("int")),Leaf(TEXT("byte")),Leaf(TEXT("string")),Leaf(TEXT("name")),Enum,Strong,Object};
 for(const auto& K:Keys){Types.Add(Shape(TEXT("set"),K));for(const auto& V:Values)Types.Add(Shape(TEXT("map"),K,V));}
 TStrongObjectPtr<UBlueprint> BP(FKismetEditorUtilities::CreateBlueprint(UObject::StaticClass(),GetTransientPackage(),MakeUniqueObjectName(GetTransientPackage(),UBlueprint::StaticClass(),TEXT("DasPinAdapterProbe")),BPTYPE_Normal,UBlueprint::StaticClass(),UBlueprintGeneratedClass::StaticClass()));
 if(!TestNotNull(TEXT("Temporary Blueprint"),BP.Get()))return false;
 TArray<FEdGraphPinType> Pins;
 FString Error;
 for(int32 I=0;I<Types.Num();++I){
  FEdGraphPinType Pin;if(!DasTypeToPin(Types[I],Pin,Error)){AddError(Types[I].StorageIdentity()+TEXT(": ")+Error);return false;}
  FDasDeclaredType Back;if(!DescribeDasPin(Pin,Back,Error) || Back.Identity()!=Types[I].Identity()){AddError(TEXT("Pin logical roundtrip: ")+Types[I].StorageIdentity()+TEXT(" ")+Error);return false;}
  if(Types[I].GetKind()==EDasTypeKind::Array || Types[I].GetKind()==EDasTypeKind::Set || Types[I].GetKind()==EDasTypeKind::Map){
   auto* C=DasContainerCarrier(Types[I]);FDasDeclaredType Bridged;
   if(!C || !DasSchemaType(EDasMethodSignature::Struct,C->GetPathName(),Bridged,Error) || DasTypePinChecked(Bridged)!=Pin){AddError(TEXT("Carrier schema pin bridge mismatch"));return false;}
  }
  auto Directed=Pin;Directed.bIsReference=true;Directed.bIsConst=true;
  if(!DescribeDasPin(Directed,Back,Error) || Back.Identity()!=Types[I].Identity()){AddError(TEXT("Pin direction leaked into value type"));return false;}
  if(!FBlueprintEditorUtils::AddMemberVariable(BP.Get(),*FString::Printf(TEXT("Value%d"),I),Pin)){AddError(TEXT("Cannot add pin as Blueprint variable: ")+Types[I].StorageIdentity());return false;}
  Pins.Add(Pin);
 }
 FKismetEditorUtilities::CompileBlueprint(BP.Get());
 if(!TestTrue(TEXT("Blueprint compilation"),BP->Status!=BS_Error))return false;
 for(int32 I=0;I<Types.Num();++I){
  auto* P=BP->GeneratedClass->FindPropertyByName(*FString::Printf(TEXT("Value%d"),I));FEdGraphPinType Actual;FDasDeclaredType Back;
  if(!P || !GetDefault<UEdGraphSchema_K2>()->ConvertPropertyToPinType(P,Actual) || Actual!=Pins[I] || !DescribeDasProperty(P,Back,Error) || Back.Identity()!=Types[I].Identity()){AddError(TEXT("Compiled Blueprint property mismatch: ")+Types[I].StorageIdentity()+TEXT(" ")+Error);return false;}
 }
 auto Symbol=Leaf(TEXT("struct"),TEXT("/DasTypes/pin-test.Type"));const auto SymbolMap=Shape(TEXT("map"),Leaf(TEXT("name")),Symbol);
 FDasPinTypeResolver Resolve=[](EDasTypeKind Kind,const FString& Path)->UObject*{return Kind==EDasTypeKind::Struct && Path==TEXT("/DasTypes/pin-test.Type")?TBaseStructure<FVector2D>::Get():nullptr;};
 FEdGraphPinType Resolved;FDasDeclaredType Back;
 if(!DasTypeToPin(SymbolMap,Resolved,Error,Resolve) || !DescribeDasPin(Resolved,Back,Error) || Back.Value->TypePath!=TBaseStructure<FVector2D>::Get()->GetPathName() || SymbolMap.Value->TypePath!=Symbol.TypePath){AddError(TEXT("Symbolic nested type resolution mutated input or failed"));return false;}
 auto Reject=[&](const FDasDeclaredType& T){auto Out=Pins[0];const auto Before=Out;FString Why;if(DasTypeToPin(T,Out,Why) || Why.IsEmpty() || Out!=Before){AddError(TEXT("Invalid descriptor changed output pin"));return false;}return true;};
 if(!Reject(Leaf(TEXT("unknown"))) || !Reject(Leaf(TEXT("object"),TEXT("/Script/Engine.MissingPinClass"))) || !Reject(Shape(TEXT("array"),Shape(TEXT("array"),Leaf(TEXT("int"))))) || !Reject(Shape(TEXT("map"),Leaf(TEXT("text")),Leaf(TEXT("int")))))return false;
 auto Output=Pins[0];const auto Original=Output;
 if(DasTypeToPin(SymbolMap,Output,Error,[](EDasTypeKind,const FString&)->UObject*{return UObject::StaticClass();}) || Error.IsEmpty() || Output!=Original){AddError(TEXT("Wrong symbolic resolver class accepted"));return false;}
 auto InvalidPin=Pins[0];InvalidPin.PinCategory=UEdGraphSchema_K2::PC_Wildcard;Back=Leaf(TEXT("string"));
 if(DescribeDasPin(InvalidPin,Back,Error) || Error.IsEmpty() || Back.Kind!=TEXT("string")){AddError(TEXT("Invalid pin changed output descriptor"));return false;}
 AddInfo(FString::Printf(TEXT("DAS_TYPE_PINS_RESULT=PASS combinations=%d"),Types.Num()));return true;
}
#endif
