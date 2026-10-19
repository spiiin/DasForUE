#include "DasCookedGameplayCommandlet.h"
#include "DasLibraryTestTypes.h"
#include "Misc/ScopeExit.h"
#include "DasPlayableDemo.h"
#include "DasMontageFixtures.h"
#include "DasInstanceDefaultsGameFixtures.h"
#include "DasPawnCharacterFixtures.h"
#include "DasStaticLibraryProbe.h"
#include "DasDSLService.h"
#include "DasPureFunctionTestGraph.h"
#include "DasProbeActor.h"
#include "DasScriptComponent.h"
#include "UObject/UnrealType.h"
#include "Engine/Blueprint.h"
#include "Engine/BlueprintGeneratedClass.h"
#include "Engine/World.h"
#include "UObject/SavePackage.h"
#include "Misc/PackageName.h"
#include "Misc/Paths.h"
#include "HAL/FileManager.h"

UDasCookedGameplayCommandlet::UDasCookedGameplayCommandlet()
{
    IsClient = false;
    IsServer = false;
    IsEditor = true;
    LogToConsole = true;
}
int32 UDasCookedGameplayCommandlet::Main(const FString& Params)
{
    FString Id;
    FParse::Value(*Params, TEXT("ProbeId="), Id);
    FGuid Guid;
    if (!FGuid::ParseExact(Id, EGuidFormats::Digits, Guid))
        return 1;
    if (FParse::Param(*Params, TEXT("MontageOnly")))
    {
        FString Error;
        const FString Root = TEXT("/Game/DasMontage/")+ Id;
        if (!CreateDasMontageFixture(Root, Error))
        {
            UE_LOG(LogTemp, Error, TEXT("DAS_MONTAGE_EXPORT=FAIL %s"), *Error);
            return 1;
        }
        UE_LOG(LogTemp, Display, TEXT("DAS_MONTAGE_EXPORT=PASS map=%s/MontageMap"), *Root);
        return 0;
    }
    if (FParse::Param(*Params, TEXT("AIOnly")))
    {
        FString Error;
        const FString Root = TEXT("/Game/DasExamples/AI/")+ Id;
        if (!CreateDasStandalonePlayableDemo(Root, Error, true))
        {
            UE_LOG(LogTemp, Error, TEXT("DAS_AI_EXPORT=FAIL %s"), *Error);
            return 1;
        }
        UE_LOG(LogTemp, Display, TEXT("DAS_AI_EXPORT=PASS map=%s/PlayableMap"), *Root);
        return 0;
    }
    const bool PlayableOnly = FParse::Param(*Params, TEXT("PlayableOnly"));
    const FString Root = (PlayableOnly ? TEXT("/Game/DasExamples/Playable/"): TEXT("/Game/DasCookedGameplay/")) + Id;
    FString Error;
    if (PlayableOnly)
    {
        if (!CreateDasStandalonePlayableDemo(Root, Error))
        {
            UE_LOG(LogTemp, Error, TEXT("DAS_PLAYABLE_EXAMPLE_EXPORT=FAIL %s"), *Error);
            return 1;
        }
        UE_LOG(LogTemp, Display, TEXT("DAS_PLAYABLE_EXAMPLE_EXPORT=PASS map=%s/PlayableMap"), *Root);
        return 0;
    }
    UBlueprint *Gameplay = nullptr, *Native = nullptr, *Delegate = nullptr, *Single = nullptr, *Inherited = nullptr,
               *Life = nullptr, *TickOnly = nullptr, *Helper = nullptr, *Standalone = nullptr;
    if (!FDasDSLService::Apply(TEXT("dsl_gameplay_runtime_probe.das"),
            Root + TEXT("/BP_Gameplay"), TEXT("gameplayprobe.GameplayRuntimeProbe"), Gameplay, Error) ||
                        !FDasDSLService::Apply(TEXT("dsl_cooked_native_owner.das"),
                            Root + TEXT("/BP_NativeOwner"), TEXT("cookednative.CookedNativeOwner"), Native, Error) ||
                                        !FDasDSLService::Apply(TEXT("dsl_delegate_actor.das"),
                                            Root + TEXT("/BP_Delegate"),
                                                        TEXT("delegateexample.DelegateExample"), Delegate, Error) ||
                                                        !FDasDSLService::Apply(
                                                            TEXT("dsl_single_delegate_actor.das"),
                                                                 Root + TEXT("/BP_SingleDelegate"),
                                                                             TEXT("singledelegate.SingleDelegateActor"),
                                                                                  Single, Error))
    {
        UE_LOG(LogTemp, Error, TEXT("Cooked fixtures: %s"), *Error);
        return 1;
    }
    if (!FDasDSLService::Apply(TEXT("dsl_inheritance_child.das"),
                                    Root + TEXT("/BP_ChildCounter"), TEXT("inheritance.ChildCounter"), Inherited,
                                                                          Error))
    {
        UE_LOG(LogTemp, Error, TEXT("Cooked inheritance: %s"), *Error);
        return 1;
    }
    if (!FDasDSLService::Apply(TEXT("dsl_life_actor_child.das"),
                                    Root + TEXT("/BP_ChildLifeActor"), TEXT("lifefamily.ChildLifeActor"), Life, Error))
    {
        UE_LOG(LogTemp, Error, TEXT("Cooked lifecycle: %s"), *Error);
        return 1;
    }
    if (!FDasDSLService::Apply(TEXT("dsl_life_tick_child.das"),
                                    Root + TEXT("/BP_TickOnlyChild"), TEXT("lifetick.TickOnlyChild"), TickOnly, Error))
    {
        UE_LOG(LogTemp, Error, TEXT("Cooked tick-only: %s"), *Error);
        return 1;
    }
    if (!FDasDSLService::Apply(TEXT("dsl_inheritance_grandchild.das"),
                                    Root + TEXT("/BP_GrandCounter"), TEXT("inheritance.GrandCounter"), Helper, Error))
    {
        UE_LOG(LogTemp, Error, TEXT("Cooked helpers: %s"), *Error);
        return 1;
    }
    if (!FDasDSLService::Apply(TEXT("dsl_construction_component_leaf.das"),
                                    Root + TEXT("/BP_LeafConstruction"), TEXT("construction.LeafConstruction"),
                                                                              Standalone, Error))
    {
        UE_LOG(LogTemp, Error, TEXT("Cooked component construction: %s"), *Error);
        return 1;
    }
    auto Save = [](UObject* O)
    {
        const FString File = FPackageName::LongPackageNameToFilename(
            O->GetOutermost()->GetName(),
            O->IsA<UWorld>() ? FPackageName::GetMapPackageExtension() : FPackageName::GetAssetPackageExtension());
        IFileManager::Get().MakeDirectory(*FPaths::GetPath(File), true);
        FSavePackageArgs Args;
        Args.TopLevelFlags = RF_Public | RF_Standalone;
        return UPackage::SavePackage(O->GetOutermost(), O, *File, Args);
    };
    auto* Pure = CreateDasPureConsumer(Helper->GeneratedClass, CreatePackage(*(Root + TEXT("/BP_PureConsumer"))),
                                                                             TEXT("BP_PureConsumer"), Error);
    if (!Pure || !Save(Pure))
    {
        UE_LOG(LogTemp, Error, TEXT("Pure consumer export: %s"), *Error);
        return 1;
    }
    // Save generated component classes too; the map retains their cook dependencies.
    for (const auto* Name :
         {TEXT("BP_Gameplay"),
               TEXT("BP_NativeOwner"),
                    TEXT("BP_Delegate"),
                         TEXT("BP_CookedBehavior"),
                              TEXT("BP_SingleDelegate"),
                                   TEXT("BP_ChildCounter"),
                                        TEXT("BP_BaseCounter"),
                                             TEXT("BP_BaseLifeActor"),
                                                  TEXT("BP_ChildLifeActor"),
                                                       TEXT("BP_BaseBehavior"),
                                                            TEXT("BP_ChildBehavior"),
                                                                 TEXT("BP_TickOnlyBase"),
                                                                      TEXT("BP_TickOnlyChild"),
                                                                           TEXT("BP_GrandCounter"),
                                                                                TEXT("BP_StandaloneConstruction"),
                                                                                     TEXT("BP_LeafConstruction")})
        if (auto* BP = FindObject<UBlueprint>(nullptr, *(Root + TEXT("/")+ Name + TEXT(".")+ Name)))
            if (!Save(BP))
                return 1;
    UBlueprint* Library = nullptr;
    if (!FDasDSLService::Apply(TEXT("dsl_math_library.das"),
                                    Root + TEXT("/BP_GameMath"), TEXT("dsl_math_library.GameMath"), Library, Error) ||
                                                !Save(Library))
    {
        UE_LOG(LogTemp, Error, TEXT("Library export: %s"), *Error);
        return 1;
    }
    if (!PrepareDasLibraryTestTypes(Error))
    {
        UE_LOG(LogTemp, Error, TEXT("%s"), *Error);
        return 1;
    }
    UBlueprint* Values = nullptr;
    if (!FDasDSLService::Apply(TEXT("dsl_library_values.das"),
                                    Root + TEXT("/BP_GameValues"),
                                                TEXT("dsl_library_values.GameValues"), Values, Error) || !Save(Values))
    {
        UE_LOG(LogTemp, Error, TEXT("Values export: %s"), *Error);
        return 1;
    }
    auto* LibraryConsumer =
        CreateDasPureConsumer(Library->GeneratedClass, CreatePackage(*(Root + TEXT("/BP_LibraryConsumer"))),
                                                                     TEXT("BP_LibraryConsumer"), Error);
    if (!LibraryConsumer || !Save(LibraryConsumer))
        return 1;
    auto* DefaultConsumer = CreateDasPureConsumer(
        Library->GeneratedClass, CreatePackage(*(Root + TEXT("/BP_DefaultConsumer"))),
                                               TEXT("BP_DefaultConsumer"), Error, TEXT("DefaultAdd"), true);
    if (!DefaultConsumer || !Save(DefaultConsumer))
    {
        UE_LOG(LogTemp, Error, TEXT("Default consumer: %s"), *Error);
        return 1;
    }
    auto* WorldConsumer = CreateDasPureConsumer(
        Values->GeneratedClass, CreatePackage(*(Root + TEXT("/BP_WorldConsumer"))), TEXT("BP_WorldConsumer"), Error);
    if (!WorldConsumer || !Save(WorldConsumer))
    {
        UE_LOG(LogTemp, Error, TEXT("World consumer: %s"), *Error);
        return 1;
    }
    auto* World = UWorld::CreateWorld(EWorldType::Editor, false, TEXT("Map"), CreatePackage(*(Root + TEXT("/Map"))));
    World->SetFlags(RF_Public | RF_Standalone);
    World->GetOutermost()->SetPackageFlags(PKG_ContainsMap);
    auto* LibraryCarrier = World->SpawnActor<ADasStaticLibraryProbe>();
    LibraryCarrier->LibraryClass = Library->GeneratedClass;
    LibraryCarrier->ValuesClass = Values->GeneratedClass;
    TArray<UBlueprint*> DefaultAssets;
    if (!PrepareDasInstanceDefaultGameFixtures(Root, LibraryCarrier, DefaultAssets, Error, true))
    {
        UE_LOG(LogTemp, Error, TEXT("Instance defaults export: %s"), *Error);
        World->DestroyWorld(false);
        return 1;
    }
    if (!PrepareDasPawnCharacterFixtures(Root, LibraryCarrier, DefaultAssets, Error, true))
    {
        UE_LOG(LogTemp, Error, TEXT("Pawn/Character export: %s"), *Error);
        World->DestroyWorld(false);
        return 1;
    }
    auto Spawn = [&](UBlueprint* BP, const TCHAR* Name)
    {
        FActorSpawnParameters P;
        P.Name = Name;
        auto* A = World->SpawnActor<ADasProbeActor>(BP->GeneratedClass, FTransform::Identity, P);
        if (A)
            A->Tags.Add(Name);
        return A;
    };
    auto* NativeConstructionOwner = World->SpawnActor<AActor>();
    NativeConstructionOwner->Tags.Add(TEXT("StandaloneConstructionOwner"));
    auto* NativeConstruction = NewObject<UDasScriptComponent>(NativeConstructionOwner, Standalone->GeneratedClass,
                                                              TEXT("StandaloneConstruction"), RF_Transactional);
    NativeConstructionOwner->AddInstanceComponent(NativeConstruction);
    NativeConstruction->RegisterComponent();
    // Save a deliberate difference: loading must retain the result, not execute construction again.
    FindFProperty<FIntProperty>(NativeConstruction->GetClass(),
                                TEXT("Score"))->SetPropertyValue_InContainer(NativeConstruction, 8);
    const bool Good =
        Spawn(Gameplay, TEXT("GameplayFirst")) &&
              Spawn(Gameplay, TEXT("GameplaySecond")) &&
                    Spawn(Native, TEXT("NativeOwner")) &&
                          Spawn(Delegate, TEXT("DelegateOwner")) &&
                                Spawn(Single, TEXT("SingleOwner")) &&
                                      Spawn(Inherited, TEXT("InheritedOwner")) &&
                                            Spawn(Life, TEXT("LifeOwner")) &&
                                                  Spawn(TickOnly, TEXT("TickOnlyOwner")) &&
                                                        Spawn(Helper, TEXT("HelperOwner")) &&
                                                              Spawn(Pure, TEXT("PureOwner")) &&
                                                                    Spawn(LibraryConsumer, TEXT("LibraryConsumer")) &&
                                                                          Spawn(WorldConsumer,
                                                                                TEXT("WorldLibraryConsumer")) &&
                                                                                Spawn(DefaultConsumer,
                                                                                      TEXT("DefaultLibraryConsumer")) &&
                                                                                      Save(World);
    if (!SaveDasPlayableDemo(Root, LibraryCarrier->PlayableCharacterClass, Error))
    {
        UE_LOG(LogTemp, Error, TEXT("Playable map: %s"), *Error);
        World->DestroyWorld(false);
        return 1;
    }
    World->DestroyWorld(false);
    if (!Good)
        return 1;
    UE_LOG(LogTemp, Display, TEXT("DAS_COOKED_GAMEPLAY_EXPORT=PASS map=%s/Map"), *Root);
    return 0;
}
