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

UClass * _dasUnreal_static_19_StaticClass(const UObject &) {
	return UObject::StaticClass();
}

const wchar_t * _dasUnreal_static_20_StaticConfigName(const UObject &) {
	return UObject::StaticConfigName();
}

void _dasUnreal_static_21_StaticRegisterNativesUObject(const UObject &) {
	UObject::StaticRegisterNativesUObject();
}

void _dasUnreal_virtual_22_PostInitProperties(UObject & self) {
	self.PostInitProperties();
}

void _dasUnreal_virtual_23_PostReinitProperties(UObject & self) {
	self.PostReinitProperties();
}

void _dasUnreal_virtual_24_PostCDOContruct(UObject & self) {
	self.PostCDOContruct();
}

bool _dasUnreal_virtual_25_PreSaveRoot(UObject & self,const wchar_t * Filename) {
	return self.PreSaveRoot(Filename);
}

void _dasUnreal_virtual_26_PostSaveRoot(UObject & self,bool bCleanupIsRequired) {
	self.PostSaveRoot(bCleanupIsRequired);
}

bool _dasUnreal_virtual_27_Modify(UObject & self,bool bAlwaysMarkDirty) {
	return self.Modify(bAlwaysMarkDirty);
}

bool _dasUnreal_virtual_28_IsCapturingAsRootObjectForTransaction(const UObject & self) {
	return self.IsCapturingAsRootObjectForTransaction();
}

void _dasUnreal_virtual_29_LoadedFromAnotherClass(UObject & self,const FName & OldClassName) {
	self.LoadedFromAnotherClass(OldClassName);
}

bool _dasUnreal_virtual_30_ResolveSubobject(UObject & self,const wchar_t * SubObjectPath,UObject *& OutObject,bool bLoadIfExists) {
	return self.ResolveSubobject(SubObjectPath,OutObject,bLoadIfExists);
}

bool _dasUnreal_virtual_31_IsReadyForAsyncPostLoad(const UObject & self) {
	return self.IsReadyForAsyncPostLoad();
}

void _dasUnreal_virtual_32_PostLoad(UObject & self) {
	self.PostLoad();
}

void _dasUnreal_virtual_33_BeginDestroy(UObject & self) {
	self.BeginDestroy();
}

bool _dasUnreal_virtual_34_IsReadyForFinishDestroy(UObject & self) {
	return self.IsReadyForFinishDestroy();
}

void _dasUnreal_virtual_35_PostLinkerChange(UObject & self) {
	self.PostLinkerChange();
}

void _dasUnreal_virtual_36_FinishDestroy(UObject & self) {
	self.FinishDestroy();
}

void _dasUnreal_virtual_37_ShutdownAfterError(UObject & self) {
	self.ShutdownAfterError();
}

void _dasUnreal_virtual_38_PreEditUndo(UObject & self) {
	self.PreEditUndo();
}

void _dasUnreal_virtual_39_PostRename(UObject & self,UObject * OldOuter,const FName OldName) {
	self.PostRename(OldOuter,OldName);
}

bool _dasUnreal_virtual_40_NeedsLoadForClient(const UObject & self) {
	return self.NeedsLoadForClient();
}

bool _dasUnreal_virtual_41_NeedsLoadForServer(const UObject & self) {
	return self.NeedsLoadForServer();
}

bool _dasUnreal_virtual_42_NeedsLoadForEditorGame(const UObject & self) {
	return self.NeedsLoadForEditorGame();
}

bool _dasUnreal_virtual_43_IsEditorOnly(const UObject & self) {
	return self.IsEditorOnly();
}

bool _dasUnreal_virtual_44_HasNonEditorOnlyReferences(const UObject & self) {
	return self.HasNonEditorOnlyReferences();
}

bool _dasUnreal_virtual_45_IsPostLoadThreadSafe(const UObject & self) {
	return self.IsPostLoadThreadSafe();
}

bool _dasUnreal_virtual_46_IsDestructionThreadSafe(const UObject & self) {
	return self.IsDestructionThreadSafe();
}

bool _dasUnreal_static_47_CanCreateInCurrentContext(const UObject &,UObject * Template) {
	return UObject::CanCreateInCurrentContext(Template);
}

