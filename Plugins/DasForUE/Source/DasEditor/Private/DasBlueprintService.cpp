#include "DasBlueprintService.h"
#include "DasAuthoring.h"
#include "DasProbeActor.h"
#include "DasRuntime.h"
#include "Editor.h"
#include "Engine/Blueprint.h"
#include "Engine/BlueprintGeneratedClass.h"
#include "Engine/Level.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "EdGraph/EdGraph.h"
#include "EdGraphSchema_K2.h"
#include "K2Node_CallFunction.h"
#include "K2Node_MakeArray.h"
#include "K2Node_MakeStruct.h"
#include "K2Node_BreakStruct.h"
#include "K2Node_FunctionEntry.h"
#include "K2Node_FunctionResult.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "Kismet2/KismetEditorUtilities.h"
#include "BlueprintCompilationManager.h"
#include "Misc/PackageName.h"
#include "Misc/Paths.h"
#include "Misc/FileHelper.h"
#include "HAL/FileManager.h"
#include "Modules/ModuleManager.h"
#include "UObject/SavePackage.h"
#include "UObject/StrongObjectPtr.h"
#include "UObject/StructOnScope.h"
#include "UObject/UnrealType.h"
#include "UObject/MetaData.h"

namespace
{
    template<class T> T* Node(UEdGraph* Graph)
    {
        FGraphNodeCreator<T> Creator(*Graph);
        T* Result = Creator.CreateNode();
        Creator.Finalize();
        return Result;
    }

