    {
        FDasAuthoringModule Docs;FString Issue;
        const bool OK=Runtime.ReadAuthoringScript(Runtime.ScriptPath(TEXT("dsl_comment_tooltips.das")),Docs,Issue);
        Expect(OK,TEXT("comment_tooltips_compile"),Issue);
        if(OK){
            FDasAuthoringModule Interfaces;FString InterfaceIssue;const bool InterfaceOK=Runtime.ReadAuthoredInterfaces(Runtime.ScriptPath(TEXT("dsl_comment_tooltips.das")),Interfaces,InterfaceIssue);
            const auto* Contract=Interfaces.Classes.FindByPredicate([](const auto& V){return V.Name==TEXT("DocContract");});
            Expect(InterfaceOK && Contract && Contract->Metadata.FindRef(TEXT("ToolTip"))==TEXT("Shared query contract.") && Contract->Methods.Num()==1 && Contract->Methods[0].Metadata.FindRef(TEXT("ToolTip"))==TEXT("Reads the current score."),TEXT("comment_tooltips_interface_and_stub"),InterfaceIssue);
            const auto& C=Docs.Classes[0];auto Tip=[&](const TCHAR* Name){const auto* P=C.Properties.FindByPredicate([&](const auto& V){return V.Name==Name;});return P?P->Metadata.FindRef(TEXT("ToolTip")):FString();};
            Expect(C.Metadata.FindRef(TEXT("ToolTip"))==TEXT("Счётчик игрока.\nTwo documentation lines."),TEXT("comment_tooltips_class_block_utf8"));
            Expect(Tip(TEXT("Score"))==TEXT("Текущие очки.") && Tip(TEXT("Speed"))==TEXT("Movement speed.") && Tip(TEXT("Title"))==TEXT("Название объекта."),TEXT("comment_tooltips_group_and_annotated_fields"),FString::Printf(TEXT("Score=%s | Speed=%s | Title=%s"),*Tip(TEXT("Score")),*Tip(TEXT("Speed")),*Tip(TEXT("Title"))));
            Expect(Tip(TEXT("Explicit"))==TEXT("Explicit tooltip") && Tip(TEXT("Detached")).IsEmpty() && Tip(TEXT("Next")).IsEmpty(),TEXT("comment_tooltips_explicit_override_no_detached_or_trailing_leaks"));
            Expect(C.Methods[0].Metadata.FindRef(TEXT("ToolTip"))==TEXT("Возвращает сумму.\nKeeps line breaks."),TEXT("comment_tooltips_reflected_function"),C.Methods[0].Metadata.FindRef(TEXT("ToolTip")));
        }
    }
    {
        FDasReflectedTypes Types;FString Issue;
        const bool OK=Runtime.ReadReflectedTypes(Runtime.ScriptPath(TEXT("dsl_comment_types.das")),Types,Issue);
        Expect(OK,TEXT("comment_tooltips_types_compile"),Issue);
        Expect(OK && Types.Enums.Num()==1 && Types.Structs.Num()==1 && Types.Enums[0].Metadata.FindRef(TEXT("ToolTip"))==TEXT("Current operating mode.") && Types.Structs[0].Metadata.FindRef(TEXT("ToolTip"))==TEXT("Editable score settings.") && Types.Structs[0].Properties[0].Metadata.FindRef(TEXT("ToolTip"))==TEXT("Initial score."),TEXT("comment_tooltips_enum_struct_and_field"));
    }
