# NCKU 1142 Compiler Construction — Homework 2: Wenyan LLVM Compiler

This assignment builds on the lexer (`compiler.l`) from Homework 1 and requires you to implement:
1. YACC grammar rules (`compiler.y`)
2. Symbol table management (two functions in `scope.c`)
3. Object system (partial functions in `object.c` and `value_data.c`)
4. Core semantic action functions (`main.c` partial functions)
5. Control-flow IR generation (`control/for.c`, `if.c`, `while.c`)
6. Function-definition symbol registration (two functions in `control/function.c`)

**The utility library (`lib/`) and Runtime (`wy_rt/`) are fully provided — you do not need to implement them.**

---

## Document Overview

| Document | Purpose | When to read |
|----------|---------|--------------|
| [Assignment Spec](114-2%20Compiler%20Homework%202.md) | Assignment description, YACC concepts intro, four-week schedule | **Read first** |
| This file (`README.md`) | Environment setup, fill-in specs, utility function reference | Read through before starting; refer back while implementing |
| [`YACC_CHEATSHEET.md`](YACC_CHEATSHEET.md) | Advanced `compiler.y` syntax: `$<type>N`, `$0`/`$-1`, mid-rule actions, shift/reduce conflict debugging | Refer to when you hit problems in `compiler.y` |
| [`LLVM_IR_CHEATSHEET.md`](LLVM_IR_CHEATSHEET.md) | All IR instruction references, phi nodes, if/for/while block structures | Refer to when implementing IR generation functions |
| [`CHANGELOG.md`](CHANGELOG.md) | Post-release fixes, whether you need to change your own code, how to merge | check before every `git pull`/`merge` |

---

## Environment Setup

### Getting the Assignment Files

```bash
git clone --recurse-submodules https://github.com/WavJaby/NCKU_Compiler_HW2-EN.git NCKU_Compiler_HW2
cd NCKU_Compiler_HW2
```

If you already cloned without `--recurse-submodules`:

```bash
git submodule update --init
```

### Version Requirements

| Tool | Minimum version | Notes |
|------|----------------|-------|
| `cmake` | 3.10 | Build system |
| `flex` | 2.6 | Lexer generator |
| `bison` | 3.6 (≥ 3.8 for counterexample hints) | Parser generator |
| `gcc` | C11 support | Compiler and linker |
| `llvm` / `llc` | 14 | IR → executable |

### Windows (MSYS2)

MSYS2 was installed in the previous assignment. Open an **MSYS2 UCRT64** terminal and run:

```bash
# Install required tools (skip if already installed)
pacman -S bison flex mingw-w64-ucrt-x86_64-cmake \
          mingw-w64-ucrt-x86_64-gcc mingw-w64-ucrt-x86_64-llvm

# Build
cmake -B build -S . -G "MinGW Makefiles"
cmake --build build
```

> Use the **UCRT64** terminal, not the MSYS terminal — the toolchain paths are incompatible.

### Linux (Ubuntu / Debian)

```bash
sudo apt install cmake flex bison gcc llvm

cmake -B build -S .
cmake --build build
```

### macOS

```bash
brew install cmake flex bison llvm

cmake -B build -S .
cmake --build build
```

### Verify the build

```bash
./test/test.sh -n    # -n skips rebuild; remove -n on first run
```

On Windows (native PowerShell, outside MSYS2):

```powershell
.\test\test.ps1 -NoCompile   # -NoCompile skips rebuild
```

### Test script options (`test.sh` / `test.ps1`)

| Short flag | PowerShell param | Description |
|-----------|-----------------|-------------|
| `-f <name>` | `-File <name>` | Filter test cases by filename substring |
| `-s` | `-Stop` | Stop on the first failure |
| `-n` | `-NoCompile` | Skip CMake rebuild |
| `-i` | `-Interactive` | Interactive diff (pager mode) |
| `-b <dir>` | `-BuildDir <dir>` | Specify a custom build directory |

---

## Adjustments Needed in Your HW1 `compiler.l`

The provided `compiler.l` starter file contains:
- **`%{...%}` section**: Pre-updated with Bison-required includes, `yylloc` position tracking, and unrecognized-character buffering
- **Base rules**: `.`、`,`、whitespace, newline, EOF, unrecognized characters — already provided
- **Token rules section (TODO)**: Empty — paste your HW1 output and make the two adjustments below

### 1. Change each rule to set `yylval` and return a token

Change each rule from `PRINT_TOKEN(...)` to:
- Write the token's value into the appropriate `yylval` field
- Call `return TOKEN_NAME` to hand the token to Bison

Which tokens need `yylval` and which field to use: cross-reference the `%union` definition and `%token <field>` declarations at the top of `compiler.y`.
Keywords with no `<field>` declaration only need `return`.

### 2. Add or modify two rules

| Item | Description |
|------|-------------|
| Add `"以施"` | Corresponds to token `TO_CALL`; postfix function call syntax (not in HW1) |
| Modify `"乃得"` | Add optional suffix `矣`: `"乃得""矣"?` |

