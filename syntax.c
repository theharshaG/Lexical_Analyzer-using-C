#include <stdio.h>
#include <string.h>
#include "syntax.h"

Token tokens[1000];
int tokenCount = 0;

void storeTokens()
{
    Token token;

    tokenCount = 0;

    while (1)
    {
        token = getNextToken();

        if (token.type == EOF_TOKEN)
            break;

        tokens[tokenCount] = token;
        tokenCount++;
    }

    //printf("Total tokens stored: %d\n", tokenCount);
}

int isValidHeader(const char *header)
{
    const char *validHeaders[] =
    {
        "<stdio.h>",
        "<stdlib.h>",
        "<string.h>",
        "<math.h>",
        "<ctype.h>",
        "<time.h>",
        "<stdbool.h>",
        "<stdint.h>",
        "<limits.h>",
        "<float.h>"
    };

    int size = sizeof(validHeaders) / sizeof(validHeaders[0]);

    for (int i = 0; i < size; i++)
    {
        if (strcmp(header, validHeaders[i]) == 0)
        {
            return 1;
        }
    }

    return 0;
}    
void checkHeaders()
{
    Token token;

    while (1)
    {
        token = getNextToken();

        if (token.type == EOF_TOKEN)
            break;

        if (token.type == HEADER)
        {
            if (isValidHeader(token.lexeme) == 0)
            {
                printf("Syntax Error: Unknown header file '%s' at line %d\n",
                       token.lexeme, token.lineNumber);
            }
        }
    }
}
void checkDelimiters()
{
    Token token;

    int curlyCount = 0;
    int roundCount = 0;
    int squareCount = 0;

    int curlyLine = 0;
    int roundLine = 0;
    int squareLine = 0;

    while (1)
    {
        token = getNextToken();

        if (token.type == EOF_TOKEN)
            break;

        //Curly braces 
        if (strcmp(token.lexeme, "{") == 0)
        {
            curlyCount++;
            curlyLine = token.lineNumber;
        }
        else if (strcmp(token.lexeme, "}") == 0)
        {
            if (curlyCount == 0)
            {
                printf("Syntax Error: Missing opening brace '{' "
                       "for '}' at line %d\n", token.lineNumber);
            }
            else
            {
                curlyCount--;
            }
        }

        // Parentheses 
        else if (strcmp(token.lexeme, "(") == 0)
        {
            roundCount++;
            roundLine = token.lineNumber;
        }
        else if (strcmp(token.lexeme, ")") == 0)
        {
            if (roundCount == 0)
            {
                printf("Syntax Error: Missing opening parenthesis '(' "
                       "for ')' at line %d\n", token.lineNumber);
            }
            else
            {
                roundCount--;
            }
        }

        //Square brackets
        else if (strcmp(token.lexeme, "[") == 0)
        {
            squareCount++;
            squareLine = token.lineNumber;
        }
        else if (strcmp(token.lexeme, "]") == 0)
        {
            if (squareCount == 0)
            {
                printf("Syntax Error: Missing opening bracket '[' "
                       "for ']' at line %d\n", token.lineNumber);
            }
            else
            {
                squareCount--;
            }
        }
    }

    //Check missing closing delimiters 
    if (curlyCount > 0)
        printf("Syntax Error: Missing closing brace. "
               "Opening '{' at line %d\n", curlyLine);

    if (roundCount > 0)
        printf("Syntax Error: Missing closing parenthesis. "
               "Opening '(' at line %d\n", roundLine);

    if (squareCount > 0)
        printf("Syntax Error: Missing closing bracket. "
               "Opening '[' at line %d\n", squareLine);

}
int isHeaderPresent(const char *header)
{
    for (int i = 0; i < tokenCount; i++)
    {
        if (tokens[i].type == HEADER &&
            strcmp(tokens[i].lexeme, header) == 0)
        {
            return 1;
        }
    }

    return 0;
}
void checkRequiredHeaders()
{
    int printfFound = 0;
    int scanfFound = 0;

    int strlenFound = 0;
    int strcmpFound = 0;
    int strcpyFound = 0;

    int mallocFound = 0;
    int callocFound = 0;
    int freeFound = 0;

    int sqrtFound = 0;
    int powFound = 0;

    /* Search for functions in stored tokens */
    for (int i = 0; i < tokenCount; i++)
    {
        if (strcmp(tokens[i].lexeme, "printf") == 0)
            printfFound = 1;

        else if (strcmp(tokens[i].lexeme, "scanf") == 0)
            scanfFound = 1;

        else if (strcmp(tokens[i].lexeme, "strlen") == 0)
            strlenFound = 1;

        else if (strcmp(tokens[i].lexeme, "strcmp") == 0)
            strcmpFound = 1;

        else if (strcmp(tokens[i].lexeme, "strcpy") == 0)
            strcpyFound = 1;

        else if (strcmp(tokens[i].lexeme, "malloc") == 0)
            mallocFound = 1;

        else if (strcmp(tokens[i].lexeme, "calloc") == 0)
            callocFound = 1;

        else if (strcmp(tokens[i].lexeme, "free") == 0)
            freeFound = 1;

        else if (strcmp(tokens[i].lexeme, "sqrt") == 0)
            sqrtFound = 1;

        else if (strcmp(tokens[i].lexeme, "pow") == 0)
            powFound = 1;
    }

    /* Check stdio.h */
    if ((printfFound || scanfFound) &&
        isHeaderPresent("<stdio.h>") == 0)
    {
        printf("Syntax Error: Missing required header <stdio.h>\n");
    }

    /* Check string.h */
    if ((strlenFound || strcmpFound || strcpyFound) &&
        isHeaderPresent("<string.h>") == 0)
    {
        printf("Syntax Error: Missing required header <string.h>\n");
    }

    /* Check stdlib.h */
    if ((mallocFound || callocFound || freeFound) &&
        isHeaderPresent("<stdlib.h>") == 0)
    {
        printf("Syntax Error: Missing required header <stdlib.h>\n");
    }

    /* Check math.h */
    if ((sqrtFound || powFound) &&
        isHeaderPresent("<math.h>") == 0)
    {
        printf("Syntax Error: Missing required header <math.h>\n");
    }
}
int isDataType(const char *word)
{
    if (strcmp(word, "int") == 0 ||
        strcmp(word, "char") == 0 ||
        strcmp(word, "float") == 0 ||
        strcmp(word, "double") == 0 ||
        strcmp(word, "void") == 0 ||
        strcmp(word, "short") == 0 ||
        strcmp(word, "long") == 0)
    {
        return 1;
    }

    return 0;
}

