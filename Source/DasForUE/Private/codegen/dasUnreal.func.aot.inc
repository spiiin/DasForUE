// this file is generated via daScript automatic C++ binder
// all user modifications will be lost after this file is re-generated

EVersionComparison _dasUnreal_static_0_GetNewest(const FEngineVersionBase *,const FEngineVersionBase & First,const FEngineVersionBase & Second,EVersionComponent * OutComponent) {
	return FEngineVersionBase::GetNewest(First,Second,OutComponent);
}

unsigned int _dasUnreal_static_1_EncodeLicenseeChangelist(const FEngineVersionBase *,unsigned int Changelist) {
	return FEngineVersionBase::EncodeLicenseeChangelist(Changelist);
}

FArchiveState & _dasUnreal_virtual_2_GetInnermostState(FArchiveState & self) {
	return self.GetInnermostState();
}

void _dasUnreal_virtual_3_CountBytes(FArchiveState & self,SIZE_T InNum,SIZE_T InMax) {
	self.CountBytes(InNum,InMax);
}

FString _dasUnreal_virtual_4_GetArchiveName(const FArchiveState & self) {
	return self.GetArchiveName();
}

FLinker * _dasUnreal_virtual_5_GetLinker(FArchiveState & self) {
	return self.GetLinker();
}

int64 _dasUnreal_virtual_6_Tell(FArchiveState & self) {
	return self.Tell();
}

int64 _dasUnreal_virtual_7_TotalSize(FArchiveState & self) {
	return self.TotalSize();
}

bool _dasUnreal_virtual_8_AtEnd(FArchiveState & self) {
	return self.AtEnd();
}

UObject * _dasUnreal_virtual_9_GetArchetypeFromLoader(FArchiveState & self,const UObject * Obj) {
	return self.GetArchetypeFromLoader(Obj);
}

unsigned int _dasUnreal_virtual_10_EngineNetVer(const FArchiveState & self) {
	return self.EngineNetVer();
}

unsigned int _dasUnreal_virtual_11_GameNetVer(const FArchiveState & self) {
	return self.GameNetVer();
}

void _dasUnreal_virtual_12_ResetCustomVersions(FArchiveState & self) {
	self.ResetCustomVersions();
}

void _dasUnreal_virtual_13_SetFilterEditorOnly(FArchiveState & self,bool InFilterEditorOnly) {
	self.SetFilterEditorOnly(InFilterEditorOnly);
}

bool _dasUnreal_virtual_14_UseToResolveEnumerators(const FArchiveState & self) {
	return self.UseToResolveEnumerators();
}

bool _dasUnreal_virtual_15_ShouldSkipProperty(const FArchiveState & self,const FProperty * InProperty) {
	return self.ShouldSkipProperty(InProperty);
}

void _dasUnreal_virtual_16_SetSerializedProperty(FArchiveState & self,FProperty * InProperty) {
	self.SetSerializedProperty(InProperty);
}

void _dasUnreal_virtual_17_SetSerializedPropertyChain(FArchiveState & self,const FArchiveSerializedPropertyChain * InSerializedPropertyChain,FProperty * InSerializedPropertyOverride) {
	self.SetSerializedPropertyChain(InSerializedPropertyChain,InSerializedPropertyOverride);
}

bool _dasUnreal_virtual_18_IsEditorOnlyPropertyOnTheStack(const FArchiveState & self) {
	return self.IsEditorOnlyPropertyOnTheStack();
}

void _dasUnreal_virtual_19_SetSerializeContext(FArchiveState & self,FUObjectSerializeContext * InLoadContext) {
	self.SetSerializeContext(InLoadContext);
}

FUObjectSerializeContext * _dasUnreal_virtual_20_GetSerializeContext(FArchiveState & self) {
	return self.GetSerializeContext();
}

void _dasUnreal_virtual_21_SetLocalizationNamespace(FArchiveState & self,const FString & InLocalizationNamespace) {
	self.SetLocalizationNamespace(InLocalizationNamespace);
}

FString _dasUnreal_virtual_22_GetLocalizationNamespace(const FArchiveState & self) {
	return self.GetLocalizationNamespace();
}

void _dasUnreal_virtual_23_Reset(FArchiveState & self) {
	self.Reset();
}

void _dasUnreal_virtual_24_SetIsLoading(FArchiveState & self,bool bInIsLoading) {
	self.SetIsLoading(bInIsLoading);
}

void _dasUnreal_virtual_25_SetIsLoadingFromCookedPackage(FArchiveState & self,bool bInIsLoadingFromCookedPackage) {
	self.SetIsLoadingFromCookedPackage(bInIsLoadingFromCookedPackage);
}

void _dasUnreal_virtual_26_SetIsSaving(FArchiveState & self,bool bInIsSaving) {
	self.SetIsSaving(bInIsSaving);
}

void _dasUnreal_virtual_27_SetIsTransacting(FArchiveState & self,bool bInIsTransacting) {
	self.SetIsTransacting(bInIsTransacting);
}

void _dasUnreal_virtual_28_SetIsTextFormat(FArchiveState & self,bool bInIsTextFormat) {
	self.SetIsTextFormat(bInIsTextFormat);
}

void _dasUnreal_virtual_29_SetWantBinaryPropertySerialization(FArchiveState & self,bool bInWantBinaryPropertySerialization) {
	self.SetWantBinaryPropertySerialization(bInWantBinaryPropertySerialization);
}

void _dasUnreal_virtual_30_SetUseUnversionedPropertySerialization(FArchiveState & self,bool bInUseUnversioned) {
	self.SetUseUnversionedPropertySerialization(bInUseUnversioned);
}

void _dasUnreal_virtual_31_SetForceUnicode(FArchiveState & self,bool bInForceUnicode) {
	self.SetForceUnicode(bInForceUnicode);
}

void _dasUnreal_virtual_32_SetIsPersistent(FArchiveState & self,bool bInIsPersistent) {
	self.SetIsPersistent(bInIsPersistent);
}

void _dasUnreal_virtual_33_SetLicenseeUEVer(FArchiveState & self,int InVer) {
	self.SetLicenseeUEVer(InVer);
}

void _dasUnreal_virtual_34_SetEngineVer(FArchiveState & self,const FEngineVersionBase & InVer) {
	self.SetEngineVer(InVer);
}

void _dasUnreal_virtual_35_SetEngineNetVer(FArchiveState & self,const unsigned int InEngineNetVer) {
	self.SetEngineNetVer(InEngineNetVer);
}

void _dasUnreal_virtual_36_SetGameNetVer(FArchiveState & self,const unsigned int InGameNetVer) {
	self.SetGameNetVer(InGameNetVer);
}

void _dasUnreal_virtual_37_ForceBlueprintFinalization(FArchive & self) {
	self.ForceBlueprintFinalization();
}

void _dasUnreal_virtual_38_Serialize(FArchive & self,void * V,int64 Length) {
	self.Serialize(V,Length);
}

void _dasUnreal_virtual_39_SerializeBits(FArchive & self,void * V,int64 LengthBits) {
	self.SerializeBits(V,LengthBits);
}

void _dasUnreal_virtual_40_SerializeInt(FArchive & self,unsigned int & Value,unsigned int Max) {
	self.SerializeInt(Value,Max);
}

void _dasUnreal_virtual_41_SerializeIntPacked(FArchive & self,unsigned int & Value) {
	self.SerializeIntPacked(Value);
}

void _dasUnreal_virtual_42_SerializeIntPacked64(FArchive & self,unsigned long long & Value) {
	self.SerializeIntPacked64(Value);
}

void _dasUnreal_virtual_43_Preload(FArchive & self,UObject * Object) {
	self.Preload(Object);
}

void _dasUnreal_virtual_44_Seek(FArchive & self,int64 InPos) {
	self.Seek(InPos);
}

void _dasUnreal_virtual_45_AttachBulkData(FArchive & self,UE::Serialization::FEditorBulkData * BulkData) {
	self.AttachBulkData(BulkData);
}

void _dasUnreal_virtual_46_DetachBulkData(FArchive & self,UE::Serialization::FEditorBulkData * BulkData,bool bEnsureBulkDataIsLoaded) {
	self.DetachBulkData(BulkData,bEnsureBulkDataIsLoaded);
}

bool _dasUnreal_virtual_47_IsProxyOf(const FArchive & self,FArchive * InOther) {
	return self.IsProxyOf(InOther);
}

bool _dasUnreal_virtual_48_Precache(FArchive & self,int64 PrecacheOffset,int64 PrecacheSize) {
	return self.Precache(PrecacheOffset,PrecacheSize);
}

void _dasUnreal_virtual_49_FlushCache(FArchive & self) {
	self.FlushCache();
}

void _dasUnreal_virtual_50_Flush(FArchive & self) {
	self.Flush();
}

bool _dasUnreal_virtual_51_Close(FArchive & self) {
	return self.Close();
}

void _dasUnreal_virtual_52_MarkScriptSerializationStart(FArchive & self,const UObject * Obj) {
	self.MarkScriptSerializationStart(Obj);
}

void _dasUnreal_virtual_53_MarkScriptSerializationEnd(FArchive & self,const UObject * Obj) {
	self.MarkScriptSerializationEnd(Obj);
}

