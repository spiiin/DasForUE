
// Included after the shared native parity formatting helpers.
int32 RunCompleteBuiltinTypes(FDasRuntimeModule& Runtime, const FString& Path)
{
    int32 Checks = 0, Failures = 0;
    auto Check = [&](bool Good, const TCHAR* Label, const FString& Error = FString())
    {
        ++Checks;
        if (!Good)
            ++Failures;
        UE_LOG(LogTemp, Display,
               TEXT("DAS_COMPLETE_BUILTINS_%s %s %s"), Good ? TEXT("PASS"): TEXT("FAIL"), Label, *Error);
    };
    auto Run = [&](const FString& Body, const TCHAR* Label, bool Refuse = false)
    {
        const FString Source = TEXT("options gen2\nrequire ue_api_math\nrequire ue_api_collision\nrequire ue_api_world\nrequire ue_api_umg_input\nrequire ue_native_gameplay\n[export]\ndef main(input:int):int {\n")+
            Body + TEXT("\nreturn 42\n}\n");
        if (!FFileHelper::SaveStringToFile(Source, *Path, FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM))
        {
            Check(false, Label, TEXT("Could not write probe"));
            return;
        }
        const auto R = Runtime.RunFile(Path, 0);
        Check(Refuse ? !R.bSucceeded && R.Error.Contains(TEXT("error[")) : R.bSucceeded && R.Value == 42, Label,
                                                         R.Error);
    };
    auto Quat = [](const FQuat& Q)
    {
        return FString::Printf(TEXT("ue_quat(%s,%s,%s,%s)"), *Number(Q.X), *Number(Q.Y), *Number(Q.Z), *Number(Q.W));
    };
    auto NearQ = [&](const FString& Expr, const FQuat& Q)
    {
        return FString::Printf(TEXT("verify(ue_nearly_equal(%s,%s,0.000000001lf))\n"), *Expr, *Quat(Q));
    };
    auto Linear = [](const FLinearColor& C)
    {
        return FString::Printf(TEXT("ue_linear_color(float(%s),float(%s),float(%s),float(%s))"), *Number(C.R),
                                    *Number(C.G), *Number(C.B), *Number(C.A));
    };
    auto NearC = [&](const FString& Expr, const FLinearColor& C)
    {
        return FString::Printf(TEXT("verify(ue_nearly_equal(%s,%s,0.000001lf))\n"), *Expr, *Linear(C));
    };
    auto Color = [](const FColor& C)
    {
        return FString::Printf(TEXT("ue_color(%d,%d,%d,%d)"), C.R, C.G, C.B, C.A);
    };
    const FVector A(2, -3, 4), B(5, 1, -2);
    const FQuat QA = FRotator(15, 80, -25).Quaternion(), QB = FRotator(-20, 35, 40).Quaternion();
    const FLinearColor CA(0.25f, 0.5f, 0.75f, 0.4f), CB(0.8f, 0.2f, 0.1f, 0.9f);
    const FTransform TA(QA, A, FVector(2, 3, 4)), TB(QB, B, FVector(-1, 2, 5));
    FTransform Blend;
    Blend.Blend(TA, TB, 0.3);
    FString Body = TEXT("let a=")+ Vec(A) +
                        TEXT("\nlet b=")+ Vec(B) +
                             TEXT("\nlet qa=")+ Quat(QA) +
                                  TEXT("\nlet qb=")+ Quat(QB) +
                                       TEXT("\nlet ca=")+ Linear(CA) + TEXT("\nlet cb=")+ Linear(CB) + TEXT("\n");
    Body += Near(TEXT("ue_lerp(a,b,0.3lf)"), FMath::Lerp(A, B, 0.3));
    Body += Near(TEXT("ue_clamp_length(a,2.0lf)"), A.GetClampedToMaxSize(2));
    Body += Near(TEXT("ue_project_onto(a,b)"), A.ProjectOnTo(B));
    Body += Near2(TEXT("ue_project_onto(ue_vector2d(2.0lf,-3.0lf),ue_vector2d(5.0lf,1.0lf))"),
                       FVector2D(5, 1) * (7.0 / 26.0));
    Body += Near2(TEXT("ue_lerp(ue_vector2d(2.0lf,-3.0lf),ue_vector2d(5.0lf,1.0lf),0.3lf)"),
                       FMath::Lerp(FVector2D(2, -3), FVector2D(5, 1), 0.3));
    Body += Near2(TEXT("ue_clamp_length(ue_vector2d(2.0lf,-3.0lf),2.0lf)"), FVector2D(2, -3).GetClampedToMaxSize(2));
    Body += Near(TEXT("ue_project_onto_plane(a,ue_vector(0.0lf,0.0lf,1.0lf))"),
                      FVector::VectorPlaneProject(A, FVector::UpVector));
    Body += Near(TEXT("ue_reflect(a,ue_vector(0.0lf,0.0lf,1.0lf))"), A.MirrorByVector(FVector::UpVector));
    Body += NearQ(TEXT("qa*qb"), QA * QB) + NearQ(TEXT("ue_compose_rotation(qa,qb)"), QB * QA);
    Body += TEXT("var q=qa\nq*=q\n")+ NearQ(TEXT("q"), QA * QA);
    Body +=
        NearQ(TEXT("ue_inverse(qa)"), QA.Inverse()) + NearQ(TEXT("ue_slerp(qa,qb,0.3lf)"), FQuat::Slerp(QA, QB, 0.3));
    Body += NearQ(TEXT("ue_normalized(ue_quat(0.0lf,0.0lf,0.0lf,2.0lf),0.00000001lf)"), FQuat::Identity);
    Body += NearR(TEXT("ue_slerp(ue_rotator(15.0lf,80.0lf,-25.0lf),ue_rotator(-20.0lf,35.0lf,40.0lf),0.3lf)"),
                       FQuat::Slerp(QA, QB, 0.3).Rotator());
    Body += Near(TEXT("ue_rotate_vector(qa,a)"), QA.RotateVector(A)) +
                 Near(TEXT("ue_unrotate_vector(qa,a)"), QA.UnrotateVector(A));
    Body += NearQ(TEXT("ue_transform_quat(ue_transform(a,qa,ue_vector(2.0lf,3.0lf,4.0lf)))"), QA);
    Body += Near(TEXT("ue_transform_position(ue_blend(ue_transform(a,qa,ue_vector(2.0lf,3.0lf,4.0lf)),ue_transform(b,qb,ue_vector(-1.0lf,2.0lf,5.0lf)),0.3lf),a)"),
        Blend.TransformPosition(A));
    Body += NearC(TEXT("ca+cb"), CA + CB) + NearC(TEXT("ca-cb"), CA - CB) + NearC(TEXT("ca*2.0lf"), CA * 2) +
                                                                                  NearC(TEXT("ca/2.0lf"), CA / 2);
    Body += TEXT("var c=ca\nc+=cb\nc-=cb\nc*=2.0lf\nc/=2.0lf\n")+ NearC(TEXT("c"), CA);
    Body += NearC(TEXT("ue_lerp(ca,cb,0.3lf)"), FMath::Lerp(CA, CB, 0.3f));
    Body += TEXT("verify(ue_linear_to_srgb(ca)==")+ Color(CA.ToFColorSRGB()) +
                 TEXT(")\nverify(ue_quantize_linear(ca)==")+ Color(CA.ToFColor(false)) + TEXT(")\n");
    Body += NearC(TEXT("ue_srgb_to_linear(ue_color(64,128,192,102))"),
                       FLinearColor::FromSRGBColor(FColor(64, 128, 192, 102)));
    Body +=
        NearC(TEXT("ue_reinterpret_linear(ue_color(64,128,192,102))"), FColor(64, 128, 192, 102).ReinterpretAsLinear());
    Body += NearC(TEXT("ue_math_MakeColor(0.25f,0.5f,0.75f,0.4f)"), CA);
    Body += NearQ(TEXT("ue_math_Conv_RotatorToQuaternion(ue_rotator(15.0lf,80.0lf,-25.0lf))"), QA);
    Body += NearC(TEXT("ue_math_Conv_ColorToLinearColor(ue_color(64,128,192,102))"),
                       FLinearColor(FColor(64, 128, 192, 102)));
    // A native FQuat can contain finite non-unit values; only rotation operations require unit length.
    Body += TEXT("let sum:UEQuat=ue_math_Add_QuatQuat(qa,qa)\nverify(sum.w==qa.w*2.0lf)\nverify(ue_color(1,2,3,4)!=ue_color(4,3,2,1))\n");
    Run(Body, TEXT("native_math_and_generated_api_parity"));
    Run(TEXT("var refused=false\ntry {let q=ue_inverse(ue_quat(0.0lf,0.0lf,0.0lf,0.0lf));return int(q.w)} recover {refused=true}\nverify(refused)\nrefused=false\ntry {let c=ue_color(256,0,0,255);return int(c.r)} recover {refused=true}\nverify(refused)\nrefused=false\ntry {let v=ue_project_onto(ue_vector(1.0lf,2.0lf,3.0lf),ue_vector(0.0lf,0.0lf,0.0lf));return int(v.x)} recover {refused=true}\nverify(refused)\nvar c=ue_linear_color(0.25f,0.5f,0.75f,1.0f)\nrefused=false\ntry {c/=0.0lf} recover {refused=true}\nverify(refused && c==ue_linear_color(0.25f,0.5f,0.75f,1.0f))\n"),
        TEXT("invalid_inputs_and_atomic_compound_write"));
    Run(TEXT("let q:UEQuat=ue_rotator(1.0lf,2.0lf,3.0lf)"), TEXT("implicit_quaternion_conversion_refused"), true);
    FString Error;
    const FString Script = TEXT("builtin_types_example.das");
    const FString Base = TEXT("/Game/Tests/DasBuiltinValues/")+ FGuid::NewGuid().ToString(EGuidFormats::Digits);
    TArray<UObject*> Types;
    const bool Applied = FDasTypeService::Apply(Script, Base + TEXT("/Types"), Types, Error);
    bool Defaults = false;
    for (UObject* Object : Types)
        if (auto* Type = Cast<UUserDefinedStruct>(Object))
        {
            const uint8* Data = Type->GetDefaultInstance();
            const auto* Q = FindFProperty<FStructProperty>(Type, TEXT("Rotation"));
            // User-defined struct field names carry GUID suffixes; use reflected struct identity.
            int32 Matches = 0;
            for (TFieldIterator<FStructProperty> It(Type); It; ++It)
            {
                if (It->Struct == TBaseStructure<FQuat>::Get())
                    Matches += It->ContainerPtrToValuePtr<FQuat>(Data)->Equals(FQuat::Identity) ? 1 : 0;
                if (It->Struct == TBaseStructure<FLinearColor>::Get())
                    Matches +=
                        *It->ContainerPtrToValuePtr<FLinearColor>(Data) == FLinearColor(0.25f, 0.5f, 0.75f, 1) ? 1 : 0;
                if (It->Struct == TBaseStructure<FColor>::Get())
                    Matches += *It->ContainerPtrToValuePtr<FColor>(Data) == FColor(64, 128, 192, 255) ? 1 : 0;
            }
            Defaults = Matches == 3;
        }
    Check(Applied && Defaults, TEXT("authored_struct_native_defaults"), Error);
    FDasAuthoringModule Model;
    UBlueprint* BP = nullptr;
    bool ClassGood = Runtime.ReadAuthoringScript(Runtime.ScriptPath(Script), Model, Error) &&
                     Model.Classes.Num() == 1 &&
                     FDasDSLService::Apply(Script, Base + TEXT("/BP_Values"), Model.Classes[0].Id, BP, Error);
    if (ClassGood)
    {
        TStrongObjectPtr<UObject> Object(NewObject<UObject>(GetTransientPackage(), BP->GeneratedClass));
        UFunction* Function = Object->FindFunction(TEXT("Exercise"));
        ClassGood = Function != nullptr;
        if (ClassGood)
        {
            FStructOnScope Frame(Function);
            Object->ProcessEvent(Function, Frame.GetStructMemory());
            const auto* Return = FindFProperty<FBoolProperty>(Function, TEXT("ReturnValue"));
            ClassGood = Return && Return->GetPropertyValue_InContainer(Frame.GetStructMemory());
            auto* Rotations = FindFProperty<FArrayProperty>(BP->GeneratedClass, TEXT("Rotations"));
            auto* Palette = FindFProperty<FMapProperty>(BP->GeneratedClass, TEXT("Palette"));
            auto* Rotation = FindFProperty<FStructProperty>(BP->GeneratedClass, TEXT("Rotation"));
            auto* Tint = FindFProperty<FStructProperty>(BP->GeneratedClass, TEXT("Tint"));
            auto* Packed = FindFProperty<FStructProperty>(BP->GeneratedClass, TEXT("Packed"));
            ClassGood &= Rotations && Palette && Rotation && Tint && Packed;
            if (ClassGood)
            {
                FScriptArrayHelper Array(Rotations, Rotations->ContainerPtrToValuePtr<void>(Object.Get()));
                FScriptMapHelper Map(Palette, Palette->ContainerPtrToValuePtr<void>(Object.Get()));
                const FQuat& ActualRotation = *Rotation->ContainerPtrToValuePtr<FQuat>(Object.Get());
                const FLinearColor ExpectedTint(0.25f, 0.5f, 0.75f, 1.0f);
                ClassGood = Array.Num() == 1 && Map.Num() == 1 && ActualRotation.IsNormalized() &&
                            reinterpret_cast<const FQuat*>(Array.GetRawPtr(0))->Equals(ActualRotation) &&
                            *Tint->ContainerPtrToValuePtr<FLinearColor>(Object.Get()) == ExpectedTint &&
                            *Packed->ContainerPtrToValuePtr<FColor>(Object.Get()) == FColor(42, 128, 192, 255);
                for (int32 I = 0; I < Map.GetMaxIndex(); ++I)
                    if (Map.IsValidIndex(I))
                        ClassGood &=
                            *reinterpret_cast<const FString*>(Map.GetKeyPtr(I)) ==
                            TEXT("Main") && *reinterpret_cast<const FLinearColor*>(Map.GetValuePtr(I)) == ExpectedTint;
            }
        }
    }
    Check(ClassGood, TEXT("authored_fields_containers_and_reflected_methods"), Error);
    for (UObject* Type : Types)
    {
        FAssetRegistryModule::AssetDeleted(Type);
        Type->GetOutermost()->SetDirtyFlag(false);
        Type->ClearFlags(RF_Public | RF_Standalone);
    }
    if (BP)
    {
        FAssetRegistryModule::AssetDeleted(BP);
        BP->GetOutermost()->SetDirtyFlag(false);
        BP->ClearFlags(RF_Public | RF_Standalone);
    }
    const FString Repo = FPaths::ConvertRelativePathToFull(FPaths::ProjectDir() / TEXT("../.."));
    FFileHelper::SaveStringToFile(DasFoundationExternManifest(), *(Repo / TEXT("logs/complete-builtins-externs.txt")),
                                                                   FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM);
    UE_LOG(LogTemp, Display,
           TEXT("DAS_COMPLETE_BUILTINS_RESULT=%s checks=%d"), Failures ? TEXT("FAIL"): TEXT("PASS"), Checks);
    return Failures ? 1 : 0;
}
