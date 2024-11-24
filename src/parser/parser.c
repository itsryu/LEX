#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "../includes/lexer.h"
#include "../includes/parser.h"

ASTNode *createNode(int type, const char *value);
void freeNode(ASTNode *node);
int isValidNumber(const char *str);
ASTNode *parseRepetitive(Table *table, Entry **currentEntry);
ASTNode *parseConditional(Table *table, Entry **currentEntry);
ASTNode *parseProgram(Table *table, Entry **currentEntry);
ASTNode *parseBlock(Table *table, Entry **currentEntry);
ASTNode *parseVarDeclaration(Table *table, Entry **currentEntry);
ASTNode *parseDeclaration(Table *table, Entry **currentEntry);
ASTNode *parseIdentifierList(Table *table, Entry **currentEntry);
ASTNode *parseCompoundStatement(Table *table, Entry **currentEntry);
ASTNode *parseStatement(Table *table, Entry **currentEntry);
ASTNode *parseAssignment(Table *table, Entry **currentEntry);
ASTNode *parseExpression(Table *table, Entry **currentEntry);
ASTNode *parseSimpleExpression(Table *table, Entry **currentEntry);
ASTNode *parseTerm(Table *table, Entry **currentEntry);
ASTNode *parseFactor(Table *table, Entry **currentEntry);
ASTNode *parseTokens(Table *table);

ASTNode *createNode(int type, const char *value)
{
    ASTNode *node = (ASTNode *)malloc(sizeof(ASTNode));
    if (!node)
    {
        fprintf(stderr, "Memory allocation failed\n");
        exit(EXIT_FAILURE);
    }

    node->type = type;
    node->value = value ? strdup(value) : NULL;
    node->left = NULL;
    node->right = NULL;

    return node;
}

void freeNode(ASTNode *node)
{
    if (node)
    {
        if (node->value)
            free(node->value);
        freeNode(node->left);
        freeNode(node->right);
        free(node);
    }
}

int isValidNumber(const char *str)
{
    if (*str == '\0')
        return 0;
    while (*str)
    {
        if (!isdigit(*str) && *str != '.')
            return 0;
        str++;
    }
    return 1;
}

