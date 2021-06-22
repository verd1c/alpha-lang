#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include "consts.h"
#include "intermediate.h"
#include "stack.h"

Stack *_func_stack;

unsigned _curr_proc_quad;

incomplete_jump *ij_head = (incomplete_jump*)0;
unsigned ij_total = 0;

void setArgsNull(Instruction *t) {

    t->arg1.val = -1; t->arg1.type = 11;
    t->arg2.val = -1; t->arg2.type = 11;
    t->result.val = -1; t->result.type = 11;

}

void expand_table(enum expand_t expand_type){
    switch(expand_type){
        case EXPAND_NUM:
            assert(totalNumConsts == currNum);

            double* a = (double*)malloc(NEW_SIZE_NUMS);
            memset(a, 0, NEW_SIZE_NUMS);
            if (numConsts) {
                memcpy(a, numConsts, CURR_SIZE_NUMS);
                free(numConsts);
            }
            numConsts = a;
            totalNumConsts += EXPAND_SIZE;
            break;
        case EXPAND_STRING:
            assert(totalStringConsts == currString);

            char** b = (char**)malloc(NEW_SIZE_STRINGS);
            memset(b, 0, NEW_SIZE_STRINGS);
            if (stringConsts) {
                memcpy(b, stringConsts, CURR_SIZE_STRINGS);
                free(stringConsts);
            }
            stringConsts = b;
            totalStringConsts += EXPAND_SIZE;
            break;
        case EXPAND_LIB:
            assert(totalNamedLibfuncs == currLib);

            char** c = (char**)malloc(NEW_SIZE_LIBS);
            memset(c, 0, NEW_SIZE_LIBS);
            if (namedLibfuncs) {
                memcpy(c, namedLibfuncs, CURR_SIZE_LIBS);
                free(namedLibfuncs);
            }
            namedLibfuncs = c;
            totalNamedLibfuncs += EXPAND_SIZE;
            break;
        case EXPAND_USER:
            assert(totalUserFuncs == currUser);

            userfunc* d = (userfunc*)malloc(NEW_SIZE_USERS);
            memset(d, 0, NEW_SIZE_USERS);
            if (userFuncs) {
                memcpy(d, userFuncs, CURR_SIZE_USERS);
                free(userFuncs);
            }
            userFuncs = d;
            totalUserFuncs += EXPAND_SIZE;
            break;
        case EXPAND_INSTR:
            assert(totalInstr == currInstr);

            Instruction* e = (Instruction*)malloc(NEW_SIZE_INSTR);
            memset(e, 0, NEW_SIZE_INSTR);
            if (instructions) {
                memcpy(e, instructions, CURR_SIZE_INSTR);
                free(instructions);
            }
            instructions = e;
            totalInstr += EXPAND_SIZE;
            break;
        case EXPAND_IJ:
            assert(totalIJ == currIJ);

            incomplete_jump *f = (incomplete_jump *)malloc(NEW_SIZE_IJ);
            memset(f, 0, NEW_SIZE_IJ);
            if (ij_head) {
                memcpy(f, ij_head, CURR_SIZE_IJ);
                free(ij_head);
            }
            ij_head = f;
            totalIJ += EXPAND_SIZE;
            break;
    }
}

unsigned consts_newstring(char *s);
unsigned consts_newnumber(double n);
unsigned libfuncs_newused(char *s);
unsigned userfuncs_newfunc(SymTableEntry *sym);

void patch_jump_label(unsigned instr_no, unsigned addr) {
    instructions[instr_no].result.val = addr;
}

void patch_incomplete_jumps() {
    int i;

    for (i = 0; i < currIJ; i++) {
        if (ij_head[i].iaddress == currQuad) {
            patch_jump_label(ij_head[i].instrNo, currInstr);
        }
        else {
            patch_jump_label(ij_head[i].instrNo, quads[ij_head[i].iaddress].taddress);
        }
    }
}

void make_numberoperand(VMArg *arg, double val){
    arg->val = consts_newnumber(val);
    arg->type = number_a;
}

void reset_operand(VMArg *arg){

}

void make_booloperand(VMArg *arg, unsigned val){
    arg->val = val;
    arg->type = bool_a;
}

void make_retvaloperand(VMArg *arg){
    arg->type = retval_a;
}

