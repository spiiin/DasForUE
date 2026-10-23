// Included in the foundation constructor, after its type annotations exist.
AddUEExtern<DAS_BIND_FUN(BuiltinAdd<FVector>), das::SimNode_ExtFuncCallAndCopyOrMove>(*this, Library, "+",
                                                                                      das::SideEffects::none,
                                                                                      "BuiltinAdd<FVector>");
AddUEExtern<DAS_BIND_FUN(BuiltinSubtract<FVector>), das::SimNode_ExtFuncCallAndCopyOrMove>(*this, Library, "-",
                                                                                           das::SideEffects::none,
                                                                                           "BuiltinSubtract<FVector>");
AddUEExtern<DAS_BIND_FUN(BuiltinNegate<FVector>), das::SimNode_ExtFuncCallAndCopyOrMove>(*this, Library, "-",
                                                                                         das::SideEffects::none,
                                                                                         "BuiltinNegate<FVector>");
AddUEExtern<DAS_BIND_FUN(BuiltinMultiply<FVector>), das::SimNode_ExtFuncCallAndCopyOrMove>(*this, Library, "*",
                                                                                           das::SideEffects::none,
                                                                                           "BuiltinMultiply<FVector>");
AddUEExtern<DAS_BIND_FUN(BuiltinMultiplyLeft<FVector>), das::SimNode_ExtFuncCallAndCopyOrMove>(
    *this, Library, "*", das::SideEffects::none, "BuiltinMultiplyLeft<FVector>");
AddUEExtern<DAS_BIND_FUN(BuiltinEqual<FVector>)>(*this, Library, "==", das::SideEffects::none, "BuiltinEqual<FVector>");
AddUEExtern<DAS_BIND_FUN(BuiltinNotEqual<FVector>)>(*this, Library, "!=", das::SideEffects::none,
                                                    "BuiltinNotEqual<FVector>");
AddUEExtern<DAS_BIND_FUN(BuiltinNearly<FVector>)>(*this, Library, "ue_nearly_equal", das::SideEffects::none,
                                                  "BuiltinNearly<FVector>");
AddUEExtern<DAS_BIND_FUN(BuiltinAddAssign<FVector>)>(*this, Library, "+=", das::SideEffects::modifyArgument,
                                                     "BuiltinAddAssign<FVector>");
AddUEExtern<DAS_BIND_FUN(BuiltinSubtractAssign<FVector>)>(*this, Library, "-=", das::SideEffects::modifyArgument,
                                                          "BuiltinSubtractAssign<FVector>");
AddUEExtern<DAS_BIND_FUN(BuiltinMultiplyAssign<FVector>)>(*this, Library, "*=", das::SideEffects::modifyArgument,
                                                          "BuiltinMultiplyAssign<FVector>");
AddUEExtern<DAS_BIND_FUN(BuiltinDivide<FVector>), das::SimNode_ExtFuncCallAndCopyOrMove>(*this, Library, "/",
                                                                                         das::SideEffects::none,
                                                                                         "BuiltinDivide<FVector>");
AddUEExtern<DAS_BIND_FUN(BuiltinDivideAssign<FVector>)>(*this, Library, "/=", das::SideEffects::modifyArgument,
                                                        "BuiltinDivideAssign<FVector>");
AddUEExtern<DAS_BIND_FUN(BuiltinLength<FVector>)>(*this, Library, "ue_length", das::SideEffects::none,
                                                  "BuiltinLength<FVector>");
AddUEExtern<DAS_BIND_FUN(BuiltinLengthSquared<FVector>)>(*this, Library, "ue_length_squared", das::SideEffects::none,
                                                         "BuiltinLengthSquared<FVector>");
AddUEExtern<DAS_BIND_FUN(BuiltinDot<FVector>)>(*this, Library, "ue_dot", das::SideEffects::none, "BuiltinDot<FVector>");
AddUEExtern<DAS_BIND_FUN(BuiltinDistance<FVector>)>(*this, Library, "ue_distance", das::SideEffects::none,
                                                    "BuiltinDistance<FVector>");
