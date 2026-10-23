#include "DasNativeBindsManifestCommandlet.h"
#include "DasNativeBinds.h"
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
    for (const FDasNativeModuleInfo& Module : DescribeDasNativeModules())
    {
        auto Object = MakeShared<FJsonObject>();
        Object->SetStringField(TEXT("name"), Module.Name);
        Object->SetStringField(TEXT("error"), Module.Error);
        Object->SetArrayField(TEXT("classes"), Strings(Module.Classes));
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
    const bool bManifest = bWritten && Failed == 0 && Modules.Num() > 0;
    const TCHAR* ManifestResult = bManifest ? TEXT("PASS") : TEXT("FAIL");
    UE_LOG(LogTemp, Display,
           TEXT("DAS_NATIVE_BINDS_MANIFEST=%s modules=%d functions=%d output=%s"), ManifestResult, Modules.Num(),
                Functions, *Output);

    // AOT: native_binds_aot_check.das is generated for AOT (not compiled here; the generated source is kept next to
    // the manifest). open_only must call native binds through the guarded bridge; with_closed, which calls a bind
    // without an AOT ABI, must stay in the interpreter.
    const FString AotOutput = FPaths::GetPath(Output) / TEXT("native-binds-aot-check.cpp");
    FString AotError, AotText;
    bool bAot = GenerateDasUEAotProbe(AotOutput, AotError, TEXT("native_binds_aot_check.das")) &&
                                      FFileHelper::LoadFileToString(AotText, *AotOutput);
    if (bAot && !AotText.Contains(TEXT("open_only")))
        AotError = TEXT("open_only is not compiled for AOT");
    else if (bAot && AotText.Contains(TEXT("with_closed")))
        AotError = TEXT("with_closed is compiled for AOT although it calls a bind without an AOT ABI");
    bAot = bAot && AotError.IsEmpty();
    const TCHAR* AotResult = bAot ? TEXT("PASS") : TEXT("FAIL");
    UE_LOG(LogTemp, Display, TEXT("DAS_NATIVE_BINDS_AOT=%s output=%s %s"), AotResult, *AotOutput, *AotError.Left(1000));
    return bManifest && bAot ? 0 : 1;
}
