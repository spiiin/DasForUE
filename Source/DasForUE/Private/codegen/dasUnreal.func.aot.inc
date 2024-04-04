// this file is generated via daScript automatic C++ binder
// all user modifications will be lost after this file is re-generated

FString _dasUnreal_static_0_Chr(const FString &,wchar_t Ch) {
	return FString::Chr(Ch);
}

FString _dasUnreal_static_1_ChrN(const FString &,int NumCharacters,wchar_t Char) {
	return FString::ChrN(NumCharacters,Char);
}

FString _dasUnreal_static_2_FormatAsNumber(const FString &,int InNumber) {
	return FString::FormatAsNumber(InNumber);
}

FString _dasUnreal_static_3_FromInt(const FString &,int Num) {
	return FString::FromInt(Num);
}

FString _dasUnreal_static_4_FromBlob(const FString &,const unsigned char * SrcBuffer,const unsigned int SrcSize) {
	return FString::FromBlob(SrcBuffer,SrcSize);
}

bool _dasUnreal_static_5_ToBlob(const FString &,const FString & Source,unsigned char * DestBuffer,const unsigned int DestSize) {
	return FString::ToBlob(Source,DestBuffer,DestSize);
}

FString _dasUnreal_static_6_FromHexBlob(const FString &,const unsigned char * SrcBuffer,const unsigned int SrcSize) {
	return FString::FromHexBlob(SrcBuffer,SrcSize);
}

bool _dasUnreal_static_7_ToHexBlob(const FString &,const FString & Source,unsigned char * DestBuffer,const unsigned int DestSize) {
	return FString::ToHexBlob(Source,DestBuffer,DestSize);
}

FString _dasUnreal_static_8_SanitizeFloat(const FString &,double InFloat,const int InMinFractionalDigits) {
	return FString::SanitizeFloat(InFloat,InMinFractionalDigits);
}

FName _dasUnreal_virtual_9_GetFNameForStatID(const UObjectBase & self) {
	return self.GetFNameForStatID();
}

FString _dasUnreal_static_10_RemoveClassPrefix(const UObjectBase &,const wchar_t * ClassName) {
	return UObjectBase::RemoveClassPrefix(ClassName);
}

void _dasUnreal_static_11_PrefetchClass(const UObjectBase &,UObject * Object) {
	UObjectBase::PrefetchClass(Object);
}

void _dasUnreal_static_12_PrefetchOuter(const UObjectBase &,UObject * Object) {
	UObjectBase::PrefetchOuter(Object);
}

bool _dasUnreal_static_13_IsPendingKillEnabled(const UObjectBaseUtility &) {
	return UObjectBaseUtility::IsPendingKillEnabled();
}

EInternalObjectFlags _dasUnreal_static_14_FixGarbageOrPendingKillInternalObjectFlags(const UObjectBaseUtility &,const EInternalObjectFlags InFlags) {
	return UObjectBaseUtility::FixGarbageOrPendingKillInternalObjectFlags(InFlags);
}

bool _dasUnreal_virtual_15_CanBeClusterRoot(const UObjectBaseUtility & self) {
	return self.CanBeClusterRoot();
}

bool _dasUnreal_virtual_16_CanBeInCluster(const UObjectBaseUtility & self) {
	return self.CanBeInCluster();
}

void _dasUnreal_virtual_17_CreateCluster(UObjectBaseUtility & self) {
	self.CreateCluster();
}

void _dasUnreal_virtual_18_OnClusterMarkedAsPendingKill(UObjectBaseUtility & self) {
	self.OnClusterMarkedAsPendingKill();
}

const wchar_t * _dasUnreal_static_19_StaticConfigName(const UObject &) {
	return UObject::StaticConfigName();
}

void _dasUnreal_static_20_StaticRegisterNativesUObject(const UObject &) {
	UObject::StaticRegisterNativesUObject();
}

void _dasUnreal_virtual_21_PostInitProperties(UObject & self) {
	self.PostInitProperties();
}

