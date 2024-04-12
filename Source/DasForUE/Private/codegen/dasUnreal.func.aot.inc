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

FSetElementId _dasUnreal_static_79_FromInteger(const FSetElementId *,int Integer) {
	return FSetElementId::FromInteger(Integer);
}

FPrimaryAssetId _dasUnreal_static_80_ParseTypeAndName(const FPrimaryAssetId *,const wchar_t * TypeAndName,unsigned int Len) {
	return FPrimaryAssetId::ParseTypeAndName(TypeAndName,Len);
}

FPrimaryAssetId _dasUnreal_static_81_ParseTypeAndName(const FPrimaryAssetId *,FName TypeAndName) {
	return FPrimaryAssetId::ParseTypeAndName(TypeAndName);
}

FPrimaryAssetId _dasUnreal_static_82_ParseTypeAndName(const FPrimaryAssetId *,const FString & TypeAndName) {
	return FPrimaryAssetId::ParseTypeAndName(TypeAndName);
}

FPrimaryAssetId _dasUnreal_static_83_FromString(const FPrimaryAssetId *,const FString & String) {
	return FPrimaryAssetId::FromString(String);
}

const char * _dasUnreal_static_84_GetGroupName(const FStatGroup_STATGROUP_Script *) {
	return FStatGroup_STATGROUP_Script::GetGroupName();
}

const char * _dasUnreal_static_85_GetGroupCategory(const FStatGroup_STATGROUP_Script *) {
	return FStatGroup_STATGROUP_Script::GetGroupCategory();
}

const wchar_t * _dasUnreal_static_86_GetDescription(const FStatGroup_STATGROUP_Script *) {
	return FStatGroup_STATGROUP_Script::GetDescription();
}

bool _dasUnreal_static_87_IsDefaultEnabled(const FStatGroup_STATGROUP_Script *) {
	return FStatGroup_STATGROUP_Script::IsDefaultEnabled();
}

bool _dasUnreal_static_88_IsCompileTimeEnable(const FStatGroup_STATGROUP_Script *) {
	return FStatGroup_STATGROUP_Script::IsCompileTimeEnable();
}

bool _dasUnreal_static_89_GetSortByName(const FStatGroup_STATGROUP_Script *) {
	return FStatGroup_STATGROUP_Script::GetSortByName();
}

void _dasUnreal_static_90_ThrowScriptException(const FBlueprintCoreDelegates *,const UObject * ActiveObject,FFrame & StackFrame,const FBlueprintExceptionInfo & Info) {
	FBlueprintCoreDelegates::ThrowScriptException(ActiveObject,StackFrame,Info);
}

void _dasUnreal_static_91_InstrumentScriptEvent(const FBlueprintCoreDelegates *,const FScriptInstrumentationSignal & Info) {
	FBlueprintCoreDelegates::InstrumentScriptEvent(Info);
}

void _dasUnreal_static_92_SetScriptMaximumLoopIterations(const FBlueprintCoreDelegates *,const int MaximumLoopIterations) {
	FBlueprintCoreDelegates::SetScriptMaximumLoopIterations(MaximumLoopIterations);
}

bool _dasUnreal_static_93_IsDebuggingEnabled(const FBlueprintCoreDelegates *) {
	return FBlueprintCoreDelegates::IsDebuggingEnabled();
}

FBlueprintContextTracker & _dasUnreal_static_94_Get(const FBlueprintContextTracker *) {
	return FBlueprintContextTracker::Get();
}

const FBlueprintContextTracker * _dasUnreal_static_95_TryGet(const FBlueprintContextTracker *) {
	return FBlueprintContextTracker::TryGet();
}

void _dasUnreal_static_96_AssertIfInConstructor(const FObjectInitializer *,UObject * Outer,const wchar_t * ErrorMessage) {
	FObjectInitializer::AssertIfInConstructor(Outer,ErrorMessage);
}

FObjectInitializer & _dasUnreal_static_97_Get(const FObjectInitializer *) {
	return FObjectInitializer::Get();
}

void _dasUnreal_static_98_InitProperties(const FScriptIntegrationObjectHelper *,const FObjectInitializer & ObjectInitializer,UObject * Obj,UClass * DefaultsClass,UObject * DefaultData) {
	FScriptIntegrationObjectHelper::InitProperties(ObjectInitializer,Obj,DefaultsClass,DefaultData);
}

bool _dasUnreal_static_99_InitSubobjectProperties(const FScriptIntegrationObjectHelper *,const FObjectInitializer & ObjectInitializer) {
	return FScriptIntegrationObjectHelper::InitSubobjectProperties(ObjectInitializer);
}

void _dasUnreal_static_100_InstanceSubobjects(const FScriptIntegrationObjectHelper *,const FObjectInitializer & ObjectInitializer,UClass * Class,bool bNeedInstancing,bool bNeedSubobjectInstancing) {
	FScriptIntegrationObjectHelper::InstanceSubobjects(ObjectInitializer,Class,bNeedInstancing,bNeedSubobjectInstancing);
}

void _dasUnreal_static_101_PostConstructInitObject(const FScriptIntegrationObjectHelper *,FObjectInitializer & ObjectInitializer) {
	FScriptIntegrationObjectHelper::PostConstructInitObject(ObjectInitializer);
}

void _dasUnreal_virtual_102_AddStableReference(FReferenceCollector & self,UObject ** Object) {
	self.AddStableReference(Object);
}

bool _dasUnreal_virtual_103_NeedsPropertyReferencer(const FReferenceCollector & self) {
	return self.NeedsPropertyReferencer();
}

void _dasUnreal_virtual_104_AllowEliminatingReferences(FReferenceCollector & self,bool bAllow) {
	self.AllowEliminatingReferences(bAllow);
}

void _dasUnreal_virtual_105_SetSerializedProperty(FReferenceCollector & self,FProperty * Inproperty) {
	self.SetSerializedProperty(Inproperty);
}

FProperty * _dasUnreal_virtual_106_GetSerializedProperty(const FReferenceCollector & self) {
	return self.GetSerializedProperty();
}

bool _dasUnreal_virtual_107_MarkWeakObjectReferenceForClearing(FReferenceCollector & self,UObject ** WeakReference) {
	return self.MarkWeakObjectReferenceForClearing(WeakReference);
}

void _dasUnreal_virtual_108_SetIsProcessingNativeReferences(FReferenceCollector & self,bool bIsNative) {
	self.SetIsProcessingNativeReferences(bIsNative);
}

bool _dasUnreal_virtual_109_IsProcessingNativeReferences(const FReferenceCollector & self) {
	return self.IsProcessingNativeReferences();
}

bool _dasUnreal_virtual_110_NeedsInitialReferences(const FReferenceCollector & self) {
	return self.NeedsInitialReferences();
}

void _dasUnreal_virtual_111_FindReferences(FReferenceFinder & self,UObject * Object,UObject * ReferencingObject,FProperty * ReferencingProperty) {
	self.FindReferences(Object,ReferencingObject,ReferencingProperty);
}

bool _dasUnreal_virtual_112_IsIgnoringArchetypeRef(const FReferenceFinder & self) {
	return self.IsIgnoringArchetypeRef();
}

bool _dasUnreal_virtual_113_IsIgnoringTransient(const FReferenceFinder & self) {
	return self.IsIgnoringTransient();
}

void _dasUnreal_virtual_114_SetSerializedProperty(FReferenceFinder & self,FProperty * Inproperty) {
	self.SetSerializedProperty(Inproperty);
}

FProperty * _dasUnreal_virtual_115_GetSerializedProperty(const FReferenceFinder & self) {
	return self.GetSerializedProperty();
}

void _dasUnreal_static_116_BroadcastOnObjectModified(const FCoreUObjectDelegates *,UObject * Object) {
	FCoreUObjectDelegates::BroadcastOnObjectModified(Object);
}

FString _dasUnreal_static_117_FormatPathForAssetLog(const FAssetMsg *,const wchar_t * Path) {
	return FAssetMsg::FormatPathForAssetLog(Path);
}

FString _dasUnreal_static_118_FormatPathForAssetLog(const FAssetMsg *,const UObject * Object) {
	return FAssetMsg::FormatPathForAssetLog(Object);
}

FString _dasUnreal_static_119_GetAssetLogString(const FAssetMsg *,const wchar_t * Path,const FString & Message) {
	return FAssetMsg::GetAssetLogString(Path,Message);
}

FString _dasUnreal_static_120_GetAssetLogString(const FAssetMsg *,const UObject * Object,const FString & Message) {
	return FAssetMsg::GetAssetLogString(Object,Message);
}

FName _dasUnreal_virtual_121_GetFNameForStatID(const UObjectBase & self) {
	return self.GetFNameForStatID();
}

FString _dasUnreal_static_122_RemoveClassPrefix(const UObjectBase *,const wchar_t * ClassName) {
	return UObjectBase::RemoveClassPrefix(ClassName);
}

void _dasUnreal_static_123_PrefetchClass(const UObjectBase *,UObject * Object) {
	UObjectBase::PrefetchClass(Object);
}

void _dasUnreal_static_124_PrefetchOuter(const UObjectBase *,UObject * Object) {
	UObjectBase::PrefetchOuter(Object);
}

bool _dasUnreal_static_125_IsPendingKillEnabled(const UObjectBaseUtility *) {
	return UObjectBaseUtility::IsPendingKillEnabled();
}

EInternalObjectFlags _dasUnreal_static_126_FixGarbageOrPendingKillInternalObjectFlags(const UObjectBaseUtility *,const EInternalObjectFlags InFlags) {
	return UObjectBaseUtility::FixGarbageOrPendingKillInternalObjectFlags(InFlags);
}

bool _dasUnreal_virtual_127_CanBeClusterRoot(const UObjectBaseUtility & self) {
	return self.CanBeClusterRoot();
}

bool _dasUnreal_virtual_128_CanBeInCluster(const UObjectBaseUtility & self) {
	return self.CanBeInCluster();
}

void _dasUnreal_virtual_129_CreateCluster(UObjectBaseUtility & self) {
	self.CreateCluster();
}

void _dasUnreal_virtual_130_OnClusterMarkedAsPendingKill(UObjectBaseUtility & self) {
	self.OnClusterMarkedAsPendingKill();
}

UClass * _dasUnreal_static_131_StaticClass(const UObject *) {
	return UObject::StaticClass();
}

const wchar_t * _dasUnreal_static_132_StaticConfigName(const UObject *) {
	return UObject::StaticConfigName();
}

void _dasUnreal_static_133_StaticRegisterNativesUObject(const UObject *) {
	UObject::StaticRegisterNativesUObject();
}

void _dasUnreal_virtual_134_PostInitProperties(UObject & self) {
	self.PostInitProperties();
}

void _dasUnreal_virtual_135_PostReinitProperties(UObject & self) {
	self.PostReinitProperties();
}

void _dasUnreal_virtual_136_PostCDOContruct(UObject & self) {
	self.PostCDOContruct();
}

bool _dasUnreal_virtual_137_Modify(UObject & self,bool bAlwaysMarkDirty) {
	return self.Modify(bAlwaysMarkDirty);
}

bool _dasUnreal_virtual_138_IsCapturingAsRootObjectForTransaction(const UObject & self) {
	return self.IsCapturingAsRootObjectForTransaction();
}

void _dasUnreal_virtual_139_LoadedFromAnotherClass(UObject & self,const FName & OldClassName) {
	self.LoadedFromAnotherClass(OldClassName);
}

