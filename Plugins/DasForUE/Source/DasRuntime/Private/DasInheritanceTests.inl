{
    FDasAuthoringModule InheritanceModel;
    FString InheritanceError;
    const bool InheritanceGood = Runtime.ReadAuthoringScript(Runtime.ScriptPath(TEXT("dsl_inheritance_child.das")),
                                                                                InheritanceModel, InheritanceError);
    FDasDeclaredClass InheritanceEffective;
    const auto* InheritanceChild = InheritanceModel.Classes.FindByPredicate(
        [](const auto& C) { return C.Id == TEXT("inheritance.ChildCounter"); });
    Expect(InheritanceGood && InheritanceChild &&
           InheritanceChild->ParentId ==
               TEXT("inheritance.BaseCounter") && InheritanceChild->Properties.Num() == 1 &&
                    InheritanceModel.ResolveClass(InheritanceChild->Id, InheritanceEffective, InheritanceError) &&
                    InheritanceEffective.Properties.Num() == 4,
                    TEXT("authored_inheritance_own_and_effective_members"), InheritanceError);
    FDasAuthoringModule InheritanceRejected;
    const bool InheritanceBad = Runtime.ReadAuthoringScript(Runtime.ScriptPath(TEXT("dsl_inheritance_bad_final.das")),
                                                                               InheritanceRejected, InheritanceError);
    Expect(!InheritanceBad && InheritanceError.Contains(TEXT("Override requires")),
                                                        TEXT("authored_inheritance_final_override_rejected"),
                                                             InheritanceError);
}

{
    FDasAuthoringModule LifeModel;
    FString LifeError;
    FDasDeclaredClass LifeEffective;
    const bool LifeRead =
        Runtime.ReadAuthoringScript(Runtime.ScriptPath(TEXT("dsl_life_actor_child.das")), LifeModel, LifeError);
    const auto* LifeChild =
        LifeModel.Classes.FindByPredicate([](const auto& C) { return C.Id == TEXT("lifefamily.ChildLifeActor"); });
    Expect(LifeRead && LifeChild && LifeChild->Components.Num() == 3 && LifeChild->Callbacks.Num() == 3 &&
           LifeChild->SourceScript == TEXT("dsl_life_actor_child.das") &&
                                           LifeModel.ResolveClass(LifeChild->Id, LifeEffective, LifeError) &&
                                           LifeEffective.Components.Num() == 5 && LifeEffective.Callbacks.Num() == 6,
                                           TEXT("inherited_components_owned_callbacks_and_source"), LifeError);
    if (LifeRead)
    {
        auto Invalid = LifeModel;
        auto* Child =
            Invalid.Classes.FindByPredicate([](const auto& C) { return C.Id == TEXT("lifefamily.ChildLifeActor"); });
        Child->Components[0].bRoot = true;
        Expect(!Invalid.Validate(LifeError), TEXT("derived_second_root_rejected"), LifeError);
        Invalid = LifeModel;
        Child =
            Invalid.Classes.FindByPredicate([](const auto& C) { return C.Id == TEXT("lifefamily.ChildLifeActor"); });
        Child->Components[0].Attach = TEXT("MissingParent");
        Expect(!Invalid.Validate(LifeError), TEXT("derived_unknown_attachment_rejected"), LifeError);
    }
    if (LifeRead)
    {
        const auto* Base =
            LifeModel.Classes.FindByPredicate([](const auto& C) { return C.Id == TEXT("lifefamily.BaseLifeActor"); });
        const auto* BaseBehavior =
            Base ? Base->Components.FindByPredicate([](const auto& C) { return C.Name == TEXT("Behavior"); }) : nullptr;
        const auto* EffectiveBehavior =
            LifeEffective.Components.FindByPredicate([](const auto& C) { return C.Name == TEXT("Behavior"); });
        Expect(LifeChild->ComponentOverrides.Num() == 2 && BaseBehavior && EffectiveBehavior &&
               BaseBehavior->Defaults[TEXT("Bonus")].Number == 42 &&
                                      EffectiveBehavior->Defaults[TEXT("Bonus")].Number == 55,
                                                                  TEXT("inherited_override_IR_keeps_parent_defaults"),
                                                                       LifeError);
        auto Invalid = LifeModel;
        auto* Child =
            Invalid.Classes.FindByPredicate([](const auto& C) { return C.Id == TEXT("lifefamily.ChildLifeActor"); });
        Child->ComponentOverrides[0].Id = TEXT("missing.component");
        Expect(!Invalid.Validate(LifeError), TEXT("unknown_inherited_override_identity_rejected"), LifeError);
    }
    if (LifeRead)
    {
        FDasAuthoringModule LifeRoundTrip;
        FString LifeJson;
        Expect(LifeModel.ToJson(LifeJson, LifeError) &&
                   FDasAuthoringModule::FromJson(LifeJson, LifeRoundTrip, LifeError),
               TEXT("inherited_lifecycle_source_IR_roundtrip"), LifeError);
    }
}

