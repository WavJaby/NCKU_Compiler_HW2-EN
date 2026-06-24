# Changelog

---

## 2026-06-24 — fixed position (line/column) display bug in verbose log

Last commit before this update: `b0335a3`

### What this fixes

The `檔名:行:列` (file:line:column) position printed by `-v` verbose mode had two bug classes:

1. **Column start point didn't match Bison's convention**: at program start (`SCOPE_MAIN`) it printed `1:2` instead of `1:1`.
2. **Some positions "leak" into the next statement**: for rules like `return`, `break`, and arithmetic expressions, the LALR(1) parser needs one extra token of lookahead before it can confirm the statement is finished. The verbose log ends up printing the *next* statement's start position instead of this statement's own position.

`test/*/*.verbose` answer keys are compared line-by-line, character-by-character, so once the column numbers shift the whole comparison goes out of sync — this update already regenerates every `test/*/*.verbose` answer key in the repo to match the new output, so you don't need to regenerate anything yourself.
`.expected` (the actual runtime output) is unaffected — IR generation logic didn't change.

### Do you need to change your own code?

**Most people don't need to do anything.** Only the `code_return`/`code_returnValue` and `code_break` groups got an extra signature parameter, and **only if you've already written code that calls them** do you need to add one argument. The expression group (`code_expression*`) kept its signature — just double-check what you pass as `aLoc` (see table below).

| File | Function(s) | What changed | What you need to do |
|---|---|---|---|
| `compiler.l` | (`yycolumnUtf8`, `updateNewline()` in the `%{...%}` prologue) | column counting now starts at `1` instead of `0`, matching Bison's default `yylloc` init (`{1,1,1,1}`) | **nothing**, internal-only fix |
| `compiler_util.h` / `compiler.h` / `lib/code_gen.h` | `yyerrorf`/`yyerrorlf`/`yyerrortf`/`yyerror`/`compilerLog` macros | dropped the `+1` when printing column (no longer needed now that the start point is 1-based); `compilerLog` now forwards to a new macro `compilerLogAt(loc, ...)` | **nothing**, internal-only fix |
| `compiler.y` | (no new rules, TODO comment text only) | the `OperationStmt` and `Expressions` TODO blocks got extra notes about passing a location argument to `code_return`/`code_returnValue`/`code_break`, and which `@N` to pass as `aLoc` | **the rules themselves are still blank** — nothing was written for you, since those rules are part of your TODO. Just follow the hint once you get there |
| `control/function.c`, `main.c`, `object.c` | `func_call`, `code_arrayPush`, `object_getIndex` | internal log now uses the correct location | **nothing**, these functions were already given complete, no TODO involved |
| `control/function.h` / `function.c` | `code_return`, `code_returnValue` | added a `const YYLTYPE* tokenLoc` parameter | if you've already written `ReturnStmt` in `compiler.y`, add `&@1` (for the `RETURN ExpressionChainStmt` rule) or `&@2` (for the `ExpressionChainStmt RETURN` rule) at the call site |
| `control/while.h` / `while.c` | `code_break` | added a `const YYLTYPE* tokenLoc` parameter | if you've already written the `code_break` definition/call, add the `tokenLoc` parameter on both sides; the `compiler.y` call site needs `&@1` |
| `expression.h` / `expression.c` | `code_expression`, `code_expressionMod`, `code_expressionChain`, `code_expressionChainMod` | signature unchanged, internals now log with `compilerLogAt(aLoc, ...)` | if you've already written the expression rules, **no argument to add** — but double-check what you pass as `aLoc`: it means "the whole expression's start token," not "the left operand's position." For most rules the first symbol is the start, so `&@1` works directly. For rules where the operator comes first (e.g. starting with the arithmetic-op token, or `THOSE`), pass that leading token's `&@1`, not the operand's position |

`code_getLength`'s TODO comment got an extra hint line (reminding you to log with `compilerLogAt(loc, ...)` instead of `compilerLog(...)`) — that's a text-only hint, no signature change, won't break code you've already written.

### How to update (git pull / merge)

If you forked this repo to do your assignment, in your own repo:

```bash
# if you haven't added the upstream remote yet
git remote add upstream git@github.com:WavJaby/NCKU_Compiler_HW2-EN.git

git fetch upstream
git merge upstream/master
```

Most people will get a **clean merge**, since the files touched this time are mostly infra you haven't edited (`compiler.l`'s prologue, `compiler_util.h`, `code_gen.h`, etc.).

If you've already written `ReturnStmt`/`code_break`/expression rules, the merge may conflict in `compiler.y`, `function.h`, `while.h`, or `expression.h` (because you added your own code near the same lines). To resolve:

1. Keep your own logic at the conflict
2. Cross-reference the table above and add the matching location argument (`&@1`/`&@2`) to the signature/call site
3. Rebuild and rerun `test/test.sh` to confirm nothing broke

If you'd rather not use `git merge`, you can also just manually add the parameters per the table above — the diff per call site is small (one extra argument).