bool _dasUnreal_virtual_140_ResolveSubobject(UObject & self,const wchar_t * SubObjectPath,UObject *& OutObject,bool bLoadIfExists) {
	return self.ResolveSubobject(SubObjectPath,OutObject,bLoadIfExists);
}

bool _dasUnreal_virtual_141_IsReadyForAsyncPostLoad(const UObject & self) {
	return self.IsReadyForAsyncPostLoad();
}

void _dasUnreal_virtual_142_PostLoad(UObject & self) {
	self.PostLoad();
}

void _dasUnreal_virtual_143_BeginDestroy(UObject & self) {
	self.BeginDestroy();
}

bool _dasUnreal_virtual_144_IsReadyForFinishDestroy(UObject & self) {
	return self.IsReadyForFinishDestroy();
}

void _dasUnreal_virtual_145_PostLinkerChange(UObject & self) {
	self.PostLinkerChange();
}

void _dasUnreal_virtual_146_FinishDestroy(UObject & self) {
	self.FinishDestroy();
}

void _dasUnreal_virtual_147_Serialize(UObject & self,FArchive & Ar) {
	self.Serialize(Ar);
}

void _dasUnreal_virtual_148_ShutdownAfterError(UObject & self) {
	self.ShutdownAfterError();
}

void _dasUnreal_virtual_149_PreEditUndo(UObject & self) {
	self.PreEditUndo();
}

void _dasUnreal_virtual_150_PostRename(UObject & self,UObject * OldOuter,const FName OldName) {
	self.PostRename(OldOuter,OldName);
}

void _dasUnreal_virtual_151_PostDuplicate(UObject & self,bool bDuplicateForPIE) {
	self.PostDuplicate(bDuplicateForPIE);
}

void _dasUnreal_virtual_152_PostDuplicate(UObject & self,EDuplicateMode::Type DuplicateMode) {
	self.PostDuplicate(DuplicateMode);
}

bool _dasUnreal_virtual_153_NeedsLoadForClient(const UObject & self) {
	return self.NeedsLoadForClient();
}

bool _dasUnreal_virtual_154_NeedsLoadForServer(const UObject & self) {
	return self.NeedsLoadForServer();
}

bool _dasUnreal_virtual_155_NeedsLoadForEditorGame(const UObject & self) {
	return self.NeedsLoadForEditorGame();
}

bool _dasUnreal_virtual_156_IsEditorOnly(const UObject & self) {
	return self.IsEditorOnly();
}

bool _dasUnreal_virtual_157_HasNonEditorOnlyReferences(const UObject & self) {
	return self.HasNonEditorOnlyReferences();
}

bool _dasUnreal_virtual_158_IsPostLoadThreadSafe(const UObject & self) {
	return self.IsPostLoadThreadSafe();
}

bool _dasUnreal_virtual_159_IsDestructionThreadSafe(const UObject & self) {
	return self.IsDestructionThreadSafe();
}

bool _dasUnreal_static_160_CanCreateInCurrentContext(const UObject *,UObject * Template) {
	return UObject::CanCreateInCurrentContext(Template);
}

void _dasUnreal_virtual_161_PostEditImport(UObject & self) {
	self.PostEditImport();
}

void _dasUnreal_virtual_162_PostReloadConfig(UObject & self,FProperty * PropertyThatWasLoaded) {
	self.PostReloadConfig(PropertyThatWasLoaded);
}

bool _dasUnreal_virtual_163_Rename(UObject & self,const wchar_t * NewName,UObject * NewOuter,unsigned int Flags) {
	return self.Rename(NewName,NewOuter,Flags);
}

FString _dasUnreal_virtual_164_GetDesc(UObject & self) {
	return self.GetDesc();
}

void _dasUnreal_virtual_165_MoveDataToSparseClassDataStruct(const UObject & self) {
	self.MoveDataToSparseClassDataStruct();
}

bool _dasUnreal_virtual_166_ConformSparseClassDataStruct(const UObject & self,UScriptStruct * SourceSparseClassDataStruct,void * SourceSparseClassData) {
	return self.ConformSparseClassDataStruct(SourceSparseClassDataStruct,SourceSparseClassData);
}

bool _dasUnreal_virtual_167_ImplementsGetWorld(const UObject & self) {
	return self.ImplementsGetWorld();
}

FName _dasUnreal_virtual_168_GetExporterName(UObject & self) {
	return self.GetExporterName();
}

void _dasUnreal_static_169_AddReferencedObjects(const UObject *,UObject * InThis,FReferenceCollector & Collector) {
	UObject::AddReferencedObjects(InThis,Collector);
}

FRestoreForUObjectOverwrite * _dasUnreal_virtual_170_GetRestoreForUObjectOverwrite(UObject & self) {
	return self.GetRestoreForUObjectOverwrite();
}

bool _dasUnreal_virtual_171_AreNativePropertiesIdenticalTo(const UObject & self,UObject * Other) {
	return self.AreNativePropertiesIdenticalTo(Other);
}

const FName & _dasUnreal_static_172_SourceFileTagName(const UObject *) {
	return UObject::SourceFileTagName();
}

const FName & _dasUnreal_static_173_AssetVersePathTagName(const UObject *) {
	return UObject::AssetVersePathTagName();
}

bool _dasUnreal_virtual_174_IsAsset(const UObject & self) {
	return self.IsAsset();
}

FPrimaryAssetId _dasUnreal_virtual_175_GetPrimaryAssetId(const UObject & self) {
	return self.GetPrimaryAssetId();
}

bool _dasUnreal_virtual_176_IsLocalizedResource(const UObject & self) {
	return self.IsLocalizedResource();
}

bool _dasUnreal_virtual_177_IsSafeForRootSet(const UObject & self) {
	return self.IsSafeForRootSet();
}

void _dasUnreal_virtual_178_TagSubobjects(UObject & self,EObjectFlags NewFlags) {
	self.TagSubobjects(NewFlags);
}

bool _dasUnreal_virtual_179_IsNameStableForNetworking(const UObject & self) {
	return self.IsNameStableForNetworking();
}

bool _dasUnreal_virtual_180_IsFullNameStableForNetworking(const UObject & self) {
	return self.IsFullNameStableForNetworking();
}

bool _dasUnreal_virtual_181_IsSupportedForNetworking(const UObject & self) {
	return self.IsSupportedForNetworking();
}

void _dasUnreal_virtual_182_PreNetReceive(UObject & self) {
	self.PreNetReceive();
}

void _dasUnreal_virtual_183_PostNetReceive(UObject & self) {
	self.PostNetReceive();
}

void _dasUnreal_virtual_184_PostRepNotifies(UObject & self) {
	self.PostRepNotifies();
}

void _dasUnreal_virtual_185_PreDestroyFromReplication(UObject & self) {
	self.PreDestroyFromReplication();
}

void _dasUnreal_virtual_186_WillNeverCacheCookedPlatformDataAgain(UObject & self) {
	self.WillNeverCacheCookedPlatformDataAgain();
}

void _dasUnreal_virtual_187_ClearAllCachedCookedPlatformData(UObject & self) {
	self.ClearAllCachedCookedPlatformData();
}

const wchar_t * _dasUnreal_virtual_188_GetConfigOverridePlatform(const UObject & self) {
	return self.GetConfigOverridePlatform();
}

void _dasUnreal_virtual_189_OverrideConfigSection(UObject & self,FString & SectionName) {
	self.OverrideConfigSection(SectionName);
}

void _dasUnreal_virtual_190_OverridePerObjectConfigSection(UObject & self,FString & SectionName) {
	self.OverridePerObjectConfigSection(SectionName);
}

UObject * _dasUnreal_static_191_GetArchetypeFromRequiredInfo(const UObject *,const UClass * Class,const UObject * Outer,FName Name,EObjectFlags ObjectFlags) {
	return UObject::GetArchetypeFromRequiredInfo(Class,Outer,Name,ObjectFlags);
}

void _dasUnreal_virtual_192_ProcessEvent(UObject & self,UFunction * Function,void * Parms) {
	self.ProcessEvent(Function,Parms);
}

int _dasUnreal_virtual_193_GetFunctionCallspace(UObject & self,UFunction * Function,FFrame * Stack) {
	return self.GetFunctionCallspace(Function,Stack);
}

bool _dasUnreal_virtual_194_CallRemoteFunction(UObject & self,UFunction * Function,void * Parms,FOutParmRec * OutParms,FFrame * Stack) {
	return self.CallRemoteFunction(Function,Parms,OutParms,Stack);
}

UClass * _dasUnreal_virtual_195_RegenerateClass(UObject & self,UClass * ClassToRegenerate,UObject * PreviousCDO) {
	return self.RegenerateClass(ClassToRegenerate,PreviousCDO);
}

void _dasUnreal_virtual_196_MarkAsEditorOnlySubobject(UObject & self) {
	self.MarkAsEditorOnlySubobject();
}

uint64 _dasUnreal_virtual_197_GetNetPushIdDynamic(const UObject & self) {
	return self.GetNetPushIdDynamic();
}

FFieldClass * _dasUnreal_static_198_StaticClass(const FField *) {
	return FField::StaticClass();
}

uint64 _dasUnreal_static_199_StaticClassCastFlagsPrivate(const FField *) {
	return FField::StaticClassCastFlagsPrivate();
}

uint64 _dasUnreal_static_200_StaticClassCastFlags(const FField *) {
	return FField::StaticClassCastFlags();
}

void _dasUnreal_virtual_201_Serialize(FField & self,FArchive & Ar) {
	self.Serialize(Ar);
}

void _dasUnreal_virtual_202_PostLoad(FField & self) {
	self.PostLoad();
}

void _dasUnreal_virtual_203_BeginDestroy(FField & self) {
	self.BeginDestroy();
}

void _dasUnreal_virtual_204_AddReferencedObjects(FField & self,FReferenceCollector & Collector) {
	self.AddReferencedObjects(Collector);
}

void _dasUnreal_virtual_205_AddCppProperty(FField & self,FProperty * Property) {
	self.AddCppProperty(Property);
}

void _dasUnreal_virtual_206_Bind(FField & self) {
	self.Bind();
}

FField * _dasUnreal_static_207_Construct(const FField *,const FFieldVariant & InOwner,const FName & InName,EObjectFlags InFlags) {
	return FField::Construct(InOwner,InName,InFlags);
}

FField * _dasUnreal_static_208_Construct(const FField *,const FName & FieldTypeName,const FFieldVariant & InOwner,const FName & InName,EObjectFlags InFlags) {
	return FField::Construct(FieldTypeName,InOwner,InName,InFlags);
}

void _dasUnreal_virtual_209_PostDuplicate(FField & self,const FField & InField) {
	self.PostDuplicate(InField);
}

FField * _dasUnreal_virtual_210_GetInnerFieldByName(FField & self,const FName & InName) {
	return self.GetInnerFieldByName(InName);
}

void _dasUnreal_static_211_CopyMetaData(const FField *,const FField * InSourceField,FField * InDestField) {
	FField::CopyMetaData(InSourceField,InDestField);
}

FField * _dasUnreal_static_212_CreateFromUField(const FField *,UField * InField) {
	return FField::CreateFromUField(InField);
}

FField * _dasUnreal_static_213_Duplicate(const FField *,const FField * InField,FFieldVariant DestOwner,const FName DestName,EObjectFlags FlagMask,EInternalObjectFlags InternalFlagsMask) {
	return FField::Duplicate(InField,DestOwner,DestName,FlagMask,InternalFlagsMask);
}

