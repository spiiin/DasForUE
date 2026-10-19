#include "DasComponentCompilerExtension.h"
#include "Engine/Blueprint.h"
#include "Engine/BlueprintGeneratedClass.h"
#include "Engine/SimpleConstructionScript.h"
#include "Engine/SCS_Node.h"
#include "UObject/UnrealType.h"
void UDasComponentCompilerExtension::ProcessBlueprintCompiled(const FKismetCompilerContext& C,
                                                              const FBlueprintCompiledData&)
{
    if (!C.Blueprint || !C.NewClass || !C.Blueprint->SimpleConstructionScript)
        return;
    for (auto* Node : C.Blueprint->SimpleConstructionScript->GetAllNodes())
    {
        if (Node->FindMetaDataEntryIndexForKey(TEXT("DasComponentId")) == INDEX_NONE)
            continue;
        auto* P = C.NewClass->FindPropertyByName(Node->GetVariableName());
        if (!P)
            continue;
        auto Flag = [&](const TCHAR* Key)
        {
            return Node->FindMetaDataEntryIndexForKey(Key) != INDEX_NONE && Node->GetMetaData(Key) == TEXT("true");
        };
        P->ClearPropertyFlags(CPF_BlueprintReadOnly | CPF_Edit | CPF_EditConst);
        P->SetPropertyFlags(CPF_BlueprintVisible);
        if (Flag(TEXT("DasComponentReadOnly")))
            P->SetPropertyFlags(CPF_BlueprintReadOnly);
        if (Flag(TEXT("DasComponentEditable")) || Flag(TEXT("DasComponentVisible")))
            P->SetPropertyFlags(CPF_Edit);
        if (Flag(TEXT("DasComponentVisible")))
            P->SetPropertyFlags(CPF_EditConst);
        for (const auto* Key : {TEXT("Category"), TEXT("DisplayName"), TEXT("ToolTip")})
            if (Node->FindMetaDataEntryIndexForKey(Key) != INDEX_NONE)
                P->SetMetaData(Key, *Node->GetMetaData(Key));
    }
}
