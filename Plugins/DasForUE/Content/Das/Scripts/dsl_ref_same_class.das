options gen2
require ue_dsl
require ue_same_class_refs
[ue_class(module_id="refpeer", Blueprintable)]
class Peer {
 @ue_property=(EditAnywhere, BlueprintReadWrite)
 Score:int=10
 [ue_function(BlueprintCallable)]
 def AddToPeer(target:UObjectHandle):int {
  let peer = ue_ref(type<PeerRef>,target)
  peer.Score += self.Score
  return peer.Score
 }
}
