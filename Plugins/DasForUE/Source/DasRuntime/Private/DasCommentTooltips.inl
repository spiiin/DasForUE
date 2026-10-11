// Comments come from daScript's lexer, so comment-like text inside strings is
// never documentation. Cache lifetime is one outer DSL compilation.
struct FDSLDocComment {uint32 First=0,Last=0;FString Text;};
TMap<const das::FileInfo*,TArray<FDSLDocComment>> DSLDocComments;
void DSLResetDocComments(){DSLDocComments.Reset();}
FString DSLDocLine(das::FileInfo* File,uint32 Line){const char* Text=nullptr;uint32 Size=0;if(!File || !File->getLine(Line,Text,Size))return {};return FString(FUTF8ToTCHAR(Text,int32(Size)).Length(),FUTF8ToTCHAR(Text,int32(Size)).Get());}
struct FUECommentReader:das::CommentReader {
    das::LineInfo Start;das::string Text;bool LineComment=false;
    void open(bool Cpp,const das::LineInfo& At)override{Start=At;Text.clear();LineComment=Cpp || DSLDocLine(At.fileInfo,At.line).TrimStart().StartsWith(TEXT("//"));}
    void accept(int Ch,const das::LineInfo&)override{if(Text.size()<65536)Text.push_back(char(Ch));}
    void close(const das::LineInfo& At)override{
        const FString Source=DSLDocLine(Start.fileInfo,Start.line).TrimStart();
        if(!Source.StartsWith(LineComment?TEXT("//"):TEXT("/*")))return; // no trailing comments
        FString Value=UTF8_TO_TCHAR(Text.c_str());TArray<FString> Lines;Value.ReplaceInline(TEXT("\r"),TEXT(""));Value.ParseIntoArrayLines(Lines,false);
        for(auto& Line:Lines){Line=Line.TrimStartAndEnd();if(!LineComment && Line.StartsWith(TEXT("*")))Line=Line.RightChop(1).TrimStart();}
        Value=FString::Join(Lines,TEXT("\n")).TrimStartAndEnd();
        if(LineComment && Source.StartsWith(TEXT("///")) && Value.StartsWith(TEXT("/")))Value=Value.RightChop(1).TrimStart();
        if(!Value.IsEmpty())DSLDocComments.FindOrAdd(Start.fileInfo).Add({Start.line,LineComment?Start.line:At.line,Value});
    }
#define DAS_DOC_NOOP(Name) void Name(const das::LineInfo&)override{}
    DAS_DOC_NOOP(beforeStructure) DAS_DOC_NOOP(beforeStructureFields) DAS_DOC_NOOP(afterStructureFields)
    DAS_DOC_NOOP(beforeFunction) DAS_DOC_NOOP(beforeGlobalVariables) DAS_DOC_NOOP(afterGlobalVariables)
    DAS_DOC_NOOP(beforeVariant) DAS_DOC_NOOP(beforeVariantEntries) DAS_DOC_NOOP(afterVariantEntries)
    DAS_DOC_NOOP(beforeTuple) DAS_DOC_NOOP(beforeTupleEntries) DAS_DOC_NOOP(afterTupleEntries)
    DAS_DOC_NOOP(beforeBitfield) DAS_DOC_NOOP(beforeBitfieldEntries) DAS_DOC_NOOP(afterBitfieldEntries)
    DAS_DOC_NOOP(beforeEnumeration) DAS_DOC_NOOP(beforeEnumerationEntries) DAS_DOC_NOOP(afterEnumerationEntries)
    DAS_DOC_NOOP(beforeAlias)
#undef DAS_DOC_NOOP
#define DAS_DOC_NAME_NOOP(Name) void Name(const char*,const das::LineInfo&)override{}
    DAS_DOC_NAME_NOOP(afterStructureField) DAS_DOC_NAME_NOOP(afterGlobalVariable)
    DAS_DOC_NAME_NOOP(afterVariantEntry) DAS_DOC_NAME_NOOP(afterVariant)
    DAS_DOC_NAME_NOOP(afterTupleEntry) DAS_DOC_NAME_NOOP(afterTuple)
    DAS_DOC_NAME_NOOP(afterBitfieldEntry) DAS_DOC_NAME_NOOP(afterBitfield)
    DAS_DOC_NAME_NOOP(afterEnumerationEntry) DAS_DOC_NAME_NOOP(afterEnumeration)
    DAS_DOC_NAME_NOOP(afterAlias)
#undef DAS_DOC_NAME_NOOP
    void afterStructure(das::Structure*,const das::LineInfo&)override{}
    void afterFunction(das::Function*,const das::LineInfo&)override{}
};
void DSLCommentTooltip(const das::LineInfo& At,das::AnnotationArgumentList& Args,const das::AnnotationList* Annotations=nullptr){
    for(const auto& A:Args)if(A.name=="ToolTip")return;
    uint32 First=At.line;if(!At.fileInfo || First==0)return;
    for(const auto& A:Args)if(A.at.fileInfo==At.fileInfo && A.at.line)First=FMath::Min(First,A.at.line);
    if(Annotations)for(const auto* A:*Annotations)if(A->at.fileInfo==At.fileInfo && A->at.line)First=FMath::Min(First,A->at.line);
    while(First>1){const FString Line=DSLDocLine(At.fileInfo,First-1).TrimStartAndEnd();if(Line.StartsWith(TEXT("@")) || Line.StartsWith(TEXT("[")))--First;else break;}
    const auto* Comments=DSLDocComments.Find(At.fileInfo);if(!Comments)return;
    TArray<FString> Parts;
    while(First>1){const auto* Found=Comments->FindByPredicate([&](const auto& C){return C.Last==First-1;});if(!Found)break;Parts.Insert(Found->Text,0);First=Found->First;}
    const FString Value=FString::Join(Parts,TEXT("\n"));if(Value.Len()>4096)DSLFail(TEXT("Comment ToolTip exceeds 4096 characters"));
    if(!Value.IsEmpty())Args.push_back(das::AnnotationArgument("ToolTip",DSLUtf8(Value)));
}