FArchive * _dasUnreal_virtual_54_GetCacheableArchive(FArchive & self) {
	return self.GetCacheableArchive();
}

void _dasUnreal_virtual_55_PushSerializedProperty(FArchive & self,FProperty * InProperty,const bool bIsEditorOnlyProperty) {
	self.PushSerializedProperty(InProperty,bIsEditorOnlyProperty);
}

void _dasUnreal_virtual_56_PopSerializedProperty(FArchive & self,FProperty * InProperty,const bool bIsEditorOnlyProperty) {
	self.PopSerializedProperty(InProperty,bIsEditorOnlyProperty);
}

void _dasUnreal_virtual_57_PushDebugDataString(FArchive & self,const FName & DebugData) {
	self.PushDebugDataString(DebugData);
}

void _dasUnreal_virtual_58_PopDebugDataString(FArchive & self) {
	self.PopDebugDataString();
}

void _dasUnreal_virtual_59_PushFileRegionType(FArchive & self,EFileRegionType Type) {
	self.PushFileRegionType(Type);
}

void _dasUnreal_virtual_60_PopFileRegionType(FArchive & self) {
	self.PopFileRegionType();
}

FString _dasUnreal_static_61_Chr(const FString *,wchar_t Ch) {
	return FString::Chr(Ch);
}

FString _dasUnreal_static_62_ChrN(const FString *,int NumCharacters,wchar_t Char) {
	return FString::ChrN(NumCharacters,Char);
}

FString _dasUnreal_static_63_FormatAsNumber(const FString *,int InNumber) {
	return FString::FormatAsNumber(InNumber);
}

FString _dasUnreal_static_64_FromInt(const FString *,int Num) {
	return FString::FromInt(Num);
}

FString _dasUnreal_static_65_FromBlob(const FString *,const unsigned char * SrcBuffer,const unsigned int SrcSize) {
	return FString::FromBlob(SrcBuffer,SrcSize);
}

bool _dasUnreal_static_66_ToBlob(const FString *,const FString & Source,unsigned char * DestBuffer,const unsigned int DestSize) {
	return FString::ToBlob(Source,DestBuffer,DestSize);
}

FString _dasUnreal_static_67_FromHexBlob(const FString *,const unsigned char * SrcBuffer,const unsigned int SrcSize) {
	return FString::FromHexBlob(SrcBuffer,SrcSize);
}

bool _dasUnreal_static_68_ToHexBlob(const FString *,const FString & Source,unsigned char * DestBuffer,const unsigned int DestSize) {
	return FString::ToHexBlob(Source,DestBuffer,DestSize);
}

FString _dasUnreal_static_69_SanitizeFloat(const FString *,double InFloat,const int InMinFractionalDigits) {
	return FString::SanitizeFloat(InFloat,InMinFractionalDigits);
}

void _dasUnreal_static_70_AssertIfInConstructor(const FObjectInitializer *,UObject * Outer,const wchar_t * ErrorMessage) {
	FObjectInitializer::AssertIfInConstructor(Outer,ErrorMessage);
}

FObjectInitializer & _dasUnreal_static_71_Get(const FObjectInitializer *) {
	return FObjectInitializer::Get();
}

void _dasUnreal_static_72_InitProperties(const FScriptIntegrationObjectHelper *,const FObjectInitializer & ObjectInitializer,UObject * Obj,UClass * DefaultsClass,UObject * DefaultData) {
	FScriptIntegrationObjectHelper::InitProperties(ObjectInitializer,Obj,DefaultsClass,DefaultData);
}

bool _dasUnreal_static_73_InitSubobjectProperties(const FScriptIntegrationObjectHelper *,const FObjectInitializer & ObjectInitializer) {
	return FScriptIntegrationObjectHelper::InitSubobjectProperties(ObjectInitializer);
}

void _dasUnreal_static_74_InstanceSubobjects(const FScriptIntegrationObjectHelper *,const FObjectInitializer & ObjectInitializer,UClass * Class,bool bNeedInstancing,bool bNeedSubobjectInstancing) {
	FScriptIntegrationObjectHelper::InstanceSubobjects(ObjectInitializer,Class,bNeedInstancing,bNeedSubobjectInstancing);
}

void _dasUnreal_static_75_PostConstructInitObject(const FScriptIntegrationObjectHelper *,FObjectInitializer & ObjectInitializer) {
	FScriptIntegrationObjectHelper::PostConstructInitObject(ObjectInitializer);
}

void _dasUnreal_virtual_76_AddStableReference(FReferenceCollector & self,UObject ** Object) {
	self.AddStableReference(Object);
}

bool _dasUnreal_virtual_77_NeedsPropertyReferencer(const FReferenceCollector & self) {
	return self.NeedsPropertyReferencer();
}

void _dasUnreal_virtual_78_AllowEliminatingReferences(FReferenceCollector & self,bool bAllow) {
	self.AllowEliminatingReferences(bAllow);
}

void _dasUnreal_virtual_79_SetSerializedProperty(FReferenceCollector & self,FProperty * Inproperty) {
	self.SetSerializedProperty(Inproperty);
}

FProperty * _dasUnreal_virtual_80_GetSerializedProperty(const FReferenceCollector & self) {
	return self.GetSerializedProperty();
}

bool _dasUnreal_virtual_81_MarkWeakObjectReferenceForClearing(FReferenceCollector & self,UObject ** WeakReference) {
	return self.MarkWeakObjectReferenceForClearing(WeakReference);
}

void _dasUnreal_virtual_82_SetIsProcessingNativeReferences(FReferenceCollector & self,bool bIsNative) {
	self.SetIsProcessingNativeReferences(bIsNative);
}

bool _dasUnreal_virtual_83_IsProcessingNativeReferences(const FReferenceCollector & self) {
	return self.IsProcessingNativeReferences();
}

bool _dasUnreal_virtual_84_NeedsInitialReferences(const FReferenceCollector & self) {
	return self.NeedsInitialReferences();
}

void _dasUnreal_virtual_85_FindReferences(FReferenceFinder & self,UObject * Object,UObject * ReferencingObject,FProperty * ReferencingProperty) {
	self.FindReferences(Object,ReferencingObject,ReferencingProperty);
}

bool _dasUnreal_virtual_86_IsIgnoringArchetypeRef(const FReferenceFinder & self) {
	return self.IsIgnoringArchetypeRef();
}

bool _dasUnreal_virtual_87_IsIgnoringTransient(const FReferenceFinder & self) {
	return self.IsIgnoringTransient();
}

void _dasUnreal_virtual_88_SetSerializedProperty(FReferenceFinder & self,FProperty * Inproperty) {
	self.SetSerializedProperty(Inproperty);
}

FProperty * _dasUnreal_virtual_89_GetSerializedProperty(const FReferenceFinder & self) {
	return self.GetSerializedProperty();
}

void _dasUnreal_static_90_BroadcastOnObjectModified(const FCoreUObjectDelegates *,UObject * Object) {
	FCoreUObjectDelegates::BroadcastOnObjectModified(Object);
}

FString _dasUnreal_static_91_FormatPathForAssetLog(const FAssetMsg *,const wchar_t * Path) {
	return FAssetMsg::FormatPathForAssetLog(Path);
}

FString _dasUnreal_static_92_FormatPathForAssetLog(const FAssetMsg *,const UObject * Object) {
	return FAssetMsg::FormatPathForAssetLog(Object);
}

FString _dasUnreal_static_93_GetAssetLogString(const FAssetMsg *,const wchar_t * Path,const FString & Message) {
	return FAssetMsg::GetAssetLogString(Path,Message);
}

FString _dasUnreal_static_94_GetAssetLogString(const FAssetMsg *,const UObject * Object,const FString & Message) {
	return FAssetMsg::GetAssetLogString(Object,Message);
}

FName _dasUnreal_virtual_95_GetFNameForStatID(const UObjectBase & self) {
	return self.GetFNameForStatID();
}

FString _dasUnreal_static_96_RemoveClassPrefix(const UObjectBase *,const wchar_t * ClassName) {
	return UObjectBase::RemoveClassPrefix(ClassName);
}

void _dasUnreal_static_97_PrefetchClass(const UObjectBase *,UObject * Object) {
	UObjectBase::PrefetchClass(Object);
}

void _dasUnreal_static_98_PrefetchOuter(const UObjectBase *,UObject * Object) {
	UObjectBase::PrefetchOuter(Object);
}

bool _dasUnreal_static_99_IsPendingKillEnabled(const UObjectBaseUtility *) {
	return UObjectBaseUtility::IsPendingKillEnabled();
}

EInternalObjectFlags _dasUnreal_static_100_FixGarbageOrPendingKillInternalObjectFlags(const UObjectBaseUtility *,const EInternalObjectFlags InFlags) {
	return UObjectBaseUtility::FixGarbageOrPendingKillInternalObjectFlags(InFlags);
}

bool _dasUnreal_virtual_101_CanBeClusterRoot(const UObjectBaseUtility & self) {
	return self.CanBeClusterRoot();
}

bool _dasUnreal_virtual_102_CanBeInCluster(const UObjectBaseUtility & self) {
	return self.CanBeInCluster();
}

void _dasUnreal_virtual_103_CreateCluster(UObjectBaseUtility & self) {
	self.CreateCluster();
}