---

## File Breakdown

### Entry & Driver

| File | Status | Description |
|------|--------|-------------|
| `src/wyc.c` | ✅ Fully provided | Compiler entry point; CLI argument handling |
| `src/compiler.l` | ✏️ HW1 output + modifications | Flex lexer rules (set `yylval` + `return`; `%{%}` section already provided) |
| `src/compiler.y` | ✏️ Fill in | Bison grammar rules and semantic actions |
| `src/compiler.h` | ✅ Fully provided | `yyerror` implementation |
| `src/compiler_util.c` / `.h` | ✅ Fully provided | Error messages, compile state, shared macros |

### Semantic Analysis & Symbol Table

| File | Status | Description |
|------|--------|-------------|
| `src/scope.c` / `.h` | ✏️ Fill in | Symbol table (`scope_addSymbol`, `scope_findSymbol`) |
| `src/object.c` / `.h` | ✏️ Fill in | Object value system; array index IR (`object_getIndex`) already provided |
| `src/object_type.h` | ✅ Fully provided | Type enum (`ObjectType`) |
| `src/value_data.c` / `.h` | ✏️ Fill in | Multi-value declaration container (`ValueData`) |
| `src/main.c` / `.h` | ✏️ Fill in | Core semantic action functions |
| `src/expression.c` / `.h` | ✏️ Fill in | Expression IR generation (`code_expression`, `code_expressionChain`, etc.) |

### Control-Flow IR Generation

| File | Status | Description |
|------|--------|-------------|
| `src/control/for.c` / `.h` | ✏️ Fill in | for-loop IR generation |
| `src/control/if.c` / `.h` | ✏️ Fill in | if/else IR generation |
| `src/control/while.c` / `.h` | ✏️ Fill in | while-loop IR generation |
| `src/control/function.c` / `.h` | ✅ Fully provided | All C functions provided; Week 4 work is in `compiler.y` function grammar rules |
| `src/control/function_info.h` | ✅ Fully provided | `FuncInfo`, `FuncCallInfo`, `CapturedEntry` struct definitions |
| `src/control/function_builtin.h` | ✅ Fully provided | Built-in function definitions |

### Utility Library (`src/lib/`)

| File | Status | Description |
|------|--------|-------------|
| `src/lib/code_gen.c` / `.h` | ✅ Fully provided | LLVM IR output utilities (`buffPrintln`, `ByteBuffer`, etc.) |
| `src/lib/byte_buffer.c` / `.h` | ✅ Fully provided | Dynamic string buffer |
| `src/lib/chinese_number.c` / `.h` | ✅ Fully provided | Chinese numeral conversion (`chineseToArabic`) |
| `src/lib/console_color.c` / `.h` | ✅ Fully provided | Terminal color output |
| `src/lib/tab_width.h` | ✅ Fully provided | Tab width calculation |
| `src/lib/utf8_console.h` | ✅ Fully provided | UTF-8 terminal output helpers |

### Runtime

| File | Status | Description |
|------|--------|-------------|
| `src/wy_rt/wy_rt.c` / `.h` | ✅ Fully provided | Runtime library (string ops, array ops, print) |

### External Libraries (`lib/`)

| Directory | Description |
|-----------|-------------|
| `lib/WJCL/` | Data structure library (LinkedList, HashMap, etc.) |
| `lib/utf8.c/` | UTF-8 string processing |

---

## Fill-in Module Specifications

### Function fill-in overview

