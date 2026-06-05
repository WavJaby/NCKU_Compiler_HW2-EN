//
// Created by Wavjaby on 2026/3/26.
//

#include "if.h"

#include <WJCL/string/wjcl_string.h>

#include "lib/code_gen.h"
#include "compiler_util.h"

inline bool code_if(Object* src) {
    compilerLog("> (if)\n");
    // TODO: implement if-branch IR generation
    //   1. get the condition operand string
    //   2. push a new scope; initialize elseifCount and containsElse
    //   3. emit the conditional branch IR, naming the true/false labels using scope->id
    //   4. emit the true label (use buffPrintlnS)
    //   5. clean up Object and return false
    //   see LLVM_IR_CHEATSHEET.md §if / elseif / else Structure for the block layout
    return false;
}

inline bool code_elseIfLabel() {
    // TODO: close the previous if/elseif branch and prepare to enter the next elseif
    //   1. emit an unconditional branch to the endif label
    //   2. emit the previous segment's false label (first time: if.false; thereafter: elseif<n>.false)
    //   3. update elseifCount
    //   see LLVM_IR_CHEATSHEET.md §if / elseif / else Structure for label naming
    return false;
}

inline bool code_elseIf(Object* src) {
    compilerLog("> (else if)\n");
    // TODO: pop the current scope, push a new scope with the same id, and continue accumulating elseifCount
    //   1. read scopeId and elseifCount from the old scope, then scope_dump()
    //   2. scope_pushId with the same id and elseifCount + 1
    //   3. get the condition operand string; emit the conditional branch IR (true/false labels include elseifCount)
    //   4. emit the elseif true label, clean up Object, return false
    return false;
}

inline bool code_else() {
    // TODO: switch scope and emit the else entry label
    //   1. read the current scope info, then scope_dump()
    //   2. emit an unconditional branch to endif, then emit the previous segment's false label
    //   3. scope_pushId with the same id and set containsElse=true
    //   label naming follows the same rules as code_elseIfLabel; see LLVM_IR_CHEATSHEET.md
    return false;
}

inline bool code_ifEnd() {
    // TODO: emit the if-end IR based on scope state, then pop the scope
    //   three cases each require a different label combination (see LLVM_IR_CHEATSHEET.md §if Structure):
    //   - has else: jump to endif at the end of the body; endif label serves as the merge point
    //   - no else, no elseif: fall through directly to the false label after the body
    //   - has elseif but no else: last elseif's false label + endif merge point
    //   call scope_dump() at the end
    return false;
}
