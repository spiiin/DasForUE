FDasStructValue SoftCheck(const FDasStructValue& H,const char* Path,das::Context* C){
 RequireObjectContext(C);const FString Expected=Path?UTF8_TO_TCHAR(Path):TEXT("");auto* P=DasSoftProperty(Expected);if(!P)C->throw_error("unknown soft reference type");
 if(!H.Token){FInstancedStruct V;V.InitializeAs(FindObject<UScriptStruct>(nullptr,*Expected));return StoreStructValue(MoveTemp(V),C);}
 const auto& V=ResolveStructValue(H,C);FString Error;if(V.GetScriptStruct()->GetPathName()!=Expected || !ValidateDasStructSnapshot(V,Error))C->throw_error("soft reference type/context mismatch");return H;
}
FDasStructValue SoftMake(const char* Type,const char* Path,das::Context* C){
 RequireObjectContext(C);
 const FString Expected=Type?UTF8_TO_TCHAR(Type):TEXT(""),Text=Path?UTF8_TO_TCHAR(Path):TEXT("");FString Error;auto* P=DasSoftProperty(Expected);if(!P || !ValidateDasSoftPath(Text,Error))C->throw_error("invalid soft reference type or asset path");
 FInstancedStruct V;V.InitializeAs(FindObject<UScriptStruct>(nullptr,*Expected));P->SetPropertyValue(P->ContainerPtrToValuePtr<void>(V.GetMutableMemory()),FSoftObjectPtr(FSoftObjectPath(Text)));
 if(!ValidateDasStructSnapshot(V,Error))C->throw_error(TCHAR_TO_UTF8(*Error));return StoreStructValue(MoveTemp(V),C);
}
FDasStructValue SoftUpcast(const FDasStructValue& H,const char* SourcePath,const char* TargetPath,das::Context* C){
 const auto Checked=SoftCheck(H,SourcePath,C);FInstancedStruct Result;FString Error;
 if(!UpcastDasSoftValue(ResolveStructValue(Checked,C),TargetPath?UTF8_TO_TCHAR(TargetPath):TEXT(""),Result,Error))C->throw_error(TCHAR_TO_UTF8(*Error));
 return StoreStructValue(MoveTemp(Result),C);
}
const FInstancedStruct& SoftValue(const FDasStructValue& H,das::Context* C){const auto& V=ResolveStructValue(H,C);if(!DasSoftProperty(V.GetScriptStruct()->GetPathName()))C->throw_error("expected soft reference value");FString Error;if(!ValidateDasStructSnapshot(V,Error))C->throw_error(TCHAR_TO_UTF8(*Error));return V;}
FSoftObjectPath SoftAssetPath(const FInstancedStruct& V){auto* P=DasSoftProperty(V.GetScriptStruct()->GetPathName());return P->GetPropertyValue(P->ContainerPtrToValuePtr<void>(V.GetMemory())).ToSoftObjectPath();}
char* SoftPath(const FDasStructValue& H,das::Context* C){return CopyDSLString(SoftAssetPath(SoftValue(H,C)).ToString(),C);}
bool SoftNull(const FDasStructValue& H,das::Context* C){return SoftAssetPath(SoftValue(H,C)).IsNull();}
bool SoftLoaded(const FDasStructValue& H,das::Context* C){return SoftAssetPath(SoftValue(H,C)).ResolveObject()!=nullptr;}
UObject* ResolveSoft(const FDasStructValue& H,bool Load,bool Class,das::Context* C){
 const auto& V=SoftValue(H,C);auto* P=DasSoftProperty(V.GetScriptStruct()->GetPathName());auto* ClassProperty=CastField<FSoftClassProperty>(P);if((ClassProperty!=nullptr)!=Class)C->throw_error("soft object/class kind mismatch");const auto Path=SoftAssetPath(V);auto* Object=Load?Path.TryLoad():Path.ResolveObject();
 if(!DasSoftTargetMatches(Object,ClassProperty?ClassProperty->MetaClass.Get():P->PropertyClass.Get(),Class))C->throw_error("loaded soft reference type mismatch");return Object;
}
FDasObjectHandle SoftGetObject(const FDasStructValue& H,bool Load,das::Context* C){return MakeObjectHandle(ResolveSoft(H,Load,false,C),UObject::StaticClass(),C);}
FDasClassHandle SoftGetClass(const FDasStructValue& H,bool Load,das::Context* C){return {MakeObjectHandle(ResolveSoft(H,Load,true,C),UClass::StaticClass(),C).Token};}
void ScriptSoftAssetLoad(const char*,const FDasStructValue&,const char*,das::Context*);