void make_operand(Expr *e, VMArg *arg){
    if (!e) {
        arg->val = -1;
        return;
    }

    switch(e->type){
        case VAR_E:
        case TABLEITEM_E:
        case ARITHEXPR_E:
        case BOOLEXPR_E:
        case ASSIGNEXPR_E:
        case NEWTABLE_E: {
            assert(e->sym);

            arg->val = e->sym->offset;

            switch(e->sym->scspace){
                case programvar:
                    arg->type = global_a;
                    break;
                case functionlocal:
                    arg->type = local_a;
                    break;
                case formalarg:
                    arg->type = formal_a;
                    break;
                default:
                    assert(0);
            }
            break;
        }
        case CONSTBOOL_E: {
            arg->val = e->boolConst;
            arg->type = bool_a;
            break;
        }
        case CONSTSTRING_E: {
            arg->val = consts_newstring(e->strConst);
            arg->type = string_a;
            break;
        }
        case CONSTNUM_E: {
            arg->val = consts_newnumber(e->numConst);
            arg->type = number_a;
            break;
        }
        case NIL_E: {
            arg->type = nil_a;
            break;
        }
        case PROGRAMFUNC_E: {
            arg->type = userfunc_a;
            arg->val = e->sym->iaddress;
            arg->val = userfuncs_newfunc(e->sym);
            break;
        }
        case LIBRARYFUNC_E: {
            arg->type = libfunc_a;
            arg->val = libfuncs_newused(e->sym->value.funcValue->name);
            break;
        }
        default:
            assert(0);

    }
}

void add_incomplete_jump(unsigned instrNo, unsigned iaddress) {

    if (currIJ == totalIJ)
        expand_table(EXPAND_IJ);

    incomplete_jump *ij = ij_head + currIJ++;

    ij->instrNo = instrNo;
    ij->iaddress = iaddress;
}

unsigned consts_newstring(char *s){
    unsigned addr;

    if(currString == totalStringConsts)
        expand_table(EXPAND_STRING);

    addr = currString;

    stringConsts[currString++] = (char *)strdup(s);


    return addr;
}

unsigned consts_newnumber(double n){
    unsigned addr;
    int i;

    printf("hi\n");

    if(currNum == totalNumConsts)
        expand_table(EXPAND_NUM);

    for (i = 0; i < currNum; i++) {
        if (numConsts[i] == n) return i;
    }

    addr = currNum;

    numConsts[currNum++] = n;

    return addr;
}

unsigned libfuncs_newused(char *s){
    unsigned addr;
    int i;

    if(currLib == totalNamedLibfuncs)
        expand_table(EXPAND_LIB);

    addr = currLib;

    for (i = 0; i < currLib; i++) {
        if (strcmp(s, namedLibfuncs[i]) == 0) return i;
    }

    namedLibfuncs[currLib++] = (char*)strdup(s);

    return addr;
}

unsigned userfuncs_newfunc(SymTableEntry *sym){
    unsigned addr;
    int i;

    if(currUser == totalUserFuncs)
        expand_table(EXPAND_USER);

    addr = currUser;


    userfunc *f = userFuncs + currUser++;

    f->address = sym->iaddress;
    f->local_size = sym->total_locals;
    f->id = sym->value.funcValue->name;

    return addr;
}

void emit_instr(Instruction i){
    if(totalInstr == currInstr)
        expand_table(EXPAND_INSTR);

    Instruction *instr = instructions + currInstr++;
    instr->opcode = i.opcode;
    instr->result = i.result;
    instr->arg1 = i.arg1;
    instr->arg2 = i.arg2;
    instr->src_line = i.src_line;

    return;
}

void appendReturnList(SymTableEntry *f, unsigned label) {
    rlist *iter;

    if (!f->value.funcValue->return_list) {
        f->value.funcValue->return_list = (rlist *)malloc(sizeof(rlist));
        f->value.funcValue->return_list->label = label;
        f->value.funcValue->return_list->next = NULL;
        return;
    }

    iter = f->value.funcValue->return_list;

    while (iter->next) {
        iter = iter->next;
    }

    iter->next = (rlist *)malloc(sizeof(rlist));
    iter->next->label = label;
    iter->next->next = NULL;
    return;
}

