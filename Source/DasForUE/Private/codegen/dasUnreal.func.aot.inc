// this file is generated via daScript automatic C++ binder
// all user modifications will be lost after this file is re-generated

FString _dasUnreal_static_0_Chr(const FString *,wchar_t Ch) {
	return FString::Chr(Ch);
}

FString _dasUnreal_static_1_ChrN(const FString *,int NumCharacters,wchar_t Char) {
	return FString::ChrN(NumCharacters,Char);
}

FString _dasUnreal_static_2_FormatAsNumber(const FString *,int InNumber) {
	return FString::FormatAsNumber(InNumber);
}

FString _dasUnreal_static_3_FromInt(const FString *,int Num) {
	return FString::FromInt(Num);
}

FString _dasUnreal_static_4_FromBlob(const FString *,const unsigned char * SrcBuffer,const unsigned int SrcSize) {
	return FString::FromBlob(SrcBuffer,SrcSize);
}

bool _dasUnreal_static_5_ToBlob(const FString *,const FString & Source,unsigned char * DestBuffer,const unsigned int DestSize) {
	return FString::ToBlob(Source,DestBuffer,DestSize);
}

FString _dasUnreal_static_6_FromHexBlob(const FString *,const unsigned char * SrcBuffer,const unsigned int SrcSize) {
	return FString::FromHexBlob(SrcBuffer,SrcSize);
}

bool _dasUnreal_static_7_ToHexBlob(const FString *,const FString & Source,unsigned char * DestBuffer,const unsigned int DestSize) {
	return FString::ToHexBlob(Source,DestBuffer,DestSize);
}

FString _dasUnreal_static_8_SanitizeFloat(const FString *,double InFloat,const int InMinFractionalDigits) {
	return FString::SanitizeFloat(InFloat,InMinFractionalDigits);
}

void _dasUnreal_static_9_AssertIfInConstructor(const FObjectInitializer *,UObject * Outer,const wchar_t * ErrorMessage) {
	FObjectInitializer::AssertIfInConstructor(Outer,ErrorMessage);
}

FObjectInitializer & _dasUnreal_static_10_Get(const FObjectInitializer *) {
	return FObjectInitializer::Get();
}

void _dasUnreal_static_11_InitProperties(const FScriptIntegrationObjectHelper *,const FObjectInitializer & ObjectInitializer,UObject * Obj,UClass * DefaultsClass,UObject * DefaultData) {
	FScriptIntegrationObjectHelper::InitProperties(ObjectInitializer,Obj,DefaultsClass,DefaultData);
}

bool _dasUnreal_static_12_InitSubobjectProperties(const FScriptIntegrationObjectHelper *,const FObjectInitializer & ObjectInitializer) {
	return FScriptIntegrationObjectHelper::InitSubobjectProperties(ObjectInitializer);
}

void _dasUnreal_static_13_InstanceSubobjects(const FScriptIntegrationObjectHelper *,const FObjectInitializer & ObjectInitializer,UClass * Class,bool bNeedInstancing,bool bNeedSubobjectInstancing) {
	FScriptIntegrationObjectHelper::InstanceSubobjects(ObjectInitializer,Class,bNeedInstancing,bNeedSubobjectInstancing);
}

void _dasUnreal_static_14_PostConstructInitObject(const FScriptIntegrationObjectHelper *,FObjectInitializer & ObjectInitializer) {
	FScriptIntegrationObjectHelper::PostConstructInitObject(ObjectInitializer);
}

void _dasUnreal_virtual_15_AddStableReference(FReferenceCollector & self,UObject ** Object) {
	self.AddStableReference(Object);
}

bool _dasUnreal_virtual_16_NeedsPropertyReferencer(const FReferenceCollector & self) {
	return self.NeedsPropertyReferencer();
}

void _dasUnreal_virtual_17_AllowEliminatingReferences(FReferenceCollector & self,bool bAllow) {
	self.AllowEliminatingReferences(bAllow);
}

void _dasUnreal_virtual_18_SetSerializedProperty(FReferenceCollector & self,FProperty * Inproperty) {
	self.SetSerializedProperty(Inproperty);
}

FProperty * _dasUnreal_virtual_19_GetSerializedProperty(const FReferenceCollector & self) {
	return self.GetSerializedProperty();
}

bool _dasUnreal_virtual_20_MarkWeakObjectReferenceForClearing(FReferenceCollector & self,UObject ** WeakReference) {
	return self.MarkWeakObjectReferenceForClearing(WeakReference);
}

void _dasUnreal_virtual_21_SetIsProcessingNativeReferences(FReferenceCollector & self,bool bIsNative) {
	self.SetIsProcessingNativeReferences(bIsNative);
}

bool _dasUnreal_virtual_22_IsProcessingNativeReferences(const FReferenceCollector & self) {
	return self.IsProcessingNativeReferences();
}

bool _dasUnreal_virtual_23_NeedsInitialReferences(const FReferenceCollector & self) {
	return self.NeedsInitialReferences();
}

void _dasUnreal_virtual_24_FindReferences(FReferenceFinder & self,UObject * Object,UObject * ReferencingObject,FProperty * ReferencingProperty) {
	self.FindReferences(Object,ReferencingObject,ReferencingProperty);
}

bool _dasUnreal_virtual_25_IsIgnoringArchetypeRef(const FReferenceFinder & self) {
	return self.IsIgnoringArchetypeRef();
}

bool _dasUnreal_virtual_26_IsIgnoringTransient(const FReferenceFinder & self) {
	return self.IsIgnoringTransient();
}

void _dasUnreal_virtual_27_SetSerializedProperty(FReferenceFinder & self,FProperty * Inproperty) {
	self.SetSerializedProperty(Inproperty);
}

FProperty * _dasUnreal_virtual_28_GetSerializedProperty(const FReferenceFinder & self) {
	return self.GetSerializedProperty();
}

void _dasUnreal_static_29_BroadcastOnObjectModified(const FCoreUObjectDelegates *,UObject * Object) {
	FCoreUObjectDelegates::BroadcastOnObjectModified(Object);
}

FString _dasUnreal_static_30_FormatPathForAssetLog(const FAssetMsg *,const wchar_t * Path) {
	return FAssetMsg::FormatPathForAssetLog(Path);
}

FString _dasUnreal_static_31_FormatPathForAssetLog(const FAssetMsg *,const UObject * Object) {
	return FAssetMsg::FormatPathForAssetLog(Object);
}

FString _dasUnreal_static_32_GetAssetLogString(const FAssetMsg *,const wchar_t * Path,const FString & Message) {
	return FAssetMsg::GetAssetLogString(Path,Message);
}

FString _dasUnreal_static_33_GetAssetLogString(const FAssetMsg *,const UObject * Object,const FString & Message) {
	return FAssetMsg::GetAssetLogString(Object,Message);
}

FName _dasUnreal_virtual_34_GetFNameForStatID(const UObjectBase & self) {
	return self.GetFNameForStatID();
}

FString _dasUnreal_static_35_RemoveClassPrefix(const UObjectBase *,const wchar_t * ClassName) {
	return UObjectBase::RemoveClassPrefix(ClassName);
}

void _dasUnreal_static_36_PrefetchClass(const UObjectBase *,UObject * Object) {
	UObjectBase::PrefetchClass(Object);
}

void _dasUnreal_static_37_PrefetchOuter(const UObjectBase *,UObject * Object) {
	UObjectBase::PrefetchOuter(Object);
}

bool _dasUnreal_static_38_IsPendingKillEnabled(const UObjectBaseUtility *) {
	return UObjectBaseUtility::IsPendingKillEnabled();
}

EInternalObjectFlags _dasUnreal_static_39_FixGarbageOrPendingKillInternalObjectFlags(const UObjectBaseUtility *,const EInternalObjectFlags InFlags) {
	return UObjectBaseUtility::FixGarbageOrPendingKillInternalObjectFlags(InFlags);
}

bool _dasUnreal_virtual_40_CanBeClusterRoot(const UObjectBaseUtility & self) {
	return self.CanBeClusterRoot();
}

