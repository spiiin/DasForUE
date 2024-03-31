#include "das_compile.h"

#include "das_config.h"
THIRD_PARTY_INCLUDES_START
#include "daScript/daScript.h"
THIRD_PARTY_INCLUDES_END

#include "TextPrinterUE.h"

#include "using_das.inc"

void compile_and_run_script(const char* fname) {
    TextPrinterUE tout;
    ModuleGroup dummyLibGroup;
    auto fAccess = make_smart<FsFileAccess>();

    auto program = compileDaScript(fname, fAccess, tout, dummyLibGroup);
    if (program->failed()) {
        tout << "failed to compile\n";
        for (auto& err : program->errors) {
            tout << reportError(err.at, err.what, err.extra, err.fixme, err.cerr);
        }
        return;
    }

    Context ctx(program->getContextStackSize());
    if (!program->simulate(ctx, tout))
    {
        tout << "failed to simulate\n";
        for (auto& err : program->errors) {
            tout << reportError(err.at, err.what, err.extra, err.fixme, err.cerr);
        }
        return;
    }

    auto fnTest = ctx.findFunction("main");
    if (!fnTest)
    {
        tout << "function 'main' not found\n";
        return;
    }

    if (!verifyCall<void>(fnTest->debugInfo, dummyLibGroup))
    {
        tout << "function 'main', call arguments do not match. expecting def main : void\n";
        return;
    }

    ctx.evalWithCatch(fnTest, nullptr);
    if (auto ex = ctx.getException())
    {
        tout << "exception: " << ex << "\n";
        return;
    }
}