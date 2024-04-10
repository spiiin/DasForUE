// this file is generated via daScript automatic C++ binder
// all user modifications will be lost after this file is re-generated

EVersionComparison _dasUnreal_static_0_GetNewest(const FEngineVersionBase *,const FEngineVersionBase & First,const FEngineVersionBase & Second,EVersionComponent * OutComponent) {
	return FEngineVersionBase::GetNewest(First,Second,OutComponent);
}

unsigned int _dasUnreal_static_1_EncodeLicenseeChangelist(const FEngineVersionBase *,unsigned int Changelist) {
	return FEngineVersionBase::EncodeLicenseeChangelist(Changelist);
}

void _dasUnreal_virtual_2_Serialize(FOutputDevice & self,const wchar_t * V,ELogVerbosity::Type Verbosity,const FName & Category,const double Time) {
	self.Serialize(V,Verbosity,Category,Time);
}

void _dasUnreal_virtual_3_SerializeRecord(FOutputDevice & self,const UE::FLogRecord & Record) {
	self.SerializeRecord(Record);
}

void _dasUnreal_virtual_4_Flush(FOutputDevice & self) {
	self.Flush();
}

void _dasUnreal_virtual_5_TearDown(FOutputDevice & self) {
	self.TearDown();
}

void _dasUnreal_virtual_6_Dump(FOutputDevice & self,FArchive & Ar) {
	self.Dump(Ar);
}

bool _dasUnreal_virtual_7_IsMemoryOnly(const FOutputDevice & self) {
	return self.IsMemoryOnly();
}

bool _dasUnreal_virtual_8_CanBeUsedOnAnyThread(const FOutputDevice & self) {
	return self.CanBeUsedOnAnyThread();
}

bool _dasUnreal_virtual_9_CanBeUsedOnMultipleThreads(const FOutputDevice & self) {
	return self.CanBeUsedOnMultipleThreads();
}

bool _dasUnreal_virtual_10_CanBeUsedOnPanicThread(const FOutputDevice & self) {
	return self.CanBeUsedOnPanicThread();
}

FArchiveState & _dasUnreal_virtual_11_GetInnermostState(FArchiveState & self) {
	return self.GetInnermostState();
}

void _dasUnreal_virtual_12_CountBytes(FArchiveState & self,SIZE_T InNum,SIZE_T InMax) {
	self.CountBytes(InNum,InMax);
}

FString _dasUnreal_virtual_13_GetArchiveName(const FArchiveState & self) {
	return self.GetArchiveName();
}

FLinker * _dasUnreal_virtual_14_GetLinker(FArchiveState & self) {
	return self.GetLinker();
}

int64 _dasUnreal_virtual_15_Tell(FArchiveState & self) {
	return self.Tell();
}

int64 _dasUnreal_virtual_16_TotalSize(FArchiveState & self) {
	return self.TotalSize();
}

bool _dasUnreal_virtual_17_AtEnd(FArchiveState & self) {
	return self.AtEnd();
}

UObject * _dasUnreal_virtual_18_GetArchetypeFromLoader(FArchiveState & self,const UObject * Obj) {
	return self.GetArchetypeFromLoader(Obj);
}

unsigned int _dasUnreal_virtual_19_EngineNetVer(const FArchiveState & self) {
	return self.EngineNetVer();
}

unsigned int _dasUnreal_virtual_20_GameNetVer(const FArchiveState & self) {
	return self.GameNetVer();
}

void _dasUnreal_virtual_21_ResetCustomVersions(FArchiveState & self) {
	self.ResetCustomVersions();
}

void _dasUnreal_virtual_22_SetFilterEditorOnly(FArchiveState & self,bool InFilterEditorOnly) {
	self.SetFilterEditorOnly(InFilterEditorOnly);
}

bool _dasUnreal_virtual_23_UseToResolveEnumerators(const FArchiveState & self) {
	return self.UseToResolveEnumerators();
}

bool _dasUnreal_virtual_24_ShouldSkipProperty(const FArchiveState & self,const FProperty * InProperty) {
	return self.ShouldSkipProperty(InProperty);
}

void _dasUnreal_virtual_25_SetSerializedProperty(FArchiveState & self,FProperty * InProperty) {
	self.SetSerializedProperty(InProperty);
}

void _dasUnreal_virtual_26_SetSerializedPropertyChain(FArchiveState & self,const FArchiveSerializedPropertyChain * InSerializedPropertyChain,FProperty * InSerializedPropertyOverride) {
	self.SetSerializedPropertyChain(InSerializedPropertyChain,InSerializedPropertyOverride);
}

bool _dasUnreal_virtual_27_IsEditorOnlyPropertyOnTheStack(const FArchiveState & self) {
	return self.IsEditorOnlyPropertyOnTheStack();
}

void _dasUnreal_virtual_28_SetSerializeContext(FArchiveState & self,FUObjectSerializeContext * InLoadContext) {
	self.SetSerializeContext(InLoadContext);
}

FUObjectSerializeContext * _dasUnreal_virtual_29_GetSerializeContext(FArchiveState & self) {
	return self.GetSerializeContext();
}

void _dasUnreal_virtual_30_SetLocalizationNamespace(FArchiveState & self,const FString & InLocalizationNamespace) {
	self.SetLocalizationNamespace(InLocalizationNamespace);
}

FString _dasUnreal_virtual_31_GetLocalizationNamespace(const FArchiveState & self) {
	return self.GetLocalizationNamespace();
}

void _dasUnreal_virtual_32_Reset(FArchiveState & self) {
	self.Reset();
}

void _dasUnreal_virtual_33_SetIsLoading(FArchiveState & self,bool bInIsLoading) {
	self.SetIsLoading(bInIsLoading);
}

void _dasUnreal_virtual_34_SetIsLoadingFromCookedPackage(FArchiveState & self,bool bInIsLoadingFromCookedPackage) {
	self.SetIsLoadingFromCookedPackage(bInIsLoadingFromCookedPackage);
}

void _dasUnreal_virtual_35_SetIsSaving(FArchiveState & self,bool bInIsSaving) {
	self.SetIsSaving(bInIsSaving);
}

void _dasUnreal_virtual_36_SetIsTransacting(FArchiveState & self,bool bInIsTransacting) {
	self.SetIsTransacting(bInIsTransacting);
}

void _dasUnreal_virtual_37_SetIsTextFormat(FArchiveState & self,bool bInIsTextFormat) {
	self.SetIsTextFormat(bInIsTextFormat);
}

void _dasUnreal_virtual_38_SetWantBinaryPropertySerialization(FArchiveState & self,bool bInWantBinaryPropertySerialization) {
	self.SetWantBinaryPropertySerialization(bInWantBinaryPropertySerialization);
}

void _dasUnreal_virtual_39_SetUseUnversionedPropertySerialization(FArchiveState & self,bool bInUseUnversioned) {
	self.SetUseUnversionedPropertySerialization(bInUseUnversioned);
}

void _dasUnreal_virtual_40_SetForceUnicode(FArchiveState & self,bool bInForceUnicode) {
	self.SetForceUnicode(bInForceUnicode);
}

void _dasUnreal_virtual_41_SetIsPersistent(FArchiveState & self,bool bInIsPersistent) {
	self.SetIsPersistent(bInIsPersistent);
}

void _dasUnreal_virtual_42_SetLicenseeUEVer(FArchiveState & self,int InVer) {
	self.SetLicenseeUEVer(InVer);
}

void _dasUnreal_virtual_43_SetEngineVer(FArchiveState & self,const FEngineVersionBase & InVer) {
	self.SetEngineVer(InVer);
}

void _dasUnreal_virtual_44_SetEngineNetVer(FArchiveState & self,const unsigned int InEngineNetVer) {
	self.SetEngineNetVer(InEngineNetVer);
}

void _dasUnreal_virtual_45_SetGameNetVer(FArchiveState & self,const unsigned int InGameNetVer) {
	self.SetGameNetVer(InGameNetVer);
}

void _dasUnreal_virtual_46_ForceBlueprintFinalization(FArchive & self) {
	self.ForceBlueprintFinalization();
}

void _dasUnreal_virtual_47_Serialize(FArchive & self,void * V,int64 Length) {
	self.Serialize(V,Length);
}

void _dasUnreal_virtual_48_SerializeBits(FArchive & self,void * V,int64 LengthBits) {
	self.SerializeBits(V,LengthBits);
}

void _dasUnreal_virtual_49_SerializeInt(FArchive & self,unsigned int & Value,unsigned int Max) {
	self.SerializeInt(Value,Max);
}

void _dasUnreal_virtual_50_SerializeIntPacked(FArchive & self,unsigned int & Value) {
	self.SerializeIntPacked(Value);
}

void _dasUnreal_virtual_51_SerializeIntPacked64(FArchive & self,unsigned long long & Value) {
	self.SerializeIntPacked64(Value);
}

void _dasUnreal_virtual_52_Preload(FArchive & self,UObject * Object) {
	self.Preload(Object);
}

void _dasUnreal_virtual_53_Seek(FArchive & self,int64 InPos) {
	self.Seek(InPos);
}

void _dasUnreal_virtual_54_AttachBulkData(FArchive & self,UE::Serialization::FEditorBulkData * BulkData) {
	self.AttachBulkData(BulkData);
}

void _dasUnreal_virtual_55_DetachBulkData(FArchive & self,UE::Serialization::FEditorBulkData * BulkData,bool bEnsureBulkDataIsLoaded) {
	self.DetachBulkData(BulkData,bEnsureBulkDataIsLoaded);
}

bool _dasUnreal_virtual_56_IsProxyOf(const FArchive & self,FArchive * InOther) {
	return self.IsProxyOf(InOther);
}

bool _dasUnreal_virtual_57_Precache(FArchive & self,int64 PrecacheOffset,int64 PrecacheSize) {
	return self.Precache(PrecacheOffset,PrecacheSize);
}

void _dasUnreal_virtual_58_FlushCache(FArchive & self) {
	self.FlushCache();
}

void _dasUnreal_virtual_59_Flush(FArchive & self) {
	self.Flush();
}

bool _dasUnreal_virtual_60_Close(FArchive & self) {
	return self.Close();
}

void _dasUnreal_virtual_61_MarkScriptSerializationStart(FArchive & self,const UObject * Obj) {
	self.MarkScriptSerializationStart(Obj);
}

void _dasUnreal_virtual_62_MarkScriptSerializationEnd(FArchive & self,const UObject * Obj) {
	self.MarkScriptSerializationEnd(Obj);
}

FArchive * _dasUnreal_virtual_63_GetCacheableArchive(FArchive & self) {
	return self.GetCacheableArchive();
}

