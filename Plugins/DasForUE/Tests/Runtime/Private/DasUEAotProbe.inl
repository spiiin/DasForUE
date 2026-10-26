// Included in DasRuntime.cpp; does not expose VM internals to user scripts.
bool GenerateDasUEAotProbe(const FString& Output, FString& Error, const FString& Script)
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
        FTCHARToUTF8 File(*Runtime.ScriptPath(Script));
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
    // Object externs: compiled, with the interpreter's value; an extern's error has the interpreter's text.
    R = Run(5);
    const auto Interpreted = Runtime.RunFile(Path, 5, false, Owner.Get());
    Check(R.bSucceeded && R.bUsedAot && R.Value == 15 && Interpreted.bSucceeded && Interpreted.Value == 15,
          TEXT("object_externs"), R.Error + Interpreted.Error);
    auto Message = [](const FString& Error)
    {
        FString Text = Error;
        if (const int32 Bar = Text.Find(TEXT(" | ")); Bar != INDEX_NONE)
            Text.MidInline(Bar + 3);
        if (const int32 End = Text.Find(TEXT("\n")); End != INDEX_NONE)
            Text.LeftInline(End);
        return Text.TrimEnd();
    };
    const auto AotError = Run(6);
    const auto InterpretedError = Runtime.RunFile(Path, 6, false, Owner.Get());
    Check(!AotError.bSucceeded && !InterpretedError.bSucceeded && !Message(AotError.Error).IsEmpty() &&
              Message(AotError.Error) == Message(InterpretedError.Error),
          TEXT("object_extern_error"), Message(AotError.Error) + TEXT(" / ") + Message(InterpretedError.Error));
    // Math externs and operators (CallOp passes the context the generated code does not).
    R = Run(7);
    const auto InterpretedMath = Runtime.RunFile(Path, 7, false, Owner.Get());
    Check(R.bSucceeded && R.bUsedAot && R.Value == 6 && InterpretedMath.bSucceeded && InterpretedMath.Value == 6,
          TEXT("math_externs"), R.Error + InterpretedMath.Error);
    const auto MathError = Run(8);
    const auto InterpretedMathError = Runtime.RunFile(Path, 8, false, Owner.Get());
    Check(!MathError.bSucceeded && !InterpretedMathError.bSucceeded && !Message(MathError.Error).IsEmpty() &&
              Message(MathError.Error) == Message(InterpretedMathError.Error),
          TEXT("math_extern_error"), Message(MathError.Error) + TEXT(" / ") + Message(InterpretedMathError.Error));
    // Names and texts (their tokens cross the bridge).
    R = Run(9);
    Check(R.bSucceeded && R.bUsedAot && R.Value == 7, TEXT("name_text_externs"), R.Error);
    // A reflected call frame, and the error of a missing function.
    R = Run(10);
    Check(R.bSucceeded && R.bUsedAot && R.Value == 5, TEXT("call_frame_externs"), R.Error);
    R = Run(12);
    Check(R.bSucceeded && R.bUsedAot && R.Value == 20123, TEXT("value_fields"), R.Error);
    // A block argument: the compiled block changes a captured variable; an error thrown in the block.
    R = Run(13);
    Check(R.bSucceeded && R.bUsedAot && R.Value == 41, TEXT("block_externs"), R.Error);
    const auto BlockError = Run(14);
    const auto InterpretedBlockError = Runtime.RunFile(Path, 14, false, Owner.Get());
    Check(!BlockError.bSucceeded && !InterpretedBlockError.bSucceeded && !Message(BlockError.Error).IsEmpty() &&
              Message(BlockError.Error) == Message(InterpretedBlockError.Error),
          TEXT("block_error"), Message(BlockError.Error) + TEXT(" / ") + Message(InterpretedBlockError.Error));
    const auto FrameError = Run(11);
    const auto InterpretedFrameError = Runtime.RunFile(Path, 11, false, Owner.Get());
    Check(!FrameError.bSucceeded && !InterpretedFrameError.bSucceeded && !Message(FrameError.Error).IsEmpty() &&
              Message(FrameError.Error) == Message(InterpretedFrameError.Error),
          TEXT("call_frame_error"), Message(FrameError.Error) + TEXT(" / ") + Message(InterpretedFrameError.Error));
    R = Run(1);
    Check(!R.bSucceeded && !R.Error.IsEmpty(), TEXT("missing_property_refused"), R.Error);
    R = Run(2);
    Check(!R.bSucceeded && Owner->readOnly == 7, TEXT("readonly_refused"), R.Error);
    R = Run(3);
    Check(!R.bSucceeded && Owner->speed == 3.5f, TEXT("wrong_property_type_refused"), R.Error);
    R = Runtime.RunFile(Path, 0, true);
    Check(!R.bSucceeded && Owner->health == 102, TEXT("missing_owner_refused"), R.Error);
    R = Run(15);
    Check(R.bSucceeded && R.bUsedAot && R.Value == 1 && Owner->InteropCounts.FindRef(3) == 41 &&
          Owner->InteropStrings.FindRef(TEXT("世界 Key")) == 2.5 && Owner->InteropBytes.FindRef(7) == 11,
                                        TEXT("interop_native_int_string_byte_keys_and_references"), R.Error);
    R = Run(16);
    Check(!R.bSucceeded && R.Error.Contains(TEXT("map key/property type mismatch")) && Owner->InteropStrings.Num() == 1,
                                            TEXT("interop_key_mismatch_atomic"), R.Error);
    R = Run(17);
    Check(!R.bSucceeded && Owner->InteropLocked.IsEmpty(), TEXT("interop_readonly_atomic"), R.Error);
    const int32 BeforeForbidden = Owner->InteropCounts.FindRef(0);
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
    das::InteropFunction* CheckedMap = nullptr;
    das::Module::require("ue_foundation")
        ->functions.foreach (
            [&](const das::FunctionPtr& Fn)
            {
                if (Fn->name == "__ue_self_map_get_int" && Fn->arguments[1]->type->baseType == das::Type::anyArgument)
                    CheckedMap = reinterpret_cast<das::InteropFunction*>(
                        static_cast<das::BuiltInFunction*>(Fn)->getBuiltinAddress());
            });
    auto Direct = [&]()
    {
        C.restart();
        C.runWithCatch(
            [&]
            {
                if (CheckedMap)
                    CheckedMap(C, nullptr, nullptr);
            });
        const auto* E = C.getException();
        return E ? FString(UTF8_TO_TCHAR(E)) : FString();
    };
    const auto DirectMissing = Direct();
    Check(DirectMissing.Contains(TEXT("active game-thread")), TEXT("interop_direct_missing_context_before_metadata"),
                                                                   DirectMissing);
    {
        FUnrealDasContext Other(16384);
        FActiveContextScope Scope(Other);
        const auto E = Direct();
        Check(E.Contains(TEXT("active game-thread")), TEXT("interop_direct_foreign_context_before_metadata"), E);
    }
    {
        FActiveContextScope Scope(C);
        const auto E = Direct();
        Check(E.Contains(TEXT("invalid map interop call metadata")),
                         TEXT("interop_valid_context_null_metadata_rejected"), E);
    }
    const auto Recovered = Eval(true);
    Check(Recovered.IsEmpty() && Owner->health == 104, TEXT("recovered_without_forbidden_writes"), Recovered);
    Check(Owner->InteropCounts.FindRef(0) == BeforeForbidden + 1,
          TEXT("interop_forbidden_roles_do_not_write_and_recover"));
    bool InteropOpen = false, NameKeyClosed = false;
    das::Module::require("ue_foundation")
        ->functions.foreach (
            [&](const das::FunctionPtr& Fn)
            {
                if (Fn->name == "__ue_self_map_get_int")
                {
                    if (Fn->arguments[1]->type->baseType == das::Type::anyArgument)
                        InteropOpen = !Fn->noAot;
                    else
                        NameKeyClosed = Fn->noAot;
                }
            });
    Check(InteropOpen && NameKeyClosed, TEXT("interop_AOT_scalar_open_opaque_name_closed"));
    bool Closed = false, Open = false, ObjectOpen = false, FrameOpen = false, BlockOpen = false, CollectClosed = false;
    das::Module::require("ue_foundation")
        ->functions.foreach (
            [&](const das::FunctionPtr& Fn)
            {
                if (Fn->name == "ue_physics_input") // the isolated physics VM's externs stay interpreted
                    Closed = Fn->noAot;
                if (Fn->name == "__ue_library_scope") // a block crosses the bridge as a das::Block
                    BlockOpen = !Fn->noAot;
                if (Fn->name == "ue_struct_default") // may collect struct snapshots: interpreter frames only
                    CollectClosed = Fn->noAot;
                if (Fn->name == "ue_begin_call") // call frame handles cross the bridge as tokens
                    FrameOpen = !Fn->noAot;
                if (Fn->name == "ue_get_self_int")
                    Open = !Fn->noAot;
                if (Fn->name == "ue_self_object")
                    ObjectOpen = !Fn->noAot;
            });
    Check(Closed && Open && ObjectOpen && FrameOpen && BlockOpen && CollectClosed,
          TEXT("unsupported_abi_stays_closed"));
    Owner.Reset();
    CollectGarbage(RF_NoFlags);
    const auto Dead = Eval(true);
    Check(Dead.Contains(TEXT("missing or expired")), TEXT("expired_owner_refused"), Dead);
    Check(UEAotEntries.Num() > 0, TEXT("registered_checked_abi"));
    Report += FString::Printf(TEXT("DAS_UE_AOT_ABI_ENTRIES=%d\n"), UEAotEntries.Num());
    Report += FString::Printf(TEXT("DAS_UE_AOT_RESULT=%s checks=%d\n"), Good ? TEXT("PASS"): TEXT("FAIL"), Checks);
    return Good;
}