AddUEExtern<DAS_BIND_FUN(BuiltinNormalized<FVector>), das::SimNode_ExtFuncCallAndCopyOrMove>(
    *this, Library, "ue_normalized", das::SideEffects::none, "BuiltinNormalized<FVector>");
AddUEExtern<DAS_BIND_FUN(BuiltinAdd<FVector2D>), das::SimNode_ExtFuncCallAndCopyOrMove>(*this, Library, "+",
                                                                                        das::SideEffects::none,
                                                                                        "BuiltinAdd<FVector2D>");
AddUEExtern<DAS_BIND_FUN(BuiltinSubtract<FVector2D>), das::SimNode_ExtFuncCallAndCopyOrMove>(
    *this, Library, "-", das::SideEffects::none, "BuiltinSubtract<FVector2D>");
AddUEExtern<DAS_BIND_FUN(BuiltinNegate<FVector2D>), das::SimNode_ExtFuncCallAndCopyOrMove>(*this, Library, "-",
                                                                                           das::SideEffects::none,
                                                                                           "BuiltinNegate<FVector2D>");
AddUEExtern<DAS_BIND_FUN(BuiltinMultiply<FVector2D>), das::SimNode_ExtFuncCallAndCopyOrMove>(
    *this, Library, "*", das::SideEffects::none, "BuiltinMultiply<FVector2D>");
AddUEExtern<DAS_BIND_FUN(BuiltinMultiplyLeft<FVector2D>), das::SimNode_ExtFuncCallAndCopyOrMove>(
    *this, Library, "*", das::SideEffects::none, "BuiltinMultiplyLeft<FVector2D>");
AddUEExtern<DAS_BIND_FUN(BuiltinEqual<FVector2D>)>(*this, Library, "==", das::SideEffects::none,
                                                   "BuiltinEqual<FVector2D>");
AddUEExtern<DAS_BIND_FUN(BuiltinNotEqual<FVector2D>)>(*this, Library, "!=", das::SideEffects::none,
                                                      "BuiltinNotEqual<FVector2D>");
AddUEExtern<DAS_BIND_FUN(BuiltinNearly<FVector2D>)>(*this, Library, "ue_nearly_equal", das::SideEffects::none,
                                                    "BuiltinNearly<FVector2D>");
AddUEExtern<DAS_BIND_FUN(BuiltinAddAssign<FVector2D>)>(*this, Library, "+=", das::SideEffects::modifyArgument,
                                                       "BuiltinAddAssign<FVector2D>");
AddUEExtern<DAS_BIND_FUN(BuiltinSubtractAssign<FVector2D>)>(*this, Library, "-=", das::SideEffects::modifyArgument,
                                                            "BuiltinSubtractAssign<FVector2D>");
AddUEExtern<DAS_BIND_FUN(BuiltinMultiplyAssign<FVector2D>)>(*this, Library, "*=", das::SideEffects::modifyArgument,
                                                            "BuiltinMultiplyAssign<FVector2D>");
AddUEExtern<DAS_BIND_FUN(BuiltinDivide<FVector2D>), das::SimNode_ExtFuncCallAndCopyOrMove>(*this, Library, "/",
                                                                                           das::SideEffects::none,
                                                                                           "BuiltinDivide<FVector2D>");
AddUEExtern<DAS_BIND_FUN(BuiltinDivideAssign<FVector2D>)>(*this, Library, "/=", das::SideEffects::modifyArgument,
                                                          "BuiltinDivideAssign<FVector2D>");
AddUEExtern<DAS_BIND_FUN(BuiltinLength<FVector2D>)>(*this, Library, "ue_length", das::SideEffects::none,
                                                    "BuiltinLength<FVector2D>");
AddUEExtern<DAS_BIND_FUN(BuiltinLengthSquared<FVector2D>)>(*this, Library, "ue_length_squared", das::SideEffects::none,
                                                           "BuiltinLengthSquared<FVector2D>");
AddUEExtern<DAS_BIND_FUN(BuiltinDot<FVector2D>)>(*this, Library, "ue_dot", das::SideEffects::none,
                                                 "BuiltinDot<FVector2D>");
AddUEExtern<DAS_BIND_FUN(BuiltinDistance<FVector2D>)>(*this, Library, "ue_distance", das::SideEffects::none,
                                                      "BuiltinDistance<FVector2D>");