void _dasUnreal_virtual_22_PostReinitProperties(UObject & self) {
	self.PostReinitProperties();
}

void _dasUnreal_virtual_23_PostCDOContruct(UObject & self) {
	self.PostCDOContruct();
}

bool _dasUnreal_virtual_24_PreSaveRoot(UObject & self,const wchar_t * Filename) {
	return self.PreSaveRoot(Filename);
}

void _dasUnreal_virtual_25_PostSaveRoot(UObject & self,bool bCleanupIsRequired) {
	self.PostSaveRoot(bCleanupIsRequired);
}

bool _dasUnreal_virtual_26_Modify(UObject & self,bool bAlwaysMarkDirty) {
	return self.Modify(bAlwaysMarkDirty);
}

bool _dasUnreal_virtual_27_IsCapturingAsRootObjectForTransaction(const UObject & self) {
	return self.IsCapturingAsRootObjectForTransaction();
}

void _dasUnreal_virtual_28_LoadedFromAnotherClass(UObject & self,const FName & OldClassName) {
	self.LoadedFromAnotherClass(OldClassName);
}

bool _dasUnreal_virtual_29_ResolveSubobject(UObject & self,const wchar_t * SubObjectPath,UObject *& OutObject,bool bLoadIfExists) {
	return self.ResolveSubobject(SubObjectPath,OutObject,bLoadIfExists);
}

bool _dasUnreal_virtual_30_IsReadyForAsyncPostLoad(const UObject & self) {
	return self.IsReadyForAsyncPostLoad();
}

void _dasUnreal_virtual_31_PostLoad(UObject & self) {
	self.PostLoad();
}

void _dasUnreal_virtual_32_BeginDestroy(UObject & self) {
	self.BeginDestroy();
}

bool _dasUnreal_virtual_33_IsReadyForFinishDestroy(UObject & self) {
	return self.IsReadyForFinishDestroy();
}

void _dasUnreal_virtual_34_PostLinkerChange(UObject & self) {
	self.PostLinkerChange();
}

void _dasUnreal_virtual_35_FinishDestroy(UObject & self) {
	self.FinishDestroy();
}

void _dasUnreal_virtual_36_ShutdownAfterError(UObject & self) {
	self.ShutdownAfterError();
}

void _dasUnreal_virtual_37_PreEditUndo(UObject & self) {
	self.PreEditUndo();
}

void _dasUnreal_virtual_38_PostRename(UObject & self,UObject * OldOuter,const FName OldName) {
	self.PostRename(OldOuter,OldName);
}

bool _dasUnreal_virtual_39_NeedsLoadForClient(const UObject & self) {
	return self.NeedsLoadForClient();
}

bool _dasUnreal_virtual_40_NeedsLoadForServer(const UObject & self) {
	return self.NeedsLoadForServer();
}

bool _dasUnreal_virtual_41_NeedsLoadForEditorGame(const UObject & self) {
	return self.NeedsLoadForEditorGame();
}

bool _dasUnreal_virtual_42_IsEditorOnly(const UObject & self) {
	return self.IsEditorOnly();
}

bool _dasUnreal_virtual_43_HasNonEditorOnlyReferences(const UObject & self) {
	return self.HasNonEditorOnlyReferences();
}

bool _dasUnreal_virtual_44_IsPostLoadThreadSafe(const UObject & self) {
	return self.IsPostLoadThreadSafe();
}

bool _dasUnreal_virtual_45_IsDestructionThreadSafe(const UObject & self) {
	return self.IsDestructionThreadSafe();
}

bool _dasUnreal_static_46_CanCreateInCurrentContext(const UObject &,UObject * Template) {
	return UObject::CanCreateInCurrentContext(Template);
}

void _dasUnreal_virtual_47_PostEditImport(UObject & self) {
	self.PostEditImport();
}

bool _dasUnreal_virtual_48_Rename(UObject & self,const wchar_t * NewName,UObject * NewOuter,unsigned int Flags) {
	return self.Rename(NewName,NewOuter,Flags);
}

