#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "includes/lexer.h"
#include "includes/parser.h"

static void saveFile(Token *token);
static char *createOutputPath(const char *inputName);

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
 * and performs to analyse it.
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
				char *inputName = argv[2];
				char *outputPath = createOutputPath(inputName);
				input = fopen(inputName, "r");

				if (input == NULL)
				{
					printf("File not found:\n\t--file <file>\n");
					return 1;
				}
				else
				{
					Token *token;
					Table *table = initTable();
					output = fopen(outputPath, "w");

					while ((token = lexerAnalysis(table)) && token->type != ERROR && token->type != END_OF_FILE && token != NULL);

					free(token);

					Entry *entry = table->entries[0];

					while (entry != NULL)
					{
						printf("<%d, %s, '%s'> : <%d, %d>\n", entry->token->type, entry->token->name, entry->token->word, entry->token->row, entry->token->column);
						saveFile(entry->token);
						entry = entry->next;
					}

					ASTNode *ast = parseTokens(table);

					free(table);
					fclose(input);
					fclose(output);
				}
			}
		}
	}

	return 0;
}


static void saveFile(Token *token)
{
	fprintf(output, "<%d, %s, '%s'> : <%d, %d>\n", token->type, token->name, token->word, token->row, token->column);
}

static char *createOutputPath(const char *inputName)
{
	const char *baseName = strrchr(inputName, '/');
	baseName = baseName ? baseName + 1 : inputName;

	size_t outputNameLen = strlen(baseName) + 5;
	char *outputName = malloc(outputNameLen);

	if (outputName == NULL)
	{
		return NULL;
	}

	strcpy(outputName, baseName);
	strcpy(strrchr(outputName, '.'), ".lex");

	size_t outputPathLen = strlen("./output/") + strlen(outputName) + 1;
	char *outputPath = malloc(outputPathLen);

	if (outputPath == NULL)
	{
		free(outputName);
		return NULL;
	}

	strcpy(outputPath, "./output/");
	strcat(outputPath, outputName);

	free(outputName);
	return outputPath;
}