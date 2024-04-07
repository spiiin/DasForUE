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
	using _method_207 = das::das_call_member< void (FString::*)(FArchive &,int) const,&FString::SerializeAsANSICharArray >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Containers/UnrealString.h:1922:16
	makeExtern<DAS_CALL_METHOD(_method_207), SimNode_ExtFuncCall >(lib,"SerializeAsANSICharArray","das_call_member< void (FString::*)(FArchive &,int) const , &FString::SerializeAsANSICharArray >::invoke")
		->args({"self","Ar","MinCharacters"})
		->arg_init(2,make_smart<ExprConstInt>(0))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Containers/UnrealString.h:1926:42
	makeExtern< FString (*)(const FString *,int) , _dasUnreal_static_64_FromInt , SimNode_ExtFuncCallAndCopyOrMove >(lib,"FromInt","_dasUnreal_static_64_FromInt")
		->args({"self","Num"})
		->arg_type(0,makeType<TExplicit<const FString*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
	using _method_209 = das::das_call_member< void (FString::*)(int),&FString::AppendInt >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Containers/UnrealString.h:1934:16
	makeExtern<DAS_CALL_METHOD(_method_209), SimNode_ExtFuncCall >(lib,"AppendInt","das_call_member< void (FString::*)(int) , &FString::AppendInt >::invoke")
		->args({"self","InNum"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_210 = das::das_call_member< bool (FString::*)() const,&FString::ToBool >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Containers/UnrealString.h:1943:29
	makeExtern<DAS_CALL_METHOD(_method_210), SimNode_ExtFuncCall >(lib,"ToBool","das_call_member< bool (FString::*)() const , &FString::ToBool >::invoke")
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
	using _method_216 = das::das_call_member< void (FString::*)(FArchive &) const,&FString::CountBytes >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Containers/UnrealString.h:2057:19
	makeExtern<DAS_CALL_METHOD(_method_216), SimNode_ExtFuncCall >(lib,"CountBytes","das_call_member< void (FString::*)(FArchive &) const , &FString::CountBytes >::invoke")
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
	using _method_217 = das::das_call_member< int (FTextRange::*)() const,&FTextRange::Len >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Containers/UnrealString.h:2569:8
	makeExtern<DAS_CALL_METHOD(_method_217), SimNode_ExtFuncCall >(lib,"Len","das_call_member< int (FTextRange::*)() const , &FTextRange::Len >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_218 = das::das_call_member< bool (FTextRange::*)() const,&FTextRange::IsEmpty >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Containers/UnrealString.h:2570:7
	makeExtern<DAS_CALL_METHOD(_method_218), SimNode_ExtFuncCall >(lib,"IsEmpty","das_call_member< bool (FTextRange::*)() const , &FTextRange::IsEmpty >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_219 = das::das_call_member< void (FTextRange::*)(int),&FTextRange::Offset >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Containers/UnrealString.h:2571:7
	makeExtern<DAS_CALL_METHOD(_method_219), SimNode_ExtFuncCall >(lib,"Offset","das_call_member< void (FTextRange::*)(int) , &FTextRange::Offset >::invoke")
		->args({"self","Amount"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_220 = das::das_call_member< bool (FTextRange::*)(int) const,&FTextRange::Contains >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Containers/UnrealString.h:2572:7
	makeExtern<DAS_CALL_METHOD(_method_220), SimNode_ExtFuncCall >(lib,"Contains","das_call_member< bool (FTextRange::*)(int) const , &FTextRange::Contains >::invoke")
		->args({"self","Index"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_221 = das::das_call_member< bool (FTextRange::*)(int) const,&FTextRange::InclusiveContains >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Containers/UnrealString.h:2573:7
	makeExtern<DAS_CALL_METHOD(_method_221), SimNode_ExtFuncCall >(lib,"InclusiveContains","das_call_member< bool (FTextRange::*)(int) const , &FTextRange::InclusiveContains >::invoke")
		->args({"self","Index"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_222 = das::das_call_member< FTextRange (FTextRange::*)(const FTextRange &) const,&FTextRange::Intersect >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Containers/UnrealString.h:2575:13
	makeExtern<DAS_CALL_METHOD(_method_222), SimNode_ExtFuncCallAndCopyOrMove >(lib,"Intersect","das_call_member< FTextRange (FTextRange::*)(const FTextRange &) const , &FTextRange::Intersect >::invoke")
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
	addCtorAndUsing<FPrimaryAssetType>(*this,lib,"FPrimaryAssetType","FPrimaryAssetType");
	addCtorAndUsing<FPrimaryAssetType,FName>(*this,lib,"FPrimaryAssetType","FPrimaryAssetType")
		->args({"InName"});
	addCtorAndUsing<FPrimaryAssetType,EName>(*this,lib,"FPrimaryAssetType","FPrimaryAssetType")
		->args({"InName"});
	addCtorAndUsing<FPrimaryAssetType,const char *>(*this,lib,"FPrimaryAssetType","FPrimaryAssetType")
		->args({"InName"});
	addCtorAndUsing<FPrimaryAssetType,const FPrimaryAssetType &>(*this,lib,"FPrimaryAssetType","FPrimaryAssetType")
		->args({""});
	using _method_223 = das::das_call_member< FName (FPrimaryAssetType::*)() const,&FPrimaryAssetType::GetName >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/PrimaryAssetId.h:41:8
	makeExtern<DAS_CALL_METHOD(_method_223), SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetName","das_call_member< FName (FPrimaryAssetType::*)() const , &FPrimaryAssetType::GetName >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_224 = das::das_call_member< bool (FPrimaryAssetType::*)() const,&FPrimaryAssetType::IsValid >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/PrimaryAssetId.h:57:7
	makeExtern<DAS_CALL_METHOD(_method_224), SimNode_ExtFuncCall >(lib,"IsValid","das_call_member< bool (FPrimaryAssetType::*)() const , &FPrimaryAssetType::IsValid >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_225 = das::das_call_member< FString (FPrimaryAssetType::*)() const,&FPrimaryAssetType::ToString >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/PrimaryAssetId.h:63:10
	makeExtern<DAS_CALL_METHOD(_method_225), SimNode_ExtFuncCallAndCopyOrMove >(lib,"ToString","das_call_member< FString (FPrimaryAssetType::*)() const , &FPrimaryAssetType::ToString >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_226 = das::das_call_member< bool (FPrimaryAssetType::*)(const FPrimaryAssetType &) const,&FPrimaryAssetType::LexicalLess >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/PrimaryAssetId.h:84:7
	makeExtern<DAS_CALL_METHOD(_method_226), SimNode_ExtFuncCall >(lib,"LexicalLess","das_call_member< bool (FPrimaryAssetType::*)(const FPrimaryAssetType &) const , &FPrimaryAssetType::LexicalLess >::invoke")
		->args({"self","Other"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_227 = das::das_call_member< bool (FPrimaryAssetType::*)(const FPrimaryAssetType &) const,&FPrimaryAssetType::FastLess >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/PrimaryAssetId.h:89:7
	makeExtern<DAS_CALL_METHOD(_method_227), SimNode_ExtFuncCall >(lib,"FastLess","das_call_member< bool (FPrimaryAssetType::*)(const FPrimaryAssetType &) const , &FPrimaryAssetType::FastLess >::invoke")
		->args({"self","Other"})
		->addToModule(*this, SideEffects::worstDefault);
	addCtorAndUsing<FPrimaryAssetId>(*this,lib,"FPrimaryAssetId","FPrimaryAssetId");
	addCtorAndUsing<FPrimaryAssetId,FPrimaryAssetType,FName>(*this,lib,"FPrimaryAssetId","FPrimaryAssetId")
		->args({"InAssetType","InAssetName"});
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/PrimaryAssetId.h:122:41
	makeExtern< FPrimaryAssetId (*)(const FPrimaryAssetId *,const wchar_t *,unsigned int) , _dasUnreal_static_70_ParseTypeAndName , SimNode_ExtFuncCallAndCopyOrMove >(lib,"ParseTypeAndName","_dasUnreal_static_70_ParseTypeAndName")
		->args({"self","TypeAndName","Len"})
		->arg_type(0,makeType<TExplicit<const FPrimaryAssetId*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/PrimaryAssetId.h:123:41
	makeExtern< FPrimaryAssetId (*)(const FPrimaryAssetId *,FName) , _dasUnreal_static_71_ParseTypeAndName , SimNode_ExtFuncCallAndCopyOrMove >(lib,"ParseTypeAndName","_dasUnreal_static_71_ParseTypeAndName")
		->args({"self","TypeAndName"})
		->arg_type(0,makeType<TExplicit<const FPrimaryAssetId*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/PrimaryAssetId.h:124:25
	makeExtern< FPrimaryAssetId (*)(const FPrimaryAssetId *,const FString &) , _dasUnreal_static_72_ParseTypeAndName , SimNode_ExtFuncCallAndCopyOrMove >(lib,"ParseTypeAndName","_dasUnreal_static_72_ParseTypeAndName")
		->args({"self","TypeAndName"})
		->arg_type(0,makeType<TExplicit<const FPrimaryAssetId*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
	addCtorAndUsing<FPrimaryAssetId,const FString &>(*this,lib,"FPrimaryAssetId","FPrimaryAssetId")
		->args({"TypeAndName"});
	addCtorAndUsing<FPrimaryAssetId,const FPrimaryAssetId &>(*this,lib,"FPrimaryAssetId","FPrimaryAssetId")
		->args({""});
	using _method_231 = das::das_call_member< bool (FPrimaryAssetId::*)() const,&FPrimaryAssetId::IsValid >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/PrimaryAssetId.h:139:7
	makeExtern<DAS_CALL_METHOD(_method_231), SimNode_ExtFuncCall >(lib,"IsValid","das_call_member< bool (FPrimaryAssetId::*)() const , &FPrimaryAssetId::IsValid >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_232 = das::das_call_member< FString (FPrimaryAssetId::*)() const,&FPrimaryAssetId::ToString >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/PrimaryAssetId.h:145:10
	makeExtern<DAS_CALL_METHOD(_method_232), SimNode_ExtFuncCallAndCopyOrMove >(lib,"ToString","das_call_member< FString (FPrimaryAssetId::*)() const , &FPrimaryAssetId::ToString >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/PrimaryAssetId.h:164:25
	makeExtern< FPrimaryAssetId (*)(const FPrimaryAssetId *,const FString &) , _dasUnreal_static_73_FromString , SimNode_ExtFuncCallAndCopyOrMove >(lib,"FromString","_dasUnreal_static_73_FromString")
		->args({"self","String"})
		->arg_type(0,makeType<TExplicit<const FPrimaryAssetId*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
}
}

