options gen2
require ue_dsl
require @MIDDLE@ public
[ue_class(module_id="handlerinherit",Blueprintable)]
class CallbackLeaf : CallbackMiddle {
 def override Pulse():void {Super.Pulse();self.Pulses+=10} // RELOAD
 def override Bump():void {Super.Bump();self.Bumps+=20}
 def override Payload(value:int):void {Super.Payload(value);self.PayloadTotal+=30}
 def override Multi(left:int;right:int):void {Super.Multi(left,right);self.PayloadTotal+=40}
 def override InheritedVisible():void {Super.InheritedVisible();self.PublicHits+=70}
 def override PublicCallback():void {Super.PublicCallback();self.PublicHits+=50}
 [ue_function]
 def override Input(value:float):void {Super.Input(value);self.InputTotal+=20}
 def override Query(value:int):int {return Super.Query(value)+100}
 def override Mutate(var value:int&;@ue_out var accepted:bool&):int {let result=Super.Mutate(value,accepted);value+=1;return result+100}
 def override Hit(value:int):void {Super.Hit(value);self.HitTotal+=value*10}
 [ue_function(BlueprintCallable)]
 def LeafStart():void {ue_timer_next_tick("leaf",self.Pulse)}
 [ue_function(BlueprintCallable)]
 def LeafQuery():int {return ue_bind(type<InheritQuery>,ue_self_object(),self.Query).Execute(3)}
 def override Tick(delta:float):void {}
}
