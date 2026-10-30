
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

    for (i = 0; i < ARRAY_SIZE(res_keywords); i++)
    {
        if (strcmp(str, res_keywords[i]) == 0)
        {
            return 1;
        }
    }

    for (i = 0; i < ARRAY_SIZE(non_res_keywords); i++)
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
Token readHeader()
{
    Token token;
    token.lineNumber=lineNumber;
    size_t ind=0;
    int ch;
    token.lexeme[ind++]='<';
    while((ch=fgetc(sourceFile))!=EOF)
    {
        if(ch=='>')
        {
            token.lexeme[ind++]='>';
            break;
        }
        if(ind<MAX_TOKEN_SIZE-1)
        {
            token.lexeme[ind++]=(char)ch;
        }
    }
    token.lexeme[ind]='\0';
    token.type=HEADER;
    expectHeader=0;
    return token;
}

Token readIdentifier(char firstchar)
{
    Token token;
    token.lineNumber=lineNumber;
    size_t ind=0;
    int ch;
    token.lexeme[ind++]=firstchar;
    while((ch=fgetc(sourceFile))!=EOF)
    {
        if(isalnum((unsigned char)ch) || ch=='_')
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
    if(isKeyword(token.lexeme))
    {
        token.type=RESERVED_KEYWORD;
    }
    else
    {
        token.type=IDENTIFIER;
    }
    return token;
}

Token readNumber(char firstchar)
{
    int ch;
    size_t ind=0;
    Token token;
    token.lineNumber=lineNumber;
    token.lexeme[ind++]=firstchar;

    while((ch=fgetc(sourceFile))!=EOF)
    {
        if(isdigit((unsigned char)ch))
        {
            if(ind<MAX_TOKEN_SIZE-1)
            {
                token.lexeme[ind++]=(char)ch;
            }
        }
        else if(ch=='.' && ind<MAX_TOKEN_SIZE-1)
        {
            token.lexeme[ind++]=(char)ch;
        }
        else
        {
            ungetc(ch,sourceFile);
            break;
        }
    }
    token.lexeme[ind]='\0';
    token.type=NUMERIC_CONSTANT;
    return token;
}
int CompoundOperator(const char str)
{
    size_t i;
    for(int i=0;i<ARRAY_SIZE(compound_operators);i++)
    {
        if(strcmp(str,compound_operators[i])==0)
        {
            return 1;
        }
    }
    return 0;
}
Token readOperator(char firstchar)
{
    Token token;
    int ch;
    token.lineNumber=lineNumber;
    token.lexeme[0]=firstchar;
    token.lexeme[1]='\0';
    ch=fgetc(sourceFile);
    if(ch!=EOF)
    {
        token.lexeme[1]=ch;
        token.lexeme[2]='\0';
        if(CompoundOperator(token.lexeme))
        {
            token.type=OPERATOR;
            return token;
        }
        ungetc(ch,sourceFile);
    }
    token.lexeme[1]='\0';
    token.type=OPERATOR;
    return token;
}

Token getNextToken()
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
        if (ch == '<' && expectHeader)
        {
            return readHeader();
        }
        if (isalpha((unsigned char)ch) || ch == '_')
        {
            return readIdentifier((char)ch);
        }
        if (isdigit((unsigned char)ch))
        {
            return readNumber((char)ch);
        }
        if (strchr(operators, ch) != NULL)
        {
            return  readOperator((char)ch);  
        }
    }
}