| File | Function | Type | Notes |
|------|----------|------|-------|
| `compiler.y` | Grammar rules (see next section) | Full fill-in | — |
| `scope.c` | `scope_addSymbol` | Partial fill-in | Simplified version provided (usable for Quick Start); add duplicate-symbol check |
| `scope.c` | `scope_findSymbol` | Partial fill-in | Closure cross-context capture branch provided; add `searchCtx == ctx` return |
| `object.c` | `object_createStrConst` | ✅ Fully provided | — |
| `object.c` | `object_createStr` | ✅ Fully provided | — |
| `object.c` | `object_createArray` | ✅ Fully provided | — |
| `object.c` | `object_createNumber` | ✅ Fully provided | — |
| `object.c` | `object_createBool` | ✅ Fully provided | — |
| `object.c` | `object_loadRegAndPromote` | ✅ Fully provided | Gets operand string and promotes type (NUM→I32, etc.); essential tool for fill-in functions like `code_expression` |
| `object.c` | `object_nameLiteralOrLoadReg` | Partial fill-in | SYMBOL normal load / I32/I64/F64 / BOOL / STR provided; add REGISTER (Week 2), capturedIndex / funcArg / FUNC (Week 4) |
| `value_data.c` | `object_ValueDataListCreate` | Partial fill-in | Simplified version provided (Quick Start usable); add count validation |
| `value_data.c` | `object_ValueDataListAdd` | Partial fill-in | Simplified version provided (Quick Start usable); add type check and count cap |
| `value_data.c` | `object_ValueDataListAddDefaults` | Full fill-in | Add zero values for each type (needed from `02_流轉_易值` onward) |
| `value_data.c` | `object_ValueDataListPop` | ✅ Fully provided | — |
| `value_data.c` | `object_ValueDataListFree` | ✅ Fully provided | — |
| `main.c` | `code_stdoutPrintObject` | Partial fill-in | `object_nameLiteralOrLoadReg` call and BOOL branch provided; add `@printf` call for I32/I64/F64/STR |
| `main.c` | `code_stdoutPrint` | ✅ Fully provided | — |
| `main.c` | `code_createVariable` | Partial fill-in | Simplified version provided (Quick Start usable); add `object_nameLiteralOrLoadReg` support for symbol references and non-numeric types |
| `main.c` | `code_assign` | Full fill-in | dest type validation provided; everything else is fill-in |
| `main.c` | `code_getLength` | Full fill-in | Type validation provided; everything else is fill-in |
| `main.c` | `code_arrayPush` | ✅ Fully provided | — |
| `expression.c` | `object_sameRegister` | ✅ Fully provided | static helper |
| `expression.c` | `isExpressionOperationLegal` | ✅ Fully provided | static helper |
| `expression.c` | `code_expression` | Partial fill-in | Type inference (`getPromotedType`) provided; add register allocation, `opLeft` direction, operand loading, IR instruction, return |
| `expression.c` | `code_expressionMod` | ✅ Fully provided | thin wrapper with preceding division check |
| `expression.c` | `code_expressionChain` | ✅ Fully provided | thin wrapper |
| `expression.c` | `code_expressionChainMod` | ✅ Fully provided | thin wrapper |
| `control/for.c` | `code_forLoop` | Full fill-in | undefined check provided |
| `control/for.c` | `code_forLoopEnd` | Full fill-in | — |
| `control/if.c` | `code_if` | Full fill-in | compilerLog provided |
| `control/if.c` | `code_elseIfLabel` | Full fill-in | — |
| `control/if.c` | `code_elseIf` | Full fill-in | compilerLog provided |
| `control/if.c` | `code_else` | Full fill-in | — |
| `control/if.c` | `code_ifEnd` | Full fill-in | — |
| `control/while.c` | `code_whileLoopStart` | ✅ Fully provided | — |
| `control/while.c` | `code_whileLoopEnd` | ✅ Fully provided | — |
| `control/while.c` | `code_break` | Full fill-in | Guided TODO: read `scope_findNearestLoop`, review provided `whileLoopEnd`/`forLoopEnd`, then implement (~4 lines) |
| `control/function.c` | `func_define` | ✅ Fully provided | — |
| `control/function.c` | `func_defineAddParam` | ✅ Fully provided | — |
| `control/function.c` | `func_defineBodyEnd` | ✅ Fully provided | — |
| `control/function.c` | `code_return` | ✅ Fully provided | — |

**Functions fully provided per file (no fill-in needed):**

| File | Fully provided functions |
|------|--------------------------|
| `scope.c` | `scope_push*`, `scope_dump`, `scope_peek`, `scope_getFunction`, `scope_findNearestLoop`, `context_push`, `context_pop`, `scope_free_all`, etc. |
| `object.c` | `object_loadRegAndPromote`, `object_getIndex`, `object_packAsPtr`, `object_createRegisterSymbol`, `object_print`, `object_free`, `object_getValueType`, `object_getPromotedType`, `symbol_clone`, `symbol_freeReg`, `symbol_freeSelf`, etc. |
| `main.c` | `code_arrayPush`, `writeOutputHeader`, `writeOutputMain`, `freeAll`, `main` |
| `control/while.c` | `code_whileLoopStart`, `code_whileLoopEnd` |
| `control/function.c` | `func_defineBody`, `func_defineBodyEnd`, `code_return`, `code_returnValue`, `func_callInit`, `func_callArgAdd`, `func_call`, `func_takeAndCall` |

---

## Utility Function Reference

Framework functions and macros you will call directly in your fill-in implementations.

### IR Output

| Function | Purpose | Notes |
|----------|---------|-------|
| `buffPrintln(&ctx->code, fmt, ...)` | Write one line to the current context's IR buffer | `%` must be written as `%%`; indentation auto-adjusts to scope depth |
| `compilerLog(fmt, ...)` | Output verbose log (shown with `-v`; includes position prefix) | Same format as printf |
| `lexerLog(fmt, ...)` | Output lexer token log (shown with `-l`; includes position prefix) | Only for use in `.l` rules; not shown by `-v` |

### Symbol Table / HashMap

| Function | Purpose |
|----------|---------|
| `map_putpp(&map, key, value)` | Insert key/value (both pointers); duplicate keys are overwritten |
| `map_get(&map, key)` | Look up; returns NULL if not found |

### LinkedList

