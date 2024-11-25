#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../includes/lexer.h"
#include "../includes/tokens.h"
#include "../includes/errors.h"

static int column = 0, row = 1;
static char ch;

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
Token *lexerAnalysis(Table *table)
{
	char *word = (char *)malloc(sizeof(char));
	int state = 0, size = 0;

	while ((ch = fgetc(input)) != EOF)
	{
		column++;

		switch (state)
		{
		// q0:
		// initial state, identifying white spaces, numeric values,
		// alphanumeric values, symbolic values, and operators:
		case 0:
		{
			// identyfing spaces, tabs, new lines and comments:
			if (ch == SPACE || ch == TAB || ch == NEW_LINE || ch == '/')
			{
				if (ch == NEW_LINE)
				{
					row++;
					column = 0;
				}

				if (ch == '/')
				{
					ch = fgetc(input);
					column++;

					if (ch == '/')
					{
						while ((ch = fgetc(input)) != EOF && ch != NEW_LINE)
						{
							column++;
						}
						if (ch == NEW_LINE)
						{
							row++;
							column = 0;
						}
					}
					else
					{
						ungetc(ch, input);
						column--;
					}
				}

				break;
			}

			// identifying numeric values:
			if (ch >= '0' && ch <= '9')
			{
				addWord(&word, &size, ch);
				state = 2;

				break;
			}

			// identyfing alfanumeric values:
			if (ch >= 'a' && ch <= 'z' || ch >= 'A' && ch <= 'Z' || ch >= '0' && ch <= '9' || ch == SMB_UNDER)
			{
				addWord(&word, &size, ch);
				state = 1;

				break;
			}

			// identifying symbols:
			if (ch == SMB_OBC || ch == SMB_CBC || ch == SMB_SEM || ch == SMB_OPA || ch == SMB_CPA || ch == SMB_DOT || ch == SMB_COM || ch == SMB_COLON || ch == SMB_SQT || ch == SMB_DQT)
			{
				addWord(&word, &size, ch);

				if (ch == SMB_COLON)
				{
					state = 5;
					break;
				}
				else if (ch == SMB_SQT || ch == SMB_DQT)
				{
					state = 6;
					break;
				}
				else
				{
					Token *token = createToken(SYMBOL, "Symbol", word, row, column);
					insertTable(table, word, token);
					return token;
				}
			}

			// identyfing operators:
			if (ch == OP_SUM || ch == OP_SUB || ch == OP_DIV || ch == OP_MUL || ch == OP_LT || ch == OP_GT)
			{
				addWord(&word, &size, ch);

				if (ch == OP_SUM || ch == OP_SUB || ch == OP_DIV || ch == OP_MUL)
				{
					Token *token = createToken(OPERATOR, "Binary Arithmetic Operator", word, row, column);
					insertTable(table, word, token);
					return token;
				}
				else
				{
					state = 4;
				}

				break;
			}

			addWord(&word, &size, ch);
			fprintf(stderr, ERR_UNKOWN_CHARACTER, ch, row, column);
			return NULL;
		}

		// q1:
		// handling alphanumeric values and identifying reserved words
		// and identifiers:
		case 1:
		{
			if (ch >= 'a' && ch <= 'z' || ch >= 'A' && ch <= 'Z' || ch >= '0' && ch <= '9' || ch == SMB_UNDER)
			{
				addWord(&word, &size, ch);
			}
			else
			{
				ungetc(ch, input);
				column--;

				if (isReservedWord(word))
				{
					Token *token = createToken(RESERVED_WORD, "Reserved-word", word, row, column);
					insertTable(table, word, token);
					return token;
				}
				else if (isReservedType(word))
				{
					Token *token = createToken(RESERVED_TYPE, "Reserved-type", word, row, column);
					insertTable(table, word, token);
					return token;
				}
				else if (isReservedOperator(word))
				{
					Token *token = createToken(RESERVED_OPERATOR, "Reserved-operator", word, row, column);
					insertTable(table, word, token);
					return token;
				}
				else
				{
					Token *token = createToken(IDENTIFIER, "Identifier", word, row, column);
					insertTable(table, word, token);
					return token;
				}
			}

			break;
		}

		// q2:
		// handling integers and identifying possible real numbers:
		case 2:
		{
			if (ch >= '0' && ch <= '9')
			{
				addWord(&word, &size, ch);
			}
			else if (ch == SMB_DOT)
			{
				addWord(&word, &size, ch);
				state = 3;
			}
			else
			{
				ungetc(ch, input);
				column--;

				Token *token = createToken(NUMBER, "Integer number", word, row, column);
				insertTable(table, word, token);
				return token;
			}

			break;
		}

		// q3:
		// handling real numbers:
		case 3:
		{
			if (ch >= '0' && ch <= '9')
			{
				addWord(&word, &size, ch);
			}
			else
			{
				ungetc(ch, input);
				column--;

				Token *token = createToken(NUMBER, "Real number", word, row, column);
				insertTable(table, word, token);
				return token;
			}

			break;
		}

		// q4:
		// handling relational operators:
		case 4:
		{
			if (ch == OP_EQU && (word[size - 1] == OP_GT || word[size - 1] == OP_LT))
			{
				addWord(&word, &size, ch);
			}
			else
			{
				ungetc(ch, input);
				column--;

				Token *token = createToken(OPERATOR, "Relational Operator", word, row, column);
				insertTable(table, word, token);
				return token;
			}

			break;
		}

		// q5:
		// identifying assignment operators and symbols:
		case 5:
		{
			if (ch == OP_EQU && word[size - 1] == SMB_COLON)
			{
				addWord(&word, &size, ch);

				Token *token = createToken(OPERATOR, "Assignment Operator", word, row, column);
				insertTable(table, word, token);
				return token;
			}
			else
			{
				ungetc(ch, input);
				column--;

				Token *token = createToken(SYMBOL, "Symbol", word, row, column);
				insertTable(table, word, token);
				return token;
			}
		}

		// q6:
		// identifying strings:
		case 6:
		{
			if (ch == SMB_SQT || ch == SMB_DQT)
			{
				addWord(&word, &size, ch);
				Token *token = createToken(STRING, "String", word, row, column);
				insertTable(table, word, token);
				return token;
			}
			else if (ch == END_OF_FILE || ch == NEW_LINE)
			{
				fprintf(stderr, ERR_STRING_NOT_CLOSED, row, column);
				return NULL;
			}
			else
			{
				addWord(&word, &size, ch);
			}

			break;
		}

		default:
		{
			fprintf(stderr, ERR_UNKOWN_STATE, row, column);
			return NULL;
		}
		}
	}

	if (size > 0)
	{
		if (state == 1)
		{
			if (isReservedWord(word))
			{
				Token *token = createToken(RESERVED_WORD, "Reserved-word", word, row, column);
				insertTable(table, word, token);
			}
			else if (isReservedType(word))
			{
				Token *token = createToken(RESERVED_TYPE, "Reserved-type", word, row, column);
				insertTable(table, word, token);
			}
			else if (isReservedOperator(word))
			{
				Token *token = createToken(RESERVED_OPERATOR, "Reserved-operator", word, row, column);
				insertTable(table, word, token);
			}
			else
			{
				Token *token = createToken(IDENTIFIER, "Identifier", word, row, column);
				insertTable(table, word, token);
			}
		}
		else if (state == 2)
		{
			Token *token = createToken(NUMBER, "Integer number", word, row, column);
			insertTable(table, word, token);
		}
		else if (state == 3)
		{
			Token *token = createToken(NUMBER, "Real number", word, row, column);
			insertTable(table, word, token);
		}
	}

	free(word);
	Token *token = createToken(END_OF_FILE, "EOF", "EOF", row, column);
	return token;
}