{
    FDasAuthoringModule HelperModel;
    FString HelperError;
    const bool HelperRead = Runtime.ReadAuthoringScript(Runtime.ScriptPath(TEXT("dsl_inheritance_grandchild.das")),
                                                                           HelperModel, HelperError);
    if (HelperRead)
    {
        FDasAuthoringModule RoundTrip;
        FString Json;
        FDasDeclaredClass Effective;
        Expect(HelperModel.ToJson(Json, HelperError) && FDasAuthoringModule::FromJson(Json, RoundTrip, HelperError) &&
               RoundTrip.ResolveClass(TEXT("inheritance.GrandCounter"), Effective, HelperError) &&
                                      Effective.Callbacks.ContainsByPredicate(
                                          [](const auto& Hook) { return Hook.Kind == TEXT("ConstructionScript"); }),
                                      TEXT("inherited_construction_IR_roundtrip"), HelperError);
        auto Invalid = HelperModel;
        auto* Base =
            Invalid.Classes.FindByPredicate([](const auto& C) { return C.Id == TEXT("inheritance.BaseCounter"); });
        Base->NativeParent = TEXT("/Script/Engine.ActorComponent");
        Expect(Invalid.Validate(HelperError), TEXT("component_construction_IR_supported"), HelperError);
    }
    Expect(HelperRead && !HelperModel.Classes.ContainsByPredicate(
                             [](const auto& C)
                             {
                                 return C.Methods.ContainsByPredicate(
                                     [](const auto& M)
                                     { return M.Name == TEXT("Offset") || M.Name == TEXT("Template"); });
                             }),
           TEXT("helpers_are_not_reflected_IR_members"), HelperError);
}

{
    FDasAuthoringModule PureModel;
    FString Error;
    const bool Read =
        Runtime.ReadAuthoringScript(Runtime.ScriptPath(TEXT("dsl_inheritance_child.das")), PureModel, Error);
    FDasClassSchema Schema;
    const auto* Base =
        PureModel.Classes.FindByPredicate([](const auto& C) { return C.Id == TEXT("inheritance.BaseCounter"); });
    Expect(Read && Base &&
           Base->Methods.ContainsByPredicate([](const auto& M)
                                             { return M.Name == TEXT("ReadScore") && M.bPure && M.bCallable; }) &&
           PureModel.ToActorSchema(
               TEXT("inheritance.BaseCounter"), Schema, Error) &&
               Schema.Methods.ContainsByPredicate([](const auto& M) { return M.Name == TEXT("ReadScore") && M.bPure; }),
               TEXT("pure_DSL_IR_adapter_flags"), Error);
    if (Read)
    {
        FString Json;
        FDasAuthoringModule RoundTrip;
        Expect(PureModel.ToJson(Json, Error) && FDasAuthoringModule::FromJson(Json, RoundTrip, Error),
               TEXT("pure_IR_roundtrip"), Error);
        auto Invalid = PureModel;
        auto* C =
            Invalid.Classes.FindByPredicate([](const auto& X) { return X.Id == TEXT("inheritance.BaseCounter"); });
        auto* M = C->Methods.FindByPredicate([](const auto& X) { return X.Name == TEXT("ReadScore"); });
        M->ReturnType.Kind = TEXT("void");
        Expect(!Invalid.Validate(Error) && Error.Contains(TEXT("BlueprintPure requires")),
                                                          TEXT("pure_void_no_outputs_IR_rejected"), Error);
    }
    for (const auto* Script :
         {TEXT("dsl_pure_bad_void.das"), TEXT("dsl_pure_bad_callable.das"), TEXT("dsl_pure_bad_override.das")})
    {
        FDasAuthoringModule Invalid;
        Expect(!Runtime.ReadAuthoringScript(Runtime.ScriptPath(Script), Invalid, Error),
               *(FString(TEXT("pure_negative_")) + Script), Error);
    }
}

