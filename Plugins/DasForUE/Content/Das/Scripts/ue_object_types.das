options gen2
module ue_object_types public
require ue_foundation public
require ue_generated public
// This module compiles before the importing class declarations are parsed.
typedef ActorRef = $UEObject<Actor>
typedef ObjectRef = $ue_foundation::UEObject<Object>
