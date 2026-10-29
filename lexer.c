
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "lexer.h"

FILE *sourceFile;
int lineNumber = 1;
int expectHeader = 0;


#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))


/* Reserved keywords */
static const char *res_keywords[] =
{
    "const", "volatile", "extern", "auto", "register", "static",
    "signed", "unsigned", "short", "long", "double", "char",
    "int", "float", "struct", "enum", "union", "void", "typedef"
};

/* Non-reserved keywords */
static const char *non_res_keywords[] =
{
    "if", "else", "switch", "case", "goto", "label", "default",
    "break", "continue", "for", "do", "while", "return", "sizeof"
};

/* Preprocessor directives */
static const char *directives[] =
{
    "#include", "#define", "#if", "#endif",
    "#ifdef", "#ifndef", "#undef"
};

/* Compound operators */
static const char *compound_operators[] =
{
    "==", "!=", "|=", "&=", "^=", "<=", ">=",
    "+=", "-=", "*=", "/=", "%=", "<<", ">>",
    "++", "--", "->"
};

/* Single-character operators */
static const char operators[] =
{
    '/', '*', '-', '+', '%', '=', '<', '>',
    '&', '!', '^', '|', '~', ','
};

/* Symbols */
static const char symbol[] =
{
    '(', ')', '{', '}', '[', ']', ':'
};

/* Check whether a string is a keyword */
int isKeyword(const char *str)
{
    int i;

    for (i = 0; i < 19; i++)
    {
        if (strcmp(str, res_keywords[i]) == 0)
        {
            return 1;
        }
    }

    for (i = 0; i < 14; i++)
    {
        if (strcmp(str, non_res_keywords[i]) == 0)
        {
            return 1;
        }
    }

    return 0;
}

/* Open the input file */
void initializeLexer(const char *filename)
{
    sourceFile = fopen(filename, "r");

    if (sourceFile == NULL)
    {
        printf("Error: Cannot open file %s\n", filename);
        return;
    }
}
   

void skipComment(char nextchar)
{
    int ch;
    if(nextchar=='/')
    {
        while((ch=fgetc(sourceFile))!=EOF)
        {
            if(ch=='\n')
            {
                lineNumber++;
                break;
            }
        }
    }
    else if(nextchar=='*')
    {
        int previous=0;
        while((ch=fgetc(sourceFile))!=EOF)
        {
            if(ch=='\n')
            {
                lineNumber++;
            }
            if(previous=='*' && ch=='/')
            {
                break;
            }
            previous=ch;
        }
    }
}

int isDirective(const char *str)
{
    size_t i;

    for (i = 0; i < ARRAY_SIZE(directives); i++)
    {
        if (strcmp(str, directives[i]) == 0)
        {
            return 1;
        }
    }

    return 0;
}

Token readDirective(char firstchar)
{
    Token token;
    int ch;
    size_t ind=0;
    token.lineNumber=lineNumber;
    token.lexeme[ind++]=(char)firstchar;

    while((ch=fgetc(sourceFile))!=EOF)
    {
        if(isalpha((unsigned char)ch))
        {
            if(ind<MAX_TOKEN_SIZE-1)
            {
                token.lexeme[ind++]=(char)ch;
            }
        }
        else
        {
            ungetc(ch,sourceFile);
            break;
        }
    }
    token.lexeme[ind]='\0';
    
    if(isDirective(token.lexeme))
    {
        token.type=PRE_PROCESSOR;
        if(strcmp(token.lexeme,"#include")==0)
        {
            expectHeader=1;
        }
    }
    else
    {
        token.type=UNKNOWN;
    }
    return token;
}


Token getNextToken(void)
{
    Token token;
    int ch;

    token.lexeme[0] = '\0';
    token.type = EOF_TOKEN;
    token.lineNumber = lineNumber;

    if (sourceFile == NULL)
    {
        return token;
    }

    while ((ch = fgetc(sourceFile)) != EOF)
    {
        if (ch == ' ' || ch == '\t' || ch == '\r')
        {
            continue;
        }

        if (ch == '\n')
        {
            lineNumber++;
            continue;
        }

        if (ch == '/')
        {
            int nextChar = fgetc(sourceFile);

            if (nextChar == '/' || nextChar == '*')
            {
                skipComment(nextChar);
                continue;
            }

            if (nextChar != EOF)
            {
                ungetc(nextChar, sourceFile);
            }

            token.lexeme[0] = '/';
            token.lexeme[1] = '\0';
            token.type = OPERATOR;
            token.lineNumber = lineNumber;

            return token;
        }
        if (ch == '#')
        {
            return readDirective(ch);
        }
    }
}

        