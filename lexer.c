#include "lexer.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int column = 0, row = 1;
static char ch;
static void addWord(char** word, int* size, const char ch);
static Token* createToken(TokenType type, char* name, char* word, int row, int column);
static void insertTable(Table* table, char* key, Token* token);
static void showError(const char* message, const int row, const int column);
static int isReservedWord(const char* word);
static int isReservedType(const char* word);
static int isReservedOperator(const char* word);

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
Token* lexerAnalysis(Table* table) {
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
					if(ch >= 'a' && ch <= 'z' || ch >= 'A' && ch <= 'Z' || ch >= '0' && ch <= '9' || ch == SMB_UNDER) {
						addWord(&word, &size, ch);
						state = 1;

						break;
					}

					// Identificando si�mbolos:
					if(ch == SMB_OBC || ch == SMB_CBC || ch == SMB_SEM || ch == SMB_OPA || ch == SMB_CPA || ch == SMB_DOT || ch == SMB_COM || ch == SMB_COLON || ch == SMB_SQT || ch == SMB_DQT) {
						addWord(&word, &size, ch);

						if(ch == SMB_COLON) {
							state = 5;
							break;
						} else if(ch == SMB_SQT || ch == SMB_DQT) {
							state = 6;
							break;
						} else {
							Token* token = createToken(SYMBOL, "Symbol", word, row, column);
							insertTable(table, word, token);
							return token;
						}
					}

					// Identificando operadores:
					if(ch == OP_SUM || ch == OP_SUB || ch == OP_DIV || ch == OP_MUL || ch == OP_LT || ch == OP_GT) {
						addWord(&word, &size, ch);

						if(ch == OP_SUM || ch == OP_SUB || ch == OP_DIV || ch == OP_MUL) {
							Token* token =  createToken(OPERATOR, "Binary Arithmetic Operator", word, row, column);
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
					if(ch >= 'a' && ch <= 'z' || ch >= 'A' && ch <= 'Z' || ch >= '0' && ch <= '9' || ch == SMB_UNDER) {
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
							Token* token = createToken(RESERVED_OPERATOR, "Reserved-operator", word, row, column);
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

						Token* token = createToken(NUMBER, "Integer number", word, row, column);
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

						Token* token = createToken(NUMBER, "Real number", word, row, column);
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

						Token* token = createToken(OPERATOR, "Relational Operator", word, row, column);
						insertTable(table, word, token);
						return token;
					}

					break;
				}

			// q5:
			// Identificando simbolos:
			case 5:
				{
					if(ch == OP_EQU && word[size - 1] == SMB_COLON) {
						addWord(&word, &size, ch);

						Token* token = createToken(OPERATOR, "Assignment Operator", word, row, column);
						insertTable(table, word, token);
						return token;
					} else {
						ungetc(ch, input);
						column--;

						Token* token = createToken(SYMBOL, "Symbol", word, row, column);
						insertTable(table, word, token);
						return token;
					}
				}

			// q6:
			// Identificando strings:
			case 6:
				{
					if(ch == SMB_SQT || ch == SMB_DQT) {
						addWord(&word, &size, ch);
						Token* token = createToken(STRING, "String", word, row, column);
						insertTable(table, word, token);
						return token;
					} else if(ch == END_OF_FILE || ch == NEW_LINE) {
						showError("String not closed", row, column);
						Token* token = createToken(ERROR, "LexicalError", word, row, column);
						insertTable(table, word, token);
						return token;
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

/**
 * @brief Initializes a new Table structure.
 *
 * This function allocates memory for a Table structure and its entries.
 * It initializes each entry to NULL.
 *
 * @return A pointer to the newly initialized Table structure.
 */
Table* initTable() {
	Table* table = (Table*) malloc(sizeof(Table));
	table->entries = (Entry**) malloc(100 * sizeof(Entry*));

	for (int i = 0; i < 100; i++) {
		table->entries[i] = NULL;
	}

	return table;
}

/**
 * @brief Computes a hash value for a given key.
 *
 * This function takes a string key and computes its hash value using a 
 * simple hash function. The hash value is then modded by the table size 
 * to ensure it fits within the bounds of the hash table.
 *
 * @param key The string key to be hashed.
 * @param tableSize The size of the hash table.
 * @return The computed hash value modded by the table size.
 */
static unsigned int hash(char* key, int tableSize) {
	unsigned int hash = 0;

	while (*key) {
		hash = (hash << 5) + *key++;
	}

	return hash % tableSize;
}

/**
 * @brief Inserts a new entry into the hash table.
 *
 * This function creates a new entry with the given key and token, computes the hash index for the key,
 * and inserts the entry into the hash table at the computed index.
 *
 * @param table A pointer to the hash table where the entry will be inserted.
 * @param key A string representing the key for the new entry.
 * @param token A pointer to the token associated with the key.
 */
static void insertTable(Table* table, char* key, Token* token) {
	unsigned int index = hash(key, sizeof(table->entries) / sizeof(Entry*));
    Entry* entry = (Entry*) malloc(sizeof(Entry));

	entry->key = key;
	entry->token = token;
	entry->next = table->entries[index];
	table->entries[index] = entry;
}

/**
 * @brief Searches for a token in the hash table using the given key.
 *
 * This function searches the hash table for an entry with the specified key.
 * If the key is found, the associated token is returned. If the key is not found,
 * the function returns NULL.
 *
 * @param table A pointer to the hash table to search.
 * @param key The key to search for in the hash table.
 * @return A pointer to the token associated with the key, or NULL if the key is not found.
 */
Token* searchTable(Table* table, char* key) {
    unsigned int index = hash(key, sizeof(table->entries) / sizeof(Entry*));
    Entry* entry = table->entries[index];

    while (entry != NULL) {
        if (strcmp(entry->key, key) == 0) {
            return entry->token;
        }

        entry = entry->next;
    }
	
    return NULL;
}

/**
 * @brief Creates a new Token with the specified attributes.
 *
 * @param type The type of the token.
 * @param name The name of the token.
 * @param word The word associated with the token.
 * @param row The line number where the token is found.
 * @param column The column number where the token is found.
 * @return Token* A pointer to the newly created Token.
 */
static Token* createToken(TokenType type, char* name, char* word, int row, int column) {
	Token* token = (Token*) malloc(sizeof(Token));

	token->type = type;
	token->name = name;
	token->word = word;
	token->row = row;
	token->column = column;

	return token;
}