void _dasUnreal_virtual_64_PushSerializedProperty(FArchive & self,FProperty * InProperty,const bool bIsEditorOnlyProperty) {
	self.PushSerializedProperty(InProperty,bIsEditorOnlyProperty);
}

void _dasUnreal_virtual_65_PopSerializedProperty(FArchive & self,FProperty * InProperty,const bool bIsEditorOnlyProperty) {
	self.PopSerializedProperty(InProperty,bIsEditorOnlyProperty);
}

void _dasUnreal_virtual_66_PushDebugDataString(FArchive & self,const FName & DebugData) {
	self.PushDebugDataString(DebugData);
}

void _dasUnreal_virtual_67_PopDebugDataString(FArchive & self) {
	self.PopDebugDataString();
}

void _dasUnreal_virtual_68_PushFileRegionType(FArchive & self,EFileRegionType Type) {
	self.PushFileRegionType(Type);
}

void _dasUnreal_virtual_69_PopFileRegionType(FArchive & self) {
	self.PopFileRegionType();
}

FString _dasUnreal_static_70_Chr(const FString *,wchar_t Ch) {
	return FString::Chr(Ch);
}

FString _dasUnreal_static_71_ChrN(const FString *,int NumCharacters,wchar_t Char) {
	return FString::ChrN(NumCharacters,Char);
}

FString _dasUnreal_static_72_FormatAsNumber(const FString *,int InNumber) {
	return FString::FormatAsNumber(InNumber);
}

FString _dasUnreal_static_73_FromInt(const FString *,int Num) {
	return FString::FromInt(Num);
}

FString _dasUnreal_static_74_FromBlob(const FString *,const unsigned char * SrcBuffer,const unsigned int SrcSize) {
	return FString::FromBlob(SrcBuffer,SrcSize);
}

bool _dasUnreal_static_75_ToBlob(const FString *,const FString & Source,unsigned char * DestBuffer,const unsigned int DestSize) {
	return FString::ToBlob(Source,DestBuffer,DestSize);
}

FString _dasUnreal_static_76_FromHexBlob(const FString *,const unsigned char * SrcBuffer,const unsigned int SrcSize) {
	return FString::FromHexBlob(SrcBuffer,SrcSize);
}

bool _dasUnreal_static_77_ToHexBlob(const FString *,const FString & Source,unsigned char * DestBuffer,const unsigned int DestSize) {
	return FString::ToHexBlob(Source,DestBuffer,DestSize);
}

FString _dasUnreal_static_78_SanitizeFloat(const FString *,double InFloat,const int InMinFractionalDigits) {
	return FString::SanitizeFloat(InFloat,InMinFractionalDigits);
}

FPrimaryAssetId _dasUnreal_static_79_ParseTypeAndName(const FPrimaryAssetId *,const wchar_t * TypeAndName,unsigned int Len) {
	return FPrimaryAssetId::ParseTypeAndName(TypeAndName,Len);
}

FPrimaryAssetId _dasUnreal_static_80_ParseTypeAndName(const FPrimaryAssetId *,FName TypeAndName) {
	return FPrimaryAssetId::ParseTypeAndName(TypeAndName);
}

FPrimaryAssetId _dasUnreal_static_81_ParseTypeAndName(const FPrimaryAssetId *,const FString & TypeAndName) {
	return FPrimaryAssetId::ParseTypeAndName(TypeAndName);
}

FPrimaryAssetId _dasUnreal_static_82_FromString(const FPrimaryAssetId *,const FString & String) {
	return FPrimaryAssetId::FromString(String);
}

const char * _dasUnreal_static_83_GetGroupName(const FStatGroup_STATGROUP_Script *) {
	return FStatGroup_STATGROUP_Script::GetGroupName();
}

const char * _dasUnreal_static_84_GetGroupCategory(const FStatGroup_STATGROUP_Script *) {
	return FStatGroup_STATGROUP_Script::GetGroupCategory();
}

const wchar_t * _dasUnreal_static_85_GetDescription(const FStatGroup_STATGROUP_Script *) {
	return FStatGroup_STATGROUP_Script::GetDescription();
}

bool _dasUnreal_static_86_IsDefaultEnabled(const FStatGroup_STATGROUP_Script *) {
	return FStatGroup_STATGROUP_Script::IsDefaultEnabled();
}

bool _dasUnreal_static_87_IsCompileTimeEnable(const FStatGroup_STATGROUP_Script *) {
	return FStatGroup_STATGROUP_Script::IsCompileTimeEnable();
}

bool _dasUnreal_static_88_GetSortByName(const FStatGroup_STATGROUP_Script *) {
	return FStatGroup_STATGROUP_Script::GetSortByName();
}

void _dasUnreal_static_89_ThrowScriptException(const FBlueprintCoreDelegates *,const UObject * ActiveObject,FFrame & StackFrame,const FBlueprintExceptionInfo & Info) {
	FBlueprintCoreDelegates::ThrowScriptException(ActiveObject,StackFrame,Info);
}

void _dasUnreal_static_90_InstrumentScriptEvent(const FBlueprintCoreDelegates *,const FScriptInstrumentationSignal & Info) {
	FBlueprintCoreDelegates::InstrumentScriptEvent(Info);
}

void _dasUnreal_static_91_SetScriptMaximumLoopIterations(const FBlueprintCoreDelegates *,const int MaximumLoopIterations) {
	FBlueprintCoreDelegates::SetScriptMaximumLoopIterations(MaximumLoopIterations);
}

bool _dasUnreal_static_92_IsDebuggingEnabled(const FBlueprintCoreDelegates *) {
	return FBlueprintCoreDelegates::IsDebuggingEnabled();
}

FBlueprintContextTracker & _dasUnreal_static_93_Get(const FBlueprintContextTracker *) {
	return FBlueprintContextTracker::Get();
}

const FBlueprintContextTracker * _dasUnreal_static_94_TryGet(const FBlueprintContextTracker *) {
	return FBlueprintContextTracker::TryGet();
}

void _dasUnreal_static_95_AssertIfInConstructor(const FObjectInitializer *,UObject * Outer,const wchar_t * ErrorMessage) {
	FObjectInitializer::AssertIfInConstructor(Outer,ErrorMessage);
}

FObjectInitializer & _dasUnreal_static_96_Get(const FObjectInitializer *) {
	return FObjectInitializer::Get();
}

void _dasUnreal_static_97_InitProperties(const FScriptIntegrationObjectHelper *,const FObjectInitializer & ObjectInitializer,UObject * Obj,UClass * DefaultsClass,UObject * DefaultData) {
	FScriptIntegrationObjectHelper::InitProperties(ObjectInitializer,Obj,DefaultsClass,DefaultData);
}

bool _dasUnreal_static_98_InitSubobjectProperties(const FScriptIntegrationObjectHelper *,const FObjectInitializer & ObjectInitializer) {
	return FScriptIntegrationObjectHelper::InitSubobjectProperties(ObjectInitializer);
}

void _dasUnreal_static_99_InstanceSubobjects(const FScriptIntegrationObjectHelper *,const FObjectInitializer & ObjectInitializer,UClass * Class,bool bNeedInstancing,bool bNeedSubobjectInstancing) {
	FScriptIntegrationObjectHelper::InstanceSubobjects(ObjectInitializer,Class,bNeedInstancing,bNeedSubobjectInstancing);
}

void _dasUnreal_static_100_PostConstructInitObject(const FScriptIntegrationObjectHelper *,FObjectInitializer & ObjectInitializer) {
	FScriptIntegrationObjectHelper::PostConstructInitObject(ObjectInitializer);
}

void _dasUnreal_virtual_101_AddStableReference(FReferenceCollector & self,UObject ** Object) {
	self.AddStableReference(Object);
}

bool _dasUnreal_virtual_102_NeedsPropertyReferencer(const FReferenceCollector & self) {
	return self.NeedsPropertyReferencer();
}

void _dasUnreal_virtual_103_AllowEliminatingReferences(FReferenceCollector & self,bool bAllow) {
	self.AllowEliminatingReferences(bAllow);
}

void _dasUnreal_virtual_104_SetSerializedProperty(FReferenceCollector & self,FProperty * Inproperty) {
	self.SetSerializedProperty(Inproperty);
}

FProperty * _dasUnreal_virtual_105_GetSerializedProperty(const FReferenceCollector & self) {
	return self.GetSerializedProperty();
}

bool _dasUnreal_virtual_106_MarkWeakObjectReferenceForClearing(FReferenceCollector & self,UObject ** WeakReference) {
	return self.MarkWeakObjectReferenceForClearing(WeakReference);
}

void _dasUnreal_virtual_107_SetIsProcessingNativeReferences(FReferenceCollector & self,bool bIsNative) {
	self.SetIsProcessingNativeReferences(bIsNative);
}

bool _dasUnreal_virtual_108_IsProcessingNativeReferences(const FReferenceCollector & self) {
	return self.IsProcessingNativeReferences();
}

bool _dasUnreal_virtual_109_NeedsInitialReferences(const FReferenceCollector & self) {
	return self.NeedsInitialReferences();
}

void _dasUnreal_virtual_110_FindReferences(FReferenceFinder & self,UObject * Object,UObject * ReferencingObject,FProperty * ReferencingProperty) {
	self.FindReferences(Object,ReferencingObject,ReferencingProperty);
}

bool _dasUnreal_virtual_111_IsIgnoringArchetypeRef(const FReferenceFinder & self) {
	return self.IsIgnoringArchetypeRef();
}

bool _dasUnreal_virtual_112_IsIgnoringTransient(const FReferenceFinder & self) {
	return self.IsIgnoringTransient();
}

void _dasUnreal_virtual_113_SetSerializedProperty(FReferenceFinder & self,FProperty * Inproperty) {
	self.SetSerializedProperty(Inproperty);
}

FProperty * _dasUnreal_virtual_114_GetSerializedProperty(const FReferenceFinder & self) {
	return self.GetSerializedProperty();
}

void _dasUnreal_static_115_BroadcastOnObjectModified(const FCoreUObjectDelegates *,UObject * Object) {
	FCoreUObjectDelegates::BroadcastOnObjectModified(Object);
}

FString _dasUnreal_static_116_FormatPathForAssetLog(const FAssetMsg *,const wchar_t * Path) {
	return FAssetMsg::FormatPathForAssetLog(Path);
}

FString _dasUnreal_static_117_FormatPathForAssetLog(const FAssetMsg *,const UObject * Object) {
	return FAssetMsg::FormatPathForAssetLog(Object);
}

