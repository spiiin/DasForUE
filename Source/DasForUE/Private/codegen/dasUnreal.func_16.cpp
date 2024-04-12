// this file is generated via daScript automatic C++ binder
// all user modifications will be lost after this file is re-generated

#include "dasUnreal.h"
#include "need_dasUnreal.h"
namespace das {
#include "dasUnreal.func.aot.decl.inc"
void Module_dasUnreal::initFunctions_16() {
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Engine/Classes\Kismet/KismetArrayLibrary.h:272:25
	makeExtern< void (*)(const UKismetArrayLibrary *,void *,const FArrayProperty *) , _dasUnreal_static_576_GenericArray_Shuffle , SimNode_ExtFuncCall >(lib,"GenericArray_Shuffle","_dasUnreal_static_576_GenericArray_Shuffle")
		->args({"self","TargetArray","ArrayProp"})
		->arg_type(0,makeType<TExplicit<const UKismetArrayLibrary*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Engine/Classes\Kismet/KismetArrayLibrary.h:274:25
	makeExtern< bool (*)(const UKismetArrayLibrary *,void *,const FArrayProperty *,void *,const FArrayProperty *) , _dasUnreal_static_577_GenericArray_Identical , SimNode_ExtFuncCall >(lib,"GenericArray_Identical","_dasUnreal_static_577_GenericArray_Identical")
		->args({"self","ArrayA","ArrayAProp","ArrayB","ArrayBProperty"})
		->arg_type(0,makeType<TExplicit<const UKismetArrayLibrary*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Engine/Classes\Kismet/KismetArrayLibrary.h:275:25
	makeExtern< void (*)(const UKismetArrayLibrary *,void *,const FArrayProperty *,void *,const FArrayProperty *) , _dasUnreal_static_578_GenericArray_Append , SimNode_ExtFuncCall >(lib,"GenericArray_Append","_dasUnreal_static_578_GenericArray_Append")
		->args({"self","TargetArray","TargetArrayProp","SourceArray","SourceArrayProperty"})
		->arg_type(0,makeType<TExplicit<const UKismetArrayLibrary*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Engine/Classes\Kismet/KismetArrayLibrary.h:276:25
	makeExtern< void (*)(const UKismetArrayLibrary *,void *,const FArrayProperty *,const void *,int) , _dasUnreal_static_579_GenericArray_Insert , SimNode_ExtFuncCall >(lib,"GenericArray_Insert","_dasUnreal_static_579_GenericArray_Insert")
		->args({"self","TargetArray","ArrayProp","NewItem","Index"})
		->arg_type(0,makeType<TExplicit<const UKismetArrayLibrary*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Engine/Classes\Kismet/KismetArrayLibrary.h:277:25
	makeExtern< void (*)(const UKismetArrayLibrary *,void *,const FArrayProperty *,int) , _dasUnreal_static_580_GenericArray_Remove , SimNode_ExtFuncCall >(lib,"GenericArray_Remove","_dasUnreal_static_580_GenericArray_Remove")
		->args({"self","TargetArray","ArrayProp","IndexToRemove"})
		->arg_type(0,makeType<TExplicit<const UKismetArrayLibrary*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Engine/Classes\Kismet/KismetArrayLibrary.h:278:25
	makeExtern< bool (*)(const UKismetArrayLibrary *,void *,const FArrayProperty *,const void *) , _dasUnreal_static_581_GenericArray_RemoveItem , SimNode_ExtFuncCall >(lib,"GenericArray_RemoveItem","_dasUnreal_static_581_GenericArray_RemoveItem")
		->args({"self","TargetArray","ArrayProp","Item"})
		->arg_type(0,makeType<TExplicit<const UKismetArrayLibrary*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Engine/Classes\Kismet/KismetArrayLibrary.h:279:25
	makeExtern< void (*)(const UKismetArrayLibrary *,void *,const FArrayProperty *) , _dasUnreal_static_582_GenericArray_Clear , SimNode_ExtFuncCall >(lib,"GenericArray_Clear","_dasUnreal_static_582_GenericArray_Clear")
		->args({"self","TargetArray","ArrayProp"})
		->arg_type(0,makeType<TExplicit<const UKismetArrayLibrary*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Engine/Classes\Kismet/KismetArrayLibrary.h:280:25
	makeExtern< void (*)(const UKismetArrayLibrary *,void *,const FArrayProperty *,int) , _dasUnreal_static_583_GenericArray_Resize , SimNode_ExtFuncCall >(lib,"GenericArray_Resize","_dasUnreal_static_583_GenericArray_Resize")
		->args({"self","TargetArray","ArrayProp","Size"})
		->arg_type(0,makeType<TExplicit<const UKismetArrayLibrary*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Engine/Classes\Kismet/KismetArrayLibrary.h:281:25
	makeExtern< void (*)(const UKismetArrayLibrary *,void *,const FArrayProperty *) , _dasUnreal_static_584_GenericArray_Reverse , SimNode_ExtFuncCall >(lib,"GenericArray_Reverse","_dasUnreal_static_584_GenericArray_Reverse")
		->args({"self","TargetArray","ArrayProp"})
		->arg_type(0,makeType<TExplicit<const UKismetArrayLibrary*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Engine/Classes\Kismet/KismetArrayLibrary.h:282:26
	makeExtern< int (*)(const UKismetArrayLibrary *,const void *,const FArrayProperty *) , _dasUnreal_static_585_GenericArray_Length , SimNode_ExtFuncCall >(lib,"GenericArray_Length","_dasUnreal_static_585_GenericArray_Length")
		->args({"self","TargetArray","ArrayProp"})
		->arg_type(0,makeType<TExplicit<const UKismetArrayLibrary*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Engine/Classes\Kismet/KismetArrayLibrary.h:283:25
	makeExtern< bool (*)(const UKismetArrayLibrary *,const void *,const FArrayProperty *) , _dasUnreal_static_586_GenericArray_IsEmpty , SimNode_ExtFuncCall >(lib,"GenericArray_IsEmpty","_dasUnreal_static_586_GenericArray_IsEmpty")
		->args({"self","TargetArray","ArrayProp"})
		->arg_type(0,makeType<TExplicit<const UKismetArrayLibrary*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Engine/Classes\Kismet/KismetArrayLibrary.h:284:25
	makeExtern< bool (*)(const UKismetArrayLibrary *,const void *,const FArrayProperty *) , _dasUnreal_static_587_GenericArray_IsNotEmpty , SimNode_ExtFuncCall >(lib,"GenericArray_IsNotEmpty","_dasUnreal_static_587_GenericArray_IsNotEmpty")
		->args({"self","TargetArray","ArrayProp"})
		->arg_type(0,makeType<TExplicit<const UKismetArrayLibrary*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Engine/Classes\Kismet/KismetArrayLibrary.h:285:26
	makeExtern< int (*)(const UKismetArrayLibrary *,const void *,const FArrayProperty *) , _dasUnreal_static_588_GenericArray_LastIndex , SimNode_ExtFuncCall >(lib,"GenericArray_LastIndex","_dasUnreal_static_588_GenericArray_LastIndex")
		->args({"self","TargetArray","ArrayProp"})
		->arg_type(0,makeType<TExplicit<const UKismetArrayLibrary*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Engine/Classes\Kismet/KismetArrayLibrary.h:286:25
	makeExtern< void (*)(const UKismetArrayLibrary *,void *,const FArrayProperty *,int,void *) , _dasUnreal_static_589_GenericArray_Get , SimNode_ExtFuncCall >(lib,"GenericArray_Get","_dasUnreal_static_589_GenericArray_Get")
		->args({"self","TargetArray","ArrayProp","Index","Item"})
		->arg_type(0,makeType<TExplicit<const UKismetArrayLibrary*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Engine/Classes\Kismet/KismetArrayLibrary.h:287:25
	makeExtern< void (*)(const UKismetArrayLibrary *,void *,const FArrayProperty *,int,const void *,bool) , _dasUnreal_static_590_GenericArray_Set , SimNode_ExtFuncCall >(lib,"GenericArray_Set","_dasUnreal_static_590_GenericArray_Set")
		->args({"self","TargetArray","ArrayProp","Index","NewItem","bSizeToFit"})
		->arg_type(0,makeType<TExplicit<const UKismetArrayLibrary*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Engine/Classes\Kismet/KismetArrayLibrary.h:288:25
	makeExtern< void (*)(const UKismetArrayLibrary *,const void *,const FArrayProperty *,int,int) , _dasUnreal_static_591_GenericArray_Swap , SimNode_ExtFuncCall >(lib,"GenericArray_Swap","_dasUnreal_static_591_GenericArray_Swap")
		->args({"self","TargetArray","ArrayProp","First","Second"})
		->arg_type(0,makeType<TExplicit<const UKismetArrayLibrary*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Engine/Classes\Kismet/KismetArrayLibrary.h:289:26
	makeExtern< int (*)(const UKismetArrayLibrary *,const void *,const FArrayProperty *,const void *) , _dasUnreal_static_592_GenericArray_Find , SimNode_ExtFuncCall >(lib,"GenericArray_Find","_dasUnreal_static_592_GenericArray_Find")
		->args({"self","TargetArray","ArrayProperty","ItemToFind"})
		->arg_type(0,makeType<TExplicit<const UKismetArrayLibrary*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Engine/Classes\Kismet/KismetArrayLibrary.h:290:25
	makeExtern< void (*)(const UKismetArrayLibrary *,UObject *,FName,const void *) , _dasUnreal_static_593_GenericArray_SetArrayPropertyByName , SimNode_ExtFuncCall >(lib,"GenericArray_SetArrayPropertyByName","_dasUnreal_static_593_GenericArray_SetArrayPropertyByName")
		->args({"self","OwnerObject","ArrayPropertyName","SrcArrayAddr"})
		->arg_type(0,makeType<TExplicit<const UKismetArrayLibrary*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Engine/Classes\Kismet/KismetArrayLibrary.h:291:25
	makeExtern< void (*)(const UKismetArrayLibrary *,void *,const FArrayProperty *,void *,int *) , _dasUnreal_static_594_GenericArray_Random , SimNode_ExtFuncCall >(lib,"GenericArray_Random","_dasUnreal_static_594_GenericArray_Random")
		->args({"self","TargetArray","ArrayProp","OutItem","OutIndex"})
		->arg_type(0,makeType<TExplicit<const UKismetArrayLibrary*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Engine/Classes\Kismet/KismetArrayLibrary.h:293:25
	makeExtern< bool (*)(const UKismetArrayLibrary *,const void *,const FArrayProperty *,int) , _dasUnreal_static_595_GenericArray_IsValidIndex , SimNode_ExtFuncCall >(lib,"GenericArray_IsValidIndex","_dasUnreal_static_595_GenericArray_IsValidIndex")
		->args({"self","TargetArray","ArrayProp","IndexToTest"})
		->arg_type(0,makeType<TExplicit<const UKismetArrayLibrary*> >(lib))
		->addToModule(*this, SideEffects::worstDefault);
}
}