    FEdGraphPinType ScalarPin(EDasScalarKind K){FEdGraphPinType P;P.PinCategory=K==EDasScalarKind::Int32?UEdGraphSchema_K2::PC_Int:K==EDasScalarKind::Bool?UEdGraphSchema_K2::PC_Boolean:K==EDasScalarKind::String?UEdGraphSchema_K2::PC_String:K==EDasScalarKind::Name?UEdGraphSchema_K2::PC_Name:UEdGraphSchema_K2::PC_Real;if(K==EDasScalarKind::Float32 || K==EDasScalarKind::Double)P.PinSubCategory=K==EDasScalarKind::Float32?UEdGraphSchema_K2::PC_Float:UEdGraphSchema_K2::PC_Double;return P;}
    bool ScalarProperty(FProperty* P,EDasMethodSignature K){return K==EDasMethodSignature::Int?CastField<FIntProperty>(P)!=nullptr:K==EDasMethodSignature::Float?CastField<FFloatProperty>(P)!=nullptr:K==EDasMethodSignature::Bool?CastField<FBoolProperty>(P)!=nullptr:K==EDasMethodSignature::String?CastField<FStrProperty>(P)!=nullptr:K==EDasMethodSignature::Double?CastField<FDoubleProperty>(P)!=nullptr:CastField<FNameProperty>(P)!=nullptr;}
    FName FrameField(EDasMethodSignature K){return K==EDasMethodSignature::Int?TEXT("IntValue"):K==EDasMethodSignature::Float?TEXT("FloatValue"):K==EDasMethodSignature::Bool?TEXT("BoolValue"):K==EDasMethodSignature::String?TEXT("StringValue"):K==EDasMethodSignature::Double?TEXT("DoubleValue"):TEXT("NameValue");}
    bool AdapterCall(const UK2Node_CallFunction* Call)
    {const auto* T=Call->GetTargetFunction();for(const auto Name:{GET_FUNCTION_NAME_CHECKED(ADasProbeActor,ExecuteDas),GET_FUNCTION_NAME_CHECKED(ADasProbeActor,ExecuteDasNamed),GET_FUNCTION_NAME_CHECKED(ADasProbeActor,ExecuteDasFloat),GET_FUNCTION_NAME_CHECKED(ADasProbeActor,ExecuteDasBool),GET_FUNCTION_NAME_CHECKED(ADasProbeActor,ExecuteDasVoid),GET_FUNCTION_NAME_CHECKED(ADasProbeActor,ExecuteDasFrame)}) if(T==ADasProbeActor::StaticClass()->FindFunctionByName(Name)) return true;return false;}
    bool MethodShape(UFunction* F,const FDasMethodSchema& M)
    {
        if(!F) return false;const auto C=M.Contract();const auto Inputs=M.EffectiveParameters();
        auto Matches=[](FProperty* P,EDasMethodSignature K){return ScalarProperty(P,K);};
        if(F->NumParms!=Inputs.Num()+(C.ReturnKind==EDasMethodSignature::Void?0:1)) return false;
        int32 Index=0;for(TFieldIterator<FProperty> It(F);It;++It) if(It->HasAnyPropertyFlags(CPF_Parm) && !It->HasAnyPropertyFlags(CPF_ReturnParm)){if(!Inputs.IsValidIndex(Index) || It->GetFName()!=Inputs[Index].Name) return false;++Index;}if(Index!=Inputs.Num()) return false;
        for(const auto& P:Inputs){auto* Actual=F->FindPropertyByName(P.Name);if(!Matches(Actual,P.Kind) || Actual->HasAnyPropertyFlags(CPF_OutParm|CPF_ReferenceParm)) return false;}
        return C.ReturnKind==EDasMethodSignature::Void?!F->GetReturnProperty():Matches(F->GetReturnProperty(),C.ReturnKind);

    }
    void MethodMetadata(UEdGraph* Graph,const FDasMethodSchema& Method)
    {
        TArray<UK2Node_FunctionEntry*> Entries;Graph->GetNodesOfClass(Entries);
        for (auto* Entry:Entries)
        {
            for (const auto* Key:{TEXT("Category"),TEXT("DisplayName"),TEXT("ToolTip")}) Entry->MetaData.RemoveMetaData(Key);
            for (const auto& Pair:Method.Metadata) Entry->MetaData.SetMetaData(*Pair.Key,FString(Pair.Value));
        }
    }
    template<class T> T* StructNode(UEdGraph* G){auto* N=NewObject<T>(G);N->StructType=FDasScalarValue::StaticStruct();G->AddNode(N,false,false);N->CreateNewGuid();N->PostPlacedNewNode();N->AllocateDefaultPins();return N;}
    bool SetFrameMethod(UBlueprint* BP,const FDasMethodSchema& M)
    {
        UEdGraph* G=nullptr;for(UEdGraph* Existing:BP->FunctionGraphs) if(Existing->GetFName()==M.Name) G=Existing;
        if(G){for(UEdGraphNode* Item:G->Nodes) if(auto* Call=Cast<UK2Node_CallFunction>(Item)) if(Call->GetTargetFunction()==ADasProbeActor::StaticClass()->FindFunctionByName(GET_FUNCTION_NAME_CHECKED(ADasProbeActor,ExecuteDasFrame))){G->GetSchema()->TrySetDefaultValue(*Call->FindPinChecked(TEXT("ScriptFile")),M.ScriptFile);G->GetSchema()->TrySetDefaultValue(*Call->FindPinChecked(TEXT("EntryPoint")),M.EntryPoint);MethodMetadata(G,M);return true;}return false;}
        G=FBlueprintEditorUtils::CreateNewGraph(BP,M.Name,UEdGraph::StaticClass(),UEdGraphSchema_K2::StaticClass());FBlueprintEditorUtils::AddFunctionGraph<UClass>(BP,G,true,nullptr);
        TArray<UK2Node_FunctionEntry*> Entries;G->GetNodesOfClass(Entries);if(Entries.Num()!=1) return false;auto* Entry=Entries[0];
        for(const auto& P:M.Parameters) Entry->CreateUserDefinedPin(P.Name,ScalarPin(static_cast<EDasScalarKind>(P.Kind)),EGPD_Output);
        auto* Result=Node<UK2Node_FunctionResult>(G);if(M.ReturnKind!=EDasMethodSignature::Void) Result->CreateUserDefinedPin(UEdGraphSchema_K2::PN_ReturnValue,ScalarPin(static_cast<EDasScalarKind>(M.ReturnKind)),EGPD_Input);
        auto* Call=NewObject<UK2Node_CallFunction>(G);Call->SetFromFunction(ADasProbeActor::StaticClass()->FindFunctionByName(GET_FUNCTION_NAME_CHECKED(ADasProbeActor,ExecuteDasFrame)));G->AddNode(Call,false,false);Call->CreateNewGuid();Call->PostPlacedNewNode();Call->AllocateDefaultPins();
        const auto* Schema=G->GetSchema();Schema->TrySetDefaultValue(*Call->FindPinChecked(TEXT("ScriptFile")),M.ScriptFile);Schema->TrySetDefaultValue(*Call->FindPinChecked(TEXT("EntryPoint")),M.EntryPoint);Schema->TrySetDefaultValue(*Call->FindPinChecked(TEXT("ResultKind")),FString::FromInt(static_cast<int32>(M.ReturnKind)));
        bool Linked=Schema->TryCreateConnection(Entry->FindPinChecked(UEdGraphSchema_K2::PN_Then),Call->GetExecPin()) && Schema->TryCreateConnection(Call->GetThenPin(),Result->FindPinChecked(UEdGraphSchema_K2::PN_Execute));
        auto* Array=NewObject<UK2Node_MakeArray>(G);Array->NumInputs=M.Parameters.Num();G->AddNode(Array,false,false);Array->CreateNewGuid();Array->PostPlacedNewNode();Array->AllocateDefaultPins();
        Linked &= Schema->TryCreateConnection(Array->GetOutputPin(),Call->FindPinChecked(TEXT("Arguments")));
        for(int32 I=0;I<M.Parameters.Num();++I){const auto& P=M.Parameters[I];auto* Make=StructNode<UK2Node_MakeStruct>(G);Schema->TrySetDefaultValue(*Make->FindPinChecked(TEXT("Kind")),FString::FromInt(static_cast<int32>(P.Kind)));const FName Field=FrameField(P.Kind);Linked &= Schema->TryCreateConnection(Entry->FindPinChecked(P.Name),Make->FindPinChecked(Field));Linked &= Schema->TryCreateConnection(Make->FindPinChecked(FDasScalarValue::StaticStruct()->GetFName()),Array->FindPinChecked(Array->GetPinName(I)));}
        if(M.ReturnKind!=EDasMethodSignature::Void){auto* Break=StructNode<UK2Node_BreakStruct>(G);Linked &= Schema->TryCreateConnection(Call->GetReturnValuePin(),Break->FindPinChecked(FDasScalarValue::StaticStruct()->GetFName()));const FName Field=FrameField(M.ReturnKind);Linked &= Schema->TryCreateConnection(Break->FindPinChecked(Field),Result->FindPinChecked(UEdGraphSchema_K2::PN_ReturnValue));}
        MethodMetadata(G,M);FBlueprintEditorUtils::MarkBlueprintAsStructurallyModified(BP);return Linked;
    }
    bool SetMethod(UBlueprint* BP, const FDasMethodSchema& Method)
    {
        if(Method.Signature==EDasMethodSignature::Frame) return SetFrameMethod(BP,Method);
        const bool Action=Method.Signature==EDasMethodSignature::Void;
        const FName TargetName=Action?GET_FUNCTION_NAME_CHECKED(ADasProbeActor,ExecuteDasVoid):Method.Signature==EDasMethodSignature::Float?GET_FUNCTION_NAME_CHECKED(ADasProbeActor,ExecuteDasFloat):Method.Signature==EDasMethodSignature::Bool?GET_FUNCTION_NAME_CHECKED(ADasProbeActor,ExecuteDasBool):Method.EntryPoint==TEXT("main")?GET_FUNCTION_NAME_CHECKED(ADasProbeActor,ExecuteDas):GET_FUNCTION_NAME_CHECKED(ADasProbeActor,ExecuteDasNamed);
        UFunction* Target=ADasProbeActor::StaticClass()->FindFunctionByName(TargetName);
        UEdGraph* Graph = nullptr;
        for (UEdGraph* Existing : BP->FunctionGraphs)
            if (Existing->GetFName() == Method.Name) Graph = Existing;
        if (Graph)
        {
            for (UEdGraphNode* Item : Graph->Nodes)
                if (auto* Call = Cast<UK2Node_CallFunction>(Item))
                    if (AdapterCall(Call))
                    if (auto* Pin = Call->FindPin(TEXT("ScriptFile")))
                    {
                        if (Call->GetTargetFunction()!=Target) {Call->SetFromFunction(Target);Call->ReconstructNode();}
                        Graph->GetSchema()->TrySetDefaultValue(*Call->FindPinChecked(TEXT("ScriptFile")),Method.ScriptFile);
                        if (auto* EntryPin=Call->FindPin(TEXT("EntryPoint"))) Graph->GetSchema()->TrySetDefaultValue(*EntryPin,Method.EntryPoint);
                        MethodMetadata(Graph,Method);
                        FBlueprintEditorUtils::MarkBlueprintAsModified(BP);
                        return true;
                    }
            return false;
        }
        Graph = FBlueprintEditorUtils::CreateNewGraph(BP, Method.Name, UEdGraph::StaticClass(), UEdGraphSchema_K2::StaticClass());
        FBlueprintEditorUtils::AddFunctionGraph<UClass>(BP, Graph, true, nullptr);
        TArray<UK2Node_FunctionEntry*> Entries;
        Graph->GetNodesOfClass(Entries);
        if (Entries.Num() != 1) return false;
        auto* Entry = Entries[0];
        const auto ValueType=ScalarPin(Method.Signature==EDasMethodSignature::Float?EDasScalarKind::Float32:Method.Signature==EDasMethodSignature::Bool?EDasScalarKind::Bool:EDasScalarKind::Int32);
        if(!Action) Entry->CreateUserDefinedPin(TEXT("Input"),ValueType,EGPD_Output);
        TArray<UK2Node_FunctionResult*> Results;
        Graph->GetNodesOfClass(Results);
        auto* Result = Results.Num() ? Results[0] : Node<UK2Node_FunctionResult>(Graph);
        if(!Action) Result->CreateUserDefinedPin(UEdGraphSchema_K2::PN_ReturnValue,ValueType,EGPD_Input);
        auto* Call = NewObject<UK2Node_CallFunction>(Graph);
        Call->SetFromFunction(Target);
        Graph->AddNode(Call, false, false);
        Call->CreateNewGuid();
        Call->PostPlacedNewNode();
        Call->AllocateDefaultPins();
        const auto* Schema = Graph->GetSchema();
        Schema->TrySetDefaultValue(*Call->FindPinChecked(TEXT("ScriptFile")), Method.ScriptFile);
        if (auto* EntryPin=Call->FindPin(TEXT("EntryPoint"))) Schema->TrySetDefaultValue(*EntryPin,Method.EntryPoint);
        MethodMetadata(Graph,Method);
        const bool Linked =
            Schema->TryCreateConnection(Entry->FindPinChecked(UEdGraphSchema_K2::PN_Then), Call->GetExecPin()) &&
            Schema->TryCreateConnection(Call->GetThenPin(), Result->FindPinChecked(UEdGraphSchema_K2::PN_Execute)) &&
            (Action || (Schema->TryCreateConnection(Entry->FindPinChecked(TEXT("Input")),Call->FindPinChecked(TEXT("Input"))) && Schema->TryCreateConnection(Call->GetReturnValuePin(),Result->FindPinChecked(UEdGraphSchema_K2::PN_ReturnValue))));
        FBlueprintEditorUtils::MarkBlueprintAsStructurallyModified(BP);
        return Linked;
    }

}

