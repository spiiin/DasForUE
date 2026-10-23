#include "DasBuiltinValuesCommandlet.h"
#include "DasRuntime.h"
#include "DasFoundationManifest.h"
#include "Kismet/KismetMathLibrary.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "Misc/Parse.h"
#include "Misc/ScopeExit.h"
#include "HAL/FileManager.h"
#include "DasTypeService.h"
#include "DasDSLService.h"
#include "DasAuthoring.h"
#include "Engine/Blueprint.h"
#include "StructUtils/UserDefinedStruct.h"
#include "UObject/StructOnScope.h"
#include "UObject/StrongObjectPtr.h"
#include "UObject/UnrealType.h"
#include "AssetRegistry/AssetRegistryModule.h"

namespace
{
FString Number(double V)
{
    FString S = FString::Printf(TEXT("%.17g"), V);
    if (!S.Contains(TEXT(".")) && !S.Contains(TEXT("e")) && !S.Contains(TEXT("E")))
        S += TEXT(".0");
    return S + TEXT("lf");
}
FString Vec(FVector V)
{
    return FString::Printf(TEXT("ue_vector(%s,%s,%s)"), *Number(V.X), *Number(V.Y), *Number(V.Z));
}
FString Vec2(FVector2D V)
{
    return FString::Printf(TEXT("ue_vector2d(%s,%s)"), *Number(V.X), *Number(V.Y));
}
FString Rot(FRotator V)
{
    return FString::Printf(TEXT("ue_rotator(%s,%s,%s)"), *Number(V.Pitch), *Number(V.Yaw), *Number(V.Roll));
}
FString Transform(const FTransform& T)
{
    return FString::Printf(TEXT("ue_transform(%s,%s,%s)"), *Vec(T.GetTranslation()), *Rot(T.Rotator()),
                                *Vec(T.GetScale3D()));
}
FString Near(const FString& Expr, FVector V)
{
    return FString::Printf(TEXT("verify(ue_nearly_equal(%s,%s,0.000000001lf))\n"), *Expr, *Vec(V));
}
FString Near2(const FString& Expr, FVector2D V)
{
    return FString::Printf(TEXT("verify(ue_nearly_equal(%s,%s,0.000000001lf))\n"), *Expr, *Vec2(V));
}
FString NearR(const FString& Expr, FRotator V)
{
    return FString::Printf(TEXT("verify(ue_nearly_equal(%s,%s,0.000000001lf))\n"), *Expr, *Rot(V));
}
FString Scalar(const FString& Expr, double V)
{
    return FString::Printf(
        TEXT("{ let difference=(%s)-(%s)\nverify(difference<0.000000001lf && difference> -0.000000001lf) }\n"), *Expr,
             *Number(V));
}
}
#include "DasCompleteBuiltinValues.inl"