int isFunctionCall(int index)
{
    if (index + 1 >= tokenCount)
        return 0;

    if (tokens[index].type != IDENTIFIER)
        return 0;

    if (strcmp(tokens[index + 1].lexeme, "(") != 0)
        return 0;

    /* Ignore function definitions */
    if (index > 0 && isDataType(tokens[index - 1].lexeme))
        return 0;

    return 1;
}

int findClosingParen(int index)
{
    int parenCount = 0;

    for (int i = index; i < tokenCount; i++)
    {
        if (strcmp(tokens[i].lexeme, "(") == 0)
            parenCount++;

        else if (strcmp(tokens[i].lexeme, ")") == 0)
        {
            parenCount--;

            if (parenCount == 0)
                return i;
        }
    }

    return -1;
}

int isFunctionDefinition(int index)
{
    if (index + 2 < tokenCount &&
        tokens[index].type == RESERVED_KEYWORD &&
        tokens[index + 1].type == IDENTIFIER &&
        strcmp(tokens[index + 2].lexeme, "(") == 0)
    {
        return 1;
    }

    return 0;
}

int isAssignment(int index)
{
    if (index + 1 >= tokenCount)
        return 0;

    if (tokens[index].type != IDENTIFIER)
        return 0;

    if (strcmp(tokens[index + 1].lexeme, "=") == 0 ||
        strcmp(tokens[index + 1].lexeme, "+=") == 0 ||
        strcmp(tokens[index + 1].lexeme, "-=") == 0 ||
        strcmp(tokens[index + 1].lexeme, "*=") == 0 ||
        strcmp(tokens[index + 1].lexeme, "/=") == 0 ||
        strcmp(tokens[index + 1].lexeme, "%=") == 0)
    {
        return 1;
    }

    return 0;
}

void checkSemicolon(void)
{
    int statementType = 0;
    int statementLine = 0;
    int declarationNameIndex = -1;

    /*
       statementType:
       0 - No statement
       1 - Declaration
       2 - Assignment
       3 - Return
       4 - Function call
    */

    for (int i = 0; i < tokenCount; i++)
    {
        /* Variable declaration */
        if (isDataType(tokens[i].lexeme))
        {
            if (isFunctionDefinition(i))
                continue;

            if (statementType != 0)
            {
                printf("Syntax Error: Missing semicolon near line %d\n",
                       statementLine);
            }

            statementType = 1;
            statementLine = tokens[i].lineNumber;

            /* Store the declaration variable's token index */
            declarationNameIndex = i + 1;
        }

        /* Assignment statement */
        else if (isAssignment(i))
        {
            /*
             * Ignore the initializer assignment
             * belonging to the declaration.
             */
            if (statementType == 1 &&
                i == declarationNameIndex)
            {
                continue;
            }

            if (statementType != 0)
            {
                printf("Syntax Error: Missing semicolon near line %d\n",
                       statementLine);
            }

            statementType = 2;
            statementLine = tokens[i].lineNumber;
            declarationNameIndex = -1;
        }

        /* Return statement */
        else if (strcmp(tokens[i].lexeme, "return") == 0)
        {
            if (statementType != 0)
            {
                printf("Syntax Error: Missing semicolon near line %d\n",
                       statementLine);
            }

            statementType = 3;
            statementLine = tokens[i].lineNumber;
        }

        /* Function call */
        else if (isFunctionCall(i))
        {
            int closingIndex = findClosingParen(i + 1);

            if (closingIndex == -1)
                continue;

            if (statementType != 0)
            {
                printf("Syntax Error: Missing semicolon near line %d\n",
                       statementLine);
            }

            statementType = 4;
            statementLine = tokens[i].lineNumber;

            i = closingIndex;
        }

        /* Semicolon */
        else if (strcmp(tokens[i].lexeme, ";") == 0)
        {
            statementType = 0;
            declarationNameIndex = -1;
        }

        /* Closing block */
        else if (strcmp(tokens[i].lexeme, "}") == 0)
        {
            if (statementType != 0)
            {
                printf("Syntax Error: Missing semicolon near line %d\n",
                       statementLine);
            }

            statementType = 0;
            declarationNameIndex = -1;
        }
    }
}