void _dasUnreal_virtual_48_PostEditImport(UObject & self) {
	self.PostEditImport();
}

bool _dasUnreal_virtual_49_Rename(UObject & self,const wchar_t * NewName,UObject * NewOuter,unsigned int Flags) {
	return self.Rename(NewName,NewOuter,Flags);
}

FString _dasUnreal_virtual_50_GetDesc(UObject & self) {
	return self.GetDesc();
}

void _dasUnreal_virtual_51_MoveDataToSparseClassDataStruct(const UObject & self) {
	self.MoveDataToSparseClassDataStruct();
}

bool _dasUnreal_virtual_52_ImplementsGetWorld(const UObject & self) {
	return self.ImplementsGetWorld();
}

FName _dasUnreal_virtual_53_GetExporterName(UObject & self) {
	return self.GetExporterName();
}

FRestoreForUObjectOverwrite * _dasUnreal_virtual_54_GetRestoreForUObjectOverwrite(UObject & self) {
	return self.GetRestoreForUObjectOverwrite();
}

bool _dasUnreal_virtual_55_AreNativePropertiesIdenticalTo(const UObject & self,UObject * Other) {
	return self.AreNativePropertiesIdenticalTo(Other);
}

const FName & _dasUnreal_static_56_SourceFileTagName(const UObject &) {
	return UObject::SourceFileTagName();
}

const FName & _dasUnreal_static_57_AssetVersePathTagName(const UObject &) {
	return UObject::AssetVersePathTagName();
}

bool _dasUnreal_virtual_58_IsAsset(const UObject & self) {
	return self.IsAsset();
}

bool _dasUnreal_virtual_59_IsLocalizedResource(const UObject & self) {
	return self.IsLocalizedResource();
}

bool _dasUnreal_virtual_60_IsSafeForRootSet(const UObject & self) {
	return self.IsSafeForRootSet();
}

void _dasUnreal_virtual_61_TagSubobjects(UObject & self,EObjectFlags NewFlags) {
	self.TagSubobjects(NewFlags);
}

bool _dasUnreal_virtual_62_IsNameStableForNetworking(const UObject & self) {
	return self.IsNameStableForNetworking();
}

bool _dasUnreal_virtual_63_IsFullNameStableForNetworking(const UObject & self) {
	return self.IsFullNameStableForNetworking();
}

bool _dasUnreal_virtual_64_IsSupportedForNetworking(const UObject & self) {
	return self.IsSupportedForNetworking();
}

void _dasUnreal_virtual_65_PreNetReceive(UObject & self) {
	self.PreNetReceive();
}

void _dasUnreal_virtual_66_PostNetReceive(UObject & self) {
	self.PostNetReceive();
}

void _dasUnreal_virtual_67_PostRepNotifies(UObject & self) {
	self.PostRepNotifies();
}

void _dasUnreal_virtual_68_PreDestroyFromReplication(UObject & self) {
	self.PreDestroyFromReplication();
}

void _dasUnreal_virtual_69_WillNeverCacheCookedPlatformDataAgain(UObject & self) {
	self.WillNeverCacheCookedPlatformDataAgain();
}

void _dasUnreal_virtual_70_ClearAllCachedCookedPlatformData(UObject & self) {
	self.ClearAllCachedCookedPlatformData();
}

const wchar_t * _dasUnreal_virtual_71_GetConfigOverridePlatform(const UObject & self) {
	return self.GetConfigOverridePlatform();
}

void _dasUnreal_virtual_72_OverrideConfigSection(UObject & self,FString & SectionName) {
	self.OverrideConfigSection(SectionName);
}

void _dasUnreal_virtual_73_OverridePerObjectConfigSection(UObject & self,FString & SectionName) {
	self.OverridePerObjectConfigSection(SectionName);
}

UObject * _dasUnreal_static_74_GetArchetypeFromRequiredInfo(const UObject &,const UClass * Class,const UObject * Outer,FName Name,EObjectFlags ObjectFlags) {
	return UObject::GetArchetypeFromRequiredInfo(Class,Outer,Name,ObjectFlags);
}

UClass * _dasUnreal_virtual_75_RegenerateClass(UObject & self,UClass * ClassToRegenerate,UObject * PreviousCDO) {
	return self.RegenerateClass(ClassToRegenerate,PreviousCDO);
}