FString _dasUnreal_static_118_GetAssetLogString(const FAssetMsg *,const wchar_t * Path,const FString & Message) {
	return FAssetMsg::GetAssetLogString(Path,Message);
}

FString _dasUnreal_static_119_GetAssetLogString(const FAssetMsg *,const UObject * Object,const FString & Message) {
	return FAssetMsg::GetAssetLogString(Object,Message);
}

FName _dasUnreal_virtual_120_GetFNameForStatID(const UObjectBase & self) {
	return self.GetFNameForStatID();
}

FString _dasUnreal_static_121_RemoveClassPrefix(const UObjectBase *,const wchar_t * ClassName) {
	return UObjectBase::RemoveClassPrefix(ClassName);
}

void _dasUnreal_static_122_PrefetchClass(const UObjectBase *,UObject * Object) {
	UObjectBase::PrefetchClass(Object);
}

void _dasUnreal_static_123_PrefetchOuter(const UObjectBase *,UObject * Object) {
	UObjectBase::PrefetchOuter(Object);
}

bool _dasUnreal_static_124_IsPendingKillEnabled(const UObjectBaseUtility *) {
	return UObjectBaseUtility::IsPendingKillEnabled();
}

EInternalObjectFlags _dasUnreal_static_125_FixGarbageOrPendingKillInternalObjectFlags(const UObjectBaseUtility *,const EInternalObjectFlags InFlags) {
	return UObjectBaseUtility::FixGarbageOrPendingKillInternalObjectFlags(InFlags);
}

bool _dasUnreal_virtual_126_CanBeClusterRoot(const UObjectBaseUtility & self) {
	return self.CanBeClusterRoot();
}

bool _dasUnreal_virtual_127_CanBeInCluster(const UObjectBaseUtility & self) {
	return self.CanBeInCluster();
}

void _dasUnreal_virtual_128_CreateCluster(UObjectBaseUtility & self) {
	self.CreateCluster();
}

void _dasUnreal_virtual_129_OnClusterMarkedAsPendingKill(UObjectBaseUtility & self) {
	self.OnClusterMarkedAsPendingKill();
}

UClass * _dasUnreal_static_130_StaticClass(const UObject *) {
	return UObject::StaticClass();
}

const wchar_t * _dasUnreal_static_131_StaticConfigName(const UObject *) {
	return UObject::StaticConfigName();
}

void _dasUnreal_static_132_StaticRegisterNativesUObject(const UObject *) {
	UObject::StaticRegisterNativesUObject();
}

void _dasUnreal_virtual_133_PostInitProperties(UObject & self) {
	self.PostInitProperties();
}

void _dasUnreal_virtual_134_PostReinitProperties(UObject & self) {
	self.PostReinitProperties();
}

void _dasUnreal_virtual_135_PostCDOContruct(UObject & self) {
	self.PostCDOContruct();
}

bool _dasUnreal_virtual_136_Modify(UObject & self,bool bAlwaysMarkDirty) {
	return self.Modify(bAlwaysMarkDirty);
}

bool _dasUnreal_virtual_137_IsCapturingAsRootObjectForTransaction(const UObject & self) {
	return self.IsCapturingAsRootObjectForTransaction();
}

void _dasUnreal_virtual_138_LoadedFromAnotherClass(UObject & self,const FName & OldClassName) {
	self.LoadedFromAnotherClass(OldClassName);
}

bool _dasUnreal_virtual_139_ResolveSubobject(UObject & self,const wchar_t * SubObjectPath,UObject *& OutObject,bool bLoadIfExists) {
	return self.ResolveSubobject(SubObjectPath,OutObject,bLoadIfExists);
}

bool _dasUnreal_virtual_140_IsReadyForAsyncPostLoad(const UObject & self) {
	return self.IsReadyForAsyncPostLoad();
}

void _dasUnreal_virtual_141_PostLoad(UObject & self) {
	self.PostLoad();
}

void _dasUnreal_virtual_142_BeginDestroy(UObject & self) {
	self.BeginDestroy();
}

bool _dasUnreal_virtual_143_IsReadyForFinishDestroy(UObject & self) {
	return self.IsReadyForFinishDestroy();
}

void _dasUnreal_virtual_144_PostLinkerChange(UObject & self) {
	self.PostLinkerChange();
}

void _dasUnreal_virtual_145_FinishDestroy(UObject & self) {
	self.FinishDestroy();
}

void _dasUnreal_virtual_146_Serialize(UObject & self,FArchive & Ar) {
	self.Serialize(Ar);
}

void _dasUnreal_virtual_147_ShutdownAfterError(UObject & self) {
	self.ShutdownAfterError();
}

void _dasUnreal_virtual_148_PreEditUndo(UObject & self) {
	self.PreEditUndo();
}

void _dasUnreal_virtual_149_PostRename(UObject & self,UObject * OldOuter,const FName OldName) {
	self.PostRename(OldOuter,OldName);
}

void _dasUnreal_virtual_150_PostDuplicate(UObject & self,bool bDuplicateForPIE) {
	self.PostDuplicate(bDuplicateForPIE);
}

void _dasUnreal_virtual_151_PostDuplicate(UObject & self,EDuplicateMode::Type DuplicateMode) {
	self.PostDuplicate(DuplicateMode);
}

bool _dasUnreal_virtual_152_NeedsLoadForClient(const UObject & self) {
	return self.NeedsLoadForClient();
}

bool _dasUnreal_virtual_153_NeedsLoadForServer(const UObject & self) {
	return self.NeedsLoadForServer();
}

bool _dasUnreal_virtual_154_NeedsLoadForEditorGame(const UObject & self) {
	return self.NeedsLoadForEditorGame();
}

bool _dasUnreal_virtual_155_IsEditorOnly(const UObject & self) {
	return self.IsEditorOnly();
}

bool _dasUnreal_virtual_156_HasNonEditorOnlyReferences(const UObject & self) {
	return self.HasNonEditorOnlyReferences();
}

bool _dasUnreal_virtual_157_IsPostLoadThreadSafe(const UObject & self) {
	return self.IsPostLoadThreadSafe();
}

bool _dasUnreal_virtual_158_IsDestructionThreadSafe(const UObject & self) {
	return self.IsDestructionThreadSafe();
}

bool _dasUnreal_static_159_CanCreateInCurrentContext(const UObject *,UObject * Template) {
	return UObject::CanCreateInCurrentContext(Template);
}

void _dasUnreal_virtual_160_PostEditImport(UObject & self) {
	self.PostEditImport();
}

void _dasUnreal_virtual_161_PostReloadConfig(UObject & self,FProperty * PropertyThatWasLoaded) {
	self.PostReloadConfig(PropertyThatWasLoaded);
}

bool _dasUnreal_virtual_162_Rename(UObject & self,const wchar_t * NewName,UObject * NewOuter,unsigned int Flags) {
	return self.Rename(NewName,NewOuter,Flags);
}

FString _dasUnreal_virtual_163_GetDesc(UObject & self) {
	return self.GetDesc();
}

void _dasUnreal_virtual_164_MoveDataToSparseClassDataStruct(const UObject & self) {
	self.MoveDataToSparseClassDataStruct();
}

bool _dasUnreal_virtual_165_ConformSparseClassDataStruct(const UObject & self,UScriptStruct * SourceSparseClassDataStruct,void * SourceSparseClassData) {
	return self.ConformSparseClassDataStruct(SourceSparseClassDataStruct,SourceSparseClassData);
}

bool _dasUnreal_virtual_166_ImplementsGetWorld(const UObject & self) {
	return self.ImplementsGetWorld();
}

FName _dasUnreal_virtual_167_GetExporterName(UObject & self) {
	return self.GetExporterName();
}

void _dasUnreal_static_168_AddReferencedObjects(const UObject *,UObject * InThis,FReferenceCollector & Collector) {
	UObject::AddReferencedObjects(InThis,Collector);
}

FRestoreForUObjectOverwrite * _dasUnreal_virtual_169_GetRestoreForUObjectOverwrite(UObject & self) {
	return self.GetRestoreForUObjectOverwrite();
}

bool _dasUnreal_virtual_170_AreNativePropertiesIdenticalTo(const UObject & self,UObject * Other) {
	return self.AreNativePropertiesIdenticalTo(Other);
}

const FName & _dasUnreal_static_171_SourceFileTagName(const UObject *) {
	return UObject::SourceFileTagName();
}

const FName & _dasUnreal_static_172_AssetVersePathTagName(const UObject *) {
	return UObject::AssetVersePathTagName();
}

bool _dasUnreal_virtual_173_IsAsset(const UObject & self) {
	return self.IsAsset();
}

FPrimaryAssetId _dasUnreal_virtual_174_GetPrimaryAssetId(const UObject & self) {
	return self.GetPrimaryAssetId();
}

bool _dasUnreal_virtual_175_IsLocalizedResource(const UObject & self) {
	return self.IsLocalizedResource();
}

bool _dasUnreal_virtual_176_IsSafeForRootSet(const UObject & self) {
	return self.IsSafeForRootSet();
}

void _dasUnreal_virtual_177_TagSubobjects(UObject & self,EObjectFlags NewFlags) {
	self.TagSubobjects(NewFlags);
}

bool _dasUnreal_virtual_178_IsNameStableForNetworking(const UObject & self) {
	return self.IsNameStableForNetworking();
}

bool _dasUnreal_virtual_179_IsFullNameStableForNetworking(const UObject & self) {
	return self.IsFullNameStableForNetworking();
}

bool _dasUnreal_virtual_180_IsSupportedForNetworking(const UObject & self) {
	return self.IsSupportedForNetworking();
}

void _dasUnreal_virtual_181_PreNetReceive(UObject & self) {
	self.PreNetReceive();
}

void _dasUnreal_virtual_182_PostNetReceive(UObject & self) {
	self.PostNetReceive();
}

void _dasUnreal_virtual_183_PostRepNotifies(UObject & self) {
	self.PostRepNotifies();
}

void _dasUnreal_virtual_184_PreDestroyFromReplication(UObject & self) {
	self.PreDestroyFromReplication();
}

void _dasUnreal_virtual_185_WillNeverCacheCookedPlatformDataAgain(UObject & self) {
	self.WillNeverCacheCookedPlatformDataAgain();
}

void _dasUnreal_virtual_186_ClearAllCachedCookedPlatformData(UObject & self) {
	self.ClearAllCachedCookedPlatformData();
}

const wchar_t * _dasUnreal_virtual_187_GetConfigOverridePlatform(const UObject & self) {
	return self.GetConfigOverridePlatform();
}

void _dasUnreal_virtual_188_OverrideConfigSection(UObject & self,FString & SectionName) {
	self.OverrideConfigSection(SectionName);
}