bool _dasUnreal_virtual_41_CanBeInCluster(const UObjectBaseUtility & self) {
	return self.CanBeInCluster();
}

void _dasUnreal_virtual_42_CreateCluster(UObjectBaseUtility & self) {
	self.CreateCluster();
}

void _dasUnreal_virtual_43_OnClusterMarkedAsPendingKill(UObjectBaseUtility & self) {
	self.OnClusterMarkedAsPendingKill();
}

UClass * _dasUnreal_static_44_StaticClass(const UObject *) {
	return UObject::StaticClass();
}

const wchar_t * _dasUnreal_static_45_StaticConfigName(const UObject *) {
	return UObject::StaticConfigName();
}

void _dasUnreal_static_46_StaticRegisterNativesUObject(const UObject *) {
	UObject::StaticRegisterNativesUObject();
}

void _dasUnreal_virtual_47_PostInitProperties(UObject & self) {
	self.PostInitProperties();
}

void _dasUnreal_virtual_48_PostReinitProperties(UObject & self) {
	self.PostReinitProperties();
}

void _dasUnreal_virtual_49_PostCDOContruct(UObject & self) {
	self.PostCDOContruct();
}

bool _dasUnreal_virtual_50_PreSaveRoot(UObject & self,const wchar_t * Filename) {
	return self.PreSaveRoot(Filename);
}

void _dasUnreal_virtual_51_PostSaveRoot(UObject & self,bool bCleanupIsRequired) {
	self.PostSaveRoot(bCleanupIsRequired);
}

bool _dasUnreal_virtual_52_Modify(UObject & self,bool bAlwaysMarkDirty) {
	return self.Modify(bAlwaysMarkDirty);
}

bool _dasUnreal_virtual_53_IsCapturingAsRootObjectForTransaction(const UObject & self) {
	return self.IsCapturingAsRootObjectForTransaction();
}

void _dasUnreal_virtual_54_LoadedFromAnotherClass(UObject & self,const FName & OldClassName) {
	self.LoadedFromAnotherClass(OldClassName);
}

bool _dasUnreal_virtual_55_ResolveSubobject(UObject & self,const wchar_t * SubObjectPath,UObject *& OutObject,bool bLoadIfExists) {
	return self.ResolveSubobject(SubObjectPath,OutObject,bLoadIfExists);
}

bool _dasUnreal_virtual_56_IsReadyForAsyncPostLoad(const UObject & self) {
	return self.IsReadyForAsyncPostLoad();
}

void _dasUnreal_virtual_57_PostLoad(UObject & self) {
	self.PostLoad();
}

void _dasUnreal_virtual_58_BeginDestroy(UObject & self) {
	self.BeginDestroy();
}

bool _dasUnreal_virtual_59_IsReadyForFinishDestroy(UObject & self) {
	return self.IsReadyForFinishDestroy();
}

void _dasUnreal_virtual_60_PostLinkerChange(UObject & self) {
	self.PostLinkerChange();
}

void _dasUnreal_virtual_61_FinishDestroy(UObject & self) {
	self.FinishDestroy();
}

void _dasUnreal_virtual_62_ShutdownAfterError(UObject & self) {
	self.ShutdownAfterError();
}

void _dasUnreal_virtual_63_PostInterpChange(UObject & self,FProperty * PropertyThatChanged) {
	self.PostInterpChange(PropertyThatChanged);
}

void _dasUnreal_virtual_64_PreEditUndo(UObject & self) {
	self.PreEditUndo();
}

void _dasUnreal_virtual_65_PostRename(UObject & self,UObject * OldOuter,const FName OldName) {
	self.PostRename(OldOuter,OldName);
}

bool _dasUnreal_virtual_66_NeedsLoadForClient(const UObject & self) {
	return self.NeedsLoadForClient();
}

bool _dasUnreal_virtual_67_NeedsLoadForServer(const UObject & self) {
	return self.NeedsLoadForServer();
}

bool _dasUnreal_virtual_68_NeedsLoadForEditorGame(const UObject & self) {
	return self.NeedsLoadForEditorGame();
}

bool _dasUnreal_virtual_69_IsEditorOnly(const UObject & self) {
	return self.IsEditorOnly();
}

bool _dasUnreal_virtual_70_HasNonEditorOnlyReferences(const UObject & self) {
	return self.HasNonEditorOnlyReferences();
}

bool _dasUnreal_virtual_71_IsPostLoadThreadSafe(const UObject & self) {
	return self.IsPostLoadThreadSafe();
}

bool _dasUnreal_virtual_72_IsDestructionThreadSafe(const UObject & self) {
	return self.IsDestructionThreadSafe();
}

bool _dasUnreal_static_73_CanCreateInCurrentContext(const UObject *,UObject * Template) {
	return UObject::CanCreateInCurrentContext(Template);
}

void _dasUnreal_virtual_74_PostEditImport(UObject & self) {
	self.PostEditImport();
}

void _dasUnreal_virtual_75_PostReloadConfig(UObject & self,FProperty * PropertyThatWasLoaded) {
	self.PostReloadConfig(PropertyThatWasLoaded);
}

bool _dasUnreal_virtual_76_Rename(UObject & self,const wchar_t * NewName,UObject * NewOuter,unsigned int Flags) {
	return self.Rename(NewName,NewOuter,Flags);
}

FString _dasUnreal_virtual_77_GetDesc(UObject & self) {
	return self.GetDesc();
}

void _dasUnreal_virtual_78_MoveDataToSparseClassDataStruct(const UObject & self) {
	self.MoveDataToSparseClassDataStruct();
}

bool _dasUnreal_virtual_79_ConformSparseClassDataStruct(const UObject & self,UScriptStruct * SourceSparseClassDataStruct,void * SourceSparseClassData) {
	return self.ConformSparseClassDataStruct(SourceSparseClassDataStruct,SourceSparseClassData);
}

bool _dasUnreal_virtual_80_ImplementsGetWorld(const UObject & self) {
	return self.ImplementsGetWorld();
}

FName _dasUnreal_virtual_81_GetExporterName(UObject & self) {
	return self.GetExporterName();
}

FRestoreForUObjectOverwrite * _dasUnreal_virtual_82_GetRestoreForUObjectOverwrite(UObject & self) {
	return self.GetRestoreForUObjectOverwrite();
}

bool _dasUnreal_virtual_83_AreNativePropertiesIdenticalTo(const UObject & self,UObject * Other) {
	return self.AreNativePropertiesIdenticalTo(Other);
}

const FName & _dasUnreal_static_84_SourceFileTagName(const UObject *) {
	return UObject::SourceFileTagName();
}

const FName & _dasUnreal_static_85_AssetVersePathTagName(const UObject *) {
	return UObject::AssetVersePathTagName();
}

bool _dasUnreal_virtual_86_IsAsset(const UObject & self) {
	return self.IsAsset();
}

bool _dasUnreal_virtual_87_IsLocalizedResource(const UObject & self) {
	return self.IsLocalizedResource();
}

bool _dasUnreal_virtual_88_IsSafeForRootSet(const UObject & self) {
	return self.IsSafeForRootSet();
}

void _dasUnreal_virtual_89_TagSubobjects(UObject & self,EObjectFlags NewFlags) {
	self.TagSubobjects(NewFlags);
}

bool _dasUnreal_virtual_90_IsNameStableForNetworking(const UObject & self) {
	return self.IsNameStableForNetworking();
}

bool _dasUnreal_virtual_91_IsFullNameStableForNetworking(const UObject & self) {
	return self.IsFullNameStableForNetworking();
}

bool _dasUnreal_virtual_92_IsSupportedForNetworking(const UObject & self) {
	return self.IsSupportedForNetworking();
}

void _dasUnreal_virtual_93_PreNetReceive(UObject & self) {
	self.PreNetReceive();
}

void _dasUnreal_virtual_94_PostNetReceive(UObject & self) {
	self.PostNetReceive();
}

void _dasUnreal_virtual_95_PostRepNotifies(UObject & self) {
	self.PostRepNotifies();
}

