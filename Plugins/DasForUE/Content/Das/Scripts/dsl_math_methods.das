options gen2
require ue_dsl
var remembered = ue_transform_identity()
[ue_class(module_id="math",Blueprintable)]
class MathBridge {
    [ue_function(BlueprintCallable)]
    def Shift(value:UEVector; delta:double):UEVector {
        return ue_vector(value.x + delta, value.y, value.z)
    }
    [ue_function(BlueprintCallable)]
    def Turn(value:UERotator; yaw:double):UERotator {
        return ue_rotator(value.pitch, value.yaw + yaw, value.roll)
    }
    [ue_function(BlueprintCallable)]
    def PassTransform(value:UETransform):UETransform { return value }
    [ue_function(BlueprintCallable)]
    def Apply(value:UETransform; position:UEVector):UEVector {
        return ue_transform_position(value, position)
    }
    [ue_function(BlueprintCallable)]
    def Build(translation:UEVector; rotation:UERotator; scale:UEVector):UETransform {
        return ue_transform(translation,rotation,scale)
    }
    [ue_function(BlueprintCallable)]
    def Remember(value:UETransform):void { remembered = value }
    [ue_function(BlueprintCallable)]
    def Recall():UETransform { return remembered }
}