FName _dasUnreal_static_214_GenerateFFieldName(const FField *,FFieldVariant InOwner,FFieldClass * InClass) {
	return FField::GenerateFFieldName(InOwner,InClass);
}

UClass * _dasUnreal_static_215_StaticClass(const UField *) {
	return UField::StaticClass();
}

const wchar_t * _dasUnreal_static_216_StaticPackage(const UField *) {
	return UField::StaticPackage();
}

void _dasUnreal_static_217_StaticRegisterNativesUField(const UField *) {
	UField::StaticRegisterNativesUField();
}

void _dasUnreal_virtual_218_Serialize(UField & self,FArchive & Ar) {
	self.Serialize(Ar);
}

void _dasUnreal_virtual_219_PostLoad(UField & self) {
	self.PostLoad();
}

bool _dasUnreal_virtual_220_NeedsLoadForClient(const UField & self) {
	return self.NeedsLoadForClient();
}

bool _dasUnreal_virtual_221_NeedsLoadForServer(const UField & self) {
	return self.NeedsLoadForServer();
}

void _dasUnreal_virtual_222_Bind(UField & self) {
	self.Bind();
}

void _dasUnreal_static_223_FormatNativeToolTip(const UField *,FString & ToolTipString,bool bRemoveExtraSections) {
	UField::FormatNativeToolTip(ToolTipString,bRemoveExtraSections);
}

FField * _dasUnreal_virtual_224_GetAssociatedFField(UField & self) {
	return self.GetAssociatedFField();
}

void _dasUnreal_virtual_225_SetAssociatedFField(UField & self,FField * InField) {
	self.SetAssociatedFField(InField);
}

UClass * _dasUnreal_static_226_StaticClass(const UStruct *) {
	return UStruct::StaticClass();
}

const wchar_t * _dasUnreal_static_227_StaticPackage(const UStruct *) {
	return UStruct::StaticPackage();
}

void _dasUnreal_static_228_StaticRegisterNativesUStruct(const UStruct *) {
	UStruct::StaticRegisterNativesUStruct();
}

void _dasUnreal_virtual_229_Serialize(UStruct & self,FArchive & Ar) {
	self.Serialize(Ar);
}

void _dasUnreal_virtual_230_PostLoad(UStruct & self) {
	self.PostLoad();
}

void _dasUnreal_virtual_231_FinishDestroy(UStruct & self) {
	self.FinishDestroy();
}

void _dasUnreal_virtual_232_RegisterDependencies(UStruct & self) {
	self.RegisterDependencies();
}

void _dasUnreal_static_233_AddReferencedObjects(const UStruct *,UObject * InThis,FReferenceCollector & Collector) {
	UStruct::AddReferencedObjects(InThis,Collector);
}

void _dasUnreal_virtual_234_TagSubobjects(UStruct & self,EObjectFlags NewFlags) {
	self.TagSubobjects(NewFlags);
}

void _dasUnreal_virtual_235_AddCppProperty(UStruct & self,FProperty * Property) {
	self.AddCppProperty(Property);
}

UStruct * _dasUnreal_virtual_236_GetInheritanceSuper(const UStruct & self) {
	return self.GetInheritanceSuper();
}

void _dasUnreal_virtual_237_Link(UStruct & self,FArchive & Ar,bool bRelinkExistingProperties) {
	self.Link(Ar,bRelinkExistingProperties);
}

void _dasUnreal_virtual_238_SerializeBin(const UStruct & self,FArchive & Ar,void * Data) {
	self.SerializeBin(Ar,Data);
}

void _dasUnreal_virtual_239_InitializeStruct(const UStruct & self,void * Dest,int ArrayDim) {
	self.InitializeStruct(Dest,ArrayDim);
}

void _dasUnreal_virtual_240_DestroyStruct(const UStruct & self,void * Dest,int ArrayDim) {
	self.DestroyStruct(Dest,ArrayDim);
}

FProperty * _dasUnreal_virtual_241_CustomFindProperty(const UStruct & self,const FName InName) {
	return self.CustomFindProperty(InName);
}

const wchar_t * _dasUnreal_virtual_242_GetPrefixCPP(const UStruct & self) {
	return self.GetPrefixCPP();
}

void _dasUnreal_virtual_243_SetSuperStruct(UStruct & self,UStruct * NewSuperStruct) {
	self.SetSuperStruct(NewSuperStruct);
}

FString _dasUnreal_virtual_244_GetAuthoredNameForField(const UStruct & self,const UField * Field) {
	return self.GetAuthoredNameForField(Field);
}

FString _dasUnreal_virtual_245_GetAuthoredNameForField(const UStruct & self,const FField * Field) {
	return self.GetAuthoredNameForField(Field);
}

bool _dasUnreal_virtual_246_IsStructTrashed(const UStruct & self) {
	return self.IsStructTrashed();
}

UClass * _dasUnreal_static_247_StaticClass(const UScriptStruct *) {
	return UScriptStruct::StaticClass();
}

const wchar_t * _dasUnreal_static_248_StaticPackage(const UScriptStruct *) {
	return UScriptStruct::StaticPackage();
}

void _dasUnreal_static_249_StaticRegisterNativesUScriptStruct(const UScriptStruct *) {
	UScriptStruct::StaticRegisterNativesUScriptStruct();
}

void _dasUnreal_virtual_250_Serialize(UScriptStruct & self,FArchive & Ar) {
	self.Serialize(Ar);
}

void _dasUnreal_virtual_251_Link(UScriptStruct & self,FArchive & Ar,bool bRelinkExistingProperties) {
	self.Link(Ar,bRelinkExistingProperties);
}

void _dasUnreal_virtual_252_InitializeStruct(const UScriptStruct & self,void * Dest,int ArrayDim) {
	self.InitializeStruct(Dest,ArrayDim);
}

void _dasUnreal_virtual_253_DestroyStruct(const UScriptStruct & self,void * Dest,int ArrayDim) {
	self.DestroyStruct(Dest,ArrayDim);
}

bool _dasUnreal_virtual_254_IsStructTrashed(const UScriptStruct & self) {
	return self.IsStructTrashed();
}

void _dasUnreal_virtual_255_PrepareCppStructOps(UScriptStruct & self) {
	self.PrepareCppStructOps();
}

unsigned int _dasUnreal_virtual_256_GetStructTypeHash(const UScriptStruct & self,const void * Src) {
	return self.GetStructTypeHash(Src);
}

void _dasUnreal_virtual_257_RecursivelyPreload(UScriptStruct & self) {
	self.RecursivelyPreload();
}

FString _dasUnreal_virtual_258_GetStructCPPName(const UScriptStruct & self,unsigned int CPPExportFlags) {
	return self.GetStructCPPName(CPPExportFlags);
}

void _dasUnreal_virtual_259_InitializeDefaultValue(const UScriptStruct & self,unsigned char * InStructData) {
	self.InitializeDefaultValue(InStructData);
}

UClass * _dasUnreal_static_260_StaticClass(const UFunction *) {
	return UFunction::StaticClass();
}

const wchar_t * _dasUnreal_static_261_StaticPackage(const UFunction *) {
	return UFunction::StaticPackage();
}

void _dasUnreal_static_262_StaticRegisterNativesUFunction(const UFunction *) {
	UFunction::StaticRegisterNativesUFunction();
}

void _dasUnreal_virtual_263_Serialize(UFunction & self,FArchive & Ar) {
	self.Serialize(Ar);
}

void _dasUnreal_virtual_264_PostLoad(UFunction & self) {
	self.PostLoad();
}

void _dasUnreal_virtual_265_Bind(UFunction & self) {
	self.Bind();
}

UStruct * _dasUnreal_virtual_266_GetInheritanceSuper(const UFunction & self) {
	return self.GetInheritanceSuper();
}

void _dasUnreal_virtual_267_Link(UFunction & self,FArchive & Ar,bool bRelinkExistingProperties) {
	self.Link(Ar,bRelinkExistingProperties);
}

uint64 _dasUnreal_static_268_GetDefaultIgnoredSignatureCompatibilityFlags(const UFunction *) {
	return UFunction::GetDefaultIgnoredSignatureCompatibilityFlags();
}

UClass * _dasUnreal_static_269_StaticClass(const UEnum *) {
	return UEnum::StaticClass();
}

const wchar_t * _dasUnreal_static_270_StaticPackage(const UEnum *) {
	return UEnum::StaticPackage();
}

void _dasUnreal_static_271_StaticRegisterNativesUEnum(const UEnum *) {
	UEnum::StaticRegisterNativesUEnum();
}

FText _dasUnreal_virtual_272_GetDisplayNameTextByIndex(const UEnum & self,int InIndex) {
	return self.GetDisplayNameTextByIndex(InIndex);
}

FString _dasUnreal_virtual_273_GetAuthoredNameStringByIndex(const UEnum & self,int InIndex) {
	return self.GetAuthoredNameStringByIndex(InIndex);
}

bool _dasUnreal_static_274_IsFullEnumName(const UEnum *,const wchar_t * InEnumName) {
	return UEnum::IsFullEnumName(InEnumName);
}

FString _dasUnreal_virtual_275_GenerateFullEnumName(const UEnum & self,const wchar_t * InEnumName) {
	return self.GenerateFullEnumName(InEnumName);
}

int64 _dasUnreal_static_276_ParseEnum(const UEnum *,const wchar_t *& Str) {
	return UEnum::ParseEnum(Str);
}

void _dasUnreal_virtual_277_Serialize(UEnum & self,FArchive & Ar) {
	self.Serialize(Ar);
}

void _dasUnreal_virtual_278_BeginDestroy(UEnum & self) {
	self.BeginDestroy();
}

bool _dasUnreal_virtual_279_IsAbstract(const FCppClassTypeInfo & self) {
	return self.IsAbstract();
}

UClass * _dasUnreal_static_280_StaticClass(const UClass *) {
	return UClass::StaticClass();
}

const wchar_t * _dasUnreal_static_281_StaticPackage(const UClass *) {
	return UClass::StaticPackage();
}

void _dasUnreal_static_282_StaticRegisterNativesUClass(const UClass *) {
	UClass::StaticRegisterNativesUClass();
}

void _dasUnreal_virtual_283_ConditionalRecompileClass(UClass & self,FUObjectSerializeContext * InLoadContext) {
	self.ConditionalRecompileClass(InLoadContext);
}

void _dasUnreal_virtual_284_FlushCompilationQueueForLevel(UClass & self) {
	self.FlushCompilationQueueForLevel();
}

void _dasUnreal_static_285_AssembleReferenceTokenStreams(const UClass *) {
	UClass::AssembleReferenceTokenStreams();
}

UClass * _dasUnreal_virtual_286_GetAuthoritativeClass(UClass & self) {
	return self.GetAuthoritativeClass();
}

void _dasUnreal_virtual_287_Serialize(UClass & self,FArchive & Ar) {
	self.Serialize(Ar);
}

void _dasUnreal_virtual_288_PostLoad(UClass & self) {
	self.PostLoad();
}

void _dasUnreal_virtual_289_FinishDestroy(UClass & self) {
	self.FinishDestroy();
}

void _dasUnreal_virtual_290_DeferredRegister(UClass & self,UClass * UClassStaticClass,const wchar_t * PackageName,const wchar_t * InName) {
	self.DeferredRegister(UClassStaticClass,PackageName,InName);
}

