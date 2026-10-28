#include <stdio.h>
#include "lexer.h"

int main()
{
    initializeLexer("input.c");

    Token token;

    while (1)
    {
        token = getNextToken();

        if (token.type == EOF_TOKEN)
        {
            break;
        }

        printf("Lexeme: %-15s Type: %d\n",
               token.lexeme, token.type);
    }

    return 0;
}