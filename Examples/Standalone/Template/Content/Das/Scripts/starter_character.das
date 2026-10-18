options gen2
require ue_native_gameplay
require starter_refs
require starter_values
[ue_class(Blueprintable)]
class StarterCharacter : Character {
    state=ue_properties(BlueprintReadWrite) {
        var Score:int=0
        var LastPulse:int=0
        var ControlsBound:bool=false
    }
    @ue_component
    Camera:CameraComponentRef
    @ue_component
    Logic:StarterLogicRef
    defaults=ue_defaults() {
        self.bUseControllerRotationYaw=true
        self.Camera.bUsePawnControlRotation=true
        self.Camera.RelativeLocation=ue_vector(-250.0lf,0.0lf,100.0lf)
    }
    def BeginPlay():void { self.CharacterMovement.MaxWalkSpeed=450.0f }
    def Tick(delta:float):void {
        self.LastPulse=self.Score+starter_bonus()
        if(!self.ControlsBound && ue_input_is_available()) {
            ue_input_bind_axis("MoveForward","MoveForward",true,false)
            ue_input_bind_axis("MoveRight","MoveRight",true,false)
            ue_input_bind_axis("Turn","Turn",true,false)
            ue_input_bind_axis("LookUp","LookUp",true,false)
            self.ControlsBound=true
        }
    }
    [ue_function(BlueprintCallable)]
    def AddScore(amount:int):void { self.Score+=amount;self.Logic.Count+=1 }
    [ue_function(BlueprintCallable)]
    def MoveForward(value:float):void { ue_character_move(ue_character_self(),value,0.0f) }
    [ue_function(BlueprintCallable)]
    def MoveRight(value:float):void { ue_character_move(ue_character_self(),0.0f,value) }
    [ue_function(BlueprintCallable)]
    def Turn(value:float):void { ue_character_look(ue_character_self(),value,0.0f) }
    [ue_function(BlueprintCallable)]
    def LookUp(value:float):void { ue_character_look(ue_character_self(),0.0f,value) }
}
