// this file is generated via daScript automatic C++ binder
// all user modifications will be lost after this file is re-generated

#include "dasUnreal.h"
#include "need_dasUnreal.h"
namespace das {
#include "dasUnreal.func.aot.decl.inc"
void Module_dasUnreal::initFunctions_4() {
	using _method_166 = das::das_call_member< void (UObjectBaseUtility::*)(FString &) const,&UObjectBaseUtility::GetName >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBaseUtility.h:549:19
	makeExtern<DAS_CALL_METHOD(_method_166), SimNode_ExtFuncCall >(lib,"GetName","das_call_member< void (UObjectBaseUtility::*)(FString &) const , &UObjectBaseUtility::GetName >::invoke")
		->args({"self","ResultString"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_167 = das::das_call_member< void (UObjectBaseUtility::*)(FString &) const,&UObjectBaseUtility::AppendName >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBaseUtility.h:554:19
	makeExtern<DAS_CALL_METHOD(_method_167), SimNode_ExtFuncCall >(lib,"AppendName","das_call_member< void (UObjectBaseUtility::*)(FString &) const , &UObjectBaseUtility::AppendName >::invoke")
		->args({"self","ResultString"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_168 = das::das_call_member< bool (UObjectBaseUtility::*)() const,&UObjectBaseUtility::IsPackageExternal >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBaseUtility.h:568:23
	makeExtern<DAS_CALL_METHOD(_method_168), SimNode_ExtFuncCall >(lib,"IsPackageExternal","das_call_member< bool (UObjectBaseUtility::*)() const , &UObjectBaseUtility::IsPackageExternal >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_169 = das::das_call_member< void (UObjectBaseUtility::*)(),&UObjectBaseUtility::DetachExternalPackage >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBaseUtility.h:574:23
	makeExtern<DAS_CALL_METHOD(_method_169), SimNode_ExtFuncCall >(lib,"DetachExternalPackage","das_call_member< void (UObjectBaseUtility::*)() , &UObjectBaseUtility::DetachExternalPackage >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_170 = das::das_call_member< void (UObjectBaseUtility::*)(),&UObjectBaseUtility::ReattachExternalPackage >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBaseUtility.h:580:23
	makeExtern<DAS_CALL_METHOD(_method_170), SimNode_ExtFuncCall >(lib,"ReattachExternalPackage","das_call_member< void (UObjectBaseUtility::*)() , &UObjectBaseUtility::ReattachExternalPackage >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_171 = das::das_call_member< UObject * (UObjectBaseUtility::*)() const,&UObjectBaseUtility::GetOutermostObject >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBaseUtility.h:587:27
	makeExtern<DAS_CALL_METHOD(_method_171), SimNode_ExtFuncCall >(lib,"GetOutermostObject","das_call_member< UObject * (UObjectBaseUtility::*)() const , &UObjectBaseUtility::GetOutermostObject >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_172 = das::das_call_member< UPackage * (UObjectBaseUtility::*)() const,&UObjectBaseUtility::GetPackage >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBaseUtility.h:594:28
	makeExtern<DAS_CALL_METHOD(_method_172), SimNode_ExtFuncCall >(lib,"GetPackage","das_call_member< UPackage * (UObjectBaseUtility::*)() const , &UObjectBaseUtility::GetPackage >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_173 = das::das_call_member< UPackage * (UObjectBaseUtility::*)() const,&UObjectBaseUtility::GetOutermost >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBaseUtility.h:611:28
	makeExtern<DAS_CALL_METHOD(_method_173), SimNode_ExtFuncCall >(lib,"GetOutermost","das_call_member< UPackage * (UObjectBaseUtility::*)() const , &UObjectBaseUtility::GetOutermost >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_174 = das::das_call_member< bool (UObjectBaseUtility::*)() const,&UObjectBaseUtility::MarkPackageDirty >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBaseUtility.h:619:23
	makeExtern<DAS_CALL_METHOD(_method_174), SimNode_ExtFuncCall >(lib,"MarkPackageDirty","das_call_member< bool (UObjectBaseUtility::*)() const , &UObjectBaseUtility::MarkPackageDirty >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_175 = das::das_call_member< bool (UObjectBaseUtility::*)(EObjectFlags) const,&UObjectBaseUtility::IsTemplate >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBaseUtility.h:626:23
	makeExtern<DAS_CALL_METHOD(_method_175), SimNode_ExtFuncCall >(lib,"IsTemplate","das_call_member< bool (UObjectBaseUtility::*)(EObjectFlags) const , &UObjectBaseUtility::IsTemplate >::invoke")
		->args({"self","TemplateTypes"})
		->arg_init(1,make_smart<ExprConstEnumeration>(48,makeType<EObjectFlags>(lib)))
		->addToModule(*this, SideEffects::worstDefault);
	using _method_176 = das::das_call_member< UObject * (UObjectBaseUtility::*)(UClass *) const,&UObjectBaseUtility::GetTypedOuter >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBaseUtility.h:634:27
	makeExtern<DAS_CALL_METHOD(_method_176), SimNode_ExtFuncCall >(lib,"GetTypedOuter","das_call_member< UObject * (UObjectBaseUtility::*)(UClass *) const , &UObjectBaseUtility::GetTypedOuter >::invoke")
		->args({"self","Target"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_177 = das::das_call_member< UObjectBaseUtility * (UObjectBaseUtility::*)(const UClass *) const,&UObjectBaseUtility::GetImplementingOuterObject >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBaseUtility.h:669:38
	makeExtern<DAS_CALL_METHOD(_method_177), SimNode_ExtFuncCall >(lib,"GetImplementingOuterObject","das_call_member< UObjectBaseUtility * (UObjectBaseUtility::*)(const UClass *) const , &UObjectBaseUtility::GetImplementingOuterObject >::invoke")
		->args({"self","InInterfaceClass"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_178 = das::das_call_member< bool (UObjectBaseUtility::*)(const UObject *) const,&UObjectBaseUtility::IsIn >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBaseUtility.h:675:23
	makeExtern<DAS_CALL_METHOD(_method_178), SimNode_ExtFuncCall >(lib,"IsIn","das_call_member< bool (UObjectBaseUtility::*)(const UObject *) const , &UObjectBaseUtility::IsIn >::invoke")
		->args({"self","SomeOuter"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_179 = das::das_call_member< bool (UObjectBaseUtility::*)(const UPackage *) const,&UObjectBaseUtility::IsIn >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBaseUtility.h:681:23
	makeExtern<DAS_CALL_METHOD(_method_179), SimNode_ExtFuncCall >(lib,"IsIn","das_call_member< bool (UObjectBaseUtility::*)(const UPackage *) const , &UObjectBaseUtility::IsIn >::invoke")
		->args({"self","SomePackage"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_180 = das::das_call_member< bool (UObjectBaseUtility::*)(const UObject *) const,&UObjectBaseUtility::IsInOuter >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBaseUtility.h:684:23
	makeExtern<DAS_CALL_METHOD(_method_180), SimNode_ExtFuncCall >(lib,"IsInOuter","das_call_member< bool (UObjectBaseUtility::*)(const UObject *) const , &UObjectBaseUtility::IsInOuter >::invoke")
		->args({"self","SomeOuter"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_181 = das::das_call_member< bool (UObjectBaseUtility::*)(const UPackage *) const,&UObjectBaseUtility::IsInPackage >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBaseUtility.h:687:23
	makeExtern<DAS_CALL_METHOD(_method_181), SimNode_ExtFuncCall >(lib,"IsInPackage","das_call_member< bool (UObjectBaseUtility::*)(const UPackage *) const , &UObjectBaseUtility::IsInPackage >::invoke")
		->args({"self","SomePackage"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_182 = das::das_call_member< bool (UObjectBaseUtility::*)(const UClass *) const,&UObjectBaseUtility::IsInA >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBaseUtility.h:694:23
	makeExtern<DAS_CALL_METHOD(_method_182), SimNode_ExtFuncCall >(lib,"IsInA","das_call_member< bool (UObjectBaseUtility::*)(const UClass *) const , &UObjectBaseUtility::IsInA >::invoke")
		->args({"self","SomeBaseClass"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_183 = das::das_call_member< bool (UObjectBaseUtility::*)(unsigned int) const,&UObjectBaseUtility::RootPackageHasAnyFlags >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBaseUtility.h:703:23
	makeExtern<DAS_CALL_METHOD(_method_183), SimNode_ExtFuncCall >(lib,"RootPackageHasAnyFlags","das_call_member< bool (UObjectBaseUtility::*)(unsigned int) const , &UObjectBaseUtility::RootPackageHasAnyFlags >::invoke")
		->args({"self","CheckFlagMask"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_184 = das::das_call_member< const UClass * (UObjectBaseUtility::*)(const UClass *) const,&UObjectBaseUtility::FindNearestCommonBaseClass >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBaseUtility.h:751:32
	makeExtern<DAS_CALL_METHOD(_method_184), SimNode_ExtFuncCall >(lib,"FindNearestCommonBaseClass","das_call_member< const UClass * (UObjectBaseUtility::*)(const UClass *) const , &UObjectBaseUtility::FindNearestCommonBaseClass >::invoke")
		->args({"self","TestClass"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_185 = das::das_call_member< void * (UObjectBaseUtility::*)(UClass *),&UObjectBaseUtility::GetInterfaceAddress >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBaseUtility.h:762:24
	makeExtern<DAS_CALL_METHOD(_method_185), SimNode_ExtFuncCall >(lib,"GetInterfaceAddress","das_call_member< void * (UObjectBaseUtility::*)(UClass *) , &UObjectBaseUtility::GetInterfaceAddress >::invoke")
		->args({"self","InterfaceClass"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_186 = das::das_call_member< void * (UObjectBaseUtility::*)(UClass *),&UObjectBaseUtility::GetNativeInterfaceAddress >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBaseUtility.h:768:24
	makeExtern<DAS_CALL_METHOD(_method_186), SimNode_ExtFuncCall >(lib,"GetNativeInterfaceAddress","das_call_member< void * (UObjectBaseUtility::*)(UClass *) , &UObjectBaseUtility::GetNativeInterfaceAddress >::invoke")
		->args({"self","InterfaceClass"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_187 = das::das_call_member< const void * (UObjectBaseUtility::*)(UClass *) const,&UObjectBaseUtility::GetNativeInterfaceAddress >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBaseUtility.h:774:14
	makeExtern<DAS_CALL_METHOD(_method_187), SimNode_ExtFuncCall >(lib,"GetNativeInterfaceAddress","das_call_member< const void * (UObjectBaseUtility::*)(UClass *) const , &UObjectBaseUtility::GetNativeInterfaceAddress >::invoke")
		->args({"self","InterfaceClass"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_188 = das::das_call_member< bool (UObjectBaseUtility::*)() const,&UObjectBaseUtility::IsDefaultSubobject >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBaseUtility.h:784:23
	makeExtern<DAS_CALL_METHOD(_method_188), SimNode_ExtFuncCall >(lib,"IsDefaultSubobject","das_call_member< bool (UObjectBaseUtility::*)() const , &UObjectBaseUtility::IsDefaultSubobject >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_189 = das::das_call_member< int (UObjectBaseUtility::*)() const,&UObjectBaseUtility::GetLinkerIndex >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBaseUtility.h:804:24
	makeExtern<DAS_CALL_METHOD(_method_189), SimNode_ExtFuncCall >(lib,"GetLinkerIndex","das_call_member< int (UObjectBaseUtility::*)() const , &UObjectBaseUtility::GetLinkerIndex >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_190 = das::das_call_member< int (UObjectBaseUtility::*)() const,&UObjectBaseUtility::GetLinkerUE4Version >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBaseUtility.h:818:15
	makeExtern<DAS_CALL_METHOD(_method_190), SimNode_ExtFuncCall >(lib,"GetLinkerUE4Version","das_call_member< int (UObjectBaseUtility::*)() const , &UObjectBaseUtility::GetLinkerUE4Version >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_191 = das::das_call_member< int (UObjectBaseUtility::*)() const,&UObjectBaseUtility::GetLinkerLicenseeUEVersion >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBaseUtility.h:835:24
	makeExtern<DAS_CALL_METHOD(_method_191), SimNode_ExtFuncCall >(lib,"GetLinkerLicenseeUEVersion","das_call_member< int (UObjectBaseUtility::*)() const , &UObjectBaseUtility::GetLinkerLicenseeUEVersion >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_192 = das::das_call_member< int (UObjectBaseUtility::*)() const,&UObjectBaseUtility::GetLinkerLicenseeUE4Version >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBaseUtility.h:838:15
	makeExtern<DAS_CALL_METHOD(_method_192), SimNode_ExtFuncCall >(lib,"GetLinkerLicenseeUE4Version","das_call_member< int (UObjectBaseUtility::*)() const , &UObjectBaseUtility::GetLinkerLicenseeUE4Version >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_193 = das::das_call_member< void (UObjectBaseUtility::*)(),&UObjectBaseUtility::ResetStatID >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBaseUtility.h:868:19
	makeExtern<DAS_CALL_METHOD(_method_193), SimNode_ExtFuncCall >(lib,"ResetStatID","das_call_member< void (UObjectBaseUtility::*)() , &UObjectBaseUtility::ResetStatID >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBaseUtility.h:918:18
	makeExtern< bool (*)(UObject *) , IsPossiblyAllocatedUObjectPointer , SimNode_ExtFuncCall >(lib,"IsPossiblyAllocatedUObjectPointer","IsPossiblyAllocatedUObjectPointer")
		->args({"Ptr"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBaseUtility.h:946:21
	makeExtern< FString (*)(const UObjectBaseUtility *) , GetNameSafe , SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetNameSafe","GetNameSafe")
		->args({"Object"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBaseUtility.h:963:21
	makeExtern< FString (*)(const UObjectBaseUtility *) , GetPathNameSafe , SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetPathNameSafe","GetPathNameSafe")
		->args({"Object"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBaseUtility.h:980:21
	makeExtern< FString (*)(const UObjectBaseUtility *) , GetFullNameSafe , SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetFullNameSafe","GetFullNameSafe")
		->args({"Object"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBaseUtility.h:996:25
	makeExtern< UClass * (*)(UClass *) , GetParentNativeClass , SimNode_ExtFuncCall >(lib,"GetParentNativeClass","GetParentNativeClass")
		->args({"Class"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBaseUtility.h:1003:23
	makeExtern< FName (*)(const UObjectBaseUtility *) , GetClassTraceScope , SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetClassTraceScope","GetClassTraceScope")
		->args({"Object"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Object.h:88:2
	makeExtern< UClass * (*)(const UObject *) , _dasUnreal_static_39_StaticClass , SimNode_ExtFuncCall >(lib,"StaticClass","_dasUnreal_static_39_StaticClass")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const UObject*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Object.h:95:22
	makeExtern< const wchar_t * (*)(const UObject *) , _dasUnreal_static_40_StaticConfigName , SimNode_ExtFuncCall >(lib,"StaticConfigName","_dasUnreal_static_40_StaticConfigName")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const UObject*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Object.h:99:14
	makeExtern< void (*)(const UObject *) , _dasUnreal_static_41_StaticRegisterNativesUObject , SimNode_ExtFuncCall >(lib,"StaticRegisterNativesUObject","_dasUnreal_static_41_StaticRegisterNativesUObject")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const UObject*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
	using _method_197 = das::das_call_member< UObject * (UObject::*)(FName,UClass *,UClass *,bool,bool),&UObject::CreateDefaultSubobject >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Object.h:120:27
	makeExtern<DAS_CALL_METHOD(_method_197), SimNode_ExtFuncCall >(lib,"CreateDefaultSubobject","das_call_member< UObject * (UObject::*)(FName,UClass *,UClass *,bool,bool) , &UObject::CreateDefaultSubobject >::invoke")
		->args({"self","SubobjectFName","ReturnType","ClassToCreateByDefault","bIsRequired","bIsTransient"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_198 = das::das_call_member< UObject * (UObject::*)(FName),&UObject::GetDefaultSubobjectByName >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Object.h:199:27
	makeExtern<DAS_CALL_METHOD(_method_198), SimNode_ExtFuncCall >(lib,"GetDefaultSubobjectByName","das_call_member< UObject * (UObject::*)(FName) , &UObject::GetDefaultSubobjectByName >::invoke")
		->args({"self","ToFind"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Object.h:217:31
	makeExtern< void (*)(UObject &) , _dasUnreal_virtual_42_PostInitProperties , SimNode_ExtFuncCall >(lib,"PostInitProperties","_dasUnreal_virtual_42_PostInitProperties")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Object.h:223:31
	makeExtern< void (*)(UObject &) , _dasUnreal_virtual_43_PostReinitProperties , SimNode_ExtFuncCall >(lib,"PostReinitProperties","_dasUnreal_virtual_43_PostReinitProperties")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Object.h:229:15
	makeExtern< void (*)(UObject &) , _dasUnreal_virtual_44_PostCDOContruct , SimNode_ExtFuncCall >(lib,"PostCDOContruct","_dasUnreal_virtual_44_PostCDOContruct")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Object.h:256:31
	makeExtern< bool (*)(UObject &,const wchar_t *) , _dasUnreal_virtual_45_PreSaveRoot , SimNode_ExtFuncCall >(lib,"PreSaveRoot","_dasUnreal_virtual_45_PreSaveRoot")
		->args({"self","Filename"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Object.h:268:31
	makeExtern< void (*)(UObject &,bool) , _dasUnreal_virtual_46_PostSaveRoot , SimNode_ExtFuncCall >(lib,"PostSaveRoot","_dasUnreal_virtual_46_PostSaveRoot")
		->args({"self","bCleanupIsRequired"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Object.h:299:31
	makeExtern< bool (*)(UObject &,bool) , _dasUnreal_virtual_47_Modify , SimNode_ExtFuncCall >(lib,"Modify","_dasUnreal_virtual_47_Modify")
		->args({"self","bAlwaysMarkDirty"})
		->arg_init(1,make_smart<ExprConstBool>(true))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Object.h:302:31
	makeExtern< bool (*)(const UObject &) , _dasUnreal_virtual_48_IsCapturingAsRootObjectForTransaction , SimNode_ExtFuncCall >(lib,"IsCapturingAsRootObjectForTransaction","_dasUnreal_virtual_48_IsCapturingAsRootObjectForTransaction")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_206 = das::das_call_member< bool (UObject::*)() const,&UObject::CanModify >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Object.h:305:23
	makeExtern<DAS_CALL_METHOD(_method_206), SimNode_ExtFuncCall >(lib,"CanModify","das_call_member< bool (UObject::*)() const , &UObject::CanModify >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Object.h:315:15
	makeExtern< void (*)(UObject &,const FName &) , _dasUnreal_virtual_49_LoadedFromAnotherClass , SimNode_ExtFuncCall >(lib,"LoadedFromAnotherClass","_dasUnreal_virtual_49_LoadedFromAnotherClass")
		->args({"self","OldClassName"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Object.h:330:15
	makeExtern< bool (*)(UObject &,const wchar_t *,UObject *&,bool) , _dasUnreal_virtual_50_ResolveSubobject , SimNode_ExtFuncCall >(lib,"ResolveSubobject","_dasUnreal_virtual_50_ResolveSubobject")
		->args({"self","SubObjectPath","OutObject","bLoadIfExists"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Object.h:335:15
	makeExtern< bool (*)(const UObject &) , _dasUnreal_virtual_51_IsReadyForAsyncPostLoad , SimNode_ExtFuncCall >(lib,"IsReadyForAsyncPostLoad","_dasUnreal_virtual_51_IsReadyForAsyncPostLoad")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Object.h:341:31
	makeExtern< void (*)(UObject &) , _dasUnreal_virtual_52_PostLoad , SimNode_ExtFuncCall >(lib,"PostLoad","_dasUnreal_virtual_52_PostLoad")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Object.h:356:31
	makeExtern< void (*)(UObject &) , _dasUnreal_virtual_53_BeginDestroy , SimNode_ExtFuncCall >(lib,"BeginDestroy","_dasUnreal_virtual_53_BeginDestroy")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Object.h:363:15
	makeExtern< bool (*)(UObject &) , _dasUnreal_virtual_54_IsReadyForFinishDestroy , SimNode_ExtFuncCall >(lib,"IsReadyForFinishDestroy","_dasUnreal_virtual_54_IsReadyForFinishDestroy")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Object.h:369:15
	makeExtern< void (*)(UObject &) , _dasUnreal_virtual_55_PostLinkerChange , SimNode_ExtFuncCall >(lib,"PostLinkerChange","_dasUnreal_virtual_55_PostLinkerChange")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Object.h:377:31
	makeExtern< void (*)(UObject &) , _dasUnreal_virtual_56_FinishDestroy , SimNode_ExtFuncCall >(lib,"FinishDestroy","_dasUnreal_virtual_56_FinishDestroy")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Object.h:410:15
	makeExtern< void (*)(UObject &) , _dasUnreal_virtual_57_ShutdownAfterError , SimNode_ExtFuncCall >(lib,"ShutdownAfterError","_dasUnreal_virtual_57_ShutdownAfterError")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_216 = das::das_call_member< void (UObject::*)(),&UObject::PostEditChange >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Object.h:456:23
	makeExtern<DAS_CALL_METHOD(_method_216), SimNode_ExtFuncCall >(lib,"PostEditChange","das_call_member< void (UObject::*)() , &UObject::PostEditChange >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_217 = das::das_call_member< void (UObject::*)(EEditChangePropagationFlags),&UObject::SetEditChangePropagationFlags >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Object.h:472:23
	makeExtern<DAS_CALL_METHOD(_method_217), SimNode_ExtFuncCall >(lib,"SetEditChangePropagationFlags","das_call_member< void (UObject::*)(EEditChangePropagationFlags) , &UObject::SetEditChangePropagationFlags >::invoke")
		->args({"self","InFlags"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Object.h:475:31
	makeExtern< void (*)(UObject &) , _dasUnreal_virtual_58_PreEditUndo , SimNode_ExtFuncCall >(lib,"PreEditUndo","_dasUnreal_virtual_58_PreEditUndo")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Object.h:513:15
	makeExtern< void (*)(UObject &,UObject *,const FName) , _dasUnreal_virtual_59_PostRename , SimNode_ExtFuncCall >(lib,"PostRename","_dasUnreal_virtual_59_PostRename")
		->args({"self","OldOuter","OldName"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Object.h:540:31
	makeExtern< bool (*)(const UObject &) , _dasUnreal_virtual_60_NeedsLoadForClient , SimNode_ExtFuncCall >(lib,"NeedsLoadForClient","_dasUnreal_virtual_60_NeedsLoadForClient")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Object.h:548:31
	makeExtern< bool (*)(const UObject &) , _dasUnreal_virtual_61_NeedsLoadForServer , SimNode_ExtFuncCall >(lib,"NeedsLoadForServer","_dasUnreal_virtual_61_NeedsLoadForServer")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Object.h:564:15
	makeExtern< bool (*)(const UObject &) , _dasUnreal_virtual_62_NeedsLoadForEditorGame , SimNode_ExtFuncCall >(lib,"NeedsLoadForEditorGame","_dasUnreal_virtual_62_NeedsLoadForEditorGame")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Object.h:574:15
	makeExtern< bool (*)(const UObject &) , _dasUnreal_virtual_63_IsEditorOnly , SimNode_ExtFuncCall >(lib,"IsEditorOnly","_dasUnreal_virtual_63_IsEditorOnly")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Object.h:585:15
	makeExtern< bool (*)(const UObject &) , _dasUnreal_virtual_64_HasNonEditorOnlyReferences , SimNode_ExtFuncCall >(lib,"HasNonEditorOnlyReferences","_dasUnreal_virtual_64_HasNonEditorOnlyReferences")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Object.h:595:15
	makeExtern< bool (*)(const UObject &) , _dasUnreal_virtual_65_IsPostLoadThreadSafe , SimNode_ExtFuncCall >(lib,"IsPostLoadThreadSafe","_dasUnreal_virtual_65_IsPostLoadThreadSafe")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Object.h:605:31
	makeExtern< bool (*)(const UObject &) , _dasUnreal_virtual_66_IsDestructionThreadSafe , SimNode_ExtFuncCall >(lib,"IsDestructionThreadSafe","_dasUnreal_virtual_66_IsDestructionThreadSafe")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Object.h:637:30
	makeExtern< bool (*)(const UObject *,UObject *) , _dasUnreal_static_67_CanCreateInCurrentContext , SimNode_ExtFuncCall >(lib,"CanCreateInCurrentContext","_dasUnreal_static_67_CanCreateInCurrentContext")
		->args({"self","Template"})
		->arg_type(0,makeType<TExplicit<const UObject*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Object.h:666:15
	makeExtern< void (*)(UObject &) , _dasUnreal_virtual_68_PostEditImport , SimNode_ExtFuncCall >(lib,"PostEditImport","_dasUnreal_virtual_68_PostEditImport")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Object.h:681:31
	makeExtern< bool (*)(UObject &,const wchar_t *,UObject *,unsigned int) , _dasUnreal_virtual_69_Rename , SimNode_ExtFuncCall >(lib,"Rename","_dasUnreal_virtual_69_Rename")
		->args({"self","NewName","NewOuter","Flags"})
		->arg_init(1,make_smart<ExprConstPtr>())
		->arg_init(2,make_smart<ExprConstPtr>())
		->arg_init(3,make_smart<ExprConstUInt>(0x0))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Object.h:684:18
	makeExtern< FString (*)(UObject &) , _dasUnreal_virtual_70_GetDesc , SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetDesc","_dasUnreal_virtual_70_GetDesc")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_231 = das::das_call_member< UScriptStruct * (UObject::*)() const,&UObject::GetSparseClassDataStruct >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Object.h:687:33
	makeExtern<DAS_CALL_METHOD(_method_231), SimNode_ExtFuncCall >(lib,"GetSparseClassDataStruct","das_call_member< UScriptStruct * (UObject::*)() const , &UObject::GetSparseClassDataStruct >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Object.h:690:15
	makeExtern< void (*)(const UObject &) , _dasUnreal_virtual_71_MoveDataToSparseClassDataStruct , SimNode_ExtFuncCall >(lib,"MoveDataToSparseClassDataStruct","_dasUnreal_virtual_71_MoveDataToSparseClassDataStruct")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Object.h:691:15
	makeExtern< bool (*)(const UObject &,UScriptStruct *,void *) , _dasUnreal_virtual_72_ConformSparseClassDataStruct , SimNode_ExtFuncCall >(lib,"ConformSparseClassDataStruct","_dasUnreal_virtual_72_ConformSparseClassDataStruct")
		->args({"self","SourceSparseClassDataStruct","SourceSparseClassData"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Object.h:706:31
	makeExtern< bool (*)(const UObject &) , _dasUnreal_virtual_73_ImplementsGetWorld , SimNode_ExtFuncCall >(lib,"ImplementsGetWorld","_dasUnreal_virtual_73_ImplementsGetWorld")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Object.h:754:16
	makeExtern< FName (*)(UObject &) , _dasUnreal_virtual_74_GetExporterName , SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetExporterName","_dasUnreal_virtual_74_GetExporterName")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Object.h:781:39
	makeExtern< FRestoreForUObjectOverwrite * (*)(UObject &) , _dasUnreal_virtual_75_GetRestoreForUObjectOverwrite , SimNode_ExtFuncCall >(lib,"GetRestoreForUObjectOverwrite","_dasUnreal_virtual_75_GetRestoreForUObjectOverwrite")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Object.h:793:15
	makeExtern< bool (*)(const UObject &,UObject *) , _dasUnreal_virtual_76_AreNativePropertiesIdenticalTo , SimNode_ExtFuncCall >(lib,"AreNativePropertiesIdenticalTo","_dasUnreal_virtual_76_AreNativePropertiesIdenticalTo")
		->args({"self","Other"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_238 = das::das_call_member< void (UObject::*)(FAssetData &) const,&UObject::GetAssetRegistryTags >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Object.h:881:23
	makeExtern<DAS_CALL_METHOD(_method_238), SimNode_ExtFuncCall >(lib,"GetAssetRegistryTags","das_call_member< void (UObject::*)(FAssetData &) const , &UObject::GetAssetRegistryTags >::invoke")
		->args({"self","Out"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Object.h:884:38
	makeExtern< const FName & (*)(const UObject *) , _dasUnreal_static_77_SourceFileTagName , SimNode_ExtFuncCallRef >(lib,"SourceFileTagName","_dasUnreal_static_77_SourceFileTagName")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const UObject*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Object.h:888:38
	makeExtern< const FName & (*)(const UObject *) , _dasUnreal_static_78_AssetVersePathTagName , SimNode_ExtFuncCallRef >(lib,"AssetVersePathTagName","_dasUnreal_static_78_AssetVersePathTagName")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const UObject*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
	using _method_241 = das::das_call_member< UObject::FAssetRegistryTagMetadata & (UObject::FAssetRegistryTagMetadata::*)(const FText &),&UObject::FAssetRegistryTagMetadata::SetDisplayName >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Object.h:914:30
	makeExtern<DAS_CALL_METHOD(_method_241), SimNode_ExtFuncCallRef >(lib,"SetDisplayName","das_call_member< UObject::FAssetRegistryTagMetadata & (UObject::FAssetRegistryTagMetadata::*)(const FText &) , &UObject::FAssetRegistryTagMetadata::SetDisplayName >::invoke")
		->args({"self","InDisplayName"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_242 = das::das_call_member< UObject::FAssetRegistryTagMetadata & (UObject::FAssetRegistryTagMetadata::*)(const FText &),&UObject::FAssetRegistryTagMetadata::SetTooltip >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Object.h:921:30
	makeExtern<DAS_CALL_METHOD(_method_242), SimNode_ExtFuncCallRef >(lib,"SetTooltip","das_call_member< UObject::FAssetRegistryTagMetadata & (UObject::FAssetRegistryTagMetadata::*)(const FText &) , &UObject::FAssetRegistryTagMetadata::SetTooltip >::invoke")
		->args({"self","InTooltipText"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_243 = das::das_call_member< UObject::FAssetRegistryTagMetadata & (UObject::FAssetRegistryTagMetadata::*)(const FText &),&UObject::FAssetRegistryTagMetadata::SetSuffix >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Object.h:928:30
	makeExtern<DAS_CALL_METHOD(_method_243), SimNode_ExtFuncCallRef >(lib,"SetSuffix","das_call_member< UObject::FAssetRegistryTagMetadata & (UObject::FAssetRegistryTagMetadata::*)(const FText &) , &UObject::FAssetRegistryTagMetadata::SetSuffix >::invoke")
		->args({"self","InSuffix"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_244 = das::das_call_member< UObject::FAssetRegistryTagMetadata & (UObject::FAssetRegistryTagMetadata::*)(const FString &),&UObject::FAssetRegistryTagMetadata::SetImportantValue >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Object.h:935:30
	makeExtern<DAS_CALL_METHOD(_method_244), SimNode_ExtFuncCallRef >(lib,"SetImportantValue","das_call_member< UObject::FAssetRegistryTagMetadata & (UObject::FAssetRegistryTagMetadata::*)(const FString &) , &UObject::FAssetRegistryTagMetadata::SetImportantValue >::invoke")
		->args({"self","InImportantValue"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Object.h:951:31
	makeExtern< bool (*)(const UObject &) , _dasUnreal_virtual_79_IsAsset , SimNode_ExtFuncCall >(lib,"IsAsset","_dasUnreal_virtual_79_IsAsset")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Object.h:961:31
	makeExtern< bool (*)(const UObject &) , _dasUnreal_virtual_80_IsLocalizedResource , SimNode_ExtFuncCall >(lib,"IsLocalizedResource","_dasUnreal_virtual_80_IsLocalizedResource")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Object.h:964:31
	makeExtern< bool (*)(const UObject &) , _dasUnreal_virtual_81_IsSafeForRootSet , SimNode_ExtFuncCall >(lib,"IsSafeForRootSet","_dasUnreal_virtual_81_IsSafeForRootSet")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Object.h:971:31
	makeExtern< void (*)(UObject &,EObjectFlags) , _dasUnreal_virtual_82_TagSubobjects , SimNode_ExtFuncCall >(lib,"TagSubobjects","_dasUnreal_virtual_82_TagSubobjects")
		->args({"self","NewFlags"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Object.h:990:31
	makeExtern< bool (*)(const UObject &) , _dasUnreal_virtual_83_IsNameStableForNetworking , SimNode_ExtFuncCall >(lib,"IsNameStableForNetworking","_dasUnreal_virtual_83_IsNameStableForNetworking")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Object.h:993:31
	makeExtern< bool (*)(const UObject &) , _dasUnreal_virtual_84_IsFullNameStableForNetworking , SimNode_ExtFuncCall >(lib,"IsFullNameStableForNetworking","_dasUnreal_virtual_84_IsFullNameStableForNetworking")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Object.h:996:31
	makeExtern< bool (*)(const UObject &) , _dasUnreal_virtual_85_IsSupportedForNetworking , SimNode_ExtFuncCall >(lib,"IsSupportedForNetworking","_dasUnreal_virtual_85_IsSupportedForNetworking")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Object.h:1002:31
	makeExtern< void (*)(UObject &) , _dasUnreal_virtual_86_PreNetReceive , SimNode_ExtFuncCall >(lib,"PreNetReceive","_dasUnreal_virtual_86_PreNetReceive")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Object.h:1005:31
	makeExtern< void (*)(UObject &) , _dasUnreal_virtual_87_PostNetReceive , SimNode_ExtFuncCall >(lib,"PostNetReceive","_dasUnreal_virtual_87_PostNetReceive")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Object.h:1008:15
	makeExtern< void (*)(UObject &) , _dasUnreal_virtual_88_PostRepNotifies , SimNode_ExtFuncCall >(lib,"PostRepNotifies","_dasUnreal_virtual_88_PostRepNotifies")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Object.h:1011:31
	makeExtern< void (*)(UObject &) , _dasUnreal_virtual_89_PreDestroyFromReplication , SimNode_ExtFuncCall >(lib,"PreDestroyFromReplication","_dasUnreal_virtual_89_PreDestroyFromReplication")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_256 = das::das_call_member< bool (UObject::*)() const,&UObject::IsSelected >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Object.h:1047:23
	makeExtern<DAS_CALL_METHOD(_method_256), SimNode_ExtFuncCall >(lib,"IsSelected","das_call_member< bool (UObject::*)() const , &UObject::IsSelected >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_257 = das::das_call_member< FString (UObject::*)() const,&UObject::GetDetailedInfo >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Object.h:1100:26
	makeExtern<DAS_CALL_METHOD(_method_257), SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetDetailedInfo","das_call_member< FString (UObject::*)() const , &UObject::GetDetailedInfo >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_258 = das::das_call_member< bool (UObject::*)(),&UObject::ConditionalBeginDestroy >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Object.h:1106:23
	makeExtern<DAS_CALL_METHOD(_method_258), SimNode_ExtFuncCall >(lib,"ConditionalBeginDestroy","das_call_member< bool (UObject::*)() , &UObject::ConditionalBeginDestroy >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_259 = das::das_call_member< bool (UObject::*)(),&UObject::ConditionalFinishDestroy >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Object.h:1109:23
	makeExtern<DAS_CALL_METHOD(_method_259), SimNode_ExtFuncCall >(lib,"ConditionalFinishDestroy","das_call_member< bool (UObject::*)() , &UObject::ConditionalFinishDestroy >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
}
}

