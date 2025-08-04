#include <stdio.h>
#include <ctype.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include "algo.h"



struct Token{
    TokenType tokenType;
    double num;
    char op;  
};

struct charStack{
    char data[1024];
    int top;
};

struct DoubleStack{
    double data[1024];
    int top;
};

int precedence(char op){
    switch (op)
    {
    case '^': return 3;
    case '*':
    case '/':
    case '%': return 2;
    case '+':
    case '-': return 1;
    default:
        return 0;
    }
}

int isRigthAssociative(char op){
    return op == '^';
}

void push(DoubleStack *s, double val){
    s->data[++(s->top)] = val;
}

double pop(DoubleStack *s){
    return s->data[(s->top)--];
}

double applyOps(double val1, double val2, char op){
    switch (op)
    {
    case '+': return val1 + val2;
    case '-': return val1 - val2;
    case '*': return val1 * val2;
    case '/': return val1 / val2;
    case '%': return fmod(val1,val2);
    case '^': return pow(val1, val2);
    
    default:
        printf("Unknwon Operator %c", op);
        exit(1);
    }
}

void makeTokens(char *exp, Token *tokens, int *numTokens){
    int i = 0;
    int index = 0;
    int hasPrev = 0;
    Token previous;

    while (exp[i] != '\0') {
        if (isspace(exp[i])) {
            i++;
            continue;
        }
        
        if (isdigit(exp[i]) || exp[i] == '.') {
            if (hasPrev && 
               (previous.tokenType == NUMBER || 
               (previous.tokenType == PARENTHESIS && previous.op == ')'))) {
                tokens[index++] = (Token){OPERATOR, 0, '*'};
            }

            char buffer[100];
            int j = 0;
            while (isdigit(exp[i]) || exp[i] == '.') {
                buffer[j++] = exp[i++];
            }
            buffer[j] = '\0';

            tokens[index].tokenType = NUMBER;
            tokens[index].num = atof(buffer);
            previous = tokens[index++];
            hasPrev = 1;
        }

        // Operator token
        else if (strchr("+-*/%^", exp[i])) {
            tokens[index].tokenType = OPERATOR;
            tokens[index].op = exp[i];
            previous = tokens[index++];
            i++;
            hasPrev = 1;
        }

        // Parenthesis token
        else if (exp[i] == '(' || exp[i] == ')') {
            if (exp[i] == '(' && hasPrev &&
               (previous.tokenType == NUMBER || 
               (previous.tokenType == PARENTHESIS && previous.op == ')'))) {
                tokens[index++] = (Token){OPERATOR, 0, '*'};
            }

            tokens[index].tokenType = PARENTHESIS;
            tokens[index].op = exp[i];
            previous = tokens[index++];
            i++;
            hasPrev = 1;
        }

        else {
            printf("Unexpected Character '%c'\n", exp[i]);
            exit(1);
        }
    }

    *numTokens = index;
}

void infixToPostfix(Token *infix, int inCount, Token *postfix, int *postCount){
    charStack opStack = {.top = -1};
    int outIndex = 0;
    
    for (int i = 0; i < inCount; i++)
    {
        Token j = infix[i];

        if (j.tokenType == NUMBER)
        {
            postfix[outIndex++]  = j;
        }
        else if(j.tokenType == PARENTHESIS && j.op == '('){
            opStack.data[++opStack.top] = j.op;
        }
        else if (j.tokenType == PARENTHESIS && j.op == ')')
        {
            while (opStack.top >= 0 && opStack.data[opStack.top] != '(')
            {
                Token opToken = {OPERATOR, 0, opStack.data[opStack.top--]};
                postfix[outIndex++] = opToken;
            }
            if(opStack.top >= 0 && opStack.data[opStack.top] == '('){
                opStack.top--;
            }
        }
        else if(j.tokenType == OPERATOR){
            while (opStack.top >= 0)
            {
                char topOperator = opStack.data[opStack.top];

                if ((precedence(topOperator) > precedence(j.op)) || (precedence(topOperator) == precedence(j.op)) 
                && !isRigthAssociative(j.op))
                {
                    Token opToken = {OPERATOR, 0, opStack.data[opStack.top--]};
                    postfix[outIndex++] = opToken;
                }
                else{
                    break;
                }
                
            }
            opStack.data[++opStack.top] = j.op;
        }
        
    }
    
    while (opStack.top >= 0)
    {
        Token opToken = {OPERATOR, 0, opStack.data[opStack.top--]};
        postfix[outIndex++] = opToken;
    }
    
    *postCount = outIndex;

}

double evalExpr(Token *postFix, int postCount){
    DoubleStack stack = {.top = -1};

    for (int i = 0; i < postCount; i++)
    {
        Token k = postFix[i];

        if (k.tokenType == NUMBER)
        {
            push(&stack, k.num);
        }
        else if (k.tokenType ==OPERATOR)
        {
            double b = pop(&stack);
            double a = pop(&stack);

            double result = applyOps(a, b, k.op);
            push(&stack, result);
        }
        
        
    }
    return pop(&stack);
}

