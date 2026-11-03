options gen2
module ue_navigation public
require ue_gameplay_services public

typedef NavigationPathRef = UENativeObjectRef("/Script/NavigationSystem.NavigationPath")

enum NavigationQueryStatus {
    Invalid = 0
    Error = 1
    Fail = 2
    Success = 3
}

struct NavigationDistance {
    Status : NavigationQueryStatus = NavigationQueryStatus.Invalid
    Length : double = 0.0lf
}

// WorldContext is supplied by the owner of the current script call.
def ue_navigation_path(start : UEVector; finish : UEVector;
    querier : ActorRef = default<ActorRef>) : NavigationPathRef {
    let call = ue_begin_static_call("/Script/NavigationSystem.NavigationSystemV1",
        "FindPathToLocationSynchronously", "WorldContextObject")
    ue_arg(call, "PathStart", start)
    ue_arg(call, "PathEnd", finish)
    ue_arg(call, "PathfindingContext", querier)
    ue_arg_class(call, "FilterClass", ue_class_null())
    ue_invoke_call(call)
    let result = ue_ref(type<NavigationPathRef>, ue_result_object(call, "ReturnValue"))
    ue_release_call(call)
    return result
}

// Preserve the engine's corridor estimate and distinguish unavailable data from no route.
def ue_navigation_distance(start : UEVector; finish : UEVector) : NavigationDistance {
    let call = ue_begin_static_call("/Script/NavigationSystem.NavigationSystemV1",
        "GetPathLength", "WorldContextObject")
    ue_arg(call, "PathStart", start)
    ue_arg(call, "PathEnd", finish)
    ue_arg(call, "NavData", default<ObjectRef>)
    ue_arg_class(call, "FilterClass", ue_class_null())
    ue_invoke_call(call)
    let symbol = ue_result_enum(call, "ReturnValue", "/Script/Engine.ENavigationQueryResult")
    var result = NavigationDistance()
    result.Length = ue_result_double(call, "PathLength")
    if (symbol == "Success") { result.Status = NavigationQueryStatus.Success }
    elif (symbol == "Fail") { result.Status = NavigationQueryStatus.Fail }
    elif (symbol == "Error") { result.Status = NavigationQueryStatus.Error }
    ue_release_call(call)
    return result
}