bool _dasUnreal_virtual_291_Rename(UClass & self,const wchar_t * NewName,UObject * NewOuter,unsigned int Flags) {
	return self.Rename(NewName,NewOuter,Flags);
}

void _dasUnreal_virtual_292_TagSubobjects(UClass & self,EObjectFlags NewFlags) {
	self.TagSubobjects(NewFlags);
}

void _dasUnreal_virtual_293_PostInitProperties(UClass & self) {
	self.PostInitProperties();
}

void _dasUnreal_static_294_AddReferencedObjects(const UClass *,UObject * InThis,FReferenceCollector & Collector) {
	UClass::AddReferencedObjects(InThis,Collector);
}

FRestoreForUObjectOverwrite * _dasUnreal_virtual_295_GetRestoreForUObjectOverwrite(UClass & self) {
	return self.GetRestoreForUObjectOverwrite();
}

FString _dasUnreal_virtual_296_GetDesc(UClass & self) {
	return self.GetDesc();
}

bool _dasUnreal_virtual_297_IsAsset(const UClass & self) {
	return self.IsAsset();
}

bool _dasUnreal_virtual_298_IsNameStableForNetworking(const UClass & self) {
	return self.IsNameStableForNetworking();
}

void _dasUnreal_virtual_299_Bind(UClass & self) {
	self.Bind();
}

const wchar_t * _dasUnreal_virtual_300_GetPrefixCPP(const UClass & self) {
	return self.GetPrefixCPP();
}

void _dasUnreal_virtual_301_Link(UClass & self,FArchive & Ar,bool bRelinkExistingProperties) {
	self.Link(Ar,bRelinkExistingProperties);
}

void _dasUnreal_virtual_302_SetSuperStruct(UClass & self,UStruct * NewSuperStruct) {
	self.SetSuperStruct(NewSuperStruct);
}

bool _dasUnreal_virtual_303_IsStructTrashed(const UClass & self) {
	return self.IsStructTrashed();
}

void _dasUnreal_virtual_304_PostLoadInstance(UClass & self,UObject * InObj) {
	self.PostLoadInstance(InObj);
}

void _dasUnreal_virtual_305_InitPropertiesFromCustomList(UClass & self,unsigned char * DataPtr,const unsigned char * DefaultDataPtr) {
	self.InitPropertiesFromCustomList(DataPtr,DefaultDataPtr);
}

bool _dasUnreal_virtual_306_CanCreateAssetOfClass(const UClass & self) {
	return self.CanCreateAssetOfClass();
}

unsigned char * _dasUnreal_virtual_307_GetPersistentUberGraphFrame(const UClass & self,UObject * Obj,UFunction * FuncToCheck) {
	return self.GetPersistentUberGraphFrame(Obj,FuncToCheck);
}

void _dasUnreal_virtual_308_CreatePersistentUberGraphFrame(const UClass & self,UObject * Obj,bool bCreateOnlyIfEmpty,bool bSkipSuperClass,UClass * OldClass) {
	self.CreatePersistentUberGraphFrame(Obj,bCreateOnlyIfEmpty,bSkipSuperClass,OldClass);
}

void _dasUnreal_virtual_309_DestroyPersistentUberGraphFrame(const UClass & self,UObject * Obj,bool bSkipSuperClass) {
	self.DestroyPersistentUberGraphFrame(Obj,bSkipSuperClass);
}

void _dasUnreal_virtual_310_PostLoadDefaultObject(UClass & self,UObject * Object) {
	self.PostLoadDefaultObject(Object);
}

void _dasUnreal_virtual_311_PurgeClass(UClass & self,bool bRecompilingOnLoad) {
	self.PurgeClass(bRecompilingOnLoad);
}

UClass * _dasUnreal_static_312_FindCommonBase(const UClass *,UClass * InClassA,UClass * InClassB) {
	return UClass::FindCommonBase(InClassA,InClassB);
}

bool _dasUnreal_virtual_313_IsFunctionImplementedInScript(const UClass & self,FName InFunctionName) {
	return self.IsFunctionImplementedInScript(InFunctionName);
}

bool _dasUnreal_virtual_314_HasProperty(const UClass & self,FProperty * InProperty) {
	return self.HasProperty(InProperty);
}

UObject * _dasUnreal_virtual_315_FindArchetype(const UClass & self,const UClass * ArchetypeClass,const FName ArchetypeName) {
	return self.FindArchetype(ArchetypeClass,ArchetypeName);
}

UObject * _dasUnreal_virtual_316_GetArchetypeForCDO(const UClass & self) {
	return self.GetArchetypeForCDO();
}

bool _dasUnreal_static_317_IsSafeToSerializeToStructuredArchives(const UClass *,UClass * InClass) {
	return UClass::IsSafeToSerializeToStructuredArchives(InClass);
}

UClass * _dasUnreal_static_318_StaticClass(const UPackage *) {
	return UPackage::StaticClass();
}

const wchar_t * _dasUnreal_static_319_StaticPackage(const UPackage *) {
	return UPackage::StaticPackage();
}

void _dasUnreal_static_320_StaticRegisterNativesUPackage(const UPackage *) {
	UPackage::StaticRegisterNativesUPackage();
}

bool _dasUnreal_virtual_321_IsNameStableForNetworking(const UPackage & self) {
	return self.IsNameStableForNetworking();
}

bool _dasUnreal_virtual_322_NeedsLoadForClient(const UPackage & self) {
	return self.NeedsLoadForClient();
}

bool _dasUnreal_virtual_323_NeedsLoadForServer(const UPackage & self) {
	return self.NeedsLoadForServer();
}

bool _dasUnreal_virtual_324_IsPostLoadThreadSafe(const UPackage & self) {
	return self.IsPostLoadThreadSafe();
}

bool _dasUnreal_virtual_325_IsDestructionThreadSafe(const UPackage & self) {
	return self.IsDestructionThreadSafe();
}

void _dasUnreal_virtual_326_PostInitProperties(UPackage & self) {
	self.PostInitProperties();
}

void _dasUnreal_virtual_327_FinishDestroy(UPackage & self) {
	self.FinishDestroy();
}

void _dasUnreal_virtual_328_Serialize(UPackage & self,FArchive & Ar) {
	self.Serialize(Ar);
}

bool _dasUnreal_virtual_329_IsAsset(const UPackage & self) {
	return self.IsAsset();
}

void _dasUnreal_static_330_AddReferencedObjects(const UPackage *,UObject * InThis,FReferenceCollector & Collector) {
	UPackage::AddReferencedObjects(InThis,Collector);
}

void _dasUnreal_virtual_331_TagSubobjects(UPackage & self,EObjectFlags NewFlags) {
	self.TagSubobjects(NewFlags);
}

void _dasUnreal_static_332_WaitForAsyncFileWrites(const UPackage *) {
	UPackage::WaitForAsyncFileWrites();
}

bool _dasUnreal_static_333_HasAsyncFileWrites(const UPackage *) {
	return UPackage::HasAsyncFileWrites();
}

bool _dasUnreal_static_334_IsEmptyPackage(const UPackage *,UPackage * Package,const UObject * LastReferencer) {
	return UPackage::IsEmptyPackage(Package,LastReferencer);
}

FFieldClass * _dasUnreal_static_335_StaticClass(const FProperty *) {
	return FProperty::StaticClass();
}

uint64 _dasUnreal_static_336_StaticClassCastFlagsPrivate(const FProperty *) {
	return FProperty::StaticClassCastFlagsPrivate();
}

uint64 _dasUnreal_static_337_StaticClassCastFlags(const FProperty *) {
	return FProperty::StaticClassCastFlags();
}

void _dasUnreal_virtual_338_Serialize(FProperty & self,FArchive & Ar) {
	self.Serialize(Ar);
}

void _dasUnreal_virtual_339_PostDuplicate(FProperty & self,const FField & InField) {
	self.PostDuplicate(InField);
}

FName _dasUnreal_static_340_FindRedirectedPropertyName(const FProperty *,UStruct * ObjectStruct,FName OldName) {
	return FProperty::FindRedirectedPropertyName(ObjectStruct,OldName);
}

FString _dasUnreal_virtual_341_GetCPPMacroType(const FProperty & self,FString & ExtendedTypeText) {
	return self.GetCPPMacroType(ExtendedTypeText);
}

bool _dasUnreal_virtual_342_PassCPPArgsByRef(const FProperty & self) {
	return self.PassCPPArgsByRef();
}

FString _dasUnreal_virtual_343_GetCPPType(const FProperty & self,FString * ExtendedTypeText,unsigned int CPPExportFlags) {
	return self.GetCPPType(ExtendedTypeText,CPPExportFlags);
}

FString _dasUnreal_virtual_344_GetCPPTypeForwardDeclaration(const FProperty & self) {
	return self.GetCPPTypeForwardDeclaration();
}

bool _dasUnreal_virtual_345_HasSetter(const FProperty & self) {
	return self.HasSetter();
}

bool _dasUnreal_virtual_346_HasGetter(const FProperty & self) {
	return self.HasGetter();
}

bool _dasUnreal_virtual_347_HasSetterOrGetter(const FProperty & self) {
	return self.HasSetterOrGetter();
}

void _dasUnreal_virtual_348_CallSetter(const FProperty & self,void * Container,const void * InValue) {
	self.CallSetter(Container,InValue);
}

void _dasUnreal_virtual_349_CallGetter(const FProperty & self,const void * Container,void * OutValue) {
	self.CallGetter(Container,OutValue);
}

bool _dasUnreal_virtual_350_Identical(const FProperty & self,const void * A,const void * B,unsigned int PortFlags) {
	return self.Identical(A,B,PortFlags);
}

bool _dasUnreal_virtual_351_SupportsNetSharedSerialization(const FProperty & self) {
	return self.SupportsNetSharedSerialization();
}

void * _dasUnreal_virtual_352_GetValueAddressAtIndex_Direct(const FProperty & self,const FProperty * Inner,void * InValueAddress,int Index) {
	return self.GetValueAddressAtIndex_Direct(Inner,InValueAddress,Index);
}

void _dasUnreal_virtual_353_AppendSchemaHash(const FProperty & self,FBlake3 & Builder,bool bSkipEditorOnly) {
	self.AppendSchemaHash(Builder,bSkipEditorOnly);
}

void _dasUnreal_virtual_354_CopySingleValueToScriptVM(const FProperty & self,void * Dest,const void * Src) {
	self.CopySingleValueToScriptVM(Dest,Src);
}

void _dasUnreal_virtual_355_CopyCompleteValueToScriptVM(const FProperty & self,void * Dest,const void * Src) {
	self.CopyCompleteValueToScriptVM(Dest,Src);
}

void _dasUnreal_virtual_356_CopyCompleteValueToScriptVM_InContainer(const FProperty & self,void * OutValue,const void * InContainer) {
	self.CopyCompleteValueToScriptVM_InContainer(OutValue,InContainer);
}

void _dasUnreal_virtual_357_CopyCompleteValueFromScriptVM_InContainer(const FProperty & self,void * OutContainer,const void * InValue) {
	self.CopyCompleteValueFromScriptVM_InContainer(OutContainer,InValue);
}

void _dasUnreal_virtual_358_CopySingleValueFromScriptVM(const FProperty & self,void * Dest,const void * Src) {
	self.CopySingleValueFromScriptVM(Dest,Src);
}

