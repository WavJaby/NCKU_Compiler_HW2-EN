# 114-2 Compiler Homework 2

<details><summary>Message from the compiler TA:</summary>

This semester was originally planned to have three assignments. The third one I had wanted to be a fun GPU-acceleration project using MLIR for matrix operations ~~(so you'd have a cool project to talk about in interviews)~~. Unfortunately, I didn't have time to finish it (I originally planned to release assignments 2 and 3 together since they're connected). Then midterms hit and things got really busy, and combined with my chronic procrastination (I think it's actually that I set the bar too high for code quality 🤥), the assignment that should have been out two weeks ago still isn't done because I haven't fully figured out MLIR yet.

So after discussion we decided to merge assignments 2 and 3 (don't worry — the new assignment 2 is essentially what assignment 3 would have produced. Once you finish assignment 2, assignment 3 is already about 80% done — I'll explain below). I'm really sorry for the delay 🙇.

To make up for the two weeks, I'll be providing pre-written functions and array code that you can use directly — details below.

![17804999081472542509659562187672](https://hackmd.io/_uploads/ryjxIpalzg.png)

As extra compensation I've already handled the function closure part for you. Please forgive me for staying up until the middle of the night writing it 🥹. **I swear I'll start writing assignments before the semester even begins from now on.**

Also, aside from the grammar (which is the most important part), the rest of the C code uses a fill-in-the-blank format. I emptied out the function bodies and used AI to document what each function needs to do — you just fill in the blanks. (I think this is exactly where AI shines: it's amazing at writing documentation, and it reduces the time you spend figuring out what's expected.) I also broke things into a roughly four-week schedule sorted by difficulty and dependency — treat it as a guide, not a hard constraint.

We're still doing things the old-fashioned way: all the source code in this assignment is hand-written by me. I always write the assignment myself first before assigning it to students, to make sure everyone can finish it without any paywall obstacles.

At the end of the semester I'll open-source everything. Contributions are very welcome if you're interested.

</details>

> International students who need English test results can obtain them from me (Email: F74114760@gs.ncku.edu.tw, or Moodle message).

## YACC (Yet Another Compiler-Compiler)

YACC is a tool that accepts a CFG (Context-Free Grammar) and automatically generates a C-language parser. In compiler construction, YACC is typically used alongside Lex/Flex (Homework 1) — Lex/Flex produces tokens, and YACC analyzes the syntactic structure.

---

### How YACC Works

YACC uses the **LALR(1)** parsing algorithm, using **Shift** and **Reduce** operations to check whether a sequence of input tokens conforms to the defined grammar.

---

### Structure of a YACC File

A YACC file (`.y` extension) is divided into three sections, separated by `%%`:

```
[Definitions]
%%
[Rules]
%%
[User Code]
```

---

## Core Concepts

### 1. Semantic Actions `{ ... }` — What to do when a rule matches

In the YACC rules section, each grammar rule can be followed by a **C code block** enclosed in `{}`, called a "Semantic Action."

**Concept:** When YACC successfully matches (reduces) a rule, it executes the corresponding `{ ... }` code block.

```
left-hand side:  right-hand side symbol sequence   { C code here — executed when this rule is Reduced }
```

**Example:**

```clike
statement
    : declaration ';'   { printf("Variable declaration detected\n"); }
    | assignment ';'    { printf("Variable assignment detected\n"); }
;
```

When the parser successfully matches a `declaration ';'` combination, it immediately executes `printf("Variable declaration detected\n");`.

> If a rule has no `{ ... }`, YACC defaults to `{ $$ = $1; }` (passes the value of the first symbol up).

---

### 2. `$$`, `$1`, `$2`... — Passing Symbol Values

YACC allows each grammar symbol to carry a "value." These values are accessed with the following syntax:

| Symbol | Meaning |
|--------|---------|
| `$$` | The value of the **current rule's left-hand side** (non-terminal) — the "output value" of this rule |
| `$1` | The value of the **1st** symbol on the right-hand side |
| `$2` | The value of the **2nd** symbol on the right-hand side |
| `$3` | The value of the **3rd** symbol on the right-hand side |
| `$N` | The value of the **N-th** symbol on the right-hand side |

**Concrete example — addition rule:**

```c
expr:   expr '+' expr   { $$ = $1 + $3; }
```

Breaking down the right-hand side:
- `$1` → 1st symbol `expr` (left operand value)
- `$2` → 2nd symbol `'+'` (the plus sign itself, usually not needed)
- `$3` → 3rd symbol `expr` (right operand value)
- `$$` → sets the computed result `$1 + $3` as the value of this `expr`

**Another example — pass NUMBER value up:**

```c
expr:   NUMBER   { $$ = $1; }
```

The value of `NUMBER` (passed in by Lex via `yylval`) directly becomes the value of this `expr`.

**How does Lex set the value of `$1`?**

In Lex, the value is set via the global variable `yylval` when a token is returned:

```c
[0-9]+  { yylval = atoi(yytext); return NUMBER; }
```

When YACC receives the `NUMBER` token, `$1` is the value of `yylval` — i.e., the result of `atoi(yytext)`.

---

### 3. `%left`, `%right`, `%nonassoc` — Operator Associativity and Precedence

When the grammar contains multiple operators, e.g. `1 + 2 * 3` or `a - b - c`, YACC needs to know:
1. **Precedence:** Which operator is computed first? (`*` before `+`)
2. **Associativity:** For operators at the same precedence level, do we compute left-to-right or right-to-left? (Is `a - b - c` parsed as `(a - b) - c` or `a - (b - c)`?)

YACC's solution is to declare operators in the definitions section — **later declarations have higher precedence:**

```c
%left '+' '-'      // lower precedence, left-associative
%left '*' '/'      // higher precedence, left-associative
```

#### `%left` — Left-associative

When the same-precedence operators appear in sequence, **compute from the left.**

```
a - b - c  →  (a - b) - c
```

Declaration:

```c
%left '+' '-'
%left '*' '/'
```

#### `%right` — Right-associative

When the same-precedence operators appear in sequence, **compute from the right.**

```
a = b = c  →  a = (b = c)
2 ^ 3 ^ 2  →  2 ^ (3 ^ 2)
```

Declaration:

```c
%right '='         // assignment operator, right-associative
%right UMINUS      // unary minus, right-associative
```

#### `%nonassoc` — Non-associative

The same-precedence operator is not allowed to appear consecutively.

```
a < b < c  →  syntax error
```

Declaration:

```c
%nonassoc '<' '>' LE GE EQ NE
```

#### Precedence determination rules

Operators declared **on the same line** have the same precedence; **later lines** have higher precedence.

```c
%left '+' '-'      // precedence 1 (lowest)
%left '*' '/'      // precedence 2
%right UMINUS      // precedence 3 (highest)
```

With this, YACC knows to compute `*` before `+` in `1 + 2 * 3`.

---

## Complete Example: Four-Operation Calculator

A complete YACC example combining the three concepts above:

```c
%{
#include <stdio.h>
#include <ctype.h>
int yylex(void);
void yyerror(const char *s);
%}

/* ── Definitions ─────────────────────────────── */

%token NUMBER       // Numeric token from Lex

%left '+' '-'       // + and - are left-associative, lower precedence
%left '*' '/'       // * and / are left-associative, higher precedence

/* ── Rules ───────────────────────────────────── */
%%

expr
    :   expr '+' expr   { $$ = $1 + $3; }   // $1 = left operand, $3 = right operand
    |   expr '-' expr   { $$ = $1 - $3; }
    |   expr '*' expr   { $$ = $1 * $3; }
    |   expr '/' expr   { $$ = $1 / $3; }
    |   NUMBER          { $$ = $1; }
;

%%

/* ── User Code ───────────────────────────────── */

int main(void) {
    return yyparse();
}

void yyerror(const char *s) {
    fprintf(stderr, "error: %s\n", s);
}
```

---

## Implementation Example: Parsing Variable Declarations and Assignments

Goal: parse the following program:

```cpp
int a;
double b;
a = 10;
b = 3.14;
```

#### Step 1. Declare tokens from Lex (`%token`)

```c
%token INT DOUBLE    // data type keywords
%token IDENTIFIER    // variable name
%token NUMBER        // numeric constant
```

#### Step 2. Write grammar rules (BNF)

```clike
%%

/* Rule 1: a program is a sequence of statements */
program
    : program statement
    | /* empty input is also valid */
;

/* Rule 2: a statement is either a declaration or an assignment */
statement
    : declaration ';'
    | assignment ';'
;

/* Rule 3: variable declaration, e.g. int a */
declaration
    : type IDENTIFIER
;

/* Rule 4: supported data types */
type
    : INT
    | DOUBLE
;

/* Rule 5: variable assignment, e.g. a = 10 */
assignment
    : IDENTIFIER '=' NUMBER
;

%%
```

#### Step 3. Add semantic actions and assemble the complete file

```c
%{
#include <stdio.h>
#include <stdlib.h>

int yylex(void);
void yyerror(const char *s);
%}

%token INT DOUBLE
%token IDENTIFIER
%token NUMBER

%%

program
    : program statement
    | /* empty */
;

statement
    : declaration ';'   { printf("[OK] Variable declaration detected\n"); }
    | assignment ';'    { printf("[OK] Variable assignment detected\n"); }
;

declaration
    : type IDENTIFIER
;

type
    : INT
    | DOUBLE
;

assignment
    : IDENTIFIER '=' NUMBER
;

%%

int main(void) {
    return yyparse();
}

void yyerror(const char *s) {
    fprintf(stderr, "syntax error: %s\n", s);
}
```

---

### Concept Summary

| Syntax | Purpose |
|--------|---------|
| `{ ... }` | Semantic action — C code executed when the rule is Reduced |
| `$$` | Set the value of the current rule (left-hand side non-terminal) |
| `$1`, `$2`, `$N` | Get the value of the N-th symbol on the right-hand side |
| `%left` | Declare left-associative operator (same precedence computes left-to-right) |
| `%right` | Declare right-associative operator (same precedence computes right-to-left) |
| `%nonassoc` | Declare non-associative operator (consecutive use is a syntax error) |
| `%token` | Declare a terminal symbol (token) |
| `yylval` | Global variable used by Lex to pass token values to YACC |

> **Advanced usage** (`$<type>N`, Mid-Rule Actions, `$0`, `YYABORT`, location tracking `@N`) — see [YACC_CHEATSHEET.md](https://github.com/WavJaby/NCKU_Compiler_HW2/blob/master/YACC_CHEATSHEET.md).

---

## Assignment Description

> [!NOTE]
> **Source code / assignment skeleton**: [github.com/WavJaby/NCKU_Compiler_HW2](https://github.com/WavJaby/NCKU_Compiler_HW2)
> Environment setup and detailed documentation (function signatures, step-by-step TODO hints, utility function reference) are in [README.md](https://github.com/WavJaby/NCKU_Compiler_HW2/blob/master/README.md)
> Not sure where to start? Begin with [Quick Start — First Statement](https://github.com/WavJaby/NCKU_Compiler_HW2/blob/master/README.md#quick-start--first-statement)

Don't be intimidated by the length of the assignment description and the number of TODOs. All these documents were organized with Claude as a collaborator, working from the source code, so they contain everything you need to complete the assignment. I've read through all of them as well — theoretically this is everything. If anything is missing, please let me know.

## Grading

| Week | Test | Part 1 Verbose | Part 2 Runtime | Subtotal |
|------|------|:-:|:-:|:-:|
| Week 1 | `策問/00_快速入門` | 3 | 2 | **5** |
| Week 1 | `策問/01_開物_定名` | 12 | 8 | **20** |
| Week 2 | `策問/02–05` (each) | 3 | 2 | **5 × 4 = 20** |
| Week 3 | `策問/06–11` (each) | 3 | 2 | **5 × 6 = 30** |
| Week 4 | `策問/12_萬化_方術` | 4 | 2 | **6** |
| Week 4 | `殿試/` (each) | 5 | 3 | **8 × 3 = 24** |
| **Total** | | **64** | **41** | **105** |

- **Part 1 (Verbose)**: `wy -v` verbose log output (stdout/stderr) must match the expected output; does not require the program to be executable (`.ll` files are not graded)
- **Part 2 (Runtime)**: the program must compile and execute, and the standard output must match the expected output

> [!TIP]
> Use `-f <test-name>` to run a single test
>
> **Local grading**: `./test/test.sh`
> **Server grading**: `submit_test_hw2`

> [!WARNING]
> Final score = min(server score, 100)
> This assignment counts for a maximum of 100 points, worth 25% of the semester grade (100 × 25% = 25 points).
> Note: after submission, the actual grading run will substitute different variable values. For example, the test file `01_開物_定名.wy` contains `吾有一爻。曰陰。名之曰「陰陽」。` — the grader may change it to `吾有一爻。曰陽。名之曰「陰陽」。`. The `submit_test_hw2` command does not change variable values; be aware of this.

---

## Local Development and Debugging

**Strongly recommended: develop locally, and only test on the server when you're ready to submit.**

A local IDE (CLion / VS Code) offers the following advantages:
- **Exact segfault location**: IDE + GDB jumps directly to the offending line — no guessing
- **Bison conflict hints**: CLion can show conflict line numbers directly in `.y` files (even better with Bison 3.8 `-Wcounterexamples`)
- **No need to add log calls**: IDE shows the full call stack; much faster than scattering `compilerLog` / `printf` calls, and you won't accidentally leave debug logs in and fail Part 1

### Debugging Tool Reference

| Need | Tool | Notes |
|------|------|-------|
| **Part 1 verbose output** (graded) | [`compilerLog(fmt, ...)`](https://github.com/WavJaby/NCKU_Compiler_HW2/blob/master/README.md#utility-function-reference) | Output with `-v`; includes line prefix; this is what `.verbose` reference files compare against |
| Lexer token tracing | `lexerLog(fmt, ...)` | Output with `-l`; lexer debugging only; does not affect grading |
| Report semantic errors | [`yyerrorf(fmt, ...)` / `yyerrorlf(fmt, loc, ...)`](https://github.com/WavJaby/NCKU_Compiler_HW2/blob/master/README.md#error-reporting) | Prints offending line number + source block; much faster to locate than printf; **delete when done debugging** |
| Crash location | IDE / GDB, `cmake -DCMAKE_BUILD_TYPE=Debug` | — |

> [!CAUTION]
> **Delete all extra log calls after debugging**: Part 1 compares the full verbose log — even one extra `compilerLog` or `printf` call will cause a **Part 1 test FAIL**.

---

## Four-Week Schedule

<details><summary>Week 1: Environment Setup & Single-Value Variables (7–9h)</summary>

**Goal**: Declare single-type variables (number/string/boolean) and print them

| Task | Description |
|------|-------------|
| Environment setup | CMake build, run existing test cases |
| `compiler.l` tweaks | Update HW1 rules to set `yylval` + `return`; add `"以施"` / modify `"乃得"` |
| [Quick Start](https://github.com/WavJaby/NCKU_Compiler_HW2/blob/master/README.md#quick-start--first-statement) | Write the long rule directly in `OperationStmt` and get the first statement working; confirm the pipeline works end to end |
| [Sub-rule refactor](https://github.com/WavJaby/NCKU_Compiler_HW2/blob/master/README.md#after-quick-start--refactoring-direction) | Break the Quick Start long rule into `CreateValueDataListStmt`, `LitOrVarStmt`, `VariableDefineStmt` |
| `scope_addSymbol` | Add duplicate-symbol check |
| `scope_findSymbol` | Add the `searchCtx == ctx` normal return branch |
| `value_data.c` | Add type check and count cap |
| `code_createVariable` | Switch to `object_nameLiteralOrLoadReg`; support symbol references and non-numeric types |
| `code_stdoutPrintObject` | Add `@printf` calls for I32 / I64 / F64 / STR |

</details>

<details><summary>Week 2: Multi-value Declarations & Assignment & Expressions (10–13h)</summary>

**Goal**: Multi-value declarations, assignments, and arithmetic/logic expressions

| Task | Description |
|------|-------------|
| `CreateValueDataListStmt` (multi-value) | `HERE_ARE NUMBER_LIT VAR_TYPE` |
| `VariableDefineStmt` (multi-name) | `NAME_IT IDENT` followed by repeated `SAID IDENT` |
| `object_ValueDataListAddDefaults` | Add zero values for each type |
| `code_assign` | dest type validation + store IR |
| `ExpressionStmt` / `ExpressionNextStmt` | Arithmetic, logic, and chained expression rules |
| `code_expression` | Register allocation, type promotion, IR instruction output |
| `object_nameLiteralOrLoadReg` REGISTER branch | Chain expression pass-through |

</details>

<details><summary>Week 3: Control Flow & Arrays (11–18h)</summary>

**Goal**: if / for / while control flow; array operations

> **Suggested order**: Implement `code_if` + `code_ifEnd` (no else) first to confirm basic blocks work correctly, then add else/elseif.

| Task | Description |
|------|-------------|
| `code_if` / `code_ifEnd` | Basic branch; handle the no-else path first |
| `code_elseIfLabel` / `code_elseIf` / `code_else` | Complete if-elseif-else chain |
| `code_forLoop` / `code_forLoopEnd` | phi node + 4-label structure (see [LLVM_IR_CHEATSHEET.md](https://github.com/WavJaby/NCKU_Compiler_HW2/blob/master/LLVM_IR_CHEATSHEET.md) §phi nodes) |
| `code_break` | Get the nearest loop scope; emit `br` to the exit label (~4 lines) |
| Array index access | `夫「X」之「N」` → call the provided `object_getIndex` |
| `code_getLength` | Get string/array length; call runtime functions |

</details>

<details><summary>Week 4: Functions & Final Tests (9–12h)</summary>

**Goal**: Define and call custom functions; pass comprehensive tests

| Task | Description |
|------|-------------|
| `object_nameLiteralOrLoadReg` Week 4 branches | `funcArg`, `capturedIndex`, and `FUNC` — three special SYMBOL cases |
| `FunctionStmt` YACC rule | Function definition syntax; `func_define` is provided |
| `FunctionArgsStmt` / `FunctionArgListStmt` | Parameter type list declaration and individual registration |
| `FunctionCallArgsStmt` | Argument passing for function calls |
| Integration tests | Circle area (割圓術), Mandelbrot set (曼德博集), Newton's method (牛頓求根法) |

</details>


### Fill-in Function Overview

Full specifications in [README.md — Fill-in Module Specifications](https://github.com/WavJaby/NCKU_Compiler_HW2/blob/master/README.md#fill-in-module-specifications)

| File | Function | Week |
|------|----------|------|
| `compiler.l` | Token rules (copy from HW1 and add TOKEN returns) | W1 |
| `compiler.y` | All grammar rules (skeleton provided; fill in the rules section) | W1–W4 |
| `scope.c` | `scope_addSymbol`, `scope_findSymbol` | W1 |
| `value_data.c` | `object_ValueDataListCreate/Add` (add validation), `object_ValueDataListAddDefaults` | W1–W2 |
| `main.c` | `code_stdoutPrintObject`, `code_createVariable`, `code_assign`, `code_getLength` | W1–W3 |
| `expression.c` | `code_expression` | W2 |
| `object.c` | `object_nameLiteralOrLoadReg` (REGISTER branch W2; Week 4 branches W4) | W2, W4 |
| `control/if.c` | `code_if`, `code_elseIfLabel`, `code_elseIf`, `code_else`, `code_ifEnd` | W3 |
| `control/for.c` | `code_forLoop`, `code_forLoopEnd` | W3 |
| `control/while.c` | `code_break` | W3 |


---

### LLVM IR Quick Reference

For-loop phi nodes, if/else basic block structure, and all `buffPrintln` usage patterns: see [LLVM_IR_CHEATSHEET.md](https://github.com/WavJaby/NCKU_Compiler_HW2/blob/master/LLVM_IR_CHEATSHEET.md) (written specifically for this assignment; naming and framework match exactly).

---

## Submission Instructions (Read Carefully)

To provide a consistent testing and grading environment, we have a server (Ubuntu 20.04) available for testing. Since this server is shared with other lab work, please use it responsibly.

> [!CAUTION]
> Any attempt to disrupt or overload the system will result in immediate account suspension.



### Login

Connect to [NCKU VPN](https://ncku.twaren.net) before logging in:

```bash
ssh StudentID@140.116.154.65 -p 20261
```


### Testing (For HW2)

First grant execute permission to the test script:

```bash
chmod +x ./test/test.sh
```

Then run:

```bash
submit_test_hw2
```

This shows your test results. By default it stops on the first error, rebuilds each time, and ignores the `-n` and `-i` flags.

![截圖 2026-06-04 上午11.32.21](https://hackmd.io/_uploads/B1pCbu0lGl.png)



Because many students are enrolled, your code runs inside a dedicated container. When many students test simultaneously there may be a queue, so you cannot install additional tools on the server. The server's results are used for grading.

### Submit (For HW2)

Create an `HW2` directory (uppercase) in your home directory:

```
mkdir -p ~/HW2
```

Place the `NCKU_Compiler_HW2` folder inside `HW2` to submit.

> [!TIP]
> Recommended: create the `HW2` directory first, then `git clone` the assignment template inside it, and edit files directly there.

The Moodle submission area is for backup only — grading is based on the server. If you want a backup, compress your assignment as a `.zip` file.

> [!WARNING]
> **Leave time before the deadline**: The server environment may differ from your local machine (compiler versions, etc.). Test and fix on the server **at least one day** before the deadline to avoid last-minute surprises or long queues. Don't leave it to the last minute ヾ(•ω•`)o

> [!WARNING]
> **Do not modify `CMakeLists.txt` (root) or `test/test.sh`**: These files are part of the grading environment. Modifying them may cause the server to fail to build or score correctly.
> - To add new `.c` source files or a library → modify `src/CMakeLists.txt`
> - To add a custom test script → create a new `.sh` file; do not touch the original `test/test.sh`
