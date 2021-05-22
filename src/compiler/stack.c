#include <stdio.h>
#include <stdlib.h>
#include "stack.h"

Stack *stack(){
    Stack *s;

    s = (Stack*)malloc(sizeof(Stack));
    s->size = STACK_SIZE;
    s->top = -1;

    return s;
}

void push(Stack *s, void *data) {
    if (s->top == -1) { // Empty
        s->stack[s->size - 1] = data;
        s->top = s->size - 1;
    }else if (s->top == 0) { // Full
        return;
    }else {
        s->stack[s->top - 1] = data;
        s->top = s->top - 1;
    }
    return;
}

void pop(Stack *s) {
    if (s->top == -1) {
        return;
    }else {
        if (s->top == s->size - 1) {
            s->top = -1;
        }else {
            s->top = s->top + 1;
        }
    }
    return;
}

void *top(Stack *s){
    if (s->top == -1) {
        return (void*)0;
    }else {
        return s->stack[s->top];
    }
}