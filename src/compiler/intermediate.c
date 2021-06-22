#include "intermediate.h"
#include "tools.h"
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include "consts.h"
#include <math.h>

unsigned int programVarOffset = 0;
unsigned int functionLocalOffset = 0;
unsigned int formalArgOffset = 0;
unsigned int scopeSpaceCounter = 1;

int is_arith(Expr *e){
	if(	e->type == CONSTBOOL_E 		||
		e->type == CONSTSTRING_E	||
		e->type == NIL_E			||
		e->type == NEWTABLE_E		||
		e->type == PROGRAMFUNC_E	||
		e->type == LIBRARYFUNC_E	||
		e->type == BOOLEXPR_E){
			return 0;
	}
	return 1;
}

unsigned next_quad(void){
	return currQuad;
}

void init_quads(void) {
	quads = (Quad*)0;
	total = 1;
	currQuad = 1;

	return;
}

void expand(void) {

	// expand should only be called when full
	assert(total == currQuad);

	Quad* p = (Quad*)malloc(NEW_SIZE);
	memset(p, 0, NEW_SIZE);
	if (quads) {
		memcpy(p, quads, CURR_SIZE);
		free(quads);
	}
	quads = p;
	total += EXPAND_SIZE;

	return;
}

struct ExprList *mkExprList() {
	struct ExprList *el;
	el = (struct ExprList *)malloc(sizeof(struct ExprList));
	el->val = NULL;
	el->next = NULL;
	return el;
}

void addExprList(struct ExprList *head, Expr *node) {
	struct ExprList *iter;

	if (!head->val) {
		head->val = node;
	}

	iter = head;

	while (iter->next) {
		iter = iter->next;
	}

	iter->next = (struct ExprList *)malloc(sizeof(struct ExprList));
	iter->next->val = node;
	iter->next->next = NULL;
	return;
}

void patchExprList(struct ExprList *head) {
	int count = 0;
	struct ExprList *iter;

	iter = head;

	while (iter) {
		count++;
		printf("%s ?\n", iter->val->sym->value.varValue->name);
		iter = iter->next;
	}

	iter = head;
	count = count - 1;
	while (iter) {
		iter->val->sym->offset = count--;
		iter = iter->next;
	}
}

void emit(
	enum iopcode_t	op,
	Expr* result,
	Expr* arg1,
	Expr* arg2,
	unsigned		label,
	unsigned		line) {

	if (currQuad == total)
		expand();

	Quad* p = quads + currQuad++;
	p->op = op;
	p->result = result;
	p->arg1 = arg1;
	p->arg2 = arg2;
	p->label = label;
	p->line = line;

	return;
}


// create string const expr
Expr* string_expr(char* str) {
	Expr* expr = (Expr*)malloc(sizeof(Expr));

	if (!expr) {
		alpha_message(stdout, MEMORY_ERROR, "malloc");
		exit(0);
	}

	memset(expr, 0, sizeof(Expr));

	expr->type = CONSTSTRING_E;
	expr->strConst = strdup(str);

	return expr;
}

Expr *nil_expr(void) {
	Expr *expr = (Expr *)malloc(sizeof(Expr));

	if (!expr) {
		alpha_message(stdout, MEMORY_ERROR, "malloc");
		exit(0);
	}

	memset(expr, 0, sizeof(Expr));

	expr->type = NIL_E;

	return expr;
}

Expr* bool_expr(unsigned char bool) {
	Expr* expr = (Expr*)malloc(sizeof(Expr));

	assert(bool == 0 || bool == 1);

	if (!expr) {
		alpha_message(stdout, MEMORY_ERROR, "malloc");
		exit(0);
	}

	memset(expr, 0, sizeof(Expr));

	expr->type = CONSTBOOL_E;
	expr->boolConst = bool;

	return expr;
}

Expr *num_expr(double num) {
	Expr *expr = (Expr *)malloc(sizeof(Expr));

	if (!expr) {
		alpha_message(stdout, MEMORY_ERROR, "malloc");
		exit(0);
	}

	memset(expr, 0, sizeof(Expr));

	expr->type = CONSTNUM_E;
	expr->numConst = num;

	return expr;
}

Expr* sym_expr(SymTableEntry *e){
	Expr *expr = (Expr*)malloc(sizeof(Expr));

	memset(expr, 0, sizeof(Expr));

	expr->type = VAR_E;

	if (e->type == LIB_FUNC) {
		expr->type = LIBRARYFUNC_E;
	}else if (e->type == USER_FUNC) {
		expr->type = PROGRAMFUNC_E;
	}

	expr->sym = e;

	return expr;
}


/**/
void reset_temp_counter(void) {
	_temp_counter = 0;
}

