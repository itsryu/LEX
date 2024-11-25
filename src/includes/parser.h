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

/**
 * @brief Creates a new ASTNode with the given type and value.
 *
 * This function allocates memory for a new ASTNode, initializes its type and value,
 * and sets its left and right children to NULL. If memory allocation fails, the
 * function prints an error message and exits the program.
 *
 * @param type The type of the ASTNode.
 * @param value The value of the ASTNode.
 * @return A pointer to the newly created ASTNode.
 */
ASTNode *createNode(int type, char *value);

/**
 * @brief Frees the memory allocated for an ASTNode and its children.
 *
 * This function recursively frees the memory allocated for an ASTNode,
 * including its value, left child, and right child.
 *
 * @param node A pointer to the ASTNode to be freed. If the node is NULL,
 *             the function does nothing.
 */
void freeNode(ASTNode *node);

/**
 * Parses a conditional statement (if-then-else) from the given token table.
 *
 * @param table The symbol table used for parsing.
 * @param currentEntry A pointer to the current entry in the token table.
 * @return A pointer to the root ASTNode of the parsed conditional statement.
 *
 * The function expects the following structure in the token table:
 * - An 'if' reserved word followed by a condition expression.
 * - A 'then' reserved word followed by a statement.
 * - Optionally, an 'else' reserved word followed by another statement.
 *
 * If any of these expected tokens are missing or in the wrong order, the function
 * will print an error message to stderr and exit the program with a failure status.
 *
 * The returned ASTNode will have the following structure:
 * - The root node represents the 'if' statement.
 * - The left child of the root node represents the condition expression.
 * - The right child of the root node represents the 'then' statement.
 * - If an 'else' statement is present, the right child of the root node will be an
 *   intermediate node representing the 'else' branch, with its left child being the
 *   'then' statement and its right child being the 'else' statement.
 */
ASTNode *parseConditional(Table *table, Entry **currentEntry);

/**
 * Parses a repetitive structure (while-do loop) from the given table and current entry.
 *
 * @param table The symbol table used for parsing.
 * @param currentEntry A pointer to the current entry being parsed.
 * @return A pointer to the root ASTNode of the parsed while-do loop.
 *
 * The function expects the current entry to be a 'while' reserved word, followed by an expression,
 * then a 'do' reserved word, and finally a statement. If any of these expectations are not met,
 * the function will print an error message to stderr and exit the program.
 *
 * The resulting ASTNode will have the 'while' token as its root, the parsed expression as its left child,
 * and the parsed statement as its right child.
 */
ASTNode *parseRepetitive(Table *table, Entry **currentEntry);

/**
 * Parses a factor in the given table and updates the current entry.
 *
 * A factor can be a number, an identifier, or an expression enclosed in parentheses.
 * This function validates the factor and creates an AST node for it.
 *
 * @param table The symbol table used for parsing.
 * @param currentEntry A pointer to the current entry in the table. This will be updated to the next entry after parsing the factor.
 * @return A pointer to the created AST node representing the factor.
 *
 * @note This function will terminate the program with an error message if an invalid factor is encountered or if expected tokens are missing.
 */
ASTNode *parseFactor(Table *table, Entry **currentEntry);

/**
 * Parses a term in the input and constructs an abstract syntax tree (AST) node representing the term.
 * A term is defined as a factor possibly followed by a multiplication operator and another factor.
 *
 * @param table A pointer to the symbol table used for parsing.
 * @param currentEntry A double pointer to the current entry in the input token stream.
 *                     This pointer will be updated as tokens are consumed during parsing.
 * 
 * @return A pointer to the root AST node representing the parsed term.
 *         The returned node may represent a single factor or a multiplication operation with factors as operands.
 */
ASTNode *parseTerm(Table *table, Entry **currentEntry);

/**
 * Parses a simple expression from the given table and current entry.
 * A simple expression can be a term optionally preceded by a unary operator
 * (either '+' or '-') and followed by zero or more terms separated by binary
 * operators ('+' or '-') forming a left-associative expression.
 *
 * @param table The symbol table used for parsing.
 * @param currentEntry A pointer to the current entry in the token list.
 *                     This pointer will be updated to point to the next entry
 *                     after parsing the simple expression.
 * @return A pointer to the root of the abstract syntax tree (AST) representing
 *         the parsed simple expression.
 */
ASTNode *parseSimpleExpression(Table *table, Entry **currentEntry);

/**
 * @brief Parses an expression from the given table and current entry.
 *
 * This function parses an expression, which may include relational operators
 * such as '=', '<>', '<', '<=', '>', and '>='. It first parses a simple expression,
 * then checks if the next token is a relational operator. If it is, it creates a
 * relation node and parses the right-hand side expression. The resulting AST node
 * represents the entire expression.
 *
 * @param table The symbol table used for parsing.
 * @param currentEntry A pointer to the current entry in the token list.
 * @return A pointer to the root AST node representing the parsed expression.
 *
 * @note If an expected expression is missing after a relational operator, the function
 *       prints an error message and exits the program.
 */
ASTNode *parseExpression(Table *table, Entry **currentEntry);

/**
 * Parses an assignment statement from the given token table and current entry.
 *
 * @param table The symbol table containing the tokens.
 * @param currentEntry A pointer to the current entry in the token table.
 * @return A pointer to the root of the AST subtree representing the assignment statement.
 *
 * The function expects the following sequence of tokens:
 * - An identifier (variable name).
 * - An assignment operator ('=').
 * - An expression.
 * - A semicolon (';').
 *
 * If any of these expectations are not met, the function prints an error message to stderr
 * and exits the program with a failure status.
 *
 * The function creates and returns an ASTNode representing the assignment statement.
 * The left child of this node is the identifier, and the right child is the expression.
 *
 * Error messages:
 * - ERR_EXPECTED_IDENTIFIER: Expected an identifier at the beginning of the assignment.
 * - ERR_EXPECTED_ASSIGNMENT_OPERATOR: Expected an assignment operator after the identifier.
 * - ERR_EXPECTED_EXPRESSION_AFTER_ASSIGNMENT: Expected an expression after the assignment operator.
 * - ERR_EXPECTED_SEMICOLON: Expected a semicolon at the end of the assignment statement.
 */