void _dasUnreal_virtual_189_OverridePerObjectConfigSection(UObject & self,FString & SectionName) {
	self.OverridePerObjectConfigSection(SectionName);
}

UObject * _dasUnreal_static_190_GetArchetypeFromRequiredInfo(const UObject *,const UClass * Class,const UObject * Outer,FName Name,EObjectFlags ObjectFlags) {
	return UObject::GetArchetypeFromRequiredInfo(Class,Outer,Name,ObjectFlags);
}

void _dasUnreal_virtual_191_ProcessEvent(UObject & self,UFunction * Function,void * Parms) {
	self.ProcessEvent(Function,Parms);
}

int _dasUnreal_virtual_192_GetFunctionCallspace(UObject & self,UFunction * Function,FFrame * Stack) {
	return self.GetFunctionCallspace(Function,Stack);
}

UClass * _dasUnreal_virtual_193_RegenerateClass(UObject & self,UClass * ClassToRegenerate,UObject * PreviousCDO) {
	return self.RegenerateClass(ClassToRegenerate,PreviousCDO);
}

void _dasUnreal_virtual_194_MarkAsEditorOnlySubobject(UObject & self) {
	self.MarkAsEditorOnlySubobject();
}

uint64 _dasUnreal_virtual_195_GetNetPushIdDynamic(const UObject & self) {
	return self.GetNetPushIdDynamic();
}

FFieldClass * _dasUnreal_static_196_StaticClass(const FField *) {
	return FField::StaticClass();
}

uint64 _dasUnreal_static_197_StaticClassCastFlagsPrivate(const FField *) {
	return FField::StaticClassCastFlagsPrivate();
}

uint64 _dasUnreal_static_198_StaticClassCastFlags(const FField *) {
	return FField::StaticClassCastFlags();
}

void _dasUnreal_virtual_199_Serialize(FField & self,FArchive & Ar) {
	self.Serialize(Ar);
}

void _dasUnreal_virtual_200_PostLoad(FField & self) {
	self.PostLoad();
}

void _dasUnreal_virtual_201_BeginDestroy(FField & self) {
	self.BeginDestroy();
}

void _dasUnreal_virtual_202_AddReferencedObjects(FField & self,FReferenceCollector & Collector) {
	self.AddReferencedObjects(Collector);
}

void _dasUnreal_virtual_203_AddCppProperty(FField & self,FProperty * Property) {
	self.AddCppProperty(Property);
}

void _dasUnreal_virtual_204_Bind(FField & self) {
	self.Bind();
}

FField * _dasUnreal_static_205_Construct(const FField *,const FFieldVariant & InOwner,const FName & InName,EObjectFlags InFlags) {
	return FField::Construct(InOwner,InName,InFlags);
}

FField * _dasUnreal_static_206_Construct(const FField *,const FName & FieldTypeName,const FFieldVariant & InOwner,const FName & InName,EObjectFlags InFlags) {
	return FField::Construct(FieldTypeName,InOwner,InName,InFlags);
}

void _dasUnreal_virtual_207_PostDuplicate(FField & self,const FField & InField) {
	self.PostDuplicate(InField);
}

FField * _dasUnreal_virtual_208_GetInnerFieldByName(FField & self,const FName & InName) {
	return self.GetInnerFieldByName(InName);
}

void _dasUnreal_static_209_CopyMetaData(const FField *,const FField * InSourceField,FField * InDestField) {
	FField::CopyMetaData(InSourceField,InDestField);
}

FField * _dasUnreal_static_210_CreateFromUField(const FField *,UField * InField) {
	return FField::CreateFromUField(InField);
}

FField * _dasUnreal_static_211_Duplicate(const FField *,const FField * InField,FFieldVariant DestOwner,const FName DestName,EObjectFlags FlagMask,EInternalObjectFlags InternalFlagsMask) {
	return FField::Duplicate(InField,DestOwner,DestName,FlagMask,InternalFlagsMask);
}

FName _dasUnreal_static_212_GenerateFFieldName(const FField *,FFieldVariant InOwner,FFieldClass * InClass) {
	return FField::GenerateFFieldName(InOwner,InClass);
}

UClass * _dasUnreal_static_213_StaticClass(const UField *) {
	return UField::StaticClass();
}

const wchar_t * _dasUnreal_static_214_StaticPackage(const UField *) {
	return UField::StaticPackage();
}

void _dasUnreal_static_215_StaticRegisterNativesUField(const UField *) {
	UField::StaticRegisterNativesUField();
}

void _dasUnreal_virtual_216_Serialize(UField & self,FArchive & Ar) {
	self.Serialize(Ar);
}

void _dasUnreal_virtual_217_PostLoad(UField & self) {
	self.PostLoad();
}

bool _dasUnreal_virtual_218_NeedsLoadForClient(const UField & self) {
	return self.NeedsLoadForClient();
}

bool _dasUnreal_virtual_219_NeedsLoadForServer(const UField & self) {
	return self.NeedsLoadForServer();
}

void _dasUnreal_virtual_220_Bind(UField & self) {
	self.Bind();
}

void _dasUnreal_static_221_FormatNativeToolTip(const UField *,FString & ToolTipString,bool bRemoveExtraSections) {
	UField::FormatNativeToolTip(ToolTipString,bRemoveExtraSections);
}

FField * _dasUnreal_virtual_222_GetAssociatedFField(UField & self) {
	return self.GetAssociatedFField();
}

void _dasUnreal_virtual_223_SetAssociatedFField(UField & self,FField * InField) {
	self.SetAssociatedFField(InField);
}

UClass * _dasUnreal_static_224_StaticClass(const UStruct *) {
	return UStruct::StaticClass();
}

const wchar_t * _dasUnreal_static_225_StaticPackage(const UStruct *) {
	return UStruct::StaticPackage();
}

void _dasUnreal_static_226_StaticRegisterNativesUStruct(const UStruct *) {
	UStruct::StaticRegisterNativesUStruct();
}

void _dasUnreal_virtual_227_Serialize(UStruct & self,FArchive & Ar) {
	self.Serialize(Ar);
}

void _dasUnreal_virtual_228_PostLoad(UStruct & self) {
	self.PostLoad();
}

void _dasUnreal_virtual_229_FinishDestroy(UStruct & self) {
	self.FinishDestroy();
}

void _dasUnreal_virtual_230_RegisterDependencies(UStruct & self) {
	self.RegisterDependencies();
}

void _dasUnreal_static_231_AddReferencedObjects(const UStruct *,UObject * InThis,FReferenceCollector & Collector) {
	UStruct::AddReferencedObjects(InThis,Collector);
}

void _dasUnreal_virtual_232_TagSubobjects(UStruct & self,EObjectFlags NewFlags) {
	self.TagSubobjects(NewFlags);
}

void _dasUnreal_virtual_233_AddCppProperty(UStruct & self,FProperty * Property) {
	self.AddCppProperty(Property);
}

UStruct * _dasUnreal_virtual_234_GetInheritanceSuper(const UStruct & self) {
	return self.GetInheritanceSuper();
}

void _dasUnreal_virtual_235_Link(UStruct & self,FArchive & Ar,bool bRelinkExistingProperties) {
	self.Link(Ar,bRelinkExistingProperties);
}

void _dasUnreal_virtual_236_SerializeBin(const UStruct & self,FArchive & Ar,void * Data) {
	self.SerializeBin(Ar,Data);
}

void _dasUnreal_virtual_237_InitializeStruct(const UStruct & self,void * Dest,int ArrayDim) {
	self.InitializeStruct(Dest,ArrayDim);
}

void _dasUnreal_virtual_238_DestroyStruct(const UStruct & self,void * Dest,int ArrayDim) {
	self.DestroyStruct(Dest,ArrayDim);
}

FProperty * _dasUnreal_virtual_239_CustomFindProperty(const UStruct & self,const FName InName) {
	return self.CustomFindProperty(InName);
}

const wchar_t * _dasUnreal_virtual_240_GetPrefixCPP(const UStruct & self) {
	return self.GetPrefixCPP();
}

void _dasUnreal_virtual_241_SetSuperStruct(UStruct & self,UStruct * NewSuperStruct) {
	self.SetSuperStruct(NewSuperStruct);
}

FString _dasUnreal_virtual_242_GetAuthoredNameForField(const UStruct & self,const UField * Field) {
	return self.GetAuthoredNameForField(Field);
}

FString _dasUnreal_virtual_243_GetAuthoredNameForField(const UStruct & self,const FField * Field) {
	return self.GetAuthoredNameForField(Field);
}

bool _dasUnreal_virtual_244_IsStructTrashed(const UStruct & self) {
	return self.IsStructTrashed();
}

UClass * _dasUnreal_static_245_StaticClass(const UScriptStruct *) {
	return UScriptStruct::StaticClass();
}

const wchar_t * _dasUnreal_static_246_StaticPackage(const UScriptStruct *) {
	return UScriptStruct::StaticPackage();
}

void _dasUnreal_static_247_StaticRegisterNativesUScriptStruct(const UScriptStruct *) {
	UScriptStruct::StaticRegisterNativesUScriptStruct();
}

void _dasUnreal_virtual_248_Serialize(UScriptStruct & self,FArchive & Ar) {
	self.Serialize(Ar);
}

void _dasUnreal_virtual_249_Link(UScriptStruct & self,FArchive & Ar,bool bRelinkExistingProperties) {
	self.Link(Ar,bRelinkExistingProperties);
}

void _dasUnreal_virtual_250_InitializeStruct(const UScriptStruct & self,void * Dest,int ArrayDim) {
	self.InitializeStruct(Dest,ArrayDim);
}

void _dasUnreal_virtual_251_DestroyStruct(const UScriptStruct & self,void * Dest,int ArrayDim) {
	self.DestroyStruct(Dest,ArrayDim);
}

bool _dasUnreal_virtual_252_IsStructTrashed(const UScriptStruct & self) {
	return self.IsStructTrashed();
}

void _dasUnreal_virtual_253_PrepareCppStructOps(UScriptStruct & self) {
	self.PrepareCppStructOps();
}

unsigned int _dasUnreal_virtual_254_GetStructTypeHash(const UScriptStruct & self,const void * Src) {
	return self.GetStructTypeHash(Src);
}

void _dasUnreal_virtual_255_RecursivelyPreload(UScriptStruct & self) {
	self.RecursivelyPreload();
}

FString _dasUnreal_virtual_256_GetStructCPPName(const UScriptStruct & self,unsigned int CPPExportFlags) {
	return self.GetStructCPPName(CPPExportFlags);
}