void _dasUnreal_virtual_104_OnClusterMarkedAsPendingKill(UObjectBaseUtility & self) {
	self.OnClusterMarkedAsPendingKill();
}

UClass * _dasUnreal_static_105_StaticClass(const UObject *) {
	return UObject::StaticClass();
}

const wchar_t * _dasUnreal_static_106_StaticConfigName(const UObject *) {
	return UObject::StaticConfigName();
}

void _dasUnreal_static_107_StaticRegisterNativesUObject(const UObject *) {
	UObject::StaticRegisterNativesUObject();
}

void _dasUnreal_virtual_108_PostInitProperties(UObject & self) {
	self.PostInitProperties();
}

void _dasUnreal_virtual_109_PostReinitProperties(UObject & self) {
	self.PostReinitProperties();
}

void _dasUnreal_virtual_110_PostCDOContruct(UObject & self) {
	self.PostCDOContruct();
}

bool _dasUnreal_virtual_111_PreSaveRoot(UObject & self,const wchar_t * Filename) {
	return self.PreSaveRoot(Filename);
}

void _dasUnreal_virtual_112_PostSaveRoot(UObject & self,bool bCleanupIsRequired) {
	self.PostSaveRoot(bCleanupIsRequired);
}

bool _dasUnreal_virtual_113_Modify(UObject & self,bool bAlwaysMarkDirty) {
	return self.Modify(bAlwaysMarkDirty);
}

bool _dasUnreal_virtual_114_IsCapturingAsRootObjectForTransaction(const UObject & self) {
	return self.IsCapturingAsRootObjectForTransaction();
}

void _dasUnreal_virtual_115_LoadedFromAnotherClass(UObject & self,const FName & OldClassName) {
	self.LoadedFromAnotherClass(OldClassName);
}

bool _dasUnreal_virtual_116_ResolveSubobject(UObject & self,const wchar_t * SubObjectPath,UObject *& OutObject,bool bLoadIfExists) {
	return self.ResolveSubobject(SubObjectPath,OutObject,bLoadIfExists);
}

bool _dasUnreal_virtual_117_IsReadyForAsyncPostLoad(const UObject & self) {
	return self.IsReadyForAsyncPostLoad();
}

void _dasUnreal_virtual_118_PostLoad(UObject & self) {
	self.PostLoad();
}

void _dasUnreal_virtual_119_BeginDestroy(UObject & self) {
	self.BeginDestroy();
}

bool _dasUnreal_virtual_120_IsReadyForFinishDestroy(UObject & self) {
	return self.IsReadyForFinishDestroy();
}

void _dasUnreal_virtual_121_PostLinkerChange(UObject & self) {
	self.PostLinkerChange();
}

void _dasUnreal_virtual_122_FinishDestroy(UObject & self) {
	self.FinishDestroy();
}

void _dasUnreal_virtual_123_ShutdownAfterError(UObject & self) {
	self.ShutdownAfterError();
}

void _dasUnreal_virtual_124_PostInterpChange(UObject & self,FProperty * PropertyThatChanged) {
	self.PostInterpChange(PropertyThatChanged);
}

void _dasUnreal_virtual_125_PreEditUndo(UObject & self) {
	self.PreEditUndo();
}

void _dasUnreal_virtual_126_PostRename(UObject & self,UObject * OldOuter,const FName OldName) {
	self.PostRename(OldOuter,OldName);
}

bool _dasUnreal_virtual_127_NeedsLoadForClient(const UObject & self) {
	return self.NeedsLoadForClient();
}

bool _dasUnreal_virtual_128_NeedsLoadForServer(const UObject & self) {
	return self.NeedsLoadForServer();
}

bool _dasUnreal_virtual_129_NeedsLoadForEditorGame(const UObject & self) {
	return self.NeedsLoadForEditorGame();
}

bool _dasUnreal_virtual_130_IsEditorOnly(const UObject & self) {
	return self.IsEditorOnly();
}

bool _dasUnreal_virtual_131_HasNonEditorOnlyReferences(const UObject & self) {
	return self.HasNonEditorOnlyReferences();
}

bool _dasUnreal_virtual_132_IsPostLoadThreadSafe(const UObject & self) {
	return self.IsPostLoadThreadSafe();
}

bool _dasUnreal_virtual_133_IsDestructionThreadSafe(const UObject & self) {
	return self.IsDestructionThreadSafe();
}

bool _dasUnreal_static_134_CanCreateInCurrentContext(const UObject *,UObject * Template) {
	return UObject::CanCreateInCurrentContext(Template);
}

void _dasUnreal_virtual_135_PostEditImport(UObject & self) {
	self.PostEditImport();
}

void _dasUnreal_virtual_136_PostReloadConfig(UObject & self,FProperty * PropertyThatWasLoaded) {
	self.PostReloadConfig(PropertyThatWasLoaded);
}

bool _dasUnreal_virtual_137_Rename(UObject & self,const wchar_t * NewName,UObject * NewOuter,unsigned int Flags) {
	return self.Rename(NewName,NewOuter,Flags);
}

FString _dasUnreal_virtual_138_GetDesc(UObject & self) {
	return self.GetDesc();
}

void _dasUnreal_virtual_139_MoveDataToSparseClassDataStruct(const UObject & self) {
	self.MoveDataToSparseClassDataStruct();
}

bool _dasUnreal_virtual_140_ConformSparseClassDataStruct(const UObject & self,UScriptStruct * SourceSparseClassDataStruct,void * SourceSparseClassData) {
	return self.ConformSparseClassDataStruct(SourceSparseClassDataStruct,SourceSparseClassData);
}

bool _dasUnreal_virtual_141_ImplementsGetWorld(const UObject & self) {
	return self.ImplementsGetWorld();
}

FName _dasUnreal_virtual_142_GetExporterName(UObject & self) {
	return self.GetExporterName();
}

FRestoreForUObjectOverwrite * _dasUnreal_virtual_143_GetRestoreForUObjectOverwrite(UObject & self) {
	return self.GetRestoreForUObjectOverwrite();
}

bool _dasUnreal_virtual_144_AreNativePropertiesIdenticalTo(const UObject & self,UObject * Other) {
	return self.AreNativePropertiesIdenticalTo(Other);
}

const FName & _dasUnreal_static_145_SourceFileTagName(const UObject *) {
	return UObject::SourceFileTagName();
}

const FName & _dasUnreal_static_146_AssetVersePathTagName(const UObject *) {
	return UObject::AssetVersePathTagName();
}

bool _dasUnreal_virtual_147_IsAsset(const UObject & self) {
	return self.IsAsset();
}

bool _dasUnreal_virtual_148_IsLocalizedResource(const UObject & self) {
	return self.IsLocalizedResource();
}

bool _dasUnreal_virtual_149_IsSafeForRootSet(const UObject & self) {
	return self.IsSafeForRootSet();
}

void _dasUnreal_virtual_150_TagSubobjects(UObject & self,EObjectFlags NewFlags) {
	self.TagSubobjects(NewFlags);
}

bool _dasUnreal_virtual_151_IsNameStableForNetworking(const UObject & self) {
	return self.IsNameStableForNetworking();
}

bool _dasUnreal_virtual_152_IsFullNameStableForNetworking(const UObject & self) {
	return self.IsFullNameStableForNetworking();
}

bool _dasUnreal_virtual_153_IsSupportedForNetworking(const UObject & self) {
	return self.IsSupportedForNetworking();
}

void _dasUnreal_virtual_154_PreNetReceive(UObject & self) {
	self.PreNetReceive();
}

void _dasUnreal_virtual_155_PostNetReceive(UObject & self) {
	self.PostNetReceive();
}

void _dasUnreal_virtual_156_PostRepNotifies(UObject & self) {
	self.PostRepNotifies();
}

void _dasUnreal_virtual_157_PreDestroyFromReplication(UObject & self) {
	self.PreDestroyFromReplication();
}

void _dasUnreal_virtual_158_WillNeverCacheCookedPlatformDataAgain(UObject & self) {
	self.WillNeverCacheCookedPlatformDataAgain();
}

void _dasUnreal_virtual_159_ClearAllCachedCookedPlatformData(UObject & self) {
	self.ClearAllCachedCookedPlatformData();
}

const wchar_t * _dasUnreal_virtual_160_GetConfigOverridePlatform(const UObject & self) {
	return self.GetConfigOverridePlatform();
}

void _dasUnreal_virtual_161_OverrideConfigSection(UObject & self,FString & SectionName) {
	self.OverrideConfigSection(SectionName);
}

void _dasUnreal_virtual_162_OverridePerObjectConfigSection(UObject & self,FString & SectionName) {
	self.OverridePerObjectConfigSection(SectionName);
}

UObject * _dasUnreal_static_163_GetArchetypeFromRequiredInfo(const UObject *,const UClass * Class,const UObject * Outer,FName Name,EObjectFlags ObjectFlags) {
	return UObject::GetArchetypeFromRequiredInfo(Class,Outer,Name,ObjectFlags);
}

void _dasUnreal_virtual_164_ProcessEvent(UObject & self,UFunction * Function,void * Parms) {
	self.ProcessEvent(Function,Parms);
}

UClass * _dasUnreal_virtual_165_RegenerateClass(UObject & self,UClass * ClassToRegenerate,UObject * PreviousCDO) {
	return self.RegenerateClass(ClassToRegenerate,PreviousCDO);
}

