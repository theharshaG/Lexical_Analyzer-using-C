#include <stdio.h>
#include "lexer.h"
#include "syntax.h"

// Convert token type values into names 
static const char *getTokenTypeName(TokenType type)
{
    switch (type)
    {
        case PRE_PROCESSOR:
            return "PRE-PROCESSOR";

        case HEADER:
            return "HEADER";

        case RESERVED_KEYWORD:
            return "RESERVED KEYWORD";

        case NON_RESERVED_KEYWORD:
            return "NON RESERVED KEYWORD";

        case OPERATOR:
            return "OPERATOR";

        case OPEN_BRACE:
            return "OPEN_BRACE";

        case CLOSE_BRACE:
            return "CLOSE_BRACE";

        case TERMINATING_OPERATOR:
            return "TERMINATING OPERATOR";

        case NUMERIC_CONSTANT:
            return "NUMERIC CONSTANT";

        case IDENTIFIER:
            return "IDENTIFIER";

        case UNKNOWN:
            return "UNKNOWN";

        case STRING_LITERAL:
            return "STRING LITERAL";

        case CHARACTER_CONSTANT:
            return "CHARACTER CONSTANT";

        case LEXICAL_ERROR:
            return "LEXICAL_ERROR";

        case EOF_TOKEN:
            return "EOF";

        default:
            return "INVALID";
    }
}

// Process a single file: perform syntax analysis and print tokens
static void processFile(const char *filename)
{
    Token token;

    if (!initializeLexer(filename))
    {
        return;
    }

    printf("\n======================================================\n");
    printf(" FILE: %s\n", filename);
    printf("======================================================\n");

    storeTokens();
    checkHeaders();
    checkDelimiters();
    checkRequiredHeaders();
    checkSemicolon();
    closeLexer();

    initializeLexer(filename);

    printf("------------------------------------------------------\n");
    printf("%-10s %-20s : %s\n", "Line No", "Token", "Token Type");
    printf("------------------------------------------------------\n");

    while (1)
    {
        token = getNextToken();

        if (token.type == EOF_TOKEN)
        {
            break;
        }

        printf("%-10d %-20s : %s\n", token.lineNumber, token.lexeme, getTokenTypeName(token.type));
    }

    printf("------------------------------------------------------\n");
    closeLexer();
}

int main(int argc, char *argv[])
{
    if (argc > 1)
    {
        for (int i = 1; i < argc; i++)
        {
            processFile(argv[i]);
        }
    }
    else
    {
        processFile("input.c");
    }

    return 0;
}
