#include "DasMethodValuesCommandlet.h"
#include "DasContainerTypes.h"
#include "DasMethodValuesProbe.h"
#include "DasTypeService.h"
#include "DasTypeRegistry.h"
#include "DasDSLService.h"
#include "DasAuthoring.h"
#include "Engine/Blueprint.h"
#include "Misc/FileHelper.h"
#include "Misc/PackageName.h"
#include "Misc/Parse.h"
#include "UObject/SavePackage.h"
UDasMethodValuesCommandlet::UDasMethodValuesCommandlet()
{
    IsClient = false;
    IsServer = false;
    IsEditor = true;
    LogToConsole = true;
}
int32 UDasMethodValuesCommandlet::Main(const FString& Params)
{
    FString Id, Phase, Error;
    FParse::Value(*Params, TEXT("ProbeId="), Id);
    FParse::Value(*Params, TEXT("Phase="), Phase);
    FGuid Guid;
    if (!FGuid::ParseExact(Id, EGuidFormats::Digits, Guid))
        return 1;
    const FString Base = TEXT("/Game/DasMethodValues/")+ Id,
                              RegistryPath =
                                  Base + TEXT("/Registry.Registry"),
                                              Prefix = TEXT("dsl_registered_methodvalues_")+ Id,
                                                            TargetFile = Prefix + TEXT("_target.das"),
                                                                                       DriverFile =
                                                                                           Prefix + TEXT("_driver.das");
    auto& Runtime = FModuleManager::LoadModuleChecked<FDasRuntimeModule>(TEXT("DasRuntime"));
    UBlueprint* Target = nullptr;
    UBlueprint* Driver = nullptr;
    auto Save = [&](UObject* Object)
    {
        FSavePackageArgs Args;
        Args.TopLevelFlags = RF_Public | RF_Standalone;
        return UPackage::SavePackage(
            Object->GetOutermost(), Object,
            *FPackageName::LongPackageNameToFilename(Object->GetOutermost()->GetName(),
                                                     Object->IsA<UWorld>() ? FPackageName::GetMapPackageExtension()
                                                                           : FPackageName::GetAssetPackageExtension()),
            Args);
    };
    if (Phase == TEXT("Create"))
    {
        TArray<UObject*> Types;
        UDasTypeRegistry* Registry = nullptr;
        if (!FDasTypeService::Apply(TEXT("ue_method_values.das"), Base + TEXT("/Types"), Types, Error) ||
                                         !FDasTypeService::BuildRegistry(Types, Base + TEXT("/Registry"), Registry,
                                                                                            Error))
        {
            UE_LOG(LogTemp, Error, TEXT("%s"), *Error);
            return 1;
        }
        FString TargetSource, DriverSource;
        FFileHelper::LoadFileToString(TargetSource, *Runtime.ScriptPath(TEXT("dsl_method_value_target.das")));
        FFileHelper::LoadFileToString(DriverSource, *Runtime.ScriptPath(TEXT("dsl_method_value_driver.das")));
        const FString ReferenceModule = Prefix + TEXT("_refs");
        FString References;
        FFileHelper::LoadFileToString(References, *Runtime.ScriptPath(TEXT("ue_method_target_refs.das")));
        References = References.Replace(TEXT("METHOD_REFS_MODULE"), *ReferenceModule, ESearchCase::CaseSensitive)
                                            .Replace(TEXT("TARGET_SOURCE"), *TargetFile, ESearchCase::CaseSensitive)
                                                         .Replace(TEXT("DRIVER_SOURCE"), *DriverFile,
                                                                       ESearchCase::CaseSensitive);
        if (!FFileHelper::SaveStringToFile(References,
                                           *Runtime.ScriptPath(ReferenceModule + TEXT(".das")),
                                                               FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM))
            return 1;
        TargetSource =
            TargetSource.Replace(TEXT("REGISTRY"), *RegistryPath, ESearchCase::CaseSensitive)
                                     .Replace(TEXT("METHOD_REFS_MODULE"), *ReferenceModule, ESearchCase::CaseSensitive);
        DriverSource =
            DriverSource.Replace(TEXT("REGISTRY"), *RegistryPath, ESearchCase::CaseSensitive)
                                     .Replace(TEXT("METHOD_REFS_MODULE"), *ReferenceModule, ESearchCase::CaseSensitive);
        const FString KeyModule = Prefix + TEXT("_keys");
        FString KeySource;
        if (!FFileHelper::LoadFileToString(KeySource, *Runtime.ScriptPath(TEXT("ue_method_object_keys.das"))))
            return 1;
        KeySource =
            KeySource.Replace(TEXT("METHOD_KEYS_MODULE"), *KeyModule, ESearchCase::CaseSensitive)
                                  .Replace(TEXT("METHOD_REFS_MODULE"), *ReferenceModule, ESearchCase::CaseSensitive);
        DriverSource = DriverSource.Replace(TEXT("METHOD_KEYS_MODULE"), *KeyModule, ESearchCase::CaseSensitive);
        if (!FFileHelper::SaveStringToFile(KeySource,
                                           *Runtime.ScriptPath(KeyModule + TEXT(".das")),
                                                               FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM))
            return 1;
        if (!FFileHelper::SaveStringToFile(TargetSource, *Runtime.ScriptPath(TargetFile),
                                           FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM) ||
            !FFileHelper::SaveStringToFile(DriverSource, *Runtime.ScriptPath(DriverFile),
                                           FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM))
            return 1;
        if (!FDasDSLService::Apply(
                TargetFile, Base + TEXT("/BP_Target"), TEXT("methodvalues.ValueTarget"), Target, Error) ||
                                        !FDasDSLService::Apply(DriverFile, Base + TEXT("/BP_Driver"),
                                                                                       TEXT("methodvalues.ValueDriver"),
                                                                                            Driver, Error))
        {
            UE_LOG(LogTemp, Error, TEXT("%s"), *Error);
            return 1;
        }
        FDasAuthoringModule AuthoredModel, Roundtrip;
        FString AuthoredWire;
        if (!Runtime.ReadAuthoringScript(Runtime.ScriptPath(TargetFile), AuthoredModel, Error) ||
            !AuthoredModel.ToJson(AuthoredWire, Error) ||
            !FDasAuthoringModule::FromJson(AuthoredWire, Roundtrip, Error))
            return 1;
        const auto* Echo = Roundtrip.Classes[0].Methods.FindByPredicate(
            [](const auto& Method) { return Method.Name == TEXT("EchoAuthored"); });
        if (!Echo ||
            Echo->ReturnType.AuthoredId != TEXT("methodvalues.ValueTarget") ||
                                                Echo->Parameters[0].Type.AuthoredId != Echo->ReturnType.AuthoredId ||
                                                Echo->ReturnType.TypePath != TEXT("/Script/DasRuntime.DasProbeActor"))
            return 1;
        UE_LOG(LogTemp, Display, TEXT("PASS authored_signature_identity_roundtrip_and_host_storage"));
        // A valid call preflight and a wrong enum with the same numeric values must differ.
        FDasAuthoringModule Model;
        const FString Bad = Prefix + TEXT("_bad.das");
        FString BadSource = DriverSource.Replace(TEXT("api.EchoState(MethodState.Active)"),
                                                      TEXT("api.EchoState(OtherMethodState.Active)"));
        if (!FFileHelper::SaveStringToFile(BadSource, *Runtime.ScriptPath(Bad),
                                           FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM) ||
            Runtime.ReadAuthoringScript(Runtime.ScriptPath(Bad), Model, Error) || !Error.Contains(TEXT("error[")))
            return 1;
        UE_LOG(LogTemp, Display, TEXT("PASS natural_method_same_values_wrong_enum_compile_error"));
        BadSource = DriverSource.Replace(TEXT("api.EchoSettings(input)"), TEXT("api.EchoSettings(MethodPoint())"));
        if (!FFileHelper::SaveStringToFile(BadSource, *Runtime.ScriptPath(Bad),
                                           FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM) ||
            Runtime.ReadAuthoringScript(Runtime.ScriptPath(Bad), Model, Error) || !Error.Contains(TEXT("error[")))
            return 1;
        UE_LOG(LogTemp, Display, TEXT("PASS natural_method_wrong_struct_compile_error"));
        BadSource = DriverSource.Replace(TEXT("api.CopyInts(ints)"), TEXT("api.CopyInts(input.Items)"));
        if (!FFileHelper::SaveStringToFile(BadSource, *Runtime.ScriptPath(Bad),
                                           FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM) ||
            Runtime.ReadAuthoringScript(Runtime.ScriptPath(Bad), Model, Error) || !Error.Contains(TEXT("error[")))
            return 1;
        UE_LOG(LogTemp, Display, TEXT("PASS method_wrong_container_element_compile_error"));
        BadSource = DriverSource.Replace(TEXT("api.EchoActor(actor)"),
                                              TEXT("api.EchoActor(ue_ref(type<ObjectRef>,ue_self_object()))"));
        if (!FFileHelper::SaveStringToFile(BadSource, *Runtime.ScriptPath(Bad),
                                           FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM) ||
            Runtime.ReadAuthoringScript(Runtime.ScriptPath(Bad), Model, Error) || !Error.Contains(TEXT("error[")))
            return 1;
        UE_LOG(LogTemp, Display, TEXT("PASS method_wrong_object_compile_error"));
        BadSource = DriverSource.Replace(TEXT("api.EchoVector(ue_vector(1.0lf,2.0lf,3.0lf))"),
                                              TEXT("api.EchoVector(ue_rotator(1.0lf,2.0lf,3.0lf))"));
        if (!FFileHelper::SaveStringToFile(BadSource, *Runtime.ScriptPath(Bad),
                                           FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM) ||
            Runtime.ReadAuthoringScript(Runtime.ScriptPath(Bad), Model, Error) || !Error.Contains(TEXT("error[")))
            return 1;
        UE_LOG(LogTemp, Display, TEXT("PASS method_wrong_math_compile_error"));
        for (const auto& Change : TArray<TPair<FString, FString>>{
                 {TEXT("insert(self.ActorKeys,ue_key(value))"), TEXT("insert(self.ActorKeys,value)")},
                  {TEXT("insert(self.ActorKeys,ue_key(value))"),
                        TEXT("insert(self.ActorKeys,ue_key(type<ObjectKey>,value))")},
                   {TEXT("insert(self.ActorKeys,ue_key(value))"), TEXT("insert(self.LockedActorKeys,ue_key(value))")},
                    {TEXT("var objectSet=api.CopyObjectSet(api.ActorKeys)"),
                          TEXT("var objectSet=api.CopyObjectSet(api.ObjectScores)")},
                     {TEXT("ObjectScores:table<ActorKey;int>"), TEXT("ObjectScores:table<ActorRef;int>")}})
        {
            BadSource = DriverSource.Replace(*Change.Key, *Change.Value);
            if (BadSource == DriverSource ||
                !FFileHelper::SaveStringToFile(BadSource, *Runtime.ScriptPath(Bad),
                                               FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM) ||
                Runtime.ReadAuthoringScript(Runtime.ScriptPath(Bad), Model, Error))
            {
                UE_LOG(LogTemp, Error, TEXT("Object hash negative accepted: %s"), *Change.Value);
                return 1;
            }
        }
        UE_LOG(LogTemp, Display, TEXT("PASS object_hash_reference_key_constraint_readonly_signature_compile_errors"));
        for (const auto& Change : TArray<TPair<FString, FString>>{
                 {TEXT("self.EnumScores[MethodState.Idle]=7"), TEXT("self.EnumScores[7]=7")},
                  {TEXT("self.EnumScores[MethodState.Idle]=7"), TEXT("self.EnumScores[OtherMethodState.Idle]=7")},
                   {TEXT("self.EnumScores[MethodState.Idle]=7"), TEXT("self.LockedEnumScores[MethodState.Idle]=7")},
                    {TEXT("var enumMap=api.CopyEnumMap(api.EnumScores)"),
                          TEXT("var enumMap=api.CopyEnumMap(api.NativeEnumScores)")},
                     {TEXT("api.EnumScores := enumMap"), TEXT("api.LockedEnumScores := enumMap")},
                      {TEXT("EnumScores:table<MethodState;int>"), TEXT("EnumScores:table<MethodState;int> = {7=>7}")}})
        {
            BadSource = DriverSource.Replace(*Change.Key, *Change.Value);
            if (BadSource == DriverSource ||
                !FFileHelper::SaveStringToFile(BadSource, *Runtime.ScriptPath(Bad),
                                               FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM) ||
                Runtime.ReadAuthoringScript(Runtime.ScriptPath(Bad), Model, Error))
            {
                UE_LOG(LogTemp, Error, TEXT("Enum map negative accepted: %s"), *Change.Value);
                return 1;
            }
        }
        UE_LOG(LogTemp, Display, TEXT("PASS enum_map_integer_wrong_enum_readonly_signature_and_default_refused"));
        for (const auto& Change : TArray<TPair<FString, FString>>{
                 {TEXT("insert(self.EnumStates,MethodState.Idle)"),
                       TEXT("insert(self.EnumStates,OtherMethodState.Idle)")},
                  {TEXT("insert(self.EnumStates,MethodState.Idle)"), TEXT("insert(self.EnumStates,7)")},
                   {TEXT("insert(self.EnumStates,MethodState.Idle)"),
                         TEXT("insert(self.LockedEnumStates,MethodState.Idle)")},
                    {TEXT("var states=api.CopyEnumSet(api.EnumStates)"),
                          TEXT("var states=api.CopyEnumSet(api.NativeStates)")},
                     {TEXT("api.EnumStates := states"), TEXT("api.LockedEnumStates := states")},
                      {TEXT("EnumStates:table<MethodState>"), TEXT("EnumStates:table<MethodState> = {7}")}})
        {
            BadSource = DriverSource.Replace(*Change.Key, *Change.Value);
            if (BadSource == DriverSource ||
                !FFileHelper::SaveStringToFile(BadSource, *Runtime.ScriptPath(Bad),
                                               FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM) ||
                Runtime.ReadAuthoringScript(Runtime.ScriptPath(Bad), Model, Error))
            {
                UE_LOG(LogTemp, Error, TEXT("Enum set negative accepted: %s"), *Change.Value);
                return 1;
            }
        }
        UE_LOG(LogTemp, Display, TEXT("PASS enum_set_wrong_enum_integer_readonly_signature_default_compile_errors"));
        for (const auto& Change :
             TArray<TPair<FString, FString>>{{TEXT("api.Position=ue_vector(10.0lf,11.0lf,12.0lf)"),
                                                   TEXT("api.Position=ue_rotator(10.0lf,11.0lf,12.0lf)")},
                                              {TEXT("api.Position=ue_vector(10.0lf,11.0lf,12.0lf)"),
                                                    TEXT("api.LockedPosition=ue_vector(10.0lf,11.0lf,12.0lf)")}})
        {
            BadSource = DriverSource.Replace(*Change.Key, *Change.Value);
            if (!FFileHelper::SaveStringToFile(BadSource, *Runtime.ScriptPath(Bad),
                                               FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM) ||
                Runtime.ReadAuthoringScript(Runtime.ScriptPath(Bad), Model, Error) || !Error.Contains(TEXT("error[")))
                return 1;
        }
        UE_LOG(LogTemp, Display, TEXT("PASS math_field_wrong_type_and_readonly_compile_errors"));
        BadSource = DriverSource.Replace(TEXT("input.Point.Count=11"),
                                              TEXT("input.Point.Position=ue_rotator(1.0lf,2.0lf,3.0lf)"));
        if (!FFileHelper::SaveStringToFile(BadSource, *Runtime.ScriptPath(Bad),
                                           FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM) ||
            Runtime.ReadAuthoringScript(Runtime.ScriptPath(Bad), Model, Error) || !Error.Contains(TEXT("error[")))
            return 1;
        UE_LOG(LogTemp, Display, TEXT("PASS struct_math_wrong_type_compile_error"));
        BadSource = TargetSource.Replace(TEXT("self.Settings.Point.Position.x += 4.0lf"),
                                              TEXT("self.LockedSettings.Point.Position.x += 4.0lf"));
        if (!FFileHelper::SaveStringToFile(BadSource, *Runtime.ScriptPath(Bad),
                                           FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM) ||
            Runtime.ReadAuthoringScript(Runtime.ScriptPath(Bad), Model, Error) ||
            !Error.Contains(TEXT("Cannot write BlueprintReadOnly self struct")))
            return 1;
        UE_LOG(LogTemp, Display, TEXT("PASS nested_math_readonly_self_compile_error"));
        BadSource = TargetSource.Replace(TEXT("self.Settings.Point.Position.y = 8.0lf"),
                                              TEXT("self.Settings.Point.Position.y = \"wrong\""));
        if (!FFileHelper::SaveStringToFile(BadSource, *Runtime.ScriptPath(Bad),
                                           FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM) ||
            Runtime.ReadAuthoringScript(Runtime.ScriptPath(Bad), Model, Error) || !Error.Contains(TEXT("error[")))
            return 1;
        UE_LOG(LogTemp, Display, TEXT("PASS nested_math_wrong_scalar_compile_error"));
        for (const auto& Change : TArray<TPair<FString, FString>>{
                 {TEXT("api.Settings=propertyValue"), TEXT("api.Settings=MethodPoint()")},
                  {TEXT("api.Settings=propertyValue"), TEXT("api.LockedSettings=propertyValue")},
                   {TEXT("api.State=MethodState.Idle"), TEXT("api.State=OtherMethodState.Idle")},
                    {TEXT("api.State=MethodState.Idle"), TEXT("api.LockedState=MethodState.Idle")}})
        {
            BadSource = DriverSource.Replace(*Change.Key, *Change.Value);
            if (!FFileHelper::SaveStringToFile(BadSource, *Runtime.ScriptPath(Bad),
                                               FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM) ||
                Runtime.ReadAuthoringScript(Runtime.ScriptPath(Bad), Model, Error) || !Error.Contains(TEXT("error[")))
                return 1;
        }
        UE_LOG(LogTemp, Display, TEXT("PASS source_property_wrong_type_and_readonly_compile_errors"));
        for (const auto& Change : TArray<TPair<FString, FString>>{
                 {TEXT("api.NativeActor=returnedActor"),
                       TEXT("api.NativeActor=ue_ref(type<ObjectRef>,returnedActor.handle)")},
                  {TEXT("api.NativeActor=returnedActor"), TEXT("api.LockedNativeActor=returnedActor")},
                   {TEXT("api.NativeEvents=DasEventInterface_from(api.handle)"),
                         TEXT("api.NativeEvents=ue_ref(type<ObjectRef>,api.handle)")}})
        {
            BadSource = DriverSource.Replace(*Change.Key, *Change.Value);
            if (!FFileHelper::SaveStringToFile(BadSource, *Runtime.ScriptPath(Bad),
                                               FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM) ||
                Runtime.ReadAuthoringScript(Runtime.ScriptPath(Bad), Model, Error) || !Error.Contains(TEXT("error[")))
                return 1;
        }
        UE_LOG(LogTemp, Display, TEXT("PASS native_reference_property_wrong_type_and_readonly_compile_errors"));
        BadSource = DriverSource.Replace(TEXT("propertyValue.Point.Mode=MethodState.Idle"),
                                              TEXT("propertyValue.Point.Mode=OtherMethodState.Idle"));
        if (!FFileHelper::SaveStringToFile(BadSource, *Runtime.ScriptPath(Bad),
                                           FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM) ||
            Runtime.ReadAuthoringScript(Runtime.ScriptPath(Bad), Model, Error) || !Error.Contains(TEXT("error[")))
            return 1;
        UE_LOG(LogTemp, Display, TEXT("PASS struct_enum_same_values_wrong_type_compile_error"));
        for (const auto& Value : TArray<FString>{TEXT("OtherMethodState.Idle"),
                                                      TEXT("7"), TEXT("value; self.LockedSettings.Point.Mode=value"),
                                                                      TEXT("value; self.Settings.Point.Mode += value")})
        {
            BadSource = TargetSource.Replace(TEXT("self.Settings.Point.Mode=value"),
                                                  *(TEXT("self.Settings.Point.Mode=")+ Value));
            if (!FFileHelper::SaveStringToFile(BadSource, *Runtime.ScriptPath(Bad),
                                               FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM) ||
                Runtime.ReadAuthoringScript(Runtime.ScriptPath(Bad), Model, Error) || !Error.Contains(TEXT("error[")))
                return 1;
        }
        UE_LOG(LogTemp, Display, TEXT("PASS self_enum_wrong_type_integer_readonly_arithmetic_compile_errors"));
        for (const auto& Change : TArray<TPair<FString, FString>>{
                 {TEXT("self.Settings.Point.Owner=owner"),
                       TEXT("self.Settings.Point.Owner=ue_ref(type<ObjectRef>,owner.handle)")},
                  {TEXT("self.Settings.Point.Events=events"),
                        TEXT("self.Settings.Point.Events=ue_ref(type<ObjectRef>,events.handle)")},
                   {TEXT("self.Settings.Point.Owner=owner"), TEXT("self.LockedSettings.Point.Owner=owner")},
                    {TEXT("self.Settings.Point.Owner=owner"), TEXT("self.Settings.Point.Owner += owner")}})
        {
            BadSource = TargetSource.Replace(*Change.Key, *Change.Value);
            if (!FFileHelper::SaveStringToFile(BadSource, *Runtime.ScriptPath(Bad),
                                               FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM) ||
                Runtime.ReadAuthoringScript(Runtime.ScriptPath(Bad), Model, Error) || !Error.Contains(TEXT("error[")))
                return 1;
        }
        UE_LOG(LogTemp, Display, TEXT("PASS self_reference_wrong_type_readonly_arithmetic_compile_errors"));
        for (const auto& Change : TArray<TPair<FString, FString>>{
                 {TEXT("self.Settings.Point=point"), TEXT("self.Settings.Point=settings")},
                  {TEXT("self.Settings.Point.Position=vector"), TEXT("self.Settings.Point.Position=rotation")},
                   {TEXT("self.Settings.Pose=transform"), TEXT("self.Settings.Pose=vector")},
                    {TEXT("self.Settings.Point=point"), TEXT("self.LockedSettings.Point=point")},
                     {TEXT("self.Settings.Point=point"), TEXT("self.Settings.Point += point")}})
        {
            BadSource = TargetSource.Replace(*Change.Key, *Change.Value);
            if (!FFileHelper::SaveStringToFile(BadSource, *Runtime.ScriptPath(Bad),
                                               FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM) ||
                Runtime.ReadAuthoringScript(Runtime.ScriptPath(Bad), Model, Error) || !Error.Contains(TEXT("error[")))
                return 1;
        }
        UE_LOG(LogTemp, Display, TEXT("PASS self_nested_value_wrong_type_readonly_arithmetic_compile_errors"));
        for (const auto& Change : TArray<TPair<FString, FString>>{
                 {TEXT("self.Settings.Items := items"), TEXT("self.Settings.Items := weights")},
                  {TEXT("self.Settings.Items := items"), TEXT("var wrong:array<int>; self.Settings.Items := wrong")},
                   {TEXT("self.Settings.Weights := weights"), TEXT("self.Settings.Weights := tags")},
                    {TEXT("self.Settings.Items := items"), TEXT("self.LockedSettings.Items := items")}})
        {
            BadSource = TargetSource.Replace(*Change.Key, *Change.Value);
            if (!FFileHelper::SaveStringToFile(BadSource, *Runtime.ScriptPath(Bad),
                                               FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM) ||
                Runtime.ReadAuthoringScript(Runtime.ScriptPath(Bad), Model, Error) || !Error.Contains(TEXT("error[")))
                return 1;
        }
        UE_LOG(LogTemp, Display, TEXT("PASS self_container_wrong_type_element_readonly_compile_errors"));
        for (const auto& Change : TArray<TPair<FString, FString>>{
                 {TEXT("api.Partner=api"), TEXT("api.Partner=ue_ref(type<ActorRef>,api.handle)")},
                  {TEXT("api.Partner=api"), TEXT("api.LockedPartner=api")}})
        {
            BadSource = DriverSource.Replace(*Change.Key, *Change.Value);
            if (!FFileHelper::SaveStringToFile(BadSource, *Runtime.ScriptPath(Bad),
                                               FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM) ||
                Runtime.ReadAuthoringScript(Runtime.ScriptPath(Bad), Model, Error) || !Error.Contains(TEXT("error[")))
                return 1;
        }
        BadSource =
            TargetSource.Replace(TEXT("self.Partner=value"), TEXT("self.Partner=ue_ref(type<ActorRef>,value.handle)"));
        if (!FFileHelper::SaveStringToFile(BadSource, *Runtime.ScriptPath(Bad),
                                           FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM) ||
            Runtime.ReadAuthoringScript(Runtime.ScriptPath(Bad), Model, Error) || !Error.Contains(TEXT("error[")))
            return 1;
        UE_LOG(LogTemp, Display, TEXT("PASS authored_field_wrong_wrapper_and_readonly_compile_errors"));
        for (const auto& Change : TArray<TPair<FString, FString>>{
                 {TEXT("insert(self.Settings.Tags,\"operation\")"),
                       TEXT("insert(self.LockedSettings.Tags,\"operation\")")},
                  {TEXT("insert(self.Settings.Tags,\"operation\")"), TEXT("insert(self.Settings.Tags,7)")},
                   {TEXT("insert(self.Settings.Weights,\"operation\",8.5lf)"),
                         TEXT("insert(self.Settings.Weights,\"operation\",\"wrong\")")}})
        {
            BadSource = TargetSource.Replace(*Change.Key, *Change.Value);
            if (!FFileHelper::SaveStringToFile(BadSource, *Runtime.ScriptPath(Bad),
                                               FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM) ||
                Runtime.ReadAuthoringScript(Runtime.ScriptPath(Bad), Model, Error) || !Error.Contains(TEXT("error[")))
                return 1;
        }
        UE_LOG(LogTemp, Display, TEXT("PASS self_container_operation_readonly_key_value_compile_errors"));
        for (const auto& Change : TArray<TPair<FString, FString>>{
                 {TEXT("self.Settings.Samples[0]=5.0lf"), TEXT("self.LockedSettings.Samples[0]=5.0lf")},
                  {TEXT("self.Settings.Samples[0]=5.0lf"), TEXT("self.Settings.Samples[0]=\"wrong\"")},
                   {TEXT("self.Settings.Weights[\"indexed\"]=7.0lf"), TEXT("self.Settings.Weights[7]=7.0lf")},
                    {TEXT("self.Settings.Items[0].Mode=MethodState.Idle"),
                          TEXT("self.Settings.Items[0].Mode=OtherMethodState.Idle")},
                     {TEXT("push(self.Settings.Samples,2.0lf)"), TEXT("push(self.LockedSettings.Samples,2.0lf)")},
                      {TEXT("push(self.Settings.Samples,2.0lf)"), TEXT("push(self.Settings.Samples,\"wrong\")")}})
        {
            BadSource = TargetSource.Replace(*Change.Key, *Change.Value);
            if (BadSource == TargetSource ||
                !FFileHelper::SaveStringToFile(BadSource, *Runtime.ScriptPath(Bad),
                                               FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM) ||
                Runtime.ReadAuthoringScript(Runtime.ScriptPath(Bad), Model, Error) || !Error.Contains(TEXT("error[")))
                return 1;
        }
        UE_LOG(LogTemp, Display, TEXT("PASS indexed_container_types_readonly_compile_errors"));
        for (const auto& Change : TArray<TPair<FString, FString>>{
                 {TEXT("api.Settings.Items[0].Count=29"), TEXT("api.LockedSettings.Items[0].Count=29")},
                  {TEXT("api.Settings.Items[0].Count=29"), TEXT("api.Settings.Items[0].Count=\"wrong\"")},
                   {TEXT("api.Settings.Weights[\"foreign\"]=7.5lf"), TEXT("api.Settings.Weights[7]=7.5lf")},
                    {TEXT("push(api.Settings.Samples,2.0lf)"), TEXT("push(api.LockedSettings.Samples,2.0lf)")},
                     {TEXT("push(api.Settings.Samples,2.0lf)"), TEXT("push(api.Settings.Samples,\"wrong\")")}})
        {
            BadSource = DriverSource.Replace(*Change.Key, *Change.Value);
            if (BadSource == DriverSource ||
                !FFileHelper::SaveStringToFile(BadSource, *Runtime.ScriptPath(Bad),
                                               FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM))
                return 1;
            const bool Accepted = Runtime.ReadAuthoringScript(Runtime.ScriptPath(Bad), Model, Error);
            if (Accepted || !Error.Contains(TEXT("error[")))
            {
                UE_LOG(LogTemp, Error,
                       TEXT("Foreign negative check failed: %s accepted=%d error=%s"), *Change.Value, Accepted, *Error);
                return 1;
            }
        }
        UE_LOG(LogTemp, Display, TEXT("PASS foreign_container_types_readonly_compile_errors"));
        for (const auto& Change : TArray<TPair<FString, FString>>{
                 {TEXT("push(self.DirectSamples,2.0lf)"), TEXT("push(self.LockedDirect,2.0lf)")},
                  {TEXT("push(self.DirectSamples,2.0lf)"), TEXT("push(self.DirectSamples,\"wrong\")")},
                   {TEXT("DirectSamples:array<double>"), TEXT("DirectSamples:array<double> = [true]")},
                    {TEXT("DirectSamples:array<double>"), TEXT("DirectSamples:array<array<int>>")}})
        {
            BadSource = TargetSource.Replace(*Change.Key, *Change.Value);
            if (BadSource == TargetSource ||
                !FFileHelper::SaveStringToFile(BadSource, *Runtime.ScriptPath(Bad),
                                               FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM) ||
                Runtime.ReadAuthoringScript(Runtime.ScriptPath(Bad), Model, Error) || !Error.Contains(TEXT("error[")))
                return 1;
        }
        for (const auto& Change : TArray<TPair<FString, FString>>{
                 {TEXT("push(api.DirectSamples,9.0lf)"), TEXT("push(api.LockedDirect,9.0lf)")},
                  {TEXT("api.DirectSamples := directCopy"), TEXT("api.DirectSamples := api.DirectPoints")},
                   {TEXT("api.DirectSamples := directCopy"), TEXT("api.LockedDirect := directCopy")}})
        {
            BadSource = DriverSource.Replace(*Change.Key, *Change.Value);
            if (BadSource == DriverSource ||
                !FFileHelper::SaveStringToFile(BadSource, *Runtime.ScriptPath(Bad),
                                               FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM))
                return 1;
            const bool Accepted = Runtime.ReadAuthoringScript(Runtime.ScriptPath(Bad), Model, Error);
            if (Accepted || !Error.Contains(TEXT("error[")))
            {
                UE_LOG(LogTemp, Error,
                       TEXT("Direct negative failed: %s accepted=%d error=%s"), *Change.Value, Accepted, *Error);
                return 1;
            }
        }
        UE_LOG(LogTemp, Display, TEXT("PASS direct_container_readonly_types_defaults_compile_errors"));
        for (const auto& Change : TArray<TPair<FString, FString>>{
                 {TEXT("self.NameScores[ue_name_value(\"Unit_3\")]=7"), TEXT("self.NameScores[\"Unit_3\"]=7")},
                  {TEXT("insert(self.NameTags,ue_name_value(\"Unit_3\"))"),
                        TEXT("insert(self.LockedNames,ue_name_value(\"Unit_3\"))")},
                   {TEXT("NameScores:table<UENameValue;int>"),
                         TEXT("NameScores:table<UENameValue;int> = {ue_name_value(\"A\")=>true}")}})
        {
            BadSource = DriverSource.Replace(*Change.Key, *Change.Value);
            if (BadSource == DriverSource ||
                !FFileHelper::SaveStringToFile(BadSource, *Runtime.ScriptPath(Bad),
                                               FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM) ||
                Runtime.ReadAuthoringScript(Runtime.ScriptPath(Bad), Model, Error))
            {
                UE_LOG(LogTemp, Error, TEXT("Name container negative accepted: %s"), *Change.Value);
                return 1;
            }
        }
        UE_LOG(LogTemp, Display, TEXT("PASS name_container_key_readonly_invalid_default_refused"));
        if (!FindFProperty<FArrayProperty>(Target->GeneratedClass, TEXT("DirectSamples")) ||
                                           !FindFProperty<FArrayProperty>(
                                               Target->GeneratedClass, TEXT("DirectPoints")) ||
                                               !FindFProperty<FMapProperty>(
                                                   Target->GeneratedClass, TEXT("DirectWeights")) ||
                                                   !FindFProperty<FSetProperty>(Target->GeneratedClass,
                                                                                TEXT("DirectTags")))
            return 1;
        UE_LOG(LogTemp, Display, TEXT("PASS direct_container_native_properties"));
        UBlueprint* Reimported = nullptr;
        auto* BeforeReimport = Target->GeneratedClass.Get();
        if (!FDasDSLService::Apply(TargetFile,
                                   Base + TEXT("/BP_Target"), TEXT("methodvalues.ValueTarget"), Reimported, Error) ||
                                               Reimported != Target || Target->GeneratedClass.Get() != BeforeReimport)
        {
            UE_LOG(LogTemp, Error, TEXT("Direct container reimport failed: %s"), *Error);
            return 1;
        }
        UE_LOG(LogTemp, Display, TEXT("PASS direct_container_body_only_reimport"));
        BadSource =
            TargetSource.Replace(TEXT("DirectSamples:array<double>"),
                                      TEXT("@DisplayName=\"Changed samples\"\n    DirectSamples:array<double>"));
        if (BadSource == TargetSource ||
            !FFileHelper::SaveStringToFile(BadSource, *Runtime.ScriptPath(Bad),
                                           FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM) ||
            FDasDSLService::Apply(
                Bad, Base + TEXT("/BP_Target"), TEXT("methodvalues.ValueTarget"), Reimported, Error) ||
                                 !Error.Contains(TEXT("Direct container class declaration migration is deferred")) ||
                                                 Target->GeneratedClass.Get() != BeforeReimport)
            return 1;
        UE_LOG(LogTemp, Display, TEXT("PASS direct_container_declaration_change_refused"));
        for (const auto& Change : TArray<TPair<FString, FString>>{
                 {TEXT("self.Groups[0].Scores[1l] += 3.0lf"), TEXT("self.LockedGroups[0].Scores[1l] += 3.0lf")},
                  {TEXT("self.Groups[0].Scores[1l] += 3.0lf"), TEXT("self.Groups[\"bad\"].Scores[1l] += 3.0lf")},
                   {TEXT("self.Groups[0].Scores[1l] += 3.0lf"), TEXT("self.Groups[0].Scores[\"bad\"] += 3.0lf")},
                    {TEXT("self.Groups[0].Scores[1l] += 3.0lf"), TEXT("self.Groups[0].Scores[1l] = \"bad\"")},
                     {TEXT("self.Groups[0].Points[0l].Mode=MethodState.Idle"),
                           TEXT("self.Groups[0].Points[0l].Mode=OtherMethodState.Idle")},
                      {TEXT("push(self.Groups[0].Scores,2.0lf)"), TEXT("push(self.LockedGroups[0].Scores,2.0lf)")}})
        {
            BadSource = TargetSource.Replace(*Change.Key, *Change.Value);
            if (BadSource == TargetSource ||
                !FFileHelper::SaveStringToFile(BadSource, *Runtime.ScriptPath(Bad),
                                               FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM))
                return 1;
            const bool Accepted = Runtime.ReadAuthoringScript(Runtime.ScriptPath(Bad), Model, Error);
            if (Accepted || !Error.Contains(TEXT("error[")))
            {
                UE_LOG(LogTemp, Error,
                       TEXT("Multi-index negative failed: %s accepted=%d error=%s"), *Change.Value, Accepted, *Error);
                return 1;
            }
        }
        UE_LOG(LogTemp, Display, TEXT("PASS multi_index_readonly_types_compile_errors"));
        for (const auto& Change :
             TArray<TPair<FString, FString>>{{TEXT("push(self.DirectVectors,ue_vector(1.0lf,2.0lf,3.0lf))"),
                                                   TEXT("push(self.DirectVectors,ue_rotator(1.0lf,2.0lf,3.0lf))")},
                                              {TEXT("push(self.DirectVectors,ue_vector(1.0lf,2.0lf,3.0lf))"),
                                                    TEXT("push(self.LockedVectors,ue_vector(1.0lf,2.0lf,3.0lf))")}})
        {
            BadSource = TargetSource.Replace(*Change.Key, *Change.Value);
            if (BadSource == TargetSource ||
                !FFileHelper::SaveStringToFile(BadSource, *Runtime.ScriptPath(Bad),
                                               FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM) ||
                Runtime.ReadAuthoringScript(Runtime.ScriptPath(Bad), Model, Error) || !Error.Contains(TEXT("error[")))
                return 1;
        }
        for (const auto& Change : TArray<TPair<FString, FString>>{
                 {TEXT("api.CopyVectors(api.DirectVectors)"), TEXT("api.CopyVectors(api.DirectRotators)")},
                  {TEXT("api.DirectVectors := vectors"), TEXT("api.DirectVectors := rotators")},
                   {TEXT("api.DirectVectors := vectors"), TEXT("api.LockedVectors := vectors")}})
        {
            BadSource = DriverSource.Replace(*Change.Key, *Change.Value);
            if (BadSource == DriverSource ||
                !FFileHelper::SaveStringToFile(BadSource, *Runtime.ScriptPath(Bad),
                                               FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM) ||
                Runtime.ReadAuthoringScript(Runtime.ScriptPath(Bad), Model, Error) || !Error.Contains(TEXT("error[")))
                return 1;
        }
        UE_LOG(LogTemp, Display, TEXT("PASS math_array_wrong_type_and_readonly_compile_errors"));
        for (const auto& Item :
             TArray<TPair<FString, FString>>{{TEXT("DirectVectors"), TEXT("/Script/CoreUObject.Vector")},
                                              {TEXT("DirectRotators"), TEXT("/Script/CoreUObject.Rotator")},
                                               {TEXT("DirectTransforms"), TEXT("/Script/CoreUObject.Transform")}})
        {
            auto* A = FindFProperty<FArrayProperty>(Target->GeneratedClass, *Item.Key);
            auto* E = A ? CastField<FStructProperty>(A->Inner) : nullptr;
            if (!E || E->Struct->GetPathName() != Item.Value)
                return 1;
        }
        UE_LOG(LogTemp, Display, TEXT("PASS math_array_native_element_identity"));
        for (const auto& Change : TArray<TPair<FString, FString>>{
                 {TEXT("push(self.DirectStates,MethodState.Active)"),
                       TEXT("push(self.DirectStates,OtherMethodState.Active)")},
                  {TEXT("self.DirectStates[1l]=MethodState.Active"), TEXT("self.DirectStates[1l]=42")},
                   {TEXT("push(self.DirectStates,MethodState.Active)"),
                         TEXT("push(self.LockedStates,MethodState.Active)")}})
        {
            BadSource = TargetSource.Replace(*Change.Key, *Change.Value);
            if (BadSource == TargetSource ||
                !FFileHelper::SaveStringToFile(BadSource, *Runtime.ScriptPath(Bad),
                                               FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM) ||
                Runtime.ReadAuthoringScript(Runtime.ScriptPath(Bad), Model, Error) || !Error.Contains(TEXT("error[")))
                return 1;
        }
        for (
            const auto& Change :
            TArray<TPair<FString, FString>>{{TEXT("var states=api.CopyStates(api.DirectStates)"),
                TEXT("var otherStates:array<OtherMethodState>; push(otherStates,OtherMethodState.Active); var states=api.CopyStates(otherStates)")},
                                             {TEXT("api.DirectStates := states"), TEXT("api.LockedStates := states")}})
        {
            BadSource = DriverSource.Replace(*Change.Key, *Change.Value);
            if (BadSource == DriverSource ||
                !FFileHelper::SaveStringToFile(BadSource, *Runtime.ScriptPath(Bad),
                                               FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM) ||
                Runtime.ReadAuthoringScript(Runtime.ScriptPath(Bad), Model, Error) || !Error.Contains(TEXT("error[")))
                return 1;
        }
        UE_LOG(LogTemp, Display, TEXT("PASS enum_array_nominal_integer_readonly_compile_errors"));
        auto* StatesField = FindFProperty<FArrayProperty>(Target->GeneratedClass, TEXT("DirectStates"));
        auto* EnumType = StatesField ? DasEnumPropertyType(StatesField->Inner) : nullptr;
        if (!EnumType ||
            EnumType->GetValueByNameString(TEXT("Idle")) != 7 ||
                                           EnumType->GetValueByNameString(TEXT("Active")) != 42 ||
                                                                          EnumType->GetIndexByValue(0) != INDEX_NONE)
            return 1;
        UE_LOG(LogTemp, Display, TEXT("PASS enum_array_native_sparse_identity"));
        auto* EnumCarrier = DasContainerCarrierForProperty(StatesField);
        if (!EnumCarrier)
            return 1;
        FInstancedStruct Candidate;
        Candidate.InitializeAs(EnumCarrier);
        auto* EnumValues = CastFieldChecked<FArrayProperty>(EnumCarrier->FindPropertyByName(TEXT("Values")));
        FScriptArrayHelper EnumItems(EnumValues,
                                     EnumValues->ContainerPtrToValuePtr<void>(Candidate.GetMutableMemory()));
        EnumItems.AddValue();
        auto* RawByte = CastFieldChecked<FByteProperty>(EnumValues->Inner);
        RawByte->SetPropertyValue(EnumItems.GetRawPtr(0), 0);
        FString SnapshotError;
        if (ValidateDasStructSnapshot(Candidate, SnapshotError) ||
            !SetDasEnumPropertyNumber(EnumValues->Inner, EnumItems.GetRawPtr(0), 7) ||
            !ValidateDasStructSnapshot(Candidate, SnapshotError))
            return 1;
        RawByte->SetPropertyValue(EnumItems.GetRawPtr(0), uint8(EnumType->GetValueByIndex(EnumType->NumEnums() - 1)));
        if (ValidateDasStructSnapshot(Candidate, SnapshotError))
            return 1;
        UE_LOG(LogTemp, Display, TEXT("PASS enum_array_invalid_zero_and_reserved_native_snapshot_refused"));

        for (const auto& Change : TArray<TPair<FString, FString>>{
                 {TEXT("push(self.DirectActors,actor)"), TEXT("push(self.DirectActors,default<ObjectRef>)")},
                  {TEXT("push(self.DirectEventRefs,events)"), TEXT("push(self.DirectEventRefs,actor)")},
                   {TEXT("push(self.DirectActors,actor)"), TEXT("push(self.LockedActors,actor)")}})
        {
            BadSource = TargetSource.Replace(*Change.Key, *Change.Value);
            if (BadSource == TargetSource ||
                !FFileHelper::SaveStringToFile(BadSource, *Runtime.ScriptPath(Bad),
                                               FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM) ||
                Runtime.ReadAuthoringScript(Runtime.ScriptPath(Bad), Model, Error) || !Error.Contains(TEXT("error[")))
                return 1;
        }
        for (const auto& Change : TArray<TPair<FString, FString>>{
                 {TEXT("api.CopyActors(api.DirectActors)"), TEXT("api.CopyActors(api.DirectEventRefs)")},
                  {TEXT("api.DirectActors := actors"), TEXT("api.LockedActors := actors")}})
        {
            BadSource = DriverSource.Replace(*Change.Key, *Change.Value);
            if (BadSource == DriverSource ||
                !FFileHelper::SaveStringToFile(BadSource, *Runtime.ScriptPath(Bad),
                                               FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM) ||
                Runtime.ReadAuthoringScript(Runtime.ScriptPath(Bad), Model, Error) || !Error.Contains(TEXT("error[")))
                return 1;
        }
        UE_LOG(LogTemp, Display, TEXT("PASS reference_array_nominal_readonly_compile_errors"));
        auto* ActorArray = FindFProperty<FArrayProperty>(Target->GeneratedClass, TEXT("DirectActors"));
        auto* EventArray = FindFProperty<FArrayProperty>(Target->GeneratedClass, TEXT("DirectEventRefs"));
        if (!ActorArray || !EventArray || DasReferencePropertyClass(ActorArray->Inner) != AActor::StaticClass() ||
            !CastField<FInterfaceProperty>(EventArray->Inner))
            return 1;
        UE_LOG(LogTemp, Display, TEXT("PASS reference_array_native_class_interface_identity"));
        for (const auto& Change : TArray<TPair<FString, FString>>{
                 {TEXT("push(self.DirectPartners,value)"),
                       TEXT("push(self.DirectPartners,ue_ref(type<ActorRef>,value.handle))")},
                  {TEXT("push(self.DirectPartners,value)"), TEXT("push(self.LockedPartners,value)")},
                   {TEXT("push(self.DirectPartners,value)"),
                         TEXT("push(self.DirectPartners,ue_ref(type<AlternativeTargetRef>,value.handle))")}})
        {
            BadSource = TargetSource.Replace(*Change.Key, *Change.Value);
            if (BadSource == TargetSource ||
                !FFileHelper::SaveStringToFile(BadSource, *Runtime.ScriptPath(Bad),
                                               FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM) ||
                Runtime.ReadAuthoringScript(Runtime.ScriptPath(Bad), Model, Error) || !Error.Contains(TEXT("error[")))
                return 1;
        }
        for (const auto& Change : TArray<TPair<FString, FString>>{
                 {TEXT("api.DirectPartners := partners"), TEXT("api.LockedPartners := partners")},
                  {TEXT("api.AlternativePartners := alternative"), TEXT("api.DirectPartners := alternative")}})
        {
            BadSource = DriverSource.Replace(*Change.Key, *Change.Value);
            if (BadSource == DriverSource ||
                !FFileHelper::SaveStringToFile(BadSource, *Runtime.ScriptPath(Bad),
                                               FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM) ||
                Runtime.ReadAuthoringScript(Runtime.ScriptPath(Bad), Model, Error) || !Error.Contains(TEXT("error[")))
                return 1;
        }
        UE_LOG(LogTemp, Display, TEXT("PASS authored_array_nominal_readonly_compile_errors"));
        const auto* AuthoredTarget =
            Model.Classes.FindByPredicate([](const auto& C) { return C.Id == TEXT("methodvalues.ValueTarget"); });
        FDasAuthoringModule ActualTarget;
        if (!Runtime.ReadAuthoringScript(Runtime.ScriptPath(TargetFile), ActualTarget, Error))
            return 1;
        AuthoredTarget = ActualTarget.Classes.FindByPredicate([](const auto& C)
                                                              { return C.Id == TEXT("methodvalues.ValueTarget"); });
        const auto* AuthoredArray = AuthoredTarget ? AuthoredTarget->Properties.FindByPredicate(
                                                         [](const auto& P) { return P.Name == TEXT("DirectPartners"); })
                                                   : nullptr;
        if (!AuthoredArray || !AuthoredArray->Type.Element ||
            AuthoredArray->Type.Element->AuthoredId != TEXT("methodvalues.ValueTarget"))
            return 1;
        UE_LOG(LogTemp, Display, TEXT("PASS authored_array_element_identity_model"));
        CastChecked<ADasProbeActor>(Target->GeneratedClass->GetDefaultObject())->TypeRegistry = Registry;
        CastChecked<ADasProbeActor>(Driver->GeneratedClass->GetDefaultObject())->TypeRegistry = Registry;
        for (auto* Type : Types)
            if (!Save(Type))
                return 1;
        if (!Save(Registry) || !Save(Target) || !Save(Driver))
            return 1;
        auto* Map = UWorld::CreateWorld(EWorldType::Editor, false, TEXT("Map"), CreatePackage(*(Base + TEXT("/Map"))));
        Map->SetFlags(RF_Public | RF_Standalone);
        Map->GetOutermost()->SetPackageFlags(PKG_ContainsMap);
        FActorSpawnParameters Spawn;
        Spawn.Name = TEXT("MethodValueTarget");
        auto* T = Map->SpawnActor<ADasProbeActor>(Target->GeneratedClass, FTransform::Identity, Spawn);
        Spawn.Name = TEXT("MethodValueDriver");
        auto* D = Map->SpawnActor<ADasProbeActor>(Driver->GeneratedClass, FTransform::Identity, Spawn);
        D->Peer = T;
        if (!Save(Map))
            return 1;
        Map->DestroyWorld(false);
    }
    else if (Phase == TEXT("Reopen"))
    {
        Target = LoadObject<UBlueprint>(nullptr, *(Base + TEXT("/BP_Target.BP_Target")));
        Driver = LoadObject<UBlueprint>(nullptr, *(Base + TEXT("/BP_Driver.BP_Driver")));
    }
    else
        return 1;
    if (!Target || !Driver)
        return 1;
    FString Report;
    const bool Good = RunDasMethodValuesProbe(Driver->GeneratedClass, Target->GeneratedClass, Report);
    UE_LOG(LogTemp, Display, TEXT("%s"), *Report);
    return Good ? 0 : 1;
}
