options gen2
module ue_object_types public
require ue_foundation public
require ue_generated public
// This module compiles before the importing class declarations are parsed.
typedef ActorRef = $UEObject<Actor>
typedef ObjectRef = $ue_foundation::UEObject<Object>

// Resolve type macros in this imported module before class signatures are parsed.
typedef ActorClass = $UEClass<Actor>
typedef ActorClassViaRef = $ue_foundation::UEClass<ActorRef>
typedef ObjectClass = $ue_foundation::UEClass<Object>
def ue_class_valid(value:auto):bool { return ue_class_valid(ue_class_handle(value)) }
def ue_class_path(value:auto):string { return ue_class_path(ue_class_handle(value)) }

// Checked weak UObject views: callers do not need to unwrap their handle.
def ue_is_valid(value:auto):bool { return ue_object_valid(value.handle) }

// True only for two live references to the same UObject; no pointer/key escapes.
def ue_same_object(left:auto; right:auto):bool { return __ue_same_object(left.handle,right.handle) }