void _dasUnreal_virtual_359_CopyCompleteValueFromScriptVM(const FProperty & self,void * Dest,const void * Src) {
	self.CopyCompleteValueFromScriptVM(Dest,Src);
}

FName _dasUnreal_virtual_360_GetID(const FProperty & self) {
	return self.GetID();
}

int _dasUnreal_virtual_361_GetMinAlignment(const FProperty & self) {
	return self.GetMinAlignment();
}

bool _dasUnreal_virtual_362_SameType(const FProperty & self,const FProperty * Other) {
	return self.SameType(Other);
}

FFieldClass * _dasUnreal_static_363_StaticClass(const FNumericProperty *) {
	return FNumericProperty::StaticClass();
}

FField * _dasUnreal_static_364_Construct(const FNumericProperty *,const FFieldVariant & InOwner,const FName & InName,EObjectFlags InObjectFlags) {
	return FNumericProperty::Construct(InOwner,InName,InObjectFlags);
}

uint64 _dasUnreal_static_365_StaticClassCastFlagsPrivate(const FNumericProperty *) {
	return FNumericProperty::StaticClassCastFlagsPrivate();
}

uint64 _dasUnreal_static_366_StaticClassCastFlags(const FNumericProperty *) {
	return FNumericProperty::StaticClassCastFlags();
}

const wchar_t * _dasUnreal_virtual_367_ImportText_Internal(const FNumericProperty & self,const wchar_t * Buffer,void * ContainerOrPropertyPtr,EPropertyPointerType PropertyPointerType,UObject * Parent,int PortFlags,FOutputDevice * ErrorText) {
	return self.ImportText_Internal(Buffer,ContainerOrPropertyPtr,PropertyPointerType,Parent,PortFlags,ErrorText);
}

void _dasUnreal_virtual_368_ExportText_Internal(const FNumericProperty & self,FString & ValueStr,const void * PropertyValueOrContainer,EPropertyPointerType PropertyPointerType,const void * DefaultValue,UObject * Parent,int PortFlags,UObject * ExportRootScope) {
	self.ExportText_Internal(ValueStr,PropertyValueOrContainer,PropertyPointerType,DefaultValue,Parent,PortFlags,ExportRootScope);
}

bool _dasUnreal_virtual_369_IsFloatingPoint(const FNumericProperty & self) {
	return self.IsFloatingPoint();
}

bool _dasUnreal_virtual_370_IsInteger(const FNumericProperty & self) {
	return self.IsInteger();
}

UEnum * _dasUnreal_virtual_371_GetIntPropertyEnum(const FNumericProperty & self) {
	return self.GetIntPropertyEnum();
}

void _dasUnreal_virtual_372_SetIntPropertyValue(const FNumericProperty & self,void * Data,uint64 Value) {
	self.SetIntPropertyValue(Data,Value);
}

void _dasUnreal_virtual_373_SetIntPropertyValue(const FNumericProperty & self,void * Data,int64 Value) {
	self.SetIntPropertyValue(Data,Value);
}

void _dasUnreal_virtual_374_SetFloatingPointPropertyValue(const FNumericProperty & self,void * Data,double Value) {
	self.SetFloatingPointPropertyValue(Data,Value);
}

void _dasUnreal_virtual_375_SetNumericPropertyValueFromString(const FNumericProperty & self,void * Data,const wchar_t * Value) {
	self.SetNumericPropertyValueFromString(Data,Value);
}

void _dasUnreal_virtual_376_SetNumericPropertyValueFromString_InContainer(const FNumericProperty & self,void * Container,const wchar_t * Value) {
	self.SetNumericPropertyValueFromString_InContainer(Container,Value);
}

int64 _dasUnreal_virtual_377_GetSignedIntPropertyValue(const FNumericProperty & self,const void * Data) {
	return self.GetSignedIntPropertyValue(Data);
}

int64 _dasUnreal_virtual_378_GetSignedIntPropertyValue_InContainer(const FNumericProperty & self,const void * Container) {
	return self.GetSignedIntPropertyValue_InContainer(Container);
}

uint64 _dasUnreal_virtual_379_GetUnsignedIntPropertyValue(const FNumericProperty & self,const void * Data) {
	return self.GetUnsignedIntPropertyValue(Data);
}

uint64 _dasUnreal_virtual_380_GetUnsignedIntPropertyValue_InContainer(const FNumericProperty & self,const void * Container) {
	return self.GetUnsignedIntPropertyValue_InContainer(Container);
}

double _dasUnreal_virtual_381_GetFloatingPointPropertyValue(const FNumericProperty & self,const void * Data) {
	return self.GetFloatingPointPropertyValue(Data);
}

FString _dasUnreal_virtual_382_GetNumericPropertyValueToString(const FNumericProperty & self,const void * Data) {
	return self.GetNumericPropertyValueToString(Data);
}

FString _dasUnreal_virtual_383_GetNumericPropertyValueToString_InContainer(const FNumericProperty & self,const void * Container) {
	return self.GetNumericPropertyValueToString_InContainer(Container);
}

FFieldClass * _dasUnreal_static_384_StaticClass(const FByteProperty *) {
	return FByteProperty::StaticClass();
}

FField * _dasUnreal_static_385_Construct(const FByteProperty *,const FFieldVariant & InOwner,const FName & InName,EObjectFlags InObjectFlags) {
	return FByteProperty::Construct(InOwner,InName,InObjectFlags);
}

uint64 _dasUnreal_static_386_StaticClassCastFlagsPrivate(const FByteProperty *) {
	return FByteProperty::StaticClassCastFlagsPrivate();
}

uint64 _dasUnreal_static_387_StaticClassCastFlags(const FByteProperty *) {
	return FByteProperty::StaticClassCastFlags();
}

void _dasUnreal_virtual_388_Serialize(FByteProperty & self,FArchive & Ar) {
	self.Serialize(Ar);
}

void _dasUnreal_virtual_389_AddReferencedObjects(FByteProperty & self,FReferenceCollector & Collector) {
	self.AddReferencedObjects(Collector);
}

void _dasUnreal_virtual_390_PostDuplicate(FByteProperty & self,const FField & InField) {
	self.PostDuplicate(InField);
}

FString _dasUnreal_virtual_391_GetCPPType(const FByteProperty & self,FString * ExtendedTypeText,unsigned int CPPExportFlags) {
	return self.GetCPPType(ExtendedTypeText,CPPExportFlags);
}

void _dasUnreal_virtual_392_AppendSchemaHash(const FByteProperty & self,FBlake3 & Builder,bool bSkipEditorOnly) {
	self.AppendSchemaHash(Builder,bSkipEditorOnly);
}

UEnum * _dasUnreal_virtual_393_GetIntPropertyEnum(const FByteProperty & self) {
	return self.GetIntPropertyEnum();
}

FFieldClass * _dasUnreal_static_394_StaticClass(const FInt8Property *) {
	return FInt8Property::StaticClass();
}

FField * _dasUnreal_static_395_Construct(const FInt8Property *,const FFieldVariant & InOwner,const FName & InName,EObjectFlags InObjectFlags) {
	return FInt8Property::Construct(InOwner,InName,InObjectFlags);
}

uint64 _dasUnreal_static_396_StaticClassCastFlagsPrivate(const FInt8Property *) {
	return FInt8Property::StaticClassCastFlagsPrivate();
}

uint64 _dasUnreal_static_397_StaticClassCastFlags(const FInt8Property *) {
	return FInt8Property::StaticClassCastFlags();
}

FFieldClass * _dasUnreal_static_398_StaticClass(const FInt16Property *) {
	return FInt16Property::StaticClass();
}

FField * _dasUnreal_static_399_Construct(const FInt16Property *,const FFieldVariant & InOwner,const FName & InName,EObjectFlags InObjectFlags) {
	return FInt16Property::Construct(InOwner,InName,InObjectFlags);
}

uint64 _dasUnreal_static_400_StaticClassCastFlagsPrivate(const FInt16Property *) {
	return FInt16Property::StaticClassCastFlagsPrivate();
}

uint64 _dasUnreal_static_401_StaticClassCastFlags(const FInt16Property *) {
	return FInt16Property::StaticClassCastFlags();
}

FFieldClass * _dasUnreal_static_402_StaticClass(const FIntProperty *) {
	return FIntProperty::StaticClass();
}

FField * _dasUnreal_static_403_Construct(const FIntProperty *,const FFieldVariant & InOwner,const FName & InName,EObjectFlags InObjectFlags) {
	return FIntProperty::Construct(InOwner,InName,InObjectFlags);
}

uint64 _dasUnreal_static_404_StaticClassCastFlagsPrivate(const FIntProperty *) {
	return FIntProperty::StaticClassCastFlagsPrivate();
}

uint64 _dasUnreal_static_405_StaticClassCastFlags(const FIntProperty *) {
	return FIntProperty::StaticClassCastFlags();
}

FFieldClass * _dasUnreal_static_406_StaticClass(const FInt64Property *) {
	return FInt64Property::StaticClass();
}

FField * _dasUnreal_static_407_Construct(const FInt64Property *,const FFieldVariant & InOwner,const FName & InName,EObjectFlags InObjectFlags) {
	return FInt64Property::Construct(InOwner,InName,InObjectFlags);
}

uint64 _dasUnreal_static_408_StaticClassCastFlagsPrivate(const FInt64Property *) {
	return FInt64Property::StaticClassCastFlagsPrivate();
}

uint64 _dasUnreal_static_409_StaticClassCastFlags(const FInt64Property *) {
	return FInt64Property::StaticClassCastFlags();
}

FFieldClass * _dasUnreal_static_410_StaticClass(const FUInt16Property *) {
	return FUInt16Property::StaticClass();
}

FField * _dasUnreal_static_411_Construct(const FUInt16Property *,const FFieldVariant & InOwner,const FName & InName,EObjectFlags InObjectFlags) {
	return FUInt16Property::Construct(InOwner,InName,InObjectFlags);
}

uint64 _dasUnreal_static_412_StaticClassCastFlagsPrivate(const FUInt16Property *) {
	return FUInt16Property::StaticClassCastFlagsPrivate();
}

uint64 _dasUnreal_static_413_StaticClassCastFlags(const FUInt16Property *) {
	return FUInt16Property::StaticClassCastFlags();
}

FFieldClass * _dasUnreal_static_414_StaticClass(const FUInt32Property *) {
	return FUInt32Property::StaticClass();
}

FField * _dasUnreal_static_415_Construct(const FUInt32Property *,const FFieldVariant & InOwner,const FName & InName,EObjectFlags InObjectFlags) {
	return FUInt32Property::Construct(InOwner,InName,InObjectFlags);
}

uint64 _dasUnreal_static_416_StaticClassCastFlagsPrivate(const FUInt32Property *) {
	return FUInt32Property::StaticClassCastFlagsPrivate();
}

uint64 _dasUnreal_static_417_StaticClassCastFlags(const FUInt32Property *) {
	return FUInt32Property::StaticClassCastFlags();
}

FFieldClass * _dasUnreal_static_418_StaticClass(const FUInt64Property *) {
	return FUInt64Property::StaticClass();
}

FField * _dasUnreal_static_419_Construct(const FUInt64Property *,const FFieldVariant & InOwner,const FName & InName,EObjectFlags InObjectFlags) {
	return FUInt64Property::Construct(InOwner,InName,InObjectFlags);
}

