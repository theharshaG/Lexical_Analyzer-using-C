#ifndef LEXER_H
#define LEXER_H

#define MAX_TOKEN_SIZE 100

typedef enum
{
    PRE_PROCESSOR,
    HEADER,
    RESERVED_KEYWORD,
    NON_RESERVED_KEYWORD,
    OPERATOR,
    OPEN_BRACE,
    CLOSE_BRACE,
    TERMINATING_OPERATOR,
    NUMERIC_CONSTANT,
    IDENTIFIER,
    UNKNOWN,
    EOF_TOKEN
} TokenType;

typedef struct
{
    char lexeme[MAX_TOKEN_SIZE];
    TokenType type;
    int lineNumber;
} Token;

void initializeLexer(const char *filename);
Token getNextToken(void);
TokenType getKeywordType(const char *str);

#endif