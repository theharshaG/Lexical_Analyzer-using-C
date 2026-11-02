
#ifndef SYNTAX_H
#define SYNTAX_H

#include "lexer.h"

//Store lexer tokens 
void storeTokens(void);

//Header checking 
int isValidHeader(const char *header);
void checkHeaders(void);
int isHeaderPresent(const char *header);
void checkRequiredHeaders(void);

// Delimiter checking 
void checkDelimiters(void);

//Data type checking 
int isDataType(const char *word);

//Function checking 
int isFunctionCall(int index);
int isFunctionDefinition(int index);
int findClosingParen(int index);

// Operator checking 
int isAssignment(int index);
int isIncrementDecrement(int index);

// Semicolon checking 
void checkSemicolon(void);

#endif
