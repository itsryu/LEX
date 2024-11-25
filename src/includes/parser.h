#pragma once

#include "./lexer.h"

/**
 * @brief Represents a node in an Abstract Syntax Tree (AST).
 * 
 * This structure is used to represent nodes in an AST, which is a tree representation
 * of the abstract syntactic structure of source code.
 */
typedef struct ASTNode
{
    int type;
    char *value;
    struct ASTNode *left;
    struct ASTNode *right;
} ASTNode;

ASTNode *parseTokens(Table *table);