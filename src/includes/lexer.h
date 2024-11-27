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
    struct Entry *prev;
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

/**
 * @brief Creates a new token with the specified attributes.
 *
 * @param type The type of the token.
 * @param name The name of the token.
 * @param word The word associated with the token.
 * @param row The row number where the token is found.
 * @param column The column number where the token is found.
 * @return Token* A pointer to the newly created token.
 */
static Token *createToken(TokenType type, char *name, char *word, int row, int column);

/**
 * @brief Searches for a token in the hash table using the given key.
 *
 * This function searches for a token in the provided hash table by computing
 * the hash index of the key and then traversing the linked list at that index
 * to find the matching key. If a matching key is found, the corresponding token
 * is returned. If no matching key is found, the function returns NULL.
 *
 * @param table Pointer to the hash table to search.
 * @param key The key to search for in the hash table.
 * @return Pointer to the token associated with the key, or NULL if the key is not found.
 */
Token *searchTable(Table *table, char *key);

/**
 * @brief Inserts a token into the hash table with the given key.
 *
 * This function creates a new entry with the specified key and token,
 * computes the hash index for the key, and inserts the entry into the
 * hash table. If there is a collision (i.e., another entry already exists
 * at the computed index), the new entry is added to the end of the linked
 * list at that index.
 *
 * @param table Pointer to the hash table where the entry will be inserted.
 * @param key The key associated with the token to be inserted.
 * @param token Pointer to the token to be inserted into the table.
 */
static void insertTable(Table *table, char *key, Token *token);

/**
 * @brief Computes a hash value for a given key.
 *
 * This function takes a string key and computes its hash value using a simple
 * hash function. The hash value is then modded by the table size to ensure it
 * fits within the bounds of the hash table.
 *
 * @param key The string key to be hashed.
 * @param tableSize The size of the hash table.
 * @return The computed hash value modded by the table size.
 */
static unsigned int hash(char *key, int tableSize);

/**
 * @brief Initializes a new Table structure.
 *
 * This function allocates memory for a new Table structure and its entries.
 * It sets all entries to NULL and initializes the entry count to 0.
 *
 * @return A pointer to the newly initialized Table structure.
 */
Table *initTable();

/**
 * @brief Performs lexical analysis on the input and generates tokens.
 *
 * This function reads characters from the input and identifies different types of tokens such as
 * spaces, numeric values, alphanumeric values, symbols, operators, reserved words, identifiers,
 * integer values, real values, relational operators, assignment operators, and strings. It also
 * handles lexical errors and end-of-file conditions.
 *
 * @param table A pointer to the symbol table where tokens will be inserted.
 * @return A pointer to the generated token.
 */
Token *lexerAnalysis(Table *table);

/**
 * @brief Adds a character to a dynamically allocated word array.
 *
 * This function appends a given character to the end of a dynamically
 * allocated word array, resizing the array if necessary.
 *
 * @param word A pointer to the dynamically allocated word array.
 * @param size A pointer to the current size of the word array.
 * @param ch The character to be added to the word array.
 */
static void addWord(char **word, int *size, const char ch);

/**
 * @brief Checks if a given word is a reserved word.
 *
 * This function compares the input word against a list of reserved words
 * and returns 1 if the word is found in the list, otherwise returns 0.
 *
 * @param word The word to be checked.
 * @return int Returns 1 if the word is a reserved word, otherwise returns 0.
 */
static int isReservedWord(const char *word);

/**
 * @brief Checks if a given word is a reserved type.
 *
 * This function takes a string as input and checks if it matches any of the predefined reserved type words.
 *
 * @param word The word to check. If NULL, the function returns 0.
 * @return Returns 1 if the word is a reserved type, otherwise returns 0.
 */
static int isReservedType(const char *word);

/**
 * @brief Checks if a given word is a reserved operator.
 *
 * This function compares the input word against a list of reserved operator words.
 * If the word matches any of the reserved operators, the function returns 1.
 * Otherwise, it returns 0.
 *
 * @param word The word to check against the list of reserved operators.
 *             If the word is NULL, the function returns 0.
 *
 * @return 1 if the word is a reserved operator, 0 otherwise.
 */
static int isReservedOperator(const char *word);

/**
 * @brief Checks if a given word is a valid identifier.
 *
 * An identifier is considered valid if it starts with an underscore ('_') or a letter (a-z, A-Z),
 * and is followed by any combination of underscores, letters, or digits (0-9).
 *
 * @param word The word to be checked.
 * @return int Returns 1 if the word is a valid identifier, 0 otherwise.
 */
static int isValidIdentifier(const char *word);