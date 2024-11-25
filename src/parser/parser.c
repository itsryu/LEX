#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../includes/lexer.h"
#include "../includes/parser.h"
#include "../includes/errors.h"
#include "../includes/tokens.h"

static int isValidNumber(const char *str);

ASTNode *createNode(int type, char *value)
{
    ASTNode *node = (ASTNode *)malloc(sizeof(ASTNode));

    if (!node)
    {
        fprintf(stderr, ERR_MEMORY_ALLOCATION_FAILED);
        exit(EXIT_FAILURE);
    }

    node->type = type;
    node->value = value;
    node->left = NULL;
    node->right = NULL;

    return node;
}

void freeNode(ASTNode *node)
{
    if (node)
    {
        if (node->value) {
            free(node->value);
        }

        freeNode(node->left);
        freeNode(node->right);
        free(node);
    }
}

static int isValidNumber(const char *str)
{
    if (*str == END_OF_STRING)
        return 0;
    while (*str)
    {
        if ((*str < '0' || *str > '9') && *str != SMB_DOT)
            return 0;
        str++;
    }
    return 1;
}

ASTNode *parseConditional(Table *table, Entry **currentEntry)
{
    Entry *entry = *currentEntry;

    if (entry->token->type != RESERVED_WORD || strcmp(entry->token->word, RESERVED_WORD_IF) != 0)
    {
        fprintf(stderr, ERR_EXPECTED_IF, entry->token->row, entry->token->column);
        exit(EXIT_FAILURE);
    }

    ASTNode *ifNode = createNode(entry->token->type, entry->token->word);

    if (entry->next == NULL)
    {
        fprintf(stderr, ERR_EXPECTED_EXPRESSION_AFTER_IF, entry->token->row, entry->token->column);
        freeNode(ifNode);
        exit(EXIT_FAILURE);
    }

    *currentEntry = entry->next;
    entry = *currentEntry;

    ASTNode *conditionNode = parseExpression(table, currentEntry);
    ifNode->left = conditionNode;
    entry = *currentEntry;

    if (entry->token->type != RESERVED_WORD || strcmp(entry->token->word, RESERVED_WORD_THEN) != 0)
    {
        fprintf(stderr, ERR_EXPECTED_THEN, entry->token->row, entry->token->column);
        freeNode(ifNode);
        exit(EXIT_FAILURE);
    }

    if (entry->next == NULL)
    {
        fprintf(stderr, ERR_EXPECTED_STATEMENT_AFTER_THEN, entry->token->row, entry->token->column);
        freeNode(ifNode);
        exit(EXIT_FAILURE);
    }

    *currentEntry = entry->next;
    entry = *currentEntry;

    ASTNode *thenNode = parseStatement(table, currentEntry);
    ifNode->right = thenNode;
    entry = *currentEntry;

    if (entry && entry->token->type == RESERVED_WORD && strcmp(entry->token->word, RESERVED_WORD_ELSE) == 0)
    {
        if (entry->next == NULL)
        {
            fprintf(stderr, ERR_EXPECTED_STATEMENT_AFTER_ELSE, entry->token->row, entry->token->column);
            freeNode(ifNode);
            exit(EXIT_FAILURE);
        }

        *currentEntry = entry->next;
        entry = *currentEntry;

        ASTNode *elseNode = parseStatement(table, currentEntry);
        ASTNode *elseBranchNode = createNode(RESERVED_WORD, RESERVED_WORD_ELSE);
        elseBranchNode->left = ifNode->right;
        elseBranchNode->right = elseNode;
        ifNode->right = elseBranchNode;
    }

    return ifNode;
}

