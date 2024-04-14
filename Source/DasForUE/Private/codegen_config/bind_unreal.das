require cbind/cbind_boost
require fio
require daslib/defer
require daslib/strings_boost
require daslib/safe_addr

let UNREAL_PATH = "../../../../../../../UnrealEngine" //TODO: get from environment?
let BIND_ROOT = "../codegen/"

class UnrealGen : CppGenBind
    override generate_lsp = true
    override func_to_stdout = false
    override generate_constant_decl = false

    unreal_allowed_fnames: table<string>
    unreal_skip_func: table<string>
    unreal_skip_struct: table<string>
    unreal_arg_part_names: array<string>

    unrealReflectionTypes: table<string>

    dummyTemplateInstanceAnnotations: table<string>

    declaredFunctionsHashes: table<uint64>
    declaredFunctionsHashesWithoutResults: table<uint64>
    declaredEnumsHashes: table<uint64>

    replaceTypeNames: table<string;string>

    def UnrealGen()
        bind_root = BIND_ROOT
        bind_module = "dasUnreal"
        bind_das_module = "unreal"
        let pfp = "./"
        let pfn = "definitions.h"
        let engineIncludes = [[string
            "{UNREAL_PATH}/Engine/Source/Runtime/Core/Public";
            "{UNREAL_PATH}/Engine/Source/Runtime/Core/Public/Containers";
            "{UNREAL_PATH}/Engine/Source/Runtime/Core/Public/Serialization";

            "{UNREAL_PATH}/Engine/Source/Runtime/CoreUObject/Public";

            "{UNREAL_PATH}/Engine/Source/Runtime/TraceLog/Public";
            "{UNREAL_PATH}/Engine/Source/Runtime/Engine/Classes";
            "{UNREAL_PATH}/Engine/Source/Runtime/Net/Core/Public";

            "{UNREAL_PATH}/Engine/Source/Runtime/AssetRegistry/Public";
            
             //path to UHT generated files. Not need to parse, but for correct includes for other files
             //TODO: find other way to skip them
            "{UNREAL_PATH}/Engine/Intermediate/Build/Win64/UnrealEditor/Inc/Engine/UHT";
        ]]

        var args <- [{string
            "-xc++-header";
            "-std=c++1z";
            "-Wno-inconsistent-missing-override";
            "-Wno-microsoft-unqualified-friend";
        }]
        for engineInclude in engineIncludes
            args |> push <| "-I{engineInclude}"

        func_per_chunk = 100
        init_args(pfn,pfp,args)
        setDefaultFiles()
        init_skip()
        openAllFiles()

    def init_skip()

        unreal_allowed_fnames <- {{
            "ProfilingDebugging/ResourceSize.h";
            "Misc/EngineVersionBase.h";
            "Misc/OutputDevice.h";
            "Misc/CompressionFlags.h";
            "Misc/MiscDefines.h";
            "Serialization/Archive.h";
            "UnrealString.h";
            "Logging/LogVerbosity.h";
            "UObject/UnrealNames.h";
            "UObject/PrimaryAssetId.h";
            "UObject/UObjectBase.h";
            "UObject/UObjectMarks.h";
            "UObject/ObjectMacros.h";
            "UObject/UObjectBaseUtility.h";
            "UObject/Object.h";
            "UObject/Class.h";
            "UObject/Package.h";
            "UObject/UObjectGlobals.h";
            "UObject/UnrealType.h";
            "UObject/Field.h";
            "UObject/Stack.h";
            "UObject/Script.h";
            "UObject/ObjectPtr.h";
            "Kismet/BlueprintFunctionLibrary.h";
            "Kismet/KismetArrayLibrary.h";
            "Containers/SparseArray.h";
            "Containers/Set.h";
            "Containers/Map.h";
            "AssetRegistry/AssetRegistryModule.h"
        }}

        unrealReflectionTypes <- {{
            "UObject";
            "UField";
            "UStruct";
            "UFunction";
            "UScriptStruct";
            "UClass";
            "UEnum";
            "UPackage";
            "UPropertyWrapper";
            "UBlueprintFunctionLibrary";
            "UKismetArrayLibrary";

            "FAssetRegistryModule"; //modules can't be created directly
        }}

        dummyTemplateInstanceAnnotations <- {{
            "TArray<int>";
            "TArray<float>";
            "TArray<FString>";

            "TObjectPtr<UClass>";
            "TObjectPtr<UObject>";
            "TObjectPtr<UField>";
            "TObjectPtr<UStruct>";
            "TObjectPtr<UScriptStruct>";
            "TObjectPtr<UFunction>";
            "TObjectPtr<UEnum>";
        }}

        unreal_skip_func <- {{
            //UnrealString.h
            "FString::Format";
            "FString::CreateIterator";
            "FString::CreateConstIterator";
            "FString::GetCharArray";
            "FString::begin";
            "FString::end";
            "FString::rbegin";
            "FString::rend";

             //UObject/UObjectBase.h
             "RegisterCompiledInInfo"; //FClassRegisterCompiledInInfo
             "UObjectBaseInit"; //some inner stuff?
             "UObjectBaseShutdown";

             //UObject/UObjectMarks.h
             "GetObjectsWithAllMarks";
             "GetObjectsWithAnyMarks";

             //UObject/UObjectBaseUtility.h
             "UObjectBaseUtility::GetVersePath";
             "UObjectBaseUtility::GetStatID"; //TStatId
             "UObjectBaseUtility::GetLinkerCustomVersion"; //FGuid
             "UObjectBaseUtility::GetLinkerUEVersion"; //FPackageFileVersion
             "UObjectBaseUtility::GetFullName"; //TStringBuilderBase<wchar_t>
             "UObjectBaseUtility::GetPathName"; //TStringBuilderBase<wchar_t>

            //UObject/Object.h
            "UObject::PreSave"; //FObjectPreSaveContext
            "UObject::PostEditUndo"; //TSharedPtr<ITransactionObjectAnnotation, ESPMode::ThreadSafe>
            "UObject::GetNativePropertyValues"; //TMap<FString, FString>
            "UObject::GetAssetRegistryTagMetadata"; //TMap<FName, UObject::FAssetRegistryTagMetadata>
            "UObject::BuildSubobjectMapping"; //TMap<UObject *, UObject *>
            "UObject::BeginCacheForCookedPlatformData"; //ITargetPlatform
            "UObject::IsCachedCookedPlatformDataLoaded"; //ITargetPlatform
            "UObject::ClearCachedCookedPlatformData"; //ITargetPlatform
            "UObject::IsDataValid";
            "UObject::FindOrCreateTransactionAnnotation";
            "UObject::CreateAndRestoreTransactionAnnotation";
            "UObject::PostCDOCompiled";
            "UObject::DeclareCustomVersions";
            "UObject::AppendToClassSchema";
            "UObject::PreEditChange";
            "UObject::CanEditChange";
            "UObject::PostEditChangeProperty";
            "UObject::PostEditChangeChainProperty";
            "UObject::PostTransacted";
            "UObject::PreDuplicate";
            "UObject::NeedsLoadForTargetPlatform";
            "UObject::ExportCustomProperties";
            "UObject::GetResourceSizeEx";
            "UObject::GetReplicatedCustomConditionState";
            "UObject::ProcessConsoleExec";
            "UObject::GetMetaDataTagsForAssetRegistry";
            "UObject::RegisterReplicationFragments";
            "UObject::SerializeScriptProperties";
            "UObject::UpdateSinglePropertyInConfigFile";
            "UObject::CallFunctionByNameWithArguments";
            "UObject::StaticPackage";
            "UObject::ProcessInternal"; //linker error"

            //UObject/Class.h
            "FCppClassTypeInfo::FCppClassTypeInfo";
            "UStruct::SetUnresolvedScriptProperties"; //TArray<TTuple<TFieldPath<FField>
            "UStruct::SerializeExpr";
            "UStruct::GetStructPathName";
            "UStruct::SerializeTaggedProperties";
            "UStruct::PreloadChildren";

            "UEnum::ResolveEnumerator";
            "UEnum::LookupEnumName";
            "UEnum::LookupEnumNameSlow";

            "UClass::TryFixShortClassNameExportPath";
            "UClass::TryConvertShortTypeNameToPathName";
            "UClass::ConvertPathNameToShortTypeName";
            "UClass::ConvertFullNameToShortTypeFullName";
            "UClass::IsShortTypeName";
            "UClass::TryFindTypeSlow";
            "UClass::TryFindTypeSlowSafe";
            "UClass::GetClassPathName";
            "UClass::CallDeclareCustomVersions";
            "UClass::SerializeDefaultObject";
            "UClass::CallAppendToClassSchema";
            "UClass::GetSparseClassData";
            "UClass::CreateLinkAndAddChildFunctionsToMap";
            "UClass::GetDefaultPropertiesFeedbackContext";
            "UClass::SetupObjectInitializer";

            "UScriptStruct::UseBinarySerialization";
            "UScriptStruct::SerializeItem";
            "UScriptStruct::ShouldSerializeAtomically";
            "UScriptStruct::FindInnerPropertyInstance";
            "UScriptStruct::ICppStructOps::GetComputedPropertyFlags";

            "UField::AddCppProperty"; //just runtime hook to fail

            //UObject/Package.h
            "UPackage::GetWorldTileInfo";
            "UPackage::GetLinkerPackageVersion";
            "UPackage::GetLoadedPath";
            "UPackage::SetLoadedPath";
            "UPackage::AccessThumbnailMap";
            "UPackage::GetThumbnailMap";
            "UPackage::Save";
            "UPackage::SavePackage";

            //UObject/UObjectGlobals.h
            "SetMountPointDefaultPackageFlags";
            "RemoveMountPointDefaultPackageFlags";
            "GetConvertedDynamicPackageNameToTypeName";
            "GetDynamicClassMap";
            "LoadPackageAsync";
            "FlushAsyncLoading";
            "FCoreUObjectDelegates::GetPreGarbageCollectDelegate";
            "FCoreUObjectDelegates::GetPostGarbageCollect";
            "FCoreUObjectDelegates::GetPostPurgeGarbageDelegate";
            "FCoreUObjectDelegates::GetGarbageCollectReportGarbageReferencers";
            "FObjectInitializer::SetNestedDefaultSubobjectClass";
            "FObjectInitializer::DoNotCreateNestedDefaultSubobject";
            "StaticDuplicateObjectEx";
            "FReferenceFinder::HandleObjectReference";
            "FScopedAllowAbstractClassAllocation::IsDisallowedAbstractClass";
            "StaticAllocateObjectErrorTests";
            "FVerySlowReferenceCollectorArchiveScope::GetArchive";
            "FReferenceCollector::AROPrivate::AddReferencedObjects";
            "FReferenceCollector::GetVerySlowReferenceCollectorArchive";
            "FReferenceCollector::AddStableReferenceSet";
            "FReferenceCollector::AddStableReferenceArray";
            "FReferenceCollector::AddReferencedObjects";

            "UECodeGen_Private::ConstructUFunction";
            "UECodeGen_Private::ConstructUEnum";
            "UECodeGen_Private::ConstructUScriptStruct";
            "UECodeGen_Private::ConstructUPackage";
            "UECodeGen_Private::ConstructUClass";

            //UObject/UnrealType.h
            "FProperty::ExportCppDeclaration";
            "GlobalSetProperty";
            "UEProperty_Private::FProperty_DoNotUse::Unsafe_AlterOffset";
            "UPropertyWrapper::GetProperty";
            "FArrayProperty::ImportTextInnerItem"; //FScriptArrayHelper

            //UObject/Field.h
            "FField::GetPathName";
            "FField::GetConvertCustomUFieldToFFieldDelegate";
            "FProperty::FProperty";

            "FProperty::Construct"; //"abstract macro" -- has no implementation, ancestor implement it
            "FPropertyHelpers::ReadToken";

            //FArchive.h
            "FArchive::SerializeBulkData";
            "FArchive::AttachExternalReadDependency";
            "FArchive::MarkSearchableName";
            "FArchiveState::SetCookingTarget";
            "FArchiveState::CookingTarget";
            
            //PrimaryAssetId.h
            "FPrimaryAssetId::AppendString"; //TStringBuilderBase
            "FPrimaryAssetType::AppendString"; //TStringBuilderBase
            "FPrimaryAssetId::ExportTextItem"; //linker error
            "FPrimaryAssetType::ExportTextItem"; //linker error
            "FPrimaryAssetType::ImportTextItem"; //linker error
            "FPrimaryAssetId::ImportTextItem"; //linker error

            //FStack.h
            "FFrame::GetScriptCallstack"; //TStringBuilderBase
            "FFrame::GetStackTrace"; //TStringBuilderBase
            "FFrame::GetStackDescription"; //TStringBuilderBase
            "FFrame::InitPrintScriptCallstack"; //linker error

            //Script.h
            //"FBlueprintContext::FBlueprintContext"; //Linker error
            //"FBlueprintContext::GetVirtualStackAllocator"; //FVirtualStackAllocator

            //Manual disabled function if TArray enabled
            /*"FArchiveState::GetSerializedPropertyChain"; //TArray<FProperty *>
            "FArchive::SetCompressionMap"; //TArray<FCompressedChunk
            "FString::ReplaceCharWithEscapedCharInline"; //TArray<wchar_t>
            "FString::ReplaceEscapedCharWithCharInline"; //TArray<wchar_t>
            "ParsePropertyFlags"; //TArray<const wchar_t *>
            "FTextRange::CalculateLineRangesFromString"; //TArray<FTextRange>
            "StaticFindAllObjectsFast"; //TArray<UObject *>
            "StaticFindAllObjectsFastSafe";
            "StaticFindAllObjects";
            "StaticFindAllObjectsSafe";
            "FReferenceFinder::FReferenceFinder"; //TArray<UObject *>
            "UObject::GetDefaultSubobjects"; //TArray<UObject *>
            "UObject::GetPreloadDependencies"; //TArray<UObject *>
            "UObject::GetPrestreamPackages"; //TArray<UObject *>
            "UObject::GetAssetRegistryTags"; //TArray<UObject::FAssetRegistryTag>
            "UObject::GetExtendedAssetRegistryTagsForSave";
            "UObject::PostLoadAssetRegistryTags";
            "UObject::GetLifetimeReplicatedProps"; //TArray<FLifetimeProperty>
            "UObject::GetSubobjectsWithStableNamesForNetworking"; //TArray<UObject *>
            "UObject::PropagatePreEditChange"; //TArray<UObject *>
            "UObject::PropagatePostEditChange"; //TArray<UObject *>
            "UObject::CollectDefaultSubobject"; //TArray<UObject *>
            "UObject::RetrieveReferencers"; //TArray<FReferencerInformation>
            "UObject::GetArchetypeInstances"; //TArray<UObject *>
            "UObject::ValidateGeneratedRepEnums"; //TArray<FRepRecord>
            "UObject::CollectDefaultSubobjects";
            "FFieldClass::GetAllFieldClasses"; //TArray<FFieldClass *>
            "FField::GetPreloadDependencies"; //TArray<UObject *>
            "FField::GetInnerFields"; //TArray<FField *>
            "UStruct::GetPreloadDependencies";
            "UStruct::CollectBytecodeReferencedObjects";
            "UStruct::CollectPropertyReferencedObjects";
            "UEnum::SetEnums"; //TArray<TTuple<FName, long long>>
            "UClass::GenerateFunctionList"; //TArray<FName>
            "UClass::GetAssetRegistryTags"; //TArray<UObject::FAssetRegistryTag>
            "UClass::GetDefaultObjectSubobjects";
            "UClass::FindCommonBase"; //TArray<UClass *>
            "UClass::GetDefaultObjectPreloadDependencies"; //TArray<UObject *>
            "UClass::PostLoadAssetRegistryTags"; //TArray<UObject::FAssetRegistryTag>
            "UClass::GetPreloadDependencies";
            "UPackage::GetReloadDelegates"; //TArray<UFunction *>
            "UPackage::SetReloadDelegates";
            "UPackage::GetExternalPackages"; //TArray<UPackage *>
            "FProperty::ImportSingleProperty"; //TArray<FDefinedProperty>
            "FProperty::NetSerializeItem"; //TArray<unsigned char>
            "FProperty::ContainsObjectReference"; //TArray<const FStructProperty *>
            "FProperty::EmitReferenceInfo"; //TArray<const FStructProperty *>
            "FByteProperty::GetPreloadDependencies";
            "FByteProperty::NetSerializeItem";
            "FBoolProperty::NetSerializeItem";
            "FObjectPropertyBase::NetSerializeItem";
            "FStructProperty::NetSerializeItem";
            "FStructProperty::GetPreloadDependencies";
            "FStructProperty::ContainsObjectReference";
            "FStructProperty::EmitReferenceInfo";*/

            "FObjectPropertyBase::GetExportPath";

            //KismetArrayLibrary.h
            "UKismetArrayLibrary::Array_ShuffleFromStream"; //FRandomStream
            "UKismetArrayLibrary::Array_RandomFromStream";
            "UKismetArrayLibrary::GenericArray_ShuffleFromStream";
            "UKismetArrayLibrary::GenericArray_RandomFromStream";
            "UKismetArrayLibrary::GetLastIndex"; //FScriptArrayHelper

            //FAssetRegistryModule
            "FAssetRegistryModule::TryGetAssetByObjectPath"; //UE::AssetRegistry::EExists
            "FAssetRegistryModule::TryGetAssetPackageData"; //UE::AssetRegistry::EExists
            "FAssetRegistryModule::GetDependencies"; //UE::AssetRegistry::EDependencyCategory
            "FAssetRegistryModule::Get"; //IAssetRegistry
            "FAssetRegistryModule::GetRegistry"; //IAssetRegistry
            "FAssetRegistryModule::TryGet";
            "FAssetRegistryModule::StartupModule";
        }}

        for unrealReflType in keys(unrealReflectionTypes)
            unreal_skip_func |> insert <| "{unrealReflType}::{unrealReflType}" //disable ctors, because type must be construct with StaticConstructObject()
            unreal_skip_func |> insert <| "{unrealReflType}::__DefaultConstructor" //meta-function
            unreal_skip_func |> insert <| "{unrealReflType}::__VTableCtorCaller"   //meta-function
            unreal_skip_func |> insert <| "{unrealReflType}::StaticClassCastFlags" //idk why it crashes

        //TODO:
        // - radical way, find substring is slow
        unreal_arg_part_names <- [{string
            "TArray";
            "FSchemaOwner";
            "FUnversionedStructSchema";
            "TUniquePtr";
            "TStringBuilderBase";

            //UPackage
            "TPimplPtr";
            "TFuture";
            "FCustomVersionContainer";
            "FPackageFileVersion";
            "FGuid"; //Guid has not been used by the engine for a long time and GetGuid will be removed
            "FPackageId";

            //UObjectGlobals
            "TFunction";
            "TMap";
            "FTopLevelAssetPath";
            "TDelegate";
            "TArrayView";
            "FPackagePath";
            "UE::Core::FVersePath";
            "FWeakObjectPtr";

            //UObjectTypes

            "const WIDECHAR *";  //mostly (for UnrealString, at least), it related to overloads char/wchar/unsigned short/UTF8CHAR
            "const UCS2CHAR *";  //check what can be done with them
            "const UTF8CHAR *";  //
            
            "TYPE_OF_NULLPTR";  //macro for std::nullptr_t
            "ENoInit";
            "UECodeGen_Private::"; //recheck, must be skipped dy skip_struct
            "FObjectHandle"; //UE::CoreUObject::Private::
        }]
        
        unreal_skip_struct <- {{
            //UnrealString.h
            "FStringOutputDevice";
            "FStringOutputDeviceCountLines";

            //UObject/UObjectBase.h
            "FRegisterCompiledInInfo";
            "FClassReloadVersionInfo";
            "FClassRegisterCompiledInInfo";
            "FStructReloadVersionInfo";
            "FStructRegisterCompiledInInfo";
            "FEnumReloadVersionInfo";
            "FEnumRegisterCompiledInInfo";
            "FPackageReloadVersionInfo";
            "FStat_STAT_UObjectsStatGroupTester"; //unrelated

            //UObject/ObjectMacros.h
            "FReferencerInformationList";
            "FReferencerInformation";
            "FUObjectCppClassStaticFunctions";

            //UObject/UObjectBaseUtility.h
            "FScopeCycleCounterUObject"; //TStatId

            //UObject/Object.h
            "FDataValidationContext";
            "FLifetimeProperty";
            "FRepRecord";
            "Object_eventExecuteUbergraph_Parms";
            "FEditPropertyChain";
            "FAppendToClassSchemaContext";
            "FStructuredArchiveRecord";
            "FPropertyChangedEvent";
            "FPropertyChangedChainEvent";
            "FObjectPreSaveRootContext";
            "FObjectPostSaveContext";
            "FObjectPreSaveContext";
            "FObjectPostSaveRootContext";
            "FObjectInstancingGraph";
            "FTransactionObjectEvent";
            "FObjectDuplicationParameters";
            "FFeedbackContext";
            "FResourceSizeEx";
            "FCustomPropertyConditionState";
            "FFragmentRegistrationContext";
            "FInternalUObjectBaseUtilityIsValidFlagsChecker";
            "EDataValidationResult";
            "FStructuredArchiveSlot";
            "FConfigCacheIni";
            "FLinkerLoad";
            "UWorld";
            "FAssetRegistryTag";

            //UObject/Class.h
            "FUnversionedStructSchema";
            "UDelegateFunction";
            "USparseDelegateFunction";
            "UDynamicClass";
            "FStructUtils";
            //"FCppClassTypeInfo";

            //UObject/Package.h
            "Error";

            //UObject/UObjectGlobals.h
            "FScopedObjectFlagMarker";
            "FScopedAllowAbstractClassAllocation";
            "FReferenceCollectorArchive";

            //UObject/UnrealType.h
            "FScriptMapHelper";
            "FScriptSetHelper";
            "UMulticastDelegatePropertyWrapper";
            "FScriptArrayHelper";
            "UEProperty_Private_FProperty_DoNotUse";
            "FPropertyValueIterator";
            "UMulticastInlineDelegatePropertyWrapper";
            "FScriptArrayHelper_InContainer";
            "FScriptMapHelper_InContainer";
            "FMapPropertyParams";
            "FArrayPropertyParams";
            "FScriptSetHelper_InContainer";
            "FVerySlowReferenceCollectorArchiveScope";

            "FDelegateProperty";
            "FMulticastInlineDelegateProperty";
            "FMulticastSparseDelegateProperty";
            "FMulticastDelegateProperty";
            "FInterfaceProperty";
            "FNameProperty";
            "FLargeWorldCoordinatesRealProperty";

            "FDefaultConstructedPropertyElement";

            //FArchive.h
            "FCompressedChunk";
            "FBulkData";
            "FBulkDataSerializationParams";
            "FScopeSeekTo";
            "FArchiveCookData";
            "ITargetPlatform";

            //Script.h
            "FBlueprintContext";

            //ObjectPtr.h
            "FObjectHandle"; //UE::CoreUObject::Private::FObjectHandlePrivate;

            //
            
        }}

        enums_with_flags <- {{
            "EObjectFlags" => true;
            "EFunctionFlags" => true;
            "EPropertyFlags" => true;
            "EClassFlags" => true;
        }}

    //TODO: check if this may be moved to cbind_boost
    def isMoveType(name, cppname: string)
        return true //check if copy constructible code moved based on type_traits

    def isDisableNewDelete(name, cppname: string)
        return unrealReflectionTypes|> key_exists <| name

    def override isArgByValue(name:string)
        return true

    def override skip_file(fname:string) : bool
        //print("fname = {fname}\n")
        for allowedFname in keys(unreal_allowed_fnames)
            if fname |> ends_with(allowedFname)
                return false
        return true

    def functionArgumentRules : TypeRules
        return TypeRules char_ptr_is_string | TypeRules function_proto_is_void_ptr | TypeRules top_level_pointers_are_implicit

    //TODO: check if possible to override skip_type instead of create new function
    def skip_arguments_and_result_types(var c : CXCursor) : bool
        let narg = clang_Cursor_getNumArguments(c)
        for ai in urange(narg)
            var carg = clang_Cursor_getArgument(c, ai)
            let arg_name = string(clang_getCursorSpelling(carg))
            var arg_type = clang_getCursorType(carg)
            let ncname = string(clang_getTypeSpelling(arg_type))

            //skip rvalues args
            if arg_type.kind == CXTypeKind RValueReference
                return true
            
            //print("ArgType: {ncname}\n")
            for skipName in unreal_arg_part_names
                if ncname |> find(skipName) != -1
                    return true

        var fun_type = clang_getCursorType(c)
        var res_type = clang_getResultType(fun_type)
        //skip rvalue results
        if res_type.kind == CXTypeKind RValueReference
            return true

        let ncresult = string(clang_getTypeSpelling(res_type))
        for skipName in unreal_arg_part_names
            if ncresult |> find(skipName) != -1
                return true

        //skip methods on rvalue objects
        let refQualifier = clang_Type_getCXXRefQualifier(fun_type)
        if refQualifier != CXRefQualifierKind None
            return true
        
        return false

    def override skip_anyFunction(var c : CXCursor; isMethod:bool) : bool
        if AnyGenBind`skip_anyFunction(self, c, isMethod)
            return true
        let function_name = namespace_name(string(clang_getCursorSpelling(c)))
        if function_name |> find("operator") != -1 //TODO: check this
            return true
        if function_name |> find("::exec") != -1 //DECLARE_FUNCTION macro, internal function call processing
            return true
        if check_deprecated_attribute(c)
            return true
        if skip_arguments_and_result_types(c)
            return true
        if unreal_skip_func |> key_exists(function_name)
            return true
        return false

    def override skip_struct(name: string)
        if name |> starts_with("FLogCategory") //DECLARE_LOG_CATEGORY_EXTERN macro
            return true
        if name |> starts_with("FStat_") //DECLARE_CYCLE_STAT_EXTERN macro
            return true
        let struct_namespace_name = namespace_name(name)
        if struct_namespace_name |> starts_with("UECodeGen_Private::")
            return true
        return unreal_skip_struct |> key_exists(name)

    def override skip_enum ( ns_en, en : string )
        //print("enum {ns_en} {en}\n")
        if en |> find("unnamed enum at") != -1
            return true
        return false

    def override dasEnumName(enum_name,cpp_enum_name:string) : string
        var das_enum_name = cpp_enum_name |> replace("::", "_")
        if das_enum_name |> ends_with("_")
            das_enum_name = das_enum_name |> slice(0,-1)
        return das_enum_name

    def override writeFuncPrefix
        fwrite(func_file, "#include \"{bind_module}.h\"\n")
        fwrite(func_file, "#include \"need_{bind_module}.h\"\n")
        fwrite(func_file, "namespace das \{\n")
        fwrite(func_file, "#include \"{bind_module}.func.aot.decl.inc\"\n")
        fwrite(func_file, "void Module_{bind_module}::initFunctions_{func_chunk_index}() \{\n")

    def override generateModuleNeedH
        module_need_h_file |> fwrite("#pragma once\n")
        generateModuleNeedHPrefix()
        module_need_h_file |> fwrite("#include \"CoreMinimal.h\"\n")
        module_need_h_file |> fwrite("#include \"AssetRegistry/AssetRegistryModule.h\"\n")
        module_need_h_file |> fwrite("#include \"Kismet/KismetArrayLibrary.h\"\n")

        module_need_h_file |> fwrite("#include \"{bind_module}.enum.decl.inc\"\n")
        module_need_h_file |> fwrite("#include \"{bind_module}.enum.decl.cast.inc\"\n")
        module_need_h_file |> fwrite("#include \"{bind_module}.struct.decl.inc\"\n")
        module_need_h_file |> fwrite("namespace das \{\n")
        module_need_h_file |> fwrite("\t#include \"{bind_module}.struct.cast.inc\"\n")
        module_need_h_file |> fwrite("\}\n\n")

    def override generateModuleHPrefix
        fwrite(module_h_file, "#include \"das_config.h\"\n")
        fwrite(module_h_file, "THIRD_PARTY_INCLUDES_START\n")
        fwrite(module_h_file, "#include \"daScript/misc/platform.h\"\n")
        fwrite(module_h_file, "#include \"daScript/ast/ast.h\"\n")
        fwrite(module_h_file, "#include \"daScript/ast/ast_interop.h\"\n")
        fwrite(module_h_file, "#include \"daScript/ast/ast_handle.h\"\n")
        fwrite(module_h_file, "#include \"daScript/ast/ast_typefactory_bind.h\"\n")
        fwrite(module_h_file, "#include \"daScript/simulate/bind_enum.h\"\n")
        fwrite(module_h_file, "#include \"daScript/simulate/fs_file_info.h\"\n")
        fwrite(module_h_file, "THIRD_PARTY_INCLUDES_END\n")
        fwrite(module_h_file, "#include \"using_das.inc\"\n")
        fwrite(module_h_file, "#include \"cb_dasUnreal.h\"\n")

    def override generateModuleHSuffix
        module_h_file |> fwrite("\tpublic:\n")
        module_h_file |> fwrite("\t\tvirtual ~Module_{bind_module}();\n")
        module_h_file |> fwrite("\tpublic:\n")
        module_h_file |> fwrite("\t\tstatic thread_local das_map<const UFunction*, UnrealNativeFunc> g_Callbacks;\n")

    def override generateModuleH
        module_h_file |> fwrite("#pragma once\n")
        generateModuleHPrefix()
        module_h_file |> fwrite("namespace das \{\n")
        module_h_file |> fwrite("class Module_{bind_module} : public Module \{\n")
        module_h_file |> fwrite("public:\n")
        module_h_file |> fwrite("\tModule_{bind_module}();\n")
        module_h_file |> fwrite("protected:\n")
        module_h_file |> fwrite("\tvirtual bool initDependencies() override;\n")
        module_h_file |> fwrite("\tvoid initAdditionalAnnotations();\n")
        module_h_file |> fwrite("\tvoid initAdditionalFunctions();\n")
        module_h_file |> fwrite("\tvoid initMain ();\n")
        if length(aot_alias)!=0
            module_h_file |> fwrite("\tvoid initAotAlias ();\n")
        module_h_file |> fwrite("\tvirtual ModuleAotType aotRequire ( TextWriter & tw ) const override;\n")
        module_h_file |> fwrite("\t#include \"{bind_module}.func.decl.inc\"\n")
        module_h_file |> fwrite("public:\n")
        module_h_file |> fwrite("\tModuleLibrary lib;\n")
        module_h_file |> fwrite("\tbool initialized = false;\n")
        generateModuleHSuffix()
        module_h_file |> fwrite("};\n")
        module_h_file |> fwrite("}\n\n")

    def override generateModuleCpp
        module_cpp_file |> fwrite("#include \"{bind_module}.h\"\n")
        module_cpp_file |> fwrite("#include \"need_{bind_module}.h\"\n")
        module_cpp_file |> fwrite("#include \"{bind_module}.struct.impl.inc\"\n")
        module_cpp_file |> fwrite("namespace das \{\n")
        module_cpp_file |> fwrite("#include \"{bind_module}.enum.class.inc\"\n")
        module_cpp_file |> fwrite("#include \"{bind_module}.struct.class.inc\"\n")
        module_cpp_file |> fwrite("#include \"{bind_module}.func.aot.inc\"\n")
        module_cpp_file |> fwrite("Module_{bind_module}::Module_{bind_module}() : Module(\"{bind_das_module}\") \{\n")  // TODO: das module name
        module_cpp_file |> fwrite("\}\n")
        module_cpp_file |> fwrite("bool Module_{bind_module}::initDependencies() \{\n")
        module_cpp_file |> fwrite("\tif ( initialized ) return true;\n")
        for rm in require_modules
            module_cpp_file |> fwrite("\tauto mod_{rm} = Module::require(\"{rm}\");\n")
            module_cpp_file |> fwrite("\tif ( !mod_{rm} ) return false;\n")
            module_cpp_file |> fwrite("\tif ( !mod_{rm}->initDependencies() ) return false;\n")
        module_cpp_file |> fwrite("\tinitialized = true;\n")
        module_cpp_file |> fwrite("\tlib.addModule(this);\n")
        module_cpp_file |> fwrite("\tlib.addBuiltInModule();\n")
        for rm in require_modules
            module_cpp_file |> fwrite("\tlib.addModule(mod_{rm});\n")
        if length(aot_alias)!=0
            module_cpp_file |> fwrite("\tinitAotAlias();\n")
        if generate_constant_decl
            module_cpp_file |> fwrite("\t#include \"{bind_module}.const.inc\"\n")
        module_cpp_file |> fwrite("\tinitAdditionalAnnotations();\n")
        module_cpp_file |> fwrite("\t#include \"{bind_module}.enum.add.inc\"\n")
        module_cpp_file |> fwrite("\t#include \"{bind_module}.dummy.add.inc\"\n")
        module_cpp_file |> fwrite("\t#include \"{bind_module}.struct.add.inc\"\n")
        module_cpp_file |> fwrite("\t#include \"{bind_module}.struct.postadd.inc\"\n")
        module_cpp_file |> fwrite("\t#include \"{bind_module}.alias.add.inc\"\n")
        module_cpp_file |> fwrite("\tinitAdditionalFunctions();\n")
        module_cpp_file |> fwrite("\t#include \"{bind_module}.func.reg.inc\"\n")
        module_cpp_file |> fwrite("\treturn true;\n")
        module_cpp_file |> fwrite("\}\n")
        module_cpp_file |> fwrite("}\n")
        module_cpp_file |> fwrite("REGISTER_MODULE_IN_NAMESPACE(Module_{bind_module},das);\n\n")

    //TODO: check if really need overload, or base class expanded needed
    def override parse_Struct(var c: CXCursor; struct_name: string)
        return if check_deprecated_attribute(c)
        return if struct_name |> starts_with <| "(" // do nothing for anonymous unions/structs
        let struct_namespace_name = namespace_name(struct_name)
        nsst2st[struct_namespace_name] = struct_name
        st2nsst[struct_name] = struct_namespace_name
        let isAotAlias = aot_alias |> key_exists(struct_namespace_name)
        let isAlreadyDefined = already_defined |> key_exists(struct_namespace_name)
        let isForward = clang_isForwardDeclaration(c)
        var isForwardDummy = false
        if isForward
            var stdecl = clang_getTypeDeclaration(clang_getCursorType(c))
            isForwardDummy = clang_isForwardDeclaration(stdecl)
        if !struct_ann |> key_exists(struct_namespace_name)
            if !isAotAlias && !isAlreadyDefined
                if !isForward || isForwardDummy
                    fprint(struct_decl_file, "MAKE_EXTERNAL_TYPE_FACTORY({struct_name},{struct_namespace_name});\n")
                    fprint(struct_impl_file, "IMPLEMENT_EXTERNAL_TYPE_FACTORY({struct_name},{struct_namespace_name});\n")
        if isForward
            if isForwardDummy
                to_log(LOG_INFO, "Forward declaration {struct_namespace_name} at {clang_getCursorLocationDescription(c)}\n")
                struct_ann[struct_namespace_name] ||= false
            return
        struct_ann[struct_namespace_name] = true
        if isAotAlias || isAlreadyDefined
            return
        let dashing_name = namespace_name(struct_name,"_")
        let st = build_string() <| $ ( var writer:StringBuilderWriter )
            swrite |> push(unsafe(addr(writer)))
            writer |> write("// from {clang_getCursorLocationDescription(c)}\n")
            if isDisableNewDelete(struct_name, struct_namespace_name)
                writer |> write("struct {dashing_name}_GeneratedAnnotation : ManagedStructureAnnotation<{struct_namespace_name}, false, false> \{\n")
            else
                writer |> write("struct {dashing_name}_GeneratedAnnotation : ManagedStructureAnnotation<{struct_namespace_name}> \{\n")

            if isMoveType(struct_name, struct_namespace_name)
                //TODO:
                // - split tp copy/move/clone?
                // - check if it safe to move this logic in some daScript base Annotation type, remove copy-paste of compile-time checks
                // - is_copy_constructible type-trait for additional compiler check if it is ok
                writer |> write("\tvirtual bool canCopy() const override \{ return std::is_copy_constructible<{struct_namespace_name}>::value; \}\n")
                writer |> write("\tvirtual bool canClone() const override \{ return std::is_copy_constructible<{struct_namespace_name}>::value; \}\n")
                writer |> write("\tvirtual bool canMove() const override \{ return std::is_copy_constructible<{struct_namespace_name}>::value; \}\n")
            if isLocalType(struct_name, struct_namespace_name)
                writer |> write("\tvirtual bool isLocal() const override \{ return true; \}\n")
                writer |> write("\tvirtual bool canBePlacedInContainer() const override \{ return true; \}\n")
            writer |> write("\t{dashing_name}_GeneratedAnnotation(ModuleLibrary & ml) : ManagedStructureAnnotation (\"{struct_name}\", ml, \"{struct_namespace_name}\") \{\n")
            writer |> write("\t\}\n")
            writer |> write("\tvoid init () \{\n")
            let isAbstract = clang_CXXRecord_isAbstract(c)!=0u
            abstract_class |> push(isAbstract)
            AnyGenBind`parse_Struct(self, c, struct_name)
            abstract_class |> pop()
            writer |> write("\t\}\n\};\n")
            if isArgByValue(struct_namespace_name)
                fprint(struct_cast_file, "template <> struct cast_arg<{struct_namespace_name}> \{\n")
                fprint(struct_cast_file, "\tstatic __forceinline const {struct_namespace_name} & to ( Context & ctx, SimNode * node ) \{\n")
                fprint(struct_cast_file, "\t\tvec4f res = node->eval(ctx);\n")
                fprint(struct_cast_file, "\t\treturn * cast<{struct_namespace_name} *>::to(res);\n")
                fprint(struct_cast_file, "\t\}\n")
                fprint(struct_cast_file, "\};\n")
            swrite |> pop()
        fprint(struct_class_file, "{st}\n")
        fprint(struct_add_file,"auto ann_{struct_name} = make_smart<{dashing_name}_GeneratedAnnotation>(lib);\n")
        fprint(struct_add_file,"addAnnotation(ann_{struct_name});\n")
        fprint(struct_postadd_file,"initRecAnnotation(ann_{struct_name},lib);\n")

    //TODO: check if really need overload, or base class expanded needed
    def override parse_anyFunctionDecl (var c: CXCursor; isMethod: bool)
        step_Function()
        let isStatic = clang_Cursor_getStorageClass(c)==CX_StorageClass Static
        let function_name = string(clang_getCursorSpelling(c))
        var das_function_name = function_name
        var isOperator = false
        if function_name |> starts_with("operator")
            let Ch = character_at(function_name,8)
            if !(is_alpha(Ch) || is_number(Ch) || Ch=='_')
                das_function_name = function_name |> slice(8)
                isOperator = true
        let isVirtual = clang_CXXMethod_isVirtual(c)!=0u
        var fun_type = clang_getCanonicalType(clang_getCursorType(c))
        var res_type = clang_getResultType(fun_type)
        var nc_res_type = clang_getResultType(clang_getCursorType(c))
        let function_cpp_name = namespace_name(function_name)
        das_function_name = rename_function(das_function_name,function_cpp_name)
        var bind_enchantation = ""
        var cpp_enchantation = ""
        var extra_enchantation = ", SimNode_ExtFuncCall "
        if clang_das_isRef(res_type)
            extra_enchantation = ", SimNode_ExtFuncCallRef "
        elif clang_das_isCopyOnReturn(res_type) && !isAotAliasType(nc_res_type)
            extra_enchantation = ", SimNode_ExtFuncCallAndCopyOrMove "
        if isMethod
            let using_mname = methodName()
            if isVirtual || isStatic
                let self_type = namespace_struct_name()
                let function_cpp_type = functionWrapperSpelling(c,self_type)
                let wrapper_cpp_name = gen_functionWrapper(c,self_type)
                bind_enchantation = " {function_cpp_type} , {wrapper_cpp_name} "
                cpp_enchantation = wrapper_cpp_name
            else
                let function_cpp_type = functionPtrSpelling(c,true)
                let using_mname_expr = "das::das_call_member< {function_cpp_type},&{function_cpp_name} >"
                bind_enchantation = "DAS_CALL_METHOD({using_mname})"
                cpp_enchantation = "das_call_member< {function_cpp_type} , &{function_cpp_name} >::invoke"
                fprint(func_file,"\tusing {using_mname} = {using_mname_expr};\n");
        else
            let function_cpp_type = functionPtrSpelling(c,false)
            bind_enchantation = " {function_cpp_type} , {function_cpp_name} "
            cpp_enchantation = function_cpp_name

        //skip same declarations
        let funcNameHash = hash("{bind_enchantation}{extra_enchantation}")
        let skipAlreadyDeclared = declaredFunctionsHashes |> key_exists(funcNameHash)
        if skipAlreadyDeclared
            to_log(LOG_INFO, "Function declaration {bind_enchantation} {extra_enchantation} already generated, skipped (possible declared in header twice)\n")
            return 
        declaredFunctionsHashes |> insert <| funcNameHash

        //rename overloads different in result type only
        let funcNameHashWithoutResultHash = hash(functionHeaderSpellingSkipResult(c, true, false, false, das_function_name, namespace_struct_name(), true))
        let needRenameInDas = declaredFunctionsHashesWithoutResults |> key_exists(funcNameHashWithoutResultHash)
        if needRenameInDas
            var res = string(clang_getTypeSpelling(clang_getCanonicalType(res_type)))
            res = replace(replace(res, "*", ""), "&", "") |> strip() //hardcode, remove pointers and references from result type name
            das_function_name = "{das_function_name}_{res}"
        else
            declaredFunctionsHashesWithoutResults |> insert <| funcNameHashWithoutResultHash

        fprint(func_file, "// from {clang_getCursorLocationDescription(c)}\n")
        fprint(func_file, "\tmakeExtern<{bind_enchantation}{extra_enchantation}{get_module_temp_fn()}>(lib,\"{das_function_name}\",\"{cpp_enchantation}\")")
        let narg = clang_Cursor_getNumArguments(c)
        if narg!=0 || isMethod
            fprint(func_file,"\n\t\t->args(\{")
            if isMethod
                fprint(func_file,"\"self\"")
            for ai in urange(narg)
                if ai != 0u || isMethod
                    fprint(func_file,",")
                var carg = clang_Cursor_getArgument(c, ai)
                let arg_name = string(clang_getCursorSpelling(carg))
                fprint(func_file,"\"{arg_name}\"")
            fprint(func_file,"\})")

            //Unreal only logic?, maybe better to declare explicit functions explicitly via config
            let isExplicitFirstArgument = isMethod && isStatic
            if isExplicitFirstArgument
                let self_type = namespace_struct_name()
                fprint(func_file,"\n\t\t->arg_type({0},makeType<TExplicit<const {self_type}*> >(lib))")

            for ai in urange(narg)
                let aii = int(isMethod ? ai+1u : ai)
                var carg = clang_Cursor_getArgument(c, ai)
                let cp = getCursorArgType(clang_getCursorType(carg))     // non-canonical
                if cp != ""
                    fprint(func_file,"\n\t\t->arg_type({aii},makeType<{cp}>(lib))")
                let cv = getCursorInit(carg,cp)
                if cv != ""
                    fprint(func_file,"\n\t\t->arg_init({aii},{cv})")
        let rp = getCursorArgType(nc_res_type)
        if rp != ""
            fprint(func_file,"\n\t\t->res_type(makeType<{rp}>(lib))")
		fprint(func_file, "\n\t\t->addToModule(*this, SideEffects::worstDefault)")
        fprint(func_file,";\n")

    def override typeSpelling(var c: CXType)
        let nspl = string(clang_getTypeSpelling(c))
        if nspl=="size_t" || nspl=="std::size_t" || nspl=="intptr_t" || nspl=="uintptr_t" || nspl=="size_t *"
            return nspl
        var t = clang_getCanonicalType(c)
        var spl = string(clang_getTypeSpelling(t))
        if c.kind == CXTypeKind Enum
            return enum2enum?[spl] ?? spl
        elif t.kind==CXTypeKind Long || t.kind==CXTypeKind LongLong || t.kind==CXTypeKind ULong || t.kind==CXTypeKind ULongLong
            return nspl // if its a fancy type, which may be different on different platform

        var canonicalType = t
        while clang_getPointeeType(canonicalType).kind != CXTypeKind Invalid
            canonicalType = clang_getPointeeType(canonicalType)
        var canonicalTypeStr = string(clang_getTypeSpelling(canonicalType))
        //clang_getUnqualifiedType(t) & clang_getNonReferenceType(canonicalType) doesn't work?
        canonicalTypeStr = replace(replace(canonicalTypeStr, " &", ""), "const ", "") //hardcode
        if replaceTypeNames |> key_exists(canonicalTypeStr)
            spl = spl |> replace(canonicalTypeStr, replaceTypeNames[canonicalTypeStr])
        return spl

    def override functionHeaderSpelling(var c : CXCursor; isMethod, needMethodQualifier, needArgumentNames: bool; function_name, self_type: string) : string
        return functionHeaderSpellingSkipResult(c, isMethod, needMethodQualifier, needArgumentNames, function_name, self_type, false)

    def functionHeaderSpellingSkipResult(var c: CXCursor; isMethod, needMethodQualifier, needArgumentNames:bool; function_name, self_type:string; skip_result: bool): string
        var fun_type = clang_getCursorType(c)
        var res_type = clang_getResultType(fun_type)
        let function_result_type = skip_result ? "" : typeSpelling(res_type)
        let isStatic = clang_Cursor_getStorageClass(c)==CX_StorageClass Static
        return build_string() <| $ ( str )
            str |> write("{function_result_type} {function_name}(")
            if !empty(self_type)
                if isMethod && (clang_CXXMethod_isConst(c)!=0u || isStatic)
                    str |> write("const ")
                
                if isStatic
                    //for static method, convert argument to pointer, so it is possible to call it with nullptr, other it need to create unneeded temp object
                    //but param type still useful for resolve overloads
                    str |> write("{self_type} *") 
                else
                    //for virtual methods -- some object still needed, so it is ok to have const reference argument
                    str |> write("{self_type} &")
                    
                if needArgumentNames && !isStatic
                    str |> write(" self")
            let narg = clang_Cursor_getNumArguments(c)
            for ai in urange(narg)
                if ai != 0u || !empty(self_type)
                    str |> write(",")
                var carg = clang_Cursor_getArgument(c, ai)
                var arg_type = clang_getCursorType(carg)
                let argument_type = typeSpelling(arg_type)
                str |> write("{argument_type}")
                if needArgumentNames
                    var arg_name = string(clang_getCursorSpelling(carg))
                    if empty(arg_name)
                        arg_name = "_arg_{int(ai)}"
                    str |> write(" {arg_name}")
            str |> write(")")
            if isMethod && needMethodQualifier && clang_CXXMethod_isConst(c)!=0u
                str |> write(" const")

    def override parse_Field(var c : CXCursor)
        return if check_deprecated_attribute(c)
        
        let field_name = string(clang_getCursorSpelling(c))
        var field_type = clang_getCanonicalType(clang_getCursorType(c))
        let field_type_name = string(clang_getTypeSpelling(field_type))

        for skipName in unreal_arg_part_names
            if field_type_name |> find(skipName) != -1
                return
        var wr = swrite |> back
        let substType = substFieldType(c)
        if substType |> empty()
            *wr |> write("\t\taddField<DAS_BIND_MANAGED_FIELD({field_name})>(\"{getBindFldName(field_name)}\",\"{field_name}\");\n")
        else
            *wr |> write("\t\taddField<{substType},offsetof(ManagedType,{field_name})>(\"{getBindFldName(field_name)}\",\"{field_name}\")\n")
            *wr |> write("\t\t\t.adjustAot(\"das_reinterpret<{substType}>::pass(\",\")\");\n")

    def check_deprecated_attribute(var cursor : CXCursor)
        let av = clang_getCursorAvailability(cursor)
        return av == CXAvailabilityKind Deprecated

    def override parse_Enum(var cursor: CXCursor; enum_name: string)
        return if check_deprecated_attribute(cursor)
    
        let ns_en = namespace_name(enum_name)
        let das_enum_name = dasEnumName(enum_name,ns_en)
        return if skip_enum(ns_en,enum_name)

        let enumNameHash = hash("{ns_en},{enum_name}")
        var skipAlreadyDeclared = declaredEnumsHashes |> key_exists(enumNameHash)
        if skipAlreadyDeclared
            to_log(LOG_INFO, "Enum declaration {ns_en} {enum_name} already generated, skipped (possible declared twice)\n")
            return
        declaredEnumsHashes |> insert <| enumNameHash

        enumDecl |> push([[auto ns_en,enum_name]])
        let dashing_name = namespace_name(enum_name,"_")
        fprint(enum_class_file, "// from {clang_getCursorLocationDescription(cursor)}\n")
        fprint(enum_class_file, "class Enumeration_{dashing_name} : public das::Enumeration \{\n")
        fprint(enum_class_file, "public:\n")
        fprint(enum_class_file, "\tEnumeration_{dashing_name}() : das::Enumeration(\"{das_enum_name}\") \{\n")
        fprint(enum_class_file, "\t\texternal = true;\n")
        fprint(enum_class_file, "\t\tcppName = \"{ns_en}\";\n")
        let itype = string(clang_getTypeSpelling(clang_getEnumDeclIntegerType(cursor)))
        fprint(enum_class_file, "\t\tbaseType = (das::Type) das::ToBasicType<{itype}>::type;\n")
        clang_visitChildren(cursor) <| $ (var c, parent)
            if c.kind==CXCursorKind EnumConstantDecl
                let een = string(clang_getCursorDisplayName(c))
                let deen = enum_entry_name(enum_name, een)
                
                if !check_deprecated_attribute(c)
                    fprint(enum_class_file, "\t\taddIEx(\"{deen}\", \"{een}\", int64_t({ns_en}::{een}), das::LineInfo());\n")
                return CXChildVisitResult Continue
            else
                return CXChildVisitResult Recurse
        fprint(enum_class_file, "\t\}\n\};\n\n")
        fprint(enum_add_file, "addEnumeration(make_smart<Enumeration_{dashing_name}>());\n");

    def isTemplateType(typeName: string): bool
        return true if typeName == "FStrProperty_Super"   //unreal typedef hardcode, need to find a way resolve it
        return true if typeName == "FArrayProperty_Super"
        return true if typeName == "FSetProperty_Super"
        return true if typeName == "FMapProperty_Super"
        //interfaces
        return true if typeName == "IModuleInterface"
        return true if typeName == "IAssetRegistryInterface"
        
        return typeName |> find("<") != -1

     //TODO: send fix to cbind_boost (lspClone)
    def override generateStructLsp
        var lspClone := lsp //create clone, because iterator lock table
        for k,v in keys(lsp), values(lsp)

            //quick check if first parent is template for skip whole setParent line
            if isTemplateType(v)
                continue
            
            fprint(struct_add_file,"setParents(this,\"{k}\",  \{")
            var pp : array<string>
            pp |> push(v)
            var qv = v
            while lspClone |> key_exists(qv)
                qv = lspClone[qv]
                pp |> push(qv)
            for p,i in pp,count()
                if i!=0
                    fprint(struct_add_file,",")
                if isTemplateType(p)
                    if dummyTemplateInstanceAnnotations |> key_exists(p)
                        fprint(struct_add_file,"\"{transformTemplateName(p)}\"")
                    else
                        to_log(LOG_WARNING, "\nTemplate in parents: {p} for type:{k}, skipped\n")
                else
                    fprint(struct_add_file,"\"{p}\"")
            fprint(struct_add_file,"\});\n")

    def transformTemplateName(s: string)
        return replace(replace(replace(s, "<", "_"), ">", ""),",","_")

    def generateDummpyTemplateInstaceAnnotations
        for templName, i in keys(dummyTemplateInstanceAnnotations), count()
            let templDashedName = transformTemplateName(templName)
            let using_line = "using {templDashedName} = {templName};\n" //comma in template parameters can't be used in macro
            fprint(struct_decl_file, using_line)
            fprint(struct_decl_file, "MAKE_EXTERNAL_TYPE_FACTORY({templDashedName},{templDashedName});\n")
            fprint(struct_impl_file, "IMPLEMENT_EXTERNAL_TYPE_FACTORY({templDashedName},{templDashedName});\n")
            fprint(struct_add_file,"auto ann_{templDashedName} = make_smart<{templDashedName}_GeneratedAnnotation>(lib);\n")
            fprint(struct_add_file,"addAnnotation(ann_{templDashedName});\n")
            fprint(struct_postadd_file,"initRecAnnotation(ann_{templDashedName},lib);\n")

            fprint(struct_class_file, "\n")
            //template can't new/delete by default
            fprint(struct_class_file, "struct {templDashedName}_GeneratedAnnotation final : ManagedStructureAnnotation<\n")
            fprint(struct_class_file, "\t{templDashedName},\n")
            fprint(struct_class_file, "\tstd::is_constructible<{templDashedName}>::value,\n")
            fprint(struct_class_file, "\tstd::is_destructible<{templDashedName}>::value\n")
            fprint(struct_class_file, "> \{\n")
            fprint(struct_class_file, "\t{templDashedName}_GeneratedAnnotation(ModuleLibrary& ml) : ManagedStructureAnnotation(\"{templDashedName}\", ml) \{\n")
            fprint(struct_class_file, "\t\}\n")
            //TODO: split copy-constructible/no constructible templates?
            fprint(struct_class_file, "\tvirtual bool canCopy() const override \{ return true; \}\n")
            fprint(struct_class_file, "\tvirtual bool canClone() const override \{ return true; \}\n")
            fprint(struct_class_file, "\tvirtual bool canMove() const override \{ return true; \}\n")
            fprint(struct_class_file, "\tvoid init() \{\n")
            fprint(struct_class_file, "\t\}\n")
            fprint(struct_class_file, "\};\n")

            if isArgByValue(templName)
                fprint(struct_cast_file, "template <> struct cast_arg<{templDashedName}> \{\n")
                fprint(struct_cast_file, "\tstatic __forceinline const {templDashedName} & to ( Context & ctx, SimNode * node ) \{\n")
                fprint(struct_cast_file, "\t\tvec4f res = node->eval(ctx);\n")
                fprint(struct_cast_file, "\t\treturn * cast<{templDashedName} *>::to(res);\n")
                fprint(struct_cast_file, "\t\}\n")
                fprint(struct_cast_file, "\};\n")

            replaceTypeNames[templName] = templDashedName

    def isTemplateSpecialization(var cursor: CXCursor): bool
        return clang_Cursor_isNull(clang_getSpecializedCursorTemplate(cursor)) == 0

    def override parse(var cursor: CXCursor)
        clang_visitChildren(cursor) <| $(var c, parent)
            var file: CXFile
            var line, column, offset : uint
            clang_getSpellingLocation(clang_getCursorLocation(c), safe_addr(file), safe_addr(line), safe_addr(column), safe_addr(offset))
            var fname = clang_getFileName(file)
            peek(fname) <| $ ( fn )
                if prevFileName != fn
                    prevFileName := fn
                    if skip_file(prevFileName) && verbose_file_skipping
                        to_log(LOG_INFO, "skipping {prevFileName}\n")
            if !skip_file(prevFileName)
                //TODO: move to cbind_boost
                if parent.kind == CXCursorKind ClassTemplate || isTemplateSpecialization(parent)    // skipping just about everything from template body
                    return CXChildVisitResult Continue
                let kind = clang_getCursorKind(c)
                if kind == CXCursorKind TypedefDecl
                    let tdn = string(clang_getCursorDisplayName(c))
                    var tdt = clang_getCanonicalType(clang_getCursorType(c))
                    if hasEmptyStruct
                        if tdt.kind == CXTypeKind Record
                            if !skip_struct(tdn)
                                let acs = clang_getCXXAccessSpecifier(c)
                                if acs != CX_CXXAccessSpecifier Public && acs != CX_CXXAccessSpecifier InvalidAccessSpecifier
                                    to_log(LOG_INFO, "skipping nameless structure {tdn} due to access specifier {acs}\n")
                                else
                                    parse_Struct(emptyStruct, tdn)
                            hasEmptyStruct = false
                            return CXChildVisitResult Continue
                        else
                            to_log(LOG_ERROR, "typedef of type {tdt.kind} with nameless structure at {clang_getCursorLocationDescription(c)}\n")
                            hasEmptyStruct = false
                    elif hasEmptyEnum
                        if tdt.kind == CXTypeKind Enum
                            let acs = clang_getCXXAccessSpecifier(c)
                            if acs != CX_CXXAccessSpecifier Public && acs != CX_CXXAccessSpecifier InvalidAccessSpecifier
                                to_log(LOG_INFO, "skipping nameless enumeration {tdn} due to access specifier {acs}\n")
                            else
                                parse_Enum(emptyEnum, tdn)
                            hasEmptyEnum = false
                            return CXChildVisitResult Continue
                        else
                            to_log(LOG_ERROR, "typedef of type {tdt.kind} with nameless enumeration at {clang_getCursorLocationDescription(c)}\n")
                            hasEmptyEnum = false
                    elif tdt.kind == CXTypeKind Record
                        let sn = string(clang_getTypeSpelling(tdt))
                        if sn == tdn
                            to_log(LOG_INFO, "skipping typedef struct {sn} {tdn};\n")
                            return CXChildVisitResult Continue
                        if skip_struct(sn) || skip_struct(tdn)
                            to_log(LOG_INFO, "skipping typedef struct {sn} {tdn};\n")
                            return CXChildVisitResult Continue
                    elif tdt.kind == CXTypeKind Enum
                        let sn = string(clang_getTypeSpelling(tdt))
                        if sn == tdn
                            to_log(LOG_INFO, "skipping typedef enum {sn} {tdn};\n")
                            return CXChildVisitResult Continue
                        else
                            enum2enum[namespace_name(sn)] = tdn
                            return CXChildVisitResult Continue
                    if inClass == 0
                        if !skip_alias(c)
                            parse_Alias(c)
                    return CXChildVisitResult Continue
                elif kind == CXCursorKind FunctionDecl
                    if !skip_function(c)
                        parse_FunctionDecl(c)
                    return CXChildVisitResult Continue
                elif kind == CXCursorKind CXXMethod
                    let acs = clang_getCXXAccessSpecifier(c)
                    if acs != CX_CXXAccessSpecifier Public
                        let method_name = string(clang_getCursorSpelling(c))
                        to_log(LOG_INFO, "skipping {namespace_name(method_name)} due to access specifier {acs}\n")
                    else
                        if !skip_method(c)
                            parse_Method(c)
                    return CXChildVisitResult Continue
                elif kind == CXCursorKind Constructor
                    let acs = clang_getCXXAccessSpecifier(c)
                    if acs != CX_CXXAccessSpecifier Public
                        let ctor_name = string(clang_getCursorSpelling(c))
                        to_log(LOG_INFO, "skipping {namespace_name(ctor_name)} constructor due to access specifier {acs}\n")
                    else
                        if !skip_constructor(c)
                            parse_Constructor(c)
                    return CXChildVisitResult Continue
                /*
                elif kind == CXCursorKind CXXConversionFunction
                    // TODO: support autobind of cast?
                    return CXChildVisitResult Continue
                elif kind == CXCursorKind CXXDestructor
                    return CXChildVisitResult Continue
                */
                elif kind == CXCursorKind StructDecl || kind == CXCursorKind ClassDecl || kind == CXCursorKind UnionDecl
                    let struct_name = string(clang_getCursorSpelling(c))
                    let nta = clang_Type_getNumTemplateArguments(clang_getCursorType(c))
                    if nta != -1
                        to_log(LOG_INFO, "skipping structure {struct_name} due to being template instance\n")
                        return CXChildVisitResult Continue
                    if empty(struct_name)
                        if hasEmptyStruct
                            to_log(LOG_ERROR, "already in empty struct mode from {clang_getCursorLocationDescription(emptyStruct)}, at {clang_getCursorLocationDescription(c)}\n")
                        emptyStruct = c
                        hasEmptyStruct = true
                    else
                        if !skip_struct(struct_name)
                            let acs = clang_getCXXAccessSpecifier(c)
                            if acs != CX_CXXAccessSpecifier Public && acs != CX_CXXAccessSpecifier InvalidAccessSpecifier
                                var canonical = clang_getCanonicalType(clang_getCursorType(c))
                                let sname = string(clang_getTypeSpelling(clang_getCursorType(c)))
                                to_log(LOG_INFO, "skipping structure {struct_name} aka {sname} due to access specifier {acs}\n")
                                skip_struct_by_access |> insert(sname)
                                return CXChildVisitResult Continue
                            else
                                parse_Struct(c, struct_name)
                    return CXChildVisitResult Continue
                elif kind == CXCursorKind Namespace
                    parse_Namespace(c)
                    return CXChildVisitResult Continue
                elif kind == CXCursorKind EnumDecl
                    let enum_name = string(clang_getCursorSpelling(c))
                    let acs = clang_getCXXAccessSpecifier(c)
                    if acs != CX_CXXAccessSpecifier Public && acs != CX_CXXAccessSpecifier InvalidAccessSpecifier
                        to_log(LOG_INFO, "skipping enumeration {enum_name} due to access specifier {acs}\n")
                    elif empty(enum_name)
                        if hasEmptyEnum
                            to_log(LOG_ERROR, "already in empty enum mode from {clang_getCursorLocationDescription(emptyEnum)}, at {clang_getCursorLocationDescription(c)}\n")
                        emptyEnum = c
                        hasEmptyEnum = true
                    else
                        parse_Enum(c, enum_name)
                    return CXChildVisitResult Continue
                elif kind == CXCursorKind FieldDecl
                    if skip_type(clang_getCursorType(c))
                        let field_name = string(clang_getCursorSpelling(c))
                        to_log(LOG_INFO, "skipping {namespace_name(field_name)} becuase its type is skipped\n")
                        return CXChildVisitResult Continue
                    if clang_Cursor_isBitField(c)!=0u
                        let field_name = string(clang_getCursorSpelling(c))
                        to_log(LOG_INFO, "skipping {namespace_name(field_name)} because its a bitfield\n")
                        return CXChildVisitResult Continue
                    let acs = clang_getCXXAccessSpecifier(c)
                    if acs != CX_CXXAccessSpecifier Public
                        let field_name = string(clang_getCursorSpelling(c))
                        to_log(LOG_INFO, "skipping {namespace_name(field_name)} due to access specifier {acs}\n")
                        return CXChildVisitResult Continue
                    var field_type = clang_getCursorType(c)
                    if skip_type(field_type)
                        let field_name = string(clang_getCursorSpelling(c))
                        let field_type_name = string(clang_getTypeSpelling(field_type))
                        to_log(LOG_INFO, "skipping {namespace_name(field_name)} due to type {field_type_name}\n")
                    else
                        parse_Field(c)
                    return CXChildVisitResult Continue
                elif kind == CXCursorKind VarDecl
                    /*
                    let var_decl_name = string(clang_getCursorSpelling(c))
                    var var_decl_type = clang_getCanonicalType(clang_getCursorType(c))
                    let var_decl_type_name = string(clang_getTypeSpelling(var_decl_type))
                    to_log(LOG_WARNING, "VarDecl {var_decl_name}:{var_decl_type_name} at {clang_getCursorLocationDescription(c)}\n")
                    */
                    return CXChildVisitResult Continue
                elif kind == CXCursorKind CXXAccessSpecifier
                    // nada?
                    return CXChildVisitResult Recurse
                elif kind == CXCursorKind UnexposedDecl
                    // nada?
                    return CXChildVisitResult Recurse
                elif kind == CXCursorKind FunctionTemplate
                    // nada?
                    CXChildVisitResult Continue
                /*
                elif kind == CXCursorKind TemplateTypeParameter
                    // nada?
                    CXChildVisitResult Continue
                elif kind == CXCursorKind TypeRef
                    // nada?
                    CXChildVisitResult Continue
                */
                elif kind == CXCursorKind CXXBaseSpecifier
                    let class_name = back(nspaces)
                    var tps = string(clang_getTypeSpelling(clang_getCursorType(c)))
                    var tpq <- split(tps,"::")  // TODO: better inheritance?
                    let base_name = back(tpq)
                    delete tpq
                    if !(lsp |> key_exists(class_name))
                        if !isTemplateType(base_name)
                            lsp[class_name] = base_name
                    else
                        var acs = clang_getCXXAccessSpecifier(c)
                        if acs == CX_CXXAccessSpecifier Public
                            if !isTemplateType(base_name)
                                interfaces[class_name] |> push(base_name)
                        else
                            to_log(LOG_WARNING, "skipping interface {base_name} due to access specifier {acs} at {clang_getCursorLocationDescription(c)}\n")
                    CXChildVisitResult Recurse
                elif kind == CXCursorKind ClassTemplate
                    let template_name = string(clang_getCursorSpelling(c))
                    to_log(LOG_WARNING, "skipping template {template_name} at {clang_getCursorLocationDescription(c)}\n")
                    CXChildVisitResult Continue
                else
                    to_log(LOG_WARNING, "unsupported {describe(c)} <- {describe(parent)} at {clang_getCursorLocationDescription(c)}\n")
                return CXChildVisitResult Recurse
            else
                return CXChildVisitResult Recurse

    def override generate
        generateDummpyTemplateInstaceAnnotations()
        AnyGenBind`generate(self)
        generateEnumDecl()
        generateDummy()
        if generate_lsp
            generateStructLsp()
            generateStructInterfaces()
        generateModuleH()
        generateModuleNeedH()
        generateModuleCpp()
        if generate_constant_decl
            if empty(CONST_FILE_NAMES)
                generateConstants(open_file_name())
            else
                generateConstantsFrom(CONST_FILE_NAMES)
        closeAllFiles()

[export]
def main
    // generate
    var cgb = new UnrealGen()
    defer <|
        unsafe
            delete cgb
    cgb->generate()
    cgb->genCMakeDecl("DAS_UNREAL_BIND")