void _dasUnreal_virtual_96_PreDestroyFromReplication(UObject & self) {
	self.PreDestroyFromReplication();
}

void _dasUnreal_virtual_97_WillNeverCacheCookedPlatformDataAgain(UObject & self) {
	self.WillNeverCacheCookedPlatformDataAgain();
}

void _dasUnreal_virtual_98_ClearAllCachedCookedPlatformData(UObject & self) {
	self.ClearAllCachedCookedPlatformData();
}

const wchar_t * _dasUnreal_virtual_99_GetConfigOverridePlatform(const UObject & self) {
	return self.GetConfigOverridePlatform();
}

void _dasUnreal_virtual_100_OverrideConfigSection(UObject & self,FString & SectionName) {
	self.OverrideConfigSection(SectionName);
}

void _dasUnreal_virtual_101_OverridePerObjectConfigSection(UObject & self,FString & SectionName) {
	self.OverridePerObjectConfigSection(SectionName);
}

UObject * _dasUnreal_static_102_GetArchetypeFromRequiredInfo(const UObject *,const UClass * Class,const UObject * Outer,FName Name,EObjectFlags ObjectFlags) {
	return UObject::GetArchetypeFromRequiredInfo(Class,Outer,Name,ObjectFlags);
}

void _dasUnreal_virtual_103_ProcessEvent(UObject & self,UFunction * Function,void * Parms) {
	self.ProcessEvent(Function,Parms);
}

UClass * _dasUnreal_virtual_104_RegenerateClass(UObject & self,UClass * ClassToRegenerate,UObject * PreviousCDO) {
	return self.RegenerateClass(ClassToRegenerate,PreviousCDO);
}

void _dasUnreal_virtual_105_MarkAsEditorOnlySubobject(UObject & self) {
	self.MarkAsEditorOnlySubobject();
}

uint64 _dasUnreal_virtual_106_GetNetPushIdDynamic(const UObject & self) {
	return self.GetNetPushIdDynamic();
}

FFieldClass * _dasUnreal_static_107_StaticClass(const FField *) {
	return FField::StaticClass();
}

uint64 _dasUnreal_static_108_StaticClassCastFlagsPrivate(const FField *) {
	return FField::StaticClassCastFlagsPrivate();
}

uint64 _dasUnreal_static_109_StaticClassCastFlags(const FField *) {
	return FField::StaticClassCastFlags();
}

void _dasUnreal_virtual_110_PostLoad(FField & self) {
	self.PostLoad();
}

void _dasUnreal_virtual_111_BeginDestroy(FField & self) {
	self.BeginDestroy();
}

void _dasUnreal_virtual_112_AddReferencedObjects(FField & self,FReferenceCollector & Collector) {
	self.AddReferencedObjects(Collector);
}

void _dasUnreal_virtual_113_AddCppProperty(FField & self,FProperty * Property) {
	self.AddCppProperty(Property);
}

void _dasUnreal_virtual_114_Bind(FField & self) {
	self.Bind();
}

FField * _dasUnreal_static_115_Construct(const FField *,const FFieldVariant & InOwner,const FName & InName,EObjectFlags InFlags) {
	return FField::Construct(InOwner,InName,InFlags);
}

FField * _dasUnreal_static_116_Construct(const FField *,const FName & FieldTypeName,const FFieldVariant & InOwner,const FName & InName,EObjectFlags InFlags) {
	return FField::Construct(FieldTypeName,InOwner,InName,InFlags);
}

void _dasUnreal_virtual_117_PostDuplicate(FField & self,const FField & InField) {
	self.PostDuplicate(InField);
}

FField * _dasUnreal_virtual_118_GetInnerFieldByName(FField & self,const FName & InName) {
	return self.GetInnerFieldByName(InName);
}

void _dasUnreal_static_119_CopyMetaData(const FField *,const FField * InSourceField,FField * InDestField) {
	FField::CopyMetaData(InSourceField,InDestField);
}

FField * _dasUnreal_static_120_CreateFromUField(const FField *,UField * InField) {
	return FField::CreateFromUField(InField);
}

FField * _dasUnreal_static_121_Duplicate(const FField *,const FField * InField,FFieldVariant DestOwner,const FName DestName,EObjectFlags FlagMask,EInternalObjectFlags InternalFlagsMask) {
	return FField::Duplicate(InField,DestOwner,DestName,FlagMask,InternalFlagsMask);
}

FName _dasUnreal_static_122_GenerateFFieldName(const FField *,FFieldVariant InOwner,FFieldClass * InClass) {
	return FField::GenerateFFieldName(InOwner,InClass);
}

UClass * _dasUnreal_static_123_StaticClass(const UField *) {
	return UField::StaticClass();
}

const wchar_t * _dasUnreal_static_124_StaticPackage(const UField *) {
	return UField::StaticPackage();
}

void _dasUnreal_static_125_StaticRegisterNativesUField(const UField *) {
	UField::StaticRegisterNativesUField();
}

void _dasUnreal_virtual_126_PostLoad(UField & self) {
	self.PostLoad();
}

bool _dasUnreal_virtual_127_NeedsLoadForClient(const UField & self) {
	return self.NeedsLoadForClient();
}

bool _dasUnreal_virtual_128_NeedsLoadForServer(const UField & self) {
	return self.NeedsLoadForServer();
}

void _dasUnreal_virtual_129_AddCppProperty(UField & self,FProperty * Property) {
	self.AddCppProperty(Property);
}

void _dasUnreal_virtual_130_Bind(UField & self) {
	self.Bind();
}

void _dasUnreal_static_131_FormatNativeToolTip(const UField *,FString & ToolTipString,bool bRemoveExtraSections) {
	UField::FormatNativeToolTip(ToolTipString,bRemoveExtraSections);
}

FField * _dasUnreal_virtual_132_GetAssociatedFField(UField & self) {
	return self.GetAssociatedFField();
}

void _dasUnreal_virtual_133_SetAssociatedFField(UField & self,FField * InField) {
	self.SetAssociatedFField(InField);
}

UClass * _dasUnreal_static_134_StaticClass(const UStruct *) {
	return UStruct::StaticClass();
}

const wchar_t * _dasUnreal_static_135_StaticPackage(const UStruct *) {
	return UStruct::StaticPackage();
}

void _dasUnreal_static_136_StaticRegisterNativesUStruct(const UStruct *) {
	UStruct::StaticRegisterNativesUStruct();
}

void _dasUnreal_virtual_137_PostLoad(UStruct & self) {
	self.PostLoad();
}

void _dasUnreal_virtual_138_FinishDestroy(UStruct & self) {
	self.FinishDestroy();
}

void _dasUnreal_virtual_139_RegisterDependencies(UStruct & self) {
	self.RegisterDependencies();
}

void _dasUnreal_virtual_140_TagSubobjects(UStruct & self,EObjectFlags NewFlags) {
	self.TagSubobjects(NewFlags);
}

void _dasUnreal_virtual_141_AddCppProperty(UStruct & self,FProperty * Property) {
	self.AddCppProperty(Property);
}

UStruct * _dasUnreal_virtual_142_GetInheritanceSuper(const UStruct & self) {
	return self.GetInheritanceSuper();
}

void _dasUnreal_virtual_143_InitializeStruct(const UStruct & self,void * Dest,int ArrayDim) {
	self.InitializeStruct(Dest,ArrayDim);
}

void _dasUnreal_virtual_144_DestroyStruct(const UStruct & self,void * Dest,int ArrayDim) {
	self.DestroyStruct(Dest,ArrayDim);
}

FProperty * _dasUnreal_virtual_145_CustomFindProperty(const UStruct & self,const FName InName) {
	return self.CustomFindProperty(InName);
}

const wchar_t * _dasUnreal_virtual_146_GetPrefixCPP(const UStruct & self) {
	return self.GetPrefixCPP();
}

void _dasUnreal_virtual_147_SetSuperStruct(UStruct & self,UStruct * NewSuperStruct) {
	self.SetSuperStruct(NewSuperStruct);
}

