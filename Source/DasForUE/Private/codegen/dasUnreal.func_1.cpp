// this file is generated via daScript automatic C++ binder
// all user modifications will be lost after this file is re-generated

#include "dasUnreal.h"
#include "need_dasUnreal.h"
namespace das {
#include "dasUnreal.func.aot.decl.inc"
void Module_dasUnreal::initFunctions_1() {
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Logging/LogVerbosity.h:72:23
	makeExtern< const wchar_t * (*)(ELogVerbosity::Type) , ToString , SimNode_ExtFuncCall >(lib,"ToString","ToString")
		->args({"Verbosity"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Logging/LogVerbosity.h:79:37
	makeExtern< ELogVerbosity::Type (*)(const FString &) , ParseLogVerbosityFromString , SimNode_ExtFuncCall >(lib,"ParseLogVerbosityFromString","ParseLogVerbosityFromString")
		->args({"VerbosityString"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Containers/UnrealString.h:41:14
	makeExtern< wchar_t * (*)(FString &) , GetData , SimNode_ExtFuncCall >(lib,"GetData","GetData")
		->args({""})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Containers/UnrealString.h:42:14
	makeExtern< const wchar_t * (*)(const FString &) , GetData , SimNode_ExtFuncCall >(lib,"GetData","GetData")
		->args({""})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Containers/UnrealString.h:43:14
	makeExtern< int (*)(const FString &) , GetNum , SimNode_ExtFuncCall >(lib,"GetNum","GetNum")
		->args({"String"})
		->addToModule(*this, SideEffects::worstDefault);
	addCtorAndUsing<FString>(*this,lib,"FString","FString");
	addCtorAndUsing<FString,const FString &>(*this,lib,"FString","FString")
		->args({""});
	addCtorAndUsing<FString,const FString &,int>(*this,lib,"FString","FString")
		->args({"Other","ExtraSlack"});
	addCtorAndUsing<FString,const char *>(*this,lib,"FString","FString")
		->args({"Str"});
	addCtorAndUsing<FString,int,const char *>(*this,lib,"FString","FString")
		->args({"Len","Str"});
	addCtorAndUsing<FString,const char *,int>(*this,lib,"FString","FString")
		->args({"Str","ExtraSlack"});
	using _method_1 = das::das_call_member< SIZE_T (FString::*)() const,&FString::GetAllocatedSize >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Containers/UnrealString.h:266:21
	makeExtern<DAS_CALL_METHOD(_method_1), SimNode_ExtFuncCall >(lib,"GetAllocatedSize","das_call_member< SIZE_T (FString::*)() const , &FString::GetAllocatedSize >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_2 = das::das_call_member< void (FString::*)() const,&FString::CheckInvariants >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Containers/UnrealString.h:274:19
	makeExtern<DAS_CALL_METHOD(_method_2), SimNode_ExtFuncCall >(lib,"CheckInvariants","das_call_member< void (FString::*)() const , &FString::CheckInvariants >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_3 = das::das_call_member< void (FString::*)(),&FString::Empty >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Containers/UnrealString.h:287:16
	makeExtern<DAS_CALL_METHOD(_method_3), SimNode_ExtFuncCall >(lib,"Empty","das_call_member< void (FString::*)() , &FString::Empty >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_4 = das::das_call_member< void (FString::*)(int),&FString::Empty >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Containers/UnrealString.h:288:16
	makeExtern<DAS_CALL_METHOD(_method_4), SimNode_ExtFuncCall >(lib,"Empty","das_call_member< void (FString::*)(int) , &FString::Empty >::invoke")
		->args({"self","Slack"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_5 = das::das_call_member< bool (FString::*)() const,&FString::IsEmpty >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Containers/UnrealString.h:295:32
	makeExtern<DAS_CALL_METHOD(_method_5), SimNode_ExtFuncCall >(lib,"IsEmpty","das_call_member< bool (FString::*)() const , &FString::IsEmpty >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_6 = das::das_call_member< void (FString::*)(int),&FString::Reset >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Containers/UnrealString.h:305:16
	makeExtern<DAS_CALL_METHOD(_method_6), SimNode_ExtFuncCall >(lib,"Reset","das_call_member< void (FString::*)(int) , &FString::Reset >::invoke")
		->args({"self","NewReservedSize"})
		->arg_init(1,make_smart<ExprConstInt>(0))
		->addToModule(*this, SideEffects::worstDefault);
	using _method_7 = das::das_call_member< void (FString::*)(),&FString::Shrink >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Containers/UnrealString.h:310:16
	makeExtern<DAS_CALL_METHOD(_method_7), SimNode_ExtFuncCall >(lib,"Shrink","das_call_member< void (FString::*)() , &FString::Shrink >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_8 = das::das_call_member< bool (FString::*)(int) const,&FString::IsValidIndex >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Containers/UnrealString.h:319:32
	makeExtern<DAS_CALL_METHOD(_method_8), SimNode_ExtFuncCall >(lib,"IsValidIndex","das_call_member< bool (FString::*)(int) const , &FString::IsValidIndex >::invoke")
		->args({"self","Index"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_9 = das::das_call_member< void (FString::*)(const char *,int),&FString::AppendChars >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Containers/UnrealString.h:361:16
	makeExtern<DAS_CALL_METHOD(_method_9), SimNode_ExtFuncCall >(lib,"AppendChars","das_call_member< void (FString::*)(const char *,int) , &FString::AppendChars >::invoke")
		->args({"self","Str","Count"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_10 = das::das_call_member< FString & (FString::*)(wchar_t),&FString::AppendChar >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Containers/UnrealString.h:398:20
	makeExtern<DAS_CALL_METHOD(_method_10), SimNode_ExtFuncCallRef >(lib,"AppendChar","das_call_member< FString & (FString::*)(wchar_t) , &FString::AppendChar >::invoke")
		->args({"self","InChar"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_11 = das::das_call_member< void (FString::*)(int,wchar_t),&FString::InsertAt >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Containers/UnrealString.h:425:16
	makeExtern<DAS_CALL_METHOD(_method_11), SimNode_ExtFuncCall >(lib,"InsertAt","das_call_member< void (FString::*)(int,wchar_t) , &FString::InsertAt >::invoke")
		->args({"self","Index","Character"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_12 = das::das_call_member< void (FString::*)(int,const FString &),&FString::InsertAt >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Containers/UnrealString.h:426:16
	makeExtern<DAS_CALL_METHOD(_method_12), SimNode_ExtFuncCall >(lib,"InsertAt","das_call_member< void (FString::*)(int,const FString &) , &FString::InsertAt >::invoke")
		->args({"self","Index","Characters"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_13 = das::das_call_member< void (FString::*)(int,int,bool),&FString::RemoveAt >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Containers/UnrealString.h:435:16
	makeExtern<DAS_CALL_METHOD(_method_13), SimNode_ExtFuncCall >(lib,"RemoveAt","das_call_member< void (FString::*)(int,int,bool) , &FString::RemoveAt >::invoke")
		->args({"self","Index","Count","bAllowShrinking"})
		->arg_init(2,make_smart<ExprConstInt>(1))
		->arg_init(3,make_smart<ExprConstBool>(true))
		->addToModule(*this, SideEffects::worstDefault);
	using _method_14 = das::das_call_member< bool (FString::*)(const wchar_t *,ESearchCase::Type),&FString::RemoveFromStart >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Containers/UnrealString.h:456:7
	makeExtern<DAS_CALL_METHOD(_method_14), SimNode_ExtFuncCall >(lib,"RemoveFromStart","das_call_member< bool (FString::*)(const wchar_t *,ESearchCase::Type) , &FString::RemoveFromStart >::invoke")
		->args({"self","InPrefix","SearchCase"})
		->arg_init(2,make_smart<ExprConstEnumeration>(1,makeType<ESearchCase::Type>(lib)))
		->addToModule(*this, SideEffects::worstDefault);
	using _method_15 = das::das_call_member< bool (FString::*)(const FString &,ESearchCase::Type),&FString::RemoveFromStart >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Containers/UnrealString.h:467:7
	makeExtern<DAS_CALL_METHOD(_method_15), SimNode_ExtFuncCall >(lib,"RemoveFromStart","das_call_member< bool (FString::*)(const FString &,ESearchCase::Type) , &FString::RemoveFromStart >::invoke")
		->args({"self","InPrefix","SearchCase"})
		->arg_init(2,make_smart<ExprConstEnumeration>(1,makeType<ESearchCase::Type>(lib)))
		->addToModule(*this, SideEffects::worstDefault);
	using _method_16 = das::das_call_member< bool (FString::*)(const wchar_t *,int,ESearchCase::Type),&FString::RemoveFromStart >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Containers/UnrealString.h:479:16
	makeExtern<DAS_CALL_METHOD(_method_16), SimNode_ExtFuncCall >(lib,"RemoveFromStart","das_call_member< bool (FString::*)(const wchar_t *,int,ESearchCase::Type) , &FString::RemoveFromStart >::invoke")
		->args({"self","InPrefix","InPrefixLen","SearchCase"})
		->arg_init(3,make_smart<ExprConstEnumeration>(1,makeType<ESearchCase::Type>(lib)))
		->addToModule(*this, SideEffects::worstDefault);
	using _method_17 = das::das_call_member< bool (FString::*)(const wchar_t *,ESearchCase::Type),&FString::RemoveFromEnd >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Containers/UnrealString.h:500:7
	makeExtern<DAS_CALL_METHOD(_method_17), SimNode_ExtFuncCall >(lib,"RemoveFromEnd","das_call_member< bool (FString::*)(const wchar_t *,ESearchCase::Type) , &FString::RemoveFromEnd >::invoke")
		->args({"self","InSuffix","SearchCase"})
		->arg_init(2,make_smart<ExprConstEnumeration>(1,makeType<ESearchCase::Type>(lib)))
		->addToModule(*this, SideEffects::worstDefault);
	using _method_18 = das::das_call_member< bool (FString::*)(const FString &,ESearchCase::Type),&FString::RemoveFromEnd >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Containers/UnrealString.h:511:7
	makeExtern<DAS_CALL_METHOD(_method_18), SimNode_ExtFuncCall >(lib,"RemoveFromEnd","das_call_member< bool (FString::*)(const FString &,ESearchCase::Type) , &FString::RemoveFromEnd >::invoke")
		->args({"self","InSuffix","SearchCase"})
		->arg_init(2,make_smart<ExprConstEnumeration>(1,makeType<ESearchCase::Type>(lib)))
		->addToModule(*this, SideEffects::worstDefault);
	using _method_19 = das::das_call_member< bool (FString::*)(const wchar_t *,int,ESearchCase::Type),&FString::RemoveFromEnd >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Containers/UnrealString.h:523:16
	makeExtern<DAS_CALL_METHOD(_method_19), SimNode_ExtFuncCall >(lib,"RemoveFromEnd","das_call_member< bool (FString::*)(const wchar_t *,int,ESearchCase::Type) , &FString::RemoveFromEnd >::invoke")
		->args({"self","InSuffix","InSuffixLen","SearchCase"})
		->arg_init(3,make_smart<ExprConstEnumeration>(1,makeType<ESearchCase::Type>(lib)))
		->addToModule(*this, SideEffects::worstDefault);
	using _method_20 = das::das_call_member< void (FString::*)(const wchar_t *,int),&FString::PathAppend >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Containers/UnrealString.h:531:16
	makeExtern<DAS_CALL_METHOD(_method_20), SimNode_ExtFuncCall >(lib,"PathAppend","das_call_member< void (FString::*)(const wchar_t *,int) , &FString::PathAppend >::invoke")
		->args({"self","Str","StrLength"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_21 = das::das_call_member< int (FString::*)() const,&FString::Len >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Containers/UnrealString.h:976:33
	makeExtern<DAS_CALL_METHOD(_method_21), SimNode_ExtFuncCall >(lib,"Len","das_call_member< int (FString::*)() const , &FString::Len >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_22 = das::das_call_member< void (FString::*)(int,bool),&FString::LeftInline >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Containers/UnrealString.h:994:19
	makeExtern<DAS_CALL_METHOD(_method_22), SimNode_ExtFuncCall >(lib,"LeftInline","das_call_member< void (FString::*)(int,bool) , &FString::LeftInline >::invoke")
		->args({"self","Count","bAllowShrinking"})
		->arg_init(2,make_smart<ExprConstBool>(true))
		->addToModule(*this, SideEffects::worstDefault);
	using _method_23 = das::das_call_member< void (FString::*)(int,bool),&FString::LeftChopInline >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Containers/UnrealString.h:1015:19
	makeExtern<DAS_CALL_METHOD(_method_23), SimNode_ExtFuncCall >(lib,"LeftChopInline","das_call_member< void (FString::*)(int,bool) , &FString::LeftChopInline >::invoke")
		->args({"self","Count","bAllowShrinking"})
		->arg_init(2,make_smart<ExprConstBool>(true))
		->addToModule(*this, SideEffects::worstDefault);
	using _method_24 = das::das_call_member< void (FString::*)(int,bool),&FString::RightInline >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Containers/UnrealString.h:1035:19
	makeExtern<DAS_CALL_METHOD(_method_24), SimNode_ExtFuncCall >(lib,"RightInline","das_call_member< void (FString::*)(int,bool) , &FString::RightInline >::invoke")
		->args({"self","Count","bAllowShrinking"})
		->arg_init(2,make_smart<ExprConstBool>(true))
		->addToModule(*this, SideEffects::worstDefault);
	using _method_25 = das::das_call_member< void (FString::*)(int,bool),&FString::RightChopInline >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Containers/UnrealString.h:1051:19
	makeExtern<DAS_CALL_METHOD(_method_25), SimNode_ExtFuncCall >(lib,"RightChopInline","das_call_member< void (FString::*)(int,bool) , &FString::RightChopInline >::invoke")
		->args({"self","Count","bAllowShrinking"})
		->arg_init(2,make_smart<ExprConstBool>(true))
		->addToModule(*this, SideEffects::worstDefault);
	using _method_26 = das::das_call_member< void (FString::*)(int,int,bool),&FString::MidInline >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Containers/UnrealString.h:1065:19
	makeExtern<DAS_CALL_METHOD(_method_26), SimNode_ExtFuncCall >(lib,"MidInline","das_call_member< void (FString::*)(int,int,bool) , &FString::MidInline >::invoke")
		->args({"self","Start","Count","bAllowShrinking"})
		->arg_init(2,make_smart<ExprConstInt>(2147483647))
		->arg_init(3,make_smart<ExprConstBool>(true))
		->addToModule(*this, SideEffects::worstDefault);
	using _method_27 = das::das_call_member< int (FString::*)(const wchar_t *,ESearchCase::Type,ESearchDir::Type,int) const,&FString::Find >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Containers/UnrealString.h:1107:21
	makeExtern<DAS_CALL_METHOD(_method_27), SimNode_ExtFuncCall >(lib,"Find","das_call_member< int (FString::*)(const wchar_t *,ESearchCase::Type,ESearchDir::Type,int) const , &FString::Find >::invoke")
		->args({"self","SubStr","SearchCase","SearchDir","StartPosition"})
		->arg_init(2,make_smart<ExprConstEnumeration>(1,makeType<ESearchCase::Type>(lib)))
		->arg_init(3,make_smart<ExprConstEnumeration>(0,makeType<ESearchDir::Type>(lib)))
		->arg_init(4,make_smart<ExprConstInt>(-1))
		->addToModule(*this, SideEffects::worstDefault);
	using _method_28 = das::das_call_member< int (FString::*)(const FString &,ESearchCase::Type,ESearchDir::Type,int) const,&FString::Find >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Containers/UnrealString.h:1129:21
	makeExtern<DAS_CALL_METHOD(_method_28), SimNode_ExtFuncCall >(lib,"Find","das_call_member< int (FString::*)(const FString &,ESearchCase::Type,ESearchDir::Type,int) const , &FString::Find >::invoke")
		->args({"self","SubStr","SearchCase","SearchDir","StartPosition"})
		->arg_init(2,make_smart<ExprConstEnumeration>(1,makeType<ESearchCase::Type>(lib)))
		->arg_init(3,make_smart<ExprConstEnumeration>(0,makeType<ESearchDir::Type>(lib)))
		->arg_init(4,make_smart<ExprConstInt>(-1))
		->addToModule(*this, SideEffects::worstDefault);
	using _method_29 = das::das_call_member< int (FString::*)(const wchar_t *,int,ESearchCase::Type,ESearchDir::Type,int) const,&FString::Find >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Containers/UnrealString.h:1152:30
	makeExtern<DAS_CALL_METHOD(_method_29), SimNode_ExtFuncCall >(lib,"Find","das_call_member< int (FString::*)(const wchar_t *,int,ESearchCase::Type,ESearchDir::Type,int) const , &FString::Find >::invoke")
		->args({"self","SubStr","InSubStrLen","SearchCase","SearchDir","StartPosition"})
		->arg_init(3,make_smart<ExprConstEnumeration>(1,makeType<ESearchCase::Type>(lib)))
		->arg_init(4,make_smart<ExprConstEnumeration>(0,makeType<ESearchDir::Type>(lib)))
		->arg_init(5,make_smart<ExprConstInt>(-1))
		->addToModule(*this, SideEffects::worstDefault);
	using _method_30 = das::das_call_member< bool (FString::*)(const wchar_t *,ESearchCase::Type,ESearchDir::Type) const,&FString::Contains >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Containers/UnrealString.h:1180:32
	makeExtern<DAS_CALL_METHOD(_method_30), SimNode_ExtFuncCall >(lib,"Contains","das_call_member< bool (FString::*)(const wchar_t *,ESearchCase::Type,ESearchDir::Type) const , &FString::Contains >::invoke")
		->args({"self","SubStr","SearchCase","SearchDir"})
		->arg_init(2,make_smart<ExprConstEnumeration>(1,makeType<ESearchCase::Type>(lib)))
		->arg_init(3,make_smart<ExprConstEnumeration>(0,makeType<ESearchDir::Type>(lib)))
		->addToModule(*this, SideEffects::worstDefault);
	using _method_31 = das::das_call_member< bool (FString::*)(const FString &,ESearchCase::Type,ESearchDir::Type) const,&FString::Contains >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Containers/UnrealString.h:1194:32
	makeExtern<DAS_CALL_METHOD(_method_31), SimNode_ExtFuncCall >(lib,"Contains","das_call_member< bool (FString::*)(const FString &,ESearchCase::Type,ESearchDir::Type) const , &FString::Contains >::invoke")
		->args({"self","SubStr","SearchCase","SearchDir"})
		->arg_init(2,make_smart<ExprConstEnumeration>(1,makeType<ESearchCase::Type>(lib)))
		->arg_init(3,make_smart<ExprConstEnumeration>(0,makeType<ESearchDir::Type>(lib)))
		->addToModule(*this, SideEffects::worstDefault);
	using _method_32 = das::das_call_member< bool (FString::*)(const wchar_t *,int,ESearchCase::Type,ESearchDir::Type) const,&FString::Contains >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Containers/UnrealString.h:1209:32
	makeExtern<DAS_CALL_METHOD(_method_32), SimNode_ExtFuncCall >(lib,"Contains","das_call_member< bool (FString::*)(const wchar_t *,int,ESearchCase::Type,ESearchDir::Type) const , &FString::Contains >::invoke")
		->args({"self","SubStr","SubStrLen","SearchCase","SearchDir"})
		->arg_init(3,make_smart<ExprConstEnumeration>(1,makeType<ESearchCase::Type>(lib)))
		->arg_init(4,make_smart<ExprConstEnumeration>(0,makeType<ESearchDir::Type>(lib)))
		->addToModule(*this, SideEffects::worstDefault);
	using _method_33 = das::das_call_member< bool (FString::*)(wchar_t,int &) const,&FString::FindChar >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Containers/UnrealString.h:1222:19
	makeExtern<DAS_CALL_METHOD(_method_33), SimNode_ExtFuncCall >(lib,"FindChar","das_call_member< bool (FString::*)(wchar_t,int &) const , &FString::FindChar >::invoke")
		->args({"self","InChar","Index"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_34 = das::das_call_member< bool (FString::*)(wchar_t,int &) const,&FString::FindLastChar >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Containers/UnrealString.h:1234:19
	makeExtern<DAS_CALL_METHOD(_method_34), SimNode_ExtFuncCall >(lib,"FindLastChar","das_call_member< bool (FString::*)(wchar_t,int &) const , &FString::FindLastChar >::invoke")
		->args({"self","InChar","Index"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_35 = das::das_call_member< bool (FString::*)(const FString &,ESearchCase::Type) const,&FString::Equals >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Containers/UnrealString.h:1275:32
	makeExtern<DAS_CALL_METHOD(_method_35), SimNode_ExtFuncCall >(lib,"Equals","das_call_member< bool (FString::*)(const FString &,ESearchCase::Type) const , &FString::Equals >::invoke")
		->args({"self","Other","SearchCase"})
		->arg_init(2,make_smart<ExprConstEnumeration>(0,makeType<ESearchCase::Type>(lib)))
		->addToModule(*this, SideEffects::worstDefault);
	using _method_36 = das::das_call_member< int (FString::*)(const FString &,ESearchCase::Type) const,&FString::Compare >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Containers/UnrealString.h:1307:33
	makeExtern<DAS_CALL_METHOD(_method_36), SimNode_ExtFuncCall >(lib,"Compare","das_call_member< int (FString::*)(const FString &,ESearchCase::Type) const , &FString::Compare >::invoke")
		->args({"self","Other","SearchCase"})
		->arg_init(2,make_smart<ExprConstEnumeration>(0,makeType<ESearchCase::Type>(lib)))
		->addToModule(*this, SideEffects::worstDefault);
	using _method_37 = das::das_call_member< bool (FString::*)(const FString &,FString *,FString *,ESearchCase::Type,ESearchDir::Type) const,&FString::Split >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Containers/UnrealString.h:1329:16
	makeExtern<DAS_CALL_METHOD(_method_37), SimNode_ExtFuncCall >(lib,"Split","das_call_member< bool (FString::*)(const FString &,FString *,FString *,ESearchCase::Type,ESearchDir::Type) const , &FString::Split >::invoke")
		->args({"self","InS","LeftS","RightS","SearchCase","SearchDir"})
		->arg_init(5,make_smart<ExprConstEnumeration>(0,makeType<ESearchDir::Type>(lib)))
		->addToModule(*this, SideEffects::worstDefault);
	using _method_38 = das::das_call_member< bool (FString::*)(const FString &,FString *,FString *) const,&FString::Split >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Containers/UnrealString.h:1333:16
	makeExtern<DAS_CALL_METHOD(_method_38), SimNode_ExtFuncCall >(lib,"Split","das_call_member< bool (FString::*)(const FString &,FString *,FString *) const , &FString::Split >::invoke")
		->args({"self","InS","LeftS","RightS"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_39 = das::das_call_member< void (FString::*)(),&FString::ToUpperInline >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Containers/UnrealString.h:1345:16
	makeExtern<DAS_CALL_METHOD(_method_39), SimNode_ExtFuncCall >(lib,"ToUpperInline","das_call_member< void (FString::*)() , &FString::ToUpperInline >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_40 = das::das_call_member< void (FString::*)(),&FString::ToLowerInline >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Containers/UnrealString.h:1357:16
	makeExtern<DAS_CALL_METHOD(_method_40), SimNode_ExtFuncCall >(lib,"ToLowerInline","das_call_member< void (FString::*)() , &FString::ToLowerInline >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_41 = das::das_call_member< FString (FString::*)(int) const,&FString::LeftPad >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Containers/UnrealString.h:1360:32
	makeExtern<DAS_CALL_METHOD(_method_41), SimNode_ExtFuncCallAndCopyOrMove >(lib,"LeftPad","das_call_member< FString (FString::*)(int) const , &FString::LeftPad >::invoke")
		->args({"self","ChCount"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_42 = das::das_call_member< FString (FString::*)(int) const,&FString::RightPad >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Containers/UnrealString.h:1363:32
	makeExtern<DAS_CALL_METHOD(_method_42), SimNode_ExtFuncCallAndCopyOrMove >(lib,"RightPad","das_call_member< FString (FString::*)(int) const , &FString::RightPad >::invoke")
		->args({"self","ChCount"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_43 = das::das_call_member< bool (FString::*)() const,&FString::IsNumeric >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Containers/UnrealString.h:1366:29
	makeExtern<DAS_CALL_METHOD(_method_43), SimNode_ExtFuncCall >(lib,"IsNumeric","das_call_member< bool (FString::*)() const , &FString::IsNumeric >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_44 = das::das_call_member< void (FString::*)(),&FString::RemoveSpacesInline >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Containers/UnrealString.h:1369:16
	makeExtern<DAS_CALL_METHOD(_method_44), SimNode_ExtFuncCall >(lib,"RemoveSpacesInline","das_call_member< void (FString::*)() , &FString::RemoveSpacesInline >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Containers/UnrealString.h:1424:39
	makeExtern< FString (*)(const FString *,wchar_t) , _dasUnreal_static_0_Chr , SimNode_ExtFuncCallAndCopyOrMove >(lib,"Chr","_dasUnreal_static_0_Chr")
		->args({"self","Ch"})
		->arg_type(0,makeType<TExplicit<const FString*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Containers/UnrealString.h:1434:39
	makeExtern< FString (*)(const FString *,int,wchar_t) , _dasUnreal_static_1_ChrN , SimNode_ExtFuncCallAndCopyOrMove >(lib,"ChrN","_dasUnreal_static_1_ChrN")
		->args({"self","NumCharacters","Char"})
		->arg_type(0,makeType<TExplicit<const FString*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
	using _method_47 = das::das_call_member< bool (FString::*)(const wchar_t *,ESearchCase::Type) const,&FString::StartsWith >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Containers/UnrealString.h:1466:20
	makeExtern<DAS_CALL_METHOD(_method_47), SimNode_ExtFuncCall >(lib,"StartsWith","das_call_member< bool (FString::*)(const wchar_t *,ESearchCase::Type) const , &FString::StartsWith >::invoke")
		->args({"self","InPrefix","SearchCase"})
		->arg_init(2,make_smart<ExprConstEnumeration>(1,makeType<ESearchCase::Type>(lib)))
		->addToModule(*this, SideEffects::worstDefault);
	using _method_48 = das::das_call_member< bool (FString::*)(const FString &,ESearchCase::Type) const,&FString::StartsWith >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Containers/UnrealString.h:1477:20
	makeExtern<DAS_CALL_METHOD(_method_48), SimNode_ExtFuncCall >(lib,"StartsWith","das_call_member< bool (FString::*)(const FString &,ESearchCase::Type) const , &FString::StartsWith >::invoke")
		->args({"self","InPrefix","SearchCase"})
		->arg_init(2,make_smart<ExprConstEnumeration>(1,makeType<ESearchCase::Type>(lib)))
		->addToModule(*this, SideEffects::worstDefault);
	using _method_49 = das::das_call_member< bool (FString::*)(const wchar_t *,int,ESearchCase::Type) const,&FString::StartsWith >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Containers/UnrealString.h:1488:29
	makeExtern<DAS_CALL_METHOD(_method_49), SimNode_ExtFuncCall >(lib,"StartsWith","das_call_member< bool (FString::*)(const wchar_t *,int,ESearchCase::Type) const , &FString::StartsWith >::invoke")
		->args({"self","InPrefix","InPrefixLen","SearchCase"})
		->arg_init(3,make_smart<ExprConstEnumeration>(1,makeType<ESearchCase::Type>(lib)))
		->addToModule(*this, SideEffects::worstDefault);
	using _method_50 = das::das_call_member< bool (FString::*)(const wchar_t *,ESearchCase::Type) const,&FString::EndsWith >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Containers/UnrealString.h:1509:20
	makeExtern<DAS_CALL_METHOD(_method_50), SimNode_ExtFuncCall >(lib,"EndsWith","das_call_member< bool (FString::*)(const wchar_t *,ESearchCase::Type) const , &FString::EndsWith >::invoke")
		->args({"self","InSuffix","SearchCase"})
		->arg_init(2,make_smart<ExprConstEnumeration>(1,makeType<ESearchCase::Type>(lib)))
		->addToModule(*this, SideEffects::worstDefault);
	using _method_51 = das::das_call_member< bool (FString::*)(const FString &,ESearchCase::Type) const,&FString::EndsWith >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Containers/UnrealString.h:1520:20
	makeExtern<DAS_CALL_METHOD(_method_51), SimNode_ExtFuncCall >(lib,"EndsWith","das_call_member< bool (FString::*)(const FString &,ESearchCase::Type) const , &FString::EndsWith >::invoke")
		->args({"self","InSuffix","SearchCase"})
		->arg_init(2,make_smart<ExprConstEnumeration>(1,makeType<ESearchCase::Type>(lib)))
		->addToModule(*this, SideEffects::worstDefault);
	using _method_52 = das::das_call_member< bool (FString::*)(const wchar_t *,int,ESearchCase::Type) const,&FString::EndsWith >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Containers/UnrealString.h:1531:29
	makeExtern<DAS_CALL_METHOD(_method_52), SimNode_ExtFuncCall >(lib,"EndsWith","das_call_member< bool (FString::*)(const wchar_t *,int,ESearchCase::Type) const , &FString::EndsWith >::invoke")
		->args({"self","InSuffix","InSuffixLen","SearchCase"})
		->arg_init(3,make_smart<ExprConstEnumeration>(1,makeType<ESearchCase::Type>(lib)))
		->addToModule(*this, SideEffects::worstDefault);
	using _method_53 = das::das_call_member< bool (FString::*)(const wchar_t *,ESearchCase::Type) const,&FString::MatchesWildcard >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Containers/UnrealString.h:1556:20
	makeExtern<DAS_CALL_METHOD(_method_53), SimNode_ExtFuncCall >(lib,"MatchesWildcard","das_call_member< bool (FString::*)(const wchar_t *,ESearchCase::Type) const , &FString::MatchesWildcard >::invoke")
		->args({"self","Wildcard","SearchCase"})
		->arg_init(2,make_smart<ExprConstEnumeration>(1,makeType<ESearchCase::Type>(lib)))
		->addToModule(*this, SideEffects::worstDefault);
	using _method_54 = das::das_call_member< bool (FString::*)(const FString &,ESearchCase::Type) const,&FString::MatchesWildcard >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Containers/UnrealString.h:1569:20
	makeExtern<DAS_CALL_METHOD(_method_54), SimNode_ExtFuncCall >(lib,"MatchesWildcard","das_call_member< bool (FString::*)(const FString &,ESearchCase::Type) const , &FString::MatchesWildcard >::invoke")
		->args({"self","Wildcard","SearchCase"})
		->arg_init(2,make_smart<ExprConstEnumeration>(1,makeType<ESearchCase::Type>(lib)))
		->addToModule(*this, SideEffects::worstDefault);
	using _method_55 = das::das_call_member< bool (FString::*)(const wchar_t *,int,ESearchCase::Type) const,&FString::MatchesWildcard >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Containers/UnrealString.h:1582:29
	makeExtern<DAS_CALL_METHOD(_method_55), SimNode_ExtFuncCall >(lib,"MatchesWildcard","das_call_member< bool (FString::*)(const wchar_t *,int,ESearchCase::Type) const , &FString::MatchesWildcard >::invoke")
		->args({"self","Wildcard","WildcardLen","SearchCase"})
		->arg_init(3,make_smart<ExprConstEnumeration>(1,makeType<ESearchCase::Type>(lib)))
		->addToModule(*this, SideEffects::worstDefault);
	using _method_56 = das::das_call_member< void (FString::*)(),&FString::TrimStartAndEndInline >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Containers/UnrealString.h:1587:16
	makeExtern<DAS_CALL_METHOD(_method_56), SimNode_ExtFuncCall >(lib,"TrimStartAndEndInline","das_call_member< void (FString::*)() , &FString::TrimStartAndEndInline >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_57 = das::das_call_member< void (FString::*)(),&FString::TrimStartInline >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Containers/UnrealString.h:1604:16
	makeExtern<DAS_CALL_METHOD(_method_57), SimNode_ExtFuncCall >(lib,"TrimStartInline","das_call_member< void (FString::*)() , &FString::TrimStartInline >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_58 = das::das_call_member< void (FString::*)(),&FString::TrimEndInline >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Containers/UnrealString.h:1621:16
	makeExtern<DAS_CALL_METHOD(_method_58), SimNode_ExtFuncCall >(lib,"TrimEndInline","das_call_member< void (FString::*)() , &FString::TrimEndInline >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_59 = das::das_call_member< void (FString::*)(),&FString::TrimToNullTerminator >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Containers/UnrealString.h:1638:16
	makeExtern<DAS_CALL_METHOD(_method_59), SimNode_ExtFuncCall >(lib,"TrimToNullTerminator","das_call_member< void (FString::*)() , &FString::TrimToNullTerminator >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_60 = das::das_call_member< void (FString::*)(bool *),&FString::TrimQuotesInline >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Containers/UnrealString.h:1644:16
	makeExtern<DAS_CALL_METHOD(_method_60), SimNode_ExtFuncCall >(lib,"TrimQuotesInline","das_call_member< void (FString::*)(bool *) , &FString::TrimQuotesInline >::invoke")
		->args({"self","bQuotesRemoved"})
		->arg_init(1,make_smart<ExprConstPtr>())
		->addToModule(*this, SideEffects::worstDefault);
	using _method_61 = das::das_call_member< void (FString::*)(const wchar_t,bool *),&FString::TrimCharInline >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Containers/UnrealString.h:1650:16
	makeExtern<DAS_CALL_METHOD(_method_61), SimNode_ExtFuncCall >(lib,"TrimCharInline","das_call_member< void (FString::*)(const wchar_t,bool *) , &FString::TrimCharInline >::invoke")
		->args({"self","CharacterToTrim","bCharRemoved"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_62 = das::das_call_member< void (FString::*)(),&FString::ReverseString >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Containers/UnrealString.h:1740:16
	makeExtern<DAS_CALL_METHOD(_method_62), SimNode_ExtFuncCall >(lib,"ReverseString","das_call_member< void (FString::*)() , &FString::ReverseString >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_63 = das::das_call_member< int (FString::*)(const wchar_t *,const wchar_t *,ESearchCase::Type),&FString::ReplaceInline >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Containers/UnrealString.h:1771:17
	makeExtern<DAS_CALL_METHOD(_method_63), SimNode_ExtFuncCall >(lib,"ReplaceInline","das_call_member< int (FString::*)(const wchar_t *,const wchar_t *,ESearchCase::Type) , &FString::ReplaceInline >::invoke")
		->args({"self","SearchText","ReplacementText","SearchCase"})
		->arg_init(3,make_smart<ExprConstEnumeration>(1,makeType<ESearchCase::Type>(lib)))
		->addToModule(*this, SideEffects::worstDefault);
	using _method_64 = das::das_call_member< void (FString::*)(const wchar_t,const wchar_t,ESearchCase::Type),&FString::ReplaceCharInline >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Containers/UnrealString.h:1781:7
	makeExtern<DAS_CALL_METHOD(_method_64), SimNode_ExtFuncCall >(lib,"ReplaceCharInline","das_call_member< void (FString::*)(const wchar_t,const wchar_t,ESearchCase::Type) , &FString::ReplaceCharInline >::invoke")
		->args({"self","SearchChar","ReplacementChar","SearchCase"})
		->arg_init(3,make_smart<ExprConstEnumeration>(1,makeType<ESearchCase::Type>(lib)))
		->addToModule(*this, SideEffects::worstDefault);
	using _method_65 = das::das_call_member< void (FString::*)(const int),&FString::ConvertTabsToSpacesInline >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Containers/UnrealString.h:1884:16
	makeExtern<DAS_CALL_METHOD(_method_65), SimNode_ExtFuncCall >(lib,"ConvertTabsToSpacesInline","das_call_member< void (FString::*)(const int) , &FString::ConvertTabsToSpacesInline >::invoke")
		->args({"self","InSpacesPerTab"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Containers/UnrealString.h:1910:39
	makeExtern< FString (*)(const FString *,int) , _dasUnreal_static_2_FormatAsNumber , SimNode_ExtFuncCallAndCopyOrMove >(lib,"FormatAsNumber","_dasUnreal_static_2_FormatAsNumber")
		->args({"self","InNumber"})
		->arg_type(0,makeType<TExplicit<const FString*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
	using _method_67 = das::das_call_member< void (FString::*)(int),&FString::Reserve >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Containers/UnrealString.h:1913:16
	makeExtern<DAS_CALL_METHOD(_method_67), SimNode_ExtFuncCall >(lib,"Reserve","das_call_member< void (FString::*)(int) , &FString::Reserve >::invoke")
		->args({"self","CharacterCount"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Containers/UnrealString.h:1926:42
	makeExtern< FString (*)(const FString *,int) , _dasUnreal_static_3_FromInt , SimNode_ExtFuncCallAndCopyOrMove >(lib,"FromInt","_dasUnreal_static_3_FromInt")
		->args({"self","Num"})
		->arg_type(0,makeType<TExplicit<const FString*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
	using _method_69 = das::das_call_member< void (FString::*)(int),&FString::AppendInt >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Containers/UnrealString.h:1934:16
	makeExtern<DAS_CALL_METHOD(_method_69), SimNode_ExtFuncCall >(lib,"AppendInt","das_call_member< void (FString::*)(int) , &FString::AppendInt >::invoke")
		->args({"self","InNum"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_70 = das::das_call_member< bool (FString::*)() const,&FString::ToBool >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Containers/UnrealString.h:1943:29
	makeExtern<DAS_CALL_METHOD(_method_70), SimNode_ExtFuncCall >(lib,"ToBool","das_call_member< bool (FString::*)() const , &FString::ToBool >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Containers/UnrealString.h:1953:39
	makeExtern< FString (*)(const FString *,const unsigned char *,const unsigned int) , _dasUnreal_static_4_FromBlob , SimNode_ExtFuncCallAndCopyOrMove >(lib,"FromBlob","_dasUnreal_static_4_FromBlob")
		->args({"self","SrcBuffer","SrcSize"})
		->arg_type(0,makeType<TExplicit<const FString*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Containers/UnrealString.h:1963:23
	makeExtern< bool (*)(const FString *,const FString &,unsigned char *,const unsigned int) , _dasUnreal_static_5_ToBlob , SimNode_ExtFuncCall >(lib,"ToBlob","_dasUnreal_static_5_ToBlob")
		->args({"self","Source","DestBuffer","DestSize"})
		->arg_type(0,makeType<TExplicit<const FString*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Containers/UnrealString.h:1973:39
	makeExtern< FString (*)(const FString *,const unsigned char *,const unsigned int) , _dasUnreal_static_6_FromHexBlob , SimNode_ExtFuncCallAndCopyOrMove >(lib,"FromHexBlob","_dasUnreal_static_6_FromHexBlob")
		->args({"self","SrcBuffer","SrcSize"})
		->arg_type(0,makeType<TExplicit<const FString*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Containers/UnrealString.h:1983:23
	makeExtern< bool (*)(const FString *,const FString &,unsigned char *,const unsigned int) , _dasUnreal_static_7_ToHexBlob , SimNode_ExtFuncCall >(lib,"ToHexBlob","_dasUnreal_static_7_ToHexBlob")
		->args({"self","Source","DestBuffer","DestSize"})
		->arg_type(0,makeType<TExplicit<const FString*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Containers/UnrealString.h:1994:39
	makeExtern< FString (*)(const FString *,double,const int) , _dasUnreal_static_8_SanitizeFloat , SimNode_ExtFuncCallAndCopyOrMove >(lib,"SanitizeFloat","_dasUnreal_static_8_SanitizeFloat")
		->args({"self","InFloat","InMinFractionalDigits"})
		->arg_type(0,makeType<TExplicit<const FString*> >(lib))
		->arg_init(2,make_smart<ExprConstInt>(1))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Containers/UnrealString.h:2094:29
	makeExtern< FString (*)(const unsigned char *,int) , BytesToString , SimNode_ExtFuncCallAndCopyOrMove >(lib,"BytesToString","BytesToString")
		->args({"In","Count"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Containers/UnrealString.h:2120:14
	makeExtern< int (*)(const FString &,unsigned char *,int) , StringToBytes , SimNode_ExtFuncCall >(lib,"StringToBytes","StringToBytes")
		->args({"String","OutBytes","MaxBufferSize"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Containers/UnrealString.h:2135:27
	makeExtern< wchar_t (*)(unsigned char) , NibbleToTChar , SimNode_ExtFuncCall >(lib,"NibbleToTChar","NibbleToTChar")
		->args({"Num"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Containers/UnrealString.h:2149:13
	makeExtern< void (*)(unsigned char,FString &) , ByteToHex , SimNode_ExtFuncCall >(lib,"ByteToHex","ByteToHex")
		->args({"In","Result"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Containers/UnrealString.h:2156:15
	makeExtern< void (*)(const unsigned char *,int,FString &) , BytesToHex , SimNode_ExtFuncCall >(lib,"BytesToHex","BytesToHex")
		->args({"Bytes","NumBytes","Out"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Containers/UnrealString.h:2158:15
	makeExtern< void (*)(const unsigned char *,int,FString &) , BytesToHexLower , SimNode_ExtFuncCall >(lib,"BytesToHexLower","BytesToHexLower")
		->args({"Bytes","NumBytes","Out"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Containers/UnrealString.h:2161:29
	makeExtern< FString (*)(const unsigned char *,int) , BytesToHex , SimNode_ExtFuncCallAndCopyOrMove >(lib,"BytesToHex","BytesToHex")
		->args({"Bytes","NumBytes"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Containers/UnrealString.h:2169:29
	makeExtern< FString (*)(const unsigned char *,int) , BytesToHexLower , SimNode_ExtFuncCallAndCopyOrMove >(lib,"BytesToHexLower","BytesToHexLower")
		->args({"Bytes","NumBytes"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Containers/UnrealString.h:2181:32
	makeExtern< const bool (*)(const wchar_t) , CheckTCharIsHex , SimNode_ExtFuncCall >(lib,"CheckTCharIsHex","CheckTCharIsHex")
		->args({"Char"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Containers/UnrealString.h:2191:33
	makeExtern< const unsigned char (*)(const wchar_t) , TCharToNibble , SimNode_ExtFuncCall >(lib,"TCharToNibble","TCharToNibble")
		->args({"Hex"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Containers/UnrealString.h:2215:16
	makeExtern< int (*)(const FString &,unsigned char *) , HexToBytes , SimNode_ExtFuncCall >(lib,"HexToBytes","HexToBytes")
		->args({"HexString","OutBytes"})
		->addToModule(*this, SideEffects::worstDefault);
}
}

