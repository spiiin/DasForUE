#include "DasBlueprintService.h"
#include "DasTypePins.h"
#include "DasBlueprintDefaults.h"
#include "DasPropertyTypes.h"
#include "DasDefaultRoots.h"
#include "DasScriptHosts.h"
#include "DasSaveGame.h"
#include "DasBlueprintLibrary.h"
#include "DasAuthoring.h"
#include "DasContainerTypes.h"
#include "DasFrameLibrary.h"
#include "K2Node_AssignmentStatement.h"
#include "DasStructLibrary.h"
#include "DasEnumLibrary.h"
#include "DasInterfaceLibrary.h"
#include "DasDelegateLibrary.h"
#include "Misc/SecureHash.h"
#include "StructUtils/InstancedStruct.h"
#include "DasProbeActor.h"
#include "DasScriptComponent.h"
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
#include "K2Node_CallParentFunction.h"
#include "K2Node_MakeArray.h"
#include "K2Node_MakeStruct.h"
#include "K2Node_BreakStruct.h"
#include "K2Node_DynamicCast.h"
#include "K2Node_ClassDynamicCast.h"
#include "UObject/NoExportTypes.h"
#include "K2Node_FunctionEntry.h"
#include "K2Node_Event.h"
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


#include "DasEventDispatchers.h"
#include "DasSingleDelegateSignatures.h"
namespace
{
    template<class T> T* Node(UEdGraph* Graph)
    {
        FGraphNodeCreator<T> Creator(*Graph);
        T* Result = Creator.CreateNode();
        Creator.Finalize();
        return Result;
    }