ASTNode *parseProgram(Table *table, Entry **currentEntry)
{
    Entry *entry = *currentEntry;

    if (entry->token->type != RESERVED_WORD || strcmp(entry->token->word, RESERVED_WORD_PROGRAM) != 0)
    {
        fprintf(stderr, "Syntax error: expected 'program' at %d:%d\n", entry->token->row, entry->token->column);
        exit(EXIT_FAILURE);
    }

    ASTNode *programNode = createNode(entry->token->type, entry->token->word);

    if (entry->next == NULL)
    {
        fprintf(stderr, "Syntax error: expected identifier after 'program' at %d:%d\n", entry->token->row, entry->token->column);
        freeNode(programNode);
        exit(EXIT_FAILURE);
    }

    *currentEntry = entry->next;
    entry = *currentEntry;

    if (entry->token->type != IDENTIFIER)
    {
        fprintf(stderr, "Syntax error: expected identifier after 'program' at %d:%d\n", entry->token->row, entry->token->column);
        freeNode(programNode);
        exit(EXIT_FAILURE);
    }

    ASTNode *idNode = createNode(entry->token->type, entry->token->word);
    programNode->right = idNode;

    if (entry->next == NULL)
    {
        fprintf(stderr, "Syntax error: expected ';' after program identifier at %d:%d\n", entry->token->row, entry->token->column);
        freeNode(programNode);
        exit(EXIT_FAILURE);
    }

    *currentEntry = entry->next;
    entry = *currentEntry;

    if (strcmp(entry->token->word, SYMBOL_SEM) != 0)
    {
        fprintf(stderr, "Syntax error: expected ';' after program identifier at %d:%d\n", entry->token->row, entry->token->column);
        freeNode(programNode);
        exit(EXIT_FAILURE);
    }

    if (entry->next == NULL)
    {
        fprintf(stderr, "Syntax error: expected block after program declaration at %d:%d\n", entry->token->row, entry->token->column);
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
        fprintf(stderr, "Syntax error: expected '.' after program block at %d:%d\n", entry->token->row, entry->token->column);
        freeNode(programNode);
        exit(EXIT_FAILURE);
    }

    *currentEntry = entry->next;

    return programNode;
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

ASTNode *parseVarDeclaration(Table *table, Entry **currentEntry)
{
    Entry *entry = *currentEntry;
    ASTNode *varDeclNode = createNode(entry->token->type, entry->token->word);

    while (entry && strcmp(entry->token->word, RESERVED_WORD_VAR) == 0)
    {
        if (entry->next == NULL)
        {
            fprintf(stderr, "Syntax error: expected identifier after 'var' at %d:%d\n", entry->token->row, entry->token->column);
            exit(EXIT_FAILURE);
        }

        *currentEntry = entry->next;
        entry = *currentEntry;

        ASTNode *declNode = parseDeclaration(table, currentEntry);
        varDeclNode->right = declNode;
        entry = *currentEntry;

        if (entry && strcmp(entry->token->word, ";") == 0)
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

ASTNode *parseDeclaration(Table *table, Entry **currentEntry)
{
    Entry *entry = *currentEntry;
    ASTNode *declNode = createNode(entry->token->type, entry->token->word);

    ASTNode *idListNode = parseIdentifierList(table, currentEntry);
    declNode->left = idListNode;
    entry = *currentEntry;

    if (strcmp(entry->token->word, ":") != 0)
    {
        fprintf(stderr, "Syntax error: expected ':' or ',' at %d:%d\n", entry->token->row, entry->token->column);
        freeNode(declNode);
        exit(EXIT_FAILURE);
    }

    if (entry->next == NULL)
    {
        fprintf(stderr, "Syntax error: expected type after ':' at %d:%d\n", entry->token->row, entry->token->column);
        exit(EXIT_FAILURE);
    }

    *currentEntry = entry->next;
    entry = *currentEntry;

    if (entry->token->type != RESERVED_TYPE)
    {
        fprintf(stderr, "Syntax error: expected type after ':' at %d:%d\n", entry->token->row, entry->token->column);
        freeNode(declNode);
        exit(EXIT_FAILURE);
    }

    ASTNode *typeNode = createNode(entry->token->type, entry->token->word);
    declNode->right = typeNode;

    if (entry->next == NULL || strcmp(entry->next->token->word, ";") != 0)
    {
        fprintf(stderr, "Syntax error: expected ';' at %d:%d\n", entry->token->row, entry->token->column);
        freeNode(declNode);
        exit(EXIT_FAILURE);
    }

    *currentEntry = entry->next;

    return declNode;
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
            fprintf(stderr, "Syntax error: expected ',' or ':' at %d:%d\n", entry->token->row, entry->token->column);
            freeNode(idListNode);
            exit(EXIT_FAILURE);
        }

        *currentEntry = entry->next;
        entry = *currentEntry;

        if (entry && strcmp(entry->token->word, ",") == 0)
        {
            if (entry->next == NULL || entry->next->token->type != IDENTIFIER)
            {
                fprintf(stderr, "Syntax error: expected identifier after ',' at %d:%d\n", entry->token->row, entry->token->column);
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

ASTNode *parseCompoundStatement(Table *table, Entry **currentEntry)
{
    Entry *entry = *currentEntry;

    if (entry->token->type != RESERVED_WORD || strcmp(entry->token->word, RESERVED_WORD_BEGIN) != 0)
    {
        fprintf(stderr, "Syntax error: expected 'begin' at %d:%d\n", entry->token->row, entry->token->column);
        exit(EXIT_FAILURE);
    }

    ASTNode *compoundStmtNode = createNode(entry->token->type, entry->token->word);

    if (entry->next == NULL)
    {
        fprintf(stderr, "Syntax error: expected statement after 'begin' at %d:%d\n", entry->token->row, entry->token->column);
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
                fprintf(stderr, "Syntax error: expected 'end' at %d:%d\n", entry->token->row, entry->token->column);
                exit(EXIT_FAILURE);
            }
        }
    }

    compoundStmtNode->left = stmtListNode;
    *currentEntry = entry->next;

    return compoundStmtNode;
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
        fprintf(stderr, "Syntax error: invalid command '%s' at %d:%d\n", entry->token->word, entry->token->row, entry->token->column);
        exit(EXIT_FAILURE);
    }
}

ASTNode *parseConditional(Table *table, Entry **currentEntry)
{
    Entry *entry = *currentEntry;

    if (entry->token->type != RESERVED_WORD || strcmp(entry->token->word, RESERVED_WORD_IF) != 0)
    {
        fprintf(stderr, "Syntax error: expected 'if' at %d:%d\n", entry->token->row, entry->token->column);
        exit(EXIT_FAILURE);
    }

    ASTNode *ifNode = createNode(entry->token->type, entry->token->word);

    if (entry->next == NULL)
    {
        fprintf(stderr, "Syntax error: expected expression after 'if' at %d:%d\n", entry->token->row, entry->token->column);
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
        fprintf(stderr, "Syntax error: expected 'then' at %d:%d\n", entry->token->row, entry->token->column);
        freeNode(ifNode);
        exit(EXIT_FAILURE);
    }

    if (entry->next == NULL)
    {
        fprintf(stderr, "Syntax error: expected statement after 'then' at %d:%d\n", entry->token->row, entry->token->column);
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
            fprintf(stderr, "Syntax error: expected statement after 'else' at %d:%d\n", entry->token->row, entry->token->column);
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

    if (entry->token->type != RESERVED_WORD || strcmp(entry->token->word, "while") != 0)
    {
        fprintf(stderr, "Syntax error: expected 'while' at %d:%d\n", entry->token->row, entry->token->column);
        exit(EXIT_FAILURE);
    }

    ASTNode *whileNode = createNode(entry->token->type, entry->token->word);

    if (entry->next == NULL)
    {
        fprintf(stderr, "Syntax error: expected expression after 'while' at %d:%d\n", entry->token->row, entry->token->column);
        freeNode(whileNode);
        exit(EXIT_FAILURE);
    }

    *currentEntry = entry->next;
    entry = *currentEntry;

    ASTNode *conditionNode = parseExpression(table, currentEntry);
    whileNode->left = conditionNode;
    entry = *currentEntry;

    if (entry->token->type != RESERVED_WORD || strcmp(entry->token->word, "do") != 0)
    {
        fprintf(stderr, "Syntax error: expected 'do' at %d:%d\n", entry->token->row, entry->token->column);
        freeNode(whileNode);
        exit(EXIT_FAILURE);
    }

    if (entry->next == NULL)
    {
        fprintf(stderr, "Syntax error: expected statement after 'do' at %d:%d\n", entry->token->row, entry->token->column);
        freeNode(whileNode);
        exit(EXIT_FAILURE);
    }

    *currentEntry = entry->next;
    entry = *currentEntry;

    ASTNode *doNode = parseStatement(table, currentEntry);
    whileNode->right = doNode;

    return whileNode;
}

ASTNode *parseAssignment(Table *table, Entry **currentEntry)
{
    Entry *entry = *currentEntry;

    if (entry->token->type != IDENTIFIER)
    {
        fprintf(stderr, "Syntax error: expected identifier at %d:%d\n", entry->token->row, entry->token->column);
        exit(EXIT_FAILURE);
    }

    ASTNode *assignmentNode = createNode(OPERATOR, ":=");
    ASTNode *idNode = createNode(entry->token->type, entry->token->word);
    assignmentNode->left = idNode;

    if (entry->next == NULL)
    {
        fprintf(stderr, "Syntax error: expected ':=' after identifier at %d:%d\n", entry->token->row, entry->token->column);
        freeNode(assignmentNode);
        exit(EXIT_FAILURE);
    }

    *currentEntry = entry->next;
    entry = *currentEntry;

    if (entry == NULL || strcmp(entry->token->word, ":=") != 0)
    {
        fprintf(stderr, "Syntax error: expected ':=' at %d:%d\n", entry->token->row, entry->token->column);
        freeNode(assignmentNode);
        exit(EXIT_FAILURE);
    }

    if (entry->next == NULL)
    {
        fprintf(stderr, "Syntax error: expected expression after ':=' at %d:%d\n", entry->token->row, entry->token->column);
        freeNode(assignmentNode);
        exit(EXIT_FAILURE);
    }

    *currentEntry = entry->next;
    entry = *currentEntry;

    if (entry == NULL)
    {
        fprintf(stderr, "Syntax error: expected expression after ':=' at %d:%d\n", entry->token->row, entry->token->column);
        freeNode(assignmentNode);
        exit(EXIT_FAILURE);
    }

    ASTNode *exprNode = parseExpression(table, currentEntry);
    assignmentNode->right = exprNode;
    entry = *currentEntry;

    if (entry == NULL || strcmp(entry->token->word, ";") != 0)
    {
        fprintf(stderr, "Syntax error: expected ';' at %d:%d\n", entry->token->row, entry->token->column);
        freeNode(assignmentNode);
        exit(EXIT_FAILURE);
    }

    *currentEntry = entry->next;

    return assignmentNode;
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
            fprintf(stderr, "Syntax error: expected expression after '%s' at %d:%d\n", entry->token->word, entry->token->row, entry->token->column);
            freeNode(expressionNode);
            freeNode(relationNode);
            exit(EXIT_FAILURE);
        }

        *currentEntry = entry->next;
        entry = *currentEntry;

        if (entry == NULL)
        {
            fprintf(stderr, "Syntax error: expected expression after '%s' at %d:%d\n", entry->token->word, entry->token->row, entry->token->column);
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

ASTNode *parseSimpleExpression(Table *table, Entry **currentEntry)
{
    Entry *entry = *currentEntry;
    ASTNode *simpleExprNode = NULL;

    if (entry->token->type == OPERATOR && (strcmp(entry->token->word, OPERATOR_SUM) == 0 || strcmp(entry->token->word, OPERATOR_SUB) == 0))
    {
        simpleExprNode = createNode(entry->token->type, entry->token->word);

        if (entry->next == NULL)
        {
            fprintf(stderr, "Syntax error: expected term after '%s' at %d:%d\n", entry->token->word, entry->token->row, entry->token->column);
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
            fprintf(stderr, "Syntax error: expected term after '%s' at %d:%d\n", entry->token->word, entry->token->row, entry->token->column);
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

ASTNode *parseTerm(Table *table, Entry **currentEntry)
{
    ASTNode *termNode = parseFactor(table, currentEntry);
    Entry *entry = *currentEntry;

    while (entry && (strcmp(entry->token->word, OPERATOR_MUL) == 0 || strcmp(entry->token->word, OPERATOR_MUL) == 0))
    {
        ASTNode *operatorNode = createNode(entry->token->type, entry->token->word);

        if (entry->next == NULL)
        {
            fprintf(stderr, "Syntax error: expected factor after '%s' at %d:%d\n", entry->token->word, entry->token->row, entry->token->column);
            freeNode(termNode);
            freeNode(operatorNode);
            exit(EXIT_FAILURE);
        }

        *currentEntry = entry->next;

        if (*currentEntry == NULL)
        {
            fprintf(stderr, "Syntax error: expected factor after '%s' at %d:%d\n", entry->token->word, entry->token->row, entry->token->column);
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

ASTNode *parseFactor(Table *table, Entry **currentEntry)
{
    Entry *entry = *currentEntry;
    ASTNode *factorNode = NULL;

    if (entry->token->type == NUMBER)
    {
        if (!isValidNumber(entry->token->word))
        {
            fprintf(stderr, "Syntax error: invalid number '%s' at %d:%d\n", entry->token->word, entry->token->row, entry->token->column);
            exit(EXIT_FAILURE);
        }
        factorNode = createNode(entry->token->type, entry->token->word);

        if (entry->next == NULL)
        {
            fprintf(stderr, "Syntax error: expected expression or ';' at %d:%d\n", entry->token->row, entry->token->column);
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
            fprintf(stderr, "Syntax error: expected expression after '(' at %d:%d\n", entry->token->row, entry->token->column);
            exit(EXIT_FAILURE);
        }

        *currentEntry = entry->next;
        factorNode = parseExpression(table, currentEntry);
        entry = *currentEntry;

        if (entry && strcmp(entry->token->word, SYMBOL_CPA) == 0)
        {
            if (entry->next == NULL)
            {
                fprintf(stderr, "Syntax error: expected expression or ';' at %d:%d\n", entry->token->row, entry->token->column);
                freeNode(factorNode);
                exit(EXIT_FAILURE);
            }

            *currentEntry = entry->next;
            entry = *currentEntry;
        }
        else
        {
            fprintf(stderr, "Syntax error: expected ')' at %d:%d\n", entry->token->row, entry->token->column);
            freeNode(factorNode);
            exit(EXIT_FAILURE);
        }
    }
    else
    {
        fprintf(stderr, "Syntax error: invalid factor '%s' at %d:%d\n", entry->token->word, entry->token->row, entry->token->column);
        exit(EXIT_FAILURE);
    }

    return factorNode;
}

ASTNode *parseTokens(Table *table)
{
    if (table->entryCount == 0)
    {
        fprintf(stderr, "No tokens to parse\n");
        exit(EXIT_FAILURE);
    }

    Entry *entry = table->entries[0];
    ASTNode *root = parseProgram(table, &entry);

    return root;
}