void _dasUnreal_virtual_166_MarkAsEditorOnlySubobject(UObject & self) {
	self.MarkAsEditorOnlySubobject();
}

uint64 _dasUnreal_virtual_167_GetNetPushIdDynamic(const UObject & self) {
	return self.GetNetPushIdDynamic();
}

FFieldClass * _dasUnreal_static_168_StaticClass(const FField *) {
	return FField::StaticClass();
}

uint64 _dasUnreal_static_169_StaticClassCastFlagsPrivate(const FField *) {
	return FField::StaticClassCastFlagsPrivate();
}

uint64 _dasUnreal_static_170_StaticClassCastFlags(const FField *) {
	return FField::StaticClassCastFlags();
}

void _dasUnreal_virtual_171_PostLoad(FField & self) {
	self.PostLoad();
}

void _dasUnreal_virtual_172_BeginDestroy(FField & self) {
	self.BeginDestroy();
}

void _dasUnreal_virtual_173_AddReferencedObjects(FField & self,FReferenceCollector & Collector) {
	self.AddReferencedObjects(Collector);
}

void _dasUnreal_virtual_174_AddCppProperty(FField & self,FProperty * Property) {
	self.AddCppProperty(Property);
}

void _dasUnreal_virtual_175_Bind(FField & self) {
	self.Bind();
}

FField * _dasUnreal_static_176_Construct(const FField *,const FFieldVariant & InOwner,const FName & InName,EObjectFlags InFlags) {
	return FField::Construct(InOwner,InName,InFlags);
}

FField * _dasUnreal_static_177_Construct(const FField *,const FName & FieldTypeName,const FFieldVariant & InOwner,const FName & InName,EObjectFlags InFlags) {
	return FField::Construct(FieldTypeName,InOwner,InName,InFlags);
}

void _dasUnreal_virtual_178_PostDuplicate(FField & self,const FField & InField) {
	self.PostDuplicate(InField);
}

FField * _dasUnreal_virtual_179_GetInnerFieldByName(FField & self,const FName & InName) {
	return self.GetInnerFieldByName(InName);
}

void _dasUnreal_static_180_CopyMetaData(const FField *,const FField * InSourceField,FField * InDestField) {
	FField::CopyMetaData(InSourceField,InDestField);
}

FField * _dasUnreal_static_181_CreateFromUField(const FField *,UField * InField) {
	return FField::CreateFromUField(InField);
}

FField * _dasUnreal_static_182_Duplicate(const FField *,const FField * InField,FFieldVariant DestOwner,const FName DestName,EObjectFlags FlagMask,EInternalObjectFlags InternalFlagsMask) {
	return FField::Duplicate(InField,DestOwner,DestName,FlagMask,InternalFlagsMask);
}

FName _dasUnreal_static_183_GenerateFFieldName(const FField *,FFieldVariant InOwner,FFieldClass * InClass) {
	return FField::GenerateFFieldName(InOwner,InClass);
}

UClass * _dasUnreal_static_184_StaticClass(const UField *) {
	return UField::StaticClass();
}

const wchar_t * _dasUnreal_static_185_StaticPackage(const UField *) {
	return UField::StaticPackage();
}

void _dasUnreal_static_186_StaticRegisterNativesUField(const UField *) {
	UField::StaticRegisterNativesUField();
}

void _dasUnreal_virtual_187_PostLoad(UField & self) {
	self.PostLoad();
}

bool _dasUnreal_virtual_188_NeedsLoadForClient(const UField & self) {
	return self.NeedsLoadForClient();
}

bool _dasUnreal_virtual_189_NeedsLoadForServer(const UField & self) {
	return self.NeedsLoadForServer();
}

void _dasUnreal_virtual_190_Bind(UField & self) {
	self.Bind();
}

void _dasUnreal_static_191_FormatNativeToolTip(const UField *,FString & ToolTipString,bool bRemoveExtraSections) {
	UField::FormatNativeToolTip(ToolTipString,bRemoveExtraSections);
}

FField * _dasUnreal_virtual_192_GetAssociatedFField(UField & self) {
	return self.GetAssociatedFField();
}

void _dasUnreal_virtual_193_SetAssociatedFField(UField & self,FField * InField) {
	self.SetAssociatedFField(InField);
}

UClass * _dasUnreal_static_194_StaticClass(const UStruct *) {
	return UStruct::StaticClass();
}

const wchar_t * _dasUnreal_static_195_StaticPackage(const UStruct *) {
	return UStruct::StaticPackage();
}

void _dasUnreal_static_196_StaticRegisterNativesUStruct(const UStruct *) {
	UStruct::StaticRegisterNativesUStruct();
}

void _dasUnreal_virtual_197_PostLoad(UStruct & self) {
	self.PostLoad();
}

void _dasUnreal_virtual_198_FinishDestroy(UStruct & self) {
	self.FinishDestroy();
}

void _dasUnreal_virtual_199_RegisterDependencies(UStruct & self) {
	self.RegisterDependencies();
}

void _dasUnreal_virtual_200_TagSubobjects(UStruct & self,EObjectFlags NewFlags) {
	self.TagSubobjects(NewFlags);
}

void _dasUnreal_virtual_201_AddCppProperty(UStruct & self,FProperty * Property) {
	self.AddCppProperty(Property);
}

UStruct * _dasUnreal_virtual_202_GetInheritanceSuper(const UStruct & self) {
	return self.GetInheritanceSuper();
}

void _dasUnreal_virtual_203_InitializeStruct(const UStruct & self,void * Dest,int ArrayDim) {
	self.InitializeStruct(Dest,ArrayDim);
}

void _dasUnreal_virtual_204_DestroyStruct(const UStruct & self,void * Dest,int ArrayDim) {
	self.DestroyStruct(Dest,ArrayDim);
}

FProperty * _dasUnreal_virtual_205_CustomFindProperty(const UStruct & self,const FName InName) {
	return self.CustomFindProperty(InName);
}

const wchar_t * _dasUnreal_virtual_206_GetPrefixCPP(const UStruct & self) {
	return self.GetPrefixCPP();
}

void _dasUnreal_virtual_207_SetSuperStruct(UStruct & self,UStruct * NewSuperStruct) {
	self.SetSuperStruct(NewSuperStruct);
}

FString _dasUnreal_virtual_208_GetAuthoredNameForField(const UStruct & self,const UField * Field) {
	return self.GetAuthoredNameForField(Field);
}

FString _dasUnreal_virtual_209_GetAuthoredNameForField(const UStruct & self,const FField * Field) {
	return self.GetAuthoredNameForField(Field);
}

bool _dasUnreal_virtual_210_IsStructTrashed(const UStruct & self) {
	return self.IsStructTrashed();
}

UClass * _dasUnreal_static_211_StaticClass(const UScriptStruct *) {
	return UScriptStruct::StaticClass();
}

const wchar_t * _dasUnreal_static_212_StaticPackage(const UScriptStruct *) {
	return UScriptStruct::StaticPackage();
}

void _dasUnreal_static_213_StaticRegisterNativesUScriptStruct(const UScriptStruct *) {
	UScriptStruct::StaticRegisterNativesUScriptStruct();
}

void _dasUnreal_virtual_214_InitializeStruct(const UScriptStruct & self,void * Dest,int ArrayDim) {
	self.InitializeStruct(Dest,ArrayDim);
}

void _dasUnreal_virtual_215_DestroyStruct(const UScriptStruct & self,void * Dest,int ArrayDim) {
	self.DestroyStruct(Dest,ArrayDim);
}

bool _dasUnreal_virtual_216_IsStructTrashed(const UScriptStruct & self) {
	return self.IsStructTrashed();
}

void _dasUnreal_virtual_217_PrepareCppStructOps(UScriptStruct & self) {
	self.PrepareCppStructOps();
}

unsigned int _dasUnreal_virtual_218_GetStructTypeHash(const UScriptStruct & self,const void * Src) {
	return self.GetStructTypeHash(Src);
}

void _dasUnreal_virtual_219_RecursivelyPreload(UScriptStruct & self) {
	self.RecursivelyPreload();
}

FString _dasUnreal_virtual_220_GetStructCPPName(const UScriptStruct & self,unsigned int CPPExportFlags) {
	return self.GetStructCPPName(CPPExportFlags);
}

void _dasUnreal_virtual_221_InitializeDefaultValue(const UScriptStruct & self,unsigned char * InStructData) {
	self.InitializeDefaultValue(InStructData);
}

UClass * _dasUnreal_static_222_StaticClass(const UFunction *) {
	return UFunction::StaticClass();
}

const wchar_t * _dasUnreal_static_223_StaticPackage(const UFunction *) {
	return UFunction::StaticPackage();
}

void _dasUnreal_static_224_StaticRegisterNativesUFunction(const UFunction *) {
	UFunction::StaticRegisterNativesUFunction();
}

void _dasUnreal_virtual_225_PostLoad(UFunction & self) {
	self.PostLoad();
}

void _dasUnreal_virtual_226_Bind(UFunction & self) {
	self.Bind();
}

UStruct * _dasUnreal_virtual_227_GetInheritanceSuper(const UFunction & self) {
	return self.GetInheritanceSuper();
}

