// this file is generated via daScript automatic C++ binder
// all user modifications will be lost after this file is re-generated

template <> struct cast_arg<TArray_float> {
	static __forceinline const TArray_float & to ( Context & ctx, SimNode * node ) {
		vec4f res = node->eval(ctx);
		return * cast<TArray_float *>::to(res);
	}
};
template <> struct cast_arg<TObjectPtr_UStruct> {
	static __forceinline const TObjectPtr_UStruct & to ( Context & ctx, SimNode * node ) {
		vec4f res = node->eval(ctx);
		return * cast<TObjectPtr_UStruct *>::to(res);
	}
};
template <> struct cast_arg<TObjectPtr_UEnum> {
	static __forceinline const TObjectPtr_UEnum & to ( Context & ctx, SimNode * node ) {
		vec4f res = node->eval(ctx);
		return * cast<TObjectPtr_UEnum *>::to(res);
	}
};
template <> struct cast_arg<TObjectPtr_UField> {
	static __forceinline const TObjectPtr_UField & to ( Context & ctx, SimNode * node ) {
		vec4f res = node->eval(ctx);
		return * cast<TObjectPtr_UField *>::to(res);
	}
};
template <> struct cast_arg<TObjectPtr_UClass> {
	static __forceinline const TObjectPtr_UClass & to ( Context & ctx, SimNode * node ) {
		vec4f res = node->eval(ctx);
		return * cast<TObjectPtr_UClass *>::to(res);
	}
};
template <> struct cast_arg<TObjectPtr_UScriptStruct> {
	static __forceinline const TObjectPtr_UScriptStruct & to ( Context & ctx, SimNode * node ) {
		vec4f res = node->eval(ctx);
		return * cast<TObjectPtr_UScriptStruct *>::to(res);
	}
};
template <> struct cast_arg<TArray_FString> {
	static __forceinline const TArray_FString & to ( Context & ctx, SimNode * node ) {
		vec4f res = node->eval(ctx);
		return * cast<TArray_FString *>::to(res);
	}
};
template <> struct cast_arg<TObjectPtr_UFunction> {
	static __forceinline const TObjectPtr_UFunction & to ( Context & ctx, SimNode * node ) {
		vec4f res = node->eval(ctx);
		return * cast<TObjectPtr_UFunction *>::to(res);
	}
};
template <> struct cast_arg<TArray_int> {
	static __forceinline const TArray_int & to ( Context & ctx, SimNode * node ) {
		vec4f res = node->eval(ctx);
		return * cast<TArray_int *>::to(res);
	}
};
template <> struct cast_arg<TObjectPtr_UObject> {
	static __forceinline const TObjectPtr_UObject & to ( Context & ctx, SimNode * node ) {
		vec4f res = node->eval(ctx);
		return * cast<TObjectPtr_UObject *>::to(res);
	}
};
template <> struct cast_arg<FEngineVersionBase> {
	static __forceinline const FEngineVersionBase & to ( Context & ctx, SimNode * node ) {
		vec4f res = node->eval(ctx);
		return * cast<FEngineVersionBase *>::to(res);
	}
};
template <> struct cast_arg<FOutputDevice> {
	static __forceinline const FOutputDevice & to ( Context & ctx, SimNode * node ) {
		vec4f res = node->eval(ctx);
		return * cast<FOutputDevice *>::to(res);
	}
};
template <> struct cast_arg<FArchiveState::FFastPathLoadBuffer> {
	static __forceinline const FArchiveState::FFastPathLoadBuffer & to ( Context & ctx, SimNode * node ) {
		vec4f res = node->eval(ctx);
		return * cast<FArchiveState::FFastPathLoadBuffer *>::to(res);
	}
};
template <> struct cast_arg<FArchiveState> {
	static __forceinline const FArchiveState & to ( Context & ctx, SimNode * node ) {
		vec4f res = node->eval(ctx);
		return * cast<FArchiveState *>::to(res);
	}
};
template <> struct cast_arg<FArchive::FScopeSetDebugSerializationFlags> {
	static __forceinline const FArchive::FScopeSetDebugSerializationFlags & to ( Context & ctx, SimNode * node ) {
		vec4f res = node->eval(ctx);
		return * cast<FArchive::FScopeSetDebugSerializationFlags *>::to(res);
	}
};
template <> struct cast_arg<FArchive::FScopeAddDebugData> {
	static __forceinline const FArchive::FScopeAddDebugData & to ( Context & ctx, SimNode * node ) {
		vec4f res = node->eval(ctx);
		return * cast<FArchive::FScopeAddDebugData *>::to(res);
	}
};
template <> struct cast_arg<FArchive> {
	static __forceinline const FArchive & to ( Context & ctx, SimNode * node ) {
		vec4f res = node->eval(ctx);
		return * cast<FArchive *>::to(res);
	}
};
template <> struct cast_arg<FString> {
	static __forceinline const FString & to ( Context & ctx, SimNode * node ) {
		vec4f res = node->eval(ctx);
		return * cast<FString *>::to(res);
	}
};
template <> struct cast_arg<FTextRange> {
	static __forceinline const FTextRange & to ( Context & ctx, SimNode * node ) {
		vec4f res = node->eval(ctx);
		return * cast<FTextRange *>::to(res);
	}
};
template <> struct cast_arg<FSparseArrayAllocationInfo> {
	static __forceinline const FSparseArrayAllocationInfo & to ( Context & ctx, SimNode * node ) {
		vec4f res = node->eval(ctx);
		return * cast<FSparseArrayAllocationInfo *>::to(res);
	}
};
template <> struct cast_arg<FScriptSparseArrayLayout> {
	static __forceinline const FScriptSparseArrayLayout & to ( Context & ctx, SimNode * node ) {
		vec4f res = node->eval(ctx);
		return * cast<FScriptSparseArrayLayout *>::to(res);
	}
};
template <> struct cast_arg<FScriptSparseArray> {
	static __forceinline const FScriptSparseArray & to ( Context & ctx, SimNode * node ) {
		vec4f res = node->eval(ctx);
		return * cast<FScriptSparseArray *>::to(res);
	}
};
template <> struct cast_arg<FSetElementId> {
	static __forceinline const FSetElementId & to ( Context & ctx, SimNode * node ) {
		vec4f res = node->eval(ctx);
		return * cast<FSetElementId *>::to(res);
	}
};
template <> struct cast_arg<FScriptSetLayout> {
	static __forceinline const FScriptSetLayout & to ( Context & ctx, SimNode * node ) {
		vec4f res = node->eval(ctx);
		return * cast<FScriptSetLayout *>::to(res);
	}
};
template <> struct cast_arg<FScriptSet> {
	static __forceinline const FScriptSet & to ( Context & ctx, SimNode * node ) {
		vec4f res = node->eval(ctx);
		return * cast<FScriptSet *>::to(res);
	}
};
template <> struct cast_arg<TSetPrivateFriend> {
	static __forceinline const TSetPrivateFriend & to ( Context & ctx, SimNode * node ) {
		vec4f res = node->eval(ctx);
		return * cast<TSetPrivateFriend *>::to(res);
	}
};
template <> struct cast_arg<FScriptMapLayout> {
	static __forceinline const FScriptMapLayout & to ( Context & ctx, SimNode * node ) {
		vec4f res = node->eval(ctx);
		return * cast<FScriptMapLayout *>::to(res);
	}
};
template <> struct cast_arg<FScriptMap> {
	static __forceinline const FScriptMap & to ( Context & ctx, SimNode * node ) {
		vec4f res = node->eval(ctx);
		return * cast<FScriptMap *>::to(res);
	}
};
template <> struct cast_arg<TMapPrivateFriend> {
	static __forceinline const TMapPrivateFriend & to ( Context & ctx, SimNode * node ) {
		vec4f res = node->eval(ctx);
		return * cast<TMapPrivateFriend *>::to(res);
	}
};
template <> struct cast_arg<FPrimaryAssetType> {
	static __forceinline const FPrimaryAssetType & to ( Context & ctx, SimNode * node ) {
		vec4f res = node->eval(ctx);
		return * cast<FPrimaryAssetType *>::to(res);
	}
};
template <> struct cast_arg<FPrimaryAssetId> {
	static __forceinline const FPrimaryAssetId & to ( Context & ctx, SimNode * node ) {
		vec4f res = node->eval(ctx);
		return * cast<FPrimaryAssetId *>::to(res);
	}
};
template <> struct cast_arg<FStatGroup_STATGROUP_Script> {
	static __forceinline const FStatGroup_STATGROUP_Script & to ( Context & ctx, SimNode * node ) {
		vec4f res = node->eval(ctx);
		return * cast<FStatGroup_STATGROUP_Script *>::to(res);
	}
};
template <> struct cast_arg<FBlueprintExceptionInfo> {
	static __forceinline const FBlueprintExceptionInfo & to ( Context & ctx, SimNode * node ) {
		vec4f res = node->eval(ctx);
		return * cast<FBlueprintExceptionInfo *>::to(res);
	}
};
template <> struct cast_arg<FScriptInstrumentationSignal> {
	static __forceinline const FScriptInstrumentationSignal & to ( Context & ctx, SimNode * node ) {
		vec4f res = node->eval(ctx);
		return * cast<FScriptInstrumentationSignal *>::to(res);
	}
};
template <> struct cast_arg<FBlueprintCoreDelegates> {
	static __forceinline const FBlueprintCoreDelegates & to ( Context & ctx, SimNode * node ) {
		vec4f res = node->eval(ctx);
		return * cast<FBlueprintCoreDelegates *>::to(res);
	}
};
template <> struct cast_arg<FBlueprintContextTracker> {
	static __forceinline const FBlueprintContextTracker & to ( Context & ctx, SimNode * node ) {
		vec4f res = node->eval(ctx);
		return * cast<FBlueprintContextTracker *>::to(res);
	}
};
template <> struct cast_arg<FEditorScriptExecutionGuard> {
	static __forceinline const FEditorScriptExecutionGuard & to ( Context & ctx, SimNode * node ) {
		vec4f res = node->eval(ctx);
		return * cast<FEditorScriptExecutionGuard *>::to(res);
	}
};
template <> struct cast_arg<FVTableHelper> {
	static __forceinline const FVTableHelper & to ( Context & ctx, SimNode * node ) {
		vec4f res = node->eval(ctx);
		return * cast<FVTableHelper *>::to(res);
	}
};
template <> struct cast_arg<FRestoreForUObjectOverwrite> {
	static __forceinline const FRestoreForUObjectOverwrite & to ( Context & ctx, SimNode * node ) {
		vec4f res = node->eval(ctx);
		return * cast<FRestoreForUObjectOverwrite *>::to(res);
	}
};
template <> struct cast_arg<FObjectInitializer> {
	static __forceinline const FObjectInitializer & to ( Context & ctx, SimNode * node ) {
		vec4f res = node->eval(ctx);
		return * cast<FObjectInitializer *>::to(res);
	}
};
template <> struct cast_arg<FStaticConstructObjectParameters> {
	static __forceinline const FStaticConstructObjectParameters & to ( Context & ctx, SimNode * node ) {
		vec4f res = node->eval(ctx);
		return * cast<FStaticConstructObjectParameters *>::to(res);
	}
};
template <> struct cast_arg<FScriptIntegrationObjectHelper> {
	static __forceinline const FScriptIntegrationObjectHelper & to ( Context & ctx, SimNode * node ) {
		vec4f res = node->eval(ctx);
		return * cast<FScriptIntegrationObjectHelper *>::to(res);
	}
};
template <> struct cast_arg<FReferenceCollector::AROPrivate> {
	static __forceinline const FReferenceCollector::AROPrivate & to ( Context & ctx, SimNode * node ) {
		vec4f res = node->eval(ctx);
		return * cast<FReferenceCollector::AROPrivate *>::to(res);
	}
};
template <> struct cast_arg<FReferenceCollector> {
	static __forceinline const FReferenceCollector & to ( Context & ctx, SimNode * node ) {
		vec4f res = node->eval(ctx);
		return * cast<FReferenceCollector *>::to(res);
	}
};
template <> struct cast_arg<FReferenceFinder> {
	static __forceinline const FReferenceFinder & to ( Context & ctx, SimNode * node ) {
		vec4f res = node->eval(ctx);
		return * cast<FReferenceFinder *>::to(res);
	}
};
template <> struct cast_arg<FEndLoadPackageContext> {
	static __forceinline const FEndLoadPackageContext & to ( Context & ctx, SimNode * node ) {
		vec4f res = node->eval(ctx);
		return * cast<FEndLoadPackageContext *>::to(res);
	}
};
template <> struct cast_arg<FCoreUObjectDelegates> {
	static __forceinline const FCoreUObjectDelegates & to ( Context & ctx, SimNode * node ) {
		vec4f res = node->eval(ctx);
		return * cast<FCoreUObjectDelegates *>::to(res);
	}
};
template <> struct cast_arg<UE::FAssetLog> {
	static __forceinline const UE::FAssetLog & to ( Context & ctx, SimNode * node ) {
		vec4f res = node->eval(ctx);
		return * cast<UE::FAssetLog *>::to(res);
	}
};
template <> struct cast_arg<FAssetMsg> {
	static __forceinline const FAssetMsg & to ( Context & ctx, SimNode * node ) {
		vec4f res = node->eval(ctx);
		return * cast<FAssetMsg *>::to(res);
	}
};
template <> struct cast_arg<UObjectBase> {
	static __forceinline const UObjectBase & to ( Context & ctx, SimNode * node ) {
		vec4f res = node->eval(ctx);
		return * cast<UObjectBase *>::to(res);
	}
};
template <> struct cast_arg<UObjectBaseUtility> {
	static __forceinline const UObjectBaseUtility & to ( Context & ctx, SimNode * node ) {
		vec4f res = node->eval(ctx);
		return * cast<UObjectBaseUtility *>::to(res);
	}
};
template <> struct cast_arg<UObject::FAssetRegistryTagMetadata> {
	static __forceinline const UObject::FAssetRegistryTagMetadata & to ( Context & ctx, SimNode * node ) {
		vec4f res = node->eval(ctx);
		return * cast<UObject::FAssetRegistryTagMetadata *>::to(res);
	}
};
template <> struct cast_arg<UObject> {
	static __forceinline const UObject & to ( Context & ctx, SimNode * node ) {
		vec4f res = node->eval(ctx);
		return * cast<UObject *>::to(res);
	}
};
template <> struct cast_arg<FObjectNetPushIdHelper> {
	static __forceinline const FObjectNetPushIdHelper & to ( Context & ctx, SimNode * node ) {
		vec4f res = node->eval(ctx);
		return * cast<FObjectNetPushIdHelper *>::to(res);
	}
};
template <> struct cast_arg<FFieldClass> {
	static __forceinline const FFieldClass & to ( Context & ctx, SimNode * node ) {
		vec4f res = node->eval(ctx);
		return * cast<FFieldClass *>::to(res);
	}
};
template <> struct cast_arg<FFieldVariant> {
	static __forceinline const FFieldVariant & to ( Context & ctx, SimNode * node ) {
		vec4f res = node->eval(ctx);
		return * cast<FFieldVariant *>::to(res);
	}
};
template <> struct cast_arg<FField> {
	static __forceinline const FField & to ( Context & ctx, SimNode * node ) {
		vec4f res = node->eval(ctx);
		return * cast<FField *>::to(res);
	}
};
template <> struct cast_arg<FObjectPtr> {
	static __forceinline const FObjectPtr & to ( Context & ctx, SimNode * node ) {
		vec4f res = node->eval(ctx);
		return * cast<FObjectPtr *>::to(res);
	}
};
template <> struct cast_arg<ObjectPtr_Private::Friend> {
	static __forceinline const ObjectPtr_Private::Friend & to ( Context & ctx, SimNode * node ) {
		vec4f res = node->eval(ctx);
		return * cast<ObjectPtr_Private::Friend *>::to(res);
	}
};
template <> struct cast_arg<UField> {
	static __forceinline const UField & to ( Context & ctx, SimNode * node ) {
		vec4f res = node->eval(ctx);
		return * cast<UField *>::to(res);
	}
};
template <> struct cast_arg<UStruct> {
	static __forceinline const UStruct & to ( Context & ctx, SimNode * node ) {
		vec4f res = node->eval(ctx);
		return * cast<UStruct *>::to(res);
	}
};
template <> struct cast_arg<UScriptStruct::ICppStructOps::FCapabilities> {
	static __forceinline const UScriptStruct::ICppStructOps::FCapabilities & to ( Context & ctx, SimNode * node ) {
		vec4f res = node->eval(ctx);
		return * cast<UScriptStruct::ICppStructOps::FCapabilities *>::to(res);
	}
};
template <> struct cast_arg<UScriptStruct::ICppStructOps> {
	static __forceinline const UScriptStruct::ICppStructOps & to ( Context & ctx, SimNode * node ) {
		vec4f res = node->eval(ctx);
		return * cast<UScriptStruct::ICppStructOps *>::to(res);
	}
};
template <> struct cast_arg<UScriptStruct> {
	static __forceinline const UScriptStruct & to ( Context & ctx, SimNode * node ) {
		vec4f res = node->eval(ctx);
		return * cast<UScriptStruct *>::to(res);
	}
};
template <> struct cast_arg<UFunction> {
	static __forceinline const UFunction & to ( Context & ctx, SimNode * node ) {
		vec4f res = node->eval(ctx);
		return * cast<UFunction *>::to(res);
	}
};
template <> struct cast_arg<UEnum> {
	static __forceinline const UEnum & to ( Context & ctx, SimNode * node ) {
		vec4f res = node->eval(ctx);
		return * cast<UEnum *>::to(res);
	}
};
template <> struct cast_arg<FCppClassTypeTraitsBase> {
	static __forceinline const FCppClassTypeTraitsBase & to ( Context & ctx, SimNode * node ) {
		vec4f res = node->eval(ctx);
		return * cast<FCppClassTypeTraitsBase *>::to(res);
	}
};
template <> struct cast_arg<ICppClassTypeInfo> {
	static __forceinline const ICppClassTypeInfo & to ( Context & ctx, SimNode * node ) {
		vec4f res = node->eval(ctx);
		return * cast<ICppClassTypeInfo *>::to(res);
	}
};
template <> struct cast_arg<FCppClassTypeInfoStatic> {
	static __forceinline const FCppClassTypeInfoStatic & to ( Context & ctx, SimNode * node ) {
		vec4f res = node->eval(ctx);
		return * cast<FCppClassTypeInfoStatic *>::to(res);
	}
};
template <> struct cast_arg<FCppClassTypeInfo> {
	static __forceinline const FCppClassTypeInfo & to ( Context & ctx, SimNode * node ) {
		vec4f res = node->eval(ctx);
		return * cast<FCppClassTypeInfo *>::to(res);
	}
};
template <> struct cast_arg<FImplementedInterface> {
	static __forceinline const FImplementedInterface & to ( Context & ctx, SimNode * node ) {
		vec4f res = node->eval(ctx);
		return * cast<FImplementedInterface *>::to(res);
	}
};
template <> struct cast_arg<FNativeFunctionLookup> {
	static __forceinline const FNativeFunctionLookup & to ( Context & ctx, SimNode * node ) {
		vec4f res = node->eval(ctx);
		return * cast<FNativeFunctionLookup *>::to(res);
	}
};
template <> struct cast_arg<FClassFunctionLinkInfo> {
	static __forceinline const FClassFunctionLinkInfo & to ( Context & ctx, SimNode * node ) {
		vec4f res = node->eval(ctx);
		return * cast<FClassFunctionLinkInfo *>::to(res);
	}
};
template <> struct cast_arg<UClass> {
	static __forceinline const UClass & to ( Context & ctx, SimNode * node ) {
		vec4f res = node->eval(ctx);
		return * cast<UClass *>::to(res);
	}
};
template <> struct cast_arg<FTestUninitializedScriptStructMembersTest> {
	static __forceinline const FTestUninitializedScriptStructMembersTest & to ( Context & ctx, SimNode * node ) {
		vec4f res = node->eval(ctx);
		return * cast<FTestUninitializedScriptStructMembersTest *>::to(res);
	}
};
template <> struct cast_arg<FSavePackageResultStruct> {
	static __forceinline const FSavePackageResultStruct & to ( Context & ctx, SimNode * node ) {
		vec4f res = node->eval(ctx);
		return * cast<FSavePackageResultStruct *>::to(res);
	}
};
template <> struct cast_arg<UPackage> {
	static __forceinline const UPackage & to ( Context & ctx, SimNode * node ) {
		vec4f res = node->eval(ctx);
		return * cast<UPackage *>::to(res);
	}
};
template <> struct cast_arg<FProperty> {
	static __forceinline const FProperty & to ( Context & ctx, SimNode * node ) {
		vec4f res = node->eval(ctx);
		return * cast<FProperty *>::to(res);
	}
};
template <> struct cast_arg<FPropertyHelpers> {
	static __forceinline const FPropertyHelpers & to ( Context & ctx, SimNode * node ) {
		vec4f res = node->eval(ctx);
		return * cast<FPropertyHelpers *>::to(res);
	}
};
template <> struct cast_arg<UEProperty_Private::FProperty_DoNotUse> {
	static __forceinline const UEProperty_Private::FProperty_DoNotUse & to ( Context & ctx, SimNode * node ) {
		vec4f res = node->eval(ctx);
		return * cast<UEProperty_Private::FProperty_DoNotUse *>::to(res);
	}
};
template <> struct cast_arg<FDefinedProperty> {
	static __forceinline const FDefinedProperty & to ( Context & ctx, SimNode * node ) {
		vec4f res = node->eval(ctx);
		return * cast<FDefinedProperty *>::to(res);
	}
};
template <> struct cast_arg<FNumericProperty> {
	static __forceinline const FNumericProperty & to ( Context & ctx, SimNode * node ) {
		vec4f res = node->eval(ctx);
		return * cast<FNumericProperty *>::to(res);
	}
};
template <> struct cast_arg<FByteProperty> {
	static __forceinline const FByteProperty & to ( Context & ctx, SimNode * node ) {
		vec4f res = node->eval(ctx);
		return * cast<FByteProperty *>::to(res);
	}
};
template <> struct cast_arg<FInt8Property> {
	static __forceinline const FInt8Property & to ( Context & ctx, SimNode * node ) {
		vec4f res = node->eval(ctx);
		return * cast<FInt8Property *>::to(res);
	}
};
template <> struct cast_arg<FInt16Property> {
	static __forceinline const FInt16Property & to ( Context & ctx, SimNode * node ) {
		vec4f res = node->eval(ctx);
		return * cast<FInt16Property *>::to(res);
	}
};
template <> struct cast_arg<FIntProperty> {
	static __forceinline const FIntProperty & to ( Context & ctx, SimNode * node ) {
		vec4f res = node->eval(ctx);
		return * cast<FIntProperty *>::to(res);
	}
};
template <> struct cast_arg<FInt64Property> {
	static __forceinline const FInt64Property & to ( Context & ctx, SimNode * node ) {
		vec4f res = node->eval(ctx);
		return * cast<FInt64Property *>::to(res);
	}
};
template <> struct cast_arg<FUInt16Property> {
	static __forceinline const FUInt16Property & to ( Context & ctx, SimNode * node ) {
		vec4f res = node->eval(ctx);
		return * cast<FUInt16Property *>::to(res);
	}
};
template <> struct cast_arg<FUInt32Property> {
	static __forceinline const FUInt32Property & to ( Context & ctx, SimNode * node ) {
		vec4f res = node->eval(ctx);
		return * cast<FUInt32Property *>::to(res);
	}
};
template <> struct cast_arg<FUInt64Property> {
	static __forceinline const FUInt64Property & to ( Context & ctx, SimNode * node ) {
		vec4f res = node->eval(ctx);
		return * cast<FUInt64Property *>::to(res);
	}
};
template <> struct cast_arg<FFloatProperty> {
	static __forceinline const FFloatProperty & to ( Context & ctx, SimNode * node ) {
		vec4f res = node->eval(ctx);
		return * cast<FFloatProperty *>::to(res);
	}
};
template <> struct cast_arg<FDoubleProperty> {
	static __forceinline const FDoubleProperty & to ( Context & ctx, SimNode * node ) {
		vec4f res = node->eval(ctx);
		return * cast<FDoubleProperty *>::to(res);
	}
};
template <> struct cast_arg<FBoolProperty> {
	static __forceinline const FBoolProperty & to ( Context & ctx, SimNode * node ) {
		vec4f res = node->eval(ctx);
		return * cast<FBoolProperty *>::to(res);
	}
};
template <> struct cast_arg<FObjectPropertyBase> {
	static __forceinline const FObjectPropertyBase & to ( Context & ctx, SimNode * node ) {
		vec4f res = node->eval(ctx);
		return * cast<FObjectPropertyBase *>::to(res);
	}
};
template <> struct cast_arg<FObjectProperty> {
	static __forceinline const FObjectProperty & to ( Context & ctx, SimNode * node ) {
		vec4f res = node->eval(ctx);
		return * cast<FObjectProperty *>::to(res);
	}
};
template <> struct cast_arg<FObjectPtrProperty> {
	static __forceinline const FObjectPtrProperty & to ( Context & ctx, SimNode * node ) {
		vec4f res = node->eval(ctx);
		return * cast<FObjectPtrProperty *>::to(res);
	}
};
template <> struct cast_arg<FWeakObjectProperty> {
	static __forceinline const FWeakObjectProperty & to ( Context & ctx, SimNode * node ) {
		vec4f res = node->eval(ctx);
		return * cast<FWeakObjectProperty *>::to(res);
	}
};
template <> struct cast_arg<FLazyObjectProperty> {
	static __forceinline const FLazyObjectProperty & to ( Context & ctx, SimNode * node ) {
		vec4f res = node->eval(ctx);
		return * cast<FLazyObjectProperty *>::to(res);
	}
};
template <> struct cast_arg<FSoftObjectProperty> {
	static __forceinline const FSoftObjectProperty & to ( Context & ctx, SimNode * node ) {
		vec4f res = node->eval(ctx);
		return * cast<FSoftObjectProperty *>::to(res);
	}
};
template <> struct cast_arg<FClassProperty> {
	static __forceinline const FClassProperty & to ( Context & ctx, SimNode * node ) {
		vec4f res = node->eval(ctx);
		return * cast<FClassProperty *>::to(res);
	}
};
template <> struct cast_arg<FClassPtrProperty> {
	static __forceinline const FClassPtrProperty & to ( Context & ctx, SimNode * node ) {
		vec4f res = node->eval(ctx);
		return * cast<FClassPtrProperty *>::to(res);
	}
};
template <> struct cast_arg<FSoftClassProperty> {
	static __forceinline const FSoftClassProperty & to ( Context & ctx, SimNode * node ) {
		vec4f res = node->eval(ctx);
		return * cast<FSoftClassProperty *>::to(res);
	}
};
template <> struct cast_arg<FStrProperty> {
	static __forceinline const FStrProperty & to ( Context & ctx, SimNode * node ) {
		vec4f res = node->eval(ctx);
		return * cast<FStrProperty *>::to(res);
	}
};
template <> struct cast_arg<FArrayProperty> {
	static __forceinline const FArrayProperty & to ( Context & ctx, SimNode * node ) {
		vec4f res = node->eval(ctx);
		return * cast<FArrayProperty *>::to(res);
	}
};
template <> struct cast_arg<FMapProperty> {
	static __forceinline const FMapProperty & to ( Context & ctx, SimNode * node ) {
		vec4f res = node->eval(ctx);
		return * cast<FMapProperty *>::to(res);
	}
};
template <> struct cast_arg<FSetProperty> {
	static __forceinline const FSetProperty & to ( Context & ctx, SimNode * node ) {
		vec4f res = node->eval(ctx);
		return * cast<FSetProperty *>::to(res);
	}
};
template <> struct cast_arg<FStructProperty> {
	static __forceinline const FStructProperty & to ( Context & ctx, SimNode * node ) {
		vec4f res = node->eval(ctx);
		return * cast<FStructProperty *>::to(res);
	}
};
template <> struct cast_arg<FCustomPropertyListNode> {
	static __forceinline const FCustomPropertyListNode & to ( Context & ctx, SimNode * node ) {
		vec4f res = node->eval(ctx);
		return * cast<FCustomPropertyListNode *>::to(res);
	}
};
template <> struct cast_arg<UPropertyWrapper> {
	static __forceinline const UPropertyWrapper & to ( Context & ctx, SimNode * node ) {
		vec4f res = node->eval(ctx);
		return * cast<UPropertyWrapper *>::to(res);
	}
};
template <> struct cast_arg<FOutParmRec> {
	static __forceinline const FOutParmRec & to ( Context & ctx, SimNode * node ) {
		vec4f res = node->eval(ctx);
		return * cast<FOutParmRec *>::to(res);
	}
};
template <> struct cast_arg<FFrame> {
	static __forceinline const FFrame & to ( Context & ctx, SimNode * node ) {
		vec4f res = node->eval(ctx);
		return * cast<FFrame *>::to(res);
	}
};
template <> struct cast_arg<UBlueprintFunctionLibrary> {
	static __forceinline const UBlueprintFunctionLibrary & to ( Context & ctx, SimNode * node ) {
		vec4f res = node->eval(ctx);
		return * cast<UBlueprintFunctionLibrary *>::to(res);
	}
};
template <> struct cast_arg<UKismetArrayLibrary> {
	static __forceinline const UKismetArrayLibrary & to ( Context & ctx, SimNode * node ) {
		vec4f res = node->eval(ctx);
		return * cast<UKismetArrayLibrary *>::to(res);
	}
};