void _dasUnreal_virtual_76_MarkAsEditorOnlySubobject(UObject & self) {
	self.MarkAsEditorOnlySubobject();
}

uint64 _dasUnreal_virtual_77_GetNetPushIdDynamic(const UObject & self) {
	return self.GetNetPushIdDynamic();
}

UClass * _dasUnreal_static_78_StaticClass(const UField &) {
	return UField::StaticClass();
}

const wchar_t * _dasUnreal_static_79_StaticPackage(const UField &) {
	return UField::StaticPackage();
}

void _dasUnreal_static_80_StaticRegisterNativesUField(const UField &) {
	UField::StaticRegisterNativesUField();
}

void _dasUnreal_virtual_81_PostLoad(UField & self) {
	self.PostLoad();
}

bool _dasUnreal_virtual_82_NeedsLoadForClient(const UField & self) {
	return self.NeedsLoadForClient();
}

bool _dasUnreal_virtual_83_NeedsLoadForServer(const UField & self) {
	return self.NeedsLoadForServer();
}

void _dasUnreal_virtual_84_Bind(UField & self) {
	self.Bind();
}

void _dasUnreal_static_85_FormatNativeToolTip(const UField &,FString & ToolTipString,bool bRemoveExtraSections) {
	UField::FormatNativeToolTip(ToolTipString,bRemoveExtraSections);
}

UClass * _dasUnreal_static_86_StaticClass(const UStruct &) {
	return UStruct::StaticClass();
}

const wchar_t * _dasUnreal_static_87_StaticPackage(const UStruct &) {
	return UStruct::StaticPackage();
}

void _dasUnreal_static_88_StaticRegisterNativesUStruct(const UStruct &) {
	UStruct::StaticRegisterNativesUStruct();
}

void _dasUnreal_virtual_89_PostLoad(UStruct & self) {
	self.PostLoad();
}

void _dasUnreal_virtual_90_FinishDestroy(UStruct & self) {
	self.FinishDestroy();
}

void _dasUnreal_virtual_91_RegisterDependencies(UStruct & self) {
	self.RegisterDependencies();
}

void _dasUnreal_virtual_92_TagSubobjects(UStruct & self,EObjectFlags NewFlags) {
	self.TagSubobjects(NewFlags);
}

UStruct * _dasUnreal_virtual_93_GetInheritanceSuper(const UStruct & self) {
	return self.GetInheritanceSuper();
}

void _dasUnreal_virtual_94_InitializeStruct(const UStruct & self,void * Dest,int ArrayDim) {
	self.InitializeStruct(Dest,ArrayDim);
}

void _dasUnreal_virtual_95_DestroyStruct(const UStruct & self,void * Dest,int ArrayDim) {
	self.DestroyStruct(Dest,ArrayDim);
}

const wchar_t * _dasUnreal_virtual_96_GetPrefixCPP(const UStruct & self) {
	return self.GetPrefixCPP();
}

void _dasUnreal_virtual_97_SetSuperStruct(UStruct & self,UStruct * NewSuperStruct) {
	self.SetSuperStruct(NewSuperStruct);
}

FString _dasUnreal_virtual_98_GetAuthoredNameForField(const UStruct & self,const UField * Field) {
	return self.GetAuthoredNameForField(Field);
}

bool _dasUnreal_virtual_99_IsStructTrashed(const UStruct & self) {
	return self.IsStructTrashed();
}

UClass * _dasUnreal_static_100_StaticClass(const UEnum &) {
	return UEnum::StaticClass();
}

const wchar_t * _dasUnreal_static_101_StaticPackage(const UEnum &) {
	return UEnum::StaticPackage();
}

void _dasUnreal_static_102_StaticRegisterNativesUEnum(const UEnum &) {
	UEnum::StaticRegisterNativesUEnum();
}

FText _dasUnreal_virtual_103_GetDisplayNameTextByIndex(const UEnum & self,int InIndex) {
	return self.GetDisplayNameTextByIndex(InIndex);
}

FString _dasUnreal_virtual_104_GetAuthoredNameStringByIndex(const UEnum & self,int InIndex) {
	return self.GetAuthoredNameStringByIndex(InIndex);
}

