#ifndef ALGO_H
#define ALGO_H

#include <stdio.h>
#include <ctype.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

typedef enum {NUMBER, OPERATOR, PARENTHESIS} TokenType;

typedef struct {
    TokenType tokenType;
    double num;
    char op;
} Token;

typedef struct {
    char data[1024];
    int top;
} charStack;

typedef struct {
    double data[1024];
    int top;
} DoubleStack;

// Function declarations
int precedence(char op);
int isRigthAssociative(char op);

void push(DoubleStack *s, double val);
double pop(DoubleStack *s);
double applyOps(double val1, double val2, char op);

void makeTokens(char *exp, Token *tokens, int *numTokens);
void infixToPostfix(Token *infix, int inCount, Token *postfix, int *postCount);
double evalExpr(Token *postFix, int postCount);

#endif