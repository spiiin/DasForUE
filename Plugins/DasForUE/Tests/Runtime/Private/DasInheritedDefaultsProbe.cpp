#include "DasInheritedDefaultsProbe.h"
#include "DasProbeActor.h"
#include "DasScriptComponent.h"
#include "Engine/World.h"
#include "Engine/Level.h"
#include "UObject/UnrealType.h"
#include "UObject/StructOnScope.h"
bool RunDasInheritedDefaultsProbe(UWorld* World, FString& Report)
{
    ADasProbeActor* A = nullptr;
    ADasProbeActor* M = nullptr;
    ADasProbeActor* Leaf = nullptr;
    if (!World || !World->PersistentLevel)
        return false;
    for (const auto& Actor : World->PersistentLevel->Actors)
        if (auto* P = Cast<ADasProbeActor>(Actor))
        {
            if (P->Tags.Contains(TEXT("InheritedDefault")))
                A = P;
            if (P->Tags.Contains(TEXT("InheritedManual")))
                M = P;
            if (P->Tags.Contains(TEXT("InheritedLeaf")))
                Leaf = P;
        }
    if (!A || !M || !Leaf)
        return false;
    bool Good = true;
    auto Check = [&](bool OK, const TCHAR* Name)
    {
        Good &= OK;
        Report += FString::Printf(TEXT("DAS_INHERITED_COOKED %s %s\n"), OK ? TEXT("PASS"): TEXT("FAIL"), Name);
    };
    auto Int = [](UObject* O, const TCHAR* N)
    {
        auto* P = O ? FindFProperty<FIntProperty>(O->GetClass(), N) : nullptr;
        return P ? P->GetPropertyValue_InContainer(O) : MIN_int32;
    };
    auto Path = [](UObject* O, const TCHAR* N)
    {
        auto* P = O ? FindFProperty<FObjectPropertyBase>(O->GetClass(), N) : nullptr;
        auto* Value = P ? P->GetObjectPropertyValue_InContainer(O) : nullptr;
        return Value ? Value->GetPathName() : FString();
    };
    auto* Class = A->GetClass();
    auto* CDO = Class->GetDefaultObject();
    auto* Base = Class->GetSuperClass()->GetDefaultObject();
    Check(Int(Base, TEXT("Score")) == 12 && Int(Base, TEXT("Keep")) == 150 &&
                                                Int(CDO, TEXT("Keep")) == 150 &&
                                                    Int(CDO, TEXT("Score")) == 25 &&
                                                        Int(CDO, TEXT("Manual")) == 77 &&
                                                            Int(CDO, TEXT("Frozen")) == 10 &&
                                                                Int(Base, TEXT("Frozen")) == 12,
                                                                    TEXT("parent_child_defaults_and_manual_CDO"));
    Check(Int(A, TEXT("Score")) == 25 && Int(M, TEXT("Score")) == 111 &&
                                             Int(Leaf, TEXT("Score")) == 30 &&
                                                 Int(Leaf, TEXT("Frozen")) == 10,
                                                     TEXT("placed_instances_and_three_level_overrides"));
    Check(Path(CDO, TEXT("Asset")) == TEXT("/Engine/BasicShapes/Sphere.Sphere") &&
                                           Path(Base, TEXT("Asset")) ==
                                                TEXT("/Engine/BasicShapes/Cube.Cube") &&
                                                     Path(CDO, TEXT("SpawnType")) ==
                                                          TEXT("/Script/Engine.Pawn") &&
                                                               Path(Base, TEXT("SpawnType")) ==
                                                                    TEXT("/Script/Engine.Actor"),
                                                                         TEXT("inherited_asset_and_class_defaults"));
    auto* Vec = FindFProperty<FStructProperty>(Class, TEXT("Position"));
    Check(Vec && *Vec->ContainerPtrToValuePtr<FVector>(CDO) == FVector(99, 5, 6) &&
              *Vec->ContainerPtrToValuePtr<FVector>(M) == FVector(99, 222, 6),
          TEXT("inherited_struct_member_overrides"));
    auto Call = [](UObject* O, const TCHAR* Name)
    {
        auto* F = O->FindFunction(Name);
        if (!F)
            return MIN_int32;
        FStructOnScope Args(F);
        FindFProperty<FIntProperty>(F, TEXT("input"))->SetPropertyValue_InContainer(Args.GetStructMemory(), 0);
        O->ProcessEvent(F, Args.GetStructMemory());
        return CastFieldChecked<FIntProperty>(F->GetReturnProperty())
            ->GetPropertyValue_InContainer(Args.GetStructMemory());
    };
    Check(Call(A, TEXT("Read")) == 102 && Call(A, TEXT("BaseRead")) == 25 &&
                                               Call(Leaf, TEXT("LeafRead")) == 30 && A->LastDasError.IsEmpty() &&
                                                    Leaf->LastDasError.IsEmpty(),
                                                    TEXT("cooked_default_owners_and_base_entry"));
    TArray<UDasScriptComponent*> Components;
    A->GetComponents(Components);
    UDasScriptComponent* C = nullptr;
    for (auto* Item : Components)
        if (Item->ComponentTags.Contains(TEXT("InheritedComponentFixture")))
            C = Item;
    Check(C && Int(C, TEXT("Bonus")) == 55 &&
                   Int(C->GetClass()->GetDefaultObject(), TEXT("Bonus")) == 19 &&
                       Int(C->GetClass()->GetSuperClass()->GetDefaultObject(), TEXT("Bonus")) == 7 &&
                           Call(C, TEXT("Read")) == 55 && C->LastDasError.IsEmpty(),
                                TEXT("component_inherited_defaults_and_manual_overrides"));
    auto* Fresh = World->SpawnActor<ADasProbeActor>(Class);
    Check(Fresh && Int(Fresh, TEXT("Score")) == 25 && Int(Fresh, TEXT("Manual")) == 77 &&
                                                          Int(Fresh, TEXT("Frozen")) == 10, TEXT("fresh_spawn"));
    if (Fresh)
        Fresh->Destroy();
    return Good;
}
