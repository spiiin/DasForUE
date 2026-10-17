FDasDeclaredClass DSLReferenceDeclaration(const das::TypeDeclPtr&);
// UObject references are structs; table keys need a nominal by-value type.
struct FDSLObjectKeyAnnotation:das::TypeAnnotation {
    FString ClassPath,AuthoredId;das::string Wrapper;
    FDSLObjectKeyAnnotation(const das::string& N,const FString& P,const FString& Id,const das::string& W):TypeAnnotation(N,"uint64_t"),ClassPath(P),AuthoredId(Id),Wrapper(W){}
    virtual size_t getSizeOf()const override{return sizeof(uint64);}
    virtual size_t getAlignOf()const override{return alignof(uint64);}
    virtual bool isRefType()const override{return false;}
    virtual bool isLocal()const override{return true;}
    virtual bool isPod()const override{return true;}
    virtual bool isRawPod()const override{return true;}
    virtual bool canCopy()const override{return true;}
    virtual bool canMove()const override{return true;}
    virtual bool canClone()const override{return true;}
    virtual bool canBePlacedInContainer()const override{return true;}
    virtual bool hasNonTrivialCtor()const override{return false;}
    virtual bool hasNonTrivialDtor()const override{return false;}
    virtual das::TypeDeclPtr makeValueType()const override{return new das::TypeDecl(das::Type::tUInt64,das::LineInfo());}
    virtual das::SimNode* simulateClone(das::Context& C,const das::LineInfo& At,das::SimNode* L,das::SimNode* R)const override{return C.code->makeNode<das::SimNode_Set<uint64>>(At,L,R);}
};
bool DSLObjectKey(const das::TypeDeclPtr& T){return T && T->baseType==das::Type::tHandle && T->annotation && T->annotation->name.find("__ue_object_key_")==0;}
const FDSLObjectKeyAnnotation* DSLKeyAnnotation(const das::TypeDeclPtr& T){if(!DSLObjectKey(T))DSLFail(TEXT("Expected a typed UObject key"));return static_cast<const FDSLObjectKeyAnnotation*>(T->annotation);}
das::TypeDeclPtr DSLMakeObjectKeyType(const das::TypeDeclPtr& Ref,das::ModuleGroup& G,das::Module* M,const das::LineInfo& At){
    if(!DSLTypedReference(Ref) || DSLTypedInterface(Ref))DSLFail(TEXT("UEObjectKey requires a native or authored UObject reference, not an interface"));
    const FString Path=DSLTypedReferencePath(Ref),Id=DSLAuthoredView(Ref)?DSLReferenceDeclaration(Ref).Id:FString();const das::string Wrapper=Ref->structType->module->name+"::"+Ref->structType->name;
    const das::string Name=DSLUtf8(TEXT("__ue_object_key_")+FMD5::HashAnsiString(*(Path+TEXT("|")+Id+TEXT("|")+DSLText(Wrapper))));auto Existing=G.findAnnotation(Name,M);das::TypeAnnotation* A=nullptr;
    if(Existing.size()>1)DSLFail(TEXT("Ambiguous object key annotation; import one canonical key module"));
    if(Existing.size()==1)A=static_cast<das::TypeAnnotation*>(Existing[0]);else {auto* New=new FDSLObjectKeyAnnotation(Name,Path,Id,Wrapper);if(!M->addAnnotation(New))DSLFail(TEXT("Object key annotation collision"));A=New;}
    auto* T=new das::TypeDecl(das::Type::tHandle,At);T->annotation=A;return T;
}
das::ExpressionPtr DSLKeyCast(const das::LineInfo& At,const das::TypeDeclPtr& T,das::ExpressionPtr V){auto* C=new das::ExprCast(At,V,DSLObjectCopyType(T));C->castType->ref=false;C->castType->constant=false;C->reinterpret=true;auto* U=new das::ExprUnsafe(At);U->body=C;return U;}
das::ExpressionPtr DSLKeyBits(const das::LineInfo& At,das::ExpressionPtr V){return DSLKeyCast(At,new das::TypeDecl(das::Type::tUInt64,At),V);}
das::ExpressionPtr DSLKeyUnpack(const das::LineInfo& At,const das::TypeDeclPtr& T,das::ExpressionPtr V){const auto* A=DSLKeyAnnotation(T);return DSLKeyCast(At,T,DSLCall(At,"__ue_object_key_check",{V,new das::ExprConstString(At,DSLUtf8(A->ClassPath)),new das::ExprConstString(At,DSLUtf8(A->AuthoredId))}));}
das::ExpressionPtr DSLKeyPack(const das::LineInfo& At,const das::TypeDeclPtr& T,das::ExpressionPtr V){const auto* A=DSLKeyAnnotation(T);return DSLCall(At,"__ue_object_key_check",{DSLKeyBits(At,V),new das::ExprConstString(At,DSLUtf8(A->ClassPath)),new das::ExprConstString(At,DSLUtf8(A->AuthoredId))});}
struct FUEObjectKeyTypeMacro:das::TypeMacro {
    FUEObjectKeyTypeMacro():TypeMacro("UEObjectKey"){}
    virtual das::TypeDeclPtr visit(das::Program* P,das::Module* M,const das::TypeDeclPtr& Type,const das::TypeDeclPtr&)override{try{if(Type->typeMacroExpr.size()!=2 || !Type->typeMacroExpr[1]->rtti_isTypeDecl())DSLFail(TEXT("UEObjectKey requires one reference type"));auto Ref=DSLObjectNormalize(static_cast<das::ExprTypeDecl*>(Type->typeMacroExpr[1])->typeexpr,*P->thisModuleGroup,M);auto Result=DSLMakeObjectKeyType(Ref,*P->thisModuleGroup,M,Type->at);das::TypeDecl::applyAutoContracts(Result,Type);return Result;}catch(const std::exception& X){P->error("UEObjectKey failed",X.what(),"",Type->at,das::CompilationError::invalid_type);return nullptr;}}
};
struct FUEObjectKeyFactory:das::CallMacro {
    int Mode;
    FUEObjectKeyFactory(int InMode):CallMacro(InMode==0?"ue_key":InMode==1?"ue_key_ref":"ue_key_valid"),Mode(InMode){}
    virtual bool canVisitArguments(das::ExprCallMacro* E,int I)override{return I!=0 || !E->arguments[0]->rtti_isTypeDecl();}
    virtual das::ExpressionPtr visit(das::Program* P,das::Module* M,das::ExprCallMacro* E)override{try{
        if(Mode){if(E->arguments.size()!=1 || !DSLObjectKey(E->arguments[0]->type))DSLFail(TEXT("Expected one typed object key"));auto T=E->arguments[0]->type;const auto* A=DSLKeyAnnotation(T);if(Mode==2)return DSLCall(E->at,"__ue_object_key_valid",{DSLKeyBits(E->at,E->arguments[0])});auto Refs=P->thisModuleGroup->findStructure(A->Wrapper,M);if(Refs.size()!=1)DSLFail(TEXT("Import the key's reference wrapper module"));auto* Ref=new das::TypeDecl(das::Type::tStructure,E->at);Ref->structType=Refs[0];return DSLDirectUnpack(E->at,Ref,DSLCall(E->at,"__ue_object_key_reference",{DSLKeyPack(E->at,T,E->arguments[0]),new das::ExprConstString(E->at,DSLUtf8(A->ClassPath)),new das::ExprConstString(E->at,DSLUtf8(A->AuthoredId))}),A->ClassPath);}
        das::TypeDeclPtr Key=nullptr;das::ExpressionPtr Value=nullptr;if(E->arguments.size()==2 && E->arguments[0]->rtti_isTypeDecl()){Key=DSLObjectNormalize(static_cast<das::ExprTypeDecl*>(E->arguments[0])->typeexpr,*P->thisModuleGroup,M);Value=E->arguments[1];}else if(E->arguments.size()==1 && DSLTypedReference(E->arguments[0]->type)){Value=E->arguments[0];Key=DSLMakeObjectKeyType(Value->type,*P->thisModuleGroup,M,E->at);}else DSLFail(TEXT("Expected ue_key(reference) or ue_key(type<Key>,reference/handle)"));
        const auto* A=DSLKeyAnnotation(Key);das::ExpressionPtr Raw=Value;if(DSLTypedReference(Value->type))Raw=DSLDirectPack(E->at,Value->type,Value,DSLTypedReferencePath(Value->type));else if(!Value->type || Value->type->baseType!=das::Type::tHandle || !Value->type->annotation || Value->type->annotation->name!="UObjectHandle")DSLFail(TEXT("Object key construction requires a typed UObject reference or UObjectHandle"));
        return DSLKeyCast(E->at,Key,DSLCall(E->at,"__ue_object_key_from_ref",{Raw,new das::ExprConstString(E->at,DSLUtf8(A->ClassPath)),new das::ExprConstString(E->at,DSLUtf8(A->AuthoredId))}));
    }catch(const std::exception& X){P->error("Object key conversion failed",X.what(),"",E->at,das::CompilationError::invalid_type);return nullptr;}}
};