bool _dasUnreal_static_105_IsFullEnumName(const UEnum &,const wchar_t * InEnumName) {
	return UEnum::IsFullEnumName(InEnumName);
}

FString _dasUnreal_virtual_106_GenerateFullEnumName(const UEnum & self,const wchar_t * InEnumName) {
	return self.GenerateFullEnumName(InEnumName);
}

int64 _dasUnreal_static_107_ParseEnum(const UEnum &,const wchar_t *& Str) {
	return UEnum::ParseEnum(Str);
}

void _dasUnreal_virtual_108_BeginDestroy(UEnum & self) {
	self.BeginDestroy();
}

bool _dasUnreal_virtual_109_IsAbstract(const FCppClassTypeInfo & self) {
	return self.IsAbstract();
}

UClass * _dasUnreal_static_110_StaticClass(const UClass &) {
	return UClass::StaticClass();
}

const wchar_t * _dasUnreal_static_111_StaticPackage(const UClass &) {
	return UClass::StaticPackage();
}

void _dasUnreal_static_112_StaticRegisterNativesUClass(const UClass &) {
	UClass::StaticRegisterNativesUClass();
}

void _dasUnreal_virtual_113_ConditionalRecompileClass(UClass & self,FUObjectSerializeContext * InLoadContext) {
	self.ConditionalRecompileClass(InLoadContext);
}

void _dasUnreal_virtual_114_FlushCompilationQueueForLevel(UClass & self) {
	self.FlushCompilationQueueForLevel();
}

void _dasUnreal_static_115_AssembleReferenceTokenStreams(const UClass &) {
	UClass::AssembleReferenceTokenStreams();
}

UClass * _dasUnreal_virtual_116_GetAuthoritativeClass(UClass & self) {
	return self.GetAuthoritativeClass();
}

void _dasUnreal_virtual_117_PostLoad(UClass & self) {
	self.PostLoad();
}

void _dasUnreal_virtual_118_FinishDestroy(UClass & self) {
	self.FinishDestroy();
}

void _dasUnreal_virtual_119_DeferredRegister(UClass & self,UClass * UClassStaticClass,const wchar_t * PackageName,const wchar_t * InName) {
	self.DeferredRegister(UClassStaticClass,PackageName,InName);
}

bool _dasUnreal_virtual_120_Rename(UClass & self,const wchar_t * NewName,UObject * NewOuter,unsigned int Flags) {
	return self.Rename(NewName,NewOuter,Flags);
}

void _dasUnreal_virtual_121_TagSubobjects(UClass & self,EObjectFlags NewFlags) {
	self.TagSubobjects(NewFlags);
}

void _dasUnreal_virtual_122_PostInitProperties(UClass & self) {
	self.PostInitProperties();
}

FRestoreForUObjectOverwrite * _dasUnreal_virtual_123_GetRestoreForUObjectOverwrite(UClass & self) {
	return self.GetRestoreForUObjectOverwrite();
}

FString _dasUnreal_virtual_124_GetDesc(UClass & self) {
	return self.GetDesc();
}

bool _dasUnreal_virtual_125_IsAsset(const UClass & self) {
	return self.IsAsset();
}

bool _dasUnreal_virtual_126_IsNameStableForNetworking(const UClass & self) {
	return self.IsNameStableForNetworking();
}

void _dasUnreal_virtual_127_Bind(UClass & self) {
	self.Bind();
}

const wchar_t * _dasUnreal_virtual_128_GetPrefixCPP(const UClass & self) {
	return self.GetPrefixCPP();
}

void _dasUnreal_virtual_129_SetSuperStruct(UClass & self,UStruct * NewSuperStruct) {
	self.SetSuperStruct(NewSuperStruct);
}

bool _dasUnreal_virtual_130_IsStructTrashed(const UClass & self) {
	return self.IsStructTrashed();
}

void _dasUnreal_virtual_131_PostLoadInstance(UClass & self,UObject * InObj) {
	self.PostLoadInstance(InObj);
}

void _dasUnreal_virtual_132_InitPropertiesFromCustomList(UClass & self,unsigned char * DataPtr,const unsigned char * DefaultDataPtr) {
	self.InitPropertiesFromCustomList(DataPtr,DefaultDataPtr);
}

