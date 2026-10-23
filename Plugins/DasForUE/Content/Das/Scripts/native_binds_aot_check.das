options gen2
require ue_foundation
require ue_time
require ue_math

// AOT check of the DasNativeBindsManifest commandlet: a script that calls native binds (DasNativeBinds.h). The
// ue_get_self_int call goes through the guarded UE bridge, which the AOT probe requires before it keeps the output.
[export]
def main(input:int):int {
    return days_in_year(2023) + int(f_interp_to(0.0lf, 10.0lf, 0.1lf, 5.0lf)) + ue_get_self_int("health") + input
}

// The same bridge call without native binds: it is generated for AOT, main shows whether native calls are.
[export]
def bridge_only(input:int):int {
    return ue_get_self_int("health") + input
}
