#pragma once

#include <stdio.h>

/**
 * @file lexer.h
 * @brief Defines the TokenType enumeration used for lexical analysis.
 *
 * This enumeration represents the different types of tokens that can be
 * identified during the lexical analysis phase of a compiler or interpreter.
 * Each token type corresponds to a specific category of lexical elements.
 *
 * TokenType:
 * - RESERVED_WORD: Keywords reserved by the language.
 * - RESERVED_TYPE: Reserved type names.
 * - RESERVED_OP: Reserved operators.
 * - IDENTIFIER: Identifiers such as variable names.
 * - OP: Operators.
 * - SMB: Symbols.
 * - NUM: Numeric literals.
 * - STRING: String literals.
 * - END_OF_FILE: End of the input file.
 * - ERROR: Represents an error in tokenization.
 */
typedef enum TokenType
{
    RESERVED_WORD,
    RESERVED_TYPE,
    RESERVED_OPERATOR,
    IDENTIFIER,
    OPERATOR,
    SYMBOL,
    NUMBER,
    STRING,
    END_OF_FILE,
    ERROR
} TokenType;

/**
 * @struct Token
 * @brief Represents a lexical token.
 *
 * This structure is used to store information about a token identified
 * during lexical analysis.
 *
 * @var Token::name
 * The name of the token.
 *
 * @var Token::word
 * The actual word or lexeme that the token represents.
 *
 * @var Token::row
 * The row number where the token was found in the source code.
 *
 * @var Token::column
 * The column number where the token starts in the source code.
 *
 * @var Token::type
 * The type of the token, represented by the TokenType enumeration.
 */
typedef struct Token
{
    char *name;
    char *word;
    int row;
    int column;
    TokenType type;
    struct Token *next;
} Token;

/**
 * @struct Entry
 * @brief Represents an entry in a hash table or linked list.
 *
 * This structure is used to store a key-value pair where the key is a string
 * and the value is a pointer to a Token. Each entry also contains a pointer
 * to the next entry, allowing for the creation of a linked list.
 *
 * @var Entry::key
 * The key associated with the entry, represented as a string.
 *
 * @var Entry::token
 * A pointer to the Token associated with the key.
 *
 * @var Entry::next
 * A pointer to the next entry in the linked list.
 */
typedef struct Entry
{
    char *key;
    Token *token;
    struct Entry *next;
} Entry;

/**
 * @struct Table
 * @brief Represents a table containing an array of entries.
 *
 * This structure is used to store a collection of entries, where each entry
 * is a pointer to an Entry object. The entries are stored in a dynamically
 * allocated array.
 *
 * @var Table::entries
 * Pointer to an array of Entry pointers. Each element in the array points
 * to an Entry object.
 */
typedef struct
{
    Entry **entries;
    int entryCount;
} Table;

FILE *input, *output;

Token *lexerAnalysis(Table *table);
Table *initTable();
Token *searchTable(Table *symbolTable, char *key);