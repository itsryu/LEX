#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lexer.h"

static int column = 0, row = 1;
static char ch;

// Cria um token:
static Token* createToken(char *name, char *word, int row, int column) {
    Token *token = (Token*)malloc(sizeof(Token));

    token->name = name;
    token->word = word;
    token->row = row;
    token->column = column;

    return token;
}

// Exibe um erro léxico:
static void showError(const char* message, const int row, const int column) {
    printf("LexicalError: %s at %d:%d\n", message, row, column);
    fprintf(output, "LexicalError: %s at %d:%d\n", message, row, column);
}

// Adiciona um caractere a uma palavra
static void addWord(char** word, int *i, const char ch) {
    char* temp = (char*) realloc(*word, sizeof(char) * (*i + 2));

    if (temp == NULL) {
        showError("Error allocating memory", row, column);
        exit(EXIT_FAILURE);
    } else {
        *word = temp;
        (*word)[(*i)++] = ch;
        (*word)[*i] = '\0';
    }
}

// Verifica se uma palavra é reservada:
static int isReservedWord(const char* word) {
    if (word == NULL) return 0;

    static const char* reserverdWords[] = {
        RESERVED_WORD_PROGRAM, 
        RESERVED_WORD_VAR,
        RESERVED_WORD_BEGIN, 
        RESERVED_WORD_END, 
        RESERVED_WORD_IF,
        RESERVED_WORD_ELSE,
        RESERVED_WORD_THEN,
        RESERVED_WORD_DO,   
        RESERVED_WORD_WHILE,
        RESERVED_TYPE_INTEGER,
        RESERVED_TYPE_REAL,
    };

    static const int reservedWordsSize = sizeof(reserverdWords) / sizeof(char*);

    for (int i = 0; i < reservedWordsSize; i++) {
        if (strcmp(word, reserverdWords[i]) == 0) return 1;
    }

    return 0;
}

Token* lexerAnalysis() {
    char* word = (char*) malloc(sizeof(char));
    int state = 0, i = 0;

    while((ch = fgetc(input)) != EOF) {
        column++;

        switch (state) {
            // q0:
            // Estado inicial, identificando espaços em branco, valores numéricos, valores alfanuméricos e valores alfabéticos:
            case 0: {
                // Identificando espaços em branco:
                if (ch == SPACE || ch == TAB || ch == NEW_LINE) {
                    if (ch == NEW_LINE) {
                        row++;
                        column = 0;
                    }

                    break;
                }

                // Identificando valores numéricos:s
                if (ch >= '0' && ch <= '9') {
                    addWord(&word, &i, ch);
                    state = 2;

                    break;
                }

                // Identificando valores alfanuméricos:
                if (ch >= 'a' && ch <= 'z' || ch >= 'A' && ch <= 'Z' || ch >= '0' && ch <= '9') {
                    addWord(&word, &i, ch);
                    state = 1;

                    break;
                }
        
                // Identificando valores alfabéticos:
                if (ch >= 'a' && ch <= 'z' || ch >= 'A' && ch <= 'Z') {
                    addWord(&word, &i, ch);
                    state = 1;

                    break;
                }

                char message[50] = "Unknown character: '";
                message[20] = ch;
                strcat(message, "'");

                showError(message, row, column);

                break;
            }

            // q1:
            // Lidando com valores alfanuméricos e indentificando palavras reservadas e identificadores:
            case 1: {
                if (ch >= 'a' && ch <= 'z' || ch >= 'A' && ch <= 'Z' || ch >= '0' && ch <= '9') {
                    addWord(&word, &i, ch);
                } else { 
                    ungetc(ch, input);
                    column--;

                    if (isReservedWord(word)) {
                        Token* token = createToken("Reserved-word", word, row, column);
                        return token;
                    } else {
                        Token* token = createToken("Identifier", word, row, column);
                        return token;
                    }
                }
                
                break;
            }

            // q2:
            // Lidando com números inteiros e identificando possíveis números reais:
            case 2: {
                if (ch >= '0' && ch <= '9') {
                    addWord(&word, &i, ch);
                } else if (ch == '.') {
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
            // Lidando com números reais:
            case 3: {
                if (ch >= '0' && ch <= '9') {
                    addWord(&word, &i, ch);
                } else {
                    ungetc(ch, input);
                    column--;

                    Token* token = createToken("Real value", word, row, column);
                    return token;
                }

                break;
            }

            default: {
                showError("Unknown state", row, column);

                break;
            }
        }
    }

    free(word);
    Token* token = createToken("EOF", "EOF", row, column);

    return token;
}