FString _dasUnreal_virtual_148_GetAuthoredNameForField(const UStruct & self,const UField * Field) {
	return self.GetAuthoredNameForField(Field);
}

FString _dasUnreal_virtual_149_GetAuthoredNameForField(const UStruct & self,const FField * Field) {
	return self.GetAuthoredNameForField(Field);
}

bool _dasUnreal_virtual_150_IsStructTrashed(const UStruct & self) {
	return self.IsStructTrashed();
}

UClass * _dasUnreal_static_151_StaticClass(const UScriptStruct *) {
	return UScriptStruct::StaticClass();
}

const wchar_t * _dasUnreal_static_152_StaticPackage(const UScriptStruct *) {
	return UScriptStruct::StaticPackage();
}

void _dasUnreal_static_153_StaticRegisterNativesUScriptStruct(const UScriptStruct *) {
	UScriptStruct::StaticRegisterNativesUScriptStruct();
}

void _dasUnreal_virtual_154_InitializeStruct(const UScriptStruct & self,void * Dest,int ArrayDim) {
	self.InitializeStruct(Dest,ArrayDim);
}

void _dasUnreal_virtual_155_DestroyStruct(const UScriptStruct & self,void * Dest,int ArrayDim) {
	self.DestroyStruct(Dest,ArrayDim);
}

bool _dasUnreal_virtual_156_IsStructTrashed(const UScriptStruct & self) {
	return self.IsStructTrashed();
}

void _dasUnreal_virtual_157_PrepareCppStructOps(UScriptStruct & self) {
	self.PrepareCppStructOps();
}

unsigned int _dasUnreal_virtual_158_GetStructTypeHash(const UScriptStruct & self,const void * Src) {
	return self.GetStructTypeHash(Src);
}

void _dasUnreal_virtual_159_RecursivelyPreload(UScriptStruct & self) {
	self.RecursivelyPreload();
}

FString _dasUnreal_virtual_160_GetStructCPPName(const UScriptStruct & self,unsigned int CPPExportFlags) {
	return self.GetStructCPPName(CPPExportFlags);
}

void _dasUnreal_virtual_161_InitializeDefaultValue(const UScriptStruct & self,unsigned char * InStructData) {
	self.InitializeDefaultValue(InStructData);
}

UClass * _dasUnreal_static_162_StaticClass(const UFunction *) {
	return UFunction::StaticClass();
}

const wchar_t * _dasUnreal_static_163_StaticPackage(const UFunction *) {
	return UFunction::StaticPackage();
}

void _dasUnreal_static_164_StaticRegisterNativesUFunction(const UFunction *) {
	UFunction::StaticRegisterNativesUFunction();
}

void _dasUnreal_virtual_165_PostLoad(UFunction & self) {
	self.PostLoad();
}

void _dasUnreal_virtual_166_Bind(UFunction & self) {
	self.Bind();
}

UStruct * _dasUnreal_virtual_167_GetInheritanceSuper(const UFunction & self) {
	return self.GetInheritanceSuper();
}

uint64 _dasUnreal_static_168_GetDefaultIgnoredSignatureCompatibilityFlags(const UFunction *) {
	return UFunction::GetDefaultIgnoredSignatureCompatibilityFlags();
}

UClass * _dasUnreal_static_169_StaticClass(const UEnum *) {
	return UEnum::StaticClass();
}

const wchar_t * _dasUnreal_static_170_StaticPackage(const UEnum *) {
	return UEnum::StaticPackage();
}

void _dasUnreal_static_171_StaticRegisterNativesUEnum(const UEnum *) {
	UEnum::StaticRegisterNativesUEnum();
}

FText _dasUnreal_virtual_172_GetDisplayNameTextByIndex(const UEnum & self,int InIndex) {
	return self.GetDisplayNameTextByIndex(InIndex);
}

FString _dasUnreal_virtual_173_GetAuthoredNameStringByIndex(const UEnum & self,int InIndex) {
	return self.GetAuthoredNameStringByIndex(InIndex);
}

bool _dasUnreal_static_174_IsFullEnumName(const UEnum *,const wchar_t * InEnumName) {
	return UEnum::IsFullEnumName(InEnumName);
}

FString _dasUnreal_virtual_175_GenerateFullEnumName(const UEnum & self,const wchar_t * InEnumName) {
	return self.GenerateFullEnumName(InEnumName);
}

int64 _dasUnreal_static_176_ParseEnum(const UEnum *,const wchar_t *& Str) {
	return UEnum::ParseEnum(Str);
}

void _dasUnreal_virtual_177_BeginDestroy(UEnum & self) {
	self.BeginDestroy();
}

bool _dasUnreal_virtual_178_IsAbstract(const FCppClassTypeInfo & self) {
	return self.IsAbstract();
}

UClass * _dasUnreal_static_179_StaticClass(const UClass *) {
	return UClass::StaticClass();
}

const wchar_t * _dasUnreal_static_180_StaticPackage(const UClass *) {
	return UClass::StaticPackage();
}

void _dasUnreal_static_181_StaticRegisterNativesUClass(const UClass *) {
	UClass::StaticRegisterNativesUClass();
}

void _dasUnreal_virtual_182_ConditionalRecompileClass(UClass & self,FUObjectSerializeContext * InLoadContext) {
	self.ConditionalRecompileClass(InLoadContext);
}

void _dasUnreal_virtual_183_FlushCompilationQueueForLevel(UClass & self) {
	self.FlushCompilationQueueForLevel();
}

void _dasUnreal_static_184_AssembleReferenceTokenStreams(const UClass *) {
	UClass::AssembleReferenceTokenStreams();
}

UClass * _dasUnreal_virtual_185_GetAuthoritativeClass(UClass & self) {
	return self.GetAuthoritativeClass();
}

void _dasUnreal_virtual_186_PostLoad(UClass & self) {
	self.PostLoad();
}

void _dasUnreal_virtual_187_FinishDestroy(UClass & self) {
	self.FinishDestroy();
}

void _dasUnreal_virtual_188_DeferredRegister(UClass & self,UClass * UClassStaticClass,const wchar_t * PackageName,const wchar_t * InName) {
	self.DeferredRegister(UClassStaticClass,PackageName,InName);
}

bool _dasUnreal_virtual_189_Rename(UClass & self,const wchar_t * NewName,UObject * NewOuter,unsigned int Flags) {
	return self.Rename(NewName,NewOuter,Flags);
}

void _dasUnreal_virtual_190_TagSubobjects(UClass & self,EObjectFlags NewFlags) {
	self.TagSubobjects(NewFlags);
}

void _dasUnreal_virtual_191_PostInitProperties(UClass & self) {
	self.PostInitProperties();
}

FRestoreForUObjectOverwrite * _dasUnreal_virtual_192_GetRestoreForUObjectOverwrite(UClass & self) {
	return self.GetRestoreForUObjectOverwrite();
}

FString _dasUnreal_virtual_193_GetDesc(UClass & self) {
	return self.GetDesc();
}

bool _dasUnreal_virtual_194_IsAsset(const UClass & self) {
	return self.IsAsset();
}

bool _dasUnreal_virtual_195_IsNameStableForNetworking(const UClass & self) {
	return self.IsNameStableForNetworking();
}

void _dasUnreal_virtual_196_Bind(UClass & self) {
	self.Bind();
}

const wchar_t * _dasUnreal_virtual_197_GetPrefixCPP(const UClass & self) {
	return self.GetPrefixCPP();
}

void _dasUnreal_virtual_198_SetSuperStruct(UClass & self,UStruct * NewSuperStruct) {
	self.SetSuperStruct(NewSuperStruct);
}

bool _dasUnreal_virtual_199_IsStructTrashed(const UClass & self) {
	return self.IsStructTrashed();
}

void _dasUnreal_virtual_200_PostLoadInstance(UClass & self,UObject * InObj) {
	self.PostLoadInstance(InObj);
}

void _dasUnreal_virtual_201_InitPropertiesFromCustomList(UClass & self,unsigned char * DataPtr,const unsigned char * DefaultDataPtr) {
	self.InitPropertiesFromCustomList(DataPtr,DefaultDataPtr);
}