void _dasUnreal_virtual_257_InitializeDefaultValue(const UScriptStruct & self,unsigned char * InStructData) {
	self.InitializeDefaultValue(InStructData);
}

UClass * _dasUnreal_static_258_StaticClass(const UFunction *) {
	return UFunction::StaticClass();
}

const wchar_t * _dasUnreal_static_259_StaticPackage(const UFunction *) {
	return UFunction::StaticPackage();
}

void _dasUnreal_static_260_StaticRegisterNativesUFunction(const UFunction *) {
	UFunction::StaticRegisterNativesUFunction();
}

void _dasUnreal_virtual_261_Serialize(UFunction & self,FArchive & Ar) {
	self.Serialize(Ar);
}

void _dasUnreal_virtual_262_PostLoad(UFunction & self) {
	self.PostLoad();
}

void _dasUnreal_virtual_263_Bind(UFunction & self) {
	self.Bind();
}

UStruct * _dasUnreal_virtual_264_GetInheritanceSuper(const UFunction & self) {
	return self.GetInheritanceSuper();
}

void _dasUnreal_virtual_265_Link(UFunction & self,FArchive & Ar,bool bRelinkExistingProperties) {
	self.Link(Ar,bRelinkExistingProperties);
}

uint64 _dasUnreal_static_266_GetDefaultIgnoredSignatureCompatibilityFlags(const UFunction *) {
	return UFunction::GetDefaultIgnoredSignatureCompatibilityFlags();
}

UClass * _dasUnreal_static_267_StaticClass(const UEnum *) {
	return UEnum::StaticClass();
}

const wchar_t * _dasUnreal_static_268_StaticPackage(const UEnum *) {
	return UEnum::StaticPackage();
}

void _dasUnreal_static_269_StaticRegisterNativesUEnum(const UEnum *) {
	UEnum::StaticRegisterNativesUEnum();
}

FText _dasUnreal_virtual_270_GetDisplayNameTextByIndex(const UEnum & self,int InIndex) {
	return self.GetDisplayNameTextByIndex(InIndex);
}

FString _dasUnreal_virtual_271_GetAuthoredNameStringByIndex(const UEnum & self,int InIndex) {
	return self.GetAuthoredNameStringByIndex(InIndex);
}

bool _dasUnreal_static_272_IsFullEnumName(const UEnum *,const wchar_t * InEnumName) {
	return UEnum::IsFullEnumName(InEnumName);
}

FString _dasUnreal_virtual_273_GenerateFullEnumName(const UEnum & self,const wchar_t * InEnumName) {
	return self.GenerateFullEnumName(InEnumName);
}

int64 _dasUnreal_static_274_ParseEnum(const UEnum *,const wchar_t *& Str) {
	return UEnum::ParseEnum(Str);
}

void _dasUnreal_virtual_275_Serialize(UEnum & self,FArchive & Ar) {
	self.Serialize(Ar);
}

void _dasUnreal_virtual_276_BeginDestroy(UEnum & self) {
	self.BeginDestroy();
}

bool _dasUnreal_virtual_277_IsAbstract(const FCppClassTypeInfo & self) {
	return self.IsAbstract();
}

UClass * _dasUnreal_static_278_StaticClass(const UClass *) {
	return UClass::StaticClass();
}

const wchar_t * _dasUnreal_static_279_StaticPackage(const UClass *) {
	return UClass::StaticPackage();
}

void _dasUnreal_static_280_StaticRegisterNativesUClass(const UClass *) {
	UClass::StaticRegisterNativesUClass();
}

void _dasUnreal_virtual_281_ConditionalRecompileClass(UClass & self,FUObjectSerializeContext * InLoadContext) {
	self.ConditionalRecompileClass(InLoadContext);
}

void _dasUnreal_virtual_282_FlushCompilationQueueForLevel(UClass & self) {
	self.FlushCompilationQueueForLevel();
}

void _dasUnreal_static_283_AssembleReferenceTokenStreams(const UClass *) {
	UClass::AssembleReferenceTokenStreams();
}

UClass * _dasUnreal_virtual_284_GetAuthoritativeClass(UClass & self) {
	return self.GetAuthoritativeClass();
}

void _dasUnreal_virtual_285_Serialize(UClass & self,FArchive & Ar) {
	self.Serialize(Ar);
}

void _dasUnreal_virtual_286_PostLoad(UClass & self) {
	self.PostLoad();
}

void _dasUnreal_virtual_287_FinishDestroy(UClass & self) {
	self.FinishDestroy();
}

void _dasUnreal_virtual_288_DeferredRegister(UClass & self,UClass * UClassStaticClass,const wchar_t * PackageName,const wchar_t * InName) {
	self.DeferredRegister(UClassStaticClass,PackageName,InName);
}

bool _dasUnreal_virtual_289_Rename(UClass & self,const wchar_t * NewName,UObject * NewOuter,unsigned int Flags) {
	return self.Rename(NewName,NewOuter,Flags);
}

void _dasUnreal_virtual_290_TagSubobjects(UClass & self,EObjectFlags NewFlags) {
	self.TagSubobjects(NewFlags);
}

void _dasUnreal_virtual_291_PostInitProperties(UClass & self) {
	self.PostInitProperties();
}

void _dasUnreal_static_292_AddReferencedObjects(const UClass *,UObject * InThis,FReferenceCollector & Collector) {
	UClass::AddReferencedObjects(InThis,Collector);
}

FRestoreForUObjectOverwrite * _dasUnreal_virtual_293_GetRestoreForUObjectOverwrite(UClass & self) {
	return self.GetRestoreForUObjectOverwrite();
}

FString _dasUnreal_virtual_294_GetDesc(UClass & self) {
	return self.GetDesc();
}

bool _dasUnreal_virtual_295_IsAsset(const UClass & self) {
	return self.IsAsset();
}

bool _dasUnreal_virtual_296_IsNameStableForNetworking(const UClass & self) {
	return self.IsNameStableForNetworking();
}

void _dasUnreal_virtual_297_Bind(UClass & self) {
	self.Bind();
}

const wchar_t * _dasUnreal_virtual_298_GetPrefixCPP(const UClass & self) {
	return self.GetPrefixCPP();
}

void _dasUnreal_virtual_299_Link(UClass & self,FArchive & Ar,bool bRelinkExistingProperties) {
	self.Link(Ar,bRelinkExistingProperties);
}

void _dasUnreal_virtual_300_SetSuperStruct(UClass & self,UStruct * NewSuperStruct) {
	self.SetSuperStruct(NewSuperStruct);
}

bool _dasUnreal_virtual_301_IsStructTrashed(const UClass & self) {
	return self.IsStructTrashed();
}

void _dasUnreal_virtual_302_PostLoadInstance(UClass & self,UObject * InObj) {
	self.PostLoadInstance(InObj);
}

void _dasUnreal_virtual_303_InitPropertiesFromCustomList(UClass & self,unsigned char * DataPtr,const unsigned char * DefaultDataPtr) {
	self.InitPropertiesFromCustomList(DataPtr,DefaultDataPtr);
}

bool _dasUnreal_virtual_304_CanCreateAssetOfClass(const UClass & self) {
	return self.CanCreateAssetOfClass();
}

unsigned char * _dasUnreal_virtual_305_GetPersistentUberGraphFrame(const UClass & self,UObject * Obj,UFunction * FuncToCheck) {
	return self.GetPersistentUberGraphFrame(Obj,FuncToCheck);
}

void _dasUnreal_virtual_306_CreatePersistentUberGraphFrame(const UClass & self,UObject * Obj,bool bCreateOnlyIfEmpty,bool bSkipSuperClass,UClass * OldClass) {
	self.CreatePersistentUberGraphFrame(Obj,bCreateOnlyIfEmpty,bSkipSuperClass,OldClass);
}

void _dasUnreal_virtual_307_DestroyPersistentUberGraphFrame(const UClass & self,UObject * Obj,bool bSkipSuperClass) {
	self.DestroyPersistentUberGraphFrame(Obj,bSkipSuperClass);
}

void _dasUnreal_virtual_308_PostLoadDefaultObject(UClass & self,UObject * Object) {
	self.PostLoadDefaultObject(Object);
}

void _dasUnreal_virtual_309_PurgeClass(UClass & self,bool bRecompilingOnLoad) {
	self.PurgeClass(bRecompilingOnLoad);
}

UClass * _dasUnreal_static_310_FindCommonBase(const UClass *,UClass * InClassA,UClass * InClassB) {
	return UClass::FindCommonBase(InClassA,InClassB);
}

bool _dasUnreal_virtual_311_IsFunctionImplementedInScript(const UClass & self,FName InFunctionName) {
	return self.IsFunctionImplementedInScript(InFunctionName);
}

bool _dasUnreal_virtual_312_HasProperty(const UClass & self,FProperty * InProperty) {
	return self.HasProperty(InProperty);
}

UObject * _dasUnreal_virtual_313_FindArchetype(const UClass & self,const UClass * ArchetypeClass,const FName ArchetypeName) {
	return self.FindArchetype(ArchetypeClass,ArchetypeName);
}

UObject * _dasUnreal_virtual_314_GetArchetypeForCDO(const UClass & self) {
	return self.GetArchetypeForCDO();
}

bool _dasUnreal_static_315_IsSafeToSerializeToStructuredArchives(const UClass *,UClass * InClass) {
	return UClass::IsSafeToSerializeToStructuredArchives(InClass);
}

UClass * _dasUnreal_static_316_StaticClass(const UPackage *) {
	return UPackage::StaticClass();
}

const wchar_t * _dasUnreal_static_317_StaticPackage(const UPackage *) {
	return UPackage::StaticPackage();
}

void _dasUnreal_static_318_StaticRegisterNativesUPackage(const UPackage *) {
	UPackage::StaticRegisterNativesUPackage();
}

bool _dasUnreal_virtual_319_IsNameStableForNetworking(const UPackage & self) {
	return self.IsNameStableForNetworking();
}

bool _dasUnreal_virtual_320_NeedsLoadForClient(const UPackage & self) {
	return self.NeedsLoadForClient();
}

bool _dasUnreal_virtual_321_NeedsLoadForServer(const UPackage & self) {
	return self.NeedsLoadForServer();
}

bool _dasUnreal_virtual_322_IsPostLoadThreadSafe(const UPackage & self) {
	return self.IsPostLoadThreadSafe();
}

bool _dasUnreal_virtual_323_IsDestructionThreadSafe(const UPackage & self) {
	return self.IsDestructionThreadSafe();
}

void _dasUnreal_virtual_324_PostInitProperties(UPackage & self) {
	self.PostInitProperties();
}

