options gen2
module METHOD_KEYS_MODULE public
require ue_object_keys public
require METHOD_REFS_MODULE public
// Resolve the key type before the importing ue_class annotations run.
typedef ValueTargetKey = $UEObjectKey<ValueTargetRef>
