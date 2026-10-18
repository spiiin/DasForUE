namespace
{
    template<typename T> TSharedPtr<FJsonObject> ExportMetadata(const T* Field)
    {
        auto Result = MakeShared<FJsonObject>();
#if WITH_EDITOR
        for (const TCHAR* Key : {TEXT("Category"), TEXT("DisplayName"), TEXT("ToolTip"), TEXT("Units"), TEXT("ClampMin"), TEXT("ClampMax"), TEXT("UIMin"), TEXT("UIMax"), TEXT("DeprecatedFunction"), TEXT("DeprecationMessage"), TEXT("BlueprintInternalUseOnly"), TEXT("UnsafeDuringActorConstruction")})
            if (Field->HasMetaData(Key)) Result->SetStringField(Key, Field->GetMetaData(Key));
#endif
        return Result;
    }
    void ExportPolicyError(const FString& Message) { throw std::runtime_error(TCHAR_TO_UTF8(*Message)); }
    void CheckSelectionKeys(const TSharedPtr<FJsonObject>& Object, std::initializer_list<const TCHAR*> Allowed)
    {
        for (const auto& Pair : Object->Values)
        {
            bool bKnown = false;
            for (const TCHAR* Key : Allowed) if (Pair.Key == Key) bKnown = true;
            if (!bKnown) ExportPolicyError(FString(TEXT("Unknown selection key: ")) + FString(Pair.Key));
        }
    }
    bool PolicyBool(const TSharedPtr<FJsonObject>& Object, const TCHAR* Key, bool Default)
    {
        bool Value = Default;
        if (Object->HasField(Key) && (!Object->HasTypedField<EJson::Boolean>(Key) || !Object->TryGetBoolField(Key, Value))) ExportPolicyError(FString(Key) + TEXT(" must be boolean"));
        return Value;
    }
    TArray<FString> PolicyPatterns(const TSharedPtr<FJsonObject>& Object, const TCHAR* Key)
    {
        TArray<FString> Result;
        const TArray<TSharedPtr<FJsonValue>>* Values;
        if (!Object->HasField(Key)) return Result;
        if (!Object->TryGetArrayField(Key, Values)) ExportPolicyError(FString(Key) + TEXT(" must be an array"));
        for (const auto& Value : *Values)
        {
            FString Pattern;
            if (Value->Type != EJson::String || !Value->TryGetString(Pattern) || Pattern.IsEmpty()) ExportPolicyError(TEXT("Expected nonempty pattern"));
            Result.Add(Pattern);
        }
        return Result;
    }
    bool MatchesPolicy(const FString& Name, const TArray<FString>& Patterns)
    {
        for (const auto& Pattern : Patterns) if (Name.MatchesWildcard(Pattern, ESearchCase::CaseSensitive)) return true;
        return false;
    }
    TSharedPtr<FJsonObject> ExportParameterType(FProperty* Property, TMap<FString, TSharedPtr<FJsonObject>>& Structs)
    {
        // Soft references have a snapshot representation, but no generic wrapper ABI yet.
        if (CastField<FSoftObjectProperty>(Property)) ExportPolicyError(TEXT("soft_reference_requires_binding_adapter"));
        auto Field = MakeShared<FJsonObject>();
        Field->SetStringField(TEXT("path"), Property->GetName().ToLower());
        Field->SetObjectField(TEXT("metadata"), ExportMetadata(Property));
        if(CastField<FWeakObjectProperty>(Property))Field->SetBoolField(TEXT("weak_reference"),true);
        if (auto* Map = CastField<FMapProperty>(Property))
        {
            Field->SetStringField(TEXT("kind"), TEXT("map"));
            Field->SetStringField(TEXT("type_path"), Map->KeyProp->GetClass()->GetName() + TEXT(";") + Map->ValueProp->GetClass()->GetName());
            Field->SetObjectField(TEXT("key"), ExportParameterType(Map->KeyProp, Structs));
            Field->SetObjectField(TEXT("value"), ExportParameterType(Map->ValueProp, Structs));
        }
        else if (auto* Set = CastField<FSetProperty>(Property))
        {
            Field->SetStringField(TEXT("kind"), TEXT("set"));
            Field->SetStringField(TEXT("type_path"), Set->ElementProp->GetClass()->GetName());
            Field->SetObjectField(TEXT("element"), ExportParameterType(Set->ElementProp, Structs));
        }
        else if (auto* Array = CastField<FArrayProperty>(Property))
        {
            Field->SetStringField(TEXT("kind"), TEXT("array"));
            Field->SetStringField(TEXT("type_path"), Array->Inner->GetClass()->GetName());
            Field->SetObjectField(TEXT("element"), ExportParameterType(Array->Inner, Structs));
        }
        else if (auto* StructProperty = CastField<FStructProperty>(Property))
        {
            UScriptStruct* Type = StructProperty->Struct;
            const FString Path = Type->GetPathName();
            Field->SetStringField(TEXT("kind"), TEXT("struct"));
            Field->SetStringField(TEXT("type_path"), Path);
            if(DasIsGeometryType(Type)){
                Field->SetStringField(TEXT("transport"),TEXT("slate_geometry_snapshot_v1"));
                if(!Structs.Contains(Path)){
                    auto Definition=MakeShared<FJsonObject>();Definition->SetStringField(TEXT("type_path"),Path);Definition->SetStringField(TEXT("cpp_type"),Type->GetStructCPPName());
                    Definition->SetStringField(TEXT("transport"),TEXT("slate_geometry_snapshot_v1"));Definition->SetArrayField(TEXT("fields"),{});Structs.Add(Path,Definition);
                }return Field;
            }
            if(DasIsInputEventType(Type)){
                Field->SetStringField(TEXT("transport"),TEXT("slate_input_event_snapshot_v1"));
                if(!Structs.Contains(Path)){
                    auto Definition=MakeShared<FJsonObject>();Definition->SetStringField(TEXT("type_path"),Path);Definition->SetStringField(TEXT("cpp_type"),Type->GetStructCPPName());
                    Definition->SetStringField(TEXT("transport"),TEXT("slate_input_event_snapshot_v1"));Definition->SetArrayField(TEXT("fields"),{});Structs.Add(Path,Definition);
                }return Field;
            }
            if(Type==FInputActionValue::StaticStruct()){
                Field->SetStringField(TEXT("transport"),TEXT("input_action_value_v1"));
                if(!Structs.Contains(Path)){
                    auto Definition=MakeShared<FJsonObject>();Definition->SetStringField(TEXT("type_path"),Path);
                    Definition->SetStringField(TEXT("cpp_type"),TEXT("FInputActionValue"));
                    Definition->SetStringField(TEXT("transport"),TEXT("input_action_value_v1"));
                    Definition->SetArrayField(TEXT("fields"),{});Structs.Add(Path,Definition);
                }return Field;
            }
            if(Type==FConstraintInstanceAccessor::StaticStruct()){
                Field->SetStringField(TEXT("transport"),TEXT("constraint_component_v1"));
                if(!Structs.Contains(Path)){
                    auto Definition=MakeShared<FJsonObject>();Definition->SetStringField(TEXT("type_path"),Path);Definition->SetStringField(TEXT("cpp_type"),TEXT("FConstraintInstanceAccessor"));
                    Definition->SetStringField(TEXT("transport"),TEXT("constraint_component_v1"));Definition->SetArrayField(TEXT("fields"),{});Structs.Add(Path,Definition);
                }return Field;
            }
            if(Type==FHitResult::StaticStruct()){
                Field->SetStringField(TEXT("transport"),TEXT("hit_result_v1"));
                if(!Structs.Contains(Path)){
                    auto Definition=MakeShared<FJsonObject>();Definition->SetStringField(TEXT("type_path"),Path);
                    Definition->SetStringField(TEXT("cpp_type"),TEXT("FHitResult"));
                    Definition->SetStringField(TEXT("transport"),TEXT("hit_result_v1"));
                    Definition->SetArrayField(TEXT("fields"),{});Structs.Add(Path,Definition);
                }
                return Field;
            }
            if (!Structs.Contains(Path))
            {
                auto Definition = MakeShared<FJsonObject>();
                Definition->SetStringField(TEXT("type_path"), Path);
                Definition->SetStringField(TEXT("cpp_type"), Type->GetStructCPPName());
                Definition->SetObjectField(TEXT("metadata"), ExportMetadata(Type));
                Structs.Add(Path, Definition);
                TArray<TSharedPtr<FJsonValue>> Fields;
                for (TFieldIterator<FProperty> It(Type); It; ++It)
                    Fields.Add(MakeShared<FJsonValueObject>(ExportParameterType(*It, Structs)));
                Definition->SetArrayField(TEXT("fields"), Fields);
            }
        }
        else
        {
            // DescribeCall already validated every leaf before this helper is called.
            FUnrealDasContext Context(4096);
            Field->SetStringField(TEXT("kind"), CallKindName(FieldKind(Property, &Context)));
            FString Path;
            if(auto* Delegate=CastField<FDelegateProperty>(Property))Path=Delegate->SignatureFunction->GetPathName();
            else if(auto* Class=CastField<FClassProperty>(Property))Path=Class->MetaClass->GetPathName();
            else if (UEnum* Enum = CallEnumType(Property)) Path = Enum->GetPathName();
            else if (auto* ReferenceClass = DasReferencePropertyClass(Property)) Path = ReferenceClass->GetPathName();
            Field->SetStringField(TEXT("type_path"), Path);
        }
        return Field;
    }
}

