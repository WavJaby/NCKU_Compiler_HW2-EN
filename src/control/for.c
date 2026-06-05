//
// Created by WavJaby on 2026/03/26.
//

#include "for.h"

#include <WJCL/string/wjcl_string.h>

#include "lib/code_gen.h"
#include "compiler_util.h"

bool code_forLoop(Object* src) {
    if (src->type == OBJECT_TYPE_UNDEFINED)
        goto FAILED;

    compilerLog("> (for loop, count: %s)\n", object_print(src));

    // TODO: implement for-loop IR generation
    //   1. push a new scope; get the loop count operand string (promote to I32)
    //   2. set scope->u.forLoop.symbol based on src->type to record the counter type
    //   3. emit the full IR sequence: entry → header → phi → icmp → conditional branch → body label
    //   see LLVM_IR_CHEATSHEET.md §phi Node (for loop) for the full block structure and label naming

FAILED:
    object_free(src);
    return true;
}

bool code_forLoopEnd(Object* obj) {
    // TODO: emit the loop update/exit IR and pop the scope
    //   emit: update label → counter increment → jump back to header → exit label
    //   see LLVM_IR_CHEATSHEET.md §phi Node (for loop) for IR instructions and label naming
    return false;
}
