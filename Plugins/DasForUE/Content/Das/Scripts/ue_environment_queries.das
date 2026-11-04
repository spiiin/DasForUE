options gen2
module ue_environment_queries public
require ue_native_gameplay public

typedef EnvQueryRef = UENativeObjectRef("/Script/AIModule.EnvQuery")
typedef EnvQueryRunRef = UENativeObjectRef("/Script/AIModule.EnvQueryInstanceBlueprintWrapper")

[ue_native_enum(path="/Script/AIModule.EEnvQueryStatus")]
enum EnvQueryStatus {
    Processing = 0
    Success = 1
    Failed = 2
    Aborted = 3
    OwnerLost = 4
    MissingParam = 5
}

// Keep the returned reference in a reflected property until completion/cancellation.
// Its native OnQueryFinishedEvent supports typed += / -= subscriptions.
def ue_environment_query(query : EnvQueryRef; querier : ActorRef) : EnvQueryRunRef {
    let call = ue_begin_static_call("/Script/AIModule.EnvQueryManager", "RunEQSQuery", "WorldContextObject")
    ue_arg(call, "QueryTemplate", query)
    ue_arg(call, "Querier", querier)
    ue_arg_enum(call, "RunMode", "/Script/AIModule.EEnvQueryRunMode", "SingleResult")
    ue_arg_class(call, "WrapperClass", ue_class_null())
    ue_invoke_call(call)
    let result = ue_ref(type<EnvQueryRunRef>, ue_result_object(call, "ReturnValue"))
    ue_release_call(call)
    return result
}

def ue_environment_query_locations(query : EnvQueryRunRef) : array<UEVector> {
    let call = ue_begin_call(query.handle, "GetQueryResultsAsLocations")
    ue_invoke_call(call)
    var points : array<UEVector>
    if (ue_result_bool(call, "ReturnValue")) {
        for (index in range(ue_result_array_size(call, "ResultLocations"))) {
            push(points, __ue_method_result_vector(call, "ResultLocations.{index}"))
        }
    }
    ue_release_call(call)
    return <- points
}

def ue_environment_query_abort(query : EnvQueryRunRef) : bool {
    return ue_is_valid(query) && __ue_environment_query_abort(query.handle)
}
