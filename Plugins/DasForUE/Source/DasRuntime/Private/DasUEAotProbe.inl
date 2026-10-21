// Included in DasRuntime.cpp; does not expose VM internals to user scripts.
bool GenerateDasUEAotProbe(const FString& Output, FString& Error)
{
    if (!IsInGameThread())
    {
        Error = TEXT("AOT generation requires game thread");
        return false;
    }
    try
    {
        das::TextWriter Log;
        das::ModuleGroup Modules;
        auto Access = das::make_smart<FUnrealFileAccess>();
        Access->addFileSystem(new FUnrealFileSystem(), true, false);
        auto& Runtime = FModuleManager::GetModuleChecked<FDasRuntimeModule>(TEXT("DasRuntime"));
        FTCHARToUTF8 File(*Runtime.ScriptPath(TEXT("aot_ue_guard.das")));
        das::CodeOfPolicies Policies;
        Policies.aot = false;
        Policies.aot_module = true;
        auto Program = DSLCompile(File.Get(), Access, Log, Modules, Policies);
        if (!Program || Program->failed())
        {
            Error = Errors(Program, Log);
            return false;
        }
        FUnrealDasContext C(Program->getContextStackSize());
        FActiveContextScope Scope(C);
        if (!Program->simulate(C, Log))
        {
            Error = Errors(Program, Log);
            return false;
        }
        // Current daScript emits C++ through daslib/aot_cpp; the old native
        // Program::aotCpp entry point is no longer linked in the pinned runtime.
        das::TextWriter EmitterLog;
        das::ModuleGroup EmitterModules;
        FTCHARToUTF8 Driver(*Runtime.ScriptPath(TEXT("ue_aot_emit.das")));
        auto Emitter = DSLCompile(Driver.Get(), Access, EmitterLog, EmitterModules);
        if (!Emitter || Emitter->failed())
        {
            Error = Errors(Emitter, EmitterLog);
            return false;
        }
        FUnrealDasContext EC(Emitter->getContextStackSize());
        FActiveContextScope EmitterScope(EC);
        if (!Emitter->simulate(EC, EmitterLog))
        {
            Error = Errors(Emitter, EmitterLog);
            return false;
        }
        auto* Emit = EC.findFunction("emit");
        if (!Emit)
        {
            Error = TEXT("AOT emitter entry missing");
            return false;
        }
        vec4f Args[] = {das::cast<das::Program*>::from(Program.get()), das::cast<das::Context*>::from(&C)};
        const auto Value = EC.evalWithCatch(Emit, Args);
        if (const auto* X = EC.getException())
        {
            Error = UTF8_TO_TCHAR(X);
            return false;
        }
        const auto* Chars = das::cast<char*>::to(Value);
        const das::string Text = Chars ? Chars : "";
        if (Text.find("dasue::aot::Call<") == das::string::npos)
        {
            Error = TEXT("AOT emitter did not use guarded UE bridge");
            return false;
        }
        return FFileHelper::SaveStringToFile(UTF8_TO_TCHAR(Text.c_str()), *Output,
                                             FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM);
    }
    catch (const std::exception& X)
    {
        Error = UTF8_TO_TCHAR(X.what());
        return false;
    }
}

