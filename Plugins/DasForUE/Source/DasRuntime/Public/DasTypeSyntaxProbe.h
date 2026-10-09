#pragma once
#include "DasRuntime.h"
inline void RunDasNamedBodyTypes(FDasRuntimeModule& R,UObject* Actor,const FString& Script,TFunctionRef<void(bool,const TCHAR*)> Check);
inline void RunDasTypeSyntaxValues(FDasRuntimeModule& R,UObject* Actor,const FString& Script,UEnum* Enum,UScriptStruct* Struct,TFunctionRef<void(bool,const TCHAR*)> Check){FDasScalarValue EnumArg;EnumArg.Kind=13;EnumArg.ObjectClass=Enum->GetPathName();EnumArg.IntValue=42;auto Mode=R.RunNamedFramePersistent(R.ScriptPath(Script),TEXT("ue_TypedActor_RawMode"),{EnumArg},EDasMethodSignature::Enum,Actor,EnumArg.ObjectClass);Check(Mode.bSucceeded && Mode.Value==42,TEXT("registered_named_enum_runtime_roundtrip"));FDasScalarValue StructArg;StructArg.Kind=12;StructArg.ObjectClass=Struct->GetPathName();StructArg.StructValue.InitializeAs(Struct);auto Config=R.RunNamedFramePersistent(R.ScriptPath(Script),TEXT("ue_TypedActor_RawConfig"),{StructArg},EDasMethodSignature::Struct,Actor,StructArg.ObjectClass);Check(Config.bSucceeded && Config.StructValue.IsValid() && Config.StructValue.GetScriptStruct()==Struct,TEXT("registered_named_struct_runtime_roundtrip"));RunDasNamedBodyTypes(R,Actor,Script,Check);}

inline void RunDasNamedBodyTypes(FDasRuntimeModule& R,UObject* Actor,const FString& Script,TFunctionRef<void(bool,const TCHAR*)> Check){
 auto Mode=R.RunNamedFramePersistent(R.ScriptPath(Script),TEXT("ue_TypedActor_NamedMode"),{},EDasMethodSignature::Int,Actor);
 Check(Mode.bSucceeded && Mode.Value==42,TEXT("body_authored_enum_type_token_executes"));
 auto Struct=R.RunNamedFramePersistent(R.ScriptPath(Script),TEXT("ue_TypedActor_NamedStruct"),{},EDasMethodSignature::Int,Actor);
 Check(Struct.bSucceeded && Struct.Value==15,TEXT("body_authored_struct_type_token_executes"));
 auto Native=R.RunNamedFramePersistent(R.ScriptPath(Script),TEXT("ue_TypedActor_NativeValues"),{},EDasMethodSignature::Int,Actor);
 Check(Native.bSucceeded && Native.Value==7,TEXT("body_native_types_resolve_with_explicit_registry"));
}
