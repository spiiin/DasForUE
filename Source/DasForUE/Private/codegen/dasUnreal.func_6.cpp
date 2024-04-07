// this file is generated via daScript automatic C++ binder
// all user modifications will be lost after this file is re-generated

#include "dasUnreal.h"
#include "need_dasUnreal.h"
namespace das {
#include "dasUnreal.func.aot.decl.inc"
void Module_dasUnreal::initFunctions_6() {
	using _method_352 = das::das_call_member< FFieldClass * (FField::*)() const,&FField::GetClass >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Field.h:537:22
	makeExtern<DAS_CALL_METHOD(_method_352), SimNode_ExtFuncCall >(lib,"GetClass","das_call_member< FFieldClass * (FField::*)() const , &FField::GetClass >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_353 = das::das_call_member< uint64 (FField::*)() const,&FField::GetCastFlags >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Field.h:541:16
	makeExtern<DAS_CALL_METHOD(_method_353), SimNode_ExtFuncCall >(lib,"GetCastFlags","das_call_member< uint64 (FField::*)() const , &FField::GetCastFlags >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_354 = das::das_call_member< bool (FField::*)(const FFieldClass *) const,&FField::IsA >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Field.h:546:14
	makeExtern<DAS_CALL_METHOD(_method_354), SimNode_ExtFuncCall >(lib,"IsA","das_call_member< bool (FField::*)(const FFieldClass *) const , &FField::IsA >::invoke")
		->args({"self","FieldType"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_355 = das::das_call_member< bool (FField::*)(const uint64) const,&FField::HasAnyCastFlags >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Field.h:566:14
	makeExtern<DAS_CALL_METHOD(_method_355), SimNode_ExtFuncCall >(lib,"HasAnyCastFlags","das_call_member< bool (FField::*)(const uint64) const , &FField::HasAnyCastFlags >::invoke")
		->args({"self","InCastFlags"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_356 = das::das_call_member< bool (FField::*)(const uint64) const,&FField::HasAllCastFlags >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Field.h:572:14
	makeExtern<DAS_CALL_METHOD(_method_356), SimNode_ExtFuncCall >(lib,"HasAllCastFlags","das_call_member< bool (FField::*)(const uint64) const , &FField::HasAllCastFlags >::invoke")
		->args({"self","InCastFlags"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_357 = das::das_call_member< void (FField::*)(FString &) const,&FField::AppendName >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Field.h:577:7
	makeExtern<DAS_CALL_METHOD(_method_357), SimNode_ExtFuncCall >(lib,"AppendName","das_call_member< void (FField::*)(FString &) const , &FField::AppendName >::invoke")
		->args({"self","ResultString"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_358 = das::das_call_member< FFieldVariant (FField::*)() const,&FField::GetOwnerVariant >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Field.h:583:16
	makeExtern<DAS_CALL_METHOD(_method_358), SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetOwnerVariant","das_call_member< FFieldVariant (FField::*)() const , &FField::GetOwnerVariant >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_359 = das::das_call_member< UObject * (FField::*)() const,&FField::GetOwnerUObject >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Field.h:589:23
	makeExtern<DAS_CALL_METHOD(_method_359), SimNode_ExtFuncCall >(lib,"GetOwnerUObject","das_call_member< UObject * (FField::*)() const , &FField::GetOwnerUObject >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_360 = das::das_call_member< UObject * (FField::*)() const,&FField::InternalGetOwnerAsUObjectUnsafe >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Field.h:601:23
	makeExtern<DAS_CALL_METHOD(_method_360), SimNode_ExtFuncCall >(lib,"InternalGetOwnerAsUObjectUnsafe","das_call_member< UObject * (FField::*)() const , &FField::InternalGetOwnerAsUObjectUnsafe >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_361 = das::das_call_member< UClass * (FField::*)() const,&FField::GetOwnerClass >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Field.h:607:26
	makeExtern<DAS_CALL_METHOD(_method_361), SimNode_ExtFuncCall >(lib,"GetOwnerClass","das_call_member< UClass * (FField::*)() const , &FField::GetOwnerClass >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_362 = das::das_call_member< UStruct * (FField::*)() const,&FField::GetOwnerStruct >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Field.h:610:27
	makeExtern<DAS_CALL_METHOD(_method_362), SimNode_ExtFuncCall >(lib,"GetOwnerStruct","das_call_member< UStruct * (FField::*)() const , &FField::GetOwnerStruct >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_363 = das::das_call_member< UField * (FField::*)() const,&FField::GetOwnerUField >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Field.h:613:26
	makeExtern<DAS_CALL_METHOD(_method_363), SimNode_ExtFuncCall >(lib,"GetOwnerUField","das_call_member< UField * (FField::*)() const , &FField::GetOwnerUField >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_364 = das::das_call_member< UPackage * (FField::*)() const,&FField::GetOutermost >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Field.h:616:28
	makeExtern<DAS_CALL_METHOD(_method_364), SimNode_ExtFuncCall >(lib,"GetOutermost","das_call_member< UPackage * (FField::*)() const , &FField::GetOutermost >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_365 = das::das_call_member< UObject * (FField::*)(UClass *) const,&FField::GetTypedOwner >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Field.h:619:27
	makeExtern<DAS_CALL_METHOD(_method_365), SimNode_ExtFuncCall >(lib,"GetTypedOwner","das_call_member< UObject * (FField::*)(UClass *) const , &FField::GetTypedOwner >::invoke")
		->args({"self","Target"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_366 = das::das_call_member< FField * (FField::*)(FFieldClass *) const,&FField::GetTypedOwner >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Field.h:622:26
	makeExtern<DAS_CALL_METHOD(_method_366), SimNode_ExtFuncCall >(lib,"GetTypedOwner","das_call_member< FField * (FField::*)(FFieldClass *) const , &FField::GetTypedOwner >::invoke")
		->args({"self","Target"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_367 = das::das_call_member< FName (FField::*)() const,&FField::GetFName >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Field.h:649:20
	makeExtern<DAS_CALL_METHOD(_method_367), SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetFName","das_call_member< FName (FField::*)() const , &FField::GetFName >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_368 = das::das_call_member< FString (FField::*)() const,&FField::GetName >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Field.h:661:22
	makeExtern<DAS_CALL_METHOD(_method_368), SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetName","das_call_member< FString (FField::*)() const , &FField::GetName >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_369 = das::das_call_member< void (FField::*)(FString &) const,&FField::GetName >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Field.h:673:19
	makeExtern<DAS_CALL_METHOD(_method_369), SimNode_ExtFuncCall >(lib,"GetName","das_call_member< void (FField::*)(FString &) const , &FField::GetName >::invoke")
		->args({"self","OutName"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_370 = das::das_call_member< void (FField::*)(const FName &),&FField::Rename >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Field.h:685:23
	makeExtern<DAS_CALL_METHOD(_method_370), SimNode_ExtFuncCall >(lib,"Rename","das_call_member< void (FField::*)(const FName &) , &FField::Rename >::invoke")
		->args({"self","NewName"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_371 = das::das_call_member< FString (FField::*)() const,&FField::GetFullName >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Field.h:689:26
	makeExtern<DAS_CALL_METHOD(_method_371), SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetFullName","das_call_member< FString (FField::*)() const , &FField::GetFullName >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_372 = das::das_call_member< FString (FField::*)() const,&FField::GetAuthoredName >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Field.h:696:26
	makeExtern<DAS_CALL_METHOD(_method_372), SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetAuthoredName","das_call_member< FString (FField::*)() const , &FField::GetAuthoredName >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Field.h:699:18
	makeExtern< FField * (*)(FField &,const FName &) , _dasUnreal_virtual_118_GetInnerFieldByName , SimNode_ExtFuncCall >(lib,"GetInnerFieldByName","_dasUnreal_virtual_118_GetInnerFieldByName")
		->args({"self","InName"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_374 = das::das_call_member< FString (FField::*)(bool) const,&FField::GetFullGroupName >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Field.h:723:26
	makeExtern<DAS_CALL_METHOD(_method_374), SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetFullGroupName","das_call_member< FString (FField::*)(bool) const , &FField::GetFullGroupName >::invoke")
		->args({"self","bStartWithOuter"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_375 = das::das_call_member< FText (FField::*)() const,&FField::GetDisplayNameText >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Field.h:730:24
	makeExtern<DAS_CALL_METHOD(_method_375), SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetDisplayNameText","das_call_member< FText (FField::*)() const , &FField::GetDisplayNameText >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_376 = das::das_call_member< FText (FField::*)(bool) const,&FField::GetToolTipText >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Field.h:739:24
	makeExtern<DAS_CALL_METHOD(_method_376), SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetToolTipText","das_call_member< FText (FField::*)(bool) const , &FField::GetToolTipText >::invoke")
		->args({"self","bShortTooltip"})
		->arg_init(1,make_smart<ExprConstBool>(false))
		->addToModule(*this, SideEffects::worstDefault);
	using _method_377 = das::das_call_member< bool (FField::*)(const wchar_t *) const,&FField::HasMetaData >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Field.h:747:7
	makeExtern<DAS_CALL_METHOD(_method_377), SimNode_ExtFuncCall >(lib,"HasMetaData","das_call_member< bool (FField::*)(const wchar_t *) const , &FField::HasMetaData >::invoke")
		->args({"self","Key"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_378 = das::das_call_member< bool (FField::*)(const FName &) const,&FField::HasMetaData >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Field.h:748:7
	makeExtern<DAS_CALL_METHOD(_method_378), SimNode_ExtFuncCall >(lib,"HasMetaData","das_call_member< bool (FField::*)(const FName &) const , &FField::HasMetaData >::invoke")
		->args({"self","Key"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_379 = das::das_call_member< const FString * (FField::*)(const wchar_t *) const,&FField::FindMetaData >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Field.h:756:33
	makeExtern<DAS_CALL_METHOD(_method_379), SimNode_ExtFuncCall >(lib,"FindMetaData","das_call_member< const FString * (FField::*)(const wchar_t *) const , &FField::FindMetaData >::invoke")
		->args({"self","Key"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_380 = das::das_call_member< const FString * (FField::*)(const FName &) const,&FField::FindMetaData >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Field.h:757:33
	makeExtern<DAS_CALL_METHOD(_method_380), SimNode_ExtFuncCall >(lib,"FindMetaData","das_call_member< const FString * (FField::*)(const FName &) const , &FField::FindMetaData >::invoke")
		->args({"self","Key"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_381 = das::das_call_member< const FString & (FField::*)(const wchar_t *) const,&FField::GetMetaData >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Field.h:765:33
	makeExtern<DAS_CALL_METHOD(_method_381), SimNode_ExtFuncCallRef >(lib,"GetMetaData","das_call_member< const FString & (FField::*)(const wchar_t *) const , &FField::GetMetaData >::invoke")
		->args({"self","Key"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_382 = das::das_call_member< const FString & (FField::*)(const FName &) const,&FField::GetMetaData >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Field.h:766:33
	makeExtern<DAS_CALL_METHOD(_method_382), SimNode_ExtFuncCallRef >(lib,"GetMetaData","das_call_member< const FString & (FField::*)(const FName &) const , &FField::GetMetaData >::invoke")
		->args({"self","Key"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_383 = das::das_call_member< const FText (FField::*)(const wchar_t *,const FString,const FString) const,&FField::GetMetaDataText >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Field.h:776:30
	makeExtern<DAS_CALL_METHOD(_method_383), SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetMetaDataText","das_call_member< const FText (FField::*)(const wchar_t *,const FString,const FString) const , &FField::GetMetaDataText >::invoke")
		->args({"self","MetaDataKey","LocalizationNamespace","LocalizationKey"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_384 = das::das_call_member< const FText (FField::*)(const FName &,const FString,const FString) const,&FField::GetMetaDataText >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Field.h:777:30
	makeExtern<DAS_CALL_METHOD(_method_384), SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetMetaDataText","das_call_member< const FText (FField::*)(const FName &,const FString,const FString) const , &FField::GetMetaDataText >::invoke")
		->args({"self","MetaDataKey","LocalizationNamespace","LocalizationKey"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_385 = das::das_call_member< void (FField::*)(const wchar_t *,const wchar_t *),&FField::SetMetaData >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Field.h:785:23
	makeExtern<DAS_CALL_METHOD(_method_385), SimNode_ExtFuncCall >(lib,"SetMetaData","das_call_member< void (FField::*)(const wchar_t *,const wchar_t *) , &FField::SetMetaData >::invoke")
		->args({"self","Key","InValue"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_386 = das::das_call_member< void (FField::*)(const FName &,const wchar_t *),&FField::SetMetaData >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Field.h:786:23
	makeExtern<DAS_CALL_METHOD(_method_386), SimNode_ExtFuncCall >(lib,"SetMetaData","das_call_member< void (FField::*)(const FName &,const wchar_t *) , &FField::SetMetaData >::invoke")
		->args({"self","Key","InValue"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_387 = das::das_call_member< bool (FField::*)(const wchar_t *) const,&FField::GetBoolMetaData >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Field.h:797:7
	makeExtern<DAS_CALL_METHOD(_method_387), SimNode_ExtFuncCall >(lib,"GetBoolMetaData","das_call_member< bool (FField::*)(const wchar_t *) const , &FField::GetBoolMetaData >::invoke")
		->args({"self","Key"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_388 = das::das_call_member< bool (FField::*)(const FName &) const,&FField::GetBoolMetaData >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Field.h:803:7
	makeExtern<DAS_CALL_METHOD(_method_388), SimNode_ExtFuncCall >(lib,"GetBoolMetaData","das_call_member< bool (FField::*)(const FName &) const , &FField::GetBoolMetaData >::invoke")
		->args({"self","Key"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_389 = das::das_call_member< int (FField::*)(const wchar_t *) const,&FField::GetIntMetaData >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Field.h:816:8
	makeExtern<DAS_CALL_METHOD(_method_389), SimNode_ExtFuncCall >(lib,"GetIntMetaData","das_call_member< int (FField::*)(const wchar_t *) const , &FField::GetIntMetaData >::invoke")
		->args({"self","Key"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_390 = das::das_call_member< int (FField::*)(const FName &) const,&FField::GetIntMetaData >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Field.h:822:8
	makeExtern<DAS_CALL_METHOD(_method_390), SimNode_ExtFuncCall >(lib,"GetIntMetaData","das_call_member< int (FField::*)(const FName &) const , &FField::GetIntMetaData >::invoke")
		->args({"self","Key"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_391 = das::das_call_member< float (FField::*)(const wchar_t *) const,&FField::GetFloatMetaData >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Field.h:835:8
	makeExtern<DAS_CALL_METHOD(_method_391), SimNode_ExtFuncCall >(lib,"GetFloatMetaData","das_call_member< float (FField::*)(const wchar_t *) const , &FField::GetFloatMetaData >::invoke")
		->args({"self","Key"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_392 = das::das_call_member< float (FField::*)(const FName &) const,&FField::GetFloatMetaData >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Field.h:842:8
	makeExtern<DAS_CALL_METHOD(_method_392), SimNode_ExtFuncCall >(lib,"GetFloatMetaData","das_call_member< float (FField::*)(const FName &) const , &FField::GetFloatMetaData >::invoke")
		->args({"self","Key"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_393 = das::das_call_member< double (FField::*)(const wchar_t *) const,&FField::GetDoubleMetaData >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Field.h:856:9
	makeExtern<DAS_CALL_METHOD(_method_393), SimNode_ExtFuncCall >(lib,"GetDoubleMetaData","das_call_member< double (FField::*)(const wchar_t *) const , &FField::GetDoubleMetaData >::invoke")
		->args({"self","Key"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_394 = das::das_call_member< double (FField::*)(const FName &) const,&FField::GetDoubleMetaData >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Field.h:863:9
	makeExtern<DAS_CALL_METHOD(_method_394), SimNode_ExtFuncCall >(lib,"GetDoubleMetaData","das_call_member< double (FField::*)(const FName &) const , &FField::GetDoubleMetaData >::invoke")
		->args({"self","Key"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_395 = das::das_call_member< UClass * (FField::*)(const wchar_t *) const,&FField::GetClassMetaData >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Field.h:877:26
	makeExtern<DAS_CALL_METHOD(_method_395), SimNode_ExtFuncCall >(lib,"GetClassMetaData","das_call_member< UClass * (FField::*)(const wchar_t *) const , &FField::GetClassMetaData >::invoke")
		->args({"self","Key"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_396 = das::das_call_member< UClass * (FField::*)(const FName &) const,&FField::GetClassMetaData >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Field.h:878:26
	makeExtern<DAS_CALL_METHOD(_method_396), SimNode_ExtFuncCall >(lib,"GetClassMetaData","das_call_member< UClass * (FField::*)(const FName &) const , &FField::GetClassMetaData >::invoke")
		->args({"self","Key"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_397 = das::das_call_member< void (FField::*)(const wchar_t *),&FField::RemoveMetaData >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Field.h:881:23
	makeExtern<DAS_CALL_METHOD(_method_397), SimNode_ExtFuncCall >(lib,"RemoveMetaData","das_call_member< void (FField::*)(const wchar_t *) , &FField::RemoveMetaData >::invoke")
		->args({"self","Key"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_398 = das::das_call_member< void (FField::*)(const FName &),&FField::RemoveMetaData >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Field.h:882:23
	makeExtern<DAS_CALL_METHOD(_method_398), SimNode_ExtFuncCall >(lib,"RemoveMetaData","das_call_member< void (FField::*)(const FName &) , &FField::RemoveMetaData >::invoke")
		->args({"self","Key"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Field.h:891:30
	makeExtern< void (*)(const FField *,const FField *,FField *) , _dasUnreal_static_119_CopyMetaData , SimNode_ExtFuncCall >(lib,"CopyMetaData","_dasUnreal_static_119_CopyMetaData")
		->args({"self","InSourceField","InDestField"})
		->arg_type(0,makeType<TExplicit<const FField*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Field.h:894:33
	makeExtern< FField * (*)(const FField *,UField *) , _dasUnreal_static_120_CreateFromUField , SimNode_ExtFuncCall >(lib,"CreateFromUField","_dasUnreal_static_120_CreateFromUField")
		->args({"self","InField"})
		->arg_type(0,makeType<TExplicit<const FField*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Field.h:903:33
	makeExtern< FField * (*)(const FField *,const FField *,FFieldVariant,const FName,EObjectFlags,EInternalObjectFlags) , _dasUnreal_static_121_Duplicate , SimNode_ExtFuncCall >(lib,"Duplicate","_dasUnreal_static_121_Duplicate")
		->args({"self","InField","DestOwner","DestName","FlagMask","InternalFlagsMask"})
		->arg_type(0,makeType<TExplicit<const FField*> >(lib))
		->arg_init(4,make_smart<ExprConstEnumeration>(-1,makeType<EObjectFlags>(lib)))
		->arg_init(5,make_smart<ExprConstEnumeration>(-5242880,makeType<EInternalObjectFlags>(lib)))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Field.h:906:31
	makeExtern< FName (*)(const FField *,FFieldVariant,FFieldClass *) , _dasUnreal_static_122_GenerateFFieldName , SimNode_ExtFuncCallAndCopyOrMove >(lib,"GenerateFFieldName","_dasUnreal_static_122_GenerateFFieldName")
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
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:157:2
	makeExtern< UClass * (*)(const UField *) , _dasUnreal_static_123_StaticClass , SimNode_ExtFuncCall >(lib,"StaticClass","_dasUnreal_static_123_StaticClass")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const UField*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:157:2
	makeExtern< const wchar_t * (*)(const UField *) , _dasUnreal_static_124_StaticPackage , SimNode_ExtFuncCall >(lib,"StaticPackage","_dasUnreal_static_124_StaticPackage")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const UField*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:157:2
	makeExtern< void (*)(const UField *) , _dasUnreal_static_125_StaticRegisterNativesUField , SimNode_ExtFuncCall >(lib,"StaticRegisterNativesUField","_dasUnreal_static_125_StaticRegisterNativesUField")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const UField*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:170:31
	makeExtern< void (*)(UField &) , _dasUnreal_virtual_126_PostLoad , SimNode_ExtFuncCall >(lib,"PostLoad","_dasUnreal_virtual_126_PostLoad")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:171:31
	makeExtern< bool (*)(const UField &) , _dasUnreal_virtual_127_NeedsLoadForClient , SimNode_ExtFuncCall >(lib,"NeedsLoadForClient","_dasUnreal_virtual_127_NeedsLoadForClient")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:172:31
	makeExtern< bool (*)(const UField &) , _dasUnreal_virtual_128_NeedsLoadForServer , SimNode_ExtFuncCall >(lib,"NeedsLoadForServer","_dasUnreal_virtual_128_NeedsLoadForServer")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:176:31
	makeExtern< void (*)(UField &) , _dasUnreal_virtual_129_Bind , SimNode_ExtFuncCall >(lib,"Bind","_dasUnreal_virtual_129_Bind")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_410 = das::das_call_member< UClass * (UField::*)() const,&UField::GetOwnerClass >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:179:26
	makeExtern<DAS_CALL_METHOD(_method_410), SimNode_ExtFuncCall >(lib,"GetOwnerClass","das_call_member< UClass * (UField::*)() const , &UField::GetOwnerClass >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_411 = das::das_call_member< UStruct * (UField::*)() const,&UField::GetOwnerStruct >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:182:27
	makeExtern<DAS_CALL_METHOD(_method_411), SimNode_ExtFuncCall >(lib,"GetOwnerStruct","das_call_member< UStruct * (UField::*)() const , &UField::GetOwnerStruct >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_412 = das::das_call_member< FString (UField::*)() const,&UField::GetAuthoredName >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:189:26
	makeExtern<DAS_CALL_METHOD(_method_412), SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetAuthoredName","das_call_member< FString (UField::*)() const , &UField::GetAuthoredName >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_413 = das::das_call_member< FText (UField::*)() const,&UField::GetDisplayNameText >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:197:24
	makeExtern<DAS_CALL_METHOD(_method_413), SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetDisplayNameText","das_call_member< FText (UField::*)() const , &UField::GetDisplayNameText >::invoke")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_414 = das::das_call_member< FText (UField::*)(bool) const,&UField::GetToolTipText >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:206:24
	makeExtern<DAS_CALL_METHOD(_method_414), SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetToolTipText","das_call_member< FText (UField::*)(bool) const , &UField::GetToolTipText >::invoke")
		->args({"self","bShortTooltip"})
		->arg_init(1,make_smart<ExprConstBool>(false))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:214:30
	makeExtern< void (*)(const UField *,FString &,bool) , _dasUnreal_static_130_FormatNativeToolTip , SimNode_ExtFuncCall >(lib,"FormatNativeToolTip","_dasUnreal_static_130_FormatNativeToolTip")
		->args({"self","ToolTipString","bRemoveExtraSections"})
		->arg_type(0,makeType<TExplicit<const UField*> >(lib))
		->arg_init(2,make_smart<ExprConstBool>(true))
		->addToModule(*this, SideEffects::worstDefault);
	using _method_416 = das::das_call_member< bool (UField::*)(const wchar_t *) const,&UField::HasMetaData >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:222:7
	makeExtern<DAS_CALL_METHOD(_method_416), SimNode_ExtFuncCall >(lib,"HasMetaData","das_call_member< bool (UField::*)(const wchar_t *) const , &UField::HasMetaData >::invoke")
		->args({"self","Key"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_417 = das::das_call_member< bool (UField::*)(const FName &) const,&UField::HasMetaData >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:223:7
	makeExtern<DAS_CALL_METHOD(_method_417), SimNode_ExtFuncCall >(lib,"HasMetaData","das_call_member< bool (UField::*)(const FName &) const , &UField::HasMetaData >::invoke")
		->args({"self","Key"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_418 = das::das_call_member< const FString * (UField::*)(const wchar_t *) const,&UField::FindMetaData >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:231:33
	makeExtern<DAS_CALL_METHOD(_method_418), SimNode_ExtFuncCall >(lib,"FindMetaData","das_call_member< const FString * (UField::*)(const wchar_t *) const , &UField::FindMetaData >::invoke")
		->args({"self","Key"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_419 = das::das_call_member< const FString * (UField::*)(const FName &) const,&UField::FindMetaData >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:232:33
	makeExtern<DAS_CALL_METHOD(_method_419), SimNode_ExtFuncCall >(lib,"FindMetaData","das_call_member< const FString * (UField::*)(const FName &) const , &UField::FindMetaData >::invoke")
		->args({"self","Key"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_420 = das::das_call_member< const FString & (UField::*)(const wchar_t *) const,&UField::GetMetaData >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:240:33
	makeExtern<DAS_CALL_METHOD(_method_420), SimNode_ExtFuncCallRef >(lib,"GetMetaData","das_call_member< const FString & (UField::*)(const wchar_t *) const , &UField::GetMetaData >::invoke")
		->args({"self","Key"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_421 = das::das_call_member< const FString & (UField::*)(const FName &) const,&UField::GetMetaData >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:241:33
	makeExtern<DAS_CALL_METHOD(_method_421), SimNode_ExtFuncCallRef >(lib,"GetMetaData","das_call_member< const FString & (UField::*)(const FName &) const , &UField::GetMetaData >::invoke")
		->args({"self","Key"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_422 = das::das_call_member< FText (UField::*)(const wchar_t *,const FString,const FString) const,&UField::GetMetaDataText >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:251:24
	makeExtern<DAS_CALL_METHOD(_method_422), SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetMetaDataText","das_call_member< FText (UField::*)(const wchar_t *,const FString,const FString) const , &UField::GetMetaDataText >::invoke")
		->args({"self","MetaDataKey","LocalizationNamespace","LocalizationKey"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_423 = das::das_call_member< FText (UField::*)(const FName &,const FString,const FString) const,&UField::GetMetaDataText >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:252:24
	makeExtern<DAS_CALL_METHOD(_method_423), SimNode_ExtFuncCallAndCopyOrMove >(lib,"GetMetaDataText","das_call_member< FText (UField::*)(const FName &,const FString,const FString) const , &UField::GetMetaDataText >::invoke")
		->args({"self","MetaDataKey","LocalizationNamespace","LocalizationKey"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_424 = das::das_call_member< void (UField::*)(const wchar_t *,const wchar_t *),&UField::SetMetaData >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:260:23
	makeExtern<DAS_CALL_METHOD(_method_424), SimNode_ExtFuncCall >(lib,"SetMetaData","das_call_member< void (UField::*)(const wchar_t *,const wchar_t *) , &UField::SetMetaData >::invoke")
		->args({"self","Key","InValue"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_425 = das::das_call_member< void (UField::*)(const FName &,const wchar_t *),&UField::SetMetaData >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:261:23
	makeExtern<DAS_CALL_METHOD(_method_425), SimNode_ExtFuncCall >(lib,"SetMetaData","das_call_member< void (UField::*)(const FName &,const wchar_t *) , &UField::SetMetaData >::invoke")
		->args({"self","Key","InValue"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_426 = das::das_call_member< bool (UField::*)(const wchar_t *) const,&UField::GetBoolMetaData >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:269:7
	makeExtern<DAS_CALL_METHOD(_method_426), SimNode_ExtFuncCall >(lib,"GetBoolMetaData","das_call_member< bool (UField::*)(const wchar_t *) const , &UField::GetBoolMetaData >::invoke")
		->args({"self","Key"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_427 = das::das_call_member< bool (UField::*)(const FName &) const,&UField::GetBoolMetaData >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:275:7
	makeExtern<DAS_CALL_METHOD(_method_427), SimNode_ExtFuncCall >(lib,"GetBoolMetaData","das_call_member< bool (UField::*)(const FName &) const , &UField::GetBoolMetaData >::invoke")
		->args({"self","Key"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_428 = das::das_call_member< int (UField::*)(const wchar_t *) const,&UField::GetIntMetaData >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:288:8
	makeExtern<DAS_CALL_METHOD(_method_428), SimNode_ExtFuncCall >(lib,"GetIntMetaData","das_call_member< int (UField::*)(const wchar_t *) const , &UField::GetIntMetaData >::invoke")
		->args({"self","Key"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_429 = das::das_call_member< int (UField::*)(const FName &) const,&UField::GetIntMetaData >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:294:8
	makeExtern<DAS_CALL_METHOD(_method_429), SimNode_ExtFuncCall >(lib,"GetIntMetaData","das_call_member< int (UField::*)(const FName &) const , &UField::GetIntMetaData >::invoke")
		->args({"self","Key"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_430 = das::das_call_member< float (UField::*)(const wchar_t *) const,&UField::GetFloatMetaData >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:307:8
	makeExtern<DAS_CALL_METHOD(_method_430), SimNode_ExtFuncCall >(lib,"GetFloatMetaData","das_call_member< float (UField::*)(const wchar_t *) const , &UField::GetFloatMetaData >::invoke")
		->args({"self","Key"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_431 = das::das_call_member< float (UField::*)(const FName &) const,&UField::GetFloatMetaData >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:313:8
	makeExtern<DAS_CALL_METHOD(_method_431), SimNode_ExtFuncCall >(lib,"GetFloatMetaData","das_call_member< float (UField::*)(const FName &) const , &UField::GetFloatMetaData >::invoke")
		->args({"self","Key"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_432 = das::das_call_member< UClass * (UField::*)(const wchar_t *) const,&UField::GetClassMetaData >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:326:26
	makeExtern<DAS_CALL_METHOD(_method_432), SimNode_ExtFuncCall >(lib,"GetClassMetaData","das_call_member< UClass * (UField::*)(const wchar_t *) const , &UField::GetClassMetaData >::invoke")
		->args({"self","Key"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_433 = das::das_call_member< UClass * (UField::*)(const FName &) const,&UField::GetClassMetaData >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:327:26
	makeExtern<DAS_CALL_METHOD(_method_433), SimNode_ExtFuncCall >(lib,"GetClassMetaData","das_call_member< UClass * (UField::*)(const FName &) const , &UField::GetClassMetaData >::invoke")
		->args({"self","Key"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_434 = das::das_call_member< void (UField::*)(const wchar_t *),&UField::RemoveMetaData >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:330:23
	makeExtern<DAS_CALL_METHOD(_method_434), SimNode_ExtFuncCall >(lib,"RemoveMetaData","das_call_member< void (UField::*)(const wchar_t *) , &UField::RemoveMetaData >::invoke")
		->args({"self","Key"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_435 = das::das_call_member< void (UField::*)(const FName &),&UField::RemoveMetaData >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:331:23
	makeExtern<DAS_CALL_METHOD(_method_435), SimNode_ExtFuncCall >(lib,"RemoveMetaData","das_call_member< void (UField::*)(const FName &) , &UField::RemoveMetaData >::invoke")
		->args({"self","Key"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_436 = das::das_call_member< bool (UField::*)(const uint64) const,&UField::HasAnyCastFlags >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:334:23
	makeExtern<DAS_CALL_METHOD(_method_436), SimNode_ExtFuncCall >(lib,"HasAnyCastFlags","das_call_member< bool (UField::*)(const uint64) const , &UField::HasAnyCastFlags >::invoke")
		->args({"self","InCastFlags"})
		->addToModule(*this, SideEffects::worstDefault);
	using _method_437 = das::das_call_member< bool (UField::*)(const uint64) const,&UField::HasAllCastFlags >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:335:23
	makeExtern<DAS_CALL_METHOD(_method_437), SimNode_ExtFuncCall >(lib,"HasAllCastFlags","das_call_member< bool (UField::*)(const uint64) const , &UField::HasAllCastFlags >::invoke")
		->args({"self","InCastFlags"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:341:34
	makeExtern< FField * (*)(UField &) , _dasUnreal_virtual_131_GetAssociatedFField , SimNode_ExtFuncCall >(lib,"GetAssociatedFField","_dasUnreal_virtual_131_GetAssociatedFField")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:345:31
	makeExtern< void (*)(UField &,FField *) , _dasUnreal_virtual_132_SetAssociatedFField , SimNode_ExtFuncCall >(lib,"SetAssociatedFField","_dasUnreal_virtual_132_SetAssociatedFField")
		->args({"self","InField"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:388:2
	makeExtern< UClass * (*)(const UStruct *) , _dasUnreal_static_133_StaticClass , SimNode_ExtFuncCall >(lib,"StaticClass","_dasUnreal_static_133_StaticClass")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const UStruct*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:388:2
	makeExtern< const wchar_t * (*)(const UStruct *) , _dasUnreal_static_134_StaticPackage , SimNode_ExtFuncCall >(lib,"StaticPackage","_dasUnreal_static_134_StaticPackage")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const UStruct*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:388:2
	makeExtern< void (*)(const UStruct *) , _dasUnreal_static_135_StaticRegisterNativesUStruct , SimNode_ExtFuncCall >(lib,"StaticRegisterNativesUStruct","_dasUnreal_static_135_StaticRegisterNativesUStruct")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const UStruct*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
	using _method_443 = das::das_call_member< const FBlake3Hash & (UStruct::*)(bool) const,&UStruct::GetSchemaHash >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:441:37
	makeExtern<DAS_CALL_METHOD(_method_443), SimNode_ExtFuncCallRef >(lib,"GetSchemaHash","das_call_member< const FBlake3Hash & (UStruct::*)(bool) const , &UStruct::GetSchemaHash >::invoke")
		->args({"self","bSkipEditorOnly"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:458:31
	makeExtern< void (*)(UStruct &) , _dasUnreal_virtual_136_PostLoad , SimNode_ExtFuncCall >(lib,"PostLoad","_dasUnreal_virtual_136_PostLoad")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:459:31
	makeExtern< void (*)(UStruct &) , _dasUnreal_virtual_137_FinishDestroy , SimNode_ExtFuncCall >(lib,"FinishDestroy","_dasUnreal_virtual_137_FinishDestroy")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:460:31
	makeExtern< void (*)(UStruct &) , _dasUnreal_virtual_138_RegisterDependencies , SimNode_ExtFuncCall >(lib,"RegisterDependencies","_dasUnreal_virtual_138_RegisterDependencies")
		->args({"self"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:463:31
	makeExtern< void (*)(UStruct &,EObjectFlags) , _dasUnreal_virtual_139_TagSubobjects , SimNode_ExtFuncCall >(lib,"TagSubobjects","_dasUnreal_virtual_139_TagSubobjects")
		->args({"self","NewFlags"})
		->addToModule(*this, SideEffects::worstDefault);
}
}