void backpatchReturnList(SymTableEntry *f, unsigned label) {
    rlist *iter;

    iter = f->value.funcValue->return_list;

    while (iter) {
        patch_jump_label(iter->label, label);
        iter = iter->next;
    }

    return;
}

unsigned next_instruction_label(){
    return currInstr;
}

unsigned curr_processed_quad(){
    return _curr_proc_quad;
}

void generate(enum iopcode_t op, Quad *quad){
   Instruction t;
   setArgsNull(&t);
   t.src_line = quad->line;
   
   t.opcode = op;
   make_operand(quad->arg1, &t.arg1);
   make_operand(quad->arg2, &t.arg2);
   make_operand(quad->result, &t.result);
   quad->taddress = next_instruction_label();
   emit_instr(t);
}

void generate_relational (enum iopcode_t op, Quad *quad) {
    struct instruction t;
    setArgsNull(&t);
    t.src_line = quad->line;
    t.opcode = op;
    make_operand(quad->arg1, &t.arg1);
    make_operand(quad->arg2, &t.arg2);
    t.result.type = label_a;

    if (quad->label < curr_processed_quad()) {
        printf("taddress -> %d\n", quads[quad->label].taddress);
        t.result.val = quads[quad->label].taddress;
    }
    else
        add_incomplete_jump(next_instruction_label(), quad->label);

    quad->taddress = next_instruction_label();
    emit_instr(t);
}

// generators
void generate_ADD(Quad *quad) { generate(add_v, quad); }
void generate_SUB(Quad *quad) { generate(sub_v, quad); }
void generate_MUL(Quad *quad) { generate(mul_v, quad); }
void generate_UMINUS(Quad *quad) { 

    quad->arg2 = num_expr(-1);

    generate(mul_v,quad);
}
void generate_DIV(Quad *quad) { generate(div_v, quad); }
void generate_MOD(Quad *quad) { generate(mod_v, quad); }
void generate_NEWTABLE(Quad *quad) { generate(newtable_v, quad); }
void generate_TABLEGETELEM(Quad *quad) { generate(tablegetelem_v, quad); }
void generate_TABLESETELEM(Quad *quad) { generate(tablesetelem_v, quad); }
void generate_ASSIGN(Quad *quad) { generate(assign_v, quad); }
void generate_NOP() { Instruction t; t.opcode = nop_v; 
	emit_instr(t); 
}
void generate_JUMP(Quad *quad) { generate_relational(jump_v, quad); }
void generate_IF_EQ(Quad *quad) { generate_relational(jeq_v, quad); }
void generate_IF_NOTEQ(Quad *quad) { generate_relational(jne_v, quad); }
void generate_IF_GREATER(Quad *quad) { generate_relational(jgt_v, quad); }
void generate_IF_GREATEREQ(Quad *quad) { generate_relational(jge_v, quad); }
void generate_IF_LESS(Quad *quad) { generate_relational(jlt_v, quad); }
void generate_IF_LESSEQ(Quad *quad) { generate_relational(jle_v, quad); }

void generate_NOT (Quad *quad) {
    quad->taddress = next_instruction_label();
    Instruction t;
    setArgsNull(&t);
    t.src_line = quad->line;
    t.opcode = jeq_v;
    make_operand(quad->arg1, &t.arg1);
    make_booloperand(&t.arg2, 0);
    t.result.type = label_a;
    t.result.val = next_instruction_label()+3;
    emit_instr(t);
    t.opcode = assign_v;
    make_booloperand(&t.arg1, 0);
    reset_operand(&t.arg2);
    make_operand(quad->result, &t.result);
    emit_instr(t);
    t.opcode = jump_v;
    reset_operand (&t.arg1);
    reset_operand(&t.arg2);
    t.result.type = label_a;
    t.result.val = next_instruction_label()+2;
    emit_instr(t);
    t.opcode = assign_v;
    make_booloperand(&t.arg1, 1);
    reset_operand(&t.arg2);
    make_operand(quad->result, &t.result);
    emit_instr(t);
}

