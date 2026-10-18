options gen2
require @PAWN_AI@
[export]
def main(phase:int):int {
    if(phase==0) {
        let pawn=C_Pawn_ba3732bb_from(ue_self_object())
        let character=C_Character_ab4a5baa_from(ue_self_object())
        ue_pawn_AddMovementInput(pawn,ue_vector(1.0lf,0.0lf,0.0lf),0.5,true)
        verify(ue_pawn_GetPendingMovementInputVector(pawn).x==0.5lf,"pending movement input")
        verify(ue_pawn_ConsumeMovementInputVector(pawn).x==0.5lf,"consume movement input")
        verify(ue_pawn_GetPendingMovementInputVector(pawn).x==0.0lf,"consume clears input")
        let movement=C_CharacterMovementComponent_a8110e7e_from(C_PawnMovementComponent_653cf1d0_raw(ue_pawn_GetMovementComponent(pawn)))
        verify(__ue_same_object(C_Character_ab4a5baa_raw(ue_character_move_GetCharacterOwner(movement)),ue_self_object()),"movement owner identity")
        ue_character_move_SetMovementMode(movement,E_EMovementMode_f8bcb8c7.v_MOVE_Flying,uint8(0))
        ue_character_LaunchCharacter(character,ue_vector(100.0lf,0.0lf,150.0lf),true,true)
        var rejected=false
        try { let bad=ue_path_GetPathLength(C_NavigationPath_550114bf_from(ue_self_object())) } recover { rejected=true }
        verify(rejected,"wrong receiver class")
        rejected=false
        try { let bad=ue_pawn_ConsumeMovementInputVector(C_Pawn_ba3732bb_null()) } recover { rejected=true }
        verify(rejected,"null receiver")
    } elif(phase==1) {
        let controller=C_Controller_78a9db5e_from(ue_self_object())
        let ai=C_AIController_2e0321e8_from(ue_self_object())
        let pawn=ue_controller_K2_GetPawn(controller)
        verify(C_Pawn_ba3732bb_valid(pawn),"possessed pawn")
        verify(__ue_same_object(C_Controller_78a9db5e_raw(ue_pawn_GetController(pawn)),ue_self_object()),"controller identity")
        let actor=C_Actor_adf65d5b_from(C_Pawn_ba3732bb_raw(pawn))
        ue_ai_controller_K2_SetFocus(ai,actor)
        verify(__ue_same_object(C_Actor_adf65d5b_raw(ue_ai_controller_GetFocusActor(ai)),C_Actor_adf65d5b_raw(actor)),"focus target")
        ue_ai_controller_K2_ClearFocus(ai)
        verify(!C_Actor_adf65d5b_valid(ue_ai_controller_GetFocusActor(ai)),"cleared focus is nullable")
        ue_controller_UnPossess(controller)
        verify(!C_Pawn_ba3732bb_valid(ue_controller_K2_GetPawn(controller)),"unpossess is nullable")
        ue_controller_Possess(controller,pawn)
        verify(__ue_same_object(C_Pawn_ba3732bb_raw(ue_controller_K2_GetPawn(controller)),C_Pawn_ba3732bb_raw(pawn)),"repossess")
    } elif(phase==2 || phase==3) {
        let path=C_NavigationPath_550114bf_from(ue_self_object())
        if(phase==2) {
            verify(!ue_path_IsValid(path) && !ue_path_IsPartial(path),"empty path state")
            verify(ue_path_GetPathLength(path)==-1.0lf && ue_path_GetPathCost(path)==-1.0lf,"native empty path sentinels")
        } else {
            verify(ue_path_IsValid(path) && ue_path_IsPartial(path),"constructed partial path")
            verify(ue_path_GetPathLength(path)==200.0lf,"native path length")
        }
    } elif(phase==4) {
        let perception=C_AIPerceptionComponent_e0f15310_from(ue_self_object())
        let sense=__ue_class_loaded("/Script/AIModule.AISense_Sight","/Script/CoreUObject.Object","")
        let current <- ue_perception_GetCurrentlyPerceivedActors(perception,sense)
        let known <- ue_perception_GetKnownPerceivedActors(perception,sense)
        verify(length(current)==0 && length(known)==0,"empty perceived Actor arrays")
        let owner=C_Actor_adf65d5b_from(ue_component_owner(ue_self_object()))
        let unknown <- ue_perception_GetActorsPerception(perception,owner)
        verify(!unknown.out_returnvalue,"unseen owner has no perception record")
        var rejected=false
        try { let bad <- ue_perception_GetKnownPerceivedActors(perception,__ue_class_loaded("/Script/Engine.Actor","/Script/CoreUObject.Object","")) } recover { rejected=true }
        verify(rejected,"wrong sense MetaClass")
    } else { return -1 }
    return 42
}