| Function | Purpose |
|----------|---------|
| `linkedList_init(&list)` | Allocate sentinel node and initialize; **must** be called after declaring a list variable |
| `linkedList_addp(&list, freeFlag, ptr)` | Append to tail; `freeFlag=1` means `free(value)` on `deleteNode`, `0` means don't auto-free |
| `linkedList_deleteNode(&list, node)` | Delete the specified node (does **not** free value unless freeFlag=1) |
| `linkedList_freeA(&list, freeFn)` | Call `freeFn` on each value, then free all nodes (including sentinel); do not use after this |
| `linkedList_foreach(&list, node)` | Loop macro; `node` is the current `LinkedListNode*`; iterates head to tail |

### Memory / Strings

| Function | Purpose |
|----------|---------|
| `cloneStruct(Type, ptr)` | `malloc(sizeof(Type))` + `memcpy`; returns new pointer |
| `sciToStr(sn)` | `ScientificNotation*` → heap string; caller must `free()` |
| `strdup(s)` | Copy C string into newly allocated memory (standard POSIX) |

### Object Operations

#### `object_nameLiteralOrLoadReg` — Core operand conversion (partial fill-in)

```c
Object object_nameLiteralOrLoadReg(const Object* src, char* buffer, size_t bufferLen);
```

Accepts any `Object`, converts it to an LLVM IR operand string ready to paste into an instruction, writes it into `buffer`, and returns an Object representing the "ready value."

Nearly every semantic action needs to pass through this function (or its wrapper `object_loadRegAndPromote`) before using an Object's value.

| `src->type` | Written to buffer | Return | Week |
|-------------|------------------|--------|------|
| `REGISTER` | `%%reg<name>` | `*src` (no copy) | **Week 2 TODO** |
| `SYMBOL` (normal variable) | `%%reg<N>` | new REGISTER (`cloneStruct`) | ✅ Provided; emits `load` IR |
| `SYMBOL` (capturedIndex≥0) | `%%upval.<idx>` | new REGISTER (`symbol_clone`) | Week 4 TODO |
| `SYMBOL` (funcArg) | `%%var.<idx>`, no load | new REGISTER (`symbol_clone`) | Week 4 TODO |
| `SYMBOL` (FUNC type) | `@func_<ctxId>_<idx>` | new REGISTER (`symbol_clone`) | Week 4 TODO |
| `I32 / I64 / F64` | number string | `*src` | ✅ Provided |
| `BOOL` | `0` or `1` | `*src` | ✅ Provided |
| `STR` | `@str.<N>` (written to constBuff) | `*src` | ✅ Provided |

**Memory rules:**
- Returns REGISTER and `src->type == OBJECT_TYPE_SYMBOL` → return value is newly allocated; **call `object_free` after use**
- All other cases return `*src` (borrowed); **do not** free the return value

#### Other Object utilities

| Function | Signature | Purpose |
|----------|-----------|---------|
| `object_loadRegAndPromote` | `(src, targetType, buffer, bufferLen)` | Calls `object_nameLiteralOrLoadReg` then promotes type if needed (I32→I64, I32→F64, etc.); use this (not the above) for IR instructions that require consistent types (`add`, `icmp`, etc.) |
| `scope_dump` | `()` | Pop the current scope (prints symbol table in `-v` mode); call at the end of every for/if/while/function block |
| `object_createRegisterSymbol` | `(type)` | Allocate the next register index; returns `SymbolData` (not a pointer); used as the LHS of IR instructions |

### Error Reporting

| Function | Purpose |
|----------|---------|
| `yyerrorf(fmt, ...)` | Report a semantic error (no location); sets `compileError = true` |
| `yyerrorlf(fmt, loc, ...)` | Same, but includes `YYLTYPE` location info |

> `yyerrorf` / `yyerrorlf` **do not stop parsing automatically** — you must add `YYABORT` yourself (see YACC_CHEATSHEET.md §YYABORT).

### LLVM Built-ins and Global Constants

`writeOutputHeader` declares the following at the top of the IR output; use them directly in your fill-in code.

**`@printf` format strings**

Naming: `@fmt_<llvmType>[_n]`; `_n` = with newline (`\n`).

| Global constant | C format | Type |
|----------------|----------|------|
| `@fmt_i32` / `@fmt_i32_n` | `%d` | I32 |
| `@fmt_i64` / `@fmt_i64_n` | `%lld` | I64 |
| `@fmt_double` / `@fmt_double_n` | `%g` | F64 |
| `@fmt_ptr` / `@fmt_ptr_n` | `%s` | STR |

The mapping between `llvmType` and `ObjectType` is provided by `objectType2llvmType[srcValueType]`, usable directly in format string names:

```llvm
call i32 (ptr, ...) @printf(ptr @fmt_i32_n, i32 %reg0)
```

**Boolean output strings**

| Global constant | Content |
|----------------|---------|
| `@str_true` / `@str_true_n` | `陽` / `陽\n` |
| `@str_false` / `@str_false_n` | `陰` / `陰\n` |

Used with `select` + `fwrite` (`sizeof("陽") - 1` = byte length excluding null; add 1 for the newline variant).

