//
// Created by Wavjaby on 2026/3/27.
//

#include "while.h"

#include <WJCL/string/wjcl_string.h>

#include "lib/code_gen.h"
#include "../compiler_util.h"

bool code_whileLoopStart() {
    compilerLog("> (while loop)\n");

    const ScopeData* scope = scope_pushType(SCOPE_WHILE_LOOP);

    buffPrintln(&ctx->code, "");
    buffPrintln(&ctx->code, "br label %%loop%d.body", scope->id);
    buffPrintlnS(&ctx->code, "loop%d.body:", scope->id);

    return false;
}

bool code_whileLoopEnd(Object* obj) {
    const ScopeData* scope = scope_peek();

    buffPrintln(&ctx->code, "br label %%loop%d.body", scope->id);
    buffPrintlnS(&ctx->code, "loop%d.exit:", scope->id);
    buffPrintln(&ctx->code, "");

    scope_dump();
    compilerLog("< (while loop end)\n");
    return false;
}

bool code_break() {
    // TODO: break out of the nearest enclosing loop
    //
    // Before implementing, read:
    //   1. scope_findNearestLoop() in scope.c:
    //      what does it return? what does loopScope->id < 0 mean?
    //   2. code_whileLoopEnd above and code_forLoopEnd in for.c:
    //      what is the naming convention for the loop exit label?
    //   3. the other compilerLog call formats in compiler_util.h
    //
    // Implementation (~4 lines):
    //   - find the nearest enclosing loop scope
    //   - if not inside a loop, report an error and return true
    //   - emit a compilerLog break message
    //   - emit a br to the exit label
    return false;
}