uint64 _dasUnreal_static_228_GetDefaultIgnoredSignatureCompatibilityFlags(const UFunction *) {
	return UFunction::GetDefaultIgnoredSignatureCompatibilityFlags();
}

UClass * _dasUnreal_static_229_StaticClass(const UEnum *) {
	return UEnum::StaticClass();
}

const wchar_t * _dasUnreal_static_230_StaticPackage(const UEnum *) {
	return UEnum::StaticPackage();
}

void _dasUnreal_static_231_StaticRegisterNativesUEnum(const UEnum *) {
	UEnum::StaticRegisterNativesUEnum();
}

FText _dasUnreal_virtual_232_GetDisplayNameTextByIndex(const UEnum & self,int InIndex) {
	return self.GetDisplayNameTextByIndex(InIndex);
}

FString _dasUnreal_virtual_233_GetAuthoredNameStringByIndex(const UEnum & self,int InIndex) {
	return self.GetAuthoredNameStringByIndex(InIndex);
}

bool _dasUnreal_static_234_IsFullEnumName(const UEnum *,const wchar_t * InEnumName) {
	return UEnum::IsFullEnumName(InEnumName);
}

FString _dasUnreal_virtual_235_GenerateFullEnumName(const UEnum & self,const wchar_t * InEnumName) {
	return self.GenerateFullEnumName(InEnumName);
}

int64 _dasUnreal_static_236_ParseEnum(const UEnum *,const wchar_t *& Str) {
	return UEnum::ParseEnum(Str);
}

void _dasUnreal_virtual_237_BeginDestroy(UEnum & self) {
	self.BeginDestroy();
}

bool _dasUnreal_virtual_238_IsAbstract(const FCppClassTypeInfo & self) {
	return self.IsAbstract();
}

UClass * _dasUnreal_static_239_StaticClass(const UClass *) {
	return UClass::StaticClass();
}

const wchar_t * _dasUnreal_static_240_StaticPackage(const UClass *) {
	return UClass::StaticPackage();
}

void _dasUnreal_static_241_StaticRegisterNativesUClass(const UClass *) {
	UClass::StaticRegisterNativesUClass();
}

void _dasUnreal_virtual_242_ConditionalRecompileClass(UClass & self,FUObjectSerializeContext * InLoadContext) {
	self.ConditionalRecompileClass(InLoadContext);
}

void _dasUnreal_virtual_243_FlushCompilationQueueForLevel(UClass & self) {
	self.FlushCompilationQueueForLevel();
}

void _dasUnreal_static_244_AssembleReferenceTokenStreams(const UClass *) {
	UClass::AssembleReferenceTokenStreams();
}

UClass * _dasUnreal_virtual_245_GetAuthoritativeClass(UClass & self) {
	return self.GetAuthoritativeClass();
}

void _dasUnreal_virtual_246_PostLoad(UClass & self) {
	self.PostLoad();
}

void _dasUnreal_virtual_247_FinishDestroy(UClass & self) {
	self.FinishDestroy();
}

void _dasUnreal_virtual_248_DeferredRegister(UClass & self,UClass * UClassStaticClass,const wchar_t * PackageName,const wchar_t * InName) {
	self.DeferredRegister(UClassStaticClass,PackageName,InName);
}

bool _dasUnreal_virtual_249_Rename(UClass & self,const wchar_t * NewName,UObject * NewOuter,unsigned int Flags) {
	return self.Rename(NewName,NewOuter,Flags);
}

void _dasUnreal_virtual_250_TagSubobjects(UClass & self,EObjectFlags NewFlags) {
	self.TagSubobjects(NewFlags);
}

void _dasUnreal_virtual_251_PostInitProperties(UClass & self) {
	self.PostInitProperties();
}

FRestoreForUObjectOverwrite * _dasUnreal_virtual_252_GetRestoreForUObjectOverwrite(UClass & self) {
	return self.GetRestoreForUObjectOverwrite();
}

FString _dasUnreal_virtual_253_GetDesc(UClass & self) {
	return self.GetDesc();
}

bool _dasUnreal_virtual_254_IsAsset(const UClass & self) {
	return self.IsAsset();
}

bool _dasUnreal_virtual_255_IsNameStableForNetworking(const UClass & self) {
	return self.IsNameStableForNetworking();
}

void _dasUnreal_virtual_256_Bind(UClass & self) {
	self.Bind();
}

const wchar_t * _dasUnreal_virtual_257_GetPrefixCPP(const UClass & self) {
	return self.GetPrefixCPP();
}

void _dasUnreal_virtual_258_SetSuperStruct(UClass & self,UStruct * NewSuperStruct) {
	self.SetSuperStruct(NewSuperStruct);
}

bool _dasUnreal_virtual_259_IsStructTrashed(const UClass & self) {
	return self.IsStructTrashed();
}

void _dasUnreal_virtual_260_PostLoadInstance(UClass & self,UObject * InObj) {
	self.PostLoadInstance(InObj);
}

void _dasUnreal_virtual_261_InitPropertiesFromCustomList(UClass & self,unsigned char * DataPtr,const unsigned char * DefaultDataPtr) {
	self.InitPropertiesFromCustomList(DataPtr,DefaultDataPtr);
}

bool _dasUnreal_virtual_262_CanCreateAssetOfClass(const UClass & self) {
	return self.CanCreateAssetOfClass();
}

unsigned char * _dasUnreal_virtual_263_GetPersistentUberGraphFrame(const UClass & self,UObject * Obj,UFunction * FuncToCheck) {
	return self.GetPersistentUberGraphFrame(Obj,FuncToCheck);
}

void _dasUnreal_virtual_264_CreatePersistentUberGraphFrame(const UClass & self,UObject * Obj,bool bCreateOnlyIfEmpty,bool bSkipSuperClass,UClass * OldClass) {
	self.CreatePersistentUberGraphFrame(Obj,bCreateOnlyIfEmpty,bSkipSuperClass,OldClass);
}

void _dasUnreal_virtual_265_DestroyPersistentUberGraphFrame(const UClass & self,UObject * Obj,bool bSkipSuperClass) {
	self.DestroyPersistentUberGraphFrame(Obj,bSkipSuperClass);
}

void _dasUnreal_virtual_266_PostLoadDefaultObject(UClass & self,UObject * Object) {
	self.PostLoadDefaultObject(Object);
}

void _dasUnreal_virtual_267_PurgeClass(UClass & self,bool bRecompilingOnLoad) {
	self.PurgeClass(bRecompilingOnLoad);
}

UClass * _dasUnreal_static_268_FindCommonBase(const UClass *,UClass * InClassA,UClass * InClassB) {
	return UClass::FindCommonBase(InClassA,InClassB);
}

bool _dasUnreal_virtual_269_IsFunctionImplementedInScript(const UClass & self,FName InFunctionName) {
	return self.IsFunctionImplementedInScript(InFunctionName);
}

bool _dasUnreal_virtual_270_HasProperty(const UClass & self,FProperty * InProperty) {
	return self.HasProperty(InProperty);
}

UObject * _dasUnreal_virtual_271_FindArchetype(const UClass & self,const UClass * ArchetypeClass,const FName ArchetypeName) {
	return self.FindArchetype(ArchetypeClass,ArchetypeName);
}

UObject * _dasUnreal_virtual_272_GetArchetypeForCDO(const UClass & self) {
	return self.GetArchetypeForCDO();
}

bool _dasUnreal_static_273_IsSafeToSerializeToStructuredArchives(const UClass *,UClass * InClass) {
	return UClass::IsSafeToSerializeToStructuredArchives(InClass);
}

UClass * _dasUnreal_static_274_StaticClass(const UPackage *) {
	return UPackage::StaticClass();
}

const wchar_t * _dasUnreal_static_275_StaticPackage(const UPackage *) {
	return UPackage::StaticPackage();
}

void _dasUnreal_static_276_StaticRegisterNativesUPackage(const UPackage *) {
	UPackage::StaticRegisterNativesUPackage();
}

bool _dasUnreal_virtual_277_IsNameStableForNetworking(const UPackage & self) {
	return self.IsNameStableForNetworking();
}

bool _dasUnreal_virtual_278_NeedsLoadForClient(const UPackage & self) {
	return self.NeedsLoadForClient();
}

bool _dasUnreal_virtual_279_NeedsLoadForServer(const UPackage & self) {
	return self.NeedsLoadForServer();
}

bool _dasUnreal_virtual_280_IsPostLoadThreadSafe(const UPackage & self) {
	return self.IsPostLoadThreadSafe();
}

bool _dasUnreal_virtual_281_IsDestructionThreadSafe(const UPackage & self) {
	return self.IsDestructionThreadSafe();
}

void _dasUnreal_virtual_282_PostInitProperties(UPackage & self) {
	self.PostInitProperties();
}

void _dasUnreal_virtual_283_FinishDestroy(UPackage & self) {
	self.FinishDestroy();
}

bool _dasUnreal_virtual_284_IsAsset(const UPackage & self) {
	return self.IsAsset();
}

void _dasUnreal_virtual_285_TagSubobjects(UPackage & self,EObjectFlags NewFlags) {
	self.TagSubobjects(NewFlags);
}

void _dasUnreal_static_286_WaitForAsyncFileWrites(const UPackage *) {
	UPackage::WaitForAsyncFileWrites();
}

