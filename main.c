#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lexer.h"

static void saveFile(Token* token);

int main(int argc, char** argv) {
	if(argv[1] == NULL) {
		printf("Argument not specified, use:\n\t--help\n");
		return 1;
	} else {
		if(strcmp(argv[1], "--help") == 0 || strcmp(argv[1], "-h") == 0) {
			printf("Usage:\n\t--file <file>\t\tReads a pascal file and do the lexical analysis\n");
			return 0;
		}

		if(strcmp(argv[1], "--file") != 0 || strcmp(argv[1], "-f") != 0 && argv[2] == NULL) {
			printf("File not specified:\n\t--file <file>\n");
			return 1;
		} else {
			input = fopen(argv[2], "r");

			if(input == NULL) {
				printf("File not found:\n\t--file <file>\n");
				return 1;
			} else {
				Token* token;
				SymbolTable* table = initTable();
				output = fopen("./output/output.lex", "w");

				while((token = lexerAnalysis(table)) && token->type != ERROR && token->type != END_OF_FILE && token != NULL) {
					printf("<%d, %s, '%s'> : <%d, %d>\n", token->type, token->name, token->word, token->row, token->column);
					saveFile(token);
				}

				free(token);
				free(table);
			}
		}
	}

	return 0;
}

static void saveFile(Token* token) {
	fprintf(output, "<%d, %s, '%s'> : <%d, %d>\n", token->type, token->name, token->word, token->row, token->column);
}