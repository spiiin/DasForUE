// this file is generated via daScript automatic C++ binder
// all user modifications will be lost after this file is re-generated

#include "dasUnreal.h"
#include "need_dasUnreal.h"
namespace das {
#include "dasUnreal.func.aot.decl.inc"
void Module_dasUnreal::initFunctions_15() {
	using _method_1165 = das::das_call_member< bool (FFrame::*)(UObject *,void *const),&FFrame::StepAndCheckMostRecentProperty >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Stack.h:211:30
	makeExtern<DAS_CALL_METHOD(_method_1165), SimNode_ExtFuncCall >(lib,"StepAndCheckMostRecentProperty","das_call_member< bool (FFrame::*)(UObject *,void *const) , &FFrame::StepAndCheckMostRecentProperty >::invoke")
		->args({"self","Context","Z_Param__Result"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_1166 = das::das_call_member< void (FFrame::*)(void *const,FProperty *),&FFrame::StepExplicitProperty >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Stack.h:214:23
	makeExtern<DAS_CALL_METHOD(_method_1166), SimNode_ExtFuncCall >(lib,"StepExplicitProperty","das_call_member< void (FFrame::*)(void *const,FProperty *) , &FFrame::StepExplicitProperty >::invoke")
		->args({"self","Result","Property"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_1167 = das::das_call_member< void (FFrame::*)(void *,const FFieldClass *),&FFrame::StepCompiledIn >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Stack.h:219:30
	makeExtern<DAS_CALL_METHOD(_method_1167), SimNode_ExtFuncCall >(lib,"StepCompiledIn","das_call_member< void (FFrame::*)(void *,const FFieldClass *) , &FFrame::StepCompiledIn >::invoke")
		->args({"self","Result","ExpectedPropertyType"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Stack.h:225:31
	makeExtern< void (*)(FFrame &,const wchar_t *,ELogVerbosity::Type,const FName &) , _dasUnreal_virtual_499_Serialize , SimNode_ExtFuncCall >(lib,"Serialize","_dasUnreal_virtual_499_Serialize")
		->args({"self","V","Verbosity","Category"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Stack.h:227:30
	makeExtern< void (*)(const FFrame *,const wchar_t *,ELogVerbosity::Type,FName) , _dasUnreal_static_500_KismetExecutionMessage , SimNode_ExtFuncCall >(lib,"KismetExecutionMessage","_dasUnreal_static_500_KismetExecutionMessage")
		->args({"self","Message","Verbosity","WarningId"})
		->arg_type(0,makeType<TExplicit<const FFrame*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
	using _method_1170 = das::das_call_member< const unsigned char (FFrame::*)() const,&FFrame::PeekCode >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Stack.h:230:14
	makeExtern<DAS_CALL_METHOD(_method_1170), SimNode_ExtFuncCall >(lib,"PeekCode","das_call_member< const unsigned char (FFrame::*)() const , &FFrame::PeekCode >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_1171 = das::das_call_member< void (FFrame::*)(const int),&FFrame::SkipCode >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Stack.h:233:7
	makeExtern<DAS_CALL_METHOD(_method_1171), SimNode_ExtFuncCall >(lib,"SkipCode","das_call_member< void (FFrame::*)(const int) , &FFrame::SkipCode >::invoke")
		->args({"self","NumOps"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_1172 = das::das_call_member< float (FFrame::*)(),&FFrame::ReadFloat >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Stack.h:239:8
	makeExtern<DAS_CALL_METHOD(_method_1172), SimNode_ExtFuncCall >(lib,"ReadFloat","das_call_member< float (FFrame::*)() , &FFrame::ReadFloat >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_1173 = das::das_call_member< double (FFrame::*)(),&FFrame::ReadDouble >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Stack.h:240:9
	makeExtern<DAS_CALL_METHOD(_method_1173), SimNode_ExtFuncCall >(lib,"ReadDouble","das_call_member< double (FFrame::*)() , &FFrame::ReadDouble >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_1174 = das::das_call_member< FName (FFrame::*)(),&FFrame::ReadName >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Stack.h:241:8
	makeExtern<DAS_CALL_METHOD(_method_1174), SimNode_ExtFuncCallAndCopyOrMove >(lib,"ReadName","das_call_member< FName (FFrame::*)() , &FFrame::ReadName >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_1175 = das::das_call_member< UObject * (FFrame::*)(),&FFrame::ReadObject >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Stack.h:242:11
	makeExtern<DAS_CALL_METHOD(_method_1175), SimNode_ExtFuncCall >(lib,"ReadObject","das_call_member< UObject * (FFrame::*)() , &FFrame::ReadObject >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_1176 = das::das_call_member< int (FFrame::*)(),&FFrame::ReadWord >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Stack.h:243:8
	makeExtern<DAS_CALL_METHOD(_method_1176), SimNode_ExtFuncCall >(lib,"ReadWord","das_call_member< int (FFrame::*)() , &FFrame::ReadWord >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_1177 = das::das_call_member< FProperty * (FFrame::*)(),&FFrame::ReadProperty >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Stack.h:244:13
	makeExtern<DAS_CALL_METHOD(_method_1177), SimNode_ExtFuncCall >(lib,"ReadProperty","das_call_member< FProperty * (FFrame::*)() , &FFrame::ReadProperty >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_1178 = das::das_call_member< FProperty * (FFrame::*)(),&FFrame::ReadPropertyUnchecked >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Stack.h:247:13
	makeExtern<DAS_CALL_METHOD(_method_1178), SimNode_ExtFuncCall >(lib,"ReadPropertyUnchecked","das_call_member< FProperty * (FFrame::*)() , &FFrame::ReadPropertyUnchecked >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_1179 = das::das_call_member< unsigned int (FFrame::*)(),&FFrame::ReadCodeSkipCount >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Stack.h:256:19
	makeExtern<DAS_CALL_METHOD(_method_1179), SimNode_ExtFuncCall >(lib,"ReadCodeSkipCount","das_call_member< unsigned int (FFrame::*)() , &FFrame::ReadCodeSkipCount >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_1180 = das::das_call_member< unsigned short (FFrame::*)(FProperty **),&FFrame::ReadVariableSize >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Stack.h:265:19
	makeExtern<DAS_CALL_METHOD(_method_1180), SimNode_ExtFuncCall >(lib,"ReadVariableSize","das_call_member< unsigned short (FFrame::*)(FProperty **) , &FFrame::ReadVariableSize >::invoke")
		->args({"self","ExpressionField"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Stack.h:313:33
	makeExtern< FFrame * (*)(const FFrame *,FFrame *) , _dasUnreal_static_501_PushThreadLocalTopStackFrame , SimNode_ExtFuncCall >(lib,"PushThreadLocalTopStackFrame","_dasUnreal_static_501_PushThreadLocalTopStackFrame")
		->args({"self","NewTopStackFrame"})
		->arg_type(0,makeType<TExplicit<const FFrame*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Stack.h:314:30
	makeExtern< void (*)(const FFrame *,FFrame *) , _dasUnreal_static_502_PopThreadLocalTopStackFrame , SimNode_ExtFuncCall >(lib,"PopThreadLocalTopStackFrame","_dasUnreal_static_502_PopThreadLocalTopStackFrame")
		->args({"self","NewTopStackFrame"})
		->arg_type(0,makeType<TExplicit<const FFrame*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Stack.h:315:33
	makeExtern< FFrame * (*)(const FFrame *) , _dasUnreal_static_503_GetThreadLocalTopStackFrame , SimNode_ExtFuncCall >(lib,"GetThreadLocalTopStackFrame","_dasUnreal_static_503_GetThreadLocalTopStackFrame")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const FFrame*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Stack.h:464:22
	makeExtern< void (*)() , GInitRunaway , SimNode_ExtFuncCall >(lib,"GInitRunaway","GInitRunaway")
		->addToModule(*this, SideEffects::worstDefault);
}
}

