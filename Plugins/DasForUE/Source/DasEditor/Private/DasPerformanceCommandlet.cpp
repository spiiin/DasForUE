#include "DasPerformanceCommandlet.h"
#include "DasRuntime.h"
#include "Misc/Parse.h"
#include "Misc/FileHelper.h"

UDasPerformanceCommandlet::UDasPerformanceCommandlet(){IsClient=false;IsServer=false;IsEditor=true;LogToConsole=true;}
int32 UDasPerformanceCommandlet::Main(const FString& Params){
    int32 Iterations=64,Samples=9;FString Output,Report,Json;
    FParse::Value(*Params,TEXT("PerfIterations="),Iterations);FParse::Value(*Params,TEXT("PerfSamples="),Samples);
    FParse::Value(*Params,TEXT("PerfOutput="),Output);
    bool Passed=RunDasPerformanceProbe(Report,Json,Iterations,Samples);
    if(!Output.IsEmpty())Passed &= FFileHelper::SaveStringToFile(Json,*Output,FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM);
    UE_LOG(LogTemp,Display,TEXT("%s"),*Report);UE_LOG(LogTemp,Display,TEXT("DAS_PERFORMANCE_RESULT=%s"),Passed?TEXT("PASS"):TEXT("FAIL"));
    return Passed?0:1;
}