void generate_OR (Quad *quad) {
    quad->taddress = next_instruction_label();
    Instruction t;
    setArgsNull(&t);
    t.src_line = quad->line;
    t.opcode = jeq_v;
    make_operand(quad->arg1, &t.arg1);
    make_booloperand(&t.arg2, 1);
    t.result.type = label_a;
    t.result.val = next_instruction_label()+4;
    emit_instr(t);
    make_operand(quad->arg2, &t.arg1);
    t.result.val = next_instruction_label()+3;
    emit_instr(t); 
    t.opcode = assign_v;
    make_booloperand(&t.arg1, 0);
    reset_operand(&t.arg2);
    make_operand(quad->result, &t.result);
    emit_instr(t);
    t.opcode = jump_v;
    reset_operand (&t.arg1);
    reset_operand(&t.arg2);
    t.result.type = label_a;
    t.result.val = next_instruction_label()+2;
    emit_instr(t);
    t.opcode = assign_v;
    make_booloperand(&t.arg1, 1);
    reset_operand(&t.arg2);
    make_operand(quad->result, &t.result);
    emit_instr(t);
}

void generate_AND(Quad *quad) {
    quad->taddress = next_instruction_label();
    Instruction t;
    setArgsNull(&t);
    t.src_line = quad->line;
    t.opcode = jeq_v;
    make_operand(quad->arg1, &t.arg1);
    make_booloperand(&t.arg2, 1);
    t.result.type = label_a;
    t.result.val = next_instruction_label() + 4;
    emit_instr(t);
    make_operand(quad->arg2, &t.arg1);
    t.result.val = next_instruction_label() + 3;
    emit_instr(t);
    t.opcode = assign_v;
    make_booloperand(&t.arg1, 0);
    reset_operand(&t.arg2);
    make_operand(quad->result, &t.result);
    emit_instr(t);
    t.opcode = jump_v;
    reset_operand(&t.arg1);
    reset_operand(&t.arg2);
    t.result.type = label_a;
    t.result.val = next_instruction_label() + 2;
    emit_instr(t);
    t.opcode = assign_v;
    make_booloperand(&t.arg1, 1);
    reset_operand(&t.arg2);
    make_operand(quad->result, &t.result);
    emit_instr(t);
}

void generate_PARAM(Quad *quad) {
	quad->taddress = next_instruction_label();
	Instruction t;
    setArgsNull(&t);
    t.src_line = quad->line;
	t.opcode = pusharg_v;
	make_operand(quad->arg1, &t.arg1);
	emit_instr(t);
}

void generate_CALL(Quad *quad) {
	quad->taddress = next_instruction_label();
	Instruction t;
    setArgsNull(&t);
    t.src_line = quad->line;
	t.opcode = call_v;
	make_operand(quad->arg1, &t.arg1);
	emit_instr(t);
}

void generate_GETRETVAL(Quad *quad) {
	quad->taddress = next_instruction_label();
	Instruction t;
    setArgsNull(&t);
    t.src_line = quad->line;
	t.opcode = assign_v;
	make_operand(quad->result, &t.result);
	make_retvaloperand(&t.arg1);
	emit_instr(t);
}

void generate_FUNCSTART (Quad *quad) {
    SymTableEntry *f = quad->result->sym;
    f->iaddress = next_instruction_label();
    quad->taddress = next_instruction_label();
    //userfunctions.add(f->id,f->iaddress,f->totallocals);

    push(_func_stack, (void*)f);
    Instruction t;
    setArgsNull(&t);
    t.src_line = quad->line;
    t.opcode = funcenter_v;
    make_operand(quad->result, &t.result);
    make_booloperand(&t.arg2, 1);
    emit_instr(t);

}

void generate_RETURN (Quad *quad) {

    quad->taddress = next_instruction_label();

    Instruction t;
    setArgsNull(&t);
    t.src_line = quad->line;
    t.opcode=assign_v;
    make_retvaloperand(&t.result);
    make_operand(quad->result, &t.arg1);


    //xd
    if (!quad->result) {
        t.arg1.type = nil_a;
        t.arg1.val = -1;
    }

    emit_instr(t);


    SymTableEntry *f = (SymTableEntry *)top(_func_stack);

    //append(f.returnList,nextinstructionlabel());
    appendReturnList(f, next_instruction_label());

    t.opcode = jump_v;
    reset_operand(&t.arg1);
    reset_operand(&t.arg2);
    t.result.type = label_a;
    emit_instr(t);
}

