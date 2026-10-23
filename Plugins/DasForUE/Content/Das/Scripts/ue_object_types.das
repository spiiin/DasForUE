options gen2
module ue_object_types public
require ue_foundation public
require ue_generated public
// This module compiles before the importing class declarations are parsed. Actor has no short generated alias:
// the name belongs to the native bind type ue_engine::Actor (DasNativeBinds.h), so the full wrapper name is used.
typedef ActorRef = $UEObject<C_Actor_adf65d5b>
typedef ObjectRef = $ue_foundation::UEObject<Object>

// Resolve type macros in this imported module before class signatures are parsed.
typedef ActorClass = $UEClass<C_Actor_adf65d5b>
typedef ActorClassViaRef = $ue_foundation::UEClass<ActorRef>
typedef ObjectClass = $ue_foundation::UEClass<Object>
def ue_class_valid(value:auto):bool { return ue_class_valid(ue_class_handle(value)) }
def ue_class_path(value:auto):string { return ue_class_path(ue_class_handle(value)) }

// Checked weak UObject views: callers do not need to unwrap their handle.
def ue_is_valid(value:auto):bool { return ue_object_valid(value.handle) }

// True only for two live references to the same UObject; no pointer/key escapes.
def ue_same_object(left:auto; right:auto):bool { return __ue_same_object(left.handle,right.handle) }
