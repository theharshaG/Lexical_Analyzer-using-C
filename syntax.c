#include <stdio.h>
#include <string.h>
#include "syntax.h"

Token tokens[1000];
int tokenCount = 0;
int delimiterError = 0;

//Store all lexer tokens in an array 
void storeTokens()
{
    Token token;
    tokenCount = 0;

    while (1)
    {
        token = getNextToken();

        if (token.type == EOF_TOKEN)
            break;

        if (tokenCount >= 1000)
        {
            printf("Syntax Error: Too many tokens\n");
            break;
        }

        tokens[tokenCount] = token;
        tokenCount++;
    }
}

// Check whether a header is supported
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
            return 1;
    }

    return 0;
}

//Check header files using stored tokens 
void checkHeaders()
{
    for (int i = 0; i < tokenCount; i++)
    {
        if (tokens[i].type == HEADER)
        {
            if (isValidHeader(tokens[i].lexeme) == 0)
            {
                printf("Syntax Error: Unknown header file '%s' at line %d\n",
                       tokens[i].lexeme,
                       tokens[i].lineNumber);
            }
        }
    }
}

//Check opening and closing delimiters 
void checkDelimiters()
{
    char delimiter[1000];
    int top = -1;
    int error = 0;

    delimiterError = 0;

    for (int i = 0; i < tokenCount; i++)
    {
        char ch = tokens[i].lexeme[0];

        // Opening delimiters
        if (strcmp(tokens[i].lexeme, "(") == 0 ||
            strcmp(tokens[i].lexeme, "{") == 0 ||
            strcmp(tokens[i].lexeme, "[") == 0)
        {
            if (top == 999)
            {
                printf("Syntax Error: Too many opening delimiters\n");
                error = 1;
                break;
            }

            top++;
            delimiter[top] = ch;
        }

        //Closing delimiters
        else if (strcmp(tokens[i].lexeme, ")") == 0 ||
                 strcmp(tokens[i].lexeme, "}") == 0 ||
                 strcmp(tokens[i].lexeme, "]") == 0)
        {
            if (top == -1)
            {
                printf("Syntax Error: Unexpected closing delimiter '%c' near line %d\n",
                       ch, tokens[i].lineNumber);

                error = 1;
                continue;
            }

            if ((ch == ')' && delimiter[top] == '(') ||
                (ch == '}' && delimiter[top] == '{') ||
                (ch == ']' && delimiter[top] == '['))
            {
                top--;
            }
            else
            {
                printf("Syntax Error: Mismatched delimiter '%c' near line %d\n",
                       ch, tokens[i].lineNumber);

                error = 1;

                // Remove the unmatched opening delimiter
                top--;
            }
        }
    }

    // Check remaining opening delimiters
    if (top != -1)
    {
        printf("Syntax Error: Missing closing delimiter for '%c'\n",
               delimiter[top]);

        error = 1;
    }

    if (error == 1)
        delimiterError = 1;
}

//Check whether a particular header exists
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

//Check required header files
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

    if ((printfFound || scanfFound) &&
        isHeaderPresent("<stdio.h>") == 0)
    {
        printf("Syntax Error: Missing required header <stdio.h>\n");
    }

    if ((strlenFound || strcmpFound || strcpyFound) &&
        isHeaderPresent("<string.h>") == 0)
    {
        printf("Syntax Error: Missing required header <string.h>\n");
    }

    if ((mallocFound || callocFound || freeFound) &&
        isHeaderPresent("<stdlib.h>") == 0)
    {
        printf("Syntax Error: Missing required header <stdlib.h>\n");
    }

    if ((sqrtFound || powFound) &&
        isHeaderPresent("<math.h>") == 0)
    {
        printf("Syntax Error: Missing required header <math.h>\n");
    }
}

//Check whether a word is a data type
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

//Check whether an identifier is followed by '(' 
int isFunctionCall(int index)
{
    if (index + 1 >= tokenCount)
        return 0;

    if (tokens[index].type != IDENTIFIER)
        return 0;

    if (strcmp(tokens[index + 1].lexeme, "(") != 0)
        return 0;

    //Ignore function definitions (including pointer return types like char *foo())
    int prev = index - 1;
    while (prev >= 0 && strcmp(tokens[prev].lexeme, "*") == 0)
    {
        prev--;
    }
    if (prev >= 0 && isDataType(tokens[prev].lexeme))
        return 0;

    return 1;
}

//Find the matching closing parenthesis 
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

//Check whether the current token starts a function definition
int isFunctionDefinition(int index)
{
    if (!isDataType(tokens[index].lexeme))
        return 0;

    int idx = index + 1;
    while (idx < tokenCount && strcmp(tokens[idx].lexeme, "*") == 0)
    {
        idx++;
    }

    if (idx + 1 < tokenCount &&
        tokens[idx].type == IDENTIFIER &&
        strcmp(tokens[idx + 1].lexeme, "(") == 0)
    {
        return 1;
    }

    return 0;
}

