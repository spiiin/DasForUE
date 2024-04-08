// this file is generated via daScript automatic C++ binder
// all user modifications will be lost after this file is re-generated

#include "dasUnreal.h"
#include "need_dasUnreal.h"
namespace das {
#include "dasUnreal.func.aot.decl.inc"
void Module_dasUnreal::initFunctions_9() {
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:157:2
	makeExtern< const wchar_t * (*)(const UField *) , _dasUnreal_static_217_StaticPackage , SimNode_ExtFuncCall >(lib,"StaticPackage","_dasUnreal_static_217_StaticPackage")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const UField*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:157:2
	makeExtern< void (*)(const UField *) , _dasUnreal_static_218_StaticRegisterNativesUField , SimNode_ExtFuncCall >(lib,"StaticRegisterNativesUField","_dasUnreal_static_218_StaticRegisterNativesUField")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const UField*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:169:31
	makeExtern< void (*)(UField &,FArchive &) , _dasUnreal_virtual_219_Serialize , SimNode_ExtFuncCall >(lib,"Serialize","_dasUnreal_virtual_219_Serialize")
		->args({"self","Ar"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:170:31
	makeExtern< void (*)(UField &) , _dasUnreal_virtual_220_PostLoad , SimNode_ExtFuncCall >(lib,"PostLoad","_dasUnreal_virtual_220_PostLoad")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:171:31
	makeExtern< bool (*)(const UField &) , _dasUnreal_virtual_221_NeedsLoadForClient , SimNode_ExtFuncCall >(lib,"NeedsLoadForClient","_dasUnreal_virtual_221_NeedsLoadForClient")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:172:31
	makeExtern< bool (*)(const UField &) , _dasUnreal_virtual_222_NeedsLoadForServer , SimNode_ExtFuncCall >(lib,"NeedsLoadForServer","_dasUnreal_virtual_222_NeedsLoadForServer")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:176:31
	makeExtern< void (*)(UField &) , _dasUnreal_virtual_223_Bind , SimNode_ExtFuncCall >(lib,"Bind","_dasUnreal_virtual_223_Bind")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_625 = das::das_call_member< UClass * (UField::*)() const,&UField::GetOwnerClass >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:179:26
	makeExtern<DAS_CALL_METHOD(_method_625), SimNode_ExtFuncCall >(lib,"GetOwnerClass","das_call_member< UClass * (UField::*)() const , &UField::GetOwnerClass >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_626 = das::das_call_member< UStruct * (UField::*)() const,&UField::GetOwnerStruct >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:182:27
	makeExtern<DAS_CALL_METHOD(_method_626), SimNode_ExtFuncCall >(lib,"GetOwnerStruct","das_call_member< UStruct * (UField::*)() const , &UField::GetOwnerStruct >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_627 = das::das_call_member< FString (UField::*)() const,&UField::GetAuthoredName >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:189:26
	makeExtern<DAS_CALL_METHOD(_method_627), SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetAuthoredName","das_call_member< FString (UField::*)() const , &UField::GetAuthoredName >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_628 = das::das_call_member< FText (UField::*)() const,&UField::GetDisplayNameText >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:197:24
	makeExtern<DAS_CALL_METHOD(_method_628), SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetDisplayNameText","das_call_member< FText (UField::*)() const , &UField::GetDisplayNameText >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_629 = das::das_call_member< FText (UField::*)(bool) const,&UField::GetToolTipText >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:206:24
	makeExtern<DAS_CALL_METHOD(_method_629), SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetToolTipText","das_call_member< FText (UField::*)(bool) const , &UField::GetToolTipText >::invoke")
		->args({"self","bShortTooltip"})
		->arg_init(1,make_smart<ExprConstBool>(false))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:214:30
	makeExtern< void (*)(const UField *,FString &,bool) , _dasUnreal_static_224_FormatNativeToolTip , SimNode_ExtFuncCall >(lib,"FormatNativeToolTip","_dasUnreal_static_224_FormatNativeToolTip")
		->args({"self","ToolTipString","bRemoveExtraSections"})
		->arg_type(0,makeType<TExplicit<const UField*> >(lib))
		->arg_init(2,make_smart<ExprConstBool>(true))
		->addToModule(*this, SideEffects::worstDefault);
	using _method_631 = das::das_call_member< bool (UField::*)(const wchar_t *) const,&UField::HasMetaData >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:222:7
	makeExtern<DAS_CALL_METHOD(_method_631), SimNode_ExtFuncCall >(lib,"HasMetaData","das_call_member< bool (UField::*)(const wchar_t *) const , &UField::HasMetaData >::invoke")
		->args({"self","Key"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_632 = das::das_call_member< bool (UField::*)(const FName &) const,&UField::HasMetaData >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:223:7
	makeExtern<DAS_CALL_METHOD(_method_632), SimNode_ExtFuncCall >(lib,"HasMetaData","das_call_member< bool (UField::*)(const FName &) const , &UField::HasMetaData >::invoke")
		->args({"self","Key"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_633 = das::das_call_member< const FString * (UField::*)(const wchar_t *) const,&UField::FindMetaData >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:231:33
	makeExtern<DAS_CALL_METHOD(_method_633), SimNode_ExtFuncCall >(lib,"FindMetaData","das_call_member< const FString * (UField::*)(const wchar_t *) const , &UField::FindMetaData >::invoke")
		->args({"self","Key"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_634 = das::das_call_member< const FString * (UField::*)(const FName &) const,&UField::FindMetaData >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:232:33
	makeExtern<DAS_CALL_METHOD(_method_634), SimNode_ExtFuncCall >(lib,"FindMetaData","das_call_member< const FString * (UField::*)(const FName &) const , &UField::FindMetaData >::invoke")
		->args({"self","Key"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_635 = das::das_call_member< const FString & (UField::*)(const wchar_t *) const,&UField::GetMetaData >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:240:33
	makeExtern<DAS_CALL_METHOD(_method_635), SimNode_ExtFuncCallRef >(lib,"GetMetaData","das_call_member< const FString & (UField::*)(const wchar_t *) const , &UField::GetMetaData >::invoke")
		->args({"self","Key"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_636 = das::das_call_member< const FString & (UField::*)(const FName &) const,&UField::GetMetaData >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:241:33
	makeExtern<DAS_CALL_METHOD(_method_636), SimNode_ExtFuncCallRef >(lib,"GetMetaData","das_call_member< const FString & (UField::*)(const FName &) const , &UField::GetMetaData >::invoke")
		->args({"self","Key"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_637 = das::das_call_member< FText (UField::*)(const wchar_t *,const FString,const FString) const,&UField::GetMetaDataText >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:251:24
	makeExtern<DAS_CALL_METHOD(_method_637), SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetMetaDataText","das_call_member< FText (UField::*)(const wchar_t *,const FString,const FString) const , &UField::GetMetaDataText >::invoke")
		->args({"self","MetaDataKey","LocalizationNamespace","LocalizationKey"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_638 = das::das_call_member< FText (UField::*)(const FName &,const FString,const FString) const,&UField::GetMetaDataText >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:252:24
	makeExtern<DAS_CALL_METHOD(_method_638), SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetMetaDataText","das_call_member< FText (UField::*)(const FName &,const FString,const FString) const , &UField::GetMetaDataText >::invoke")
		->args({"self","MetaDataKey","LocalizationNamespace","LocalizationKey"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_639 = das::das_call_member< void (UField::*)(const wchar_t *,const wchar_t *),&UField::SetMetaData >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:260:23
	makeExtern<DAS_CALL_METHOD(_method_639), SimNode_ExtFuncCall >(lib,"SetMetaData","das_call_member< void (UField::*)(const wchar_t *,const wchar_t *) , &UField::SetMetaData >::invoke")
		->args({"self","Key","InValue"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_640 = das::das_call_member< void (UField::*)(const FName &,const wchar_t *),&UField::SetMetaData >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:261:23
	makeExtern<DAS_CALL_METHOD(_method_640), SimNode_ExtFuncCall >(lib,"SetMetaData","das_call_member< void (UField::*)(const FName &,const wchar_t *) , &UField::SetMetaData >::invoke")
		->args({"self","Key","InValue"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_641 = das::das_call_member< bool (UField::*)(const wchar_t *) const,&UField::GetBoolMetaData >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:269:7
	makeExtern<DAS_CALL_METHOD(_method_641), SimNode_ExtFuncCall >(lib,"GetBoolMetaData","das_call_member< bool (UField::*)(const wchar_t *) const , &UField::GetBoolMetaData >::invoke")
		->args({"self","Key"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_642 = das::das_call_member< bool (UField::*)(const FName &) const,&UField::GetBoolMetaData >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:275:7
	makeExtern<DAS_CALL_METHOD(_method_642), SimNode_ExtFuncCall >(lib,"GetBoolMetaData","das_call_member< bool (UField::*)(const FName &) const , &UField::GetBoolMetaData >::invoke")
		->args({"self","Key"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_643 = das::das_call_member< int (UField::*)(const wchar_t *) const,&UField::GetIntMetaData >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:288:8
	makeExtern<DAS_CALL_METHOD(_method_643), SimNode_ExtFuncCall >(lib,"GetIntMetaData","das_call_member< int (UField::*)(const wchar_t *) const , &UField::GetIntMetaData >::invoke")
		->args({"self","Key"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_644 = das::das_call_member< int (UField::*)(const FName &) const,&UField::GetIntMetaData >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:294:8
	makeExtern<DAS_CALL_METHOD(_method_644), SimNode_ExtFuncCall >(lib,"GetIntMetaData","das_call_member< int (UField::*)(const FName &) const , &UField::GetIntMetaData >::invoke")
		->args({"self","Key"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_645 = das::das_call_member< float (UField::*)(const wchar_t *) const,&UField::GetFloatMetaData >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:307:8
	makeExtern<DAS_CALL_METHOD(_method_645), SimNode_ExtFuncCall >(lib,"GetFloatMetaData","das_call_member< float (UField::*)(const wchar_t *) const , &UField::GetFloatMetaData >::invoke")
		->args({"self","Key"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_646 = das::das_call_member< float (UField::*)(const FName &) const,&UField::GetFloatMetaData >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:313:8
	makeExtern<DAS_CALL_METHOD(_method_646), SimNode_ExtFuncCall >(lib,"GetFloatMetaData","das_call_member< float (UField::*)(const FName &) const , &UField::GetFloatMetaData >::invoke")
		->args({"self","Key"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_647 = das::das_call_member< UClass * (UField::*)(const wchar_t *) const,&UField::GetClassMetaData >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:326:26
	makeExtern<DAS_CALL_METHOD(_method_647), SimNode_ExtFuncCall >(lib,"GetClassMetaData","das_call_member< UClass * (UField::*)(const wchar_t *) const , &UField::GetClassMetaData >::invoke")
		->args({"self","Key"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_648 = das::das_call_member< UClass * (UField::*)(const FName &) const,&UField::GetClassMetaData >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:327:26
	makeExtern<DAS_CALL_METHOD(_method_648), SimNode_ExtFuncCall >(lib,"GetClassMetaData","das_call_member< UClass * (UField::*)(const FName &) const , &UField::GetClassMetaData >::invoke")
		->args({"self","Key"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_649 = das::das_call_member< void (UField::*)(const wchar_t *),&UField::RemoveMetaData >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:330:23
	makeExtern<DAS_CALL_METHOD(_method_649), SimNode_ExtFuncCall >(lib,"RemoveMetaData","das_call_member< void (UField::*)(const wchar_t *) , &UField::RemoveMetaData >::invoke")
		->args({"self","Key"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_650 = das::das_call_member< void (UField::*)(const FName &),&UField::RemoveMetaData >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:331:23
	makeExtern<DAS_CALL_METHOD(_method_650), SimNode_ExtFuncCall >(lib,"RemoveMetaData","das_call_member< void (UField::*)(const FName &) , &UField::RemoveMetaData >::invoke")
		->args({"self","Key"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_651 = das::das_call_member< bool (UField::*)(const uint64) const,&UField::HasAnyCastFlags >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:334:23
	makeExtern<DAS_CALL_METHOD(_method_651), SimNode_ExtFuncCall >(lib,"HasAnyCastFlags","das_call_member< bool (UField::*)(const uint64) const , &UField::HasAnyCastFlags >::invoke")
		->args({"self","InCastFlags"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_652 = das::das_call_member< bool (UField::*)(const uint64) const,&UField::HasAllCastFlags >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:335:23
	makeExtern<DAS_CALL_METHOD(_method_652), SimNode_ExtFuncCall >(lib,"HasAllCastFlags","das_call_member< bool (UField::*)(const uint64) const , &UField::HasAllCastFlags >::invoke")
		->args({"self","InCastFlags"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:341:34
	makeExtern< FField * (*)(UField &) , _dasUnreal_virtual_225_GetAssociatedFField , SimNode_ExtFuncCall >(lib,"GetAssociatedFField","_dasUnreal_virtual_225_GetAssociatedFField")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:345:31
	makeExtern< void (*)(UField &,FField *) , _dasUnreal_virtual_226_SetAssociatedFField , SimNode_ExtFuncCall >(lib,"SetAssociatedFField","_dasUnreal_virtual_226_SetAssociatedFField")
		->args({"self","InField"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:388:2
	makeExtern< UClass * (*)(const UStruct *) , _dasUnreal_static_227_StaticClass , SimNode_ExtFuncCall >(lib,"StaticClass","_dasUnreal_static_227_StaticClass")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const UStruct*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:388:2
	makeExtern< const wchar_t * (*)(const UStruct *) , _dasUnreal_static_228_StaticPackage , SimNode_ExtFuncCall >(lib,"StaticPackage","_dasUnreal_static_228_StaticPackage")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const UStruct*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:388:2
	makeExtern< void (*)(const UStruct *) , _dasUnreal_static_229_StaticRegisterNativesUStruct , SimNode_ExtFuncCall >(lib,"StaticRegisterNativesUStruct","_dasUnreal_static_229_StaticRegisterNativesUStruct")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const UStruct*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
	using _method_658 = das::das_call_member< const FBlake3Hash & (UStruct::*)(bool) const,&UStruct::GetSchemaHash >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:441:37
	makeExtern<DAS_CALL_METHOD(_method_658), SimNode_ExtFuncCallRef >(lib,"GetSchemaHash","das_call_member< const FBlake3Hash & (UStruct::*)(bool) const , &UStruct::GetSchemaHash >::invoke")
		->args({"self","bSkipEditorOnly"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:456:31
	makeExtern< void (*)(UStruct &,FArchive &) , _dasUnreal_virtual_230_Serialize , SimNode_ExtFuncCall >(lib,"Serialize","_dasUnreal_virtual_230_Serialize")
		->args({"self","Ar"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:458:31
	makeExtern< void (*)(UStruct &) , _dasUnreal_virtual_231_PostLoad , SimNode_ExtFuncCall >(lib,"PostLoad","_dasUnreal_virtual_231_PostLoad")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:459:31
	makeExtern< void (*)(UStruct &) , _dasUnreal_virtual_232_FinishDestroy , SimNode_ExtFuncCall >(lib,"FinishDestroy","_dasUnreal_virtual_232_FinishDestroy")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:460:31
	makeExtern< void (*)(UStruct &) , _dasUnreal_virtual_233_RegisterDependencies , SimNode_ExtFuncCall >(lib,"RegisterDependencies","_dasUnreal_virtual_233_RegisterDependencies")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:461:30
	makeExtern< void (*)(const UStruct *,UObject *,FReferenceCollector &) , _dasUnreal_static_234_AddReferencedObjects , SimNode_ExtFuncCall >(lib,"AddReferencedObjects","_dasUnreal_static_234_AddReferencedObjects")
		->args({"self","InThis","Collector"})
		->arg_type(0,makeType<TExplicit<const UStruct*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:463:31
	makeExtern< void (*)(UStruct &,EObjectFlags) , _dasUnreal_virtual_235_TagSubobjects , SimNode_ExtFuncCall >(lib,"TagSubobjects","_dasUnreal_virtual_235_TagSubobjects")
		->args({"self","NewFlags"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:466:31
	makeExtern< void (*)(UStruct &,FProperty *) , _dasUnreal_virtual_236_AddCppProperty , SimNode_ExtFuncCall >(lib,"AddCppProperty","_dasUnreal_virtual_236_AddCppProperty")
		->args({"self","Property"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_666 = das::das_call_member< FProperty * (UStruct::*)(FName) const,&UStruct::FindPropertyByName >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:474:29
	makeExtern<DAS_CALL_METHOD(_method_666), SimNode_ExtFuncCall >(lib,"FindPropertyByName","das_call_member< FProperty * (UStruct::*)(FName) const , &UStruct::FindPropertyByName >::invoke")
		->args({"self","InName"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:488:19
	makeExtern< UStruct * (*)(const UStruct &) , _dasUnreal_virtual_237_GetInheritanceSuper , SimNode_ExtFuncCall >(lib,"GetInheritanceSuper","_dasUnreal_virtual_237_GetInheritanceSuper")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_668 = das::das_call_member< void (UStruct::*)(bool),&UStruct::StaticLink >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:491:23
	makeExtern<DAS_CALL_METHOD(_method_668), SimNode_ExtFuncCall >(lib,"StaticLink","das_call_member< void (UStruct::*)(bool) , &UStruct::StaticLink >::invoke")
		->args({"self","bRelinkExistingProperties"})
		->arg_init(1,make_smart<ExprConstBool>(false))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:494:31
	makeExtern< void (*)(UStruct &,FArchive &,bool) , _dasUnreal_virtual_238_Link , SimNode_ExtFuncCall >(lib,"Link","_dasUnreal_virtual_238_Link")
		->args({"self","Ar","bRelinkExistingProperties"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:507:15
	makeExtern< void (*)(const UStruct &,FArchive &,void *) , _dasUnreal_virtual_239_SerializeBin , SimNode_ExtFuncCall >(lib,"SerializeBin","_dasUnreal_virtual_239_SerializeBin")
		->args({"self","Ar","Data"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:562:31
	makeExtern< void (*)(const UStruct &,void *,int) , _dasUnreal_virtual_240_InitializeStruct , SimNode_ExtFuncCall >(lib,"InitializeStruct","_dasUnreal_virtual_240_InitializeStruct")
		->args({"self","Dest","ArrayDim"})
		->arg_init(2,make_smart<ExprConstInt>(1))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:571:31
	makeExtern< void (*)(const UStruct &,void *,int) , _dasUnreal_virtual_241_DestroyStruct , SimNode_ExtFuncCall >(lib,"DestroyStruct","_dasUnreal_virtual_241_DestroyStruct")
		->args({"self","Dest","ArrayDim"})
		->arg_init(2,make_smart<ExprConstInt>(1))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:574:21
	makeExtern< FProperty * (*)(const UStruct &,const FName) , _dasUnreal_virtual_242_CustomFindProperty , SimNode_ExtFuncCall >(lib,"CustomFindProperty","_dasUnreal_virtual_242_CustomFindProperty")
		->args({"self","InName"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:584:23
	makeExtern< const wchar_t * (*)(const UStruct &) , _dasUnreal_virtual_243_GetPrefixCPP , SimNode_ExtFuncCall >(lib,"GetPrefixCPP","_dasUnreal_virtual_243_GetPrefixCPP")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_675 = das::das_call_member< int (UStruct::*)() const,&UStruct::GetPropertiesSize >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:587:20
	makeExtern<DAS_CALL_METHOD(_method_675), SimNode_ExtFuncCall >(lib,"GetPropertiesSize","das_call_member< int (UStruct::*)() const , &UStruct::GetPropertiesSize >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_676 = das::das_call_member< int (UStruct::*)() const,&UStruct::GetMinAlignment >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:593:20
	makeExtern<DAS_CALL_METHOD(_method_676), SimNode_ExtFuncCall >(lib,"GetMinAlignment","das_call_member< int (UStruct::*)() const , &UStruct::GetMinAlignment >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_677 = das::das_call_member< int (UStruct::*)() const,&UStruct::GetStructureSize >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:599:20
	makeExtern<DAS_CALL_METHOD(_method_677), SimNode_ExtFuncCall >(lib,"GetStructureSize","das_call_member< int (UStruct::*)() const , &UStruct::GetStructureSize >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_678 = das::das_call_member< void (UStruct::*)(int),&UStruct::SetPropertiesSize >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:605:7
	makeExtern<DAS_CALL_METHOD(_method_678), SimNode_ExtFuncCall >(lib,"SetPropertiesSize","das_call_member< void (UStruct::*)(int) , &UStruct::SetPropertiesSize >::invoke")
		->args({"self","NewSize"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_679 = das::das_call_member< bool (UStruct::*)(const UStruct *) const,&UStruct::IsChildOf >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:619:23
	makeExtern<DAS_CALL_METHOD(_method_679), SimNode_ExtFuncCall >(lib,"IsChildOf","das_call_member< bool (UStruct::*)(const UStruct *) const , &UStruct::IsChildOf >::invoke")
		->args({"self","SomeBase"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_680 = das::das_call_member< UStruct * (UStruct::*)() const,&UStruct::GetSuperStruct >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:628:11
	makeExtern<DAS_CALL_METHOD(_method_680), SimNode_ExtFuncCall >(lib,"GetSuperStruct","das_call_member< UStruct * (UStruct::*)() const , &UStruct::GetSuperStruct >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:637:31
	makeExtern< void (*)(UStruct &,UStruct *) , _dasUnreal_virtual_244_SetSuperStruct , SimNode_ExtFuncCall >(lib,"SetSuperStruct","_dasUnreal_virtual_244_SetSuperStruct")
		->args({"self","NewSuperStruct"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:640:34
	makeExtern< FString (*)(const UStruct &,const UField *) , _dasUnreal_virtual_245_GetAuthoredNameForField , SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetAuthoredNameForField","_dasUnreal_virtual_245_GetAuthoredNameForField")
		->args({"self","Field"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:643:34
	makeExtern< FString (*)(const UStruct &,const FField *) , _dasUnreal_virtual_246_GetAuthoredNameForField , SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetAuthoredNameForField","_dasUnreal_virtual_246_GetAuthoredNameForField")
		->args({"self","Field"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:646:15
	makeExtern< bool (*)(const UStruct &) , _dasUnreal_virtual_247_IsStructTrashed , SimNode_ExtFuncCall >(lib,"IsStructTrashed","_dasUnreal_virtual_247_IsStructTrashed")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_685 = das::das_call_member< void (UStruct::*)(),&UStruct::DestroyChildPropertiesAndResetPropertyLinks >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:652:23
	makeExtern<DAS_CALL_METHOD(_method_685), SimNode_ExtFuncCall >(lib,"DestroyChildPropertiesAndResetPropertyLinks","das_call_member< void (UStruct::*)() , &UStruct::DestroyChildPropertiesAndResetPropertyLinks >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_686 = das::das_call_member< bool (UStruct::*)(const FName &) const,&UStruct::GetBoolMetaDataHierarchical >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:656:23
	makeExtern<DAS_CALL_METHOD(_method_686), SimNode_ExtFuncCall >(lib,"GetBoolMetaDataHierarchical","das_call_member< bool (UStruct::*)(const FName &) const , &UStruct::GetBoolMetaDataHierarchical >::invoke")
		->args({"self","Key"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_687 = das::das_call_member< bool (UStruct::*)(const FName &,FString *) const,&UStruct::GetStringMetaDataHierarchical >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:659:23
	makeExtern<DAS_CALL_METHOD(_method_687), SimNode_ExtFuncCall >(lib,"GetStringMetaDataHierarchical","das_call_member< bool (UStruct::*)(const FName &,FString *) const , &UStruct::GetStringMetaDataHierarchical >::invoke")
		->args({"self","Key","OutValue"})
		->arg_init(2,make_smart<ExprConstPtr>())
		->addToModule(*this, SideEffects::worstDefault);
	using _method_688 = das::das_call_member< const UStruct * (UStruct::*)(const FName &) const,&UStruct::HasMetaDataHierarchical >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:667:33
	makeExtern<DAS_CALL_METHOD(_method_688), SimNode_ExtFuncCall >(lib,"HasMetaDataHierarchical","das_call_member< const UStruct * (UStruct::*)(const FName &) const , &UStruct::HasMetaDataHierarchical >::invoke")
		->args({"self","Key"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_689 = das::das_call_member< bool (UStruct::*)() const,&UStruct::HasAssetRegistrySearchableProperties >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:670:19
	makeExtern<DAS_CALL_METHOD(_method_689), SimNode_ExtFuncCall >(lib,"HasAssetRegistrySearchableProperties","das_call_member< bool (UStruct::*)() const , &UStruct::HasAssetRegistrySearchableProperties >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_690 = das::das_call_member< void (UStruct::*)(),&UStruct::DeleteUnresolvedScriptProperties >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:687:19
	makeExtern<DAS_CALL_METHOD(_method_690), SimNode_ExtFuncCall >(lib,"DeleteUnresolvedScriptProperties","das_call_member< void (UStruct::*)() , &UStruct::DeleteUnresolvedScriptProperties >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_691 = das::das_call_member< void (UStruct::*)(),&UStruct::CollectBytecodeAndPropertyReferencedObjects >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:709:23
	makeExtern<DAS_CALL_METHOD(_method_691), SimNode_ExtFuncCall >(lib,"CollectBytecodeAndPropertyReferencedObjects","das_call_member< void (UStruct::*)() , &UStruct::CollectBytecodeAndPropertyReferencedObjects >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_692 = das::das_call_member< void (UStruct::*)(),&UStruct::CollectBytecodeAndPropertyReferencedObjectsRecursively >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:713:23
	makeExtern<DAS_CALL_METHOD(_method_692), SimNode_ExtFuncCall >(lib,"CollectBytecodeAndPropertyReferencedObjectsRecursively","das_call_member< void (UStruct::*)() , &UStruct::CollectBytecodeAndPropertyReferencedObjectsRecursively >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_693 = das::das_call_member< bool (UScriptStruct::ICppStructOps::*)() const,&UScriptStruct::ICppStructOps::HasNoopConstructor >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:951:8
	makeExtern<DAS_CALL_METHOD(_method_693), SimNode_ExtFuncCall >(lib,"HasNoopConstructor","das_call_member< bool (UScriptStruct::ICppStructOps::*)() const , &UScriptStruct::ICppStructOps::HasNoopConstructor >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_694 = das::das_call_member< bool (UScriptStruct::ICppStructOps::*)() const,&UScriptStruct::ICppStructOps::HasZeroConstructor >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:956:8
	makeExtern<DAS_CALL_METHOD(_method_694), SimNode_ExtFuncCall >(lib,"HasZeroConstructor","das_call_member< bool (UScriptStruct::ICppStructOps::*)() const , &UScriptStruct::ICppStructOps::HasZeroConstructor >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_695 = das::das_call_member< bool (UScriptStruct::ICppStructOps::*)() const,&UScriptStruct::ICppStructOps::HasDestructor >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:965:8
	makeExtern<DAS_CALL_METHOD(_method_695), SimNode_ExtFuncCall >(lib,"HasDestructor","das_call_member< bool (UScriptStruct::ICppStructOps::*)() const , &UScriptStruct::ICppStructOps::HasDestructor >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_696 = das::das_call_member< int (UScriptStruct::ICppStructOps::*)() const,&UScriptStruct::ICppStructOps::GetSize >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:972:21
	makeExtern<DAS_CALL_METHOD(_method_696), SimNode_ExtFuncCall >(lib,"GetSize","das_call_member< int (UScriptStruct::ICppStructOps::*)() const , &UScriptStruct::ICppStructOps::GetSize >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_697 = das::das_call_member< int (UScriptStruct::ICppStructOps::*)() const,&UScriptStruct::ICppStructOps::GetAlignment >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:977:21
	makeExtern<DAS_CALL_METHOD(_method_697), SimNode_ExtFuncCall >(lib,"GetAlignment","das_call_member< int (UScriptStruct::ICppStructOps::*)() const , &UScriptStruct::ICppStructOps::GetAlignment >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_698 = das::das_call_member< bool (UScriptStruct::ICppStructOps::*)() const,&UScriptStruct::ICppStructOps::HasSerializer >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:983:8
	makeExtern<DAS_CALL_METHOD(_method_698), SimNode_ExtFuncCall >(lib,"HasSerializer","das_call_member< bool (UScriptStruct::ICppStructOps::*)() const , &UScriptStruct::ICppStructOps::HasSerializer >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_699 = das::das_call_member< bool (UScriptStruct::ICppStructOps::*)() const,&UScriptStruct::ICppStructOps::HasStructuredSerializer >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:988:8
	makeExtern<DAS_CALL_METHOD(_method_699), SimNode_ExtFuncCall >(lib,"HasStructuredSerializer","das_call_member< bool (UScriptStruct::ICppStructOps::*)() const , &UScriptStruct::ICppStructOps::HasStructuredSerializer >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_700 = das::das_call_member< bool (UScriptStruct::ICppStructOps::*)() const,&UScriptStruct::ICppStructOps::HasPostSerialize >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:1000:8
	makeExtern<DAS_CALL_METHOD(_method_700), SimNode_ExtFuncCall >(lib,"HasPostSerialize","das_call_member< bool (UScriptStruct::ICppStructOps::*)() const , &UScriptStruct::ICppStructOps::HasPostSerialize >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_701 = das::das_call_member< bool (UScriptStruct::ICppStructOps::*)() const,&UScriptStruct::ICppStructOps::HasNetSerializer >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:1008:8
	makeExtern<DAS_CALL_METHOD(_method_701), SimNode_ExtFuncCall >(lib,"HasNetSerializer","das_call_member< bool (UScriptStruct::ICppStructOps::*)() const , &UScriptStruct::ICppStructOps::HasNetSerializer >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_702 = das::das_call_member< bool (UScriptStruct::ICppStructOps::*)() const,&UScriptStruct::ICppStructOps::HasNetSharedSerialization >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:1014:8
	makeExtern<DAS_CALL_METHOD(_method_702), SimNode_ExtFuncCall >(lib,"HasNetSharedSerialization","das_call_member< bool (UScriptStruct::ICppStructOps::*)() const , &UScriptStruct::ICppStructOps::HasNetSharedSerialization >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_703 = das::das_call_member< bool (UScriptStruct::ICppStructOps::*)() const,&UScriptStruct::ICppStructOps::HasNetDeltaSerializer >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:1025:8
	makeExtern<DAS_CALL_METHOD(_method_703), SimNode_ExtFuncCall >(lib,"HasNetDeltaSerializer","das_call_member< bool (UScriptStruct::ICppStructOps::*)() const , &UScriptStruct::ICppStructOps::HasNetDeltaSerializer >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_704 = das::das_call_member< bool (UScriptStruct::ICppStructOps::*)() const,&UScriptStruct::ICppStructOps::HasPostScriptConstruct >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:1036:8
	makeExtern<DAS_CALL_METHOD(_method_704), SimNode_ExtFuncCall >(lib,"HasPostScriptConstruct","das_call_member< bool (UScriptStruct::ICppStructOps::*)() const , &UScriptStruct::ICppStructOps::HasPostScriptConstruct >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_705 = das::das_call_member< bool (UScriptStruct::ICppStructOps::*)() const,&UScriptStruct::ICppStructOps::IsPlainOldData >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:1047:8
	makeExtern<DAS_CALL_METHOD(_method_705), SimNode_ExtFuncCall >(lib,"IsPlainOldData","das_call_member< bool (UScriptStruct::ICppStructOps::*)() const , &UScriptStruct::ICppStructOps::IsPlainOldData >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_706 = das::das_call_member< bool (UScriptStruct::ICppStructOps::*)() const,&UScriptStruct::ICppStructOps::IsUECoreType >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:1053:8
	makeExtern<DAS_CALL_METHOD(_method_706), SimNode_ExtFuncCall >(lib,"IsUECoreType","das_call_member< bool (UScriptStruct::ICppStructOps::*)() const , &UScriptStruct::ICppStructOps::IsUECoreType >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_707 = das::das_call_member< bool (UScriptStruct::ICppStructOps::*)() const,&UScriptStruct::ICppStructOps::IsUECoreVariant >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:1059:8
	makeExtern<DAS_CALL_METHOD(_method_707), SimNode_ExtFuncCall >(lib,"IsUECoreVariant","das_call_member< bool (UScriptStruct::ICppStructOps::*)() const , &UScriptStruct::ICppStructOps::IsUECoreVariant >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_708 = das::das_call_member< bool (UScriptStruct::ICppStructOps::*)() const,&UScriptStruct::ICppStructOps::HasCopy >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:1065:8
	makeExtern<DAS_CALL_METHOD(_method_708), SimNode_ExtFuncCall >(lib,"HasCopy","das_call_member< bool (UScriptStruct::ICppStructOps::*)() const , &UScriptStruct::ICppStructOps::HasCopy >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_709 = das::das_call_member< bool (UScriptStruct::ICppStructOps::*)() const,&UScriptStruct::ICppStructOps::HasIdentical >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:1076:8
	makeExtern<DAS_CALL_METHOD(_method_709), SimNode_ExtFuncCall >(lib,"HasIdentical","das_call_member< bool (UScriptStruct::ICppStructOps::*)() const , &UScriptStruct::ICppStructOps::HasIdentical >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_710 = das::das_call_member< bool (UScriptStruct::ICppStructOps::*)() const,&UScriptStruct::ICppStructOps::HasExportTextItem >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:1087:8
	makeExtern<DAS_CALL_METHOD(_method_710), SimNode_ExtFuncCall >(lib,"HasExportTextItem","das_call_member< bool (UScriptStruct::ICppStructOps::*)() const , &UScriptStruct::ICppStructOps::HasExportTextItem >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_711 = das::das_call_member< bool (UScriptStruct::ICppStructOps::*)() const,&UScriptStruct::ICppStructOps::HasImportTextItem >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:1098:8
	makeExtern<DAS_CALL_METHOD(_method_711), SimNode_ExtFuncCall >(lib,"HasImportTextItem","das_call_member< bool (UScriptStruct::ICppStructOps::*)() const , &UScriptStruct::ICppStructOps::HasImportTextItem >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_712 = das::das_call_member< bool (UScriptStruct::ICppStructOps::*)() const,&UScriptStruct::ICppStructOps::HasFindInnerPropertyInstance >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:1108:8
	makeExtern<DAS_CALL_METHOD(_method_712), SimNode_ExtFuncCall >(lib,"HasFindInnerPropertyInstance","das_call_member< bool (UScriptStruct::ICppStructOps::*)() const , &UScriptStruct::ICppStructOps::HasFindInnerPropertyInstance >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_713 = das::das_call_member< bool (UScriptStruct::ICppStructOps::*)() const,&UScriptStruct::ICppStructOps::HasAddStructReferencedObjects >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:1115:8
	makeExtern<DAS_CALL_METHOD(_method_713), SimNode_ExtFuncCall >(lib,"HasAddStructReferencedObjects","das_call_member< bool (UScriptStruct::ICppStructOps::*)() const , &UScriptStruct::ICppStructOps::HasAddStructReferencedObjects >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_714 = das::das_call_member< bool (UScriptStruct::ICppStructOps::*)(EPropertyObjectReferenceType) const,&UScriptStruct::ICppStructOps::HasSerializerObjectReferences >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:1120:8
	makeExtern<DAS_CALL_METHOD(_method_714), SimNode_ExtFuncCall >(lib,"HasSerializerObjectReferences","das_call_member< bool (UScriptStruct::ICppStructOps::*)(EPropertyObjectReferenceType) const , &UScriptStruct::ICppStructOps::HasSerializerObjectReferences >::invoke")
		->args({"self","Type"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_715 = das::das_call_member< bool (UScriptStruct::ICppStructOps::*)() const,&UScriptStruct::ICppStructOps::HasSerializeFromMismatchedTag >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:1132:8
	makeExtern<DAS_CALL_METHOD(_method_715), SimNode_ExtFuncCall >(lib,"HasSerializeFromMismatchedTag","das_call_member< bool (UScriptStruct::ICppStructOps::*)() const , &UScriptStruct::ICppStructOps::HasSerializeFromMismatchedTag >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_716 = das::das_call_member< bool (UScriptStruct::ICppStructOps::*)() const,&UScriptStruct::ICppStructOps::HasStructuredSerializeFromMismatchedTag >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:1136:8
	makeExtern<DAS_CALL_METHOD(_method_716), SimNode_ExtFuncCall >(lib,"HasStructuredSerializeFromMismatchedTag","das_call_member< bool (UScriptStruct::ICppStructOps::*)() const , &UScriptStruct::ICppStructOps::HasStructuredSerializeFromMismatchedTag >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_717 = das::das_call_member< bool (UScriptStruct::ICppStructOps::*)() const,&UScriptStruct::ICppStructOps::HasGetTypeHash >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:1149:8
	makeExtern<DAS_CALL_METHOD(_method_717), SimNode_ExtFuncCall >(lib,"HasGetTypeHash","das_call_member< bool (UScriptStruct::ICppStructOps::*)() const , &UScriptStruct::ICppStructOps::HasGetTypeHash >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
}
}

