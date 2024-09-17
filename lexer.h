#pragma once

#include <stdio.h>

typedef struct {
    char* name;
    char* word;
    int row;
    int column;
} Token;

// Operadores:
#define OP_SUM '+'
#define OP_SUB '-'
#define OP_DIV '/'
#define OP_MUL '*'
#define OP_EQU '='

// Símbolos
#define SMB_OBC '{'
#define SMB_COM ','
#define SMB_CBC '}'
#define SMB_SEM ';'
#define SMB_OPA '('
#define SMB_CPA ')'

// Palavras-reservadas:
#define RESERVED_WORD_PROGRAM "program"
#define RESERVED_WORD_VAR "var"
#define RESERVED_WORD_IF "if"
#define RESERVED_WORD_THEN "then"
#define RESERVED_WORD_ELSE "else"
#define RESERVED_WORD_DO "do"
#define RESERVED_WORD_BEGIN "begin"
#define RESERVED_WORD_END "end"
#define RESERVED_WORD_WHILE "while"

// Tipos:
#define RESERVED_TYPE_INTEGER "integer"
#define RESERVED_TYPE_REAL "real"

// Especiais
#define TAB '\t'
#define SPACE ' '
#define NEW_LINE '\n'

FILE *input, *output;

// Funções:
Token* lexerAnalysis();