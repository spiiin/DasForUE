#include "DasDelegateSignatures.h"
#include "DasAuthoring.h"
#include "DasSchema.h"
#include "UObject/UnrealType.h"
#include "DasContainerTypes.h"

namespace {
FProperty* DelegateProperty(FFieldVariant Owner,FName Name,const FDasDeclaredType& T,FString& Error) {
 FProperty* P=nullptr;
 if(T.Kind==TEXT("int"))P=new FIntProperty(Owner,Name,RF_Public);
 else if(T.Kind==TEXT("float"))P=new FFloatProperty(Owner,Name,RF_Public);
 else if(T.Kind==TEXT("double"))P=new FDoubleProperty(Owner,Name,RF_Public);
 else if(T.Kind==TEXT("bool")){auto* B=new FBoolProperty(Owner,Name,RF_Public);B->SetBoolSize(sizeof(bool),true);P=B;}
 else if(T.Kind==TEXT("string"))P=new FStrProperty(Owner,Name,RF_Public);
 else if(T.Kind==TEXT("name"))P=new FNameProperty(Owner,Name,RF_Public);
 else if(T.Kind==TEXT("object")){auto* C=FindObject<UClass>(nullptr,*T.TypePath);if(C && !C->HasAnyClassFlags(CLASS_Interface)){auto* O=new FObjectProperty(Owner,Name,RF_Public);O->PropertyClass=C;P=O;}else if(C){auto* I=new FInterfaceProperty(Owner,Name,RF_Public);I->InterfaceClass=C;P=I;}}
 else if(T.Kind==TEXT("struct")){auto* S=FindObject<UScriptStruct>(nullptr,*T.TypePath);if(S){auto* V=new FStructProperty(Owner,Name,RF_Public);V->Struct=S;P=V;}}
 else if(T.Kind==TEXT("enum")){auto* E=ResolveDasEnum(T.TypePath,Error);if(E){auto* B=new FByteProperty(Owner,Name,RF_Public);B->Enum=E;P=B;}}
 else if(T.Kind==TEXT("array") && T.Element){auto* A=new FArrayProperty(Owner,Name,RF_Public);A->Inner=DelegateProperty(A,TEXT("Inner"),*T.Element,Error);if(A->Inner)P=A;else delete A;}
 if(!P && Error.IsEmpty())Error=TEXT("Unsupported/unloaded delegate signature type: ")+T.Kind+TEXT(" ")+T.TypePath;
 return P;
}
}
UFunction* MakeDasDelegateSignature(const FDasDeclaredMethod& M,bool Multicast,UObject* Outer,FName Name,FString& Error) {
 Error.Reset();if(M.Parameters.Num()>16 || (Multicast && M.ReturnType.Kind!=TEXT("void"))){Error=TEXT("Delegates support up to 16 inputs; multicast events cannot return a value");return nullptr;}
 for(const auto& P:M.Parameters)if(P.Direction!=TEXT("in")){Error=TEXT("Delegate ref/out signatures are not supported yet");return nullptr;}
 auto* F=NewObject<UFunction>(Outer,Name,RF_Transient);
 F->FunctionFlags=FUNC_Public|FUNC_Delegate|(Multicast?FUNC_MulticastDelegate:FUNC_None);
 TArray<TPair<FName,FDasDeclaredType>> Params;for(const auto& P:M.Parameters)Params.Emplace(*P.Name,P.Type);
 if(M.ReturnType.Kind!=TEXT("void"))Params.Emplace(TEXT("ReturnValue"),M.ReturnType);
 for(int32 I=Params.Num()-1;I>=0;--I){auto* P=DelegateProperty(F,Params[I].Key,Params[I].Value,Error);if(!P)return nullptr;P->SetPropertyFlags(CPF_Parm);if(Params[I].Key==TEXT("ReturnValue"))P->SetPropertyFlags(CPF_ReturnParm|CPF_OutParm);F->AddCppProperty(P);}
 F->Bind();F->StaticLink(true);F->InitializeDerivedMembers();return F;
}
