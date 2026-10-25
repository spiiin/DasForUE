#include "DasScriptComponent.h"
// Run alongside library defaults so both collectors exercise the shared literal rules.
bool TestDasInstanceDefaults(const FString& Id, const FString& Phase, FString& Error)
{
    FEditorScriptExecutionGuard ExecutionGuard;
    auto& Runtime = FModuleManager::LoadModuleChecked<FDasRuntimeModule>(TEXT("DasRuntime"));
    auto Fail = [&](const FString& Why)
    {
        Error = Why + TEXT(": ")+ Error;
        return false;
    };
    if (Phase == TEXT("Create"))
        for (const auto* File :
             {TEXT("dsl_instance_default_bad_ref.das"), TEXT("dsl_instance_default_bad_out.das"),
                                                             TEXT("dsl_instance_default_bad_expression.das"),
                                                                  TEXT("dsl_instance_default_bad_order.das"),
                                                                       TEXT("dsl_instance_default_bad_type.das")})
        {
            FDasAuthoringModule Model;
            if (Runtime.ReadAuthoringScript(Runtime.ScriptPath(File), Model, Error))
                return Fail(FString(TEXT("Accepted invalid default ")) + File);
            UE_LOG(LogTemp, Display, TEXT("PASS %s"), File);
            Error.Reset();
        }
    auto* World = UWorld::CreateWorld(EWorldType::Game, false);
    ON_SCOPE_EXIT
    {
        World->DestroyWorld(false);
    };
    auto* Owner = World->SpawnActor<ADasActor>();
    for (const auto* Kind : {TEXT("actor"), TEXT("component")})
    {
        const FString File = FString(TEXT("dsl_defaults_")) + Kind +
                                     TEXT(".das"), Name = FString(Kind) == TEXT("actor")? TEXT("DefaultActor")
                                                                                        : TEXT("DefaultComponent");
        const FString Package = TEXT("/Game/DasInstanceDefaults/")+ Id + TEXT("/BP_")+ Name;
        UBlueprint* BP = nullptr;
        UBlueprint* Consumer = nullptr;
        if (Phase == TEXT("Create"))
        {
            if (!FDasDSLService::Apply(File, Package, FPaths::GetBaseFilename(File) + TEXT(".")+ Name, BP, Error))
                return Fail(File);
            if (!FDasDSLService::Reimport(BP, Error))
                return Fail(TEXT("Instance reimport"));
            Consumer =
                CreateDasPureConsumer(BP->GeneratedClass, CreatePackage(*(Package + TEXT("Consumer"))),
                                                                        *FString(TEXT("BP_")+ Name + TEXT("Consumer")),
                                                                                 Error, TEXT("DefaultAdd"), true);
            if (!Consumer)
                return Fail(TEXT("Instance consumer"));
            for (auto* Asset : {BP, Consumer})
            {
                FSavePackageArgs Save;
                Save.TopLevelFlags = RF_Public | RF_Standalone;
                if (!UPackage::SavePackage(
                        Asset->GetOutermost(), Asset,
                        *FPackageName::LongPackageNameToFilename(Asset->GetOutermost()->GetName(),
                                                                 FPackageName::GetAssetPackageExtension()),
                        Save))
                    return Fail(TEXT("Save instance defaults"));
            }
        }
        else
        {
            BP = LoadObject<UBlueprint>(nullptr, *(Package + TEXT(".BP_")+ Name));
            Consumer = LoadObject<UBlueprint>(nullptr, *(Package + TEXT("Consumer.BP_")+ Name + TEXT("Consumer")));
            if (!BP || !Consumer)
                return Fail(TEXT("Reopen instance defaults"));
            FKismetEditorUtilities::CompileBlueprint(BP);
            FKismetEditorUtilities::CompileBlueprint(Consumer);
        }
        UObject* Target = nullptr;
        if (FString(Kind) == TEXT("actor"))
            Target = World->SpawnActor<ADasActor>(Consumer->GeneratedClass);
        else
        {
            auto* Component = NewObject<UDasScriptComponent>(Owner, Consumer->GeneratedClass);
            Owner->AddInstanceComponent(Component);
            Component->RegisterComponent();
            Target = Component;
        }
        if (!Target)
            return Fail(TEXT("Create instance"));
        ON_SCOPE_EXIT
        {
            Runtime.ReleaseObject(Target);
        };
        int32 Result = 0;
        Target->ProcessEvent(Target->FindFunction(TEXT("CheckDefaults")), &Result);
        if (Result != 1)
            return Fail(FString::Printf(TEXT("Self %s defaults result %d"), Kind, Result));
        Result = 0;
        Target->ProcessEvent(Target->FindFunction(TEXT("EvaluatePure")), &Result);
        if (Result != 5)
            return Fail(TEXT("Blueprint default pins"));
        const auto Direct = Runtime.RunNamedFramePersistent(
            Runtime.ScriptPath(FString(TEXT("dsl_defaults_")) + Kind + TEXT("_calls.das")),
                               TEXT("main"), {}, EDasMethodSignature::Int, Target);
        if (!Direct.bSucceeded || Direct.Value != 1)
        {
            Error = Direct.Error;
            return Fail(FString::Printf(TEXT("Reference %s defaults result %d"), Kind, Direct.Value));
        }
        for (const auto* Method :
             {TEXT("DefaultDouble"), TEXT("DefaultFloat"), TEXT("DefaultBool"), TEXT("DefaultText"),
                                                                                     TEXT("DefaultEmpty")})
        {
            auto* Scratch = NewObject<UEdGraph>(BP);
            Scratch->Schema = UEdGraphSchema_K2::StaticClass();
            auto* Node = NewObject<UK2Node_CallFunction>(Scratch);
            Node->SetFromFunction(BP->GeneratedClass->FindFunctionByName(Method));
            Node->AllocateDefaultPins();
            const auto* Pin =
                Node->FindPin(TEXT("value")) ? Node->FindPin(TEXT("value")) : Node->FindPin(TEXT("Input"));
            if (!Pin)
                return Fail(TEXT("Missing instance scalar pin"));
            const auto Value = Pin->DefaultValue;
            const bool Good =
                FString(Method) == TEXT("DefaultDouble")
                                        ? FCString::Atod(*Value) == 1.23456789012345
                                        : FString(Method) == TEXT("DefaultFloat")
                                                                  ? FCString::Atof(*Value) == -0.125f
                                                                  : FString(Method) == TEXT("DefaultBool")
                                                                        ? Value == TEXT("false")
                                                                        : FString(Method) == TEXT("DefaultText")
                                                                                  ? Value == TEXT("Пример 世界")
                                                                                  : Value.IsEmpty();
            if (!Good)
                return Fail(TEXT("Instance default pin ")+ FString(Method));
        }
        UE_LOG(LogTemp, Display,
               TEXT("PASS instance_defaults_%s_%s_self_reference_Blueprint_saved_recompile"), Kind, *Phase);
    }
    if (Phase == TEXT("Create"))
    {
        const FString File = TEXT("dsl_instance_defaults_reload_")+ Id + TEXT(".das"), Path = Runtime.ScriptPath(File);
        FString Source;
        if (!FFileHelper::LoadFileToString(Source, *Runtime.ScriptPath(TEXT("dsl_defaults_actor.das"))))
            return Fail(TEXT("Read default reload fixture"));
        Source = Source.Replace(TEXT("module dsl_defaults_actor public"),
                                     *(TEXT("module ")+ FPaths::GetBaseFilename(File) + TEXT(" public")));
        ON_SCOPE_EXIT
        {
            IFileManager::Get().Delete(*Path);
        };
        if (!FFileHelper::SaveStringToFile(Source, *Path, FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM))
            return Fail(TEXT("Write default reload fixture"));
        const FString ReloadPackage = TEXT("/Game/DasInstanceDefaults/")+ Id + TEXT("/BP_ReloadDefaults");
        UBlueprint* Reload = nullptr;
        if (!FDasDSLService::Apply(File, ReloadPackage,
                                   FPaths::GetBaseFilename(File) + TEXT(".DefaultActor"), Reload, Error))
            return Fail(TEXT("Import default reload fixture"));
        auto* Caller = CreateDasPureConsumer(
            Reload->GeneratedClass, CreatePackage(*(ReloadPackage + TEXT("Caller"))),
                                                  TEXT("BP_ReloadDefaultsCaller"), Error, TEXT("DefaultAdd"), true);
        if (!Caller)
            return Fail(TEXT("Reload caller"));
        UK2Node_CallFunction* Call = nullptr;
        for (UEdGraph* Graph : Caller->FunctionGraphs)
            for (UEdGraphNode* Node : Graph->Nodes)
                if (auto* Candidate = Cast<UK2Node_CallFunction>(Node))
                    if (Candidate->FunctionReference.GetMemberName() == TEXT("DefaultAdd"))
                        Call = Candidate;
        if (!Call)
            return Fail(TEXT("Reload caller node"));
        GetDefault<UEdGraphSchema_K2>()->TrySetDefaultValue(*Call->FindPinChecked(TEXT("right")), TEXT("20"));
        FKismetEditorUtilities::CompileBlueprint(Caller);
        const FString Changed = Source.Replace(TEXT("right:int=-2"), TEXT("right:int=-4"));
        if (!FFileHelper::SaveStringToFile(Changed, *Path, FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM) ||
            !FDasDSLService::Reimport(Reload, Error))
            return Fail(TEXT("Reimport changed instance default"));
        FKismetEditorUtilities::CompileBlueprint(Caller);
        auto* Actor = World->SpawnActor<ADasActor>(Caller->GeneratedClass);
        if (!Actor)
            return Fail(TEXT("Reload caller spawn"));
        int32 Result = 0;
        Actor->ProcessEvent(Actor->FindFunction(TEXT("EvaluatePure")), &Result);
        Runtime.ReleaseObject(Actor);
        Actor->Destroy();
        if (Result != 27)
            return Fail(TEXT("Reimport changed an explicit caller argument"));
        auto CheckPin = [&](const FString& Expected)
        {
            auto* Scratch = NewObject<UEdGraph>(Reload);
            Scratch->Schema = UEdGraphSchema_K2::StaticClass();
            auto* Node = NewObject<UK2Node_CallFunction>(Scratch);
            Node->SetFromFunction(Reload->GeneratedClass->FindFunctionByName(TEXT("DefaultAdd")));
            Node->AllocateDefaultPins();
            return Node->FindPinChecked(TEXT("right"))->DefaultValue == Expected;
        };
        if (!CheckPin(TEXT("-4")))
            return Fail(TEXT("Reimport did not refresh instance default"));
        const FString Removed = Changed.Replace(
            TEXT("left:int=7; right:int=-4"),
                 TEXT("left:int; right:int"))
                     .Replace(TEXT("self.DefaultAdd()"),
                                   TEXT("self.DefaultAdd(7,-4)"))
                                       .Replace(TEXT("self.DefaultAdd(10)"), TEXT("self.DefaultAdd(10,-4)"));
        if (!FFileHelper::SaveStringToFile(Removed, *Path, FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM) ||
            !FDasDSLService::Reimport(Reload, Error) || !CheckPin(TEXT("0")))
            return Fail(TEXT("Removing instance defaults retained stale metadata"));
        UE_LOG(LogTemp, Display, TEXT("PASS instance_defaults_reimport_change_remove_preserves_explicit_caller"));
    }
    const FString ChildPackage = TEXT("/Game/DasInstanceDefaults/")+ Id + TEXT("/BP_DefaultChild");
    UBlueprint* Child = nullptr;
    if (Phase == TEXT("Create"))
    {
        if (!FDasDSLService::Apply(TEXT("dsl_defaults_child.das"), ChildPackage,
                                        TEXT("dsl_defaults_child.DefaultChild"), Child, Error))
            return Fail(TEXT("Default inheritance import"));
        FSavePackageArgs Save;
        Save.TopLevelFlags = RF_Public | RF_Standalone;
        if (!UPackage::SavePackage(
                Child->GetOutermost(), Child,
                *FPackageName::LongPackageNameToFilename(ChildPackage, FPackageName::GetAssetPackageExtension()), Save))
            return Fail(TEXT("Save default child"));
    }
    else
    {
        Child = LoadObject<UBlueprint>(nullptr, *(ChildPackage + TEXT(".BP_DefaultChild")));
        if (Child)
            FKismetEditorUtilities::CompileBlueprint(Child);
    }
    if (!Child)
        return Fail(TEXT("Load default child"));
    auto* Instance = World->SpawnActor<ADasActor>(Child->GeneratedClass);
    if (!Instance)
        return Fail(TEXT("Spawn default child"));
    ON_SCOPE_EXIT
    {
        Runtime.ReleaseObject(Instance);
    };
    const auto Inherited = Runtime.RunNamedFramePersistent(
        Runtime.ScriptPath(TEXT("dsl_defaults_child_calls.das")), TEXT("main"), {}, EDasMethodSignature::Int, Instance);
    if (!Inherited.bSucceeded || Inherited.Value != 1)
    {
        Error = Inherited.Error;
        return Fail(FString::Printf(TEXT("Inherited default dispatch result %d"), Inherited.Value));
    }
    UE_LOG(LogTemp, Display, TEXT("PASS instance_defaults_inherited_virtual_Super_%s"), *Phase);
    Runtime.ReleaseObject(Owner);
    return true;
}