ASTNode *parseRepetitive(Table *table, Entry **currentEntry)
{
    Entry *entry = *currentEntry;

    if (entry->token->type != RESERVED_WORD || strcmp(entry->token->word, RESERVED_WORD_WHILE) != 0)
    {
        fprintf(stderr, ERR_EXPECTED_WHILE, entry->token->row, entry->token->column);
        exit(EXIT_FAILURE);
    }

    ASTNode *whileNode = createNode(entry->token->type, entry->token->word);

    if (entry->next == NULL)
    {
        fprintf(stderr, ERR_EXPECTED_EXPRESSION_AFTER_WHILE, entry->token->row, entry->token->column);
        freeNode(whileNode);
        exit(EXIT_FAILURE);
    }

    *currentEntry = entry->next;
    entry = *currentEntry;

    ASTNode *conditionNode = parseExpression(table, currentEntry);
    whileNode->left = conditionNode;
    entry = *currentEntry;

    if (entry->token->type != RESERVED_WORD || strcmp(entry->token->word, RESERVED_WORD_DO) != 0)
    {
        fprintf(stderr, ERR_EXPECTED_DO, entry->token->row, entry->token->column);
        freeNode(whileNode);
        exit(EXIT_FAILURE);
    }

    if (entry->next == NULL)
    {
        fprintf(stderr, ERR_EXPECTED_STATEMENT_AFTER_DO, entry->token->row, entry->token->column);
        freeNode(whileNode);
        exit(EXIT_FAILURE);
    }

    *currentEntry = entry->next;
    entry = *currentEntry;

    ASTNode *doNode = parseStatement(table, currentEntry);
    whileNode->right = doNode;

    return whileNode;
}

ASTNode *parseFactor(Table *table, Entry **currentEntry)
{
    Entry *entry = *currentEntry;
    ASTNode *factorNode = NULL;

    if (entry->token->type == NUMBER)
    {
        if (!isValidNumber(entry->token->word))
        {
            fprintf(stderr, ERR_INVALID_NUMBER, entry->token->word, entry->token->row, entry->token->column);
            exit(EXIT_FAILURE);
        }
        factorNode = createNode(entry->token->type, entry->token->word);

        if (entry->next == NULL)
        {
            fprintf(stderr, ERR_EXPECTED_EXPRESSION_OR_SEMICOLON, entry->token->row, entry->token->column);
            freeNode(factorNode);
            exit(EXIT_FAILURE);
        }

        *currentEntry = entry->next;
    }
    else if (entry->token->type == IDENTIFIER)
    {
        factorNode = createNode(entry->token->type, entry->token->word);
        *currentEntry = entry->next;
    }
    else if (strcmp(entry->token->word, SYMBOL_OPA) == 0)
    {
        if (entry->next == NULL)
        {
            fprintf(stderr, ERR_EXPECTED_EXPRESSION_AFTER_OPEN_PAREN, entry->token->row, entry->token->column);
            exit(EXIT_FAILURE);
        }

        *currentEntry = entry->next;
        factorNode = parseExpression(table, currentEntry);
        entry = *currentEntry;

        if (entry && strcmp(entry->token->word, SYMBOL_CPA) == 0)
        {
            if (entry->next == NULL)
            {
                fprintf(stderr, ERR_EXPECTED_EXPRESSION_OR_SEMICOLON, entry->token->row, entry->token->column);
                freeNode(factorNode);
                exit(EXIT_FAILURE);
            }

            *currentEntry = entry->next;
            entry = *currentEntry;
        }
        else
        {
            fprintf(stderr, ERR_EXPECTED_CLOSE_PAREN, entry->token->row, entry->token->column);
            freeNode(factorNode);
            exit(EXIT_FAILURE);
        }
    }
    else
    {
        fprintf(stderr, ERR_INVALID_FACTOR, entry->token->word, entry->token->row, entry->token->column);
        exit(EXIT_FAILURE);
    }

    return factorNode;
}

ASTNode *parseTerm(Table *table, Entry **currentEntry)
{
    ASTNode *termNode = parseFactor(table, currentEntry);
    Entry *entry = *currentEntry;

    while (entry && (strcmp(entry->token->word, OPERATOR_MUL) == 0 || strcmp(entry->token->word, OPERATOR_MUL) == 0))
    {
        ASTNode *operatorNode = createNode(entry->token->type, entry->token->word);

        if (entry->next == NULL)
        {
            fprintf(stderr, ERR_EXPECTED_FACTOR_AFTER_OPERATOR, entry->token->word, entry->token->row, entry->token->column);
            freeNode(termNode);
            freeNode(operatorNode);
            exit(EXIT_FAILURE);
        }

        *currentEntry = entry->next;

        if (*currentEntry == NULL)
        {
            fprintf(stderr, ERR_EXPECTED_FACTOR_AFTER_OPERATOR, entry->token->word, entry->token->row, entry->token->column);
            freeNode(termNode);
            freeNode(operatorNode);
            exit(EXIT_FAILURE);
        }

        ASTNode *rightFactor = parseFactor(table, currentEntry);

        operatorNode->left = termNode;
        operatorNode->right = rightFactor;
        termNode = operatorNode;
        entry = *currentEntry;
    }

    return termNode;
}