char* new_temp_name(void) {
	char* name;
	name = (char*)malloc(10 * sizeof(char));

	if (!name) {
		alpha_message(stdout, MEMORY_ERROR, "malloc");
		exit(0);
	}

	sprintf(name, "^%d", (int)_temp_counter++);
	return name;
}


// (x = (i + (1 + c)));
//             ^1
//           ^2

SymTableEntry* new_temp(SymTable* t, int scope) {
	SymTableEntry *e;
	char *t_name;

	t_name = new_temp_name();

	scopespace_t dd = currscopespace();
	printf("At quad %d I am at scope space: %d\n", currQuad, dd);
	if (dd == 0) {
		e = lookup_no_type(t, t_name, 0);
	}
	else {
		e = lookup_no_type(t, t_name, scope);
	}

	if (e) {
		return e;
	}
	else {

		// BREAK EVERYTHING
		if (scope == 0) {
			e = insert(t, t_name, scope, yylineno, GLOBAL_VAR);
		}
		else {
			e = insert(t, t_name, scope, yylineno, LOCAL_VAR);
		}
		//inccurrscopeoffset();
		return e;
	}
}

Call *function_call(unsigned char isMethod,
					char *name,
					int scope,
					Expr *elist){
	Call *call = (Call *)malloc(sizeof(Call));
	assert(call);
	call->isMethod = isMethod;
	if(name)
		call->name = strdup(name);
	else
		call->name = NULL;
	call->scope = scope;
	call->elist = elist;
	return call;
}


Expr *make_call(SymTable* t, int scope, Expr *call, Expr *revelist) {
	Expr *iter, *res;
	call = emit_if_table_item(t, scope, call);

	if (revelist->type != NIL_E) {
		iter = revelist;
		while (iter) {
			emit(PARAM_I, NULL, iter, NULL, 0, yylineno);
			iter = iter->next;
		}
	}
	emit(CALL_I, NULL, call, NULL, 0, yylineno);

	res = sym_expr(new_temp(t, scope));
	emit(GETRETVAL_I, res, NULL, NULL, 0, yylineno);
	return res;
}

Expr *reverse_elist(Expr **elist) {
	Expr *prev = NULL, *cur = NULL, *next = NULL;

	cur = *elist;
	while (cur) {
		next = cur->next;
		cur->next = prev;

		prev = cur;
		cur = next;
	}
	*elist = prev;
	return *elist;
}

Expr *emit_if_table_item(SymTable *t, int scope, Expr *e) {
	Expr *result;

	if (e->type != TABLEITEM_E)
		return e;

	result = sym_expr(new_temp(t, scope));
	emit(TABLEGETELEM_I, result, e, e->index, next_quad(), yylineno);
	return result;
}

Expr *expr(enum expression_type_t  type) {
	Expr *e;

	e = (Expr *)malloc(sizeof(Expr));

	if (!e) {
		alpha_message(stdout, MEMORY_ERROR, "malloc");
		exit(0);
	}

	memset(e, 0, sizeof(Expr));

	e->type = type;

	return e;
}

Expr *member_expr(SymTable *t, int scope, Expr *lvalue, char *name) {
	Expr *ti;
	
	lvalue = emit_if_table_item(t, scope, lvalue);
	ti = expr(TABLEITEM_E);
	ti->sym = lvalue->sym;
	ti->index = string_expr(name);
	return ti;
}

llist_t llist(int i){
	return i;
}

llist_t llist_merge(llist_t l1, llist_t l2){
	llist_t iter;

	if(!l1){
		return l2;
	}else if(!l2){
		return l1;
	}else{
		iter = l1;
		while(quads[iter].label){
			iter = quads[iter].label;
		}
		quads[iter].label = l2;
		return l1;
	}
}

void patch_label(unsigned quad, unsigned label){
	quads[quad].label = label;
}

void llist_patch(llist_t list, int label){
	llist_t next;
	while(list){
		next = quads[list].label;
		quads[list].label = label;
		list = next;
	}
	return;
}

void print_expression(Expr *e){
	char *appended;

	if(e){
		if(e->type == CONSTNUM_E){
			if(ceilf(e->numConst) == e->numConst)
				printf(" %-3d ", (int)ceilf(e->numConst));
			else
				printf(" %-3f ", e->numConst);
		}else if(e->type == CONSTBOOL_E){
			if(e->boolConst == 0)
				printf(" %-5s ", "false");
			else
				printf(" %-5s ", "true");
		}else if(e->type == CONSTSTRING_E){
			appended = (char*)malloc((strlen(e->strConst) + 2) * sizeof(char));
			memset(appended, 0, strlen(e->strConst) + 2);
			strcpy(appended, e->strConst);
			appended[strlen(e->strConst)] = '\"';

			printf(" \"%-13s ", appended);
		}else if(e->type == NIL_E){
			printf(" NIL ");
		}
		else {
			if(e->sym->type == LOCAL_VAR || e->sym->type == GLOBAL_VAR || e->sym->type == ARGUMENT_VAR)
				printf(" %-13s ", e->sym->value.varValue->name);
			else
				printf(" %-13s ", e->sym->value.funcValue->name);
		}
	}
	return;
}