// Reflection exporter shares validation and signature keys with the runtime marshaler.
bool FDasRuntimeModule::ExportBindings(const FString& SelectionPath, const FString& OutputPath, FString& Error)
{
#if !WITH_EDITOR
    Error = TEXT("Binding export requires editor metadata; use the DasExport commandlet");
    return false;
#endif
    if (!IsInGameThread()) { Error = TEXT("Export requires the game thread"); return false; }
    FString Source;
    TSharedPtr<FJsonObject> Selection;
    if (!FFileHelper::LoadFileToString(Source, *SelectionPath) ||
        !FJsonSerializer::Deserialize(TJsonReaderFactory<>::Create(Source), Selection) || !Selection.IsValid())
    { Error = TEXT("Cannot read selection JSON"); return false; }
    TArray<TSharedPtr<FJsonValue>> Requests;
    TArray<TSharedPtr<FJsonValue>> Diagnostics;
    TSet<FString> Excluded;
    auto RootObject = MakeShared<FJsonObject>();
    RootObject->SetNumberField(TEXT("schema_version"), 5);
    RootObject->SetStringField(TEXT("engine_version"), FEngineVersion::Current().ToString());
    TArray<TSharedPtr<FJsonValue>> Functions;
    TSet<FString> Aliases;
    TMap<FString, UClass*> Classes;
    auto AddClass = [&](UClass* Class) { for (; Class; Class = Class->GetSuperClass()) Classes.Add(Class->GetPathName(), Class); };
    TMap<FString, TSharedPtr<FJsonObject>> Structs;
    try
    {
        CheckSelectionKeys(Selection, {TEXT("functions"), TEXT("class_rules"), TEXT("policy")});
        const TArray<TSharedPtr<FJsonValue>>* Explicit;
        if (Selection->HasField(TEXT("functions")))
        {
            if (!Selection->TryGetArrayField(TEXT("functions"), Explicit)) ExportPolicyError(TEXT("functions must be an array"));
            Requests = *Explicit;
        }
        TSharedPtr<FJsonObject> Policy = MakeShared<FJsonObject>();
        if (Selection->HasField(TEXT("policy")))
        {
            const TSharedPtr<FJsonObject>* Object;
            if (!Selection->TryGetObjectField(TEXT("policy"), Object)) ExportPolicyError(TEXT("policy must be an object"));
            Policy = *Object;
        }
        CheckSelectionKeys(Policy, {TEXT("allow_deprecated"), TEXT("allow_internal")});
        const bool bAllowDeprecated = PolicyBool(Policy, TEXT("allow_deprecated"), false);
        const bool bAllowInternal = PolicyBool(Policy, TEXT("allow_internal"), false);
        const TArray<TSharedPtr<FJsonValue>>* Rules;
        if (Selection->HasField(TEXT("class_rules")))
        {
            if (!Selection->TryGetArrayField(TEXT("class_rules"), Rules)) ExportPolicyError(TEXT("class_rules must be an array"));
            for (const auto& Value : *Rules)
            {
                const TSharedPtr<FJsonObject>* Rule;
                if (!Value->TryGetObject(Rule)) ExportPolicyError(TEXT("class rule must be an object"));
                CheckSelectionKeys(*Rule, {TEXT("class"), TEXT("prefix"), TEXT("include"), TEXT("exclude"), TEXT("include_inherited")});
                FString ClassPath, Prefix;
                if (!(*Rule)->TryGetStringField(TEXT("class"), ClassPath) || !(*Rule)->TryGetStringField(TEXT("prefix"), Prefix) || Prefix.IsEmpty()) ExportPolicyError(TEXT("class rule needs class and prefix"));
                UClass* Class = FindObject<UClass>(nullptr, *ClassPath);
            if(!Class && ClassPath.StartsWith(TEXT("/Game/")))Class=LoadObject<UDasAuthoredInterfaceClass>(nullptr,*ClassPath,nullptr,LOAD_NoWarn);
            if(ClassPath.StartsWith(TEXT("/Game/")) && (!Cast<UDasAuthoredInterfaceClass>(Class) || CastChecked<UDasAuthoredInterfaceClass>(Class)->StableId.IsEmpty()))ExportPolicyError(TEXT("Expected an authored interface identity"));
                if (!Class) ExportPolicyError(TEXT("Rule class is not loaded: ") + ClassPath);
                const auto Include = PolicyPatterns(*Rule, TEXT("include"));
                const auto Exclude = PolicyPatterns(*Rule, TEXT("exclude"));
                if (Include.IsEmpty()) ExportPolicyError(TEXT("class rule needs nonempty include patterns"));
                const bool bInherited = PolicyBool(*Rule, TEXT("include_inherited"), false);
                TArray<FString> Names;
                for (TFieldIterator<UFunction> It(Class); It; ++It)
                    if ((bInherited || It->GetOuterUClass() == Class) && MatchesPolicy(It->GetName(), Include)) Names.AddUnique(It->GetName());
                Names.Sort();
                if (Names.IsEmpty()) ExportPolicyError(TEXT("class rule matched no functions: ") + ClassPath);
                for (const auto& Name : Names)
                {
                    auto Request = MakeShared<FJsonObject>();
                    Request->SetStringField(TEXT("class"), ClassPath);
                    Request->SetStringField(TEXT("function"), Name);
                    Request->SetStringField(TEXT("alias"), Prefix + Name);
                    Request->SetBoolField(TEXT("required"), false);
                    if (MatchesPolicy(Name, Exclude)) Excluded.Add(Prefix + Name);
                    Requests.Add(MakeShared<FJsonValueObject>(Request));
                }
            }
        }
        if (Requests.IsEmpty()) ExportPolicyError(TEXT("Selection has no candidates"));
        RootObject->SetNumberField(TEXT("selection_policy_version"), 1);
        auto EffectivePolicy = MakeShared<FJsonObject>();
        EffectivePolicy->SetBoolField(TEXT("allow_deprecated"), bAllowDeprecated);
        EffectivePolicy->SetBoolField(TEXT("allow_internal"), bAllowInternal);
        RootObject->SetObjectField(TEXT("policy"), EffectivePolicy);
        FUnrealDasContext Context(4096);
        for (const auto& RequestValue : Requests)
        {
            const TSharedPtr<FJsonObject>* Request;
            FString ClassPath, FunctionName, Alias;
            if (!RequestValue->TryGetObject(Request) || !(*Request)->TryGetStringField(TEXT("class"), ClassPath) ||
                !(*Request)->TryGetStringField(TEXT("function"), FunctionName) || !(*Request)->TryGetStringField(TEXT("alias"), Alias))
            { Error = TEXT("Each function needs class, function and alias strings"); return false; }
            CheckSelectionKeys(*Request, {TEXT("class"), TEXT("function"), TEXT("alias"), TEXT("required")});
            const bool bRequired = PolicyBool(*Request, TEXT("required"), true);
            if (Aliases.Contains(Alias) || Alias.IsEmpty()) { Error = TEXT("Duplicate or empty alias"); return false; }
            Aliases.Add(Alias);
            try
            {
            if (Excluded.Contains(Alias)) ExportPolicyError(TEXT("excluded_by_rule"));
            UClass* Class = FindObject<UClass>(nullptr, *ClassPath);
            UFunction* Function = Class && FunctionName.Len() < NAME_SIZE ? Class->FindFunctionByName(FName(*FunctionName)) : nullptr;
            if (!Function || (!Function->HasAnyFunctionFlags(FUNC_BlueprintCallable) && !(Cast<UDasAuthoredInterfaceClass>(Class) && Function->HasAnyFunctionFlags(FUNC_BlueprintEvent))) ||
                Function->HasAnyFunctionFlags(FUNC_Net | FUNC_Delegate | FUNC_MulticastDelegate))
            { ExportPolicyError(TEXT("Expected loaded class and local BlueprintCallable instance function: ") + ClassPath + TEXT(".") + FunctionName); }
            const bool ProfileAdapter=IsCollisionProfilePropertyAdapter(Function);
            if(ProfileAdapter)ValidateCollisionProfileAdapterSignature(Function,&Context);
#if WITH_EDITOR
            if (Function->HasMetaData(TEXT("Latent")) || (Function->HasMetaData(TEXT("CustomThunk")) && !ProfileAdapter)) ExportPolicyError(TEXT("requires_special_call_adapter"));
            if (Function->HasMetaData(TEXT("BlueprintProtected"))) ExportPolicyError(TEXT("protected_function"));
            if (!bAllowDeprecated && Function->HasMetaData(TEXT("DeprecatedFunction"))) ExportPolicyError(TEXT("deprecated_function"));
            if (!bAllowInternal && !ProfileAdapter && Function->HasMetaData(TEXT("BlueprintInternalUseOnly"))) ExportPolicyError(TEXT("internal_function"));
            // Construction restrictions are retained in generated wrappers and checked by the frame.
#endif
            if(Class->HasAnyClassFlags(CLASS_Interface) && (!Function->GetOuterUClass()->HasAnyClassFlags(CLASS_Interface) || !Function->HasAnyFunctionFlags(FUNC_BlueprintEvent)))ExportPolicyError(TEXT("requires_reflected_interface_method"));
            const bool bStatic=Function->HasAnyFunctionFlags(FUNC_Static);
            if(bStatic && (!ClassPath.StartsWith(TEXT("/Script/")) || !Class->HasAnyClassFlags(CLASS_Native) || !Function->HasAnyFunctionFlags(FUNC_Native)))ExportPolicyError(TEXT("requires_native_static_function"));
            FString WorldContext;
#if WITH_EDITOR
            WorldContext=Function->GetMetaData(TEXT("WorldContext")).ToLower();
#endif
            if(!WorldContext.IsEmpty()){if(!bStatic)ExportPolicyError(TEXT("instance_world_context_requires_adapter"));StaticWorldParameter(Function,WorldContext,&Context);}
            auto Signature = DescribeCall(Function, &Context);
            AddClass(Class);
            auto Item = MakeShared<FJsonObject>();
            Item->SetStringField(TEXT("class"), ClassPath);
            Item->SetStringField(TEXT("function"), FunctionName);
            Item->SetStringField(TEXT("alias"), Alias);
            Item->SetStringField(TEXT("signature"), CallSignatureKey(Function));
            auto Metadata=ExportMetadata(Function);
            if(ProfileAdapter){Metadata->SetStringField(TEXT("UnsafeDuringActorConstruction"),TEXT("true"));Item->SetStringField(TEXT("call_adapter"),TEXT("collision_profile_property_v1"));}
            Item->SetObjectField(TEXT("metadata"), Metadata);
            if(DasGeometryFunctionAllowed(Function))Item->SetStringField(TEXT("call_adapter"),TEXT("slate_geometry_snapshot_v1"));
            if(DasInputEventFunctionAllowed(Function))Item->SetStringField(TEXT("call_adapter"),TEXT("slate_input_event_snapshot_v1"));
            Item->SetStringField(TEXT("declaring_class"), Function->GetOuterUClass()->GetPathName());
            if(bStatic){Item->SetBoolField(TEXT("static"),true);Item->SetStringField(TEXT("world_context"),WorldContext);}
            Item->SetBoolField(TEXT("pure"), Function->HasAnyFunctionFlags(FUNC_BlueprintPure));
            Item->SetBoolField(TEXT("const"), Function->HasAnyFunctionFlags(FUNC_Const));
            TArray<TSharedPtr<FJsonValue>> Fields;
            for (const auto& Parameter : Signature)
            {
                if (Parameter.Kind == EMarshallingKind::Object || Parameter.Kind == EMarshallingKind::Class) AddClass(FindObject<UClass>(nullptr, *Parameter.TypePath));
                auto Field = MakeShared<FJsonObject>();
                Field->SetStringField(TEXT("path"), Parameter.Path);
                Field->SetStringField(TEXT("kind"), CallKindName(Parameter.Kind));
                Field->SetBoolField(TEXT("input"), Parameter.bInput);
                Field->SetBoolField(TEXT("output"), Parameter.bOutput);
                Field->SetStringField(TEXT("type_path"), Parameter.TypePath);
                if(Parameter.Kind==EMarshallingKind::Struct && Parameter.TypePath==FConstraintInstanceAccessor::StaticStruct()->GetPathName())Field->SetStringField(TEXT("transport"),TEXT("constraint_component_v1"));
                if(Parameter.Kind==EMarshallingKind::Struct && Parameter.TypePath==FHitResult::StaticStruct()->GetPathName())Field->SetStringField(TEXT("transport"),TEXT("hit_result_v1"));
                if(Parameter.Kind==EMarshallingKind::Struct && DasIsGeometryType(FindObject<UScriptStruct>(nullptr,*Parameter.TypePath)))Field->SetStringField(TEXT("transport"),TEXT("slate_geometry_snapshot_v1"));
                if(Parameter.Kind==EMarshallingKind::Struct && DasIsInputEventType(FindObject<UScriptStruct>(nullptr,*Parameter.TypePath)))Field->SetStringField(TEXT("transport"),TEXT("slate_input_event_snapshot_v1"));
                if(Parameter.Kind==EMarshallingKind::Struct && Parameter.TypePath==FInputActionValue::StaticStruct()->GetPathName())Field->SetStringField(TEXT("transport"),TEXT("input_action_value_v1"));
                if (Parameter.Kind == EMarshallingKind::Enum)
                {
                    UEnum* Enum = FindObject<UEnum>(nullptr, *Parameter.TypePath);
                    TArray<TSharedPtr<FJsonValue>> Members;
                    for (int32 Index = 0; Enum && Index < Enum->NumEnums(); ++Index)
                        if (IsCallEnumMember(Enum, Index))
                        {
                            auto Member = MakeShared<FJsonObject>();
                            Member->SetStringField(TEXT("name"), Enum->GetNameStringByIndex(Index));
                            Member->SetStringField(TEXT("value"), FString::Printf(TEXT("%lld"), Enum->GetValueByIndex(Index)));
                            Members.Add(MakeShared<FJsonValueObject>(Member));
                        }
                    Field->SetArrayField(TEXT("members"), Members);
                }
                Fields.Add(MakeShared<FJsonValueObject>(Field));
            }
            Item->SetArrayField(TEXT("fields"), Fields);
            TArray<TSharedPtr<FJsonValue>> Parameters;
            for (TFieldIterator<FProperty> It(Function); It; ++It)
            {
                if (!It->HasAnyPropertyFlags(CPF_Parm)) continue;
                auto Parameter = ExportParameterType(*It, Structs);
#if WITH_EDITOR
                const FName DefaultKey(*(TEXT("CPP_Default_") + It->GetName()));
                if (Function->HasMetaData(DefaultKey)) Parameter->SetStringField(TEXT("default_text"), Function->GetMetaData(DefaultKey));
#endif
                const bool bReturn = It->HasAnyPropertyFlags(CPF_ReturnParm);
                const bool bMutable = !It->HasAnyPropertyFlags(CPF_ConstParm) && It->HasAnyPropertyFlags(CPF_OutParm | CPF_ReferenceParm);
                Parameter->SetBoolField(TEXT("input"), !bReturn && (!bMutable || It->HasAnyPropertyFlags(CPF_ReferenceParm)));
                Parameter->SetBoolField(TEXT("output"), bReturn || bMutable);
                Parameters.Add(MakeShared<FJsonValueObject>(Parameter));
            }
            Item->SetArrayField(TEXT("parameters"), Parameters);
            Functions.Add(MakeShared<FJsonValueObject>(Item));
            }
            catch (const std::exception& Exception)
            {
                if (bRequired) throw;
                auto Diagnostic = MakeShared<FJsonObject>();
                Diagnostic->SetStringField(TEXT("class"), ClassPath);
                Diagnostic->SetStringField(TEXT("function"), FunctionName);
                Diagnostic->SetStringField(TEXT("alias"), Alias);
                Diagnostic->SetStringField(TEXT("reason"), UTF8_TO_TCHAR(Exception.what()));
                Diagnostics.Add(MakeShared<FJsonValueObject>(Diagnostic));
            }
        }
    }
    catch (const std::exception& Exception) { Error = UTF8_TO_TCHAR(Exception.what()); return false; }
    RootObject->SetArrayField(TEXT("functions"), Functions);
    RootObject->SetArrayField(TEXT("diagnostics"), Diagnostics);
    TArray<FString> StructPaths;
    Structs.GetKeys(StructPaths);
    StructPaths.Sort();
    TArray<TSharedPtr<FJsonValue>> Definitions;
    for (const auto& Path : StructPaths) Definitions.Add(MakeShared<FJsonValueObject>(Structs[Path]));
    RootObject->SetArrayField(TEXT("structs"), Definitions);
    TArray<FString> ClassPaths;
    Classes.GetKeys(ClassPaths);
    ClassPaths.Sort();
    TArray<TSharedPtr<FJsonValue>> ClassDefinitions;
    for (const auto& Path : ClassPaths)
    {
        auto Definition = MakeShared<FJsonObject>();
        Definition->SetStringField(TEXT("type_path"), Path);
        if(Classes[Path]->HasAnyClassFlags(CLASS_Interface))Definition->SetBoolField(TEXT("interface"),true);
        if(auto* Authored=Cast<UDasAuthoredInterfaceClass>(Classes[Path]))Definition->SetStringField(TEXT("interface_id"),Authored->StableId);
        Definition->SetObjectField(TEXT("metadata"), ExportMetadata(Classes[Path]));
        UClass* Parent = Classes[Path]->GetSuperClass();
        Definition->SetStringField(TEXT("parent"), Parent ? Parent->GetPathName() : FString());
        ClassDefinitions.Add(MakeShared<FJsonValueObject>(Definition));
    }
    RootObject->SetArrayField(TEXT("classes"), ClassDefinitions);
    FString Json;
    FJsonSerializer::Serialize(RootObject, TJsonWriterFactory<>::Create(&Json));
    if (!FFileHelper::SaveStringToFile(Json, *OutputPath, FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM))
    { Error = TEXT("Cannot write IR output"); return false; }
    return true;
}
