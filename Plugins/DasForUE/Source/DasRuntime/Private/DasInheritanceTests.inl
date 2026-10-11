    {
        FDasAuthoringModule InheritanceModel;FString InheritanceError;
        const bool InheritanceGood=Runtime.ReadAuthoringScript(Runtime.ScriptPath(TEXT("dsl_inheritance_child.das")),InheritanceModel,InheritanceError);
        FDasDeclaredClass InheritanceEffective;const auto* InheritanceChild=InheritanceModel.Classes.FindByPredicate([](const auto& C){return C.Id==TEXT("inheritance.ChildCounter");});
        Expect(InheritanceGood && InheritanceChild && InheritanceChild->ParentId==TEXT("inheritance.BaseCounter") && InheritanceChild->Properties.Num()==1 && InheritanceModel.ResolveClass(InheritanceChild->Id,InheritanceEffective,InheritanceError) && InheritanceEffective.Properties.Num()==2,TEXT("authored_inheritance_own_and_effective_members"),InheritanceError);
        FDasAuthoringModule InheritanceRejected;const bool InheritanceBad=Runtime.ReadAuthoringScript(Runtime.ScriptPath(TEXT("dsl_inheritance_bad_final.das")),InheritanceRejected,InheritanceError);
        Expect(!InheritanceBad && InheritanceError.Contains(TEXT("Override requires")),TEXT("authored_inheritance_final_override_rejected"),InheritanceError);
    }

    {
        FDasAuthoringModule LifeModel;FString LifeError;FDasDeclaredClass LifeEffective;
        const bool LifeRead=Runtime.ReadAuthoringScript(Runtime.ScriptPath(TEXT("dsl_life_actor_child.das")),LifeModel,LifeError);
        const auto* LifeChild=LifeModel.Classes.FindByPredicate([](const auto& C){return C.Id==TEXT("lifefamily.ChildLifeActor");});
        Expect(LifeRead && LifeChild && LifeChild->Components.Num()==3 && LifeChild->Callbacks.Num()==2 && LifeChild->SourceScript==TEXT("dsl_life_actor_child.das") && LifeModel.ResolveClass(LifeChild->Id,LifeEffective,LifeError) && LifeEffective.Components.Num()==5 && LifeEffective.Callbacks.Num()==3,TEXT("inherited_components_owned_callbacks_and_source"),LifeError);
        if(LifeRead){
            auto Invalid=LifeModel;auto* Child=Invalid.Classes.FindByPredicate([](const auto& C){return C.Id==TEXT("lifefamily.ChildLifeActor");});
            Child->Components[0].bRoot=true;Expect(!Invalid.Validate(LifeError),TEXT("derived_second_root_rejected"),LifeError);
            Invalid=LifeModel;Child=Invalid.Classes.FindByPredicate([](const auto& C){return C.Id==TEXT("lifefamily.ChildLifeActor");});Child->Components[0].Attach=TEXT("MissingParent");Expect(!Invalid.Validate(LifeError),TEXT("derived_unknown_attachment_rejected"),LifeError);
        }
        if(LifeRead){
            const auto* Base=LifeModel.Classes.FindByPredicate([](const auto& C){return C.Id==TEXT("lifefamily.BaseLifeActor");});
            const auto* BaseBehavior=Base?Base->Components.FindByPredicate([](const auto& C){return C.Name==TEXT("Behavior");}):nullptr;
            const auto* EffectiveBehavior=LifeEffective.Components.FindByPredicate([](const auto& C){return C.Name==TEXT("Behavior");});
            Expect(LifeChild->ComponentOverrides.Num()==2 && BaseBehavior && EffectiveBehavior && BaseBehavior->Defaults[TEXT("Bonus")].Number==42 && EffectiveBehavior->Defaults[TEXT("Bonus")].Number==55,TEXT("inherited_override_IR_keeps_parent_defaults"),LifeError);
            auto Invalid=LifeModel;auto* Child=Invalid.Classes.FindByPredicate([](const auto& C){return C.Id==TEXT("lifefamily.ChildLifeActor");});Child->ComponentOverrides[0].Id=TEXT("missing.component");Expect(!Invalid.Validate(LifeError),TEXT("unknown_inherited_override_identity_rejected"),LifeError);
        }
        if(LifeRead){FDasAuthoringModule LifeRoundTrip;FString LifeJson;Expect(LifeModel.ToJson(LifeJson,LifeError) && FDasAuthoringModule::FromJson(LifeJson,LifeRoundTrip,LifeError),TEXT("inherited_lifecycle_source_IR_roundtrip"),LifeError);}
    }