ASTNode *parseAssignment(Table *table, Entry **currentEntry);

/**
 * @brief Parses a statement from the given table and current entry.
 *
 * This function determines the type of statement based on the token type
 * and delegates the parsing to the appropriate function.
 *
 * @param table Pointer to the symbol table.
 * @param currentEntry Pointer to the current entry in the table.
 * @return ASTNode* Pointer to the abstract syntax tree node representing the parsed statement.
 *
 * The function handles the following types of statements:
 * - Assignment statements (IDENTIFIER)
 * - Variable declarations (RESERVED_WORD_VAR)
 * - Compound statements (RESERVED_WORD_BEGIN)
 * - Conditional statements (RESERVED_WORD_IF)
 * - Repetitive statements (RESERVED_WORD_WHILE)
 *
 * If the token type does not match any of the expected types, the function
 * prints an error message and exits the program.
 */
ASTNode *parseStatement(Table *table, Entry **currentEntry);

/**
 * Parses a compound statement from the given table and current entry.
 *
 * A compound statement is expected to start with the RESERVED_WORD_BEGIN and end with the RESERVED_WORD_END.
 * This function will create an ASTNode representing the compound statement and link it to the list of statements
 * contained within the compound statement.
 *
 * @param table The symbol table used for parsing.
 * @param currentEntry A pointer to the current entry in the token list. This will be updated to point to the next entry after parsing.
 * @return A pointer to the ASTNode representing the compound statement.
 *
 * @note This function will terminate the program with an error message if the expected tokens are not found.
 */
ASTNode *parseCompoundStatement(Table *table, Entry **currentEntry);

/**
 * Parses a list of identifiers from the given table starting at the current entry.
 *
 * @param table The table containing the entries to parse.
 * @param currentEntry A pointer to the current entry in the table. This will be updated to the next entry after parsing.
 * @return A pointer to the root of the AST node representing the identifier list.
 *
 * The function creates an AST node for each identifier in the list and links them together.
 * If an error is encountered (e.g., missing comma or colon, or an identifier expected after a comma),
 * an error message is printed to stderr and the program exits with a failure status.
 */
ASTNode *parseIdentifierList(Table *table, Entry **currentEntry);

/**
 * @brief Parses a declaration from the given table and current entry.
 *
 * This function creates an AST node for a declaration, parses the identifier list,
 * and ensures the correct syntax for a declaration, including the presence of a colon,
 * a type, and a semicolon.
 *
 * @param table Pointer to the symbol table.
 * @param currentEntry Pointer to the current entry in the symbol table.
 * @return Pointer to the root AST node of the parsed declaration.
 *
 * @note This function will exit the program with an error message if the syntax is incorrect.
 */
ASTNode *parseDeclaration(Table *table, Entry **currentEntry);

/**
 * @brief Parses a variable declaration from the given table and entry.
 *
 * This function creates an AST node for a variable declaration and processes
 * subsequent declarations if they are present. It expects the current entry
 * to point to a token representing the 'var' keyword.
 *
 * @param table Pointer to the symbol table.
 * @param currentEntry Pointer to the current entry in the token list.
 * @return Pointer to the root AST node representing the variable declaration.
 *
 * The function will exit with an error message if an identifier is not found
 * after the 'var' keyword or if there is a syntax error in the declaration.
 */
ASTNode *parseVarDeclaration(Table *table, Entry **currentEntry);

/**
 * @brief Parses a block of code and constructs an abstract syntax tree (AST) node representing the block.
 *
 * This function parses a block of code, which typically consists of variable declarations followed by a compound statement.
 * It creates an AST node for the block and attaches the parsed variable declarations and compound statement as child nodes.
 *
 * @param table A pointer to the symbol table used for parsing.
 * @param currentEntry A double pointer to the current entry in the symbol table. This pointer is updated as the parsing progresses.
 * @return A pointer to the root AST node representing the parsed block.
 */
ASTNode *parseBlock(Table *table, Entry **currentEntry);

/**
 * Parses the program structure from the given token entries.
 *
 * This function expects the following structure:
 * - The first token should be the reserved word "program".
 * - The second token should be an identifier (the program name).
 * - The third token should be a semicolon.
 * - The subsequent tokens should form a block of statements.
 * - The final token should be a dot.
 *
 * If any of these expectations are not met, the function will print an error
 * message to stderr and exit the program with a failure status.
 *
 * @param table The symbol table used for parsing.
 * @param currentEntry A pointer to the current entry in the token list.
 *                     This pointer will be updated to point to the next entry
 *                     after parsing the program structure.
 * @return A pointer to the root ASTNode representing the parsed program.
 */
ASTNode *parseProgram(Table *table, Entry **currentEntry);

/**
 * @brief Parses the tokens from the given table and constructs an abstract syntax tree (AST).
 *
 * This function takes a table of tokens and parses them to construct an AST. If the table is empty,
 * it prints an error message and exits the program.
 *
 * @param table A pointer to the Table structure containing the tokens to be parsed.
 * @return A pointer to the root ASTNode of the constructed abstract syntax tree.
 */
ASTNode *parseTokens(Table *table);