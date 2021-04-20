#ifndef __INTERMEDIATE__
#define __INTERMEDIATE__

#include "symbol_table.h"

#define EXPAND_SIZE 1024
#define CURR_SIZE   (total*sizeof(Quad))
#define NEW_SIZE    (EXPAND_SIZE*sizeof(Quad)+CURR_SIZE)

typedef struct Quad_t Quad;
typedef struct Expression Expr;

Quad*           quads;
unsigned        total;
unsigned int    currQuad;

enum iopcode_t {
    ASSIGN_I, ADD_I, SUB_I,
    MUL_I, DIV_I, MOD_I,
    UMINUS_I, AND_I, OR_I,
    NOT_I, IF_EQ_I, IF_NOTEQ_I,
    IF_LESSEQ_I, IF_GREATEREQ_I, IF_LESS_I,
    IF_GREATER_I, CALL_I, PARAM_I,
    RET_I, GETRETVAL_I, FUNCSTART_I,
    FUNCEND_I, TABLECREATE_I,
    TABLEGETELEM_I, TABLESETELEM_I
};

enum expression_type_t {
    VAR_E,
    TABLEITEM_E,

    PROGRAMFUNC_E,
    LIBRARYFUNC_E,

    ARITHEXPR_E,
    BOOLEXPR_E,
    ASSIGNEXPR_E,
    NEWTABLE_E,

    CONSTNUM_E,
    CONSTBOOL_E,
    CONSTSTRING_E,

    NIL_E
};

struct Expression {
    enum expression_type_t  type;

    // types
    SymTableEntry* sym;
    struct Expression* index;
    double numConst;
    char* strConst;
    unsigned char boolConst;

    struct Expression* next; // next
};

struct Quad_t {
    enum iopcode_t op;
    Expr* result;
    Expr* arg1;
    Expr* arg2;
    unsigned label;
    unsigned line;
};

void init_quads(void);
void emit(enum iopcode_t op, Expr* result, Expr* arg1, Expr* arg2, unsigned label, unsigned line);

#endif