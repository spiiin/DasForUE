options gen2
require ue_vector_math

[export]
def main(input : int) : int {
    let v2 = ue_vector2d(3.0lf, 4.0lf)
    verify(v2.xx.x == 3.0lf && v2.xx.y == 3.0lf)
    verify(v2.xy.x == 3.0lf && v2.xy.y == 4.0lf)
    verify(v2.yx.x == 4.0lf && v2.yx.y == 3.0lf)
    verify(v2.yy.x == 4.0lf && v2.yy.y == 4.0lf)
    verify(v2.xxx.x == 3.0lf && v2.xxx.y == 3.0lf && v2.xxx.z == 3.0lf)
    verify(v2.xxy.x == 3.0lf && v2.xxy.y == 3.0lf && v2.xxy.z == 4.0lf)
    verify(v2.xyx.x == 3.0lf && v2.xyx.y == 4.0lf && v2.xyx.z == 3.0lf)
    verify(v2.xyy.x == 3.0lf && v2.xyy.y == 4.0lf && v2.xyy.z == 4.0lf)
    verify(v2.yxx.x == 4.0lf && v2.yxx.y == 3.0lf && v2.yxx.z == 3.0lf)
    verify(v2.yxy.x == 4.0lf && v2.yxy.y == 3.0lf && v2.yxy.z == 4.0lf)
    verify(v2.yyx.x == 4.0lf && v2.yyx.y == 4.0lf && v2.yyx.z == 3.0lf)
    verify(v2.yyy.x == 4.0lf && v2.yyy.y == 4.0lf && v2.yyy.z == 4.0lf)
    var write_v2_xy = v2
    write_v2_xy.xy = ue_vector2d(7.0lf, 8.0lf)
    verify(write_v2_xy.x == 7.0lf && write_v2_xy.y == 8.0lf)
    var write_v2_yx = v2
    write_v2_yx.yx = ue_vector2d(7.0lf, 8.0lf)
    verify(write_v2_yx.y == 7.0lf && write_v2_yx.x == 8.0lf)
    let v3 = ue_vector(3.0lf, 4.0lf, 12.0lf)
    verify(v3.xx.x == 3.0lf && v3.xx.y == 3.0lf)
    verify(v3.xy.x == 3.0lf && v3.xy.y == 4.0lf)
    verify(v3.xz.x == 3.0lf && v3.xz.y == 12.0lf)
    verify(v3.yx.x == 4.0lf && v3.yx.y == 3.0lf)
    verify(v3.yy.x == 4.0lf && v3.yy.y == 4.0lf)
    verify(v3.yz.x == 4.0lf && v3.yz.y == 12.0lf)
    verify(v3.zx.x == 12.0lf && v3.zx.y == 3.0lf)
    verify(v3.zy.x == 12.0lf && v3.zy.y == 4.0lf)
    verify(v3.zz.x == 12.0lf && v3.zz.y == 12.0lf)
    verify(v3.xxx.x == 3.0lf && v3.xxx.y == 3.0lf && v3.xxx.z == 3.0lf)
    verify(v3.xxy.x == 3.0lf && v3.xxy.y == 3.0lf && v3.xxy.z == 4.0lf)
    verify(v3.xxz.x == 3.0lf && v3.xxz.y == 3.0lf && v3.xxz.z == 12.0lf)
    verify(v3.xyx.x == 3.0lf && v3.xyx.y == 4.0lf && v3.xyx.z == 3.0lf)
    verify(v3.xyy.x == 3.0lf && v3.xyy.y == 4.0lf && v3.xyy.z == 4.0lf)
    verify(v3.xyz.x == 3.0lf && v3.xyz.y == 4.0lf && v3.xyz.z == 12.0lf)
    verify(v3.xzx.x == 3.0lf && v3.xzx.y == 12.0lf && v3.xzx.z == 3.0lf)
    verify(v3.xzy.x == 3.0lf && v3.xzy.y == 12.0lf && v3.xzy.z == 4.0lf)
    verify(v3.xzz.x == 3.0lf && v3.xzz.y == 12.0lf && v3.xzz.z == 12.0lf)
    verify(v3.yxx.x == 4.0lf && v3.yxx.y == 3.0lf && v3.yxx.z == 3.0lf)
    verify(v3.yxy.x == 4.0lf && v3.yxy.y == 3.0lf && v3.yxy.z == 4.0lf)
    verify(v3.yxz.x == 4.0lf && v3.yxz.y == 3.0lf && v3.yxz.z == 12.0lf)
    verify(v3.yyx.x == 4.0lf && v3.yyx.y == 4.0lf && v3.yyx.z == 3.0lf)
    verify(v3.yyy.x == 4.0lf && v3.yyy.y == 4.0lf && v3.yyy.z == 4.0lf)
    verify(v3.yyz.x == 4.0lf && v3.yyz.y == 4.0lf && v3.yyz.z == 12.0lf)
    verify(v3.yzx.x == 4.0lf && v3.yzx.y == 12.0lf && v3.yzx.z == 3.0lf)
    verify(v3.yzy.x == 4.0lf && v3.yzy.y == 12.0lf && v3.yzy.z == 4.0lf)
    verify(v3.yzz.x == 4.0lf && v3.yzz.y == 12.0lf && v3.yzz.z == 12.0lf)
    verify(v3.zxx.x == 12.0lf && v3.zxx.y == 3.0lf && v3.zxx.z == 3.0lf)
    verify(v3.zxy.x == 12.0lf && v3.zxy.y == 3.0lf && v3.zxy.z == 4.0lf)
    verify(v3.zxz.x == 12.0lf && v3.zxz.y == 3.0lf && v3.zxz.z == 12.0lf)
    verify(v3.zyx.x == 12.0lf && v3.zyx.y == 4.0lf && v3.zyx.z == 3.0lf)
    verify(v3.zyy.x == 12.0lf && v3.zyy.y == 4.0lf && v3.zyy.z == 4.0lf)
    verify(v3.zyz.x == 12.0lf && v3.zyz.y == 4.0lf && v3.zyz.z == 12.0lf)
    verify(v3.zzx.x == 12.0lf && v3.zzx.y == 12.0lf && v3.zzx.z == 3.0lf)
    verify(v3.zzy.x == 12.0lf && v3.zzy.y == 12.0lf && v3.zzy.z == 4.0lf)
    verify(v3.zzz.x == 12.0lf && v3.zzz.y == 12.0lf && v3.zzz.z == 12.0lf)
    var write_v3_xy = v3
    write_v3_xy.xy = ue_vector2d(7.0lf, 8.0lf)
    verify(write_v3_xy.x == 7.0lf && write_v3_xy.y == 8.0lf)
    var write_v3_xz = v3
    write_v3_xz.xz = ue_vector2d(7.0lf, 8.0lf)
    verify(write_v3_xz.x == 7.0lf && write_v3_xz.z == 8.0lf)
    var write_v3_yx = v3
    write_v3_yx.yx = ue_vector2d(7.0lf, 8.0lf)
    verify(write_v3_yx.y == 7.0lf && write_v3_yx.x == 8.0lf)
    var write_v3_yz = v3
    write_v3_yz.yz = ue_vector2d(7.0lf, 8.0lf)
    verify(write_v3_yz.y == 7.0lf && write_v3_yz.z == 8.0lf)
    var write_v3_zx = v3
    write_v3_zx.zx = ue_vector2d(7.0lf, 8.0lf)
    verify(write_v3_zx.z == 7.0lf && write_v3_zx.x == 8.0lf)
    var write_v3_zy = v3
    write_v3_zy.zy = ue_vector2d(7.0lf, 8.0lf)
    verify(write_v3_zy.z == 7.0lf && write_v3_zy.y == 8.0lf)
    var write_v3_xyz = v3
    write_v3_xyz.xyz = ue_vector(7.0lf, 8.0lf, 9.0lf)
    verify(write_v3_xyz.x == 7.0lf && write_v3_xyz.y == 8.0lf && write_v3_xyz.z == 9.0lf)
    var write_v3_xzy = v3
    write_v3_xzy.xzy = ue_vector(7.0lf, 8.0lf, 9.0lf)
    verify(write_v3_xzy.x == 7.0lf && write_v3_xzy.z == 8.0lf && write_v3_xzy.y == 9.0lf)
    var write_v3_yxz = v3
    write_v3_yxz.yxz = ue_vector(7.0lf, 8.0lf, 9.0lf)
    verify(write_v3_yxz.y == 7.0lf && write_v3_yxz.x == 8.0lf && write_v3_yxz.z == 9.0lf)
    var write_v3_yzx = v3
    write_v3_yzx.yzx = ue_vector(7.0lf, 8.0lf, 9.0lf)
    verify(write_v3_yzx.y == 7.0lf && write_v3_yzx.z == 8.0lf && write_v3_yzx.x == 9.0lf)
    var write_v3_zxy = v3
    write_v3_zxy.zxy = ue_vector(7.0lf, 8.0lf, 9.0lf)
    verify(write_v3_zxy.z == 7.0lf && write_v3_zxy.x == 8.0lf && write_v3_zxy.y == 9.0lf)
    var write_v3_zyx = v3
    write_v3_zyx.zyx = ue_vector(7.0lf, 8.0lf, 9.0lf)
    verify(write_v3_zyx.z == 7.0lf && write_v3_zyx.y == 8.0lf && write_v3_zyx.x == 9.0lf)
    var alias = ue_vector(1.0lf, 2.0lf, 3.0lf)
    alias.zyx = alias
    verify(alias.x == 3.0lf && alias.y == 2.0lf && alias.z == 1.0lf)
    let origin = ue_vector(1000000000000.0lf, -1000000000000.0lf, 500000000000.0lf)
    let delta = (origin + v3) - origin
    verify(delta == v3)
    verify(length_sq(delta) == 169.0lf && length(delta.xy) == 5.0lf)
    verify(dot(delta, UE_VECTOR_FORWARD) == 3.0lf)
    verify(UE_VECTOR_ZERO + UE_VECTOR_ONE == ue_vector(1.0lf, 1.0lf, 1.0lf))
    verify(UE_VECTOR_RIGHT + UE_VECTOR_LEFT == UE_VECTOR_ZERO)
    verify(UE_VECTOR_UP + UE_VECTOR_DOWN == UE_VECTOR_ZERO)
    verify(UE_VECTOR_FORWARD + UE_VECTOR_BACKWARD == UE_VECTOR_ZERO)
    verify(UE_VECTOR2D_RIGHT + UE_VECTOR2D_LEFT == UE_VECTOR2D_ZERO)
    verify(UE_VECTOR2D_UP + UE_VECTOR2D_DOWN == UE_VECTOR2D_ZERO)
    return input + 2
}