bool _dasUnreal_virtual_202_CanCreateAssetOfClass(const UClass & self) {
	return self.CanCreateAssetOfClass();
}

unsigned char * _dasUnreal_virtual_203_GetPersistentUberGraphFrame(const UClass & self,UObject * Obj,UFunction * FuncToCheck) {
	return self.GetPersistentUberGraphFrame(Obj,FuncToCheck);
}

void _dasUnreal_virtual_204_CreatePersistentUberGraphFrame(const UClass & self,UObject * Obj,bool bCreateOnlyIfEmpty,bool bSkipSuperClass,UClass * OldClass) {
	self.CreatePersistentUberGraphFrame(Obj,bCreateOnlyIfEmpty,bSkipSuperClass,OldClass);
}

void _dasUnreal_virtual_205_DestroyPersistentUberGraphFrame(const UClass & self,UObject * Obj,bool bSkipSuperClass) {
	self.DestroyPersistentUberGraphFrame(Obj,bSkipSuperClass);
}

void _dasUnreal_virtual_206_PostLoadDefaultObject(UClass & self,UObject * Object) {
	self.PostLoadDefaultObject(Object);
}

void _dasUnreal_virtual_207_PurgeClass(UClass & self,bool bRecompilingOnLoad) {
	self.PurgeClass(bRecompilingOnLoad);
}

UClass * _dasUnreal_static_208_FindCommonBase(const UClass *,UClass * InClassA,UClass * InClassB) {
	return UClass::FindCommonBase(InClassA,InClassB);
}

bool _dasUnreal_virtual_209_IsFunctionImplementedInScript(const UClass & self,FName InFunctionName) {
	return self.IsFunctionImplementedInScript(InFunctionName);
}

bool _dasUnreal_virtual_210_HasProperty(const UClass & self,FProperty * InProperty) {
	return self.HasProperty(InProperty);
}

UObject * _dasUnreal_virtual_211_FindArchetype(const UClass & self,const UClass * ArchetypeClass,const FName ArchetypeName) {
	return self.FindArchetype(ArchetypeClass,ArchetypeName);
}

UObject * _dasUnreal_virtual_212_GetArchetypeForCDO(const UClass & self) {
	return self.GetArchetypeForCDO();
}

bool _dasUnreal_static_213_IsSafeToSerializeToStructuredArchives(const UClass *,UClass * InClass) {
	return UClass::IsSafeToSerializeToStructuredArchives(InClass);
}

UClass * _dasUnreal_static_214_StaticClass(const UPackage *) {
	return UPackage::StaticClass();
}

const wchar_t * _dasUnreal_static_215_StaticPackage(const UPackage *) {
	return UPackage::StaticPackage();
}

void _dasUnreal_static_216_StaticRegisterNativesUPackage(const UPackage *) {
	UPackage::StaticRegisterNativesUPackage();
}

bool _dasUnreal_virtual_217_IsNameStableForNetworking(const UPackage & self) {
	return self.IsNameStableForNetworking();
}

bool _dasUnreal_virtual_218_NeedsLoadForClient(const UPackage & self) {
	return self.NeedsLoadForClient();
}

bool _dasUnreal_virtual_219_NeedsLoadForServer(const UPackage & self) {
	return self.NeedsLoadForServer();
}

bool _dasUnreal_virtual_220_IsPostLoadThreadSafe(const UPackage & self) {
	return self.IsPostLoadThreadSafe();
}

bool _dasUnreal_virtual_221_IsDestructionThreadSafe(const UPackage & self) {
	return self.IsDestructionThreadSafe();
}

void _dasUnreal_virtual_222_PostInitProperties(UPackage & self) {
	self.PostInitProperties();
}

void _dasUnreal_virtual_223_FinishDestroy(UPackage & self) {
	self.FinishDestroy();
}

bool _dasUnreal_virtual_224_IsAsset(const UPackage & self) {
	return self.IsAsset();
}

void _dasUnreal_virtual_225_TagSubobjects(UPackage & self,EObjectFlags NewFlags) {
	self.TagSubobjects(NewFlags);
}

void _dasUnreal_static_226_WaitForAsyncFileWrites(const UPackage *) {
	UPackage::WaitForAsyncFileWrites();
}

bool _dasUnreal_static_227_HasAsyncFileWrites(const UPackage *) {
	return UPackage::HasAsyncFileWrites();
}

bool _dasUnreal_static_228_IsEmptyPackage(const UPackage *,UPackage * Package,const UObject * LastReferencer) {
	return UPackage::IsEmptyPackage(Package,LastReferencer);
}

FFieldClass * _dasUnreal_static_229_StaticClass(const FProperty *) {
	return FProperty::StaticClass();
}

uint64 _dasUnreal_static_230_StaticClassCastFlagsPrivate(const FProperty *) {
	return FProperty::StaticClassCastFlagsPrivate();
}

uint64 _dasUnreal_static_231_StaticClassCastFlags(const FProperty *) {
	return FProperty::StaticClassCastFlags();
}

void _dasUnreal_virtual_232_PostDuplicate(FProperty & self,const FField & InField) {
	self.PostDuplicate(InField);
}

FName _dasUnreal_static_233_FindRedirectedPropertyName(const FProperty *,UStruct * ObjectStruct,FName OldName) {
	return FProperty::FindRedirectedPropertyName(ObjectStruct,OldName);
}

FString _dasUnreal_virtual_234_GetCPPMacroType(const FProperty & self,FString & ExtendedTypeText) {
	return self.GetCPPMacroType(ExtendedTypeText);
}

bool _dasUnreal_virtual_235_PassCPPArgsByRef(const FProperty & self) {
	return self.PassCPPArgsByRef();
}

FString _dasUnreal_virtual_236_GetCPPType(const FProperty & self,FString * ExtendedTypeText,unsigned int CPPExportFlags) {
	return self.GetCPPType(ExtendedTypeText,CPPExportFlags);
}

FString _dasUnreal_virtual_237_GetCPPTypeForwardDeclaration(const FProperty & self) {
	return self.GetCPPTypeForwardDeclaration();
}

bool _dasUnreal_virtual_238_HasSetter(const FProperty & self) {
	return self.HasSetter();
}

bool _dasUnreal_virtual_239_HasGetter(const FProperty & self) {
	return self.HasGetter();
}

bool _dasUnreal_virtual_240_HasSetterOrGetter(const FProperty & self) {
	return self.HasSetterOrGetter();
}

void _dasUnreal_virtual_241_CallSetter(const FProperty & self,void * Container,const void * InValue) {
	self.CallSetter(Container,InValue);
}

void _dasUnreal_virtual_242_CallGetter(const FProperty & self,const void * Container,void * OutValue) {
	self.CallGetter(Container,OutValue);
}

bool _dasUnreal_virtual_243_Identical(const FProperty & self,const void * A,const void * B,unsigned int PortFlags) {
	return self.Identical(A,B,PortFlags);
}

bool _dasUnreal_virtual_244_SupportsNetSharedSerialization(const FProperty & self) {
	return self.SupportsNetSharedSerialization();
}

void _dasUnreal_virtual_245_ExportTextItem(const FProperty & self,FString & ValueStr,const void * PropertyValue,const void * DefaultValue,UObject * Parent,int PortFlags,UObject * ExportRootScope) {
	self.ExportTextItem(ValueStr,PropertyValue,DefaultValue,Parent,PortFlags,ExportRootScope);
}

void * _dasUnreal_virtual_246_GetValueAddressAtIndex_Direct(const FProperty & self,const FProperty * Inner,void * InValueAddress,int Index) {
	return self.GetValueAddressAtIndex_Direct(Inner,InValueAddress,Index);
}

void _dasUnreal_virtual_247_AppendSchemaHash(const FProperty & self,FBlake3 & Builder,bool bSkipEditorOnly) {
	self.AppendSchemaHash(Builder,bSkipEditorOnly);
}

