FString DSLNativeBasePath(const das::StructurePtr& St);
// Unqualified name of a class method function (after the last ` or ' separator).
das::string DSLMethodName(const das::FunctionPtr& F)
{
    return F->name.substr(F->name.find_last_of(das::string(1, char(96)) + das::string(1, char(39))) + 1);
}
// Nearest declaration wins; never combine an override with its ancestor overloads.
template <class Visitor> void DSLVisitHandlerMethods(const das::StructurePtr& St, Visitor Visit)
{
    TSet<FString> Selected;
    for (auto Owner = St; Owner && DSLNativeBasePath(Owner).IsEmpty(); Owner = Owner->parent)
    {
        TSet<FString> Local;
        Owner->module->functions.foreach (
            [&](const das::FunctionPtr& F)
            {
                if (F->classParent != Owner)
                    return;
                const auto Name = DSLText(DSLMethodName(F));
                if (!Selected.Contains(Name))
                {
                    Local.Add(Name);
                    Visit(F);
                }
            });
        Selected.Append(Local);
    }
}
das::FunctionPtr DSLFindHandlerMethod(const das::StructurePtr& St, const FString& Name)
{
    das::FunctionPtr Found = nullptr;
    DSLVisitHandlerMethods(St,
                           [&](const das::FunctionPtr& F)
                           {
                               if (DSLText(DSLMethodName(F)) == Name)
                               {
                                   if (Found)
                                       DSLFail(TEXT("Ambiguous callback method: ") + Name);
                                   Found = F;
                               }
                           });
    return Found;
}
das::string DSLCallbackEntry(const FDasDeclaredClass& C, const FString& Name)
{
    // Inline callbacks retain their entry identities and existing reload transfer policy.
    return DSLUtf8((Name.StartsWith(TEXT("__ue_cb_")) ? TEXT("ue_") : TEXT("__ue_dispatch_")) + C.Name +
                    TEXT("_") + Name);
}
