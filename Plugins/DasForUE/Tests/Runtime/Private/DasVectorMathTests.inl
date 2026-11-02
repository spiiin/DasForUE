{
    const auto VectorMath = Run(TEXT("vector_math_check.das"));
    Expect(VectorMath.bSucceeded && VectorMath.Value == 42,
           TEXT("vector_math_constants_all_swizzles_aliasing_and_precision"), VectorMath.Error);
    const auto RepeatedWrite = Run(TEXT("vector_swizzle_bad_write.das"));
    Expect(!RepeatedWrite.bSucceeded, TEXT("vector_swizzle_repeated_write_rejected"), RepeatedWrite.Error);
    FDasAuthoringModule NamedVectorModel, LiteralVectorModel;
    FString VectorDefaultsIssue;
    const bool NamedVectorOK = Runtime.ReadAuthoringScript(Runtime.ScriptPath(TEXT("dsl_vector_constant_defaults.das")),
                                                                              NamedVectorModel, VectorDefaultsIssue);
    Expect(NamedVectorOK && NamedVectorModel.Classes.Num() == 1, TEXT("vector_named_constants_defaults_compile"),
                                                                      VectorDefaultsIssue);
    const bool LiteralVectorOK = Runtime.ReadAuthoringScript(
        Runtime.ScriptPath(TEXT("dsl_vector_literal_defaults.das")), LiteralVectorModel, VectorDefaultsIssue);
    Expect(LiteralVectorOK && LiteralVectorModel.Classes.Num() == 1, TEXT("vector_literal_defaults_compile"),
                                                                          VectorDefaultsIssue);
    if (NamedVectorOK && LiteralVectorOK && NamedVectorModel.Classes.Num() == 1 &&
        LiteralVectorModel.Classes.Num() == 1)
    {
        const auto& Named = NamedVectorModel.Classes[0];
        const auto& Literal = LiteralVectorModel.Classes[0];
        Expect(Named.Components.Num() == 1 && Literal.Components.Num() == 1 &&
                   Named.Components[0].Defaults.OrderIndependentCompareEqual(Literal.Components[0].Defaults) &&
                   Named.Properties.Num() == 1 && Literal.Properties.Num() == 1 &&
                   Named.Properties[0].Default == Literal.Properties[0].Default,
               TEXT("vector_named_constants_defaults_equal_literal_defaults"));
    }
    const bool MutableVectorOK = Runtime.ReadAuthoringScript(
        Runtime.ScriptPath(TEXT("dsl_vector_constant_bad_mutable.das")), NamedVectorModel, VectorDefaultsIssue);
    Expect(!MutableVectorOK && VectorDefaultsIssue.Contains(TEXT("pure scalar helpers")),
                                                            TEXT("vector_mutable_global_default_rejected"),
                                                                 VectorDefaultsIssue);
    const bool FunctionVectorOK = Runtime.ReadAuthoringScript(
        Runtime.ScriptPath(TEXT("dsl_vector_constant_bad_call.das")), NamedVectorModel, VectorDefaultsIssue);
    Expect(!FunctionVectorOK && VectorDefaultsIssue.Contains(TEXT("pure scalar helpers")),
                                                             TEXT("vector_function_global_default_rejected"),
                                                                  VectorDefaultsIssue);
}
