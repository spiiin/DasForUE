#include "DasCurveTablesCommandlet.h"
#include "DasRuntime.h"
#include "Engine/CurveTable.h"
#include "Engine/DataTable.h"
#include "Curves/RichCurve.h"
#include "UObject/StrongObjectPtr.h"
#include "UObject/GarbageCollection.h"
#include "Misc/FileHelper.h"
#include "Misc/ScopeExit.h"
#include "HAL/FileManager.h"

UDasCurveTablesCommandlet::UDasCurveTablesCommandlet(){IsClient=false;IsServer=false;IsEditor=true;LogToConsole=true;}
int32 UDasCurveTablesCommandlet::Main(const FString& Params){
 auto& R=FModuleManager::LoadModuleChecked<FDasRuntimeModule>(TEXT("DasRuntime"));
 int32 Checks=0,Failures=0;auto Check=[&](bool Good,const TCHAR* Label){++Checks;if(!Good)++Failures;UE_LOG(LogTemp,Display,TEXT("DAS_CURVE_TABLES_%s %s"),Good?TEXT("PASS"):TEXT("FAIL"),Label);return Good;};
 const FString ProbePath=R.ScriptPath(TEXT("curvetable_runtime_probe_")+FGuid::NewGuid().ToString(EGuidFormats::Digits)+TEXT(".das"));
 FString Fixture,Probe;if(!FParse::Value(*Params,TEXT("Fixture="),Fixture) || !FFileHelper::LoadFileToString(Probe,*Fixture) || !FFileHelper::SaveStringToFile(Probe,*ProbePath,FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM))return 2;
 ON_SCOPE_EXIT{IFileManager::Get().Delete(*ProbePath,false,true);};
 TStrongObjectPtr<UCurveTable> Simple(NewObject<UCurveTable>()),Rich(NewObject<UCurveTable>());
 TStrongObjectPtr<UObject> Wrong(NewObject<UDataTable>());
 ON_SCOPE_EXIT{R.ReleaseObject(Simple.Get());R.ReleaseObject(Rich.Get());R.ReleaseObject(Wrong.Get());};
 auto& Line=Simple->AddSimpleCurve(TEXT("Damage"));Line.AddKey(0.f,10.f);Line.AddKey(2.f,30.f);Line.SetKeyInterpMode(RCIM_Linear);Line.PreInfinityExtrap=RCCE_Linear;Line.PostInfinityExtrap=RCCE_Linear;
 Simple->AddSimpleCurve(TEXT("Empty"));Simple->AddSimpleCurve(TEXT("StoredDefault")).SetDefaultValue(7.f);
 auto& Cubic=Rich->AddRichCurve(TEXT("Damage"));auto A=Cubic.AddKey(0.f,0.f),B=Cubic.AddKey(2.f,20.f);
 for(auto Key:{A,B}){Cubic.SetKeyInterpMode(Key,RCIM_Cubic,false);Cubic.SetKeyTangentMode(Key,RCTM_User,false);Cubic.GetKey(Key).ArriveTangent=0.f;Cubic.GetKey(Key).LeaveTangent=0.f;}
 Rich->AddRichCurve(TEXT("Empty"));Rich->AddRichCurve(TEXT("StoredDefault")).SetDefaultValue(7.f);
 Check(Simple->GetCurveTableMode()==ECurveTableMode::SimpleCurves && Rich->GetCurveTableMode()==ECurveTableMode::RichCurves && FMath::IsNearlyEqual(Cubic.Eval(.5f),3.125f),TEXT("native_simple_linear_and_rich_cubic_fixtures"));
 auto Call=[&](UObject* Owner,const TCHAR* Entry,int32 Input=0){auto V=R.RunNamedPersistent(ProbePath,Entry,Input,Owner);if(!V.bSucceeded || V.Value!=1)UE_LOG(LogTemp,Display,TEXT("CurveTable VM entry=%s value=%d: %s"),Entry,V.Value,*V.Error);return V.bSucceeded && V.Value==1;};
 Check(Call(Simple.Get(),TEXT("read"),0),TEXT("simple_names_lookup_interpolation_extrapolation_defaults_and_refusals"));
 Check(Call(Rich.Get(),TEXT("read"),1),TEXT("rich_names_lookup_cubic_extrapolation_defaults_and_refusals"));
 Simple->EmptyTable();Rich->EmptyTable();CollectGarbage(RF_NoFlags);
 Check(Call(Simple.Get(),TEXT("after_clear")) && Call(Rich.Get(),TEXT("after_clear")),TEXT("empty_tables_and_detached_names_after_clear_and_gc"));
 Check(Call(Wrong.Get(),TEXT("wrong_owner")),TEXT("non_curve_table_handle_rejected"));
 UE_LOG(LogTemp,Display,TEXT("DAS_CURVE_TABLES_RESULT=%s checks=%d"),Failures?TEXT("FAIL"):TEXT("PASS"),Checks);return Failures?1:0;
}
