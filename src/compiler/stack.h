#ifndef __STACK_H__
#define __STACK_H__

#define STACK_SIZE 64

typedef struct Stack Stack;

struct Stack {
	int size;
	int top;

	void *stack[STACK_SIZE];
};

void push_c(Stack *s, void *data);
void pop_c(Stack *s);
void *top_c(Stack *s);


#endif