bool _dasUnreal_static_287_HasAsyncFileWrites(const UPackage *) {
	return UPackage::HasAsyncFileWrites();
}

bool _dasUnreal_static_288_IsEmptyPackage(const UPackage *,UPackage * Package,const UObject * LastReferencer) {
	return UPackage::IsEmptyPackage(Package,LastReferencer);
}

FFieldClass * _dasUnreal_static_289_StaticClass(const FProperty *) {
	return FProperty::StaticClass();
}

uint64 _dasUnreal_static_290_StaticClassCastFlagsPrivate(const FProperty *) {
	return FProperty::StaticClassCastFlagsPrivate();
}

uint64 _dasUnreal_static_291_StaticClassCastFlags(const FProperty *) {
	return FProperty::StaticClassCastFlags();
}

void _dasUnreal_virtual_292_PostDuplicate(FProperty & self,const FField & InField) {
	self.PostDuplicate(InField);
}

FName _dasUnreal_static_293_FindRedirectedPropertyName(const FProperty *,UStruct * ObjectStruct,FName OldName) {
	return FProperty::FindRedirectedPropertyName(ObjectStruct,OldName);
}

FString _dasUnreal_virtual_294_GetCPPMacroType(const FProperty & self,FString & ExtendedTypeText) {
	return self.GetCPPMacroType(ExtendedTypeText);
}

bool _dasUnreal_virtual_295_PassCPPArgsByRef(const FProperty & self) {
	return self.PassCPPArgsByRef();
}

FString _dasUnreal_virtual_296_GetCPPType(const FProperty & self,FString * ExtendedTypeText,unsigned int CPPExportFlags) {
	return self.GetCPPType(ExtendedTypeText,CPPExportFlags);
}

FString _dasUnreal_virtual_297_GetCPPTypeForwardDeclaration(const FProperty & self) {
	return self.GetCPPTypeForwardDeclaration();
}

bool _dasUnreal_virtual_298_HasSetter(const FProperty & self) {
	return self.HasSetter();
}

bool _dasUnreal_virtual_299_HasGetter(const FProperty & self) {
	return self.HasGetter();
}

bool _dasUnreal_virtual_300_HasSetterOrGetter(const FProperty & self) {
	return self.HasSetterOrGetter();
}

void _dasUnreal_virtual_301_CallSetter(const FProperty & self,void * Container,const void * InValue) {
	self.CallSetter(Container,InValue);
}

void _dasUnreal_virtual_302_CallGetter(const FProperty & self,const void * Container,void * OutValue) {
	self.CallGetter(Container,OutValue);
}

bool _dasUnreal_virtual_303_Identical(const FProperty & self,const void * A,const void * B,unsigned int PortFlags) {
	return self.Identical(A,B,PortFlags);
}

bool _dasUnreal_virtual_304_SupportsNetSharedSerialization(const FProperty & self) {
	return self.SupportsNetSharedSerialization();
}

void _dasUnreal_virtual_305_ExportTextItem(const FProperty & self,FString & ValueStr,const void * PropertyValue,const void * DefaultValue,UObject * Parent,int PortFlags,UObject * ExportRootScope) {
	self.ExportTextItem(ValueStr,PropertyValue,DefaultValue,Parent,PortFlags,ExportRootScope);
}

void * _dasUnreal_virtual_306_GetValueAddressAtIndex_Direct(const FProperty & self,const FProperty * Inner,void * InValueAddress,int Index) {
	return self.GetValueAddressAtIndex_Direct(Inner,InValueAddress,Index);
}

void _dasUnreal_virtual_307_AppendSchemaHash(const FProperty & self,FBlake3 & Builder,bool bSkipEditorOnly) {
	self.AppendSchemaHash(Builder,bSkipEditorOnly);
}

void _dasUnreal_virtual_308_CopySingleValueToScriptVM(const FProperty & self,void * Dest,const void * Src) {
	self.CopySingleValueToScriptVM(Dest,Src);
}

void _dasUnreal_virtual_309_CopyCompleteValueToScriptVM(const FProperty & self,void * Dest,const void * Src) {
	self.CopyCompleteValueToScriptVM(Dest,Src);
}

void _dasUnreal_virtual_310_CopyCompleteValueToScriptVM_InContainer(const FProperty & self,void * OutValue,const void * InContainer) {
	self.CopyCompleteValueToScriptVM_InContainer(OutValue,InContainer);
}

void _dasUnreal_virtual_311_CopyCompleteValueFromScriptVM_InContainer(const FProperty & self,void * OutContainer,const void * InValue) {
	self.CopyCompleteValueFromScriptVM_InContainer(OutContainer,InValue);
}

void _dasUnreal_virtual_312_CopySingleValueFromScriptVM(const FProperty & self,void * Dest,const void * Src) {
	self.CopySingleValueFromScriptVM(Dest,Src);
}

void _dasUnreal_virtual_313_CopyCompleteValueFromScriptVM(const FProperty & self,void * Dest,const void * Src) {
	self.CopyCompleteValueFromScriptVM(Dest,Src);
}

FName _dasUnreal_virtual_314_GetID(const FProperty & self) {
	return self.GetID();
}

int _dasUnreal_virtual_315_GetMinAlignment(const FProperty & self) {
	return self.GetMinAlignment();
}

bool _dasUnreal_virtual_316_SameType(const FProperty & self,const FProperty * Other) {
	return self.SameType(Other);
}

FFieldClass * _dasUnreal_static_317_StaticClass(const FNumericProperty *) {
	return FNumericProperty::StaticClass();
}

FField * _dasUnreal_static_318_Construct(const FNumericProperty *,const FFieldVariant & InOwner,const FName & InName,EObjectFlags InObjectFlags) {
	return FNumericProperty::Construct(InOwner,InName,InObjectFlags);
}

uint64 _dasUnreal_static_319_StaticClassCastFlagsPrivate(const FNumericProperty *) {
	return FNumericProperty::StaticClassCastFlagsPrivate();
}

uint64 _dasUnreal_static_320_StaticClassCastFlags(const FNumericProperty *) {
	return FNumericProperty::StaticClassCastFlags();
}

void _dasUnreal_virtual_321_ExportText_Internal(const FNumericProperty & self,FString & ValueStr,const void * PropertyValueOrContainer,EPropertyPointerType PropertyPointerType,const void * DefaultValue,UObject * Parent,int PortFlags,UObject * ExportRootScope) {
	self.ExportText_Internal(ValueStr,PropertyValueOrContainer,PropertyPointerType,DefaultValue,Parent,PortFlags,ExportRootScope);
}

bool _dasUnreal_virtual_322_IsFloatingPoint(const FNumericProperty & self) {
	return self.IsFloatingPoint();
}

bool _dasUnreal_virtual_323_IsInteger(const FNumericProperty & self) {
	return self.IsInteger();
}

UEnum * _dasUnreal_virtual_324_GetIntPropertyEnum(const FNumericProperty & self) {
	return self.GetIntPropertyEnum();
}

void _dasUnreal_virtual_325_SetIntPropertyValue(const FNumericProperty & self,void * Data,uint64 Value) {
	self.SetIntPropertyValue(Data,Value);
}

void _dasUnreal_virtual_326_SetIntPropertyValue(const FNumericProperty & self,void * Data,int64 Value) {
	self.SetIntPropertyValue(Data,Value);
}

void _dasUnreal_virtual_327_SetFloatingPointPropertyValue(const FNumericProperty & self,void * Data,double Value) {
	self.SetFloatingPointPropertyValue(Data,Value);
}

void _dasUnreal_virtual_328_SetNumericPropertyValueFromString(const FNumericProperty & self,void * Data,const wchar_t * Value) {
	self.SetNumericPropertyValueFromString(Data,Value);
}

void _dasUnreal_virtual_329_SetNumericPropertyValueFromString_InContainer(const FNumericProperty & self,void * Container,const wchar_t * Value) {
	self.SetNumericPropertyValueFromString_InContainer(Container,Value);
}

int64 _dasUnreal_virtual_330_GetSignedIntPropertyValue(const FNumericProperty & self,const void * Data) {
	return self.GetSignedIntPropertyValue(Data);
}

int64 _dasUnreal_virtual_331_GetSignedIntPropertyValue_InContainer(const FNumericProperty & self,const void * Container) {
	return self.GetSignedIntPropertyValue_InContainer(Container);
}

uint64 _dasUnreal_virtual_332_GetUnsignedIntPropertyValue(const FNumericProperty & self,const void * Data) {
	return self.GetUnsignedIntPropertyValue(Data);
}

uint64 _dasUnreal_virtual_333_GetUnsignedIntPropertyValue_InContainer(const FNumericProperty & self,const void * Container) {
	return self.GetUnsignedIntPropertyValue_InContainer(Container);
}

double _dasUnreal_virtual_334_GetFloatingPointPropertyValue(const FNumericProperty & self,const void * Data) {
	return self.GetFloatingPointPropertyValue(Data);
}

FString _dasUnreal_virtual_335_GetNumericPropertyValueToString(const FNumericProperty & self,const void * Data) {
	return self.GetNumericPropertyValueToString(Data);
}

FString _dasUnreal_virtual_336_GetNumericPropertyValueToString_InContainer(const FNumericProperty & self,const void * Container) {
	return self.GetNumericPropertyValueToString_InContainer(Container);
}