AddUEExtern<DAS_BIND_FUN(BuiltinNormalized<FVector2D>), das::SimNode_ExtFuncCallAndCopyOrMove>(
    *this, Library, "ue_normalized", das::SideEffects::none, "BuiltinNormalized<FVector2D>");
AddUEExtern<DAS_BIND_FUN(BuiltinAdd<FRotator>), das::SimNode_ExtFuncCallAndCopyOrMove>(*this, Library, "+",
                                                                                       das::SideEffects::none,
                                                                                       "BuiltinAdd<FRotator>");
AddUEExtern<DAS_BIND_FUN(BuiltinSubtract<FRotator>), das::SimNode_ExtFuncCallAndCopyOrMove>(
    *this, Library, "-", das::SideEffects::none, "BuiltinSubtract<FRotator>");
AddUEExtern<DAS_BIND_FUN(BuiltinNegate<FRotator>), das::SimNode_ExtFuncCallAndCopyOrMove>(*this, Library, "-",
                                                                                          das::SideEffects::none,
                                                                                          "BuiltinNegate<FRotator>");
AddUEExtern<DAS_BIND_FUN(BuiltinMultiply<FRotator>), das::SimNode_ExtFuncCallAndCopyOrMove>(
    *this, Library, "*", das::SideEffects::none, "BuiltinMultiply<FRotator>");
AddUEExtern<DAS_BIND_FUN(BuiltinMultiplyLeft<FRotator>), das::SimNode_ExtFuncCallAndCopyOrMove>(
    *this, Library, "*", das::SideEffects::none, "BuiltinMultiplyLeft<FRotator>");
AddUEExtern<DAS_BIND_FUN(BuiltinEqual<FRotator>)>(*this, Library, "==", das::SideEffects::none,
                                                  "BuiltinEqual<FRotator>");
AddUEExtern<DAS_BIND_FUN(BuiltinNotEqual<FRotator>)>(*this, Library, "!=", das::SideEffects::none,
                                                     "BuiltinNotEqual<FRotator>");
AddUEExtern<DAS_BIND_FUN(BuiltinNearly<FRotator>)>(*this, Library, "ue_nearly_equal", das::SideEffects::none,
                                                   "BuiltinNearly<FRotator>");
AddUEExtern<DAS_BIND_FUN(BuiltinAddAssign<FRotator>)>(*this, Library, "+=", das::SideEffects::modifyArgument,
                                                      "BuiltinAddAssign<FRotator>");
AddUEExtern<DAS_BIND_FUN(BuiltinSubtractAssign<FRotator>)>(*this, Library, "-=", das::SideEffects::modifyArgument,
                                                           "BuiltinSubtractAssign<FRotator>");
AddUEExtern<DAS_BIND_FUN(BuiltinMultiplyAssign<FRotator>)>(*this, Library, "*=", das::SideEffects::modifyArgument,
                                                           "BuiltinMultiplyAssign<FRotator>");
AddUEExtern<DAS_BIND_FUN(BuiltinCross), das::SimNode_ExtFuncCallAndCopyOrMove>(*this, Library, "ue_cross",
                                                                               das::SideEffects::none, "BuiltinCross");
AddUEExtern<DAS_BIND_FUN(BuiltinCross2D)>(*this, Library, "ue_cross", das::SideEffects::none, "BuiltinCross2D");
AddUEExtern<DAS_BIND_FUN(BuiltinRotationNormalized), das::SimNode_ExtFuncCallAndCopyOrMove>(
    *this, Library, "ue_normalized", das::SideEffects::none, "BuiltinRotationNormalized");
AddUEExtern<DAS_BIND_FUN(BuiltinComposeRotation), das::SimNode_ExtFuncCallAndCopyOrMove>(*this, Library,
                                                                                         "ue_compose_rotation",
                                                                                         das::SideEffects::none,
                                                                                         "BuiltinComposeRotation");
AddUEExtern<DAS_BIND_FUN(BuiltinRotate), das::SimNode_ExtFuncCallAndCopyOrMove>(*this, Library, "ue_rotate_vector",
                                                                                das::SideEffects::none,
                                                                                "BuiltinRotate");