FString _dasUnreal_virtual_49_GetDesc(UObject & self) {
	return self.GetDesc();
}

void _dasUnreal_virtual_50_MoveDataToSparseClassDataStruct(const UObject & self) {
	self.MoveDataToSparseClassDataStruct();
}

bool _dasUnreal_virtual_51_ImplementsGetWorld(const UObject & self) {
	return self.ImplementsGetWorld();
}

FName _dasUnreal_virtual_52_GetExporterName(UObject & self) {
	return self.GetExporterName();
}

FRestoreForUObjectOverwrite * _dasUnreal_virtual_53_GetRestoreForUObjectOverwrite(UObject & self) {
	return self.GetRestoreForUObjectOverwrite();
}

bool _dasUnreal_virtual_54_AreNativePropertiesIdenticalTo(const UObject & self,UObject * Other) {
	return self.AreNativePropertiesIdenticalTo(Other);
}

const FName & _dasUnreal_static_55_SourceFileTagName(const UObject &) {
	return UObject::SourceFileTagName();
}

const FName & _dasUnreal_static_56_AssetVersePathTagName(const UObject &) {
	return UObject::AssetVersePathTagName();
}

bool _dasUnreal_virtual_57_IsAsset(const UObject & self) {
	return self.IsAsset();
}

bool _dasUnreal_virtual_58_IsLocalizedResource(const UObject & self) {
	return self.IsLocalizedResource();
}

bool _dasUnreal_virtual_59_IsSafeForRootSet(const UObject & self) {
	return self.IsSafeForRootSet();
}

void _dasUnreal_virtual_60_TagSubobjects(UObject & self,EObjectFlags NewFlags) {
	self.TagSubobjects(NewFlags);
}

bool _dasUnreal_virtual_61_IsNameStableForNetworking(const UObject & self) {
	return self.IsNameStableForNetworking();
}

bool _dasUnreal_virtual_62_IsFullNameStableForNetworking(const UObject & self) {
	return self.IsFullNameStableForNetworking();
}

bool _dasUnreal_virtual_63_IsSupportedForNetworking(const UObject & self) {
	return self.IsSupportedForNetworking();
}

void _dasUnreal_virtual_64_PreNetReceive(UObject & self) {
	self.PreNetReceive();
}

void _dasUnreal_virtual_65_PostNetReceive(UObject & self) {
	self.PostNetReceive();
}

void _dasUnreal_virtual_66_PostRepNotifies(UObject & self) {
	self.PostRepNotifies();
}

void _dasUnreal_virtual_67_PreDestroyFromReplication(UObject & self) {
	self.PreDestroyFromReplication();
}

void _dasUnreal_virtual_68_WillNeverCacheCookedPlatformDataAgain(UObject & self) {
	self.WillNeverCacheCookedPlatformDataAgain();
}

void _dasUnreal_virtual_69_ClearAllCachedCookedPlatformData(UObject & self) {
	self.ClearAllCachedCookedPlatformData();
}

const wchar_t * _dasUnreal_virtual_70_GetConfigOverridePlatform(const UObject & self) {
	return self.GetConfigOverridePlatform();
}

void _dasUnreal_virtual_71_OverrideConfigSection(UObject & self,FString & SectionName) {
	self.OverrideConfigSection(SectionName);
}

void _dasUnreal_virtual_72_OverridePerObjectConfigSection(UObject & self,FString & SectionName) {
	self.OverridePerObjectConfigSection(SectionName);
}

UObject * _dasUnreal_static_73_GetArchetypeFromRequiredInfo(const UObject &,const UClass * Class,const UObject * Outer,FName Name,EObjectFlags ObjectFlags) {
	return UObject::GetArchetypeFromRequiredInfo(Class,Outer,Name,ObjectFlags);
}

void _dasUnreal_virtual_74_MarkAsEditorOnlySubobject(UObject & self) {
	self.MarkAsEditorOnlySubobject();
}

uint64 _dasUnreal_virtual_75_GetNetPushIdDynamic(const UObject & self) {
	return self.GetNetPushIdDynamic();
}