uint64 _dasUnreal_static_420_StaticClassCastFlagsPrivate(const FUInt64Property *) {
	return FUInt64Property::StaticClassCastFlagsPrivate();
}

uint64 _dasUnreal_static_421_StaticClassCastFlags(const FUInt64Property *) {
	return FUInt64Property::StaticClassCastFlags();
}

FFieldClass * _dasUnreal_static_422_StaticClass(const FFloatProperty *) {
	return FFloatProperty::StaticClass();
}

FField * _dasUnreal_static_423_Construct(const FFloatProperty *,const FFieldVariant & InOwner,const FName & InName,EObjectFlags InObjectFlags) {
	return FFloatProperty::Construct(InOwner,InName,InObjectFlags);
}

uint64 _dasUnreal_static_424_StaticClassCastFlagsPrivate(const FFloatProperty *) {
	return FFloatProperty::StaticClassCastFlagsPrivate();
}

uint64 _dasUnreal_static_425_StaticClassCastFlags(const FFloatProperty *) {
	return FFloatProperty::StaticClassCastFlags();
}

FFieldClass * _dasUnreal_static_426_StaticClass(const FDoubleProperty *) {
	return FDoubleProperty::StaticClass();
}

FField * _dasUnreal_static_427_Construct(const FDoubleProperty *,const FFieldVariant & InOwner,const FName & InName,EObjectFlags InObjectFlags) {
	return FDoubleProperty::Construct(InOwner,InName,InObjectFlags);
}

uint64 _dasUnreal_static_428_StaticClassCastFlagsPrivate(const FDoubleProperty *) {
	return FDoubleProperty::StaticClassCastFlagsPrivate();
}

uint64 _dasUnreal_static_429_StaticClassCastFlags(const FDoubleProperty *) {
	return FDoubleProperty::StaticClassCastFlags();
}

FFieldClass * _dasUnreal_static_430_StaticClass(const FBoolProperty *) {
	return FBoolProperty::StaticClass();
}

FField * _dasUnreal_static_431_Construct(const FBoolProperty *,const FFieldVariant & InOwner,const FName & InName,EObjectFlags InObjectFlags) {
	return FBoolProperty::Construct(InOwner,InName,InObjectFlags);
}

uint64 _dasUnreal_static_432_StaticClassCastFlagsPrivate(const FBoolProperty *) {
	return FBoolProperty::StaticClassCastFlagsPrivate();
}

uint64 _dasUnreal_static_433_StaticClassCastFlags(const FBoolProperty *) {
	return FBoolProperty::StaticClassCastFlags();
}

void _dasUnreal_virtual_434_Serialize(FBoolProperty & self,FArchive & Ar) {
	self.Serialize(Ar);
}

void _dasUnreal_virtual_435_PostDuplicate(FBoolProperty & self,const FField & InField) {
	self.PostDuplicate(InField);
}

FString _dasUnreal_virtual_436_GetCPPType(const FBoolProperty & self,FString * ExtendedTypeText,unsigned int CPPExportFlags) {
	return self.GetCPPType(ExtendedTypeText,CPPExportFlags);
}

FString _dasUnreal_virtual_437_GetCPPMacroType(const FBoolProperty & self,FString & ExtendedTypeText) {
	return self.GetCPPMacroType(ExtendedTypeText);
}

FString _dasUnreal_virtual_438_GetCPPTypeForwardDeclaration(const FBoolProperty & self) {
	return self.GetCPPTypeForwardDeclaration();
}

void _dasUnreal_virtual_439_LinkInternal(FBoolProperty & self,FArchive & Ar) {
	self.LinkInternal(Ar);
}

bool _dasUnreal_virtual_440_Identical(const FBoolProperty & self,const void * A,const void * B,unsigned int PortFlags) {
	return self.Identical(A,B,PortFlags);
}

void _dasUnreal_virtual_441_CopyValuesInternal(const FBoolProperty & self,void * Dest,const void * Src,int Count) {
	self.CopyValuesInternal(Dest,Src,Count);
}

void _dasUnreal_virtual_442_ClearValueInternal(const FBoolProperty & self,void * Data) {
	self.ClearValueInternal(Data);
}

void _dasUnreal_virtual_443_InitializeValueInternal(const FBoolProperty & self,void * Dest) {
	self.InitializeValueInternal(Dest);
}

int _dasUnreal_virtual_444_GetMinAlignment(const FBoolProperty & self) {
	return self.GetMinAlignment();
}

void _dasUnreal_virtual_445_AppendSchemaHash(const FBoolProperty & self,FBlake3 & Builder,bool bSkipEditorOnly) {
	self.AppendSchemaHash(Builder,bSkipEditorOnly);
}

bool _dasUnreal_static_446_GetDefaultPropertyValue(const FBoolProperty *) {
	return FBoolProperty::GetDefaultPropertyValue();
}

unsigned int _dasUnreal_virtual_447_GetValueTypeHashInternal(const FBoolProperty & self,const void * Src) {
	return self.GetValueTypeHashInternal(Src);
}

FFieldClass * _dasUnreal_static_448_StaticClass(const FObjectPropertyBase *) {
	return FObjectPropertyBase::StaticClass();
}

FField * _dasUnreal_static_449_Construct(const FObjectPropertyBase *,const FFieldVariant & InOwner,const FName & InName,EObjectFlags InObjectFlags) {
	return FObjectPropertyBase::Construct(InOwner,InName,InObjectFlags);
}

uint64 _dasUnreal_static_450_StaticClassCastFlagsPrivate(const FObjectPropertyBase *) {
	return FObjectPropertyBase::StaticClassCastFlagsPrivate();
}

uint64 _dasUnreal_static_451_StaticClassCastFlags(const FObjectPropertyBase *) {
	return FObjectPropertyBase::StaticClassCastFlags();
}

void _dasUnreal_virtual_452_Serialize(FObjectPropertyBase & self,FArchive & Ar) {
	self.Serialize(Ar);
}

void _dasUnreal_virtual_453_AddReferencedObjects(FObjectPropertyBase & self,FReferenceCollector & Collector) {
	self.AddReferencedObjects(Collector);
}

void _dasUnreal_virtual_454_BeginDestroy(FObjectPropertyBase & self) {
	self.BeginDestroy();
}

void _dasUnreal_virtual_455_PostDuplicate(FObjectPropertyBase & self,const FField & InField) {
	self.PostDuplicate(InField);
}

bool _dasUnreal_virtual_456_Identical(const FObjectPropertyBase & self,const void * A,const void * B,unsigned int PortFlags) {
	return self.Identical(A,B,PortFlags);
}

bool _dasUnreal_virtual_457_SupportsNetSharedSerialization(const FObjectPropertyBase & self) {
	return self.SupportsNetSharedSerialization();
}

FName _dasUnreal_virtual_458_GetID(const FObjectPropertyBase & self) {
	return self.GetID();
}

bool _dasUnreal_virtual_459_SameType(const FObjectPropertyBase & self,const FProperty * Other) {
	return self.SameType(Other);
}

FString _dasUnreal_virtual_460_GetCPPTypeCustom(const FObjectPropertyBase & self,FString * ExtendedTypeText,unsigned int CPPExportFlags,const FString & InnerNativeTypeName) {
	return self.GetCPPTypeCustom(ExtendedTypeText,CPPExportFlags,InnerNativeTypeName);
}

bool _dasUnreal_static_461_StaticIdentical(const FObjectPropertyBase *,UObject * A,UObject * B,unsigned int PortFlags) {
	return FObjectPropertyBase::StaticIdentical(A,B,PortFlags);
}

UObject * _dasUnreal_virtual_462_LoadObjectPropertyValue(const FObjectPropertyBase & self,const void * PropertyValueAddress) {
	return self.LoadObjectPropertyValue(PropertyValueAddress);
}

UObject * _dasUnreal_virtual_463_GetObjectPropertyValue(const FObjectPropertyBase & self,const void * PropertyValueAddress) {
	return self.GetObjectPropertyValue(PropertyValueAddress);
}

UObject * _dasUnreal_virtual_464_GetObjectPropertyValue_InContainer(const FObjectPropertyBase & self,const void * ContainerAddress,int ArrayIndex) {
	return self.GetObjectPropertyValue_InContainer(ContainerAddress,ArrayIndex);
}

void _dasUnreal_virtual_465_SetObjectPropertyValue(const FObjectPropertyBase & self,void * PropertyValueAddress,UObject * Value) {
	self.SetObjectPropertyValue(PropertyValueAddress,Value);
}

void _dasUnreal_virtual_466_SetObjectPropertyValue_InContainer(const FObjectPropertyBase & self,void * ContainerAddress,UObject * Value,int ArrayIndex) {
	self.SetObjectPropertyValue_InContainer(ContainerAddress,Value,ArrayIndex);
}

FFieldClass * _dasUnreal_static_467_StaticClass(const FStrProperty *) {
	return FStrProperty::StaticClass();
}

FField * _dasUnreal_static_468_Construct(const FStrProperty *,const FFieldVariant & InOwner,const FName & InName,EObjectFlags InObjectFlags) {
	return FStrProperty::Construct(InOwner,InName,InObjectFlags);
}

uint64 _dasUnreal_static_469_StaticClassCastFlagsPrivate(const FStrProperty *) {
	return FStrProperty::StaticClassCastFlagsPrivate();
}

uint64 _dasUnreal_static_470_StaticClassCastFlags(const FStrProperty *) {
	return FStrProperty::StaticClassCastFlags();
}

FString _dasUnreal_virtual_471_GetCPPTypeForwardDeclaration(const FStrProperty & self) {
	return self.GetCPPTypeForwardDeclaration();
}

unsigned int _dasUnreal_virtual_472_GetValueTypeHashInternal(const FStrProperty & self,const void * Src) {
	return self.GetValueTypeHashInternal(Src);
}

FFieldClass * _dasUnreal_static_473_StaticClass(const FArrayProperty *) {
	return FArrayProperty::StaticClass();
}

FField * _dasUnreal_static_474_Construct(const FArrayProperty *,const FFieldVariant & InOwner,const FName & InName,EObjectFlags InObjectFlags) {
	return FArrayProperty::Construct(InOwner,InName,InObjectFlags);
}

uint64 _dasUnreal_static_475_StaticClassCastFlagsPrivate(const FArrayProperty *) {
	return FArrayProperty::StaticClassCastFlagsPrivate();
}

uint64 _dasUnreal_static_476_StaticClassCastFlags(const FArrayProperty *) {
	return FArrayProperty::StaticClassCastFlags();
}

void _dasUnreal_virtual_477_Serialize(FArrayProperty & self,FArchive & Ar) {
	self.Serialize(Ar);
}

void _dasUnreal_virtual_478_AddReferencedObjects(FArrayProperty & self,FReferenceCollector & Collector) {
	self.AddReferencedObjects(Collector);
}

void _dasUnreal_virtual_479_PostDuplicate(FArrayProperty & self,const FField & InField) {
	self.PostDuplicate(InField);
}

void _dasUnreal_virtual_480_AddCppProperty(FArrayProperty & self,FProperty * Property) {
	self.AddCppProperty(Property);
}

FField * _dasUnreal_virtual_481_GetInnerFieldByName(FArrayProperty & self,const FName & InName) {
	return self.GetInnerFieldByName(InName);
}

FString _dasUnreal_virtual_482_GetCPPMacroType(const FArrayProperty & self,FString & ExtendedTypeText) {
	return self.GetCPPMacroType(ExtendedTypeText);
}

