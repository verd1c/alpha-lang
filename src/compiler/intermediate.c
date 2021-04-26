#include "intermediate.h"
#include "tools.h"
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>

void init_quads(void) {
	quads = (Quad*)0;
	total = 0;
	currQuad = 0;

	return;
}

void expand(void) {

	// expand should only be called when full
	assert(total == currQuad);

	Quad* p = (Quad*)malloc(NEW_SIZE);
	if (quads) {
		memcpy(p, quads, CURR_SIZE);
		free(quads);
	}
	quads = p;
	total += EXPAND_SIZE;

	return;
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
	expr->sym = e;

	return expr;
}


/**/
inline void reset_temp_counter(void) {
	_temp_counter = 0;
}

char* new_temp_name(void) {
	char* name;
	name = (char*)malloc(10 * sizeof(char));

	if (!name) {
		alpha_message(stdout, MEMORY_ERROR, "malloc");
		exit(0);
	}

	sprintf(name, "^%d", _temp_counter++);
	return name;
}


// (x = (i + (1 + c)));
//             ^1
//           ^2

SymTableEntry* new_temp(SymTable* t, int scope) {
	return insert(t, new_temp_name(), scope, yylineno, LOCAL_VAR);
}

Call *function_call(unsigned char isMethod,
					char *name,
					int scope,
					Expr *elist){
	Call *call = (Call *)malloc(sizeof(Call));
	assert(call);
	call->isMethod = isMethod;
	call->name = strdup(name);
	call->scope = scope;
	call->elist = elist;
	return call;
}


Expr *make_call(SymTable* t, int scope, Expr *call, Expr *revelist) {
	Expr *iter, *res;

	iter = revelist;
	while (iter) {
		emit(PARAM_I, NULL, iter, NULL, 0, 0);
		iter = iter->next;
	}
	emit(CALL_I, NULL, call, NULL, 0, 0);

	res = sym_expr(new_temp(t, scope));
	emit(GETRETVAL_I, res, NULL, NULL, 0, 0);
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

void print_call(Call *c) {
	printf("Call [%d] [%-10s] [%-3d]\n", c->isMethod, c->name, c->scope);
}