static void addWord(char **word, int *size, const char ch)
{
	*word = (char *)realloc(*word, sizeof(char) * (*size + 2));
	(*word)[(*size)++] = ch;
	(*word)[*size] = '\0';
}

static int isReservedWord(const char *word)
{
	if (word == NULL)
		return 0;

	static const char *reserverdWords[] = {
		RESERVED_WORD_PROGRAM, RESERVED_WORD_VAR, RESERVED_WORD_BEGIN,
		RESERVED_WORD_END, RESERVED_WORD_IF, RESERVED_WORD_ELSE,
		RESERVED_WORD_THEN, RESERVED_WORD_DO, RESERVED_WORD_WHILE,
		RESERVED_WORD_FOR, RESERVED_WORD_TO, RESERVED_WORD_DOWNTO,
		RESERVED_WORD_REPEAT, RESERVED_WORD_UNTIL, RESERVED_WORD_CASE,
		RESERVED_WORD_OF, RESERVED_WORD_FUNCTION, RESERVED_WORD_PROCEDURE,
		RESERVED_WORD_ARRAY, RESERVED_WORD_RECORD, RESERVED_WORD_CONST,
		RESERVED_WORD_TYPE, RESERVED_WORD_FILE, RESERVED_WORD_SET,
		RESERVED_WORD_GOTO, RESERVED_WORD_WITH, RESERVED_WORD_IN};

	static const int reservedWordsSize = sizeof(reserverdWords) / sizeof(char *);

	for (int size = 0; size < reservedWordsSize; size++)
	{
		if (strcmp(word, reserverdWords[size]) == 0)
			return 1;
	}

	return 0;
}