FString _dasUnreal_virtual_483_GetCPPType(const FArrayProperty & self,FString * ExtendedTypeText,unsigned int CPPExportFlags) {
	return self.GetCPPType(ExtendedTypeText,CPPExportFlags);
}

FString _dasUnreal_virtual_484_GetCPPTypeForwardDeclaration(const FArrayProperty & self) {
	return self.GetCPPTypeForwardDeclaration();
}

void _dasUnreal_virtual_485_LinkInternal(FArrayProperty & self,FArchive & Ar) {
	self.LinkInternal(Ar);
}

bool _dasUnreal_virtual_486_Identical(const FArrayProperty & self,const void * A,const void * B,unsigned int PortFlags) {
	return self.Identical(A,B,PortFlags);
}

void _dasUnreal_virtual_487_InitializeValueInternal(const FArrayProperty & self,void * Dest) {
	self.InitializeValueInternal(Dest);
}

void _dasUnreal_virtual_488_CopyValuesInternal(const FArrayProperty & self,void * Dest,const void * Src,int Count) {
	self.CopyValuesInternal(Dest,Src,Count);
}

void _dasUnreal_virtual_489_ClearValueInternal(const FArrayProperty & self,void * Data) {
	self.ClearValueInternal(Data);
}

void _dasUnreal_virtual_490_DestroyValueInternal(const FArrayProperty & self,void * Dest) {
	self.DestroyValueInternal(Dest);
}

bool _dasUnreal_virtual_491_PassCPPArgsByRef(const FArrayProperty & self) {
	return self.PassCPPArgsByRef();
}

bool _dasUnreal_virtual_492_SameType(const FArrayProperty & self,const FProperty * Other) {
	return self.SameType(Other);
}

int _dasUnreal_virtual_493_GetMinAlignment(const FArrayProperty & self) {
	return self.GetMinAlignment();
}

void * _dasUnreal_virtual_494_GetValueAddressAtIndex_Direct(const FArrayProperty & self,const FProperty * Inner,void * InValueAddress,int Index) {
	return self.GetValueAddressAtIndex_Direct(Inner,InValueAddress,Index);
}

void _dasUnreal_static_495_ExportTextInnerItem(const FArrayProperty *,FString & ValueStr,const FProperty * Inner,const void * PropertyValue,int PropertySize,const void * DefaultValue,int DefaultSize,UObject * Parent,int PortFlags,UObject * ExportRootScope) {
	FArrayProperty::ExportTextInnerItem(ValueStr,Inner,PropertyValue,PropertySize,DefaultValue,DefaultSize,Parent,PortFlags,ExportRootScope);
}

void _dasUnreal_virtual_496_AppendSchemaHash(const FArrayProperty & self,FBlake3 & Builder,bool bSkipEditorOnly) {
	self.AppendSchemaHash(Builder,bSkipEditorOnly);
}

FFieldClass * _dasUnreal_static_497_StaticClass(const FMapProperty *) {
	return FMapProperty::StaticClass();
}

FField * _dasUnreal_static_498_Construct(const FMapProperty *,const FFieldVariant & InOwner,const FName & InName,EObjectFlags InObjectFlags) {
	return FMapProperty::Construct(InOwner,InName,InObjectFlags);
}

uint64 _dasUnreal_static_499_StaticClassCastFlagsPrivate(const FMapProperty *) {
	return FMapProperty::StaticClassCastFlagsPrivate();
}

uint64 _dasUnreal_static_500_StaticClassCastFlags(const FMapProperty *) {
	return FMapProperty::StaticClassCastFlags();
}

void _dasUnreal_virtual_501_Serialize(FMapProperty & self,FArchive & Ar) {
	self.Serialize(Ar);
}

void _dasUnreal_virtual_502_AddReferencedObjects(FMapProperty & self,FReferenceCollector & Collector) {
	self.AddReferencedObjects(Collector);
}

void _dasUnreal_virtual_503_PostDuplicate(FMapProperty & self,const FField & InField) {
	self.PostDuplicate(InField);
}

FField * _dasUnreal_virtual_504_GetInnerFieldByName(FMapProperty & self,const FName & InName) {
	return self.GetInnerFieldByName(InName);
}

void _dasUnreal_virtual_505_AddCppProperty(FMapProperty & self,FProperty * Property) {
	self.AddCppProperty(Property);
}

FString _dasUnreal_virtual_506_GetCPPMacroType(const FMapProperty & self,FString & ExtendedTypeText) {
	return self.GetCPPMacroType(ExtendedTypeText);
}

FString _dasUnreal_virtual_507_GetCPPType(const FMapProperty & self,FString * ExtendedTypeText,unsigned int CPPExportFlags) {
	return self.GetCPPType(ExtendedTypeText,CPPExportFlags);
}

FString _dasUnreal_virtual_508_GetCPPTypeForwardDeclaration(const FMapProperty & self) {
	return self.GetCPPTypeForwardDeclaration();
}

void _dasUnreal_virtual_509_LinkInternal(FMapProperty & self,FArchive & Ar) {
	self.LinkInternal(Ar);
}

bool _dasUnreal_virtual_510_Identical(const FMapProperty & self,const void * A,const void * B,unsigned int PortFlags) {
	return self.Identical(A,B,PortFlags);
}

void _dasUnreal_virtual_511_InitializeValueInternal(const FMapProperty & self,void * Dest) {
	self.InitializeValueInternal(Dest);
}

void _dasUnreal_virtual_512_CopyValuesInternal(const FMapProperty & self,void * Dest,const void * Src,int Count) {
	self.CopyValuesInternal(Dest,Src,Count);
}

void _dasUnreal_virtual_513_ClearValueInternal(const FMapProperty & self,void * Data) {
	self.ClearValueInternal(Data);
}

void _dasUnreal_virtual_514_DestroyValueInternal(const FMapProperty & self,void * Dest) {
	self.DestroyValueInternal(Dest);
}

bool _dasUnreal_virtual_515_PassCPPArgsByRef(const FMapProperty & self) {
	return self.PassCPPArgsByRef();
}

bool _dasUnreal_virtual_516_SameType(const FMapProperty & self,const FProperty * Other) {
	return self.SameType(Other);
}

void * _dasUnreal_virtual_517_GetValueAddressAtIndex_Direct(const FMapProperty & self,const FProperty * Inner,void * InValueAddress,int Index) {
	return self.GetValueAddressAtIndex_Direct(Inner,InValueAddress,Index);
}

void _dasUnreal_virtual_518_AppendSchemaHash(const FMapProperty & self,FBlake3 & Builder,bool bSkipEditorOnly) {
	self.AppendSchemaHash(Builder,bSkipEditorOnly);
}

FFieldClass * _dasUnreal_static_519_StaticClass(const FSetProperty *) {
	return FSetProperty::StaticClass();
}

FField * _dasUnreal_static_520_Construct(const FSetProperty *,const FFieldVariant & InOwner,const FName & InName,EObjectFlags InObjectFlags) {
	return FSetProperty::Construct(InOwner,InName,InObjectFlags);
}

uint64 _dasUnreal_static_521_StaticClassCastFlagsPrivate(const FSetProperty *) {
	return FSetProperty::StaticClassCastFlagsPrivate();
}

uint64 _dasUnreal_static_522_StaticClassCastFlags(const FSetProperty *) {
	return FSetProperty::StaticClassCastFlags();
}

void _dasUnreal_virtual_523_Serialize(FSetProperty & self,FArchive & Ar) {
	self.Serialize(Ar);
}

void _dasUnreal_virtual_524_AddReferencedObjects(FSetProperty & self,FReferenceCollector & Collector) {
	self.AddReferencedObjects(Collector);
}

void _dasUnreal_virtual_525_PostDuplicate(FSetProperty & self,const FField & InField) {
	self.PostDuplicate(InField);
}

FField * _dasUnreal_virtual_526_GetInnerFieldByName(FSetProperty & self,const FName & InName) {
	return self.GetInnerFieldByName(InName);
}

void _dasUnreal_virtual_527_AddCppProperty(FSetProperty & self,FProperty * Property) {
	self.AddCppProperty(Property);
}

FString _dasUnreal_virtual_528_GetCPPMacroType(const FSetProperty & self,FString & ExtendedTypeText) {
	return self.GetCPPMacroType(ExtendedTypeText);
}

FString _dasUnreal_virtual_529_GetCPPType(const FSetProperty & self,FString * ExtendedTypeText,unsigned int CPPExportFlags) {
	return self.GetCPPType(ExtendedTypeText,CPPExportFlags);
}

FString _dasUnreal_virtual_530_GetCPPTypeForwardDeclaration(const FSetProperty & self) {
	return self.GetCPPTypeForwardDeclaration();
}

void _dasUnreal_virtual_531_LinkInternal(FSetProperty & self,FArchive & Ar) {
	self.LinkInternal(Ar);
}

bool _dasUnreal_virtual_532_Identical(const FSetProperty & self,const void * A,const void * B,unsigned int PortFlags) {
	return self.Identical(A,B,PortFlags);
}

void _dasUnreal_virtual_533_CopyValuesInternal(const FSetProperty & self,void * Dest,const void * Src,int Count) {
	self.CopyValuesInternal(Dest,Src,Count);
}

void _dasUnreal_virtual_534_ClearValueInternal(const FSetProperty & self,void * Data) {
	self.ClearValueInternal(Data);
}

void _dasUnreal_virtual_535_DestroyValueInternal(const FSetProperty & self,void * Dest) {
	self.DestroyValueInternal(Dest);
}

bool _dasUnreal_virtual_536_PassCPPArgsByRef(const FSetProperty & self) {
	return self.PassCPPArgsByRef();
}

bool _dasUnreal_virtual_537_SameType(const FSetProperty & self,const FProperty * Other) {
	return self.SameType(Other);
}

void * _dasUnreal_virtual_538_GetValueAddressAtIndex_Direct(const FSetProperty & self,const FProperty * Inner,void * InValueAddress,int Index) {
	return self.GetValueAddressAtIndex_Direct(Inner,InValueAddress,Index);
}

void _dasUnreal_virtual_539_AppendSchemaHash(const FSetProperty & self,FBlake3 & Builder,bool bSkipEditorOnly) {
	self.AppendSchemaHash(Builder,bSkipEditorOnly);
}

FFieldClass * _dasUnreal_static_540_StaticClass(const FStructProperty *) {
	return FStructProperty::StaticClass();
}

FField * _dasUnreal_static_541_Construct(const FStructProperty *,const FFieldVariant & InOwner,const FName & InName,EObjectFlags InObjectFlags) {
	return FStructProperty::Construct(InOwner,InName,InObjectFlags);
}

uint64 _dasUnreal_static_542_StaticClassCastFlagsPrivate(const FStructProperty *) {
	return FStructProperty::StaticClassCastFlagsPrivate();
}

uint64 _dasUnreal_static_543_StaticClassCastFlags(const FStructProperty *) {
	return FStructProperty::StaticClassCastFlags();
}

void _dasUnreal_virtual_544_Serialize(FStructProperty & self,FArchive & Ar) {
	self.Serialize(Ar);
}

void _dasUnreal_virtual_545_AddReferencedObjects(FStructProperty & self,FReferenceCollector & Collector) {
	self.AddReferencedObjects(Collector);
}

void _dasUnreal_virtual_546_PostDuplicate(FStructProperty & self,const FField & InField) {
	self.PostDuplicate(InField);
}

