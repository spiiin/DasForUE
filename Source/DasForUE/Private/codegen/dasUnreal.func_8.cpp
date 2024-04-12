// this file is generated via daScript automatic C++ binder
// all user modifications will be lost after this file is re-generated

#include "dasUnreal.h"
#include "need_dasUnreal.h"
namespace das {
#include "dasUnreal.func.aot.decl.inc"
void Module_dasUnreal::initFunctions_8() {
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Field.h:427:33
	makeExtern< uint64 (*)(const FField *) , _dasUnreal_static_200_StaticClassCastFlagsPrivate , SimNode_ExtFuncCall >(lib,"StaticClassCastFlagsPrivate","_dasUnreal_static_200_StaticClassCastFlagsPrivate")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const FField*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Field.h:431:33
	makeExtern< uint64 (*)(const FField *) , _dasUnreal_static_201_StaticClassCastFlags , SimNode_ExtFuncCall >(lib,"StaticClassCastFlags","_dasUnreal_static_201_StaticClassCastFlags")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const FField*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
	addCtorAndUsing<FField,EInternal,FFieldClass *>(*this,lib,"FField","FField")
		->args({"InInernal","InClass"});
	addCtorAndUsing<FField,FFieldVariant,const FName &,EObjectFlags>(*this,lib,"FField","FField")
		->args({"InOwner","InName","InObjectFlags"});
	addCtorAndUsing<FField,UField *>(*this,lib,"FField","FField")
		->args({"InField"});
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Field.h:457:31
	makeExtern< void (*)(FField &,FArchive &) , _dasUnreal_virtual_202_Serialize , SimNode_ExtFuncCall >(lib,"Serialize","_dasUnreal_virtual_202_Serialize")
		->args({"self","Ar"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Field.h:458:31
	makeExtern< void (*)(FField &) , _dasUnreal_virtual_203_PostLoad , SimNode_ExtFuncCall >(lib,"PostLoad","_dasUnreal_virtual_203_PostLoad")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Field.h:460:31
	makeExtern< void (*)(FField &) , _dasUnreal_virtual_204_BeginDestroy , SimNode_ExtFuncCall >(lib,"BeginDestroy","_dasUnreal_virtual_204_BeginDestroy")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Field.h:461:31
	makeExtern< void (*)(FField &,FReferenceCollector &) , _dasUnreal_virtual_205_AddReferencedObjects , SimNode_ExtFuncCall >(lib,"AddReferencedObjects","_dasUnreal_virtual_205_AddReferencedObjects")
		->args({"self","Collector"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_542 = das::das_call_member< bool (FField::*)() const,&FField::IsRooted >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Field.h:462:23
	makeExtern<DAS_CALL_METHOD(_method_542), SimNode_ExtFuncCall >(lib,"IsRooted","das_call_member< bool (FField::*)() const , &FField::IsRooted >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_543 = das::das_call_member< bool (FField::*)() const,&FField::IsNative >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Field.h:463:23
	makeExtern<DAS_CALL_METHOD(_method_543), SimNode_ExtFuncCall >(lib,"IsNative","das_call_member< bool (FField::*)() const , &FField::IsNative >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_544 = das::das_call_member< bool (FField::*)() const,&FField::IsValidLowLevel >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Field.h:464:23
	makeExtern<DAS_CALL_METHOD(_method_544), SimNode_ExtFuncCall >(lib,"IsValidLowLevel","das_call_member< bool (FField::*)() const , &FField::IsValidLowLevel >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_545 = das::das_call_member< bool (FField::*)(const UObject *) const,&FField::IsIn >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Field.h:465:23
	makeExtern<DAS_CALL_METHOD(_method_545), SimNode_ExtFuncCall >(lib,"IsIn","das_call_member< bool (FField::*)(const UObject *) const , &FField::IsIn >::invoke")
		->args({"self","InOwner"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_546 = das::das_call_member< bool (FField::*)(const FField *) const,&FField::IsIn >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Field.h:466:23
	makeExtern<DAS_CALL_METHOD(_method_546), SimNode_ExtFuncCall >(lib,"IsIn","das_call_member< bool (FField::*)(const FField *) const , &FField::IsIn >::invoke")
		->args({"self","InOwner"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Field.h:471:31
	makeExtern< void (*)(FField &,FProperty *) , _dasUnreal_virtual_206_AddCppProperty , SimNode_ExtFuncCall >(lib,"AddCppProperty","_dasUnreal_virtual_206_AddCppProperty")
		->args({"self","Property"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Field.h:472:31
	makeExtern< void (*)(FField &) , _dasUnreal_virtual_207_Bind , SimNode_ExtFuncCall >(lib,"Bind","_dasUnreal_virtual_207_Bind")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Field.h:476:33
	makeExtern< FField * (*)(const FField *,const FFieldVariant &,const FName &,EObjectFlags) , _dasUnreal_static_208_Construct , SimNode_ExtFuncCall >(lib,"Construct","_dasUnreal_static_208_Construct")
		->args({"self","InOwner","InName","InFlags"})
		->arg_type(0,makeType<TExplicit<const FField*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Field.h:478:33
	makeExtern< FField * (*)(const FField *,const FName &,const FFieldVariant &,const FName &,EObjectFlags) , _dasUnreal_static_209_Construct , SimNode_ExtFuncCall >(lib,"Construct","_dasUnreal_static_209_Construct")
		->args({"self","FieldTypeName","InOwner","InName","InFlags"})
		->arg_type(0,makeType<TExplicit<const FField*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Field.h:481:31
	makeExtern< void (*)(FField &,const FField &) , _dasUnreal_virtual_210_PostDuplicate , SimNode_ExtFuncCall >(lib,"PostDuplicate","_dasUnreal_virtual_210_PostDuplicate")
		->args({"self","InField"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_552 = das::das_call_member< EObjectFlags (FField::*)() const,&FField::GetFlags >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Field.h:498:15
	makeExtern<DAS_CALL_METHOD(_method_552), SimNode_ExtFuncCall >(lib,"GetFlags","das_call_member< EObjectFlags (FField::*)() const , &FField::GetFlags >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_553 = das::das_call_member< void (FField::*)(EObjectFlags),&FField::SetFlags >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Field.h:504:7
	makeExtern<DAS_CALL_METHOD(_method_553), SimNode_ExtFuncCall >(lib,"SetFlags","das_call_member< void (FField::*)(EObjectFlags) , &FField::SetFlags >::invoke")
		->args({"self","NewFlags"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_554 = das::das_call_member< void (FField::*)(EObjectFlags),&FField::ClearFlags >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Field.h:509:7
	makeExtern<DAS_CALL_METHOD(_method_554), SimNode_ExtFuncCall >(lib,"ClearFlags","das_call_member< void (FField::*)(EObjectFlags) , &FField::ClearFlags >::invoke")
		->args({"self","NewFlags"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_555 = das::das_call_member< bool (FField::*)(EObjectFlags) const,&FField::HasAnyFlags >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Field.h:520:7
	makeExtern<DAS_CALL_METHOD(_method_555), SimNode_ExtFuncCall >(lib,"HasAnyFlags","das_call_member< bool (FField::*)(EObjectFlags) const , &FField::HasAnyFlags >::invoke")
		->args({"self","FlagsToCheck"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_556 = das::das_call_member< bool (FField::*)(EObjectFlags) const,&FField::HasAllFlags >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Field.h:531:7
	makeExtern<DAS_CALL_METHOD(_method_556), SimNode_ExtFuncCall >(lib,"HasAllFlags","das_call_member< bool (FField::*)(EObjectFlags) const , &FField::HasAllFlags >::invoke")
		->args({"self","FlagsToCheck"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_557 = das::das_call_member< FFieldClass * (FField::*)() const,&FField::GetClass >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Field.h:537:22
	makeExtern<DAS_CALL_METHOD(_method_557), SimNode_ExtFuncCall >(lib,"GetClass","das_call_member< FFieldClass * (FField::*)() const , &FField::GetClass >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_558 = das::das_call_member< uint64 (FField::*)() const,&FField::GetCastFlags >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Field.h:541:16
	makeExtern<DAS_CALL_METHOD(_method_558), SimNode_ExtFuncCall >(lib,"GetCastFlags","das_call_member< uint64 (FField::*)() const , &FField::GetCastFlags >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_559 = das::das_call_member< bool (FField::*)(const FFieldClass *) const,&FField::IsA >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Field.h:546:14
	makeExtern<DAS_CALL_METHOD(_method_559), SimNode_ExtFuncCall >(lib,"IsA","das_call_member< bool (FField::*)(const FFieldClass *) const , &FField::IsA >::invoke")
		->args({"self","FieldType"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_560 = das::das_call_member< void (FField::*)(FString &) const,&FField::AppendName >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Field.h:577:7
	makeExtern<DAS_CALL_METHOD(_method_560), SimNode_ExtFuncCall >(lib,"AppendName","das_call_member< void (FField::*)(FString &) const , &FField::AppendName >::invoke")
		->args({"self","ResultString"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_561 = das::das_call_member< FFieldVariant (FField::*)() const,&FField::GetOwnerVariant >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Field.h:583:16
	makeExtern<DAS_CALL_METHOD(_method_561), SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetOwnerVariant","das_call_member< FFieldVariant (FField::*)() const , &FField::GetOwnerVariant >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_562 = das::das_call_member< UObject * (FField::*)() const,&FField::GetOwnerUObject >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Field.h:589:23
	makeExtern<DAS_CALL_METHOD(_method_562), SimNode_ExtFuncCall >(lib,"GetOwnerUObject","das_call_member< UObject * (FField::*)() const , &FField::GetOwnerUObject >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_563 = das::das_call_member< UObject * (FField::*)() const,&FField::InternalGetOwnerAsUObjectUnsafe >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Field.h:601:23
	makeExtern<DAS_CALL_METHOD(_method_563), SimNode_ExtFuncCall >(lib,"InternalGetOwnerAsUObjectUnsafe","das_call_member< UObject * (FField::*)() const , &FField::InternalGetOwnerAsUObjectUnsafe >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_564 = das::das_call_member< UClass * (FField::*)() const,&FField::GetOwnerClass >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Field.h:607:26
	makeExtern<DAS_CALL_METHOD(_method_564), SimNode_ExtFuncCall >(lib,"GetOwnerClass","das_call_member< UClass * (FField::*)() const , &FField::GetOwnerClass >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_565 = das::das_call_member< UStruct * (FField::*)() const,&FField::GetOwnerStruct >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Field.h:610:27
	makeExtern<DAS_CALL_METHOD(_method_565), SimNode_ExtFuncCall >(lib,"GetOwnerStruct","das_call_member< UStruct * (FField::*)() const , &FField::GetOwnerStruct >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_566 = das::das_call_member< UField * (FField::*)() const,&FField::GetOwnerUField >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Field.h:613:26
	makeExtern<DAS_CALL_METHOD(_method_566), SimNode_ExtFuncCall >(lib,"GetOwnerUField","das_call_member< UField * (FField::*)() const , &FField::GetOwnerUField >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_567 = das::das_call_member< UPackage * (FField::*)() const,&FField::GetOutermost >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Field.h:616:28
	makeExtern<DAS_CALL_METHOD(_method_567), SimNode_ExtFuncCall >(lib,"GetOutermost","das_call_member< UPackage * (FField::*)() const , &FField::GetOutermost >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_568 = das::das_call_member< UObject * (FField::*)(UClass *) const,&FField::GetTypedOwner >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Field.h:619:27
	makeExtern<DAS_CALL_METHOD(_method_568), SimNode_ExtFuncCall >(lib,"GetTypedOwner","das_call_member< UObject * (FField::*)(UClass *) const , &FField::GetTypedOwner >::invoke")
		->args({"self","Target"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_569 = das::das_call_member< FField * (FField::*)(FFieldClass *) const,&FField::GetTypedOwner >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Field.h:622:26
	makeExtern<DAS_CALL_METHOD(_method_569), SimNode_ExtFuncCall >(lib,"GetTypedOwner","das_call_member< FField * (FField::*)(FFieldClass *) const , &FField::GetTypedOwner >::invoke")
		->args({"self","Target"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_570 = das::das_call_member< FName (FField::*)() const,&FField::GetFName >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Field.h:649:20
	makeExtern<DAS_CALL_METHOD(_method_570), SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetFName","das_call_member< FName (FField::*)() const , &FField::GetFName >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_571 = das::das_call_member< FString (FField::*)() const,&FField::GetName >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Field.h:661:22
	makeExtern<DAS_CALL_METHOD(_method_571), SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetName","das_call_member< FString (FField::*)() const , &FField::GetName >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_572 = das::das_call_member< void (FField::*)(FString &) const,&FField::GetName >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Field.h:673:19
	makeExtern<DAS_CALL_METHOD(_method_572), SimNode_ExtFuncCall >(lib,"GetName","das_call_member< void (FField::*)(FString &) const , &FField::GetName >::invoke")
		->args({"self","OutName"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_573 = das::das_call_member< void (FField::*)(const FName &),&FField::Rename >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Field.h:685:23
	makeExtern<DAS_CALL_METHOD(_method_573), SimNode_ExtFuncCall >(lib,"Rename","das_call_member< void (FField::*)(const FName &) , &FField::Rename >::invoke")
		->args({"self","NewName"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_574 = das::das_call_member< FString (FField::*)() const,&FField::GetFullName >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Field.h:689:26
	makeExtern<DAS_CALL_METHOD(_method_574), SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetFullName","das_call_member< FString (FField::*)() const , &FField::GetFullName >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_575 = das::das_call_member< FString (FField::*)() const,&FField::GetAuthoredName >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Field.h:696:26
	makeExtern<DAS_CALL_METHOD(_method_575), SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetAuthoredName","das_call_member< FString (FField::*)() const , &FField::GetAuthoredName >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Field.h:699:18
	makeExtern< FField * (*)(FField &,const FName &) , _dasUnreal_virtual_211_GetInnerFieldByName , SimNode_ExtFuncCall >(lib,"GetInnerFieldByName","_dasUnreal_virtual_211_GetInnerFieldByName")
		->args({"self","InName"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_577 = das::das_call_member< FString (FField::*)(bool) const,&FField::GetFullGroupName >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Field.h:723:26
	makeExtern<DAS_CALL_METHOD(_method_577), SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetFullGroupName","das_call_member< FString (FField::*)(bool) const , &FField::GetFullGroupName >::invoke")
		->args({"self","bStartWithOuter"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_578 = das::das_call_member< FText (FField::*)() const,&FField::GetDisplayNameText >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Field.h:730:24
	makeExtern<DAS_CALL_METHOD(_method_578), SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetDisplayNameText","das_call_member< FText (FField::*)() const , &FField::GetDisplayNameText >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_579 = das::das_call_member< FText (FField::*)(bool) const,&FField::GetToolTipText >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Field.h:739:24
	makeExtern<DAS_CALL_METHOD(_method_579), SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetToolTipText","das_call_member< FText (FField::*)(bool) const , &FField::GetToolTipText >::invoke")
		->args({"self","bShortTooltip"})
		->arg_init(1,make_smart<ExprConstBool>(false))
		->addToModule(*this, SideEffects::worstDefault);
	using _method_580 = das::das_call_member< bool (FField::*)(const wchar_t *) const,&FField::HasMetaData >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Field.h:747:7
	makeExtern<DAS_CALL_METHOD(_method_580), SimNode_ExtFuncCall >(lib,"HasMetaData","das_call_member< bool (FField::*)(const wchar_t *) const , &FField::HasMetaData >::invoke")
		->args({"self","Key"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_581 = das::das_call_member< bool (FField::*)(const FName &) const,&FField::HasMetaData >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Field.h:748:7
	makeExtern<DAS_CALL_METHOD(_method_581), SimNode_ExtFuncCall >(lib,"HasMetaData","das_call_member< bool (FField::*)(const FName &) const , &FField::HasMetaData >::invoke")
		->args({"self","Key"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_582 = das::das_call_member< const FString * (FField::*)(const wchar_t *) const,&FField::FindMetaData >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Field.h:756:33
	makeExtern<DAS_CALL_METHOD(_method_582), SimNode_ExtFuncCall >(lib,"FindMetaData","das_call_member< const FString * (FField::*)(const wchar_t *) const , &FField::FindMetaData >::invoke")
		->args({"self","Key"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_583 = das::das_call_member< const FString * (FField::*)(const FName &) const,&FField::FindMetaData >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Field.h:757:33
	makeExtern<DAS_CALL_METHOD(_method_583), SimNode_ExtFuncCall >(lib,"FindMetaData","das_call_member< const FString * (FField::*)(const FName &) const , &FField::FindMetaData >::invoke")
		->args({"self","Key"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_584 = das::das_call_member< const FString & (FField::*)(const wchar_t *) const,&FField::GetMetaData >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Field.h:765:33
	makeExtern<DAS_CALL_METHOD(_method_584), SimNode_ExtFuncCallRef >(lib,"GetMetaData","das_call_member< const FString & (FField::*)(const wchar_t *) const , &FField::GetMetaData >::invoke")
		->args({"self","Key"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_585 = das::das_call_member< const FString & (FField::*)(const FName &) const,&FField::GetMetaData >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Field.h:766:33
	makeExtern<DAS_CALL_METHOD(_method_585), SimNode_ExtFuncCallRef >(lib,"GetMetaData","das_call_member< const FString & (FField::*)(const FName &) const , &FField::GetMetaData >::invoke")
		->args({"self","Key"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_586 = das::das_call_member< const FText (FField::*)(const wchar_t *,const FString,const FString) const,&FField::GetMetaDataText >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Field.h:776:30
	makeExtern<DAS_CALL_METHOD(_method_586), SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetMetaDataText","das_call_member< const FText (FField::*)(const wchar_t *,const FString,const FString) const , &FField::GetMetaDataText >::invoke")
		->args({"self","MetaDataKey","LocalizationNamespace","LocalizationKey"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_587 = das::das_call_member< const FText (FField::*)(const FName &,const FString,const FString) const,&FField::GetMetaDataText >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Field.h:777:30
	makeExtern<DAS_CALL_METHOD(_method_587), SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetMetaDataText","das_call_member< const FText (FField::*)(const FName &,const FString,const FString) const , &FField::GetMetaDataText >::invoke")
		->args({"self","MetaDataKey","LocalizationNamespace","LocalizationKey"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_588 = das::das_call_member< void (FField::*)(const wchar_t *,const wchar_t *),&FField::SetMetaData >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Field.h:785:23
	makeExtern<DAS_CALL_METHOD(_method_588), SimNode_ExtFuncCall >(lib,"SetMetaData","das_call_member< void (FField::*)(const wchar_t *,const wchar_t *) , &FField::SetMetaData >::invoke")
		->args({"self","Key","InValue"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_589 = das::das_call_member< void (FField::*)(const FName &,const wchar_t *),&FField::SetMetaData >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Field.h:786:23
	makeExtern<DAS_CALL_METHOD(_method_589), SimNode_ExtFuncCall >(lib,"SetMetaData","das_call_member< void (FField::*)(const FName &,const wchar_t *) , &FField::SetMetaData >::invoke")
		->args({"self","Key","InValue"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_590 = das::das_call_member< bool (FField::*)(const wchar_t *) const,&FField::GetBoolMetaData >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Field.h:797:7
	makeExtern<DAS_CALL_METHOD(_method_590), SimNode_ExtFuncCall >(lib,"GetBoolMetaData","das_call_member< bool (FField::*)(const wchar_t *) const , &FField::GetBoolMetaData >::invoke")
		->args({"self","Key"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_591 = das::das_call_member< bool (FField::*)(const FName &) const,&FField::GetBoolMetaData >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Field.h:803:7
	makeExtern<DAS_CALL_METHOD(_method_591), SimNode_ExtFuncCall >(lib,"GetBoolMetaData","das_call_member< bool (FField::*)(const FName &) const , &FField::GetBoolMetaData >::invoke")
		->args({"self","Key"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_592 = das::das_call_member< int (FField::*)(const wchar_t *) const,&FField::GetIntMetaData >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Field.h:816:8
	makeExtern<DAS_CALL_METHOD(_method_592), SimNode_ExtFuncCall >(lib,"GetIntMetaData","das_call_member< int (FField::*)(const wchar_t *) const , &FField::GetIntMetaData >::invoke")
		->args({"self","Key"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_593 = das::das_call_member< int (FField::*)(const FName &) const,&FField::GetIntMetaData >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Field.h:822:8
	makeExtern<DAS_CALL_METHOD(_method_593), SimNode_ExtFuncCall >(lib,"GetIntMetaData","das_call_member< int (FField::*)(const FName &) const , &FField::GetIntMetaData >::invoke")
		->args({"self","Key"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_594 = das::das_call_member< float (FField::*)(const wchar_t *) const,&FField::GetFloatMetaData >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Field.h:835:8
	makeExtern<DAS_CALL_METHOD(_method_594), SimNode_ExtFuncCall >(lib,"GetFloatMetaData","das_call_member< float (FField::*)(const wchar_t *) const , &FField::GetFloatMetaData >::invoke")
		->args({"self","Key"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_595 = das::das_call_member< float (FField::*)(const FName &) const,&FField::GetFloatMetaData >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Field.h:842:8
	makeExtern<DAS_CALL_METHOD(_method_595), SimNode_ExtFuncCall >(lib,"GetFloatMetaData","das_call_member< float (FField::*)(const FName &) const , &FField::GetFloatMetaData >::invoke")
		->args({"self","Key"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_596 = das::das_call_member< double (FField::*)(const wchar_t *) const,&FField::GetDoubleMetaData >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Field.h:856:9
	makeExtern<DAS_CALL_METHOD(_method_596), SimNode_ExtFuncCall >(lib,"GetDoubleMetaData","das_call_member< double (FField::*)(const wchar_t *) const , &FField::GetDoubleMetaData >::invoke")
		->args({"self","Key"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_597 = das::das_call_member< double (FField::*)(const FName &) const,&FField::GetDoubleMetaData >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Field.h:863:9
	makeExtern<DAS_CALL_METHOD(_method_597), SimNode_ExtFuncCall >(lib,"GetDoubleMetaData","das_call_member< double (FField::*)(const FName &) const , &FField::GetDoubleMetaData >::invoke")
		->args({"self","Key"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_598 = das::das_call_member< UClass * (FField::*)(const wchar_t *) const,&FField::GetClassMetaData >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Field.h:877:26
	makeExtern<DAS_CALL_METHOD(_method_598), SimNode_ExtFuncCall >(lib,"GetClassMetaData","das_call_member< UClass * (FField::*)(const wchar_t *) const , &FField::GetClassMetaData >::invoke")
		->args({"self","Key"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_599 = das::das_call_member< UClass * (FField::*)(const FName &) const,&FField::GetClassMetaData >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Field.h:878:26
	makeExtern<DAS_CALL_METHOD(_method_599), SimNode_ExtFuncCall >(lib,"GetClassMetaData","das_call_member< UClass * (FField::*)(const FName &) const , &FField::GetClassMetaData >::invoke")
		->args({"self","Key"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_600 = das::das_call_member< void (FField::*)(const wchar_t *),&FField::RemoveMetaData >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Field.h:881:23
	makeExtern<DAS_CALL_METHOD(_method_600), SimNode_ExtFuncCall >(lib,"RemoveMetaData","das_call_member< void (FField::*)(const wchar_t *) , &FField::RemoveMetaData >::invoke")
		->args({"self","Key"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_601 = das::das_call_member< void (FField::*)(const FName &),&FField::RemoveMetaData >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Field.h:882:23
	makeExtern<DAS_CALL_METHOD(_method_601), SimNode_ExtFuncCall >(lib,"RemoveMetaData","das_call_member< void (FField::*)(const FName &) , &FField::RemoveMetaData >::invoke")
		->args({"self","Key"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Field.h:891:30
	makeExtern< void (*)(const FField *,const FField *,FField *) , _dasUnreal_static_212_CopyMetaData , SimNode_ExtFuncCall >(lib,"CopyMetaData","_dasUnreal_static_212_CopyMetaData")
		->args({"self","InSourceField","InDestField"})
		->arg_type(0,makeType<TExplicit<const FField*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Field.h:894:33
	makeExtern< FField * (*)(const FField *,UField *) , _dasUnreal_static_213_CreateFromUField , SimNode_ExtFuncCall >(lib,"CreateFromUField","_dasUnreal_static_213_CreateFromUField")
		->args({"self","InField"})
		->arg_type(0,makeType<TExplicit<const FField*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Field.h:903:33
	makeExtern< FField * (*)(const FField *,const FField *,FFieldVariant,const FName,EObjectFlags,EInternalObjectFlags) , _dasUnreal_static_214_Duplicate , SimNode_ExtFuncCall >(lib,"Duplicate","_dasUnreal_static_214_Duplicate")
		->args({"self","InField","DestOwner","DestName","FlagMask","InternalFlagsMask"})
		->arg_type(0,makeType<TExplicit<const FField*> >(lib))
		->arg_init(4,make_smart<ExprConstEnumeration>(-1,makeType<EObjectFlags>(lib)))
		->arg_init(5,make_smart<ExprConstEnumeration>(-5242880,makeType<EInternalObjectFlags>(lib)))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Field.h:906:31
	makeExtern< FName (*)(const FField *,FFieldVariant,FFieldClass *) , _dasUnreal_static_215_GenerateFFieldName , SimNode_ExtFuncCallAndCopyOrMove >(lib,"GenerateFFieldName","_dasUnreal_static_215_GenerateFFieldName")
		->args({"self","InOwner","InClass"})
		->arg_type(0,makeType<TExplicit<const FField*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Field.h:1022:16
	makeExtern< FString (*)(const FField *) , GetNameSafe , SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetNameSafe","GetNameSafe")
		->args({"InField"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Field.h:1036:25
	makeExtern< FString (*)(const FField *) , GetFullNameSafe , SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetFullNameSafe","GetFullNameSafe")
		->args({"InField"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Field.h:1040:25
	makeExtern< FString (*)(const FField *) , GetPathNameSafe , SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetPathNameSafe","GetPathNameSafe")
		->args({"InField"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Field.h:1044:25
	makeExtern< FField * (*)(const wchar_t *) , FindFPropertyByPath , SimNode_ExtFuncCall >(lib,"FindFPropertyByPath","FindFPropertyByPath")
		->args({"InFieldPath"})
		->addToModule(*this, SideEffects::worstDefault);
	addCtorAndUsing<FObjectPtr>(*this,lib,"FObjectPtr","FObjectPtr");
	addCtorAndUsing<FObjectPtr,UObject *>(*this,lib,"FObjectPtr","FObjectPtr")
		->args({"Object"});
	addCtorAndUsing<FObjectPtr,void *>(*this,lib,"FObjectPtr","FObjectPtr")
		->args({"IncompleteObject"});
	using _method_606 = das::das_call_member< UObject * (FObjectPtr::*)() const,&FObjectPtr::Get >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/ObjectPtr.h:91:23
	makeExtern<DAS_CALL_METHOD(_method_606), SimNode_ExtFuncCall >(lib,"Get","das_call_member< UObject * (FObjectPtr::*)() const , &FObjectPtr::Get >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_607 = das::das_call_member< UClass * (FObjectPtr::*)() const,&FObjectPtr::GetClass >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/ObjectPtr.h:96:22
	makeExtern<DAS_CALL_METHOD(_method_607), SimNode_ExtFuncCall >(lib,"GetClass","das_call_member< UClass * (FObjectPtr::*)() const , &FObjectPtr::GetClass >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	addCtorAndUsing<FObjectPtr,const FObjectPtr &>(*this,lib,"FObjectPtr","FObjectPtr")
		->args({""});
	using _method_608 = das::das_call_member< TObjectPtr_UObject & (FObjectPtr::*)(),&FObjectPtr::ToTObjectPtr >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/ObjectPtr.h:129:35
	makeExtern<DAS_CALL_METHOD(_method_608), SimNode_ExtFuncCallRef >(lib,"ToTObjectPtr","das_call_member< TObjectPtr_UObject & (FObjectPtr::*)() , &FObjectPtr::ToTObjectPtr >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_609 = das::das_call_member< const TObjectPtr_UObject & (FObjectPtr::*)() const,&FObjectPtr::ToTObjectPtr >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/ObjectPtr.h:131:41
	makeExtern<DAS_CALL_METHOD(_method_609), SimNode_ExtFuncCallRef >(lib,"ToTObjectPtr","das_call_member< const TObjectPtr_UObject & (FObjectPtr::*)() const , &FObjectPtr::ToTObjectPtr >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_610 = das::das_call_member< bool (FObjectPtr::*)() const,&FObjectPtr::IsResolved >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/ObjectPtr.h:145:19
	makeExtern<DAS_CALL_METHOD(_method_610), SimNode_ExtFuncCall >(lib,"IsResolved","das_call_member< bool (FObjectPtr::*)() const , &FObjectPtr::IsResolved >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_611 = das::das_call_member< FString (FObjectPtr::*)() const,&FObjectPtr::GetPath >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/ObjectPtr.h:149:10
	makeExtern<DAS_CALL_METHOD(_method_611), SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetPath","das_call_member< FString (FObjectPtr::*)() const , &FObjectPtr::GetPath >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_612 = das::das_call_member< FString (FObjectPtr::*)() const,&FObjectPtr::GetPathName >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/ObjectPtr.h:153:26
	makeExtern<DAS_CALL_METHOD(_method_612), SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetPathName","das_call_member< FString (FObjectPtr::*)() const , &FObjectPtr::GetPathName >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_613 = das::das_call_member< FName (FObjectPtr::*)() const,&FObjectPtr::GetFName >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/ObjectPtr.h:156:24
	makeExtern<DAS_CALL_METHOD(_method_613), SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetFName","das_call_member< FName (FObjectPtr::*)() const , &FObjectPtr::GetFName >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_614 = das::das_call_member< FString (FObjectPtr::*)() const,&FObjectPtr::GetName >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/ObjectPtr.h:159:22
	makeExtern<DAS_CALL_METHOD(_method_614), SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetName","das_call_member< FString (FObjectPtr::*)() const , &FObjectPtr::GetName >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_615 = das::das_call_member< FObjectPtr (FObjectPtr::*)() const,&FObjectPtr::GetOuter >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/ObjectPtr.h:166:29
	makeExtern<DAS_CALL_METHOD(_method_615), SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetOuter","das_call_member< FObjectPtr (FObjectPtr::*)() const , &FObjectPtr::GetOuter >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_616 = das::das_call_member< FObjectPtr (FObjectPtr::*)() const,&FObjectPtr::GetPackage >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/ObjectPtr.h:170:29
	makeExtern<DAS_CALL_METHOD(_method_616), SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetPackage","das_call_member< FObjectPtr (FObjectPtr::*)() const , &FObjectPtr::GetPackage >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_617 = das::das_call_member< FString (FObjectPtr::*)(EObjectFullNameFlags) const,&FObjectPtr::GetFullName >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/ObjectPtr.h:173:26
	makeExtern<DAS_CALL_METHOD(_method_617), SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetFullName","das_call_member< FString (FObjectPtr::*)(EObjectFullNameFlags) const , &FObjectPtr::GetFullName >::invoke")
		->args({"self","Flags"})
		->arg_init(1,make_smart<ExprConstEnumeration>(0,makeType<EObjectFullNameFlags>(lib)))
		->addToModule(*this, SideEffects::worstDefault);
	using _method_618 = das::das_call_member< bool (FObjectPtr::*)(const UClass *) const,&FObjectPtr::IsA >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/ObjectPtr.h:217:23
	makeExtern<DAS_CALL_METHOD(_method_618), SimNode_ExtFuncCall >(lib,"IsA","das_call_member< bool (FObjectPtr::*)(const UClass *) const , &FObjectPtr::IsA >::invoke")
		->args({"self","SomeBase"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:157:2
	makeExtern< UClass * (*)(const UField *) , _dasUnreal_static_216_StaticClass , SimNode_ExtFuncCall >(lib,"StaticClass","_dasUnreal_static_216_StaticClass")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const UField*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
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
}
}