void _dasUnreal_virtual_248_CopySingleValueToScriptVM(const FProperty & self,void * Dest,const void * Src) {
	self.CopySingleValueToScriptVM(Dest,Src);
}

void _dasUnreal_virtual_249_CopyCompleteValueToScriptVM(const FProperty & self,void * Dest,const void * Src) {
	self.CopyCompleteValueToScriptVM(Dest,Src);
}

void _dasUnreal_virtual_250_CopyCompleteValueToScriptVM_InContainer(const FProperty & self,void * OutValue,const void * InContainer) {
	self.CopyCompleteValueToScriptVM_InContainer(OutValue,InContainer);
}

void _dasUnreal_virtual_251_CopyCompleteValueFromScriptVM_InContainer(const FProperty & self,void * OutContainer,const void * InValue) {
	self.CopyCompleteValueFromScriptVM_InContainer(OutContainer,InValue);
}

void _dasUnreal_virtual_252_CopySingleValueFromScriptVM(const FProperty & self,void * Dest,const void * Src) {
	self.CopySingleValueFromScriptVM(Dest,Src);
}

void _dasUnreal_virtual_253_CopyCompleteValueFromScriptVM(const FProperty & self,void * Dest,const void * Src) {
	self.CopyCompleteValueFromScriptVM(Dest,Src);
}

FName _dasUnreal_virtual_254_GetID(const FProperty & self) {
	return self.GetID();
}

int _dasUnreal_virtual_255_GetMinAlignment(const FProperty & self) {
	return self.GetMinAlignment();
}

bool _dasUnreal_virtual_256_SameType(const FProperty & self,const FProperty * Other) {
	return self.SameType(Other);
}

FFieldClass * _dasUnreal_static_257_StaticClass(const FNumericProperty *) {
	return FNumericProperty::StaticClass();
}

FField * _dasUnreal_static_258_Construct(const FNumericProperty *,const FFieldVariant & InOwner,const FName & InName,EObjectFlags InObjectFlags) {
	return FNumericProperty::Construct(InOwner,InName,InObjectFlags);
}

uint64 _dasUnreal_static_259_StaticClassCastFlagsPrivate(const FNumericProperty *) {
	return FNumericProperty::StaticClassCastFlagsPrivate();
}

uint64 _dasUnreal_static_260_StaticClassCastFlags(const FNumericProperty *) {
	return FNumericProperty::StaticClassCastFlags();
}

void _dasUnreal_virtual_261_ExportText_Internal(const FNumericProperty & self,FString & ValueStr,const void * PropertyValueOrContainer,EPropertyPointerType PropertyPointerType,const void * DefaultValue,UObject * Parent,int PortFlags,UObject * ExportRootScope) {
	self.ExportText_Internal(ValueStr,PropertyValueOrContainer,PropertyPointerType,DefaultValue,Parent,PortFlags,ExportRootScope);
}

bool _dasUnreal_virtual_262_IsFloatingPoint(const FNumericProperty & self) {
	return self.IsFloatingPoint();
}

bool _dasUnreal_virtual_263_IsInteger(const FNumericProperty & self) {
	return self.IsInteger();
}

UEnum * _dasUnreal_virtual_264_GetIntPropertyEnum(const FNumericProperty & self) {
	return self.GetIntPropertyEnum();
}

void _dasUnreal_virtual_265_SetIntPropertyValue(const FNumericProperty & self,void * Data,uint64 Value) {
	self.SetIntPropertyValue(Data,Value);
}

void _dasUnreal_virtual_266_SetIntPropertyValue(const FNumericProperty & self,void * Data,int64 Value) {
	self.SetIntPropertyValue(Data,Value);
}

void _dasUnreal_virtual_267_SetFloatingPointPropertyValue(const FNumericProperty & self,void * Data,double Value) {
	self.SetFloatingPointPropertyValue(Data,Value);
}

void _dasUnreal_virtual_268_SetNumericPropertyValueFromString(const FNumericProperty & self,void * Data,const wchar_t * Value) {
	self.SetNumericPropertyValueFromString(Data,Value);
}

void _dasUnreal_virtual_269_SetNumericPropertyValueFromString_InContainer(const FNumericProperty & self,void * Container,const wchar_t * Value) {
	self.SetNumericPropertyValueFromString_InContainer(Container,Value);
}

int64 _dasUnreal_virtual_270_GetSignedIntPropertyValue(const FNumericProperty & self,const void * Data) {
	return self.GetSignedIntPropertyValue(Data);
}

int64 _dasUnreal_virtual_271_GetSignedIntPropertyValue_InContainer(const FNumericProperty & self,const void * Container) {
	return self.GetSignedIntPropertyValue_InContainer(Container);
}

uint64 _dasUnreal_virtual_272_GetUnsignedIntPropertyValue(const FNumericProperty & self,const void * Data) {
	return self.GetUnsignedIntPropertyValue(Data);
}

uint64 _dasUnreal_virtual_273_GetUnsignedIntPropertyValue_InContainer(const FNumericProperty & self,const void * Container) {
	return self.GetUnsignedIntPropertyValue_InContainer(Container);
}

double _dasUnreal_virtual_274_GetFloatingPointPropertyValue(const FNumericProperty & self,const void * Data) {
	return self.GetFloatingPointPropertyValue(Data);
}

FString _dasUnreal_virtual_275_GetNumericPropertyValueToString(const FNumericProperty & self,const void * Data) {
	return self.GetNumericPropertyValueToString(Data);
}

FString _dasUnreal_virtual_276_GetNumericPropertyValueToString_InContainer(const FNumericProperty & self,const void * Container) {
	return self.GetNumericPropertyValueToString_InContainer(Container);
}

FFieldClass * _dasUnreal_static_277_StaticClass(const FByteProperty *) {
	return FByteProperty::StaticClass();
}

FField * _dasUnreal_static_278_Construct(const FByteProperty *,const FFieldVariant & InOwner,const FName & InName,EObjectFlags InObjectFlags) {
	return FByteProperty::Construct(InOwner,InName,InObjectFlags);
}

uint64 _dasUnreal_static_279_StaticClassCastFlagsPrivate(const FByteProperty *) {
	return FByteProperty::StaticClassCastFlagsPrivate();
}

uint64 _dasUnreal_static_280_StaticClassCastFlags(const FByteProperty *) {
	return FByteProperty::StaticClassCastFlags();
}

void _dasUnreal_virtual_281_AddReferencedObjects(FByteProperty & self,FReferenceCollector & Collector) {
	self.AddReferencedObjects(Collector);
}

void _dasUnreal_virtual_282_PostDuplicate(FByteProperty & self,const FField & InField) {
	self.PostDuplicate(InField);
}

FString _dasUnreal_virtual_283_GetCPPType(const FByteProperty & self,FString * ExtendedTypeText,unsigned int CPPExportFlags) {
	return self.GetCPPType(ExtendedTypeText,CPPExportFlags);
}

void _dasUnreal_virtual_284_AppendSchemaHash(const FByteProperty & self,FBlake3 & Builder,bool bSkipEditorOnly) {
	self.AppendSchemaHash(Builder,bSkipEditorOnly);
}

UEnum * _dasUnreal_virtual_285_GetIntPropertyEnum(const FByteProperty & self) {
	return self.GetIntPropertyEnum();
}

FFieldClass * _dasUnreal_static_286_StaticClass(const FInt8Property *) {
	return FInt8Property::StaticClass();
}

FField * _dasUnreal_static_287_Construct(const FInt8Property *,const FFieldVariant & InOwner,const FName & InName,EObjectFlags InObjectFlags) {
	return FInt8Property::Construct(InOwner,InName,InObjectFlags);
}

uint64 _dasUnreal_static_288_StaticClassCastFlagsPrivate(const FInt8Property *) {
	return FInt8Property::StaticClassCastFlagsPrivate();
}

uint64 _dasUnreal_static_289_StaticClassCastFlags(const FInt8Property *) {
	return FInt8Property::StaticClassCastFlags();
}

FFieldClass * _dasUnreal_static_290_StaticClass(const FInt16Property *) {
	return FInt16Property::StaticClass();
}

