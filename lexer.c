
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "lexer.h"

/* File pointer for reading input.c */
static FILE *sourceFile;

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

