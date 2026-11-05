options gen2
module ue_navigation public
require ue_gameplay_services public
require math

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

// A populated navigation system can still be rebuilding its tiles.
def ue_navigation_is_building() : bool {
    let call = ue_begin_static_call("/Script/NavigationSystem.NavigationSystemV1",
        "IsNavigationBeingBuiltOrLocked", "WorldContextObject")
    ue_invoke_call(call)
    let building = ue_result_bool(call, "ReturnValue")
    ue_release_call(call)
    return building
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


struct NavigationPoint {
    Found : bool = false
    Location : UEVector
}

typedef NavigationSystemRef = UENativeObjectRef("/Script/NavigationSystem.NavigationSystemV1")

// Resolve once per block. References remain checked world/context handles, never raw pointers.
def with_ue_navigation(blk : block<(navigation : NavigationSystemRef) : void>) : void {
    let navigation = ue_ref(type<NavigationSystemRef>, __ue_navigation_system())
    invoke(blk, navigation)
}

def ue_navigation_random_point(navigation : NavigationSystemRef; origin : UEVector; radius : float) : NavigationPoint {
    var result = NavigationPoint(Location=origin)
    if (!is_finite(radius) || radius < 0.0f) { return result }
    result.Found = __ue_navigation_random_point(navigation.handle, origin, radius, result.Location)
    return result
}

def ue_navigation_random_point(origin : UEVector; radius : float) : NavigationPoint {
    return ue_navigation_random_point(ue_ref(type<NavigationSystemRef>, __ue_navigation_system()), origin, radius)
}

// Exactly count entries; an unavailable navigation point falls back to origin.
def ue_navigation_random_points(navigation : NavigationSystemRef; origin : UEVector; radius : float; count : int) : array<UEVector> {
    var points : array<UEVector>
    __ue_navigation_random_points(navigation.handle, origin, radius, count, points)
    return <- points
}
