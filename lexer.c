#include "lexer.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int column = 0, row = 1;
static char ch;
static void addWord(char** word, int* i, const char ch);
static Token* createToken(char* name, char* word, int row, int column);
static void showError(const char* message, const int row, const int column);
static int isReservedWord(const char* word);
static int isReservedType(const char* word);
static int isReservedOperator(const char* word);

Token* lexerAnalysis() {
	char* word = (char*) malloc(sizeof(char));
	int state = 0, i = 0;

	while((ch = fgetc(input)) != EOF) {
		column++;

		switch(state) {
			// q0:
			// Estado inicial, identificando espacos em branco, valores numericos,
			// valores alfanumericos e valores alfabaticos:
			case 0:
				{
					// Identificando espacos em branco:
					if(ch == SPACE || ch == TAB || ch == NEW_LINE) {
						if(ch == NEW_LINE) {
							row++;
							column = 0;
						}

						break;
					}

					// Identificando valores numericos:
					if(ch >= '0' && ch <= '9') {
						addWord(&word, &i, ch);
						state = 2;

						break;
					}


					// Identificando valores alfanumericos:
					if(ch >= 'a' && ch <= 'z' || ch >= 'A' && ch <= 'Z' || ch >= '0' && ch <= '9') {
						addWord(&word, &i, ch);
						state = 1;

						break;
					}

					// Identificando string não fechadas:
					if(ch == SMB_SQT) {
						while((ch = fgetc(input)) != SMB_SQT) {
							if(ch == EOF) {
								showError("String not closed", row, column);
								break;
							}
						}

						break;
					}

					// Identificando operadores:
					if(ch == OP_SUM || ch == OP_SUB || ch == OP_DIV || ch == OP_MUL || ch == OP_LT || ch == OP_GT) {
						addWord(&word, &i, ch);

						if(ch == OP_SUM || ch == OP_SUB || ch == OP_DIV || ch == OP_MUL) {
							Token* token = createToken("Binary Arithmetic Operator", word, row, column);
							return token;
						} else {
							state = 4; // Identificando operadores de '<=' e '>=':
						}

						break;
					}

					char message[50] = "Unknown character: '";
					message[20] = ch;
					strcat(message, "'");

					showError(message, row, column);

					break;
				}

			// q1:
			// Lidando com valores alfanumericos e indentificando palavras reservadas
			// e identificadores:
			case 1:
				{
					if(ch >= 'a' && ch <= 'z' || ch >= 'A' && ch <= 'Z' || ch >= '0' && ch <= '9') {
						addWord(&word, &i, ch);
					} else {
						ungetc(ch, input);
						column--;

						if(isReservedWord(word)) {
							Token* token = createToken("Reserved-word", word, row, column);
							return token;
						} else if(isReservedType(word)) {
							Token* token = createToken("Reserved-type", word, row, column);
							return token;
						} else if(isReservedOperator(word)) {
							Token* token = createToken("Reserved-operator", word, row, column);
							return token;
						} else {
							Token* token = createToken("Identifier", word, row, column);
							return token;
						}
					}

					break;
				}

			// q2:
			// Lidando com nemeros inteiros e identificando possiveis numeros reais:
			case 2:
				{
					if(ch >= '0' && ch <= '9') {
						addWord(&word, &i, ch);
					} else if(ch == SMB_DOT) {
						addWord(&word, &i, ch);
						state = 3;
					} else {
						ungetc(ch, input);
						column--;

						Token* token = createToken("Integer value", word, row, column);
						return token;
					}

					break;
				}

			// q3:
			// Lidando com numeros reais:
			case 3:
				{
					if(ch >= '0' && ch <= '9') {
						addWord(&word, &i, ch);
					} else {
						ungetc(ch, input);
						column--;

						Token* token = createToken("Real value", word, row, column);
						return token;
					}

					break;
				}

			// q6:
			// Lidando com operadores de menor ou igual e maior ou igual:
			case 4:
				{
					if(ch == OP_EQU) {
						addWord(&word, &i, ch);
					} else {
						ungetc(ch, input);
						column--;
					}

					Token* token = createToken("Relational Operator", word, row, column);
					return token;

					break;
				}

			default:
				{
					showError("Unknown state", row, column);
					break;
				}
		}
	}

	free(word);
	Token* token = createToken("EOF", "EOF", row, column);

	return token;
}