ASTNode *parseSimpleExpression(Table *table, Entry **currentEntry)
{
    Entry *entry = *currentEntry;
    ASTNode *simpleExprNode = NULL;

    if (entry->token->type == OPERATOR && (strcmp(entry->token->word, OPERATOR_SUM) == 0 || strcmp(entry->token->word, OPERATOR_SUB) == 0))
    {
        simpleExprNode = createNode(entry->token->type, entry->token->word);

        if (entry->next == NULL)
        {
            fprintf(stderr, ERR_EXPECTED_TERM_AFTER_OPERATOR, entry->token->word, entry->token->row, entry->token->column);
            freeNode(simpleExprNode);
            exit(EXIT_FAILURE);
        }

        *currentEntry = entry->next;
        entry = *currentEntry;
    }

    ASTNode *termNode = parseTerm(table, currentEntry);
    if (simpleExprNode)
    {
        simpleExprNode->left = termNode;
    }
    else
    {
        simpleExprNode = termNode;
    }

    entry = *currentEntry;

    while (entry && (strcmp(entry->token->word, OPERATOR_SUM) == 0 || strcmp(entry->token->word, OPERATOR_SUB) == 0))
    {
        ASTNode *operatorNode = createNode(entry->token->type, entry->token->word);

        if (entry->next == NULL)
        {
            fprintf(stderr, ERR_EXPECTED_TERM_AFTER_OPERATOR, entry->token->word, entry->token->row, entry->token->column);
            freeNode(simpleExprNode);
            freeNode(operatorNode);
            exit(EXIT_FAILURE);
        }

        *currentEntry = entry->next;
        entry = *currentEntry;

        ASTNode *rightTermNode = parseTerm(table, currentEntry);
        operatorNode->left = simpleExprNode;
        operatorNode->right = rightTermNode;
        simpleExprNode = operatorNode;
        entry = *currentEntry;
    }

    return simpleExprNode;
}

ASTNode *parseExpression(Table *table, Entry **currentEntry)
{
    ASTNode *expressionNode = parseSimpleExpression(table, currentEntry);
    Entry *entry = *currentEntry;

    if (entry && (strcmp(entry->token->word, "=") == 0 || strcmp(entry->token->word, "<>") == 0 ||
                  strcmp(entry->token->word, "<") == 0 || strcmp(entry->token->word, "<=") == 0 ||
                  strcmp(entry->token->word, ">") == 0 || strcmp(entry->token->word, ">=") == 0))
    {
        ASTNode *relationNode = createNode(entry->token->type, entry->token->word);

        if (entry->next == NULL)
        {
            fprintf(stderr, ERR_EXPECTED_EXPRESSION_AFTER_OPERATOR, entry->token->word, entry->token->row, entry->token->column);
            freeNode(expressionNode);
            freeNode(relationNode);
            exit(EXIT_FAILURE);
        }

        *currentEntry = entry->next;
        entry = *currentEntry;

        if (entry == NULL)
        {
            fprintf(stderr, ERR_EXPECTED_EXPRESSION_AFTER_OPERATOR, entry->token->word, entry->token->row, entry->token->column);
            freeNode(expressionNode);
            freeNode(relationNode);
            exit(EXIT_FAILURE);
        }

        ASTNode *rightExprNode = parseSimpleExpression(table, currentEntry);
        relationNode->left = expressionNode;
        relationNode->right = rightExprNode;
        expressionNode = relationNode;
    }

    return expressionNode;
}

