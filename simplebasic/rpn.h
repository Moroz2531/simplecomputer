#ifndef REVERSE_POLISH_NOTATION_H
#define REVERSE_POLISH_NOTATION_H

typedef struct node
{
  char data;
  struct node *next;
} node;

void errOutput (char *message);

// Стек
void stack_push (char data, node **top);
void stack_print (node *top);
char stack_pop (node **top);
char stack_top (node *top);

// RPN
int check_priority (char sign);
char *translateToRPN (char *inf, char rpn[]);

#endif