static void showError(const char* message, const int row, const int column) {
	printf("LexicalError: %s at %d:%d\n", message, row, column);
	fprintf(output, "LexicalError: %s at %d:%d\n", message, row, column);
}

static void addWord(char** word, int* i, const char ch) {
	*word = (char*) realloc(*word, sizeof(char) * (*i + 2));
	(*word)[(*i)++] = ch;
	(*word)[*i] = '\0';
}

static int isReservedWord(const char* word) {
	if(word == NULL) return 0;

	static const char* reserverdWords[] = {
		RESERVED_WORD_PROGRAM, RESERVED_WORD_VAR,  RESERVED_WORD_BEGIN,
		RESERVED_WORD_END,     RESERVED_WORD_IF,   RESERVED_WORD_ELSE,
		RESERVED_WORD_THEN,    RESERVED_WORD_DO,   RESERVED_WORD_WHILE,
		RESERVED_WORD_FOR,     RESERVED_WORD_TO,   RESERVED_WORD_DOWNTO,
		RESERVED_WORD_REPEAT,  RESERVED_WORD_UNTIL, RESERVED_WORD_CASE,
		RESERVED_WORD_OF,      RESERVED_WORD_FUNCTION, RESERVED_WORD_PROCEDURE,
		RESERVED_WORD_ARRAY,   RESERVED_WORD_RECORD, RESERVED_WORD_CONST,
		RESERVED_WORD_TYPE,    RESERVED_WORD_FILE, RESERVED_WORD_SET,
		RESERVED_WORD_GOTO,    RESERVED_WORD_WITH, RESERVED_WORD_IN
	};

	static const int reservedWordsSize = sizeof(reserverdWords) / sizeof(char*);

	for(int i = 0; i < reservedWordsSize; i++) {
		if(strcmp(word, reserverdWords[i]) == 0) return 1;
	}

	return 0;
}

static int isReservedType(const char* word) {
	if(word == NULL) return 0;

    static const char* reserverdWords[] = {
        RESERVED_TYPE_INTEGER,  RESERVED_TYPE_REAL,
        RESERVED_TYPE_CHAR,     RESERVED_TYPE_STRING,
        RESERVED_TYPE_DOUBLE,   RESERVED_TYPE_BYTE,
        RESERVED_TYPE_WORD,     RESERVED_TYPE_LONGINT,
        RESERVED_TYPE_SHORTINT, RESERVED_TYPE_SINGLE,
        RESERVED_TYPE_EXTENDED, RESERVED_TYPE_COMP,
        RESERVED_TYPE_CURRENCY
	};

    static const int reservedWordsSize = sizeof(reserverdWords) / sizeof(char*);

	for(int i = 0; i < reservedWordsSize; i++) {
		if(strcmp(word, reserverdWords[i]) == 0) return 1;
	}

	return 0;
}

static int isReservedOperator(const char* word) {
	if(word == NULL) return 0;

	static const char* reserverdWords[] = {
		RESERVED_OP_NOT, RESERVED_OP_AND, RESERVED_OP_OR, RESERVED_OP_MOD
	};

	static const int reservedWordsSize = sizeof(reserverdWords) / sizeof(char*);

	for(int i = 0; i < reservedWordsSize; i++) {
		if(strcmp(word, reserverdWords[i]) == 0) return 1;
	}

	return 0;
}

static Token* createToken(char* name, char* word, int row, int column) {
	Token* token = (Token*) malloc(sizeof(Token));

	token->name = name;
	token->word = word;
	token->row = row;
	token->column = column;

	return token;
}