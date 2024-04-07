// this file is generated via daScript automatic C++ binder
// all user modifications will be lost after this file is re-generated

#include "dasUnreal.h"
#include "need_dasUnreal.h"
namespace das {
#include "dasUnreal.func.aot.decl.inc"
void Module_dasUnreal::initFunctions_3() {
	using _method_183 = das::das_call_member< void (FString::*)(),&FString::RemoveSpacesInline >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Containers/UnrealString.h:1369:16
	makeExtern<DAS_CALL_METHOD(_method_183), SimNode_ExtFuncCall >(lib,"RemoveSpacesInline","das_call_member< void (FString::*)() , &FString::RemoveSpacesInline >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Containers/UnrealString.h:1424:39
	makeExtern< FString (*)(const FString *,wchar_t) , _dasUnreal_static_61_Chr , SimNode_ExtFuncCallAndCopyOrMove >(lib,"Chr","_dasUnreal_static_61_Chr")
		->args({"self","Ch"})
		->arg_type(0,makeType<TExplicit<const FString*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Containers/UnrealString.h:1434:39
	makeExtern< FString (*)(const FString *,int,wchar_t) , _dasUnreal_static_62_ChrN , SimNode_ExtFuncCallAndCopyOrMove >(lib,"ChrN","_dasUnreal_static_62_ChrN")
		->args({"self","NumCharacters","Char"})
		->arg_type(0,makeType<TExplicit<const FString*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
	using _method_186 = das::das_call_member< bool (FString::*)(const wchar_t *,ESearchCase::Type) const,&FString::StartsWith >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Containers/UnrealString.h:1466:20
	makeExtern<DAS_CALL_METHOD(_method_186), SimNode_ExtFuncCall >(lib,"StartsWith","das_call_member< bool (FString::*)(const wchar_t *,ESearchCase::Type) const , &FString::StartsWith >::invoke")
		->args({"self","InPrefix","SearchCase"})
		->arg_init(2,make_smart<ExprConstEnumeration>(1,makeType<ESearchCase::Type>(lib)))
		->addToModule(*this, SideEffects::worstDefault);
	using _method_187 = das::das_call_member< bool (FString::*)(const FString &,ESearchCase::Type) const,&FString::StartsWith >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Containers/UnrealString.h:1477:20
	makeExtern<DAS_CALL_METHOD(_method_187), SimNode_ExtFuncCall >(lib,"StartsWith","das_call_member< bool (FString::*)(const FString &,ESearchCase::Type) const , &FString::StartsWith >::invoke")
		->args({"self","InPrefix","SearchCase"})
		->arg_init(2,make_smart<ExprConstEnumeration>(1,makeType<ESearchCase::Type>(lib)))
		->addToModule(*this, SideEffects::worstDefault);
	using _method_188 = das::das_call_member< bool (FString::*)(const wchar_t *,int,ESearchCase::Type) const,&FString::StartsWith >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Containers/UnrealString.h:1488:29
	makeExtern<DAS_CALL_METHOD(_method_188), SimNode_ExtFuncCall >(lib,"StartsWith","das_call_member< bool (FString::*)(const wchar_t *,int,ESearchCase::Type) const , &FString::StartsWith >::invoke")
		->args({"self","InPrefix","InPrefixLen","SearchCase"})
		->arg_init(3,make_smart<ExprConstEnumeration>(1,makeType<ESearchCase::Type>(lib)))
		->addToModule(*this, SideEffects::worstDefault);
	using _method_189 = das::das_call_member< bool (FString::*)(const wchar_t *,ESearchCase::Type) const,&FString::EndsWith >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Containers/UnrealString.h:1509:20
	makeExtern<DAS_CALL_METHOD(_method_189), SimNode_ExtFuncCall >(lib,"EndsWith","das_call_member< bool (FString::*)(const wchar_t *,ESearchCase::Type) const , &FString::EndsWith >::invoke")
		->args({"self","InSuffix","SearchCase"})
		->arg_init(2,make_smart<ExprConstEnumeration>(1,makeType<ESearchCase::Type>(lib)))
		->addToModule(*this, SideEffects::worstDefault);
	using _method_190 = das::das_call_member< bool (FString::*)(const FString &,ESearchCase::Type) const,&FString::EndsWith >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Containers/UnrealString.h:1520:20
	makeExtern<DAS_CALL_METHOD(_method_190), SimNode_ExtFuncCall >(lib,"EndsWith","das_call_member< bool (FString::*)(const FString &,ESearchCase::Type) const , &FString::EndsWith >::invoke")
		->args({"self","InSuffix","SearchCase"})
		->arg_init(2,make_smart<ExprConstEnumeration>(1,makeType<ESearchCase::Type>(lib)))
		->addToModule(*this, SideEffects::worstDefault);
	using _method_191 = das::das_call_member< bool (FString::*)(const wchar_t *,int,ESearchCase::Type) const,&FString::EndsWith >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Containers/UnrealString.h:1531:29
	makeExtern<DAS_CALL_METHOD(_method_191), SimNode_ExtFuncCall >(lib,"EndsWith","das_call_member< bool (FString::*)(const wchar_t *,int,ESearchCase::Type) const , &FString::EndsWith >::invoke")
		->args({"self","InSuffix","InSuffixLen","SearchCase"})
		->arg_init(3,make_smart<ExprConstEnumeration>(1,makeType<ESearchCase::Type>(lib)))
		->addToModule(*this, SideEffects::worstDefault);
	using _method_192 = das::das_call_member< bool (FString::*)(const wchar_t *,ESearchCase::Type) const,&FString::MatchesWildcard >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Containers/UnrealString.h:1556:20
	makeExtern<DAS_CALL_METHOD(_method_192), SimNode_ExtFuncCall >(lib,"MatchesWildcard","das_call_member< bool (FString::*)(const wchar_t *,ESearchCase::Type) const , &FString::MatchesWildcard >::invoke")
		->args({"self","Wildcard","SearchCase"})
		->arg_init(2,make_smart<ExprConstEnumeration>(1,makeType<ESearchCase::Type>(lib)))
		->addToModule(*this, SideEffects::worstDefault);
	using _method_193 = das::das_call_member< bool (FString::*)(const FString &,ESearchCase::Type) const,&FString::MatchesWildcard >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Containers/UnrealString.h:1569:20
	makeExtern<DAS_CALL_METHOD(_method_193), SimNode_ExtFuncCall >(lib,"MatchesWildcard","das_call_member< bool (FString::*)(const FString &,ESearchCase::Type) const , &FString::MatchesWildcard >::invoke")
		->args({"self","Wildcard","SearchCase"})
		->arg_init(2,make_smart<ExprConstEnumeration>(1,makeType<ESearchCase::Type>(lib)))
		->addToModule(*this, SideEffects::worstDefault);
	using _method_194 = das::das_call_member< bool (FString::*)(const wchar_t *,int,ESearchCase::Type) const,&FString::MatchesWildcard >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Containers/UnrealString.h:1582:29
	makeExtern<DAS_CALL_METHOD(_method_194), SimNode_ExtFuncCall >(lib,"MatchesWildcard","das_call_member< bool (FString::*)(const wchar_t *,int,ESearchCase::Type) const , &FString::MatchesWildcard >::invoke")
		->args({"self","Wildcard","WildcardLen","SearchCase"})
		->arg_init(3,make_smart<ExprConstEnumeration>(1,makeType<ESearchCase::Type>(lib)))
		->addToModule(*this, SideEffects::worstDefault);
	using _method_195 = das::das_call_member< void (FString::*)(),&FString::TrimStartAndEndInline >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Containers/UnrealString.h:1587:16
	makeExtern<DAS_CALL_METHOD(_method_195), SimNode_ExtFuncCall >(lib,"TrimStartAndEndInline","das_call_member< void (FString::*)() , &FString::TrimStartAndEndInline >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_196 = das::das_call_member< void (FString::*)(),&FString::TrimStartInline >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Containers/UnrealString.h:1604:16
	makeExtern<DAS_CALL_METHOD(_method_196), SimNode_ExtFuncCall >(lib,"TrimStartInline","das_call_member< void (FString::*)() , &FString::TrimStartInline >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_197 = das::das_call_member< void (FString::*)(),&FString::TrimEndInline >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Containers/UnrealString.h:1621:16
	makeExtern<DAS_CALL_METHOD(_method_197), SimNode_ExtFuncCall >(lib,"TrimEndInline","das_call_member< void (FString::*)() , &FString::TrimEndInline >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_198 = das::das_call_member< void (FString::*)(),&FString::TrimToNullTerminator >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Containers/UnrealString.h:1638:16
	makeExtern<DAS_CALL_METHOD(_method_198), SimNode_ExtFuncCall >(lib,"TrimToNullTerminator","das_call_member< void (FString::*)() , &FString::TrimToNullTerminator >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_199 = das::das_call_member< void (FString::*)(bool *),&FString::TrimQuotesInline >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Containers/UnrealString.h:1644:16
	makeExtern<DAS_CALL_METHOD(_method_199), SimNode_ExtFuncCall >(lib,"TrimQuotesInline","das_call_member< void (FString::*)(bool *) , &FString::TrimQuotesInline >::invoke")
		->args({"self","bQuotesRemoved"})
		->arg_init(1,make_smart<ExprConstPtr>())
		->addToModule(*this, SideEffects::worstDefault);
	using _method_200 = das::das_call_member< void (FString::*)(const wchar_t,bool *),&FString::TrimCharInline >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Containers/UnrealString.h:1650:16
	makeExtern<DAS_CALL_METHOD(_method_200), SimNode_ExtFuncCall >(lib,"TrimCharInline","das_call_member< void (FString::*)(const wchar_t,bool *) , &FString::TrimCharInline >::invoke")
		->args({"self","CharacterToTrim","bCharRemoved"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_201 = das::das_call_member< void (FString::*)(),&FString::ReverseString >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Containers/UnrealString.h:1740:16
	makeExtern<DAS_CALL_METHOD(_method_201), SimNode_ExtFuncCall >(lib,"ReverseString","das_call_member< void (FString::*)() , &FString::ReverseString >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_202 = das::das_call_member< int (FString::*)(const wchar_t *,const wchar_t *,ESearchCase::Type),&FString::ReplaceInline >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Containers/UnrealString.h:1771:17
	makeExtern<DAS_CALL_METHOD(_method_202), SimNode_ExtFuncCall >(lib,"ReplaceInline","das_call_member< int (FString::*)(const wchar_t *,const wchar_t *,ESearchCase::Type) , &FString::ReplaceInline >::invoke")
		->args({"self","SearchText","ReplacementText","SearchCase"})
		->arg_init(3,make_smart<ExprConstEnumeration>(1,makeType<ESearchCase::Type>(lib)))
		->addToModule(*this, SideEffects::worstDefault);
	using _method_203 = das::das_call_member< void (FString::*)(const wchar_t,const wchar_t,ESearchCase::Type),&FString::ReplaceCharInline >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Containers/UnrealString.h:1781:7
	makeExtern<DAS_CALL_METHOD(_method_203), SimNode_ExtFuncCall >(lib,"ReplaceCharInline","das_call_member< void (FString::*)(const wchar_t,const wchar_t,ESearchCase::Type) , &FString::ReplaceCharInline >::invoke")
		->args({"self","SearchChar","ReplacementChar","SearchCase"})
		->arg_init(3,make_smart<ExprConstEnumeration>(1,makeType<ESearchCase::Type>(lib)))
		->addToModule(*this, SideEffects::worstDefault);
	using _method_204 = das::das_call_member< void (FString::*)(const int),&FString::ConvertTabsToSpacesInline >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Containers/UnrealString.h:1884:16
	makeExtern<DAS_CALL_METHOD(_method_204), SimNode_ExtFuncCall >(lib,"ConvertTabsToSpacesInline","das_call_member< void (FString::*)(const int) , &FString::ConvertTabsToSpacesInline >::invoke")
		->args({"self","InSpacesPerTab"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Containers/UnrealString.h:1910:39
	makeExtern< FString (*)(const FString *,int) , _dasUnreal_static_63_FormatAsNumber , SimNode_ExtFuncCallAndCopyOrMove >(lib,"FormatAsNumber","_dasUnreal_static_63_FormatAsNumber")
		->args({"self","InNumber"})
		->arg_type(0,makeType<TExplicit<const FString*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
	using _method_206 = das::das_call_member< void (FString::*)(int),&FString::Reserve >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Containers/UnrealString.h:1913:16
	makeExtern<DAS_CALL_METHOD(_method_206), SimNode_ExtFuncCall >(lib,"Reserve","das_call_member< void (FString::*)(int) , &FString::Reserve >::invoke")
		->args({"self","CharacterCount"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Containers/UnrealString.h:1926:42
	makeExtern< FString (*)(const FString *,int) , _dasUnreal_static_64_FromInt , SimNode_ExtFuncCallAndCopyOrMove >(lib,"FromInt","_dasUnreal_static_64_FromInt")
		->args({"self","Num"})
		->arg_type(0,makeType<TExplicit<const FString*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
	using _method_208 = das::das_call_member< void (FString::*)(int),&FString::AppendInt >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Containers/UnrealString.h:1934:16
	makeExtern<DAS_CALL_METHOD(_method_208), SimNode_ExtFuncCall >(lib,"AppendInt","das_call_member< void (FString::*)(int) , &FString::AppendInt >::invoke")
		->args({"self","InNum"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_209 = das::das_call_member< bool (FString::*)() const,&FString::ToBool >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Containers/UnrealString.h:1943:29
	makeExtern<DAS_CALL_METHOD(_method_209), SimNode_ExtFuncCall >(lib,"ToBool","das_call_member< bool (FString::*)() const , &FString::ToBool >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Containers/UnrealString.h:1953:39
	makeExtern< FString (*)(const FString *,const unsigned char *,const unsigned int) , _dasUnreal_static_65_FromBlob , SimNode_ExtFuncCallAndCopyOrMove >(lib,"FromBlob","_dasUnreal_static_65_FromBlob")
		->args({"self","SrcBuffer","SrcSize"})
		->arg_type(0,makeType<TExplicit<const FString*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Containers/UnrealString.h:1963:23
	makeExtern< bool (*)(const FString *,const FString &,unsigned char *,const unsigned int) , _dasUnreal_static_66_ToBlob , SimNode_ExtFuncCall >(lib,"ToBlob","_dasUnreal_static_66_ToBlob")
		->args({"self","Source","DestBuffer","DestSize"})
		->arg_type(0,makeType<TExplicit<const FString*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Containers/UnrealString.h:1973:39
	makeExtern< FString (*)(const FString *,const unsigned char *,const unsigned int) , _dasUnreal_static_67_FromHexBlob , SimNode_ExtFuncCallAndCopyOrMove >(lib,"FromHexBlob","_dasUnreal_static_67_FromHexBlob")
		->args({"self","SrcBuffer","SrcSize"})
		->arg_type(0,makeType<TExplicit<const FString*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Containers/UnrealString.h:1983:23
	makeExtern< bool (*)(const FString *,const FString &,unsigned char *,const unsigned int) , _dasUnreal_static_68_ToHexBlob , SimNode_ExtFuncCall >(lib,"ToHexBlob","_dasUnreal_static_68_ToHexBlob")
		->args({"self","Source","DestBuffer","DestSize"})
		->arg_type(0,makeType<TExplicit<const FString*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Containers/UnrealString.h:1994:39
	makeExtern< FString (*)(const FString *,double,const int) , _dasUnreal_static_69_SanitizeFloat , SimNode_ExtFuncCallAndCopyOrMove >(lib,"SanitizeFloat","_dasUnreal_static_69_SanitizeFloat")
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
	using _method_215 = das::das_call_member< int (FTextRange::*)() const,&FTextRange::Len >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Containers/UnrealString.h:2569:8
	makeExtern<DAS_CALL_METHOD(_method_215), SimNode_ExtFuncCall >(lib,"Len","das_call_member< int (FTextRange::*)() const , &FTextRange::Len >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_216 = das::das_call_member< bool (FTextRange::*)() const,&FTextRange::IsEmpty >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Containers/UnrealString.h:2570:7
	makeExtern<DAS_CALL_METHOD(_method_216), SimNode_ExtFuncCall >(lib,"IsEmpty","das_call_member< bool (FTextRange::*)() const , &FTextRange::IsEmpty >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_217 = das::das_call_member< void (FTextRange::*)(int),&FTextRange::Offset >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Containers/UnrealString.h:2571:7
	makeExtern<DAS_CALL_METHOD(_method_217), SimNode_ExtFuncCall >(lib,"Offset","das_call_member< void (FTextRange::*)(int) , &FTextRange::Offset >::invoke")
		->args({"self","Amount"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_218 = das::das_call_member< bool (FTextRange::*)(int) const,&FTextRange::Contains >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Containers/UnrealString.h:2572:7
	makeExtern<DAS_CALL_METHOD(_method_218), SimNode_ExtFuncCall >(lib,"Contains","das_call_member< bool (FTextRange::*)(int) const , &FTextRange::Contains >::invoke")
		->args({"self","Index"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_219 = das::das_call_member< bool (FTextRange::*)(int) const,&FTextRange::InclusiveContains >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Containers/UnrealString.h:2573:7
	makeExtern<DAS_CALL_METHOD(_method_219), SimNode_ExtFuncCall >(lib,"InclusiveContains","das_call_member< bool (FTextRange::*)(int) const , &FTextRange::InclusiveContains >::invoke")
		->args({"self","Index"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_220 = das::das_call_member< FTextRange (FTextRange::*)(const FTextRange &) const,&FTextRange::Intersect >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Containers/UnrealString.h:2575:13
	makeExtern<DAS_CALL_METHOD(_method_220), SimNode_ExtFuncCallAndCopyOrMove >(lib,"Intersect","das_call_member< FTextRange (FTextRange::*)(const FTextRange &) const , &FTextRange::Intersect >::invoke")
		->args({"self","Other"})
		->addToModule(*this, SideEffects::worstDefault);
	addCtorAndUsing<FVTableHelper>(*this,lib,"FVTableHelper","FVTableHelper");
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/ObjectMacros.h:516:30
	makeExtern< const wchar_t * (*)(EPropertyObjectReferenceType) , LexToString , SimNode_ExtFuncCall >(lib,"LexToString","LexToString")
		->args({"Type"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:231:23
	makeExtern< bool (*)(UObject *) , UE::IsSavingPackage , SimNode_ExtFuncCall >(lib,"IsSavingPackage","UE::IsSavingPackage")
		->args({"InOuter"})
		->arg_init(0,make_smart<ExprConstPtr>())
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:240:27
	makeExtern< UPackage * (*)() , GetTransientPackage , SimNode_ExtFuncCall >(lib,"GetTransientPackage","GetTransientPackage")
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:243:26
	makeExtern< UObject * (*)(UClass *) , GetTransientOuterForRename , SimNode_ExtFuncCall >(lib,"GetTransientOuterForRename","GetTransientOuterForRename")
		->args({"ForClass"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:250:32
	makeExtern< const FString * (*)(const FString &) , GetIniFilenameFromObjectsReference , SimNode_ExtFuncCall >(lib,"GetIniFilenameFromObjectsReference","GetIniFilenameFromObjectsReference")
		->args({"ObjectsReferenceString"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:261:25
	makeExtern< FString (*)(const FString &,const FString *,bool) , ResolveIniObjectsReference , SimNode_ExtFuncCallAndCopyOrMove >(lib,"ResolveIniObjectsReference","ResolveIniObjectsReference")
		->args({"ObjectReference","IniFilename","bThrow"})
		->arg_init(1,make_smart<ExprConstPtr>())
		->arg_init(2,make_smart<ExprConstBool>(false))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:275:22
	makeExtern< bool (*)(UObject *&,FString &,bool,bool,unsigned int,const FLinkerInstancingContext *) , ResolveName , SimNode_ExtFuncCall >(lib,"ResolveName","ResolveName")
		->args({"Outer","ObjectsReferenceString","Create","Throw","LoadFlags","InstancingContext"})
		->arg_init(4,make_smart<ExprConstUInt>(0x0))
		->arg_init(5,make_smart<ExprConstPtr>())
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:278:22
	makeExtern< bool (*)(UObject *,unsigned int,const wchar_t *) , SafeLoadError , SimNode_ExtFuncCall >(lib,"SafeLoadError","SafeLoadError")
		->args({"Outer","LoadFlags","ErrorMessage"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:298:26
	makeExtern< UObject * (*)(UClass *,UObject *,FName,bool,bool,EObjectFlags,EInternalObjectFlags) , StaticFindObjectFast , SimNode_ExtFuncCall >(lib,"StaticFindObjectFast","StaticFindObjectFast")
		->args({"Class","InOuter","InName","bExactClass","bAnyPackage","ExclusiveFlags","ExclusiveInternalFlags"})
		->arg_init(5,make_smart<ExprConstEnumeration>(0,makeType<EObjectFlags>(lib)))
		->arg_init(6,make_smart<ExprConstEnumeration>(0,makeType<EInternalObjectFlags>(lib)))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:313:26
	makeExtern< UObject * (*)(UClass *,UObject *,FName,bool,EObjectFlags,EInternalObjectFlags) , StaticFindObjectFast , SimNode_ExtFuncCall >(lib,"StaticFindObjectFast","StaticFindObjectFast")
		->args({"Class","InOuter","InName","bExactClass","ExclusiveFlags","ExclusiveInternalFlags"})
		->arg_init(3,make_smart<ExprConstBool>(false))
		->arg_init(4,make_smart<ExprConstEnumeration>(0,makeType<EObjectFlags>(lib)))
		->arg_init(5,make_smart<ExprConstEnumeration>(0,makeType<EInternalObjectFlags>(lib)))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:331:26
	makeExtern< UObject * (*)(UClass *,UObject *,FName,bool,bool,EObjectFlags,EInternalObjectFlags) , StaticFindObjectFastSafe , SimNode_ExtFuncCall >(lib,"StaticFindObjectFastSafe","StaticFindObjectFastSafe")
		->args({"Class","InOuter","InName","bExactClass","bAnyPackage","ExclusiveFlags","ExclusiveInternalFlags"})
		->arg_init(5,make_smart<ExprConstEnumeration>(0,makeType<EObjectFlags>(lib)))
		->arg_init(6,make_smart<ExprConstEnumeration>(0,makeType<EInternalObjectFlags>(lib)))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:348:26
	makeExtern< UObject * (*)(UClass *,UObject *,FName,bool,EObjectFlags,EInternalObjectFlags) , StaticFindObjectFastSafe , SimNode_ExtFuncCall >(lib,"StaticFindObjectFastSafe","StaticFindObjectFastSafe")
		->args({"Class","InOuter","InName","bExactClass","ExclusiveFlags","ExclusiveInternalFlags"})
		->arg_init(3,make_smart<ExprConstBool>(false))
		->arg_init(4,make_smart<ExprConstEnumeration>(0,makeType<EObjectFlags>(lib)))
		->arg_init(5,make_smart<ExprConstEnumeration>(0,makeType<EInternalObjectFlags>(lib)))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:360:26
	makeExtern< UObject * (*)(UClass *,UObject *,const wchar_t *,bool) , StaticFindObject , SimNode_ExtFuncCall >(lib,"StaticFindObject","StaticFindObject")
		->args({"Class","InOuter","Name","ExactClass"})
		->arg_init(3,make_smart<ExprConstBool>(false))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:374:26
	makeExtern< UObject * (*)(UClass *,UObject *,const wchar_t *,bool) , StaticFindObjectChecked , SimNode_ExtFuncCall >(lib,"StaticFindObjectChecked","StaticFindObjectChecked")
		->args({"Class","InOuter","Name","ExactClass"})
		->arg_init(3,make_smart<ExprConstBool>(false))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:377:26
	makeExtern< UObject * (*)(UClass *,UObject *,const wchar_t *,bool) , StaticFindObjectSafe , SimNode_ExtFuncCall >(lib,"StaticFindObjectSafe","StaticFindObjectSafe")
		->args({"Class","InOuter","Name","ExactClass"})
		->arg_init(3,make_smart<ExprConstBool>(false))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:483:26
	makeExtern< UObject * (*)(UClass *,const wchar_t *,EFindFirstObjectOptions,ELogVerbosity::Type,const wchar_t *) , StaticFindFirstObject , SimNode_ExtFuncCall >(lib,"StaticFindFirstObject","StaticFindFirstObject")
		->args({"Class","Name","Options","AmbiguousMessageVerbosity","InCurrentOperation"})
		->arg_init(2,make_smart<ExprConstEnumeration>(0,makeType<EFindFirstObjectOptions>(lib)))
		->arg_init(3,make_smart<ExprConstEnumeration>(0,makeType<ELogVerbosity::Type>(lib)))
		->arg_init(4,make_smart<ExprConstPtr>())
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:499:26
	makeExtern< UObject * (*)(UClass *,const wchar_t *,EFindFirstObjectOptions,ELogVerbosity::Type,const wchar_t *) , StaticFindFirstObjectSafe , SimNode_ExtFuncCall >(lib,"StaticFindFirstObjectSafe","StaticFindFirstObjectSafe")
		->args({"Class","Name","Options","AmbiguousMessageVerbosity","InCurrentOperation"})
		->arg_init(2,make_smart<ExprConstEnumeration>(0,makeType<EFindFirstObjectOptions>(lib)))
		->arg_init(3,make_smart<ExprConstEnumeration>(0,makeType<ELogVerbosity::Type>(lib)))
		->arg_init(4,make_smart<ExprConstPtr>())
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:523:22
	makeExtern< bool (*)(const wchar_t *,const wchar_t *,UClass *,UObject *&,UObject *,EParseObjectLoadingPolicy,bool *) , ParseObject , SimNode_ExtFuncCall >(lib,"ParseObject","ParseObject")
		->args({"Stream","Match","Class","DestRes","InParent","LoadingPolicy","bInvalidObject"})
		->arg_init(6,make_smart<ExprConstPtr>())
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:524:13
	makeExtern< bool (*)(const wchar_t *,const wchar_t *,UClass *,UObject *&,UObject *,bool *) , ParseObject , SimNode_ExtFuncCall >(lib,"ParseObject","ParseObject")
		->args({"Stream","Match","Class","DestRes","InParent","bInvalidObject"})
		->arg_init(5,make_smart<ExprConstPtr>())
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:544:26
	makeExtern< UObject * (*)(UClass *,UObject *,const wchar_t *,const wchar_t *,unsigned int,UPackageMap *,bool,const FLinkerInstancingContext *) , StaticLoadObject , SimNode_ExtFuncCall >(lib,"StaticLoadObject","StaticLoadObject")
		->args({"Class","InOuter","Name","Filename","LoadFlags","Sandbox","bAllowObjectReconciliation","InstancingContext"})
		->arg_init(3,make_smart<ExprConstPtr>())
		->arg_init(4,make_smart<ExprConstUInt>(0x0))
		->arg_init(5,make_smart<ExprConstPtr>())
		->arg_init(6,make_smart<ExprConstBool>(true))
		->arg_init(7,make_smart<ExprConstPtr>())
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:547:25
	makeExtern< UClass * (*)(UClass *,UObject *,const wchar_t *,const wchar_t *,unsigned int,UPackageMap *) , StaticLoadClass , SimNode_ExtFuncCall >(lib,"StaticLoadClass","StaticLoadClass")
		->args({"BaseClass","InOuter","Name","Filename","LoadFlags","Sandbox"})
		->arg_init(3,make_smart<ExprConstPtr>())
		->arg_init(4,make_smart<ExprConstUInt>(0x0))
		->arg_init(5,make_smart<ExprConstPtr>())
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:558:26
	makeExtern< UObject * (*)(const FStaticConstructObjectParameters &) , StaticConstructObject_Internal , SimNode_ExtFuncCall >(lib,"StaticConstructObject_Internal","StaticConstructObject_Internal")
		->args({"Params"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:577:26
	makeExtern< UObject * (*)(const UObject *,UObject *,const FName,EObjectFlags,UClass *,EDuplicateMode::Type,EInternalObjectFlags) , StaticDuplicateObject , SimNode_ExtFuncCall >(lib,"StaticDuplicateObject","StaticDuplicateObject")
		->args({"SourceObject","DestOuter","DestName","FlagMask","DestClass","DuplicateMode","InternalFlagsMask"})
		->arg_init(3,make_smart<ExprConstEnumeration>(-1,makeType<EObjectFlags>(lib)))
		->arg_init(4,make_smart<ExprConstPtr>())
		->arg_init(5,make_smart<ExprConstEnumeration>(0,makeType<EDuplicateMode::Type>(lib)))
		->arg_init(6,make_smart<ExprConstEnumeration>(-5242880,makeType<EInternalObjectFlags>(lib)))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:613:22
	makeExtern< void (*)(float,bool,float) , StaticTick , SimNode_ExtFuncCall >(lib,"StaticTick","StaticTick")
		->args({"DeltaTime","bUseFullTimeLimit","AsyncLoadingTime"})
		->arg_init(1,make_smart<ExprConstBool>(true))
		->arg_init(2,make_smart<ExprConstFloat>(0.00499999988824129))
		->addToModule(*this, SideEffects::worstDefault);
}
}