bool RunDasUEAotProbe(FString& Report)
{
    int32 Checks = 0;
    bool Good = true;
    auto Check = [&](bool V, const TCHAR* Name, const FString& Error = FString())
    {
        ++Checks;
        Good &= V;
        Report += FString::Printf(TEXT("DAS_UE_AOT_%s %s %s\n"), V ? TEXT("PASS"): TEXT("FAIL"), Name, *Error);
    };
    auto& Runtime = FModuleManager::GetModuleChecked<FDasRuntimeModule>(TEXT("DasRuntime"));
    auto Owner = TStrongObjectPtr<UDasObjectFixture>(NewObject<UDasObjectFixture>());
    const auto Path = Runtime.ScriptPath(TEXT("aot_ue_guard.das"));
    auto Run = [&](int32 Mode)
    {
        return Runtime.RunFile(Path, Mode, true, Owner.Get());
    };
    auto R = Run(0);
    Check(R.bSucceeded && R.bUsedAot && R.Value == 102 && Owner->health == 102, TEXT("native_read_write"), R.Error);
    R = Run(4);
    Check(R.bSucceeded && R.bUsedAot && R.Value == 6 &&
          Owner->DSLText == TEXT("native AOT") && Owner->bScalar, TEXT("scalar_string_bool_float_double"), R.Error);
    R = Run(1);
    Check(!R.bSucceeded && !R.Error.IsEmpty(), TEXT("missing_property_refused"), R.Error);
    R = Run(2);
    Check(!R.bSucceeded && Owner->readOnly == 7, TEXT("readonly_refused"), R.Error);
    R = Run(3);
    Check(!R.bSucceeded && Owner->speed == 3.5f, TEXT("wrong_property_type_refused"), R.Error);
    R = Runtime.RunFile(Path, 0, true);
    Check(!R.bSucceeded && Owner->health == 102, TEXT("missing_owner_refused"), R.Error);
    // Prepare the linked function once, then enter its native body under each forbidden
    // execution state. RunFile itself would refuse before reaching the extern guard.
    das::TextWriter Log;
    FCompiledContext Live;
    auto Access = das::make_smart<FUnrealFileAccess>();
    Access->addFileSystem(new FUnrealFileSystem(), true, false);
    Live.Access = Access;
    FTCHARToUTF8 File(*Path);
    das::CodeOfPolicies Policies;
    Policies.aot = true;
    Live.Program = DSLCompile(File.Get(), Access, Log, Live.Modules, Policies);
    if (!Live.Program || Live.Program->failed())
    {
        Check(false, TEXT("prepare"), Errors(Live.Program, Log));
        return false;
    }
    Live.Context = MakeUnique<FUnrealDasContext>(Live.Program->getContextStackSize());
    auto& C = *Live.Context;
    C.Self = Owner.Get();
    {
        FActiveContextScope Scope(C);
        if (!Live.Program->simulate(C, Log))
        {
            Check(false, TEXT("simulate"));
            return false;
        }
    }
    Live.Function = C.findFunction("main");
    if (!Live.Function || !Live.Function->aot)
    {
        Check(false, TEXT("native_required"));
        return false;
    }
    auto Eval = [&](bool Active)
    {
        C.restart();
        vec4f Args[] = {das::cast<int32>::from(0)};
        auto Invoke = [&]
        {
            C.evalWithCatch(Live.Function, Args);
            const auto* E = C.getException();
            return E ? FString(UTF8_TO_TCHAR(E)) : FString();
        };
        if (Active)
        {
            FActiveContextScope Scope(C);
            return Invoke();
        }
        return Invoke();
    };
    {
        FDasPhysicsExecutionScope Physics;
        const auto E = Eval(true);
        Check(E.Contains(TEXT("PhysicsPreSim")), TEXT("physics_role_refused"), E);
    }
    {
        TGuardValue<das::Context*> Physics(RestrictedPhysicsContext, &C);
        const auto E = Eval(true);
        Check(E.Contains(TEXT("isolated physics")), TEXT("isolated_vm_refused"), E);
    }
    const auto NoActive = Eval(false);
    Check(NoActive.Contains(TEXT("active game-thread")), TEXT("missing_context_refused"), NoActive);
    const auto Worker = Async(EAsyncExecution::ThreadPool, [&] { return Eval(true); }).Get();
    Check(Worker.Contains(TEXT("game-thread")), TEXT("worker_thread_refused"), Worker);
    {
        FUnrealDasContext Other(16384);
        FActiveContextScope Scope(Other);
        const auto E = Eval(false);
        Check(E.Contains(TEXT("active game-thread")), TEXT("foreign_context_refused"), E);
    }
    const auto Recovered = Eval(true);
    Check(Recovered.IsEmpty() && Owner->health == 104, TEXT("recovered_without_forbidden_writes"), Recovered);
    bool Closed = false, Open = false;
    das::Module::require("ue_foundation")
        ->functions.foreach (
            [&](const das::FunctionPtr& Fn)
            {
                if (Fn->name == "ue_self_object")
                    Closed = Fn->noAot;
                if (Fn->name == "ue_get_self_int")
                    Open = !Fn->noAot;
            });
    Check(Closed && Open, TEXT("unsupported_abi_stays_closed"));
    Owner.Reset();
    CollectGarbage(RF_NoFlags);
    const auto Dead = Eval(true);
    Check(Dead.Contains(TEXT("missing or expired")), TEXT("expired_owner_refused"), Dead);
    Check(UEAotEntries.Num() > 0, TEXT("registered_checked_abi"));
    Report += FString::Printf(TEXT("DAS_UE_AOT_ABI_ENTRIES=%d\n"), UEAotEntries.Num());
    Report += FString::Printf(TEXT("DAS_UE_AOT_RESULT=%s checks=%d\n"), Good ? TEXT("PASS"): TEXT("FAIL"), Checks);
    return Good;
}