void _dasUnreal_virtual_325_FinishDestroy(UPackage & self) {
	self.FinishDestroy();
}

void _dasUnreal_virtual_326_Serialize(UPackage & self,FArchive & Ar) {
	self.Serialize(Ar);
}

bool _dasUnreal_virtual_327_IsAsset(const UPackage & self) {
	return self.IsAsset();
}

void _dasUnreal_static_328_AddReferencedObjects(const UPackage *,UObject * InThis,FReferenceCollector & Collector) {
	UPackage::AddReferencedObjects(InThis,Collector);
}

void _dasUnreal_virtual_329_TagSubobjects(UPackage & self,EObjectFlags NewFlags) {
	self.TagSubobjects(NewFlags);
}

void _dasUnreal_static_330_WaitForAsyncFileWrites(const UPackage *) {
	UPackage::WaitForAsyncFileWrites();
}

bool _dasUnreal_static_331_HasAsyncFileWrites(const UPackage *) {
	return UPackage::HasAsyncFileWrites();
}

bool _dasUnreal_static_332_IsEmptyPackage(const UPackage *,UPackage * Package,const UObject * LastReferencer) {
	return UPackage::IsEmptyPackage(Package,LastReferencer);
}

FFieldClass * _dasUnreal_static_333_StaticClass(const FProperty *) {
	return FProperty::StaticClass();
}

uint64 _dasUnreal_static_334_StaticClassCastFlagsPrivate(const FProperty *) {
	return FProperty::StaticClassCastFlagsPrivate();
}

uint64 _dasUnreal_static_335_StaticClassCastFlags(const FProperty *) {
	return FProperty::StaticClassCastFlags();
}

void _dasUnreal_virtual_336_Serialize(FProperty & self,FArchive & Ar) {
	self.Serialize(Ar);
}

void _dasUnreal_virtual_337_PostDuplicate(FProperty & self,const FField & InField) {
	self.PostDuplicate(InField);
}

FName _dasUnreal_static_338_FindRedirectedPropertyName(const FProperty *,UStruct * ObjectStruct,FName OldName) {
	return FProperty::FindRedirectedPropertyName(ObjectStruct,OldName);
}

FString _dasUnreal_virtual_339_GetCPPMacroType(const FProperty & self,FString & ExtendedTypeText) {
	return self.GetCPPMacroType(ExtendedTypeText);
}

bool _dasUnreal_virtual_340_PassCPPArgsByRef(const FProperty & self) {
	return self.PassCPPArgsByRef();
}

FString _dasUnreal_virtual_341_GetCPPType(const FProperty & self,FString * ExtendedTypeText,unsigned int CPPExportFlags) {
	return self.GetCPPType(ExtendedTypeText,CPPExportFlags);
}

FString _dasUnreal_virtual_342_GetCPPTypeForwardDeclaration(const FProperty & self) {
	return self.GetCPPTypeForwardDeclaration();
}

bool _dasUnreal_virtual_343_HasSetter(const FProperty & self) {
	return self.HasSetter();
}

bool _dasUnreal_virtual_344_HasGetter(const FProperty & self) {
	return self.HasGetter();
}

bool _dasUnreal_virtual_345_HasSetterOrGetter(const FProperty & self) {
	return self.HasSetterOrGetter();
}

void _dasUnreal_virtual_346_CallSetter(const FProperty & self,void * Container,const void * InValue) {
	self.CallSetter(Container,InValue);
}

void _dasUnreal_virtual_347_CallGetter(const FProperty & self,const void * Container,void * OutValue) {
	self.CallGetter(Container,OutValue);
}

bool _dasUnreal_virtual_348_Identical(const FProperty & self,const void * A,const void * B,unsigned int PortFlags) {
	return self.Identical(A,B,PortFlags);
}

bool _dasUnreal_virtual_349_SupportsNetSharedSerialization(const FProperty & self) {
	return self.SupportsNetSharedSerialization();
}

void * _dasUnreal_virtual_350_GetValueAddressAtIndex_Direct(const FProperty & self,const FProperty * Inner,void * InValueAddress,int Index) {
	return self.GetValueAddressAtIndex_Direct(Inner,InValueAddress,Index);
}

void _dasUnreal_virtual_351_AppendSchemaHash(const FProperty & self,FBlake3 & Builder,bool bSkipEditorOnly) {
	self.AppendSchemaHash(Builder,bSkipEditorOnly);
}

void _dasUnreal_virtual_352_CopySingleValueToScriptVM(const FProperty & self,void * Dest,const void * Src) {
	self.CopySingleValueToScriptVM(Dest,Src);
}

void _dasUnreal_virtual_353_CopyCompleteValueToScriptVM(const FProperty & self,void * Dest,const void * Src) {
	self.CopyCompleteValueToScriptVM(Dest,Src);
}

void _dasUnreal_virtual_354_CopyCompleteValueToScriptVM_InContainer(const FProperty & self,void * OutValue,const void * InContainer) {
	self.CopyCompleteValueToScriptVM_InContainer(OutValue,InContainer);
}

void _dasUnreal_virtual_355_CopyCompleteValueFromScriptVM_InContainer(const FProperty & self,void * OutContainer,const void * InValue) {
	self.CopyCompleteValueFromScriptVM_InContainer(OutContainer,InValue);
}

void _dasUnreal_virtual_356_CopySingleValueFromScriptVM(const FProperty & self,void * Dest,const void * Src) {
	self.CopySingleValueFromScriptVM(Dest,Src);
}

void _dasUnreal_virtual_357_CopyCompleteValueFromScriptVM(const FProperty & self,void * Dest,const void * Src) {
	self.CopyCompleteValueFromScriptVM(Dest,Src);
}

FName _dasUnreal_virtual_358_GetID(const FProperty & self) {
	return self.GetID();
}

int _dasUnreal_virtual_359_GetMinAlignment(const FProperty & self) {
	return self.GetMinAlignment();
}

bool _dasUnreal_virtual_360_SameType(const FProperty & self,const FProperty * Other) {
	return self.SameType(Other);
}

FFieldClass * _dasUnreal_static_361_StaticClass(const FNumericProperty *) {
	return FNumericProperty::StaticClass();
}

FField * _dasUnreal_static_362_Construct(const FNumericProperty *,const FFieldVariant & InOwner,const FName & InName,EObjectFlags InObjectFlags) {
	return FNumericProperty::Construct(InOwner,InName,InObjectFlags);
}

uint64 _dasUnreal_static_363_StaticClassCastFlagsPrivate(const FNumericProperty *) {
	return FNumericProperty::StaticClassCastFlagsPrivate();
}

uint64 _dasUnreal_static_364_StaticClassCastFlags(const FNumericProperty *) {
	return FNumericProperty::StaticClassCastFlags();
}

const wchar_t * _dasUnreal_virtual_365_ImportText_Internal(const FNumericProperty & self,const wchar_t * Buffer,void * ContainerOrPropertyPtr,EPropertyPointerType PropertyPointerType,UObject * Parent,int PortFlags,FOutputDevice * ErrorText) {
	return self.ImportText_Internal(Buffer,ContainerOrPropertyPtr,PropertyPointerType,Parent,PortFlags,ErrorText);
}

void _dasUnreal_virtual_366_ExportText_Internal(const FNumericProperty & self,FString & ValueStr,const void * PropertyValueOrContainer,EPropertyPointerType PropertyPointerType,const void * DefaultValue,UObject * Parent,int PortFlags,UObject * ExportRootScope) {
	self.ExportText_Internal(ValueStr,PropertyValueOrContainer,PropertyPointerType,DefaultValue,Parent,PortFlags,ExportRootScope);
}

bool _dasUnreal_virtual_367_IsFloatingPoint(const FNumericProperty & self) {
	return self.IsFloatingPoint();
}

bool _dasUnreal_virtual_368_IsInteger(const FNumericProperty & self) {
	return self.IsInteger();
}

UEnum * _dasUnreal_virtual_369_GetIntPropertyEnum(const FNumericProperty & self) {
	return self.GetIntPropertyEnum();
}

void _dasUnreal_virtual_370_SetIntPropertyValue(const FNumericProperty & self,void * Data,uint64 Value) {
	self.SetIntPropertyValue(Data,Value);
}

void _dasUnreal_virtual_371_SetIntPropertyValue(const FNumericProperty & self,void * Data,int64 Value) {
	self.SetIntPropertyValue(Data,Value);
}

void _dasUnreal_virtual_372_SetFloatingPointPropertyValue(const FNumericProperty & self,void * Data,double Value) {
	self.SetFloatingPointPropertyValue(Data,Value);
}

void _dasUnreal_virtual_373_SetNumericPropertyValueFromString(const FNumericProperty & self,void * Data,const wchar_t * Value) {
	self.SetNumericPropertyValueFromString(Data,Value);
}

void _dasUnreal_virtual_374_SetNumericPropertyValueFromString_InContainer(const FNumericProperty & self,void * Container,const wchar_t * Value) {
	self.SetNumericPropertyValueFromString_InContainer(Container,Value);
}

int64 _dasUnreal_virtual_375_GetSignedIntPropertyValue(const FNumericProperty & self,const void * Data) {
	return self.GetSignedIntPropertyValue(Data);
}

int64 _dasUnreal_virtual_376_GetSignedIntPropertyValue_InContainer(const FNumericProperty & self,const void * Container) {
	return self.GetSignedIntPropertyValue_InContainer(Container);
}

uint64 _dasUnreal_virtual_377_GetUnsignedIntPropertyValue(const FNumericProperty & self,const void * Data) {
	return self.GetUnsignedIntPropertyValue(Data);
}

uint64 _dasUnreal_virtual_378_GetUnsignedIntPropertyValue_InContainer(const FNumericProperty & self,const void * Container) {
	return self.GetUnsignedIntPropertyValue_InContainer(Container);
}

double _dasUnreal_virtual_379_GetFloatingPointPropertyValue(const FNumericProperty & self,const void * Data) {
	return self.GetFloatingPointPropertyValue(Data);
}

FString _dasUnreal_virtual_380_GetNumericPropertyValueToString(const FNumericProperty & self,const void * Data) {
	return self.GetNumericPropertyValueToString(Data);
}

FString _dasUnreal_virtual_381_GetNumericPropertyValueToString_InContainer(const FNumericProperty & self,const void * Container) {
	return self.GetNumericPropertyValueToString_InContainer(Container);
}

FFieldClass * _dasUnreal_static_382_StaticClass(const FByteProperty *) {
	return FByteProperty::StaticClass();
}

