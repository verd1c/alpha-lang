#ifndef __INTERMEDIATE__
#define __INTERMEDIATE__

#include "symbol_table.h"

#define EXPAND_SIZE 1024
#define CURR_SIZE   (total*sizeof(Quad))
#define NEW_SIZE    (EXPAND_SIZE*sizeof(Quad)+CURR_SIZE)

// label list
typedef int llist_t;

typedef struct Quad         Quad;
typedef struct Expression   Expr;
typedef struct Call         Call;
typedef struct Stmt         Stmt;

extern Quad*            quads;
extern unsigned         total;
extern unsigned int     currQuad;
extern long             _temp_counter;

enum iopcode_t {
    ASSIGN_I, ADD_I, SUB_I,
    MUL_I, DIV_I, MOD_I,
    UMINUS_I, AND_I, OR_I,
    NOT_I, IF_EQ_I, IF_NOTEQ_I,
    IF_LESSEQ_I, IF_GREATEREQ_I, IF_LESS_I,
    IF_GREATER_I, CALL_I, PARAM_I,
    RET_I, GETRETVAL_I, FUNCSTART_I,
    FUNCEND_I, TABLECREATE_I,
    TABLEGETELEM_I, TABLESETELEM_I, JUMP_I
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

    // in case of boolexpr
    llist_t truelist;
    llist_t falselist;
    llist_t test;
    llist_t enter;

    int offset;

    struct Expression* next; // next
    struct Expression* next_index;
};

struct Quad {
    enum iopcode_t op;
    Expr* result;
    Expr* arg1;
    Expr* arg2;
    unsigned label;
    unsigned taddress;
    unsigned line;
};

struct Call {
    unsigned char isMethod;
    char *name;
    int scope;
    Expr *elist;
};

struct Stmt {
    llist_t breaklist;
    llist_t contlist;
};

struct ExprList {
    Expr *val;
    struct ExprList *next;
};

struct ExprList *mkExprList();
void addExprList(struct ExprList *head, Expr *node);
void patchExprList(struct ExprList *head);

// VM ASM creation
unsigned next_quad(void);
int is_arith(Expr *e);
void init_quads(void);
void emit(enum iopcode_t op, Expr* result, Expr* arg1, Expr* arg2, unsigned label, unsigned line);
int mk_bool_vmasm(Expr *e);
int mk_nbe_vmasm(enum iopcode_t	op, Expr *e);

// Expression creation
Expr *expr(enum expression_type_t  type);
Expr *sym_expr(SymTableEntry *e);
Expr *string_expr(char *str);
Expr *nil_expr(void);
Expr *num_expr(double num);
Expr *bool_expr(unsigned char bool);
Expr *member_expr(SymTable *t, int scope, Expr *lvalue, char *name);

//offset 
scopespace_t currscopespace(void);
unsigned currscopeoffset(void);
void inccurrscopeoffset(void);
void enterscopespace(void);
void exitscopespace(void);
void resetformalargsoffset(void);
void resetfunctionlocalsoffset(void);
void restorecurrscopeoffset(unsigned n);

void reset_temp_counter(void);
SymTableEntry *new_temp(SymTable *t, int scope);
Expr *make_call(SymTable *t, int scope, Expr *call, Expr *revelist);
Expr *reverse_elist(Expr **elist);
Expr *emit_if_table_item(SymTable *t, int scope, Expr *e);

Call *function_call(unsigned char isMethod, char *name, int scope, Expr *elist);

llist_t llist(int i);
llist_t llist_merge(llist_t l1, llist_t l2);
void llist_patch(llist_t list, int label);
void patch_label(unsigned quad, unsigned label);

Stmt *stmt(void);
void print_call(Call *c);
void print_expression(Expr *e);

void printQuads(void);
#endif