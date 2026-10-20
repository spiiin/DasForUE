options gen2
require ue_enhanced_input
require ue_gameplay_services
require ue_effects
require ue_animation
[ue_class(module_id="hiddencallbacks",Blueprintable)]
class HiddenActor {
 @ue_property=(BlueprintReadWrite)
 Interval:float=0.05f
 @ue_property=(BlueprintReadWrite)
 Key:string="dynamic"
 [ue_function(BlueprintCallable)]
 def LegacyProperties():void {ue_timer_start(self.Key,self.Interval,self.Pulse,true);ue_timer_cancel(self.Key)}
 @ue_property=(BlueprintReadWrite)
 TimerHits:int=0
 @ue_property=(BlueprintReadWrite)
 InputHits:int=0
 @ue_property=(BlueprintReadWrite)
 Action:InputActionRef
 def Tick(delta:float):void {}
 [ue_function(BlueprintCallable)]
 def Start():void {
  ue_timer_start(0.05f,self.Pulse,true)
  ue_enhanced_bind(self.Action,TriggerEvent.Triggered,self.Input)
 }
 def Pulse():void {self.TimerHits+=1}
 [ue_function]
 def Input(value:float):void {self.InputHits+=int(value)}
 [ue_function(BlueprintCallable)]
 def StartPayload():void {ue_timer_next_tick(self.Payload,3)}
 def Payload(value:int):void {self.TimerHits+=value}
 [ue_function(BlueprintCallable)]
 def StartMulti():void {ue_timer_next_tick(self.Multi,3,4)}
 def Multi(left:int;right:int):void {self.TimerHits+=left+right}
 [ue_function(BlueprintCallable)]
 def BindButton(button:ButtonRef):void {ue_widget_bind_click(button,self.Clicked)}
 def Clicked():void {self.TimerHits+=10}
 [ue_function(BlueprintCallable)]
 def StartInline():void {ue_timer_next_tick("inline",$ {self.TimerHits+=20})}
 [ue_function(BlueprintCallable)]
 def StartPublic():void {ue_timer_next_tick(self.PublicHandler)}
 [ue_function(BlueprintCallable)]
 def PublicHandler():void {self.TimerHits+=30}
 [ue_function(BlueprintCallable)]
 def Direct():void {self.Pulse()}
 def Unused():int {return 99}
 [ue_function(BlueprintCallable)]
 def Read():int {return self.TimerHits} // RELOAD_MARKER
 [ue_function(BlueprintCallable)]
 def ExerciseKeys():int {
  if(!ue_timer_is_active(self.Pulse)) {return -1}
  if(!ue_timer_pause(self.Pulse) || !ue_timer_is_paused(self.Pulse)) {return -2}
  if(!ue_timer_resume(self.Pulse) || ue_timer_remaining(self.Pulse)<0.0f) {return -3}
  ue_latent_delay(99.0f,self.Pulse)
  if(!ue_latent_is_pending(self.Pulse)) {return -4}
  if(!ue_timer_cancel(self.Pulse) || !ue_latent_is_pending(self.Pulse)) {return -5}
  if(!ue_latent_cancel(self.Pulse) || ue_latent_is_pending(self.Pulse)) {return -6}
  return 1
 }
 def EffectsFinished():void {}
 def MontageUpdate(value:MontageEvent):void {}
 [ue_function(BlueprintCallable)]
 def CompileFamilies(sound:SoundBaseRef;system:NiagaraSystemRef;mesh:SkeletalMeshComponentRef;montage:AnimMontageRef):void {
  let at=ue_vector(0.0lf,0.0lf,0.0lf)
  ue_sound_play(sound,at,self.EffectsFinished)
  ue_niagara_spawn(system,at,self.EffectsFinished)
  ue_effect_at(sound,system,at,self.EffectsFinished)
  ue_effect_play(sound,system,ue_transform_identity(),self.EffectsFinished)
  ue_effect_active(self.EffectsFinished)
  ue_effect_stop(self.EffectsFinished)
  ue_montage_play(mesh,montage,self.MontageUpdate)
  ue_montage_playing(self.MontageUpdate)
  ue_montage_stop(self.MontageUpdate)
  ue_montage_cancel(self.MontageUpdate)
 }
 [ue_function(BlueprintCallable)]
 def WarnUnknown():void {ue_timer_cancel("unknown_timer_typo")}
}
