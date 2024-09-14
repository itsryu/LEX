#include <stdio.h>
#include <string.h>

#include "lexer.h"

int main(int argc, char** argv) {
    if(argv[1] == NULL) {
        printf("Argument not specified, use:\n\t--help\n");
        return 1;
    } else {
        if(strcmp(argv[1], "--help") == 0 || strcmp(argv[1], "-h") == 0) {
            printf("Usage:\n\t--file <file>\t\tReads a pascal file and do the lexical analysis\n");
            return 0;
        }

        if (strcmp(argv[1], "--file") != 0 || strcmp(argv[1], "-f") != 0 && argv[2] == NULL) {
            printf("File not specified:\n\t--file <file>\n");
            return 1;
        }  else {
            input = fopen(argv[2], "r");

            if(input == NULL) {
                printf("File not found:\n\t--file <file>\n");
                return 1;
            } else {
                output = fopen("output.lex", "w");
                
                // TODO: Implement lexer:
            }
        }
    }

    return 0;
}