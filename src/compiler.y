/* Definition section */
%code requires {
    # define YYLTYPE_IS_DECLARED 1
    # define YYLTYPE_IS_TRIVIAL 1
}

%{
    #include "compiler_util.h"
    #include "main.h"
    #include "expression.h"
    #include "value_data.h"
    #include "scope.h"
    #include "control/for.h"
    #include "control/if.h"
    #include "control/while.h"
    #include "control/function.h"
%}

%define parse.error custom
%locations

/* Variable or self-defined structure */
%union {
    ObjectType var_type;

    bool b_var;
    ScientificNotation n_var;
    char *s_var;

    Object obj_val;
    ValueData val_data;

    FuncCallInfo* func_call;

    bool exp_left;
    ExpOp exp_op;
}
/* Token — minimal set for quick start; expand as you implement each rule */
%token COMMENT
%token HERE_ARE HERE_IS_A SAID NAME_IT
%token PRINT TO_CALL
%token RETURN

%token <n_var> NUMBER_LIT
%token <b_var> BOOL_LIT
%token <var_type> VAR_TYPE
%token <s_var> STR_LIT IDENT

/* %left example — add more when resolving shift/reduce conflicts in ValueStmt */
%left INDEX

/* Nonterminals with return type — add more %type declarations as you implement sub-rules */
%type <val_data> CreateValueDataListStmt

/* For Return — used by the provided ReturnStmt; see YACC_CHEATSHEET.md §Precedence Declarations */
%nonassoc LOWER_THAN_EXPR
%nonassoc RETURN

/* Yacc will start at this nonterminal */
%start Program
%%
/* Grammar section */

/* Scope */
Program
    : GlobalScopeStmt
;

GlobalScopeStmt
    : BodyListStmt
;

/* Scope Body */
BodyListStmt
    : BodyListStmt BodyStmt
    |
;

BodyStmt
    : COMMENT STR_LIT
    | OperationStmt
    | ConditionStmt
    | FunctionStmt
;

/* Function */
/* TODO: Function definition
 * Register the function symbol, push context/scope, register each parameter, then pop when done.
 * Functions: func_define, func_defineBody, func_defineBodyEnd, func_defineAddParam
 * Note: parameter type must be passed cross-rule via $<var_type>0; parameter types and names are each a separate rule layer
 */
FunctionStmt
    :
;

FunctionArgsStmt
    :
;

FunctionArgListStmt
    :
;

/* Condition and Operation */
/* TODO: Control flow (FOR / WHILE / IF-ELSEIF-ELSE)
 * Three branch types, each with corresponding start/end IR calls.
 * Functions: code_forLoop/End, code_whileLoopStart/End, code_if, code_elseIfLabel, code_elseIf, code_else, code_ifEnd
 * Note: else-if and else are both optional; the IF structure is composed of three sub-rules
 */
ConditionStmt
    :
;

/* TODO: Various operation statements
 * Covers variable declaration, naming, assignment, function calls, array push, print, return, and break.
 * Functions: object_ValueDataList*, code_createVariable, code_assign, code_stdoutPrint,
 *            code_arrayPush, code_return, code_returnValue, code_break,
 *            func_callInit, func_callArgAdd, func_call, func_takeAndCall
 * Note: function calls come in prefix (施) and postfix (以施) forms; use mid-rule action with $0 to pass intermediate values;
 *       the call result can be followed by naming, return, print, or omitted
 * Location arg: code_return/code_returnValue/code_break each take an extra tokenLoc
 *       parameter — pass the RETURN/BREAK token's own @N (e.g. &@1). Don't skip this:
 *       by the time the rule reduces, the parser may already have peeked one token
 *       ahead, so the global yylloc could point at the next statement instead of
 *       this token's own position.
 */
OperationStmt
    :
;

CreateValueDataListStmt
    :
;

VariableDefineStmt
    :
;

/* Expressions */
/* TODO: Expressions (arithmetic/logical, chained)
 * Functions: code_expression/Mod, code_expressionChain/Mod
 * Note: the first item in a chain uses code_expression, subsequent items use code_expressionChain; update ctx->last_result
 * Location arg: aLoc is not simply "the left operand's position" -- it's the start
 *       token of the whole expression, used for both logging and error reporting.
 *       For most rules the first symbol IS the start, so just pass &@1. But for
 *       rules where the operator/keyword comes first (e.g. starting with the
 *       arithmetic-op token, or starting with THOSE for binary logic), don't set
 *       aLoc to the operand's position -- pass that leading operator/keyword's own
 *       &@1, otherwise the verbose log position will be off.
 */
ExpressionChainStmt
    :
;

ExpressionStmt
    :
;

ExpressionNextStmt
    :
;

ValueLiteralOrLastStmt
    :
;

/* Value */
/* TODO: Values, literals, and variable lookup
 * Functions: object_createStr/Number/Bool, scope_findSymbol, object_getIndex, code_getLength
 * Note: ITS retrieves ctx->last_result; array indexing and length are extensions of ValueStmt
 */
ValueStmt
    :
;

LitOrVarStmt
    :
;

ValueLiteralStmt
    :
;

VariableStmt
    :
;

%%

#include "compiler.h"
