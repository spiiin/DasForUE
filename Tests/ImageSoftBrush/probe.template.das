options gen2
options gc = true
require _MODULE_
require ue_image_soft_brush
typedef SoftTex = $UESoftObject<C_Texture2D_2db8ea5c>
typedef SoftMat = $UESoftObject<C_MaterialInterface_d64f3063>
var image:UObjectHandle
var sync:UObjectHandle
def img():C_Image_61e460fb {return C_Image_61e460fb_from(image)}
def asset(i:int):string {
 let names=fixed_array("TexA","TexB","TexC","TexD","TexE","TexF","TexG","TexH","Mat","Missing")
 return "/DasSoftBrushProbe/{names[i]}.{names[i]}"
}
def code(state:string):int {
 if(state=="none"){return 0}
 if(state=="loading"){return 1}
 if(state=="ready"){return 2}
 if(state=="failed"){return 3}
 if(state=="cancelled"){return 4}
 return -1
}
[export]
def setup(i:UObjectHandle;s:UObjectHandle):int {image:=i;sync:=s;return 1}
[export]
def status(input:int):int {return code(ue_image_soft_brush_state(image))}
[export]
def load_texture(input:int):int {ue_image_SetBrushFromSoftTexture(img(),ue_soft(type<SoftTex>,asset(input)),true);return status(0)}
[export]
def load_material(input:int):int {ue_image_SetBrushFromSoftMaterial(img(),ue_soft(type<SoftMat>,asset(input)));return status(0)}
[export]
def supersede(input:int):int {
 ue_image_SetBrushFromSoftTexture(img(),ue_soft(type<SoftTex>,asset(1)),false)
 ue_image_SetBrushFromSoftTexture(img(),ue_soft(type<SoftTex>,asset(2)),false)
 return status(0)
}
[export]
def load_then_sync(input:int):int {
 ue_image_SetBrushFromSoftTexture(img(),ue_soft(type<SoftTex>,asset(input)),false)
 if(status(0)!=1){return -1}
 ue_image_SetBrushFromTexture(img(),C_Texture2D_2db8ea5c_from(sync),false)
 return status(0)
}
[export]
def clear_texture(input:int):int {var none:SoftTex;ue_image_SetBrushFromSoftTexture(img(),none,false);return status(0)}
