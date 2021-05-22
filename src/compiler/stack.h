#ifndef __STACK_H__
#define __STACK_H__

#define STACK_SIZE 64

typedef struct Stack Stack;

struct Stack {
	int size;
	int top;

	void *stack[STACK_SIZE];
};

Stack *stack();
void push(Stack *s, void *data);
void pop(Stack *s);
void *top(Stack *s);


#endif