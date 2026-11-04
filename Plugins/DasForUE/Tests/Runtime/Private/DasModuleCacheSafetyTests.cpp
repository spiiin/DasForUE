#include "DasRuntime.h"
#include "Misc/AutomationTest.h"
#include "Modules/ModuleManager.h"
#include "Misc/ScopeExit.h"
#pragma push_macro("verify")
#pragma push_macro("ensure")
#pragma push_macro("check")
#undef verify
#undef ensure
#undef check
THIRD_PARTY_INCLUDES_START
#include "daScript/simulate/simulate.h"
#pragma pack(push, 16)
#include "daScript/daScript.h"
#include "daScript/ast/ast_serializer.h"
#pragma pack(pop)
THIRD_PARTY_INCLUDES_END
#pragma pop_macro("check")
#pragma pop_macro("ensure")
#pragma pop_macro("verify")

namespace das { void writebackModules(ModuleGroup& Group); }

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FDasModuleCacheSafetyTest, "DasForUE.Runtime.ModuleCacheSafety",
                                EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FDasModuleCacheSafetyTest::RunTest(const FString& Parameters)
{
    FModuleManager::LoadModuleChecked<FDasRuntimeModule>(TEXT("DasRuntime"));
    das::gc_root Root;
    das::gc_active_scope Active(&Root);
    auto Reject = [&](das::gc_node* Wrong, const TCHAR* Name)
    {
        das::SerializationStorageVector Storage;
        Storage.buffer = {1, 0, 1, 'f'};
        das::AstSerializer Reader(&Storage, false);
        Reader.readNodes = {nullptr, Wrong};
        das::FunctionPtr Function = nullptr;
        const bool OK = Reader.trySerialize([&](das::AstSerializer& S) { S << Function; });
        TestFalse(Name, OK);
        TestTrue(TEXT("invalid record marked failed"), Reader.failed);
        TestTrue(TEXT("invalid node never escapes as Function"), Function == nullptr);
        TestEqual(TEXT("rejected before reading the name"), uint64(Storage.bufferPos), uint64(1));
    };
    Reject(new das::Structure(), TEXT("Structure reference rejected"));
    Reject(new das::Variable(), TEXT("Variable reference rejected"));
    auto* Expected = new das::Function();
    Expected->name = "f";
    das::SerializationStorageVector Storage;
    Storage.buffer = {1, 0, 1, 'f'};
    das::AstSerializer Reader(&Storage, false);
    Reader.readNodes = {nullptr, Expected};
    das::FunctionPtr Function = nullptr;
    TestTrue(TEXT("valid Function reference accepted"),
             Reader.trySerialize([&](das::AstSerializer& S) { S << Function; }));
    TestTrue(TEXT("valid identity preserved"), Function == Expected);
    // Reproduce the producer bug without relying on a particular UE adapter:
    // an unserializable module precedes an ordinary module in one writer stream.
    das::SerializationStorageVector Written;
    das::AstSerializer Writer(&Written, true);
    Writer.quietCache = true;
    auto Bad = das::make_smart<das::Program>();
    auto Good = das::make_smart<das::Program>();
    Bad->library.renameModule(Bad->thisModule.get(), "cache_boundary_bad");
    Good->library.renameModule(Good->thisModule.get(), "cache_boundary_good");
    Bad->thisModuleName = "cache_boundary_bad";
    Good->thisModuleName = "cache_boundary_good";
    Bad->isDependency = Good->isDependency = true;
    // Unknown type is a deterministic serializer error; it is never executed/read.
    auto* Unsupported = new das::TypeDecl();
    Unsupported->baseType = static_cast<das::Type>(255);
    Bad->thisModule->aliasTypes.insert("CannotSerialize", Unsupported);
    Writer.parsedModules.push_back({"cache_bad.das", 0, 0, Bad, Bad->thisModule.get(), {}});
    Writer.parsedModules.push_back({"cache_good.das", 0, 0, Good, Good->thisModule.get(), {}});
    das::ModuleGroup WrittenGroup;
    WrittenGroup.addBuiltInModule();
    {
        TGuardValue<das::AstSerializer*> Writing(das::daScriptEnvironment::getBound()->serializer_write, &Writer);
        das::writebackModules(WrittenGroup);
    }
    TestFalse(TEXT("cache write failure does not poison the source program"), Bad->failed());
    Written.flush();
    das::SerializationStorageVector ReadStorage;
    ReadStorage.buffer = Written.buffer;
    das::AstSerializer Records(&ReadStorage, false);
    Records.quietCache = true;
    ON_SCOPE_EXIT { Records.moduleLibrary = nullptr; };
    uint32_t Magic = 0, Version = 0;
    if (!TestTrue(TEXT("read stream header"), Records.trySerialize([&](das::AstSerializer& S) { S << Magic << Version; })))
        return false;
    TestEqual(TEXT("module stream magic"), Magic, uint32_t(0x4d534144));
    TestEqual(TEXT("module stream version"), Version, das::AstSerializer::getVersion());
    auto Header = [&](uint8_t& Flags, uint64_t& Length)
    {
        uint64_t Hash = 0;
        int64_t Size = 0;
        das::string File;
        uint32_t Dependencies = 0, Requires = 0;
        const bool OK = Records.trySerialize([&](das::AstSerializer& S) {
            S << Hash << Size;
            S.serializeTemp(File);
            S << Flags << Dependencies << Requires << Length;
        });
        return OK && Dependencies == 0 && Requires == 0 && Length <= ReadStorage.readRemaining();
    };
    uint8_t Flags = 0;
    uint64_t Length = 0;
    if (!TestTrue(TEXT("read failed record header"), Header(Flags, Length)))
        return false;
    TestEqual(TEXT("failed payload is explicitly reparse-only"), Flags, uint8_t(1));
    ReadStorage.bufferPos += size_t(Length);
    Records.clearNodeIds();
    if (!TestTrue(TEXT("read following record header"), Header(Flags, Length)))
        return false;
    TestEqual(TEXT("following valid payload remains cacheable"), Flags, uint8_t(0));
    const size_t PayloadEnd = ReadStorage.bufferPos + size_t(Length);
    auto Restored = das::make_smart<das::Program>();
    das::ModuleGroup RestoredGroup;
    RestoredGroup.addBuiltInModule();
    Records.thisModuleGroup = &RestoredGroup;
    TGuardValue<das::ProgramPtr> BoundProgram(das::daScriptEnvironment::getBound()->g_Program,
                                             das::daScriptEnvironment::getBound()->g_Program);
    Records.serializeProgram(Restored, RestoredGroup);
    TestFalse(TEXT("later AST restored without serializer failure"), Records.failed);
    TestFalse(TEXT("later program restored without errors"), Restored->failed());
    TestEqual(TEXT("later payload is complete"), uint64(ReadStorage.bufferPos), uint64(PayloadEnd));
    TestTrue(TEXT("later module identity restored"),
             Restored->thisModule && Restored->thisModule->name == "cache_boundary_good");
    return true;
}
