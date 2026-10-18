options gen2
module ue_native_delegate public
require ue_foundation public

// Endpoint has the same weak UObject semantics as a native FScriptDelegate.
struct UEFunctionDelegate { target:UObjectHandle; method:string }
def ue_function_delegate(target:UObjectHandle; method:string):UEFunctionDelegate { return UEFunctionDelegate(target=target,method=method) }
def ue_function_delegate(target:auto; method:string):UEFunctionDelegate { return ue_function_delegate(target.handle,method) }
def ue_function_delegate_null():UEFunctionDelegate { return UEFunctionDelegate() }
