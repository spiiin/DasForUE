options gen2
module ue_widget_animation public
require ue_foundation public
// Weak context-owned capability; never extends native widget or animation state lifetime.
def ue_widget_animation_is_valid(value:UEWidgetAnimationHandle):bool {return __ue_widget_animation_valid(value)}
def ue_widget_animation_is_playing(value:UEWidgetAnimationHandle):bool {return __ue_widget_animation_playing(value)}