ASTNode *parseAssignment(Table *table, Entry **currentEntry)
{
    Entry *entry = *currentEntry;

    if (entry->token->type != IDENTIFIER)
    {
        fprintf(stderr, ERR_EXPECTED_IDENTIFIER, entry->token->row, entry->token->column);
        exit(EXIT_FAILURE);
    }

    ASTNode *assignmentNode = createNode(OPERATOR, OPERATOR_ASSIGNMENT);
    ASTNode *idNode = createNode(entry->token->type, entry->token->word);
    assignmentNode->left = idNode;

    if (entry->next == NULL)
    {
        fprintf(stderr, ERR_EXPECTED_ASSIGNMENT_OPERATOR, entry->token->row, entry->token->column);
        freeNode(assignmentNode);
        exit(EXIT_FAILURE);
    }

    *currentEntry = entry->next;
    entry = *currentEntry;

    if (entry == NULL || strcmp(entry->token->word, OPERATOR_ASSIGNMENT) != 0)
    {
        fprintf(stderr, ERR_EXPECTED_ASSIGNMENT_OPERATOR, entry->token->row, entry->token->column);
        freeNode(assignmentNode);
        exit(EXIT_FAILURE);
    }

    if (entry->next == NULL)
    {
        fprintf(stderr, ERR_EXPECTED_EXPRESSION_AFTER_ASSIGNMENT, entry->token->row, entry->token->column);
        freeNode(assignmentNode);
        exit(EXIT_FAILURE);
    }

    *currentEntry = entry->next;
    entry = *currentEntry;

    if (entry == NULL)
    {
        fprintf(stderr, ERR_EXPECTED_EXPRESSION_AFTER_ASSIGNMENT, entry->token->row, entry->token->column);
        freeNode(assignmentNode);
        exit(EXIT_FAILURE);
    }

    ASTNode *exprNode = parseExpression(table, currentEntry);
    assignmentNode->right = exprNode;
    entry = *currentEntry;

    if (entry == NULL || strcmp(entry->token->word, SYMBOL_SEM) != 0)
    {
        fprintf(stderr, ERR_EXPECTED_SEMICOLON, entry->token->row, entry->token->column);
        freeNode(assignmentNode);
        exit(EXIT_FAILURE);
    }

    *currentEntry = entry->next;

    return assignmentNode;
}

ASTNode *parseStatement(Table *table, Entry **currentEntry)
{
    Entry *entry = *currentEntry;

    if (entry->token->type == IDENTIFIER)
    {
        return parseAssignment(table, currentEntry);
    }
    else if (entry->token->type == RESERVED_WORD && strcmp(entry->token->word, RESERVED_WORD_VAR) == 0)
    {
        return parseVarDeclaration(table, currentEntry);
    }
    else if (entry->token->type == RESERVED_WORD && strcmp(entry->token->word, RESERVED_WORD_BEGIN) == 0)
    {
        return parseCompoundStatement(table, currentEntry);
    }
    else if (entry->token->type == RESERVED_WORD && strcmp(entry->token->word, RESERVED_WORD_IF) == 0)
    {
        return parseConditional(table, currentEntry);
    }
    else if (entry->token->type == RESERVED_WORD && strcmp(entry->token->word, RESERVED_WORD_WHILE) == 0)
    {
        return parseRepetitive(table, currentEntry);
    }
    else
    {
        fprintf(stderr, ERR_INVALID_COMMAND, entry->token->word, entry->token->row, entry->token->column);
        exit(EXIT_FAILURE);
    }
}

