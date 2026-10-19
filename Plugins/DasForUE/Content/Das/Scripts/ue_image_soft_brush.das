options gen2
module ue_image_soft_brush public
require ue_foundation public
// Query in the same VM that requested the managed brush.
def ue_image_soft_brush_state(image:UObjectHandle):string {return __ue_image_soft_status(image)}
def ue_image_soft_brush_error(image:UObjectHandle):string {return __ue_image_soft_error(image)}