bool _dasUnreal_virtual_133_CanCreateAssetOfClass(const UClass & self) {
	return self.CanCreateAssetOfClass();
}

void _dasUnreal_virtual_134_CreatePersistentUberGraphFrame(const UClass & self,UObject * Obj,bool bCreateOnlyIfEmpty,bool bSkipSuperClass,UClass * OldClass) {
	self.CreatePersistentUberGraphFrame(Obj,bCreateOnlyIfEmpty,bSkipSuperClass,OldClass);
}

void _dasUnreal_virtual_135_DestroyPersistentUberGraphFrame(const UClass & self,UObject * Obj,bool bSkipSuperClass) {
	self.DestroyPersistentUberGraphFrame(Obj,bSkipSuperClass);
}

void _dasUnreal_virtual_136_PostLoadDefaultObject(UClass & self,UObject * Object) {
	self.PostLoadDefaultObject(Object);
}

void _dasUnreal_virtual_137_PurgeClass(UClass & self,bool bRecompilingOnLoad) {
	self.PurgeClass(bRecompilingOnLoad);
}

UClass * _dasUnreal_static_138_FindCommonBase(const UClass &,UClass * InClassA,UClass * InClassB) {
	return UClass::FindCommonBase(InClassA,InClassB);
}

bool _dasUnreal_virtual_139_IsFunctionImplementedInScript(const UClass & self,FName InFunctionName) {
	return self.IsFunctionImplementedInScript(InFunctionName);
}

UObject * _dasUnreal_virtual_140_FindArchetype(const UClass & self,const UClass * ArchetypeClass,const FName ArchetypeName) {
	return self.FindArchetype(ArchetypeClass,ArchetypeName);
}

UObject * _dasUnreal_virtual_141_GetArchetypeForCDO(const UClass & self) {
	return self.GetArchetypeForCDO();
}

bool _dasUnreal_static_142_IsSafeToSerializeToStructuredArchives(const UClass &,UClass * InClass) {
	return UClass::IsSafeToSerializeToStructuredArchives(InClass);
}

UClass * _dasUnreal_static_143_StaticClass(const UPackage &) {
	return UPackage::StaticClass();
}

const wchar_t * _dasUnreal_static_144_StaticPackage(const UPackage &) {
	return UPackage::StaticPackage();
}

void _dasUnreal_static_145_StaticRegisterNativesUPackage(const UPackage &) {
	UPackage::StaticRegisterNativesUPackage();
}

bool _dasUnreal_virtual_146_IsNameStableForNetworking(const UPackage & self) {
	return self.IsNameStableForNetworking();
}

bool _dasUnreal_virtual_147_NeedsLoadForClient(const UPackage & self) {
	return self.NeedsLoadForClient();
}

bool _dasUnreal_virtual_148_NeedsLoadForServer(const UPackage & self) {
	return self.NeedsLoadForServer();
}

bool _dasUnreal_virtual_149_IsPostLoadThreadSafe(const UPackage & self) {
	return self.IsPostLoadThreadSafe();
}

bool _dasUnreal_virtual_150_IsDestructionThreadSafe(const UPackage & self) {
	return self.IsDestructionThreadSafe();
}

void _dasUnreal_virtual_151_PostInitProperties(UPackage & self) {
	self.PostInitProperties();
}

void _dasUnreal_virtual_152_FinishDestroy(UPackage & self) {
	self.FinishDestroy();
}

bool _dasUnreal_virtual_153_IsAsset(const UPackage & self) {
	return self.IsAsset();
}

void _dasUnreal_virtual_154_TagSubobjects(UPackage & self,EObjectFlags NewFlags) {
	self.TagSubobjects(NewFlags);
}

void _dasUnreal_static_155_WaitForAsyncFileWrites(const UPackage &) {
	UPackage::WaitForAsyncFileWrites();
}

bool _dasUnreal_static_156_HasAsyncFileWrites(const UPackage &) {
	return UPackage::HasAsyncFileWrites();
}

bool _dasUnreal_static_157_IsEmptyPackage(const UPackage &,UPackage * Package,const UObject * LastReferencer) {
	return UPackage::IsEmptyPackage(Package,LastReferencer);
}

