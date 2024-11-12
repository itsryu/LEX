#pragma once

#include "lexer.h"

typedef struct ASTNode {
    int type;
    char* value;
    struct ASTNode* left;
    struct ASTNode* right;
} ASTNode;

ASTNode* parseTokens(Token* tokens);