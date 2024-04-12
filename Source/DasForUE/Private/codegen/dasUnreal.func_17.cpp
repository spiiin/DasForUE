// this file is generated via daScript automatic C++ binder
// all user modifications will be lost after this file is re-generated

#include "dasUnreal.h"
#include "need_dasUnreal.h"
namespace das {
#include "dasUnreal.func.aot.decl.inc"
void Module_dasUnreal::initFunctions_17() {
	using _method_1349 = das::das_call_member< unsigned short (FFrame::*)(FProperty **),&FFrame::ReadVariableSize >;
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Stack.h:265:19
	makeExtern<DAS_CALL_METHOD(_method_1349), SimNode_ExtFuncCall >(lib,"ReadVariableSize","das_call_member< unsigned short (FFrame::*)(FProperty **) , &FFrame::ReadVariableSize >::invoke")
		->args({"self","ExpressionField"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Stack.h:313:33
	makeExtern< FFrame * (*)(const FFrame *,FFrame *) , _dasUnreal_static_646_PushThreadLocalTopStackFrame , SimNode_ExtFuncCall >(lib,"PushThreadLocalTopStackFrame","_dasUnreal_static_646_PushThreadLocalTopStackFrame")
		->args({"self","NewTopStackFrame"})
		->arg_type(0,makeType<TExplicit<const FFrame*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Stack.h:314:30
	makeExtern< void (*)(const FFrame *,FFrame *) , _dasUnreal_static_647_PopThreadLocalTopStackFrame , SimNode_ExtFuncCall >(lib,"PopThreadLocalTopStackFrame","_dasUnreal_static_647_PopThreadLocalTopStackFrame")
		->args({"self","NewTopStackFrame"})
		->arg_type(0,makeType<TExplicit<const FFrame*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Stack.h:315:33
	makeExtern< FFrame * (*)(const FFrame *) , _dasUnreal_static_648_GetThreadLocalTopStackFrame , SimNode_ExtFuncCall >(lib,"GetThreadLocalTopStackFrame","_dasUnreal_static_648_GetThreadLocalTopStackFrame")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const FFrame*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Stack.h:464:22
	makeExtern< void (*)() , GInitRunaway , SimNode_ExtFuncCall >(lib,"GInitRunaway","GInitRunaway")
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Engine/Classes\Kismet/BlueprintFunctionLibrary.h:17:2
	makeExtern< UClass * (*)(const UBlueprintFunctionLibrary *) , _dasUnreal_static_649_StaticClass , SimNode_ExtFuncCall >(lib,"StaticClass","_dasUnreal_static_649_StaticClass")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const UBlueprintFunctionLibrary*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Engine/Classes\Kismet/BlueprintFunctionLibrary.h:17:2
	makeExtern< const wchar_t * (*)(const UBlueprintFunctionLibrary *) , _dasUnreal_static_650_StaticPackage , SimNode_ExtFuncCall >(lib,"StaticPackage","_dasUnreal_static_650_StaticPackage")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const UBlueprintFunctionLibrary*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Engine/Classes\Kismet/BlueprintFunctionLibrary.h:20:27
	makeExtern< int (*)(UBlueprintFunctionLibrary &,UFunction *,FFrame *) , _dasUnreal_virtual_651_GetFunctionCallspace , SimNode_ExtFuncCall >(lib,"GetFunctionCallspace","_dasUnreal_virtual_651_GetFunctionCallspace")
		->args({"self","Function","Stack"})
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Engine/Classes\Kismet/KismetArrayLibrary.h:21:2
	makeExtern< UClass * (*)(const UKismetArrayLibrary *) , _dasUnreal_static_652_StaticClass , SimNode_ExtFuncCall >(lib,"StaticClass","_dasUnreal_static_652_StaticClass")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const UKismetArrayLibrary*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Engine/Classes\Kismet/KismetArrayLibrary.h:21:2
	makeExtern< const wchar_t * (*)(const UKismetArrayLibrary *) , _dasUnreal_static_653_StaticPackage , SimNode_ExtFuncCall >(lib,"StaticPackage","_dasUnreal_static_653_StaticPackage")
		->args({"self"})
		->arg_type(0,makeType<TExplicit<const UKismetArrayLibrary*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Engine/Classes\Kismet/KismetArrayLibrary.h:270:26
	makeExtern< int (*)(const UKismetArrayLibrary *,void *,const FArrayProperty *,const void *) , _dasUnreal_static_654_GenericArray_Add , SimNode_ExtFuncCall >(lib,"GenericArray_Add","_dasUnreal_static_654_GenericArray_Add")
		->args({"self","TargetArray","ArrayProp","NewItem"})
		->arg_type(0,makeType<TExplicit<const UKismetArrayLibrary*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Engine/Classes\Kismet/KismetArrayLibrary.h:271:26
	makeExtern< int (*)(const UKismetArrayLibrary *,void *,const FArrayProperty *,const void *) , _dasUnreal_static_655_GenericArray_AddUnique , SimNode_ExtFuncCall >(lib,"GenericArray_AddUnique","_dasUnreal_static_655_GenericArray_AddUnique")
		->args({"self","TargetArray","ArrayProp","NewItem"})
		->arg_type(0,makeType<TExplicit<const UKismetArrayLibrary*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Engine/Classes\Kismet/KismetArrayLibrary.h:272:25
	makeExtern< void (*)(const UKismetArrayLibrary *,void *,const FArrayProperty *) , _dasUnreal_static_656_GenericArray_Shuffle , SimNode_ExtFuncCall >(lib,"GenericArray_Shuffle","_dasUnreal_static_656_GenericArray_Shuffle")
		->args({"self","TargetArray","ArrayProp"})
		->arg_type(0,makeType<TExplicit<const UKismetArrayLibrary*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Engine/Classes\Kismet/KismetArrayLibrary.h:274:25
	makeExtern< bool (*)(const UKismetArrayLibrary *,void *,const FArrayProperty *,void *,const FArrayProperty *) , _dasUnreal_static_657_GenericArray_Identical , SimNode_ExtFuncCall >(lib,"GenericArray_Identical","_dasUnreal_static_657_GenericArray_Identical")
		->args({"self","ArrayA","ArrayAProp","ArrayB","ArrayBProperty"})
		->arg_type(0,makeType<TExplicit<const UKismetArrayLibrary*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Engine/Classes\Kismet/KismetArrayLibrary.h:275:25
	makeExtern< void (*)(const UKismetArrayLibrary *,void *,const FArrayProperty *,void *,const FArrayProperty *) , _dasUnreal_static_658_GenericArray_Append , SimNode_ExtFuncCall >(lib,"GenericArray_Append","_dasUnreal_static_658_GenericArray_Append")
		->args({"self","TargetArray","TargetArrayProp","SourceArray","SourceArrayProperty"})
		->arg_type(0,makeType<TExplicit<const UKismetArrayLibrary*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Engine/Classes\Kismet/KismetArrayLibrary.h:276:25
	makeExtern< void (*)(const UKismetArrayLibrary *,void *,const FArrayProperty *,const void *,int) , _dasUnreal_static_659_GenericArray_Insert , SimNode_ExtFuncCall >(lib,"GenericArray_Insert","_dasUnreal_static_659_GenericArray_Insert")
		->args({"self","TargetArray","ArrayProp","NewItem","Index"})
		->arg_type(0,makeType<TExplicit<const UKismetArrayLibrary*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Engine/Classes\Kismet/KismetArrayLibrary.h:277:25
	makeExtern< void (*)(const UKismetArrayLibrary *,void *,const FArrayProperty *,int) , _dasUnreal_static_660_GenericArray_Remove , SimNode_ExtFuncCall >(lib,"GenericArray_Remove","_dasUnreal_static_660_GenericArray_Remove")
		->args({"self","TargetArray","ArrayProp","IndexToRemove"})
		->arg_type(0,makeType<TExplicit<const UKismetArrayLibrary*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Engine/Classes\Kismet/KismetArrayLibrary.h:278:25
	makeExtern< bool (*)(const UKismetArrayLibrary *,void *,const FArrayProperty *,const void *) , _dasUnreal_static_661_GenericArray_RemoveItem , SimNode_ExtFuncCall >(lib,"GenericArray_RemoveItem","_dasUnreal_static_661_GenericArray_RemoveItem")
		->args({"self","TargetArray","ArrayProp","Item"})
		->arg_type(0,makeType<TExplicit<const UKismetArrayLibrary*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Engine/Classes\Kismet/KismetArrayLibrary.h:279:25
	makeExtern< void (*)(const UKismetArrayLibrary *,void *,const FArrayProperty *) , _dasUnreal_static_662_GenericArray_Clear , SimNode_ExtFuncCall >(lib,"GenericArray_Clear","_dasUnreal_static_662_GenericArray_Clear")
		->args({"self","TargetArray","ArrayProp"})
		->arg_type(0,makeType<TExplicit<const UKismetArrayLibrary*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Engine/Classes\Kismet/KismetArrayLibrary.h:280:25
	makeExtern< void (*)(const UKismetArrayLibrary *,void *,const FArrayProperty *,int) , _dasUnreal_static_663_GenericArray_Resize , SimNode_ExtFuncCall >(lib,"GenericArray_Resize","_dasUnreal_static_663_GenericArray_Resize")
		->args({"self","TargetArray","ArrayProp","Size"})
		->arg_type(0,makeType<TExplicit<const UKismetArrayLibrary*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Engine/Classes\Kismet/KismetArrayLibrary.h:281:25
	makeExtern< void (*)(const UKismetArrayLibrary *,void *,const FArrayProperty *) , _dasUnreal_static_664_GenericArray_Reverse , SimNode_ExtFuncCall >(lib,"GenericArray_Reverse","_dasUnreal_static_664_GenericArray_Reverse")
		->args({"self","TargetArray","ArrayProp"})
		->arg_type(0,makeType<TExplicit<const UKismetArrayLibrary*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Engine/Classes\Kismet/KismetArrayLibrary.h:282:26
	makeExtern< int (*)(const UKismetArrayLibrary *,const void *,const FArrayProperty *) , _dasUnreal_static_665_GenericArray_Length , SimNode_ExtFuncCall >(lib,"GenericArray_Length","_dasUnreal_static_665_GenericArray_Length")
		->args({"self","TargetArray","ArrayProp"})
		->arg_type(0,makeType<TExplicit<const UKismetArrayLibrary*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Engine/Classes\Kismet/KismetArrayLibrary.h:283:25
	makeExtern< bool (*)(const UKismetArrayLibrary *,const void *,const FArrayProperty *) , _dasUnreal_static_666_GenericArray_IsEmpty , SimNode_ExtFuncCall >(lib,"GenericArray_IsEmpty","_dasUnreal_static_666_GenericArray_IsEmpty")
		->args({"self","TargetArray","ArrayProp"})
		->arg_type(0,makeType<TExplicit<const UKismetArrayLibrary*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Engine/Classes\Kismet/KismetArrayLibrary.h:284:25
	makeExtern< bool (*)(const UKismetArrayLibrary *,const void *,const FArrayProperty *) , _dasUnreal_static_667_GenericArray_IsNotEmpty , SimNode_ExtFuncCall >(lib,"GenericArray_IsNotEmpty","_dasUnreal_static_667_GenericArray_IsNotEmpty")
		->args({"self","TargetArray","ArrayProp"})
		->arg_type(0,makeType<TExplicit<const UKismetArrayLibrary*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Engine/Classes\Kismet/KismetArrayLibrary.h:285:26
	makeExtern< int (*)(const UKismetArrayLibrary *,const void *,const FArrayProperty *) , _dasUnreal_static_668_GenericArray_LastIndex , SimNode_ExtFuncCall >(lib,"GenericArray_LastIndex","_dasUnreal_static_668_GenericArray_LastIndex")
		->args({"self","TargetArray","ArrayProp"})
		->arg_type(0,makeType<TExplicit<const UKismetArrayLibrary*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Engine/Classes\Kismet/KismetArrayLibrary.h:286:25
	makeExtern< void (*)(const UKismetArrayLibrary *,void *,const FArrayProperty *,int,void *) , _dasUnreal_static_669_GenericArray_Get , SimNode_ExtFuncCall >(lib,"GenericArray_Get","_dasUnreal_static_669_GenericArray_Get")
		->args({"self","TargetArray","ArrayProp","Index","Item"})
		->arg_type(0,makeType<TExplicit<const UKismetArrayLibrary*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Engine/Classes\Kismet/KismetArrayLibrary.h:287:25
	makeExtern< void (*)(const UKismetArrayLibrary *,void *,const FArrayProperty *,int,const void *,bool) , _dasUnreal_static_670_GenericArray_Set , SimNode_ExtFuncCall >(lib,"GenericArray_Set","_dasUnreal_static_670_GenericArray_Set")
		->args({"self","TargetArray","ArrayProp","Index","NewItem","bSizeToFit"})
		->arg_type(0,makeType<TExplicit<const UKismetArrayLibrary*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Engine/Classes\Kismet/KismetArrayLibrary.h:288:25
	makeExtern< void (*)(const UKismetArrayLibrary *,const void *,const FArrayProperty *,int,int) , _dasUnreal_static_671_GenericArray_Swap , SimNode_ExtFuncCall >(lib,"GenericArray_Swap","_dasUnreal_static_671_GenericArray_Swap")
		->args({"self","TargetArray","ArrayProp","First","Second"})
		->arg_type(0,makeType<TExplicit<const UKismetArrayLibrary*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Engine/Classes\Kismet/KismetArrayLibrary.h:289:26
	makeExtern< int (*)(const UKismetArrayLibrary *,const void *,const FArrayProperty *,const void *) , _dasUnreal_static_672_GenericArray_Find , SimNode_ExtFuncCall >(lib,"GenericArray_Find","_dasUnreal_static_672_GenericArray_Find")
		->args({"self","TargetArray","ArrayProperty","ItemToFind"})
		->arg_type(0,makeType<TExplicit<const UKismetArrayLibrary*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Engine/Classes\Kismet/KismetArrayLibrary.h:290:25
	makeExtern< void (*)(const UKismetArrayLibrary *,UObject *,FName,const void *) , _dasUnreal_static_673_GenericArray_SetArrayPropertyByName , SimNode_ExtFuncCall >(lib,"GenericArray_SetArrayPropertyByName","_dasUnreal_static_673_GenericArray_SetArrayPropertyByName")
		->args({"self","OwnerObject","ArrayPropertyName","SrcArrayAddr"})
		->arg_type(0,makeType<TExplicit<const UKismetArrayLibrary*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Engine/Classes\Kismet/KismetArrayLibrary.h:291:25
	makeExtern< void (*)(const UKismetArrayLibrary *,void *,const FArrayProperty *,void *,int *) , _dasUnreal_static_674_GenericArray_Random , SimNode_ExtFuncCall >(lib,"GenericArray_Random","_dasUnreal_static_674_GenericArray_Random")
		->args({"self","TargetArray","ArrayProp","OutItem","OutIndex"})
		->arg_type(0,makeType<TExplicit<const UKismetArrayLibrary*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Engine/Classes\Kismet/KismetArrayLibrary.h:293:25
	makeExtern< bool (*)(const UKismetArrayLibrary *,const void *,const FArrayProperty *,int) , _dasUnreal_static_675_GenericArray_IsValidIndex , SimNode_ExtFuncCall >(lib,"GenericArray_IsValidIndex","_dasUnreal_static_675_GenericArray_IsValidIndex")
		->args({"self","TargetArray","ArrayProp","IndexToTest"})
		->arg_type(0,makeType<TExplicit<const UKismetArrayLibrary*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
}
}