//Check assignment operators 
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

//Check prefix and postfix increment/decrement
int isIncrementDecrement(int index)
{
    //Postfix: i++ or i-- 
    if (index + 1 < tokenCount &&
        tokens[index].type == IDENTIFIER &&
        (strcmp(tokens[index + 1].lexeme, "++") == 0 ||
         strcmp(tokens[index + 1].lexeme, "--") == 0))
    {
        return 1;
    }

    /* Prefix: ++i or --i */
    if (index + 1 < tokenCount &&
        (strcmp(tokens[index].lexeme, "++") == 0 ||
         strcmp(tokens[index].lexeme, "--") == 0) &&
        tokens[index + 1].type == IDENTIFIER)
    {
        return 1;
    }

    return 0;
}

/* Check missing semicolons */
void checkSemicolon(void)
{
    /* Avoid misleading errors when delimiters are invalid */
    if (delimiterError == 1)
        return;

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
        5 - Increment / Decrement
        6 - Break / Continue
    */

    for (int i = 0; i < tokenCount; i++)
    {
        /* Skip for loop header */
        if (strcmp(tokens[i].lexeme, "for") == 0)
        {
            int closingIndex = findClosingParen(i + 1);

            if (closingIndex != -1)
            {
                i = closingIndex;
                continue;
            }
        }

        /* Variable declaration */
        if (isDataType(tokens[i].lexeme))
        {
            /* Skip function parameters */
            if (isFunctionDefinition(i))
            {
                int parenIdx = i + 1;
                while (parenIdx < tokenCount && strcmp(tokens[parenIdx].lexeme, "(") != 0)
                {
                    parenIdx++;
                }
                int closingIndex = findClosingParen(parenIdx);

                if (closingIndex != -1)
                {
                    i = closingIndex;
                    continue;
                }
            }

            if (statementType != 0)
            {
                printf("Syntax Error: Missing semicolon near line %d\n",
                       statementLine);
            }

            statementType = 1;
            statementLine = tokens[i].lineNumber;

            /* Skip pointer asterisks to find the variable name */
            int nextIdx = i + 1;
            while (nextIdx < tokenCount && strcmp(tokens[nextIdx].lexeme, "*") == 0)
            {
                nextIdx++;
            }
            declarationNameIndex = nextIdx;
        }

        /* Assignment statement */
        else if (isAssignment(i))
        {
            if (declarationNameIndex != -1)
            {
                int commaFound = 0;

                for (int j = declarationNameIndex; j < i; j++)
                {
                    if (strcmp(tokens[j].lexeme, ",") == 0)
                    {
                        commaFound = 1;
                        break;
                    }
                }

                /* First variable assignment */
                if (i == declarationNameIndex)
                {
                    statementType = 2;
                    continue;
                }

                /* Another variable in the same declaration */
                if (commaFound)
                    continue;
            }

            if (statementType != 0)
            {
                printf("Syntax Error: Missing semicolon near line %d\n",
                       statementLine);
            }

            statementType = 2;
            statementLine = tokens[i].lineNumber;
        }

        /* Increment / Decrement */
        else if (isIncrementDecrement(i))
        {
            if (statementType != 0)
            {
                printf("Syntax Error: Missing semicolon near line %d\n",
                       statementLine);
            }

            statementType = 5;
            statementLine = tokens[i].lineNumber;

            /* Skip identifier in prefix expression */
            if (strcmp(tokens[i].lexeme, "++") == 0 ||
                strcmp(tokens[i].lexeme, "--") == 0)
            {
                i++;
            }
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

        /* Break / Continue */
        else if (strcmp(tokens[i].lexeme, "break") == 0 ||
                 strcmp(tokens[i].lexeme, "continue") == 0)
        {
            if (statementType != 0)
            {
                printf("Syntax Error: Missing semicolon near line %d\n",
                       statementLine);
            }

            statementType = 6;
            statementLine = tokens[i].lineNumber;
        }

        /* Function call */
        else if (isFunctionCall(i))
        {
            int closingIndex = findClosingParen(i + 1);

            if (closingIndex == -1)
                continue;

            /* Function call inside assignment or return */
            if ((statementType == 2 || statementType == 3) &&
                tokens[i].lineNumber == statementLine)
            {
                i = closingIndex;
                continue;
            }

            if (statementType != 0)
            {
                printf("Syntax Error: Missing semicolon near line %d\n",
                       statementLine);
            }

            statementType = 4;
            statementLine = tokens[i].lineNumber;

            /* Skip function arguments */
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