UDasBuiltinValuesCommandlet::UDasBuiltinValuesCommandlet()
{
    IsClient = false;
    IsServer = false;
    IsEditor = true;
    LogToConsole = true;
}
int32 UDasBuiltinValuesCommandlet::Main(const FString& Params)
{
    int32 Checks = 0, Failures = 0;
    auto Check = [&](bool Good, const TCHAR* Name, const FString& Error = FString())
    {
        ++Checks;
        if (!Good)
            ++Failures;
        UE_LOG(LogTemp, Display, TEXT("DAS_BUILTIN_VALUES_%s %s %s"), Good ? TEXT("PASS"): TEXT("FAIL"), Name, *Error);
    };
    auto& Runtime = FModuleManager::LoadModuleChecked<FDasRuntimeModule>(TEXT("DasRuntime"));
    const FString Path =
        Runtime.ScriptPath(TEXT("builtin_values_")+ FGuid::NewGuid().ToString(EGuidFormats::Digits) + TEXT(".das"));
    ON_SCOPE_EXIT
    {
        IFileManager::Get().Delete(*Path, false, true);
    };
    if (FParse::Param(*Params, TEXT("Complete")))
        return RunCompleteBuiltinTypes(Runtime, Path);
    auto Run = [&](const TCHAR* Name, const FString& Body, const TCHAR* Error = nullptr, bool CompileError = false)
    {
        const FString Code = TEXT("options gen2\nrequire ue_foundation\n[export]\ndef main(input:int):int {\n")+ Body +
                                  TEXT("\nreturn 42\n}\n");
        if (!FFileHelper::SaveStringToFile(Code, *Path, FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM))
        {
            Check(false, Name, TEXT("Cannot write probe"));
            return;
        }
        const auto R = Runtime.RunFile(Path, 0);
        Check(Error ? !R.bSucceeded && (CompileError ? R.Error.Contains(TEXT("error[")) : R.Error.Contains(Error))
                    : R.bSucceeded && R.Value == 42,
                                        Name, R.Error);
    };
    if (FParse::Param(*Params, TEXT("QuatOnly")))
    {
        FString Body = TEXT("let identity:UEQuat=ue_quat_identity()\nverify(identity.x==0.0lf && identity.y==0.0lf && identity.z==0.0lf && identity.w==1.0lf)\n");
        for (const FRotator& Rotation :
             TArray<FRotator>{FRotator::ZeroRotator, FRotator(25, 80, -15), FRotator(90, 40, 10),
                              FRotator(-90, -130, 35), FRotator(89.99999, 179.999, -179.5), FRotator(390, -450, 720)})
        {
            const FQuat Q = Rotation.Quaternion();
            Body += TEXT("{ let q:UEQuat=ue_quat(") + Rot(Rotation) + TEXT(")\n");
            Body += Scalar(TEXT("q.x"), Q.X) + Scalar(TEXT("q.y"), Q.Y) + Scalar(TEXT("q.z"), Q.Z) +
                                                                                 Scalar(TEXT("q.w"), Q.W);
            Body += NearR(TEXT("ue_rotator(q)"), Q.Rotator());
            Body += NearR(FString::Printf(TEXT("ue_rotator(ue_quat(%s,%s,%s,%s))"), *Number(-Q.X), *Number(-Q.Y),
                                               *Number(-Q.Z), *Number(-Q.W)),
                                          Q.Rotator());
            // At Euler singularities compare the recovered rotation, not the original Euler triple.
            Body += TEXT("let restored=ue_quat(ue_rotator(q))\n");
            Body += TEXT("let dot=q.x*restored.x+q.y*restored.y+q.z*restored.z+q.w*restored.w\nverify(dot*dot>0.999999999lf)\n}\n");
        }
        Body += TEXT("var rejected=false\ntry { let r=ue_rotator(ue_quat(0.0lf,0.0lf,0.0lf,0.0lf)); return int(r.yaw) } recover { rejected=true }\nverify(rejected)\n");
        Body += TEXT("var invalid=ue_quat_identity()\ninvalid.w=2.0lf\nrejected=false\ntry { let r=ue_rotator(invalid); return int(r.yaw) } recover { rejected=true }\nverify(rejected)\n");
        Body += TEXT("invalid=ue_quat_identity()\ninvalid.x=1.0lf/double(input)\nrejected=false\ntry { let r=ue_rotator(invalid); return int(r.yaw) } recover { rejected=true }\nverify(rejected)\n");
        Run(TEXT("quat_explicit_conversions_match_unreal"), Body);
        const FString Repo = FPaths::ConvertRelativePathToFull(FPaths::ProjectDir() / TEXT("../.."));
        FFileHelper::SaveStringToFile(DasFoundationExternManifest(),
                                      *(Repo / TEXT("logs/quat-externs.txt")),
                                        FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM);
        UE_LOG(LogTemp, Display, TEXT("DAS_QUAT_RESULT=%s checks=%d"), Failures ? TEXT("FAIL") : TEXT("PASS"), Checks);
        return Failures ? 1 : 0;
    }
    const FVector A(1.25, -2.5, 3.75), B(-4, 5, 6);
    const FVector2D A2(1.25, -2.5), B2(-4, 5);
    const FRotator RA(25, 80, -15), RB(-10, 20, 45);
    const FString V = TEXT("let a=")+ Vec(A) + TEXT("\nlet b=")+ Vec(B) + TEXT("\n");
    const FString V2 = TEXT("let a=")+ Vec2(A2) + TEXT("\nlet b=")+ Vec2(B2) + TEXT("\n");
    const FString R = TEXT("let a=")+ Rot(RA) + TEXT("\nlet b=")+ Rot(RB) + TEXT("\n");
    Run(TEXT("vector_arithmetic_and_aliasing"),
             V + Near(TEXT("a+b"), A + B) + Near(TEXT("a-b"), A - B) +
                                                 Near(TEXT("-a"), -A) +
                                                      Near(TEXT("a*2.0lf"), A * 2) +
                                                           Near(TEXT("2.0lf*a"), A * 2) +
                                                                Near(TEXT("a/2.0lf"), A / 2) +
                                                                     TEXT("var v=a\nv+=v\nv-=a\nv*=4.0lf\nv/=2.0lf\n")+
                                                                          Near(TEXT("v"), A * 2));
    Run(TEXT("vector_metrics"),
             V + Scalar(TEXT("ue_length(a)"), A.Size()) +
                        TEXT("verify(ue_length_squared(a)==21.875lf)\n")+
                             Near(TEXT("ue_cross(a,b)"), FVector::CrossProduct(A, B)) +
                                  TEXT("verify(ue_dot(a,b)==5.0lf)\n")+
                                       Near(TEXT("ue_normalized(a,0.00000001lf)"), A.GetSafeNormal(UE_SMALL_NUMBER)));
    Run(TEXT("vector2d_arithmetic"),
             V2 + Near2(TEXT("a+b"), A2 + B2) +
                        Near2(TEXT("a-b"), A2 - B2) +
                              Near2(TEXT("-a"), -A2) + TEXT("var v=a\nv+=b\nv-=b\nv*=2.0lf\nv/=2.0lf\nverify(v==a)\n")+
                                                            Near2(TEXT("2.0lf*a"), A2 * 2));
    Run(TEXT("vector2d_metrics"),
             V2 + Scalar(TEXT("ue_cross(a,b)"), FVector2D::CrossProduct(A2, B2)) +
                         TEXT("{\n")+ Scalar(TEXT("ue_dot(a,b)"), FVector2D::DotProduct(A2, B2)) +
                                             TEXT("}\n{\n")+ Scalar(TEXT("ue_distance(a,b)"), (A2 - B2).Size()) +
                                                                    TEXT("}\n")+
                                                                         Near2(TEXT("ue_normalized(a,0.00000001lf)"),
                                                                                    A2.GetSafeNormal(UE_SMALL_NUMBER)));
    Run(TEXT("exact_vs_tolerant_vector_equality"),
        TEXT("let a=ue_vector(1.0lf,2.0lf,3.0lf)\nlet b=ue_vector(1.0000000001lf,2.0lf,3.0lf)\nverify(a!=b && !(a==b))\nverify(ue_nearly_equal(a,b,0.000000001lf))\nverify(!ue_nearly_equal(a,b,0.0lf))"));
    Run(TEXT("rotator_component_arithmetic"),
             R + NearR(TEXT("a+b"), RA + RB) + NearR(TEXT("a-b"), RA - RB) +
                                                     NearR(TEXT("-a"), RA * -1.0) +
                                                           TEXT("var v=a\nv+=v\nv-=a\nv*=2.0lf\n")+
                                                                NearR(TEXT("v"), RA * 2) +
                                                                      NearR(TEXT("2.0lf*a"), RA * 2));
    Run(TEXT("rotator_wrap_and_noncommuting_composition"),
        R + NearR(
                TEXT("ue_compose_rotation(a,b)"), UKismetMathLibrary::ComposeRotators(RA, RB)) +
                TEXT("verify(ue_rotator(0.0lf,359.0lf,0.0lf)!=ue_rotator(0.0lf,-1.0lf,0.0lf))\nverify(ue_nearly_equal(ue_rotator(0.0lf,359.0lf,0.0lf),ue_rotator(0.0lf,-1.0lf,0.0lf),0.000001lf))\n")+
                    NearR(TEXT("ue_normalized(ue_rotator(390.0lf,-450.0lf,720.0lf))"),
                               FRotator(390, -450, 720).GetNormalized()));
    Run(TEXT("rotate_unrotate_vectors"),
             TEXT("let r=")+ Rot(RA) +
                  TEXT("\nlet v=")+ Vec(A) +
                       TEXT("\n")+ Near(TEXT("ue_rotate_vector(r,v)"), RA.RotateVector(A)) +
                                        Near(TEXT("ue_unrotate_vector(r,v)"), RA.UnrotateVector(A)));
    const FTransform TA(RA, FVector(10, -20, 30), FVector(2, 2, 2)), TB(RB, FVector(-7, 5, 2), FVector(3, 3, 3));
    const FString T =
        TEXT("let a=")+ Transform(TA) + TEXT("\nlet b=")+ Transform(TB) + TEXT("\nlet p=")+ Vec(A) + TEXT("\n");
    Run(TEXT("transform_order_and_compound_alias"),
        T + Near(
                TEXT("ue_transform_position(a*b,p)"), (TA * TB).TransformPosition(A)) +
                Near(
                    TEXT("ue_transform_position(a*b,p)"), TB.TransformPosition(TA.TransformPosition(A))) +
                    TEXT("verify(!ue_nearly_equal(a*b,b*a,0.000001lf))\nvar v=a\nv*=v\nverify(ue_nearly_equal(v,a*a,0.000001lf))"));
    const FTransform NonUniform(RA, FVector(10, -20, 30), FVector(-2, 3, 4));
    const FString N = TEXT("let t=")+ Transform(NonUniform) + TEXT("\nlet p=")+ Vec(A) + TEXT("\n");
    Run(TEXT("position_vector_direction_distinction"),
             N + Near(TEXT("ue_transform_position(t,p)"), NonUniform.TransformPosition(A)) +
                      Near(TEXT("ue_transform_vector(t,p)"), NonUniform.TransformVector(A)) +
                           Near(TEXT("ue_transform_direction(t,p)"), NonUniform.TransformVectorNoScale(A)));
    Run(TEXT("native_inverse_and_relative_transform"),
             N + Near(TEXT("ue_inverse_transform_position(t,p)"), NonUniform.InverseTransformPosition(A)) +
                      Near(TEXT("ue_inverse_transform_vector(t,p)"), NonUniform.InverseTransformVector(A)) +
                           Near(TEXT("ue_transform_position(ue_transform_inverse(t),p)"),
                                     NonUniform.Inverse().TransformPosition(A)) +
                                Near(TEXT("ue_transform_position(ue_transform_relative(t,")+ Transform(TB) +
                                          TEXT("),p)"), NonUniform.GetRelativeTransform(TB).TransformPosition(A)));
    Run(TEXT("native_transform_equals"),
        TEXT("let a=ue_transform_identity()\nlet b=ue_transform(ue_vector(0.0000001lf,0.0lf,0.0lf),ue_rotator(0.0lf,0.0lf,0.0lf),ue_vector(1.0lf,1.0lf,1.0lf))\nverify(ue_nearly_equal(a,b,0.000001lf))\nverify(!ue_nearly_equal(a,b,0.0lf))"));
    FString NameChecks;
    for (const auto& Pair : TArray<TPair<FString, FString>>{
             {TEXT("Pickup_7"), TEXT("pickup_7")},
              {TEXT("Pickup_7"), TEXT("Pickup_8")},
               {TEXT("None"), TEXT("")}, {TEXT("Pickup_07"), TEXT("Pickup_7")}, {TEXT("straße"), TEXT("STRASSE")}})
    {
        const bool Same = FName(*Pair.Key) == FName(*Pair.Value);
        NameChecks += FString::Printf(TEXT("{ let a=ue_name_value(\"%s\"); let b=ue_name_value(\"%s\"); verify((a==b)==%s); verify((a!=b)==%s); verify((a==b)==ue_names_equal(a,b)); }\n"),
            *Pair.Key, *Pair.Value, Same ? TEXT("true"): TEXT("false"), Same ? TEXT("false"): TEXT("true"));
    }
    Run(TEXT("name_identity_native_fname"), NameChecks);
    Run(TEXT("double_precision_and_zero_normalization"),
        Near(
            TEXT("ue_vector(123456789.125lf,0.0lf,0.0lf)*2.0lf"), FVector(123456789.125 * 2, 0, 0)) +
            TEXT("verify(ue_normalized(ue_vector(0.0lf,0.0lf,0.0lf),0.00000001lf)==ue_vector(0.0lf,0.0lf,0.0lf))\nverify(ue_normalized(ue_vector2d(0.0lf,0.0lf),0.00000001lf)==ue_vector2d(0.0lf,0.0lf))"));
    Run(TEXT("failed_compound_write_is_atomic"),
        TEXT("var v=ue_vector(1.0lf,2.0lf,3.0lf)\ntry { v/=0.0lf } recover { verify(v==ue_vector(1.0lf,2.0lf,3.0lf)); return 42 }\nreturn 0"));
    {
        const FString Code = TEXT("options gen2\nrequire ue_dsl\n[ue_class(Blueprintable)]\nclass BuiltinMover : Actor {\n properties=ue_properties(BlueprintReadWrite) {var Position:UEVector=ue_vector(0.0lf,0.0lf,0.0lf)}\n [ue_function(BlueprintCallable)]\n def Move(delta:UEVector):void { self.Position+=delta*2.0lf }\n}\n[export]\ndef main(input:int):int {return 42}\n");
        FFileHelper::SaveStringToFile(Code, *Path, FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM);
        const auto Result = Runtime.RunFile(Path, 0);
        Check(Result.bSucceeded, TEXT("authored_property_compound_lowering"), Result.Error);
    }
    Run(TEXT("name_string_conversion_is_explicit"), TEXT("let same=ue_name_value(\"A\")==\"A\""), TEXT("compile"),
                                                                                                       true);
    Run(TEXT("vector_dimensions_not_interchangeable"),
             TEXT("let bad=ue_vector(1.0lf,2.0lf,3.0lf)+ue_vector2d(1.0lf,2.0lf)"), TEXT("compile"), true);
    Run(TEXT("vector_product_requires_named_operation"), V + TEXT("let bad=a*b"), TEXT("compile"), true);
    Run(TEXT("transform_vector_product_is_explicit"), T + TEXT("let bad=a*p"), TEXT("compile"), true);
    Run(TEXT("rotation_composition_requires_named_operation"), R + TEXT("let bad=a*b"), TEXT("compile"), true);
    Run(TEXT("vector_scalar_addition_is_not_defined"), V + TEXT("let bad=a+2.0lf"), TEXT("compile"), true);
    Run(TEXT("float_scalar_requires_explicit_double"), V + TEXT("var scale:float=float(input); let bad=a*scale"),
                                                                TEXT("compile"), true);
    Run(TEXT("integer_scalar_requires_explicit_double"), V + TEXT("let bad=a*input"), TEXT("compile"), true);
    Run(TEXT("transform_exact_equality_not_defined"), T + TEXT("let bad=a==b"), TEXT("compile"), true);
    Run(TEXT("zero_divisor_refused"), V + TEXT("let bad=a/0.0lf\nreturn int(bad.x)"), TEXT("division by zero"));
    Run(TEXT("normalization_tolerance_must_be_positive"), V + TEXT("let bad=ue_normalized(a,0.0lf)\nreturn int(bad.x)"),
                                                                   TEXT("must be positive"));
    Run(TEXT("negative_comparison_tolerance_refused"), V + TEXT("verify(ue_nearly_equal(a,b,-1.0lf))"),
                                                                TEXT("non-negative"));
    Run(TEXT("singular_transform_inverse_refused"),
        TEXT("let t=ue_transform(ue_vector(0.0lf,0.0lf,0.0lf),ue_rotator(0.0lf,0.0lf,0.0lf),ue_vector(1.0lf,0.0lf,1.0lf))\nlet bad=ue_transform_inverse(t)\nreturn int(ue_transform_translation(bad).x)"),
            TEXT("scale magnitude"));
    Run(TEXT("near_singular_relative_parent_refused"),
        TEXT("let t=ue_transform(ue_vector(0.0lf,0.0lf,0.0lf),ue_rotator(0.0lf,0.0lf,0.0lf),ue_vector(1.0lf,0.0000000001lf,1.0lf))\nlet bad=ue_transform_relative(ue_transform_identity(),t)\nreturn int(ue_transform_translation(bad).x)"),
            TEXT("scale magnitude"));
    Run(TEXT("nonfinite_mutated_vector_refused"),
             TEXT("var a=ue_vector(1.0lf,2.0lf,3.0lf)\na.x=1.0lf/double(input)\nlet bad=-a\nreturn int(bad.x)"),
                  TEXT("finite"));
    Run(TEXT("overflowing_result_refused"),
             TEXT("let a=ue_vector(1.0e308lf,0.0lf,0.0lf)\nlet bad=a+a\nreturn int(bad.x)"), TEXT("finite"));
    const FString Repo = FPaths::ConvertRelativePathToFull(FPaths::ProjectDir() / TEXT("../.."));
    FString Old;
    FFileHelper::LoadFileToString(Old, *(Repo / TEXT("Bindings/FoundationExterns.txt")));
    Old.ReplaceInline(TEXT("\r\n"), TEXT("\n"));
    const FString Actual = DasFoundationExternManifest();
    bool Additive = !Old.IsEmpty();
    TArray<FString> Lines;
    Old.ParseIntoArrayLines(Lines);
    for (const auto& Line : Lines)
        if (!Actual.Contains(Line + TEXT("\n")))
            Additive = false;
    Check(Additive, TEXT("existing_extern_contract_is_unchanged"));
    FFileHelper::SaveStringToFile(Actual, *(Repo / TEXT("logs/builtin-values-externs.txt")),
                                            FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM);
    UE_LOG(LogTemp, Display,
           TEXT("DAS_BUILTIN_VALUES_RESULT=%s checks=%d"), Failures ? TEXT("FAIL"): TEXT("PASS"), Checks);
    return Failures ? 1 : 0;
}
