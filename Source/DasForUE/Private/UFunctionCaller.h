#pragma once

class UFunctionCaller {
    uint8* params;
    UFunction* function;
    uint8* memoryFrame;
public:
    static void callUFunctionOn(UObject* executor, FString& functionName, void* inParams) {
        UFunctionCaller(executor->GetClass(), functionName, inParams).invoke(executor);
    }
    static void callUFunctionOnClass(UClass* clazz, FString& functionName, void* InParams) {
        UFunctionCaller(clazz, functionName, InParams).invoke(clazz->GetDefaultObject());
    }
    
    UFunctionCaller(UFunction* inFunction, void* inParams) {
        checkf(inFunction, TEXT("The function is NULL"));
        function = inFunction;
        params = (uint8*)inParams;
    }
    UFunctionCaller(UClass* clazz, FString &functionName, void* inParams) {
        functionName.TrimToNullTerminator();
        function = clazz->FindFunctionByName(FName(functionName));
        checkf(function, TEXT("Cant find function %s in class %s"), *functionName, *clazz->GetName());
        params = (uint8*)inParams;
        
    }

    void invoke(UObject* executor) {
        // struct Params {
        // 	int32 A;
        // 	FString B;
        // 	int32 ToReturn;
        // };
        executor->ProcessEvent(function, params);

        // FProperty* ReturnProp = Function->GetReturnProperty();
        // if(ReturnProp) {
        // 	FPropertyGetter(ReturnProp, ReturnResult, Params);
        // }
    }
};