static int isReservedType(const char *word)
{
	if (word == NULL)
		return 0;

	static const char *reserverdWords[] = {
		RESERVED_TYPE_INTEGER, RESERVED_TYPE_REAL,
		RESERVED_TYPE_CHAR, RESERVED_TYPE_STRING,
		RESERVED_TYPE_DOUBLE, RESERVED_TYPE_BYTE,
		RESERVED_TYPE_WORD, RESERVED_TYPE_LONGINT,
		RESERVED_TYPE_SHORTINT, RESERVED_TYPE_SINGLE,
		RESERVED_TYPE_EXTENDED, RESERVED_TYPE_COMP,
		RESERVED_TYPE_CURRENCY};

	static const int reservedWordsSize = sizeof(reserverdWords) / sizeof(char *);

	for (int size = 0; size < reservedWordsSize; size++)
	{
		if (strcmp(word, reserverdWords[size]) == 0)
			return 1;
	}

	return 0;
}

static int isReservedOperator(const char *word)
{
	if (word == NULL)
		return 0;

	static const char *reserverdWords[] = {
		RESERVED_OP_NOT, RESERVED_OP_AND, RESERVED_OP_OR, RESERVED_OP_MOD};

	static const int reservedWordsSize = sizeof(reserverdWords) / sizeof(char *);

	for (int size = 0; size < reservedWordsSize; size++)
	{
		if (strcmp(word, reserverdWords[size]) == 0)
			return 1;
	}

	return 0;
}

Table *initTable()
{
	Table *table = (Table *)malloc(sizeof(Table));
	table->entries = (Entry **)malloc(sizeof(Entry *));

	for (int size = 0; size < sizeof(table->entries) / sizeof(Entry *); size++)
	{
		table->entries[size] = NULL;
	}

	table->entryCount = 0;

	return table;
}

static unsigned int hash(char *key, int tableSize)
{
	unsigned int hash = 0;

	while (*key)
	{
		hash = (hash << 5) + *key++;
	}

	return hash % tableSize;
}

static void insertTable(Table *table, char *key, Token *token)
{
	unsigned int index = hash(key, sizeof(table->entries) / sizeof(Entry *));
	Entry *entry = (Entry *)malloc(sizeof(Entry));

	entry->key = key;
	entry->token = token;
	entry->next = NULL;

	if (table->entries[index] == NULL)
	{
		table->entries[index] = entry;
	}
	else
	{
		Entry *current = table->entries[index];
		while (current->next != NULL)
		{
			current = current->next;
		}
		current->next = entry;
	}

	table->entryCount++;
}

Token *searchTable(Table *table, char *key)
{
	unsigned int index = hash(key, sizeof(table->entries) / sizeof(Entry *));
	Entry *entry = table->entries[index];

	while (entry != NULL)
	{
		if (strcmp(entry->key, key) == 0)
		{
			return entry->token;
		}

		entry = entry->next;
	}

	return NULL;
}

static Token *createToken(TokenType type, char *name, char *word, int row, int column)
{
	Token *token = (Token *)malloc(sizeof(Token));

	token->type = type;
	token->name = name;
	token->word = word;
	token->row = row;
	token->column = column;
	token->next = NULL;

	return token;
}