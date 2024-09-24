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
Token* lexerAnalysis();