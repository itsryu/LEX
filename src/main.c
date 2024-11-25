#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "includes/lexer.h"
#include "includes/parser.h"

static void saveFile(Token *token);

/**
 * @file main.c
 * @brief Main entry point for the lexical analysis program.
 *
 * This program reads a Pascal file and performs lexical and syntax analysis on it.
 * It supports the following command-line arguments:
 * - `--help` or `-h`: Displays usage information.
 * - `--file <file>` or `-f <file>`: Specifies the Pascal file to be analyzed.
 *
 * The program checks for valid arguments and file extensions, opens the specified file,
 * and performs lexical analysis, printing tokens and saving them to an output file.
 *
 * @param argc The number of command-line arguments.
 * @param argv The array of command-line arguments.
 * @return Returns 0 on success, or 1 on error.
 */
int main(int argc, char **argv)
{
	if (argc == 0 || argv[1] == NULL)
	{
		printf("Argument not specified, use:\n\t--help\n");
		return 1;
	}
	else
	{
		if (strcmp(argv[1], "--help") == 0 || strcmp(argv[1], "-h") == 0)
		{
			printf("Usage:\n\t--file <file>\t\tReads a pascal file and do the lexical analysis\n");
			return 0;
		}

		if (strcmp(argv[1], "--file") != 0 || strcmp(argv[1], "-f") != 0 && argv[2] == NULL)
		{
			printf("File not specified:\n\t--file <file>\n");
			return 1;
		}
		else
		{
			const char *file_ext = strrchr(argv[2], '.');

			if (!file_ext || strcmp(file_ext, ".pas") != 0)
			{
				printf("Invalid file extension. Please provide a .pas file.\n");
				return 1;
			}
			else
			{
				input = fopen(argv[2], "r");

				if (input == NULL)
				{
					printf("File not found:\n\t--file <file>\n");
					return 1;
				}
				else
				{
					Token *token;
					Table *table = initTable();
					output = fopen("./output/output.lex", "w");

					while ((token = lexerAnalysis(table)) && token->type != ERROR && token->type != END_OF_FILE && token != NULL);

					free(token);

					Entry *entry = table->entries[0];

					while (entry != NULL)
					{
						printf("LEX: <%d, %s, '%s'> : <%d, %d>\n", entry->token->type, entry->token->name, entry->token->word, entry->token->row, entry->token->column);
						saveFile(entry->token);
						entry = entry->next;
					}

					ASTNode *ast = parseTokens(table);

					while(ast != NULL)
					{
						printf("AST: <%d, %s>\n", ast->type, ast->value);
						ast = ast->left;
					}

					free(table);
					fclose(input);
					fclose(output);
				}
			}
		}
	}

	return 0;
}

/**
 * @brief Saves the token information to the output file.
 *
 * This function writes the details of a given token to the output file in a specific format.
 *
 * @param token A pointer to the Token structure containing the token information.
 */
static void saveFile(Token *token)
{
	fprintf(output, "<%d, %s, '%s'> : <%d, %d>\n", token->type, token->name, token->word, token->row, token->column);
}