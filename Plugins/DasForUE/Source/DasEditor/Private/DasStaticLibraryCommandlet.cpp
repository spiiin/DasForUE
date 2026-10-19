#include "DasStaticLibraryCommandlet.h"
#include "DasLibraryTestTypes.h"
#include "DasPureFunctionTestGraph.h"
#include "K2Node_CallFunction.h"
#include "EdGraphSchema_K2.h"
#include "K2Node_FunctionEntry.h"
#include "DasBlueprintLibrary.h"
#include "DasProbeActor.h"
#include "Engine/World.h"
#include "Engine/BlueprintGeneratedClass.h"
#include "Misc/ScopeExit.h"
#include "HAL/FileManager.h"
#include "DasStaticLibraryProbe.h"
#include "DasDSLService.h"
#include "DasRuntime.h"
#include "DasAuthoring.h"
#include "Engine/Blueprint.h"
#include "Kismet2/KismetEditorUtilities.h"
#include "Misc/PackageName.h"
#include "Misc/FileHelper.h"
#include "UObject/SavePackage.h"
#include "UObject/Script.h"
#include "Modules/ModuleManager.h"
#include "DasInstanceDefaultsTest.inl"
UDasStaticLibraryCommandlet::UDasStaticLibraryCommandlet()
{
    IsClient = false;
    IsServer = false;
    IsEditor = true;
    LogToConsole = true;
}
int32 UDasStaticLibraryCommandlet::Main(const FString& Params)
{
    FString Id, Phase;
    FParse::Value(*Params, TEXT("ProbeId="), Id);
    FParse::Value(*Params, TEXT("Phase="), Phase);
    FGuid Guid;
    if (!FGuid::ParseExact(Id, EGuidFormats::Digits, Guid))
        return 1;
    auto& Runtime = FModuleManager::LoadModuleChecked<FDasRuntimeModule>(TEXT("DasRuntime"));
    FString Error;
    FDasAuthoringModule Model;
    if (!Runtime.ReadAuthoringScript(Runtime.ScriptPath(TEXT("dsl_math_library.das")), Model, Error))
    {
        UE_LOG(LogTemp, Error, TEXT("%s"), *Error);
        return 1;
    }
    if (!PrepareDasLibraryTestTypes(Error))
    {
        UE_LOG(LogTemp, Error, TEXT("%s"), *Error);
        return 1;
    }
    bool Good = true;
    auto Check = [&](bool V, const TCHAR* N)
    {
        Good &= V;
        UE_LOG(LogTemp, Display, TEXT("%s %s %s"), V ? TEXT("PASS"): TEXT("FAIL"), N, *Error);
    };
    if (!TestDasInstanceDefaults(Id, Phase, Error))
    {
        UE_LOG(LogTemp, Error, TEXT("Instance defaults: %s"), *Error);
        return 1;
    }
    const FString Package = TEXT("/Game/DasStaticLibrary/")+ Id + TEXT("/BP_GameMath");
    UBlueprint* BP = nullptr;
    if (Phase == TEXT("Create"))
    {
        for (const auto* File :
             {TEXT("dsl_library_default_bad_ref.das"),
                   TEXT("dsl_library_default_bad_out.das"),
                        TEXT("dsl_library_default_bad_expression.das"),
                             TEXT("dsl_library_default_bad_order.das"),
                                  TEXT("dsl_library_default_bad_world.das"),
                                       TEXT("dsl_library_default_bad_type.das"),
                                            TEXT("dsl_library_world_bad_unknown.das"),
                                                 TEXT("dsl_library_world_bad_scalar.das"),
                                                      TEXT("dsl_library_world_bad_output.das"),
                                                           TEXT("dsl_library_bad_instance.das"),
                                                                TEXT("dsl_library_bad_fields.das"),
                                                                     TEXT("dsl_library_bad_world.das"),
                                                                          TEXT("dsl_library_bad_self.das"),
                                                                               TEXT("dsl_library_bad_pure_void.das")})
        {
            FDasAuthoringModule Bad;
            Check(!Runtime.ReadAuthoringScript(Runtime.ScriptPath(File), Bad, Error), File);
        }
        if (!FDasDSLService::Apply(TEXT("dsl_math_library.das"), Package, TEXT("dsl_math_library.GameMath"), BP, Error))
        {
            UE_LOG(LogTemp, Error, TEXT("%s"), *Error);
            return 1;
        }
        Check(BP->BlueprintType == BPTYPE_FunctionLibrary, TEXT("real_Blueprint_Function_Library"));
        UBlueprint* Same = nullptr;
        Check(FDasDSLService::Reimport(BP, Error), TEXT("library_reimport"));
        Check(FDasDSLService::Apply(TEXT("dsl_math_library.das"), Package,
                                         TEXT("dsl_math_library.GameMath"), Same, Error) && Same == BP,
                                    TEXT("library_idempotent_apply"));
        FSavePackageArgs Save;
        Save.TopLevelFlags = RF_Public | RF_Standalone;
        Check(UPackage::SavePackage(
                  BP->GetOutermost(), BP,
                  *FPackageName::LongPackageNameToFilename(Package, FPackageName::GetAssetPackageExtension()), Save),
              TEXT("library_saved"));
    }
    else
    {
        BP = LoadObject<UBlueprint>(nullptr, *(Package + TEXT(".BP_GameMath")));
        if (BP)
            FKismetEditorUtilities::CompileBlueprint(BP);
    }
    if (!BP)
        return 1;
    FEditorScriptExecutionGuard Guard;
    FString Report;
    Check(RunDasStaticLibraryProbe(BP->GeneratedClass, Report), TEXT("library_runtime_probe"));
    UE_LOG(LogTemp, Display, TEXT("%s"), *Report);
    UBlueprint* DefaultConsumer = nullptr;
    const FString DefaultPackage = Package + TEXT("Defaults");
    if (Phase == TEXT("Create"))
    {
        DefaultConsumer = CreateDasPureConsumer(BP->GeneratedClass, CreatePackage(*DefaultPackage),
                                                TEXT("BP_GameMathDefaults"), Error, TEXT("DefaultAdd"), true);
        if (!DefaultConsumer)
        {
            UE_LOG(LogTemp, Error, TEXT("%s"), *Error);
            return 1;
        }
        FSavePackageArgs Save;
        Save.TopLevelFlags = RF_Public | RF_Standalone;
        if (!UPackage::SavePackage(
                DefaultConsumer->GetOutermost(), DefaultConsumer,
                *FPackageName::LongPackageNameToFilename(DefaultPackage, FPackageName::GetAssetPackageExtension()),
                Save))
            return 1;
    }
    else
    {
        DefaultConsumer = LoadObject<UBlueprint>(nullptr, *(DefaultPackage + TEXT(".BP_GameMathDefaults")));
        if (DefaultConsumer)
            FKismetEditorUtilities::CompileBlueprint(DefaultConsumer);
    }
    if (!DefaultConsumer)
        return 1;
    auto* DefaultFunction = DefaultConsumer->GeneratedClass->FindFunctionByName(TEXT("EvaluatePure"));
    int32 DefaultResult = 0;
    DefaultConsumer->GeneratedClass->GetDefaultObject()->ProcessEvent(DefaultFunction, &DefaultResult);
    Check(DefaultResult == 5, TEXT("library_Blueprint_default_arguments_after_save_recompile"));
    for (const auto* Method :
         {TEXT("DefaultDouble"), TEXT("DefaultFloat"), TEXT("DefaultBool"), TEXT("DefaultText"), TEXT("DefaultEmpty")})
    {
        auto* Scratch = NewObject<UEdGraph>(BP);
        Scratch->Schema = UEdGraphSchema_K2::StaticClass();
        auto* Node = NewObject<UK2Node_CallFunction>(Scratch);
        Node->SetFromFunction(BP->GeneratedClass->FindFunctionByName(Method));
        Node->AllocateDefaultPins();
        const auto Value = Node->FindPinChecked(TEXT("value"))->DefaultValue;
        Check(FString(Method) ==
              TEXT("DefaultDouble")
                   ? FCString::Atod(*Value) == 1.23456789012345
                   : FString(Method) == TEXT("DefaultFloat")
                                             ? FCString::Atof(*Value) == -0.125f
                                             : FString(Method) == TEXT("DefaultBool")
                                                   ? Value == TEXT("false")
                                                   : FString(Method) == TEXT("DefaultText")
                                                             ? Value == TEXT("Пример 世界"): Value.IsEmpty(), Method);
    }
    UBlueprint* Values = nullptr;
    const FString ValuesPackage = Package + TEXT("Values");
    if (Phase == TEXT("Create"))
    {
        if (!FDasDSLService::Apply(TEXT("dsl_library_values.das"), ValuesPackage,
                                        TEXT("dsl_library_values.GameValues"), Values, Error))
        {
            UE_LOG(LogTemp, Error, TEXT("Values: %s"), *Error);
            return 1;
        }
        FSavePackageArgs Save;
        Save.TopLevelFlags = RF_Public | RF_Standalone;
        if (!UPackage::SavePackage(
                Values->GetOutermost(), Values,
                *FPackageName::LongPackageNameToFilename(ValuesPackage, FPackageName::GetAssetPackageExtension()),
                Save))
            return 1;
    }
    else
    {
        Values = LoadObject<UBlueprint>(nullptr, *(ValuesPackage + TEXT(".BP_GameMathValues")));
        if (Values)
            FKismetEditorUtilities::CompileBlueprint(Values);
    }
    if (!Values)
        return 1;
    for (int32 I = 0; I < 2; ++I)
    {
        auto* World = UWorld::CreateWorld(EWorldType::Game, false);
        auto* Context = World->SpawnActor<ADasStaticLibraryProbe>();
        FString ValuesReport;
        Check(RunDasLibraryValuesProbe(Values->GeneratedClass, Context, ValuesReport),
              TEXT("library_complex_values_and_world"));
        UE_LOG(LogTemp, Display, TEXT("%s"), *ValuesReport);
        World->DestroyWorld(false);
    }
    {
        auto* First = UWorld::CreateWorld(EWorldType::Game, false);
        auto* Second = UWorld::CreateWorld(EWorldType::Game, false);
        First->TimeSeconds = 17.;
        Second->TimeSeconds = 90.;
        auto* Caller = First->SpawnActor<ADasStaticLibraryProbe>();
        auto* Other = Second->SpawnActor<ADasStaticLibraryProbe>();
        FDasScalarValue Target;
        Target.Kind = 8;
        Target.ObjectClass = TEXT("/Script/CoreUObject.Object");
        Target.ObjectValue = Other;
        auto Cross = Runtime.RunNamedFramePersistent(Runtime.ScriptPath(
            TEXT("dsl_library_values_calls.das")), TEXT("cross_world"), {Target}, EDasMethodSignature::Double, Caller);
        Check(Cross.bSucceeded && Cross.DoubleValue == 91., TEXT("library_direct_DAS_uses_explicit_other_world"));
        if (!Cross.bSucceeded)
            UE_LOG(LogTemp, Error, TEXT("%s"), *Cross.Error);
        Target.ObjectValue = Caller;
        Cross = Runtime.RunNamedFramePersistent(Runtime.ScriptPath(
            TEXT("dsl_library_values_calls.das")), TEXT("cross_world"), {Target}, EDasMethodSignature::Double, Other);
        Check(Cross.bSucceeded && Cross.DoubleValue == 18., TEXT("library_direct_DAS_restores_caller_world"));
        FDasAuthoringModule ValuesModel;
        FString Contract;
        if (!Runtime.ReadAuthoringScript(Runtime.ScriptPath(TEXT("dsl_library_values.das")), ValuesModel, Error) ||
                                         !ValuesModel.ToJson(Contract, Error))
            return 1;
        const auto* Time =
            ValuesModel.Classes[0].Methods.FindByPredicate([](const auto& M) { return M.Name == TEXT("Time"); });
        if (!Time)
            return 1;
        FDasScalarValue Missing;
        Missing.Kind = 8;
        Missing.ObjectClass = TEXT("/Script/CoreUObject.Object");
        auto Rejected = Runtime.RunLibraryFrame(Contract, TEXT("dsl_library_values.das"), Time->EntryPoint, {Missing},
                                                               EDasMethodSignature::Double, TEXT(""));
        Check(!Rejected.bSucceeded && Rejected.OutputValues.IsEmpty(), TEXT("library_null_WorldContext_rejected"));
        Other->Destroy();
        Missing.ObjectValue = Other;
        Rejected = Runtime.RunLibraryFrame(Contract, TEXT("dsl_library_values.das"), Time->EntryPoint, {Missing},
                                                          EDasMethodSignature::Double, TEXT(""));
        Check(!Rejected.bSucceeded, TEXT("library_destroyed_WorldContext_rejected"));
        Runtime.ReleaseObject(Caller);
        Runtime.ReleaseObject(Other);
        First->DestroyWorld(false);
        Second->DestroyWorld(false);
    }
    {
        UBlueprint* SpawnedBP = nullptr;
        if (!FDasDSLService::Apply(TEXT("dsl_library_spawned.das"),
                                        Package + TEXT("Spawned"), TEXT("dsl_library_spawned.LibrarySpawned"),
                                                                        SpawnedBP, Error))
        {
            UE_LOG(LogTemp, Error, TEXT("%s"), *Error);
            return 1;
        }
        FDasAuthoringModule ValuesModel;
        FString Contract;
        if (!Runtime.ReadAuthoringScript(Runtime.ScriptPath(TEXT("dsl_library_values.das")), ValuesModel, Error) ||
                                         !ValuesModel.ToJson(Contract, Error))
            return 1;
        const auto* Spawn =
            ValuesModel.Classes[0].Methods.FindByPredicate([](const auto& M) { return M.Name == TEXT("Spawn"); });
        if (!Spawn)
            return 1;
        auto* World = UWorld::CreateWorld(EWorldType::Game, false);
        auto* Owner = World->SpawnActor<ADasStaticLibraryProbe>();
        FDasScalarValue Context;
        Context.Kind = 8;
        Context.ObjectClass = TEXT("/Script/CoreUObject.Object");
        Context.ObjectValue = Owner;
        FDasScalarValue Class;
        Class.Kind = 15;
        Class.ObjectClass = TEXT("/Script/Engine.Actor");
        Class.ClassValue = SpawnedBP->GeneratedClass;
        const auto Result =
            Runtime.RunLibraryFrame(Contract, TEXT("dsl_library_values.das"), Spawn->EntryPoint, {Context, Class},
                                                   EDasMethodSignature::Object, TEXT("/Script/Engine.Actor"));
        auto* Actor = Cast<ADasProbeActor>(Result.ObjectValue.Get());
        Check(Result.bSucceeded && Actor && Actor->LastDasError.IsEmpty() &&
              FindFProperty<FIntProperty>(Actor->GetClass(), TEXT("Score"))->GetPropertyValue_InContainer(Actor) == 42,
                                          TEXT("library_nested_authored_actor_construction_uses_own_contract"));
        if (!Result.bSucceeded)
            UE_LOG(LogTemp, Error, TEXT("%s"), *Result.Error);
        if (Actor)
        {
            Runtime.ReleaseObject(Actor);
            Actor->Destroy();
        }
        Runtime.ReleaseObject(Owner);
        World->DestroyWorld(false);
    }
    const auto* Fail = Model.Classes[0].Methods.FindByPredicate([](const auto& M) { return M.Name == TEXT("Fail"); });
    FString Wire;
    Model.ToJson(Wire, Error);
    FDasScalarValue In;
    In.Kind = 0;
    In.IntValue = 10;
    In.Direction = 2;
    FDasScalarValue Out;
    Out.Kind = 0;
    Out.Direction = 1;
    const auto Failed = Runtime.RunLibraryFrame(Wire, TEXT("dsl_math_library.das"), Fail->EntryPoint, {In, Out},
                                                           EDasMethodSignature::Void, TEXT(""));
    Check(!Failed.bSucceeded && Failed.OutputValues.IsEmpty() &&
          Failed.Error.Contains(TEXT("library_expected_failure")), TEXT("library_failed_outputs_not_published"));

    if (Phase == TEXT("Create"))
    {
        const FString TestFile =
                          TEXT("dsl_library_reload_")+ Id + TEXT(".das"), TestPath = Runtime.ScriptPath(TestFile);
        FString Source;
        ON_SCOPE_EXIT
        {
            Runtime.ReleaseObject(GetMutableDefault<UDasBlueprintLibrary>());
            IFileManager::Get().Delete(*TestPath);
        };
        if (!FFileHelper::LoadFileToString(Source, *Runtime.ScriptPath(TEXT("dsl_math_library.das"))) ||
                                           !FFileHelper::SaveStringToFile(
                                               Source, *TestPath, FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM))
            return 1;
        FDasAuthoringModule ReloadModel;
        if (!Runtime.ReadAuthoringScript(TestPath, ReloadModel, Error))
        {
            UE_LOG(LogTemp, Error, TEXT("Reload source: %s"), *Error);
            return 1;
        }
        FString Contract;
        if (!ReloadModel.ToJson(Contract, Error))
            return 1;
        const auto* Add =
            ReloadModel.Classes[0].Methods.FindByPredicate([](const auto& M) { return M.Name == TEXT("Add"); });
        if (!Add)
            return 1;
        FDasScalarValue Left;
        Left.Kind = 0;
        Left.IntValue = 7;
        FDasScalarValue Right;
        Right.Kind = 0;
        Right.IntValue = 5;
        auto Invoke = [&]()
        {
            return Runtime.RunLibraryFrame(Contract, TestFile, Add->EntryPoint, {Left, Right}, EDasMethodSignature::Int,
                                           TEXT(""));
        };
        auto Result = Invoke();
        Check(Result.bSucceeded && Result.Value == 12, TEXT("library_reload_initial"));
        UBlueprint* ReloadBP = nullptr;
        const FString ReloadId = FPaths::GetBaseFilename(TestFile) + TEXT(".GameMath");
        if (!FDasDSLService::Apply(TestFile, Package + TEXT("Reload"), ReloadId, ReloadBP, Error))
            return 1;
        auto* OldConsumer = CreateDasPureConsumer(
            ReloadBP->GeneratedClass, CreatePackage(*(Package + TEXT("OldDefaultConsumer"))),
                                                    TEXT("OldDefaultConsumer"), Error, TEXT("DefaultAdd"), true);
        if (!OldConsumer)
            return 1;
        UK2Node_CallFunction* OldCall = nullptr;
        for (UEdGraph* Graph : OldConsumer->FunctionGraphs)
            for (UEdGraphNode* Node : Graph->Nodes)
                if (auto* Call = Cast<UK2Node_CallFunction>(Node))
                    if (Call->FunctionReference.GetMemberName() == TEXT("DefaultAdd"))
                        OldCall = Call;
        if (!OldCall)
            return 1;
        GetDefault<UEdGraphSchema_K2>()->TrySetDefaultValue(*OldCall->FindPinChecked(TEXT("right")), TEXT("20"));
        FKismetEditorUtilities::CompileBlueprint(OldConsumer);
        const FString NewDefault = Source.Replace(TEXT("right:int=-2"), TEXT("right:int=-4"));
        if (!FFileHelper::SaveStringToFile(NewDefault, *TestPath, FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM) ||
            !FDasDSLService::Reimport(ReloadBP, Error))
            return 1;
        Check(OldCall->FindPinChecked(TEXT("right"))->DefaultValue ==
                                      TEXT("20"), TEXT("library_default_reimport_preserves_explicit_pin"));
        FKismetEditorUtilities::CompileBlueprint(OldConsumer);
        int32 Preserved = 0;
        OldConsumer->GeneratedClass->GetDefaultObject()->ProcessEvent(
            OldConsumer->GeneratedClass->FindFunctionByName(TEXT("EvaluatePure")), &Preserved);
        Check(Preserved == 27, TEXT("library_default_reimport_keeps_explicit_call_result"));
        auto* Scratch = NewObject<UEdGraph>(ReloadBP);
        Scratch->Schema = UEdGraphSchema_K2::StaticClass();
        auto* Fresh = NewObject<UK2Node_CallFunction>(Scratch);
        Fresh->SetFromFunction(ReloadBP->GeneratedClass->FindFunctionByName(TEXT("DefaultAdd")));
        Fresh->AllocateDefaultPins();
        Check(Fresh->FindPinChecked(TEXT("right"))->DefaultValue ==
                                    TEXT("-4"), TEXT("library_default_reimport_updates_new_nodes"));
        const FString Removed = NewDefault.Replace(TEXT("left:int=7; right:int=-4"), TEXT("left:int; right:int"));
        if (!FFileHelper::SaveStringToFile(Removed, *TestPath, FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM) ||
            !FDasDSLService::Reimport(ReloadBP, Error))
            return 1;
        auto* RemovedScratch = NewObject<UEdGraph>(ReloadBP);
        RemovedScratch->Schema = UEdGraphSchema_K2::StaticClass();
        auto* NoDefault = NewObject<UK2Node_CallFunction>(RemovedScratch);
        NoDefault->SetFromFunction(ReloadBP->GeneratedClass->FindFunctionByName(TEXT("DefaultAdd")));
        NoDefault->AllocateDefaultPins();
        Check(NoDefault->FindPinChecked(
            TEXT("left"))->DefaultValue ==
            TEXT("0") && NoDefault->FindPinChecked(TEXT("right"))->DefaultValue ==
                                                   TEXT("0"), TEXT("library_removed_defaults_clear_old_pin_metadata"));
        if (!FFileHelper::SaveStringToFile(Source, *TestPath, FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM))
            return 1;
        const FString Changed = Source.Replace(TEXT("return left+right"), TEXT("return left+right+1"));
        if (!FFileHelper::SaveStringToFile(Changed, *TestPath, FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM))
            return 1;
        Result = Invoke();
        Check(Result.bSucceeded && Result.Value == 13 && Result.ReloadError.IsEmpty(),
              TEXT("library_body_reload_without_reimport"));
        if (!FFileHelper::SaveStringToFile(Changed + TEXT("\n invalid ! syntax \n"), *TestPath,
                                                          FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM))
            return 1;
        Result = Invoke();
        Check(Result.bSucceeded && Result.Value == 13 && !Result.ReloadError.IsEmpty(),
              TEXT("library_invalid_source_keeps_working_revision"));
        const FString Structural =
            Changed.Replace(TEXT("Add(left:int; right:int):int"), TEXT("Add(left:int; right:int; extra:int):int"));
        if (!FFileHelper::SaveStringToFile(Structural, *TestPath, FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM))
            return 1;
        Result = Invoke();
        Check(Result.bSucceeded && Result.Value == 13 && !Result.ReloadError.IsEmpty(),
              TEXT("library_changed_contract_keeps_working_revision"));
    }
    UE_LOG(LogTemp, Display, TEXT("DAS_STATIC_LIBRARY_RESULT=%s"), Good ? TEXT("PASS"): TEXT("FAIL"));
    return Good ? 0 : 1;
}
