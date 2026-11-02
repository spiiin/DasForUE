#include "DasAnimationAssetLibrary.h"
#include "Animation/AnimBlueprint.h"
#include "Animation/AnimInstance.h"
#include "Animation/BlendSpace.h"
#include "Animation/Skeleton.h"
#include "AnimGraphNode_BlendSpacePlayer.h"
#include "AnimGraphNode_Root.h"
#include "AnimGraphNode_Slot.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "Factories/AnimBlueprintFactory.h"
#include "K2Node_CallFunction.h"
#include "Kismet/KismetMathLibrary.h"
#include "KismetAnimationLibrary.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "Kismet2/KismetEditorUtilities.h"
#include "GameFramework/Actor.h"
#include "EdGraph/EdGraph.h"
#include "EdGraph/EdGraphSchema.h"
#include "Misc/PackageName.h"
#include "UObject/MetaData.h"

namespace
{
template <class T> T* AddNode(UEdGraph* Graph, int32 X, int32 Y)
{
    auto* Node = NewObject<T>(Graph);
    Graph->AddNode(Node, false, false);
    Node->CreateNewGuid();
    Node->NodePosX = X;
    Node->NodePosY = Y;
    Node->PostPlacedNewNode();
    return Node;
}
UK2Node_CallFunction* Call(UEdGraph* Graph, UFunction* Function, int32 X, int32 Y)
{
    auto* Node = AddNode<UK2Node_CallFunction>(Graph, X, Y);
    Node->SetFromFunction(Function);
    Node->AllocateDefaultPins();
    return Node;
}
}

