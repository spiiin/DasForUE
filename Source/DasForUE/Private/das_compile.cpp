#include "das_compile.h"
#include <memory>

//TODO: normal infrastructure
FCompiledScript g_compiledScript;

FCompiledScript compile_script(const char* fname) {
    FCompiledScript result;
    TextPrinterUE tout;
    ModuleGroup dummyLibGroup;
    auto fAccess = make_smart<FsFileAccess>();

    result.program = compileDaScript(fname, fAccess, tout, dummyLibGroup);
    if (result.program->failed()) {
        tout << "[DasForUE] Failed to compile\n";
        for (auto& err : result.program->errors) {
            tout << reportError(err.at, err.what, err.extra, err.fixme, err.cerr);
        }
        return result;
    }

    result.context = std::make_shared<Context>(result.program->getContextStackSize());
    if (!result.program->simulate(*result.context, tout))
    {
        tout << "[DasForUE] Failed to simulate\n";
        for (auto& err : result.program->errors) {
            tout << reportError(err.at, err.what, err.extra, err.fixme, err.cerr);
        }
        return result;
    }

    result.fnMain = result.context->findFunction("main");
    if (!result.fnMain)
    {
        tout << "[DasForUE] Function 'main' not found\n";
        return result;
    }

    if (!verifyCall<void>(result.fnMain->debugInfo, dummyLibGroup))
    {
        tout << "[DasForUE] Function 'main', call arguments do not match. expecting def main : void\n";
        return result;
    }

    result.fnAfterCompile = result.context->findFunction("on_after_compile");
    if (!result.fnAfterCompile)
    {
        tout << "[DasForUE] Function 'on_after_compile' not found\n";
        return result;
    }

    if (!verifyCall<void>(result.fnMain->debugInfo, dummyLibGroup))
    {
        tout << "[DasForUE] Function 'on_after_compile', call arguments do not match. expecting def on_after_compile : void\n";
        return result;
    }

    result.context->evalWithCatch(result.fnAfterCompile, nullptr);
    if (auto ex = result.context->getException())
    {
        tout << "[DasForUE] exception: " << ex << "\n";
        return result;
    }

    tout << "[DasForUE] compile ok\n";

    //TODO: no global
    g_compiledScript = result;

    return result;
}


void run_script() {
    TextPrinterUE tout;
    if (!g_compiledScript.program) {
        tout << "[DasForUE] g_compiledScript.program == null\n";
        return;
    }
    if (!g_compiledScript.fnMain) {
        tout << "[DasForUE] g_compiledScript.fnMain == null\n";
        return;
    }

    g_compiledScript.context->evalWithCatch(g_compiledScript.fnMain, nullptr);
    if (auto ex = g_compiledScript.context->getException())
    {
        tout << "[DasForUE] exception: " << ex << "\n";
        return;
    }
}

void clean_scripts() {
    g_compiledScript.program = nullptr;
    g_compiledScript.context = nullptr;
}


void compile_and_run_script(const char* fname) {
    compile_script(fname);
    run_script();
}
