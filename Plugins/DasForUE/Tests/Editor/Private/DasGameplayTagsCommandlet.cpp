#include "DasGameplayTagsCommandlet.h"
#include "DasGameplayTags.h"
#include "DasDSLService.h"
#include "DasTypeService.h"
#include "DasRuntime.h"
#include "Engine/Engine.h"
#include "Editor.h"
#include "Engine/Blueprint.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "UObject/Package.h"
#include "UObject/UnrealType.h"
#include "UObject/StrongObjectPtr.h"
#include "UObject/GarbageCollection.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "Misc/FileHelper.h"
#include "Misc/ScopeExit.h"
#include "HAL/FileManager.h"
UDasGameplayTagsCommandlet::UDasGameplayTagsCommandlet()
{
    IsClient = false;
    IsServer = false;
    IsEditor = true;
    LogToConsole = true;
}
int32 UDasGameplayTagsCommandlet::Main(const FString& Params)
{
    auto& R = FModuleManager::LoadModuleChecked<FDasRuntimeModule>(TEXT("DasRuntime"));
    FString Error;
    int32 Checks = 0, Failures = 0;
    auto Check = [&](bool OK, const TCHAR* Name)
    {
        ++Checks;
        if (!OK)
            ++Failures;
        UE_LOG(LogTemp, Display, TEXT("DAS_GAMEPLAY_TAGS_%s %s"), OK ? TEXT("PASS"): TEXT("FAIL"), Name);
        return OK;
    };
    auto Sword = FGameplayTag::RequestGameplayTag(TEXT("Item.Weapon.Sword"), false);
    auto Potion = FGameplayTag::RequestGameplayTag(TEXT("Item.Consumable.Potion"), false);
    if (!Check(Sword.IsValid() && Potion.IsValid(), TEXT("project_dictionary_loaded")))
        return 1;
    const FString Tag = FGuid::NewGuid().ToString(EGuidFormats::Digits),
                  Folder = TEXT("/Game/Tests/DasGameplayTags/")+ Tag;
    TArray<UObject*> Types;
    if (!Check(FDasTypeService::Apply(TEXT("gameplaytag_item_types.das"), Folder + TEXT("/Types"), Types, Error) &&
                                           Types.Num() == 1,
                                      TEXT("authored_struct_native_gameplay_tag_fields")))
    {
        UE_LOG(LogTemp, Display, TEXT("%s"), *Error);
        return 1;
    }
    TArray<TStrongObjectPtr<UObject>> KeepTypes;
    for (auto* Type : Types)
        KeepTypes.Emplace(Type);
    // Tags are leaf values in authored map values too (type adapters, Docs/Dev/Design/TypeAdapters.md).
    bool TagMapValue = false;
    if (auto* Record = Cast<UStruct>(Types[0]))
        for (TFieldIterator<FMapProperty> It(Record); It; ++It)
            if (auto* Value = CastField<FStructProperty>(It->ValueProp))
                TagMapValue |= Value->Struct == FGameplayTag::StaticStruct();
    if (!Check(TagMapValue, TEXT("authored_struct_tag_map_values")))
        return 1;
    ON_SCOPE_EXIT
    {
        for (auto* Type : Types)
        {
            FAssetRegistryModule::AssetDeleted(Type);
            Type->GetOutermost()->SetDirtyFlag(false);
            Type->ClearFlags(RF_Public | RF_Standalone);
        }
    };
    UBlueprint* BP = nullptr;
    if (!Check(FDasDSLService::Apply(TEXT("gameplaytag_items.das"), Folder + TEXT("/BP_TaggedItem"), TEXT("gameplaytag_items.TaggedItem"), BP, Error),
                                          TEXT("authored_actor_tag_fields_and_method_signatures")))
    {
        UE_LOG(LogTemp, Display, TEXT("%s"), *Error);
        return 1;
    }
    TStrongObjectPtr<UBlueprint> KeepBP(BP);
    ON_SCOPE_EXIT
    {
        FAssetRegistryModule::AssetDeleted(BP);
        BP->GetOutermost()->SetDirtyFlag(false);
        BP->ClearFlags(RF_Public | RF_Standalone);
    };
    auto* Kind = FindFProperty<FStructProperty>(BP->GeneratedClass, TEXT("Kind"));
    auto* Tags = FindFProperty<FStructProperty>(BP->GeneratedClass, TEXT("ItemTags"));
    if (!Check(Kind && Kind->Struct == FGameplayTag::StaticStruct() && Tags &&
                   Tags->Struct == FGameplayTagContainer::StaticStruct(),
               TEXT("real_ue_gameplay_tag_properties_not_json_carriers")))
        return 1;
    FString Fixture, Source;
    if (!FParse::Value(*Params, TEXT("Fixture="), Fixture) || !FFileHelper::LoadFileToString(Source, *Fixture))
        return 2;
    const FString ProbePath = R.ScriptPath(TEXT("gameplaytags_runtime_probe_")+ Tag + TEXT(".das"));
    if (!FFileHelper::SaveStringToFile(Source, *ProbePath, FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM))
        return 2;
    ON_SCOPE_EXIT
    {
        IFileManager::Get().Delete(*ProbePath, false, true);
    };
    TStrongObjectPtr<UWorld> World(UWorld::CreateWorld(EWorldType::Game, false));
    GEngine->CreateNewWorldContext(EWorldType::Game).SetCurrentWorld(World.Get());
    TStrongObjectPtr<AActor> Actor(World->SpawnActor<AActor>(BP->GeneratedClass));
    if (!Actor.IsValid())
        return 1;
    ON_SCOPE_EXIT
    {
        R.ReleaseObject(Actor.Get());
        GEngine->DestroyWorldContext(World.Get());
        World->DestroyWorld(false);
    };
    FEditorScriptExecutionGuard ScriptGuard;
    auto Value = R.RunNamedPersistent(ProbePath, TEXT("exercise"), 0, Actor.Get());
    if (!Value.bSucceeded || Value.Value != 1)
        UE_LOG(LogTemp, Display, TEXT("GameplayTags VM value=%d: %s"), Value.Value, *Value.Error);
    Check(Value.bSucceeded && Value.Value == 1, TEXT("dictionary_matching_containers_copy_guards_and_reflected_calls"));
    Check(*Kind->ContainerPtrToValuePtr<FGameplayTag>(Actor.Get()) == Potion &&
              Tags->ContainerPtrToValuePtr<FGameplayTagContainer>(Actor.Get())->HasTagExact(Sword),
          TEXT("native_property_readback"));
    CollectGarbage(RF_NoFlags);
    Value = R.RunNamedPersistent(ProbePath, TEXT("after_gc"), 0, Actor.Get());
    Check(Value.bSucceeded && Value.Value == 1, TEXT("retained_values_survive_gc"));
    UE_LOG(LogTemp, Display,
           TEXT("DAS_GAMEPLAY_TAGS_RESULT=%s checks=%d"), Failures ? TEXT("FAIL"): TEXT("PASS"), Checks);
    return Failures ? 1 : 0;
}