    FEdGraphPinType ScalarPin(EDasScalarKind K,const FString& Path=FString(),UBlueprint* BP=nullptr){if(K==EDasScalarKind::Delegate)return DasMethodDelegatePin(BP,Path);return DasSchemaPinChecked(static_cast<EDasMethodSignature>(K),Path);}
    bool ScalarProperty(FProperty* P,EDasMethodSignature K){FDasDeclaredType Type;FString Error;if(!DasSchemaType(K,FString(),Type,Error) || DasPropertyKind(P)!=Type.GetKind())return false;if(K==EDasMethodSignature::Vector || K==EDasMethodSignature::Rotator || K==EDasMethodSignature::Transform){auto* S=CastField<FStructProperty>(P);return S && S->Struct->GetPathName()==Type.TypePath;}return true;}
    FName FrameField(EDasMethodSignature K){return K==EDasMethodSignature::Byte?TEXT("ByteValue"):K==EDasMethodSignature::Text?TEXT("TextValue"):K==EDasMethodSignature::Class?TEXT("ClassValue"):K==EDasMethodSignature::Int?TEXT("IntValue"):K==EDasMethodSignature::Float?TEXT("FloatValue"):K==EDasMethodSignature::Bool?TEXT("BoolValue"):K==EDasMethodSignature::String?TEXT("StringValue"):K==EDasMethodSignature::Double?TEXT("DoubleValue"):K==EDasMethodSignature::Name?TEXT("NameValue"):K==EDasMethodSignature::Object?TEXT("ObjectValue"):K==EDasMethodSignature::Vector?TEXT("VectorValue"):K==EDasMethodSignature::Rotator?TEXT("RotatorValue"):K==EDasMethodSignature::Transform?TEXT("TransformValue"):TEXT("StructValue");}
    bool AdapterCall(const UK2Node_CallFunction* Call)
    {const auto* T=Call->GetTargetFunction();if(!T)return false;for(const auto Name:{GET_FUNCTION_NAME_CHECKED(ADasProbeActor,ExecuteDasConstFrame),GET_FUNCTION_NAME_CHECKED(ADasProbeActor,ExecuteDasConstFrameOutputs),GET_FUNCTION_NAME_CHECKED(ADasProbeActor,ExecuteDas),GET_FUNCTION_NAME_CHECKED(ADasProbeActor,ExecuteDasNamed),GET_FUNCTION_NAME_CHECKED(ADasProbeActor,ExecuteDasFloat),GET_FUNCTION_NAME_CHECKED(ADasProbeActor,ExecuteDasBool),GET_FUNCTION_NAME_CHECKED(ADasProbeActor,ExecuteDasVoid),GET_FUNCTION_NAME_CHECKED(ADasProbeActor,ExecuteDasFrame),GET_FUNCTION_NAME_CHECKED(ADasProbeActor,ExecuteDasFrameOutputs)}) if(T==ADasProbeActor::StaticClass()->FindFunctionByName(Name) || T==UDasScriptComponent::StaticClass()->FindFunctionByName(Name) || T==DasNativeScriptHost(TEXT("Pawn"))->FindFunctionByName(Name) || T==DasNativeScriptHost(TEXT("Character"))->FindFunctionByName(Name) || T==UDasSaveGame::StaticClass()->FindFunctionByName(Name)) return true;return false;}
    bool MethodShape(UFunction* F,const FDasMethodSchema& M)
    {
        if(!F || F->HasAnyFunctionFlags(FUNC_BlueprintPure)!=M.bPure || F->HasAnyFunctionFlags(FUNC_Const)!=M.bConst || F->HasAnyFunctionFlags(FUNC_Static)!=M.bStatic) return false;const auto C=M.Contract();const auto Inputs=M.EffectiveParameters();
        auto Matches=[](FProperty* P,EDasMethodSignature K,const FString& Path=FString()){if(K==EDasMethodSignature::Class){auto* C=CastField<FClassProperty>(P);return C && C->MetaClass->GetPathName()==Path;}if(K==EDasMethodSignature::Delegate){FString Error;TStrongObjectPtr<UFunction> Sig(MakeDasSingleDelegateSignature(Path,Error));auto* D=CastField<FDelegateProperty>(P);return Sig && D && D->SignatureFunction && Sig->IsSignatureCompatibleWith(D->SignatureFunction);}if(K==EDasMethodSignature::Struct)if(auto* Native=DasContainerProperty(Path))return P && Native->SameType(P);return ScalarProperty(P,K);};
        const auto* World=M.bStatic?FindFProperty<FObjectProperty>(F,TEXT("__WorldContext")):nullptr;
        if(F->NumParms!=Inputs.Num()+(World?1:0)+(C.ReturnKind==EDasMethodSignature::Void?0:1)) return false;
        bool Directed=false;for(const auto& P:Inputs)Directed |= P.Direction!=TEXT("in");int32 Index=0;for(TFieldIterator<FProperty> It(F);!Directed && It;++It) if(It->HasAnyPropertyFlags(CPF_Parm) && !It->HasAnyPropertyFlags(CPF_ReturnParm) && *It!=World){if(!Inputs.IsValidIndex(Index) || It->GetFName()!=Inputs[Index].Name) return false;++Index;}if(!Directed && Index!=Inputs.Num()) return false;
        for(const auto& P:Inputs){auto* Actual=F->FindPropertyByName(P.Name);if(!Matches(Actual,P.Kind,P.ClassPath) || (P.Direction==TEXT("out")?(!Actual->HasAnyPropertyFlags(CPF_OutParm) || Actual->HasAnyPropertyFlags(CPF_ReferenceParm)):P.Direction==TEXT("inout")?!Actual->HasAllPropertyFlags(CPF_OutParm|CPF_ReferenceParm):(Actual->HasAnyPropertyFlags(CPF_OutParm|CPF_ReferenceParm) && !(DasContainerProperty(P.ClassPath) && Actual->IsA<FArrayProperty>())))) return false;if(P.Kind==EDasMethodSignature::Enum && DasEnumPropertyType(Actual)->GetPathName()!=P.ClassPath)return false;if(P.Kind==EDasMethodSignature::Struct && !DasContainerProperty(P.ClassPath) && CastField<FStructProperty>(Actual)->Struct->GetPathName()!=P.ClassPath)return false;if(P.Kind==EDasMethodSignature::Object && DasReferencePropertyClass(Actual)->GetPathName()!=P.ClassPath)return false;}
        if(C.ReturnKind==EDasMethodSignature::Class)return Matches(F->GetReturnProperty(),C.ReturnKind,M.ReturnClass);if(C.ReturnKind==EDasMethodSignature::Delegate)return Matches(F->GetReturnProperty(),C.ReturnKind,M.ReturnClass);if(C.ReturnKind==EDasMethodSignature::Enum)return Matches(F->GetReturnProperty(),C.ReturnKind) && DasEnumPropertyType(F->GetReturnProperty())->GetPathName()==M.ReturnClass;if(C.ReturnKind==EDasMethodSignature::Struct){if(DasContainerProperty(M.ReturnClass))return Matches(F->GetReturnProperty(),C.ReturnKind,M.ReturnClass);return Matches(F->GetReturnProperty(),C.ReturnKind) && CastField<FStructProperty>(F->GetReturnProperty())->Struct->GetPathName()==M.ReturnClass;}if(C.ReturnKind==EDasMethodSignature::Object)return Matches(F->GetReturnProperty(),C.ReturnKind) && DasReferencePropertyClass(F->GetReturnProperty())->GetPathName()==M.ReturnClass;return C.ReturnKind==EDasMethodSignature::Void?!F->GetReturnProperty():Matches(F->GetReturnProperty(),C.ReturnKind);

    }
    void MethodMetadata(UEdGraph* Graph,const FDasMethodSchema& Method)
    {
        TArray<UK2Node_FunctionEntry*> Entries;Graph->GetNodesOfClass(Entries);
        for (auto* Entry:Entries)
        {
            for (const auto* Key:{TEXT("Category"),TEXT("DisplayName"),TEXT("ToolTip")}) Entry->MetaData.RemoveMetaData(Key);
            for (const auto& Pair:Method.Metadata) Entry->MetaData.SetMetaData(*Pair.Key,FString(Pair.Value));
            Entry->MetaData.SetMetaData(TEXT("DasFunctionPure"),FString(Method.bPure?TEXT("true"):TEXT("false")));
            Entry->MetaData.SetMetaData(TEXT("DasFunctionConst"),FString(Method.bConst?TEXT("true"):TEXT("false")));
            Entry->MetaData.SetMetaData(TEXT("DasFunctionStatic"),FString(Method.bStatic?TEXT("true"):TEXT("false")));
            for(const auto& Parameter:Method.Parameters)for(const auto& Pin:Entry->UserDefinedPins)if(Pin->PinName==Parameter.Name){Pin->PinDefaultValue=Parameter.bHasDefault?Parameter.DefaultValue:FString();if(auto* Actual=Entry->FindPin(Parameter.Name)){Actual->DefaultValue=Parameter.bHasDefault?Parameter.DefaultValue:Parameter.Kind==EDasMethodSignature::Bool?TEXT("false"):(Parameter.Kind==EDasMethodSignature::Int || Parameter.Kind==EDasMethodSignature::Float || Parameter.Kind==EDasMethodSignature::Double)?TEXT("0"):TEXT("");Actual->AutogeneratedDefaultValue=Actual->DefaultValue;if(Parameter.Kind==EDasMethodSignature::Text){FText Value;FString Issue;if(Parameter.bHasDefault && !ParseDasTextDefault(Parameter.DefaultValue,Value,Issue))return;Actual->DefaultValue.Reset();Actual->DefaultTextValue=Value;}}}
            Entry->ClearExtraFlags(FUNC_BlueprintPure|FUNC_Const|FUNC_Static);
            if(Method.bStatic){Entry->AddExtraFlags(FUNC_Static);if(Method.WorldContext.IsEmpty()){Entry->MetaData.RemoveMetaData(TEXT("WorldContext"));Entry->MetaData.SetMetaData(TEXT("CallableWithoutWorldContext"),FString(TEXT("true")));}else{Entry->MetaData.SetMetaData(TEXT("WorldContext"),Method.WorldContext);Entry->MetaData.RemoveMetaData(TEXT("CallableWithoutWorldContext"));}Entry->MetaData.SetMetaData(TEXT("DasWorldContext"),Method.WorldContext);}
            if(Method.bConst)Entry->AddExtraFlags(FUNC_Const);
            if(Method.bPure)Entry->AddExtraFlags(FUNC_BlueprintPure|FUNC_BlueprintCallable);
        }
    }
    template<class T> T* StructNode(UEdGraph* G){auto* N=NewObject<T>(G);N->StructType=FDasScalarValue::StaticStruct();G->AddNode(N,false,false);N->CreateNewGuid();N->PostPlacedNewNode();N->AllocateDefaultPins();return N;}
    UK2Node_CallFunction* StructCall(UEdGraph* G,FName Function){auto* N=NewObject<UK2Node_CallFunction>(G);N->SetFromFunction(UDasStructLibrary::StaticClass()->FindFunctionByName(Function));G->AddNode(N,false,false);N->CreateNewGuid();N->PostPlacedNewNode();N->AllocateDefaultPins();return N;}
    UK2Node_CallFunction* EnumCall(UEdGraph* G,FName Function){auto* N=NewObject<UK2Node_CallFunction>(G);N->SetFromFunction(UDasEnumLibrary::StaticClass()->FindFunctionByName(Function));G->AddNode(N,false,false);N->CreateNewGuid();N->PostPlacedNewNode();N->AllocateDefaultPins();return N;}
    bool InterfaceReference(const FString& Path){auto* C=FindObject<UClass>(nullptr,*Path);return C && C->HasAnyClassFlags(CLASS_Interface);}
    UK2Node_CallFunction* InterfaceReferenceCall(UEdGraph* G,FName Function){auto* N=NewObject<UK2Node_CallFunction>(G);N->SetFromFunction(UDasInterfaceLibrary::StaticClass()->FindFunctionByName(Function));G->AddNode(N,false,false);N->CreateNewGuid();N->PostPlacedNewNode();N->AllocateDefaultPins();return N;}
    UK2Node_CallFunction* FrameCall(UEdGraph* G,FName Function){auto* N=NewObject<UK2Node_CallFunction>(G);N->SetFromFunction(UDasFrameLibrary::StaticClass()->FindFunctionByName(Function));G->AddNode(N,false,false);N->CreateNewGuid();N->PostPlacedNewNode();N->AllocateDefaultPins();return N;}
    TArray<UEdGraph*> DasOwnedMethodGraphs(UBlueprint* BP){TArray<UEdGraph*> Graphs;for(UEdGraph* G:BP->FunctionGraphs)Graphs.Add(G);for(const auto& I:BP->ImplementedInterfaces)for(UEdGraph* G:I.Graphs)Graphs.AddUnique(G);return Graphs;}
    UClass* DasMethodInterface(UBlueprint* BP,FName Name){for(const auto& I:BP->ImplementedInterfaces)if(I.Interface && I.Interface->FindFunctionByName(Name))return I.Interface;return nullptr;}
    bool BuildFrameInputs(UEdGraph* G,UK2Node* Source,const FDasMethodSchema& M,UK2Node_CallFunction* Call){const auto* Schema=G->GetSchema();bool Linked=true;
        auto* Array=NewObject<UK2Node_MakeArray>(G);Array->NumInputs=M.Parameters.Num();G->AddNode(Array,false,false);Array->CreateNewGuid();Array->PostPlacedNewNode();Array->AllocateDefaultPins();
        Linked &= Schema->TryCreateConnection(Array->GetOutputPin(),Call->FindPinChecked(TEXT("Arguments")));
        for(int32 I=0;I<M.Parameters.Num();++I){const auto& P=M.Parameters[I];if(P.Direction==TEXT("out")){auto* Default=FrameCall(G,GET_FUNCTION_NAME_CHECKED(UDasFrameLibrary,DefaultValue));Schema->TrySetDefaultValue(*Default->FindPinChecked(TEXT("Kind")),FString::FromInt(static_cast<int32>(P.Kind)));Schema->TrySetDefaultValue(*Default->FindPinChecked(TEXT("ClassPath")),P.ClassPath);Schema->TrySetDefaultValue(*Default->FindPinChecked(TEXT("Direction")),TEXT("1"));Linked &= Schema->TryCreateConnection(Default->GetReturnValuePin(),Array->FindPinChecked(Array->GetPinName(I)));continue;}if(P.Kind==EDasMethodSignature::Object && InterfaceReference(P.ClassPath)){auto* Pack=InterfaceReferenceCall(G,GET_FUNCTION_NAME_CHECKED(UDasInterfaceLibrary,PackValue));Schema->TrySetDefaultValue(*Pack->FindPinChecked(TEXT("Direction")),P.Direction==TEXT("inout")?TEXT("2"):TEXT("0"));Pack->FindPinChecked(TEXT("Value"))->PinType=ScalarPin(EDasScalarKind::Object,P.ClassPath);Linked &= Schema->TryCreateConnection(Source->FindPinChecked(P.Name),Pack->FindPinChecked(TEXT("Value")));Linked &= Schema->TryCreateConnection(Pack->GetReturnValuePin(),Array->FindPinChecked(Array->GetPinName(I)));continue;}if(P.Kind==EDasMethodSignature::Delegate){auto* Pack=NewObject<UK2Node_CallFunction>(G);Pack->SetFromFunction(UDasDelegateLibrary::StaticClass()->FindFunctionByName(GET_FUNCTION_NAME_CHECKED(UDasDelegateLibrary,PackValue)));G->AddNode(Pack,false,false);Pack->CreateNewGuid();Pack->PostPlacedNewNode();Pack->AllocateDefaultPins();Pack->FindPinChecked(TEXT("Value"))->PinType=DasMethodDelegatePin(CastChecked<UBlueprint>(G->GetOuter()),P.ClassPath);Schema->TrySetDefaultValue(*Pack->FindPinChecked(TEXT("SignatureWire")),P.ClassPath);Linked &= Schema->TryCreateConnection(Source->FindPinChecked(P.Name),Pack->FindPinChecked(TEXT("Value")));Linked &= Schema->TryCreateConnection(Pack->GetReturnValuePin(),Array->FindPinChecked(Array->GetPinName(I)));continue;}if(P.Kind==EDasMethodSignature::Enum){auto* Pack=EnumCall(G,GET_FUNCTION_NAME_CHECKED(UDasEnumLibrary,PackValue));Schema->TrySetDefaultValue(*Pack->FindPinChecked(TEXT("Direction")),P.Direction==TEXT("inout")?TEXT("2"):TEXT("0"));Linked &= Schema->TryCreateConnection(Source->FindPinChecked(P.Name),Pack->FindPinChecked(TEXT("Value")));Linked &= Schema->TryCreateConnection(Pack->GetReturnValuePin(),Array->FindPinChecked(Array->GetPinName(I)));continue;}auto* Make=StructNode<UK2Node_MakeStruct>(G);Schema->TrySetDefaultValue(*Make->FindPinChecked(TEXT("Direction")),P.Direction==TEXT("inout")?TEXT("2"):TEXT("0"));Schema->TrySetDefaultValue(*Make->FindPinChecked(TEXT("Kind")),FString::FromInt(static_cast<int32>(P.Kind)));if(P.Kind==EDasMethodSignature::Class || P.Kind==EDasMethodSignature::Object || P.Kind==EDasMethodSignature::Struct)Schema->TrySetDefaultValue(*Make->FindPinChecked(TEXT("ObjectClass")),P.ClassPath);const FName Field=FrameField(P.Kind);if(P.Kind==EDasMethodSignature::Struct){auto* Pack=StructCall(G,GET_FUNCTION_NAME_CHECKED(UDasStructLibrary,PackValue));if(DasContainerProperty(P.ClassPath))Pack->FindPinChecked(TEXT("Value"))->PinType=ScalarPin(static_cast<EDasScalarKind>(P.Kind),P.ClassPath,CastChecked<UBlueprint>(G->GetOuter()));Linked &= Schema->TryCreateConnection(Source->FindPinChecked(P.Name),Pack->FindPinChecked(TEXT("Value")));Linked &= Schema->TryCreateConnection(Pack->GetReturnValuePin(),Make->FindPinChecked(Field));}else Linked &= Schema->TryCreateConnection(Source->FindPinChecked(P.Name),Make->FindPinChecked(Field));Linked &= Schema->TryCreateConnection(Make->FindPinChecked(FDasScalarValue::StaticStruct()->GetFName()),Array->FindPinChecked(Array->GetPinName(I)));}
        return Linked;
    }
    UK2Node_Event* DasFindInterfaceEvent(UBlueprint* BP,FName Name){for(UEdGraph* G:BP->UbergraphPages)for(UEdGraphNode* N:G->Nodes)if(auto* E=Cast<UK2Node_Event>(N);E && E->bOverrideFunction && E->EventReference.GetMemberName()==Name)return E;return nullptr;}
    UK2Node_CallFunction* DasInterfaceEventAdapter(UK2Node_Event* E){auto* Then=E?E->FindPin(UEdGraphSchema_K2::PN_Then):nullptr;if(!Then || Then->LinkedTo.Num()!=1)return nullptr;auto* Call=Cast<UK2Node_CallFunction>(Then->LinkedTo[0]->GetOwningNode());return Call && AdapterCall(Call) && Call->FindPin(TEXT("ScriptFile"))?Call:nullptr;}
    bool DasHasMethod(UBlueprint* BP,FName Name){return DasOwnedMethodGraphs(BP).ContainsByPredicate([&](const auto* G){return G->GetFName()==Name;}) || DasFindInterfaceEvent(BP,Name)!=nullptr;}
    bool DasIsInterfaceEvent(UBlueprint* BP,const FDasMethodSchema& M){if(M.bConst || !DasMethodInterface(BP,M.Name) || M.Contract().ReturnKind!=EDasMethodSignature::Void)return false;for(const auto& P:M.EffectiveParameters())if(P.Direction!=TEXT("in"))return false;return true;}
    bool SetInterfaceEvent(UBlueprint* BP,const FDasMethodSchema& M){
        auto& Metadata=BP->GetOutermost()->GetMetaData();auto* Event=DasFindInterfaceEvent(BP,M.Name);
        if(Event){auto* Call=DasInterfaceEventAdapter(Event);if(!Call || Metadata.GetValue(Event,TEXT("DasMemberId"))!=M.StableId)return false;const auto* Schema=Event->GetGraph()->GetSchema();Schema->TrySetDefaultValue(*Call->FindPinChecked(TEXT("ScriptFile")),M.ScriptFile);Schema->TrySetDefaultValue(*Call->FindPinChecked(TEXT("EntryPoint")),M.EntryPoint);return true;}
        UEdGraph* Graph=FBlueprintEditorUtils::FindEventGraph(BP);if(!Graph){Graph=FBlueprintEditorUtils::CreateNewGraph(BP,TEXT("EventGraph"),UEdGraph::StaticClass(),UEdGraphSchema_K2::StaticClass());FBlueprintEditorUtils::AddUbergraphPage(BP,Graph);}
        Event=NewObject<UK2Node_Event>(Graph);Event->EventReference.SetExternalMember(M.Name,DasMethodInterface(BP,M.Name));Event->bOverrideFunction=true;Graph->AddNode(Event,false,false);Event->CreateNewGuid();Event->PostPlacedNewNode();Event->AllocateDefaultPins();
        auto* Call=NewObject<UK2Node_CallFunction>(Graph);Call->SetFromFunction(BP->ParentClass->FindFunctionByName(GET_FUNCTION_NAME_CHECKED(ADasProbeActor,ExecuteDasFrame)));Graph->AddNode(Call,false,false);Call->CreateNewGuid();Call->PostPlacedNewNode();Call->AllocateDefaultPins();const auto* Schema=Graph->GetSchema();Schema->TrySetDefaultValue(*Call->FindPinChecked(TEXT("ScriptFile")),M.ScriptFile);Schema->TrySetDefaultValue(*Call->FindPinChecked(TEXT("EntryPoint")),M.EntryPoint);Schema->TrySetDefaultValue(*Call->FindPinChecked(TEXT("ResultKind")),FString::FromInt(static_cast<int32>(EDasMethodSignature::Void)));Schema->TrySetDefaultValue(*Call->FindPinChecked(TEXT("ResultClass")),TEXT(""));
        const bool Linked=Schema->TryCreateConnection(Event->FindPinChecked(UEdGraphSchema_K2::PN_Then),Call->GetExecPin()) && BuildFrameInputs(Graph,Event,M,Call);Metadata.SetValue(Event,TEXT("DasMemberId"),*M.StableId);FBlueprintEditorUtils::MarkBlueprintAsStructurallyModified(BP);return Linked;
    }
    void DasCanonicalOverridePins(UK2Node_EditablePinBase* Node,UFunction* Parent,bool Entry){if(!Parent)return;TArray<UK2Node_CallParentFunction*> Calls;Node->GetGraph()->GetNodesOfClass(Calls);for(auto* Call:Calls){Call->BreakAllNodeLinks();Node->GetGraph()->RemoveNode(Call);}TArray<FName> Names;for(TFieldIterator<FProperty> It(Parent);It;++It)if(It->HasAnyPropertyFlags(CPF_Parm)){const bool Return=It->HasAnyPropertyFlags(CPF_ReturnParm);const bool OutOnly=It->HasAnyPropertyFlags(CPF_OutParm) && !It->HasAnyPropertyFlags(CPF_ReferenceParm|CPF_ConstParm);if(Entry?(!Return && !OutOnly):(Return || OutOnly))Names.Add(It->GetFName());}int32 I=0;for(auto* Pin:Node->Pins)if(Pin->PinType.PinCategory!=UEdGraphSchema_K2::PC_Exec && Names.IsValidIndex(I)){const FName Old=Pin->PinName,Name=Names[I++];Pin->PinName=Name;for(auto& User:Node->UserDefinedPins)if(User->PinName==Old)User->PinName=Name;}}
    bool SetFrameMethod(UBlueprint* BP,const FDasMethodSchema& M)
    {
        if(DasIsInterfaceEvent(BP,M))return SetInterfaceEvent(BP,M);
        bool Directed=false;for(const auto& P:M.Parameters)Directed |= P.Direction!=TEXT("in");const FName Target=M.bStatic?(Directed?GET_FUNCTION_NAME_CHECKED(UDasBlueprintLibrary,ExecuteLibraryFrameOutputs):GET_FUNCTION_NAME_CHECKED(UDasBlueprintLibrary,ExecuteLibraryFrame)):M.bConst?(Directed?GET_FUNCTION_NAME_CHECKED(ADasProbeActor,ExecuteDasConstFrameOutputs):GET_FUNCTION_NAME_CHECKED(ADasProbeActor,ExecuteDasConstFrame)):(Directed?GET_FUNCTION_NAME_CHECKED(ADasProbeActor,ExecuteDasFrameOutputs):GET_FUNCTION_NAME_CHECKED(ADasProbeActor,ExecuteDasFrame));UEdGraph* G=nullptr;for(UEdGraph* Existing:DasOwnedMethodGraphs(BP)) if(Existing->GetFName()==M.Name) G=Existing;
        if(G){for(UEdGraphNode* Item:G->Nodes) if(auto* Call=Cast<UK2Node_CallFunction>(Item)) if(Call->GetTargetFunction()==BP->ParentClass->FindFunctionByName(Target)){if(!Call->FindPin(TEXT("ResultClass")))Call->ReconstructNode();if(M.bStatic)G->GetSchema()->TrySetDefaultValue(*Call->FindPinChecked(TEXT("LibraryContract")),M.LibraryContract);G->GetSchema()->TrySetDefaultValue(*Call->FindPinChecked(TEXT("ResultClass")),M.ReturnClass);G->GetSchema()->TrySetDefaultValue(*Call->FindPinChecked(TEXT("ScriptFile")),M.ScriptFile);G->GetSchema()->TrySetDefaultValue(*Call->FindPinChecked(TEXT("EntryPoint")),M.EntryPoint);MethodMetadata(G,M);return true;}return false;}
        G=FBlueprintEditorUtils::CreateNewGraph(BP,M.Name,UEdGraph::StaticClass(),UEdGraphSchema_K2::StaticClass());auto* Interface=DasMethodInterface(BP,M.Name);if(Interface){FBlueprintEditorUtils::AddInterfaceGraph(BP,G,Interface);G->bAllowDeletion=false;for(auto& I:BP->ImplementedInterfaces)if(I.Interface==Interface)I.Graphs.Add(G);}else if(auto* Parent=BP->ParentClass->FindFunctionByName(M.Name))FBlueprintEditorUtils::AddFunctionGraph<UClass>(BP,G,false,Parent->GetOwnerClass());else FBlueprintEditorUtils::AddFunctionGraph<UClass>(BP,G,true,nullptr);
        TArray<UK2Node_FunctionEntry*> Entries;G->GetNodesOfClass(Entries);if(Entries.Num()!=1) return false;auto* Entry=Entries[0];DasCanonicalOverridePins(Entry,BP->ParentClass->FindFunctionByName(M.Name),true);
        for(const auto& P:M.Parameters)if(P.Direction!=TEXT("out")){auto Pin=ScalarPin(static_cast<EDasScalarKind>(P.Kind),P.ClassPath,CastChecked<UBlueprint>(G->GetOuter()));Pin.bIsReference=P.Direction==TEXT("inout");if(!Entry->FindPin(P.Name))Entry->CreateUserDefinedPin(P.Name,Pin,EGPD_Output);}
        TArray<UK2Node_FunctionResult*> ExistingResults;G->GetNodesOfClass(ExistingResults);auto* Result=ExistingResults.Num()?ExistingResults[0]:Node<UK2Node_FunctionResult>(G);DasCanonicalOverridePins(Result,BP->ParentClass->FindFunctionByName(M.Name),false);Entry->FindPinChecked(UEdGraphSchema_K2::PN_Then)->BreakAllPinLinks();
        for(const auto& P:M.Parameters)if(P.Direction==TEXT("out") && !Result->FindPin(P.Name))Result->CreateUserDefinedPin(P.Name,ScalarPin(static_cast<EDasScalarKind>(P.Kind),P.ClassPath,CastChecked<UBlueprint>(G->GetOuter())),EGPD_Input);
        // Match declaration order: inputs/ref, out-only parameters, then return.
        if(M.ReturnKind!=EDasMethodSignature::Void && !Result->FindPin(UEdGraphSchema_K2::PN_ReturnValue)) Result->CreateUserDefinedPin(UEdGraphSchema_K2::PN_ReturnValue,ScalarPin(static_cast<EDasScalarKind>(M.ReturnKind),M.ReturnClass,BP),EGPD_Input);
        auto* Call=NewObject<UK2Node_CallFunction>(G);Call->SetFromFunction(BP->ParentClass->FindFunctionByName(Target));G->AddNode(Call,false,false);Call->CreateNewGuid();Call->PostPlacedNewNode();Call->AllocateDefaultPins();
        const auto* Schema=G->GetSchema();if(M.bStatic)Schema->TrySetDefaultValue(*Call->FindPinChecked(TEXT("LibraryContract")),M.LibraryContract);Schema->TrySetDefaultValue(*Call->FindPinChecked(TEXT("ResultClass")),M.ReturnClass);Schema->TrySetDefaultValue(*Call->FindPinChecked(TEXT("ScriptFile")),M.ScriptFile);Schema->TrySetDefaultValue(*Call->FindPinChecked(TEXT("EntryPoint")),M.EntryPoint);Schema->TrySetDefaultValue(*Call->FindPinChecked(TEXT("ResultKind")),FString::FromInt(static_cast<int32>(M.ReturnKind)));
        bool Linked=Schema->TryCreateConnection(Entry->FindPinChecked(UEdGraphSchema_K2::PN_Then),Call->GetExecPin());UEdGraphPin* Tail=Call->GetThenPin();
        Linked &= BuildFrameInputs(G,Entry,M,Call);
        if(M.ReturnKind!=EDasMethodSignature::Void){auto* Break=StructNode<UK2Node_BreakStruct>(G);Linked &= Schema->TryCreateConnection(Call->GetReturnValuePin(),Break->FindPinChecked(FDasScalarValue::StaticStruct()->GetFName()));const FName Field=FrameField(M.ReturnKind);UEdGraphPin* Value=Break->FindPinChecked(Field);if(M.ReturnKind==EDasMethodSignature::Delegate){auto* Unpack=NewObject<UK2Node_CallFunction>(G);Unpack->SetFromFunction(UDasDelegateLibrary::StaticClass()->FindFunctionByName(GET_FUNCTION_NAME_CHECKED(UDasDelegateLibrary,UnpackValue)));G->AddNode(Unpack,false,false);Unpack->CreateNewGuid();Unpack->PostPlacedNewNode();Unpack->AllocateDefaultPins();Unpack->FindPinChecked(TEXT("Value"))->PinType=DasMethodDelegatePin(BP,M.ReturnClass);Linked &= Schema->TryCreateConnection(Call->GetReturnValuePin(),Unpack->FindPinChecked(TEXT("Data")));Linked &= Schema->TryCreateConnection(Unpack->FindPinChecked(TEXT("Value")),Result->FindPinChecked(UEdGraphSchema_K2::PN_ReturnValue));Value=nullptr;}if(M.ReturnKind==EDasMethodSignature::Enum){auto* Unpack=EnumCall(G,GET_FUNCTION_NAME_CHECKED(UDasEnumLibrary,UnpackValue));Linked &= Schema->TryCreateConnection(Call->GetReturnValuePin(),Unpack->FindPinChecked(TEXT("Data")));Linked &= Schema->TryCreateConnection(Unpack->FindPinChecked(TEXT("Value")),Result->FindPinChecked(UEdGraphSchema_K2::PN_ReturnValue));Value=nullptr;}if(M.ReturnKind==EDasMethodSignature::Struct){auto* Unpack=StructCall(G,GET_FUNCTION_NAME_CHECKED(UDasStructLibrary,UnpackValue));if(DasContainerProperty(M.ReturnClass))Unpack->FindPinChecked(TEXT("Value"))->PinType=ScalarPin(static_cast<EDasScalarKind>(M.ReturnKind),M.ReturnClass,BP);Linked &= Schema->TryCreateConnection(Value,Unpack->FindPinChecked(TEXT("Data")));Linked &= Schema->TryCreateConnection(Unpack->FindPinChecked(TEXT("Value")),Result->FindPinChecked(UEdGraphSchema_K2::PN_ReturnValue));Value=nullptr;}if(M.ReturnKind==EDasMethodSignature::Object && InterfaceReference(M.ReturnClass)){auto* Unpack=InterfaceReferenceCall(G,GET_FUNCTION_NAME_CHECKED(UDasInterfaceLibrary,UnpackValue));Unpack->FindPinChecked(TEXT("Value"))->PinType=ScalarPin(EDasScalarKind::Object,M.ReturnClass);Linked &= Schema->TryCreateConnection(Call->GetReturnValuePin(),Unpack->FindPinChecked(TEXT("Data")));Linked &= Schema->TryCreateConnection(Unpack->FindPinChecked(TEXT("Value")),Result->FindPinChecked(UEdGraphSchema_K2::PN_ReturnValue));Value=nullptr;}else if(M.ReturnKind==EDasMethodSignature::Object && M.ReturnClass!=TEXT("/Script/CoreUObject.Object")){auto* Cast=NewObject<UK2Node_DynamicCast>(G);Cast->TargetType=FindObject<UClass>(nullptr,*M.ReturnClass);Cast->SetPurity(true);G->AddNode(Cast,false,false);Cast->CreateNewGuid();Cast->PostPlacedNewNode();Cast->AllocateDefaultPins();Linked &= Schema->TryCreateConnection(Value,Cast->GetCastSourcePin());Value=Cast->GetCastResultPin();}if(M.ReturnKind==EDasMethodSignature::Class && M.ReturnClass!=TEXT("/Script/CoreUObject.Object")){auto* Cast=NewObject<UK2Node_ClassDynamicCast>(G);Cast->TargetType=FindObject<UClass>(nullptr,*M.ReturnClass);Cast->SetPurity(true);G->AddNode(Cast,false,false);Cast->CreateNewGuid();Cast->PostPlacedNewNode();Cast->AllocateDefaultPins();Linked &= Schema->TryCreateConnection(Value,Cast->GetCastSourcePin());Value=Cast->GetCastResultPin();}if(Value)Linked &= Schema->TryCreateConnection(Value,Result->FindPinChecked(UEdGraphSchema_K2::PN_ReturnValue));}
        if(Directed)for(int32 I=0;I<M.Parameters.Num();++I){const auto& P=M.Parameters[I];if(P.Direction==TEXT("in"))continue;
            auto* At=FrameCall(G,GET_FUNCTION_NAME_CHECKED(UDasFrameLibrary,OutputAt));Schema->TrySetDefaultValue(*At->FindPinChecked(TEXT("Index")),FString::FromInt(I));Linked &= Schema->TryCreateConnection(Call->FindPinChecked(TEXT("Outputs")),At->FindPinChecked(TEXT("Values")));
            auto* Break=StructNode<UK2Node_BreakStruct>(G);Linked &= Schema->TryCreateConnection(At->GetReturnValuePin(),Break->FindPinChecked(FDasScalarValue::StaticStruct()->GetFName()));UEdGraphPin* Value=Break->FindPinChecked(FrameField(P.Kind));
            UK2Node_AssignmentStatement* Assign=nullptr;UEdGraphPin* Destination=nullptr;
            if(P.Direction==TEXT("inout")){Assign=Node<UK2Node_AssignmentStatement>(G);Linked &= Schema->TryCreateConnection(Entry->FindPinChecked(P.Name),Assign->GetVariablePin());Destination=Assign->GetValuePin();Linked &= Schema->TryCreateConnection(Tail,Assign->FindPinChecked(UEdGraphSchema_K2::PN_Execute));Tail=Assign->FindPinChecked(UEdGraphSchema_K2::PN_Then);}else Destination=Result->FindPinChecked(P.Name);
            if(P.Kind==EDasMethodSignature::Enum){auto* Unpack=EnumCall(G,GET_FUNCTION_NAME_CHECKED(UDasEnumLibrary,UnpackValue));Unpack->FindPinChecked(TEXT("Value"))->PinType=ScalarPin(EDasScalarKind::Enum,P.ClassPath);Linked &= Schema->TryCreateConnection(At->GetReturnValuePin(),Unpack->FindPinChecked(TEXT("Data")));Value=Unpack->FindPinChecked(TEXT("Value"));}
            if(P.Kind==EDasMethodSignature::Object && InterfaceReference(P.ClassPath)){auto* Unpack=InterfaceReferenceCall(G,GET_FUNCTION_NAME_CHECKED(UDasInterfaceLibrary,UnpackValue));Unpack->FindPinChecked(TEXT("Value"))->PinType=ScalarPin(EDasScalarKind::Object,P.ClassPath);Linked &= Schema->TryCreateConnection(At->GetReturnValuePin(),Unpack->FindPinChecked(TEXT("Data")));Value=Unpack->FindPinChecked(TEXT("Value"));}
            else if(P.Kind==EDasMethodSignature::Object && P.ClassPath!=TEXT("/Script/CoreUObject.Object")){auto* Cast=NewObject<UK2Node_DynamicCast>(G);Cast->TargetType=FindObject<UClass>(nullptr,*P.ClassPath);Cast->SetPurity(true);G->AddNode(Cast,false,false);Cast->CreateNewGuid();Cast->PostPlacedNewNode();Cast->AllocateDefaultPins();Linked &= Schema->TryCreateConnection(Value,Cast->GetCastSourcePin());Value=Cast->GetCastResultPin();}
            if(P.Kind==EDasMethodSignature::Struct){auto* Unpack=StructCall(G,GET_FUNCTION_NAME_CHECKED(UDasStructLibrary,UnpackValue));Unpack->FindPinChecked(TEXT("Value"))->PinType=ScalarPin(static_cast<EDasScalarKind>(P.Kind),P.ClassPath,CastChecked<UBlueprint>(G->GetOuter()));Linked &= Schema->TryCreateConnection(Value,Unpack->FindPinChecked(TEXT("Data")));Value=Unpack->FindPinChecked(TEXT("Value"));}
            if(P.Kind==EDasMethodSignature::Class && P.ClassPath!=TEXT("/Script/CoreUObject.Object")){auto* Cast=NewObject<UK2Node_ClassDynamicCast>(G);Cast->TargetType=FindObject<UClass>(nullptr,*P.ClassPath);Cast->SetPurity(true);G->AddNode(Cast,false,false);Cast->CreateNewGuid();Cast->PostPlacedNewNode();Cast->AllocateDefaultPins();Linked &= Schema->TryCreateConnection(Value,Cast->GetCastSourcePin());Value=Cast->GetCastResultPin();}
            Linked &= Schema->TryCreateConnection(Value,Destination);
        }
        Linked &= Schema->TryCreateConnection(Tail,Result->FindPinChecked(UEdGraphSchema_K2::PN_Execute));
        MethodMetadata(G,M);FBlueprintEditorUtils::MarkBlueprintAsStructurallyModified(BP);return Linked;
    }
    bool SetMethod(UBlueprint* BP, const FDasMethodSchema& Method)
    {
        if(Method.Signature==EDasMethodSignature::Frame) return SetFrameMethod(BP,Method);
        const bool Action=Method.Signature==EDasMethodSignature::Void;
        const FName TargetName=Action?GET_FUNCTION_NAME_CHECKED(ADasProbeActor,ExecuteDasVoid):Method.Signature==EDasMethodSignature::Float?GET_FUNCTION_NAME_CHECKED(ADasProbeActor,ExecuteDasFloat):Method.Signature==EDasMethodSignature::Bool?GET_FUNCTION_NAME_CHECKED(ADasProbeActor,ExecuteDasBool):Method.EntryPoint==TEXT("main")?GET_FUNCTION_NAME_CHECKED(ADasProbeActor,ExecuteDas):GET_FUNCTION_NAME_CHECKED(ADasProbeActor,ExecuteDasNamed);
        UFunction* Target=BP->ParentClass->FindFunctionByName(TargetName);
        UEdGraph* Graph = nullptr;
        for (UEdGraph* Existing : DasOwnedMethodGraphs(BP))
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
        if(auto* Parent=BP->ParentClass->FindFunctionByName(Method.Name))FBlueprintEditorUtils::AddFunctionGraph<UClass>(BP,Graph,false,Parent->GetOwnerClass());else FBlueprintEditorUtils::AddFunctionGraph<UClass>(BP, Graph, true, nullptr);
        TArray<UK2Node_FunctionEntry*> Entries;
        Graph->GetNodesOfClass(Entries);
        if (Entries.Num() != 1) return false;
        auto* Entry = Entries[0];DasCanonicalOverridePins(Entry,BP->ParentClass->FindFunctionByName(Method.Name),true);
        const auto ValueType=ScalarPin(Method.Signature==EDasMethodSignature::Float?EDasScalarKind::Float32:Method.Signature==EDasMethodSignature::Bool?EDasScalarKind::Bool:EDasScalarKind::Int32);
        if(!Action && !Entry->FindPin(TEXT("Input"))) Entry->CreateUserDefinedPin(TEXT("Input"),ValueType,EGPD_Output);
        TArray<UK2Node_FunctionResult*> Results;
        Graph->GetNodesOfClass(Results);
        auto* Result = Results.Num() ? Results[0] : Node<UK2Node_FunctionResult>(Graph);DasCanonicalOverridePins(Result,BP->ParentClass->FindFunctionByName(Method.Name),false);
        if(!Action && !Result->FindPin(UEdGraphSchema_K2::PN_ReturnValue)) Result->CreateUserDefinedPin(UEdGraphSchema_K2::PN_ReturnValue,ValueType,EGPD_Input);
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
    if(const auto* Decl=Module.Classes.FindByPredicate([&](const auto& C){return C.Id==ClassId;});Decl && (!Decl->PropertyOverrides.IsEmpty() || !Decl->NativeDefaults.IsEmpty())){Error=TEXT("Inherited defaults require the DSL service for source/override provenance");return false;}
    auto& Runtime=FModuleManager::LoadModuleChecked<FDasRuntimeModule>(TEXT("DasRuntime"));
    for (const auto& Method:Candidate.EffectiveMethods())
        if (!Runtime.ValidateFrameMethod(Runtime.ScriptPath(Method.ScriptFile),Error,Method.EntryPoint,Method.Contract())) return false;
    return Apply(BP, Candidate, Error);
}

bool FDasBlueprintService::Apply(UBlueprint* BP, const FDasClassSchema& Schema, FString& Error, bool bValidateOnly, EDasDefaultMigrationPolicy DefaultPolicy)
{
    Error.Reset();
    if (!IsInGameThread() || !IsValid(BP) || !BP->GeneratedClass || !BP->ParentClass || (!DasActorHostClass(BP->ParentClass) && !BP->ParentClass->IsChildOf(UDasScriptComponent::StaticClass()) && !BP->ParentClass->IsChildOf(UDasSaveGame::StaticClass())))
    { Error = TEXT("Expected an Actor or Component adapter Blueprint on the game thread"); return false; }
    if (!Schema.Validate(Error)) return false;
    if(!Schema.ParentStableId.IsEmpty()){const auto* CDO=BP->ParentClass->GetDefaultObject();const FString Id=DasHostIdentity(CDO);if(Id!=Schema.ParentStableId){Error=TEXT("Authored parent identity differs from declaration");return false;}}
    if(!DasNativeScriptHost(Schema.Parent.ToString()) || !BP->ParentClass->IsChildOf(DasNativeScriptHost(Schema.Parent.ToString()))){Error=TEXT("DSL parent differs from Blueprint adapter");return false;}
    const FString ExistingId = BP->GetOutermost()->GetMetaData().GetValue(BP,TEXT("DasClassId"));
    if (!ExistingId.IsEmpty() && Schema.StableId != ExistingId)
    { Error = TEXT("Authored class stable ID changed"); return false; }
    const bool MigrateDefaults=DefaultPolicy==EDasDefaultMigrationPolicy::PreserveInstances;
    const bool CheckDefaultValues=!MigrateDefaults && DefaultPolicy!=EDasDefaultMigrationPolicy::PreserveOverrides;
    if(DefaultPolicy!=EDasDefaultMigrationPolicy::Refuse && !MigrateDefaults && !(bValidateOnly && DefaultPolicy==EDasDefaultMigrationPolicy::PreserveOverrides)){Error=TEXT("Unknown default migration policy");return false;}
    TSet<FString> ExistingInterfaces,WantedInterfaces;for(const auto& I:BP->ImplementedInterfaces)if(I.Interface)ExistingInterfaces.Add(I.Interface->GetPathName());for(const auto& I:Schema.Interfaces)WantedInterfaces.Add(I);
    if(!ExistingId.IsEmpty()){bool Same=ExistingInterfaces.Num()==WantedInterfaces.Num();for(const auto& I:ExistingInterfaces)Same &= WantedInterfaces.Contains(I);if(!Same){Error=TEXT("Changing interface implementation requires a later structural policy");return false;}}
    else for(const auto& I:ExistingInterfaces)if(!WantedInterfaces.Contains(I)){Error=TEXT("Existing undeclared interface conflict");return false;}
    const auto Methods=Schema.EffectiveMethods();
    if(MigrateDefaults && (Schema.Fields.Num()!=BP->NewVariables.Num() || Methods.ContainsByPredicate([&](const auto& M){return !DasHasMethod(BP,M.Name);}))){Error=TEXT("Default migration cannot be combined with structural changes");return false;}
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
    for (UEdGraph* Graph:DasOwnedMethodGraphs(BP))
    {
        const FString Prior=BP->GetOutermost()->GetMetaData().GetValue(Graph,TEXT("DasMemberId"));
        if (!Prior.IsEmpty() && !Methods.ContainsByPredicate([&](const auto& M){return M.StableId==Prior && M.Name==Graph->GetFName();}))
        {Error=TEXT("Method removal/rename requires migration");return false;}
    }
    for (const auto& M:Methods)
    {
        if(auto* ParentMethod=BP->ParentClass->FindFunctionByName(M.Name)){if(Schema.ParentStableId.IsEmpty() || !M.bBlueprintEvent || ParentMethod->HasAnyFunctionFlags(FUNC_Final) || !MethodShape(ParentMethod,M)){Error=TEXT("Override requires a compatible non-final authored BlueprintEvent");return false;}}else if(BP->ParentClass->FindPropertyByName(M.Name)){Error=TEXT("Method conflicts with inherited property");return false;}
        if (UFunction* Prior=BP->GeneratedClass->FindFunctionByName(M.Name);Prior && Prior->GetOuterUClass()==BP->GeneratedClass)
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
    FDasDefaultRoots DefaultRoots;
    TMap<FName, FEdGraphPinType> Pins;
    TMap<FName, FString> Defaults;
    for (const auto& Field : Schema.Fields)
    {
        if (BP->ParentClass->FindPropertyByName(Field.Name) || BP->GeneratedClass->FindFunctionByName(Field.Name))
        { Error = TEXT("Field conflicts with an inherited property or function"); return false; }
        if(Field.Kind==EDasScalarKind::Delegate){Pins.Add(Field.Name,DasSingleDelegatePin(BP,Field));Defaults.Add(Field.Name,TEXT(""));continue;}
        if(Field.Kind==EDasScalarKind::Event){FEdGraphPinType Pin;Pin.PinCategory=UEdGraphSchema_K2::PC_MCDelegate;Pins.Add(Field.Name,Pin);Defaults.Add(Field.Name,TEXT(""));continue;}
        Pins.Add(Field.Name,ScalarPin(Field.Kind,Field.Kind==EDasScalarKind::Enum?Field.EnumPath:(Field.Kind==EDasScalarKind::Object || Field.Kind==EDasScalarKind::Class)?Field.ObjectClass:Field.StructPath));
        FString Default;if(!DasBlueprintFieldDefault(Field,Default,DefaultRoots,Error))return false;
        Defaults.Add(Field.Name,MoveTemp(Default));
    }
    for (const auto& Old : BP->NewVariables)
    {
        const auto* Field = Schema.Fields.FindByPredicate([&](const FDasFieldSchema& F) { return F.Name == Old.VarName; });
        if (!Field || (Field->Kind==EDasScalarKind::Event?Old.VarType.PinCategory!=UEdGraphSchema_K2::PC_MCDelegate:Field->Kind==EDasScalarKind::Delegate?Old.VarType.PinCategory!=UEdGraphSchema_K2::PC_Delegate:Pins[Old.VarName] != Old.VarType) ||
            Field->bInstanceEditable == !!(Old.PropertyFlags & CPF_DisableEditOnInstance) || Field->bReadOnly != !!(Old.PropertyFlags & CPF_BlueprintReadOnly))
        { Error = TEXT("Removal, type, default or editability changes require a migration policy"); return false; }
        UObject* DefaultsObject = BP->GeneratedClass->GetDefaultObject();
        if(Field->Kind==EDasScalarKind::Delegate){auto* P=CastField<FDelegateProperty>(BP->GeneratedClass->FindPropertyByName(Old.VarName));FDasAuthoringModule Definition;if(!FDasAuthoringModule::FromJson(Field->DelegateWire,Definition,Error))return false;TStrongObjectPtr<UFunction> Sig(MakeDasDelegateSignature(Definition.Classes[0].Methods[0],false,GetTransientPackage(),NAME_None,Error));if(!P || !Sig || !Sig->IsSignatureCompatibleWith(P->SignatureFunction)){Error=TEXT("Single delegate signature changed; migration is deferred");return false;}if(MigrateDefaults){Error=TEXT("Delegate default migration is deferred");return false;}continue;}
        if(Field->Kind==EDasScalarKind::Event){if(!DasValidateEventField(BP,*Field,Error))return false;if(MigrateDefaults){Error=TEXT("Default migration of classes with events is deferred");return false;}continue;}
        if(Field->Kind==EDasScalarKind::Class){auto* P=CastField<FClassProperty>(BP->GeneratedClass->FindPropertyByName(Old.VarName));auto* Base=FindObject<UClass>(nullptr,*Field->ObjectClass);auto* Selected=P?Cast<UClass>(P->GetObjectPropertyValue_InContainer(DefaultsObject)):nullptr;if(!P || !Base || P->MetaClass!=Base || (Selected && !Selected->IsChildOf(Base))){Error=TEXT("Class reference base/default differs from declaration");return false;}if(CheckDefaultValues && !Field->StringDefault.IsEmpty() && (!Selected || Selected->GetPathName()!=Field->StringDefault)){Error=TEXT("Explicit class default differs; migration is deferred");return false;}if(MigrateDefaults){Error=TEXT("Default migration of class references is deferred");return false;}continue;}
        if(!(Field->Kind==EDasScalarKind::Struct && DasContainerProperty(Field->StructPath)) && !ScalarProperty(BP->GeneratedClass->FindPropertyByName(Old.VarName),static_cast<EDasMethodSignature>(Field->Kind))){Error=TEXT("Default migration requires matching reflected property");return false;}
        if(Field->Kind==EDasScalarKind::Enum){auto* P=BP->GeneratedClass->FindPropertyByName(Old.VarName);auto* E=FindObject<UEnum>(nullptr,*Field->EnumPath);if(!P || DasEnumPropertyType(P)!=E || (CheckDefaultValues && DasEnumPropertyNumber(P,P->ContainerPtrToValuePtr<void>(DefaultsObject))!=E->GetValueByNameString(Field->EnumDefault))){Error=TEXT("Enum type/default changes require explicit migration");return false;}continue;}
        if(Field->Kind==EDasScalarKind::Object){auto* P=BP->GeneratedClass->FindPropertyByName(Old.VarName);auto* ReferenceClass=DasReferencePropertyClass(P);if(!ReferenceClass || ReferenceClass->GetPathName()!=Field->ObjectClass || (CheckDefaultValues && (DasReferenceGet(P,P->ContainerPtrToValuePtr<void>(DefaultsObject))?DasReferenceGet(P,P->ContainerPtrToValuePtr<void>(DefaultsObject))->GetPathName():FString())!=Field->StringDefault)){Error=TEXT("Object class/default changes require an explicit migration");return false;}continue;}
        if(Field->Kind==EDasScalarKind::Struct)if(auto* Carrier=DasContainerProperty(Field->StructPath)){auto* P=BP->GeneratedClass->FindPropertyByName(Old.VarName);if(!P || !P->SameType(Carrier)){Error=TEXT("Container field identity mismatch");return false;}FInstancedStruct Expected;if(!ParseDasStructDefault(Field->StructPath,Field->StructDefault,Expected,Error))return false;if(CheckDefaultValues && !MatchesDasPropertyDefault(P,P->ContainerPtrToValuePtr<void>(DefaultsObject),Carrier->ContainerPtrToValuePtr<void>(Expected.GetMemory()))){Error=TEXT("Container default differs from declaration");return false;}continue;}
        if(Field->Kind==EDasScalarKind::Struct){auto* P=FindFProperty<FStructProperty>(BP->GeneratedClass,Old.VarName);FInstancedStruct Value;if(!P || P->Struct->GetPathName()!=Field->StructPath || !ParseDasStructDefault(Field->StructPath,Field->StructDefault,Value,Error))return false;if(CheckDefaultValues && !MatchesDasStructDefault(P->Struct,P->ContainerPtrToValuePtr<void>(DefaultsObject),Value.GetMemory())){Error=TEXT("Changing a struct class default requires a migration policy");return false;}continue;}
        const auto* IntProperty = FindFProperty<FIntProperty>(BP->GeneratedClass, Old.VarName);
        const auto* FloatProperty = FindFProperty<FFloatProperty>(BP->GeneratedClass, Old.VarName);
        const auto* BoolProperty=FindFProperty<FBoolProperty>(BP->GeneratedClass,Old.VarName);
        const auto* DoubleProperty=FindFProperty<FDoubleProperty>(BP->GeneratedClass,Old.VarName);const auto* StringProperty=FindFProperty<FStrProperty>(BP->GeneratedClass,Old.VarName);const auto* NameProperty=FindFProperty<FNameProperty>(BP->GeneratedClass,Old.VarName);
        const auto* ByteProperty=FindFProperty<FByteProperty>(BP->GeneratedClass,Old.VarName);const auto* TextProperty=FindFProperty<FTextProperty>(BP->GeneratedClass,Old.VarName);
        const bool bSameDefault=Field->Kind==EDasScalarKind::Byte?ByteProperty && !ByteProperty->Enum && ByteProperty->GetPropertyValue_InContainer(DefaultsObject)==Field->IntDefault:Field->Kind==EDasScalarKind::Text?TextProperty && DasTextDefaultsEqual(TextProperty->GetPropertyValue_InContainer(DefaultsObject),Field->TextDefault):Field->Kind==EDasScalarKind::Int32?IntProperty && IntProperty->GetPropertyValue_InContainer(DefaultsObject)==Field->IntDefault:Field->Kind==EDasScalarKind::Float32?FloatProperty && FloatProperty->GetPropertyValue_InContainer(DefaultsObject)==Field->FloatDefault:Field->Kind==EDasScalarKind::Bool?BoolProperty && BoolProperty->GetPropertyValue_InContainer(DefaultsObject)==Field->BoolDefault:Field->Kind==EDasScalarKind::Double?DoubleProperty && DoubleProperty->GetPropertyValue_InContainer(DefaultsObject)==Field->DoubleDefault:Field->Kind==EDasScalarKind::String?StringProperty && StringProperty->GetPropertyValue_InContainer(DefaultsObject)==Field->StringDefault:NameProperty && NameProperty->GetPropertyValue_InContainer(DefaultsObject)==Field->NameDefault;

        if (!bSameDefault && CheckDefaultValues) { Error = TEXT("Changing a class default requires a migration policy"); return false; }
    }
    // Validate existing graph shape before adding any properties.
    for (UEdGraph* Graph : DasOwnedMethodGraphs(BP))
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
    for(UEdGraph* G:BP->UbergraphPages)for(UEdGraphNode* Node:G->Nodes)if(auto* Event=Cast<UK2Node_Event>(Node);Event && Event->bOverrideFunction){const FString Id=BP->GetOutermost()->GetMetaData().GetValue(Event,TEXT("DasMemberId"));const auto* Method=Methods.FindByPredicate([&](const auto& M){return M.Name==Event->EventReference.GetMemberName();});if(!Id.IsEmpty() && (!Method || Method->StableId!=Id)){Error=TEXT("Interface event removal/rename requires migration");return false;}if(Method && DasIsInterfaceEvent(BP,*Method) && (Id!=Method->StableId || !DasInterfaceEventAdapter(Event) || !MethodShape(BP->GeneratedClass->FindFunctionByName(Method->Name),*Method))){Error=TEXT("Existing interface event is not an owned daScript adapter");return false;}}
    // Parse every struct before mutation. A default migration changes only this
    // Blueprint's CDO; existing instances and derived-class CDOs retain their values.
    TMap<FName,FInstancedStruct> StructDefaults;
    if(MigrateDefaults)for(const auto& Field:Schema.Fields)if(Field.Kind==EDasScalarKind::Struct){FInstancedStruct Value;if(!ParseDasStructDefault(Field.StructPath,Field.StructDefault,Value,Error))return false;StructDefaults.Add(Field.Name,MoveTemp(Value));}
    if(bValidateOnly) return true;
    if(!DasEnsureSingleSignatures(BP,Schema,Error))return false;
    for(const auto& Field:Schema.Fields)if(Field.Kind==EDasScalarKind::Delegate)Pins[Field.Name]=DasSingleDelegatePin(BP,Field);
    if(MigrateDefaults){
        BP->Modify();UObject* CDO=BP->GeneratedClass->GetDefaultObject();CDO->Modify();
        for(auto& Variable:BP->NewVariables){const auto& F=*Schema.Fields.FindByPredicate([&](const auto& Field){return Field.Name==Variable.VarName;});auto* P=BP->GeneratedClass->FindPropertyByName(F.Name);
            switch(F.Kind){
                case EDasScalarKind::Int32:CastFieldChecked<FIntProperty>(P)->SetPropertyValue_InContainer(CDO,F.IntDefault);break;
                case EDasScalarKind::Float32:CastFieldChecked<FFloatProperty>(P)->SetPropertyValue_InContainer(CDO,F.FloatDefault);break;
                case EDasScalarKind::Bool:CastFieldChecked<FBoolProperty>(P)->SetPropertyValue_InContainer(CDO,F.BoolDefault);break;
                case EDasScalarKind::Double:CastFieldChecked<FDoubleProperty>(P)->SetPropertyValue_InContainer(CDO,F.DoubleDefault);break;
                case EDasScalarKind::String:CastFieldChecked<FStrProperty>(P)->SetPropertyValue_InContainer(CDO,F.StringDefault);break;
                case EDasScalarKind::Byte:CastFieldChecked<FByteProperty>(P)->SetPropertyValue_InContainer(CDO,uint8(F.IntDefault));break;
                case EDasScalarKind::Text:CastFieldChecked<FTextProperty>(P)->SetPropertyValue_InContainer(CDO,F.TextDefault);break;
                case EDasScalarKind::Name:CastFieldChecked<FNameProperty>(P)->SetPropertyValue_InContainer(CDO,F.NameDefault);break;
                case EDasScalarKind::Enum:SetDasEnumPropertyNumber(P,P->ContainerPtrToValuePtr<void>(CDO),FindObject<UEnum>(nullptr,*F.EnumPath)->GetValueByNameString(F.EnumDefault));break;
                case EDasScalarKind::Object:{UObject* Value=nullptr;if(!ResolveDasReferenceDefault(false,F.ObjectClass,F.StringDefault,Value,Error))return false;DasReferenceSet(P,P->ContainerPtrToValuePtr<void>(CDO),Value);break;}
                case EDasScalarKind::Struct:if(auto* Values=DasContainerProperty(F.StructPath))P->CopyCompleteValue(P->ContainerPtrToValuePtr<void>(CDO),Values->ContainerPtrToValuePtr<void>(StructDefaults.FindChecked(F.Name).GetMemory()));else CastFieldChecked<FStructProperty>(P)->Struct->CopyScriptStruct(P->ContainerPtrToValuePtr<void>(CDO),StructDefaults.FindChecked(F.Name).GetMemory());break;
            }
            Variable.DefaultValue=Defaults.FindChecked(F.Name);
        }
        FBlueprintEditorUtils::MarkBlueprintAsModified(BP);BP->MarkPackageDirty();
        UE_LOG(LogTemp,Display,TEXT("DAS_DEFAULT_MIGRATION PASS preserve_instances; CDO updated without reinstance"));return true;
    }
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
        if(Schema.Parent==TEXT("SaveGame"))FBlueprintEditorUtils::SetVariableSaveGameFlag(BP,Field.Name,true);
        for (const auto* Key:{TEXT("Category"),TEXT("DisplayName"),TEXT("ToolTip")}) FBlueprintEditorUtils::RemoveBlueprintVariableMetaData(BP,Field.Name,nullptr,Key);
        FBlueprintEditorUtils::SetBlueprintVariableCategory(BP,Field.Name,nullptr,FText::FromString(Field.Metadata.FindRef(TEXT("Category"))),true);
        for (const auto& Pair:Field.Metadata) FBlueprintEditorUtils::SetBlueprintVariableMetaData(BP,Field.Name,nullptr,*Pair.Key,Pair.Value);
    }
    for(const auto& Field:Schema.Fields)if(Field.Kind==EDasScalarKind::Event && !DasCreateEventGraph(BP,Field,Error))return false;
    for(const auto& Path:Schema.Interfaces){auto* Interface=FindObject<UClass>(nullptr,*Path);if(!BP->ImplementedInterfaces.ContainsByPredicate([&](const auto& I){return I.Interface==Interface;})){FBPInterfaceDescription Description;Description.Interface=Interface;BP->ImplementedInterfaces.Add(Description);}}
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
        if (LegacyMethodName.IsEmpty() && !Methods.IsEmpty()) Metadata.SetValue(BP,TEXT("DasMethodName"),*Methods[0].Name.ToString());
        if (LegacyMethodName.IsEmpty() && !Methods.IsEmpty()) Metadata.SetValue(BP,TEXT("DasMethodId"),*Methods[0].StableId);
        if (LegacyMethodName.IsEmpty() && !Methods.IsEmpty()) Metadata.SetValue(BP,TEXT("DasInputId"),*Methods[0].InputStableId);
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
        for (UEdGraph* Graph:DasOwnedMethodGraphs(BP)) if (Graph->GetFName()==M.Name && !M.StableId.IsEmpty()) Metadata.SetValue(Graph,TEXT("DasMemberId"),*M.StableId);
    }
    return true;
}

bool FDasBlueprintService::ApplyLibraryGraphs(UBlueprint* BP,const FDasAuthoringModule& Model,const FString& Id,FString& Error){
 FDasClassSchema Schema;if(!BP || BP->BlueprintType!=BPTYPE_FunctionLibrary || !BP->ParentClass->IsChildOf(UDasBlueprintLibrary::StaticClass()) || !Model.ToActorSchema(Id,Schema,Error) || Schema.Parent!=TEXT("FunctionLibrary")){if(Error.IsEmpty())Error=TEXT("Expected authored function library");return false;}
 for(const auto& M:Schema.Methods)if(!SetFrameMethod(BP,M)){Error=TEXT("Cannot build static function graph: ")+M.Name.ToString();return false;}
 FKismetEditorUtilities::CompileBlueprint(BP);if(BP->Status==BS_Error){Error=TEXT("Static library Blueprint compilation failed");return false;}
 for(const auto& M:Schema.Methods)if(!MethodShape(BP->GeneratedClass->FindFunctionByName(M.Name),M)){Error=TEXT("Static function flags/signature mismatch: ")+M.Name.ToString();return false;}
 BP->MarkPackageDirty();return true;
}
