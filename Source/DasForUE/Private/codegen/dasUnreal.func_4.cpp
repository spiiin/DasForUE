// this file is generated via daScript automatic C++ binder
// all user modifications will be lost after this file is re-generated

#include "dasUnreal.h"
#include "need_dasUnreal.h"
namespace das {
#include "dasUnreal.func.aot.decl.inc"
void Module_dasUnreal::initFunctions_4() {
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Object.h:1358:34
	makeExtern< UObject * (*)(const UObject &,const UClass *,const UObject *,FName,EObjectFlags) , _dasUnreal_static_73_GetArchetypeFromRequiredInfo , SimNode_ExtFuncCall >(lib,"GetArchetypeFromRequiredInfo","_dasUnreal_static_73_GetArchetypeFromRequiredInfo")
		->args({"self","Class","Outer","Name","ObjectFlags"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_236 = das::das_call_member< UObject * (UObject::*)() const,&UObject::GetArchetype >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Object.h:1365:27
	makeExtern<DAS_CALL_METHOD(_method_236), SimNode_ExtFuncCall >(lib,"GetArchetype","das_call_member< UObject * (UObject::*)() const , &UObject::GetArchetype >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_237 = das::das_call_member< bool (UObject::*)() const,&UObject::IsInBlueprint >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Object.h:1455:23
	makeExtern<DAS_CALL_METHOD(_method_237), SimNode_ExtFuncCall >(lib,"IsInBlueprint","das_call_member< bool (UObject::*)() const , &UObject::IsInBlueprint >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_238 = das::das_call_member< void (UObject::*)(),&UObject::DestroyNonNativeProperties >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Object.h:1460:23
	makeExtern<DAS_CALL_METHOD(_method_238), SimNode_ExtFuncCall >(lib,"DestroyNonNativeProperties","das_call_member< void (UObject::*)() , &UObject::DestroyNonNativeProperties >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Object.h:1463:15
	makeExtern< void (*)(UObject &) , _dasUnreal_virtual_74_MarkAsEditorOnlySubobject , SimNode_ExtFuncCall >(lib,"MarkAsEditorOnlySubobject","_dasUnreal_virtual_74_MarkAsEditorOnlySubobject")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_240 = das::das_call_member< void (UObject::*)() const,&UObject::AbortInsideMemberFunction >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Object.h:1468:23
	makeExtern<DAS_CALL_METHOD(_method_240), SimNode_ExtFuncCall >(lib,"AbortInsideMemberFunction","das_call_member< void (UObject::*)() const , &UObject::AbortInsideMemberFunction >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_241 = das::das_call_member< void (UObject::*)(int),&UObject::ExecuteUbergraph >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Object.h:1664:7
	makeExtern<DAS_CALL_METHOD(_method_241), SimNode_ExtFuncCall >(lib,"ExecuteUbergraph","das_call_member< void (UObject::*)(int) , &UObject::ExecuteUbergraph >::invoke")
		->args({"self","EntryPoint"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Object.h:1717:17
	makeExtern< uint64 (*)(const UObject &) , _dasUnreal_virtual_75_GetNetPushIdDynamic , SimNode_ExtFuncCall >(lib,"GetNetPushIdDynamic","_dasUnreal_virtual_75_GetNetPushIdDynamic")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Object.h:1771:18
	makeExtern< bool (*)(const UObject *) , IsValid , SimNode_ExtFuncCall >(lib,"IsValid","IsValid")
		->args({"Test"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Object.h:1782:18
	makeExtern< bool (*)(const UObject *) , IsValidChecked , SimNode_ExtFuncCall >(lib,"IsValidChecked","IsValidChecked")
		->args({"Test"})
		->addToModule(*this, SideEffects::worstDefault);
}
}