**Other constants**

| Global constant | Description |
|----------------|-------------|
| `@space` | Space character (1 byte); use `@fwrite` to print inter-value spaces |

**`%g_stdout`**

`FILE*` loaded from `@stdout` at the start of `@main`, valid for the entire `@main`:

```llvm
%g_stdout = load ptr, ptr @stdout
```

`@fwrite` signature: `declare i64 @fwrite(ptr data, i64 size, i64 count, ptr stream)`

---

## Quick Start — First Statement

**Goal**: Make the following statement produce verbose output, confirming that the entire pipeline — parsing, symbol table, and IR generation — works end to end.

```
吾有一數。曰一。名之曰「甲」。
```

Expected verbose output (first line emitted by the framework automatically):

```
test/策問/00_快速入門.wy:1:2     |> (scope id: 0, type: SCOPE_MAIN)
test/策問/00_快速入門.wy:1:12    |    var 「甲」 <- 1
```

---

### Step 1 — Write the rule directly in `OperationStmt` (no sub-rules yet)

Put the full token sequence for `吾有一數。曰一。名之曰「甲」。` **directly in `OperationStmt`** without creating any sub-stmts:

| # | token | Wenyan text |
|---|-------|------------|
| $1 | `HERE_ARE` | 吾有 (`"吾有"\|"今有"` → `HERE_ARE`; standalone `"有"` is `HERE_IS_A`) |
| $2 | `NUMBER_LIT` | 一 (declaration count) |
| $3 | `VAR_TYPE` | 數 |
| $4 | `SAID` | 曰 (value follows) |
| $5 | `NUMBER_LIT` | 一 (initial value) |
| $6 | `NAME_IT` | 名之曰 (`"名之曰"` is a single token; 曰 is included) |
| $7 | `IDENT` | 「甲」 (first `IDENT` in `VariableDefineStmt` needs no preceding `SAID`) |

> **Note**: After Quick Start is working, move these token sequences into sub-rules (`CreateValueDataListStmt`, `ValueLiteralStmt`, `VariableDefineStmt`). `OperationStmt` is just temporary scaffolding for Step 1.

Add the following rule to `OperationStmt` in `compiler.y`:

```yacc
OperationStmt
    : HERE_ARE NUMBER_LIT VAR_TYPE SAID NUMBER_LIT NAME_IT IDENT {
        ValueData valData;
        object_ValueDataListCreate($<var_type>3, &$<n_var>2, &valData); /* create container: type=number, count=one */
        Object num = object_createNumber(&$<n_var>5);                   /* wrap initial value */
        object_ValueDataListAdd(&valData, &num, &@5);                   /* add to container */
        code_createVariable(&valData, $<s_var>7);                       /* create variable, emit IR + verbose */
        object_ValueDataListFree(&valData);                             /* free container */
    }
;
```

`$N` mapping:

| `$N` | token | Access | Description |
|------|-------|--------|-------------|
| `$2` | `NUMBER_LIT` | `$<n_var>2` | Count "one" (passed as count to Create) |
| `$3` | `VAR_TYPE` | `$<var_type>3` | Type "number" |
| `$5` | `NUMBER_LIT` | `$<n_var>5` | Initial value "one" (passed to createNumber) |
| `$7` | `IDENT` | `$<s_var>7` | Variable name "甲" (passed to createVariable; freed internally) |
| `@5` | — | `&@5` | Source location of token $5; used for error message positioning |

Why manual `$<type>N` annotation: `NUMBER_LIT` is declared with `%token <n_var>`, so `$2` would technically work unambiguously; same for `VAR_TYPE`. Manual annotation is only **required** when the token has no `%token <field>` declaration — but annotating consistently here makes the intent explicit and matches the style used throughout the rest of the grammar.

---

### Step 2 — All functions needed for Quick Start are already provided

No C functions need to be implemented for Quick Start. The following simplified versions are all ready to call:

| File | Function | Simplified version limitation (fill in later) |
|------|----------|-----------------------------------------------|
| `scope.c` | `scope_addSymbol` | No duplicate symbol check |
| `object.c` | `object_createNumber` etc. | Full version; no limitations |
| `value_data.c` | `object_ValueDataList*` | No type check, no multi-value validation |
| `main.c` | `code_createVariable` | Only supports numeric literals (no symbol refs, booleans, or strings) |

`scope_findSymbol` and `object_nameLiteralOrLoadReg` are not needed for this step.

---

### Step 3 — Verify

```bash
./test/test.sh -f 00_快速入門
```

The test has two parts: Part 1 compares verbose output; Part 2 compares runtime output. Quick Start only implements the first statement — Part 1 will show FAIL, but the diff will show **how many lines passed**, so you can track progress.

Quick Start is complete once the first two lines appear in the verbose diff:

```
test/策問/00_快速入門.wy:1:2     |> (scope id: 0, type: SCOPE_MAIN)
test/策問/00_快速入門.wy:1:12    |    var 「甲」 <- 1
```