FFieldClass * _dasUnreal_static_337_StaticClass(const FByteProperty *) {
	return FByteProperty::StaticClass();
}

FField * _dasUnreal_static_338_Construct(const FByteProperty *,const FFieldVariant & InOwner,const FName & InName,EObjectFlags InObjectFlags) {
	return FByteProperty::Construct(InOwner,InName,InObjectFlags);
}

uint64 _dasUnreal_static_339_StaticClassCastFlagsPrivate(const FByteProperty *) {
	return FByteProperty::StaticClassCastFlagsPrivate();
}

uint64 _dasUnreal_static_340_StaticClassCastFlags(const FByteProperty *) {
	return FByteProperty::StaticClassCastFlags();
}

void _dasUnreal_virtual_341_AddReferencedObjects(FByteProperty & self,FReferenceCollector & Collector) {
	self.AddReferencedObjects(Collector);
}

void _dasUnreal_virtual_342_PostDuplicate(FByteProperty & self,const FField & InField) {
	self.PostDuplicate(InField);
}

FString _dasUnreal_virtual_343_GetCPPType(const FByteProperty & self,FString * ExtendedTypeText,unsigned int CPPExportFlags) {
	return self.GetCPPType(ExtendedTypeText,CPPExportFlags);
}

void _dasUnreal_virtual_344_AppendSchemaHash(const FByteProperty & self,FBlake3 & Builder,bool bSkipEditorOnly) {
	self.AppendSchemaHash(Builder,bSkipEditorOnly);
}

UEnum * _dasUnreal_virtual_345_GetIntPropertyEnum(const FByteProperty & self) {
	return self.GetIntPropertyEnum();
}

FFieldClass * _dasUnreal_static_346_StaticClass(const FInt8Property *) {
	return FInt8Property::StaticClass();
}

FField * _dasUnreal_static_347_Construct(const FInt8Property *,const FFieldVariant & InOwner,const FName & InName,EObjectFlags InObjectFlags) {
	return FInt8Property::Construct(InOwner,InName,InObjectFlags);
}

uint64 _dasUnreal_static_348_StaticClassCastFlagsPrivate(const FInt8Property *) {
	return FInt8Property::StaticClassCastFlagsPrivate();
}

uint64 _dasUnreal_static_349_StaticClassCastFlags(const FInt8Property *) {
	return FInt8Property::StaticClassCastFlags();
}

FFieldClass * _dasUnreal_static_350_StaticClass(const FInt16Property *) {
	return FInt16Property::StaticClass();
}

FField * _dasUnreal_static_351_Construct(const FInt16Property *,const FFieldVariant & InOwner,const FName & InName,EObjectFlags InObjectFlags) {
	return FInt16Property::Construct(InOwner,InName,InObjectFlags);
}

uint64 _dasUnreal_static_352_StaticClassCastFlagsPrivate(const FInt16Property *) {
	return FInt16Property::StaticClassCastFlagsPrivate();
}

uint64 _dasUnreal_static_353_StaticClassCastFlags(const FInt16Property *) {
	return FInt16Property::StaticClassCastFlags();
}

FFieldClass * _dasUnreal_static_354_StaticClass(const FIntProperty *) {
	return FIntProperty::StaticClass();
}

FField * _dasUnreal_static_355_Construct(const FIntProperty *,const FFieldVariant & InOwner,const FName & InName,EObjectFlags InObjectFlags) {
	return FIntProperty::Construct(InOwner,InName,InObjectFlags);
}

uint64 _dasUnreal_static_356_StaticClassCastFlagsPrivate(const FIntProperty *) {
	return FIntProperty::StaticClassCastFlagsPrivate();
}

uint64 _dasUnreal_static_357_StaticClassCastFlags(const FIntProperty *) {
	return FIntProperty::StaticClassCastFlags();
}

FFieldClass * _dasUnreal_static_358_StaticClass(const FInt64Property *) {
	return FInt64Property::StaticClass();
}

FField * _dasUnreal_static_359_Construct(const FInt64Property *,const FFieldVariant & InOwner,const FName & InName,EObjectFlags InObjectFlags) {
	return FInt64Property::Construct(InOwner,InName,InObjectFlags);
}

uint64 _dasUnreal_static_360_StaticClassCastFlagsPrivate(const FInt64Property *) {
	return FInt64Property::StaticClassCastFlagsPrivate();
}

uint64 _dasUnreal_static_361_StaticClassCastFlags(const FInt64Property *) {
	return FInt64Property::StaticClassCastFlags();
}

FFieldClass * _dasUnreal_static_362_StaticClass(const FUInt16Property *) {
	return FUInt16Property::StaticClass();
}

FField * _dasUnreal_static_363_Construct(const FUInt16Property *,const FFieldVariant & InOwner,const FName & InName,EObjectFlags InObjectFlags) {
	return FUInt16Property::Construct(InOwner,InName,InObjectFlags);
}

uint64 _dasUnreal_static_364_StaticClassCastFlagsPrivate(const FUInt16Property *) {
	return FUInt16Property::StaticClassCastFlagsPrivate();
}

uint64 _dasUnreal_static_365_StaticClassCastFlags(const FUInt16Property *) {
	return FUInt16Property::StaticClassCastFlags();
}

FFieldClass * _dasUnreal_static_366_StaticClass(const FUInt32Property *) {
	return FUInt32Property::StaticClass();
}

FField * _dasUnreal_static_367_Construct(const FUInt32Property *,const FFieldVariant & InOwner,const FName & InName,EObjectFlags InObjectFlags) {
	return FUInt32Property::Construct(InOwner,InName,InObjectFlags);
}

uint64 _dasUnreal_static_368_StaticClassCastFlagsPrivate(const FUInt32Property *) {
	return FUInt32Property::StaticClassCastFlagsPrivate();
}

uint64 _dasUnreal_static_369_StaticClassCastFlags(const FUInt32Property *) {
	return FUInt32Property::StaticClassCastFlags();
}

FFieldClass * _dasUnreal_static_370_StaticClass(const FUInt64Property *) {
	return FUInt64Property::StaticClass();
}

FField * _dasUnreal_static_371_Construct(const FUInt64Property *,const FFieldVariant & InOwner,const FName & InName,EObjectFlags InObjectFlags) {
	return FUInt64Property::Construct(InOwner,InName,InObjectFlags);
}

uint64 _dasUnreal_static_372_StaticClassCastFlagsPrivate(const FUInt64Property *) {
	return FUInt64Property::StaticClassCastFlagsPrivate();
}

uint64 _dasUnreal_static_373_StaticClassCastFlags(const FUInt64Property *) {
	return FUInt64Property::StaticClassCastFlags();
}

FFieldClass * _dasUnreal_static_374_StaticClass(const FFloatProperty *) {
	return FFloatProperty::StaticClass();
}

FField * _dasUnreal_static_375_Construct(const FFloatProperty *,const FFieldVariant & InOwner,const FName & InName,EObjectFlags InObjectFlags) {
	return FFloatProperty::Construct(InOwner,InName,InObjectFlags);
}

uint64 _dasUnreal_static_376_StaticClassCastFlagsPrivate(const FFloatProperty *) {
	return FFloatProperty::StaticClassCastFlagsPrivate();
}

uint64 _dasUnreal_static_377_StaticClassCastFlags(const FFloatProperty *) {
	return FFloatProperty::StaticClassCastFlags();
}

FFieldClass * _dasUnreal_static_378_StaticClass(const FDoubleProperty *) {
	return FDoubleProperty::StaticClass();
}

FField * _dasUnreal_static_379_Construct(const FDoubleProperty *,const FFieldVariant & InOwner,const FName & InName,EObjectFlags InObjectFlags) {
	return FDoubleProperty::Construct(InOwner,InName,InObjectFlags);
}

uint64 _dasUnreal_static_380_StaticClassCastFlagsPrivate(const FDoubleProperty *) {
	return FDoubleProperty::StaticClassCastFlagsPrivate();
}

uint64 _dasUnreal_static_381_StaticClassCastFlags(const FDoubleProperty *) {
	return FDoubleProperty::StaticClassCastFlags();
}

FFieldClass * _dasUnreal_static_382_StaticClass(const FBoolProperty *) {
	return FBoolProperty::StaticClass();
}

FField * _dasUnreal_static_383_Construct(const FBoolProperty *,const FFieldVariant & InOwner,const FName & InName,EObjectFlags InObjectFlags) {
	return FBoolProperty::Construct(InOwner,InName,InObjectFlags);
}

uint64 _dasUnreal_static_384_StaticClassCastFlagsPrivate(const FBoolProperty *) {
	return FBoolProperty::StaticClassCastFlagsPrivate();
}

uint64 _dasUnreal_static_385_StaticClassCastFlags(const FBoolProperty *) {
	return FBoolProperty::StaticClassCastFlags();
}

void _dasUnreal_virtual_386_PostDuplicate(FBoolProperty & self,const FField & InField) {
	self.PostDuplicate(InField);
}

FString _dasUnreal_virtual_387_GetCPPType(const FBoolProperty & self,FString * ExtendedTypeText,unsigned int CPPExportFlags) {
	return self.GetCPPType(ExtendedTypeText,CPPExportFlags);
}

