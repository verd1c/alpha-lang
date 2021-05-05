#include <stdio.h>
#include "stack.h"

void push_c(Stack *s, void *data) {
    if (s->top == -1) { // Empty
        s->stack[s->size - 1] = data;
        s->top = s->size - 1;
    }else if (s->top == 0) { // Full
        printf("Error: Stack Full\n");
    }else {
        s->stack[s->top - 1] = data;
        s->top = s->top - 1;
    }
    return;
}

void pop_c(Stack *s) {
    if (s->top == -1) {
        printf("Empty stack\n");
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
    return (void*)0;
}