ASTNode *parseCompoundStatement(Table *table, Entry **currentEntry)
{
    Entry *entry = *currentEntry;

    if (entry->token->type != RESERVED_WORD || strcmp(entry->token->word, RESERVED_WORD_BEGIN) != 0)
    {
        fprintf(stderr, ERR_EXPECTED_BEGIN, entry->token->row, entry->token->column);
        exit(EXIT_FAILURE);
    }

    ASTNode *compoundStmtNode = createNode(entry->token->type, entry->token->word);

    if (entry->next == NULL)
    {
        fprintf(stderr, ERR_EXPECTED_STATEMENT_AFTER_BEGIN, entry->token->row, entry->token->column);
        exit(EXIT_FAILURE);
    }

    *currentEntry = entry->next;
    entry = *currentEntry;

    ASTNode *stmtListNode = NULL;
    ASTNode *lastStmtNode = NULL;

    while (entry != NULL && strcmp(entry->token->word, RESERVED_WORD_END) != 0)
    {
        ASTNode *stmtNode = parseStatement(table, currentEntry);
        entry = *currentEntry;

        if (stmtListNode == NULL)
        {
            stmtListNode = stmtNode;
        }
        else
        {
            lastStmtNode->right = stmtNode;
        }
        lastStmtNode = stmtNode;

        if (entry->token->type == RESERVED_WORD && strcmp(entry->token->word, RESERVED_WORD_END) == 0)
        {
            break;
        }
        else
        {
            if (entry->next == NULL)
            {
                fprintf(stderr, ERR_EXPECTED_END, entry->token->row, entry->token->column);
                exit(EXIT_FAILURE);
            }
        }
    }

    compoundStmtNode->left = stmtListNode;
    *currentEntry = entry->next;

    return compoundStmtNode;
}

ASTNode *parseIdentifierList(Table *table, Entry **currentEntry)
{
    Entry *entry = *currentEntry;
    ASTNode *idListNode = createNode(entry->token->type, entry->token->word);

    while (entry && entry->token->type == IDENTIFIER)
    {
        ASTNode *idNode = createNode(entry->token->type, entry->token->word);
        idListNode->right = idNode;
        idListNode = idNode;

        if (entry->next == NULL)
        {
            fprintf(stderr, ERR_EXPECTED_COMMA_OR_COLON, entry->token->row, entry->token->column);
            freeNode(idListNode);
            exit(EXIT_FAILURE);
        }

        *currentEntry = entry->next;
        entry = *currentEntry;

        if (entry && strcmp(entry->token->word, SYMBOL_COM) == 0)
        {
            if (entry->next == NULL || entry->next->token->type != IDENTIFIER)
            {
                fprintf(stderr, ERR_EXPECTED_IDENTIFIER_AFTER_COMMA, entry->token->row, entry->token->column);
                freeNode(idListNode);
                exit(EXIT_FAILURE);
            }

            *currentEntry = entry->next;
            entry = *currentEntry;
        }
        else
        {
            break;
        }
    }

    return idListNode;
}

ASTNode *parseDeclaration(Table *table, Entry **currentEntry)
{
    Entry *entry = *currentEntry;
    ASTNode *declNode = createNode(entry->token->type, entry->token->word);

    ASTNode *idListNode = parseIdentifierList(table, currentEntry);
    declNode->left = idListNode;
    entry = *currentEntry;

    if (strcmp(entry->token->word, SYMBOL_COLON) != 0)
    {
        fprintf(stderr, ERR_EXPECTED_COLON_OR_COMMA, entry->token->row, entry->token->column);
        freeNode(declNode);
        exit(EXIT_FAILURE);
    }

    if (entry->next == NULL)
    {
        fprintf(stderr, ERR_EXPECTED_TYPE_AFTER_COLON, entry->token->row, entry->token->column);
        exit(EXIT_FAILURE);
    }

    *currentEntry = entry->next;
    entry = *currentEntry;

    if (entry->token->type != RESERVED_TYPE)
    {
        fprintf(stderr, ERR_EXPECTED_TYPE_AFTER_COLON, entry->token->row, entry->token->column);
        freeNode(declNode);
        exit(EXIT_FAILURE);
    }

    ASTNode *typeNode = createNode(entry->token->type, entry->token->word);
    declNode->right = typeNode;

    if (entry->next == NULL || strcmp(entry->next->token->word, SYMBOL_SEM) != 0)
    {
        fprintf(stderr, ERR_EXPECTED_SEMICOLON, entry->token->row, entry->token->column);
        freeNode(declNode);
        exit(EXIT_FAILURE);
    }

    *currentEntry = entry->next;

    return declNode;
}

