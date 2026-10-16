#include "Editor.h"
#include "Misc/CoreDelegates.h"
#include "Engine/Blueprint.h"
#include "K2Node_FunctionEntry.h"

namespace {
// Blueprint compilation may rebuild entry flags. Apply the declared policy on
// every compilation, including child creation and skeleton/full recompilation.
void ApplyDasFunctionPolicy(UBlueprint* Blueprint) {
    if(!Blueprint)return;
    for(UEdGraph* Graph:Blueprint->FunctionGraphs){
        TArray<UK2Node_FunctionEntry*> Entries;Graph->GetNodesOfClass(Entries);
        for(auto* Entry:Entries){
            const FString Kind=Entry->MetaData.GetMetaData(TEXT("DasFunctionKind"));
            if(Kind.IsEmpty())continue;
            Entry->ClearExtraFlags(FUNC_Final|FUNC_BlueprintPure|FUNC_Const|FUNC_Static);
            if(Entry->MetaData.GetMetaData(TEXT("DasFunctionPure"))==TEXT("true"))Entry->AddExtraFlags(FUNC_BlueprintPure|FUNC_BlueprintCallable);
            if(Entry->MetaData.GetMetaData(TEXT("DasFunctionConst"))==TEXT("true"))Entry->AddExtraFlags(FUNC_Const);
            if(Entry->MetaData.GetMetaData(TEXT("DasFunctionStatic"))==TEXT("true")){Entry->AddExtraFlags(FUNC_Static);const FString World=Entry->MetaData.GetMetaData(TEXT("DasWorldContext"));if(World.IsEmpty())Entry->MetaData.SetMetaData(TEXT("CallableWithoutWorldContext"),FString(TEXT("true")));else{Entry->MetaData.SetMetaData(TEXT("WorldContext"),World);Entry->MetaData.RemoveMetaData(TEXT("CallableWithoutWorldContext"));}}
            if(Kind==TEXT("Callable"))Entry->AddExtraFlags(FUNC_Final);
        }
    }
}
struct FDasFunctionPolicyRegistration {
    FDelegateHandle InitHandle,CompileHandle;
    TWeakObjectPtr<UEditorEngine> Editor;
    void Install(){if(GEditor && !CompileHandle.IsValid()){Editor=GEditor;CompileHandle=GEditor->OnBlueprintPreCompile().AddStatic(&ApplyDasFunctionPolicy);}}
    FDasFunctionPolicyRegistration(){Install();InitHandle=FCoreDelegates::GetOnPostEngineInit().AddRaw(this,&FDasFunctionPolicyRegistration::Install);}
    ~FDasFunctionPolicyRegistration(){FCoreDelegates::GetOnPostEngineInit().Remove(InitHandle);if(Editor.IsValid())Editor->OnBlueprintPreCompile().Remove(CompileHandle);}
} Registration;
}