FField * _dasUnreal_static_291_Construct(const FInt16Property *,const FFieldVariant & InOwner,const FName & InName,EObjectFlags InObjectFlags) {
	return FInt16Property::Construct(InOwner,InName,InObjectFlags);
}

uint64 _dasUnreal_static_292_StaticClassCastFlagsPrivate(const FInt16Property *) {
	return FInt16Property::StaticClassCastFlagsPrivate();
}

uint64 _dasUnreal_static_293_StaticClassCastFlags(const FInt16Property *) {
	return FInt16Property::StaticClassCastFlags();
}

FFieldClass * _dasUnreal_static_294_StaticClass(const FIntProperty *) {
	return FIntProperty::StaticClass();
}

FField * _dasUnreal_static_295_Construct(const FIntProperty *,const FFieldVariant & InOwner,const FName & InName,EObjectFlags InObjectFlags) {
	return FIntProperty::Construct(InOwner,InName,InObjectFlags);
}

uint64 _dasUnreal_static_296_StaticClassCastFlagsPrivate(const FIntProperty *) {
	return FIntProperty::StaticClassCastFlagsPrivate();
}

uint64 _dasUnreal_static_297_StaticClassCastFlags(const FIntProperty *) {
	return FIntProperty::StaticClassCastFlags();
}

FFieldClass * _dasUnreal_static_298_StaticClass(const FInt64Property *) {
	return FInt64Property::StaticClass();
}

FField * _dasUnreal_static_299_Construct(const FInt64Property *,const FFieldVariant & InOwner,const FName & InName,EObjectFlags InObjectFlags) {
	return FInt64Property::Construct(InOwner,InName,InObjectFlags);
}

uint64 _dasUnreal_static_300_StaticClassCastFlagsPrivate(const FInt64Property *) {
	return FInt64Property::StaticClassCastFlagsPrivate();
}

uint64 _dasUnreal_static_301_StaticClassCastFlags(const FInt64Property *) {
	return FInt64Property::StaticClassCastFlags();
}

FFieldClass * _dasUnreal_static_302_StaticClass(const FUInt16Property *) {
	return FUInt16Property::StaticClass();
}

FField * _dasUnreal_static_303_Construct(const FUInt16Property *,const FFieldVariant & InOwner,const FName & InName,EObjectFlags InObjectFlags) {
	return FUInt16Property::Construct(InOwner,InName,InObjectFlags);
}

uint64 _dasUnreal_static_304_StaticClassCastFlagsPrivate(const FUInt16Property *) {
	return FUInt16Property::StaticClassCastFlagsPrivate();
}

uint64 _dasUnreal_static_305_StaticClassCastFlags(const FUInt16Property *) {
	return FUInt16Property::StaticClassCastFlags();
}

FFieldClass * _dasUnreal_static_306_StaticClass(const FUInt32Property *) {
	return FUInt32Property::StaticClass();
}

FField * _dasUnreal_static_307_Construct(const FUInt32Property *,const FFieldVariant & InOwner,const FName & InName,EObjectFlags InObjectFlags) {
	return FUInt32Property::Construct(InOwner,InName,InObjectFlags);
}

uint64 _dasUnreal_static_308_StaticClassCastFlagsPrivate(const FUInt32Property *) {
	return FUInt32Property::StaticClassCastFlagsPrivate();
}

uint64 _dasUnreal_static_309_StaticClassCastFlags(const FUInt32Property *) {
	return FUInt32Property::StaticClassCastFlags();
}

FFieldClass * _dasUnreal_static_310_StaticClass(const FUInt64Property *) {
	return FUInt64Property::StaticClass();
}

FField * _dasUnreal_static_311_Construct(const FUInt64Property *,const FFieldVariant & InOwner,const FName & InName,EObjectFlags InObjectFlags) {
	return FUInt64Property::Construct(InOwner,InName,InObjectFlags);
}

uint64 _dasUnreal_static_312_StaticClassCastFlagsPrivate(const FUInt64Property *) {
	return FUInt64Property::StaticClassCastFlagsPrivate();
}

uint64 _dasUnreal_static_313_StaticClassCastFlags(const FUInt64Property *) {
	return FUInt64Property::StaticClassCastFlags();
}

FFieldClass * _dasUnreal_static_314_StaticClass(const FFloatProperty *) {
	return FFloatProperty::StaticClass();
}

FField * _dasUnreal_static_315_Construct(const FFloatProperty *,const FFieldVariant & InOwner,const FName & InName,EObjectFlags InObjectFlags) {
	return FFloatProperty::Construct(InOwner,InName,InObjectFlags);
}

uint64 _dasUnreal_static_316_StaticClassCastFlagsPrivate(const FFloatProperty *) {
	return FFloatProperty::StaticClassCastFlagsPrivate();
}

uint64 _dasUnreal_static_317_StaticClassCastFlags(const FFloatProperty *) {
	return FFloatProperty::StaticClassCastFlags();
}

FFieldClass * _dasUnreal_static_318_StaticClass(const FDoubleProperty *) {
	return FDoubleProperty::StaticClass();
}

FField * _dasUnreal_static_319_Construct(const FDoubleProperty *,const FFieldVariant & InOwner,const FName & InName,EObjectFlags InObjectFlags) {
	return FDoubleProperty::Construct(InOwner,InName,InObjectFlags);
}

uint64 _dasUnreal_static_320_StaticClassCastFlagsPrivate(const FDoubleProperty *) {
	return FDoubleProperty::StaticClassCastFlagsPrivate();
}

uint64 _dasUnreal_static_321_StaticClassCastFlags(const FDoubleProperty *) {
	return FDoubleProperty::StaticClassCastFlags();
}

FFieldClass * _dasUnreal_static_322_StaticClass(const FBoolProperty *) {
	return FBoolProperty::StaticClass();
}

FField * _dasUnreal_static_323_Construct(const FBoolProperty *,const FFieldVariant & InOwner,const FName & InName,EObjectFlags InObjectFlags) {
	return FBoolProperty::Construct(InOwner,InName,InObjectFlags);
}

uint64 _dasUnreal_static_324_StaticClassCastFlagsPrivate(const FBoolProperty *) {
	return FBoolProperty::StaticClassCastFlagsPrivate();
}

uint64 _dasUnreal_static_325_StaticClassCastFlags(const FBoolProperty *) {
	return FBoolProperty::StaticClassCastFlags();
}

void _dasUnreal_virtual_326_PostDuplicate(FBoolProperty & self,const FField & InField) {
	self.PostDuplicate(InField);
}

FString _dasUnreal_virtual_327_GetCPPType(const FBoolProperty & self,FString * ExtendedTypeText,unsigned int CPPExportFlags) {
	return self.GetCPPType(ExtendedTypeText,CPPExportFlags);
}

FString _dasUnreal_virtual_328_GetCPPMacroType(const FBoolProperty & self,FString & ExtendedTypeText) {
	return self.GetCPPMacroType(ExtendedTypeText);
}

FString _dasUnreal_virtual_329_GetCPPTypeForwardDeclaration(const FBoolProperty & self) {
	return self.GetCPPTypeForwardDeclaration();
}

bool _dasUnreal_virtual_330_Identical(const FBoolProperty & self,const void * A,const void * B,unsigned int PortFlags) {
	return self.Identical(A,B,PortFlags);
}

void _dasUnreal_virtual_331_CopyValuesInternal(const FBoolProperty & self,void * Dest,const void * Src,int Count) {
	self.CopyValuesInternal(Dest,Src,Count);
}

void _dasUnreal_virtual_332_ClearValueInternal(const FBoolProperty & self,void * Data) {
	self.ClearValueInternal(Data);
}

void _dasUnreal_virtual_333_InitializeValueInternal(const FBoolProperty & self,void * Dest) {
	self.InitializeValueInternal(Dest);
}

int _dasUnreal_virtual_334_GetMinAlignment(const FBoolProperty & self) {
	return self.GetMinAlignment();
}