FField * _dasUnreal_static_383_Construct(const FByteProperty *,const FFieldVariant & InOwner,const FName & InName,EObjectFlags InObjectFlags) {
	return FByteProperty::Construct(InOwner,InName,InObjectFlags);
}

uint64 _dasUnreal_static_384_StaticClassCastFlagsPrivate(const FByteProperty *) {
	return FByteProperty::StaticClassCastFlagsPrivate();
}

uint64 _dasUnreal_static_385_StaticClassCastFlags(const FByteProperty *) {
	return FByteProperty::StaticClassCastFlags();
}

void _dasUnreal_virtual_386_Serialize(FByteProperty & self,FArchive & Ar) {
	self.Serialize(Ar);
}

void _dasUnreal_virtual_387_AddReferencedObjects(FByteProperty & self,FReferenceCollector & Collector) {
	self.AddReferencedObjects(Collector);
}

void _dasUnreal_virtual_388_PostDuplicate(FByteProperty & self,const FField & InField) {
	self.PostDuplicate(InField);
}

FString _dasUnreal_virtual_389_GetCPPType(const FByteProperty & self,FString * ExtendedTypeText,unsigned int CPPExportFlags) {
	return self.GetCPPType(ExtendedTypeText,CPPExportFlags);
}

void _dasUnreal_virtual_390_AppendSchemaHash(const FByteProperty & self,FBlake3 & Builder,bool bSkipEditorOnly) {
	self.AppendSchemaHash(Builder,bSkipEditorOnly);
}

UEnum * _dasUnreal_virtual_391_GetIntPropertyEnum(const FByteProperty & self) {
	return self.GetIntPropertyEnum();
}

FFieldClass * _dasUnreal_static_392_StaticClass(const FInt8Property *) {
	return FInt8Property::StaticClass();
}

FField * _dasUnreal_static_393_Construct(const FInt8Property *,const FFieldVariant & InOwner,const FName & InName,EObjectFlags InObjectFlags) {
	return FInt8Property::Construct(InOwner,InName,InObjectFlags);
}

uint64 _dasUnreal_static_394_StaticClassCastFlagsPrivate(const FInt8Property *) {
	return FInt8Property::StaticClassCastFlagsPrivate();
}

uint64 _dasUnreal_static_395_StaticClassCastFlags(const FInt8Property *) {
	return FInt8Property::StaticClassCastFlags();
}

FFieldClass * _dasUnreal_static_396_StaticClass(const FInt16Property *) {
	return FInt16Property::StaticClass();
}

FField * _dasUnreal_static_397_Construct(const FInt16Property *,const FFieldVariant & InOwner,const FName & InName,EObjectFlags InObjectFlags) {
	return FInt16Property::Construct(InOwner,InName,InObjectFlags);
}

uint64 _dasUnreal_static_398_StaticClassCastFlagsPrivate(const FInt16Property *) {
	return FInt16Property::StaticClassCastFlagsPrivate();
}

uint64 _dasUnreal_static_399_StaticClassCastFlags(const FInt16Property *) {
	return FInt16Property::StaticClassCastFlags();
}

FFieldClass * _dasUnreal_static_400_StaticClass(const FIntProperty *) {
	return FIntProperty::StaticClass();
}

FField * _dasUnreal_static_401_Construct(const FIntProperty *,const FFieldVariant & InOwner,const FName & InName,EObjectFlags InObjectFlags) {
	return FIntProperty::Construct(InOwner,InName,InObjectFlags);
}

uint64 _dasUnreal_static_402_StaticClassCastFlagsPrivate(const FIntProperty *) {
	return FIntProperty::StaticClassCastFlagsPrivate();
}

uint64 _dasUnreal_static_403_StaticClassCastFlags(const FIntProperty *) {
	return FIntProperty::StaticClassCastFlags();
}

FFieldClass * _dasUnreal_static_404_StaticClass(const FInt64Property *) {
	return FInt64Property::StaticClass();
}

FField * _dasUnreal_static_405_Construct(const FInt64Property *,const FFieldVariant & InOwner,const FName & InName,EObjectFlags InObjectFlags) {
	return FInt64Property::Construct(InOwner,InName,InObjectFlags);
}

uint64 _dasUnreal_static_406_StaticClassCastFlagsPrivate(const FInt64Property *) {
	return FInt64Property::StaticClassCastFlagsPrivate();
}

uint64 _dasUnreal_static_407_StaticClassCastFlags(const FInt64Property *) {
	return FInt64Property::StaticClassCastFlags();
}

FFieldClass * _dasUnreal_static_408_StaticClass(const FUInt16Property *) {
	return FUInt16Property::StaticClass();
}

FField * _dasUnreal_static_409_Construct(const FUInt16Property *,const FFieldVariant & InOwner,const FName & InName,EObjectFlags InObjectFlags) {
	return FUInt16Property::Construct(InOwner,InName,InObjectFlags);
}

uint64 _dasUnreal_static_410_StaticClassCastFlagsPrivate(const FUInt16Property *) {
	return FUInt16Property::StaticClassCastFlagsPrivate();
}

uint64 _dasUnreal_static_411_StaticClassCastFlags(const FUInt16Property *) {
	return FUInt16Property::StaticClassCastFlags();
}

FFieldClass * _dasUnreal_static_412_StaticClass(const FUInt32Property *) {
	return FUInt32Property::StaticClass();
}

FField * _dasUnreal_static_413_Construct(const FUInt32Property *,const FFieldVariant & InOwner,const FName & InName,EObjectFlags InObjectFlags) {
	return FUInt32Property::Construct(InOwner,InName,InObjectFlags);
}

uint64 _dasUnreal_static_414_StaticClassCastFlagsPrivate(const FUInt32Property *) {
	return FUInt32Property::StaticClassCastFlagsPrivate();
}

uint64 _dasUnreal_static_415_StaticClassCastFlags(const FUInt32Property *) {
	return FUInt32Property::StaticClassCastFlags();
}

FFieldClass * _dasUnreal_static_416_StaticClass(const FUInt64Property *) {
	return FUInt64Property::StaticClass();
}

FField * _dasUnreal_static_417_Construct(const FUInt64Property *,const FFieldVariant & InOwner,const FName & InName,EObjectFlags InObjectFlags) {
	return FUInt64Property::Construct(InOwner,InName,InObjectFlags);
}

uint64 _dasUnreal_static_418_StaticClassCastFlagsPrivate(const FUInt64Property *) {
	return FUInt64Property::StaticClassCastFlagsPrivate();
}

uint64 _dasUnreal_static_419_StaticClassCastFlags(const FUInt64Property *) {
	return FUInt64Property::StaticClassCastFlags();
}

FFieldClass * _dasUnreal_static_420_StaticClass(const FFloatProperty *) {
	return FFloatProperty::StaticClass();
}

FField * _dasUnreal_static_421_Construct(const FFloatProperty *,const FFieldVariant & InOwner,const FName & InName,EObjectFlags InObjectFlags) {
	return FFloatProperty::Construct(InOwner,InName,InObjectFlags);
}

uint64 _dasUnreal_static_422_StaticClassCastFlagsPrivate(const FFloatProperty *) {
	return FFloatProperty::StaticClassCastFlagsPrivate();
}

uint64 _dasUnreal_static_423_StaticClassCastFlags(const FFloatProperty *) {
	return FFloatProperty::StaticClassCastFlags();
}

FFieldClass * _dasUnreal_static_424_StaticClass(const FDoubleProperty *) {
	return FDoubleProperty::StaticClass();
}

FField * _dasUnreal_static_425_Construct(const FDoubleProperty *,const FFieldVariant & InOwner,const FName & InName,EObjectFlags InObjectFlags) {
	return FDoubleProperty::Construct(InOwner,InName,InObjectFlags);
}

uint64 _dasUnreal_static_426_StaticClassCastFlagsPrivate(const FDoubleProperty *) {
	return FDoubleProperty::StaticClassCastFlagsPrivate();
}

uint64 _dasUnreal_static_427_StaticClassCastFlags(const FDoubleProperty *) {
	return FDoubleProperty::StaticClassCastFlags();
}

FFieldClass * _dasUnreal_static_428_StaticClass(const FBoolProperty *) {
	return FBoolProperty::StaticClass();
}

FField * _dasUnreal_static_429_Construct(const FBoolProperty *,const FFieldVariant & InOwner,const FName & InName,EObjectFlags InObjectFlags) {
	return FBoolProperty::Construct(InOwner,InName,InObjectFlags);
}

uint64 _dasUnreal_static_430_StaticClassCastFlagsPrivate(const FBoolProperty *) {
	return FBoolProperty::StaticClassCastFlagsPrivate();
}

uint64 _dasUnreal_static_431_StaticClassCastFlags(const FBoolProperty *) {
	return FBoolProperty::StaticClassCastFlags();
}

void _dasUnreal_virtual_432_Serialize(FBoolProperty & self,FArchive & Ar) {
	self.Serialize(Ar);
}

void _dasUnreal_virtual_433_PostDuplicate(FBoolProperty & self,const FField & InField) {
	self.PostDuplicate(InField);
}

FString _dasUnreal_virtual_434_GetCPPType(const FBoolProperty & self,FString * ExtendedTypeText,unsigned int CPPExportFlags) {
	return self.GetCPPType(ExtendedTypeText,CPPExportFlags);
}

FString _dasUnreal_virtual_435_GetCPPMacroType(const FBoolProperty & self,FString & ExtendedTypeText) {
	return self.GetCPPMacroType(ExtendedTypeText);
}

FString _dasUnreal_virtual_436_GetCPPTypeForwardDeclaration(const FBoolProperty & self) {
	return self.GetCPPTypeForwardDeclaration();
}

void _dasUnreal_virtual_437_LinkInternal(FBoolProperty & self,FArchive & Ar) {
	self.LinkInternal(Ar);
}

bool _dasUnreal_virtual_438_Identical(const FBoolProperty & self,const void * A,const void * B,unsigned int PortFlags) {
	return self.Identical(A,B,PortFlags);
}

void _dasUnreal_virtual_439_CopyValuesInternal(const FBoolProperty & self,void * Dest,const void * Src,int Count) {
	self.CopyValuesInternal(Dest,Src,Count);
}

void _dasUnreal_virtual_440_ClearValueInternal(const FBoolProperty & self,void * Data) {
	self.ClearValueInternal(Data);
}

void _dasUnreal_virtual_441_InitializeValueInternal(const FBoolProperty & self,void * Dest) {
	self.InitializeValueInternal(Dest);
}

int _dasUnreal_virtual_442_GetMinAlignment(const FBoolProperty & self) {
	return self.GetMinAlignment();
}

