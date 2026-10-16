options gen2
require ue_dsl
[ue_class]
class LibrarySpawned : Actor {
 @ue_property=(BlueprintReadWrite)
 Score:int=1
 [ue_lifecycle]
 def ConstructionScript():void { self.Score=42 }
}
