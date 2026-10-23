options gen2
module dsl_enemy_character public
require ue_native_gameplay public
require ue_ai public
[ue_class(Blueprintable)]
class EnemyCharacter : Character {
    settings=ue_properties(EditAnywhere,BlueprintReadWrite) {
        var Target:ActorRef
        var Enabled:bool=true
        var AttackRange:float=150.0f
        var AttackDamage:float=10.0f
        var AttackInterval:float=1.0f
    }
    state=ue_properties(BlueprintReadWrite) {
        var Attacks:int=0
        var DamageDealt:float=0.0f
        var MoveCompletions:int=0
        var LastMoveCode:int=-1
        var Cooldown:float=0.0f
    }
    components=ue_components(VisibleAnywhere,BlueprintReadOnly) {
        var Visual:StaticMeshComponentRef
        var Label:TextRenderComponentRef
    }
    defaults=ue_defaults() {
        self.Label.RelativeLocation=ue_vector(0.0lf,0.0lf,130.0lf)
        self.Visual.RelativeScale3D=ue_vector(0.6lf,0.6lf,1.2lf)
    }
    def BeginPlay():void {
        self.CharacterMovement.MaxWalkSpeed=220.0f
        self.Label.K2_SetText(ue_text_invariant("Enemy"))
        ue_timer_start("brain",0.25f,"Think",true)
    }
    [ue_function(BlueprintCallable)]
    def Think():void {
        if(!self.Enabled || !ue_is_valid(self.Target)) { ue_ai_move_cancel("chase");return }
        let controller=ue_ai_self_controller()
        if(!ue_is_valid(controller)) { return }
        self.Cooldown-=0.25f
        if(self.Cooldown<0.0f) { self.Cooldown=0.0f }
        if(self.GetDistanceTo(self.Target)<=self.AttackRange) {
            ue_ai_move_cancel("chase")
            if(self.Cooldown<=0.0f) {
                self.DamageDealt+=ue_apply_damage(self.Target,self.AttackDamage,ue_self_actor(),controller)
                self.Attacks+=1
                self.Cooldown=self.AttackInterval>0.25f ? self.AttackInterval : 0.25f
            }
        } elif(!ue_ai_move_active("chase")) {
            ue_ai_move_to_actor("chase",controller,self.Target,"OnMoveFinished",40.0f)
        }
    }
    [ue_function(BlueprintCallable)]
    def OnMoveFinished(result:MoveResult):void {
        self.MoveCompletions+=1
        self.LastMoveCode=int(result.Code)
    }
    [ue_function(BlueprintCallable)]
    def StopBrain():void { self.Enabled=false;ue_timer_cancel("brain");ue_ai_move_cancel("chase") }
    [ue_function(BlueprintCallable)]
    def ResumeBrain():void { self.Enabled=true;ue_timer_start("brain",0.25f,"Think",true) }
}