---

## Quick Start Advanced — Read Variable + Print

**Prerequisite**: Quick Start completed; `甲` is declared in scope.

**Goal**: Make this statement produce verbose PRINT output and print the actual value:

```
吾有一數。曰「甲」。書之。
```

Expected verbose output:

```
test/策問/00_快速入門.wy:2:11    |    PRINT: 「甲」
```

---

### Step 1 — Add a new rule in `OperationStmt`

| # | token | Wenyan text |
|---|-------|------------|
| $1 | `HERE_ARE` | 吾有 |
| $2 | `NUMBER_LIT` | 一 (count) |
| $3 | `VAR_TYPE` | 數 |
| $4 | `SAID` | 曰 |
| $5 | `IDENT` | 「甲」 (variable name; requires symbol table lookup) |
| $6 | `PRINT` | 書之 |

```yacc
| HERE_ARE NUMBER_LIT VAR_TYPE SAID IDENT PRINT
    {
        ValueData valData;
        object_ValueDataListCreate($<var_type>3, &$<n_var>2, &valData);
        Object var = scope_findSymbol($<s_var>5);     /* look up 甲 in symbol table */
        if (var.type == OBJECT_TYPE_UNDEFINED) YYABORT;
        object_ValueDataListAdd(&valData, &var, &@5);
        code_stdoutPrint(&valData, true);             /* provided; calls object_nameLiteralOrLoadReg internally */
        object_ValueDataListFree(&valData);
    }
```

> `scope_findSymbol` returns `OBJECT_TYPE_SYMBOL`; `object_print` formats it as `「甲」`, so the verbose log shows `PRINT: 「甲」`.

---

### Step 2 — Functions to implement

**`scope_findSymbol` — add the `searchCtx == ctx` branch**

Find the TODO in `scope.c`; return format matches the provided closure branch:

```c
return (Object){OBJECT_TYPE_SYMBOL, .capturedIndex = -1, .value.symbol = symbol};
```

> `object_nameLiteralOrLoadReg` is fully provided, including all type branches — no implementation needed.

**`code_stdoutPrintObject` — add the `@printf` call for I32**

Find the TODO in `main.c`; add a `buffPrintln` call before the `break` in `case OBJECT_TYPE_I32:`:

```
call i32 (ptr, ...) @printf(ptr @fmt_i32_n, i32 %regN)
```

- `objectType2llvmType[srcValueType]` gives the LLVM type string (`"i32"`)
- When `newLine` is `true`, use `@fmt_i32_n`; otherwise `@fmt_i32`
- `regName` is the operand string written by the previous `object_nameLiteralOrLoadReg` call
- Format string naming rules: see README.md §LLVM Built-ins and Global Constants

> Quick Start Advanced only needs I32 (`一數`); I64/F64/STR follow the same pattern and are left for Week 1 completion.

---

### Step 3 — Verify

```bash
./test/test.sh -f 00_快速入門
```

Quick Start Advanced is complete when the verbose diff passes completely and Part 2 outputs `1`.

---

## After Quick Start — Refactoring Direction

The long `OperationStmt` rule from Quick Start is temporary scaffolding. The first task in Week 1 is to break it into the existing sub-rule skeletons.

### `BodyStmt` dispatch (skeleton provided — no modification needed)

```
BodyStmt → COMMENT STR_LIT
         | OperationStmt
         | ConditionStmt
         | FunctuonStmt
```

### Sub-rule skeletons and token sequences

The following non-terminals are already declared in the `compiler.y` skeleton (as empty rules); add token sequences when filling them in.

**`CreateValueDataListStmt`** — declared with `%type <val_data>`; `$$` is the ValueData container

| Syntax | Token sequence |
|--------|---------------|
| 吾有一數 (HERE_IS_A) | `HERE_IS_A VAR_TYPE` |
| 吾有三數 (HERE_ARE) | `HERE_ARE NUMBER_LIT VAR_TYPE` |

semantic action: `object_ValueDataListCreate($<var_type>N, count_ptr, &$$)`

---

**`ValueLiteralStmt`** — pure literal values; add `%type <obj_val>` yourself

| Syntax | Token sequence | semantic action |
|--------|---------------|-----------------|
| 曰一 (number) | `NUMBER_LIT` | `object_createNumber` |
| 曰陽 / 曰陰 (boolean) | `BOOL_LIT` | `object_createBool` |
| 曰「你好」 (string) | `STR_LIT` | `object_createStr` |

> `ValueLiteralStmt` does **not** include `SAID`; `SAID` stays in the parent rule (`OperationStmt` or `LitOrVarStmt`).

---

**`VariableStmt`** — IDENT looks up symbol table; add `%type <obj_val>` yourself

| Syntax | Token sequence | semantic action |
|--------|---------------|-----------------|
| 「甲」 (variable reference) | `IDENT` | `scope_findSymbol($<s_var>1)` |

---

**`LitOrVarStmt`** — literal or `SAID` + variable reference (`SAID` consumed here); add `%type <obj_val>` yourself

