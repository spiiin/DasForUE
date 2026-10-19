{
    FDasAuthoringModule Grouped, Annotated;
    FString Issue, GroupedJson, AnnotatedJson;
    const bool GroupOK =
        Runtime.ReadAuthoringScript(Runtime.ScriptPath(TEXT("dsl_declaration_groups.das")), Grouped, Issue);
    Expect(GroupOK, TEXT("declaration_groups_compile"), Issue);
    const bool AnnotatedOK =
        Runtime.ReadAuthoringScript(Runtime.ScriptPath(TEXT("dsl_declaration_groups_annotated.das")), Annotated, Issue);
    Expect(GroupOK && AnnotatedOK && Grouped.ToJson(GroupedJson, Issue) && Annotated.ToJson(AnnotatedJson, Issue) &&
           GroupedJson.Replace(TEXT("dsl_declaration_groups.das"), TEXT("dsl_declaration_groups_annotated.das")) ==
                                    AnnotatedJson,
                               TEXT("declaration_groups_have_identical_IR_and_stable_IDs"), Issue);
    if (GroupOK)
    {
        const auto& C = Grouped.Classes[0];
        const auto* GroupScore = C.Properties.FindByPredicate([](const auto& P) { return P.Name == TEXT("Score"); });
        const auto* Speed = C.Properties.FindByPredicate([](const auto& P) { return P.Name == TEXT("Speed"); });
        const auto* Title = C.Properties.FindByPredicate([](const auto& P) { return P.Name == TEXT("Title"); });
        Expect(C.Properties.Num() == 4 && GroupScore && Speed && Title &&
               GroupScore->Metadata.FindRef(
                   TEXT("Category")) ==
                   TEXT("State") &&
                        Title->Metadata.FindRef(
                            TEXT("Category")) ==
                            TEXT("Special") &&
                                 Title->Metadata.FindRef(
                                     TEXT("ToolTip")) ==
                                     TEXT("Individual tooltip") && Speed->Default.Number == 100,
                                          TEXT("declaration_groups_expand_fields_without_container_property"));
    }
    FDasReflectedTypes Types;
    Expect(Runtime.ReadReflectedTypes(Runtime.ScriptPath(TEXT("ue_grouped_struct.das")), Types, Issue) &&
                                      Types.Structs.Num() == 1 && Types.Structs[0].Properties.Num() == 2,
                                      TEXT("declaration_groups_collect_struct_fields"), Issue);
    for (const auto* Name :
         {TEXT("duplicate"), TEXT("flags"), TEXT("statement"), TEXT("empty"), TEXT("inferred"),
                                                                                   TEXT("conflict"),
                                                                                        TEXT("local_conflict")})
    {
        FDasAuthoringModule Bad;
        FString Error;
        const FString File = TEXT("dsl_group_bad_")+ FString(Name) + TEXT(".das");
        Expect(!Runtime.ReadAuthoringScript(Runtime.ScriptPath(File), Bad, Error) && !Error.IsEmpty(),
               *(TEXT("declaration_groups_reject_")+ FString(Name)), Error);
    }
}
