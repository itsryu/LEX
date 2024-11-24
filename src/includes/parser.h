#pragma once

#include "./lexer.h"

typedef struct ASTNode {
    int type;
    char* value;
    struct ASTNode* left;
    struct ASTNode* right;
} ASTNode;

// symbols:
#define SYMBOL_OBC "{"
#define SYMBOL_CBC "}"
#define SYMBOL_SEM ";"
#define SYMBOL_OPA "("
#define SYMBOL_CPA ")"
#define SYMBOL_DOT "."
#define SYMBOL_COM ","
#define SYMBOL_COLON ":"
#define SYMBOL_SQT "'"
#define SYMBOL_DQT "\""
#define SYMBOL_UNDER "_"

// operators:
#define OPERATOR_SUM "+"
#define OPERATOR_SUB "-"
#define OPERATOR_DIV "/"
#define OPERATOR_MUL "*"
#define OPERATOR_EQU "="
#define OPERATOR_LT "<"
#define OPERATOR_GT ">"

ASTNode* parseTokens(Table* table);