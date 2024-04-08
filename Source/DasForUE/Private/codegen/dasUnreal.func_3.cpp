// this file is generated via daScript automatic C++ binder
// all user modifications will be lost after this file is re-generated

#include "dasUnreal.h"
#include "need_dasUnreal.h"
namespace das {
#include "dasUnreal.func.aot.decl.inc"
void Module_dasUnreal::initFunctions_3() {
	using _method_183 = das::das_call_member< void (FString::*)(int,bool),&FString::RightInline >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Containers/UnrealString.h:1035:19
	makeExtern<DAS_CALL_METHOD(_method_183), SimNode_ExtFuncCall >(lib,"RightInline","das_call_member< void (FString::*)(int,bool) , &FString::RightInline >::invoke")
		->args({"self","Count","bAllowShrinking"})
		->arg_init(2,make_smart<ExprConstBool>(true))
		->addToModule(*this, SideEffects::worstDefault);
	using _method_184 = das::das_call_member< void (FString::*)(int,bool),&FString::RightChopInline >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Containers/UnrealString.h:1051:19
	makeExtern<DAS_CALL_METHOD(_method_184), SimNode_ExtFuncCall >(lib,"RightChopInline","das_call_member< void (FString::*)(int,bool) , &FString::RightChopInline >::invoke")
		->args({"self","Count","bAllowShrinking"})
		->arg_init(2,make_smart<ExprConstBool>(true))
		->addToModule(*this, SideEffects::worstDefault);
	using _method_185 = das::das_call_member< void (FString::*)(int,int,bool),&FString::MidInline >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Containers/UnrealString.h:1065:19
	makeExtern<DAS_CALL_METHOD(_method_185), SimNode_ExtFuncCall >(lib,"MidInline","das_call_member< void (FString::*)(int,int,bool) , &FString::MidInline >::invoke")
		->args({"self","Start","Count","bAllowShrinking"})
		->arg_init(2,make_smart<ExprConstInt>(2147483647))
		->arg_init(3,make_smart<ExprConstBool>(true))
		->addToModule(*this, SideEffects::worstDefault);
	using _method_186 = das::das_call_member< int (FString::*)(const wchar_t *,ESearchCase::Type,ESearchDir::Type,int) const,&FString::Find >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Containers/UnrealString.h:1107:21
	makeExtern<DAS_CALL_METHOD(_method_186), SimNode_ExtFuncCall >(lib,"Find","das_call_member< int (FString::*)(const wchar_t *,ESearchCase::Type,ESearchDir::Type,int) const , &FString::Find >::invoke")
		->args({"self","SubStr","SearchCase","SearchDir","StartPosition"})
		->arg_init(2,make_smart<ExprConstEnumeration>(1,makeType<ESearchCase::Type>(lib)))
		->arg_init(3,make_smart<ExprConstEnumeration>(0,makeType<ESearchDir::Type>(lib)))
		->arg_init(4,make_smart<ExprConstInt>(-1))
		->addToModule(*this, SideEffects::worstDefault);
	using _method_187 = das::das_call_member< int (FString::*)(const FString &,ESearchCase::Type,ESearchDir::Type,int) const,&FString::Find >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Containers/UnrealString.h:1129:21
	makeExtern<DAS_CALL_METHOD(_method_187), SimNode_ExtFuncCall >(lib,"Find","das_call_member< int (FString::*)(const FString &,ESearchCase::Type,ESearchDir::Type,int) const , &FString::Find >::invoke")
		->args({"self","SubStr","SearchCase","SearchDir","StartPosition"})
		->arg_init(2,make_smart<ExprConstEnumeration>(1,makeType<ESearchCase::Type>(lib)))
		->arg_init(3,make_smart<ExprConstEnumeration>(0,makeType<ESearchDir::Type>(lib)))
		->arg_init(4,make_smart<ExprConstInt>(-1))
		->addToModule(*this, SideEffects::worstDefault);
	using _method_188 = das::das_call_member< int (FString::*)(const wchar_t *,int,ESearchCase::Type,ESearchDir::Type,int) const,&FString::Find >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Containers/UnrealString.h:1152:30
	makeExtern<DAS_CALL_METHOD(_method_188), SimNode_ExtFuncCall >(lib,"Find","das_call_member< int (FString::*)(const wchar_t *,int,ESearchCase::Type,ESearchDir::Type,int) const , &FString::Find >::invoke")
		->args({"self","SubStr","InSubStrLen","SearchCase","SearchDir","StartPosition"})
		->arg_init(3,make_smart<ExprConstEnumeration>(1,makeType<ESearchCase::Type>(lib)))
		->arg_init(4,make_smart<ExprConstEnumeration>(0,makeType<ESearchDir::Type>(lib)))
		->arg_init(5,make_smart<ExprConstInt>(-1))
		->addToModule(*this, SideEffects::worstDefault);
	using _method_189 = das::das_call_member< bool (FString::*)(const wchar_t *,ESearchCase::Type,ESearchDir::Type) const,&FString::Contains >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Containers/UnrealString.h:1180:32
	makeExtern<DAS_CALL_METHOD(_method_189), SimNode_ExtFuncCall >(lib,"Contains","das_call_member< bool (FString::*)(const wchar_t *,ESearchCase::Type,ESearchDir::Type) const , &FString::Contains >::invoke")
		->args({"self","SubStr","SearchCase","SearchDir"})
		->arg_init(2,make_smart<ExprConstEnumeration>(1,makeType<ESearchCase::Type>(lib)))
		->arg_init(3,make_smart<ExprConstEnumeration>(0,makeType<ESearchDir::Type>(lib)))
		->addToModule(*this, SideEffects::worstDefault);
	using _method_190 = das::das_call_member< bool (FString::*)(const FString &,ESearchCase::Type,ESearchDir::Type) const,&FString::Contains >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Containers/UnrealString.h:1194:32
	makeExtern<DAS_CALL_METHOD(_method_190), SimNode_ExtFuncCall >(lib,"Contains","das_call_member< bool (FString::*)(const FString &,ESearchCase::Type,ESearchDir::Type) const , &FString::Contains >::invoke")
		->args({"self","SubStr","SearchCase","SearchDir"})
		->arg_init(2,make_smart<ExprConstEnumeration>(1,makeType<ESearchCase::Type>(lib)))
		->arg_init(3,make_smart<ExprConstEnumeration>(0,makeType<ESearchDir::Type>(lib)))
		->addToModule(*this, SideEffects::worstDefault);
	using _method_191 = das::das_call_member< bool (FString::*)(const wchar_t *,int,ESearchCase::Type,ESearchDir::Type) const,&FString::Contains >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Containers/UnrealString.h:1209:32
	makeExtern<DAS_CALL_METHOD(_method_191), SimNode_ExtFuncCall >(lib,"Contains","das_call_member< bool (FString::*)(const wchar_t *,int,ESearchCase::Type,ESearchDir::Type) const , &FString::Contains >::invoke")
		->args({"self","SubStr","SubStrLen","SearchCase","SearchDir"})
		->arg_init(3,make_smart<ExprConstEnumeration>(1,makeType<ESearchCase::Type>(lib)))
		->arg_init(4,make_smart<ExprConstEnumeration>(0,makeType<ESearchDir::Type>(lib)))
		->addToModule(*this, SideEffects::worstDefault);
	using _method_192 = das::das_call_member< bool (FString::*)(wchar_t,int &) const,&FString::FindChar >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Containers/UnrealString.h:1222:19
	makeExtern<DAS_CALL_METHOD(_method_192), SimNode_ExtFuncCall >(lib,"FindChar","das_call_member< bool (FString::*)(wchar_t,int &) const , &FString::FindChar >::invoke")
		->args({"self","InChar","Index"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_193 = das::das_call_member< bool (FString::*)(wchar_t,int &) const,&FString::FindLastChar >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Containers/UnrealString.h:1234:19
	makeExtern<DAS_CALL_METHOD(_method_193), SimNode_ExtFuncCall >(lib,"FindLastChar","das_call_member< bool (FString::*)(wchar_t,int &) const , &FString::FindLastChar >::invoke")
		->args({"self","InChar","Index"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_194 = das::das_call_member< bool (FString::*)(const FString &,ESearchCase::Type) const,&FString::Equals >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Containers/UnrealString.h:1275:32
	makeExtern<DAS_CALL_METHOD(_method_194), SimNode_ExtFuncCall >(lib,"Equals","das_call_member< bool (FString::*)(const FString &,ESearchCase::Type) const , &FString::Equals >::invoke")
		->args({"self","Other","SearchCase"})
		->arg_init(2,make_smart<ExprConstEnumeration>(0,makeType<ESearchCase::Type>(lib)))
		->addToModule(*this, SideEffects::worstDefault);
	using _method_195 = das::das_call_member< int (FString::*)(const FString &,ESearchCase::Type) const,&FString::Compare >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Containers/UnrealString.h:1307:33
	makeExtern<DAS_CALL_METHOD(_method_195), SimNode_ExtFuncCall >(lib,"Compare","das_call_member< int (FString::*)(const FString &,ESearchCase::Type) const , &FString::Compare >::invoke")
		->args({"self","Other","SearchCase"})
		->arg_init(2,make_smart<ExprConstEnumeration>(0,makeType<ESearchCase::Type>(lib)))
		->addToModule(*this, SideEffects::worstDefault);
	using _method_196 = das::das_call_member< bool (FString::*)(const FString &,FString *,FString *,ESearchCase::Type,ESearchDir::Type) const,&FString::Split >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Containers/UnrealString.h:1329:16
	makeExtern<DAS_CALL_METHOD(_method_196), SimNode_ExtFuncCall >(lib,"Split","das_call_member< bool (FString::*)(const FString &,FString *,FString *,ESearchCase::Type,ESearchDir::Type) const , &FString::Split >::invoke")
		->args({"self","InS","LeftS","RightS","SearchCase","SearchDir"})
		->arg_init(5,make_smart<ExprConstEnumeration>(0,makeType<ESearchDir::Type>(lib)))
		->addToModule(*this, SideEffects::worstDefault);
	using _method_197 = das::das_call_member< bool (FString::*)(const FString &,FString *,FString *) const,&FString::Split >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Containers/UnrealString.h:1333:16
	makeExtern<DAS_CALL_METHOD(_method_197), SimNode_ExtFuncCall >(lib,"Split","das_call_member< bool (FString::*)(const FString &,FString *,FString *) const , &FString::Split >::invoke")
		->args({"self","InS","LeftS","RightS"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_198 = das::das_call_member< void (FString::*)(),&FString::ToUpperInline >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Containers/UnrealString.h:1345:16
	makeExtern<DAS_CALL_METHOD(_method_198), SimNode_ExtFuncCall >(lib,"ToUpperInline","das_call_member< void (FString::*)() , &FString::ToUpperInline >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_199 = das::das_call_member< void (FString::*)(),&FString::ToLowerInline >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Containers/UnrealString.h:1357:16
	makeExtern<DAS_CALL_METHOD(_method_199), SimNode_ExtFuncCall >(lib,"ToLowerInline","das_call_member< void (FString::*)() , &FString::ToLowerInline >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_200 = das::das_call_member< FString (FString::*)(int) const,&FString::LeftPad >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Containers/UnrealString.h:1360:32
	makeExtern<DAS_CALL_METHOD(_method_200), SimNode_ExtFuncCallAndCopyOrMove >(lib,"LeftPad","das_call_member< FString (FString::*)(int) const , &FString::LeftPad >::invoke")
		->args({"self","ChCount"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_201 = das::das_call_member< FString (FString::*)(int) const,&FString::RightPad >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Containers/UnrealString.h:1363:32
	makeExtern<DAS_CALL_METHOD(_method_201), SimNode_ExtFuncCallAndCopyOrMove >(lib,"RightPad","das_call_member< FString (FString::*)(int) const , &FString::RightPad >::invoke")
		->args({"self","ChCount"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_202 = das::das_call_member< bool (FString::*)() const,&FString::IsNumeric >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Containers/UnrealString.h:1366:29
	makeExtern<DAS_CALL_METHOD(_method_202), SimNode_ExtFuncCall >(lib,"IsNumeric","das_call_member< bool (FString::*)() const , &FString::IsNumeric >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_203 = das::das_call_member< void (FString::*)(),&FString::RemoveSpacesInline >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Containers/UnrealString.h:1369:16
	makeExtern<DAS_CALL_METHOD(_method_203), SimNode_ExtFuncCall >(lib,"RemoveSpacesInline","das_call_member< void (FString::*)() , &FString::RemoveSpacesInline >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Containers/UnrealString.h:1424:39
	makeExtern< FString (*)(const FString *,wchar_t) , _dasUnreal_static_70_Chr , SimNode_ExtFuncCallAndCopyOrMove >(lib,"Chr","_dasUnreal_static_70_Chr")
		->args({"self","Ch"})
		->arg_type(0,makeType<TExplicit<const FString*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Containers/UnrealString.h:1434:39
	makeExtern< FString (*)(const FString *,int,wchar_t) , _dasUnreal_static_71_ChrN , SimNode_ExtFuncCallAndCopyOrMove >(lib,"ChrN","_dasUnreal_static_71_ChrN")
		->args({"self","NumCharacters","Char"})
		->arg_type(0,makeType<TExplicit<const FString*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
	using _method_206 = das::das_call_member< bool (FString::*)(const wchar_t *,ESearchCase::Type) const,&FString::StartsWith >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Containers/UnrealString.h:1466:20
	makeExtern<DAS_CALL_METHOD(_method_206), SimNode_ExtFuncCall >(lib,"StartsWith","das_call_member< bool (FString::*)(const wchar_t *,ESearchCase::Type) const , &FString::StartsWith >::invoke")
		->args({"self","InPrefix","SearchCase"})
		->arg_init(2,make_smart<ExprConstEnumeration>(1,makeType<ESearchCase::Type>(lib)))
		->addToModule(*this, SideEffects::worstDefault);
	using _method_207 = das::das_call_member< bool (FString::*)(const FString &,ESearchCase::Type) const,&FString::StartsWith >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Containers/UnrealString.h:1477:20
	makeExtern<DAS_CALL_METHOD(_method_207), SimNode_ExtFuncCall >(lib,"StartsWith","das_call_member< bool (FString::*)(const FString &,ESearchCase::Type) const , &FString::StartsWith >::invoke")
		->args({"self","InPrefix","SearchCase"})
		->arg_init(2,make_smart<ExprConstEnumeration>(1,makeType<ESearchCase::Type>(lib)))
		->addToModule(*this, SideEffects::worstDefault);
	using _method_208 = das::das_call_member< bool (FString::*)(const wchar_t *,int,ESearchCase::Type) const,&FString::StartsWith >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Containers/UnrealString.h:1488:29
	makeExtern<DAS_CALL_METHOD(_method_208), SimNode_ExtFuncCall >(lib,"StartsWith","das_call_member< bool (FString::*)(const wchar_t *,int,ESearchCase::Type) const , &FString::StartsWith >::invoke")
		->args({"self","InPrefix","InPrefixLen","SearchCase"})
		->arg_init(3,make_smart<ExprConstEnumeration>(1,makeType<ESearchCase::Type>(lib)))
		->addToModule(*this, SideEffects::worstDefault);
	using _method_209 = das::das_call_member< bool (FString::*)(const wchar_t *,ESearchCase::Type) const,&FString::EndsWith >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Containers/UnrealString.h:1509:20
	makeExtern<DAS_CALL_METHOD(_method_209), SimNode_ExtFuncCall >(lib,"EndsWith","das_call_member< bool (FString::*)(const wchar_t *,ESearchCase::Type) const , &FString::EndsWith >::invoke")
		->args({"self","InSuffix","SearchCase"})
		->arg_init(2,make_smart<ExprConstEnumeration>(1,makeType<ESearchCase::Type>(lib)))
		->addToModule(*this, SideEffects::worstDefault);
	using _method_210 = das::das_call_member< bool (FString::*)(const FString &,ESearchCase::Type) const,&FString::EndsWith >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Containers/UnrealString.h:1520:20
	makeExtern<DAS_CALL_METHOD(_method_210), SimNode_ExtFuncCall >(lib,"EndsWith","das_call_member< bool (FString::*)(const FString &,ESearchCase::Type) const , &FString::EndsWith >::invoke")
		->args({"self","InSuffix","SearchCase"})
		->arg_init(2,make_smart<ExprConstEnumeration>(1,makeType<ESearchCase::Type>(lib)))
		->addToModule(*this, SideEffects::worstDefault);
	using _method_211 = das::das_call_member< bool (FString::*)(const wchar_t *,int,ESearchCase::Type) const,&FString::EndsWith >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Containers/UnrealString.h:1531:29
	makeExtern<DAS_CALL_METHOD(_method_211), SimNode_ExtFuncCall >(lib,"EndsWith","das_call_member< bool (FString::*)(const wchar_t *,int,ESearchCase::Type) const , &FString::EndsWith >::invoke")
		->args({"self","InSuffix","InSuffixLen","SearchCase"})
		->arg_init(3,make_smart<ExprConstEnumeration>(1,makeType<ESearchCase::Type>(lib)))
		->addToModule(*this, SideEffects::worstDefault);
	using _method_212 = das::das_call_member< bool (FString::*)(const wchar_t *,ESearchCase::Type) const,&FString::MatchesWildcard >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Containers/UnrealString.h:1556:20
	makeExtern<DAS_CALL_METHOD(_method_212), SimNode_ExtFuncCall >(lib,"MatchesWildcard","das_call_member< bool (FString::*)(const wchar_t *,ESearchCase::Type) const , &FString::MatchesWildcard >::invoke")
		->args({"self","Wildcard","SearchCase"})
		->arg_init(2,make_smart<ExprConstEnumeration>(1,makeType<ESearchCase::Type>(lib)))
		->addToModule(*this, SideEffects::worstDefault);
	using _method_213 = das::das_call_member< bool (FString::*)(const FString &,ESearchCase::Type) const,&FString::MatchesWildcard >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Containers/UnrealString.h:1569:20
	makeExtern<DAS_CALL_METHOD(_method_213), SimNode_ExtFuncCall >(lib,"MatchesWildcard","das_call_member< bool (FString::*)(const FString &,ESearchCase::Type) const , &FString::MatchesWildcard >::invoke")
		->args({"self","Wildcard","SearchCase"})
		->arg_init(2,make_smart<ExprConstEnumeration>(1,makeType<ESearchCase::Type>(lib)))
		->addToModule(*this, SideEffects::worstDefault);
	using _method_214 = das::das_call_member< bool (FString::*)(const wchar_t *,int,ESearchCase::Type) const,&FString::MatchesWildcard >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Containers/UnrealString.h:1582:29
	makeExtern<DAS_CALL_METHOD(_method_214), SimNode_ExtFuncCall >(lib,"MatchesWildcard","das_call_member< bool (FString::*)(const wchar_t *,int,ESearchCase::Type) const , &FString::MatchesWildcard >::invoke")
		->args({"self","Wildcard","WildcardLen","SearchCase"})
		->arg_init(3,make_smart<ExprConstEnumeration>(1,makeType<ESearchCase::Type>(lib)))
		->addToModule(*this, SideEffects::worstDefault);
	using _method_215 = das::das_call_member< void (FString::*)(),&FString::TrimStartAndEndInline >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Containers/UnrealString.h:1587:16
	makeExtern<DAS_CALL_METHOD(_method_215), SimNode_ExtFuncCall >(lib,"TrimStartAndEndInline","das_call_member< void (FString::*)() , &FString::TrimStartAndEndInline >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_216 = das::das_call_member< void (FString::*)(),&FString::TrimStartInline >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Containers/UnrealString.h:1604:16
	makeExtern<DAS_CALL_METHOD(_method_216), SimNode_ExtFuncCall >(lib,"TrimStartInline","das_call_member< void (FString::*)() , &FString::TrimStartInline >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_217 = das::das_call_member< void (FString::*)(),&FString::TrimEndInline >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Containers/UnrealString.h:1621:16
	makeExtern<DAS_CALL_METHOD(_method_217), SimNode_ExtFuncCall >(lib,"TrimEndInline","das_call_member< void (FString::*)() , &FString::TrimEndInline >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_218 = das::das_call_member< void (FString::*)(),&FString::TrimToNullTerminator >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Containers/UnrealString.h:1638:16
	makeExtern<DAS_CALL_METHOD(_method_218), SimNode_ExtFuncCall >(lib,"TrimToNullTerminator","das_call_member< void (FString::*)() , &FString::TrimToNullTerminator >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_219 = das::das_call_member< void (FString::*)(bool *),&FString::TrimQuotesInline >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Containers/UnrealString.h:1644:16
	makeExtern<DAS_CALL_METHOD(_method_219), SimNode_ExtFuncCall >(lib,"TrimQuotesInline","das_call_member< void (FString::*)(bool *) , &FString::TrimQuotesInline >::invoke")
		->args({"self","bQuotesRemoved"})
		->arg_init(1,make_smart<ExprConstPtr>())
		->addToModule(*this, SideEffects::worstDefault);
	using _method_220 = das::das_call_member< void (FString::*)(const wchar_t,bool *),&FString::TrimCharInline >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Containers/UnrealString.h:1650:16
	makeExtern<DAS_CALL_METHOD(_method_220), SimNode_ExtFuncCall >(lib,"TrimCharInline","das_call_member< void (FString::*)(const wchar_t,bool *) , &FString::TrimCharInline >::invoke")
		->args({"self","CharacterToTrim","bCharRemoved"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_221 = das::das_call_member< void (FString::*)(),&FString::ReverseString >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Containers/UnrealString.h:1740:16
	makeExtern<DAS_CALL_METHOD(_method_221), SimNode_ExtFuncCall >(lib,"ReverseString","das_call_member< void (FString::*)() , &FString::ReverseString >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_222 = das::das_call_member< int (FString::*)(const wchar_t *,const wchar_t *,ESearchCase::Type),&FString::ReplaceInline >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Containers/UnrealString.h:1771:17
	makeExtern<DAS_CALL_METHOD(_method_222), SimNode_ExtFuncCall >(lib,"ReplaceInline","das_call_member< int (FString::*)(const wchar_t *,const wchar_t *,ESearchCase::Type) , &FString::ReplaceInline >::invoke")
		->args({"self","SearchText","ReplacementText","SearchCase"})
		->arg_init(3,make_smart<ExprConstEnumeration>(1,makeType<ESearchCase::Type>(lib)))
		->addToModule(*this, SideEffects::worstDefault);
	using _method_223 = das::das_call_member< void (FString::*)(const wchar_t,const wchar_t,ESearchCase::Type),&FString::ReplaceCharInline >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Containers/UnrealString.h:1781:7
	makeExtern<DAS_CALL_METHOD(_method_223), SimNode_ExtFuncCall >(lib,"ReplaceCharInline","das_call_member< void (FString::*)(const wchar_t,const wchar_t,ESearchCase::Type) , &FString::ReplaceCharInline >::invoke")
		->args({"self","SearchChar","ReplacementChar","SearchCase"})
		->arg_init(3,make_smart<ExprConstEnumeration>(1,makeType<ESearchCase::Type>(lib)))
		->addToModule(*this, SideEffects::worstDefault);
	using _method_224 = das::das_call_member< void (FString::*)(const int),&FString::ConvertTabsToSpacesInline >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Containers/UnrealString.h:1884:16
	makeExtern<DAS_CALL_METHOD(_method_224), SimNode_ExtFuncCall >(lib,"ConvertTabsToSpacesInline","das_call_member< void (FString::*)(const int) , &FString::ConvertTabsToSpacesInline >::invoke")
		->args({"self","InSpacesPerTab"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Containers/UnrealString.h:1910:39
	makeExtern< FString (*)(const FString *,int) , _dasUnreal_static_72_FormatAsNumber , SimNode_ExtFuncCallAndCopyOrMove >(lib,"FormatAsNumber","_dasUnreal_static_72_FormatAsNumber")
		->args({"self","InNumber"})
		->arg_type(0,makeType<TExplicit<const FString*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
	using _method_226 = das::das_call_member< void (FString::*)(int),&FString::Reserve >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Containers/UnrealString.h:1913:16
	makeExtern<DAS_CALL_METHOD(_method_226), SimNode_ExtFuncCall >(lib,"Reserve","das_call_member< void (FString::*)(int) , &FString::Reserve >::invoke")
		->args({"self","CharacterCount"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_227 = das::das_call_member< void (FString::*)(FArchive &,int) const,&FString::SerializeAsANSICharArray >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Containers/UnrealString.h:1922:16
	makeExtern<DAS_CALL_METHOD(_method_227), SimNode_ExtFuncCall >(lib,"SerializeAsANSICharArray","das_call_member< void (FString::*)(FArchive &,int) const , &FString::SerializeAsANSICharArray >::invoke")
		->args({"self","Ar","MinCharacters"})
		->arg_init(2,make_smart<ExprConstInt>(0))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Containers/UnrealString.h:1926:42
	makeExtern< FString (*)(const FString *,int) , _dasUnreal_static_73_FromInt , SimNode_ExtFuncCallAndCopyOrMove >(lib,"FromInt","_dasUnreal_static_73_FromInt")
		->args({"self","Num"})
		->arg_type(0,makeType<TExplicit<const FString*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
	using _method_229 = das::das_call_member< void (FString::*)(int),&FString::AppendInt >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Containers/UnrealString.h:1934:16
	makeExtern<DAS_CALL_METHOD(_method_229), SimNode_ExtFuncCall >(lib,"AppendInt","das_call_member< void (FString::*)(int) , &FString::AppendInt >::invoke")
		->args({"self","InNum"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_230 = das::das_call_member< bool (FString::*)() const,&FString::ToBool >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Containers/UnrealString.h:1943:29
	makeExtern<DAS_CALL_METHOD(_method_230), SimNode_ExtFuncCall >(lib,"ToBool","das_call_member< bool (FString::*)() const , &FString::ToBool >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Containers/UnrealString.h:1953:39
	makeExtern< FString (*)(const FString *,const unsigned char *,const unsigned int) , _dasUnreal_static_74_FromBlob , SimNode_ExtFuncCallAndCopyOrMove >(lib,"FromBlob","_dasUnreal_static_74_FromBlob")
		->args({"self","SrcBuffer","SrcSize"})
		->arg_type(0,makeType<TExplicit<const FString*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Containers/UnrealString.h:1963:23
	makeExtern< bool (*)(const FString *,const FString &,unsigned char *,const unsigned int) , _dasUnreal_static_75_ToBlob , SimNode_ExtFuncCall >(lib,"ToBlob","_dasUnreal_static_75_ToBlob")
		->args({"self","Source","DestBuffer","DestSize"})
		->arg_type(0,makeType<TExplicit<const FString*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Containers/UnrealString.h:1973:39
	makeExtern< FString (*)(const FString *,const unsigned char *,const unsigned int) , _dasUnreal_static_76_FromHexBlob , SimNode_ExtFuncCallAndCopyOrMove >(lib,"FromHexBlob","_dasUnreal_static_76_FromHexBlob")
		->args({"self","SrcBuffer","SrcSize"})
		->arg_type(0,makeType<TExplicit<const FString*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Containers/UnrealString.h:1983:23
	makeExtern< bool (*)(const FString *,const FString &,unsigned char *,const unsigned int) , _dasUnreal_static_77_ToHexBlob , SimNode_ExtFuncCall >(lib,"ToHexBlob","_dasUnreal_static_77_ToHexBlob")
		->args({"self","Source","DestBuffer","DestSize"})
		->arg_type(0,makeType<TExplicit<const FString*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Containers/UnrealString.h:1994:39
	makeExtern< FString (*)(const FString *,double,const int) , _dasUnreal_static_78_SanitizeFloat , SimNode_ExtFuncCallAndCopyOrMove >(lib,"SanitizeFloat","_dasUnreal_static_78_SanitizeFloat")
		->args({"self","InFloat","InMinFractionalDigits"})
		->arg_type(0,makeType<TExplicit<const FString*> >(lib))
		->arg_init(2,make_smart<ExprConstInt>(1))
		->addToModule(*this, SideEffects::worstDefault);
	using _method_236 = das::das_call_member< void (FString::*)(FArchive &) const,&FString::CountBytes >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Containers/UnrealString.h:2057:19
	makeExtern<DAS_CALL_METHOD(_method_236), SimNode_ExtFuncCall >(lib,"CountBytes","das_call_member< void (FString::*)(FArchive &) const , &FString::CountBytes >::invoke")
		->args({"self","Ar"})
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
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Containers/UnrealString.h:2236:13
	makeExtern< void (*)(signed char &,const wchar_t *) , LexFromString , SimNode_ExtFuncCall >(lib,"LexFromString","LexFromString")
		->args({"OutValue","Buffer"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Containers/UnrealString.h:2237:13
	makeExtern< void (*)(short &,const wchar_t *) , LexFromString , SimNode_ExtFuncCall >(lib,"LexFromString","LexFromString")
		->args({"OutValue","Buffer"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Containers/UnrealString.h:2238:13
	makeExtern< void (*)(int &,const wchar_t *) , LexFromString , SimNode_ExtFuncCall >(lib,"LexFromString","LexFromString")
		->args({"OutValue","Buffer"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Containers/UnrealString.h:2239:13
	makeExtern< void (*)(long long &,const wchar_t *) , LexFromString , SimNode_ExtFuncCall >(lib,"LexFromString","LexFromString")
		->args({"OutValue","Buffer"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Containers/UnrealString.h:2240:13
	makeExtern< void (*)(unsigned char &,const wchar_t *) , LexFromString , SimNode_ExtFuncCall >(lib,"LexFromString","LexFromString")
		->args({"OutValue","Buffer"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Containers/UnrealString.h:2241:13
	makeExtern< void (*)(unsigned short &,const wchar_t *) , LexFromString , SimNode_ExtFuncCall >(lib,"LexFromString","LexFromString")
		->args({"OutValue","Buffer"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Containers/UnrealString.h:2242:13
	makeExtern< void (*)(unsigned int &,const wchar_t *) , LexFromString , SimNode_ExtFuncCall >(lib,"LexFromString","LexFromString")
		->args({"OutValue","Buffer"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Containers/UnrealString.h:2243:13
	makeExtern< void (*)(unsigned long long &,const wchar_t *) , LexFromString , SimNode_ExtFuncCall >(lib,"LexFromString","LexFromString")
		->args({"OutValue","Buffer"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Containers/UnrealString.h:2244:13
	makeExtern< void (*)(float &,const wchar_t *) , LexFromString , SimNode_ExtFuncCall >(lib,"LexFromString","LexFromString")
		->args({"OutValue","Buffer"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Containers/UnrealString.h:2245:13
	makeExtern< void (*)(double &,const wchar_t *) , LexFromString , SimNode_ExtFuncCall >(lib,"LexFromString","LexFromString")
		->args({"OutValue","Buffer"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Containers/UnrealString.h:2246:13
	makeExtern< void (*)(bool &,const wchar_t *) , LexFromString , SimNode_ExtFuncCall >(lib,"LexFromString","LexFromString")
		->args({"OutValue","Buffer"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Containers/UnrealString.h:2247:13
	makeExtern< void (*)(FString &,const wchar_t *) , LexFromString , SimNode_ExtFuncCall >(lib,"LexFromString","LexFromString")
		->args({"OutValue","Buffer"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Containers/UnrealString.h:2265:29
	makeExtern< FString (*)(bool) , LexToString , SimNode_ExtFuncCallAndCopyOrMove >(lib,"LexToString","LexToString")
		->args({"Value"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Containers/UnrealString.h:2275:34
	makeExtern< FString (*)(const FString &) , LexToString , SimNode_ExtFuncCallAndCopyOrMove >(lib,"LexToString","LexToString")
		->args({"Str"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Containers/UnrealString.h:2288:29
	makeExtern< FString (*)(float) , LexToSanitizedString , SimNode_ExtFuncCallAndCopyOrMove >(lib,"LexToSanitizedString","LexToSanitizedString")
		->args({"Value"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Containers/UnrealString.h:2294:29
	makeExtern< FString (*)(double) , LexToSanitizedString , SimNode_ExtFuncCallAndCopyOrMove >(lib,"LexToSanitizedString","LexToSanitizedString")
		->args({"Value"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Containers/UnrealString.h:2338:13
	makeExtern< bool (*)(bool &,const wchar_t *) , LexTryParseString , SimNode_ExtFuncCall >(lib,"LexTryParseString","LexTryParseString")
		->args({"OutValue","Buffer"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Containers/UnrealString.h:2369:26
	makeExtern< const wchar_t * (*)(const FString &) , ToCStr , SimNode_ExtFuncCall >(lib,"ToCStr","ToCStr")
		->args({"Str"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Containers/UnrealString.h:2377:16
	makeExtern< void (*)(FString &) , StringConv::InlineCombineSurrogates , SimNode_ExtFuncCall >(lib,"InlineCombineSurrogates","StringConv::InlineCombineSurrogates")
		->args({"Str"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Containers/UnrealString.h:2522:16
	makeExtern< int (*)(const FString &,const int) , FindMatchingClosingParenthesis , SimNode_ExtFuncCall >(lib,"FindMatchingClosingParenthesis","FindMatchingClosingParenthesis")
		->args({"TargetString","StartSearch"})
		->arg_init(1,make_smart<ExprConstInt>(0))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Containers/UnrealString.h:2532:18
	makeExtern< FString (*)(const FString &,const wchar_t *) , SlugStringForValidName , SimNode_ExtFuncCallAndCopyOrMove >(lib,"SlugStringForValidName","SlugStringForValidName")
		->args({"DisplayString","ReplaceWith"})
		->addToModule(*this, SideEffects::worstDefault);
	addCtorAndUsing<FTextRange>(*this,lib,"FTextRange","FTextRange");
	addCtorAndUsing<FTextRange,int,int>(*this,lib,"FTextRange","FTextRange")
		->args({"InBeginIndex","InEndIndex"});
	using _method_237 = das::das_call_member< int (FTextRange::*)() const,&FTextRange::Len >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Containers/UnrealString.h:2569:8
	makeExtern<DAS_CALL_METHOD(_method_237), SimNode_ExtFuncCall >(lib,"Len","das_call_member< int (FTextRange::*)() const , &FTextRange::Len >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_238 = das::das_call_member< bool (FTextRange::*)() const,&FTextRange::IsEmpty >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Containers/UnrealString.h:2570:7
	makeExtern<DAS_CALL_METHOD(_method_238), SimNode_ExtFuncCall >(lib,"IsEmpty","das_call_member< bool (FTextRange::*)() const , &FTextRange::IsEmpty >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_239 = das::das_call_member< void (FTextRange::*)(int),&FTextRange::Offset >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Containers/UnrealString.h:2571:7
	makeExtern<DAS_CALL_METHOD(_method_239), SimNode_ExtFuncCall >(lib,"Offset","das_call_member< void (FTextRange::*)(int) , &FTextRange::Offset >::invoke")
		->args({"self","Amount"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_240 = das::das_call_member< bool (FTextRange::*)(int) const,&FTextRange::Contains >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Containers/UnrealString.h:2572:7
	makeExtern<DAS_CALL_METHOD(_method_240), SimNode_ExtFuncCall >(lib,"Contains","das_call_member< bool (FTextRange::*)(int) const , &FTextRange::Contains >::invoke")
		->args({"self","Index"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_241 = das::das_call_member< bool (FTextRange::*)(int) const,&FTextRange::InclusiveContains >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Containers/UnrealString.h:2573:7
	makeExtern<DAS_CALL_METHOD(_method_241), SimNode_ExtFuncCall >(lib,"InclusiveContains","das_call_member< bool (FTextRange::*)(int) const , &FTextRange::InclusiveContains >::invoke")
		->args({"self","Index"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_242 = das::das_call_member< FTextRange (FTextRange::*)(const FTextRange &) const,&FTextRange::Intersect >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Containers/UnrealString.h:2575:13
	makeExtern<DAS_CALL_METHOD(_method_242), SimNode_ExtFuncCallAndCopyOrMove >(lib,"Intersect","das_call_member< FTextRange (FTextRange::*)(const FTextRange &) const , &FTextRange::Intersect >::invoke")
		->args({"self","Other"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/UObject/UnrealNames.h:28:23
	makeExtern< const wchar_t * (*)(EName) , LexToString , SimNode_ExtFuncCall >(lib,"LexToString","LexToString")
		->args({"Ename"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/UObject/UnrealNames.h:41:13
	makeExtern< bool (*)(EName) , ShouldReplicateAsInteger , SimNode_ExtFuncCall >(lib,"ShouldReplicateAsInteger","ShouldReplicateAsInteger")
		->args({"Ename"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/UObject/UnrealNames.h:46:15
	makeExtern< bool (*)(EName,const FName &) , ShouldReplicateAsInteger , SimNode_ExtFuncCall >(lib,"ShouldReplicateAsInteger","ShouldReplicateAsInteger")
		->args({"Ename","Name"})
		->addToModule(*this, SideEffects::worstDefault);
}
}

