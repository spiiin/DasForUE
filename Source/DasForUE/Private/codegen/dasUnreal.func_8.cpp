// this file is generated via daScript automatic C++ binder
// all user modifications will be lost after this file is re-generated

#include "dasUnreal.h"
#include "need_dasUnreal.h"
namespace das {
#include "dasUnreal.func.aot.decl.inc"
void Module_dasUnreal::initFunctions_8() {
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Field.h:1040:25
	makeExtern< FString (*)(const FField *) , GetPathNameSafe , SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetPathNameSafe","GetPathNameSafe")
		->args({"InField"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Field.h:1044:25
	makeExtern< FField * (*)(const wchar_t *) , FindFPropertyByPath , SimNode_ExtFuncCall >(lib,"FindFPropertyByPath","FindFPropertyByPath")
		->args({"InFieldPath"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:157:2
	makeExtern< UClass * (*)(const UField *) , _dasUnreal_static_184_StaticClass , SimNode_ExtFuncCall >(lib,"StaticClass","_dasUnreal_static_184_StaticClass")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const UField*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:157:2
	makeExtern< const wchar_t * (*)(const UField *) , _dasUnreal_static_185_StaticPackage , SimNode_ExtFuncCall >(lib,"StaticPackage","_dasUnreal_static_185_StaticPackage")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const UField*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:157:2
	makeExtern< void (*)(const UField *) , _dasUnreal_static_186_StaticRegisterNativesUField , SimNode_ExtFuncCall >(lib,"StaticRegisterNativesUField","_dasUnreal_static_186_StaticRegisterNativesUField")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const UField*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:170:31
	makeExtern< void (*)(UField &) , _dasUnreal_virtual_187_PostLoad , SimNode_ExtFuncCall >(lib,"PostLoad","_dasUnreal_virtual_187_PostLoad")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:171:31
	makeExtern< bool (*)(const UField &) , _dasUnreal_virtual_188_NeedsLoadForClient , SimNode_ExtFuncCall >(lib,"NeedsLoadForClient","_dasUnreal_virtual_188_NeedsLoadForClient")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:172:31
	makeExtern< bool (*)(const UField &) , _dasUnreal_virtual_189_NeedsLoadForServer , SimNode_ExtFuncCall >(lib,"NeedsLoadForServer","_dasUnreal_virtual_189_NeedsLoadForServer")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:176:31
	makeExtern< void (*)(UField &) , _dasUnreal_virtual_190_Bind , SimNode_ExtFuncCall >(lib,"Bind","_dasUnreal_virtual_190_Bind")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_549 = das::das_call_member< UClass * (UField::*)() const,&UField::GetOwnerClass >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:179:26
	makeExtern<DAS_CALL_METHOD(_method_549), SimNode_ExtFuncCall >(lib,"GetOwnerClass","das_call_member< UClass * (UField::*)() const , &UField::GetOwnerClass >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_550 = das::das_call_member< UStruct * (UField::*)() const,&UField::GetOwnerStruct >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:182:27
	makeExtern<DAS_CALL_METHOD(_method_550), SimNode_ExtFuncCall >(lib,"GetOwnerStruct","das_call_member< UStruct * (UField::*)() const , &UField::GetOwnerStruct >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_551 = das::das_call_member< FString (UField::*)() const,&UField::GetAuthoredName >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:189:26
	makeExtern<DAS_CALL_METHOD(_method_551), SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetAuthoredName","das_call_member< FString (UField::*)() const , &UField::GetAuthoredName >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_552 = das::das_call_member< FText (UField::*)() const,&UField::GetDisplayNameText >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:197:24
	makeExtern<DAS_CALL_METHOD(_method_552), SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetDisplayNameText","das_call_member< FText (UField::*)() const , &UField::GetDisplayNameText >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_553 = das::das_call_member< FText (UField::*)(bool) const,&UField::GetToolTipText >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:206:24
	makeExtern<DAS_CALL_METHOD(_method_553), SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetToolTipText","das_call_member< FText (UField::*)(bool) const , &UField::GetToolTipText >::invoke")
		->args({"self","bShortTooltip"})
		->arg_init(1,make_smart<ExprConstBool>(false))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:214:30
	makeExtern< void (*)(const UField *,FString &,bool) , _dasUnreal_static_191_FormatNativeToolTip , SimNode_ExtFuncCall >(lib,"FormatNativeToolTip","_dasUnreal_static_191_FormatNativeToolTip")
		->args({"self","ToolTipString","bRemoveExtraSections"})
		->arg_type(0,makeType<TExplicit<const UField*> >(lib))
		->arg_init(2,make_smart<ExprConstBool>(true))
		->addToModule(*this, SideEffects::worstDefault);
	using _method_555 = das::das_call_member< bool (UField::*)(const wchar_t *) const,&UField::HasMetaData >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:222:7
	makeExtern<DAS_CALL_METHOD(_method_555), SimNode_ExtFuncCall >(lib,"HasMetaData","das_call_member< bool (UField::*)(const wchar_t *) const , &UField::HasMetaData >::invoke")
		->args({"self","Key"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_556 = das::das_call_member< bool (UField::*)(const FName &) const,&UField::HasMetaData >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:223:7
	makeExtern<DAS_CALL_METHOD(_method_556), SimNode_ExtFuncCall >(lib,"HasMetaData","das_call_member< bool (UField::*)(const FName &) const , &UField::HasMetaData >::invoke")
		->args({"self","Key"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_557 = das::das_call_member< const FString * (UField::*)(const wchar_t *) const,&UField::FindMetaData >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:231:33
	makeExtern<DAS_CALL_METHOD(_method_557), SimNode_ExtFuncCall >(lib,"FindMetaData","das_call_member< const FString * (UField::*)(const wchar_t *) const , &UField::FindMetaData >::invoke")
		->args({"self","Key"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_558 = das::das_call_member< const FString * (UField::*)(const FName &) const,&UField::FindMetaData >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:232:33
	makeExtern<DAS_CALL_METHOD(_method_558), SimNode_ExtFuncCall >(lib,"FindMetaData","das_call_member< const FString * (UField::*)(const FName &) const , &UField::FindMetaData >::invoke")
		->args({"self","Key"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_559 = das::das_call_member< const FString & (UField::*)(const wchar_t *) const,&UField::GetMetaData >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:240:33
	makeExtern<DAS_CALL_METHOD(_method_559), SimNode_ExtFuncCallRef >(lib,"GetMetaData","das_call_member< const FString & (UField::*)(const wchar_t *) const , &UField::GetMetaData >::invoke")
		->args({"self","Key"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_560 = das::das_call_member< const FString & (UField::*)(const FName &) const,&UField::GetMetaData >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:241:33
	makeExtern<DAS_CALL_METHOD(_method_560), SimNode_ExtFuncCallRef >(lib,"GetMetaData","das_call_member< const FString & (UField::*)(const FName &) const , &UField::GetMetaData >::invoke")
		->args({"self","Key"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_561 = das::das_call_member< FText (UField::*)(const wchar_t *,const FString,const FString) const,&UField::GetMetaDataText >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:251:24
	makeExtern<DAS_CALL_METHOD(_method_561), SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetMetaDataText","das_call_member< FText (UField::*)(const wchar_t *,const FString,const FString) const , &UField::GetMetaDataText >::invoke")
		->args({"self","MetaDataKey","LocalizationNamespace","LocalizationKey"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_562 = das::das_call_member< FText (UField::*)(const FName &,const FString,const FString) const,&UField::GetMetaDataText >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:252:24
	makeExtern<DAS_CALL_METHOD(_method_562), SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetMetaDataText","das_call_member< FText (UField::*)(const FName &,const FString,const FString) const , &UField::GetMetaDataText >::invoke")
		->args({"self","MetaDataKey","LocalizationNamespace","LocalizationKey"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_563 = das::das_call_member< void (UField::*)(const wchar_t *,const wchar_t *),&UField::SetMetaData >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:260:23
	makeExtern<DAS_CALL_METHOD(_method_563), SimNode_ExtFuncCall >(lib,"SetMetaData","das_call_member< void (UField::*)(const wchar_t *,const wchar_t *) , &UField::SetMetaData >::invoke")
		->args({"self","Key","InValue"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_564 = das::das_call_member< void (UField::*)(const FName &,const wchar_t *),&UField::SetMetaData >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:261:23
	makeExtern<DAS_CALL_METHOD(_method_564), SimNode_ExtFuncCall >(lib,"SetMetaData","das_call_member< void (UField::*)(const FName &,const wchar_t *) , &UField::SetMetaData >::invoke")
		->args({"self","Key","InValue"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_565 = das::das_call_member< bool (UField::*)(const wchar_t *) const,&UField::GetBoolMetaData >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:269:7
	makeExtern<DAS_CALL_METHOD(_method_565), SimNode_ExtFuncCall >(lib,"GetBoolMetaData","das_call_member< bool (UField::*)(const wchar_t *) const , &UField::GetBoolMetaData >::invoke")
		->args({"self","Key"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_566 = das::das_call_member< bool (UField::*)(const FName &) const,&UField::GetBoolMetaData >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:275:7
	makeExtern<DAS_CALL_METHOD(_method_566), SimNode_ExtFuncCall >(lib,"GetBoolMetaData","das_call_member< bool (UField::*)(const FName &) const , &UField::GetBoolMetaData >::invoke")
		->args({"self","Key"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_567 = das::das_call_member< int (UField::*)(const wchar_t *) const,&UField::GetIntMetaData >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:288:8
	makeExtern<DAS_CALL_METHOD(_method_567), SimNode_ExtFuncCall >(lib,"GetIntMetaData","das_call_member< int (UField::*)(const wchar_t *) const , &UField::GetIntMetaData >::invoke")
		->args({"self","Key"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_568 = das::das_call_member< int (UField::*)(const FName &) const,&UField::GetIntMetaData >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:294:8
	makeExtern<DAS_CALL_METHOD(_method_568), SimNode_ExtFuncCall >(lib,"GetIntMetaData","das_call_member< int (UField::*)(const FName &) const , &UField::GetIntMetaData >::invoke")
		->args({"self","Key"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_569 = das::das_call_member< float (UField::*)(const wchar_t *) const,&UField::GetFloatMetaData >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:307:8
	makeExtern<DAS_CALL_METHOD(_method_569), SimNode_ExtFuncCall >(lib,"GetFloatMetaData","das_call_member< float (UField::*)(const wchar_t *) const , &UField::GetFloatMetaData >::invoke")
		->args({"self","Key"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_570 = das::das_call_member< float (UField::*)(const FName &) const,&UField::GetFloatMetaData >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:313:8
	makeExtern<DAS_CALL_METHOD(_method_570), SimNode_ExtFuncCall >(lib,"GetFloatMetaData","das_call_member< float (UField::*)(const FName &) const , &UField::GetFloatMetaData >::invoke")
		->args({"self","Key"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_571 = das::das_call_member< UClass * (UField::*)(const wchar_t *) const,&UField::GetClassMetaData >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:326:26
	makeExtern<DAS_CALL_METHOD(_method_571), SimNode_ExtFuncCall >(lib,"GetClassMetaData","das_call_member< UClass * (UField::*)(const wchar_t *) const , &UField::GetClassMetaData >::invoke")
		->args({"self","Key"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_572 = das::das_call_member< UClass * (UField::*)(const FName &) const,&UField::GetClassMetaData >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:327:26
	makeExtern<DAS_CALL_METHOD(_method_572), SimNode_ExtFuncCall >(lib,"GetClassMetaData","das_call_member< UClass * (UField::*)(const FName &) const , &UField::GetClassMetaData >::invoke")
		->args({"self","Key"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_573 = das::das_call_member< void (UField::*)(const wchar_t *),&UField::RemoveMetaData >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:330:23
	makeExtern<DAS_CALL_METHOD(_method_573), SimNode_ExtFuncCall >(lib,"RemoveMetaData","das_call_member< void (UField::*)(const wchar_t *) , &UField::RemoveMetaData >::invoke")
		->args({"self","Key"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_574 = das::das_call_member< void (UField::*)(const FName &),&UField::RemoveMetaData >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:331:23
	makeExtern<DAS_CALL_METHOD(_method_574), SimNode_ExtFuncCall >(lib,"RemoveMetaData","das_call_member< void (UField::*)(const FName &) , &UField::RemoveMetaData >::invoke")
		->args({"self","Key"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_575 = das::das_call_member< bool (UField::*)(const uint64) const,&UField::HasAnyCastFlags >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:334:23
	makeExtern<DAS_CALL_METHOD(_method_575), SimNode_ExtFuncCall >(lib,"HasAnyCastFlags","das_call_member< bool (UField::*)(const uint64) const , &UField::HasAnyCastFlags >::invoke")
		->args({"self","InCastFlags"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_576 = das::das_call_member< bool (UField::*)(const uint64) const,&UField::HasAllCastFlags >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:335:23
	makeExtern<DAS_CALL_METHOD(_method_576), SimNode_ExtFuncCall >(lib,"HasAllCastFlags","das_call_member< bool (UField::*)(const uint64) const , &UField::HasAllCastFlags >::invoke")
		->args({"self","InCastFlags"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:341:34
	makeExtern< FField * (*)(UField &) , _dasUnreal_virtual_192_GetAssociatedFField , SimNode_ExtFuncCall >(lib,"GetAssociatedFField","_dasUnreal_virtual_192_GetAssociatedFField")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:345:31
	makeExtern< void (*)(UField &,FField *) , _dasUnreal_virtual_193_SetAssociatedFField , SimNode_ExtFuncCall >(lib,"SetAssociatedFField","_dasUnreal_virtual_193_SetAssociatedFField")
		->args({"self","InField"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:388:2
	makeExtern< UClass * (*)(const UStruct *) , _dasUnreal_static_194_StaticClass , SimNode_ExtFuncCall >(lib,"StaticClass","_dasUnreal_static_194_StaticClass")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const UStruct*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:388:2
	makeExtern< const wchar_t * (*)(const UStruct *) , _dasUnreal_static_195_StaticPackage , SimNode_ExtFuncCall >(lib,"StaticPackage","_dasUnreal_static_195_StaticPackage")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const UStruct*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:388:2
	makeExtern< void (*)(const UStruct *) , _dasUnreal_static_196_StaticRegisterNativesUStruct , SimNode_ExtFuncCall >(lib,"StaticRegisterNativesUStruct","_dasUnreal_static_196_StaticRegisterNativesUStruct")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const UStruct*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
	using _method_582 = das::das_call_member< const FBlake3Hash & (UStruct::*)(bool) const,&UStruct::GetSchemaHash >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:441:37
	makeExtern<DAS_CALL_METHOD(_method_582), SimNode_ExtFuncCallRef >(lib,"GetSchemaHash","das_call_member< const FBlake3Hash & (UStruct::*)(bool) const , &UStruct::GetSchemaHash >::invoke")
		->args({"self","bSkipEditorOnly"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:458:31
	makeExtern< void (*)(UStruct &) , _dasUnreal_virtual_197_PostLoad , SimNode_ExtFuncCall >(lib,"PostLoad","_dasUnreal_virtual_197_PostLoad")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:459:31
	makeExtern< void (*)(UStruct &) , _dasUnreal_virtual_198_FinishDestroy , SimNode_ExtFuncCall >(lib,"FinishDestroy","_dasUnreal_virtual_198_FinishDestroy")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:460:31
	makeExtern< void (*)(UStruct &) , _dasUnreal_virtual_199_RegisterDependencies , SimNode_ExtFuncCall >(lib,"RegisterDependencies","_dasUnreal_virtual_199_RegisterDependencies")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:463:31
	makeExtern< void (*)(UStruct &,EObjectFlags) , _dasUnreal_virtual_200_TagSubobjects , SimNode_ExtFuncCall >(lib,"TagSubobjects","_dasUnreal_virtual_200_TagSubobjects")
		->args({"self","NewFlags"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:466:31
	makeExtern< void (*)(UStruct &,FProperty *) , _dasUnreal_virtual_201_AddCppProperty , SimNode_ExtFuncCall >(lib,"AddCppProperty","_dasUnreal_virtual_201_AddCppProperty")
		->args({"self","Property"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_588 = das::das_call_member< FProperty * (UStruct::*)(FName) const,&UStruct::FindPropertyByName >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:474:29
	makeExtern<DAS_CALL_METHOD(_method_588), SimNode_ExtFuncCall >(lib,"FindPropertyByName","das_call_member< FProperty * (UStruct::*)(FName) const , &UStruct::FindPropertyByName >::invoke")
		->args({"self","InName"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:488:19
	makeExtern< UStruct * (*)(const UStruct &) , _dasUnreal_virtual_202_GetInheritanceSuper , SimNode_ExtFuncCall >(lib,"GetInheritanceSuper","_dasUnreal_virtual_202_GetInheritanceSuper")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_590 = das::das_call_member< void (UStruct::*)(bool),&UStruct::StaticLink >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:491:23
	makeExtern<DAS_CALL_METHOD(_method_590), SimNode_ExtFuncCall >(lib,"StaticLink","das_call_member< void (UStruct::*)(bool) , &UStruct::StaticLink >::invoke")
		->args({"self","bRelinkExistingProperties"})
		->arg_init(1,make_smart<ExprConstBool>(false))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:562:31
	makeExtern< void (*)(const UStruct &,void *,int) , _dasUnreal_virtual_203_InitializeStruct , SimNode_ExtFuncCall >(lib,"InitializeStruct","_dasUnreal_virtual_203_InitializeStruct")
		->args({"self","Dest","ArrayDim"})
		->arg_init(2,make_smart<ExprConstInt>(1))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:571:31
	makeExtern< void (*)(const UStruct &,void *,int) , _dasUnreal_virtual_204_DestroyStruct , SimNode_ExtFuncCall >(lib,"DestroyStruct","_dasUnreal_virtual_204_DestroyStruct")
		->args({"self","Dest","ArrayDim"})
		->arg_init(2,make_smart<ExprConstInt>(1))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:574:21
	makeExtern< FProperty * (*)(const UStruct &,const FName) , _dasUnreal_virtual_205_CustomFindProperty , SimNode_ExtFuncCall >(lib,"CustomFindProperty","_dasUnreal_virtual_205_CustomFindProperty")
		->args({"self","InName"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:584:23
	makeExtern< const wchar_t * (*)(const UStruct &) , _dasUnreal_virtual_206_GetPrefixCPP , SimNode_ExtFuncCall >(lib,"GetPrefixCPP","_dasUnreal_virtual_206_GetPrefixCPP")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_595 = das::das_call_member< int (UStruct::*)() const,&UStruct::GetPropertiesSize >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:587:20
	makeExtern<DAS_CALL_METHOD(_method_595), SimNode_ExtFuncCall >(lib,"GetPropertiesSize","das_call_member< int (UStruct::*)() const , &UStruct::GetPropertiesSize >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_596 = das::das_call_member< int (UStruct::*)() const,&UStruct::GetMinAlignment >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:593:20
	makeExtern<DAS_CALL_METHOD(_method_596), SimNode_ExtFuncCall >(lib,"GetMinAlignment","das_call_member< int (UStruct::*)() const , &UStruct::GetMinAlignment >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_597 = das::das_call_member< int (UStruct::*)() const,&UStruct::GetStructureSize >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:599:20
	makeExtern<DAS_CALL_METHOD(_method_597), SimNode_ExtFuncCall >(lib,"GetStructureSize","das_call_member< int (UStruct::*)() const , &UStruct::GetStructureSize >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_598 = das::das_call_member< void (UStruct::*)(int),&UStruct::SetPropertiesSize >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:605:7
	makeExtern<DAS_CALL_METHOD(_method_598), SimNode_ExtFuncCall >(lib,"SetPropertiesSize","das_call_member< void (UStruct::*)(int) , &UStruct::SetPropertiesSize >::invoke")
		->args({"self","NewSize"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_599 = das::das_call_member< bool (UStruct::*)(const UStruct *) const,&UStruct::IsChildOf >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:619:23
	makeExtern<DAS_CALL_METHOD(_method_599), SimNode_ExtFuncCall >(lib,"IsChildOf","das_call_member< bool (UStruct::*)(const UStruct *) const , &UStruct::IsChildOf >::invoke")
		->args({"self","SomeBase"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_600 = das::das_call_member< UStruct * (UStruct::*)() const,&UStruct::GetSuperStruct >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:628:11
	makeExtern<DAS_CALL_METHOD(_method_600), SimNode_ExtFuncCall >(lib,"GetSuperStruct","das_call_member< UStruct * (UStruct::*)() const , &UStruct::GetSuperStruct >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:637:31
	makeExtern< void (*)(UStruct &,UStruct *) , _dasUnreal_virtual_207_SetSuperStruct , SimNode_ExtFuncCall >(lib,"SetSuperStruct","_dasUnreal_virtual_207_SetSuperStruct")
		->args({"self","NewSuperStruct"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:640:34
	makeExtern< FString (*)(const UStruct &,const UField *) , _dasUnreal_virtual_208_GetAuthoredNameForField , SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetAuthoredNameForField","_dasUnreal_virtual_208_GetAuthoredNameForField")
		->args({"self","Field"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:643:34
	makeExtern< FString (*)(const UStruct &,const FField *) , _dasUnreal_virtual_209_GetAuthoredNameForField , SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetAuthoredNameForField","_dasUnreal_virtual_209_GetAuthoredNameForField")
		->args({"self","Field"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:646:15
	makeExtern< bool (*)(const UStruct &) , _dasUnreal_virtual_210_IsStructTrashed , SimNode_ExtFuncCall >(lib,"IsStructTrashed","_dasUnreal_virtual_210_IsStructTrashed")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_605 = das::das_call_member< void (UStruct::*)(),&UStruct::DestroyChildPropertiesAndResetPropertyLinks >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:652:23
	makeExtern<DAS_CALL_METHOD(_method_605), SimNode_ExtFuncCall >(lib,"DestroyChildPropertiesAndResetPropertyLinks","das_call_member< void (UStruct::*)() , &UStruct::DestroyChildPropertiesAndResetPropertyLinks >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_606 = das::das_call_member< bool (UStruct::*)(const FName &) const,&UStruct::GetBoolMetaDataHierarchical >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:656:23
	makeExtern<DAS_CALL_METHOD(_method_606), SimNode_ExtFuncCall >(lib,"GetBoolMetaDataHierarchical","das_call_member< bool (UStruct::*)(const FName &) const , &UStruct::GetBoolMetaDataHierarchical >::invoke")
		->args({"self","Key"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_607 = das::das_call_member< bool (UStruct::*)(const FName &,FString *) const,&UStruct::GetStringMetaDataHierarchical >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:659:23
	makeExtern<DAS_CALL_METHOD(_method_607), SimNode_ExtFuncCall >(lib,"GetStringMetaDataHierarchical","das_call_member< bool (UStruct::*)(const FName &,FString *) const , &UStruct::GetStringMetaDataHierarchical >::invoke")
		->args({"self","Key","OutValue"})
		->arg_init(2,make_smart<ExprConstPtr>())
		->addToModule(*this, SideEffects::worstDefault);
	using _method_608 = das::das_call_member< const UStruct * (UStruct::*)(const FName &) const,&UStruct::HasMetaDataHierarchical >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:667:33
	makeExtern<DAS_CALL_METHOD(_method_608), SimNode_ExtFuncCall >(lib,"HasMetaDataHierarchical","das_call_member< const UStruct * (UStruct::*)(const FName &) const , &UStruct::HasMetaDataHierarchical >::invoke")
		->args({"self","Key"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_609 = das::das_call_member< bool (UStruct::*)() const,&UStruct::HasAssetRegistrySearchableProperties >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:670:19
	makeExtern<DAS_CALL_METHOD(_method_609), SimNode_ExtFuncCall >(lib,"HasAssetRegistrySearchableProperties","das_call_member< bool (UStruct::*)() const , &UStruct::HasAssetRegistrySearchableProperties >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_610 = das::das_call_member< void (UStruct::*)(),&UStruct::DeleteUnresolvedScriptProperties >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:687:19
	makeExtern<DAS_CALL_METHOD(_method_610), SimNode_ExtFuncCall >(lib,"DeleteUnresolvedScriptProperties","das_call_member< void (UStruct::*)() , &UStruct::DeleteUnresolvedScriptProperties >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_611 = das::das_call_member< void (UStruct::*)(),&UStruct::CollectBytecodeAndPropertyReferencedObjects >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:709:23
	makeExtern<DAS_CALL_METHOD(_method_611), SimNode_ExtFuncCall >(lib,"CollectBytecodeAndPropertyReferencedObjects","das_call_member< void (UStruct::*)() , &UStruct::CollectBytecodeAndPropertyReferencedObjects >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_612 = das::das_call_member< void (UStruct::*)(),&UStruct::CollectBytecodeAndPropertyReferencedObjectsRecursively >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:713:23
	makeExtern<DAS_CALL_METHOD(_method_612), SimNode_ExtFuncCall >(lib,"CollectBytecodeAndPropertyReferencedObjectsRecursively","das_call_member< void (UStruct::*)() , &UStruct::CollectBytecodeAndPropertyReferencedObjectsRecursively >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_613 = das::das_call_member< bool (UScriptStruct::ICppStructOps::*)() const,&UScriptStruct::ICppStructOps::HasNoopConstructor >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:951:8
	makeExtern<DAS_CALL_METHOD(_method_613), SimNode_ExtFuncCall >(lib,"HasNoopConstructor","das_call_member< bool (UScriptStruct::ICppStructOps::*)() const , &UScriptStruct::ICppStructOps::HasNoopConstructor >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_614 = das::das_call_member< bool (UScriptStruct::ICppStructOps::*)() const,&UScriptStruct::ICppStructOps::HasZeroConstructor >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:956:8
	makeExtern<DAS_CALL_METHOD(_method_614), SimNode_ExtFuncCall >(lib,"HasZeroConstructor","das_call_member< bool (UScriptStruct::ICppStructOps::*)() const , &UScriptStruct::ICppStructOps::HasZeroConstructor >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_615 = das::das_call_member< bool (UScriptStruct::ICppStructOps::*)() const,&UScriptStruct::ICppStructOps::HasDestructor >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:965:8
	makeExtern<DAS_CALL_METHOD(_method_615), SimNode_ExtFuncCall >(lib,"HasDestructor","das_call_member< bool (UScriptStruct::ICppStructOps::*)() const , &UScriptStruct::ICppStructOps::HasDestructor >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_616 = das::das_call_member< int (UScriptStruct::ICppStructOps::*)() const,&UScriptStruct::ICppStructOps::GetSize >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:972:21
	makeExtern<DAS_CALL_METHOD(_method_616), SimNode_ExtFuncCall >(lib,"GetSize","das_call_member< int (UScriptStruct::ICppStructOps::*)() const , &UScriptStruct::ICppStructOps::GetSize >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_617 = das::das_call_member< int (UScriptStruct::ICppStructOps::*)() const,&UScriptStruct::ICppStructOps::GetAlignment >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:977:21
	makeExtern<DAS_CALL_METHOD(_method_617), SimNode_ExtFuncCall >(lib,"GetAlignment","das_call_member< int (UScriptStruct::ICppStructOps::*)() const , &UScriptStruct::ICppStructOps::GetAlignment >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_618 = das::das_call_member< bool (UScriptStruct::ICppStructOps::*)() const,&UScriptStruct::ICppStructOps::HasSerializer >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:983:8
	makeExtern<DAS_CALL_METHOD(_method_618), SimNode_ExtFuncCall >(lib,"HasSerializer","das_call_member< bool (UScriptStruct::ICppStructOps::*)() const , &UScriptStruct::ICppStructOps::HasSerializer >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_619 = das::das_call_member< bool (UScriptStruct::ICppStructOps::*)() const,&UScriptStruct::ICppStructOps::HasStructuredSerializer >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:988:8
	makeExtern<DAS_CALL_METHOD(_method_619), SimNode_ExtFuncCall >(lib,"HasStructuredSerializer","das_call_member< bool (UScriptStruct::ICppStructOps::*)() const , &UScriptStruct::ICppStructOps::HasStructuredSerializer >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_620 = das::das_call_member< bool (UScriptStruct::ICppStructOps::*)() const,&UScriptStruct::ICppStructOps::HasPostSerialize >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:1000:8
	makeExtern<DAS_CALL_METHOD(_method_620), SimNode_ExtFuncCall >(lib,"HasPostSerialize","das_call_member< bool (UScriptStruct::ICppStructOps::*)() const , &UScriptStruct::ICppStructOps::HasPostSerialize >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_621 = das::das_call_member< bool (UScriptStruct::ICppStructOps::*)() const,&UScriptStruct::ICppStructOps::HasNetSerializer >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:1008:8
	makeExtern<DAS_CALL_METHOD(_method_621), SimNode_ExtFuncCall >(lib,"HasNetSerializer","das_call_member< bool (UScriptStruct::ICppStructOps::*)() const , &UScriptStruct::ICppStructOps::HasNetSerializer >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_622 = das::das_call_member< bool (UScriptStruct::ICppStructOps::*)() const,&UScriptStruct::ICppStructOps::HasNetSharedSerialization >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:1014:8
	makeExtern<DAS_CALL_METHOD(_method_622), SimNode_ExtFuncCall >(lib,"HasNetSharedSerialization","das_call_member< bool (UScriptStruct::ICppStructOps::*)() const , &UScriptStruct::ICppStructOps::HasNetSharedSerialization >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_623 = das::das_call_member< bool (UScriptStruct::ICppStructOps::*)() const,&UScriptStruct::ICppStructOps::HasNetDeltaSerializer >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:1025:8
	makeExtern<DAS_CALL_METHOD(_method_623), SimNode_ExtFuncCall >(lib,"HasNetDeltaSerializer","das_call_member< bool (UScriptStruct::ICppStructOps::*)() const , &UScriptStruct::ICppStructOps::HasNetDeltaSerializer >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_624 = das::das_call_member< bool (UScriptStruct::ICppStructOps::*)() const,&UScriptStruct::ICppStructOps::HasPostScriptConstruct >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:1036:8
	makeExtern<DAS_CALL_METHOD(_method_624), SimNode_ExtFuncCall >(lib,"HasPostScriptConstruct","das_call_member< bool (UScriptStruct::ICppStructOps::*)() const , &UScriptStruct::ICppStructOps::HasPostScriptConstruct >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_625 = das::das_call_member< bool (UScriptStruct::ICppStructOps::*)() const,&UScriptStruct::ICppStructOps::IsPlainOldData >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:1047:8
	makeExtern<DAS_CALL_METHOD(_method_625), SimNode_ExtFuncCall >(lib,"IsPlainOldData","das_call_member< bool (UScriptStruct::ICppStructOps::*)() const , &UScriptStruct::ICppStructOps::IsPlainOldData >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_626 = das::das_call_member< bool (UScriptStruct::ICppStructOps::*)() const,&UScriptStruct::ICppStructOps::IsUECoreType >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:1053:8
	makeExtern<DAS_CALL_METHOD(_method_626), SimNode_ExtFuncCall >(lib,"IsUECoreType","das_call_member< bool (UScriptStruct::ICppStructOps::*)() const , &UScriptStruct::ICppStructOps::IsUECoreType >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_627 = das::das_call_member< bool (UScriptStruct::ICppStructOps::*)() const,&UScriptStruct::ICppStructOps::IsUECoreVariant >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:1059:8
	makeExtern<DAS_CALL_METHOD(_method_627), SimNode_ExtFuncCall >(lib,"IsUECoreVariant","das_call_member< bool (UScriptStruct::ICppStructOps::*)() const , &UScriptStruct::ICppStructOps::IsUECoreVariant >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_628 = das::das_call_member< bool (UScriptStruct::ICppStructOps::*)() const,&UScriptStruct::ICppStructOps::HasCopy >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:1065:8
	makeExtern<DAS_CALL_METHOD(_method_628), SimNode_ExtFuncCall >(lib,"HasCopy","das_call_member< bool (UScriptStruct::ICppStructOps::*)() const , &UScriptStruct::ICppStructOps::HasCopy >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_629 = das::das_call_member< bool (UScriptStruct::ICppStructOps::*)() const,&UScriptStruct::ICppStructOps::HasIdentical >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:1076:8
	makeExtern<DAS_CALL_METHOD(_method_629), SimNode_ExtFuncCall >(lib,"HasIdentical","das_call_member< bool (UScriptStruct::ICppStructOps::*)() const , &UScriptStruct::ICppStructOps::HasIdentical >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_630 = das::das_call_member< bool (UScriptStruct::ICppStructOps::*)() const,&UScriptStruct::ICppStructOps::HasExportTextItem >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:1087:8
	makeExtern<DAS_CALL_METHOD(_method_630), SimNode_ExtFuncCall >(lib,"HasExportTextItem","das_call_member< bool (UScriptStruct::ICppStructOps::*)() const , &UScriptStruct::ICppStructOps::HasExportTextItem >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_631 = das::das_call_member< bool (UScriptStruct::ICppStructOps::*)() const,&UScriptStruct::ICppStructOps::HasImportTextItem >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:1098:8
	makeExtern<DAS_CALL_METHOD(_method_631), SimNode_ExtFuncCall >(lib,"HasImportTextItem","das_call_member< bool (UScriptStruct::ICppStructOps::*)() const , &UScriptStruct::ICppStructOps::HasImportTextItem >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_632 = das::das_call_member< bool (UScriptStruct::ICppStructOps::*)() const,&UScriptStruct::ICppStructOps::HasFindInnerPropertyInstance >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:1108:8
	makeExtern<DAS_CALL_METHOD(_method_632), SimNode_ExtFuncCall >(lib,"HasFindInnerPropertyInstance","das_call_member< bool (UScriptStruct::ICppStructOps::*)() const , &UScriptStruct::ICppStructOps::HasFindInnerPropertyInstance >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_633 = das::das_call_member< bool (UScriptStruct::ICppStructOps::*)() const,&UScriptStruct::ICppStructOps::HasAddStructReferencedObjects >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:1115:8
	makeExtern<DAS_CALL_METHOD(_method_633), SimNode_ExtFuncCall >(lib,"HasAddStructReferencedObjects","das_call_member< bool (UScriptStruct::ICppStructOps::*)() const , &UScriptStruct::ICppStructOps::HasAddStructReferencedObjects >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_634 = das::das_call_member< bool (UScriptStruct::ICppStructOps::*)(EPropertyObjectReferenceType) const,&UScriptStruct::ICppStructOps::HasSerializerObjectReferences >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:1120:8
	makeExtern<DAS_CALL_METHOD(_method_634), SimNode_ExtFuncCall >(lib,"HasSerializerObjectReferences","das_call_member< bool (UScriptStruct::ICppStructOps::*)(EPropertyObjectReferenceType) const , &UScriptStruct::ICppStructOps::HasSerializerObjectReferences >::invoke")
		->args({"self","Type"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_635 = das::das_call_member< bool (UScriptStruct::ICppStructOps::*)() const,&UScriptStruct::ICppStructOps::HasSerializeFromMismatchedTag >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:1132:8
	makeExtern<DAS_CALL_METHOD(_method_635), SimNode_ExtFuncCall >(lib,"HasSerializeFromMismatchedTag","das_call_member< bool (UScriptStruct::ICppStructOps::*)() const , &UScriptStruct::ICppStructOps::HasSerializeFromMismatchedTag >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_636 = das::das_call_member< bool (UScriptStruct::ICppStructOps::*)() const,&UScriptStruct::ICppStructOps::HasStructuredSerializeFromMismatchedTag >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:1136:8
	makeExtern<DAS_CALL_METHOD(_method_636), SimNode_ExtFuncCall >(lib,"HasStructuredSerializeFromMismatchedTag","das_call_member< bool (UScriptStruct::ICppStructOps::*)() const , &UScriptStruct::ICppStructOps::HasStructuredSerializeFromMismatchedTag >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_637 = das::das_call_member< bool (UScriptStruct::ICppStructOps::*)() const,&UScriptStruct::ICppStructOps::HasGetTypeHash >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:1149:8
	makeExtern<DAS_CALL_METHOD(_method_637), SimNode_ExtFuncCall >(lib,"HasGetTypeHash","das_call_member< bool (UScriptStruct::ICppStructOps::*)() const , &UScriptStruct::ICppStructOps::HasGetTypeHash >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_638 = das::das_call_member< bool (UScriptStruct::ICppStructOps::*)() const,&UScriptStruct::ICppStructOps::IsAbstract >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:1164:8
	makeExtern<DAS_CALL_METHOD(_method_638), SimNode_ExtFuncCall >(lib,"IsAbstract","das_call_member< bool (UScriptStruct::ICppStructOps::*)() const , &UScriptStruct::ICppStructOps::IsAbstract >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_639 = das::das_call_member< bool (UScriptStruct::ICppStructOps::*)() const,&UScriptStruct::ICppStructOps::HasCanEditChange >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:1171:8
	makeExtern<DAS_CALL_METHOD(_method_639), SimNode_ExtFuncCall >(lib,"HasCanEditChange","das_call_member< bool (UScriptStruct::ICppStructOps::*)() const , &UScriptStruct::ICppStructOps::HasCanEditChange >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
}
}