FString _dasUnreal_virtual_388_GetCPPMacroType(const FBoolProperty & self,FString & ExtendedTypeText) {
	return self.GetCPPMacroType(ExtendedTypeText);
}

FString _dasUnreal_virtual_389_GetCPPTypeForwardDeclaration(const FBoolProperty & self) {
	return self.GetCPPTypeForwardDeclaration();
}

bool _dasUnreal_virtual_390_Identical(const FBoolProperty & self,const void * A,const void * B,unsigned int PortFlags) {
	return self.Identical(A,B,PortFlags);
}

void _dasUnreal_virtual_391_CopyValuesInternal(const FBoolProperty & self,void * Dest,const void * Src,int Count) {
	self.CopyValuesInternal(Dest,Src,Count);
}

void _dasUnreal_virtual_392_ClearValueInternal(const FBoolProperty & self,void * Data) {
	self.ClearValueInternal(Data);
}

void _dasUnreal_virtual_393_InitializeValueInternal(const FBoolProperty & self,void * Dest) {
	self.InitializeValueInternal(Dest);
}

int _dasUnreal_virtual_394_GetMinAlignment(const FBoolProperty & self) {
	return self.GetMinAlignment();
}

void _dasUnreal_virtual_395_AppendSchemaHash(const FBoolProperty & self,FBlake3 & Builder,bool bSkipEditorOnly) {
	self.AppendSchemaHash(Builder,bSkipEditorOnly);
}

bool _dasUnreal_static_396_GetDefaultPropertyValue(const FBoolProperty *) {
	return FBoolProperty::GetDefaultPropertyValue();
}

unsigned int _dasUnreal_virtual_397_GetValueTypeHashInternal(const FBoolProperty & self,const void * Src) {
	return self.GetValueTypeHashInternal(Src);
}

FFieldClass * _dasUnreal_static_398_StaticClass(const FObjectPropertyBase *) {
	return FObjectPropertyBase::StaticClass();
}

FField * _dasUnreal_static_399_Construct(const FObjectPropertyBase *,const FFieldVariant & InOwner,const FName & InName,EObjectFlags InObjectFlags) {
	return FObjectPropertyBase::Construct(InOwner,InName,InObjectFlags);
}

uint64 _dasUnreal_static_400_StaticClassCastFlagsPrivate(const FObjectPropertyBase *) {
	return FObjectPropertyBase::StaticClassCastFlagsPrivate();
}

uint64 _dasUnreal_static_401_StaticClassCastFlags(const FObjectPropertyBase *) {
	return FObjectPropertyBase::StaticClassCastFlags();
}

void _dasUnreal_virtual_402_AddReferencedObjects(FObjectPropertyBase & self,FReferenceCollector & Collector) {
	self.AddReferencedObjects(Collector);
}

void _dasUnreal_virtual_403_BeginDestroy(FObjectPropertyBase & self) {
	self.BeginDestroy();
}

void _dasUnreal_virtual_404_PostDuplicate(FObjectPropertyBase & self,const FField & InField) {
	self.PostDuplicate(InField);
}

bool _dasUnreal_virtual_405_Identical(const FObjectPropertyBase & self,const void * A,const void * B,unsigned int PortFlags) {
	return self.Identical(A,B,PortFlags);
}

bool _dasUnreal_virtual_406_SupportsNetSharedSerialization(const FObjectPropertyBase & self) {
	return self.SupportsNetSharedSerialization();
}

FName _dasUnreal_virtual_407_GetID(const FObjectPropertyBase & self) {
	return self.GetID();
}

bool _dasUnreal_virtual_408_SameType(const FObjectPropertyBase & self,const FProperty * Other) {
	return self.SameType(Other);
}

FString _dasUnreal_virtual_409_GetCPPTypeCustom(const FObjectPropertyBase & self,FString * ExtendedTypeText,unsigned int CPPExportFlags,const FString & InnerNativeTypeName) {
	return self.GetCPPTypeCustom(ExtendedTypeText,CPPExportFlags,InnerNativeTypeName);
}

bool _dasUnreal_static_410_StaticIdentical(const FObjectPropertyBase *,UObject * A,UObject * B,unsigned int PortFlags) {
	return FObjectPropertyBase::StaticIdentical(A,B,PortFlags);
}

UObject * _dasUnreal_virtual_411_LoadObjectPropertyValue(const FObjectPropertyBase & self,const void * PropertyValueAddress) {
	return self.LoadObjectPropertyValue(PropertyValueAddress);
}

UObject * _dasUnreal_virtual_412_GetObjectPropertyValue(const FObjectPropertyBase & self,const void * PropertyValueAddress) {
	return self.GetObjectPropertyValue(PropertyValueAddress);
}

UObject * _dasUnreal_virtual_413_GetObjectPropertyValue_InContainer(const FObjectPropertyBase & self,const void * ContainerAddress,int ArrayIndex) {
	return self.GetObjectPropertyValue_InContainer(ContainerAddress,ArrayIndex);
}

void _dasUnreal_virtual_414_SetObjectPropertyValue(const FObjectPropertyBase & self,void * PropertyValueAddress,UObject * Value) {
	self.SetObjectPropertyValue(PropertyValueAddress,Value);
}

void _dasUnreal_virtual_415_SetObjectPropertyValue_InContainer(const FObjectPropertyBase & self,void * ContainerAddress,UObject * Value,int ArrayIndex) {
	self.SetObjectPropertyValue_InContainer(ContainerAddress,Value,ArrayIndex);
}

FFieldClass * _dasUnreal_static_416_StaticClass(const FStructProperty *) {
	return FStructProperty::StaticClass();
}

FField * _dasUnreal_static_417_Construct(const FStructProperty *,const FFieldVariant & InOwner,const FName & InName,EObjectFlags InObjectFlags) {
	return FStructProperty::Construct(InOwner,InName,InObjectFlags);
}

uint64 _dasUnreal_static_418_StaticClassCastFlagsPrivate(const FStructProperty *) {
	return FStructProperty::StaticClassCastFlagsPrivate();
}

uint64 _dasUnreal_static_419_StaticClassCastFlags(const FStructProperty *) {
	return FStructProperty::StaticClassCastFlags();
}

void _dasUnreal_virtual_420_AddReferencedObjects(FStructProperty & self,FReferenceCollector & Collector) {
	self.AddReferencedObjects(Collector);
}

void _dasUnreal_virtual_421_PostDuplicate(FStructProperty & self,const FField & InField) {
	self.PostDuplicate(InField);
}

FString _dasUnreal_virtual_422_GetCPPMacroType(const FStructProperty & self,FString & ExtendedTypeText) {
	return self.GetCPPMacroType(ExtendedTypeText);
}

FString _dasUnreal_virtual_423_GetCPPType(const FStructProperty & self,FString * ExtendedTypeText,unsigned int CPPExportFlags) {
	return self.GetCPPType(ExtendedTypeText,CPPExportFlags);
}

FString _dasUnreal_virtual_424_GetCPPTypeForwardDeclaration(const FStructProperty & self) {
	return self.GetCPPTypeForwardDeclaration();
}

bool _dasUnreal_virtual_425_Identical(const FStructProperty & self,const void * A,const void * B,unsigned int PortFlags) {
	return self.Identical(A,B,PortFlags);
}

bool _dasUnreal_virtual_426_SupportsNetSharedSerialization(const FStructProperty & self) {
	return self.SupportsNetSharedSerialization();
}

void _dasUnreal_virtual_427_CopyValuesInternal(const FStructProperty & self,void * Dest,const void * Src,int Count) {
	self.CopyValuesInternal(Dest,Src,Count);
}

void _dasUnreal_virtual_428_ClearValueInternal(const FStructProperty & self,void * Data) {
	self.ClearValueInternal(Data);
}

void _dasUnreal_virtual_429_DestroyValueInternal(const FStructProperty & self,void * Dest) {
	self.DestroyValueInternal(Dest);
}

void _dasUnreal_virtual_430_InitializeValueInternal(const FStructProperty & self,void * Dest) {
	self.InitializeValueInternal(Dest);
}

int _dasUnreal_virtual_431_GetMinAlignment(const FStructProperty & self) {
	return self.GetMinAlignment();
}

bool _dasUnreal_virtual_432_SameType(const FStructProperty & self,const FProperty * Other) {
	return self.SameType(Other);
}

void _dasUnreal_virtual_433_AppendSchemaHash(const FStructProperty & self,FBlake3 & Builder,bool bSkipEditorOnly) {
	self.AppendSchemaHash(Builder,bSkipEditorOnly);
}

FProperty * _dasUnreal_static_434_GetNextPropertyAndAdvance(const FCustomPropertyListNode *,const FCustomPropertyListNode *& Node) {
	return FCustomPropertyListNode::GetNextPropertyAndAdvance(Node);
}

UClass * _dasUnreal_static_435_StaticClass(const UPropertyWrapper *) {
	return UPropertyWrapper::StaticClass();
}

const wchar_t * _dasUnreal_static_436_StaticPackage(const UPropertyWrapper *) {
	return UPropertyWrapper::StaticPackage();
}

void _dasUnreal_static_437_StaticRegisterNativesUPropertyWrapper(const UPropertyWrapper *) {
	UPropertyWrapper::StaticRegisterNativesUPropertyWrapper();
}

