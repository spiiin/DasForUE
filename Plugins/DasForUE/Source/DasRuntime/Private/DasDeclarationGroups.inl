// Consume declaration groups before inference. Their labels are syntax containers,
// not native fields; every member goes through the ordinary UE field collector.
void DSLExpandDeclarationGroups(const das::StructurePtr& S,bool ComponentsAllowed) {
    using Field=das::Structure::FieldDeclaration;
    das::vector<Field> Expanded;
    TSet<FString> Names;
    for(const auto& F:S->fields)Names.Add(DSLText(F.name).ToLower());
    for(const auto& F:S->fields){
        das::string Name;das::vector<das::ExpressionPtr> Arguments;
        das::MakeStructPtr Named=nullptr;
        if(F.init && F.init->rtti_isCall()){
            auto* Call=static_cast<das::ExprCall*>(F.init);Name=Call->name;Arguments=Call->arguments;
        }else if(F.init && F.init->rtti_isNamedCall()){
            auto* Call=static_cast<das::ExprNamedCall*>(F.init);Name=Call->name;Arguments=Call->nonNamedArguments;Named=Call->arguments;
        }
        if(Name.find("ue_dsl::")==0)Name=Name.substr(8);
        const bool Components=Name=="ue_components";
        if(Name!="ue_properties" && !Components){Expanded.push_back(F);continue;}
        if(Components && !ComponentsAllowed)DSLFail(TEXT("ue_components requires ue_class"));
        if(!F.annotation.empty() || F.classMethod || F.generated)DSLFail(TEXT("Declaration group labels cannot carry field annotations"));
        if(Arguments.empty() || !Arguments.back() || !Arguments.back()->rtti_isMakeBlock())DSLFail(TEXT("Declaration group requires a trailing block"));
        auto* Closure=static_cast<das::ExprMakeBlock*>(Arguments.back());
        auto* Block=Closure->block && Closure->block->rtti_isBlock()?static_cast<das::ExprBlock*>(Closure->block):nullptr;
        if(!Block || !Block->arguments.empty() || !Block->finalList.empty())DSLFail(TEXT("Declaration group requires a plain declaration block"));
        das::AnnotationArgumentList Shared;
        TSet<FString> SharedKeys;
        for(size_t I=0;I+1<Arguments.size();++I){
            auto* E=Arguments[I];if(!E || !E->rtti_isVar())DSLFail(TEXT("Declaration group flags must be bare names"));
            const auto Flag=static_cast<das::ExprVar*>(E)->name;
            if(Flag!="EditAnywhere" && Flag!="BlueprintReadWrite" && Flag!="BlueprintReadOnly" && !(!Components && Flag=="BlueprintAssignable") && !(Components && Flag=="VisibleAnywhere"))DSLFail(TEXT("Unsupported declaration group flag: ")+DSLText(Flag));
            if(SharedKeys.Contains(DSLText(Flag)))DSLFail(TEXT("Repeated declaration group flag: ")+DSLText(Flag));
            SharedKeys.Add(DSLText(Flag));Shared.push_back(das::AnnotationArgument("ue_property",Flag));
        }
        if(Named)for(const auto* Arg:*Named){
            if(!(DSLIsMetadata(Arg->name) || (!Components && Arg->name=="AssetBundles")) || !Arg->value || Arg->value->__rtti!=das::string("ExprConstString"))DSLFail(TEXT("Declaration group metadata requires Category/DisplayName/ToolTip string literals"));
            if(SharedKeys.Contains(DSLText(Arg->name)))DSLFail(TEXT("Repeated declaration group metadata: ")+DSLText(Arg->name));
            SharedKeys.Add(DSLText(Arg->name));Shared.push_back(das::AnnotationArgument(Arg->name,static_cast<das::ExprConstString*>(Arg->value)->text));
        }
        if(SharedKeys.Contains(TEXT("BlueprintReadOnly")) && SharedKeys.Contains(TEXT("BlueprintReadWrite")))DSLFail(TEXT("Conflicting declaration group read flags"));
        if(SharedKeys.Contains(TEXT("EditAnywhere")) && SharedKeys.Contains(TEXT("VisibleAnywhere")))DSLFail(TEXT("Conflicting declaration group visibility flags"));
        if(Block->list.empty())DSLFail(TEXT("Declaration group cannot be empty"));
        for(auto* E:Block->list){
            if(!E || E->__rtti!=das::string("ExprLet"))DSLFail(TEXT("Declaration groups accept only var field declarations; executable statements are forbidden"));
            for(const auto& V:static_cast<das::ExprLet*>(E)->variables){
                if(!V->type || V->type->baseType==das::Type::autoinfer || V->type->ref || V->type->constant)DSLFail(TEXT("Grouped fields require var and explicit by-value types: ")+DSLText(V->name));
                if(Names.Contains(DSLText(V->name).ToLower()))DSLFail(TEXT("Duplicate grouped field: ")+DSLText(V->name));
                Names.Add(DSLText(V->name).ToLower());
                if(V->init && V->init->rtti_isCall()){
                    const auto N=static_cast<das::ExprCall*>(V->init)->name;
                    if(N=="ue_properties" || N=="ue_components")DSLFail(TEXT("Nested declaration groups are not supported"));
                }
                auto Annotations=Shared;
                // Per-member metadata overrides the shared value; flags accumulate
                // and are validated by the same collector as individual annotations.
                for(const auto& A:V->annotation){
                    if(DSLIsMetadata(A.name) || A.name=="AssetBundles")Annotations.erase(std::remove_if(Annotations.begin(),Annotations.end(),[&](const auto& Prior){return Prior.name==A.name;}),Annotations.end());
                    Annotations.push_back(A);
                }
                Annotations.push_back(das::AnnotationArgument("ue_property",true));
                if(Components)Annotations.push_back(das::AnnotationArgument("ue_component",true));
                Field Member;Member.flags=0;Member.name=V->name;Member.type=DSLCopyType(V->type);Member.init=V->init?V->init->clone():nullptr;Member.at=V->at;Member.annotation=Annotations;
                Expanded.push_back(Member);
            }
        }
    }
    S->fields=std::move(Expanded);
}