bool FDasBlueprintService::ReadSchema(const FString& ScriptFile, FDasClassSchema& Out, FString& Error)
{
    Error.Reset();
    if (!IsInGameThread()) { Error = TEXT("Schema compilation requires the game thread"); return false; }
    auto& Runtime = FModuleManager::LoadModuleChecked<FDasRuntimeModule>(TEXT("DasRuntime"));
    const auto Candidate = Runtime.RunFile(Runtime.ScriptPath(ScriptFile), 0);
    if (!Candidate.bSucceeded || Candidate.Types.Num() != 1)
    { Error = Candidate.Error.IsEmpty() ? TEXT("Expected one type declaration") : Candidate.Error; return false; }
    return FDasClassSchema::FromDeclaration(Candidate.Types[0], ScriptFile, Out, Error);
}

bool FDasBlueprintService::ApplyAuthoring(UBlueprint* BP, const FDasAuthoringModule& Module, const FString& ClassId, FString& Error)
{
    FDasClassSchema Candidate;
    if (!Module.ToActorSchema(ClassId, Candidate, Error)) return false;
    auto& Runtime=FModuleManager::LoadModuleChecked<FDasRuntimeModule>(TEXT("DasRuntime"));
    for (const auto& Method:Candidate.EffectiveMethods())
        if (!Runtime.ValidateFrameMethod(Runtime.ScriptPath(Method.ScriptFile),Error,Method.EntryPoint,Method.Contract())) return false;
    return Apply(BP, Candidate, Error);
}

