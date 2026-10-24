#include "DasNativeBindsManifestCommandlet.h"
#include "DasNativeBinds.h"
#include "DasRuntime.h"
#include "HAL/FileManager.h"
#include "Modules/ModuleManager.h"
#include "DasUEAotProbe.h"
#include "Dom/JsonObject.h"
#include "Misc/FileHelper.h"
#include "Misc/Parse.h"
#include "Misc/Paths.h"
#include "Serialization/JsonSerializer.h"
#include "Serialization/JsonWriter.h"

UDasNativeBindsManifestCommandlet::UDasNativeBindsManifestCommandlet()
{
    IsClient = false;
    IsServer = false;
    IsEditor = true;
    LogToConsole = true;
}

int32 UDasNativeBindsManifestCommandlet::Main(const FString& Params)
{
    FString Output =
        FPaths::ConvertRelativePathToFull(FPaths::ProjectDir() / TEXT("../../logs/native-binds-manifest.json"));
    FParse::Value(*Params, TEXT("Output="), Output);
    auto Strings = [](const TArray<FString>& Values)
    {
        TArray<TSharedPtr<FJsonValue>> Array;
        for (const FString& Value : Values)
            Array.Add(MakeShared<FJsonValueString>(Value));
        return Array;
    };
    TArray<TSharedPtr<FJsonValue>> Modules;
    int32 Functions = 0;
    int32 Failed = 0;
    int32 Routed = 0, Kept = 0; // binds that reflect a UFUNCTION (Reflects)
    TMap<FString, FString> Routes; // "Receiver.name" -> "routed" or the note of a kept reflected call
    TSet<FString> NativeTypes;     // script types of the native modules
    for (const FDasNativeModuleInfo& Module : DescribeDasNativeModules())
    {
        auto Object = MakeShared<FJsonObject>();
        Object->SetStringField(TEXT("name"), Module.Name);
        Object->SetStringField(TEXT("error"), Module.Error);
        Object->SetArrayField(TEXT("classes"), Strings(Module.Classes));
        NativeTypes.Append(Module.Classes);
        TArray<TSharedPtr<FJsonValue>> List;
        for (const FDasNativeFunctionInfo& F : Module.Functions)
        {
            auto Function = MakeShared<FJsonObject>();
            Function->SetStringField(TEXT("name"), F.Name);
            Function->SetStringField(TEXT("receiver"), F.Receiver);
            Function->SetArrayField(TEXT("arguments"), Strings(F.Arguments));
            Function->SetStringField(TEXT("result"), F.Result);
            Function->SetStringField(TEXT("origin"), F.Origin);
            Function->SetBoolField(TEXT("pure"), F.bPure);
            Function->SetBoolField(TEXT("non_null_args"), F.bNonNullArgs);
            Function->SetStringField(TEXT("reflects"), F.Reflects);
            Function->SetBoolField(TEXT("routed"), F.bRouted);
            Function->SetStringField(TEXT("route_note"), F.RouteNote);
            if (!F.Reflects.IsEmpty())
            {
                ++(F.bRouted ? Routed : Kept);
                Routes.Add(F.Receiver + TEXT(".") + F.Name, F.bRouted ? FString(TEXT("routed")) : F.RouteNote);
                if (!F.bRouted)
                    UE_LOG(LogTemp, Display, TEXT("DAS_NATIVE_BINDS_ROUTE %s.%s keeps the reflected %s: %s"),
                           *F.Receiver, *F.Name, *F.Reflects, *F.RouteNote);
            }
            List.Add(MakeShared<FJsonValueObject>(Function));
        }
        Object->SetArrayField(TEXT("functions"), List);
        Modules.Add(MakeShared<FJsonValueObject>(Object));
        Functions += Module.Functions.Num();
        if (!Module.Error.IsEmpty())
        {
            UE_LOG(LogTemp, Error, TEXT("DAS_NATIVE_BINDS_MANIFEST module %s: %s"), *Module.Name, *Module.Error);
            ++Failed;
        }
    }
    auto Root = MakeShared<FJsonObject>();
    Root->SetArrayField(TEXT("modules"), Modules);
    FString Json;
    const auto Writer = TJsonWriterFactory<>::Create(&Json);
    const bool bWritten =
        FJsonSerializer::Serialize(Root, Writer) &&
        FFileHelper::SaveStringToFile(Json, *Output, FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM);
    // Reflects in ue_engine: plain getters are routed, so are object parameters and results (as native reference
    // types); an authority-only UFUNCTION keeps the reflected call.
    for (const TPair<const TCHAR*, const TCHAR*>& Expected :
         {TPair<const TCHAR*, const TCHAR*>(TEXT("AActor.get_actor_location"), TEXT("routed")),
          TPair<const TCHAR*, const TCHAR*>(TEXT("USceneComponent.get_component_location"), TEXT("routed")),
          TPair<const TCHAR*, const TCHAR*>(TEXT("AActor.get_owner"), TEXT("routed")),
          TPair<const TCHAR*, const TCHAR*>(TEXT("AActor.set_owner"), TEXT("routed")),
          TPair<const TCHAR*, const TCHAR*>(TEXT("AActor.get_root_component"), TEXT("routed")),
          TPair<const TCHAR*, const TCHAR*>(TEXT("APawn.get_controller"), TEXT("routed")),
          TPair<const TCHAR*, const TCHAR*>(TEXT("UActorComponent.get_owner"), TEXT("routed")),
          TPair<const TCHAR*, const TCHAR*>(TEXT("USceneComponent.get_attach_parent"), TEXT("routed")),
          TPair<const TCHAR*, const TCHAR*>(TEXT("AActor.set_replicates"), TEXT("requires authority"))})
        if (const FString* Got = Routes.Find(Expected.Key); !Got || *Got != Expected.Value)
        {
            UE_LOG(LogTemp, Error, TEXT("DAS_NATIVE_BINDS_MANIFEST route %s: expected '%s', got '%s'"), Expected.Key,
                   Expected.Value, Got ? **Got : TEXT("no Reflects"));
            ++Failed;
        }
    // Generated modules (Scripts/GenerateBindings.py) declare no short alias with the name of a native type: in a
    // script that requires both, the name would be ambiguous. A hit means a stale generated module.
    const FString Scripts = FPaths::GetPath(
        FModuleManager::LoadModuleChecked<FDasRuntimeModule>(TEXT("DasRuntime")).ScriptPath(TEXT("ue_generated.das")));
    TArray<FString> ScriptFiles;
    IFileManager::Get().FindFiles(ScriptFiles, *(Scripts / TEXT("*.das")), true, false);
    for (const FString& File : ScriptFiles)
    {
        FString Text;
        if (!FFileHelper::LoadFileToString(Text, *(Scripts / File)))
            continue;
        for (const FString& Name : NativeTypes)
            if (Text.StartsWith(TEXT("typedef ") + Name + TEXT(" = ")) ||
                Text.Contains(TEXT("\ntypedef ") + Name + TEXT(" = ")))
            {
                UE_LOG(LogTemp, Error, TEXT("DAS_NATIVE_BINDS_MANIFEST %s: short alias %s is the name of a native type; "
                                            "regenerate it with Scripts/GenerateBindings.py"),
                       *File, *Name);
                ++Failed;
            }
    }
    const bool bManifest = bWritten && Failed == 0 && Modules.Num() > 0;
    const TCHAR* ManifestResult = bManifest ? TEXT("PASS") : TEXT("FAIL");
    UE_LOG(LogTemp, Display,
           TEXT("DAS_NATIVE_BINDS_MANIFEST=%s modules=%d functions=%d routed=%d kept=%d output=%s"), ManifestResult,
           Modules.Num(), Functions, Routed, Kept, *Output);

    // AOT: native_binds_aot_check.das is generated for AOT (not compiled here; the generated source is kept next to
    // the manifest). open_only and with_objects (object methods and helpers) must call native binds through the
    // guarded bridge; with_closed, which calls a bind without an AOT ABI, must stay in the interpreter.
    const FString AotOutput = FPaths::GetPath(Output) / TEXT("native-binds-aot-check.cpp");
    FString AotError, AotText;
    bool bAot = GenerateDasUEAotProbe(AotOutput, AotError, TEXT("native_binds_aot_check.das")) &&
                                      FFileHelper::LoadFileToString(AotText, *AotOutput);
    if (bAot && !AotText.Contains(TEXT("open_only")))
        AotError = TEXT("open_only is not compiled for AOT");
    else if (bAot && !AotText.Contains(TEXT("with_objects")))
        AotError = TEXT("with_objects is not compiled for AOT");
    else if (bAot && !AotText.Contains(TEXT("with_refs")))
        AotError = TEXT("with_refs is not compiled for AOT");
    else if (bAot && !AotText.Contains(TEXT("with_classes")))
        AotError = TEXT("with_classes is not compiled for AOT");
    else if (bAot && AotText.Contains(TEXT("with_arrays")))
        AotError = TEXT("with_arrays is compiled for AOT although an array result has no AOT ABI");
    else if (bAot && AotText.Contains(TEXT("with_closed")))
        AotError = TEXT("with_closed is compiled for AOT although it calls a bind without an AOT ABI");
    bAot = bAot && AotError.IsEmpty();
    const TCHAR* AotResult = bAot ? TEXT("PASS") : TEXT("FAIL");
    UE_LOG(LogTemp, Display, TEXT("DAS_NATIVE_BINDS_AOT=%s output=%s %s"), AotResult, *AotOutput, *AotError.Left(1000));
    return bManifest && bAot ? 0 : 1;
}