FString _dasUnreal_virtual_547_GetCPPMacroType(const FStructProperty & self,FString & ExtendedTypeText) {
	return self.GetCPPMacroType(ExtendedTypeText);
}

FString _dasUnreal_virtual_548_GetCPPType(const FStructProperty & self,FString * ExtendedTypeText,unsigned int CPPExportFlags) {
	return self.GetCPPType(ExtendedTypeText,CPPExportFlags);
}

FString _dasUnreal_virtual_549_GetCPPTypeForwardDeclaration(const FStructProperty & self) {
	return self.GetCPPTypeForwardDeclaration();
}

void _dasUnreal_virtual_550_LinkInternal(FStructProperty & self,FArchive & Ar) {
	self.LinkInternal(Ar);
}

bool _dasUnreal_virtual_551_Identical(const FStructProperty & self,const void * A,const void * B,unsigned int PortFlags) {
	return self.Identical(A,B,PortFlags);
}

bool _dasUnreal_virtual_552_SupportsNetSharedSerialization(const FStructProperty & self) {
	return self.SupportsNetSharedSerialization();
}

void _dasUnreal_virtual_553_CopyValuesInternal(const FStructProperty & self,void * Dest,const void * Src,int Count) {
	self.CopyValuesInternal(Dest,Src,Count);
}

void _dasUnreal_virtual_554_ClearValueInternal(const FStructProperty & self,void * Data) {
	self.ClearValueInternal(Data);
}

void _dasUnreal_virtual_555_DestroyValueInternal(const FStructProperty & self,void * Dest) {
	self.DestroyValueInternal(Dest);
}

void _dasUnreal_virtual_556_InitializeValueInternal(const FStructProperty & self,void * Dest) {
	self.InitializeValueInternal(Dest);
}

int _dasUnreal_virtual_557_GetMinAlignment(const FStructProperty & self) {
	return self.GetMinAlignment();
}

bool _dasUnreal_virtual_558_SameType(const FStructProperty & self,const FProperty * Other) {
	return self.SameType(Other);
}

void _dasUnreal_virtual_559_AppendSchemaHash(const FStructProperty & self,FBlake3 & Builder,bool bSkipEditorOnly) {
	self.AppendSchemaHash(Builder,bSkipEditorOnly);
}

FProperty * _dasUnreal_static_560_GetNextPropertyAndAdvance(const FCustomPropertyListNode *,const FCustomPropertyListNode *& Node) {
	return FCustomPropertyListNode::GetNextPropertyAndAdvance(Node);
}

UClass * _dasUnreal_static_561_StaticClass(const UPropertyWrapper *) {
	return UPropertyWrapper::StaticClass();
}

const wchar_t * _dasUnreal_static_562_StaticPackage(const UPropertyWrapper *) {
	return UPropertyWrapper::StaticPackage();
}

void _dasUnreal_static_563_StaticRegisterNativesUPropertyWrapper(const UPropertyWrapper *) {
	UPropertyWrapper::StaticRegisterNativesUPropertyWrapper();
}

void _dasUnreal_virtual_564_Serialize(FFrame & self,const wchar_t * V,ELogVerbosity::Type Verbosity,const FName & Category) {
	self.Serialize(V,Verbosity,Category);
}

void _dasUnreal_static_565_KismetExecutionMessage(const FFrame *,const wchar_t * Message,ELogVerbosity::Type Verbosity,FName WarningId) {
	FFrame::KismetExecutionMessage(Message,Verbosity,WarningId);
}

FFrame * _dasUnreal_static_566_PushThreadLocalTopStackFrame(const FFrame *,FFrame * NewTopStackFrame) {
	return FFrame::PushThreadLocalTopStackFrame(NewTopStackFrame);
}

void _dasUnreal_static_567_PopThreadLocalTopStackFrame(const FFrame *,FFrame * NewTopStackFrame) {
	FFrame::PopThreadLocalTopStackFrame(NewTopStackFrame);
}

FFrame * _dasUnreal_static_568_GetThreadLocalTopStackFrame(const FFrame *) {
	return FFrame::GetThreadLocalTopStackFrame();
}

UClass * _dasUnreal_static_569_StaticClass(const UBlueprintFunctionLibrary *) {
	return UBlueprintFunctionLibrary::StaticClass();
}

const wchar_t * _dasUnreal_static_570_StaticPackage(const UBlueprintFunctionLibrary *) {
	return UBlueprintFunctionLibrary::StaticPackage();
}

int _dasUnreal_virtual_571_GetFunctionCallspace(UBlueprintFunctionLibrary & self,UFunction * Function,FFrame * Stack) {
	return self.GetFunctionCallspace(Function,Stack);
}

UClass * _dasUnreal_static_572_StaticClass(const UKismetArrayLibrary *) {
	return UKismetArrayLibrary::StaticClass();
}

const wchar_t * _dasUnreal_static_573_StaticPackage(const UKismetArrayLibrary *) {
	return UKismetArrayLibrary::StaticPackage();
}

int _dasUnreal_static_574_GenericArray_Add(const UKismetArrayLibrary *,void * TargetArray,const FArrayProperty * ArrayProp,const void * NewItem) {
	return UKismetArrayLibrary::GenericArray_Add(TargetArray,ArrayProp,NewItem);
}

int _dasUnreal_static_575_GenericArray_AddUnique(const UKismetArrayLibrary *,void * TargetArray,const FArrayProperty * ArrayProp,const void * NewItem) {
	return UKismetArrayLibrary::GenericArray_AddUnique(TargetArray,ArrayProp,NewItem);
}

void _dasUnreal_static_576_GenericArray_Shuffle(const UKismetArrayLibrary *,void * TargetArray,const FArrayProperty * ArrayProp) {
	UKismetArrayLibrary::GenericArray_Shuffle(TargetArray,ArrayProp);
}

bool _dasUnreal_static_577_GenericArray_Identical(const UKismetArrayLibrary *,void * ArrayA,const FArrayProperty * ArrayAProp,void * ArrayB,const FArrayProperty * ArrayBProperty) {
	return UKismetArrayLibrary::GenericArray_Identical(ArrayA,ArrayAProp,ArrayB,ArrayBProperty);
}

void _dasUnreal_static_578_GenericArray_Append(const UKismetArrayLibrary *,void * TargetArray,const FArrayProperty * TargetArrayProp,void * SourceArray,const FArrayProperty * SourceArrayProperty) {
	UKismetArrayLibrary::GenericArray_Append(TargetArray,TargetArrayProp,SourceArray,SourceArrayProperty);
}

void _dasUnreal_static_579_GenericArray_Insert(const UKismetArrayLibrary *,void * TargetArray,const FArrayProperty * ArrayProp,const void * NewItem,int Index) {
	UKismetArrayLibrary::GenericArray_Insert(TargetArray,ArrayProp,NewItem,Index);
}

void _dasUnreal_static_580_GenericArray_Remove(const UKismetArrayLibrary *,void * TargetArray,const FArrayProperty * ArrayProp,int IndexToRemove) {
	UKismetArrayLibrary::GenericArray_Remove(TargetArray,ArrayProp,IndexToRemove);
}

bool _dasUnreal_static_581_GenericArray_RemoveItem(const UKismetArrayLibrary *,void * TargetArray,const FArrayProperty * ArrayProp,const void * Item) {
	return UKismetArrayLibrary::GenericArray_RemoveItem(TargetArray,ArrayProp,Item);
}

void _dasUnreal_static_582_GenericArray_Clear(const UKismetArrayLibrary *,void * TargetArray,const FArrayProperty * ArrayProp) {
	UKismetArrayLibrary::GenericArray_Clear(TargetArray,ArrayProp);
}

void _dasUnreal_static_583_GenericArray_Resize(const UKismetArrayLibrary *,void * TargetArray,const FArrayProperty * ArrayProp,int Size) {
	UKismetArrayLibrary::GenericArray_Resize(TargetArray,ArrayProp,Size);
}

void _dasUnreal_static_584_GenericArray_Reverse(const UKismetArrayLibrary *,void * TargetArray,const FArrayProperty * ArrayProp) {
	UKismetArrayLibrary::GenericArray_Reverse(TargetArray,ArrayProp);
}

int _dasUnreal_static_585_GenericArray_Length(const UKismetArrayLibrary *,const void * TargetArray,const FArrayProperty * ArrayProp) {
	return UKismetArrayLibrary::GenericArray_Length(TargetArray,ArrayProp);
}

bool _dasUnreal_static_586_GenericArray_IsEmpty(const UKismetArrayLibrary *,const void * TargetArray,const FArrayProperty * ArrayProp) {
	return UKismetArrayLibrary::GenericArray_IsEmpty(TargetArray,ArrayProp);
}

bool _dasUnreal_static_587_GenericArray_IsNotEmpty(const UKismetArrayLibrary *,const void * TargetArray,const FArrayProperty * ArrayProp) {
	return UKismetArrayLibrary::GenericArray_IsNotEmpty(TargetArray,ArrayProp);
}

int _dasUnreal_static_588_GenericArray_LastIndex(const UKismetArrayLibrary *,const void * TargetArray,const FArrayProperty * ArrayProp) {
	return UKismetArrayLibrary::GenericArray_LastIndex(TargetArray,ArrayProp);
}

void _dasUnreal_static_589_GenericArray_Get(const UKismetArrayLibrary *,void * TargetArray,const FArrayProperty * ArrayProp,int Index,void * Item) {
	UKismetArrayLibrary::GenericArray_Get(TargetArray,ArrayProp,Index,Item);
}

void _dasUnreal_static_590_GenericArray_Set(const UKismetArrayLibrary *,void * TargetArray,const FArrayProperty * ArrayProp,int Index,const void * NewItem,bool bSizeToFit) {
	UKismetArrayLibrary::GenericArray_Set(TargetArray,ArrayProp,Index,NewItem,bSizeToFit);
}

void _dasUnreal_static_591_GenericArray_Swap(const UKismetArrayLibrary *,const void * TargetArray,const FArrayProperty * ArrayProp,int First,int Second) {
	UKismetArrayLibrary::GenericArray_Swap(TargetArray,ArrayProp,First,Second);
}

int _dasUnreal_static_592_GenericArray_Find(const UKismetArrayLibrary *,const void * TargetArray,const FArrayProperty * ArrayProperty,const void * ItemToFind) {
	return UKismetArrayLibrary::GenericArray_Find(TargetArray,ArrayProperty,ItemToFind);
}

void _dasUnreal_static_593_GenericArray_SetArrayPropertyByName(const UKismetArrayLibrary *,UObject * OwnerObject,FName ArrayPropertyName,const void * SrcArrayAddr) {
	UKismetArrayLibrary::GenericArray_SetArrayPropertyByName(OwnerObject,ArrayPropertyName,SrcArrayAddr);
}

void _dasUnreal_static_594_GenericArray_Random(const UKismetArrayLibrary *,void * TargetArray,const FArrayProperty * ArrayProp,void * OutItem,int * OutIndex) {
	UKismetArrayLibrary::GenericArray_Random(TargetArray,ArrayProp,OutItem,OutIndex);
}

bool _dasUnreal_static_595_GenericArray_IsValidIndex(const UKismetArrayLibrary *,const void * TargetArray,const FArrayProperty * ArrayProp,int IndexToTest) {
	return UKismetArrayLibrary::GenericArray_IsValidIndex(TargetArray,ArrayProp,IndexToTest);
}

