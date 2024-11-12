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
typedef enum TokenType {
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
typedef struct Token {
    char* name;
    char* word;
    int row;
    int column;
    TokenType type;
    struct Token* next;
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
typedef struct Entry {
    char* key;
    Token* token;
    struct Entry* next;
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
typedef struct {
    Entry** entries;
} Table;

// Operadores:
#define OP_SUM '+'
#define OP_SUB '-'
#define OP_DIV '/'
#define OP_MUL '*'
#define OP_EQU '='
#define OP_LT '<'
#define OP_GT '>'

// Operadores reservados:
#define RESERVED_OP_NOT "not"
#define RESERVED_OP_AND "and"
#define RESERVED_OP_OR "or"
#define RESERVED_OP_MOD "mod"

// Simbolos
#define SMB_OBC '{'
#define SMB_CBC '}'
#define SMB_SEM ';'
#define SMB_OPA '('
#define SMB_CPA ')'
#define SMB_DOT '.'
#define SMB_COM ','
#define SMB_COLON ':'
#define SMB_SQT '\''
#define SMB_DQT '\"'
#define SMB_UNDER '_'

// Palavras reservadas:
#define RESERVED_WORD_PROGRAM "program"
#define RESERVED_WORD_VAR "var"
#define RESERVED_WORD_IF "if"
#define RESERVED_WORD_THEN "then"
#define RESERVED_WORD_ELSE "else"
#define RESERVED_WORD_BEGIN "begin"
#define RESERVED_WORD_END "end"
#define RESERVED_WORD_WHILE "while"
#define RESERVED_WORD_FOR "for"
#define RESERVED_WORD_TO "to"
#define RESERVED_WORD_DOWNTO "downto"
#define RESERVED_WORD_REPEAT "repeat"
#define RESERVED_WORD_UNTIL "until"
#define RESERVED_WORD_CASE "case"
#define RESERVED_WORD_OF "of"
#define RESERVED_WORD_FUNCTION "function"
#define RESERVED_WORD_PROCEDURE "procedure"
#define RESERVED_WORD_ARRAY "array"
#define RESERVED_WORD_RECORD "record"
#define RESERVED_WORD_CONST "const"
#define RESERVED_WORD_TYPE "type"
#define RESERVED_WORD_FILE "file"
#define RESERVED_WORD_SET "set"
#define RESERVED_WORD_GOTO "goto"
#define RESERVED_WORD_WITH "with"
#define RESERVED_WORD_DO "do"
#define RESERVED_WORD_IN "in"

// Tipos reservados:
#define RESERVED_TYPE_INTEGER "integer"
#define RESERVED_TYPE_REAL "real"
#define RESERVED_TYPE_BOOLEAN "boolean"
#define RESERVED_TYPE_CHAR "char"
#define RESERVED_TYPE_STRING "string"
#define RESERVED_TYPE_DOUBLE "double"
#define RESERVED_TYPE_BYTE "byte"
#define RESERVED_TYPE_WORD "word"
#define RESERVED_TYPE_LONGINT "longint"
#define RESERVED_TYPE_SHORTINT "shortint"
#define RESERVED_TYPE_SINGLE "single"
#define RESERVED_TYPE_EXTENDED "extended"
#define RESERVED_TYPE_COMP "comp"
#define RESERVED_TYPE_CURRENCY "currency"

// Especiais
#define TAB '\t'
#define SPACE ' '
#define NEW_LINE '\n'
#define END_OF_STRING '\0'

FILE *input, *output;

// Funcoes:
Token* lexerAnalysis(Table* table);
Table* initTable();
Token* searchTable(Table* symbolTable, char* key);