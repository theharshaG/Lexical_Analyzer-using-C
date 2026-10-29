#include <stdio.h>
#include "lexer.h"

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

        case EOF_TOKEN:
            return "EOF";

        default:
            return "INVALID";
    }
}

int main()
{
    Token token;

    initializeLexer("input.c");

    printf("---------------------------------------------\n");
    printf("%-10s %-15s : %s\n",
           "Line No", "Token", "Token Type");
    printf("---------------------------------------------\n");

    while (1)
    {
        token = getNextToken();

        if (token.type == EOF_TOKEN)
        {
            break;
        }

        printf("%-10d %-15s : %s\n",
               token.lineNumber,
               token.lexeme,
               getTokenTypeName(token.type));
    }

    printf("---------------------------------------------\n");

    return 0;
}