ASTNode *parseVarDeclaration(Table *table, Entry **currentEntry)
{
    Entry *entry = *currentEntry;
    ASTNode *varDeclNode = createNode(entry->token->type, entry->token->word);

    while (entry && strcmp(entry->token->word, RESERVED_WORD_VAR) == 0)
    {
        if (entry->next == NULL)
        {
            fprintf(stderr, ERR_EXPECTED_IDENTIFIER_AFTER_VAR, entry->token->row, entry->token->column);
            exit(EXIT_FAILURE);
        }

        *currentEntry = entry->next;
        entry = *currentEntry;

        ASTNode *declNode = parseDeclaration(table, currentEntry);
        varDeclNode->right = declNode;
        entry = *currentEntry;

        if (entry && strcmp(entry->token->word, SYMBOL_SEM) == 0)
        {
            *currentEntry = entry->next;
            entry = *currentEntry;
        }
        else
        {
            break;
        }
    }

    return varDeclNode;
}

ASTNode *parseBlock(Table *table, Entry **currentEntry)
{
    Entry *entry = *currentEntry;
    ASTNode *blockNode = createNode(entry->token->type, entry->token->word);

    ASTNode *varDeclNode = parseVarDeclaration(table, currentEntry);
    blockNode->left = varDeclNode;

    ASTNode *compoundStmtNode = parseCompoundStatement(table, currentEntry);
    blockNode->right = compoundStmtNode;

    return blockNode;
}

ASTNode *parseProgram(Table *table, Entry **currentEntry)
{
    Entry *entry = *currentEntry;

    if (entry->token->type != RESERVED_WORD || strcmp(entry->token->word, RESERVED_WORD_PROGRAM) != 0)
    {
        fprintf(stderr, ERR_EXPECTED_PROGRAM, entry->token->row, entry->token->column);
        exit(EXIT_FAILURE);
    }

    ASTNode *programNode = createNode(entry->token->type, entry->token->word);

    if (entry->next == NULL)
    {
        fprintf(stderr, ERR_EXPECTED_IDENTIFIER_AFTER_PROGRAM, entry->token->row, entry->token->column);
        freeNode(programNode);
        exit(EXIT_FAILURE);
    }

    *currentEntry = entry->next;
    entry = *currentEntry;

    if (entry->token->type != IDENTIFIER)
    {
        fprintf(stderr, ERR_EXPECTED_IDENTIFIER_AFTER_PROGRAM, entry->token->row, entry->token->column);
        freeNode(programNode);
        exit(EXIT_FAILURE);
    }

    ASTNode *idNode = createNode(entry->token->type, entry->token->word);
    programNode->right = idNode;

    if (entry->next == NULL)
    {
        fprintf(stderr, ERR_EXPECTED_SEMICOLON, entry->token->row, entry->token->column);
        freeNode(programNode);
        exit(EXIT_FAILURE);
    }

    *currentEntry = entry->next;
    entry = *currentEntry;

    ASTNode *semicolonNode = createNode(entry->token->type, entry->token->word);
    idNode->right = semicolonNode;

    if (strcmp(entry->token->word, SYMBOL_SEM) != 0)
    {
        fprintf(stderr, ERR_EXPECTED_SEMICOLON, entry->token->row, entry->token->column);
        freeNode(programNode);
        exit(EXIT_FAILURE);
    }

    if (entry->next == NULL)
    {
        fprintf(stderr, ERR_EXPECTED_BLOCK_AFTER_PROGRAM_DECLARATION, entry->token->row, entry->token->column);
        freeNode(programNode);
        exit(EXIT_FAILURE);
    }

    *currentEntry = entry->next;
    entry = *currentEntry;

    ASTNode *blockNode = parseBlock(table, currentEntry);
    programNode->left = blockNode;
    entry = *currentEntry;

    if (strcmp(entry->token->word, SYMBOL_DOT) != 0)
    {
        fprintf(stderr, ERR_EXPECTED_DOT_AFTER_PROGRAM_BLOCK, entry->token->row, entry->token->column);
        freeNode(programNode);
        exit(EXIT_FAILURE);
    }

    *currentEntry = entry->next;

    return programNode;
}

ASTNode *parseTokens(Table *table)
{
    if (table->entryCount == 0)
    {
        fprintf(stderr, ERR_NO_TOKENS_TO_PARSE);
        exit(EXIT_FAILURE);
    }

    Entry *entry = table->entries[0];
    ASTNode *root = parseProgram(table, &entry);

    return root;
}