AddUEExtern<DAS_BIND_FUN(BuiltinUnrotate), das::SimNode_ExtFuncCallAndCopyOrMove>(*this, Library, "ue_unrotate_vector",
                                                                                  das::SideEffects::none,
                                                                                  "BuiltinUnrotate");
AddUEExtern<DAS_BIND_FUN(BuiltinComposeTransform), das::SimNode_ExtFuncCallAndCopyOrMove>(*this, Library, "*",
                                                                                          das::SideEffects::none,
                                                                                          "BuiltinComposeTransform");
AddUEExtern<DAS_BIND_FUN(BuiltinInverse), das::SimNode_ExtFuncCallAndCopyOrMove>(*this, Library, "ue_transform_inverse",
                                                                                 das::SideEffects::none,
                                                                                 "BuiltinInverse");
AddUEExtern<DAS_BIND_FUN(BuiltinRelative), das::SimNode_ExtFuncCallAndCopyOrMove>(*this, Library,
                                                                                  "ue_transform_relative",
                                                                                  das::SideEffects::none,
                                                                                  "BuiltinRelative");
AddUEExtern<DAS_BIND_FUN(BuiltinTransformVector), das::SimNode_ExtFuncCallAndCopyOrMove>(*this, Library,
                                                                                         "ue_transform_vector",
                                                                                         das::SideEffects::none,
                                                                                         "BuiltinTransformVector");
AddUEExtern<DAS_BIND_FUN(BuiltinTransformDirection), das::SimNode_ExtFuncCallAndCopyOrMove>(
    *this, Library, "ue_transform_direction", das::SideEffects::none, "BuiltinTransformDirection");
AddUEExtern<DAS_BIND_FUN(BuiltinInversePosition), das::SimNode_ExtFuncCallAndCopyOrMove>(
    *this, Library, "ue_inverse_transform_position", das::SideEffects::none, "BuiltinInversePosition");
AddUEExtern<DAS_BIND_FUN(BuiltinInverseVector), das::SimNode_ExtFuncCallAndCopyOrMove>(*this, Library,
                                                                                       "ue_inverse_transform_vector",
                                                                                       das::SideEffects::none,
                                                                                       "BuiltinInverseVector");
AddUEExtern<DAS_BIND_FUN(BuiltinComposeTransformAssign)>(*this, Library, "*=", das::SideEffects::modifyArgument,
                                                         "BuiltinComposeTransformAssign");
AddUEExtern<DAS_BIND_FUN(BuiltinNearly<FTransform>)>(*this, Library, "ue_nearly_equal", das::SideEffects::none,
                                                     "BuiltinNearly<FTransform>");
AddUEExtern<DAS_BIND_FUN(DSLNamesEqual)>(*this, Library, "==", das::SideEffects::none, "DSLNamesEqual");
AddUEExtern<DAS_BIND_FUN(BuiltinNamesNotEqual)>(*this, Library, "!=", das::SideEffects::none, "BuiltinNamesNotEqual");

AddUEExtern<DAS_BIND_FUN(BuiltinQuatFromRotator), das::SimNode_ExtFuncCallAndCopyOrMove>(*this, Library, "ue_quat",
                                                                                         das::SideEffects::none,
                                                                                         "BuiltinQuatFromRotator");
AddUEExtern<DAS_BIND_FUN(BuiltinRotatorFromQuat), das::SimNode_ExtFuncCallAndCopyOrMove>(*this, Library, "ue_rotator",
                                                                                         das::SideEffects::none,
                                                                                         "BuiltinRotatorFromQuat");
AddUEExtern<DAS_BIND_FUN(BuiltinQuat), das::SimNode_ExtFuncCallAndCopyOrMove>(*this, Library, "ue_quat",
                                                                              das::SideEffects::none, "BuiltinQuat");
AddUEExtern<DAS_BIND_FUN(BuiltinQuatIdentity), das::SimNode_ExtFuncCallAndCopyOrMove>(*this, Library,
                                                                                      "ue_quat_identity",
                                                                                      das::SideEffects::none,
                                                                                      "BuiltinQuatIdentity");