void _dasUnreal_virtual_443_AppendSchemaHash(const FBoolProperty & self,FBlake3 & Builder,bool bSkipEditorOnly) {
	self.AppendSchemaHash(Builder,bSkipEditorOnly);
}

bool _dasUnreal_static_444_GetDefaultPropertyValue(const FBoolProperty *) {
	return FBoolProperty::GetDefaultPropertyValue();
}

unsigned int _dasUnreal_virtual_445_GetValueTypeHashInternal(const FBoolProperty & self,const void * Src) {
	return self.GetValueTypeHashInternal(Src);
}

FFieldClass * _dasUnreal_static_446_StaticClass(const FObjectPropertyBase *) {
	return FObjectPropertyBase::StaticClass();
}

FField * _dasUnreal_static_447_Construct(const FObjectPropertyBase *,const FFieldVariant & InOwner,const FName & InName,EObjectFlags InObjectFlags) {
	return FObjectPropertyBase::Construct(InOwner,InName,InObjectFlags);
}

uint64 _dasUnreal_static_448_StaticClassCastFlagsPrivate(const FObjectPropertyBase *) {
	return FObjectPropertyBase::StaticClassCastFlagsPrivate();
}

uint64 _dasUnreal_static_449_StaticClassCastFlags(const FObjectPropertyBase *) {
	return FObjectPropertyBase::StaticClassCastFlags();
}

void _dasUnreal_virtual_450_Serialize(FObjectPropertyBase & self,FArchive & Ar) {
	self.Serialize(Ar);
}

void _dasUnreal_virtual_451_AddReferencedObjects(FObjectPropertyBase & self,FReferenceCollector & Collector) {
	self.AddReferencedObjects(Collector);
}

void _dasUnreal_virtual_452_BeginDestroy(FObjectPropertyBase & self) {
	self.BeginDestroy();
}

void _dasUnreal_virtual_453_PostDuplicate(FObjectPropertyBase & self,const FField & InField) {
	self.PostDuplicate(InField);
}

bool _dasUnreal_virtual_454_Identical(const FObjectPropertyBase & self,const void * A,const void * B,unsigned int PortFlags) {
	return self.Identical(A,B,PortFlags);
}

bool _dasUnreal_virtual_455_SupportsNetSharedSerialization(const FObjectPropertyBase & self) {
	return self.SupportsNetSharedSerialization();
}

FName _dasUnreal_virtual_456_GetID(const FObjectPropertyBase & self) {
	return self.GetID();
}

bool _dasUnreal_virtual_457_SameType(const FObjectPropertyBase & self,const FProperty * Other) {
	return self.SameType(Other);
}

FString _dasUnreal_virtual_458_GetCPPTypeCustom(const FObjectPropertyBase & self,FString * ExtendedTypeText,unsigned int CPPExportFlags,const FString & InnerNativeTypeName) {
	return self.GetCPPTypeCustom(ExtendedTypeText,CPPExportFlags,InnerNativeTypeName);
}

bool _dasUnreal_static_459_StaticIdentical(const FObjectPropertyBase *,UObject * A,UObject * B,unsigned int PortFlags) {
	return FObjectPropertyBase::StaticIdentical(A,B,PortFlags);
}

UObject * _dasUnreal_virtual_460_LoadObjectPropertyValue(const FObjectPropertyBase & self,const void * PropertyValueAddress) {
	return self.LoadObjectPropertyValue(PropertyValueAddress);
}

UObject * _dasUnreal_virtual_461_GetObjectPropertyValue(const FObjectPropertyBase & self,const void * PropertyValueAddress) {
	return self.GetObjectPropertyValue(PropertyValueAddress);
}

UObject * _dasUnreal_virtual_462_GetObjectPropertyValue_InContainer(const FObjectPropertyBase & self,const void * ContainerAddress,int ArrayIndex) {
	return self.GetObjectPropertyValue_InContainer(ContainerAddress,ArrayIndex);
}

void _dasUnreal_virtual_463_SetObjectPropertyValue(const FObjectPropertyBase & self,void * PropertyValueAddress,UObject * Value) {
	self.SetObjectPropertyValue(PropertyValueAddress,Value);
}

void _dasUnreal_virtual_464_SetObjectPropertyValue_InContainer(const FObjectPropertyBase & self,void * ContainerAddress,UObject * Value,int ArrayIndex) {
	self.SetObjectPropertyValue_InContainer(ContainerAddress,Value,ArrayIndex);
}

FFieldClass * _dasUnreal_static_465_StaticClass(const FStrProperty *) {
	return FStrProperty::StaticClass();
}

FField * _dasUnreal_static_466_Construct(const FStrProperty *,const FFieldVariant & InOwner,const FName & InName,EObjectFlags InObjectFlags) {
	return FStrProperty::Construct(InOwner,InName,InObjectFlags);
}

uint64 _dasUnreal_static_467_StaticClassCastFlagsPrivate(const FStrProperty *) {
	return FStrProperty::StaticClassCastFlagsPrivate();
}

uint64 _dasUnreal_static_468_StaticClassCastFlags(const FStrProperty *) {
	return FStrProperty::StaticClassCastFlags();
}

FString _dasUnreal_virtual_469_GetCPPTypeForwardDeclaration(const FStrProperty & self) {
	return self.GetCPPTypeForwardDeclaration();
}

unsigned int _dasUnreal_virtual_470_GetValueTypeHashInternal(const FStrProperty & self,const void * Src) {
	return self.GetValueTypeHashInternal(Src);
}

FFieldClass * _dasUnreal_static_471_StaticClass(const FStructProperty *) {
	return FStructProperty::StaticClass();
}

FField * _dasUnreal_static_472_Construct(const FStructProperty *,const FFieldVariant & InOwner,const FName & InName,EObjectFlags InObjectFlags) {
	return FStructProperty::Construct(InOwner,InName,InObjectFlags);
}

uint64 _dasUnreal_static_473_StaticClassCastFlagsPrivate(const FStructProperty *) {
	return FStructProperty::StaticClassCastFlagsPrivate();
}

uint64 _dasUnreal_static_474_StaticClassCastFlags(const FStructProperty *) {
	return FStructProperty::StaticClassCastFlags();
}

void _dasUnreal_virtual_475_Serialize(FStructProperty & self,FArchive & Ar) {
	self.Serialize(Ar);
}

void _dasUnreal_virtual_476_AddReferencedObjects(FStructProperty & self,FReferenceCollector & Collector) {
	self.AddReferencedObjects(Collector);
}

void _dasUnreal_virtual_477_PostDuplicate(FStructProperty & self,const FField & InField) {
	self.PostDuplicate(InField);
}

FString _dasUnreal_virtual_478_GetCPPMacroType(const FStructProperty & self,FString & ExtendedTypeText) {
	return self.GetCPPMacroType(ExtendedTypeText);
}

FString _dasUnreal_virtual_479_GetCPPType(const FStructProperty & self,FString * ExtendedTypeText,unsigned int CPPExportFlags) {
	return self.GetCPPType(ExtendedTypeText,CPPExportFlags);
}

FString _dasUnreal_virtual_480_GetCPPTypeForwardDeclaration(const FStructProperty & self) {
	return self.GetCPPTypeForwardDeclaration();
}

void _dasUnreal_virtual_481_LinkInternal(FStructProperty & self,FArchive & Ar) {
	self.LinkInternal(Ar);
}

bool _dasUnreal_virtual_482_Identical(const FStructProperty & self,const void * A,const void * B,unsigned int PortFlags) {
	return self.Identical(A,B,PortFlags);
}

bool _dasUnreal_virtual_483_SupportsNetSharedSerialization(const FStructProperty & self) {
	return self.SupportsNetSharedSerialization();
}

void _dasUnreal_virtual_484_CopyValuesInternal(const FStructProperty & self,void * Dest,const void * Src,int Count) {
	self.CopyValuesInternal(Dest,Src,Count);
}

void _dasUnreal_virtual_485_ClearValueInternal(const FStructProperty & self,void * Data) {
	self.ClearValueInternal(Data);
}

void _dasUnreal_virtual_486_DestroyValueInternal(const FStructProperty & self,void * Dest) {
	self.DestroyValueInternal(Dest);
}

void _dasUnreal_virtual_487_InitializeValueInternal(const FStructProperty & self,void * Dest) {
	self.InitializeValueInternal(Dest);
}

int _dasUnreal_virtual_488_GetMinAlignment(const FStructProperty & self) {
	return self.GetMinAlignment();
}

bool _dasUnreal_virtual_489_SameType(const FStructProperty & self,const FProperty * Other) {
	return self.SameType(Other);
}

void _dasUnreal_virtual_490_AppendSchemaHash(const FStructProperty & self,FBlake3 & Builder,bool bSkipEditorOnly) {
	self.AppendSchemaHash(Builder,bSkipEditorOnly);
}

FProperty * _dasUnreal_static_491_GetNextPropertyAndAdvance(const FCustomPropertyListNode *,const FCustomPropertyListNode *& Node) {
	return FCustomPropertyListNode::GetNextPropertyAndAdvance(Node);
}

UClass * _dasUnreal_static_492_StaticClass(const UPropertyWrapper *) {
	return UPropertyWrapper::StaticClass();
}

const wchar_t * _dasUnreal_static_493_StaticPackage(const UPropertyWrapper *) {
	return UPropertyWrapper::StaticPackage();
}

void _dasUnreal_static_494_StaticRegisterNativesUPropertyWrapper(const UPropertyWrapper *) {
	UPropertyWrapper::StaticRegisterNativesUPropertyWrapper();
}

void _dasUnreal_virtual_495_Serialize(FFrame & self,const wchar_t * V,ELogVerbosity::Type Verbosity,const FName & Category) {
	self.Serialize(V,Verbosity,Category);
}

void _dasUnreal_static_496_KismetExecutionMessage(const FFrame *,const wchar_t * Message,ELogVerbosity::Type Verbosity,FName WarningId) {
	FFrame::KismetExecutionMessage(Message,Verbosity,WarningId);
}

FFrame * _dasUnreal_static_497_PushThreadLocalTopStackFrame(const FFrame *,FFrame * NewTopStackFrame) {
	return FFrame::PushThreadLocalTopStackFrame(NewTopStackFrame);
}

void _dasUnreal_static_498_PopThreadLocalTopStackFrame(const FFrame *,FFrame * NewTopStackFrame) {
	FFrame::PopThreadLocalTopStackFrame(NewTopStackFrame);
}

FFrame * _dasUnreal_static_499_GetThreadLocalTopStackFrame(const FFrame *) {
	return FFrame::GetThreadLocalTopStackFrame();
}

