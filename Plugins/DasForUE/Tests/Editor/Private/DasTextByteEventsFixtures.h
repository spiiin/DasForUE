#pragma once
#include "DasInterfaceReentryFixtures.h"
#include "DasInterfaceService.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "Kismet2/KismetEditorUtilities.h"
#include "K2Node_FunctionEntry.h"
#include "K2Node_FunctionResult.h"
#include "EdGraphSchema_K2.h"
#include "DasDSLService.h"
#include "DasStaticLibraryProbe.h"
#include "Engine/Blueprint.h"
#include "UObject/SavePackage.h"
#include "Misc/PackageName.h"
#include "HAL/FileManager.h"
inline bool PrepareDasTextByteEventsFixtures(const FString& Root, ADasStaticLibraryProbe* Carrier,
                                             TArray<UBlueprint*>& Assets, FString& Error, bool SaveAssets)
{
    TArray<UBlueprint*> Interfaces;
    if (!FDasInterfaceService::Apply(TEXT("dsl_text_byte_interface.das"), TEXT("/Game/DasTextByteEvents/Types"),
                                                                               Interfaces, Error))
        return false;
    UBlueprint* BP = nullptr;
    if (!FDasDSLService::Apply(TEXT("dsl_text_byte_events.das"),
                                    Root + TEXT("/BP_TextByteEvents"),
                                                TEXT("dsl_text_byte_events.TextByteEvents"), BP, Error) ||
                                                !FDasDSLService::Reimport(BP, Error))
        return false;
    Assets.Append(Interfaces);
    Assets.Add(BP);
    Carrier->TextByteEventsClass = BP->GeneratedClass;
    // Plain Blueprint implementation exercises DAS -> interface calls without the
    // existing restriction on re-entering a DAS host through interface dispatch.
    const FString Package = Root + TEXT("/BP_TextByteInterfaceReceiver");
    auto* Receiver = FKismetEditorUtilities::CreateBlueprint(AActor::StaticClass(), CreatePackage(*Package),
                                                             TEXT("BP_TextByteInterfaceReceiver"), BPTYPE_Normal,
                                                                  UBlueprint::StaticClass(),
                                                                  UBlueprintGeneratedClass::StaticClass());
    if (!Receiver)
    {
        Error = TEXT("Cannot create interface receiver");
        return false;
    }
    FBPInterfaceDescription Description;
    Description.Interface = Interfaces[0]->GeneratedClass;
    for (TFieldIterator<UFunction> It(Description.Interface); It; ++It)
    {
        auto* Method = *It;
        if (Method->GetOuterUClass() != Description.Interface)
            continue;
        auto* Graph = FBlueprintEditorUtils::CreateNewGraph(Receiver, Method->GetFName(), UEdGraph::StaticClass(),
                                                            UEdGraphSchema_K2::StaticClass());
        FBlueprintEditorUtils::AddInterfaceGraph(Receiver, Graph, Description.Interface);
        Description.Graphs.Add(Graph);
        TArray<UK2Node_FunctionEntry*> Entries;
        TArray<UK2Node_FunctionResult*> Results;
        Graph->GetNodesOfClass(Entries);
        Graph->GetNodesOfClass(Results);
        if (Entries.Num() != 1 || Results.Num() != 1)
        {
            Error = TEXT("Interface receiver terminators missing");
            return false;
        }
        auto* Entry = Entries[0];
        auto* Result = Results[0];
        const auto* Schema = Graph->GetSchema();
        auto Connect = [&](const TCHAR* From, const TCHAR* To)
        {
            auto* Input = Entry->FindPin(From);
            auto* Output = Result->FindPin(To);
            return Input && Output && Schema->TryCreateConnection(Input, Output);
        };
        if (Method->GetFName() == TEXT("Change"))
        {
            if (!Connect(TEXT("number"), TEXT("previous")) ||
                              !Connect(TEXT("title"), TEXT("oldTitle")) || !Connect(TEXT("title"), TEXT("ReturnValue")))
            {
                Error = TEXT("Interface receiver ref/out connection failed");
                return false;
            }
            if (Result->FindPin(TEXT("title")))
                Connect(TEXT("title"), TEXT("title"));
            if (Result->FindPin(TEXT("number")))
                Connect(TEXT("number"), TEXT("number"));
        }
        else if (Method->GetFName() == TEXT("ChangeArrays"))
        {
            if (Result->FindPin(TEXT("numbers")))
                Connect(TEXT("numbers"), TEXT("numbers"));
        }
        else if (!Connect(TEXT("value"), TEXT("ReturnValue")))
        {
            Error = TEXT("Interface receiver echo connection failed");
            return false;
        }
    }
    Receiver->ImplementedInterfaces.Add(Description);
    FKismetEditorUtilities::CompileBlueprint(Receiver);
    if (Receiver->Status == BS_Error)
    {
        Error = TEXT("Interface receiver compilation failed");
        return false;
    }
    Carrier->TextByteInterfaceReceiverClass = Receiver->GeneratedClass;
    Assets.Add(Receiver);
    Interfaces.Add(Receiver);
    if (SaveAssets)
    {
        Interfaces.Add(BP);
        for (auto* Asset : Interfaces)
        {
            FSavePackageArgs Save;
            Save.TopLevelFlags = RF_Public | RF_Standalone;
            const FString File = FPackageName::LongPackageNameToFilename(Asset->GetOutermost()->GetName(),
                                                                         FPackageName::GetAssetPackageExtension());
            IFileManager::Get().MakeDirectory(*FPaths::GetPath(File), true);
            if (!UPackage::SavePackage(Asset->GetOutermost(), Asset, *File, Save))
            {
                Error = TEXT("Cannot save text/byte events fixture");
                return false;
            }
        }
    }
    return PrepareDasInterfaceReentryFixtures(Root, Carrier, Assets, Error, SaveAssets);
}
