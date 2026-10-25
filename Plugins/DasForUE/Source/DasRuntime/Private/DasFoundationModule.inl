// Part of DasRuntime.cpp: The ue_foundation daScript module: annotations and extern registrations.
// Included in place; declaration order matters.
das::PassMacro* MakeDasClosedCallsLint(); // DasNativeBinds.inl
class FUnrealFoundationModule final : public das::Module
{
public:
    virtual das::ModuleAotType aotRequire(das::TextWriter& Writer) const override
    {
        Writer << "#include \"DasUEAot.h\"\n";
        return das::ModuleAotType::cpp;
    }
    FUnrealFoundationModule() : das::Module("ue_foundation")
    {
        AddCollisionProfileEnum(*this);

        globalLintMacros.push_back(das::unique_ptr<das::PassMacro>(new FUEInputNameLint()));
        globalLintMacros.push_back(das::unique_ptr<das::PassMacro>(MakeDasClosedCallsLint()));
        // `options ue_native_routes = false`: native gameplay methods always go through reflection, even when a
        // native bind reflects them (DasNativeBinds.h, Reflects). Tests compare both paths with it.
        options["ue_native_routes"] = das::Type::tBool;
        addCommentReader(new FUECommentReader());
        addAnnotation(new FUENativeBaseAnnotation());
        addAnnotation(new FUEClassAnnotation());
        {
            auto* Macro = new FUEDelegateReferenceMacro();
            ownedCallMacros.push_back(das::unique_ptr<das::CallMacro>(Macro));
            Macro->module = this;
            addCallMacro(Macro->name,
                         [Macro](const das::LineInfo& At) -> das::ExprLooksLikeCall*
                         {
                             auto* E = new das::ExprCallMacro(At, Macro->name);
                             E->macro = Macro;
                             return E;
                         });
        }
        inferMacros.push_back(das::unique_ptr<das::PassMacro>(new FUEFunctionReferencePass()));
        for (const auto& Pair : {std::pair<const char*, int>{"ue_static", 0}, {"ue_call", 1}, {"ue_struct_get", 2}})
        {
            auto* Macro = new FUEReflectionMacro(Pair.first, Pair.second);
            ownedCallMacros.push_back(das::unique_ptr<das::CallMacro>(Macro));
            Macro->module = this;
            addCallMacro(Macro->name,
                         [Macro](const das::LineInfo& At) -> das::ExprLooksLikeCall*
                         {
                             auto* E = new das::ExprCallMacro(At, Macro->name);
                             E->macro = Macro;
                             return E;
                         });
        }
        addAnnotation(new FUEReflectedCallAnnotation());
        addAnnotation(new FUEWidgetLayoutAnnotation());
        {
            auto* Macro = new FUEWidgetViewMacro();
            ownedCallMacros.push_back(das::unique_ptr<das::CallMacro>(Macro));
            Macro->module = this;
            addCallMacro(Macro->name,
                         [Macro](const das::LineInfo& At) -> das::ExprLooksLikeCall*
                         {
                             auto* E = new das::ExprCallMacro(At, Macro->name);
                             E->macro = Macro;
                             return E;
                         });
        }
        addAnnotation(new FUEAsyncPhysicsAnnotation());
        addAnnotation(new FUELibraryAnnotation());
        addAnnotation(new FUEEnumAnnotation());
        addAnnotation(new FUENativeEnumAnnotation());
        addAnnotation(new FUENativeStructAnnotation());
        addAnnotation(new FUEStructAnnotation());
        addAnnotation(new FUEGASDeclarationAnnotation(false));
        addAnnotation(new FUEGASDeclarationAnnotation(true));
        addAnnotation(new FUEGASCueAnnotation());
        addAnnotation(new FUEGASAbilityAnnotation());
        addAnnotation(new FUEGASCalculationAnnotation());
        addAnnotation(new FUENativeInterfaceAnnotation());
        addAnnotation(new FUENativeInterfaceAnnotation("ue_interface_ref"));
        addAnnotation(new FUESourceValueBinding());
        addAnnotation(new FUEObjectTypeAnnotation());
        addAnnotation(new FUEAuthoredRefAnnotation());
        addTypeMacro(new FUEObjectTypeMacro());
        addTypeMacro(new FUEObjectKeyTypeMacro());
        addAnnotation(new FUEClassTypeAnnotation());
        addTypeMacro(new FUEClassTypeMacro());
        addAnnotation(new FUESoftAnnotation());
        addAnnotation(new FUEGameplayValueAnnotation());
        addTypeMacro(new FUESoftTypeMacro(false));
        addTypeMacro(new FUESoftTypeMacro(true));
        for (const auto& Pair :
             {std::pair<const char*, int>{"ue_soft", 0}, {"ue_soft_value", 1}, {"ue_soft_get", 2}, {"ue_soft_load", 3}})
        {
            auto* Macro = new FUESoftCallMacro(Pair.first, Pair.second);
            ownedCallMacros.push_back(das::unique_ptr<das::CallMacro>(Macro));
            Macro->module = this;
            addCallMacro(Macro->name,
                         [Macro](const das::LineInfo& At) -> das::ExprLooksLikeCall*
                         {
                             auto* E = new das::ExprCallMacro(At, Macro->name);
                             E->macro = Macro;
                             return E;
                         });
        }
        {
            auto* Macro = new FUESoftUpcastMacro();
            ownedCallMacros.push_back(das::unique_ptr<das::CallMacro>(Macro));
            Macro->module = this;
            addCallMacro(Macro->name,
                         [Macro](const das::LineInfo& At) -> das::ExprLooksLikeCall*
                         {
                             auto* E = new das::ExprCallMacro(At, Macro->name);
                             E->macro = Macro;
                             return E;
                         });
        }
        for (bool Raw : {false, true})
        {
            auto* Macro = new FUEClassCastFactory(Raw);
            ownedCallMacros.push_back(das::unique_ptr<das::CallMacro>(Macro));
            Macro->module = this;
            addCallMacro(Macro->name,
                         [Macro](const das::LineInfo& At) -> das::ExprLooksLikeCall*
                         {
                             auto* E = new das::ExprCallMacro(At, Macro->name);
                             E->macro = Macro;
                             return E;
                         });
        }
        addAnnotation(new FUEDelegateAnnotation(false));
        addAnnotation(new FUEDelegateAnnotation(true));
        addAnnotation(new FUECallbackFrameAnnotation());
        addAnnotation(new FUEHandlerContractAnnotation());
        addAnnotation(new FUEFunctionAnnotation());
        addAnnotation(new FUECallbackAnnotation(false));
        addAnnotation(new FUECallbackAnnotation(true));
        addAnnotation(new FUEInterfaceAnnotation());
        addAnnotation(new FUEImplementsAnnotation());
        addAnnotation(new FUEComponentAnnotation());
        {
            auto* Macro = new FUEBoundDelegateFactory();
            ownedCallMacros.push_back(das::unique_ptr<das::CallMacro>(Macro));
            Macro->module = this;
            addCallMacro(Macro->name,
                         [Macro](const das::LineInfo& At) -> das::ExprLooksLikeCall*
                         {
                             auto* E = new das::ExprCallMacro(At, Macro->name);
                             E->macro = Macro;
                             return E;
                         });
        }
        {
            auto* Macro = new FUEDataTableRowFactory();
            ownedCallMacros.push_back(das::unique_ptr<das::CallMacro>(Macro));
            Macro->module = this;
            addCallMacro(Macro->name,
                         [Macro](const das::LineInfo& At) -> das::ExprLooksLikeCall*
                         {
                             auto* E = new das::ExprCallMacro(At, Macro->name);
                             E->macro = Macro;
                             return E;
                         });
        }
        for (bool Component : {false, true})
            for (bool All : {false, true})
            {
                auto* Macro = new FUEObjectQueryFactory(Component, All);
                ownedCallMacros.push_back(das::unique_ptr<das::CallMacro>(Macro));
                Macro->module = this;
                addCallMacro(Macro->name,
                             [Macro](const das::LineInfo& At) -> das::ExprLooksLikeCall*
                             {
                                 auto* E = new das::ExprCallMacro(At, Macro->name);
                                 E->macro = Macro;
                                 return E;
                             });
            }
        {
            auto* Macro = new FUEActorSpawnFactory();
            ownedCallMacros.push_back(das::unique_ptr<das::CallMacro>(Macro));
            Macro->module = this;
            addCallMacro(Macro->name,
                         [Macro](const das::LineInfo& At) -> das::ExprLooksLikeCall*
                         {
                             auto* E = new das::ExprCallMacro(At, Macro->name);
                             E->macro = Macro;
                             return E;
                         });
        }
        {
            auto* Macro = new FUEClassLoadedFactory();
            ownedCallMacros.push_back(das::unique_ptr<das::CallMacro>(Macro));
            Macro->module = this;
            addCallMacro(Macro->name,
                         [Macro](const das::LineInfo& At) -> das::ExprLooksLikeCall*
                         {
                             auto* E = new das::ExprCallMacro(At, Macro->name);
                             E->macro = Macro;
                             return E;
                         });
        }
        for (int Mode = 0; Mode < 3; ++Mode)
        {
            auto* Macro = new FUENativeComponentFactory(Mode);
            ownedCallMacros.push_back(das::unique_ptr<das::CallMacro>(Macro));
            Macro->module = this;
            addCallMacro(Macro->name,
                         [Macro](const das::LineInfo& At) -> das::ExprLooksLikeCall*
                         {
                             auto* E = new das::ExprCallMacro(At, Macro->name);
                             E->macro = Macro;
                             return E;
                         });
        }
        for (int Mode = 0; Mode < 3; ++Mode)
        {
            auto* Macro = new FUEObjectKeyFactory(Mode);
            ownedCallMacros.push_back(das::unique_ptr<das::CallMacro>(Macro));
            Macro->module = this;
            addCallMacro(Macro->name,
                         [Macro](const das::LineInfo& At) -> das::ExprLooksLikeCall*
                         {
                             auto* E = new das::ExprCallMacro(At, Macro->name);
                             E->macro = Macro;
                             return E;
                         });
        }
        inferMacros.push_back(das::unique_ptr<das::PassMacro>(new FUEPropertyPass()));
        inferMacros.push_back(das::unique_ptr<das::PassMacro>(new FUEStatePass()));
        globalLintMacros.push_back(das::unique_ptr<das::PassMacro>(new FUEPathLintPass()));
        for (int Mode : {0, 1, 2})
        {
            auto* Macro = new FUEIdentityMacro(Mode == 1, Mode == 2);
            ownedCallMacros.push_back(das::unique_ptr<das::CallMacro>(Macro));
            Macro->module = this;
            addCallMacro(Macro->name,
                         [Macro](const das::LineInfo& At) -> das::ExprLooksLikeCall*
                         {
                             auto* E = new das::ExprCallMacro(At, Macro->name);
                             E->macro = Macro;
                             return E;
                         });
        }

        for (bool Owner : {false, true})
        {
            auto* Macro = new FUEReferenceFactory(Owner);
            ownedCallMacros.push_back(das::unique_ptr<das::CallMacro>(Macro));
            Macro->module = this;
            addCallMacro(Macro->name,
                         [Macro](const das::LineInfo& At) -> das::ExprLooksLikeCall*
                         {
                             auto* E = new das::ExprCallMacro(At, Macro->name);
                             E->macro = Macro;
                             return E;
                         });
        }
        das::ModuleLibrary Library(this);
        Library.addBuiltInModule();
        addAnnotation(new FPhysicsBodyTokenAnnotation(Library));
        AddPhysicsDataExtern<DAS_BIND_FUN(PhysicsBody)>(*this, Library, "ue_physics_body");
        AddPhysicsDataExtern<DAS_BIND_FUN(PhysicsAddForce)>(*this, Library, "ue_physics_add_force");
        AddUEExtern<DAS_BIND_FUN(AsyncPhysicsSetInput)>(*this, Library, "ue_async_physics_set_input",
                                                        das::SideEffects::modifyExternal);
        AddUEExtern<DAS_BIND_FUN(AsyncPhysicsHasOutput)>(*this, Library, "ue_async_physics_has_output",
                                                         das::SideEffects::modifyExternal);
        AddUEExtern<DAS_BIND_FUN(AsyncPhysicsOutput)>(*this, Library, "ue_async_physics_output",
                                                      das::SideEffects::modifyExternal);
        AddUEExtern<DAS_BIND_FUN(AsyncPhysicsRunning)>(*this, Library, "ue_async_physics_running",
                                                       das::SideEffects::modifyExternal);
        AddUEExtern<DAS_BIND_FUN(AsyncPhysicsError)>(*this, Library, "ue_async_physics_error",
                                                     das::SideEffects::modifyExternal);
        AddPhysicsDataExtern<DAS_BIND_FUN(PhysicsInput)>(*this, Library, "ue_physics_input");
        AddPhysicsDataExtern<DAS_BIND_FUN(PhysicsOutput)>(*this, Library, "ue_physics_output");
        addAnnotation(new FDSLTextAnnotation(Library));
        addAnnotation(new FDSLNameAnnotation(Library));
        addAnnotation(new FDSLEnumAnnotation(Library));
        addAnnotation(new TDasTicksAnnotation<FDasDateTimeValue>("UEDateTime", Library, "FDasDateTimeValue"));
        addAnnotation(new TDasTicksAnnotation<FDasTimespanValue>("UETimespan", Library, "FDasTimespanValue"));
        addAnnotation(new FReflectedValueAnnotation(Library));
        addAnnotation(new FDSLVector2DAnnotation(Library));
        addAnnotation(new FDSLVectorAnnotation(Library));
        addAnnotation(new FDSLRotatorAnnotation(Library));
        addAnnotation(new FDSLQuatAnnotation(Library));
        addAnnotation(new FDSLLinearColorAnnotation(Library));
        addAnnotation(new FDSLColorAnnotation(Library));
        addAnnotation(new FMathValueAnnotation<FTransform>("UETransform", Library, "FTransform"));

        addAnnotation(new FObjectHandleAnnotation(Library));
        addAnnotation(new FClassHandleAnnotation(Library));
        addAnnotation(new FTraceHitAnnotation(Library));
        addAnnotation(new FCallHandleAnnotation(Library));
        addAnnotation(new TInputEventAnnotation<FDasPaintValue>("UEPaintContext", Library, "FDasPaintValue"));
        AddUEExtern<DAS_BIND_FUN(ArgumentPaintContext)>(*this, Library, "__ue_arg_paint_context",
                                                        das::SideEffects::modifyExternal);
        AddUEExtern<DAS_BIND_FUN(ResultPaintContext), das::SimNode_ExtFuncCallAndCopyOrMove>(
            *this, Library, "__ue_result_paint_context", das::SideEffects::accessExternal);
        addAnnotation(new TInputEventAnnotation<FDasEventReplyValue>("UEEventReply", Library, "FDasEventReplyValue"));
        AddUEExtern<DAS_BIND_FUN(ArgumentEventReply)>(*this, Library, "__ue_arg_event_reply",
                                                      das::SideEffects::modifyExternal);
        AddUEExtern<DAS_BIND_FUN(ResultEventReply), das::SimNode_ExtFuncCallAndCopyOrMove>(
            *this, Library, "__ue_result_event_reply", das::SideEffects::accessExternal);
        AddUEExtern<DAS_BIND_FUN(WidgetHandled), das::SimNode_ExtFuncCallAndCopyOrMove>(
            *this, Library, "__ue_widget_handled", das::SideEffects::modifyExternal);
        AddUEExtern<DAS_BIND_FUN(WidgetUnhandled), das::SimNode_ExtFuncCallAndCopyOrMove>(
            *this, Library, "__ue_widget_unhandled", das::SideEffects::modifyExternal);
        AddUEExtern<DAS_BIND_FUN(WidgetReplyCapture), das::SimNode_ExtFuncCallAndCopyOrMove>(
            *this, Library, "__ue_widget_capture", das::SideEffects::modifyExternal);
        AddUEExtern<DAS_BIND_FUN(WidgetReplyRelease), das::SimNode_ExtFuncCallAndCopyOrMove>(
            *this, Library, "__ue_widget_release", das::SideEffects::modifyExternal);
        AddUEExtern<DAS_BIND_FUN(WidgetReplyFocus), das::SimNode_ExtFuncCallAndCopyOrMove>(
            *this, Library, "__ue_widget_focus", das::SideEffects::modifyExternal);
        AddUEExtern<DAS_BIND_FUN(WidgetReplyClearFocus), das::SimNode_ExtFuncCallAndCopyOrMove>(
            *this, Library, "__ue_widget_clear_focus", das::SideEffects::modifyExternal);
        AddUEExtern<DAS_BIND_FUN(WidgetReplyLock), das::SimNode_ExtFuncCallAndCopyOrMove>(
            *this, Library, "__ue_widget_lock", das::SideEffects::modifyExternal);
        AddUEExtern<DAS_BIND_FUN(WidgetReplyUnlock), das::SimNode_ExtFuncCallAndCopyOrMove>(
            *this, Library, "__ue_widget_unlock", das::SideEffects::modifyExternal);
        AddUEExtern<DAS_BIND_FUN(WidgetReplyPosition), das::SimNode_ExtFuncCallAndCopyOrMove>(
            *this, Library, "__ue_widget_position", das::SideEffects::modifyExternal);
        AddUEExtern<DAS_BIND_FUN(WidgetReplyDrag), das::SimNode_ExtFuncCallAndCopyOrMove>(
            *this, Library, "__ue_widget_drag", das::SideEffects::modifyExternal);
        AddUEExtern<DAS_BIND_FUN(WidgetReplyEndDrag), das::SimNode_ExtFuncCallAndCopyOrMove>(
            *this, Library, "__ue_widget_end_drag", das::SideEffects::modifyExternal);
        AddUEExtern<DAS_BIND_FUN(BindWidgetInput)>(*this, Library, "__ue_widget_bind_input",
                                                   das::SideEffects::modifyExternal);
        AddUEExtern<DAS_BIND_FUN(CreateDragOperation), das::SimNode_ExtFuncCallAndCopyOrMove>(
            *this, Library, "__ue_drag_create", das::SideEffects::modifyExternal);
        AddUEExtern<DAS_BIND_FUN(DragPayload), das::SimNode_ExtFuncCallAndCopyOrMove>(
            *this, Library, "__ue_drag_payload", das::SideEffects::accessExternal);
        AddUEExtern<DAS_BIND_FUN(DragVisual), das::SimNode_ExtFuncCallAndCopyOrMove>(*this, Library, "__ue_drag_visual",
                                                                                     das::SideEffects::accessExternal);
        AddUEExtern<DAS_BIND_FUN(DragTag)>(*this, Library, "__ue_drag_tag", das::SideEffects::accessExternal);
        AddUEExtern<DAS_BIND_FUN(DragValid)>(*this, Library, "__ue_drag_valid", das::SideEffects::accessExternal);
        AddUEExtern<DAS_BIND_FUN(BindWidgetDrag)>(*this, Library, "__ue_widget_bind_drag",
                                                  das::SideEffects::modifyExternal);
        addAnnotation(new TInputEventAnnotation<FDasWidgetAnimationValue>("UEWidgetAnimationHandle", Library,
                                                                          "FDasWidgetAnimationValue"));
        AddUEExtern<DAS_BIND_FUN(ResultWidgetAnimation), das::SimNode_ExtFuncCallAndCopyOrMove>(
                        *this, Library, "__ue_result_widget_animation", das::SideEffects::modifyExternal)
                        ->needCallerStackFrame = true;
        AddUEExtern<DAS_BIND_FUN(WidgetAnimationValid)>(*this, Library, "__ue_widget_animation_valid",
                                                        das::SideEffects::accessExternal);
        AddUEExtern<DAS_BIND_FUN(WidgetAnimationPlaying)>(*this, Library, "__ue_widget_animation_playing",
                                                          das::SideEffects::accessExternal);
        addAnnotation(new TInputEventAnnotation<FDasGeometryValue>("UEGeometry", Library, "FDasGeometryValue"));
        AddUEExtern<DAS_BIND_FUN(ArgumentGeometry)>(*this, Library, "__ue_arg_geometry",
                                                    das::SideEffects::modifyExternal);
        AddUEExtern<DAS_BIND_FUN(ResultGeometry), das::SimNode_ExtFuncCallAndCopyOrMove>(
                        *this, Library, "__ue_result_geometry", das::SideEffects::modifyExternal)
                        ->needCallerStackFrame = true;
        AddUEExtern<DAS_BIND_FUN(GeometryFromStruct), das::SimNode_ExtFuncCallAndCopyOrMove>(
            *this, Library, "__ue_unpack_geometry", das::SideEffects::accessExternal);
        addAnnotation(new TInputEventAnnotation<FDasInputEventValue>("UEInputEvent", Library, "FDasInputEventValue"));
        AddUEExtern<DAS_BIND_FUN(ArgumentInputEvent)>(*this, Library, "__ue_arg_inputevent",
                                                      das::SideEffects::modifyExternal);
        AddUEExtern<DAS_BIND_FUN(ResultInputEvent), das::SimNode_ExtFuncCallAndCopyOrMove>(
                        *this, Library, "__ue_result_inputevent", das::SideEffects::modifyExternal)
                        ->needCallerStackFrame = true;
        AddUEExtern<DAS_BIND_FUN(InputFromInputEvent), das::SimNode_ExtFuncCallAndCopyOrMove>(
            *this, Library, "__ue_unpack_inputevent", das::SideEffects::accessExternal);
        addAnnotation(new TInputEventAnnotation<FDasKeyEventValue>("UEKeyEvent", Library, "FDasKeyEventValue"));
        AddUEExtern<DAS_BIND_FUN(ArgumentKeyEvent)>(*this, Library, "__ue_arg_keyevent",
                                                    das::SideEffects::modifyExternal);
        AddUEExtern<DAS_BIND_FUN(ResultKeyEvent), das::SimNode_ExtFuncCallAndCopyOrMove>(
                        *this, Library, "__ue_result_keyevent", das::SideEffects::modifyExternal)
                        ->needCallerStackFrame = true;
        AddUEExtern<DAS_BIND_FUN(InputFromKeyEvent), das::SimNode_ExtFuncCallAndCopyOrMove>(
            *this, Library, "__ue_unpack_keyevent", das::SideEffects::accessExternal);
        addAnnotation(new TInputEventAnnotation<FDasAnalogInputEventValue>("UEAnalogInputEvent", Library,
                                                                           "FDasAnalogInputEventValue"));
        AddUEExtern<DAS_BIND_FUN(ArgumentAnalogInputEvent)>(*this, Library, "__ue_arg_analoginputevent",
                                                            das::SideEffects::modifyExternal);
        AddUEExtern<DAS_BIND_FUN(ResultAnalogInputEvent), das::SimNode_ExtFuncCallAndCopyOrMove>(
                        *this, Library, "__ue_result_analoginputevent", das::SideEffects::modifyExternal)
                        ->needCallerStackFrame = true;
        AddUEExtern<DAS_BIND_FUN(InputFromAnalogInputEvent), das::SimNode_ExtFuncCallAndCopyOrMove>(
            *this, Library, "__ue_unpack_analoginputevent", das::SideEffects::accessExternal);
        addAnnotation(
            new TInputEventAnnotation<FDasCharacterEventValue>("UECharacterEvent", Library, "FDasCharacterEventValue"));
        AddUEExtern<DAS_BIND_FUN(ArgumentCharacterEvent)>(*this, Library, "__ue_arg_characterevent",
                                                          das::SideEffects::modifyExternal);
        AddUEExtern<DAS_BIND_FUN(ResultCharacterEvent), das::SimNode_ExtFuncCallAndCopyOrMove>(
                        *this, Library, "__ue_result_characterevent", das::SideEffects::modifyExternal)
                        ->needCallerStackFrame = true;
        AddUEExtern<DAS_BIND_FUN(InputFromCharacterEvent), das::SimNode_ExtFuncCallAndCopyOrMove>(
            *this, Library, "__ue_unpack_characterevent", das::SideEffects::accessExternal);
        addAnnotation(
            new TInputEventAnnotation<FDasPointerEventValue>("UEPointerEvent", Library, "FDasPointerEventValue"));
        AddUEExtern<DAS_BIND_FUN(ArgumentPointerEvent)>(*this, Library, "__ue_arg_pointerevent",
                                                        das::SideEffects::modifyExternal);
        AddUEExtern<DAS_BIND_FUN(ResultPointerEvent), das::SimNode_ExtFuncCallAndCopyOrMove>(
                        *this, Library, "__ue_result_pointerevent", das::SideEffects::modifyExternal)
                        ->needCallerStackFrame = true;
        AddUEExtern<DAS_BIND_FUN(InputFromPointerEvent), das::SimNode_ExtFuncCallAndCopyOrMove>(
            *this, Library, "__ue_unpack_pointerevent", das::SideEffects::accessExternal);
        addAnnotation(new TInputEventAnnotation<FDasFocusEventValue>("UEFocusEvent", Library, "FDasFocusEventValue"));
        AddUEExtern<DAS_BIND_FUN(FocusEventUser)>(*this, Library, "__ue_focus_user", das::SideEffects::accessExternal);
        AddUEExtern<DAS_BIND_FUN(FocusEventCause)>(*this, Library, "__ue_focus_cause",
                                                   das::SideEffects::accessExternal);
        addAnnotation(new TInputEventAnnotation<FDasNavigationEventValue>("UENavigationEvent", Library,
                                                                          "FDasNavigationEventValue"));
        AddUEExtern<DAS_BIND_FUN(ArgumentNavigationEvent)>(*this, Library, "__ue_arg_navigationevent",
                                                           das::SideEffects::modifyExternal);
        AddUEExtern<DAS_BIND_FUN(ResultNavigationEvent), das::SimNode_ExtFuncCallAndCopyOrMove>(
                        *this, Library, "__ue_result_navigationevent", das::SideEffects::modifyExternal)
                        ->needCallerStackFrame = true;
        AddUEExtern<DAS_BIND_FUN(InputFromNavigationEvent), das::SimNode_ExtFuncCallAndCopyOrMove>(
            *this, Library, "__ue_unpack_navigationevent", das::SideEffects::accessExternal);
        addAnnotation(new FInputActionValueAnnotation(Library));
        AddUEExtern<DAS_BIND_FUN(PackInputValue), das::SimNode_ExtFuncCallAndCopyOrMove>(
                        *this, Library, "__ue_math_pack_input_value", das::SideEffects::modifyExternal,
                        "PackInputValue")
                        ->needCallerStackFrame = true;
        AddUEExtern<DAS_BIND_FUN(UnpackInputValue), das::SimNode_ExtFuncCallAndCopyOrMove>(
            *this, Library, "__ue_math_unpack_input_value", das::SideEffects::accessExternal, "UnpackInputValue");
        AddUEExtern<DAS_BIND_FUN(MakeInputValue), das::SimNode_ExtFuncCallAndCopyOrMove>(
            *this, Library, "__ue_input_make", das::SideEffects::none, "MakeInputValue");
        AddUEExtern<DAS_BIND_FUN(InputValueKind)>(*this, Library, "__ue_input_kind", das::SideEffects::none,
                                                  "InputValueKind");
        AddUEExtern<DAS_BIND_FUN(InputValueBool)>(*this, Library, "ue_input_bool", das::SideEffects::none,
                                                  "InputValueBool");
        AddUEExtern<DAS_BIND_FUN(InputValueAxis1D)>(*this, Library, "ue_input_axis1d", das::SideEffects::none,
                                                    "InputValueAxis1D");
        AddUEExtern<DAS_BIND_FUN(InputValueAxis2D), das::SimNode_ExtFuncCallAndCopyOrMove>(
            *this, Library, "ue_input_axis2d", das::SideEffects::none, "InputValueAxis2D");
        AddUEExtern<DAS_BIND_FUN(InputValueAxis3D), das::SimNode_ExtFuncCallAndCopyOrMove>(
            *this, Library, "ue_input_axis3d", das::SideEffects::none, "InputValueAxis3D");
        AddUEExtern<DAS_BIND_FUN(ArgumentInputValue)>(*this, Library, "__ue_arg_input_value",
                                                      das::SideEffects::modifyExternal, "ArgumentInputValue");
        AddUEExtern<DAS_BIND_FUN(ResultInputValue), das::SimNode_ExtFuncCallAndCopyOrMove>(
            *this, Library, "__ue_result_input_value", das::SideEffects::accessExternal, "ResultInputValue");
        addAnnotation(new FHitResultValueAnnotation(Library));
        addAnnotation(
            new FMathValueAnnotation<FDasConstraintHandle>("UEConstraintHandle", Library, "FDasConstraintHandle"));
        AddUEExtern<DAS_BIND_FUN(ArgumentConstraint)>(*this, Library, "__ue_arg_constraint",
                                                      das::SideEffects::modifyExternal, "ArgumentConstraint");
        AddUEExtern<DAS_BIND_FUN(ResultConstraint), das::SimNode_ExtFuncCallAndCopyOrMove>(
                        *this, Library, "__ue_result_constraint", das::SideEffects::modifyExternal, "ResultConstraint")
                        ->needCallerStackFrame = true;
        AddUEExtern<DAS_BIND_FUN(ConstraintValid)>(*this, Library, "ue_constraint_valid",
                                                   das::SideEffects::accessExternal, "ConstraintValid");
        AddUEExtern<DAS_BIND_FUN(ArgumentHitResult)>(*this, Library, "__ue_arg_hit_result",
                                                     das::SideEffects::modifyExternal, "ArgumentHitResult");
        AddUEExtern<DAS_BIND_FUN(ResultHitResult), das::SimNode_ExtFuncCallAndCopyOrMove>(
                        *this, Library, "__ue_result_hit_result", das::SideEffects::modifyExternal, "ResultHitResult")
                        ->needCallerStackFrame = true;
        AddUEExtern<DAS_BIND_FUN(EmptyHitResult), das::SimNode_ExtFuncCallAndCopyOrMove>(
                        *this, Library, "ue_hit_result_empty", das::SideEffects::modifyExternal, "EmptyHitResult")
                        ->needCallerStackFrame = true;
        AddUEExtern<DAS_BIND_FUN(ViewHitResult), das::SimNode_ExtFuncCallAndCopyOrMove>(
            *this, Library, "ue_hit_result_view", das::SideEffects::modifyExternal, "ViewHitResult");
        AddUEExtern<DAS_BIND_FUN(StoreGASTargetHit)>(*this, Library, "__ue_gas_store_hit",
                                                     das::SideEffects::modifyExternal, "StoreGASTargetHit");
        addAnnotation(new FOverlapResultAnnotation(Library));
        AddUEExtern<DAS_BIND_FUN(TraceMaterial), das::SimNode_ExtFuncCallAndCopyOrMove>(
            *this, Library, "__ue_trace_material", das::SideEffects::accessExternal, "TraceMaterial");
        AddUEExtern<DAS_BIND_FUN(TracePhysicsOwner), das::SimNode_ExtFuncCallAndCopyOrMove>(
            *this, Library, "__ue_trace_physics_owner", das::SideEffects::accessExternal, "TracePhysicsOwner");
        AddUEExtern<DAS_BIND_FUN(TraceBone)>(*this, Library, "__ue_trace_bone", das::SideEffects::accessExternal,
                                             "TraceBone")
                        ->needCallerStackFrame = true;
        AddUEExtern<DAS_BIND_FUN(OverlapWorld)>(*this, Library, "__ue_overlap_world", das::SideEffects::modifyExternal,
                                                "OverlapWorld")
                        ->needCallerStackFrame = true;
        AddUEExtern<DAS_BIND_FUN(OverlapReference), das::SimNode_ExtFuncCallAndCopyOrMove>(
            *this, Library, "__ue_overlap_reference", das::SideEffects::accessExternal, "OverlapReference");
        AddUEExtern<DAS_BIND_FUN(TraceWorld), das::SimNode_ExtFuncCallAndCopyOrMove>(
            *this, Library, "__ue_trace_world", das::SideEffects::modifyExternal, "TraceWorld");
        AddUEExtern<DAS_BIND_FUN(TraceActor), das::SimNode_ExtFuncCallAndCopyOrMove>(
            *this, Library, "__ue_trace_actor", das::SideEffects::accessExternal, "TraceActor");
        AddUEExtern<DAS_BIND_FUN(TraceComponent), das::SimNode_ExtFuncCallAndCopyOrMove>(
            *this, Library, "__ue_trace_component", das::SideEffects::accessExternal, "TraceComponent");
        AddUEExtern<DAS_BIND_FUN(ApplyGameplayDamage)>(*this, Library, "__ue_apply_damage",
                                                       das::SideEffects::modifyExternal, "ApplyGameplayDamage");
        AddUEExtern<DAS_BIND_FUN(ImageSoftBrushStatus)>(*this, Library, "__ue_image_soft_status",
                                                        das::SideEffects::accessExternal);
        AddUEExtern<DAS_BIND_FUN(ImageSoftBrushError)>(*this, Library, "__ue_image_soft_error",
                                                       das::SideEffects::accessExternal);
        AddUEExtern<DAS_BIND_FUN(CreateGameplayWidget), das::SimNode_ExtFuncCallAndCopyOrMove>(
            *this, Library, "__ue_widget_create", das::SideEffects::modifyExternal, "CreateGameplayWidget");
        AddUEExtern<DAS_BIND_FUN(GameplayWidgetVisible)>(*this, Library, "__ue_widget_visible",
                                                         das::SideEffects::accessExternal, "GameplayWidgetVisible");
        AddUEExtern<DAS_BIND_FUN(ShowGameplayWidget)>(*this, Library, "__ue_widget_show",
                                                      das::SideEffects::modifyExternal, "ShowGameplayWidget");
        AddUEExtern<DAS_BIND_FUN(RemoveGameplayWidget)>(*this, Library, "__ue_widget_remove",
                                                        das::SideEffects::modifyExternal, "RemoveGameplayWidget");
        AddUEExtern<DAS_BIND_FUN(GameplayWidgetText)>(*this, Library, "__ue_widget_text",
                                                      das::SideEffects::modifyExternal, "GameplayWidgetText");
        AddUEExtern<DAS_BIND_FUN(ResolveWidgetLayout)>(*this, Library, "__ue_widget_layout_resolve",
                                                       das::SideEffects::modifyExternal);
        AddUEExtern<DAS_BIND_FUN(WidgetLayoutField), das::SimNode_ExtFuncCallAndCopyOrMove>(
            *this, Library, "__ue_widget_layout_field", das::SideEffects::accessExternal);
        AddUEExtern<DAS_BIND_FUN(GameplayWidgetClickRef)>(*this, Library, "__ue_widget_click_ref",
                                                          das::SideEffects::modifyExternal);
        AddUEExtern<DAS_BIND_FUN(GameplayWidgetTextRef)>(*this, Library, "__ue_widget_text_ref",
                                                         das::SideEffects::modifyExternal);
        AddUEExtern<DAS_BIND_FUN(GameplayWidgetClick)>(*this, Library, "__ue_widget_click",
                                                       das::SideEffects::modifyExternal, "GameplayWidgetClick");
        AddUEExtern<DAS_BIND_FUN(GameplayWidgetInput)>(*this, Library, "__ue_widget_input",
                                                       das::SideEffects::modifyExternal, "GameplayWidgetInput");


        AddUEExtern<DAS_BIND_FUN(CharacterController), das::SimNode_ExtFuncCallAndCopyOrMove>(
            *this, Library, "__ue_pawn_controller", das::SideEffects::modifyExternal, "CharacterController");
        AddUEExtern<DAS_BIND_FUN(CharacterControlsReady)>(*this, Library, "__ue_character_controls_ready",
                                                          das::SideEffects::modifyExternal, "CharacterControlsReady");
        AddUEExtern<DAS_BIND_FUN(CharacterPossess)>(*this, Library, "__ue_pawn_possess",
                                                    das::SideEffects::modifyExternal, "CharacterPossess");
        // Scalar property accessors for self, authored and native references (DasScalarAccessors.inl).
#define DAS_REGISTER_SCALAR_ACCESSORS(Suffix, SelfKind, RefKind)                                                       \
    AddUEExtern<DAS_BIND_FUN(GetSelfScalar<SelfKind>)>(*this, Library, "ue_get_self_" #Suffix,                         \
                                                       das::SideEffects::modifyExternal, "GetSelfScalar");             \
    AddUEExtern<DAS_BIND_FUN(SetSelfScalar<SelfKind>)>(*this, Library, "ue_set_self_" #Suffix,                         \
                                                       das::SideEffects::modifyExternal, "SetSelfScalar");             \
    AddUEExtern<DAS_BIND_FUN(AuthoredGetScalar<RefKind>)>(*this, Library, "__ue_authored_get_" #Suffix,                \
                                                          das::SideEffects::modifyExternal, "AuthoredGetScalar");      \
    AddUEExtern<DAS_BIND_FUN(AuthoredSetScalar<RefKind>)>(*this, Library, "__ue_authored_set_" #Suffix,                \
                                                          das::SideEffects::modifyExternal, "AuthoredSetScalar");      \
    AddUEExtern<DAS_BIND_FUN(NativeReferenceGetScalar<RefKind>)>(                                                      \
        *this, Library, "__ue_native_get_" #Suffix, das::SideEffects::modifyExternal, "NativeReferenceGetScalar");     \
    AddUEExtern<DAS_BIND_FUN(NativeReferenceSetScalar<RefKind>)>(                                                      \
        *this, Library, "__ue_native_set_" #Suffix, das::SideEffects::modifyExternal, "NativeReferenceSetScalar");
        DAS_SCALAR_ACCESSOR_KINDS(DAS_REGISTER_SCALAR_ACCESSORS)
        DAS_REFERENCE_ACCESSOR_KINDS(DAS_REGISTER_SCALAR_ACCESSORS)
#undef DAS_REGISTER_SCALAR_ACCESSORS
        AddUEExtern<DAS_BIND_FUN(DSLTextCreate)>(*this, Library, "ue_text", das::SideEffects::modifyExternal,
                                                 "DSLTextCreate");
        AddUEExtern<DAS_BIND_FUN(DSLTextInvariant)>(*this, Library, "ue_text_invariant",
                                                    das::SideEffects::modifyExternal, "DSLTextInvariant");
        AddUEExtern<DAS_BIND_FUN(DSLTextToString)>(*this, Library, "ue_text_to_string",
                                                   das::SideEffects::modifyExternal, "DSLTextToString");
        AddUEExtern<DAS_BIND_FUN(DSLTextNamespace)>(*this, Library, "ue_text_namespace",
                                                    das::SideEffects::modifyExternal, "DSLTextNamespace");
        AddUEExtern<DAS_BIND_FUN(DSLTextKey)>(*this, Library, "ue_text_key", das::SideEffects::modifyExternal,
                                              "DSLTextKey");
        AddUEExtern<DAS_BIND_FUN(DSLTextIsInvariant)>(*this, Library, "ue_text_is_invariant",
                                                      das::SideEffects::modifyExternal, "DSLTextIsInvariant");
        AddUEExtern<DAS_BIND_FUN(NativeReferenceGetobject), das::SimNode_ExtFuncCallAndCopyOrMove>(
            *this, Library, "__ue_native_get_object", das::SideEffects::modifyExternal, "NativeReferenceGetobject");
        AddUEExtern<DAS_BIND_FUN(NativeReferenceSetobject)>(
            *this, Library, "__ue_native_set_object", das::SideEffects::modifyExternal, "NativeReferenceSetobject");
        AddUEExtern<DAS_BIND_FUN(NativeReferenceGetstruct), das::SimNode_ExtFuncCallAndCopyOrMove>(
                        *this, Library, "__ue_native_get_struct", das::SideEffects::modifyExternal,
                        "NativeReferenceGetstruct")
                        ->needCallerStackFrame = true;
        AddUEExtern<DAS_BIND_FUN(NativeReferenceSetstruct)>(
            *this, Library, "__ue_native_set_struct", das::SideEffects::modifyExternal, "NativeReferenceSetstruct");
        AddUEExtern<DAS_BIND_FUN(CharacterMove)>(*this, Library, "__ue_character_move",
                                                 das::SideEffects::modifyExternal, "CharacterMove");
        AddUEExtern<DAS_BIND_FUN(CharacterLook)>(*this, Library, "__ue_character_look",
                                                 das::SideEffects::modifyExternal, "CharacterLook");
        AddUEExtern<DAS_BIND_FUN(CharacterJump)>(*this, Library, "__ue_character_jump",
                                                 das::SideEffects::modifyExternal, "CharacterJump");
        AddUEExtern<DAS_BIND_FUN(ScriptEnhancedAddContext)>(
            *this, Library, "__ue_enhanced_add_context", das::SideEffects::modifyExternal, "ScriptEnhancedAddContext");
        AddUEExtern<DAS_BIND_FUN(ScriptEnhancedRemoveContext)>(*this, Library, "__ue_enhanced_remove_context",
                                                               das::SideEffects::modifyExternal,
                                                               "ScriptEnhancedRemoveContext");
        AddUEExtern<DAS_BIND_FUN(ScriptEnhancedHasContext)>(
            *this, Library, "__ue_enhanced_has_context", das::SideEffects::modifyExternal, "ScriptEnhancedHasContext");
        AddUEExtern<DAS_BIND_FUN(ScriptEnhancedClearContexts)>(*this, Library, "ue_enhanced_clear_contexts",
                                                               das::SideEffects::modifyExternal,
                                                               "ScriptEnhancedClearContexts");
        AddUEExtern<DAS_BIND_FUN(ScriptEnhancedBind)>(*this, Library, "__ue_enhanced_bind",
                                                      das::SideEffects::modifyExternal, "ScriptEnhancedBind");
        AddUEExtern<DAS_BIND_FUN(ScriptEnhancedUnbind)>(*this, Library, "__ue_enhanced_unbind",
                                                        das::SideEffects::modifyExternal, "ScriptEnhancedUnbind");
        AddUEExtern<DAS_BIND_FUN(ScriptEnhancedAvailable)>(*this, Library, "ue_enhanced_is_available",
                                                           das::SideEffects::modifyExternal, "ScriptEnhancedAvailable");
        AddUEExtern<DAS_BIND_FUN(ScriptInputAction)>(*this, Library, "ue_input_bind_action",
                                                     das::SideEffects::modifyExternal, "ScriptInputAction");
        AddUEExtern<DAS_BIND_FUN(ScriptInputAxis)>(*this, Library, "ue_input_bind_axis",
                                                   das::SideEffects::modifyExternal, "ScriptInputAxis");
        AddUEExtern<DAS_BIND_FUN(ScriptInputAvailable)>(*this, Library, "ue_input_is_available",
                                                        das::SideEffects::modifyExternal, "ScriptInputAvailable");
        AddUEExtern<DAS_BIND_FUN(ScriptInputUnbindAction)>(*this, Library, "ue_input_unbind_action",
                                                           das::SideEffects::modifyExternal, "ScriptInputUnbindAction");
        AddUEExtern<DAS_BIND_FUN(ScriptInputUnbindAxis)>(*this, Library, "ue_input_unbind_axis",
                                                         das::SideEffects::modifyExternal, "ScriptInputUnbindAxis");
        AddUEExtern<DAS_BIND_FUN(ScriptInputUnbindAll)>(*this, Library, "ue_input_unbind_all",
                                                        das::SideEffects::modifyExternal, "ScriptInputUnbindAll");
        AddUEExtern<DAS_BIND_FUN(GameplayCheck), das::SimNode_ExtFuncCallAndCopyOrMove>(
                        *this, Library, "__ue_tag_check", das::SideEffects::modifyExternal, "GameplayCheck")
                        ->needCallerStackFrame = true;
        AddUEExtern<DAS_BIND_FUN(GameplayRequest), das::SimNode_ExtFuncCallAndCopyOrMove>(
                        *this, Library, "__ue_tag_request", das::SideEffects::modifyExternal, "GameplayRequest")
                        ->needCallerStackFrame = true;
        AddUEExtern<DAS_BIND_FUN(GameplayValid)>(*this, Library, "__ue_tag_valid", das::SideEffects::modifyExternal,
                                                 "GameplayValid");
        AddUEExtern<DAS_BIND_FUN(GameplayName)>(*this, Library, "__ue_tag_name", das::SideEffects::modifyExternal,
                                                "GameplayName");
        AddUEExtern<DAS_BIND_FUN(GameplayMatches)>(*this, Library, "__ue_tag_matches", das::SideEffects::modifyExternal,
                                                   "GameplayMatches");
        AddUEExtern<DAS_BIND_FUN(GameplayWith), das::SimNode_ExtFuncCallAndCopyOrMove>(
                        *this, Library, "__ue_tags_with", das::SideEffects::modifyExternal, "GameplayWith")
                        ->needCallerStackFrame = true;
        AddUEExtern<DAS_BIND_FUN(GameplayCount)>(*this, Library, "__ue_tags_count", das::SideEffects::modifyExternal,
                                                 "GameplayCount");
        AddUEExtern<DAS_BIND_FUN(GameplayHas)>(*this, Library, "__ue_tags_has", das::SideEffects::modifyExternal,
                                               "GameplayHas");
        AddUEExtern<DAS_BIND_FUN(GameplayHasSet)>(*this, Library, "__ue_tags_has_set", das::SideEffects::modifyExternal,
                                                  "GameplayHasSet");
        AddUEExtern<DAS_BIND_FUN(GameplayNames), das::SimNode_ExtFuncCallAndCopyOrMove>(
                        *this, Library, "__ue_tags_names", das::SideEffects::modifyExternal, "GameplayNames")
                        ->needCallerStackFrame = true;
        AddUEExtern<DAS_BIND_FUN(ScriptCurveTableHas)>(*this, Library, "__ue_curve_table_has",
                                                       das::SideEffects::accessExternal, "ScriptCurveTableHas");
        AddUEExtern<DAS_BIND_FUN(ScriptCurveTableNames), das::SimNode_ExtFuncCallAndCopyOrMove>(
                        *this, Library, "__ue_curve_table_names", das::SideEffects::modifyExternal,
                        "ScriptCurveTableNames")
                        ->needCallerStackFrame = true;
        AddUEExtern<DAS_BIND_FUN(ScriptCurveTableEval)>(*this, Library, "__ue_curve_table_eval",
                                                        das::SideEffects::accessExternal, "ScriptCurveTableEval");
        AddUEExtern<DAS_BIND_FUN(ScriptDataTableHas)>(*this, Library, "__ue_data_table_has",
                                                      das::SideEffects::accessExternal, "ScriptDataTableHas");
        AddUEExtern<DAS_BIND_FUN(ScriptDataTableNames), das::SimNode_ExtFuncCallAndCopyOrMove>(
                        *this, Library, "__ue_data_table_names", das::SideEffects::modifyExternal,
                        "ScriptDataTableNames")
                        ->needCallerStackFrame = true;
        AddUEExtern<DAS_BIND_FUN(ScriptDataTableRow), das::SimNode_ExtFuncCallAndCopyOrMove>(
                        *this, Library, "__ue_data_table_row", das::SideEffects::modifyExternal, "ScriptDataTableRow")
                        ->needCallerStackFrame = true;
        AddUEExtern<DAS_BIND_FUN(ScriptPrimaryFind)>(*this, Library, "__ue_primary_find",
                                                     das::SideEffects::modifyExternal, "ScriptPrimaryFind");
        AddUEExtern<DAS_BIND_FUN(ScriptPrimaryQueryCount)>(*this, Library, "__ue_primary_query_count",
                                                           das::SideEffects::modifyExternal, "ScriptPrimaryQueryCount");
        AddUEExtern<DAS_BIND_FUN(ScriptPrimaryQueryName)>(*this, Library, "__ue_primary_query_name",
                                                          das::SideEffects::modifyExternal, "ScriptPrimaryQueryName");
        AddUEExtern<DAS_BIND_FUN(ScriptPrimaryQueryRelease)>(*this, Library, "__ue_primary_query_release",
                                                             das::SideEffects::modifyExternal,
                                                             "ScriptPrimaryQueryRelease");
        AddUEExtern<DAS_BIND_FUN(ScriptPrimaryExists)>(*this, Library, "__ue_primary_exists",
                                                       das::SideEffects::modifyExternal, "ScriptPrimaryExists");
        AddUEExtern<DAS_BIND_FUN(ScriptPrimaryPath)>(*this, Library, "__ue_primary_path",
                                                     das::SideEffects::modifyExternal, "ScriptPrimaryPath");
        AddUEExtern<DAS_BIND_FUN(ScriptPrimaryObject), das::SimNode_ExtFuncCallAndCopyOrMove>(
            *this, Library, "__ue_primary_object", das::SideEffects::modifyExternal, "ScriptPrimaryObject");
        AddUEExtern<DAS_BIND_FUN(ScriptPrimaryLoad)>(*this, Library, "__ue_primary_load_async",
                                                     das::SideEffects::modifyExternal, "ScriptPrimaryLoad");
        AddUEExtern<DAS_BIND_FUN(ScriptPrimaryPending)>(*this, Library, "__ue_primary_pending",
                                                        das::SideEffects::modifyExternal, "ScriptPrimaryPending");
        AddUEExtern<DAS_BIND_FUN(ScriptPrimaryCancel)>(*this, Library, "__ue_primary_cancel",
                                                       das::SideEffects::modifyExternal, "ScriptPrimaryCancel");
        AddUEExtern<DAS_BIND_FUN(ScriptPrimaryUnload)>(*this, Library, "__ue_primary_unload",
                                                       das::SideEffects::modifyExternal, "ScriptPrimaryUnload");
        AddUEExtern<DAS_BIND_FUN(ScriptSaveGameCreate), das::SimNode_ExtFuncCallAndCopyOrMove>(
            *this, Library, "__ue_savegame_create", das::SideEffects::modifyExternal, "ScriptSaveGameCreate");
        AddUEExtern<DAS_BIND_FUN(ScriptSaveGameCreatePath), das::SimNode_ExtFuncCallAndCopyOrMove>(
            *this, Library, "__ue_savegame_create_path", das::SideEffects::modifyExternal, "ScriptSaveGameCreatePath");
        AddUEExtern<DAS_BIND_FUN(ScriptSaveGameSave)>(*this, Library, "__ue_savegame_save",
                                                      das::SideEffects::modifyExternal, "ScriptSaveGameSave");
        AddUEExtern<DAS_BIND_FUN(ScriptSaveGameLoad), das::SimNode_ExtFuncCallAndCopyOrMove>(
            *this, Library, "__ue_savegame_load", das::SideEffects::modifyExternal, "ScriptSaveGameLoad");
        AddUEExtern<DAS_BIND_FUN(ScriptSaveGameExists)>(*this, Library, "__ue_savegame_exists",
                                                        das::SideEffects::modifyExternal, "ScriptSaveGameExists");
        AddUEExtern<DAS_BIND_FUN(ScriptSaveGameDelete)>(*this, Library, "__ue_savegame_delete",
                                                        das::SideEffects::modifyExternal, "ScriptSaveGameDelete");
        AddUEExtern<DAS_BIND_FUN(ScriptSaveGameRelease)>(*this, Library, "__ue_savegame_release",
                                                         das::SideEffects::modifyExternal, "ScriptSaveGameRelease");
        AddUEExtern<DAS_BIND_FUN(ScriptSaveGameValid)>(*this, Library, "__ue_savegame_valid",
                                                       das::SideEffects::modifyExternal, "ScriptSaveGameValid");
        AddUEExtern<DAS_BIND_FUN(ScriptSaveGameSaveAsync)>(*this, Library, "__ue_savegame_save_async",
                                                           das::SideEffects::modifyExternal, "ScriptSaveGameSaveAsync");
        AddUEExtern<DAS_BIND_FUN(ScriptSaveGameLoadAsync)>(*this, Library, "__ue_savegame_load_async",
                                                           das::SideEffects::modifyExternal, "ScriptSaveGameLoadAsync");
        AddUEExtern<DAS_BIND_FUN(ScriptSaveGameCancel)>(*this, Library, "__ue_savegame_cancel",
                                                        das::SideEffects::modifyExternal, "ScriptSaveGameCancel");
        AddUEExtern<DAS_BIND_FUN(ScriptSaveGamePending)>(*this, Library, "__ue_savegame_pending",
                                                         das::SideEffects::modifyExternal, "ScriptSaveGamePending");
        AddUEExtern<DAS_BIND_FUN(ScriptAssetLoad)>(*this, Library, "__ue_asset_load_async",
                                                   das::SideEffects::modifyExternal, "ScriptAssetLoad");
        AddUEExtern<DAS_BIND_FUN(SoftMake), das::SimNode_ExtFuncCallAndCopyOrMove>(
            *this, Library, "__ue_soft_make", das::SideEffects::modifyExternal, "SoftMake");
        AddUEExtern<DAS_BIND_FUN(SoftUpcast), das::SimNode_ExtFuncCallAndCopyOrMove>(
            *this, Library, "__ue_soft_upcast", das::SideEffects::modifyExternal, "SoftUpcast");
        AddUEExtern<DAS_BIND_FUN(SoftCheck), das::SimNode_ExtFuncCallAndCopyOrMove>(
            *this, Library, "__ue_soft_check", das::SideEffects::modifyExternal, "SoftCheck");
        AddUEExtern<DAS_BIND_FUN(SoftGetObject), das::SimNode_ExtFuncCallAndCopyOrMove>(
            *this, Library, "__ue_soft_get_object", das::SideEffects::modifyExternal, "SoftGetObject");
        AddUEExtern<DAS_BIND_FUN(SoftGetClass)>(*this, Library, "__ue_soft_get_class", das::SideEffects::modifyExternal,
                                                "SoftGetClass");
        AddUEExtern<DAS_BIND_FUN(SoftPath)>(*this, Library, "__ue_soft_path", das::SideEffects::modifyExternal,
                                            "SoftPath");
        AddUEExtern<DAS_BIND_FUN(SoftNull)>(*this, Library, "__ue_soft_null", das::SideEffects::modifyExternal,
                                            "SoftNull");
        AddUEExtern<DAS_BIND_FUN(SoftLoaded)>(*this, Library, "__ue_soft_loaded", das::SideEffects::modifyExternal,
                                              "SoftLoaded");
        AddUEExtern<DAS_BIND_FUN(ScriptSoftAssetLoad)>(*this, Library, "__ue_soft_load_async",
                                                       das::SideEffects::modifyExternal, "ScriptSoftAssetLoad");
        AddUEExtern<DAS_BIND_FUN(ScriptAssetLoadGeneric)>(*this, Library, "ue_asset_load_async",
                                                          das::SideEffects::modifyExternal, "ScriptAssetLoadGeneric");
        AddUEExtern<DAS_BIND_FUN(ScriptAssetCancel)>(*this, Library, "ue_asset_cancel",
                                                     das::SideEffects::modifyExternal, "ScriptAssetCancel");
        AddUEExtern<DAS_BIND_FUN(ScriptAssetCancelAll)>(*this, Library, "ue_asset_cancel_all",
                                                        das::SideEffects::modifyExternal, "ScriptAssetCancelAll");
        AddUEExtern<DAS_BIND_FUN(ScriptAssetPending)>(*this, Library, "ue_asset_is_pending",
                                                      das::SideEffects::modifyExternal, "ScriptAssetPending");
        AddUEExtern<DAS_BIND_FUN(ScriptAISelf), das::SimNode_ExtFuncCallAndCopyOrMove>(
            *this, Library, "__ue_ai_self", das::SideEffects::modifyExternal, "ScriptAISelf");
        AddUEExtern<DAS_BIND_FUN(ScriptAIMove)>(*this, Library, "__ue_ai_move", das::SideEffects::modifyExternal,
                                                "ScriptAIMove");
        AddUEExtern<DAS_BIND_FUN(ScriptAICancel)>(*this, Library, "__ue_ai_cancel", das::SideEffects::modifyExternal,
                                                  "ScriptAICancel");
        AddUEExtern<DAS_BIND_FUN(ScriptAIActive)>(*this, Library, "__ue_ai_active", das::SideEffects::modifyExternal,
                                                  "ScriptAIActive");
        AddUEExtern<DAS_BIND_FUN(ScriptEffectPlay)>(*this, Library, "__ue_effect_play",
                                                    das::SideEffects::modifyExternal, "ScriptEffectPlay");
        AddUEExtern<DAS_BIND_FUN(ScriptEffectStop)>(*this, Library, "__ue_effect_stop",
                                                    das::SideEffects::modifyExternal, "ScriptEffectStop");
        AddUEExtern<DAS_BIND_FUN(ScriptEffectStopAll)>(*this, Library, "__ue_effect_stop_all",
                                                       das::SideEffects::modifyExternal, "ScriptEffectStopAll");
        AddUEExtern<DAS_BIND_FUN(ScriptEffectActive)>(*this, Library, "__ue_effect_active",
                                                      das::SideEffects::modifyExternal, "ScriptEffectActive");
        AddUEExtern<DAS_BIND_FUN(ScriptMontagePlay)>(*this, Library, "__ue_montage_play",
                                                     das::SideEffects::modifyExternal, "ScriptMontagePlay");
        AddUEExtern<DAS_BIND_FUN(ScriptMontageStop)>(*this, Library, "__ue_montage_stop",
                                                     das::SideEffects::modifyExternal, "ScriptMontageStop");
        AddUEExtern<DAS_BIND_FUN(ScriptMontageCancel)>(*this, Library, "__ue_montage_cancel",
                                                       das::SideEffects::modifyExternal, "ScriptMontageCancel");
        AddUEExtern<DAS_BIND_FUN(ScriptMontagePlaying)>(*this, Library, "__ue_montage_playing",
                                                        das::SideEffects::modifyExternal, "ScriptMontagePlaying");
        AddUEExtern<DAS_BIND_FUN(ScriptLatentDelay)>(*this, Library, "ue_latent_delay",
                                                     das::SideEffects::modifyExternal, "ScriptLatentDelay");
        AddUEExtern<DAS_BIND_FUN(ScriptLatentCancel)>(*this, Library, "ue_latent_cancel",
                                                      das::SideEffects::modifyExternal, "ScriptLatentCancel");
        AddUEExtern<DAS_BIND_FUN(ScriptLatentCancelAll)>(*this, Library, "ue_latent_cancel_all",
                                                         das::SideEffects::modifyExternal, "ScriptLatentCancelAll");
        AddUEExtern<DAS_BIND_FUN(ScriptLatentPending)>(*this, Library, "ue_latent_is_pending",
                                                       das::SideEffects::modifyExternal, "ScriptLatentPending");
        AddUEExtern<DAS_BIND_FUN(ScriptTimerStart)>(*this, Library, "ue_timer_start", das::SideEffects::modifyExternal,
                                                    "ScriptTimerStart");
        AddUEExtern<DAS_BIND_FUN(ScriptTimerStartVector2d)>(
            *this, Library, "ue_timer_start", das::SideEffects::modifyExternal, "ScriptTimerStartVector2d");
        AddUEExtern<DAS_BIND_FUN(ScriptTimerNextTickVector2d)>(
            *this, Library, "ue_timer_next_tick", das::SideEffects::modifyExternal, "ScriptTimerNextTickVector2d");
        AddUEExtern<DAS_BIND_FUN(ScriptTimerStartClass)>(*this, Library, "__ue_timer_start_class",
                                                         das::SideEffects::modifyExternal, "ScriptTimerStartClass");
        AddUEExtern<DAS_BIND_FUN(ScriptTimerNextTickClass)>(
            *this, Library, "__ue_timer_next_tick_class", das::SideEffects::modifyExternal, "ScriptTimerNextTickClass");
        AddUEExtern<DAS_BIND_FUN(ScriptTimerStartObject)>(*this, Library, "__ue_timer_start_object",
                                                          das::SideEffects::modifyExternal, "ScriptTimerStartObject");
        AddUEExtern<DAS_BIND_FUN(ScriptTimerNextTickObject)>(*this, Library, "__ue_timer_next_tick_object",
                                                             das::SideEffects::modifyExternal,
                                                             "ScriptTimerNextTickObject");
        AddUEExtern<DAS_BIND_FUN(ScriptTimerStartArray)>(*this, Library, "__ue_timer_start_array",
                                                         das::SideEffects::modifyExternal, "ScriptTimerStartArray");
        AddUEExtern<DAS_BIND_FUN(ScriptTimerStartEnum)>(*this, Library, "ue_timer_start",
                                                        das::SideEffects::modifyExternal, "ScriptTimerStartEnum");
        AddUEExtern<DAS_BIND_FUN(ScriptTimerNextTickEnum)>(*this, Library, "ue_timer_next_tick",
                                                           das::SideEffects::modifyExternal, "ScriptTimerNextTickEnum");
        // Scalar timer payloads (DasScalarAccessors.inl).
#define DAS_REGISTER_TIMER_SCALARS(Kind)                                                                               \
    AddUEExtern<DAS_BIND_FUN(ScriptTimerStartKind<Kind>)>(*this, Library, "ue_timer_start",                            \
                                                          das::SideEffects::modifyExternal, "ScriptTimerStartKind");   \
    AddUEExtern<DAS_BIND_FUN(ScriptTimerNextTickKind<Kind>)>(                                                          \
        *this, Library, "ue_timer_next_tick", das::SideEffects::modifyExternal, "ScriptTimerNextTickKind");
        DAS_REGISTER_TIMER_SCALARS(FDasKindInt)
        DAS_REGISTER_TIMER_SCALARS(FDasKindFloat)
        DAS_REGISTER_TIMER_SCALARS(FDasKindBool)
        DAS_REGISTER_TIMER_SCALARS(FDasKindDouble)
        DAS_REGISTER_TIMER_SCALARS(FDasKindString)
        DAS_REGISTER_TIMER_SCALARS(FDasKindName)
        DAS_REGISTER_TIMER_SCALARS(FDasKindByte)
        DAS_REGISTER_TIMER_SCALARS(FDasKindText)
#undef DAS_REGISTER_TIMER_SCALARS
        AddUEExtern<DAS_BIND_FUN(ScriptTimerStartVector)>(*this, Library, "ue_timer_start",
                                                          das::SideEffects::modifyExternal, "ScriptTimerStartVector");
        AddUEExtern<DAS_BIND_FUN(ScriptTimerStartRotator)>(*this, Library, "ue_timer_start",
                                                           das::SideEffects::modifyExternal, "ScriptTimerStartRotator");
        AddUEExtern<DAS_BIND_FUN(ScriptTimerStartTransform)>(
            *this, Library, "ue_timer_start", das::SideEffects::modifyExternal, "ScriptTimerStartTransform");
        AddUEExtern<DAS_BIND_FUN(ScriptTimerNextTick)>(*this, Library, "ue_timer_next_tick",
                                                       das::SideEffects::modifyExternal, "ScriptTimerNextTick");
        AddUEExtern<DAS_BIND_FUN(ScriptTimerNextTickArray)>(
            *this, Library, "__ue_timer_next_tick_array", das::SideEffects::modifyExternal, "ScriptTimerNextTickArray");
        AddUEExtern<DAS_BIND_FUN(ScriptTimerPaused)>(*this, Library, "ue_timer_is_paused",
                                                     das::SideEffects::modifyExternal, "ScriptTimerPaused");
        AddUEExtern<DAS_BIND_FUN(ScriptTimerElapsed)>(*this, Library, "ue_timer_elapsed",
                                                      das::SideEffects::modifyExternal, "ScriptTimerElapsed");
        AddUEExtern<DAS_BIND_FUN(ScriptTimerCancelAll)>(*this, Library, "ue_timer_cancel_all",
                                                        das::SideEffects::modifyExternal, "ScriptTimerCancelAll");
        AddUEExtern<DAS_BIND_FUN(ScriptTimerNextTickVector)>(
            *this, Library, "ue_timer_next_tick", das::SideEffects::modifyExternal, "ScriptTimerNextTickVector");
        AddUEExtern<DAS_BIND_FUN(ScriptTimerNextTickRotator)>(
            *this, Library, "ue_timer_next_tick", das::SideEffects::modifyExternal, "ScriptTimerNextTickRotator");
        AddUEExtern<DAS_BIND_FUN(ScriptTimerNextTickTransform)>(
            *this, Library, "ue_timer_next_tick", das::SideEffects::modifyExternal, "ScriptTimerNextTickTransform");
        AddUEExtern<DAS_BIND_FUN(ScriptTimerCancel)>(*this, Library, "ue_timer_cancel",
                                                     das::SideEffects::modifyExternal, "ScriptTimerCancel");
        AddUEExtern<DAS_BIND_FUN(ScriptTimerPause)>(*this, Library, "ue_timer_pause", das::SideEffects::modifyExternal,
                                                    "ScriptTimerPause");
        AddUEExtern<DAS_BIND_FUN(ScriptTimerResume)>(*this, Library, "ue_timer_resume",
                                                     das::SideEffects::modifyExternal, "ScriptTimerResume");
        AddUEExtern<DAS_BIND_FUN(ScriptTimerActive)>(*this, Library, "ue_timer_is_active",
                                                     das::SideEffects::modifyExternal, "ScriptTimerActive");
        AddUEExtern<DAS_BIND_FUN(ScriptTimerRemaining)>(*this, Library, "ue_timer_remaining",
                                                        das::SideEffects::modifyExternal, "ScriptTimerRemaining");

        AddUEExtern<DAS_BIND_FUN(ObjectCastNullable), das::SimNode_ExtFuncCallAndCopyOrMove>(
            *this, Library, "ue_cast_nullable_object", das::SideEffects::modifyExternal, "ObjectCastNullable");

        AddUEExtern<DAS_BIND_FUN(BeginCallbackFrame), das::SimNode_ExtFuncCallAndCopyOrMove>(
            *this, Library, "__ue_begin_callback_frame", das::SideEffects::modifyExternal, "BeginCallbackFrame");
        AddUEExtern<DAS_BIND_FUN(StartCallbackFrameTimer)>(*this, Library, "__ue_timer_start_frame",
                                                           das::SideEffects::modifyExternal, "StartCallbackFrameTimer");
        AddUEExtern<DAS_BIND_FUN(NextCallbackFrameTimer)>(*this, Library, "__ue_timer_next_tick_frame",
                                                          das::SideEffects::modifyExternal, "NextCallbackFrameTimer");
        AddUEExtern<DAS_BIND_FUN(BeginNativeGameplayCall), das::SimNode_ExtFuncCallAndCopyOrMove>(
            *this, Library, "__ue_begin_native_gameplay_call", das::SideEffects::modifyExternal,
            "BeginNativeGameplayCall");
        AddUEExtern<DAS_BIND_FUN(ArgumentNativeDelegate)>(*this, Library, "__ue_arg_native_delegate",
                                                          das::SideEffects::modifyExternal, "ArgumentNativeDelegate");
        AddUEExtern<DAS_BIND_FUN(DelegateGetField)>(*this, Library, "__ue_delegate_get_field",
                                                    das::SideEffects::modifyExternal, "DelegateGetField");
        AddUEExtern<DAS_BIND_FUN(DelegateSetField)>(*this, Library, "__ue_delegate_set_field",
                                                    das::SideEffects::modifyExternal, "DelegateSetField");
        AddUEExtern<DAS_BIND_FUN(DelegateCreate)>(*this, Library, "__ue_delegate_create",
                                                  das::SideEffects::modifyExternal, "DelegateCreate");
        AddUEExtern<DAS_BIND_FUN(DynamicEventListen)>(*this, Library, "__ue_dynamic_event",
                                                      das::SideEffects::modifyExternal, "DynamicEventListen");
        AddUEExtern<DAS_BIND_FUN(DelegateSelfTarget), das::SimNode_ExtFuncCallAndCopyOrMove>(
            *this, Library, "__ue_delegate_self_target", das::SideEffects::modifyExternal, "DelegateSelfTarget");
        AddUEExtern<DAS_BIND_FUN(DelegateBindScript)>(*this, Library, "__ue_delegate_bind_script",
                                                      das::SideEffects::modifyExternal, "DelegateBindScript");
        AddUEExtern<DAS_BIND_FUN(DelegateBind)>(*this, Library, "__ue_delegate_bind", das::SideEffects::modifyExternal,
                                                "DelegateBind");
        AddUEExtern<DAS_BIND_FUN(DelegateRemove)>(*this, Library, "__ue_delegate_remove",
                                                  das::SideEffects::modifyExternal, "DelegateRemove");
        AddUEExtern<DAS_BIND_FUN(DelegateClear)>(*this, Library, "__ue_delegate_clear",
                                                 das::SideEffects::modifyExternal, "DelegateClear");
        AddUEExtern<DAS_BIND_FUN(DelegateIsBound)>(*this, Library, "__ue_delegate_is_bound",
                                                   das::SideEffects::modifyExternal, "DelegateIsBound");
        AddUEExtern<DAS_BIND_FUN(DelegateEventView)>(*this, Library, "__ue_delegate_event_view",
                                                     das::SideEffects::modifyExternal, "DelegateEventView");
        AddUEExtern<DAS_BIND_FUN(BeginDelegateCall), das::SimNode_ExtFuncCallAndCopyOrMove>(
            *this, Library, "__ue_delegate_begin_call", das::SideEffects::modifyExternal, "BeginDelegateCall");

        AddUEExtern<DAS_BIND_FUN(ExpectCallSignature)>(*this, Library, "ue_expect_signature",
                                                       das::SideEffects::modifyExternal, "ExpectCallSignature");
        AddUEExtern<DAS_BIND_FUN(MethodArgdelegate)>(*this, Library, "__ue_method_arg_delegate",
                                                     das::SideEffects::modifyExternal, "MethodArgdelegate");
        AddUEExtern<DAS_BIND_FUN(MethodResultdelegate)>(*this, Library, "__ue_method_result_delegate",
                                                        das::SideEffects::modifyExternal, "MethodResultdelegate");
        AddUEExtern<DAS_BIND_FUN(GetCallEnum)>(*this, Library, "ue_result_enum", das::SideEffects::modifyExternal,
                                               "GetCallEnum");
        AddUEExtern<DAS_BIND_FUN(SetCallEnum)>(*this, Library, "ue_arg_enum", das::SideEffects::modifyExternal,
                                               "SetCallEnum");
        AddUEExtern<DAS_BIND_FUN(GetCallName)>(*this, Library, "ue_result_name", das::SideEffects::modifyExternal,
                                               "GetCallName");
        AddUEExtern<DAS_BIND_FUN(SetCallName)>(*this, Library, "ue_arg_name", das::SideEffects::modifyExternal,
                                               "SetCallName");
        AddUEExtern<DAS_BIND_FUN(SetCallClass)>(*this, Library, "ue_arg_class", das::SideEffects::modifyExternal,
                                                "SetCallClass");
        AddUEExtern<DAS_BIND_FUN(GetCallClass)>(*this, Library, "ue_result_class", das::SideEffects::modifyExternal,
                                                "GetCallClass");
        AddUEExtern<DAS_BIND_FUN(CheckTypedClass)>(*this, Library, "__ue_class_check", das::SideEffects::accessExternal,
                                                   "CheckTypedClass");
        AddUEExtern<DAS_BIND_FUN(NullClass)>(*this, Library, "ue_class_null", das::SideEffects::none, "NullClass");
        AddUEExtern<DAS_BIND_FUN(SetCallObject)>(*this, Library, "ue_arg_object", das::SideEffects::modifyExternal,
                                                 "SetCallObject");
        AddUEExtern<DAS_BIND_FUN(GetCallObject), das::SimNode_ExtFuncCallAndCopyOrMove>(
            *this, Library, "ue_result_object", das::SideEffects::modifyExternal, "GetCallObject");
        AddUEExtern<DAS_BIND_FUN(RequireCallContext)>(*this, Library, "ue_require_call_context",
                                                      das::SideEffects::modifyExternal, "RequireCallContext");
        AddUEExtern<DAS_BIND_FUN(ExpectInterfaceSignature)>(*this, Library, "ue_expect_interface_signature",
                                                            das::SideEffects::modifyExternal,
                                                            "ExpectInterfaceSignature");
        AddUEExtern<DAS_BIND_FUN(BeginInterfaceCall), das::SimNode_ExtFuncCallAndCopyOrMove>(
            *this, Library, "ue_begin_interface_call", das::SideEffects::modifyExternal, "BeginInterfaceCall");

        AddUEExtern<DAS_BIND_FUN(MakeVector2D), das::SimNode_ExtFuncCallAndCopyOrMove>(
            *this, Library, "ue_vector2d", das::SideEffects::none, "MakeVector2D");
        AddUEExtern<DAS_BIND_FUN(MathPackvector2d), das::SimNode_ExtFuncCallAndCopyOrMove>(
                        *this, Library, "__ue_math_pack_vector2d", das::SideEffects::modifyExternal, "MathPackvector2d")
                        ->needCallerStackFrame = true;
        AddUEExtern<DAS_BIND_FUN(MathUnpackvector2d), das::SimNode_ExtFuncCallAndCopyOrMove>(
            *this, Library, "__ue_math_unpack_vector2d", das::SideEffects::modifyExternal, "MathUnpackvector2d");
        AddUEExtern<DAS_BIND_FUN(MathPackvector), das::SimNode_ExtFuncCallAndCopyOrMove>(
                        *this, Library, "__ue_math_pack_vector", das::SideEffects::modifyExternal, "MathPackvector")
                        ->needCallerStackFrame = true;
        AddUEExtern<DAS_BIND_FUN(MathUnpackvector), das::SimNode_ExtFuncCallAndCopyOrMove>(
            *this, Library, "__ue_math_unpack_vector", das::SideEffects::modifyExternal, "MathUnpackvector");
        AddUEExtern<DAS_BIND_FUN(MathPackrotator), das::SimNode_ExtFuncCallAndCopyOrMove>(
                        *this, Library, "__ue_math_pack_rotator", das::SideEffects::modifyExternal, "MathPackrotator")
                        ->needCallerStackFrame = true;
        AddUEExtern<DAS_BIND_FUN(MathUnpackrotator), das::SimNode_ExtFuncCallAndCopyOrMove>(
            *this, Library, "__ue_math_unpack_rotator", das::SideEffects::modifyExternal, "MathUnpackrotator");
        AddUEExtern<DAS_BIND_FUN(MathPacktransform), das::SimNode_ExtFuncCallAndCopyOrMove>(
                        *this, Library, "__ue_math_pack_transform", das::SideEffects::modifyExternal,
                        "MathPacktransform")
                        ->needCallerStackFrame = true;
        AddUEExtern<DAS_BIND_FUN(MathUnpacktransform), das::SimNode_ExtFuncCallAndCopyOrMove>(
            *this, Library, "__ue_math_unpack_transform", das::SideEffects::modifyExternal, "MathUnpacktransform");
        AddUEExtern<DAS_BIND_FUN(MethodArgvector2d)>(*this, Library, "__ue_method_arg_vector2d",
                                                     das::SideEffects::modifyExternal, "MethodArgvector2d");
        AddUEExtern<DAS_BIND_FUN(MethodResultvector2d), das::SimNode_ExtFuncCallAndCopyOrMove>(
            *this, Library, "__ue_method_result_vector2d", das::SideEffects::modifyExternal, "MethodResultvector2d");
        AddUEExtern<DAS_BIND_FUN(MethodArgvector)>(*this, Library, "__ue_method_arg_vector",
                                                   das::SideEffects::modifyExternal, "MethodArgvector");
        AddUEExtern<DAS_BIND_FUN(MethodResultvector), das::SimNode_ExtFuncCallAndCopyOrMove>(
            *this, Library, "__ue_method_result_vector", das::SideEffects::modifyExternal, "MethodResultvector");
        AddUEExtern<DAS_BIND_FUN(MethodArgrotator)>(*this, Library, "__ue_method_arg_rotator",
                                                    das::SideEffects::modifyExternal, "MethodArgrotator");
        AddUEExtern<DAS_BIND_FUN(MethodResultrotator), das::SimNode_ExtFuncCallAndCopyOrMove>(
            *this, Library, "__ue_method_result_rotator", das::SideEffects::modifyExternal, "MethodResultrotator");
        AddUEExtern<DAS_BIND_FUN(MethodArgtransform)>(*this, Library, "__ue_method_arg_transform",
                                                      das::SideEffects::modifyExternal, "MethodArgtransform");
        AddUEExtern<DAS_BIND_FUN(MethodResulttransform), das::SimNode_ExtFuncCallAndCopyOrMove>(
            *this, Library, "__ue_method_result_transform", das::SideEffects::modifyExternal, "MethodResulttransform");
        AddUEExtern<DAS_BIND_FUN(ReleaseAuthoredCall)>(*this, Library, "__ue_release_authored_call",
                                                       das::SideEffects::modifyExternal, "ReleaseAuthoredCall");
        AddUEExtern<DAS_BIND_FUN(InvokeParentLifecycle)>(*this, Library, "__ue_parent_lifecycle",
                                                         das::SideEffects::modifyExternal, "InvokeParentLifecycle");
        AddUEExtern<DAS_BIND_FUN(BeginAuthoredSuperCall), das::SimNode_ExtFuncCallAndCopyOrMove>(
            *this, Library, "__ue_begin_super_call", das::SideEffects::modifyExternal, "BeginAuthoredSuperCall");
        AddUEExtern<DAS_BIND_FUN(BeginAuthoredCall), das::SimNode_ExtFuncCallAndCopyOrMove>(
            *this, Library, "__ue_begin_authored_call", das::SideEffects::modifyExternal, "BeginAuthoredCall");
        // Struct leaf and call frame scalars (DasScalarAccessors.inl).
#define DAS_REGISTER_STRUCT_SCALARS(Suffix, SelfKind, RefKind)                                                         \
    AddUEExtern<DAS_BIND_FUN(StructGetScalar<SelfKind>)>(*this, Library, "ue_struct_get_" #Suffix,                     \
                                                         das::SideEffects::modifyExternal, "StructGetScalar");         \
    AddUEExtern<DAS_BIND_FUN(StructWithScalar<SelfKind>), das::SimNode_ExtFuncCallAndCopyOrMove>(                      \
                    *this, Library, "ue_struct_with_" #Suffix, das::SideEffects::modifyExternal, "StructWithScalar")   \
                    ->needCallerStackFrame = true;
        DAS_SCALAR_ACCESSOR_KINDS(DAS_REGISTER_STRUCT_SCALARS)
#undef DAS_REGISTER_STRUCT_SCALARS
#define DAS_REGISTER_SELF_STRUCT_MEMBERS(Suffix, SelfKind, RefKind)                                                    \
    AddUEExtern<DAS_BIND_FUN(GetSelfStructMember<SelfKind>)>(*this, Library, "__ue_self_struct_get_" #Suffix,          \
                                                             das::SideEffects::modifyExternal, "GetSelfStructMember"); \
    AddUEExtern<DAS_BIND_FUN(SetSelfStructMember<SelfKind>)>(*this, Library, "__ue_self_struct_set_" #Suffix,          \
                                                             das::SideEffects::modifyExternal, "SetSelfStructMember")  \
                    ->needCallerStackFrame = true;
        DAS_SCALAR_ACCESSOR_KINDS(DAS_REGISTER_SELF_STRUCT_MEMBERS)
#undef DAS_REGISTER_SELF_STRUCT_MEMBERS
        // In-place self containers with scalar elements (DasScalarAccessors.inl).
#define DAS_REGISTER_SELF_ARRAY(Suffix, SelfKind, RefKind)                                                             \
    AddUEExtern<DAS_BIND_FUN(SelfArrayGet<SelfKind>)>(*this, Library, "__ue_self_array_get_" #Suffix,                  \
                                                      das::SideEffects::modifyExternal, "SelfArrayGet");               \
    AddUEExtern<DAS_BIND_FUN(SelfArraySet<SelfKind>)>(*this, Library, "__ue_self_array_set_" #Suffix,                  \
                                                      das::SideEffects::modifyExternal, "SelfArraySet");               \
    AddUEExtern<DAS_BIND_FUN(SelfArrayPush<SelfKind>)>(*this, Library, "__ue_self_array_push_" #Suffix,                \
                                                       das::SideEffects::modifyExternal, "SelfArrayPush");             \
    AddUEExtern<DAS_BIND_FUN(SelfArrayInsert<SelfKind>)>(*this, Library, "__ue_self_array_insert_" #Suffix,            \
                                                         das::SideEffects::modifyExternal, "SelfArrayInsert");
        DAS_SCALAR_ACCESSOR_KINDS(DAS_REGISTER_SELF_ARRAY)
#undef DAS_REGISTER_SELF_ARRAY
        AddUEExtern<DAS_BIND_FUN(SelfArrayPop)>(*this, Library, "__ue_self_array_pop", das::SideEffects::modifyExternal,
                                                "SelfArrayPop");
        AddUEExtern<DAS_BIND_FUN(SelfArrayErase)>(*this, Library, "__ue_self_array_erase",
                                                  das::SideEffects::modifyExternal, "SelfArrayErase");
        AddUEExtern<DAS_BIND_FUN(SelfContainerClear)>(*this, Library, "__ue_self_container_clear",
                                                      das::SideEffects::modifyExternal, "SelfContainerClear");
#define DAS_REGISTER_SELF_KEYS(Key, KeyKind)                                                                           \
    AddUEExtern<DAS_BIND_FUN(SelfMapErase<KeyKind>)>(*this, Library, "__ue_self_map_erase_" #Key,                      \
                                                     das::SideEffects::modifyExternal, "SelfMapErase");                \
    AddUEExtern<DAS_BIND_FUN(SelfSetInsert<KeyKind>)>(*this, Library, "__ue_self_set_insert_" #Key,                    \
                                                      das::SideEffects::modifyExternal, "SelfSetInsert");              \
    AddUEExtern<DAS_BIND_FUN(SelfSetErase<KeyKind>)>(*this, Library, "__ue_self_set_erase_" #Key,                      \
                                                     das::SideEffects::modifyExternal, "SelfSetErase");                \
    AddSelfMapAccessors<KeyKind DAS_SCALAR_ACCESSOR_KINDS(DAS_SCALAR_KIND_TYPE)>(*this, Library, #Key);
        DAS_CONTAINER_KEY_KINDS(DAS_REGISTER_SELF_KEYS)
#undef DAS_REGISTER_SELF_KEYS
#define DAS_REGISTER_CALL_SCALARS(ArgName, ResultName, Kind)                                                           \
    AddUEExtern<DAS_BIND_FUN(SetCallKind<Kind>)>(*this, Library, ArgName, das::SideEffects::modifyExternal,            \
                                                 "SetCallKind");                                                       \
    AddUEExtern<DAS_BIND_FUN(GetCallKind<Kind>)>(*this, Library, ResultName, das::SideEffects::modifyExternal,         \
                                                 "GetCallKind");
        DAS_CALL_SCALAR_KINDS(DAS_REGISTER_CALL_SCALARS)
#undef DAS_REGISTER_CALL_SCALARS
        AddUEExtern<DAS_BIND_FUN(MethodArgenum)>(*this, Library, "__ue_method_arg_enum",
                                                 das::SideEffects::modifyExternal, "MethodArgenum");
        AddUEExtern<DAS_BIND_FUN(MethodResultenum)>(*this, Library, "__ue_method_result_enum",
                                                    das::SideEffects::modifyExternal, "MethodResultenum");
        AddUEExtern<DAS_BIND_FUN(MethodArgstruct)>(*this, Library, "__ue_method_arg_struct",
                                                   das::SideEffects::modifyExternal, "MethodArgstruct");
        AddUEExtern<DAS_BIND_FUN(MethodResultstruct), das::SimNode_ExtFuncCallAndCopyOrMove>(
                        *this, Library, "__ue_method_result_struct", das::SideEffects::modifyExternal,
                        "MethodResultstruct")
                        ->needCallerStackFrame = true;
        AddUEExtern<DAS_BIND_FUN(BeginStaticCall), das::SimNode_ExtFuncCallAndCopyOrMove>(
            *this, Library, "ue_begin_static_call", das::SideEffects::modifyExternal, "BeginStaticCall");
        AddUEExtern<DAS_BIND_FUN(BeginCall), das::SimNode_ExtFuncCallAndCopyOrMove>(
            *this, Library, "ue_begin_call", das::SideEffects::modifyExternal, "BeginCall");
        AddUEExtern<DAS_BIND_FUN(InvokeCall)>(*this, Library, "ue_invoke_call", das::SideEffects::modifyExternal,
                                              "InvokeCall");
        AddUEExtern<DAS_BIND_FUN(ReleaseCall)>(*this, Library, "ue_release_call", das::SideEffects::modifyExternal,
                                               "ReleaseCall");
        AddUEExtern<DAS_BIND_FUN(PrepareCallHash)>(*this, Library, "ue_arg_hash", das::SideEffects::modifyExternal,
                                                   "PrepareCallHash");
        AddUEExtern<DAS_BIND_FUN(GetCallHashSize)>(*this, Library, "ue_result_hash_size",
                                                   das::SideEffects::modifyExternal, "GetCallHashSize");
        AddUEExtern<DAS_BIND_FUN(PrepareCallArray)>(*this, Library, "ue_arg_array", das::SideEffects::modifyExternal,
                                                    "PrepareCallArray");
        AddUEExtern<DAS_BIND_FUN(GetCallArraySize)>(*this, Library, "ue_result_array_size",
                                                    das::SideEffects::modifyExternal, "GetCallArraySize");

        AddUEExtern<DAS_BIND_FUN(ObjectValue), das::SimNode_ExtFuncCallAndCopyOrMove>(
            *this, Library, "ue_object_value", das::SideEffects::modifyExternal, "ObjectValue");
        AddUEExtern<DAS_BIND_FUN(GetSelfObject), das::SimNode_ExtFuncCallAndCopyOrMove>(
            *this, Library, "ue_get_self_object", das::SideEffects::modifyExternal, "GetSelfObject");
        AddUEExtern<DAS_BIND_FUN(SetSelfObject)>(*this, Library, "ue_set_self_object", das::SideEffects::modifyExternal,
                                                 "SetSelfObject");
        AddUEExtern<DAS_BIND_FUN(EnumValue)>(*this, Library, "ue_enum_value", das::SideEffects::modifyExternal,
                                             "EnumValue");
        AddUEExtern<DAS_BIND_FUN(EnumFromNumber)>(*this, Library, "ue_enum_from_number",
                                                  das::SideEffects::modifyExternal, "EnumFromNumber");
        AddUEExtern<DAS_BIND_FUN(StructEnumGetValue)>(*this, Library, "__ue_struct_get_enum_value",
                                                      das::SideEffects::modifyExternal, "StructEnumGetValue");
        AddUEExtern<DAS_BIND_FUN(StructEnumWithValue), das::SimNode_ExtFuncCallAndCopyOrMove>(
                        *this, Library, "__ue_struct_with_enum_value", das::SideEffects::modifyExternal,
                        "StructEnumWithValue")
                        ->needCallerStackFrame = true;
        AddUEExtern<DAS_BIND_FUN(EnumNumber)>(*this, Library, "ue_enum_number", das::SideEffects::accessExternal,
                                              "EnumNumber");
        AddUEExtern<DAS_BIND_FUN(CheckedEnumNumber)>(*this, Library, "__ue_checked_enum_number",
                                                     das::SideEffects::accessExternal, "CheckedEnumNumber");
        AddUEExtern<DAS_BIND_FUN(RequireStructType)>(*this, Library, "__ue_require_struct_type",
                                                     das::SideEffects::accessExternal, "RequireStructType");
        AddUEExtern<DAS_BIND_FUN(LibraryScope)>(*this, Library, "__ue_library_scope", das::SideEffects::modifyExternal,
                                                "LibraryScope");
        AddUEExtern<DAS_BIND_FUN(NullObject), das::SimNode_ExtFuncCallAndCopyOrMove>(
            *this, Library, "ue_null_object", das::SideEffects::none, "NullObject");
        AddUEExtern<DAS_BIND_FUN(ObjectSelf), das::SimNode_ExtFuncCallAndCopyOrMove>(
            *this, Library, "ue_self_object", das::SideEffects::modifyExternal, "ObjectSelf");
        AddUEExtern<DAS_BIND_FUN(ComponentOwner), das::SimNode_ExtFuncCallAndCopyOrMove>(
            *this, Library, "ue_component_owner", das::SideEffects::accessExternal, "ComponentOwner");
        AddUEExtern<DAS_BIND_FUN(ObjectProperty), das::SimNode_ExtFuncCallAndCopyOrMove>(
            *this, Library, "ue_get_object", das::SideEffects::modifyExternal, "ObjectProperty");
        AddUEExtern<DAS_BIND_FUN(ObjectCast), das::SimNode_ExtFuncCallAndCopyOrMove>(
            *this, Library, "ue_cast_object", das::SideEffects::modifyExternal, "ObjectCast");
        AddUEExtern<DAS_BIND_FUN(ObjectValid)>(*this, Library, "ue_object_valid", das::SideEffects::accessExternal,
                                               "ObjectValid");
        AddUEExtern<DAS_BIND_FUN(ObjectsEqual)>(*this, Library, "__ue_same_object", das::SideEffects::accessExternal,
                                                "ObjectsEqual");
        AddUEExtern<DAS_BIND_FUN(ObjectIsA)>(*this, Library, "ue_object_is_a", das::SideEffects::accessExternal,
                                             "ObjectIsA");
        AddUEExtern<DAS_BIND_FUN(AuthoredNullableCheck), das::SimNode_ExtFuncCallAndCopyOrMove>(
            *this, Library, "__ue_authored_nullable_check", das::SideEffects::accessExternal, "AuthoredNullableCheck");
        AddUEExtern<DAS_BIND_FUN(AuthoredObjectCheck), das::SimNode_ExtFuncCallAndCopyOrMove>(
            *this, Library, "__ue_authored_object_check", das::SideEffects::accessExternal, "AuthoredObjectCheck");
        AddUEExtern<DAS_BIND_FUN(AuthoredGetstruct), das::SimNode_ExtFuncCallAndCopyOrMove>(
                        *this, Library, "__ue_authored_get_struct", das::SideEffects::modifyExternal,
                        "AuthoredGetstruct")
                        ->needCallerStackFrame = true;
        AddUEExtern<DAS_BIND_FUN(AuthoredSetstruct)>(*this, Library, "__ue_authored_set_struct",
                                                     das::SideEffects::modifyExternal, "AuthoredSetstruct");
        AddUEExtern<DAS_BIND_FUN(AuthoredGetobject), das::SimNode_ExtFuncCallAndCopyOrMove>(
            *this, Library, "__ue_authored_get_object", das::SideEffects::modifyExternal, "AuthoredGetobject");
        AddUEExtern<DAS_BIND_FUN(AuthoredSetobject)>(*this, Library, "__ue_authored_set_object",
                                                     das::SideEffects::modifyExternal, "AuthoredSetobject");
        AddUEExtern<DAS_BIND_FUN(ObjectGetInt)>(*this, Library, "ue_object_get_int", das::SideEffects::accessExternal,
                                                "ObjectGetInt");
        AddUEExtern<DAS_BIND_FUN(ObjectSetInt)>(*this, Library, "ue_object_set_int", das::SideEffects::modifyExternal,
                                                "ObjectSetInt");
        AddUEExtern<DAS_BIND_FUN(CallInt)>(*this, Library, "ue_call_int", das::SideEffects::modifyExternal, "CallInt");
        AddUEExtern<DAS_BIND_FUN(CallFloat)>(*this, Library, "ue_call_float", das::SideEffects::modifyExternal,
                                             "CallFloat");
        AddUEExtern<DAS_BIND_FUN(CallVoidInt)>(*this, Library, "ue_call_void_int", das::SideEffects::modifyExternal,
                                               "CallVoidInt");
        AddUEExtern<DAS_BIND_FUN(CallObject), das::SimNode_ExtFuncCallAndCopyOrMove>(
            *this, Library, "ue_call_object", das::SideEffects::modifyExternal, "CallObject");
        AddUEExtern<DAS_BIND_FUN(CallVoidObject)>(*this, Library, "ue_call_void_object",
                                                  das::SideEffects::modifyExternal, "CallVoidObject");
        AddUEExtern<DAS_BIND_FUN(SubscribeObjectDouble)>(*this, Library, "ue_subscribe_object_double",
                                                         das::SideEffects::modifyExternal, "SubscribeObjectDouble");
        AddUEExtern<DAS_BIND_FUN(SubscribeObjectTyped)>(*this, Library, "ue_subscribe_object_object",
                                                        das::SideEffects::modifyExternal, "SubscribeObjectTyped");
        AddUEExtern<DAS_BIND_FUN(SubscribeObjectObject)>(*this, Library, "ue_subscribe_object_object",
                                                         das::SideEffects::modifyExternal, "SubscribeObjectObject");
        AddUEExtern<DAS_BIND_FUN(SubscribeObjectRotator)>(*this, Library, "ue_subscribe_object_rotator",
                                                          das::SideEffects::modifyExternal, "SubscribeObjectRotator");
        AddUEExtern<DAS_BIND_FUN(SubscribeObjectTransform)>(*this, Library, "ue_subscribe_object_transform",
                                                            das::SideEffects::modifyExternal,
                                                            "SubscribeObjectTransform");
        AddUEExtern<DAS_BIND_FUN(SubscribeObjectName)>(*this, Library, "ue_subscribe_object_name",
                                                       das::SideEffects::modifyExternal, "SubscribeObjectName");
        AddUEExtern<DAS_BIND_FUN(SubscribeObjectArrayInt)>(*this, Library, "ue_subscribe_object_array_int",
                                                           das::SideEffects::modifyExternal, "SubscribeObjectArrayInt");
        AddUEExtern<DAS_BIND_FUN(SubscribeObjectArrayDouble)>(*this, Library, "ue_subscribe_object_array_double",
                                                              das::SideEffects::modifyExternal,
                                                              "SubscribeObjectArrayDouble");
        AddUEExtern<DAS_BIND_FUN(SubscribeObjectArrayString)>(*this, Library, "ue_subscribe_object_array_string",
                                                              das::SideEffects::modifyExternal,
                                                              "SubscribeObjectArrayString");
        AddUEExtern<DAS_BIND_FUN(SubscribeObjectVector2d)>(*this, Library, "ue_subscribe_object_vector2d",
                                                           das::SideEffects::modifyExternal, "SubscribeObjectVector2d");
        AddUEExtern<DAS_BIND_FUN(SceneValid)>(*this, Library, "__ue_scene_valid", das::SideEffects::modifyExternal,
                                              "SceneValid");
        AddUEExtern<DAS_BIND_FUN(ActorOwner), das::SimNode_ExtFuncCallAndCopyOrMove>(
            *this, Library, "__ue_actor_owner", das::SideEffects::modifyExternal, "ActorOwner");
        AddUEExtern<DAS_BIND_FUN(SceneActorRoot), das::SimNode_ExtFuncCallAndCopyOrMove>(
            *this, Library, "__ue_scene_actor_root", das::SideEffects::modifyExternal, "SceneActorRoot");
        AddUEExtern<DAS_BIND_FUN(SceneParent), das::SimNode_ExtFuncCallAndCopyOrMove>(
            *this, Library, "__ue_scene_parent", das::SideEffects::modifyExternal, "SceneParent");
        AddUEExtern<DAS_BIND_FUN(SceneRelative), das::SimNode_ExtFuncCallAndCopyOrMove>(
            *this, Library, "__ue_scene_relative_transform", das::SideEffects::modifyExternal, "SceneRelative");
        AddUEExtern<DAS_BIND_FUN(SceneWorld), das::SimNode_ExtFuncCallAndCopyOrMove>(
            *this, Library, "__ue_scene_world_transform", das::SideEffects::modifyExternal, "SceneWorld");
        AddUEExtern<DAS_BIND_FUN(SceneRelativeTranslation), das::SimNode_ExtFuncCallAndCopyOrMove>(
            *this, Library, "__ue_scene_relative_translation", das::SideEffects::modifyExternal,
            "SceneRelativeTranslation");
        AddUEExtern<DAS_BIND_FUN(SceneRelativeRotation), das::SimNode_ExtFuncCallAndCopyOrMove>(
            *this, Library, "__ue_scene_relative_rotation", das::SideEffects::modifyExternal, "SceneRelativeRotation");
        AddUEExtern<DAS_BIND_FUN(SceneRelativeScale), das::SimNode_ExtFuncCallAndCopyOrMove>(
            *this, Library, "__ue_scene_relative_scale", das::SideEffects::modifyExternal, "SceneRelativeScale");
        AddUEExtern<DAS_BIND_FUN(SceneSetRelative)>(*this, Library, "__ue_scene_set_relative_transform",
                                                    das::SideEffects::modifyExternal, "SceneSetRelative");
        AddUEExtern<DAS_BIND_FUN(SceneSetWorld)>(*this, Library, "__ue_scene_set_world_transform",
                                                 das::SideEffects::modifyExternal, "SceneSetWorld");
        AddUEExtern<DAS_BIND_FUN(SceneAttach)>(*this, Library, "__ue_scene_attach", das::SideEffects::modifyExternal,
                                               "SceneAttach");
        AddUEExtern<DAS_BIND_FUN(SceneDetach)>(*this, Library, "__ue_scene_detach", das::SideEffects::modifyExternal,
                                               "SceneDetach");
        AddUEExtern<DAS_BIND_FUN(SceneCreate), das::SimNode_ExtFuncCallAndCopyOrMove>(
            *this, Library, "__ue_scene_create", das::SideEffects::modifyExternal, "SceneCreate");
        AddUEExtern<DAS_BIND_FUN(SceneGet), das::SimNode_ExtFuncCallAndCopyOrMove>(
            *this, Library, "__ue_scene_get", das::SideEffects::modifyExternal, "SceneGet");
        AddUEExtern<DAS_BIND_FUN(SceneGetOrCreate), das::SimNode_ExtFuncCallAndCopyOrMove>(
            *this, Library, "__ue_scene_get_or_create", das::SideEffects::modifyExternal, "SceneGetOrCreate");
        AddUEExtern<DAS_BIND_FUN(ClassLoaded)>(*this, Library, "__ue_class_loaded", das::SideEffects::accessExternal,
                                               "ClassLoaded");
        AddUEExtern<DAS_BIND_FUN(ActorSpawn), das::SimNode_ExtFuncCallAndCopyOrMove>(
            *this, Library, "__ue_actor_spawn", das::SideEffects::modifyExternal, "ActorSpawn");
        AddUEExtern<DAS_BIND_FUN(ActorDestroy)>(*this, Library, "ue_actor_destroy_handle",
                                                das::SideEffects::modifyExternal, "ActorDestroy");
        AddUEExtern<DAS_BIND_FUN(ObjectFindByType), das::SimNode_ExtFuncCallAndCopyOrMove>(
            *this, Library, "__ue_find_by_type", das::SideEffects::accessExternal, "ObjectFindByType");
        AddUEExtern<DAS_BIND_FUN(ObjectQueryBegin)>(*this, Library, "__ue_query_begin",
                                                    das::SideEffects::modifyExternal, "ObjectQueryBegin");
        AddUEExtern<DAS_BIND_FUN(ObjectQueryCount)>(*this, Library, "__ue_query_count",
                                                    das::SideEffects::accessExternal, "ObjectQueryCount");
        AddUEExtern<DAS_BIND_FUN(ObjectQueryGet), das::SimNode_ExtFuncCallAndCopyOrMove>(
            *this, Library, "__ue_query_get", das::SideEffects::modifyExternal, "ObjectQueryGet");
        AddUEExtern<DAS_BIND_FUN(ObjectQueryRelease)>(*this, Library, "__ue_query_release",
                                                      das::SideEffects::modifyExternal, "ObjectQueryRelease");
        AddUEExtern<DAS_BIND_FUN(ClassValid)>(*this, Library, "ue_class_valid", das::SideEffects::accessExternal,
                                              "ClassValid");
        AddUEExtern<DAS_BIND_FUN(ClassPath)>(*this, Library, "ue_class_path", das::SideEffects::accessExternal,
                                             "ClassPath");
        AddUEExtern<DAS_BIND_FUN(PathComponentOperation), das::SimNode_ExtFuncCallAndCopyOrMove>(
            *this, Library, "__ue_class_component_operation", das::SideEffects::modifyExternal,
            "PathComponentOperation");
        AddUEExtern<DAS_BIND_FUN(ClassComponentOperation), das::SimNode_ExtFuncCallAndCopyOrMove>(
            *this, Library, "__ue_class_component_operation", das::SideEffects::modifyExternal,
            "ClassComponentOperation");
        AddUEExtern<DAS_BIND_FUN(NativeComponentOperation), das::SimNode_ExtFuncCallAndCopyOrMove>(
            *this, Library, "__ue_native_component_operation", das::SideEffects::modifyExternal,
            "NativeComponentOperation");
        AddUEExtern<DAS_BIND_FUN(AuthoredComponentGet), das::SimNode_ExtFuncCallAndCopyOrMove>(
            *this, Library, "__ue_authored_component_get", das::SideEffects::modifyExternal, "AuthoredComponentGet");
        AddUEExtern<DAS_BIND_FUN(NativeComponentDestroy)>(*this, Library, "ue_component_destroy_handle",
                                                          das::SideEffects::modifyExternal, "NativeComponentDestroy");
        AddUEExtern<DAS_BIND_FUN(SceneDestroy)>(*this, Library, "__ue_scene_destroy", das::SideEffects::modifyExternal,
                                                "SceneDestroy");
        AddUEExtern<DAS_BIND_FUN(SubscribeObjectVector)>(*this, Library, "ue_subscribe_object_vector",
                                                         das::SideEffects::modifyExternal, "SubscribeObjectVector");
        AddUEExtern<DAS_BIND_FUN(SubscribeObjectFloat)>(*this, Library, "ue_subscribe_object_float",
                                                        das::SideEffects::modifyExternal, "SubscribeObjectFloat");
        AddUEExtern<DAS_BIND_FUN(SubscribeObjectBool)>(*this, Library, "ue_subscribe_object_bool",
                                                       das::SideEffects::modifyExternal, "SubscribeObjectBool");
        AddUEExtern<DAS_BIND_FUN(SubscribeObjectString)>(*this, Library, "ue_subscribe_object_string",
                                                         das::SideEffects::modifyExternal, "SubscribeObjectString");
        AddUEExtern<DAS_BIND_FUN(SubscribeObjectVoid)>(*this, Library, "ue_subscribe_object_void",
                                                       das::SideEffects::modifyExternal, "SubscribeObjectVoid");
        AddUEExtern<DAS_BIND_FUN(SubscribeObjectByte)>(*this, Library, "ue_subscribe_object_byte",
                                                       das::SideEffects::modifyExternal, "SubscribeObjectByte");
        AddUEExtern<DAS_BIND_FUN(SubscribeObjectText)>(*this, Library, "ue_subscribe_object_text",
                                                       das::SideEffects::modifyExternal, "SubscribeObjectText");
        AddUEExtern<DAS_BIND_FUN(SubscribeObjectArrayFloat)>(*this, Library, "ue_subscribe_object_array_float",
                                                             das::SideEffects::modifyExternal,
                                                             "SubscribeObjectArrayFloat");
        AddUEExtern<DAS_BIND_FUN(SubscribeObjectArrayBool)>(*this, Library, "ue_subscribe_object_array_bool",
                                                            das::SideEffects::modifyExternal,
                                                            "SubscribeObjectArrayBool");
        AddUEExtern<DAS_BIND_FUN(SubscribeObjectArrayName)>(*this, Library, "ue_subscribe_object_array_name",
                                                            das::SideEffects::modifyExternal,
                                                            "SubscribeObjectArrayName");
        AddUEExtern<DAS_BIND_FUN(SubscribeObjectEnum)>(*this, Library, "ue_subscribe_object_enum",
                                                       das::SideEffects::modifyExternal, "SubscribeObjectEnum");
        AddUEExtern<DAS_BIND_FUN(SubscribeObjectClass)>(*this, Library, "ue_subscribe_object_class",
                                                        das::SideEffects::modifyExternal, "SubscribeObjectClass");
        AddUEExtern<DAS_BIND_FUN(SubscribeObjectStruct)>(*this, Library, "ue_subscribe_object_struct",
                                                         das::SideEffects::modifyExternal, "SubscribeObjectStruct");
        AddUEExtern<DAS_BIND_FUN(SubscribeObjectArrayByte)>(*this, Library, "ue_subscribe_object_array_byte",
                                                            das::SideEffects::modifyExternal,
                                                            "SubscribeObjectArrayByte");
        AddUEExtern<DAS_BIND_FUN(SubscribeObjectArrayText)>(*this, Library, "ue_subscribe_object_array_text",
                                                            das::SideEffects::modifyExternal,
                                                            "SubscribeObjectArrayText");
        AddUEExtern<DAS_BIND_FUN(SubscribeObjectInt)>(*this, Library, "ue_subscribe_object_int",
                                                      das::SideEffects::modifyExternal, "SubscribeObjectInt");

        AddUEExtern<DAS_BIND_FUN(SubscribeInt)>(*this, Library, "ue_subscribe_int", das::SideEffects::modifyExternal,
                                                "SubscribeInt");
        AddUEExtern<DAS_BIND_FUN(EmitSignal)>(*this, Library, "ue_emit_signal", das::SideEffects::modifyExternal,
                                              "EmitSignal");
        AddUEExtern<DAS_BIND_FUN(SubsystemObject), das::SimNode_ExtFuncCallAndCopyOrMove>(
            *this, Library, "__ue_subsystem", das::SideEffects::modifyExternal);
        AddUEExtern<DAS_BIND_FUN(ClassDefaultObject), das::SimNode_ExtFuncCallAndCopyOrMove>(
            *this, Library, "__ue_class_default_object", das::SideEffects::modifyExternal);
        AddUEExtern<DAS_BIND_FUN((InitializePersistentGlobal<int32,das::Type::tInt>))>(
            *this, Library, "__ue_state_init_int", das::SideEffects::modifyExternal);
        AddUEExtern<DAS_BIND_FUN((InitializePersistentGlobal<bool,das::Type::tBool>))>(
            *this, Library, "__ue_state_init_bool", das::SideEffects::modifyExternal);
        AddUEExtern<DAS_BIND_FUN((InitializePersistentGlobal<float,das::Type::tFloat>))>(
            *this, Library, "__ue_state_init_float", das::SideEffects::modifyExternal);
        AddUEExtern<DAS_BIND_FUN((InitializePersistentGlobal<double,das::Type::tDouble>))>(
            *this, Library, "__ue_state_init_double", das::SideEffects::modifyExternal);
        AddUEExtern<DAS_BIND_FUN(InitializePersistentString)>(*this, Library, "__ue_state_init_string",
                                                              das::SideEffects::modifyExternal);
        AddUEExtern<DAS_BIND_FUN(GetStateInt)>(*this, Library, "ue_state_get_int", das::SideEffects::modifyExternal,
                                               "GetStateInt");
        AddUEExtern<DAS_BIND_FUN(SetStateInt)>(*this, Library, "ue_state_set_int", das::SideEffects::modifyExternal,
                                               "SetStateInt");
        AddUEExtern<DAS_BIND_FUN(NativeAdd)>(*this, Library, "ue_add", das::SideEffects::none, "NativeAdd");
        AddUEExtern<DAS_BIND_FUN(DSLVector), das::SimNode_ExtFuncCallAndCopyOrMove>(
            *this, Library, "ue_vector", das::SideEffects::none, "DSLVector");
        AddUEExtern<DAS_BIND_FUN(DSLRotator), das::SimNode_ExtFuncCallAndCopyOrMove>(
            *this, Library, "ue_rotator", das::SideEffects::none, "DSLRotator");
        AddUEExtern<DAS_BIND_FUN(DSLTransform), das::SimNode_ExtFuncCallAndCopyOrMove>(
            *this, Library, "ue_transform", das::SideEffects::none, "DSLTransform");
        AddUEExtern<DAS_BIND_FUN(DSLIdentityTransform), das::SimNode_ExtFuncCallAndCopyOrMove>(
            *this, Library, "ue_transform_identity", das::SideEffects::none, "DSLIdentityTransform");
        AddUEExtern<DAS_BIND_FUN(DSLTransformPosition), das::SimNode_ExtFuncCallAndCopyOrMove>(
            *this, Library, "ue_transform_position", das::SideEffects::none, "DSLTransformPosition");
        AddUEExtern<DAS_BIND_FUN(DSLTranslation), das::SimNode_ExtFuncCallAndCopyOrMove>(
            *this, Library, "ue_transform_translation", das::SideEffects::none, "DSLTranslation");
        AddUEExtern<DAS_BIND_FUN(DSLScale), das::SimNode_ExtFuncCallAndCopyOrMove>(*this, Library, "ue_transform_scale",
                                                                                   das::SideEffects::none, "DSLScale");
        AddUEExtern<DAS_BIND_FUN(DSLRotation), das::SimNode_ExtFuncCallAndCopyOrMove>(
            *this, Library, "ue_transform_rotation", das::SideEffects::none, "DSLRotation");
        AddUEExtern<DAS_BIND_FUN(ExplicitStructCollect)>(*this, Library, "ue_struct_collect",
                                                         das::SideEffects::modifyExternal, "ExplicitStructCollect")
                        ->needCallerStackFrame = true;
        AddUEExtern<DAS_BIND_FUN(StructValueCount)>(*this, Library, "ue_struct_value_count",
                                                    das::SideEffects::accessExternal, "StructValueCount");
        AddUEExtern<DAS_BIND_FUN(EditStructValue), das::SimNode_ExtFuncCallAndCopyOrMove>(
                        *this, Library, "ue_struct_edit", das::SideEffects::modifyExternal, "EditStructValue")
                        ->needCallerStackFrame = true;
        AddUEExtern<DAS_BIND_FUN(NewStructDefault), das::SimNode_ExtFuncCallAndCopyOrMove>(
                        *this, Library, "ue_struct_default", das::SideEffects::modifyExternal, "NewStructDefault")
                        ->needCallerStackFrame = true;
        AddUEExtern<DAS_BIND_FUN(GetSelfStruct), das::SimNode_ExtFuncCallAndCopyOrMove>(
                        *this, Library, "ue_get_self_struct", das::SideEffects::modifyExternal, "GetSelfStruct")
                        ->needCallerStackFrame = true;
        AddUEExtern<DAS_BIND_FUN(SetSelfStruct)>(*this, Library, "ue_set_self_struct", das::SideEffects::modifyExternal,
                                                 "SetSelfStruct");
        AddUEExtern<DAS_BIND_FUN(NewStructValue), das::SimNode_ExtFuncCallAndCopyOrMove>(
                        *this, Library, "ue_struct_value", das::SideEffects::modifyExternal, "NewStructValue")
                        ->needCallerStackFrame = true;
        AddUEExtern<DAS_BIND_FUN(StructSetSize)>(*this, Library, "ue_struct_set_size", das::SideEffects::modifyExternal,
                                                 "StructSetSize");
        AddUEExtern<DAS_BIND_FUN(StructSetElementInt)>(*this, Library, "ue_struct_set_element_int",
                                                       das::SideEffects::modifyExternal, "StructSetElementInt");
        AddUEExtern<DAS_BIND_FUN(StructSetElementString)>(*this, Library, "ue_struct_set_element_string",
                                                          das::SideEffects::modifyExternal, "StructSetElementString");
        AddUEExtern<DAS_BIND_FUN(StructSetContainsInt)>(*this, Library, "ue_struct_set_contains_int",
                                                        das::SideEffects::modifyExternal, "StructSetContainsInt");
        AddUEExtern<DAS_BIND_FUN(StructSetContainsString)>(*this, Library, "ue_struct_set_contains_string",
                                                           das::SideEffects::modifyExternal, "StructSetContainsString");
        AddUEExtern<DAS_BIND_FUN(StructSetWithInt), das::SimNode_ExtFuncCallAndCopyOrMove>(
                        *this, Library, "ue_struct_set_with_int", das::SideEffects::modifyExternal, "StructSetWithInt")
                        ->needCallerStackFrame = true;
        AddUEExtern<DAS_BIND_FUN(StructSetWithString), das::SimNode_ExtFuncCallAndCopyOrMove>(
                        *this, Library, "ue_struct_set_with_string", das::SideEffects::modifyExternal,
                        "StructSetWithString")
                        ->needCallerStackFrame = true;
        AddUEExtern<DAS_BIND_FUN(StructMapGetIntEnum)>(*this, Library, "__ue_struct_map_get_int_enum",
                                                       das::SideEffects::modifyExternal, "StructMapGetIntEnum");
        AddUEExtern<DAS_BIND_FUN(StructMapWithIntEnum), das::SimNode_ExtFuncCallAndCopyOrMove>(
                        *this, Library, "__ue_struct_map_with_int_enum", das::SideEffects::modifyExternal,
                        "StructMapWithIntEnum")
                        ->needCallerStackFrame = true;
        AddUEExtern<DAS_BIND_FUN(StructMapGetIntObject), das::SimNode_ExtFuncCallAndCopyOrMove>(
            *this, Library, "__ue_struct_map_get_int_object", das::SideEffects::modifyExternal,
            "StructMapGetIntObject");
        AddUEExtern<DAS_BIND_FUN(StructMapWithIntObject), das::SimNode_ExtFuncCallAndCopyOrMove>(
                        *this, Library, "__ue_struct_map_with_int_object", das::SideEffects::modifyExternal,
                        "StructMapWithIntObject")
                        ->needCallerStackFrame = true;
        AddUEExtern<DAS_BIND_FUN(StructMapGetStringEnum)>(*this, Library, "__ue_struct_map_get_string_enum",
                                                          das::SideEffects::modifyExternal, "StructMapGetStringEnum");
        AddUEExtern<DAS_BIND_FUN(StructMapWithStringEnum), das::SimNode_ExtFuncCallAndCopyOrMove>(
                        *this, Library, "__ue_struct_map_with_string_enum", das::SideEffects::modifyExternal,
                        "StructMapWithStringEnum")
                        ->needCallerStackFrame = true;
        AddUEExtern<DAS_BIND_FUN(StructMapGetStringObject), das::SimNode_ExtFuncCallAndCopyOrMove>(
            *this, Library, "__ue_struct_map_get_string_object", das::SideEffects::modifyExternal,
            "StructMapGetStringObject");
        AddUEExtern<DAS_BIND_FUN(StructMapWithStringObject), das::SimNode_ExtFuncCallAndCopyOrMove>(
                        *this, Library, "__ue_struct_map_with_string_object", das::SideEffects::modifyExternal,
                        "StructMapWithStringObject")
                        ->needCallerStackFrame = true;
        AddUEExtern<DAS_BIND_FUN(StructMapGetIntStruct), das::SimNode_ExtFuncCallAndCopyOrMove>(
                        *this, Library, "__ue_struct_map_get_int_struct", das::SideEffects::modifyExternal,
                        "StructMapGetIntStruct")
                        ->needCallerStackFrame = true;
        AddUEExtern<DAS_BIND_FUN(StructMapWithIntStruct), das::SimNode_ExtFuncCallAndCopyOrMove>(
                        *this, Library, "__ue_struct_map_with_int_struct", das::SideEffects::modifyExternal,
                        "StructMapWithIntStruct")
                        ->needCallerStackFrame = true;
        AddUEExtern<DAS_BIND_FUN(StructMapGetStringStruct), das::SimNode_ExtFuncCallAndCopyOrMove>(
                        *this, Library, "__ue_struct_map_get_string_struct", das::SideEffects::modifyExternal,
                        "StructMapGetStringStruct")
                        ->needCallerStackFrame = true;
        AddUEExtern<DAS_BIND_FUN(StructMapWithStringStruct), das::SimNode_ExtFuncCallAndCopyOrMove>(
                        *this, Library, "__ue_struct_map_with_string_struct", das::SideEffects::modifyExternal,
                        "StructMapWithStringStruct")
                        ->needCallerStackFrame = true;
        // Scalar map members of struct snapshots: key kind x value kind (DasScalarAccessors.inl).
#define DAS_REGISTER_MAP_SCALAR(GetName, WithName, KeyKind, ValueKind)                                                 \
    AddUEExtern<DAS_BIND_FUN((StructMapGetScalar<KeyKind, ValueKind>))>(                                                \
        *this, Library, GetName, das::SideEffects::modifyExternal, "StructMapGetScalar");                              \
    AddUEExtern<DAS_BIND_FUN((StructMapWithScalar<KeyKind, ValueKind>)), das::SimNode_ExtFuncCallAndCopyOrMove>(        \
        *this, Library, WithName, das::SideEffects::modifyExternal, "StructMapWithScalar")                             \
                                 ->needCallerStackFrame = true;
#define DAS_REGISTER_MAP_VALUES(Key, KeyKind)                                                                          \
    DAS_REGISTER_MAP_SCALAR("ue_struct_map_get_" #Key "_bool", "ue_struct_map_with_" #Key "_bool", KeyKind,            \
                            FDasKindBool)                                                                              \
    DAS_REGISTER_MAP_SCALAR("ue_struct_map_get_" #Key "_byte", "ue_struct_map_with_" #Key "_byte", KeyKind,            \
                            FDasKindByte)                                                                              \
    DAS_REGISTER_MAP_SCALAR("ue_struct_map_get_" #Key "_double", "ue_struct_map_with_" #Key "_double", KeyKind,        \
                            FDasKindDouble)                                                                            \
    DAS_REGISTER_MAP_SCALAR("ue_struct_map_get_" #Key "_float", "ue_struct_map_with_" #Key "_float", KeyKind,          \
                            FDasKindFloat)                                                                             \
    DAS_REGISTER_MAP_SCALAR("ue_struct_map_get_" #Key "_name", "ue_struct_map_with_" #Key "_name", KeyKind,            \
                            FDasKindName)                                                                              \
    DAS_REGISTER_MAP_SCALAR("ue_struct_map_get_" #Key "_string", "ue_struct_map_with_" #Key "_string", KeyKind,        \
                            FDasKindString)                                                                            \
    DAS_REGISTER_MAP_SCALAR("ue_struct_map_get_" #Key "_text", "ue_struct_map_with_" #Key "_text", KeyKind,            \
                            FDasKindText)
        DAS_REGISTER_MAP_VALUES(int, FDasKindInt)
        DAS_REGISTER_MAP_VALUES(string, FDasKindString)
        DAS_REGISTER_MAP_VALUES(name, FDasKindName)
        DAS_REGISTER_MAP_VALUES(byte, FDasKindByte)
        // int -> int keeps its historical short name.
        DAS_REGISTER_MAP_SCALAR("ue_struct_map_get_int", "ue_struct_map_with_int", FDasKindInt, FDasKindInt)
        DAS_REGISTER_MAP_SCALAR("ue_struct_map_get_string_int", "ue_struct_map_with_string_int", FDasKindString,
                                FDasKindInt)
        DAS_REGISTER_MAP_SCALAR("ue_struct_map_get_name_int", "ue_struct_map_with_name_int", FDasKindName, FDasKindInt)
        DAS_REGISTER_MAP_SCALAR("ue_struct_map_get_byte_int", "ue_struct_map_with_byte_int", FDasKindByte, FDasKindInt)
#undef DAS_REGISTER_MAP_VALUES
#undef DAS_REGISTER_MAP_SCALAR
        AddUEExtern<DAS_BIND_FUN(PublishFrameInt)>(*this, Library, "ue_frame_out_int", das::SideEffects::modifyExternal,
                                                   "PublishFrameInt");
        AddUEExtern<DAS_BIND_FUN(PublishFrameFloat)>(*this, Library, "ue_frame_out_float",
                                                     das::SideEffects::modifyExternal, "PublishFrameFloat");
        AddUEExtern<DAS_BIND_FUN(PublishFrameBool)>(*this, Library, "ue_frame_out_bool",
                                                    das::SideEffects::modifyExternal, "PublishFrameBool");
        AddUEExtern<DAS_BIND_FUN(PublishFrameString)>(*this, Library, "ue_frame_out_string",
                                                      das::SideEffects::modifyExternal, "PublishFrameString");
        AddUEExtern<DAS_BIND_FUN(PublishFrameDouble)>(*this, Library, "ue_frame_out_double",
                                                      das::SideEffects::modifyExternal, "PublishFrameDouble");
        AddUEExtern<DAS_BIND_FUN(StructMapKeyByte)>(*this, Library, "ue_struct_map_key_byte",
                                                    das::SideEffects::modifyExternal, "StructMapKeyByte");
        AddUEExtern<DAS_BIND_FUN(StructSetElementByte)>(*this, Library, "ue_struct_set_element_byte",
                                                        das::SideEffects::modifyExternal, "StructSetElementByte");
        AddUEExtern<DAS_BIND_FUN(StructSetWithByte), das::SimNode_ExtFuncCallAndCopyOrMove>(
                        *this, Library, "ue_struct_set_with_byte", das::SideEffects::modifyExternal,
                        "StructSetWithByte")
                        ->needCallerStackFrame = true;
        AddUEExtern<DAS_BIND_FUN(GetTextByteMapObjectByte)>(*this, Library, "__ue_struct_map_get_object_byte",
                                                            das::SideEffects::modifyExternal,
                                                            "GetTextByteMapObjectByte");
        AddUEExtern<DAS_BIND_FUN(WithTextByteMapObjectByte), das::SimNode_ExtFuncCallAndCopyOrMove>(
                        *this, Library, "__ue_struct_map_with_object_byte", das::SideEffects::modifyExternal,
                        "WithTextByteMapObjectByte")
                        ->needCallerStackFrame = true;
        AddUEExtern<DAS_BIND_FUN(GetTextByteMapObjectText)>(*this, Library, "__ue_struct_map_get_object_text",
                                                            das::SideEffects::modifyExternal,
                                                            "GetTextByteMapObjectText");
        AddUEExtern<DAS_BIND_FUN(WithTextByteMapObjectText), das::SimNode_ExtFuncCallAndCopyOrMove>(
                        *this, Library, "__ue_struct_map_with_object_text", das::SideEffects::modifyExternal,
                        "WithTextByteMapObjectText")
                        ->needCallerStackFrame = true;
        AddUEExtern<DAS_BIND_FUN(GetTextByteMapEnumByte)>(*this, Library, "__ue_struct_map_get_enum_byte",
                                                          das::SideEffects::modifyExternal, "GetTextByteMapEnumByte");
        AddUEExtern<DAS_BIND_FUN(WithTextByteMapEnumByte), das::SimNode_ExtFuncCallAndCopyOrMove>(
                        *this, Library, "__ue_struct_map_with_enum_byte", das::SideEffects::modifyExternal,
                        "WithTextByteMapEnumByte")
                        ->needCallerStackFrame = true;
        AddUEExtern<DAS_BIND_FUN(GetTextByteMapEnumText)>(*this, Library, "__ue_struct_map_get_enum_text",
                                                          das::SideEffects::modifyExternal, "GetTextByteMapEnumText");
        AddUEExtern<DAS_BIND_FUN(WithTextByteMapEnumText), das::SimNode_ExtFuncCallAndCopyOrMove>(
                        *this, Library, "__ue_struct_map_with_enum_text", das::SideEffects::modifyExternal,
                        "WithTextByteMapEnumText")
                        ->needCallerStackFrame = true;
        AddUEExtern<DAS_BIND_FUN(GetTextByteMapByteStruct), das::SimNode_ExtFuncCallAndCopyOrMove>(
                        *this, Library, "__ue_struct_map_get_byte_struct", das::SideEffects::modifyExternal,
                        "GetTextByteMapByteStruct")
                        ->needCallerStackFrame = true;
        AddUEExtern<DAS_BIND_FUN(WithTextByteMapByteStruct), das::SimNode_ExtFuncCallAndCopyOrMove>(
                        *this, Library, "__ue_struct_map_with_byte_struct", das::SideEffects::modifyExternal,
                        "WithTextByteMapByteStruct")
                        ->needCallerStackFrame = true;
        AddUEExtern<DAS_BIND_FUN(GetTextByteMapByteEnum)>(*this, Library, "__ue_struct_map_get_byte_enum",
                                                          das::SideEffects::modifyExternal, "GetTextByteMapByteEnum");
        AddUEExtern<DAS_BIND_FUN(WithTextByteMapByteEnum), das::SimNode_ExtFuncCallAndCopyOrMove>(
                        *this, Library, "__ue_struct_map_with_byte_enum", das::SideEffects::modifyExternal,
                        "WithTextByteMapByteEnum")
                        ->needCallerStackFrame = true;
        AddUEExtern<DAS_BIND_FUN(GetTextByteMapByteObject), das::SimNode_ExtFuncCallAndCopyOrMove>(
            *this, Library, "__ue_struct_map_get_byte_object", das::SideEffects::modifyExternal,
            "GetTextByteMapByteObject");
        AddUEExtern<DAS_BIND_FUN(WithTextByteMapByteObject), das::SimNode_ExtFuncCallAndCopyOrMove>(
                        *this, Library, "__ue_struct_map_with_byte_object", das::SideEffects::modifyExternal,
                        "WithTextByteMapByteObject")
                        ->needCallerStackFrame = true;
        AddUEExtern<DAS_BIND_FUN(PublishFrameByte)>(*this, Library, "ue_frame_out_byte",
                                                    das::SideEffects::modifyExternal, "PublishFrameByte");
        AddUEExtern<DAS_BIND_FUN(PublishFrameText)>(*this, Library, "ue_frame_out_text",
                                                    das::SideEffects::modifyExternal, "PublishFrameText");
        AddUEExtern<DAS_BIND_FUN(PublishFrameName)>(*this, Library, "ue_frame_out_name",
                                                    das::SideEffects::modifyExternal, "PublishFrameName");
        AddUEExtern<DAS_BIND_FUN(PublishFrameObject)>(*this, Library, "ue_frame_out_object",
                                                      das::SideEffects::modifyExternal, "PublishFrameObject");
        AddUEExtern<DAS_BIND_FUN(PublishFrameClass)>(*this, Library, "ue_frame_out_class",
                                                     das::SideEffects::modifyExternal, "PublishFrameClass");
        AddUEExtern<DAS_BIND_FUN(PublishFrameEnum)>(*this, Library, "ue_frame_out_enum",
                                                    das::SideEffects::modifyExternal, "PublishFrameEnum");
        AddUEExtern<DAS_BIND_FUN(PublishFrameVector)>(*this, Library, "ue_frame_out_vector",
                                                      das::SideEffects::modifyExternal, "PublishFrameVector");
        AddUEExtern<DAS_BIND_FUN(PublishFrameRotator)>(*this, Library, "ue_frame_out_rotator",
                                                       das::SideEffects::modifyExternal, "PublishFrameRotator");
        AddUEExtern<DAS_BIND_FUN(PublishFrameTransform)>(*this, Library, "ue_frame_out_transform",
                                                         das::SideEffects::modifyExternal, "PublishFrameTransform");
        AddUEExtern<DAS_BIND_FUN(PublishFrameStruct)>(*this, Library, "ue_frame_out_struct",
                                                      das::SideEffects::modifyExternal, "PublishFrameStruct");
        AddUEExtern<DAS_BIND_FUN(CastInterface), das::SimNode_ExtFuncCallAndCopyOrMove>(
            *this, Library, "ue_cast_interface", das::SideEffects::modifyExternal, "CastInterface");
        AddUEExtern<DAS_BIND_FUN(ObjectImplementsInterface)>(
            *this, Library, "ue_implements_interface", das::SideEffects::modifyExternal, "ObjectImplementsInterface");
        AddUEExtern<DAS_BIND_FUN(InterfaceCallInt)>(*this, Library, "ue_interface_call_int",
                                                    das::SideEffects::modifyExternal, "InterfaceCallInt");
        AddUEExtern<DAS_BIND_FUN(InterfaceCallString)>(*this, Library, "ue_interface_call_string",
                                                       das::SideEffects::modifyExternal, "InterfaceCallString");
        AddUEExtern<DAS_BIND_FUN(StructMapSize)>(*this, Library, "ue_struct_map_size", das::SideEffects::modifyExternal,
                                                 "StructMapSize");
        AddUEExtern<DAS_BIND_FUN(StructMapKey)>(*this, Library, "ue_struct_map_key", das::SideEffects::modifyExternal,
                                                "StructMapKey");
        AddUEExtern<DAS_BIND_FUN(ObjectKeyFromReference)>(*this, Library, "__ue_object_key_from_ref",
                                                          das::SideEffects::modifyExternal, "ObjectKeyFromReference");
        AddUEExtern<DAS_BIND_FUN(CheckedObjectKey)>(*this, Library, "__ue_object_key_check",
                                                    das::SideEffects::modifyExternal, "CheckedObjectKey");
        AddUEExtern<DAS_BIND_FUN(ObjectKeyReference), das::SimNode_ExtFuncCallAndCopyOrMove>(
            *this, Library, "__ue_object_key_reference", das::SideEffects::modifyExternal, "ObjectKeyReference");
        AddUEExtern<DAS_BIND_FUN(ObjectKeyValid)>(*this, Library, "__ue_object_key_valid",
                                                  das::SideEffects::modifyExternal, "ObjectKeyValid");
        AddUEExtern<DAS_BIND_FUN(StructSetElementObjectKey)>(*this, Library, "__ue_struct_set_element_object_key",
                                                             das::SideEffects::modifyExternal,
                                                             "StructSetElementObjectKey");
        AddUEExtern<DAS_BIND_FUN(StructSetWithObjectKey), das::SimNode_ExtFuncCallAndCopyOrMove>(
                        *this, Library, "__ue_struct_set_with_object_key", das::SideEffects::modifyExternal,
                        "StructSetWithObjectKey")
                        ->needCallerStackFrame = true;
        AddUEExtern<DAS_BIND_FUN(StructMapObjectKey)>(*this, Library, "__ue_struct_map_key_object",
                                                      das::SideEffects::modifyExternal, "StructMapObjectKey");
        AddUEExtern<DAS_BIND_FUN(StructMapGetObjectKeyInt)>(*this, Library, "__ue_struct_map_get_object_int",
                                                            das::SideEffects::modifyExternal,
                                                            "StructMapGetObjectKeyInt");
        AddUEExtern<DAS_BIND_FUN(StructMapWithObjectKeyInt), das::SimNode_ExtFuncCallAndCopyOrMove>(
                        *this, Library, "__ue_struct_map_with_object_int", das::SideEffects::modifyExternal,
                        "StructMapWithObjectKeyInt")
                        ->needCallerStackFrame = true;
        AddUEExtern<DAS_BIND_FUN(StructMapGetObjectKeyFloat)>(*this, Library, "__ue_struct_map_get_object_float",
                                                              das::SideEffects::modifyExternal,
                                                              "StructMapGetObjectKeyFloat");
        AddUEExtern<DAS_BIND_FUN(StructMapWithObjectKeyFloat), das::SimNode_ExtFuncCallAndCopyOrMove>(
                        *this, Library, "__ue_struct_map_with_object_float", das::SideEffects::modifyExternal,
                        "StructMapWithObjectKeyFloat")
                        ->needCallerStackFrame = true;
        AddUEExtern<DAS_BIND_FUN(StructMapGetObjectKeyDouble)>(*this, Library, "__ue_struct_map_get_object_double",
                                                               das::SideEffects::modifyExternal,
                                                               "StructMapGetObjectKeyDouble");
        AddUEExtern<DAS_BIND_FUN(StructMapWithObjectKeyDouble), das::SimNode_ExtFuncCallAndCopyOrMove>(
                        *this, Library, "__ue_struct_map_with_object_double", das::SideEffects::modifyExternal,
                        "StructMapWithObjectKeyDouble")
                        ->needCallerStackFrame = true;
        AddUEExtern<DAS_BIND_FUN(StructMapGetObjectKeyBool)>(*this, Library, "__ue_struct_map_get_object_bool",
                                                             das::SideEffects::modifyExternal,
                                                             "StructMapGetObjectKeyBool");
        AddUEExtern<DAS_BIND_FUN(StructMapWithObjectKeyBool), das::SimNode_ExtFuncCallAndCopyOrMove>(
                        *this, Library, "__ue_struct_map_with_object_bool", das::SideEffects::modifyExternal,
                        "StructMapWithObjectKeyBool")
                        ->needCallerStackFrame = true;
        AddUEExtern<DAS_BIND_FUN(StructMapGetObjectKeyString)>(*this, Library, "__ue_struct_map_get_object_string",
                                                               das::SideEffects::modifyExternal,
                                                               "StructMapGetObjectKeyString");
        AddUEExtern<DAS_BIND_FUN(StructMapWithObjectKeyString), das::SimNode_ExtFuncCallAndCopyOrMove>(
                        *this, Library, "__ue_struct_map_with_object_string", das::SideEffects::modifyExternal,
                        "StructMapWithObjectKeyString")
                        ->needCallerStackFrame = true;
        AddUEExtern<DAS_BIND_FUN(StructMapGetObjectKeyName)>(*this, Library, "__ue_struct_map_get_object_name",
                                                             das::SideEffects::modifyExternal,
                                                             "StructMapGetObjectKeyName");
        AddUEExtern<DAS_BIND_FUN(StructMapWithObjectKeyName), das::SimNode_ExtFuncCallAndCopyOrMove>(
                        *this, Library, "__ue_struct_map_with_object_name", das::SideEffects::modifyExternal,
                        "StructMapWithObjectKeyName")
                        ->needCallerStackFrame = true;
        AddUEExtern<DAS_BIND_FUN(StructMapGetObjectKeyEnum)>(*this, Library, "__ue_struct_map_get_object_enum",
                                                             das::SideEffects::modifyExternal,
                                                             "StructMapGetObjectKeyEnum");
        AddUEExtern<DAS_BIND_FUN(StructMapWithObjectKeyEnum), das::SimNode_ExtFuncCallAndCopyOrMove>(
                        *this, Library, "__ue_struct_map_with_object_enum", das::SideEffects::modifyExternal,
                        "StructMapWithObjectKeyEnum")
                        ->needCallerStackFrame = true;
        AddUEExtern<DAS_BIND_FUN(StructMapGetObjectKeyObject), das::SimNode_ExtFuncCallAndCopyOrMove>(
            *this, Library, "__ue_struct_map_get_object_object", das::SideEffects::modifyExternal,
            "StructMapGetObjectKeyObject");
        AddUEExtern<DAS_BIND_FUN(StructMapWithObjectKeyObject), das::SimNode_ExtFuncCallAndCopyOrMove>(
                        *this, Library, "__ue_struct_map_with_object_object", das::SideEffects::modifyExternal,
                        "StructMapWithObjectKeyObject")
                        ->needCallerStackFrame = true;
        AddUEExtern<DAS_BIND_FUN(StructMapGetObjectKeyStruct), das::SimNode_ExtFuncCallAndCopyOrMove>(
                        *this, Library, "__ue_struct_map_get_object_struct", das::SideEffects::modifyExternal,
                        "StructMapGetObjectKeyStruct")
                        ->needCallerStackFrame = true;
        AddUEExtern<DAS_BIND_FUN(StructMapWithObjectKeyStruct), das::SimNode_ExtFuncCallAndCopyOrMove>(
                        *this, Library, "__ue_struct_map_with_object_struct", das::SideEffects::modifyExternal,
                        "StructMapWithObjectKeyStruct")
                        ->needCallerStackFrame = true;
        AddUEExtern<DAS_BIND_FUN(StructMapEnumKey)>(*this, Library, "__ue_struct_map_key_enum",
                                                    das::SideEffects::modifyExternal, "StructMapEnumKey");
        AddUEExtern<DAS_BIND_FUN(StructMapGetEnumKeyInt)>(*this, Library, "__ue_struct_map_get_enum_int",
                                                          das::SideEffects::modifyExternal, "StructMapGetEnumKeyInt");
        AddUEExtern<DAS_BIND_FUN(StructMapWithEnumKeyInt), das::SimNode_ExtFuncCallAndCopyOrMove>(
                        *this, Library, "__ue_struct_map_with_enum_int", das::SideEffects::modifyExternal,
                        "StructMapWithEnumKeyInt")
                        ->needCallerStackFrame = true;
        AddUEExtern<DAS_BIND_FUN(StructMapGetEnumKeyFloat)>(*this, Library, "__ue_struct_map_get_enum_float",
                                                            das::SideEffects::modifyExternal,
                                                            "StructMapGetEnumKeyFloat");
        AddUEExtern<DAS_BIND_FUN(StructMapWithEnumKeyFloat), das::SimNode_ExtFuncCallAndCopyOrMove>(
                        *this, Library, "__ue_struct_map_with_enum_float", das::SideEffects::modifyExternal,
                        "StructMapWithEnumKeyFloat")
                        ->needCallerStackFrame = true;
        AddUEExtern<DAS_BIND_FUN(StructMapGetEnumKeyDouble)>(*this, Library, "__ue_struct_map_get_enum_double",
                                                             das::SideEffects::modifyExternal,
                                                             "StructMapGetEnumKeyDouble");
        AddUEExtern<DAS_BIND_FUN(StructMapWithEnumKeyDouble), das::SimNode_ExtFuncCallAndCopyOrMove>(
                        *this, Library, "__ue_struct_map_with_enum_double", das::SideEffects::modifyExternal,
                        "StructMapWithEnumKeyDouble")
                        ->needCallerStackFrame = true;
        AddUEExtern<DAS_BIND_FUN(StructMapGetEnumKeyBool)>(*this, Library, "__ue_struct_map_get_enum_bool",
                                                           das::SideEffects::modifyExternal, "StructMapGetEnumKeyBool");
        AddUEExtern<DAS_BIND_FUN(StructMapWithEnumKeyBool), das::SimNode_ExtFuncCallAndCopyOrMove>(
                        *this, Library, "__ue_struct_map_with_enum_bool", das::SideEffects::modifyExternal,
                        "StructMapWithEnumKeyBool")
                        ->needCallerStackFrame = true;
        AddUEExtern<DAS_BIND_FUN(StructMapGetEnumKeyString)>(*this, Library, "__ue_struct_map_get_enum_string",
                                                             das::SideEffects::modifyExternal,
                                                             "StructMapGetEnumKeyString");
        AddUEExtern<DAS_BIND_FUN(StructMapWithEnumKeyString), das::SimNode_ExtFuncCallAndCopyOrMove>(
                        *this, Library, "__ue_struct_map_with_enum_string", das::SideEffects::modifyExternal,
                        "StructMapWithEnumKeyString")
                        ->needCallerStackFrame = true;
        AddUEExtern<DAS_BIND_FUN(StructMapGetEnumKeyName)>(*this, Library, "__ue_struct_map_get_enum_name",
                                                           das::SideEffects::modifyExternal, "StructMapGetEnumKeyName");
        AddUEExtern<DAS_BIND_FUN(StructMapWithEnumKeyName), das::SimNode_ExtFuncCallAndCopyOrMove>(
                        *this, Library, "__ue_struct_map_with_enum_name", das::SideEffects::modifyExternal,
                        "StructMapWithEnumKeyName")
                        ->needCallerStackFrame = true;
        AddUEExtern<DAS_BIND_FUN(StructMapGetEnumKeyEnum)>(*this, Library, "__ue_struct_map_get_enum_enum",
                                                           das::SideEffects::modifyExternal, "StructMapGetEnumKeyEnum");
        AddUEExtern<DAS_BIND_FUN(StructMapWithEnumKeyEnum), das::SimNode_ExtFuncCallAndCopyOrMove>(
                        *this, Library, "__ue_struct_map_with_enum_enum", das::SideEffects::modifyExternal,
                        "StructMapWithEnumKeyEnum")
                        ->needCallerStackFrame = true;
        AddUEExtern<DAS_BIND_FUN(StructMapGetEnumKeyObject), das::SimNode_ExtFuncCallAndCopyOrMove>(
            *this, Library, "__ue_struct_map_get_enum_object", das::SideEffects::modifyExternal,
            "StructMapGetEnumKeyObject");
        AddUEExtern<DAS_BIND_FUN(StructMapWithEnumKeyObject), das::SimNode_ExtFuncCallAndCopyOrMove>(
                        *this, Library, "__ue_struct_map_with_enum_object", das::SideEffects::modifyExternal,
                        "StructMapWithEnumKeyObject")
                        ->needCallerStackFrame = true;
        AddUEExtern<DAS_BIND_FUN(StructMapGetEnumKeyStruct), das::SimNode_ExtFuncCallAndCopyOrMove>(
                        *this, Library, "__ue_struct_map_get_enum_struct", das::SideEffects::modifyExternal,
                        "StructMapGetEnumKeyStruct")
                        ->needCallerStackFrame = true;
        AddUEExtern<DAS_BIND_FUN(StructMapWithEnumKeyStruct), das::SimNode_ExtFuncCallAndCopyOrMove>(
                        *this, Library, "__ue_struct_map_with_enum_struct", das::SideEffects::modifyExternal,
                        "StructMapWithEnumKeyStruct")
                        ->needCallerStackFrame = true;
        AddUEExtern<DAS_BIND_FUN(StructSetElementEnum)>(*this, Library, "__ue_struct_set_element_enum",
                                                        das::SideEffects::modifyExternal, "StructSetElementEnum");
        AddUEExtern<DAS_BIND_FUN(StructSetWithEnum), das::SimNode_ExtFuncCallAndCopyOrMove>(
                        *this, Library, "__ue_struct_set_with_enum", das::SideEffects::modifyExternal,
                        "StructSetWithEnum")
                        ->needCallerStackFrame = true;
        AddUEExtern<DAS_BIND_FUN(StructSetElementName)>(*this, Library, "ue_struct_set_element_name",
                                                        das::SideEffects::modifyExternal, "StructSetElementName");
        AddUEExtern<DAS_BIND_FUN(StructSetWithName), das::SimNode_ExtFuncCallAndCopyOrMove>(
                        *this, Library, "ue_struct_set_with_name", das::SideEffects::modifyExternal,
                        "StructSetWithName")
                        ->needCallerStackFrame = true;
        AddUEExtern<DAS_BIND_FUN(StructMapNameKey)>(*this, Library, "ue_struct_map_key_name",
                                                    das::SideEffects::modifyExternal, "StructMapNameKey");
        AddUEExtern<DAS_BIND_FUN(StructMapGetNameStruct), das::SimNode_ExtFuncCallAndCopyOrMove>(
                        *this, Library, "__ue_struct_map_get_name_struct", das::SideEffects::modifyExternal,
                        "StructMapGetNameStruct")
                        ->needCallerStackFrame = true;
        AddUEExtern<DAS_BIND_FUN(StructMapWithNameStruct), das::SimNode_ExtFuncCallAndCopyOrMove>(
                        *this, Library, "__ue_struct_map_with_name_struct", das::SideEffects::modifyExternal,
                        "StructMapWithNameStruct")
                        ->needCallerStackFrame = true;
        AddUEExtern<DAS_BIND_FUN(StructMapGetNameEnum)>(*this, Library, "__ue_struct_map_get_name_enum",
                                                        das::SideEffects::modifyExternal, "StructMapGetNameEnum");
        AddUEExtern<DAS_BIND_FUN(StructMapWithNameEnum), das::SimNode_ExtFuncCallAndCopyOrMove>(
                        *this, Library, "__ue_struct_map_with_name_enum", das::SideEffects::modifyExternal,
                        "StructMapWithNameEnum")
                        ->needCallerStackFrame = true;
        AddUEExtern<DAS_BIND_FUN(StructMapGetNameObject), das::SimNode_ExtFuncCallAndCopyOrMove>(
            *this, Library, "__ue_struct_map_get_name_object", das::SideEffects::modifyExternal,
            "StructMapGetNameObject");
        AddUEExtern<DAS_BIND_FUN(StructMapWithNameObject), das::SimNode_ExtFuncCallAndCopyOrMove>(
                        *this, Library, "__ue_struct_map_with_name_object", das::SideEffects::modifyExternal,
                        "StructMapWithNameObject")
                        ->needCallerStackFrame = true;
        AddUEExtern<DAS_BIND_FUN(StructMapStringKey)>(*this, Library, "ue_struct_map_key_string",
                                                      das::SideEffects::modifyExternal, "StructMapStringKey");
        AddUEExtern<DAS_BIND_FUN(StructArraySize)>(*this, Library, "ue_struct_array_size",
                                                   das::SideEffects::modifyExternal, "StructArraySize");
        AddUEExtern<DAS_BIND_FUN(StructArrayResize), das::SimNode_ExtFuncCallAndCopyOrMove>(
                        *this, Library, "ue_struct_array_resize", das::SideEffects::modifyExternal, "StructArrayResize")
                        ->needCallerStackFrame = true;
        AddUEExtern<DAS_BIND_FUN(StructNestedGet), das::SimNode_ExtFuncCallAndCopyOrMove>(
                        *this, Library, "ue_struct_nested_get", das::SideEffects::modifyExternal, "StructNestedGet")
                        ->needCallerStackFrame = true;
        AddUEExtern<DAS_BIND_FUN(StructNestedWith), das::SimNode_ExtFuncCallAndCopyOrMove>(
                        *this, Library, "ue_struct_nested_with", das::SideEffects::modifyExternal, "StructNestedWith")
                        ->needCallerStackFrame = true;
        AddUEExtern<DAS_BIND_FUN(StructGetEnum)>(*this, Library, "ue_struct_get_enum", das::SideEffects::modifyExternal,
                                                 "StructGetEnum");
        AddUEExtern<DAS_BIND_FUN(StructWithEnum), das::SimNode_ExtFuncCallAndCopyOrMove>(
                        *this, Library, "ue_struct_with_enum", das::SideEffects::modifyExternal, "StructWithEnum")
                        ->needCallerStackFrame = true;
        AddUEExtern<DAS_BIND_FUN(StructGetClass)>(*this, Library, "__ue_struct_get_class",
                                                  das::SideEffects::modifyExternal, "StructGetClass");
        AddUEExtern<DAS_BIND_FUN(StructGetObject), das::SimNode_ExtFuncCallAndCopyOrMove>(
            *this, Library, "ue_struct_get_object", das::SideEffects::modifyExternal, "StructGetObject");
        AddUEExtern<DAS_BIND_FUN(StructWithObject), das::SimNode_ExtFuncCallAndCopyOrMove>(
                        *this, Library, "ue_struct_with_object", das::SideEffects::modifyExternal, "StructWithObject")
                        ->needCallerStackFrame = true;
        AddUEExtern<DAS_BIND_FUN(CollisionProfileName)>(*this, Library, "__ue_collision_profile_name",
                                                        das::SideEffects::modifyExternal, "CollisionProfileName");
        AddUEExtern<DAS_BIND_FUN(DSLNameFromString)>(*this, Library, "ue_name_value", das::SideEffects::modifyExternal,
                                                     "DSLNameFromString");
        AddUEExtern<DAS_BIND_FUN(DSLNameToString)>(*this, Library, "ue_name_text", das::SideEffects::modifyExternal,
                                                   "DSLNameToString");
#include "DasBuiltinValueBindings.inl"
        AddUEExtern<DAS_BIND_FUN(DSLNamesEqual)>(*this, Library, "ue_names_equal", das::SideEffects::accessExternal,
                                                 "DSLNamesEqual");
        AddUEExtern<DAS_BIND_FUN(EmitType)>(*this, Library, "ue_emit_type", das::SideEffects::modifyExternal,
                                            "EmitType");
        // Externs that may collect struct snapshots (needCallerStackFrame) stay closed for AOT: CollectStructValues
        // finds live tokens by walking interpreter frames, and a compiled caller's locals are not on that stack.
        functions.foreach(
            [](const das::FunctionPtr& Fn)
            {
                if (Fn->needCallerStackFrame)
                    Fn->noAot = true;
            });
    }
};
