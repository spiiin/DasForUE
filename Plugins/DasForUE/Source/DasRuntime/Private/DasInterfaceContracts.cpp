#include "UObject/TextProperty.h"
#include "DasInterfaceContracts.h"
#include "DasSchema.h"
#include "DasContainerTypes.h"
#include "UObject/Class.h"
#include "UObject/UnrealType.h"
#include "UObject/NoExportTypes.h"
namespace {
bool InterfacePropertyMatches(FProperty* P,EDasMethodSignature K,const FString& Path){
 if(!P)return false;if(K==EDasMethodSignature::Struct){if(auto* C=DasContainerProperty(Path))return C->SameType(P);auto* S=CastField<FStructProperty>(P);return S && S->Struct->GetPathName()==Path;}
 if(K==EDasMethodSignature::Object){auto* C=DasReferencePropertyClass(P);return C && C->GetPathName()==Path;}if(K==EDasMethodSignature::Enum){auto* E=DasEnumPropertyType(P);return E && E->GetPathName()==Path;}
 if(K==EDasMethodSignature::Vector || K==EDasMethodSignature::Rotator || K==EDasMethodSignature::Transform){auto* S=CastField<FStructProperty>(P);return S && S->Struct==(K==EDasMethodSignature::Vector?TBaseStructure<FVector>::Get():K==EDasMethodSignature::Rotator?TBaseStructure<FRotator>::Get():TBaseStructure<FTransform>::Get());}
 return K==EDasMethodSignature::Byte?(P->IsA<FByteProperty>() && !DasEnumPropertyType(P)):K==EDasMethodSignature::Text?P->IsA<FTextProperty>():K==EDasMethodSignature::Int?P->IsA<FIntProperty>():K==EDasMethodSignature::Float?P->IsA<FFloatProperty>():K==EDasMethodSignature::Double?P->IsA<FDoubleProperty>():K==EDasMethodSignature::Bool?P->IsA<FBoolProperty>():K==EDasMethodSignature::String?P->IsA<FStrProperty>():K==EDasMethodSignature::Name?P->IsA<FNameProperty>():false;
}
}
bool ValidateDasInterfaceContracts(const FDasClassSchema& Schema,FString& Error){
 TSet<FString> Seen;TSet<FName> MethodNames;const auto Methods=Schema.EffectiveMethods();
 for(const auto& Path:Schema.Interfaces){auto* I=FindObject<UClass>(nullptr,*Path);if((!Path.StartsWith(TEXT("/Script/")) && !Path.StartsWith(TEXT("/Game/"))) || !I || !I->HasAnyClassFlags(CLASS_Interface) || Seen.Contains(Path)){Error=TEXT("Expected unique loaded native UInterface: ")+Path;return false;}Seen.Add(Path);
#if WITH_EDITOR
  if(I->HasMetaData(TEXT("CannotImplementInterfaceInBlueprint"))){Error=TEXT("Interface cannot be implemented in Blueprint");return false;}
#endif
  int32 Count=0;for(TFieldIterator<UFunction> It(I,EFieldIteratorFlags::IncludeSuper);It;++It){auto* F=*It;if(!F->GetOwnerClass()->HasAnyClassFlags(CLASS_Interface))continue;++Count;if(MethodNames.Contains(F->GetFName())){Error=TEXT("Overlapping interface method names require a later backend: ")+F->GetName();return false;}MethodNames.Add(F->GetFName());const auto* M=Methods.FindByPredicate([&](const auto& Candidate){return Candidate.Name==F->GetFName();});
   if(!M || M->bPure!=F->HasAnyFunctionFlags(FUNC_BlueprintPure) || M->bConst!=F->HasAnyFunctionFlags(FUNC_Const) || !F->HasAnyFunctionFlags(FUNC_BlueprintEvent) || F->HasAnyFunctionFlags(FUNC_Static|FUNC_Net|FUNC_Delegate|FUNC_MulticastDelegate)){Error=TEXT("Missing/unsupported interface method or purity/const mismatch: ")+F->GetName();return false;}
   #if WITH_EDITOR
   if(F->HasMetaData(TEXT("Latent"))){Error=TEXT("Latent interface method requires a later backend");return false;}
#endif
   const auto Params=M->EffectiveParameters();if(F->NumParms!=Params.Num()+(M->Contract().ReturnKind==EDasMethodSignature::Void?0:1)){Error=TEXT("Interface parameter count mismatch: ")+F->GetName();return false;}
   bool HasOutput=F->GetReturnProperty()!=nullptr;
   for(const auto& P:Params){auto* Actual=F->FindPropertyByName(P.Name);if(!InterfacePropertyMatches(Actual,P.Kind,P.ClassPath)){Error=TEXT("Interface parameter type/name mismatch: ")+F->GetName()+TEXT(".")+P.Name.ToString();return false;}const FString Direction=Actual->HasAnyPropertyFlags(CPF_OutParm) && !Actual->HasAnyPropertyFlags(CPF_ConstParm)?(Actual->HasAnyPropertyFlags(CPF_ReferenceParm)?TEXT("inout"):TEXT("out")):TEXT("in");if(P.Direction!=Direction){Error=TEXT("Interface parameter direction mismatch: ")+F->GetName();return false;}HasOutput |= Direction!=TEXT("in");}
   if(M->Contract().ReturnKind==EDasMethodSignature::Void?F->GetReturnProperty()!=nullptr:!InterfacePropertyMatches(F->GetReturnProperty(),M->Contract().ReturnKind,M->ReturnClass)){Error=TEXT("Interface return type mismatch: ")+F->GetName();return false;}
  }if(Count==0){Error=TEXT("Interface has no supported methods");return false;}
 }return true;
}