int mk_bool_vmasm(Expr *e){
	if(e->type == BOOLEXPR_E){

		// build case of true
		llist_patch(e->truelist, next_quad());
		emit(ASSIGN_I, e, bool_expr(1), NULL, 0, yylineno);
		emit(JUMP_I, NULL, NULL, NULL, next_quad() + 2, yylineno);

		// build case of false
		llist_patch(e->falselist, next_quad());
		emit(ASSIGN_I, e, bool_expr(0), NULL, 0, yylineno);

		return 1;
	}else{
		return 0;
	}
}

int mk_nbe_vmasm(enum iopcode_t	op, Expr *e){
	if(e->type != BOOLEXPR_E){
		e->truelist = llist(next_quad());
		e->falselist = llist(next_quad() + 1);
		emit(IF_EQ_I, NULL, e, bool_expr(1), 0, yylineno);
		emit(JUMP_I, NULL, NULL, NULL, 0, yylineno);
		
		if(op == AND_I)
			llist_patch(e->truelist, next_quad());
		else if(op == OR_I)
			llist_patch(e->falselist, next_quad());

		return 1;
	}else{
		return 0;
	}
}

Stmt *stmt(void){
	Stmt *statement = (Stmt*)malloc(sizeof(Stmt));
	statement->breaklist = 0;
	statement->contlist = 0;
	return statement;
}

void print_call(Call *c) {
	printf("Call [%d] [%-10s] [%-3d]\n", c->isMethod, c->name, c->scope);
}

 char *opcodeToStrin_g[] =  {
        "ASSIGN",  "ADD",    "SUB",
        "MUL",    "DIV",    "MOD",
        "UMINUS", "AND",    "OR",
        "NOT",    "IF_EQ",  "IF_NOTEQ",
        "IF_LESSEQ",  "IF_GREATEREQ",   "IF_LESS",
        "IF_GREATER",    "CALL", "PARAM",
        "RET",   "GETRETVAL",  "FUNCSTART",
        "FUNCEND",    "TABLECREATE", 
        "TABLEGETELEM",   "TABLESETELEM", "JUMP"
 };

void printQuads(void){
    Quad q;
    int i;
	
    for(i = 1; i < currQuad; i++){
        q = quads[i];
        printf(" %-3d: %-13s ", i, opcodeToStrin_g[q.op]);

        // print result
        if(q.op == JUMP_I){
            printf(" %-3d ", q.label);
        }else if(q.op == IF_LESS_I || q.op == IF_GREATER_I || q.op == IF_LESSEQ_I || q.op == IF_GREATEREQ_I || q.op == IF_NOTEQ_I || q.op == IF_EQ_I){
            print_expression(q.result);
            print_expression(q.arg1);
            print_expression(q.arg2);
            printf(" %-3d ", q.label);
        }else{
            print_expression(q.result);
            print_expression(q.arg1);
            print_expression(q.arg2);
        }

        printf("\n");
    }
}

scopespace_t currscopespace(void)
{
	if (scopeSpaceCounter == 1)
	{
		return programvar;
	}
	else if (scopeSpaceCounter % 2 == 0)
	{
		return formalarg;
	}
	else
	{
		return functionlocal;
	}
}

unsigned currscopeoffset(void)
{
	switch (currscopespace())
	{
	case programvar:
		return programVarOffset;
	case functionlocal:
		return functionLocalOffset;
	case formalarg:
		return formalArgOffset;
	default:
		assert(0);
	}
	return 0;
}

void inccurrscopeoffset(void)
{
	switch (currscopespace())
	{
	case programvar:
		++programVarOffset;
		break;
	case functionlocal:
		++functionLocalOffset;
		break;
	case formalarg:
		++formalArgOffset;
		break;
	default:
		assert(0);
	}
}

void enterscopespace(void) { ++scopeSpaceCounter; }

void exitscopespace(void)
{
	assert(scopeSpaceCounter > 1);
	--scopeSpaceCounter;
}

void resetformalargsoffset(void)
{
	formalArgOffset = 0;
}

void resetfunctionlocalsoffset(void)
{
	functionLocalOffset = 0;
}

void restorecurrscopeoffset(unsigned n)
{
	switch (currscopespace())
	{
	case programvar:
		programVarOffset = n;
		break;
	case functionlocal:
		functionLocalOffset = n;
		break;
	case formalarg:
		formalArgOffset = n;
		break;
	default:
		assert(0);
	}
}