void _dasUnreal_virtual_335_AppendSchemaHash(const FBoolProperty & self,FBlake3 & Builder,bool bSkipEditorOnly) {
	self.AppendSchemaHash(Builder,bSkipEditorOnly);
}

bool _dasUnreal_static_336_GetDefaultPropertyValue(const FBoolProperty *) {
	return FBoolProperty::GetDefaultPropertyValue();
}

unsigned int _dasUnreal_virtual_337_GetValueTypeHashInternal(const FBoolProperty & self,const void * Src) {
	return self.GetValueTypeHashInternal(Src);
}

FFieldClass * _dasUnreal_static_338_StaticClass(const FObjectPropertyBase *) {
	return FObjectPropertyBase::StaticClass();
}

FField * _dasUnreal_static_339_Construct(const FObjectPropertyBase *,const FFieldVariant & InOwner,const FName & InName,EObjectFlags InObjectFlags) {
	return FObjectPropertyBase::Construct(InOwner,InName,InObjectFlags);
}

uint64 _dasUnreal_static_340_StaticClassCastFlagsPrivate(const FObjectPropertyBase *) {
	return FObjectPropertyBase::StaticClassCastFlagsPrivate();
}

uint64 _dasUnreal_static_341_StaticClassCastFlags(const FObjectPropertyBase *) {
	return FObjectPropertyBase::StaticClassCastFlags();
}

void _dasUnreal_virtual_342_AddReferencedObjects(FObjectPropertyBase & self,FReferenceCollector & Collector) {
	self.AddReferencedObjects(Collector);
}

void _dasUnreal_virtual_343_BeginDestroy(FObjectPropertyBase & self) {
	self.BeginDestroy();
}

void _dasUnreal_virtual_344_PostDuplicate(FObjectPropertyBase & self,const FField & InField) {
	self.PostDuplicate(InField);
}

bool _dasUnreal_virtual_345_Identical(const FObjectPropertyBase & self,const void * A,const void * B,unsigned int PortFlags) {
	return self.Identical(A,B,PortFlags);
}

bool _dasUnreal_virtual_346_SupportsNetSharedSerialization(const FObjectPropertyBase & self) {
	return self.SupportsNetSharedSerialization();
}

FName _dasUnreal_virtual_347_GetID(const FObjectPropertyBase & self) {
	return self.GetID();
}

bool _dasUnreal_virtual_348_SameType(const FObjectPropertyBase & self,const FProperty * Other) {
	return self.SameType(Other);
}

FString _dasUnreal_virtual_349_GetCPPTypeCustom(const FObjectPropertyBase & self,FString * ExtendedTypeText,unsigned int CPPExportFlags,const FString & InnerNativeTypeName) {
	return self.GetCPPTypeCustom(ExtendedTypeText,CPPExportFlags,InnerNativeTypeName);
}

bool _dasUnreal_static_350_StaticIdentical(const FObjectPropertyBase *,UObject * A,UObject * B,unsigned int PortFlags) {
	return FObjectPropertyBase::StaticIdentical(A,B,PortFlags);
}

UObject * _dasUnreal_virtual_351_LoadObjectPropertyValue(const FObjectPropertyBase & self,const void * PropertyValueAddress) {
	return self.LoadObjectPropertyValue(PropertyValueAddress);
}

UObject * _dasUnreal_virtual_352_GetObjectPropertyValue(const FObjectPropertyBase & self,const void * PropertyValueAddress) {
	return self.GetObjectPropertyValue(PropertyValueAddress);
}

UObject * _dasUnreal_virtual_353_GetObjectPropertyValue_InContainer(const FObjectPropertyBase & self,const void * ContainerAddress,int ArrayIndex) {
	return self.GetObjectPropertyValue_InContainer(ContainerAddress,ArrayIndex);
}

void _dasUnreal_virtual_354_SetObjectPropertyValue(const FObjectPropertyBase & self,void * PropertyValueAddress,UObject * Value) {
	self.SetObjectPropertyValue(PropertyValueAddress,Value);
}

void _dasUnreal_virtual_355_SetObjectPropertyValue_InContainer(const FObjectPropertyBase & self,void * ContainerAddress,UObject * Value,int ArrayIndex) {
	self.SetObjectPropertyValue_InContainer(ContainerAddress,Value,ArrayIndex);
}

FFieldClass * _dasUnreal_static_356_StaticClass(const FStructProperty *) {
	return FStructProperty::StaticClass();
}

FField * _dasUnreal_static_357_Construct(const FStructProperty *,const FFieldVariant & InOwner,const FName & InName,EObjectFlags InObjectFlags) {
	return FStructProperty::Construct(InOwner,InName,InObjectFlags);
}

uint64 _dasUnreal_static_358_StaticClassCastFlagsPrivate(const FStructProperty *) {
	return FStructProperty::StaticClassCastFlagsPrivate();
}

uint64 _dasUnreal_static_359_StaticClassCastFlags(const FStructProperty *) {
	return FStructProperty::StaticClassCastFlags();
}

void _dasUnreal_virtual_360_AddReferencedObjects(FStructProperty & self,FReferenceCollector & Collector) {
	self.AddReferencedObjects(Collector);
}

void _dasUnreal_virtual_361_PostDuplicate(FStructProperty & self,const FField & InField) {
	self.PostDuplicate(InField);
}

FString _dasUnreal_virtual_362_GetCPPMacroType(const FStructProperty & self,FString & ExtendedTypeText) {
	return self.GetCPPMacroType(ExtendedTypeText);
}

FString _dasUnreal_virtual_363_GetCPPType(const FStructProperty & self,FString * ExtendedTypeText,unsigned int CPPExportFlags) {
	return self.GetCPPType(ExtendedTypeText,CPPExportFlags);
}

FString _dasUnreal_virtual_364_GetCPPTypeForwardDeclaration(const FStructProperty & self) {
	return self.GetCPPTypeForwardDeclaration();
}

bool _dasUnreal_virtual_365_Identical(const FStructProperty & self,const void * A,const void * B,unsigned int PortFlags) {
	return self.Identical(A,B,PortFlags);
}

bool _dasUnreal_virtual_366_SupportsNetSharedSerialization(const FStructProperty & self) {
	return self.SupportsNetSharedSerialization();
}

void _dasUnreal_virtual_367_CopyValuesInternal(const FStructProperty & self,void * Dest,const void * Src,int Count) {
	self.CopyValuesInternal(Dest,Src,Count);
}

void _dasUnreal_virtual_368_ClearValueInternal(const FStructProperty & self,void * Data) {
	self.ClearValueInternal(Data);
}

void _dasUnreal_virtual_369_DestroyValueInternal(const FStructProperty & self,void * Dest) {
	self.DestroyValueInternal(Dest);
}

void _dasUnreal_virtual_370_InitializeValueInternal(const FStructProperty & self,void * Dest) {
	self.InitializeValueInternal(Dest);
}

int _dasUnreal_virtual_371_GetMinAlignment(const FStructProperty & self) {
	return self.GetMinAlignment();
}

bool _dasUnreal_virtual_372_SameType(const FStructProperty & self,const FProperty * Other) {
	return self.SameType(Other);
}

void _dasUnreal_virtual_373_AppendSchemaHash(const FStructProperty & self,FBlake3 & Builder,bool bSkipEditorOnly) {
	self.AppendSchemaHash(Builder,bSkipEditorOnly);
}

FProperty * _dasUnreal_static_374_GetNextPropertyAndAdvance(const FCustomPropertyListNode *,const FCustomPropertyListNode *& Node) {
	return FCustomPropertyListNode::GetNextPropertyAndAdvance(Node);
}

UClass * _dasUnreal_static_375_StaticClass(const UPropertyWrapper *) {
	return UPropertyWrapper::StaticClass();
}

const wchar_t * _dasUnreal_static_376_StaticPackage(const UPropertyWrapper *) {
	return UPropertyWrapper::StaticPackage();
}

void _dasUnreal_static_377_StaticRegisterNativesUPropertyWrapper(const UPropertyWrapper *) {
	UPropertyWrapper::StaticRegisterNativesUPropertyWrapper();
}

