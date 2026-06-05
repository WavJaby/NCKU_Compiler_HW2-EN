# YACC / Bison Cheatsheet — Wenyan Compiler Assignment

Supplementary advanced Bison usage, focused on scenarios you will actually encounter in this assignment. Refer to this when you run into problems implementing `compiler.y`.

Related documents: [Assignment Spec](https://hackmd.io/@WavJaby/NCKU_1142_COMPILER_HW2)　[README](README.md)　[LLVM IR Cheatsheet](LLVM_IR_CHEATSHEET.md)

---

### `$N` in This Project

The examples above use a single type. This project's `%union` contains multiple fields, so you must add type annotations; otherwise Bison reads the wrong field:

```yacc
ValueLiteralStmt
    : NUMBER_LIT { $$ = object_createNumber(&$<n_var>1); }
    /*            ^^                              ^^
              returns to caller           manually specifies the n_var field  */
```

**When manual annotation is required:**
- Token declared with `%token <field>` → no annotation needed
- Non-terminal declared with `%type <field>` → no annotation needed
- All other cases → **must** manually annotate `$<field>N`

```yacc
FunctionArgListStmt
    : SAID IDENT { func_defineAddParam($<var_type>0, $<s_var>2); }
    /*                                 ^^^^^^^^^^^ manually annotated   */
```

**`%union` field quick reference:**

| Purpose | Field name | C type |
|---------|------------|--------|
| Variable / function type | `var_type` | `ObjectType` |
| Boolean literal | `b_var` | `bool` |
| Number literal | `n_var` | `ScientificNotation` |
| String / identifier | `s_var` | `char*` |
| Semantic value (expression result) | `obj_val` | `Object` |
| Multi-value declaration container | `val_data` | `ValueData` |
| Function call info | `func_call` | `FuncCallInfo*` |
| Operator direction (preposition) | `exp_left` | `bool` |
| Operator type | `exp_op` | `ExpOp` |

---

### `$0` and `$-1` — Reading Symbols to the Left in the Parent Rule

Inside an action for a production, `$0` refers to the semantic value of **the symbol immediately to the left of the current non-terminal in the parent rule**, and `$-1` refers to one position further left.

```
OperationStmt
    : CreateValueDataListStmt  NAME_IT  VariableDefineStmt
      ─────────────────────    ───────  ──────────────────
              $-1                $0        (current rule)
```

When Bison reduces `VariableDefineStmt`, its action can access:
- `$<val_data>-1` → the value of `CreateValueDataListStmt` (the declared value list)
- `$<var_type>0` → the value of the symbol to the left (a keyword in this example; usually not used)

**If the parent rule has a mid-rule action before `VariableDefineStmt`, indices shift right by one:**

```
OperationStmt
    : CreateValueDataListStmt  LitOrVarStmt  { mid-rule }  VariableDefineStmt
      ─────────────────────    ────────────  ───────────   ──────────────────
               $-2                 $-1           $0           (current rule)
```

Here `$<val_data>0` retrieves the mid-rule action's `$$` (which the mid-rule typically stores val_data into). The exact index depends on your grammar structure — count how many symbols appear to the left of the current rule.

```
FunctionArgsStmt
    : TO_PERFORM_FUNC REQUIRE_ARGS NUMBER_LIT  VAR_TYPE  FunctionArgListStmt
                                               ────────  ──────────────────
                                                  $0        (current rule)
```

`$<var_type>0` inside `FunctionArgListStmt`'s action retrieves `VAR_TYPE` (the parameter type).

**Common positions in this project:**

| Location | What it reads | Index |
|---------|--------------|-------|
| `VariableDefineStmt` (no mid-rule) | parent's `CreateValueDataListStmt` | `$<val_data>-1` |
| `VariableDefineStmt` (mid-rule on its left) | val_data stored by mid-rule | `$<val_data>0` |
| `FunctionArgListStmt` | parent's `VAR_TYPE` | `$<var_type>0` |

Other `$0` use cases (print, multi-value declaration, function call arguments, etc.) all appear in sub-rules of `OperationStmt`; follow the pattern above when implementing them.

**Note:** Bison does no type or bounds checking; a wrong annotation won't error — it will silently return garbage.

---

### Mid-Rule Action — Executing an Action in the Middle of a Rule

Bison allows you to insert actions `{ ... }` between right-hand side symbols. Each such action occupies one slot on the value stack and shifts subsequent `$N` indices:

```yacc
SomeRule
    : A B { $$ = something; } C
    /*       ^^^^^^^^^^^^^^
              mid-rule action, occupies stack position 3
              A=$1, B=$2, mid-action=$3, C=$4   */
```

**Project example: the `PUSH` syntax (you will implement this pattern in OperationStmt)**

```yacc
OperationStmt
    : PUSH VariableStmt { $<obj_val>$ = $<obj_val>2; } PushItemList
    /*      ^^^^^^^^^^   ^^^^^^^^^^^^^^^^^^^^^^^^^^^
            $2 (array)   mid-rule action, stored on stack so sub-rules can read it via $0  */

PushItemList
    : EXP_PREPOSITION LitOrVarStmt
        { code_arrayPush(&$<obj_val>0, &$<obj_val>2, &@2); }
    /*                    ^^^^^^^^^^
                $0 = the array Object stored by the mid-rule action above  */
    | PushItemList EXP_PREPOSITION LitOrVarStmt
        { code_arrayPush(&$<obj_val>0, &$<obj_val>3, &@3); }
;
```

Stack state illustration:

```
... | PUSH | VariableStmt | mid-action | (PushItemList symbols)
                                 ↑
                           $0 (array Object)
```

---

### Precedence Declarations in This Project

`compiler.y` provides two pseudo-tokens for use with `%prec`:

```yacc
%nonassoc LOWER_THAN_EXPR   /* pseudo-token, gives "lower than EXPR" precedence */
%nonassoc RETURN
```

Pseudo-tokens never appear in input; they exist solely to be used with `%prec` so that a specific rule can borrow their precedence level:

```yacc
ReturnStmt
    : RETURN ExpressionChainStmt %prec LOWER_THAN_EXPR { ... }
    /* when the production ends with no token, use %prec to specify conflict resolution */
```

**When implementing ValueStmt and ExpressionStmt,** use `bison -v compiler.y` to inspect `compiler.output` for shift/reduce conflicts, then add `%left`/`%nonassoc` as needed. `compiler.y` already has a `%left INDEX` line as a format reference.

**Default action:** if a rule has no `{ ... }`, Bison defaults to `{ $$ = $1; }`.

---

### Location Info `@N`

```yacc
ExpressionStmt
    : EXP_MATH_OP ValueStmt EXP_PREPOSITION ValueStmt
        { $$ = code_expression(..., &@2, &@4); }
        /*                         ^^   ^^
                source location of the 2nd and 4th symbols (line, column)
                type is YYLTYPE; used for error message positioning          */
```

---

### `YYABORT` and Error Handling

```yacc
VariableStmt
    : IDENT {
        if (($$ = scope_findSymbol($<s_var>1)).type == OBJECT_TYPE_UNDEFINED)
            YYABORT;   /* semantic error — immediately abort parsing */
    }
;
```

- `YYABORT`: immediately stops the entire parse and returns an error.
- Convention: C functions return `true` on failure, `false` on success.
- `yyerrorf("message")` only prints an error — it does **not** stop parsing; you must add `YYABORT` yourself.

---

### Shift/Reduce Conflicts

#### What is a Shift/Reduce Conflict

When the parser reads a token, both of the following are valid:

- **Shift**: push this token onto the stack and continue reading
- **Reduce**: apply a grammar rule to the symbols currently on top of the stack

Both are valid but only one can be chosen — that is a conflict.

**Classic example — dangling else:**

```yacc
IfStmt
    : IF Cond THEN Stmt
    | IF Cond THEN Stmt ELSE Stmt
```

On reading `ELSE`:
- Shift → `ELSE` belongs to the nearest `IF` (usually the correct behavior)
- Reduce → first reduce `IF Cond THEN Stmt`, and `ELSE` belongs to the outer `IF`

Bison defaults to **Shift** (usually what you want), but will print a warning.

#### Common S/R Conflict Locations in This Project

| Rule | Typical conflict location | Reason |
|------|--------------------------|--------|
| `ValueStmt` | seeing `INDEX` (之) | ambiguous whether it is an index or the start of the next statement |
| `ExpressionStmt` | end of a chained expression | unclear whether the next token continues the chain or starts a new statement |
| `LitOrVarStmt` | `SAID` followed by `IDENT` | could be a variable reference or a function name |

#### How to Read compiler.output

After building, `build/generated/compiler.output` is generated automatically (the `CMakeLists.txt` passes `-v -Wcounterexamples`; no need to add flags yourself).

Finding conflicts:

```
grep -n "conflict" build/generated/compiler.output
```

Conflict block format:

```
State 42 conflicts: 1 shift/reduce
...
State 42
  ExpressionStmt -> ValueStmt .           (rule 17)
  ExpressionStmt -> ValueStmt . EXP_MATH_OP ValueStmt  (rule 18)

  EXP_MATH_OP   shift, and go to state 55   ← Shift action
  EXP_MATH_OP   reduce using rule 17        ← Reduce action (conflict)
```

`-Wcounterexamples` additionally prints the concrete token sequence that causes the conflict, which makes it easier to understand.

#### Resolution Methods

| Situation | Fix |
|-----------|-----|
| Operator precedence problem | add `%left`/`%right`/`%nonassoc TOKEN` in the declarations section |
| A specific rule must force Reduce | add `%prec LOWER_THAN_XXX` at the end of the rule |
| Bison's default Shift is what you want | use `%expect N` to declare N expected S/R conflicts and suppress the warning |
| The grammar itself is ambiguous | refactor rules, introduce more specific non-terminals |

```yacc
%left  EXP_MATH_OP      /* left-associative, same precedence */
%right EXP_ASSIGN       /* right-associative */
%nonassoc EXP_COMPARE   /* non-associative: a < b < c is an error */
```

---

### Reduce/Reduce Conflicts

#### What is a Reduce/Reduce Conflict

The same stack state can apply **two different rules** for reduction, and Bison doesn't know which to choose.

```yacc
A : X Y ;
B : X Y ;   /* same right-hand side as A */
```

More serious than Shift/Reduce conflicts: Bison picks the first rule (declaration order), **silently swallowing the error**, and the semantics are usually wrong.

#### Common Causes and Fixes

| Cause | Example | Fix |
|-------|---------|-----|
| Two rules with identical right-hand sides | `LitStmt : NUMBER_LIT` and `ValStmt : NUMBER_LIT` | merge into one rule, or add a distinguishing prefix token |
| Empty-production ambiguity | multiple optional rules can all reduce to empty | restructure so each rule has a unique starting token |
| `%type` assigns the same rule to different non-terminals | — | check for duplicate token-sequence paths |

Diagnose: search `compiler.output` for `reduce/reduce`:

```
State 7 conflicts: 1 reduce/reduce
```

---

### Parser Hangs After Bison Compilation (Infinite Loop / No Progress)

#### Cause 1: Empty Productions That Reference Each Other

```yacc
A : B ;
B : A ;   /* A → B → A → B → ... */
```

Or a more subtle indirect cycle:

```yacc
OperationStmt :            /* empty */
CreateValueDataListStmt :  /* empty */
OperationStmt : CreateValueDataListStmt OperationStmt
```

The parser keeps trying to reduce empty rules in a cycle, and **the program appears frozen**.

**Diagnose:** in `compiler.output` generated with `-v`, search for your rule and look for `ε` (empty production) cycles.

**Fix:** ensure at least one non-empty base case; place empty productions only where they are genuinely optional.

#### Cause 2: Flex Rule Missing a Token Return

```c
"some-wenyan-string" { /* forgot return TOKEN_NAME; */ }
```

The lexer consumes the string but returns nothing. The parser waits forever for the next token, and **the program appears frozen**.

**Diagnose:** enable lexer verbose mode (`-l` flag) or add `fprintf(stderr, ...)` to confirm each rule fires and returns.

**Fix:** add `return TOKEN_NAME;` at the end of every token rule.

#### Cause 3: Infinite Loop Inside a Semantic Action

The semantic action `{ ... }` calls a function that contains an infinite loop or deadlock.

**Diagnose:** attach `gdb` or press `Ctrl+C` and inspect the stack trace; alternatively, add `fprintf(stderr, "rule X\n")` to each semantic action to find where it stalls.

#### Symptom Quick Reference

| Symptom | Most likely cause |
|---------|-----------------|
| Program produces no output and never terminates | Empty-production cycle or Flex missing `return` |
| Only specific tests hang | A Flex rule for a particular token is missing `return` |
| After adding `fprintf`, a semantic action executes repeatedly | Empty-production cycle |
| `gdb` stack trace stops inside `yyparse` | Flex missing `return` (parser waiting for a token) |

---

### Common Errors

| Symptom | Cause | Fix |
|---------|-------|-----|
| Shift/Reduce conflict warning | Precedence not set, or ambiguous production | Check `build/generated/compiler.output`; add `%left`/`%prec` |
| Reduce/Reduce conflict warning | Two rules with identical right-hand sides | Refactor rules; add a distinguishing prefix token |
| Value looks like garbage or NULL | Wrong field in `$<type>N` annotation | Cross-check against `%union` field names |
| Semantic value wrong but no error | Forgot to assign `$$` | Confirm every rule that returns a value sets `$$` |
| `$0`/`$-1` returns wrong value | Non-terminal appears in multiple parent rule contexts | Ensure the non-terminal has only one parent rule calling context |
| `free` after use / double free | Lifetime of `$<s_var>N` for `STR_LIT`/`IDENT` | Don't free a string you pass out; call `object_free` on strings you consume locally |
| Parser hangs without terminating | Flex rule missing `return`, or empty-production cycle | Enable lexer verbose to confirm; search `compiler.output` for empty-production cycles |