bool FDasBlueprintService::Apply(UBlueprint* BP, const FDasClassSchema& Schema, FString& Error, bool bValidateOnly)
{
    Error.Reset();
    if (!IsInGameThread() || !IsValid(BP) || !BP->GeneratedClass || !BP->ParentClass || !BP->ParentClass->IsChildOf(ADasProbeActor::StaticClass()))
    { Error = TEXT("Expected an Actor adapter Blueprint on the game thread"); return false; }
    if (!Schema.Validate(Error)) return false;
    const FString ExistingId = BP->GetOutermost()->GetMetaData().GetValue(BP,TEXT("DasClassId"));
    if (!ExistingId.IsEmpty() && Schema.StableId != ExistingId)
    { Error = TEXT("Authored class stable ID changed"); return false; }
    const auto Methods=Schema.EffectiveMethods();
    const FString ExistingName=BP->GetOutermost()->GetMetaData().GetValue(BP,TEXT("DasDeclaredName"));
    if (!ExistingName.IsEmpty() && ExistingName!=Schema.Name.ToString()) {Error=TEXT("Renaming authored class requires migration");return false;}
    const FString LegacyMethodName=BP->GetOutermost()->GetMetaData().GetValue(BP,TEXT("DasMethodName"));
    if (!LegacyMethodName.IsEmpty())
    {
        const auto* M=Methods.FindByPredicate([&](const auto& X){return X.Name==FName(*LegacyMethodName);});
        if (!M || M->StableId!=BP->GetOutermost()->GetMetaData().GetValue(BP,TEXT("DasMethodId")) || M->InputStableId!=BP->GetOutermost()->GetMetaData().GetValue(BP,TEXT("DasInputId")))
        {Error=TEXT("Legacy authored method identity changed");return false;}
    }
    for (const auto& Field:Schema.Fields)
    {
        FString Prior;FBlueprintEditorUtils::GetBlueprintVariableMetaData(BP,Field.Name,nullptr,TEXT("DasMemberId"),Prior);
        if (!Prior.IsEmpty() && Prior!=Field.StableId) {Error=TEXT("Authored property stable ID changed");return false;}
    }
    for (UEdGraph* Graph:BP->FunctionGraphs)
    {
        const FString Prior=BP->GetOutermost()->GetMetaData().GetValue(Graph,TEXT("DasMemberId"));
        if (!Prior.IsEmpty() && !Methods.ContainsByPredicate([&](const auto& M){return M.StableId==Prior && M.Name==Graph->GetFName();}))
        {Error=TEXT("Method removal/rename requires migration");return false;}
    }
    for (const auto& M:Methods)
    {
        if (BP->ParentClass->FindFunctionByName(M.Name) || BP->ParentClass->FindPropertyByName(M.Name)) {Error=TEXT("Method conflicts with inherited member");return false;}
        if (UFunction* Prior=BP->GeneratedClass->FindFunctionByName(M.Name))
        {
            const FString PriorId=Prior->GetMetaData(TEXT("DasMemberId"));
            const FString PriorInput=BP->GetOutermost()->GetMetaData().GetValue(BP,*(TEXT("DasInputId.")+M.Name.ToString()));
            if ((!PriorId.IsEmpty() && PriorId!=M.StableId) || (!PriorInput.IsEmpty() && PriorInput!=M.InputStableId)) {Error=TEXT("Authored method/parameter stable ID changed");return false;}
        }
    }
    for(const auto& M:Methods) for(const auto& P:M.EffectiveParameters()){
        const FString Key=TEXT("DasParameterId.")+M.Name.ToString()+TEXT(".")+P.Name.ToString();
        const FString Prior=BP->GetOutermost()->GetMetaData().GetValue(BP,*Key);
        if(!Prior.IsEmpty() && Prior!=P.StableId){Error=TEXT("Authored parameter stable ID changed");return false;}
    }
    TMap<FName, FEdGraphPinType> Pins;
    TMap<FName, FString> Defaults;
    for (const auto& Field : Schema.Fields)
    {
        if (BP->ParentClass->FindPropertyByName(Field.Name) || BP->GeneratedClass->FindFunctionByName(Field.Name))
        { Error = TEXT("Field conflicts with an inherited property or function"); return false; }
        Pins.Add(Field.Name,ScalarPin(Field.Kind));
        Defaults.Add(Field.Name,Field.Kind==EDasScalarKind::Int32?FString::FromInt(Field.IntDefault):Field.Kind==EDasScalarKind::Float32?FString::Printf(TEXT("%.9g"),Field.FloatDefault):Field.Kind==EDasScalarKind::Bool?(Field.BoolDefault?TEXT("true"):TEXT("false")):Field.Kind==EDasScalarKind::Double?FString::Printf(TEXT("%.17g"),Field.DoubleDefault):Field.Kind==EDasScalarKind::String?Field.StringDefault:Field.NameDefault.ToString());
    }
    for (const auto& Old : BP->NewVariables)
    {
        const auto* Field = Schema.Fields.FindByPredicate([&](const FDasFieldSchema& F) { return F.Name == Old.VarName; });
        if (!Field || Pins[Old.VarName] != Old.VarType ||
            Field->bInstanceEditable == !!(Old.PropertyFlags & CPF_DisableEditOnInstance) || Field->bReadOnly != !!(Old.PropertyFlags & CPF_BlueprintReadOnly))
        { Error = TEXT("Removal, type, default or editability changes require a migration policy"); return false; }
        UObject* DefaultsObject = BP->GeneratedClass->GetDefaultObject();
        const auto* IntProperty = FindFProperty<FIntProperty>(BP->GeneratedClass, Old.VarName);
        const auto* FloatProperty = FindFProperty<FFloatProperty>(BP->GeneratedClass, Old.VarName);
        const auto* BoolProperty=FindFProperty<FBoolProperty>(BP->GeneratedClass,Old.VarName);
        const auto* DoubleProperty=FindFProperty<FDoubleProperty>(BP->GeneratedClass,Old.VarName);const auto* StringProperty=FindFProperty<FStrProperty>(BP->GeneratedClass,Old.VarName);const auto* NameProperty=FindFProperty<FNameProperty>(BP->GeneratedClass,Old.VarName);
        const bool bSameDefault=Field->Kind==EDasScalarKind::Int32?IntProperty && IntProperty->GetPropertyValue_InContainer(DefaultsObject)==Field->IntDefault:Field->Kind==EDasScalarKind::Float32?FloatProperty && FloatProperty->GetPropertyValue_InContainer(DefaultsObject)==Field->FloatDefault:Field->Kind==EDasScalarKind::Bool?BoolProperty && BoolProperty->GetPropertyValue_InContainer(DefaultsObject)==Field->BoolDefault:Field->Kind==EDasScalarKind::Double?DoubleProperty && DoubleProperty->GetPropertyValue_InContainer(DefaultsObject)==Field->DoubleDefault:Field->Kind==EDasScalarKind::String?StringProperty && StringProperty->GetPropertyValue_InContainer(DefaultsObject)==Field->StringDefault:NameProperty && NameProperty->GetPropertyValue_InContainer(DefaultsObject)==Field->NameDefault;

        if (!bSameDefault) { Error = TEXT("Changing a class default requires a migration policy"); return false; }
    }
    // Validate existing graph shape before adding any properties.
    for (UEdGraph* Graph : BP->FunctionGraphs)
        if (Methods.ContainsByPredicate([&](const auto& M){return M.Name==Graph->GetFName();}))
        {
            UFunction* Function = BP->GeneratedClass->FindFunctionByName(Graph->GetFName());
            bool bAdapter = false;
            for (UEdGraphNode* Item : Graph->Nodes)
                if (auto* Call = Cast<UK2Node_CallFunction>(Item))
                    bAdapter |= AdapterCall(Call) && Call->FindPin(TEXT("ScriptFile"));
            const auto* Decl=Methods.FindByPredicate([&](const auto& M){return M.Name==Graph->GetFName();});
            if (!bAdapter || !Decl || !MethodShape(Function,*Decl))
            { Error = TEXT("Existing method is not a supported daScript adapter graph"); return false; }
        }
    if(bValidateOnly) return true;
    BP->Modify();
    BP->BlueprintDisplayName=Schema.Metadata.FindRef(TEXT("DisplayName"));
    BP->BlueprintDescription=Schema.Metadata.FindRef(TEXT("ToolTip"));
    BP->BlueprintCategory=Schema.Metadata.FindRef(TEXT("Category"));
    for (const auto& Field : Schema.Fields)
        if (!BP->NewVariables.ContainsByPredicate([&](const FBPVariableDescription& V) { return V.VarName == Field.Name; }))
        {
            if (!FBlueprintEditorUtils::AddMemberVariable(BP, Field.Name, Pins[Field.Name], Defaults[Field.Name]))
            { Error = TEXT("Unable to create Blueprint field"); return false; }
            FBlueprintEditorUtils::SetBlueprintOnlyEditableFlag(BP, Field.Name, !Field.bInstanceEditable);
        }
    for (const auto& Field:Schema.Fields)
    {
        FBlueprintEditorUtils::SetBlueprintPropertyReadOnlyFlag(BP,Field.Name,Field.bReadOnly);
        for (const auto* Key:{TEXT("Category"),TEXT("DisplayName"),TEXT("ToolTip")}) FBlueprintEditorUtils::RemoveBlueprintVariableMetaData(BP,Field.Name,nullptr,Key);
        FBlueprintEditorUtils::SetBlueprintVariableCategory(BP,Field.Name,nullptr,FText::FromString(Field.Metadata.FindRef(TEXT("Category"))),true);
        for (const auto& Pair:Field.Metadata) FBlueprintEditorUtils::SetBlueprintVariableMetaData(BP,Field.Name,nullptr,*Pair.Key,Pair.Value);
    }
    for (const auto& M:Methods) if (!SetMethod(BP,M)) {Error=TEXT("Unable to create method graph");return false;}
    FKismetEditorUtilities::CompileBlueprint(BP);
    FBlueprintCompilationManager::FlushCompilationQueueAndReinstance();
    if (BP->Status == BS_Error || !BP->GeneratedClass)
    { Error = TEXT("Blueprint compilation failed after mutation; inspect compiler diagnostics"); return false; }
    if (!Schema.StableId.IsEmpty())
    {
        auto& Metadata = BP->GetOutermost()->GetMetaData();
        Metadata.SetValue(BP,TEXT("DasClassId"),*Schema.StableId);
        Metadata.SetValue(BP,TEXT("DasDeclaredName"),*Schema.Name.ToString());
        if (LegacyMethodName.IsEmpty()) Metadata.SetValue(BP,TEXT("DasMethodName"),*Methods[0].Name.ToString());
        if (LegacyMethodName.IsEmpty()) Metadata.SetValue(BP,TEXT("DasMethodId"),*Methods[0].StableId);
        if (LegacyMethodName.IsEmpty()) Metadata.SetValue(BP,TEXT("DasInputId"),*Methods[0].InputStableId);
    }
    for (const auto& Field : Schema.Fields)
        if (!Field.StableId.IsEmpty()) FBlueprintEditorUtils::SetBlueprintVariableMetaData(BP, Field.Name, nullptr, TEXT("DasMemberId"), Field.StableId);
    auto& Metadata=BP->GetOutermost()->GetMetaData();
    for (const auto* Key:{TEXT("Category"),TEXT("DisplayName"),TEXT("ToolTip")})
    {
        const FString Value=Schema.Metadata.FindRef(Key);Metadata.SetValue(BP,Key,*Value);BP->GeneratedClass->SetMetaData(Key,*Value);
    }
    for (const auto& M:Methods)
    {
        Metadata.SetValue(BP,*(TEXT("DasInputId.")+M.Name.ToString()),*M.InputStableId);
        for(const auto& P:M.EffectiveParameters()) Metadata.SetValue(BP,*(TEXT("DasParameterId.")+M.Name.ToString()+TEXT(".")+P.Name.ToString()),*P.StableId);
        if (UFunction* Function=BP->GeneratedClass->FindFunctionByName(M.Name))
        {
            if (!M.StableId.IsEmpty()) Function->SetMetaData(TEXT("DasMemberId"),*M.StableId);
            for (const auto& Pair:M.Metadata) Function->SetMetaData(*Pair.Key,*Pair.Value);
        }
        for (UEdGraph* Graph:BP->FunctionGraphs) if (Graph->GetFName()==M.Name && !M.StableId.IsEmpty()) Metadata.SetValue(Graph,TEXT("DasMemberId"),*M.StableId);
    }
    return true;
}