| Syntax | Token sequence |
|--------|---------------|
| number / boolean / string | `SAID ValueLiteralStmt` |
| variable reference | `SAID VariableStmt` |

semantic action: `$$ = $2` (pass-through)

---

**`VariableDefineStmt`** — naming; no return value needed

| Syntax | Token sequence | Notes |
|--------|---------------|-------|
| 名之曰「甲」 | `NAME_IT IDENT` | First name |
| 曰「乙」 | `SAID IDENT` | Subsequent names (repeated for multi-value declarations) |

---

### Refactored OperationStmt (single-value declaration)

The original Quick Start long rule maps to:

```
OperationStmt
    : CreateValueDataListStmt LitOrVarStmt { add $2 to $1; } VariableDefineStmt
      /* $1 = val_data; mid-rule action adds $2 to container; VariableDefineStmt reads container via $<val_data>0 */
    | CreateValueDataListStmt LitOrVarStmt { add $2 to $1; } PRINT
      /* print statement: call code_stdoutPrint */
    ;
```

`VariableDefineStmt` reads `$<val_data>0` to get the `val_data` stored by the left-side mid-rule action (`$0` refers to the most recent mid-rule action or shifted value).

> **Important**: `%type` declarations determine the type of `$$`; add the following to the top of `compiler.y` for `ValueLiteralStmt`, `VariableStmt`, and `LitOrVarStmt`:
> ```yacc
> %type <obj_val> ValueLiteralStmt VariableStmt LitOrVarStmt
> ```
> Mid-rule action and `$0` usage details: see YACC_CHEATSHEET.md §Mid-Rule Action.

---

## Key Data Structure Reference

```
CompilerContext (ctx)
├── scopeStack: LinkedList<ScopeData>   ← scope stack (tail = current scope)
├── variableCount: int                  ← index for the next variable
├── registerCount: int                  ← index for the next register
└── code: ByteBuffer                    ← current context's LLVM IR output

ScopeData
├── symbolMap: Map<char*, SymbolData*>  ← symbol table for this scope
├── type: ScopeType                     ← MAIN / FUNCTION / FOR_LOOP / IF_STMT etc.
└── u.funcSymbol: SymbolData*           ← if type == SCOPE_FUNCTION, points to function symbol

SymbolData
├── type: ObjectType                    ← type (I32 / F64 / BOOL / FUNC etc.)
├── name: char*                         ← symbol name
├── index: int32_t                      ← variable index in LLVM
├── funcInfo: FuncInfo*                 ← if function: contains parameter list and captured variables
└── funcArg: bool                       ← whether this is a function parameter

Object                                  ← semantic value of a yacc non-terminal
├── type: ObjectType
├── capturedIndex: int                  ← -1=local, >=0=upvalue index (pre-computed)
└── value.symbol: SymbolData*           ← pointer into the symbol table
```

---

## Wenyan Language Reference Resources

Understanding basic wenyan syntax before implementing semantic actions makes it much easier to understand token sequences.

