    {
        TStrongObjectPtr<UDasObjectFixture> Owner(NewObject<UDasObjectFixture>());Owner->health=10;
        const auto Value=Runtime.RunFile(Runtime.ScriptPath(TEXT("delegate_values_probe.das")),0,false,Owner.Get());
        Expect(Value.bSucceeded && Value.Value==28 && Owner->health==28,TEXT("typed_delegate_return_copy_unbind_pair_multicast_remove_clear"),Value.Error);
        const auto Bad=Runtime.RunFile(Runtime.ScriptPath(TEXT("delegate_bad_bind.das")),0,false,Owner.Get());
        Expect(!Bad.bSucceeded && Bad.Error.Contains(TEXT("matching")) && Owner->health==28,TEXT("delegate_wrong_signature_rejected_before_execution"),Bad.Error);
        const auto Empty=Runtime.RunFile(Runtime.ScriptPath(TEXT("delegate_unbound_execute.das")),0,false,Owner.Get());
        Expect(!Empty.bSucceeded && Empty.Error.Contains(TEXT("bound")),TEXT("delegate_unbound_execute_rejected"),Empty.Error);
        FDasAuthoringModule DelegateModel;FString Issue;
        const bool Declared=Runtime.ReadAuthoringScript(Runtime.ScriptPath(TEXT("dsl_delegate_actor.das")),DelegateModel,Issue);
        Expect(Declared && DelegateModel.Classes[0].Properties.ContainsByPredicate([](const auto& P){return P.Name==TEXT("OnExampleEvent") && P.Type.Kind==TEXT("event") && !P.DelegateWire.IsEmpty() && P.Metadata.FindRef(TEXT("ToolTip"))==TEXT("Broadcasts each change to Blueprint and DAS handlers.");}),TEXT("authored_dispatcher_signature_and_tooltip"),Issue);
    }
    {
        TStrongObjectPtr<UDasObjectFixture> DelegateOwner(NewObject<UDasObjectFixture>());DelegateOwner->health=10;
        const auto StoredInitial=Runtime.RunPersistent(Runtime.ScriptPath(TEXT("delegate_stored_probe.das")),0,DelegateOwner.Get());
        const auto StoredLater=Runtime.RunPersistent(Runtime.ScriptPath(TEXT("delegate_stored_probe.das")),3,DelegateOwner.Get());
        Expect(StoredInitial.bSucceeded && StoredLater.bSucceeded && StoredLater.Value==13,TEXT("delegate_stored_global_survives_VM_boundary_collection"),StoredInitial.Error+StoredLater.Error);
        Runtime.ReleaseObject(DelegateOwner.Get());
        const auto Weak=Runtime.RunFile(Runtime.ScriptPath(TEXT("delegate_weak_probe.das")),0,false,DelegateOwner.Get());
        Expect(Weak.bSucceeded && Weak.Value==1,TEXT("delegate_weak_target_does_not_keep_UObject_alive"),Weak.Error);
    }

    {
        TStrongObjectPtr<UDasObjectFixture> Owner(NewObject<UDasObjectFixture>());Owner->health=10;
        const auto Ref=Runtime.RunFile(Runtime.ScriptPath(TEXT("delegate_refout_probe.das")),0,false,Owner.Get());
        Expect(Ref.bSucceeded && Ref.Value==24,TEXT("delegate_native_ref_out_return_and_unbound_optional_preservation"),Ref.Error);
        const auto Wrong=Runtime.RunFile(Runtime.ScriptPath(TEXT("delegate_ref_bad_direction.das")),0,false,Owner.Get());
        Expect(!Wrong.bSucceeded && Wrong.Error.Contains(TEXT("matching")),TEXT("delegate_ref_out_direction_mismatch_rejected"),Wrong.Error);
    }