{
    FDasAuthoringModule HostModel;
    FString HostError;
    auto CheckHost = [&](const TCHAR* Script, const TCHAR* Path)
    {
        const bool Read = Runtime.ReadAuthoringScript(Runtime.ScriptPath(Script), HostModel, HostError);
        Expect(Read && HostModel.Classes.Num() == 1 && HostModel.Classes[0].NativeParent == Path &&
                   HostModel.Classes[0].ParentId.IsEmpty(),
               Script, HostError);
    };
    CheckHost(TEXT("dsl_native_base_actor.das"), TEXT("/Script/Engine.Actor"));
    CheckHost(TEXT("dsl_native_base_component.das"), TEXT("/Script/Engine.ActorComponent"));
    for (const auto* Script :
         {TEXT("dsl_native_base_bad_conflict.das"), TEXT("dsl_native_base_bad_name.das"),
                                                         TEXT("dsl_native_base_bad_state.das"),
                                                              TEXT("dsl_native_base_bad_path.das"),
                                                                   TEXT("dsl_native_base_bad_kinds.das")})
        Expect(!Runtime.ReadAuthoringScript(Runtime.ScriptPath(Script), HostModel, HostError), Script, HostError);
}

{
    FDasAuthoringModule ConstModel;
    FDasClassSchema ConstSchema;
    FString ConstError;
    const bool ConstRead =
        Runtime.ReadAuthoringScript(Runtime.ScriptPath(TEXT("dsl_const_methods.das")), ConstModel, ConstError);
    Expect(ConstRead && ConstModel.Classes[0].Methods.Num() == 3 &&
               ConstModel.Classes[0].Methods.ContainsByPredicate(
                   [](const auto& M) { return M.Name == TEXT("ReadWithExec") && M.bConst && !M.bPure; }) &&
               ConstModel.ToActorSchema(ConstModel.Classes[0].Id, ConstSchema, ConstError) &&
               ConstSchema.Methods.ContainsByPredicate([](const auto& M)
                                                       { return M.Name == TEXT("Snapshot") && M.bConst && M.bPure; }),
           TEXT("const_DSL_IR_schema_and_explicit_impure"), ConstError);
    FString ConstJson;
    FDasAuthoringModule ConstRoundTrip;
    Expect(ConstRead && ConstModel.ToJson(ConstJson, ConstError) &&
               FDasAuthoringModule::FromJson(ConstJson, ConstRoundTrip, ConstError) &&
               ConstRoundTrip.Classes[0].Methods.ContainsByPredicate([](const auto& M)
                                                                     { return M.Name == TEXT("Query") && M.bConst; }),
           TEXT("const_IR_roundtrip"), ConstError);
    if (ConstRead)
    {
        auto Changed = ConstModel.Classes[0];
        Changed.Methods[0].bConst = !Changed.Methods[0].bConst;
        Expect(FDasAuthoringModule::Compare(ConstModel.Classes[0], Changed).bStructural,
               TEXT("const_change_is_structural"));
    }
    for (const auto* ConstScript :
         {TEXT("dsl_const_bad_write.das"), TEXT("dsl_const_bad_container.das"),
                                                TEXT("dsl_const_bad_call.das"),
                                                     TEXT("dsl_const_bad_helper.das"),
                                                          TEXT("dsl_const_bad_setter.das"),
                                                               TEXT("dsl_const_bad_override.das"),
                                                                    TEXT("dsl_const_bad_helper_override.das")})
        Expect(!Runtime.ReadAuthoringScript(Runtime.ScriptPath(ConstScript), ConstModel, ConstError), ConstScript,
               ConstError);
}