UAnimBlueprint* UDasAnimationAssetLibrary::CreateLocomotionBlueprint(const FString& PackagePath, USkeleton* Skeleton,
                                                                     UBlendSpace* BlendSpace, int32 SpeedAxis,
                                                                     int32 DirectionAxis, FName MontageSlot,
                                                                     FString& Error)
{
    Error.Reset();
    if (!IsInGameThread() || !IsValid(Skeleton) || !IsValid(BlendSpace) || BlendSpace->GetSkeleton() != Skeleton ||
        !PackagePath.StartsWith(TEXT("/Game/")) || !FPackageName::IsValidLongPackageName(PackagePath) ||
                                SpeedAxis < 0 || SpeedAxis > 2 || DirectionAxis > 2 || DirectionAxis == SpeedAxis)
    {
        Error = TEXT("Expected compatible skeleton/blendspace, distinct axes and a /Game package");
        return nullptr;
    }
    const FString ObjectPath = PackagePath + TEXT(".")+ FPackageName::GetLongPackageAssetName(PackagePath);
    if (auto* Existing = LoadObject<UAnimBlueprint>(nullptr, *ObjectPath, nullptr, LOAD_NoWarn | LOAD_Quiet))
    {
        if (Existing->TargetSkeleton == Skeleton && Existing->GetOutermost()->GetMetaData().GetValue(
                                                        Existing, TEXT("DasLocomotion")) == BlendSpace->GetPathName())
            return Existing;
        Error = TEXT("Destination already exists with different animation configuration");
        return nullptr;
    }
    auto* Factory = NewObject<UAnimBlueprintFactory>();
    Factory->ParentClass = UAnimInstance::StaticClass();
    Factory->TargetSkeleton = Skeleton;
    auto* BP = Cast<UAnimBlueprint>(Factory->FactoryCreateNew(
        UAnimBlueprint::StaticClass(), CreatePackage(*PackagePath),
        FName(*FPackageName::GetLongPackageAssetName(PackagePath)), RF_Public | RF_Standalone, nullptr, GWarn));
    if (!BP)
    {
        Error = TEXT("AnimBlueprint factory failed");
        return nullptr;
    }
    BP->bUseMultiThreadedAnimationUpdate = false;
    TArray<UEdGraph*> Graphs;
    BP->GetAllGraphs(Graphs);
    UEdGraph* Graph = nullptr;
    UAnimGraphNode_Root* Root = nullptr;
    for (auto* Candidate : Graphs)
        for (const auto& Node : Candidate->Nodes)
            if (auto* R = Cast<UAnimGraphNode_Root>(Node.Get()))
            {
                Graph = Candidate;
                Root = R;
            }
    if (!Graph)
    {
        Error = TEXT("Factory did not create an animation graph");
        return nullptr;
    }
    auto* Player = AddNode<UAnimGraphNode_BlendSpacePlayer>(Graph, 0, 0);
    Player->SetAnimationAsset(BlendSpace);
    Player->AllocateDefaultPins();
    auto* Slot = AddNode<UAnimGraphNode_Slot>(Graph, 300, 0);
    Slot->Node.SlotName = MontageSlot;
    Slot->AllocateDefaultPins();
    Root->NodePosX = 600;
    auto* Owner = Call(Graph, UAnimInstance::StaticClass()->FindFunctionByName(TEXT("GetOwningActor")), -1200, 100);
    auto* Velocity = Call(Graph, AActor::StaticClass()->FindFunctionByName(TEXT("GetVelocity")), -900, 100);
    auto* Speed = Call(Graph, UKismetMathLibrary::StaticClass()->FindFunctionByName(TEXT("VSizeXY")), -600, 100);
    auto* Rotation = Call(Graph, AActor::StaticClass()->FindFunctionByName(TEXT("K2_GetActorRotation")), -900, 400);
    auto* Direction =
        Call(Graph, UKismetAnimationLibrary::StaticClass()->FindFunctionByName(TEXT("CalculateDirection")), -300, 400);
    const auto* Schema = Graph->GetSchema();
    auto Link = [&](UEdGraphNode* A, const TCHAR* AP, UEdGraphNode* B, const TCHAR* BPin)
    {
        auto* Out = A->FindPin(AP);
        auto* In = B->FindPin(BPin);
        if (!Out || !In || !Schema->TryCreateConnection(Out, In))
        {
            Error = FString::Printf(TEXT("Cannot connect %s.%s to %s.%s"), *A->GetClass()->GetName(), AP,
                                         *B->GetClass()->GetName(), BPin);
            return false;
        }
        return true;
    };
    const TCHAR* Axes[] = {TEXT("X"), TEXT("Y"), TEXT("Z")};
    if (!Link(Owner, TEXT("ReturnValue"), Velocity, TEXT("self")) ||
                          !Link(Owner,
                                TEXT("ReturnValue"), Rotation, TEXT("self")) ||
                                     !Link(Velocity,
                                           TEXT("ReturnValue"), Speed, TEXT("A")) ||
                                                !Link(Velocity,
                                                      TEXT("ReturnValue"), Direction, TEXT("Velocity")) ||
                                                           !Link(Rotation,
                                                                 TEXT("ReturnValue"), Direction,
                                                                      TEXT("BaseRotation")) ||
                                                                      !Link(Speed, TEXT("ReturnValue"), Player,
                                                                                        Axes[SpeedAxis]) ||
                                                                            (DirectionAxis >= 0 &&
                                                                                 !Link(Direction,
                                                                                       TEXT("ReturnValue"), Player,
                                                                                            Axes[DirectionAxis])) ||
                                                                             !Link(
                                                                                 Player,
                                                                                 TEXT("Pose"), Slot, TEXT("Source")) ||
                                                                                      !Link(Slot, TEXT("Pose"), Root,
                                                                                                       TEXT("Result")))
        return nullptr;
    FBlueprintEditorUtils::MarkBlueprintAsStructurallyModified(BP);
    FKismetEditorUtilities::CompileBlueprint(BP);
    if (BP->Status == BS_Error || !BP->GeneratedClass)
    {
        Error = TEXT("Animation graph compilation failed");
        return nullptr;
    }
    BP->GetOutermost()->GetMetaData().SetValue(BP, TEXT("DasLocomotion"), *BlendSpace->GetPathName());
    FAssetRegistryModule::AssetCreated(BP);
    BP->MarkPackageDirty();
    return BP;
}
