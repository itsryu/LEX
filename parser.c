#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lexer.h"
#include "parser.h"

ASTNode* createNode(int type, char* value) {
    ASTNode* node = (ASTNode*)malloc(sizeof(ASTNode));
    node->type = type;
    node->value = strdup(value);
    node->left = NULL;
    node->right = NULL;
    return node;
}

void freeNode(ASTNode* node) {
    if (node) {
        free(node->value);
        free(node->left);
        free(node->right);
        free(node);
    }
}

ASTNode* parseVarDeclaration(Token** tokens) {
    Token* token = *tokens;
    ASTNode* varNode = createNode(token->type, token->word);
    token = token->next;

    while (token && token->word != ';') {
        ASTNode* idNode = createNode(token->word, token->word);
        varNode->right = idNode;
        varNode = idNode;
        token = token->next;
    }

    *tokens = token->next;
    return varNode;
}

// Função para analisar uma atribuição
ASTNode* parseAssignment(Token** tokens) {
    Token* token = *tokens;
    ASTNode* idNode = createNode(token->type, token->word);
    token = token->next;

    if (token && strcmp(token->word, ":=") == 0) {
        ASTNode* assignNode = createNode(token->type, token->word);
        assignNode->left = idNode;
        token = token->next;

        ASTNode* valueNode = createNode(token->type, token->word);
        assignNode->right = valueNode;
        *tokens = token->next; 
        return assignNode;
    }

    return NULL;
}

ASTNode* parseWhileLoop(Token** tokens) {
    Token* token = *tokens;
    ASTNode* whileNode = createNode(token->type, token->word);
    token = token->next;

    ASTNode* conditionNode = createNode(token->type, token->word);
    whileNode->left = conditionNode;
    token = token->next;

    ASTNode* bodyNode = createNode(token->type, "body");
    whileNode->right = bodyNode;

    while (token && token->type != END_OF_FILE) {
        ASTNode* statementNode = parseAssignment(&token);
        if (statementNode) {
            bodyNode->right = statementNode;
            bodyNode = statementNode;
        }
        token = token->next;
    }

    *tokens = token->next;
    return whileNode;
}

ASTNode* parseTokens(Token* tokens) {
    ASTNode* root = createNode(0, "program");
    ASTNode* current = root;

    while (tokens && tokens->type != END_OF_FILE) {
        if (tokens->type == IDENTIFIER) {
            ASTNode* varNode = parseVarDeclaration(&tokens);
            current->right = varNode;
            current = varNode;
        } else if (tokens->type == IDENTIFIER) {
            ASTNode* assignNode = parseAssignment(&tokens);
            current->right = assignNode;
            current = assignNode;
        } else if (strcmp(tokens->word, RESERVED_WORD_WHILE) == 0) {
            ASTNode* whileNode = parseWhileLoop(&tokens);
            current->right = whileNode;
            current = whileNode;
        } else {
            tokens = tokens->next;
        }
    }

    return root;
}