| Resource | Description | Use for |
|----------|-------------|---------|
| [Syntax Cheatsheet (Wiki)](https://github.com/wenyan-lang/wenyan/wiki/Syntax-Cheatsheet) | Quick-reference for every wenyan construct with JavaScript equivalents; covers variables, control flow, operations, arrays, functions, and imports | **Most frequently consulted page while writing YACC rules** |
| [Beginner Cheatsheet (PDF)](https://github.com/alainsaas/wenyan/blob/master/wenyan-lang%20beginner%20cheatsheet.pdf) | Beginner quick reference with pinyin and English explanations | Understanding the Chinese meaning of tokens |
| [Official Online IDE](https://ide.wy-lang.org/) | Write and run wenyan programs directly in the browser with no local environment | Quickly verifying the behavior of a wenyan syntax snippet |
| [Wenyan Introductory Textbook](https://book.wy-lang.org/) | Official interactive textbook written in wenyan; covers variables, loops, and basic function syntax | Understanding the overall design philosophy of the language |
| [Textbook Chapter 1 (GitHub)](https://github.com/wenyan-lang/book/blob/master/01%20%E6%98%8E%E7%BE%A9%E7%AC%AC%E4%B8%80.md) | Markdown version of the first chapter; introduces programming concepts with the first wenyan examples | Quick-start without local installation |
| [Official Language Specification](https://wy-lang.org/spec) | Complete grammar specification; BNF definitions and semantic descriptions for each statement type | Confirming edge cases and semantic details |

> **Suggested reading order**: Start with the Syntax Cheatsheet for a broad overview. When implementing a specific statement, check the spec to confirm semantics. If in doubt, run a quick test in the online IDE.

---

## Optional Extensions

After completing the four-week core assignment, interested students may attempt the following. All of these can be implemented on top of the existing architecture, with varying difficulty.

### Language Feature Extensions

| Feature | Description | Entry point | Difficulty |
|---------|-------------|------------|------------|
| **2D arrays** | Support the `列之列` type and multi-index syntax | Extend `OBJECT_TYPE_ARRAY`; add multi-dimensional handling in `object_getIndex` | ★★☆ |
| **First-class functions** | Allow functions to be assigned to variables and passed as arguments | `OBJECT_TYPE_FUNC` already exists; add IR generation for assignment and passing | ★★☆ |
| **Module import** | Wenyan functions in separate `.wy` files can be imported by other programs | Add multi-file compilation flow in `main.c`; merge symbol tables | ★★★ |
| **Chinese numeral output** | Currently `書之` outputs Arabic numerals; output Chinese numerals instead (一、二、十一…) | Add `wy_rt_print_chinese` in runtime (`wy_rt.c`); modify `code_stdoutPrint` | ★☆☆ |
| **Multiple return values** | `乃得甲乙` returns multiple values simultaneously | Use `sret` or pack into anonymous struct; caller unpacks | ★★★ |
| **String interpolation** | Embed variables inside string literals, e.g. `「計得「甲」也」` | Add lexer state machine; runtime concat for splicing | ★★★ |
| **Constant folding** | `一加二` is computed at compile time to `三`; no IR emitted | In YACC actions, detect when both operands are literals and compute early | ★★☆ |
| **Pattern matching** | Extend `若` syntax to support type destructuring and multi-condition matching | Add non-terminal; chain `br` links for a switch-like IR | ★★★ |

### Compiler Engineering Extensions

| Feature | Description | Entry point | Difficulty |
|---------|-------------|------------|------------|
| **Tail call optimization** | Convert tail-recursive calls into loops to avoid stack overflow | Detect tail calls in `code_return`; emit `musttail call` | ★★☆ |
| **Dead code elimination** | Warn and omit branches that are never executed (e.g., `若陰者`) | Detect literal-bool conditions in `code_if` | ★★☆ |
| **DWARF debug info** | Allow `gdb` to map back to `.wy` source line numbers | Emit `!dbg` metadata alongside `DILocation` | ★★★ |
| **LLVM optimization pass** | Pipe the compiled IR through `opt -O2` and observe the results | Pipe to `opt` after the main flow in `wyc.c` | ★☆☆ |
| **WASM backend** | Compile wenyan programs to WebAssembly to run in a browser | Switch to `llc --target wasm32`; add WASI runtime shim | ★★☆ |
| **Wenyan pseudocode printer** | Pretty-print the AST as human-readable Chinese pseudocode (teaching tool) | Add AST visitor; attach print hooks to scope/symbol layer | ★★☆ |

### Runtime Extensions

| Feature | Description | Difficulty |
|---------|-------------|------------|
| **Garbage collection** | Strings and arrays currently use manual `free`; implement mark-and-sweep or reference counting in `wy_rt.c` | ★★★★ |
| **Test framework** | `斷言甲等乙` syntax; prints line number and expected value on mismatch; no extra tools needed | ★★☆ |
| **Window UI** | Hook up GTK / SDL / Dear ImGui to display a graphical interface from wenyan programs | ★★★ |
| **GPU acceleration (MLIR/LLVM)** | Dispatch to GPU via MLIR's GPU dialect or the LLVM NVPTX backend | ★★★★ |

### Tooling Ecosystem

| Feature | Description | Entry point | Difficulty |
|---------|-------------|------------|------------|
| **REPL** | `wyc` with no arguments enters interactive mode; parse and execute line by line | JIT (`lli`) or interpreter mode; scope must persist across lines | ★★★★ |
| **LSP server** | Hover shows variable type; go-to-definition jumps to declaration | Wrap existing scope/symbol table; implement JSON-RPC | ★★★★ |

> **Recommended starting points** (best learning value vs. code change ratio):
> 1. **LLVM optimization pass** — one `system()` call; instantly see IR differences
> 2. **Constant folding** — pure YACC action layer; solidifies understanding of semantic analysis
> 3. **Tail call optimization** — a few lines of changes; lets recursive wenyan programs run millions of iterations without stack overflow

---

## Important Notes

1. **No modification restrictions**: All provided code (including `compiler.l`, `lib/`, `.h` headers, and the runtime) may be changed freely. The only requirement is that `cmake --build build` produces a `wyc` that compiles `.wy` files into executables and passes the tests.
2. **The provided code is a simplified reference implementation**, intended only to help you understand the flow. You may design your own functions, add helpers, and change data structures — you are not required to follow the provided implementation style.
3. All semantic-action error reporting must use `yyerrorf()` (formatted) or `yyerrorlf()` (with location) — do not use `printf`.
4. Memory management: `scope_addSymbol` uses `strdup` internally to copy the name; callers do not need to free it.
5. `ctx->variableCount` is incremented by `scope_addSymbol` when allocating the index (`ctx->variableCount++`); callers do not need to increment it separately.
6. Closure capture logic is pre-implemented. Your `scope_findSymbol` only needs to handle the `searchCtx == ctx` case.