void generate_FUNCEND(Quad *quad) {
    SymTableEntry *f = (SymTableEntry*)top(_func_stack);
    pop(_func_stack);
    // backpatch(f.returnList, nextinstructionlabel());
    backpatchReturnList(f, next_instruction_label());

    quad->taddress = next_instruction_label();
    Instruction t;
    t.opcode = funcexit_v;
    make_operand(quad->result, &t.result);
    emit_instr(t);
}

typedef void (*generator_func_t)(Quad *quad);

generator_func_t generators[] = {
    generate_ASSIGN,
    generate_ADD,
    generate_SUB,
    generate_MUL,
    generate_DIV,
    generate_MOD,
    generate_UMINUS,
    generate_OR,
    generate_OR,
    generate_NOT,
    generate_IF_EQ,
    generate_IF_NOTEQ,
    generate_IF_LESSEQ,
    generate_IF_GREATEREQ,
    generate_IF_LESS,
    generate_IF_GREATER,
    generate_CALL,
    generate_PARAM,
    generate_RETURN,
    generate_GETRETVAL,
    generate_FUNCSTART,
    generate_FUNCEND,
    generate_NEWTABLE,
    generate_TABLEGETELEM,
    generate_TABLESETELEM,
    generate_JUMP
};

void print_tables(){
    int i;

    printf("=====NUM CONSTS=====\n");
    for(i = 0; i < currNum; i++){
        printf("%d : %lf\n", i, numConsts[i]);
    }

    printf("=====STRING CONSTS=====\n");
    for (i = 0; i < currString; i++) {
        printf("%d : %s\n", i, stringConsts[i]);
    }

    printf("=====USER FUNCS=====\n");
    for (i = 0; i < currUser; i++) {
        printf("%d : %s %d %d\n", i, userFuncs[i].id, userFuncs[i].local_size, userFuncs[i].address);
    }

    printf("=====LIB FUNCS=====\n");
    for (i = 0; i < currLib; i++) {
        printf("%d : %s\n", i, namedLibfuncs[i]);
    }
    
}
char *vmOpCodeToStr[] = {
    "assign_v", "add_v", "sub_v",
    "mul_v", "div_v", "mod_v",
    "uminus_v", "and_v", "or_v",
    "not_v", "jeq_v", "jne_v",
    "jle_v", "jge_v", "jlt_v",
    "jgt_v", "call_v", "pusharg_v",
    "funcenter_v", "funcexit_v", "newtable_v",
    "tablegetelem_v", "tablesetelem_v", "jump_v", "nop_v"
};

char *vmArgOpToStr[] = {
    "label",
    "global",
    "formal",
    "local",
    "number",
    "string",
    "bool",
    "nil",
    "userfunc",
    "libfunc",
    "retval"
};

void encode_vmarg(FILE *out, VMArg arg) {

    fprintf(out, " [%s : %d] ", vmArgOpToStr[arg.type], arg.val);
}

void print_target() {
    Instruction instr;
    int i;

    printf("=====TARGET=====\n");
    for (i = 0; i < currInstr; i++) {
        instr = instructions[i];
        printf("%d : %s ", i, vmOpCodeToStr[instr.opcode]);

        if (instr.opcode == assign_v) {
            encode_vmarg(stdout, instr.result);
            encode_vmarg(stdout, instr.arg1);
        }
        else if (instr.opcode == jump_v || instr.opcode == funcenter_v || instr.opcode == funcexit_v) {
            encode_vmarg(stdout, instr.result);
        }
        else if (instr.opcode == pusharg_v || instr.opcode == call_v) {
            encode_vmarg(stdout, instr.arg1);
        }
        else if (instr.opcode == newtable_v) {
            encode_vmarg(stdout, instr.result);
        }else {
            encode_vmarg(stdout, instr.result);
            encode_vmarg(stdout, instr.arg1);
            encode_vmarg(stdout, instr.arg2);
        }
        printf("\n");
    }
}

void parse_target_code(){
    _func_stack = stack();
    int i;
    
    for(i = 1; i < currQuad; i++){
        _curr_proc_quad = i;
        (generators[quads[i].op])(&quads[i]);
    }

    patch_incomplete_jumps();

    print_tables();
    print_target();
}