#include "DasRuntime.h"
#include "Interfaces/IPluginManager.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "Modules/ModuleManager.h"

#pragma push_macro("verify")
#pragma push_macro("ensure")
#pragma push_macro("check")
#undef verify
#undef ensure
#undef check
THIRD_PARTY_INCLUDES_START
#include "daScript/daScript.h"
THIRD_PARTY_INCLUDES_END
#pragma pop_macro("check")
#pragma pop_macro("ensure")
#pragma pop_macro("verify")

DEFINE_LOG_CATEGORY_STATIC(LogDasRuntime, Log, All);

namespace
{
    class FUnrealTextFile final : public das::FileInfo
    {
    public:
        TArray<uint8> Bytes;
        virtual void getSourceAndLength(const char*& Source, uint32_t& Length) override
        {
            Source = reinterpret_cast<const char*>(Bytes.GetData());
            Length = static_cast<uint32_t>(Bytes.Num() - 1);
        }
    };

    // Reads through UE's platform file chain, including staged Pak/UFS files.
    class FUnrealFileSystem final : public das::AnyFileSystem
    {
    public:
        virtual das::FileInfo* tryOpenFile(const das::string& Name) override
        {
            auto File = MakeUnique<FUnrealTextFile>();
            if (!FFileHelper::LoadFileToArray(File->Bytes, UTF8_TO_TCHAR(Name.c_str())))
                return nullptr;
            File->Bytes.Add(0);
            File->name = Name;
            return File.Release();
        }
    };

    class FUnrealDasContext final : public das::Context
    {
    public:
        explicit FUnrealDasContext(uint32 StackSize) : das::Context(StackSize) {}
        TArray<FDasTypeDeclaration> Types;
    };

    thread_local FUnrealDasContext* ActiveContext = nullptr;

    struct FActiveContextScope
    {
        FUnrealDasContext* Previous;
        explicit FActiveContextScope(FUnrealDasContext& Context) : Previous(ActiveContext)
        {
            ActiveContext = &Context;
        }
        ~FActiveContextScope() { ActiveContext = Previous; }
    };

    int32 NativeAdd(int32 A, int32 B) { return A + B; }

    int32 EmitType(const char* Name, const char* Parent, const char* Fields, das::Context* Context)
    {
        // Copies strings while their owning context is alive. No UE objects or
        // compiled function pointers escape this execution in the foundation.
        if (Context != ActiveContext)
            Context->throw_error("UE schema emission is only allowed in the active runtime context");
        if (!Name || !Parent || !Fields)
            Context->throw_error("UE schema declaration strings must not be null");
        auto& UEContext = *ActiveContext;
        UEContext.Types.Add({ UTF8_TO_TCHAR(Name), UTF8_TO_TCHAR(Parent), UTF8_TO_TCHAR(Fields) });
        return UEContext.Types.Num();
    }

    class FUnrealFoundationModule final : public das::Module
    {
    public:
        FUnrealFoundationModule() : das::Module("ue_foundation")
        {
            das::ModuleLibrary Library(this);
            Library.addBuiltInModule();
            das::addExtern<DAS_BIND_FUN(NativeAdd)>(*this, Library, "ue_add", das::SideEffects::none, "NativeAdd");
            das::addExtern<DAS_BIND_FUN(EmitType)>(*this, Library, "ue_emit_type", das::SideEffects::modifyExternal, "EmitType");
        }
    };

    FString Errors(const das::ProgramPtr& Program, das::TextWriter& Output)
    {
        if (Program)
            for (const auto& Error : Program->errors)
                Output << das::reportError(Error.at, Error.what, Error.extra, Error.fixme, Error.cerr);
        const auto Text = Output.str();
        return UTF8_TO_TCHAR(Text.c_str());
    }
}

void FDasRuntimeModule::StartupModule()
{
    Root = FPaths::ConvertRelativePathToFull(IPluginManager::Get().FindPlugin(TEXT("DasForUE"))->GetContentDir() / TEXT("Das"));
    const FTCHARToUTF8 Utf8Root(*Root);
    das::setDasRoot(das::string(Utf8Root.Get(), Utf8Root.Length()));
    NEED_ALL_DEFAULT_MODULES;
    das::Module::Initialize();
    new FUnrealFoundationModule();
    bInitialized = true;
    UE_LOG(LogDasRuntime, Display, TEXT("daScript initialized; content: %s"), *Root);
}

void FDasRuntimeModule::ShutdownModule()
{
    if (bInitialized)
    {
        das::Module::Shutdown();
        bInitialized = false;
    }
}

FString FDasRuntimeModule::ScriptPath(const FString& Name) const
{
    return Root / TEXT("Scripts") / Name;
}

FDasExecutionResult FDasRuntimeModule::RunFile(const FString& Path, int32 Input, bool bUseAot)
{
    FDasExecutionResult Result;
    if (!IsInGameThread() || !bInitialized)
    {
        Result.Error = TEXT("daScript calls require an initialized runtime on the game thread.");
        return Result;
    }
    try
    {
        das::TextWriter Output;
        das::ModuleGroup Modules;
        auto Access = das::make_smart<das::FsFileAccess>();
        Access->addFileSystem(new FUnrealFileSystem(), true, false);
        const FTCHARToUTF8 Utf8Path(*Path);
        das::CodeOfPolicies Policies;
        Policies.aot = bUseAot;
        auto Program = das::compileDaScript(Utf8Path.Get(), Access, Output, Modules, Policies);
        if (!Program || Program->failed())
        {
            Result.Error = Errors(Program, Output);
            return Result;
        }
        FUnrealDasContext Context(Program->getContextStackSize());
        FActiveContextScope ActiveScope(Context);
        if (!Program->simulate(Context, Output))
        {
            Result.Error = Errors(Program, Output);
            return Result;
        }
        auto Function = Context.findFunction("main");
        if (!Function || !das::verifyCall<int32, int32>(Function->debugInfo, Modules))
        {
            Result.Error = TEXT("Expected exported main(input: int): int.");
            return Result;
        }
        vec4f Arguments[] = { das::cast<int32>::from(Input) };
        const vec4f Value = Context.evalWithCatch(Function, Arguments);
        if (const char* Exception = Context.getException())
        {
            Result.Error = UTF8_TO_TCHAR(Exception);
            return Result;
        }
        Result.bUsedAot = Function->aot;
        Result.Value = das::cast<int32>::to(Value);
        Result.Types = MoveTemp(Context.Types);
        Result.bSucceeded = true;
        return Result;
    }
    catch (const std::exception& Exception)
    {
        Result.Error = UTF8_TO_TCHAR(Exception.what());
        return Result;
    }
}

IMPLEMENT_MODULE(FDasRuntimeModule, DasRuntime)
