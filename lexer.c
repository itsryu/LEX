#include "lexer.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int column = 0, row = 1;
static char ch;
static void addWord(char** word, int* size, const char ch);
static Token* createToken(TokenType type, char* name, char* word, int row, int column);
static void insertTable(SymbolTable* symbolTable, char* key, Token* token);
static void showError(const char* message, const int row, const int column);
static int isReservedWord(const char* word);
static int isReservedType(const char* word);
static int isReservedOperator(const char* word);

Token* lexerAnalysis(SymbolTable* table) {
	char* word = (char*) malloc(sizeof(char));
	int state = 0, size = 0;

	while((ch = fgetc(input)) != EOF) {
		column++;

		switch(state) {
			// q0:
			// Estado inicial, identificando espacos em branco, valores numericos,
			// valores alfanumericos, valores simbolicos e operadores:
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
						addWord(&word, &size, ch);
						state = 2;

						break;
					}


					// Identificando valores alfanumericos:
					if(ch >= 'a' && ch <= 'z' || ch >= 'A' && ch <= 'Z' || ch >= '0' && ch <= '9') {
						addWord(&word, &size, ch);
						state = 1;

						break;
					}

					// Identificando si­mbolos:
					if(ch == SMB_OBC || ch == SMB_CBC || ch == SMB_SEM || ch == SMB_OPA || ch == SMB_CPA || ch == SMB_DOT || ch == SMB_COM || ch == SMB_COLON || ch == SMB_SQT || ch == SMB_DQT) {
						addWord(&word, &size, ch);

						if(ch == SMB_COLON) {
							state = 5;
							break;
						} else if(ch == SMB_SQT) {
							state = 6;
							break;
						} else {
							Token* token = createToken(SMB, "Symbol", word, row, column);
							insertTable(table, word, token);
							return token;
						}
					}

					// Identificando operadores:
					if(ch == OP_SUM || ch == OP_SUB || ch == OP_DIV || ch == OP_MUL || ch == OP_LT || ch == OP_GT) {
						addWord(&word, &size, ch);

						if(ch == OP_SUM || ch == OP_SUB || ch == OP_DIV || ch == OP_MUL) {
							Token* token =  createToken(OP, "Binary Arithmetic Operator", word, row, column);
							insertTable(table, word, token);
							return token;
						} else {
							state = 4;
						}

						break;
					}

					addWord(&word, &size, ch);

					char message[50] = "Unknown character: '";
					message[20] = ch;
					strcat(message, "'");
					
					showError(message, row, column);
					Token* token = createToken(ERROR, "LexicalError", word, row, column);
					insertTable(table, message, token);
					return token; 
				}

			// q1:
			// Lidando com valores alfanumericos e indentificando palavras reservadas
			// e identificadores:
			case 1:
				{
					if(ch >= 'a' && ch <= 'z' || ch >= 'A' && ch <= 'Z' || ch >= '0' && ch <= '9') {
						addWord(&word, &size, ch);
					} else {
						ungetc(ch, input);
						column--;

						if(isReservedWord(word)) {
							Token* token = createToken(RESERVED_WORD, "Reserved-word", word, row, column);
							insertTable(table, word, token);
							return token;
						} else if(isReservedType(word)) {
							Token* token = createToken(RESERVED_TYPE, "Reserved-type", word, row, column);
							insertTable(table, word, token);
							return token;						
						} else if(isReservedOperator(word)) {
							Token* token = createToken(RESERVED_OP, "Reserved-operator", word, row, column);
							insertTable(table, word, token);
							return token;
						} else {
							Token* token = createToken(IDENTIFIER, "Identifier", word, row, column);
							insertTable(table, word, token);
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
						addWord(&word, &size, ch);
					} else if(ch == SMB_DOT) {
						addWord(&word, &size, ch);
						state = 3;
					} else {
						ungetc(ch, input);
						column--;

						Token* token = createToken(RESERVED_TYPE, "Integer value", word, row, column);
						insertTable(table, word, token);
						return token;
					}

					break;
				}

			// q3:
			// Lidando com numeros reais:
			case 3:
				{
					if(ch >= '0' && ch <= '9') {
						addWord(&word, &size, ch);
					} else {
						ungetc(ch, input);
						column--;

						Token* token = createToken(RESERVED_TYPE, "Real value", word, row, column);
						insertTable(table, word, token);
						return token;
					}

					break;
				}

			// q4:
			// Lidando com operadores:
			case 4:
				{
					if(ch == OP_EQU && (word[size - 1] == OP_GT || word[size - 1] == OP_LT)) {
						addWord(&word, &size, ch);
					} else {
						ungetc(ch, input);
						column--;

						Token* token = createToken(OP, "Relational Operator", word, row, column);
						insertTable(table, word, token);
						return token;
					}

					break;
				}

			// q5:
			// Identificando si­mbolos:
			case 5:
				{
					if(ch == OP_EQU && word[size - 1] == SMB_COLON) {
						addWord(&word, &size, ch);

						Token* token = createToken(OP, "Assignment Operator", word, row, column);
						insertTable(table, word, token);
						return token;
					} else {
						ungetc(ch, input);
						column--;

						Token* token = createToken(SMB, "Symbol", word, row, column);
						insertTable(table, word, token);
						return token;
					}
				}

			// q6:
			// Identificando strings:
			case 6:
				{
					if(ch == SMB_SQT) {
						addWord(&word, &size, ch);
						Token* token = createToken(STRING, "String", word, row, column);
						insertTable(table, word, token);
						return token;
					} else if(ch == EOF || ch == NEW_LINE) {
						showError("String not closed", row, column);
						Token* token = createToken(ERROR, "LexicalError", word, row, column);
						insertTable(table, word, token);
					} else {
						addWord(&word, &size, ch);
					}

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
	Token* token = createToken(END_OF_FILE, "EOF", "EOF", row, column);
	insertTable(table, "EOF", token);
	return token;
}

static void showError(const char* message, const int row, const int column) {
	printf("LexicalError: %s at %d:%d\n", message, row, column);
	fprintf(output, "LexicalError: %s at %d:%d\n", message, row, column);
}

static void addWord(char** word, int* size, const char ch) {
	*word = (char*) realloc(*word, sizeof(char) * (*size + 2));
	(*word)[(*size)++] = ch;
	(*word)[*size] = '\0';
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

	for(int size = 0; size < reservedWordsSize; size++) {
		if(strcmp(word, reserverdWords[size]) == 0) return 1;
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

	for(int size = 0; size < reservedWordsSize; size++) {
		if(strcmp(word, reserverdWords[size]) == 0) return 1;
	}

	return 0;
}

static int isReservedOperator(const char* word) {
	if(word == NULL) return 0;

	static const char* reserverdWords[] = {
		RESERVED_OP_NOT, RESERVED_OP_AND, RESERVED_OP_OR, RESERVED_OP_MOD
	};

	static const int reservedWordsSize = sizeof(reserverdWords) / sizeof(char*);

	for(int size = 0; size < reservedWordsSize; size++) {
		if(strcmp(word, reserverdWords[size]) == 0) return 1;
	}

	return 0;
}

SymbolTable* initTable() {
	SymbolTable* symbolTable = (SymbolTable*) malloc(sizeof(SymbolTable));
	symbolTable->entries = (Table**) malloc(100 * sizeof(Table*));

	for (int i = 0; i < 100; i++) {
		symbolTable->entries[i] = NULL;
	}

	return symbolTable;
}

static unsigned int hash(char* key, int tableSize) {
	unsigned int hash = 0;

	while (*key) {
		hash = (hash << 5) + *key++;
	}

	return hash % tableSize;
}

static void insertTable(SymbolTable* symbolTable, char* key, Token* token) {
	unsigned int index = hash(key, sizeof(symbolTable->entries) / sizeof(Table*));
    Table* table = (Table*) malloc(sizeof(Table));

	table->key = key;
	table->token = token;
	table->next = symbolTable->entries[index];
	symbolTable->entries[index] = table;
}

Token* searchTable(SymbolTable* symbolTable, char* key) {
    unsigned int index = hash(key, sizeof(symbolTable->entries) / sizeof(Table*));
    Table* table = symbolTable->entries[index];

    while (table != NULL) {
        if (strcmp(table->key, key) == 0) {
            return table->token;
        }

        table = table->next;
    }
	
    return NULL;
}

static Token* createToken(TokenType type, char* name, char* word, int linha, int coluna) {
	Token* token = (Token*) malloc(sizeof(Token));

	token->type = type;
	token->name = name;
	token->word = word;
	token->row = row;
	token->column = column;

	return token;
}